#include "FIMemoryManager.h"

using namespace std;

FIMemoryManager::FIMemoryManager() {

}

FIMemoryManager::~FIMemoryManager() {
	DeallocateScores();
	DeallocateScoresXL();
}

bool FIMemoryManager::AllocateScores(int threadCount, size_t sz) {
	DeallocateScores();
	threads = threadCount;
	size = sz;
	scores = new double*[threads];
	for (size_t a = 0;a < threads;a++) scores[a] = new double[size];
	return true;
}

bool FIMemoryManager::AllocateScoresXL(int threadCount, size_t sz) {
	DeallocateScoresXL();
	threadsXL = threadCount;
	sizeXL = sz;
	scoresXL = new float* [threadsXL];
	for (size_t a = 0;a < threadsXL;a++) scoresXL[a] = new float[sizeXL];
	return true;
}

void FIMemoryManager::DeallocateScores() {
	if (scores != nullptr) {
		for (size_t a = 0;a < threads;a++) delete[] scores;
	}
}

void FIMemoryManager::DeallocateScoresXL() {
	if (scoresXL != nullptr) {
		for (size_t a = 0;a < threadsXL;a++) delete[] scoresXL;
	}
}
