#include "DBManager.h"

using namespace std;
using namespace db_ns;

/// <summary>
/// Adds a peptide to the list if it hasn't been observed in any protein. If it has, update the list
/// to indicate multiple instances of the peptide.
/// </summary>
/// <param name="pep"></param>
/// <param name="mass"></param>
/// <param name="dbIndex"></param>
/// <param name="start"></param>
/// <param name="end"></param>
/// <param name="hasMod"></param>
void DBManager::AddPeptide(string& pep, double mass, size_t dbIndex, size_t start, size_t end, bool hasMod) {
	char len = (char)(end - start + 1);
	bool newPep = false;
	if (mass > minPepMass && len >= minPepLen) {
		size_t index = 0;
		it = mPeptide.find(pep);
		if (it == mPeptide.end()) {
			index = peptides.size();
			mPeptide.insert(pair<string, size_t>(pep, index));
			peptides.emplace_back();
			peptides.back().mass = mass;
			newPep = true;
		} else index = it->second;
		peptides[index].instances.emplace_back();
		peptides[index].instances.back().dbIndex = (unsigned int)dbIndex;
		peptides[index].instances.back().start = (unsigned short)start;
		peptides[index].instances.back().len = len;

		//if peptide can be modified, recursively do so for all valid combinations here.
		if (newPep && maxMods>0 && hasMod) {
			//Diagnostics
			//cout << "New pep: " <<  GetPeptideSequence(index) << endl;

			string mStr;
			AddPeptideMod(pep, index, mass, 0, dbIndex, start, end, mStr,0,-1);
		}
	}
}

/// <summary>
/// Recursive function to expand all valid combinations of variable modifications on a peptide sequence.
/// </summary>
/// <param name="pep"></param>
/// <param name="pepIndex"></param>
/// <param name="mass"></param>
/// <param name="modMass"></param>
/// <param name="dbIndex"></param>
/// <param name="start"></param>
/// <param name="end"></param>
/// <param name="mStr"></param>
/// <param name="mCount"></param>
/// <param name="startAA"></param>
void DBManager::AddPeptideMod(string& pep, size_t pepIndex, double mass, double modMass, size_t dbIndex, size_t start, size_t end,string mStr, size_t mCount, int startAA) {

	//n-terminal modification
	if (startAA == -1) {
		for (size_t b = 0;b < aaMods['n'].size();b++) {
			if (ProcessVarMod('n', b, (char)pep.size(), mass, modMass, pepIndex, mStr)) {
				size_t mi = aaMods['n'][b];
				double mm = varMods[mi].mass;
				string tStr = mStr;
				tStr += (char)pep.size();
				tStr += (char)mi;
				varModCount[mi]++;
				if (mCount + 1 < maxMods) AddPeptideMod(pep, pepIndex, mass + mm, modMass + mm, dbIndex, start, end, tStr, mCount + 1, 0);
				varModCount[mi]--;
			}
		}
		startAA++;
	}
	//c-terminal peptide modification
	//} else if (startAA == pep.size()) {
	//	for (size_t b = 0;b < aaMods['c'].size();b++) {
	//		ProcessVarMod('c', b, (char)pep.size() + 1, mass, modMass, pepIndex, mStr);
	//	}
	//	return;
	//}

	//TODO: if start==0 or start==1, possible N-terminal protein modification

	for (int a = startAA;a < pep.size();a++) {
		for (size_t b = 0;b < aaMods[pep[a]].size();b++) {

			if (ProcessVarMod(pep[a], b, (char)a, mass, modMass, pepIndex, mStr)) {
				size_t mi= aaMods[pep[a]][b];
				double mm = varMods[mi].mass;
				string tStr = mStr;
				tStr += (char)a;
				tStr += (char)mi;
				varModCount[mi]++;
				if (mCount + 1 < maxMods) AddPeptideMod(pep, pepIndex, mass + mm, modMass+mm, dbIndex, start, end, tStr, mCount + 1, a + 1);
				varModCount[mi]--;
			}

		}
	}

	//c-terminal peptide modification
	for (size_t b = 0;b < aaMods['c'].size();b++) {
		ProcessVarMod('c', b, (char)pep.size() + 1, mass, modMass, pepIndex, mStr);
	}


	//TODO: if end==protein length, possible C-terminal protein modification


	//for (size_t a = startAA;a < pep.size();a++) {
	//	for (size_t b = 0;b < aaMods[pep[a]].size();b++) {

	//		//The index of this mod on this amino acid in our mod vector.
	//		size_t mi = aaMods[pep[a]][b];

	//		//Skip if we're already at the maximum instances for this mod
	//		if (varModCount[mi] >= varMods[mi].maxPerPeptide) continue;

	//		//Check if the new peptide mass is within database tolerances
	//		double mm = varMods[mi].mass;
	//		if (mass + mm > minPepMass && mass + mm < maxPepMass) {

	//			//Diagnostic counter
	//			modCount++;

	//			//Mark the position of the mod in the mask
	//			string tStr = mStr;
	//			tStr += (char)a;
	//			tStr += (char)mi;

	//			//See if this mod mask has been seen before
	//			size_t modIndex = 0;
	//			it = mMod.find(tStr);
	//			if (it == mMod.end()) {
	//				modIndex = modMask.size();
	//				mMod.insert(pair<string, size_t>(tStr, modIndex));
	//				modMask.push_back(tStr);
	//			} else modIndex = it->second;

	//			//Add the mod to our peptide.
	//			size_t c;
	//			double newMM = modMass + mm;
	//			for (c = 0; c < peptides[pepIndex].mods.size();c++) {
	//				if (peptides[pepIndex].mods[c].mass == newMM) break; //do we need to check floating point error?
	//			}
	//			if (c == peptides[pepIndex].mods.size()) {
	//				peptides[pepIndex].mods.emplace_back();
	//				peptides[pepIndex].mods[c].mass = newMM;
	//			}
	//			peptides[pepIndex].mods[c].maskIndex.push_back(modIndex);
	//			
	//			
	//			//if peptide is allowed more mods, look for more.
	//			varModCount[mi]++;
	//			if (mCount+1 < maxMods) AddPeptideMod(pep, pepIndex, mass + mm, newMM, dbIndex, start, end, tStr, mCount + 1,a+1);
	//			varModCount[mi]--;

	//		}

	//	}
	//}

}

