#ifndef _FISPECTRUM_H
#define _FISPECTRUM_H

#include <algorithm>
#include <string>
#include <vector>

#include "TopScore.h"

typedef struct FIPeak {
	size_t fIndex = 0;
	double mz = 0;
	float value = 0;
} FIPeak;

typedef struct FIPrecursor {
	TopScore ts;
	TopScoreXL tsXL;
	double mass = 0;				//the monoisotopic neutral mass.
	size_t pepOffset = 0;
	size_t pepOffsetXL = 0;
	size_t topIndex = 0;
	double topScore = 0;
	int scoreCount = 0;
	int scoreCountXL = 0;
	int charge = 0;
	std::string peptide;  //not normally here, for diagnostics only

	FIPrecursor() {
		mass = 0;
		charge = 0;
		pepOffset = 0;
		pepOffsetXL = 0;
		scoreCount = 0;
		scoreCountXL = 0;
		topScore = 0;
		topIndex = 0;
		ts.Init(10);
		tsXL.Init(20);
	}
	FIPrecursor(const FIPrecursor& s) {
		mass = s.mass;
		charge = s.charge;
		pepOffset = s.pepOffset;
		pepOffsetXL = s.pepOffsetXL;
		scoreCount = s.scoreCount;
		scoreCountXL = s.scoreCountXL;
		topScore = s.topScore;
		topIndex = s.topIndex;
		peptide = s.peptide;
		ts = s.ts;
		tsXL = s.tsXL;
	}
	FIPrecursor& operator=(const FIPrecursor& s) {
		if (this != &s) {
			mass = s.mass;
			charge = s.charge;
			pepOffset = s.pepOffset;
			pepOffsetXL = s.pepOffsetXL;
			scoreCount = s.scoreCount;
			scoreCountXL = s.scoreCountXL;
			topScore = s.topScore;
			topIndex = s.topIndex;
			peptide = s.peptide;
			ts = s.ts;
			tsXL = s.tsXL;
		}
		return *this;
	}
	~FIPrecursor() {
	}
} FIPrecursor;


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