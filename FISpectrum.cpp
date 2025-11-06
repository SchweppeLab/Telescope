#include "FISpectrum.h"

using namespace std;

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
void FISpectrum::AddPeak(const size_t& fIndex, const float& value) {
	peaks.emplace_back();
	peaks.back().fIndex = fIndex;
	peaks.back().value = value;
}

/// <summary>
/// Adds a peak to the spectrum
/// </summary>
/// <param name="peak"></param>
void FISpectrum::AddPeak(const FIPeak& peak) {
	peaks.push_back(peak);
}

/// <summary>
/// The current peak vector capacity of the spectrum
/// </summary>
/// <returns></returns>
size_t FISpectrum::Capacity() {
	return peaks.capacity();
}

/// <summary>
/// The reference to the vector of spectrum peaks
/// </summary>
/// <returns></returns>
vector<FIPeak>& FISpectrum::GetPeaks() {
	return peaks;
}

/// <summary>
/// Preallocates the number of peaks in the spectrum
/// </summary>
/// <param name="sz"></param>
void FISpectrum::Reserve(const size_t& sz) {
	peaks.reserve(sz);
}

/// <summary>
/// The number of peaks in the spectrum
/// </summary>
/// <returns></returns>
size_t FISpectrum::Size() {
	return peaks.size();
}

/// <summary>
/// Sorts the spectrum by m/z values (low to high)
/// </summary>
void FISpectrum::SortMz() {
	sort(peaks.begin(), peaks.end(), sortMzLH);
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



FISpectrum2::FISpectrum2() {
}

FISpectrum2::FISpectrum2(const FISpectrum2& s) {
	curSz = s.curSz;
	maxSz = s.maxSz;
	precursor = s.precursor;
	scanNumber = s.scanNumber;

	if (maxSz > 0) {
		peaks = new FIPeak[maxSz];
		for (size_t a = 0;a < curSz;a++) peaks[a] = s.peaks[a];
	}
}

FISpectrum2::~FISpectrum2() {
	Deallocate();
}

FISpectrum2& FISpectrum2::operator=(const FISpectrum2& s) {
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
FIPeak& FISpectrum2::operator[](const size_t& index) {
	return peaks[index];
}

/// <summary>
/// Adds a peak to the spectrum
/// </summary>
/// <param name="fIndex"></param>
/// <param name="value"></param>
void FISpectrum2::AddPeak(const size_t& fIndex, const double& mz, const float& value) {
	peaks[curSz].fIndex = fIndex;
	peaks[curSz].value = value;
	peaks[curSz++].mz = mz;
}

/// <summary>
/// Adds a peak to the spectrum
/// </summary>
/// <param name="peak"></param>
void FISpectrum2::AddPeak(const FIPeak& peak) {
	peaks[curSz++] = peak;
}

/// <summary>
/// Allocates the number of peaks in the spectrum
/// </summary>
/// <param name="sz"></param>
bool FISpectrum2::Allocate(const size_t& sz) {
	Deallocate();
	maxSz = sz;
	peaks = new FIPeak[maxSz]();
	return true;
}

/// <summary>
/// The current peak vector capacity of the spectrum
/// </summary>
/// <returns></returns>
size_t FISpectrum2::Capacity() {
	return maxSz;
}

void FISpectrum2::Clear() {
	curSz = 0;
}

/// <summary>
/// Deallocates the peak memory of the spectrum
/// </summary>
/// <param name="sz"></param>
void FISpectrum2::Deallocate() {
	if (peaks) {
		delete[] peaks;
		peaks = nullptr;
		maxSz = 0;
		curSz = 0;
	}
}

/// <summary>
/// The reference to the vector of spectrum peaks
/// </summary>
/// <returns></returns>
//FIPeak* FISpectrum2::GetPeaks() {
//	return peaks;
//}

/// <summary>
/// The number of peaks in the spectrum
/// </summary>
/// <returns></returns>
size_t FISpectrum2::Size() {
	return curSz;
}

/// <summary>
/// Sorts the spectrum by m/z values (low to high)
/// </summary>
void FISpectrum2::SortMz() {
	sort(peaks, peaks+curSz, sortMzLH);
}

/// <summary>
/// Comparison function for sorting spectra peaks by m/z from low to high
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <returns></returns>
bool FISpectrum2::sortMzLH(const FIPeak& a, const FIPeak& b) {
	return a.mz < b.mz;
}