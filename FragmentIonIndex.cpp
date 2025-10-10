#include "FragmentIonIndex.h"

using namespace std;

FragmentIonIndex::FragmentIonIndex() {

}

FragmentIonIndex::FragmentIonIndex(DBManager* d) {
	dbm = d;
}

FragmentIonIndex::~FragmentIonIndex() {
	DeleteIndex();
	if (peptides != NULL) delete[] peptides;
	dbm = NULL;
}

//Computes the memory requirement (number of peptides in each bin) for a subset of peptidoforms.
void FragmentIonIndex::CalculateIndex(unsigned int start, unsigned int stop, unsigned int* arr, FIMask& fim) {
	double* mods = new double[dbm->maxPepLen + 2]();
	size_t bin;
	size_t index;
	double mass;
	double revMass;
	double modMass;
	double mz;
	double revMz;

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


		//Ugh...charge states are out here because we need to prioritize the lowest
		//charge states for duplicates...
		for (int z = 0;z < 3;z++) {

			//One pass for all the ions
			mass = 0;
			revMass = peptides[a].mass;
			modMass = 0;

			//TODO: Add mask so that duplicate ions from different charge states don't happen
			//Mask will need to be reset for every peptide...possibly slow...
			// e.g. bool mask[bin] for 0 to maxBin
			//Possible to use a list to mark positions to be cleared?
			// e.g. max fragments = (maxPepLen*2-2)*3, so have list[maxFragments] and pos=sz.

			for (char b = 0;b < len - 1;b++) {
				mass += dbm->aa[seq[b]];
				revMass -= dbm->aa[seq[b]];
				if (hasMods) modMass += mods[b];


				mz = (mass + modMass + (z + 1) * PROTON) / (z + 1);
				//if(mass>800) cout << mz << "\t" << z << "\t" << (size_t)(mz * invBinSize) << endl;
				if (mz > MINMZ && mz < MAXMZ) {
					bin = (size_t)(mz * invBinSize+1);
					if (!fim[bin]) {
						fim[bin] = true;
						fim.Add(bin);
						arr[z * maxBin + bin]++;
					} //else cout << bin << " repeated." << endl;
				}

				revMz = (revMass - modMass + (z + 1) * PROTON) / (z + 1);
				if (revMz > MINMZ && revMz < MAXMZ) {
					bin = (size_t)(revMz * invBinSize+1);
					if (!fim[bin]) {
						fim[bin] = true;
						fim.Add(bin);
						arr[z * maxBin + bin]++;
					} //else cout << bin << " repeated." << endl;
				}

			}
		}

		//Reset Mask
		fim.Reset();
	}

	delete[] mods;
}

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

//Returns the first index at or above the requested index.
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

bool FragmentIonIndex::GenerateIndex() {

	DeleteIndex();
	Indexer(true);  //this computes the fragment ion index size
	Indexer(false); //this populates the fragment ion index
	return true;

}

//This function creates a sorted array of every peptide variant in the search space from low to high mass.
//The fragment ion index references these values, which can be used to obtain the protein, peptide, modification,
//and fragment ion information with a series of helper functions.
bool FragmentIonIndex::GeneratePeptideMap() {

	//Allocate the memory
	if (peptides != NULL) delete[] peptides;
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

	//Sort peptides low to high mass.
	SortPeptides(); //this is the most expensive step, and the only place where multithreading might provide benefit
	return true;
}

