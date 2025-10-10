#ifndef _FASTXCORR_H
#define _FASTXCORR_H

#include "FISpectrum.h"
#include "GlobalDefinitions.h"

//TODO: This no longer needs its own structure. GET RID OF IT!!
typedef struct XCorrPreprocessStruct { //adapted from Comet
	int iHighestIon=0;
	double dHighestIntensity=0;
	double* pdCorrelationData = nullptr;
	double* pdMzData = nullptr;
} XCorrPreprocessStruct;

class FastXCorr {
public:
	FastXCorr();
	~FastXCorr();

	bool ProcessSpectrum(FISpectrum& spec);

	//float minPeak = 1e-6;

protected:
private:

	//Memory management
	void Allocate();
	void Deallocate();

	//Adapted from Kojak, originally adapted from Comet
	void XCorr(std::vector<FIPeak>& spec, double& max);
	void BinIons(std::vector<FIPeak>& spec, double& max);
	void MakeCorrData(double scale);

	size_t maxBin = 0;

	double* pdTempRawData = nullptr;
	double* pdTmpFastXcorrData = nullptr;
	float* pfFastXcorrData = nullptr;
  XCorrPreprocessStruct pps;

};

#endif
