#ifndef _FISPECTRUM_H
#define _FISPECTRUM_H

#include <algorithm>
#include <string>
#include <vector>

#include "TopScore.h"

/// <summary>
/// In theory, this structure for a peak can be reduced to just its m/z bin (fIndex) and peak intensity (value).
/// However, having the original mz is useful for diagnostics purposes. It can be eliminated if memory becomes an
/// issue (i.e., many, many, many spectra to search).
/// </summary>
typedef struct FIPeak {
	size_t fIndex = 0;
	double mz = 0;
	float value = 0;
} FIPeak;

typedef struct FIPrecursor {
	TopScore ts;
	double mass = 0;				//the monoisotopic neutral mass.
	size_t pepOffset = 0;
	size_t topIndex = 0;
	double topScore = 0;
	int scoreCount = 0;
	int charge = 0;
	std::string peptide;  //not normally here, for diagnostics only

	FIPrecursor() {
		mass = 0;
		charge = 0;
		pepOffset = 0;
		scoreCount = 0;
		topScore = 0;
		topIndex = 0;
	}
	FIPrecursor(const FIPrecursor& s) {
		mass = s.mass;
		charge = s.charge;
		pepOffset = s.pepOffset;
		scoreCount = s.scoreCount;
		topScore = s.topScore;
		topIndex = s.topIndex;
		peptide = s.peptide;
		ts = s.ts;
	}
	FIPrecursor& operator=(const FIPrecursor& s) {
		if (this != &s) {
			mass = s.mass;
			charge = s.charge;
			pepOffset = s.pepOffset;
			scoreCount = s.scoreCount;
			topScore = s.topScore;
			topIndex = s.topIndex;
			peptide = s.peptide;
			ts = s.ts;
		}
		return *this;
	}
	~FIPrecursor() {
	}
} FIPrecursor;


/// <summary>
/// Class for storing mass spectra for database search. Not the fanciest, or most efficient (see the vectors...), 
/// way to store the information. But the class is noteworthy for allowing multiple precursor assignments 
/// (maintaining independent PSM score sets), allowing for isotope offsets or chimeric spectra search.
/// </summary>
class FISpectrum {
public:
	FIPeak& operator[](const size_t& index);

	void AddPeak(const size_t& fIndex, const float& value);
	void AddPeak(const FIPeak& peak);
	size_t Capacity();
	std::vector<FIPeak>& GetPeaks();
	size_t Size();
	void SortMz();

	std::vector<FIPrecursor> precursor;
	int scanNumber = 0;

protected:
private:

	std::vector<FIPeak> peaks;  //Should be no higher than highBin to save memory...

	static bool sortMzLH(const FIPeak& a, const FIPeak& b);
};

#endif