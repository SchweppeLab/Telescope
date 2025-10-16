#include "FIMask.h"

using namespace std;

/// <summary>
/// Default constructor
/// </summary>
FIMask::FIMask() {
	mask = nullptr;
	list = nullptr;
	listSz = 0;
}

/// <summary>
/// Overload constructor that allocates memory based on the size.
/// </summary>
/// <param name="sz">The maximum number of fragment ions for the longest possible peptide</param>
/// <param name="binSz">The maximum number of bins in a spectrum</param>
FIMask::FIMask(const size_t sz, const size_t binSz) {
	Allocate(sz, binSz);
	listSz = 0;
}

/// <summary>
/// Default destructor
/// </summary>
FIMask::~FIMask() {
	Deallocate();
}

/// <summary>
/// Returns the value of the mask at an index.
/// </summary>
/// <param name="index">The index position</param>
/// <returns>boolean true or false</returns>
bool& FIMask::operator[](const size_t& index) {
	return mask[index];
}

/// <summary>
/// Adds the index to a list. This list is later used to quickly reset the mask to an empty state by calling FIMask::Reset(),
/// so that only the values in the list are changed, rather than clear the memory of the entire [mostly empty] mask.
/// </summary>
/// <param name="bin">The index position</param>
void FIMask::Add(const size_t& bin) {
	list[listSz++] = bin;
}

/// <summary>
/// Allocates the memory for the mask. Any previously allocated memory is first freed.
/// </summary>
/// <param name="sz">The maximum number of fragment ions for the longest possible peptide</param>
/// <param name="binSz">The maximum number of bins in a spectrum</param>
void FIMask::Allocate(const size_t sz, const size_t binSz) {
	Deallocate();
	mask = new bool[binSz]();
	list = new size_t[sz]();
}

/// <summary>
/// Frees any memory allocated to the mask.
/// </summary>
void FIMask::Deallocate() {
	listSz = 0;
	if (mask != nullptr) delete[] mask;
	if (list != nullptr) delete[] list;
	mask = nullptr;
	list = nullptr;
}

/// <summary>
/// Clears any values from the mask, using the short list of values changed since the last memory allocation
/// or call to FIMask::Reset(), rather than iterating over the entire mask.
/// </summary>
void FIMask::Reset() {
	for (size_t a = 0;a < listSz;a++) mask[list[a]] = false;
	listSz = 0;
}