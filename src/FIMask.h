#ifndef _FIMASK_H
#define _FIMASK_H

/// <summary>
/// This is a support class that is used during fragment ion index generation. Fragment ion masses from a peptide are
/// added to a list in this class when they are computed. If the fragment ion mass is observed again (e.g., in a different
/// ion series or charge state), then the mask prevents it from being added a second time to the fragment ion index.
/// The result is that when peaks are matched to the index, they are only scored once per peptide.
/// 
/// The mask is only used during index generation and can be discarded afterwards. It requires little memory that can
/// be reused for every peptide without reallocation.
/// </summary>
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
