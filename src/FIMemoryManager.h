#ifndef _FIMEMORYMANAGER_H
#define _FIMEMORYMANAGER_H


/// <summary>
/// Provides a thread-safe set of memory pools for use in database searching.
/// This class might be extended to manage other memory pools currently locked inside other classes.
/// </summary>
class FIMemoryManager {
public:
	FIMemoryManager();
	~FIMemoryManager();

	bool AllocateScores(int threadCount, size_t sz);

	double** scores = nullptr;    //standard, single PSM scores, one array per thread

protected:
private:

	void DeallocateScores();

	size_t size = 0;
	size_t threads = 0;

};

#endif