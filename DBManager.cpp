#include "DBManager.h"

using namespace std;
using namespace db_ns;

DBManager::DBManager() {
	Init();
}

void DBManager::AddPeptide(string& pep, double mass, size_t dbIndex, size_t start, size_t end, bool hasMod) {
	//if (pep.compare("FDKFLQDSEAR") == 0) printf("%s\t%.6lf\t%.6lf\n",pep.c_str(),mass,mass+PROTON);
	char len = (char)(end - start + 1);
	bool newPep = false;
	if (mass > minPepMass && len >= minPepLen) {
		size_t index = 0;
		it = mPeptide.find(pep);
		if (it == mPeptide.end()) {
			index = peptides.size();
			mPeptide.insert(pair<string, size_t>(pep, index));
			//cout << "New Peptide: " << pep << "\t" << pep.size() << "\t" << mass << endl;
			peptides.emplace_back();
			peptides.back().mass = mass;
			newPep = true;
			//if (mass > 800 && mass < 3000 && precursors.size() < SPECTRUMCOUNT) addPrecursor(pep, mass + 18.01056466, (int)mass / 700);
		} else index = it->second;
		peptides[index].instances.emplace_back();
		peptides[index].instances.back().dbIndex = dbIndex;
		peptides[index].instances.back().start = (unsigned short)start;
		peptides[index].instances.back().len = len;


		if (newPep && maxMods>0 && hasMod) {
			string mStr;
			//curMods = 0;
			//memset(varModCount, 0, varMods.size());  //shouldn't be necessary.
			AddPeptideMod(pep, index, mass, 0, dbIndex, start, end, mStr,0,0);
			//if (curMods > topMods) {
			//	topMods = curMods;
			//	cout << pep << " is new max: " << curMods << endl;
			//}
		}
	}
}

void DBManager::AddPeptideMod(string& pep, size_t pepIndex, double mass, double modMass, size_t dbIndex, size_t start, size_t end,string mStr, size_t mCount, size_t startAA) {
	//cout << "Start at " << startAA << endl;
	for (size_t a = startAA;a < pep.size();a++) {

		for (size_t b = 0;b < aaMods[pep[a]].size();b++) {

			//The index of this mod on this amino acid in our mod vector.
			size_t mi = aaMods[pep[a]][b];

			//Skip if we're already at the maximum instances for this mod
			if (varModCount[mi] >= varMods[mi].maxPerPeptide) continue;

			//Check if the new peptide mass is within database tolerances
			double mm = varMods[mi].mass;
			if (mass + mm > minPepMass && mass + mm < maxPepMass) {

				//These are temporary diagnostic counters
				modCount++;
				//curMods++;

				//Mark the position of the mod in the mask
				string tStr = mStr;
				tStr += (char)a;
				tStr += (char)mi;

				//See if this mod mask has been seen before
				size_t modIndex = 0;
				it = mMod.find(tStr);
				if (it == mMod.end()) {
					modIndex = modMask.size();
					mMod.insert(pair<string, size_t>(tStr, modIndex));
					modMask.push_back(tStr);
				} else modIndex = it->second;

				//Add the mod to our peptide.
				size_t c;
				double newMM = modMass + mm;
				for (c = 0; c < peptides[pepIndex].mods.size();c++) {
					if (peptides[pepIndex].mods[c].mass == newMM) break; //do we need to check floating point error?
				}
				if (c == peptides[pepIndex].mods.size()) {
					peptides[pepIndex].mods.emplace_back();
					peptides[pepIndex].mods[c].mass = newMM;
				}
				peptides[pepIndex].mods[c].maskIndex.push_back(modIndex);
				
				
				//if peptide is allowed more mods, look for more.
				varModCount[mi]++;
				if (mCount+1 < maxMods) AddPeptideMod(pep, pepIndex, mass + mm, newMM, dbIndex, start, end, tStr, mCount + 1,a+1);
				varModCount[mi]--;

			}

		}
	
	}

//cout << "End from " << startAA << endl;
}

void DBManager::AddVariableMod(std::string sites, double mass, int maxPerPeptide, std::string description) {
	//TODO: check sites string for validity.
	DBMModDef md;
	md.sites = sites;
	md.mass = mass;
	md.maxPerPeptide = maxPerPeptide;
	md.description = description;
	varMods.push_back(md);
}

void DBManager::BuildModSet() {
	for (size_t a = 0;a < 128;a++) aaMods[a].clear();
	for (size_t a = 0;a < varMods.size();a++) {
		for (size_t b = 0;b < varMods[a].sites.size();b++) {
			aaMods[varMods[a].sites[b]].push_back(a);
		}
	}
}

