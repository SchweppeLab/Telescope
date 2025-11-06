#ifndef _RESULTSEXPORTER_H
#define _RESULTSEXPORTER_H	

#include "DataLoader.h"
#include "FragmentIonIndex.h"
#include "GlobalDefinitions.h"
#include "NeoPepXMLParser.h"
#include "ParamsManager.h"

/// <summary>
/// Exports Telescope search results in PepXML format.
/// </summary>
class ResultsExporter {
public:
	ResultsExporter();
	~ResultsExporter();

	void Initialize(DBManager* d, FragmentIonIndex* f, ParamsManager* p);
	bool Write(const std::string& fn, DataLoader& scans);

private:

	CnpxModificationInfo CreateModificationInfo(const std::string& peptide, const int& modIndex, const int& maskIndex);
	CnpxSearchHit CreateSearchHit(const ScoreStruct& ss);
	CnpxSpectrumQuery CreateSpectrumQuery(const FISpectrum2& spec);

	DBManager* dbm = nullptr;
	FragmentIonIndex* fii = nullptr;
	ParamsManager* params = nullptr;
};

#endif