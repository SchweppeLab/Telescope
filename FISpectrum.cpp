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
