#include "TopScore.h"

using namespace std;

TopScore::TopScore() {
}

TopScore::TopScore(const TopScore& s) {
	size = s.size;
	if (size > 0) {
		scores = new ScoreStruct[size]();
		for (size_t a = 0;a < size;a++) scores[a] = s.scores[a];
	} else scores = nullptr;
}

TopScore::~TopScore() {
	if(scores!= nullptr) delete[] scores;
}

TopScore& TopScore::operator=(const TopScore& s) {
	if (this != &s) {
		if(scores!= nullptr) delete[] scores;
		size = s.size;
		if (size > 0) {
			scores = new ScoreStruct[size]();
			for (size_t a = 0;a < size;a++) scores[a] = s.scores[a];
		} else scores = nullptr;
	}
	return *this;
}

ScoreStruct& TopScore::operator[](const size_t& index) {
	return scores[index];
}

void TopScore::CheckScore(const float& score, const size_t& index) {
	size_t a = size - 1;
	if (score > scores[a].score) {
		scores[a].score = score;
		scores[a].index = index;
	} else return;

	while (a > 0) {
		size_t b = a - 1;
		if (scores[a].score > scores[b].score) {
			ScoreStruct tmp = scores[b];
			scores[b] = scores[a];
			scores[a--] = tmp;
		} else return;
	}
}

void TopScore::Init(const size_t& sz) {
	size = sz;
	if (scores != nullptr) delete[] scores;
	if (size > 0) scores = new ScoreStruct[size]();
	else scores = nullptr;
}

size_t TopScore::Size() {
	return size;
}






TopScoreXL::TopScoreXL() {
}

TopScoreXL::TopScoreXL(const TopScoreXL& s) {
	size = s.size;
	if (size > 0) {
		scores = new ScoreStructXL[size]();
		for (size_t a = 0;a < size;a++) scores[a] = s.scores[a];
	} else scores = nullptr;
}

TopScoreXL::~TopScoreXL() {
	if (scores != nullptr) delete[] scores;
}

TopScoreXL& TopScoreXL::operator=(const TopScoreXL& s) {
	if (this != &s) {
		if (scores != nullptr) delete[] scores;
		size = s.size;
		if (size > 0) {
			scores = new ScoreStructXL[size]();
			for (size_t a = 0;a < size;a++) scores[a] = s.scores[a];
		} else scores = nullptr;
	}
	return *this;
}

ScoreStructXL& TopScoreXL::operator[](const size_t& index) {
	return scores[index];
}

void TopScoreXL::CheckScoreA(const float& score, const size_t& index) {
	size_t a = size - 1;
	if (score > scores[a].scoreA) {
		scores[a].scoreA = score;
		scores[a].indexA = index;
	} else return;

	while (a > 0) {
		size_t b = a - 1;
		if (scores[a].scoreA > scores[b].scoreA) {
			ScoreStructXL tmp = scores[b];
			scores[b] = scores[a];
			scores[a--] = tmp;
		} else return;
	}
}

void TopScoreXL::Init(const size_t& sz) {
	size = sz;
	if (scores != nullptr) delete[] scores;
	if (size > 0) scores = new ScoreStructXL[size]();
	else scores = nullptr;
}

size_t TopScoreXL::Size() {
	return size;
}

void TopScoreXL::Sort() {
	sort(scores, scores+size, sortTotalScore);
}

bool TopScoreXL::sortTotalScore(const ScoreStructXL& a, const ScoreStructXL& b) {
	return (a.scoreA + a.scoreB) > (b.scoreA + b.scoreB);
}