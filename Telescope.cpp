#include "DataLoader.h"
#include "DBaser.h"
#include "DBManager.h"
#include "FIManager.h"
#include "FISpectrum.h"
#include "ResultsExporter.h"
#include "SpectrumGenerator.h"
#include "TelescopeManager.h"

#include "GlobalDefinitions.h"

#include <algorithm>
#include <chrono>

using namespace dbaser;
using namespace std;

typedef struct sTop {
	size_t index=0;
	float score=0;
	double eValue = 9999;
} sTop;

typedef struct sPrecursor {
	double mass = 0;
	int charge = 0;
	string peptide;
} sPrecursor;

void eValue(sTop* arr, size_t count);
void linearRegression2(double& slope, double& intercept, int& iMaxXcorr, int& iStartXcorr, int& iNextXcorr, double& rSquared);
bool sortScores(const sTop& a, const sTop& b);

vector<sPrecursor> precursors;  //list of all candidate precursor ions; allows for multiple precursors for a single spectrum

int histogram[HISTOSZ];
int histogramCount;
double dCummulative[HISTOSZ];

int main(int argc, char* argv[]) {

	string params = argv[1];
	TelescopeManager tm;
	if (!tm.SetParams(params)) {
		cout << "Failed to set params: " << params << endl;
		return 1;
	}
	cout << tm.Launch(true) << endl;
	return 0;

}

void addPrecursor(const string& pep, double mass, int charge) {
	precursors.emplace_back();
	precursors.back().charge = charge;
	precursors.back().mass = mass;
	precursors.back().peptide = pep;
}


void eValue(sTop* arr, size_t count) {
	int iMaxCorr;
	int iStartCorr;
	int iNextCorr;
	double dSlope;
	double dIntercept;
	double dRSquare;
	bool bSkipXL = false;
	bool bSingletFail = false;
	double topScore = 0;

	memset(histogram, 0, sizeof(int) * HISTOSZ);
	for (size_t a = 0;a < count;a++) {
		int index = (int)(arr[a].score * 10+0.5);
		if (index < 0) index = 0;
		else if (index >= HISTOSZ) index = HISTOSZ - 1;
		histogram[index]++;
	}
	histogramCount = count;
	linearRegression2(dSlope, dIntercept, iMaxCorr, iStartCorr, iNextCorr, dRSquare);

	//diagnostics - probably temporary
	//tmpIntercept = (float)dIntercept;  // b
	//tmpSlope = (float)dSlope;  // m
	//tmpIStartCorr = (float)iStartCorr;
	//tmpINextCorr = (float)iNextCorr;
	//tmpIMaxCorr = (short)iMaxCorr;
	//tmpRSquare = dRSquare;
	dSlope *= 10.0;
	if (dSlope >= 0.0) return;

	for (size_t a = 0;a < count;a++) {
		if (arr[a].score <= 0) break;
		arr[a].eValue = pow(10.0, dSlope * arr[a].score + dIntercept);
		if (arr[a].eValue > 9999) {
			arr[a].eValue = 9999;
			break;
		}
	}

}

void linearRegression2(double& slope, double& intercept, int& iMaxXcorr, int& iStartXcorr, int& iNextXcorr, double& rSquared) {
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

	//double dCummulative[HISTOSZ];  // Cummulative frequency at each xcorr value.

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
	dCummulative[iMaxCorr - 1] = histogram[iMaxCorr - 1];
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

bool sortScores(const sTop& a, const sTop& b) {
	return (a.score > b.score);
}