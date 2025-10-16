#include "FragmentIonIndex.h"

using namespace std;

/// <summary>
/// Default constructor.
/// </summary>
FragmentIonIndex::FragmentIonIndex() {
}

/// <summary>
/// Default destructor.
/// </summary>
FragmentIonIndex::~FragmentIonIndex() {
	DeleteIndex();
	if (peptides) delete[] peptides;
	dbm = nullptr;
	params = nullptr;
}

/// <summary>
/// Computes the memory requirement (number of peptide indexes in each bin of the fragment ion index) for a subset of peptidoforms.
/// A subset of peptidoforms is allowed so that the task of computing the index sizes can be divided over multiple threads
/// to lower computation times.
/// </summary>
/// <param name="start">The index of the first peptidoform</param>
/// <param name="stop">The index of the last peptidoform</param>
/// <param name="arr">An array to hold the peptidoform counts at each mz value</param>
/// <param name="fim">A mask to track when mz values are observed more than once for a peptidoform's fragment ions</param>
void FragmentIonIndex::CalculateIndex(unsigned int start, unsigned int stop, unsigned int* arr, FIMask& fim) {
	double* mods = new double[dbm->maxPepLen + 2]();
	size_t bin;
	size_t index;
	double mass;
	double revMass;
	double modMass;
	double mz;
	double revMz;

	//Each peptidoform has a modification mask that indicates which positions have which modification masses.
	//This must be interpreted to make a modification mass array that aligns with each amino acid position in
	//the peptide sequence.
	for (unsigned int a = start;a < stop;a++) {
		//get peptide as a string
		char* seq = dbm->GetPepSeq(peptides[a].peptideIndex); //perhaps a bit faster to grab a pointer to memory instead of copying a string.
		char len = dbm->Peptide(peptides[a].peptideIndex).instances[0].len;

		//if there are modifications, set up an array that has the additional masses at each position
		bool hasMods = false;
		if (peptides[a].modIndex > -1) {
			hasMods = true;
			memset(mods, 0, sizeof(double) * (len + 2));

			string mask = dbm->ModMask(peptides[a].maskIndex);
			for (size_t b = 0;b < mask.size();b += 2) {
				//TODO: process the position for the special cases of n- and c-termini
				mods[mask[b]] = dbm->GetModMass(mask[b + 1]);
			}
		}


		//Iterate over each charge state to compute fragment ion mz values at different charge states.
		//Start with the lowest charge state, such that when duplicates happen, the larger charge states
		//are skipped. That way, the most complete set of fragment ion mz values is always at the lowest
		//possible charge state.
		for (int z = 0;z < 3;z++) {

			//One pass for all the ions
			mass = 0;
			revMass = peptides[a].mass;
			modMass = 0;

			//Iterate over each amino acid in the peptide sequence.
			for (char b = 0;b < len - 1;b++) {
				mass += dbm->aa[seq[b]];
				revMass -= dbm->aa[seq[b]];
				if (hasMods) modMass += mods[b];

				//compute the mz value, check if it is within our mz analysis range,
				//then check the mask to see if it has been observed before. If not,
				//add to the peptide index count at this mz position.
				mz = (mass + modMass + (z + 1) * PROTON) / (z + 1);
				if (mz > params->minMZ && mz < params->maxMZ) {
					bin = (size_t)(mz * invBinSize+1);
					if (!fim[bin]) {
						fim[bin] = true;
						fim.Add(bin);
						arr[z * maxBin + bin]++;
					}
				}

				//repeat the process for the complementary ion series.
				//Above, we computed b-ions, here we compute y-ions.
				//This can be easily adapted for a-, c-, x-, and z.- ions.
				revMz = (revMass - modMass + (z + 1) * PROTON) / (z + 1);
				if (revMz > params->minMZ && revMz < params->maxMZ) {
					bin = (size_t)(revMz * invBinSize+1);
					if (!fim[bin]) {
						fim[bin] = true;
						fim.Add(bin);
						arr[z * maxBin + bin]++;
					}
				}

			}
		}

		//Reset Mask
		fim.Reset();
	}

	//Free the temporary memory
	delete[] mods;
}