//Computes in a single process the entire fragment ion index. If preCompute is true, the memory required for the index
//is calculated and allocated, which is fast. If preCompute is false, the index is populated into the space allocated in the
//last call to Indexer(true). WARNING: never call Indexer(false) to populate the index without calling Indexer(true) first.
//This function should be considered legacy, as there are other methods (CalculateIndex, PopulateIndex) that perform the
//same steps while passing arrays as parameters to allow for multithreading.
//TODO: Abstract this function to generate other ions.
void FragmentIonIndex::Indexer(bool preCompute) {
	double* mods = new double[dbm->maxPepLen + 2]();
	size_t bin;
	double mass;
	double revMass;
	double modMass;
	double mz;
	double revMz;

	if (preCompute) {
		for (int a = 0;a < 3;a++) {
			if (binSz[a] != NULL) delete[] binSz[a];
			binSz[a] = new unsigned int[maxBin]();
		}
	}

	unsigned int sz = (unsigned int)pepArrSz;
	for (unsigned int a = 0;a < sz;a++) {
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

		//One pass for all the ions
		mass = 0;
		revMass = peptides[a].mass;
		modMass = 0;

		for (char b = 0;b < len - 1;b++) {
			mass += dbm->aa[seq[b]];
			revMass -= dbm->aa[seq[b]];
			if (hasMods) modMass += mods[b];

			for (int z = 0;z < 3;z++) {
				mz = (mass + modMass + (z + 1) * PROTON) / (z + 1);
				if (mz > MINMZ && mz < MAXMZ) {
					bin = (size_t)(mz * invBinSize+1);
					if (preCompute) binSz[z][bin]++;
					else bins[z][bin][binSz[z][bin]++] = a;
				}

				revMz = (revMass - modMass + (z + 1) * PROTON) / (z + 1);
				if (revMz > MINMZ && revMz < MAXMZ) {
					bin = (size_t)(revMz * invBinSize+1);
					if (preCompute) binSz[z][bin]++;
					else bins[z][bin][binSz[z][bin]++] = a;
				}

				//if (peptides[a].peptideIndex==1 && z == 0) cout << "B" << b + 1 << "\t" << mz << "\t" << revMz << endl;
			}
		}
	}

	if (preCompute) {
		size_t bytes = 0;
		size_t frags = 0;
		for (int a = 0;a < 3;a++) {
			bins[a] = new unsigned int* [maxBin];

			for (size_t b = 0;b < maxBin;b++) {
				if (binSz[a][b] > 0) {
					frags += binSz[a][b];
					bins[a][b] = new unsigned int[binSz[a][b]];

					bytes += binSz[a][b] * sizeof(unsigned int);
					binSz[a][b] = 0;
				}
			}
		}
		cout << "Total fragment ions: " << frags << endl;
		cout << "Estimated frament index size: " << (double)bytes / 1073741824 << " Gb." << endl;
	}

	delete[] mods;
}

//Note that if arrA is null, then index population is from the beginning of the array, not at the position
//that would otherwise be indicated in arrA
void FragmentIonIndex::PopulateIndex(unsigned int start, unsigned int stop, unsigned int* arr, FIMask& fim) {
	double* mods = new double[dbm->maxPepLen + 2]();
	size_t bin;
	size_t index;
	double mass;
	double revMass;
	double modMass;
	double mz;
	double revMz;

	for (unsigned int a = start;a < stop;a++) {
		//get peptide as a string
		char* seq = dbm->GetPepSeq(peptides[a].peptideIndex); //perhaps a bit faster to grab a pointer to memory instead of copying a string.
		char len = dbm->Peptide(peptides[a].peptideIndex).instances[0].len;
		//cout << dbm->GetPeptideSequence(peptides[a].peptideIndex) << endl;

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

		for (int z = 0;z < 3;z++) {

			//One pass for all the ions
			mass = 0;
			revMass = peptides[a].mass;
			modMass = 0;

			for (char b = 0;b < len - 1;b++) {
				mass += dbm->aa[seq[b]];
				revMass -= dbm->aa[seq[b]];
				if (hasMods) modMass += mods[b];

				mz = (mass + modMass + (z + 1) * PROTON) / (z + 1);
				if (mz > MINMZ && mz < MAXMZ) {
					bin = (size_t)(mz * invBinSize+1);
					if (!fim[bin]) {
						fim[bin] = true;
						fim.Add(bin);
						//cout << bin << " added. " << mz << " " << z << "\t" << a << endl;
						if (arr == NULL) bins[z][bin][binSz[z][bin]++] = a;
						else bins[z][bin][arr[z * maxBin + bin]++] = a;
					} //else cout << bin << " repeated. " << mz << " " << z << "\t" << a << endl;
				}

				revMz = (revMass - modMass + (z + 1) * PROTON) / (z + 1);
				if (revMz > MINMZ && revMz < MAXMZ) {
					bin = (size_t)(revMz * invBinSize+1);
					if (!fim[bin]) {
						fim[bin] = true;
						fim.Add(bin);
						//cout << bin << " added. " << revMz << " " << z << "\t" << a << endl;
						if (arr == NULL) bins[z][bin][binSz[z][bin]++] = a;
						else bins[z][bin][arr[z * maxBin + bin]++] = a;
					} //else cout << bin << " repeated. " << revMz << " " << z << "\t" << a << endl;
				}

			}
		}

		//Reset FIMask
		fim.Reset();
	}

	delete[] mods;
}

