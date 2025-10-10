#ifndef _DATALOADER_H
#define _DATALOADER_H

#include "FastXCorr.h"
#include "FISpectrum.h"
#include "FragmentIonIndexXL.h"
#include "GlobalDefinitions.h"
#include "MSReader.h"
#include "Threading.h"
#include "ThreadPool.h"

struct sSpectrumStruct {
	Mutex* mutex = nullptr;
	bool* thread = nullptr;
	FISpectrum* scan = nullptr;
	sSpectrumStruct(FISpectrum* s) {
		scan = s;
	}
	~sSpectrumStruct() {
		Threading::LockMutex(*mutex);
		*thread = false;
		Threading::UnlockMutex(*mutex);
		scan = NULL;
		thread = NULL;
		mutex = NULL;
	}
};

class DataLoader {
public:
	DataLoader(FragmentIonIndexXL* f, const size_t th=1);
	~DataLoader();

	FISpectrum& operator[](const size_t& index);

	bool ReadSpectra(const std::string& fn);
	size_t Size();

	size_t maxScoreCount = 0;
	size_t maxScoreCountXL = 0;

	DBManager* dbm; //temporary for testing only

protected:
private:

	void Init();
	static void ProcessSpectrum(FISpectrum& s, int tIndex);
	static void ProcessSpectrumProc(sSpectrumStruct* s);

	FragmentIonIndexXL* fii;
	static FastXCorr* xcorr; //array of xcorr transformers, one per thread
	std::vector<FISpectrum> scans;

	static Mutex mutexThreads;
	static bool* activeThread;
	static size_t threads;
};


#endif