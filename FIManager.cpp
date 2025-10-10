#include "FIManager.h"

using namespace std;

FIMemoryManager FIManager::mem;

Mutex FIManager::mutexThreads;
bool* FIManager::activeThread;
size_t FIManager::threads;

FIManager::FIManager(DBManager* d, const size_t count) {
	dbm = d;
	threads = count;
	if (threads < 1) threads = 1;

	Init();
}

FIManager::~FIManager() {
	if (fii != NULL) delete fii; //delete[] fii;
	dbm = NULL;
	delete[] activeThread;
	Threading::DestroyMutex(mutexThreads);
}

bool FIManager::AllocateScoreMemory(const size_t& sz, const size_t& szXL) {
	mem.AllocateScores(threads, sz);
	if (szXL > 0) mem.AllocateScoresXL(threads, szXL);
	return true;
}

void FIManager::CalcIndexProcess(sGenIndex* s) {
	s->fii->PopulateIndex(s->start, s->stop, s->arr,*s->mask);
	delete s;
}

void FIManager::CalcIndexProcessXL(sGenIndexXL* s) {
	s->fii->PopulateIndexXL(s->start, s->stop, s->arrA, s->arrB, *s->mask);
	delete s;
}

void FIManager::CalcIndexSzProcess(sGenIndex* s) {
	s->fii->CalculateIndex(s->start, s->stop, s->arr, *s->mask);
	delete s;
}

void FIManager::CalcIndexSzProcessXL(sGenIndexXL* s) {
	s->fii->CalculateIndexXL(s->start, s->stop, s->arrA,s->arrB, *s->mask);
	delete s;
}

//Generating the index is a complex process so that it can be multithreaded. Two functions do the work, one
//for regular peptide searches, the other for crosslinked peptides. These functions are only called if there
//are mapped peptidoforms for the types of search.
bool FIManager::GenerateIndex() {	

	if (fii->pepArrSz > 0) LocalGenerateIndex();
	if (fii->pepArrSzXL > 0) LocalGenerateIndexXL();

	return true;
}

bool FIManager::GeneratePeptideMap(string xl) {
	fii->maxModsXL = 0;
	fii->GeneratePeptideMap(xl);
	return true;
}


void FIManager::Init() {
	fii = new FragmentIonIndexXL(dbm); // [threads] ;
	activeThread = new bool[threads]();
	//for (size_t a = 0;a < threads;a++) fii[a].SetDBManager(dbm);
	cout << threads << " threads initialized." << endl;

	Threading::CreateMutex(&mutexThreads);
}