size_t DBManager::DigestPeptides(std::string site, std::string except, bool cterm, bool semi) {
	//Build variable mod structures
	BuildModSet();
	varModCount = new int[varMods.size()]();

	//Iterate over all proteins
	for (size_t a = 0;a < db.size();a++) {

		string pep;
		double mass = 0;
		int mc = 0;
		int sites = 0; //cut sites
		bool hasMod = false;
		bool done = false;

		//Iterate over all amino acids
		for (size_t b = 0;b < db[a].sequence.size();b++) {

			//Check if this is a valid site to start a peptide. If it follows enzymatic rules, increment
			//the enzymatic site count.
			//TODO: Throw this to a function for readability.
			bool bStart = false;
			if (b == 0 /* || b == 1*/) { //allow starting from second AA, assuming Met was cleaved.
				sites++;
				bStart = true;
			} else {
				if (cterm) {
					if (site.find(db[a].sequence[b - 1]) != string::npos && except.find(db[a].sequence[b]) == string::npos) {
						sites++;
						bStart = true;
					} else if (semi) {
						bStart = true;
					}
				} else {
					if (site.find(db[a].sequence[b]) != string::npos && except.find(db[a].sequence[b - 1]) == string::npos) {
						sites++;
						bStart = true;
					} else if (semi) {
						bStart = true;
					}
				}
			}
			if (bStart) {
				pep += db[a].sequence[b];
				mass += aa[db[a].sequence[b]];
				if (aaMods[db[a].sequence[b]].size() > 0) hasMod = true;
			} else continue;

			//Count the missed cleavages
			mc = 0;
			if (cterm) {
				if (site.find(db[a].sequence[b]) != string::npos && (b + 1 < db[a].sequence.size() && except.find(db[a].sequence[b + 1]) == string::npos)) mc = 1;
			}

			//Walk forward down the sequence from this point.
			for (size_t c = b + 1;c < db[a].sequence.size();c++) {

				//Stop if we reach the maximum mass or length
				if (mass + aa[db[a].sequence[c]] + 18.01056466 > maxPepMass) {
					done = true;
					break;
				}
				if ((c - b + 1) > maxPepLen) {
					done = true;
					break;
				}
				pep += db[a].sequence[c];
				mass += aa[db[a].sequence[c]];

				if (aaMods[db[a].sequence[c]].size() > 0) hasMod = true;

				//Check if we're at an enzyme cut site
				bool bCut = false;
				if (cterm) {
					if (site.find(db[a].sequence[c]) != string::npos && (c + 1 < db[a].sequence.size() && except.find(db[a].sequence[c + 1]) == string::npos)) {
						mc++;
						bCut = true;						//add the peptide
					}
				} else {
					if (c + 1 < db[a].sequence.size() && site.find(db[a].sequence[c + 1]) != string::npos && except.find(db[a].sequence[c] == string::npos)) {
						mc++;
						bCut = true;	//add the peptide
					}
				}
				if (bCut) {
					//add the peptide
					//cout << "ADD1" << endl;
					AddPeptide(pep, mass + 18.01056466, a, b, c,hasMod);
				} else if (semi && sites > 0) { //also add semi-enzymatic if the other end is the cut site.
					//add the peptide
					//cout << "ADD2" << endl;
					AddPeptide(pep, mass + 18.01056466, a, b, c,hasMod);
				}

				//if we've reached the maximum missed cleavages, stop here
				if (mc > maxMC) {
					done = true;
					break;
				}

			}

			//if at end of sequence, see if we can add the peptide
			if (!done) {
				//cout << "ADD3" << endl;
				AddPeptide(pep, mass + 18.01056466, a, b, db[a].sequence.size() - 1,hasMod);
			}

			//reset our peptide and move on to the next one.
			pep.clear();
			mass = 0;
			sites = 0;
			hasMod = false;
			done = false;
		}
	}

	//Sort from low to high mass, so that the peptides are ready for indexing.
	//sort(peptides.begin(), peptides.end(), sortMass);

	cout << modMask.size() << " unique modification combinations." << endl;
	cout << modCount << " modified peptide forms." << endl;
	//for (size_t a = 0;a < modMask.size();a++) {
	//	int count = 0;
	//	cout << a << "\t" << modMask[a].size() << "\t";
	//	for (size_t c = 0;c < modMask[a].size();c++) {
	//		cout << (int)modMask[a][c] << '|';
	//		if (modMask[a][c] != 127) count++;
	//	}
	//	cout << "\t" << count << endl;
	//}

	totalPeptidoforms = modCount + peptides.size();
	delete[] varModCount;
	return 0;
}

//Set to return first three letters only.
string DBManager::GetModDescription(const char& index) {
	return varMods[index].description.substr(0, 3);
}

//Note that bounds are not checked here.
double DBManager::GetModMass(const char& index) {
	return varMods[index].mass;
}

