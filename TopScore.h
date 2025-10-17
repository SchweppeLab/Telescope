#ifndef _TOPSCORE_H
#define _TOPSCORE_H

#include <algorithm>

/// <summary>
/// Holds the score and the peptide reference index of a PSM
/// </summary>
typedef struct ScoreStruct {
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

	void CheckScore(const double& score, const size_t& index);
	void Init(const size_t& sz);
	size_t Size();

	ScoreStruct* scores = nullptr;

protected:
private:

	size_t size=0;

};

#endif