/// <summary>
/// Frees all index memory.
/// </summary>
void FragmentIonIndex::DeleteIndex() {
	for (int a = 0;a < 3;a++) {
		if (binSz[a] != NULL) {
			for (size_t b = 0;b < maxBin;b++) {
				if (binSz[a][b] > 0) delete[] bins[a][b];
			}
			delete[] bins[a];
			delete[] binSz[a];
		}
	}
}

/// <summary>
/// Uses binary search to return the first position at or above the requested index.
/// </summary>
/// <param name="index">The desired index value</param>
/// <param name="arr">An array of indexes</param>
/// <param name="sz">The size of the array of indexes</param>
/// <returns>The array position at or above the requested index</returns>
size_t FragmentIonIndex::FindIndex(const size_t& index, const unsigned int* arr, const size_t& sz) {
	size_t lower = 0;
	size_t upper = sz;
	size_t mid = upper / 2;
	size_t best = 0;

	int dif = index - arr[0];
	int tdif;
	while (true) {
		tdif = index - arr[mid];
		if (tdif > 0 && tdif < dif) {
			dif = tdif;
			best = mid;
		}
		if (lower >= upper) break;
		if (tdif < 0) {
			if (mid == 0) break;
			upper = mid - 1;
		} else {
			if (mid == sz) break;
			lower = mid + 1;
		}
		mid = (upper + lower) / 2;
		if (mid == sz) break;
	}

	//because we should normally return at the index BEFORE our intended stop.
	while (best < sz && arr[best] < index) best++;
	return best;
}

/// <summary>
/// Uses binary search to find the first peptide array position below the requested mass
/// </summary>
/// <param name="pmin">The mass to find</param>
/// <returns>The first peptide array position below the requested mass</returns>
size_t FragmentIonIndex::FindPeptideIndex(double pmin) {
	size_t sz = pepArrSz;

	size_t lower = 0;
	size_t upper = sz;
	size_t mid = upper / 2;
	size_t best = 0;
	double dif = pmin - peptides[0].mass;
	double tdif;
	while (true) {
		tdif = pmin - peptides[mid].mass;
		if (tdif > 0 && tdif < dif) {
			dif = tdif;
			best = mid;
		}
		if (lower >= upper) break;
		if (tdif < 0) {
			if (mid == 0) break;
			upper = mid - 1;
		} else {
			if (mid == sz) break;
			lower = mid + 1;
		}
		mid = (upper + lower) / 2;
		if (mid == sz) break;
	}
	return best;
}

/// <summary>
/// Creates a sorted array of every peptide variant in the search space from low to high mass.
/// The fragment ion index references these values, which can be used to obtain the protein, peptide, modification,
/// and fragment ion information with a series of helper functions.
/// 
/// Note that this function must be called by the user prior to generating the fragment ion index. Thus, it would be
/// better to wrap to instead have the FragmentIonIndex::GenerateIndex() function call it upon index generation.
/// However, it was left separate for profiling purposes, so that precise timings of each function can be more
/// easily generated.
/// </summary>
/// <returns></returns>
bool FragmentIonIndex::GeneratePeptideMap() {

	//Allocate the memory
	if (peptides) delete[] peptides;
	pepArrSz = dbm->totalPeptidoforms;
	peptides = new FIIPeptide[pepArrSz];

	//Iterate the peptides to generate complete peptide map.
	size_t index = 0;
	for (size_t a = 0;a < dbm->SizePeptide();a++) {
		peptides[index].mass = dbm->Peptide(a).mass;
		peptides[index++].peptideIndex = (unsigned int)a;

		for (size_t b = 0;b < dbm->Peptide(a).mods.size();b++) {
			for (size_t c = 0;c < dbm->Peptide(a).mods[b].maskIndex.size();c++) {
				peptides[index].mass = dbm->Peptide(a).mass + dbm->Peptide(a).mods[b].mass;
				peptides[index].peptideIndex = (unsigned int)a;
				peptides[index].modIndex = (int)b;
				peptides[index++].maskIndex = (int)dbm->Peptide(a).mods[b].maskIndex[c];
			}
		}
	}

	// Sort peptides low to high mass. This is the most expensive step, and the only place where 
	// multithreading might provide benefit. However, it isn't really that slow relative to generating the index itself.
	SortPeptides();
	return true;
}

