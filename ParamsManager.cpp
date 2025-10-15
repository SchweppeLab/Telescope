#include "ParamsManager.h"

using namespace std;

bool ParamsManager::CheckParams(){
	if (fastaFile.empty()) return false;
	if (dataFile.empty()) return false;
	return true;
}

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
}

void ParamsManager::ProcessParam(vector<string>& par) {
	if (par[0].compare("bin_size") == 0) {
		binSize = stod(par[1]);
	} else if(par[0].compare("data_file")==0) {
		dataFile.push_back(par[1]);
	} else if (par[0].compare("database") == 0) {
		fastaFile = par[1];
	} else if (par[0].compare("min_mz") == 0) {
		minMZ = stod(par[1]);
	} else if(par[0].compare("threads")==0){
		threads = stoi(par[1]);
	} else {
		cout << "Unknown parameter: " << par[0] << endl;
	}
}

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