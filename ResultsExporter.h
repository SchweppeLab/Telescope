#ifndef _RESULTSEXPORTER_H
#define _RESULTSEXPORTER_H	

#include "DataLoader.h"
#include "FragmentIonIndexXL.h"
#include "GlobalDefinitions.h"
#include "NeoPepXMLParser.h"

class ResultsExporter {
public:

	bool Write(const std::string& fn, DataLoader& scans);

	DBManager* dbm; //temporary location
	FragmentIonIndexXL* fii; //temporary

private:

	CnpxModificationInfo CreateModificationInfo(const std::string& peptide, const int& modIndex, const int& maskIndex);
	CnpxSearchHit CreateSearchHit(const ScoreStruct& ss);
	CnpxSpectrumQuery CreateSpectrumQuery(const FISpectrum& spec);
};

#endif