void FragmentIonIndex::Resize() {
	maxBin = (size_t)(invBinSize * MAXMZ + 1 + 0.5);
	DeleteIndex();
}

//Think about pulling the precursor out of this and making it a separate class to pass. That way the scores
//array won't be shared in case there is post-processing to be done.
bool FragmentIonIndex::ScoreSpectrum(FISpectrum& scan, double* scores) {
	for (size_t p = 0;p < scan.precursor.size();p++) { //iterate over each precursor associated with this scan
		//cout << scan.scanNumber << " with candidates: " << scan.precursor[p].scoreCount << endl;
		if (scan.precursor[p].scoreCount == 0) continue;
		
		//reset memory
		memset(scores, 0, scan.precursor[p].scoreCount * sizeof(double));
		scan.precursor[p].topIndex = 0;
		scan.precursor[p].topScore = 0;

		size_t pLowIndex = scan.precursor[p].pepOffset;
		size_t pHighIndex = pLowIndex + scan.precursor[p].scoreCount - 1;
		//cout << scan.precursor[p].mass << "\t" << pLowIndex << "-" << pHighIndex << endl;

		int charge = scan.precursor[p].charge - 1;
		if (charge > 3) charge = 3;
		if (charge < 1) charge = 1;
		for (int z = 0;z < charge;z++) {
			for (size_t a = 0;a < scan.Size();a++) {

				size_t fIndex = scan[a].fIndex;
				if (binSz[z][fIndex] == 0) continue;

				//this binary search could be put into a function for readability.
				size_t sz = binSz[z][fIndex];
				size_t lower = 0;
				size_t upper = sz;
				size_t mid = upper / 2;
				size_t best = 0;

				int dif = pLowIndex - bins[z][fIndex][0];
				int tdif;
				while (true) {
					tdif = pLowIndex - bins[z][fIndex][mid];
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
				while (best < sz && bins[z][fIndex][best] < pLowIndex) best++;
				if (best == sz) continue;

				//iterate over all qualified peptide candidates
				while (best < sz) {
					size_t index = bins[z][fIndex][best++];
					if (index > pHighIndex) break; //stop when we run past the last qualified peptide.

					//keep the top score
					scores[index - pLowIndex] += scan[a].value;
					//cout << z<< "\t" << scan[a].mz << "\t" << scan[a].fIndex << "\t" << scan[a].value << "\t" << index - pLowIndex << "\t" << scores[index - pLowIndex] << "\t" << index << endl;
				//	if (scores[index - pLowIndex] > scan.precursor[p].topScore) {
				//		scan.precursor[p].topScore = scores[index - pLowIndex];
				//		scan.precursor[p].topIndex = index - pLowIndex; //should be just index?
				//		cout << "New top score: " << scan.precursor[p].topScore << endl;
				//	}
					//while (best<sz && bins[z][fIndex][best] == index) best++; //skip duplicates.
				}
			}
		}

		//Get the top scores
		for (size_t a = 0;a < scan.precursor[p].scoreCount;a++) {
			double fsc= round(scores[a]*5) / 1000.0;
			if (fsc > 0) scan.precursor[p].ts.CheckScore(fsc, a + pLowIndex);
		}
	}
	
	return true;
}

void FragmentIonIndex::SetBinSize(double bs) {
	binSize = bs;
	invBinSize = 1 / bs;
	Resize();
}

size_t FragmentIonIndex::SizePeptide() {
	return pepArrSz;
}

void FragmentIonIndex::SortPeptides() {
	sort(peptides, peptides + pepArrSz, sortFIIPeptideMass);
}

bool FragmentIonIndex::sortFIIPeptideMass(const FIIPeptide& a, const FIIPeptide& b) {
	return (a.mass < b.mass);
}

