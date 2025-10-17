#include "FastXCorr.h"
#include <iostream>

using namespace std;

/// <summary>
/// Default constructor
/// </summary>
FastXCorr::FastXCorr() {
}

/// <summary>
/// Default destructor
/// </summary>
FastXCorr::~FastXCorr() {
	params = nullptr;
	Deallocate();
}

/// <summary>
/// Allocates the memory necessary to process a spectrum
/// </summary>
void FastXCorr::Allocate() {
	Deallocate();
	pdTempRawData = new double[maxBin + 100]();
	pdTmpFastXcorrData = new double[maxBin + 100]();
	pfFastXcorrData = new float[maxBin + 100]();
	pdCorrelationData = new double[maxBin + 100];
	pdMzData = new double[maxBin + 100];
}

/// <summary>
/// Frees allocated memory
/// </summary>
void FastXCorr::Deallocate() {
	if(pdTempRawData) delete[] pdTempRawData;
	if(pdTmpFastXcorrData) delete[] pdTmpFastXcorrData;
	if(pfFastXcorrData) delete[] pfFastXcorrData;
	if(pdCorrelationData) delete[] pdCorrelationData;
	if(pdMzData) delete[] pdMzData;
	pdTempRawData = nullptr;
	pdTmpFastXcorrData = nullptr;
	pfFastXcorrData = nullptr;
	pdCorrelationData = nullptr;
	pdMzData = nullptr;
}

/// <summary>
/// Must be called prior to FastXCorr::ProcessSpectrum(). Allocates the right amount of memory based
/// on the user parameters provided.
/// </summary>
/// <param name="p">pointer to ParamsManager object</param>
/// <returns>true upon success</returns>
bool FastXCorr::Initialize(ParamsManager* p) {
	params = p;
	double invBinSize = 1 / params->binSize;
	maxBin = (size_t)(invBinSize * params->maxMZ + 1 + 0.5);
	Allocate();
	return true;
}

/// <summary>
/// Processes a spectrum.
/// </summary>
/// <param name="spec">FISpectrum object to be processed</param>
/// <returns>true upon success</returns>
bool FastXCorr::ProcessSpectrum(FISpectrum& spec) {
	double m = spec.precursor[0].mass + PROTON + 50; //M+H to match comet
	XCorr(spec.GetPeaks(),m);
	return true;
}

/// <summary>
/// The primary transformation function. Contains calls to other processing functions
/// related to normalizing and binning data.
/// </summary>
/// <param name="spec">vector of FIPeak spectral data points</param>
/// <param name="max">the maximum mass used for normalization</param>
void FastXCorr::XCorr(vector<FIPeak>& spec, double& max) {
	size_t i;
	double dSum;

	iHighestIon = 0;
	dHighestIntensity = 0;
	BinIons(spec,max);

	// Create data for correlation analysis.
	MakeCorrData(50.0);

	// Make fast xcorr spectrum.
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
		if (pfFastXcorrData[i] > params->minPeak || pfFastXcorrData[i] < -params->minPeak) {
			FIPeak pk;
			pk.fIndex = i;
			pk.value = pfFastXcorrData[i];
			if (pdMzData[i] > 0) pk.mz = pdMzData[i]; //restore the original mz values for crosslinked searches
			spec.push_back(pk);
		}
	}

}

/// <summary>
/// Bins the spectral data by m/z value and finds the base peak of the spectrum
/// </summary>
/// <param name="spec">vector of FIPeak spectral data points</param>
/// <param name="max">the maximum mass used for normalization</param>
void FastXCorr::BinIons(vector<FIPeak>& spec, double& max) {
	double dIntensity;

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
				if (iBinIon > iHighestIon && spec[i].mz<max) {
					iHighestIon = (int)iBinIon;
				}

				if ((iBinIon < maxBin) && (dIntensity > pdCorrelationData[iBinIon])) {
					if (dIntensity > pdCorrelationData[iBinIon]) {
						pdCorrelationData[iBinIon] = dIntensity;
					}
					if (pdCorrelationData[iBinIon] > dHighestIntensity) dHighestIntensity = pdCorrelationData[iBinIon];
				}
			}
		}
	}

	//Clear spectrum data that we no longer need
	spec.clear();

}

/// <summary>
/// Normalizes the spectral peaks
/// </summary>
/// <param name="scale">The normalization factor</param>
void FastXCorr::MakeCorrData(double scale) {
	int  i;
	int  ii;
	int  iBin;
	int  iNumWindows = 10;
	int  iWindowSize = (int)((double)(iHighestIon) / iNumWindows)+1;
	double dMaxWindowInten[10];
	double dMaxOverallInten;
	double dTmp1;
	double dTmp2;

	memset(&dMaxWindowInten, 0, 10 * sizeof(double));

	dMaxOverallInten = 0.0;
	dTmp1 = 1.0;
	if (dHighestIntensity > 0.000001) dTmp1 = 100.0 / dHighestIntensity;

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