#include "DataLoader.h"

using namespace std;
using namespace MSToolkit;

FastXCorr* DataLoader::xcorr;

Mutex DataLoader::mutexThreads;
bool* DataLoader::activeThread;
size_t DataLoader::threads;

DataLoader::DataLoader() {
	xcorr = nullptr;
	activeThread = nullptr;
}

DataLoader::~DataLoader() {
	dbm = nullptr;
	fii = nullptr;
	params = nullptr;
	Deallocate();
}

FISpectrum& DataLoader::operator[](const size_t& index) {
	return scans[index];
}

void DataLoader::Allocate() {
	Deallocate();
	threads = (size_t)params->threads;
	xcorr = new FastXCorr[threads]();
	for (size_t a = 0;a < threads;a++) xcorr[a].Initialize(params);
	activeThread = new bool[threads]();
	Threading::CreateMutex(&mutexThreads);
}

void DataLoader::Deallocate() {
	if (xcorr) delete[] xcorr;
	if (activeThread) {
		delete[] activeThread;
		Threading::DestroyMutex(mutexThreads);
	}
}

bool DataLoader::Initialize(DBManager* d, FragmentIonIndex* f, ParamsManager* p) {
	params = p;
	dbm = d;
	fii = f;
	Allocate();
	return true;
}

void DataLoader::ProcessSpectrum(FISpectrum& s, int tIndex) {
	xcorr[tIndex].ProcessSpectrum(s);
}

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

//This could be far more efficient by reading into temporary memory, transforming, then appending
//an array of processed scans. With lots and lots of memory, perhaps even precompute each scan size?
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
			if (s.size() < 25) goto NEXTSCAN;

			//Add the precursor information
			double mz = s.getMonoMZ();
			if (mz == 0) mz = s.getMZ();
			int charge = s.getCharge();

			//what to do if charge=0?
			if (charge == 0) charge = 3;
			double mass = mz * charge - (charge * PROTON);
			if (mass<params->minPepMass || mass>params->maxPepMass) goto NEXTSCAN;

			double err = mass / 1e6 * params->ppm;
			double min = mass - err;
			double max = mass + err;
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
			for (int a = 0;a < s.size();a++) {
				if (s[a].mz > params->maxMZ) break; //assuming mz values are in order from low to high.
				FIPeak p;
				p.mz = s[a].mz;
				p.fIndex = (size_t)(s[a].mz * invBinSize + 1);
				p.value = s[a].intensity;
				scans[scanIndex].AddPeak(p);
			}

			if (params->xcorr) {
				//xcorr[0].ProcessSpectrum(scans[scanIndex]);
				//cout << "Send " << scans[scanIndex].scanNumber << endl;
				//spectraPool->WaitForQueuedParams();
				//sSpectrumStruct* sp = new sSpectrumStruct(&scans[scanIndex]);
				//spectraPool->Launch(sp);
			}

		}

		NEXTSCAN:
		r.readFile(NULL, s);
	}

	if (params->xcorr) {
		ThreadPool<sSpectrumStruct*>* spectraPool = new ThreadPool<sSpectrumStruct*>(ProcessSpectrumProc, threads, threads, 1);
		for (size_t a = 0;a < scans.size();a++) {
			//cout << a << "\t" << scans[a].scanNumber << "\t" << scans[a].Size() << endl;
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

size_t DataLoader::Size() {
	return scans.size();
}