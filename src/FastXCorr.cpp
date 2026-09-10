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
	sparseIndex = new size_t[maxBin + 100];
}

/// <summary>
/// Frees allocated memory
/// </summary>
void FastXCorr::Deallocate() {
	if(pdTempRawData) delete[] pdTempRawData;
	if(pdTmpFastXcorrData) delete[] pdTmpFastXcorrData;
	if(pfFastXcorrData) delete[] pfFastXcorrData;
	if(pdCorrelationData) delete[] pdCorrelationData;
	if (sparseIndex) delete[] sparseIndex;
	pdTempRawData = nullptr;
	pdTmpFastXcorrData = nullptr;
	pfFastXcorrData = nullptr;
	pdCorrelationData = nullptr;
	sparseIndex = nullptr;
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
	std::chrono::steady_clock::time_point start_time, end_time;
	start_time = chrono::high_resolution_clock::now();
	double m = spec.precursor[0].mass + PROTON + 50; //M+H to match comet
	if(params->ultraxcorr) UltraXCorr(spec, m);
	else XCorr(spec, m);
	end_time = std::chrono::high_resolution_clock::now();
	std::chrono::microseconds us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
	spec.processTime_us = (int)us.count();
	return true;
}

/// <summary>
/// Processes a spectrum using an approximation of the Xcorr computation.
/// </summary>
/// <param name="spec">FISpectrum object to be processed</param>
/// <returns>true upon success</returns>
bool FastXCorr::ProcessSpectrumFast(FISpectrum& spec) {
	double m = spec.precursor[0].mass + PROTON + 50; //M+H to match comet
	UltraXCorr(spec, m);
	return true;
}

/// <summary>
/// The primary transformation function. Contains calls to other processing functions
/// related to normalizing and binning data.
/// </summary>
/// <param name="spec">vector of FIPeak spectral data points</param>
/// <param name="max">the maximum mass used for normalization</param>
void FastXCorr::XCorr(FISpectrum& spec, double& max) {
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
	for (i = 75; i < iMax + 75; i++) {
		if (i < iMax && pdCorrelationData[i]>0) dSum += pdCorrelationData[i];
		if (i >= 151 && pdCorrelationData[i - 151] > 0) dSum -= pdCorrelationData[i - 151];
		pdTmpFastXcorrData[i - 75] = (dSum - pdCorrelationData[i - 75]) * 0.0066666667;
	}

	size_t count = 0;
	double dTmp0 = pdCorrelationData[0] - pdTmpFastXcorrData[0];
	double dTmp1 = pdCorrelationData[1] - pdTmpFastXcorrData[1];
	double dTmp2 = pdCorrelationData[2] - pdTmpFastXcorrData[2];
	pfFastXcorrData[0] = (float)(dTmp0 + dTmp1 * 0.5);
	pfFastXcorrData[1] = (float)(dTmp1 + (dTmp0 + dTmp2) * 0.5);
	if (pfFastXcorrData[0] > params->minPeak || pfFastXcorrData[0] < -params->minPeak) sparseIndex[count++]=0;
	if (pfFastXcorrData[1] > params->minPeak || pfFastXcorrData[1] < -params->minPeak) sparseIndex[count++]=1;
	for (i = 2;i < iMax - 1;i++) {
		dTmp0 = dTmp1;
		dTmp1 = dTmp2;
		dTmp2 = pdCorrelationData[i + 1] - pdTmpFastXcorrData[i + 1];
		pfFastXcorrData[i] = (float)(dTmp1 + (dTmp0 + dTmp2) * 0.5);
		if (pfFastXcorrData[i] > params->minPeak || pfFastXcorrData[i] < -params->minPeak) sparseIndex[count++]=i;
	}
	pfFastXcorrData[iMax - 1] = (float)(dTmp2 + dTmp1 * 0.5);
	if (pfFastXcorrData[iMax - 1] > params->minPeak || pfFastXcorrData[iMax - 1] < -params->minPeak) sparseIndex[count++]= iMax -1;

	//for (size_t a = 0;a < tipTop;a++) cout << "fx\t" << a << "\t" << pfFastXcorrData[a] << endl;

	//cout << spec.scanNumber << "\t" << iHighestIon << "\t" << iMax << "\t" << sparseIndex[count - 1] << "\t" << (iMax > iHighestIon);
	//cout << "\t" << spec[spec.Size() - 1].mz << ", " << spec[spec.Size() - 1].value << ", " << spec[spec.Size() - 1].fIndex;
	//cout << endl;
	
	//MH: Fill sparse matrix
	spec.Allocate(count);
	for (i = 0;i < count;i++) {
		FIPeak pk;
		pk.fIndex = sparseIndex[i];
		pk.value = pfFastXcorrData[sparseIndex[i]];
		spec.AddPeak(pk);
		//cout << pk.fIndex << "\t" << pk.value << "\t" << pdTmpFastXcorrData[sparseIndex[i]] << "\t" << pdCorrelationData[sparseIndex[i]] << endl;
	}
	

}

