#include "FIMemoryManager.h"

using namespace std;

FIMemoryManager::FIMemoryManager() {

}

FIMemoryManager::~FIMemoryManager() {
	DeallocateScores();
}

bool FIMemoryManager::AllocateScores(int threadCount, size_t sz) {
	DeallocateScores();
	threads = threadCount;
	size = sz;
	scores = new double*[threads];
	for (size_t a = 0;a < threads;a++) scores[a] = new double[size];
	return true;
}

void FIMemoryManager::DeallocateScores() {
	if (scores) {
		for (size_t a = 0;a < threads;a++) delete[] scores[a];
		delete[] scores;
		scores = nullptr;
	}
}
