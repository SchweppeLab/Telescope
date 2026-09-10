#include "ParamsManager.h"

using namespace std;

/// <summary>
/// Checks if params object has parameters that only the user can provide, such as the FASTA database and
/// spectral data files. All other parameters have default values if not provided by the user.
/// </summary>
/// <returns>true if FASTA and spectral data files were provided.</returns>
bool ParamsManager::CheckParams(){
	if (fastaFile.empty()) return false;
	if (dataFile.empty()) return false;
	return true;
}

/// <summary>
/// Reads a text-based search parameters file.
/// </summary>
/// <param name="fn">Name (and path if not in current working directory) of the file to read.</param>
/// <returns>True if file was found and parsed.</returns>
bool ParamsManager::ReadParams(const string& fn) {
	ifstream file(fn);
	if (!file.is_open()) return false;

	string s;
	vector<string> tokens;
	while (getline(file, s)) {
		if (s.size() < 2) continue;
		if (Tokenize(s, tokens) < 2) continue;

		ProcessParam(tokens);
	}

	return true;
}
/// <summary>
/// Processes the parameter and value tokens and updates the corresponding search parameter.
/// </summary>
/// <param name="par"></param>
void ParamsManager::ProcessParam(vector<string>& par) {
	if (par[0].compare("bin_size") == 0) {
		binSize = stod(par[1]);
	} else if(par[0].compare("data_file")==0) {
		dataFile.push_back(par[1]);
	} else if (par[0].compare("database") == 0) {
		fastaFile = par[1];
	} else if (par[0].compare("enzyme_specificity") == 0) {
		if (par[1][0] == '0') semiEnzyme = false;
		else semiEnzyme = true;
	} else if (par[0].compare("expect") == 0) {
		if (par[1][0] == '0') expect = false;
		else expect = true;
	} else if (par[0].compare("max_frag_z") == 0) {
		maxFragZ = stoi(par[1]);
		if (maxFragZ < 1) {
			cout << "WARNING: max_frag_z was set to lower limit o1." << endl;
			maxFragZ = 1;
		}
	} else if (par[0].compare("max_missed_cleavage") == 0) {
		maxMC = stoi(par[1]);
	} else if (par[0].compare("max_mz") == 0) {
		maxMZ = stod(par[1]);
	} else if (par[0].compare("max_peptide_length") == 0) {
		maxPepLen = stoi(par[1]);
	} else if (par[0].compare("max_peptide_mass") == 0) {
		maxPepMass = stod(par[1]);
	} else if (par[0].compare("max_peptide_modification") == 0) {
		maxMods = stoi(par[1]);
	} else if (par[0].compare("min_mz") == 0) {
		minMZ = stod(par[1]);
	} else if (par[0].compare("min_peak_xcorr") == 0) {
		minPeak = stod(par[1]);
		if (minPeak < 1e-6) {
			cout << "WARNING: min_peak_xcorr was set to lower limit of 1e-6." << endl;
			minPeak = 1e-6;
		}
	} else if (par[0].compare("min_peptide_length") == 0) {
		minPepLen = stoi(par[1]);
	} else if (par[0].compare("min_peptide_mass") == 0) {
		minPepMass = stod(par[1]);
	} else if (par[0].compare("precursor_ppm") == 0) {
		ppm = stod(par[1]);
	} else if (par[0].compare("psm_per_scan") == 0) {
		psmCount = stoi(par[1]);
	} else if (par[0].compare("static_modification") == 0) {
		if (par.size() != 4) {
			cout << "WARNING: Malformatted static_modification. Line skipped." << endl;
		} else {
			PMMod m;
			m.variable = false;
			m.mass = stod(par[1]);
			m.sites = par[2];
			m.description = par[3];
			mods.push_back(m);
		}
	} else if(par[0].compare("threads")==0){
		threads = stoi(par[1]);
	} else if (par[0].compare("variable_modification") == 0) {
		if (par.size() != 5) {
			cout << "WARNING: Malformatted variable_modification. Line skipped." << endl;
		} else {
			PMMod m;
			m.variable = true;
			m.mass = stod(par[1]);
			m.maxPerPeptide = stoi(par[2]);
			m.sites = par[3];
			m.description = par[4];
			mods.push_back(m);
		}
	} else if (par[0].compare("xcorr") == 0) {
		if (par[1][0] == '0') xcorr = false;
		else xcorr = true;
	} else if (par[0].compare("ultra_xcorr") == 0) {
		if (par[1][0] == '0') ultraxcorr = false;
		else ultraxcorr = true;
	} else {
		cout << "Unknown parameter: " << par[0] << endl;
	}
}

/// <summary>
/// Tokenizes a parameter file line and returns a set of strings.
/// </summary>
/// <param name="st">The line to tokenize</param>
/// <param name="tok">The set of resulting tokens</param>
/// <returns>The number of tokens produced</returns>
size_t ParamsManager::Tokenize(const string& st, vector<string>& tok) {
	tok.clear();
	string token;
	istringstream ss(st);
	while (ss >> token) {
		if (token[0] == '#') break;
		else if (token[0] == '=') {
			if (tok.size() != 1) cout << "WARNING! Malformed param line: " << st << endl;
			continue;
		}
		tok.push_back(token);
	}
	return tok.size();
}