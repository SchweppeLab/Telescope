#ifndef _FIMEMORYMANAGER_H
#define _FIMEMORYMANAGER_H

class FIMemoryManager {
public:
	FIMemoryManager();
	~FIMemoryManager();

	bool AllocateScores(int threadCouunt, size_t sz);

	double** scores = nullptr;    //standard, single PSM scores, one array per thread

protected:
private:

	void DeallocateScores();

	size_t size = 0;
	size_t threads = 0;

};

#endif