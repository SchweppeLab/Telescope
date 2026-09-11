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
	histogramCount = s.histogramCount;
	for (int a = 0;a < HISTOSZ;a++) {
		histogram[a] = s.histogram[a];
		dCummulative[a] = s.dCummulative[a];
	}
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
		histogramCount = s.histogramCount;
		for (int a = 0;a < HISTOSZ;a++) {
			histogram[a] = s.histogram[a];
			dCummulative[a] = s.dCummulative[a];
		}
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

void TopScore::CalcEValue(/*bool diag*/) {
	int iMaxCorr;
	int iStartCorr;
	int iNextCorr;
	double dSlope;
	double dIntercept;
	double dRSquare;
	LinearRegression2(dSlope, dIntercept, iMaxCorr, iStartCorr, iNextCorr, dRSquare);
	/*
	if (diag) {
		cout << "histoCount: " << histogramCount << endl;
		cout << "Slope: " << dSlope << endl;
		cout << "Intercept: " << dIntercept << endl;
		cout << "iMaxCorr: " << iMaxCorr << endl;
		cout << "iStartCorr: " << iStartCorr << endl;
		cout << "iNextCorr: " << iNextCorr << endl;
		cout << "dRSquare: " << dRSquare << endl;

		for (int a = 0;a < iMaxCorr;a++) cout << a << "\t" << dCummulative[a] << endl;
	}
	*/

	dSlope *= 10.0;
	if (dSlope >= 0.0) return;

	for (size_t a = 0;a < size;a++) {
		if (scores[a].score <= 0) break;
		scores[a].eValue = pow(10.0, dSlope * scores[a].score + dIntercept);
		if (scores[a].eValue > 9999) {
			scores[a].eValue = 9999;
			break;
		}
	}

}

/// <summary>
/// Checks a PSM score against the top scores, and inserts in in the appropriate position.
/// </summary>
/// <param name="score"></param>
/// <param name="index"></param>
void TopScore::CheckScore(const double& score, const size_t& index, bool eval) {
	if (eval) {
		int index = (int)(score * 10 + 0.5);
		if (index < 0) index = 0;
		else if (index >= HISTOSZ) index = HISTOSZ - 1;
		histogram[index]++;
		histogramCount++;
	}

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
/// Significant differences than what is used in Comet. Mostly these differences are to escape scenarios with poor histogram shape (sometimes due to
/// many variations on the same sequences with modifications shuffled around). I have never evaluated the consequences to speed. The benefit might
/// be minimal for normal database search conditions.
/// </summary>
/// <param name="slope"></param>
/// <param name="intercept"></param>
/// <param name="iMaxXcorr"></param>
/// <param name="iStartXcorr"></param>
/// <param name="iNextXcorr"></param>
/// <param name="rSquared"></param>
void TopScore::LinearRegression2(double& slope, double& intercept, int& iMaxXcorr, int& iStartXcorr, int& iNextXcorr, double& rSquared) {
	double Sx, Sxy;      // Sum of square distances.
	double Mx, My;       // means
	double dx, dy;
	double b, a;
	double SumX, SumY;   // Sum of X and Y values to calculate mean.
	double SST, SSR;
	double rsq;
	double bestRSQ;
	double bestSlope;
	double bestInt;

	int i;
	int bestNC;
	int bestStart;
	int iNextCorr;    // 2nd best xcorr index
	int iMaxCorr = 0;   // max xcorr index
	int iStartCorr;
	int iNumPoints;

	// Find maximum correlation score index.
	for (i = HISTOSZ - 2; i >= 0; i--) {
		if (histogram[i] > 0)  break;
	}
	iMaxCorr = i;

	//bail now if there is no width to the distribution
	if (iMaxCorr < 3) {
		slope = 0;
		intercept = 0;
		iMaxXcorr = 0;
		iStartXcorr = 0;
		iNextXcorr = 0;
		rSquared = 0;
		return;
	}

	//More aggressive version summing everything below the max
	dCummulative[iMaxCorr - 1] = histogram[iMaxCorr - 1] + histogram[iMaxCorr];
	for (i = iMaxCorr - 2; i >= 0; i--) {
		dCummulative[i] = dCummulative[i + 1] + histogram[i];
	}

	//get middle-ish datapoint as seed. Using count/10.
	for (i = 0; i < iMaxCorr; i++) {
		if (dCummulative[i] < histogramCount / 10) break;
	}
	if (i >= (iMaxCorr - 1)) iNextCorr = iMaxCorr - 2;
	else iNextCorr = i;

	// log10...and stomp all over the original...hard to troubleshoot later
	for (i = iMaxCorr - 1; i >= 0; i--) {
		histogram[i] = (int)dCummulative[i];
		dCummulative[i] = log10(dCummulative[i]);
	}

	iStartCorr = iNextCorr - 1;
	iNextCorr++;

	bool bRight = false; // which direction to add datapoint from
	bestRSQ = 0;
	bestNC = 0;
	bestSlope = 0;
	bestInt = 0;
	rsq = Mx = My = a = b = 0.0;
	while (true) {
		Sx = Sxy = SumX = SumY = 0.0;
		iNumPoints = 0;

		// Calculate means.
		for (i = iStartCorr; i <= iNextCorr; i++) {
			if (histogram[i] > 0) {
				SumY += dCummulative[i];
				SumX += i;
				iNumPoints++;
			}
		}
		if (iNumPoints > 0) {
			Mx = SumX / iNumPoints;
			My = SumY / iNumPoints;
		} else {
			Mx = My = 0.0;
		}

		// Calculate sum of squares.
		SST = 0;
		for (i = iStartCorr; i <= iNextCorr; i++) {
			dx = i - Mx;
			dy = dCummulative[i] - My;
			Sx += dx * dx;
			Sxy += dx * dy;
			SST += dy * dy;
		}
		b = Sxy / Sx;
		a = My - b * Mx;  // y-intercept

		//MH: compute R2
		SSR = 0;
		for (i = iStartCorr; i <= iNextCorr; i++) {
			dy = dCummulative[i] - (b * i + a);
			SSR += (dy * dy);
		}
		rsq = 1 - SSR / SST;

		if (rsq > 0.95 || rsq > bestRSQ) {
			if (rsq > bestRSQ || iNextCorr - iStartCorr + 1 < 8) { //keep better RSQ only if more than 8 datapoints, otherwise keep every RSQ below 8 datapoints
				bestRSQ = rsq;
				bestNC = iNextCorr;
				bestSlope = b;
				bestInt = a;
				bestStart = iStartCorr;
			}
			if (bRight) {
				if (iNextCorr < (iMaxCorr - 1)) iNextCorr++;
				else if (iStartCorr > 0) iStartCorr--;
				else break;
			} else {
				if (iStartCorr > 0) iStartCorr--;
				else if (iNextCorr < (iMaxCorr - 1)) iNextCorr++;
				else break;
			}
			bRight = !bRight;
		} else {
			break;
		}
	}

	slope = bestSlope;
	intercept = bestInt;
	iMaxXcorr = iMaxCorr;
	iStartXcorr = bestStart;
	iNextXcorr = bestNC;
	rSquared = bestRSQ;

}


/// <summary>
/// The number of top PSMs being stored.
/// </summary>
/// <returns></returns>
size_t TopScore::Size() {
	return size;
}