/// <summary>
/// Adds a static modification to the database. Note that if an amino acid already has a static 
/// modification, it will be overwritten with the most recent call to this function.
/// </summary>
/// <param name="sites"></param>
/// <param name="mass"></param>
/// <param name="description"></param>
void DBManager::AddStaticMod(string sites, double mass, string description) {
	for (size_t a = 0;a < sites.size();a++) {
		staticMods[sites[a]].mass = mass;
		staticMods[sites[a]].description = description;
	}
}

/// <summary>
/// Adds a variable modification to the database.
/// </summary>
/// <param name="sites"></param>
/// <param name="mass"></param>
/// <param name="maxPerPeptide"></param>
/// <param name="description"></param>
void DBManager::AddVariableMod(string sites, double mass, int maxPerPeptide, string description) {
	//TODO: check sites string for validity.
	DBMModDef md;
	md.sites = sites;
	md.mass = mass;
	md.maxPerPeptide = maxPerPeptide;
	md.description = description;
	varMods.push_back(md);
}

/// <summary>
/// Creates an array of all variable modifications.
/// </summary>
void DBManager::BuildModSet() {
	for (size_t a = 0;a < 128;a++) aaMods[a].clear();
	for (size_t a = 0;a < varMods.size();a++) {
		for (size_t b = 0;b < varMods[a].sites.size();b++) {
			aaMods[varMods[a].sites[b]].push_back(a);
		}
	}
}

/// <summary>
/// Checks if an amino acid has a static modification.
/// </summary>
/// <param name="aa">: the amino acid to look up</param>
/// <param name="mass">: the modification mass if modified</param>
/// <param name="description">: the modification description if modified</param>
/// <returns>true if modified</returns>
bool DBManager::CheckStaticMod(char aa, double& mass, string& description) {
	mass = staticMods[aa].mass;
	description = staticMods[aa].description;
	return mass != 0;
}

