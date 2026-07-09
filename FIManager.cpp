#include "FIManager.h"

using namespace std;

FIMemoryManager FIManager::mem;

Mutex FIManager::mutexThreads;
bool* FIManager::activeThread;
size_t FIManager::threads;
long long FIManager::memUse;

/// <summary>
/// Default constructor. FIManager::Initialize() must be called after construction to use the FIManager object.
/// </summary>
FIManager::FIManager() {
	threads = 1;
	memUse = 0;
	activeThread = nullptr;
}

/// <summary>
/// Destructor.
/// </summary>
FIManager::~FIManager() {
	dbm = nullptr;
	params = nullptr;
	Deallocate();
}

/// <summary>
/// Allocates the memory necessary for fragment ion index manager functionality.
/// </summary>
void FIManager::Allocate() {
	Deallocate();
	threads = params->threads;
	activeThread = new bool[threads]();
	ThreadingT::CreateMutex(&mutexThreads);
}

/// <summary>
/// Allocates the memory required for scoring by calling the underlying fragment ion index memory manager.
/// </summary>
/// <param name="sz">The size of the array to allocate</param>
/// <returns>true upon success</returns>
bool FIManager::AllocateScoreMemory(const size_t& sz) {
	mem.AllocateScores((int)threads, sz);
	return true;
}

/// <summary>
/// Starts the thread process that calls FragmentIonIndex::PopulateIndex()
/// </summary>
/// <param name="s"></param>
void FIManager::CalcIndexProcess(sGenIndex* s) {
	s->fii->PopulateIndex(s->start, s->stop, s->arr,*s->mask);
	delete s;
}

/// <summary>
/// Starts the thread process that calls FragmentIonIndex::CalculateIndex()
/// </summary>
/// <param name="s"></param>
void FIManager::CalcIndexSzProcess(sGenIndex* s) {
	s->fii->CalculateIndex(s->start, s->stop, s->arr, *s->mask);
	delete s;
}

/// <summary>
/// Frees memory resources in use.
/// </summary>
void FIManager::Deallocate() {
	//if (fii) delete fii;
	if (activeThread) {
		delete[] activeThread;
		ThreadingT::DestroyMutex(mutexThreads);
	}
}

/// <summary>
/// Generates the fragment ion index. Will return false if a previous call to FIManager::GeneratePeptideMap() has not
/// been made.
/// </summary>
/// <returns>true if successful</returns>
bool FIManager::GenerateIndex() {	
	if (fii.pepArrSz > 0) InternalGenerateIndex();
	else return false;
	return true;
}

/// <summary>
/// Calls the GeneratePeptideMap function of the underlying fragment ion index class.
/// </summary>
/// <returns>true if successful</returns>
bool FIManager::GeneratePeptideMap() {
	return fii.GeneratePeptideMap();
}

/// <summary>
/// Connects the fragment ion index manager to support objects and allocates resources so that it
/// is ready for use.
/// </summary>
/// <param name="d"></param>
/// <param name="p"></param>
void FIManager::Initialize(DBManager* d, ParamsManager* p) {
	dbm = d;
	params = p;
	fii.Initialize(d, p);
	Allocate();
}

