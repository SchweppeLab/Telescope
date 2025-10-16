#ifndef _FIMANAGER_H
#define _FIMANAGER_H

#include "DataLoader.h"
#include "DBManager.h"
#include "FIMemoryManager.h"
#include "FragmentIonIndex.h"
#include "ParamsManager.h"
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
	FIManager();
	~FIManager();

	void Allocate();
	bool AllocateScoreMemory(const size_t& sz);
	void Deallocate();
	bool GeneratePeptideMap();
	bool GenerateIndex();
	void Initialize(DBManager* d, ParamsManager* p);
	bool ScoreSpectrum(std::vector<FISpectrum>& scans);
	bool ScoreSpectrum(DataLoader& scans);

	FragmentIonIndex fii;

protected:
private:

	void InternalGenerateIndex();

	//Processes run during multithreading
	static void CalcIndexProcess(sGenIndex* s);
	static void CalcIndexSzProcess(sGenIndex* s);
	static void ScoreSpectrumProcess(sSearchStruct* s);


	DBManager* dbm = nullptr;
	ParamsManager* params = nullptr;
	static FIMemoryManager mem;
	
	static Mutex mutexThreads;
	static bool* activeThread;
	static size_t threads;


};

#endif