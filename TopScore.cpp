#include "TopScore.h"

using namespace std;

/// <summary>
/// Default constructor
/// </summary>
TopScore::TopScore() {
}

/// <summary>
/// Copy constructor
/// </summary>
/// <param name="s"></param>
TopScore::TopScore(const TopScore& s) {
	size = s.size;
	if (size > 0) {
		scores = new ScoreStruct[size]();
		for (size_t a = 0;a < size;a++) scores[a] = s.scores[a];
	} else scores = nullptr;
}

/// <summary>
/// Default destructor
/// </summary>
TopScore::~TopScore() {
	if(scores) delete[] scores;
}

/// <summary>
/// Copy operator
/// </summary>
/// <param name="s"></param>
/// <returns></returns>
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

/// <summary>
/// Array subscript operator to retrieve a specific PSM score.
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
ScoreStruct& TopScore::operator[](const size_t& index) {
	return scores[index];
}

/// <summary>
/// Checks a PSM score against the top scores, and inserts in in the appropriate position.
/// </summary>
/// <param name="score"></param>
/// <param name="index"></param>
void TopScore::CheckScore(const double& score, const size_t& index) {
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

/// <summary>
/// Initializes the class and allocates the number of top PSMs to store.
/// </summary>
/// <param name="sz"></param>
void TopScore::Init(const size_t& sz) {
	size = sz;
	if (scores) delete[] scores;
	if (size > 0) scores = new ScoreStruct[size]();
	else scores = nullptr;
}

/// <summary>
/// The number of top PSMs being stored.
/// </summary>
/// <returns></returns>
size_t TopScore::Size() {
	return size;
}
