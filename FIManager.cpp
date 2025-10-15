#include "FIManager.h"

using namespace std;

FIMemoryManager FIManager::mem;

Mutex FIManager::mutexThreads;
bool* FIManager::activeThread;
size_t FIManager::threads;

FIManager::FIManager() {
	threads = 1;
	activeThread = nullptr;
}

FIManager::~FIManager() {
	dbm = NULL;
	Deallocate();
}

void FIManager::Allocate() {
	Deallocate();
	fii = new FragmentIonIndex(dbm,params);
	fii->SetBinSize(params->binSize);
	threads = params->threads;
	activeThread = new bool[threads]();
	Threading::CreateMutex(&mutexThreads);
}

bool FIManager::AllocateScoreMemory(const size_t& sz) {
	mem.AllocateScores(threads, sz);
	return true;
}

void FIManager::CalcIndexProcess(sGenIndex* s) {
	s->fii->PopulateIndex(s->start, s->stop, s->arr,*s->mask);
	delete s;
}

void FIManager::CalcIndexSzProcess(sGenIndex* s) {
	s->fii->CalculateIndex(s->start, s->stop, s->arr, *s->mask);
	delete s;
}

void FIManager::Deallocate() {
	if (fii) delete fii;
	if (activeThread) {
		delete[] activeThread;
		Threading::DestroyMutex(mutexThreads);
	}
}

//Generating the index is a complex process so that it can be multithreaded. Two functions do the work, one
//for regular peptide searches, the other for crosslinked peptides. These functions are only called if there
//are mapped peptidoforms for the types of search.
bool FIManager::GenerateIndex() {	
	if (fii->pepArrSz > 0) LocalGenerateIndex();
	return true;
}

bool FIManager::GeneratePeptideMap() {
	fii->GeneratePeptideMap();
	return true;
}

void FIManager::Initialize(DBManager* d, ParamsManager* p) {
	dbm = d;
	params = p;
	Allocate();
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
	s->fii->ScoreSpectrum(*s->scan,mem.scores[i]);
	delete s;
	s = nullptr;
}
