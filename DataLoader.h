#ifndef _DATALOADER_H
#define _DATALOADER_H

#include "FastXCorr.h"
#include "FISpectrum.h"
#include "FragmentIonIndex.h"
#include "GlobalDefinitions.h"
#include "MSReader.h"
#include "ParamsManager.h"
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
	DataLoader();
	//DataLoader(DBManager* d, FragmentIonIndex* f, const size_t th=1);
	~DataLoader();

	FISpectrum& operator[](const size_t& index);

	bool Initialize(DBManager* d, FragmentIonIndex* f, ParamsManager* p);
	bool ReadSpectra(const std::string& fn);
	size_t Size();

	size_t maxScoreCount = 0;

protected:
private:

	void Allocate();
	void Deallocate();
	static void ProcessSpectrum(FISpectrum& s, int tIndex);
	static void ProcessSpectrumProc(sSpectrumStruct* s);

	DBManager* dbm = nullptr;
	FragmentIonIndex* fii = nullptr;
	ParamsManager* params = nullptr;
	static FastXCorr* xcorr; //array of xcorr transformers, one per thread
	std::vector<FISpectrum> scans;

	static Mutex mutexThreads;
	static bool* activeThread;
	static size_t threads;
};


#endif