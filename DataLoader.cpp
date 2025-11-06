#include "DataLoader.h"

using namespace std;
using namespace MSToolkit;

FastXCorr* DataLoader::xcorr;
Mutex DataLoader::mutexThreads;
bool* DataLoader::activeThread;
size_t DataLoader::threads;

/// <summary>
/// Default constructor
/// </summary>
DataLoader::DataLoader() {
	xcorr = nullptr;
	activeThread = nullptr;
}

/// <summary>
/// Default destructor
/// </summary>
DataLoader::~DataLoader() {
	dbm = nullptr;
	fii = nullptr;
	params = nullptr;
	Deallocate();
}

/// <summary>
/// Accesses any scan stored in the DataLoader
/// </summary>
/// <param name="index">position of the scan in the scans array</param>
/// <returns>FISpectrum scan object</returns>
FISpectrum2& DataLoader::operator[](const size_t& index) {
	return scans[index];
}

/// <summary>
/// Allocates memory used by the FastXCorr objects for spectral processing.
/// </summary>
void DataLoader::Allocate() {
	Deallocate();
	threads = (size_t)params->threads;
	xcorr = new FastXCorr[threads]();
	for (size_t a = 0;a < threads;a++) xcorr[a].Initialize(params);
	activeThread = new bool[threads]();
	Threading::CreateMutex(&mutexThreads);
}

/// <summary>
/// Frees memory used by the DataLoader
/// </summary>
void DataLoader::Deallocate() {
	if (xcorr) delete[] xcorr;
	if (activeThread) {
		delete[] activeThread;
		Threading::DestroyMutex(mutexThreads);
	}
}

/// <summary>
/// Sets the support classes required for processing spectra
/// </summary>
/// <param name="d">pointer to DBManager object</param>
/// <param name="f">pointer to FragmentIonIndex object</param>
/// <param name="p">pointer to ParamsManager object</param>
/// <returns></returns>
bool DataLoader::Initialize(DBManager* d, FragmentIonIndex* f, ParamsManager* p) {
	params = p;
	dbm = d;
	fii = f;
	Allocate();
	return true;
}

/// <summary>
/// Starts the spectral processing for xcorr transformation inside a thread.
/// </summary>
/// <param name="s"></param>
/// <param name="tIndex"></param>
void DataLoader::ProcessSpectrum(FISpectrum2& s, int tIndex) {
	xcorr[tIndex].ProcessSpectrum(s);
}

/// <summary>
/// Manages the next avialble thread to perfrom spectral processing
/// </summary>
/// <param name="s">sSpectrumStruct defining the scan to be processed</param>
void DataLoader::ProcessSpectrumProc(sSpectrumStruct* s) {
	//Get next available thread number;
	int i;
	Threading::LockMutex(mutexThreads);
	for (i = 0;i < threads;i++) {
		if (!activeThread[i]) {
			activeThread[i] = true;
			break;
		}
	}
	Threading::UnlockMutex(mutexThreads);

	s->mutex = &mutexThreads;
	s->thread = &activeThread[i];
	ProcessSpectrum(*s->scan, i);
	delete s;
	s = nullptr;
}

/// <summary>
/// Reads in a spectral data file, storing spectra in FISpectrum objects. File reading is a single thread process.
/// Once all files are loaded, the array of object can optionally be processed using multiple threads.
/// </summary>
/// <param name="fn">The name (and path if not in the current working directory) of the spectral file to read.</param>
/// <returns>true upon success</returns>
bool DataLoader::ReadSpectra(const string& fn) {
	MSReader r;
	Spectrum s;

	//These are used for memory allocation later.
	maxScoreCount = 0;

	double invBinSize = 1 / params->binSize;
	scans.clear();
	
	r.setFilter(MS2);
	if (!r.readFile(fn.c_str(), s)) return false;
	while (s.getScanNumber() > 0) {

		//Process MS/MS scans
		if (s.getMsLevel() == 2) {

			//skip emptyish scans
			//TODO: Make this a user-defined parameter!
			if (s.size() < 25) goto NEXTSCAN;

			//Add the precursor information
			double mz = s.getMonoMZ();
			if (mz == 0) mz = s.getMZ();
			int charge = s.getCharge();

			//what to do if charge=0?
			if (charge == 0) charge = 3;
			double mass = mz * charge - (charge * PROTON);
			if (mass+PROTON<params->minPepMass || mass+PROTON>params->maxPepMass) goto NEXTSCAN; //M+H to match Comet...

			//The peptide mass tolerance boundaries here replicate how Comet computes them (which is M+H space...)
			double mzErr = mz / 1e6 * params->ppm;
			double mzMin = mz - mzErr;
			double mzMax = mz + mzErr;
			double min = mzMin * charge - (charge * PROTON);
			double max = mzMax *charge - (charge * PROTON);
			size_t index = fii->FindPeptideIndex(min); //should return the first index below the desired mass
			while (fii->peptides[index].mass < min) index++;

			size_t scanIndex = scans.size();
			scans.emplace_back();
			scans[scanIndex].scanNumber = s.getScanNumber();

			FIPrecursor p;
			p.charge = charge;
			p.mass = mass;
			p.peptide = "";
			p.pepOffset = index;
			scans[scanIndex].precursor.push_back(p);
			scans[scanIndex].precursor.back().ts.Init(params->psmCount);
			int count = 0;
			while (fii->peptides[index++].mass < max) count++;
			scans[scanIndex].precursor.back().scoreCount = count;
			if (count > maxScoreCount) maxScoreCount = count;

			//Add the peaks
			//scans[scanIndex].Reserve((size_t)s.size());
			//cout << scans[scanIndex].Capacity() << endl;
			scans[scanIndex].Allocate((size_t)s.size());
			for (int a = 0;a < s.size();a++) {
				if (s[a].mz > params->maxMZ) break; //assuming mz values are in order from low to high.
				FIPeak p;
				p.mz = s[a].mz;
				p.fIndex = (size_t)(s[a].mz * invBinSize + 1);
				p.value = s[a].intensity;
				scans[scanIndex].AddPeak(p);
			}
		}

		NEXTSCAN:
		r.readFile(NULL, s);
	}

	//Only process spectra if the params instruct to do so. It is done with multiple threads for speed.
	if (params->xcorr) {
		ThreadPool<sSpectrumStruct*>* spectraPool = new ThreadPool<sSpectrumStruct*>(ProcessSpectrumProc, (int)threads, (int)threads, 1);
		for (size_t a = 0;a < scans.size();a++) {
			spectraPool->WaitForQueuedParams();
			sSpectrumStruct* sp = new sSpectrumStruct(&scans[a]);
			spectraPool->Launch(sp);
		}
		spectraPool->WaitForQueuedParams();
		spectraPool->WaitForThreads();
		delete spectraPool;
	}

	return true;
}

/// <summary>
/// Returns the number of spectra.
/// </summary>
/// <returns>the number of spectra</returns>
size_t DataLoader::Size() {
	return scans.size();
}

size_t DataLoader::XCorrTime() {
	size_t ms = 0;
	for (size_t a = 0;a < threads;a++) {
		ms += xcorr[a].ReportTime();
	}
	return ms/1000;
}