#include "FISpectrum.h"

using namespace std;

FIPeak& FISpectrum::operator[](const size_t& index) {
	return peaks[index];
}

void FISpectrum::AddPeak(const size_t& fIndex, const float& value) {
	peaks.emplace_back();
	peaks.back().fIndex = fIndex;
	peaks.back().value = value;
}

void FISpectrum::AddPeak(const FIPeak& peak) {
	peaks.push_back(peak);
}

size_t FISpectrum::Capacity() {
	return peaks.capacity();
}

vector<FIPeak>& FISpectrum::GetPeaks() {
	return peaks;
}

size_t FISpectrum::Size() {
	return peaks.size();
}

void FISpectrum::SortMz() {
	sort(peaks.begin(), peaks.end(), sortMzLH);
}

bool FISpectrum::sortMzLH(const FIPeak& a, const FIPeak& b) {
	return a.mz < b.mz;
}