/// <summary>
/// Connects the fragment ion index to support objects.
/// </summary>
/// <param name="d">Pointer to DBManager object</param>
/// <param name="p">Pointer to ParamsManager object</param>
void FragmentIonIndex::Initialize(DBManager* d, ParamsManager* p) {
	dbm = d;
	params = p;
	invBinSize = 1 / params->binSize;
	maxBin = (size_t)(invBinSize * params->maxMZ + 1 + 0.5);
}

//Note that if arrA is null, then index population is from the beginning of the array, not at the position
//that would otherwise be indicated in arrA

/// <summary>
/// Populates the fragment ion index for a subset of peptides into a portion of the fragment ion index array
/// that has been previously determined. This is so that the fragment ion index can be populated concurrently
/// by multiple threads into a pool of memory that was previously allocated. This is managed by the FIManager.
/// 
/// Note that if arrA is null, then index population is from the beginning of the array, not at the position
/// that would otherwise be indicated in arrA
/// </summary>
/// <param name="start">The index of the first peptide to fragment</param>
/// <param name="stop">The index of the last peptide</param>
/// <param name="arr">The array to store the fragment ion index</param>
/// <param name="fim">A masking object to prevent double-counting fragment ions that appear twice in a peptide</param>
void FragmentIonIndex::PopulateIndex(unsigned int start, unsigned int stop, unsigned int* arr, FIMask& fim) {
	double* mods = new double[dbm->maxPepLen + 2]();
	size_t bin;
	size_t index;
	double mass;
	double revMass;
	double modMass;
	double mz;
	double revMz;

	//Iterate over the subset of peptides
	for (unsigned int a = start;a < stop;a++) {

		//get peptide as a string
		char* seq = dbm->GetPepSeq(peptides[a].peptideIndex);
		char len = dbm->Peptide(peptides[a].peptideIndex).instances[0].len;

		//if there are modifications, set up an array that has the additional masses at each position
		bool hasMods = false;
		if (peptides[a].modIndex > -1) {
			hasMods = true;
			memset(mods, 0, sizeof(double) * (len + 2));

			string mask = dbm->ModMask(peptides[a].maskIndex);
			for (size_t b = 0;b < mask.size();b += 2) {
				//TODO: process the position for the special cases of n- and c-termini
				mods[mask[b]] = dbm->GetModMass(mask[b + 1]);
			}
		}

		//Iterate over each charge state. By processing lowest charge states first,
		//duplicate fragment ion mz values will be skipped in the higher charge states.
		for (int z = 0;z < 3;z++) {

			//One pass for all the ions
			mass = 0;
			revMass = peptides[a].mass;
			modMass = 0;

			//Iterate over the amino acids
			for (char b = 0;b < len - 1;b++) {
				mass += dbm->aa[seq[b]];
				revMass -= dbm->aa[seq[b]];
				if (hasMods) modMass += mods[b];

				mz = (mass + modMass + (z + 1) * PROTON) / (z + 1);
				if (mz > params->minMZ && mz < params->maxMZ) {
					bin = (size_t)(mz * invBinSize+1);
					if (!fim[bin]) {
						fim[bin] = true;
						fim.Add(bin);
						if (arr == NULL) bins[z][bin][binSz[z][bin]++] = a;
						else bins[z][bin][arr[z * maxBin + bin]++] = a;
					}
				}

				//Repeat for the opposite ion series.
				revMz = (revMass - modMass + (z + 1) * PROTON) / (z + 1);
				if (revMz > params->minMZ && revMz < params->maxMZ) {
					bin = (size_t)(revMz * invBinSize+1);
					if (!fim[bin]) {
						fim[bin] = true;
						fim.Add(bin);
						if (arr == NULL) bins[z][bin][binSz[z][bin]++] = a;
						else bins[z][bin][arr[z * maxBin + bin]++] = a;
					}
				}

			}
		}

		//Reset FIMask
		fim.Reset();
	}

	//Free memory
	delete[] mods;
}