/// <summary>
/// A fast approximation of the original Xcorr transformation.
/// </summary>
/// <param name="spec">vector of FIPeak spectral data points</param>
/// <param name="max">the maximum mass used for normalization</param>
void FastXCorr::UltraXCorr(FISpectrum& spec, double& max) {
	size_t i;

	iHighestIon = 0;
	dHighestIntensity = 0;

	double dIntensity;
	iMax = (int)spec[spec.Size() - 1].fIndex;
	if (iMax > maxBin)iMax = maxBin;

	memset(pdCorrelationData, 0, (iMax + 75) * sizeof(double));

	for (size_t i = 0;i < spec.Size();i++) {
		if (spec[i].fIndex > maxBin - 1) break;
		pdCorrelationData[spec[i].fIndex] = 0;
	}

	for (size_t i = 0;i < spec.Size();i++) {
		if (spec[i].mz > max) break;
		size_t iBinIon = spec[i].fIndex;
		if (iBinIon > maxBin - 1) break;

		dIntensity = spec[i].value;
		//if (iBinIon > 149583) cout << "WTF: " << iBinIon << "\t" << spec[i].mz << "\t" << max << endl;
		if (dIntensity > 0.0) {
			dIntensity = sqrt(dIntensity);
			iHighestIon = (int)iBinIon;
			if (dIntensity > pdCorrelationData[iBinIon]) {
				pdCorrelationData[iBinIon] = dIntensity;
				if (pdCorrelationData[iBinIon] > dHighestIntensity) dHighestIntensity = pdCorrelationData[iBinIon];
			}

		}
	}
	iMax = iHighestIon + 75;
	//cout << iMax << "\t" << iHighestIon << "\t" << iHighestIon/10 << endl;

	//cout << pdCorrelationData[6904] << endl;

	int  iNumWindows = 10;
	int  iWindowSize = (int)((double)(iHighestIon) / iNumWindows) + 1;
	//cout << iWindowSize << endl;
	double dMaxWindowInten[10];
	double dMaxOverallInten;
	double dTmp1;
	double dTmp2;

	memset(&dMaxWindowInten, 0, 10 * sizeof(double));
	dMaxOverallInten = 0.0;
	dTmp1 = 1.0;
	if (dHighestIntensity > 0.000001) dTmp1 = 100.0 / dHighestIntensity;

	int x = 0;
	size_t lastIon = 0;
	for (size_t i = 0;i < spec.Size();i++) {
		if (spec[i].mz > max) break;
		size_t iBinIon = spec[i].fIndex;
		if (iBinIon > iMax - 1) break;
		if (iBinIon == lastIon) continue;
		else lastIon = iBinIon;
		//cout << iBinIon << "\t" << spec[i].value << endl;

		while (x<iNumWindows && iBinIon >= iWindowSize * (x + 1)) x++;

		dTmp2 = pdCorrelationData[iBinIon] * dTmp1;
		pdTempRawData[iBinIon] = dTmp2;
		pdCorrelationData[iBinIon] = 0;
		if (x < iNumWindows) {
			if (dMaxWindowInten[x] < dTmp2) dMaxWindowInten[x] = dTmp2;
		}
	}

	double scale = 50.0;
	dMaxOverallInten = 100;
	dTmp2 = 0.05 * dMaxOverallInten;
	dTmp1 = scale / dMaxWindowInten[0];
	x = 0;
	for (size_t i = 0;i < spec.Size();i++) {
		if (spec[i].mz > max) break;
		size_t iBinIon = spec[i].fIndex;
		if (iBinIon > iMax - 1) break;

		while (x < iNumWindows-1 && iBinIon >= iWindowSize * (x + 1)) {
			x++;
			dTmp1 = scale / dMaxWindowInten[x];
			//cout << x << "\t" << iBinIon << "\t" << dTmp1 << endl;
		}

		if (pdTempRawData[iBinIon] > dTmp2) pdCorrelationData[iBinIon] = (pdTempRawData[iBinIon] * dTmp1);
	}

	size_t count = 0;
	for (size_t i = 0;i < spec.Size();i++) {
		if (spec[i].mz > max) break;
		size_t iBinIon = spec[i].fIndex;
		if (iBinIon > maxBin - 1) break;

		// Make fast xcorr spectrum.
		size_t start, stop;
		if (iBinIon > 75) start = iBinIon - 76;
		else start = 0;
		stop = iBinIon + 76;
		while (i < spec.Size() - 1 && spec[i + 1].fIndex-76 <= stop) {
			i++;
			stop = spec[i].fIndex+76;
		}
		if (stop > iMax) stop = iMax;
		//cout << start << "\t" << stop  << endl;

		double dSum = 0.0;
		for (size_t a = start; a < start + 75; a++) dSum += pdCorrelationData[a];
		for (size_t a = start + 75; a < stop+76; a++) {
			if (a < stop && pdCorrelationData[a]>0) dSum += pdCorrelationData[a];
			if (a >= start + 151 && pdCorrelationData[a - 151] > 0) dSum -= pdCorrelationData[a - 151];
			pdTmpFastXcorrData[a - 75] = (dSum - pdCorrelationData[a - 75]) * 0.0066666667;
			//if (spec.scanNumber == 38289) cout << a - 75 << "\t" << pdTmpFastXcorrData[a - 75] << "\t" << dSum << endl;
		}

		double dTmp0 = pdCorrelationData[start] - pdTmpFastXcorrData[start];
		double dTmp1 = pdCorrelationData[start + 1] - pdTmpFastXcorrData[start + 1];
		double dTmp2 = pdCorrelationData[start + 2] - pdTmpFastXcorrData[start + 2];
		pfFastXcorrData[start] = (float)(dTmp0 + dTmp1 * 0.5);
		pfFastXcorrData[start + 1] = (float)(dTmp1 + (dTmp0 + dTmp2) * 0.5);
		if (pfFastXcorrData[start] > params->minPeak || pfFastXcorrData[start] < -params->minPeak) sparseIndex[count++] = start;
		if (pfFastXcorrData[start + 1] > params->minPeak || pfFastXcorrData[start + 1] < -params->minPeak) sparseIndex[count++] = start+1;
		for (size_t a = start + 2;a < stop;a++) {
			dTmp0 = dTmp1;
			dTmp1 = dTmp2;
			dTmp2 = pdCorrelationData[a + 1] - pdTmpFastXcorrData[a + 1];
			pfFastXcorrData[a] = (float)(dTmp1 + (dTmp0 + dTmp2) * 0.5);
			if (pfFastXcorrData[a] > params->minPeak || pfFastXcorrData[a] < -params->minPeak) sparseIndex[count++] = a;
		}
		pfFastXcorrData[stop] = (float)(dTmp2 + dTmp1 * 0.5);
		if (pfFastXcorrData[stop] > params->minPeak || pfFastXcorrData[stop] < -params->minPeak) sparseIndex[count++] = stop;

	}

	/*To fix later
	for (size_t i = 0;i < spec.Size();i++) {
		if (spec[i].mz > max) break;
		size_t iBinIon = spec[i].fIndex;
		if (iBinIon > iMax - 1) break;

		//cout << iBinIon << "\t" << pdCorrelationData[iBinIon];
		double dSum = pdCorrelationData[iBinIon];
		if (i > 0) {
			int a = (int)i - 1;
			while (a > -1) {
				if (spec[a].fIndex > (iBinIon - 75)) dSum += pdCorrelationData[spec[a--].fIndex];
				else break;
			}
		}
		if (i < spec.Size()) {
			int a = (int)i + 1;
			while (a < spec.Size() && max>spec[a].mz) {
				if (spec[a].fIndex < (iBinIon + 75)) dSum += pdCorrelationData[spec[a++].fIndex];
				else break;
			}
		}
		//cout << "\t" << dSum;
		if (dSum > pdCorrelationData[iBinIon]) {

		//TODO: Check whether next peak is close enough to influence value?
			pdTmpFastXcorrData[iBinIon] = dSum * 0.0066666667 + (dSum - pdCorrelationData[iBinIon]) * 0.0066666667;
			pfFastXcorrData[iBinIon] = pdCorrelationData[iBinIon] - pdTmpFastXcorrData[iBinIon];
			pfFastXcorrData[iBinIon - 1] = pfFastXcorrData[iBinIon] * 0.5 - dSum * 0.0066666667;
			pfFastXcorrData[iBinIon + 1] = pfFastXcorrData[iBinIon] * 0.5 - dSum * 0.0066666667;
		} else {
			pdTmpFastXcorrData[iBinIon] = dSum * 0.0066666667; //(dSum - pdCorrelationData[iBinIon]) * 0.0066666667;
			pfFastXcorrData[iBinIon] = pdCorrelationData[iBinIon] - pdTmpFastXcorrData[iBinIon];
			pfFastXcorrData[iBinIon - 1] = pfFastXcorrData[iBinIon] * 0.5 - pdTmpFastXcorrData[iBinIon];
			pfFastXcorrData[iBinIon + 1] = pfFastXcorrData[iBinIon] * 0.5 - pdTmpFastXcorrData[iBinIon];
		}
		//cout << "\t" << pdTmpFastXcorrData[iBinIon] << endl;

		//pfFastXcorrData[iBinIon] = pdCorrelationData[iBinIon] - pdTmpFastXcorrData[iBinIon];
		//pfFastXcorrData[iBinIon - 1] = pfFastXcorrData[iBinIon] * 0.5-pdTmpFastXcorrData[iBinIon];
		//pfFastXcorrData[iBinIon + 1] = pfFastXcorrData[iBinIon] * 0.5 - pdTmpFastXcorrData[iBinIon];
		sparseIndex[count++] = iBinIon - 1;
		sparseIndex[count++] = iBinIon;
		sparseIndex[count++] = iBinIon + 1;

	}
	*/

	//MH: Fill sparse matrix
	spec.Allocate(count);
	for (i = 0;i < count;i++) {
		FIPeak pk;
		pk.fIndex = sparseIndex[i];
		pk.value = pfFastXcorrData[sparseIndex[i]];
		spec.AddPeak(pk);
		//if(spec.scanNumber==38289) cout << pk.fIndex << "\t" << pk.value << "\t" << pdTmpFastXcorrData[sparseIndex[i]] << "\t" << pdCorrelationData[sparseIndex[i]] << endl;
	}
	//if (spec.scanNumber == 38289) exit(1);

}

