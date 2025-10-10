#ifndef _FIMANAGER_H
#define _FIMANAGER_H

#include "DataLoader.h"
#include "DBManager.h"
#include "FIMemoryManager.h"
#include "FragmentIonIndexXL.h"
#include "Threading.h"
#include "ThreadPool.h"

struct sSearchStruct {
	Mutex* mutex=nullptr;
	bool* thread=nullptr;
	FragmentIonIndexXL* fii = nullptr;
  FISpectrum* scan = nullptr;
  sSearchStruct(FragmentIonIndexXL* f, FISpectrum* s) {
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
	FragmentIonIndexXL* fii;
	int z;
	sFIndexStruct(FragmentIonIndexXL* f, const int& i) {
		fii = f;
		z = i;
	}
	~sFIndexStruct() {
		fii = NULL;
	}
};

struct sGenIndex {
	FragmentIonIndexXL* fii;
	FIMask* mask;
	unsigned int* arr;
	unsigned int start;
	unsigned int stop;
	int id = -1;
	sGenIndex(FragmentIonIndexXL* f,const unsigned int& sta, const unsigned int& sto, unsigned int* a, FIMask* m) {
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

struct sGenIndexXL {
	FragmentIonIndexXL* fii;
	FIMask* mask;
	unsigned int* arrA;
	unsigned int* arrB;
	unsigned int start;
	unsigned int stop;
	int id = -1;
	sGenIndexXL(FragmentIonIndexXL* f, const unsigned int& sta, const unsigned int& sto, unsigned int* a, unsigned int* b, FIMask* m) {
		fii = f;
		mask = m;
		start = sta;
		stop = sto;
		arrA = a;
		arrB = b;
	}
	~sGenIndexXL() {
		fii = NULL;
		mask = NULL;
		arrA = NULL;
		arrB = NULL;
	}
};

class FIManager {
public:
	FIManager(DBManager* d, const size_t count = 1);
	~FIManager();

	bool AllocateScoreMemory(const size_t& sz, const size_t& szXL=0);
	bool GeneratePeptideMap(std::string xl="");
	bool GenerateIndex();
	bool ScoreSpectrum(std::vector<FISpectrum>& scans);
	bool ScoreSpectrum(DataLoader& scans);
	void SetBinSize(double d);

	FragmentIonIndexXL* fii;  //generate an array, one per thread

protected:
private:

	void Init();
	void LocalGenerateIndex();
	void LocalGenerateIndexXL();

	//Processes run during multithreading
	static void CalcIndexProcess(sGenIndex* s);
	static void CalcIndexProcessXL(sGenIndexXL* s);
	static void CalcIndexSzProcess(sGenIndex* s);
	static void CalcIndexSzProcessXL(sGenIndexXL* s);
	static void ScoreSpectrumProcess(sSearchStruct* s);


	DBManager* dbm = NULL;
	static FIMemoryManager mem;
	
	static Mutex mutexThreads;
	static bool* activeThread;
	static size_t threads;


};

#endif