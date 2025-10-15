#include "FastXCorr.h"
#include <iostream>

using namespace std;

FastXCorr::FastXCorr() {
}

FastXCorr::~FastXCorr() {
	params = nullptr;
	Deallocate();
}

void FastXCorr::Allocate() {
	Deallocate();
	pdTempRawData = new double[maxBin + 100]();
	pdTmpFastXcorrData = new double[maxBin + 100]();
	pfFastXcorrData = new float[maxBin + 100]();
	pps.pdCorrelationData = new double[maxBin + 100];
	pps.pdMzData = new double[maxBin + 100];
}

void FastXCorr::Deallocate() {
	if(pdTempRawData) delete[] pdTempRawData;
	if(pdTmpFastXcorrData) delete[] pdTmpFastXcorrData;
	if(pfFastXcorrData) delete[] pfFastXcorrData;
	if(pps.pdCorrelationData) delete[] pps.pdCorrelationData;
	if (pps.pdMzData) delete[] pps.pdMzData;
	pdTempRawData = nullptr;
	pdTmpFastXcorrData = nullptr;
	pfFastXcorrData = nullptr;
	pps.pdCorrelationData = nullptr;
	pps.pdMzData = nullptr;
}

bool FastXCorr::Initialize(ParamsManager* p) {
	params = p;
	double invBinSize = 1 / params->binSize;
	maxBin = (size_t)(invBinSize * MAXMZ + 1 + 0.5);
	Allocate();
	return true;
}

bool FastXCorr::ProcessSpectrum(FISpectrum& spec) {
	double m = spec.precursor[0].mass + PROTON + 50; //M+H to match comet
	XCorr(spec.GetPeaks(),m);
	return true;
}

void FastXCorr::XCorr(vector<FIPeak>& spec, double& max) {
	size_t i;
	int j;
	int iTmp;
	double dTmp;
	double dSum;

	pps.iHighestIon = 0;
	pps.dHighestIntensity = 0;
	BinIons(spec,max);

	// Create data for correlation analysis.
	MakeCorrData(50.0);
	//printf("PPS:\n");
	//for (size_t a = 0;a < maxBin;a++) printf("%d\t%.6lf\n",(int)a, pps.pdCorrelationData[a]);
	//printf("End PPS\n");

	// Make fast xcorr spectrum.
	double* pdCorrelationData = pps.pdCorrelationData;
	dSum = 0.0;
	for (i = 0; i < 75; i++) dSum += pdCorrelationData[i];
	for (i = 75; i < maxBin + 75; i++) {
		if (i < maxBin && pdCorrelationData[i]>0) dSum += pdCorrelationData[i];
		if (i >= 151 && pdCorrelationData[i - 151] > 0) dSum -= pdCorrelationData[i - 151];
		pdTmpFastXcorrData[i - 75] = (dSum - pdCorrelationData[i - 75]) * 0.0066666667;
	}

	double dTmp0 = pdCorrelationData[0] - pdTmpFastXcorrData[0];
	double dTmp1 = pdCorrelationData[1] - pdTmpFastXcorrData[1];
	double dTmp2 = pdCorrelationData[2] - pdTmpFastXcorrData[2];
	pfFastXcorrData[0] = (float)(dTmp0 + dTmp1 * 0.5);
	pfFastXcorrData[1] = (float)(dTmp1 + (dTmp0 + dTmp2) * 0.5);
	for (i = 2;i < maxBin - 1;i++) {
		dTmp0 = dTmp1;
		dTmp1 = dTmp2;
		dTmp2 = pdCorrelationData[i + 1] - pdTmpFastXcorrData[i + 1];
		pfFastXcorrData[i] = (float)(dTmp1 + (dTmp0 + dTmp2) * 0.5);
	}
	pfFastXcorrData[maxBin - 1] = (float)(dTmp2 + dTmp1 * 0.5);

	//MH: Fill sparse matrix
	for (i = 0;i < maxBin;i++) {
		if (pfFastXcorrData[i] > MINPEAK || pfFastXcorrData[i] < -MINPEAK) {
			//printf("%d\t%.6f\n", i, pfFastXcorrData[i]);
			FIPeak pk;
			pk.fIndex = i;
			pk.value = pfFastXcorrData[i];
			if (pps.pdMzData[i] > 0) pk.mz = pps.pdMzData[i]; //restore the original mz values for crosslinked searches
			spec.push_back(pk);
		}
	}

}

