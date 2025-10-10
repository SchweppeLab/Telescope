#include "FIMask.h"

using namespace std;

FIMask::FIMask() {
	mask = nullptr;
	list = nullptr;
	listSz = 0;
}

FIMask::FIMask(const size_t sz, const size_t binSz) {
	mask = new bool[binSz]();
	list = new size_t[sz]();
	listSz = 0;
}

FIMask::~FIMask() {
	Deallocate();
}

bool& FIMask::operator[](const size_t& index) {
	return mask[index];
}

void FIMask::Add(const size_t& bin) {
	list[listSz++] = bin;
}

void FIMask::Allocate(const size_t sz, const size_t binSz) {
	Deallocate();
	mask = new bool[binSz]();
	list = new size_t[sz]();
}

void FIMask::Deallocate() {
	listSz = 0;
	if (mask != nullptr) delete[] mask;
	if (list != nullptr) delete[] list;
	mask = nullptr;
	list = nullptr;
}

void FIMask::Reset() {
	for (size_t a = 0;a < listSz;a++) mask[list[a]] = false;
	listSz = 0;
}