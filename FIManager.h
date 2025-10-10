#ifndef _FIMANAGER_H
#define _FIMANAGER_H

#include "DataLoader.h"
#include "DBManager.h"
#include "FIMemoryManager.h"
#include "FragmentIonIndex.h"
#include "Threading.h"
#include "ThreadPool.h"

struct sSearchStruct {
	Mutex* mutex=nullptr;
	bool* thread=nullptr;
	FragmentIonIndex* fii = nullptr;
  FISpectrum* scan = nullptr;
  sSearchStruct(FragmentIonIndex* f, FISpectrum* s) {
		fii = f;
    scan = s;
  }
  ~sSearchStruct() {
		Threading::LockMutex(*mutex);
		*thread = false;
		Threading::UnlockMutex(*mutex);
		fii = NULL;
		scan = NULL;
		thread = NULL;
		mutex = NULL;
  }
};

struct sFIndexStruct {
	FragmentIonIndex* fii;
	int z;
	sFIndexStruct(FragmentIonIndex* f, const int& i) {
		fii = f;
		z = i;
	}
	~sFIndexStruct() {
		fii = NULL;
	}
};

struct sGenIndex {
	FragmentIonIndex* fii;
	FIMask* mask;
	unsigned int* arr;
	unsigned int start;
	unsigned int stop;
	int id = -1;
	sGenIndex(FragmentIonIndex* f,const unsigned int& sta, const unsigned int& sto, unsigned int* a, FIMask* m) {
		fii = f;
		mask = m;
		start = sta;
		stop = sto;
		arr = a;
	}
	~sGenIndex() {
		fii = NULL;
		mask = NULL;
		arr = NULL;
	}
};

class FIManager {
public:
	FIManager(DBManager* d, const size_t count = 1);
	~FIManager();

	bool AllocateScoreMemory(const size_t& sz, const size_t& szXL=0);
	bool GeneratePeptideMap();
	bool GenerateIndex();
	bool ScoreSpectrum(std::vector<FISpectrum>& scans);
	bool ScoreSpectrum(DataLoader& scans);
	void SetBinSize(double d);

	FragmentIonIndex* fii;  //generate an array, one per thread

protected:
private:

	void Init();
	void LocalGenerateIndex();

	//Processes run during multithreading
	static void CalcIndexProcess(sGenIndex* s);
	static void CalcIndexSzProcess(sGenIndex* s);
	static void ScoreSpectrumProcess(sSearchStruct* s);


	DBManager* dbm = NULL;
	static FIMemoryManager mem;
	
	static Mutex mutexThreads;
	static bool* activeThread;
	static size_t threads;


};

#endif