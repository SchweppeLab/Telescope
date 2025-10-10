#include "DataLoader.h"

using namespace std;
using namespace MSToolkit;

FastXCorr* DataLoader::xcorr;

Mutex DataLoader::mutexThreads;
bool* DataLoader::activeThread;
size_t DataLoader::threads;

DataLoader::DataLoader(FragmentIonIndex* f, const size_t th) {
	fii = f;
	threads = th;
	if (threads < 1) threads = 1;

	Init();
}

DataLoader::~DataLoader() {
	fii = nullptr;
	delete[] xcorr;
	delete[] activeThread;
	Threading::DestroyMutex(mutexThreads);
}

FISpectrum& DataLoader::operator[](const size_t& index) {
	return scans[index];
}

void DataLoader::Init() {
	xcorr = new FastXCorr[threads]();
	activeThread = new bool[threads]();

	Threading::CreateMutex(&mutexThreads);
}

void DataLoader::ProcessSpectrum(FISpectrum& s, int tIndex) {
	xcorr[tIndex].ProcessSpectrum(s);
}

void DataLoader::ProcessSpectrumProc(sSpectrumStruct* s) {
	//Get next available thread number;
	size_t i;
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

bool DataLoader::ReadSpectra(const string& fn) {
	MSReader r;
	Spectrum s;

	//These are used for memory allocation later.
	maxScoreCount = 0;
	maxScoreCountXL = 0;

	double invBinSize = 1 / BINSIZE;
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
			if (mass<MINPEPMASS || mass>MAXPEPMASS) goto NEXTSCAN;

			double err = mass / 1e6 * PPM;
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
			scans[scanIndex].precursor.back().ts.Init(PSMCOUNT);
			int count = 0;
			while (fii->peptides[index++].mass < max) count++;
			scans[scanIndex].precursor.back().scoreCount = count;
			if (count > maxScoreCount) maxScoreCount = count;

			//Add the peaks
			for (int a = 0;a < s.size();a++) {
				FIPeak p;
				p.mz = s[a].mz;
				p.fIndex = s[a].mz * invBinSize + 1;
				p.value = s[a].intensity;
				scans[scanIndex].AddPeak(p);
			}

			if (XCORR) {
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
	if (XCORR) {
		ThreadPool<sSpectrumStruct*>* spectraPool = new ThreadPool<sSpectrumStruct*>(ProcessSpectrumProc, threads, threads, 1);
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

size_t DataLoader::Size() {
	return scans.size();
}