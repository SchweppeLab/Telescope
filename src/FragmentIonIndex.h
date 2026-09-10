#ifndef _FRAGMENTIONINDEX_H
#define _FRAGMENTIONINDEX_H

#include "DBManager.h"
#include "FIMask.h"
#include "FISpectrum.h"
#include "GlobalDefinitions.h"
#include "ParamsManager.h"

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include <chrono>

/// <summary>
/// Structure used to generate a peptidoform map, which is a sortable array which orders peptides
/// by their mass, but still references the correct unordered position of the peptide itself in
/// the DBManager.
/// </summary>
typedef struct FIIPeptide {
	double mass = 0;                 //needed for sorting
	unsigned int peptideIndex = 0;   //the index in the DBManager
	int modIndex = -1;               //if this is a modified peptide, this value will be >-1 (an index)
	int maskIndex = -1;							 //ditto
} FIIPeptide;

/// <summary>
/// The main class holding the fragment ion index used to score all peptides at a given mz value. Note that
/// the index relies on support classes (DBManager and ParamsManager), that must be supplied by calling the
/// FragmentIonIndex::Initialize() function BEFORE proceeding to use objects of this class.
/// 
/// Many of the functions in this class were designed for multiple threads to achieve the best results.
/// Additionally, index generation is divided into two general steps: index size calculation followed by
/// index generation. Though redundant, the index size calculation steps determine the precise amount of
/// memory required by the index, allowing it to be allocated in one step before building the index. The
/// alternative of allocating the memory while generating the index is slower due to the overhead associated
/// with dynamic memory usage.
/// 
/// With regard to the index itself, it is a simple three-dimensional array of unsigned integers (allowing
/// for ~ 4 billion peptidoforms to be indexed) with the dimensions of [charge state][mz bin][peptidoform reference].
/// Alternatives include:
///   1. Using size_t instead of unsigned int, allowing for more than 18 quintillion peptidoforms. However, not only
///      is that an impractical number of peptidoforms given today's (year 2025) memory technology, but also each value
///      in the index would require double the memory. It is impractical for most of today's applications.
///   2. Shrinking the index significantly by reducing the charge states analyzed. Simply eliminating any
///      3+ fragment ions from the analysis would reduce the memory usage by 1/3 with little to no impact on most
///      peptide identification scores, particularly as 2+ and 3+ precursor ions make the bulk of peptide ion spectra.
/// 
/// Associated with the fragment ion index is a 2-D array that defines the number of indexed peptides at each
/// [charge state][mz bin]. This is for efficient allocation of memory and rapid traversal of the peptidoform indexes 
/// to the subset specific to each spectrum being analyzed. 
/// 
/// One last note worth mentioning. All peptidoforms are stored efficiently in the DBManager, more or less in the
/// order they appear in the FASTA databse. In this class, a peptide map is created that is sorted by mass from low
/// to high, and indexes each peptidoform in the DBManager. The fragment ion index is built off of this peptide map
/// because by being sorted by mass, it means the index is necessarily ordered by mass from low to high. This makes
/// traversal of the index very efficient when scoring peptides against spectral peaks.
/// </summary>
class FragmentIonIndex {
public:
	FragmentIonIndex();
	~FragmentIonIndex();

	void CalculateIndex(unsigned int start, unsigned int stop, unsigned int* arr, FIMask& mask);
	size_t FindPeptideIndex(double pmin);

	//This function takes the peptides from the DBManager, and creates a sorted list with a unique
	//index for each variant of each peptide. This could potentially be made private and entered through
	//the GenerateIndex() function.
	bool GeneratePeptideMap();

	void Initialize(DBManager* d, ParamsManager* p);
	void PopulateIndex(unsigned int start, unsigned int stop, unsigned int* arr, FIMask& fim);
	bool ScoreSpectrum(FISpectrum& scan, double* scores);
	//void SetBinSize(double bs); //TODO: Get rid of this, and use the params object
	size_t SizePeptide();
	void SortPeptides();

	//The fragment ion index. The number of index arrays is dynamic (triple pointers...)
	//as one could argue that reducing the number of fragment ion charge states (say from 3 to 2) can
	//save a lot of memory with minimal impact on scores. The other two dimensions after charge state are
	//mz-bin and peptidoform map index: bins[z][mz][index]
	unsigned int*** bins = { nullptr };

	//The fragment ion index dimensions. This is necessary to identify the number of fragment ions
	//indexed at each mz for each charge state, and defines boundaries and memory consumption.
	//binSz[z][mz].
	unsigned int** binSz = { nullptr };

	FIIPeptide* peptides = nullptr;  //all the peptides in the database
	size_t pepArrSz = 0;             //the size of peptides

	//TODO: fix these if they are not necessary.
	//double binSize = 0.02;
	double invBinSize = 1 / 0.02;
	size_t maxBin = 100000;

protected:
private:

	void DeleteIndex();
	size_t FindIndex(const size_t& index, const unsigned int* arr, const size_t& sz);

	static bool sortFIIPeptideMass(const FIIPeptide& a, const FIIPeptide& b);

	DBManager* dbm = nullptr;
	ParamsManager* params = nullptr;

};

#endif