/// <summary>
/// Bins the spectral data by m/z value and finds the base peak of the spectrum
/// </summary>
/// <param name="spec">vector of FIPeak spectral data points</param>
/// <param name="max">the maximum mass used for normalization</param>
void FastXCorr::BinIons(FISpectrum& spec, double& max) {
	double dIntensity;

	iMax = (int)spec[spec.Size() - 1].fIndex;
	if (iMax > maxBin)iMax = maxBin;

	memset(pdCorrelationData, 0, (iMax+75) * sizeof(double));

	for (size_t i = 0;i < spec.Size();i++) {
		//cout << "s\t" << i << spec[i].mz << "\t" << spec[i].value << "\t" << spec[i].fIndex << endl;
		if (spec[i].mz > max) break;

		size_t iBinIon = spec[i].fIndex;
		if (iBinIon > maxBin - 1) break;

		dIntensity = spec[i].value;
		if (dIntensity > 0.0) {

			dIntensity = sqrt(dIntensity);
			iHighestIon = (int)iBinIon;

			if (dIntensity > pdCorrelationData[iBinIon]) {
				pdCorrelationData[iBinIon] = dIntensity;
				if (pdCorrelationData[iBinIon] > dHighestIntensity) dHighestIntensity = pdCorrelationData[iBinIon];
			}
			
		}
	}
	iMax = iHighestIon + 75; //reduce iMax to relevant size

	//for (size_t a = 0;a < iMax + 75;a++) cout << "cd\t" << a << "\t" << pdCorrelationData[a] << endl;

	//Clear spectrum data that we no longer need
	//spec.Clear();

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
	//cout << iWindowSize << endl;
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
	for (i = 0; i < iMax; i++) {
		//if (i == 6904) cout << pdCorrelationData[i] << "\t" << dTmp1 << endl;
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
			//cout << "dT1: " << i << "\t" << dTmp1 << "\t" << iNumWindows << endl;

			for (ii = 0; ii < iWindowSize; ii++) {    // Normalize to max inten. in window.      
				iBin = i * iWindowSize + ii;
				//if (iBin == 6904) cout << pdTempRawData[iBin] << endl;
				if (iBin < iMax) {
					if (pdTempRawData[iBin] > dTmp2) pdCorrelationData[iBin] = (pdTempRawData[iBin] * dTmp1);
				}
				//if (iBin == 6904) cout << pdCorrelationData[iBin] << endl;
			}
		}
	}

	//for (size_t a = 0;a < tipTop;a++) cout << "tr\t" << a << "\t" << pdCorrelationData[a] << endl;

}