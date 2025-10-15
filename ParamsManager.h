#ifndef _PARAMSMANAGER_H
#define _PARAMSMANAGER_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class ParamsManager {
public:

	bool CheckParams(); //Returns true when params are complete and well formed.
	bool ReadParams(const std::string& fn);  //Processes parameters from a data file.


	//User data files
	std::vector<std::string> dataFile;		//The spectral data file(s)
	std::string fastaFile;								//The FASTA database

	//Spectral Processing Parameters
	double binSize = 0.02;  //peak matching bin size
	double minMZ = 200.0;   //lowest spectral mass to search
	double maxMZ = 2000.0;  //highed spectral mass to search
	double minPeak = 2; //1e-6    //magnitude (absolute) threshold following Xcorr transformation
	bool xcorr = true;      //perform Xcorr transformation

	//Database Parsing Parameters
	int maxMC = 2;  //maximum number of enzyme missed cleavages allowed
	int minPepLen = 5;          //minimum peptide length allowed
	double minPepMass = 600.0;  //minimum peptide mass allowed
	int maxPepLen = 50;         //maximum peptide length allowed
	double maxPepMass = 5000.0; //maximum peptide mass  allowed
	int maxMods = 2;						//maximum modifications allowed per peptide
	bool semiEnzyme = false;    //semi-enzyme specificity

	//Search Parameters
	double ppm = 20.0;    //+/- PPM mass tolerance (symmetrical) for spectrum precursor
	int psmCount = 5;     //maximum number of PSMs to export to PepXML
	int threads = 18;     //maximum number of concurrent processing threads

protected:
private:

	void ProcessParam(std::vector<std::string>& par);
	size_t Tokenize(const std::string& st, std::vector<std::string>& tok);

};


#endif