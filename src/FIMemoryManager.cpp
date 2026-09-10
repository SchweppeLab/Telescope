#include "FIMemoryManager.h"

using namespace std;

/// <summary>
/// Default constructor
/// </summary>
FIMemoryManager::FIMemoryManager() {
}

/// <summary>
/// Destructor
/// </summary>
FIMemoryManager::~FIMemoryManager() {
	DeallocateScores();
}

/// <summary>
/// Allocates a block of memory for holding peptide scores from database search. Each call automatically
/// deallocates any previously used memory.
/// </summary>
/// <param name="threadCount">Number of arrays to allocate, one per thread reserved for searching</param>
/// <param name="sz">Size of the array</param>
/// <returns>true upon success</returns>
bool FIMemoryManager::AllocateScores(int threadCount, size_t sz) {
	DeallocateScores();
	threads = threadCount;
	size = sz;
	scores = new double*[threads];
	for (size_t a = 0;a < threads;a++) scores[a] = new double[size];
	return true;
}

/// <summary>
/// Frees all memory used by the fragment ion index memory manager.
/// </summary>
void FIMemoryManager::DeallocateScores() {
	if (scores) {
		for (size_t a = 0;a < threads;a++) delete[] scores[a];
		delete[] scores;
		scores = nullptr;
	}
}
