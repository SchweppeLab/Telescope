#ifndef _FASTXCORR_H
#define _FASTXCORR_H

#include "FISpectrum.h"
#include "GlobalDefinitions.h"
#include "ParamsManager.h"

#include <chrono>

/// <summary>
/// Class that replicates the xcorr scoring transformations in Comet to spectral data.
/// Note that this processing is somewhat expensive, and can probably be approximated
/// using much faster methods.
/// </summary>
class FastXCorr {
public:
	FastXCorr();
	~FastXCorr();

	bool Initialize(ParamsManager* p);
	bool ProcessSpectrum(FISpectrum2& spec);

	size_t ReportTime();
	void ResetTime();

protected:
private:

	//Memory management
	void Allocate();
	void Deallocate();

	//Adapted from Kojak, originally adapted from Comet
	void XCorr(FISpectrum2& spec, double& max);
	void BinIons(FISpectrum2& spec, double& max);
	void MakeCorrData(double scale);

	ParamsManager* params = nullptr;
	size_t maxBin = 0;

	double* pdTempRawData = nullptr;
	double* pdTmpFastXcorrData = nullptr;
	float* pfFastXcorrData = nullptr;
	size_t* sparseIndex = nullptr;

	int iHighestIon = 0;
	int iMax = 0;
	double dHighestIntensity = 0;
	double* pdCorrelationData = nullptr;

	size_t nanoseconds=0;

};

#endif
