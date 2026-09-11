#include "FISpectrum.h"

using namespace std;

/// <summary>
/// Default constructor
/// </summary>
FISpectrum::FISpectrum() {
}

/// <summary>
/// Copy constructor
/// </summary>
/// <param name="s"></param>
FISpectrum::FISpectrum(const FISpectrum& s) {
	curSz = s.curSz;
	maxSz = s.maxSz;
	precursor = s.precursor;
	scanNumber = s.scanNumber;

	if (maxSz > 0) {
		peaks = new FIPeak[maxSz];
		for (size_t a = 0;a < curSz;a++) peaks[a] = s.peaks[a];
	}
}

/// <summary>
/// Destructor
/// </summary>
FISpectrum::~FISpectrum() {
	Deallocate();
}

/// <summary>
/// Copy operator
/// </summary>
/// <param name="s"></param>
/// <returns></returns>
FISpectrum& FISpectrum::operator=(const FISpectrum& s) {
	if (this != &s) {
		curSz = s.curSz;
		maxSz = s.maxSz;
		precursor = s.precursor;
		scanNumber = s.scanNumber;

		if (peaks) {
			delete[] peaks;
			peaks = nullptr;
		}
		if (maxSz > 0) {
			peaks = new FIPeak[maxSz];
			for (size_t a = 0;a < curSz;a++) peaks[a] = s.peaks[a];
		}
	}
	return *this;
}

/// <summary>
/// Array subscript operator to retrieve a spectrum peak
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
FIPeak& FISpectrum::operator[](const size_t& index) {
	return peaks[index];
}

/// <summary>
/// Adds a peak to the spectrum
/// </summary>
/// <param name="fIndex"></param>
/// <param name="value"></param>
void FISpectrum::AddPeak(const size_t& fIndex, const double& mz, const float& value) {
	peaks[curSz].fIndex = fIndex;
	peaks[curSz].value = value;
	peaks[curSz++].mz = mz;
}

/// <summary>
/// Adds a peak to the spectrum
/// </summary>
/// <param name="peak"></param>
void FISpectrum::AddPeak(const FIPeak& peak) {
	peaks[curSz++] = peak;
}

/// <summary>
/// Allocates the number of peaks in the spectrum
/// </summary>
/// <param name="sz"></param>
bool FISpectrum::Allocate(const size_t& sz) {
	Deallocate();
	maxSz = sz;
	peaks = new FIPeak[maxSz]();
	return true;
}

/// <summary>
/// The current peak vector capacity of the spectrum
/// </summary>
/// <returns></returns>
size_t FISpectrum::Capacity() {
	return maxSz;
}

/// <summary>
/// Clears array. Actually, just resets the iterator position and size to 0.
/// </summary>
void FISpectrum::Clear() {
	curSz = 0;
}

/// <summary>
/// Deallocates the peak memory of the spectrum
/// </summary>
/// <param name="sz"></param>
void FISpectrum::Deallocate() {
	if (peaks) {
		delete[] peaks;
		peaks = nullptr;
		maxSz = 0;
		curSz = 0;
	}
}

/// <summary>
/// The number of peaks in the spectrum
/// </summary>
/// <returns></returns>
size_t FISpectrum::Size() {
	return curSz;
}

/// <summary>
/// Sorts the spectrum by m/z values (low to high)
/// </summary>
void FISpectrum::SortMz() {
	sort(peaks, peaks+curSz, sortMzLH);
}

/// <summary>
/// Comparison function for sorting spectra peaks by m/z from low to high
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <returns></returns>
bool FISpectrum::sortMzLH(const FIPeak& a, const FIPeak& b) {
	return a.mz < b.mz;
}