string DBManager::GetPeptideSequence(const size_t& index, int maskIndex) {
	if(maskIndex==-1) return db[peptides[index].instances[0].dbIndex].sequence.substr(peptides[index].instances[0].start, peptides[index].instances[0].len);

	string tmp = db[peptides[index].instances[0].dbIndex].sequence.substr(peptides[index].instances[0].start, peptides[index].instances[0].len);
	char* mods=new char[tmp.size()];
	for (size_t a = 0;a < tmp.size();a++) mods[a] = -1;

	string mask = ModMask(maskIndex);
	for (size_t b = 0;b < mask.size();b += 2) {
		//TODO: process the position for the special cases of n- and c-termini
		mods[mask[b]] = mask[b + 1];
	}

	string tmp2;
	for (size_t a = 0;a < tmp.size();a++) {
		tmp2 += tmp[a];
		if (mods[a] > -1) tmp2 += "[" + GetModDescription(mods[a]) + "]";
	}
	delete[] mods;
	return tmp2;
}

char* DBManager::GetPepSeq(const size_t& index) {
	return &db[peptides[index].instances[0].dbIndex].sequence[peptides[index].instances[0].start];
}

string DBManager::GetProteinName(const size_t& index) {
	return db[index].name;
}

string& DBManager::GetProteinSeq(const size_t& index) {
	return db[index].sequence;
}

void DBManager::Init() {
	//Cometize the masses
	double H, O, C, N, S, Se;
	H = 1.007825035; // hydrogen
	O = 15.99491463;  // oxygen
	C = 12.00000000;   // carbon
	N = 14.0030740;   // nitrogen
	S = 31.9720707;   // sulphur
	Se = 79.9165196;  // selenium

	aa['A'] = C * 3 + H * 5 + N + O;
	aa['C'] = C * 3 + H * 5 + N + O + S + 57.0214611;
	aa['D'] = C * 4 + H * 5 + N + O * 3;
	aa['E'] = C * 5 + H * 7 + N + O * 3;
	aa['F'] = C * 9 + H * 9 + N + O;
	aa['G'] = C * 2 + H * 3 + N + O;
	aa['H'] = C * 6 + H * 7 + N * 3 + O;
	aa['I'] = C * 6 + H * 11 + N + O;
	aa['K'] = C * 6 + H * 12 + N * 2 + O;
	aa['L'] = C * 6 + H * 11 + N + O;
	aa['M'] = C * 5 + H * 9 + N + O + S;
	aa['N'] = C * 4 + H * 6 + N * 2 + O * 2;
	aa['P'] = C * 5 + H * 7 + N + O;
	aa['Q'] = C * 5 + H * 8 + N * 2 + O * 2;
	aa['R'] = C * 6 + H * 12 + N * 4 + O;
	aa['S'] = C * 3 + H * 5 + N + O * 2;
	aa['T'] = C * 4 + H * 7 + N + O * 2;
	aa['U'] = C * 3 + H * 5 + N + O + Se;
	aa['V'] = C * 5 + H * 9 + N + O;
	aa['W'] = C * 11 + H * 10 + N * 2 + O;
	aa['Y'] = C * 9 + H * 9 + N + O * 2;


	////Amino acid masses for the less precise
	//aa['A'] = 71.0371103;
	//aa['C'] = 103.0091803+ 57.0214611;
	//aa['D'] = 115.0269385;
	//aa['E'] = 129.0425877;
	//aa['F'] = 147.0684087;
	//aa['G'] = 57.0214611;
	//aa['H'] = 137.0589059;
	//aa['I'] = 113.0840579;
	//aa['K'] = 128.0949557;
	//aa['L'] = 113.0840579;
	//aa['M'] = 131.0404787;
	//aa['N'] = 114.0429222;
	//aa['P'] = 97.0527595;
	//aa['Q'] = 128.0585714;
	//aa['R'] = 156.1011021;
	//aa['S'] = 87.0320244;
	//aa['T'] = 101.0476736;
	//aa['U'] = 150.9536303;
	//aa['V'] = 99.0684087;
	//aa['W'] = 186.0793065;
	//aa['Y'] = 163.0633228;

	//modMask.push_back("0");
	//modMass.push_back(15.9949);
	//aaMods['M'].push_back(0);
	//modMass.push_back(79.995);
	//aaMods['T'].push_back(1);
	//aaMods['S'].push_back(1);
	//aaMods['Y'].push_back(1);
}

std::string& DBManager::ModMask(const int& index) {
	return modMask[index];
}

DBMPeptide& DBManager::Peptide(const size_t& index) {
	return peptides[index];
}

bool DBManager::ReadFASTA(const char* fn) {
	return db.buildDB(fn);
}

bool DBManager::ReadFASTA(string fn) {
	return ReadFASTA(fn.c_str());
}

void DBManager::SetMaxModsPerPeptide(int i) {
	maxMods = i;
}

size_t DBManager::SizePeptide() {
	return peptides.size();
}

size_t DBManager::SizeProtein() {
	return db.size();
}


bool DBManager::sortMass(const DBMPeptide& a, const DBMPeptide& b) {
	return (a.mass < b.mass);
}