//Generating the fragment ion index from here is multi-threaded. It divides the peptidoforms in the 
//database manager into equal parts for each thread. Each thread then determines the memory needed for its
//portion of the index. The total memory is allocated, then each thread populates the index for its peptidoforms
//using the offsets determined at the memory calculation stage. No two threads will ever access the same
//block of memory, no semaphores are needed, and the processes are entirely parallel.
void FIManager::LocalGenerateIndex() {

	//Generate index to temporary memory, then add them all up in order
	size_t th = threads;
	size_t z = 3;
	size_t maxBin = fii->maxBin;
	unsigned int* binSz = new unsigned int[th * z * maxBin]();
	FIMask* mask = new FIMask[threads];
	for (size_t a = 0;a < threads;a++) mask[a].Allocate(MAXPEPLEN * 2 * 3, fii->maxBin);

	//Determine the index sizes at each bin within each thread
	ThreadPool<sGenIndex*>* pool = new ThreadPool<sGenIndex*>(CalcIndexSzProcess, threads, threads, 1);
	unsigned int set = (unsigned int)(fii->pepArrSz / threads);
	unsigned int pos = 0;
	for (int a = 0;a < th;a++) {
		sGenIndex* s = new sGenIndex(fii, pos, pos + set, &binSz[a * z * maxBin],&mask[a]);
		if (a == th - 1) s->stop = (unsigned int)fii->pepArrSz;
		pool->Launch(s);
		pos += set;
	}
	pool->WaitForQueuedParams();
	pool->WaitForThreads();
	delete pool;

	//roll-up all binSz to the last array index, such that each thread starts at the point in the final
	//array where the last thread finishes.
	for (int a = 1;a < th;a++) {
		for (int b = 0;b < z;b++) {
			for (int c = 0;c < maxBin;c++) {
				binSz[a * z * maxBin + b * maxBin + c] += binSz[(a - 1) * z * maxBin + b * maxBin + c];
			}
		}
	}

	//The last binSz array after roll-up contains the full amount of memory to allocate
	int lastBin = th - 1;
	size_t frags = 0;
	size_t bytes = 0;
	for (int a = 0;a < 3;a++) {
		fii->bins[a] = new unsigned int* [maxBin];
		fii->binSz[a] = new unsigned int[maxBin]();

		for (size_t b = 0;b < maxBin;b++) {
			unsigned int sz = binSz[lastBin * z * maxBin + a * maxBin + b];
			if (sz > 0) {
				fii->bins[a][b] = new unsigned int[sz];
				frags += sz;
				bytes += sz * sizeof(unsigned int);
			}
		}
	}

	cout << "Total fragment ions: " << frags << endl;
	cout << "Estimated frament index size: " << (double)bytes / 1073741824 << " Gb." << endl;

	//Generate the peptide index in a threaded manner
	ThreadPool<sGenIndex*>* pool2 = new ThreadPool<sGenIndex*>(CalcIndexProcess, threads, threads, 1);
	pos = 0;
	for (int a = 0;a < th;a++) {
		sGenIndex* s = new sGenIndex(fii, pos, pos + set, NULL,&mask[a]);
		if (a > 0) s->arr = &binSz[(a - 1) * z * maxBin];
		if (a == th - 1) s->stop = (unsigned int)fii->pepArrSz;
		pool2->Launch(s);
		pos += set;
	}
	pool2->WaitForQueuedParams();
	pool2->WaitForThreads();
	delete pool2;

	//Copy over index bin sizes (from last array)
	for (int a = 0;a < 3;a++) {
		for (size_t b = 0;b < maxBin;b++) {
			fii->binSz[a][b] = binSz[lastBin * z * maxBin + a * maxBin + b];
		}
	}

	//free memory
	delete[] binSz;
	delete[] mask;
}

//Fragment ion indexes for crosslinkers are generated similarly to regular indexes, but are split into
//multiple indexes according to whether or not the crosslinker is attached to the fragment ion.
void FIManager::LocalGenerateIndexXL() {

	//Generate index to temporary memory, then add them all up in order
	size_t th = threads;
	size_t z = 3;
	size_t maxBin = fii->maxBin;
	unsigned int* binSzA = new unsigned int[th * z * maxBin]();
	unsigned int* binSzB = new unsigned int[th * z * maxBin]();
	FIMask* mask = new FIMask[threads];
	for (size_t a = 0;a < threads;a++) mask[a].Allocate(MAXPEPLEN * 2 * 3, fii->maxBin);

	//Determine the index sizes at each bin within each thread
	ThreadPool<sGenIndexXL*>* pool = new ThreadPool<sGenIndexXL*>(CalcIndexSzProcessXL, threads, threads, 1);
	unsigned int set = (unsigned int)(fii->pepArrSzXL / threads);
	unsigned int pos = 0;
	for (int a = 0;a < th;a++) {
		sGenIndexXL* s = new sGenIndexXL(fii, pos, pos + set, &binSzA[a * z * maxBin], &binSzB[a * z * maxBin], &mask[a]);
		if (a == th - 1) s->stop = (unsigned int)fii->pepArrSzXL;
		pool->Launch(s);
		pos += set;
	}
	pool->WaitForQueuedParams();
	pool->WaitForThreads();
	delete pool;

	//roll-up all binSz to the last array index, such that each thread starts at the point in the final
	//array where the last thread finishes.
	for (int a = 1;a < th;a++) {
		for (int b = 0;b < z;b++) {
			for (int c = 0;c < maxBin;c++) {
				binSzA[a * z * maxBin + b * maxBin + c] += binSzA[(a - 1) * z * maxBin + b * maxBin + c];
				binSzB[a * z * maxBin + b * maxBin + c] += binSzB[(a - 1) * z * maxBin + b * maxBin + c];
			}
		}
	}

	//The last binSz array after roll-up contains the full amount of memory to allocate
	int lastBin = th - 1;
	size_t fragsA = 0;
	size_t bytesA = 0;
	size_t fragsB = 0;
	size_t bytesB = 0;
	for (int a = 0;a < 3;a++) {
		fii->binsXLA[a] = new unsigned int* [maxBin];
		fii->binSzXLA[a] = new unsigned int[maxBin]();
		fii->binsXLB[a] = new unsigned int* [maxBin];
		fii->binSzXLB[a] = new unsigned int[maxBin]();

		for (size_t b = 0;b < maxBin;b++) {
			unsigned int sz = binSzA[lastBin * z * maxBin + a * maxBin + b];
			if (sz > 0) {
				fii->binsXLA[a][b] = new unsigned int[sz];
				fragsA += sz;
				bytesA += sz * sizeof(unsigned int);
			}
			sz = binSzB[lastBin * z * maxBin + a * maxBin + b];
			if (sz > 0) {
				fii->binsXLB[a][b] = new unsigned int[sz];
				fragsB += sz;
				bytesB += sz * sizeof(unsigned int);
			}
		}
	}

	cout << "Total fragment ions: " << fragsA << "\t" << fragsB << endl;
	cout << "Estimated frament index size: " << (double)bytesA / 1073741824 << "\t" << (double)bytesB / 1073741824 << " Gb." << endl;

	//Generate the peptide index in a threaded manner
	ThreadPool<sGenIndexXL*>* pool2 = new ThreadPool<sGenIndexXL*>(CalcIndexProcessXL, threads, threads, 1);
	pos = 0;
	for (int a = 0;a < th;a++) {
		sGenIndexXL* s = new sGenIndexXL(fii, pos, pos + set, NULL,NULL, &mask[a]);
		if (a > 0) {
			s->arrA = &binSzA[(a - 1) * z * maxBin];
			s->arrB = &binSzB[(a - 1) * z * maxBin];
		}
		if (a == th - 1) s->stop = (unsigned int)fii->pepArrSzXL;
		pool2->Launch(s);
		pos += set;
	}
	pool2->WaitForQueuedParams();
	pool2->WaitForThreads();
	delete pool2;

	//Copy over index bin sizes (from last array)
	for (int a = 0;a < 3;a++) {
		for (size_t b = 0;b < maxBin;b++) {
			fii->binSzXLA[a][b] = binSzA[lastBin * z * maxBin + a * maxBin + b];
			fii->binSzXLB[a][b] = binSzB[lastBin * z * maxBin + a * maxBin + b];
		}
	}

	//free memory
	delete[] binSzA;
	delete[] binSzB;
	delete[] mask;
}

