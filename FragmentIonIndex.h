#ifndef _FRAGMENTIONINDEX_H
#define _FRAGMENTIONINDEX_H

#include "DBManager.h"
#include "FIMask.h"
#include "FISpectrum.h"
#include "GlobalDefinitions.h"

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include <chrono>

typedef struct FIIPeptide {
	double mass = 0;                 //needed for sorting
	unsigned int peptideIndex = 0;   //the index in the DBManager
	int modIndex = -1;               //if this is a modified peptide, this value will be >-1 (an index)
	int maskIndex = -1;							 //ditto
} FIIPeptide;

typedef struct FIIPepRef {
	size_t index=0;
	char linkSite = 0;
} FIIPepRef;

class FragmentIonIndex {
public:
	FragmentIonIndex(); //dangerous default constructor: must manually set DBManager.
	FragmentIonIndex(DBManager* d);
	~FragmentIonIndex();

	void CalculateIndex(unsigned int start, unsigned int stop, unsigned int* arr, FIMask& mask);

	size_t FindPeptideIndex(double pmin);

	//This function takes the peptides from the DBManager, and creates a sorted list with a unique
	//index for each variant of each peptide. This could potentially be made private and entered through
	//the GenerateIndex() function.
	bool GeneratePeptideMap();

	//Creates the frament ion index in its entirety from the PeptideMap.
	bool GenerateIndex();

	void PopulateIndex(unsigned int start, unsigned int stop, unsigned int* arr, FIMask& fim);

	bool ScoreSpectrum(FISpectrum& scan, double* scores);
	//bool ScoreSpectrumB(FISpectrum& scan, int charge);

	void SetBinSize(double bs);
	//void SetDBManager(DBManagerXL* d);
	//void SetMaxMZ(double mz);
	size_t SizePeptide();

	void SortPeptides();

	//The fragment ion indexes. Up to 3 charge states stored for fragment indexes
	//It could be argued that the +3 fragment ions would rarely be used.
	//It might make more sense then to make the number of index arrays dynamic (triple pointers...)
	//and test, as saving the memory could be big in terms of performance.
	//std::vector<std::vector<unsigned int>*>* bins[3];
	unsigned int** bins[3] = { NULL };
	unsigned int* binSz[3] = { NULL };

	FIIPeptide* peptides = NULL;  //all the peptides in the database
	size_t pepArrSz = 0;          //the size of peptides

	double binSize = 0.02;
	double invBinSize = 1 / 0.02;
	//double minMZ = 200;
	//double maxMZ = 2000;
	size_t maxBin = 100000;

protected:
private:

	void DeleteIndex();
	size_t FindIndex(const size_t& index, const unsigned int* arr, const size_t& sz);
	void Indexer(bool preCompute);
	void Resize();

	static bool sortFIIPeptideMass(const FIIPeptide& a, const FIIPeptide& b);

	DBManager* dbm;

};

#endif