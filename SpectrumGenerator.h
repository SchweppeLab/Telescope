#ifndef _SPECTRUMGENERATOR_H
#define _SPECTRUMGENERATOR_H

#include "DBManager.h"
#include "FISpectrum.h"
#include "GlobalDefinitions.h"

class SpectrumGenerator {
public:
	SpectrumGenerator(DBManager* d);
	~SpectrumGenerator();

	FISpectrum GenerateSpectrum(std::string peptide, int z);
	FISpectrum GenerateSpectrum(std::string alpha, size_t siteA, std::string beta, size_t siteB, int z, double xlMass=0);

	bool echo = false;
	double minMZ = 200;
	double maxMZ = 2000;

protected:
private:

	void CalcXLIons(FISpectrum& spec, std::string peptide, int charge, size_t site, double mass);
	double CalcPeptideMass(std::string peptide);
	FISpectrum MakeSpectrum(std::string peptide, int charge);
	FISpectrum MakeXLSpectrum(std::string alpha, size_t siteA, std::string beta, size_t siteB, int charge, double xlMass);

	DBManager* dbm;
};

#endif