bool FIManager::ScoreSpectrum(vector<FISpectrum>& scans) {

	ThreadPool<sSearchStruct*>* searchPool = new ThreadPool<sSearchStruct*>(ScoreSpectrumProcess, threads, threads, 1);
	for (size_t b = 0;b < scans.size();b++) {
		if (b % 10000 == 0) cout << ".";
		searchPool->WaitForQueuedParams();
		sSearchStruct* s = new sSearchStruct(fii,&scans[b]);
		searchPool->Launch(s);
	}
	searchPool->WaitForQueuedParams();
	searchPool->WaitForThreads();
	
	delete searchPool;
	return true;
}

bool FIManager::ScoreSpectrum(DataLoader& scans) {

	ThreadPool<sSearchStruct*>* searchPool = new ThreadPool<sSearchStruct*>(ScoreSpectrumProcess, threads, threads, 1);
	for (size_t b = 0;b < scans.Size();b++) {
		if (b % 10000 == 0) cout << ".";
		searchPool->WaitForQueuedParams();
		sSearchStruct* s = new sSearchStruct(fii, &scans[b]);
		searchPool->Launch(s);
	}
	searchPool->WaitForQueuedParams();
	searchPool->WaitForThreads();

	delete searchPool;
	return true;
}

void FIManager::ScoreSpectrumProcess(sSearchStruct* s) {
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
	if(s->fii->pepArrSz>0) s->fii->ScoreSpectrum(*s->scan,mem.scores[i]);
	if (s->fii->pepArrSzXL > 0) {
		s->fii->ScoreSpectrumXLFirst(*s->scan, mem.scoresXL[i]);
		//s->fii->ScoreSpectrumXLSecondExtended(*s->scan, mem.scoresXL[i]);
		s->fii->ScoreSpectrumXLSecond(*s->scan);
		s->fii->ScoreSpectrumXLThird(*s->scan, mem.scoresXL[i]);
	}
	delete s;
	s = nullptr;
}

void FIManager::SetBinSize(double d) {
	fii->SetBinSize(d);
}