/// <summary>
/// Scores a spectrum against all relevant peptides in the fragment ion index.
/// 
/// Developer's note to self: Think about pulling the precursor out of this and making it a separate 
/// class to pass. That way the scores array won't be shared in case there is post-processing to be done. 
/// </summary>
/// <param name="scan">The FISpectrum object data to be scored</param>
/// <param name="scores">A block of memory sufficient to hold all the PSM scores</param>
/// <returns>true upon success</returns>
bool FragmentIonIndex::ScoreSpectrum(FISpectrum& scan, double* scores) {

	//Iterate over each precursor associated with this scan. Note that having multiple precursors
	//is useful for either:
	//  1. Testing multiple precursor masses where the precursor mass is ambiguous.
	//  2. Identifying multiple PSMs from the same spectrum, provided they have unique precursor masses.
	for (size_t p = 0;p < scan.precursor.size();p++) { 
		if (scan.precursor[p].scoreCount == 0) continue;
		
		//Zero out memory from score array. It is only necessary to reset the amount of memory
		//this precursor will use.
		memset(scores, 0, scan.precursor[p].scoreCount * sizeof(double));
		scan.precursor[p].topIndex = 0;
		scan.precursor[p].topScore = 0;

		//Define the peptide index boundaries that are valid for this precursor mass.
		size_t pLowIndex = scan.precursor[p].pepOffset;
		size_t pHighIndex = pLowIndex + scan.precursor[p].scoreCount - 1;

		//Find the maximum fragment ion charge state for the precursor ion.
		int charge = scan.precursor[p].charge - 1;
		if (charge > 3) charge = 3;
		if (charge < 1) charge = 1;

		//Iterate over all possible charge states
		for (int z = 0;z < charge;z++) {

			//Iterate over all peaks in the spectrum
			for (size_t a = 0;a < scan.Size();a++) {

				//Each peak matches to an mz position in the fragment ion index. If that mz position has no
				//matching peptides, move right away to the next peak.
				size_t fIndex = scan[a].fIndex;
				if (binSz[z][fIndex] == 0) continue;

				//Binary search to the first position at first candidate peptide (or the next available candidate
				//peptide should the first not match to the peak).
				size_t sz = binSz[z][fIndex];
				size_t best = FindIndex(pLowIndex, bins[z][fIndex], binSz[z][fIndex]);

				//Iterate over all qualified peptide candidates
				while (best < sz) {

					//Get the peptide position we're currently scoring
					size_t index = bins[z][fIndex][best++];
					
					//Stop when we run past the last qualified peptide.
					if (index > pHighIndex) break; 

					//Add the score, noting that we need to offset the peptide position to the dimensions of the score array
					scores[index - pLowIndex] += scan[a].value;
				}
			}
		}

		//Iterate over all scores, ordering them from highest to lowest
		for (size_t a = 0;a < scan.precursor[p].scoreCount;a++) {

			//This conversion here replicates the xcorr score of Comet
			double fsc= round(scores[a]*5) / 1000.0;

			//Only keep scores above zero. Let the TopScore object do the sorting
			if (fsc > 0) scan.precursor[p].ts.CheckScore(fsc, a + pLowIndex);
		}
	}
	
	return true;
}

/// <summary>
/// Returns the number of mapped peptidoforms.
/// </summary>
/// <returns></returns>
size_t FragmentIonIndex::SizePeptide() {
	return pepArrSz;
}

/// <summary>
/// Sorts the peptidoform map by mass.
/// </summary>
void FragmentIonIndex::SortPeptides() {
	sort(peptides, peptides + pepArrSz, sortFIIPeptideMass);
}

/// <summary>
/// The mass comparison function used by FragmentIonIndex::SortPeptides()
/// </summary>
/// <param name="a">FIIPeptide peptidoform #1</param>
/// <param name="b">FIIPeptide peptidoform #2</param>
/// <returns></returns>
bool FragmentIonIndex::sortFIIPeptideMass(const FIIPeptide& a, const FIIPeptide& b) {
	return (a.mass < b.mass);
}

