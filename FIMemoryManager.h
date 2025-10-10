#ifndef _FIMEMORYMANAGER_H
#define _FIMEMORYMANAGER_H

class FIMemoryManager {
public:
	FIMemoryManager();
	~FIMemoryManager();

	bool AllocateScores(int threadCouunt, size_t sz);
	bool AllocateScoresXL(int threadCount, size_t sz);

	double** scores = nullptr;    //standard, single PSM scores, one array per thread
	float** scoresXL = nullptr;  //crosslinked scores: much larger arrays per thread

protected:
private:

	void DeallocateScores();
	void DeallocateScoresXL();

	size_t size = 0;
	size_t sizeXL = 0;

	size_t threads = 0;
	size_t threadsXL = 0;
};

#endif