void FastXCorr::BinIons(vector<FIPeak>& spec, double& max) {
	int i;
	unsigned int j;
	double dPrecursor;
	double dIon;
	double dIntensity;
	double* pdCorrelationData = pps.pdCorrelationData;
	double* pdMzData = pps.pdMzData;

	memset(pdCorrelationData, 0, maxBin * sizeof(double));
	memset(pdMzData, 0, maxBin * sizeof(double));

	for (size_t i = 0;i < spec.size();i++) {

		size_t iBinIon = spec[i].fIndex;
		dIntensity = spec[i].value;
		pdMzData[iBinIon] = spec[i].mz;
		pdMzData[iBinIon - 1] = spec[i].mz - params->binSize;
		pdMzData[iBinIon + 1] = spec[i].mz + params->binSize;

		if (dIntensity > 0.0) {
			if (iBinIon < maxBin) {

				dIntensity = sqrt(dIntensity);
				if (iBinIon > pps.iHighestIon && spec[i].mz<max) {
					//printf("New high: %.4lf\n",spec[i].mz);
					pps.iHighestIon = iBinIon;
				}

				if ((iBinIon < maxBin) && (dIntensity > pdCorrelationData[iBinIon])) {
					if (dIntensity > pdCorrelationData[iBinIon]) {
						pdCorrelationData[iBinIon] = dIntensity;
					}
					if (pdCorrelationData[iBinIon] > pps.dHighestIntensity) pps.dHighestIntensity = pdCorrelationData[iBinIon];
				}
			}
		}
	}

	//Clear spectrum data that we no longer need
	spec.clear();

}

// pdTempRawData now holds raw data, pdCorrelationData is windowed data.
void FastXCorr::MakeCorrData(double scale) {
	int  i;
	int  ii;
	int  iBin;
	int  iNumWindows = 10;
	int  iWindowSize = (int)((double)(pps.iHighestIon) / iNumWindows)+1;
	double dMaxWindowInten[10];
	double dMaxOverallInten;
	double dTmp1;
	double dTmp2;

	double* pdCorrelationData = pps.pdCorrelationData;
	memset(&dMaxWindowInten, 0, 10 * sizeof(double));

	//printf("HighestIon:%d\n", pps.iHighestIon);
	//printf("iWindowSize:%d\n", iWindowSize);

	dMaxOverallInten = 0.0;
	// Normalize maximum intensity to 100.
	dTmp1 = 1.0;
	if (pps.dHighestIntensity > 0.000001) dTmp1 = 100.0 / pps.dHighestIntensity;

	int x = 0;
	int c = 0;
	for (i = 0; i < maxBin; i++) {
		dTmp2 = pdCorrelationData[i] * dTmp1;
		pdTempRawData[i] = dTmp2;
		pdCorrelationData[i] = 0;
		if (x < iNumWindows) {
			if (dMaxWindowInten[x] < dTmp2) dMaxWindowInten[x] = dTmp2;
			c++;
			if (c == iWindowSize) {
				c = 0;
				x++;
			}
		}
	}

	dMaxOverallInten = 100;

	dTmp2 = 0.05 * dMaxOverallInten;
	for (i = 0; i < iNumWindows; i++) {
		//printf("%d dMaxWindowInten: %.4lf\n", i,dMaxWindowInten[i]);
		if (dMaxWindowInten[i] > 0.0) {
			dTmp1 = scale / dMaxWindowInten[i];

			for (ii = 0; ii < iWindowSize; ii++) {    // Normalize to max inten. in window.      
				iBin = i * iWindowSize + ii;
				if (iBin < maxBin) {
					if (pdTempRawData[iBin] > dTmp2) pdCorrelationData[iBin] = (pdTempRawData[iBin] * dTmp1);
				}
			}
		}
	}

}