/// <summary>
/// Digests the entire proteome to produce a set of candidate peptides for database search
/// </summary>
/// <param name="site">: the amino acids where digestion occurs</param>
/// <param name="except">: exception amino acids where digestion is skipped</param>
/// <param name="cterm">: true denotes digestion is c-terminal to the site</param>
/// <param name="semi">: true denotes digestion is correct on only one peptide terminus</param>
/// <returns></returns>
size_t DBManager::DigestPeptides(std::string site, std::string except, bool cterm, bool semi) {
	Init();

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

		if (aaMods['n'].size() > 0 || aaMods['c'].size() > 0) hasMod = true;

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
					AddPeptide(pep, mass + 18.01056466, a, b, c,hasMod);
				} else if (semi && sites > 0) { //also add semi-enzymatic if the other end is the cut site.
					AddPeptide(pep, mass + 18.01056466, a, b, c,hasMod);
				}

				//if we've reached the maximum missed cleavages, stop here
				if (mc > maxMC) {
					done = true;
					break;
				}

			}

			//if at end of sequence, see if we can add the peptide
			if (!done) AddPeptide(pep, mass + 18.01056466, a, b, db[a].sequence.size() - 1,hasMod);

			//reset our peptide and move on to the next one.
			pep.clear();
			mass = 0;
			sites = 0;
			hasMod = false;
			done = false;
			if (aaMods['n'].size() > 0 || aaMods['c'].size() > 0) hasMod = true;
		}
	}

	//Temporary diagnostics.
	//cout << modMask.size() << " unique modification combinations." << endl;
	//cout << modCount << " modified peptide forms." << endl;
	
	totalPeptidoforms = modCount + peptides.size();
	delete[] varModCount;
	return 0;
}

/// <summary>
/// Set to return first three letters only.
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
string DBManager::GetModDescription(const char& index) {
	return varMods[index].description.substr(0, 3);
}

/// <summary>
/// Gets the mass of a variable modification. Note that bounds are not checked here.
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
double DBManager::GetModMass(const char& index) {
	return varMods[index].mass;
}

/// <summary>
/// Returns a peptide sequence from an index. If the maskIndex is >=0, then include modification annotations.
/// </summary>
/// <param name="index"></param>
/// <param name="maskIndex"></param>
/// <returns></returns>
string DBManager::GetPeptideSequence(const size_t& index, int maskIndex) {
	if(maskIndex==-1) return db[peptides[index].instances[0].dbIndex].sequence.substr(peptides[index].instances[0].start, peptides[index].instances[0].len);

	string tmp = db[peptides[index].instances[0].dbIndex].sequence.substr(peptides[index].instances[0].start, peptides[index].instances[0].len);
	char* mods=new char[tmp.size()+2];
	for (size_t a = 0;a < tmp.size()+2;a++) mods[a] = -1;

	string mask = ModMask(maskIndex);
	for (size_t b = 0;b < mask.size();b += 2) mods[mask[b]] = mask[b + 1];

	string tmp2;
	//n-terminus
	if(mods[tmp.size()]>-1) tmp2 += "[" + GetModDescription(mods[tmp.size()]) + "]";
	//peptide sequence
	for (size_t a = 0;a < tmp.size();a++) {
		tmp2 += tmp[a];
		if (mods[a] > -1) tmp2 += "[" + GetModDescription(mods[a]) + "]";
	}
	//c-terminus
	if (mods[tmp.size()+1] > -1) tmp2 += "[" + GetModDescription(mods[tmp.size()+1]) + "]";
	delete[] mods;
	return tmp2;
}

/// <summary>
/// Returns the pointer to the first byte of the first instance of a peptide sequence.
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
char* DBManager::GetPepSeq(const size_t& index) {
	return &db[peptides[index].instances[0].dbIndex].sequence[peptides[index].instances[0].start];
}

/// <summary>
/// Returns the protein identifier
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
string DBManager::GetProteinName(const size_t& index) {
	return db[index].name;
}

/// <summary>
/// Returns the protein amino acid sequence
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
string& DBManager::GetProteinSeq(const size_t& index) {
	return db[index].sequence;
}

