#ifndef _FIMASK_H
#define _FIMASK_H

class FIMask {
public:
	FIMask();
	FIMask(const size_t sz, const size_t binSz);
	~FIMask();

	bool& operator[](const size_t& index);
	void Add(const size_t& bin);
	void Allocate(const size_t sz, const size_t binSz);
	void Deallocate();
	void Reset();

private:
	bool* mask=nullptr;
	size_t* list=nullptr;
	size_t listSz = 0;
};

#endif
