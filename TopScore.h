#ifndef _TOPSCORE_H
#define _TOPSCORE_H

#include <algorithm>

typedef struct ScoreStruct {
	float score = 0;
	size_t index=0;
} ScoreStruct;

class TopScore {
public:
	TopScore();
	TopScore(const TopScore& s);
	~TopScore();

	TopScore& operator=(const TopScore& s);
	ScoreStruct& operator[](const size_t& index);

	void CheckScore(const float& score, const size_t& index);
	void Init(const size_t& sz);
	size_t Size();

	ScoreStruct* scores = nullptr;

protected:
private:

	size_t size=0;

};

typedef struct ScoreStructXL {
	float scoreA = -1e6; //large negative score for to start for non-matches.
	float scoreB = -1e6;
	size_t indexA = 0;
	size_t indexB = 0;
	size_t indexXL = 0;
} ScoreStructXL;

class TopScoreXL {
public:
	TopScoreXL();
	TopScoreXL(const TopScoreXL& s);
	~TopScoreXL();

	TopScoreXL& operator=(const TopScoreXL& s);
	ScoreStructXL& operator[](const size_t& index);

	void CheckScoreA(const float& score, const size_t& index);
	void Init(const size_t& sz);
	size_t Size();
	void Sort();

	ScoreStructXL* scores = nullptr;

protected:
private:

	size_t size = 0;

	static bool sortTotalScore(const ScoreStructXL& a, const ScoreStructXL& b);
};

#endif