/// <summary>
/// Generating the fragment ion index from here is multi-threaded. It divides the peptidoforms in the 
/// database manager into equal parts for each thread. Each thread then determines the memory needed for its
/// portion of the index. The total memory is allocated, then each thread populates the index for its peptidoforms
/// using the offsets determined at the memory calculation stage. No two threads will ever access the same
/// block of memory, no semaphores are needed, and the processes are entirely parallel.
/// </summary>
void FIManager::InternalGenerateIndex() {

	//Generate index to temporary memory, then add them all up in order
	size_t th = threads;
	size_t z = params->maxFragZ;
	size_t maxBin = fii.maxBin;
	unsigned int* binSz = new unsigned int[th * z * maxBin]();
	FIMask* mask = new FIMask[threads];
	for (size_t a = 0;a < threads;a++) mask[a].Allocate(params->maxPepLen * 2 * params->maxFragZ, fii.maxBin);

	//Determine the index sizes at each bin within each thread
	ThreadPool<sGenIndex*>* pool = new ThreadPool<sGenIndex*>(CalcIndexSzProcess, (int)threads, (int)threads, 1);
	unsigned int set = (unsigned int)(fii.pepArrSz / threads);
	unsigned int pos = 0;
	for (int a = 0;a < th;a++) {
		sGenIndex* s = new sGenIndex(&fii, pos, pos + set, &binSz[a * z * maxBin],&mask[a]);
		if (a == th - 1) s->stop = (unsigned int)fii.pepArrSz;
		pool->Launch(s);
		pos += set;
	}
	pool->WaitForQueuedParams();
	pool->WaitForThreads();
	delete pool;

	//Roll-up all binSz to the last array index, such that each thread starts at the point in the final
	//array where the last thread finishes.
	for (int a = 1;a < th;a++) {
		for (int b = 0;b < z;b++) {
			for (int c = 0;c < maxBin;c++) {
				binSz[a * z * maxBin + b * maxBin + c] += binSz[(a - 1) * z * maxBin + b * maxBin + c];
			}
		}
	}

	//The last binSz array after roll-up contains the full amount of memory to allocate
	int lastBin = (int)th - 1;
	size_t frags = 0;
	size_t bytes = 0;
	for (int a = 0;a < params->maxFragZ;a++) {
		fii.bins[a] = new unsigned int* [maxBin];
		fii.binSz[a] = new unsigned int[maxBin]();

		for (size_t b = 0;b < maxBin;b++) {
			unsigned int sz = binSz[lastBin * z * maxBin + a * maxBin + b];
			if (sz > 0) {
				fii.bins[a][b] = new unsigned int[sz];
				frags += sz;
				bytes += sz * sizeof(unsigned int);
			}
		}
	}

	//TODO: Get rid of this or find a better place to export messages to the user
	cout << "Total fragment ions: " << frags << endl;
	cout << "Estimated fragment index size: " << (double)bytes / 1073741824 << " Gb." << endl;
	memUse = bytes;

	//Generate the peptide index in a threaded manner
	ThreadPool<sGenIndex*>* pool2 = new ThreadPool<sGenIndex*>(CalcIndexProcess, (int)threads, (int)threads, 1);
	pos = 0;
	for (int a = 0;a < th;a++) {
		sGenIndex* s = new sGenIndex(&fii, pos, pos + set, NULL,&mask[a]);
		if (a > 0) s->arr = &binSz[(a - 1) * z * maxBin];
		if (a == th - 1) s->stop = (unsigned int)fii.pepArrSz;
		pool2->Launch(s);
		pos += set;
	}
	pool2->WaitForQueuedParams();
	pool2->WaitForThreads();
	delete pool2;

	//Copy over index bin sizes (from last array)
	for (int a = 0;a < params->maxFragZ;a++) {
		for (size_t b = 0;b < maxBin;b++) {
			fii.binSz[a][b] = binSz[lastBin * z * maxBin + a * maxBin + b];
		}
	}

	//Free memory
	delete[] binSz;
	delete[] mask;
}

/// <summary>
/// Performs the fragment ion index database search on a set of scans. Each scan is distributed to the next
/// available thread.
/// </summary>
/// <param name="scans">FISpectrum object</param>
/// <returns>true if successful</returns>
bool FIManager::ScoreSpectrum(vector<FISpectrum>& scans) {

	ThreadPool<sSearchStruct*>* searchPool = new ThreadPool<sSearchStruct*>(ScoreSpectrumProcess, (int)threads, (int)threads, 1);
	for (size_t b = 0;b < scans.size();b++) {
		if (b % 10000 == 0) cout << ".";
		searchPool->WaitForQueuedParams();
		sSearchStruct* s = new sSearchStruct(&fii,&scans[b]);
		searchPool->Launch(s);
	}
	searchPool->WaitForQueuedParams();
	searchPool->WaitForThreads();
	
	delete searchPool;
	return true;
}

/// <summary>
/// Performs the fragment ion index database search on a set of scans. Each scan is distributed to the next
/// available thread.
/// </summary>
/// <param name="scans">DataLoader object</param>
/// <returns>true if successful</returns>
bool FIManager::ScoreSpectrum(FISpectrum* scans, const size_t& count) {

	ThreadPool<sSearchStruct*>* searchPool = new ThreadPool<sSearchStruct*>(ScoreSpectrumProcess, (int)threads, (int)threads, 1);
	for (size_t b = 0;b < count;b++) {
		if (b % 10000 == 0) cout << ".";
		searchPool->WaitForQueuedParams();
		sSearchStruct* s = new sSearchStruct(&fii, &scans[b]);
		searchPool->Launch(s);
	}
	searchPool->WaitForQueuedParams();
	searchPool->WaitForThreads();

	delete searchPool;
	return true;
}

/// <summary>
/// Internal function that finds the next available thread and launches a search on the spectrum.
/// </summary>
/// <param name="s">sSearchStruct</param>
void FIManager::ScoreSpectrumProcess(sSearchStruct* s) {
	//Get next available thread number;
	size_t i;
	ThreadingT::LockMutex(mutexThreads);
	for (i = 0;i < threads;i++) {
		if (!activeThread[i]) {
			activeThread[i] = true;
			break;
		}
	}
	ThreadingT::UnlockMutex(mutexThreads);

	s->mutex = &mutexThreads;
	s->thread = &activeThread[i];
	s->fii->ScoreSpectrum(*s->scan,mem.scores[i]);
	delete s;
	s = nullptr;
}