/// <summary>
/// Initializes the DBManager object and sets amino acid masses based on global definitions and user-defined
/// static modifications.
/// </summary>
void DBManager::Init() {
	//reset amino acid masses
	memset(aa, 0, sizeof(double) * 128);

	//Add the atom masses
	aa['A'] = CARBON * 3 + HYDROGEN * 5 + NITROGEN + OXYGEN + staticMods['A'].mass;
	aa['C'] = CARBON * 3 + HYDROGEN * 5 + NITROGEN + OXYGEN + SULFUR + staticMods['C'].mass;
	aa['D'] = CARBON * 4 + HYDROGEN * 5 + NITROGEN + OXYGEN * 3 + staticMods['D'].mass;
	aa['E'] = CARBON * 5 + HYDROGEN * 7 + NITROGEN + OXYGEN * 3 + staticMods['E'].mass;
	aa['F'] = CARBON * 9 + HYDROGEN * 9 + NITROGEN + OXYGEN + staticMods['F'].mass;
	aa['G'] = CARBON * 2 + HYDROGEN * 3 + NITROGEN + OXYGEN + staticMods['G'].mass;
	aa['H'] = CARBON * 6 + HYDROGEN * 7 + NITROGEN * 3 + OXYGEN + staticMods['H'].mass;
	aa['I'] = CARBON * 6 + HYDROGEN * 11 + NITROGEN + OXYGEN + staticMods['I'].mass;
	aa['K'] = CARBON * 6 + HYDROGEN * 12 + NITROGEN * 2 + OXYGEN + staticMods['K'].mass;
	aa['L'] = CARBON * 6 + HYDROGEN * 11 + NITROGEN + OXYGEN + staticMods['L'].mass;
	aa['M'] = CARBON * 5 + HYDROGEN * 9 + NITROGEN + OXYGEN + SULFUR + staticMods['M'].mass;
	aa['N'] = CARBON * 4 + HYDROGEN * 6 + NITROGEN * 2 + OXYGEN * 2 + staticMods['N'].mass;
	aa['P'] = CARBON * 5 + HYDROGEN * 7 + NITROGEN + OXYGEN + staticMods['P'].mass;
	aa['Q'] = CARBON * 5 + HYDROGEN * 8 + NITROGEN * 2 + OXYGEN * 2 + staticMods['Q'].mass;
	aa['R'] = CARBON * 6 + HYDROGEN * 12 + NITROGEN * 4 + OXYGEN + staticMods['R'].mass;
	aa['S'] = CARBON * 3 + HYDROGEN * 5 + NITROGEN + OXYGEN * 2 + staticMods['S'].mass;
	aa['T'] = CARBON * 4 + HYDROGEN * 7 + NITROGEN + OXYGEN * 2 + staticMods['T'].mass;
	aa['U'] = CARBON * 3 + HYDROGEN * 5 + NITROGEN + OXYGEN + SELENIUM + staticMods['U'].mass;
	aa['V'] = CARBON * 5 + HYDROGEN * 9 + NITROGEN + OXYGEN + staticMods['V'].mass;
	aa['W'] = CARBON * 11 + HYDROGEN * 10 + NITROGEN * 2 + OXYGEN + staticMods['W'].mass;
	aa['Y'] = CARBON * 9 + HYDROGEN * 9 + NITROGEN + OXYGEN * 2 + staticMods['Y'].mass;
}

/// <summary>
/// Returns the modification mask as a string of positional indexes at each amino acid.
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
std::string& DBManager::ModMask(const int& index) {
	return modMask[index];
}

/// <summary>
/// Returns a peptide's storage memory structure
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
DBMPeptide& DBManager::Peptide(const size_t& index) {
	return peptides[index];
}

bool DBManager::ProcessVarMod(char aa,size_t modIndex,char pos, double pepMass, double modMass, size_t pepIndex, const string& maskStr) {
	//The index of this mod on this amino acid in our mod vector.
	size_t mi = aaMods[aa][modIndex];

	//Skip if we're already at the maximum instances for this mod
	if (varModCount[mi] >= varMods[mi].maxPerPeptide) return false;

	//Check if the new peptide mass is within database tolerances
	double mm = varMods[mi].mass;
	if (pepMass + mm > minPepMass && pepMass + mm < maxPepMass) {

		//Diagnostic counter
		modCount++;

		//Mark the position of the mod in the mask
		string tStr = maskStr;
		tStr += pos;
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

		//For diagnostics
		//cout << GetPeptideSequence(pepIndex, peptides[pepIndex].mods[c].maskIndex.back()) << endl;

		return true;
	}
	return false;
}

/// <summary>
/// Reads a FASTA file to build a protein sequence database
/// </summary>
/// <param name="fn"></param>
/// <returns></returns>
bool DBManager::ReadFASTA(const char* fn) {
	return db.buildDB(fn);
}

/// <summary>
/// Reads a FASTA file to build a protein sequence database
/// </summary>
/// <param name="fn"></param>
/// <returns></returns>
bool DBManager::ReadFASTA(string fn) {
	return ReadFASTA(fn.c_str());
}

/// <summary>
/// Sets the maximum modifications allowed per peptide
/// </summary>
/// <param name="i"></param>
void DBManager::SetMaxModsPerPeptide(int i) {
	maxMods = i;
}

/// <summary>
/// The number of peptides in the database.
/// </summary>
/// <returns></returns>
size_t DBManager::SizePeptide() {
	return peptides.size();
}

/// <summary>
/// The number of proteins in the database
/// </summary>
/// <returns></returns>
size_t DBManager::SizeProtein() {
	return db.size();
}

/// <summary>
/// Sorting function for ordering peptide list by mass.
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <returns></returns>
bool DBManager::sortMass(const DBMPeptide& a, const DBMPeptide& b) {
	return (a.mass < b.mass);
}