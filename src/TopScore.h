#ifndef _TOPSCORE_H
#define _TOPSCORE_H

#include <algorithm>

#include "GlobalDefinitions.h"

/// <summary>
/// Holds the score and the peptide reference index of a PSM
/// </summary>
typedef struct ScoreStruct {
	double eValue = 9999;
	double score = 0;
	size_t index=0;
} ScoreStruct;

/// <summary>
/// Class for holding, in order from highest to lowest, the top user-defined number of PSMs
/// from a database search.
/// </summary>
class TopScore {
public:
	TopScore();
	TopScore(const TopScore& s);
	~TopScore();

	TopScore& operator=(const TopScore& s);
	ScoreStruct& operator[](const size_t& index);

	void CalcEValue(/*bool diag = false*/);
	void CheckScore(const double& score, const size_t& index, bool eval=false);
	void Init(const size_t& sz);
	size_t Size();

	ScoreStruct* scores = nullptr;

protected:
private:


	//TODO: simplify this function to only what is needed in this analysis
	void LinearRegression2(double& slope, double& intercept, int& iMaxXcorr, int& iStartXcorr, int& iNextXcorr, double& rSquared);

	size_t size=0;


	//TODO: Move memory for calculating evalues to some global memory pool
	int histogram[HISTOSZ] = { 0 };
	int histogramCount=0;
	double dCummulative[HISTOSZ] = { 0 };

};

#endif
