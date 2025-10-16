#include "TelescopeManager.h"

using namespace std;

int TelescopeManager::Launch(bool echo) {
	//---------------------
	// STEP #1: Confirm parameters.
	//---------------------
	if (!params.CheckParams()) return 1;
	Init();

	//---------------------
	// STEP #2: Read in FASTA database.
	//---------------------
	if (!ProcessDB(echo)) return 2;

	//---------------------
	// STEP #3: Generate fragment ion index.
	//---------------------
	if (!ProcessPeptideMap(echo)) return 3;
	if (!ProcessIndex(echo)) return 4;

	//---------------------
	// STEP #4: Read and process each spectra file.
	//---------------------
	for (size_t a = 0;a < params.dataFile.size();a++) {
		if (!ProcessSpectra(params.dataFile[a], echo)) return 5;

		//---------------------
		// STEP #5: Perform search
		//---------------------
		if (!SearchSpectra(echo)) return 6;

		//---------------------
		// STEP #6: Export results
		//---------------------
		size_t ext = params.dataFile[a].find_last_of('.');
		string out = params.dataFile[a].substr(0, ext + 1) + "ts.pep.xml";
		if (!ExportResults(out, echo)) return 7;
	}

	return 0;
}

bool TelescopeManager::ExportResults(const string& fn, bool echo) {
	if (echo) cout << "Exporting results to " + fn + " ...";
	ResultsExporter re;
	re.Initialize(&dbm, fim.fii, &params);

	start_time = chrono::high_resolution_clock::now();
	bool ret = re.Write(fn, scans);
	end_time = std::chrono::high_resolution_clock::now();
	duration_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

	if (echo) {
		if (ret) cout << "Success" << endl;
		else cout << "Failed" << endl;
		cout << "Duration: " << duration_milliseconds.count() << " ms." << endl;
	}

	return ret;
}

/// <summary>
/// Initializes the objects required for the search. This is called after
/// validating the parameters object.
/// </summary>
void TelescopeManager::Init() {
	fim.Initialize(&dbm, &params);
	scans.Initialize(&dbm, fim.fii, &params);
}

/// <summary>
/// Reads in a FASTA file and computes all the peptides that qualify for searching within the 
/// user-defined parameters.
/// </summary>
/// <param name="echo"></param>
/// <returns>True if successful</returns>
bool TelescopeManager::ProcessDB(bool echo) {
	if (echo) cout << "Digesting FASTA file...";

	//Set parameters - TODO: Pass parameters object
	dbm.minPepMass = params.minPepMass;
	dbm.maxPepMass = params.maxPepMass;
	dbm.minPepLen = params.minPepLen;
	dbm.maxPepLen = params.maxPepLen;
	dbm.maxMC = params.maxMC;
	dbm.maxMods = params.maxMods;
	for (size_t a = 0;a < params.mods.size();a++) {
		if (params.mods[a].variable) {
			dbm.AddVariableMod(params.mods[a].sites, params.mods[a].mass, params.mods[a].maxPerPeptide, params.mods[a].description);
		} else {
			dbm.AddStaticMod(params.mods[a].sites, params.mods[a].mass, params.mods[a].description);
		}
	}

	//Read the fasta file
	if (!dbm.ReadFASTA(params.fastaFile)) return false;

	//Digest the database to find all peptides
	dbm.DigestPeptides("KR", "P", true, params.semiEnzyme);

	//Echo statistics if requested.
	if (echo) {
		cout << "Done" << endl;
		cout << "Proteins: " << dbm.SizeProtein() << endl;
		cout << "Peptides: " << dbm.SizePeptide() << endl;

		size_t realCount = 0;
		long long bytes = dbm.SizePeptide() * sizeof(DBMPeptide);
		for (size_t a = 0;a < dbm.SizePeptide();a++) {
			realCount++;
			bytes += dbm.Peptide(a).instances.size() * sizeof(DBMPepIndex);
			bytes += dbm.Peptide(a).mods.size() * sizeof(DBMPepMod);
			for (size_t b = 0;b < dbm.Peptide(a).mods.size();b++) {
				bytes += dbm.Peptide(a).mods[b].maskIndex.size() * sizeof(size_t);
				realCount += dbm.Peptide(a).mods[b].maskIndex.size();
			}
		}
		cout << "Total Peptides memory: " << (double)bytes / 1073741824 << " GB." << endl;
		cout << "Total Peptides with Modifications: " << realCount << endl;
		cout << dbm.totalPeptidoforms << " standard Peptidoforms." << endl;
	}

	return true;
}

bool TelescopeManager::ProcessIndex(bool echo) {
	if(echo) cout << "Generating fragment ion index...";

	start_time = chrono::high_resolution_clock::now();
	fim.GenerateIndex();
	end_time = std::chrono::high_resolution_clock::now();
	duration_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

	if (echo) {
		cout << "Done" << endl;
		cout << "Duration: " << duration_milliseconds.count() << " ms." << endl;
	}
	return true;
}

bool TelescopeManager::ProcessPeptideMap(bool echo) {
	if(echo) cout << "Generating Peptidoform Map...";

	start_time = chrono::high_resolution_clock::now();
	fim.GeneratePeptideMap();
	end_time = std::chrono::high_resolution_clock::now();
	duration_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

	if (echo) {
		cout << "Done" << endl;
		cout << "Duration: " << duration_milliseconds.count() << " ms." << endl;
	}
	return true;
}

bool TelescopeManager::ProcessSpectra(const std::string& fn, bool echo) {
	//Load all spectra to analyze using a DataLoader. This not only opens the spectra, but also
	//does any processing (e.g., Xcorr transformation) prior to analysis. Note that when reading the
	//spectra, the fragment ion index is required to determine the peptide indexes to search.
	if (echo) cout << "Reading and Processing " + fn + " ...";
	start_time = chrono::high_resolution_clock::now();
	bool ret = scans.ReadSpectra(fn);  	//Load spectra
	end_time = std::chrono::high_resolution_clock::now();
	duration_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

	if (echo) {
		if (!ret) cout << "Fail" << endl;
		else {
			cout << "Success" << endl;

			cout << "Duration: " << duration_milliseconds.count() << " ms." << endl;
			cout << (double)duration_milliseconds.count() / scans.Size() << " ms average per scan." << endl;

			//Calculate the approximate memory useage. Note that the precursor sizes are not fully calculated
			long long bytes = 0;
			for (size_t a = 0;a < scans.Size();a++) {
				bytes += scans[a].Capacity() * sizeof(FIPeak);
			}
			cout << "Scan count: " << scans.Size() << " consuming " << (double)bytes / 1073741824 << " Gb." << endl;
		}
	}
	return ret;
}

bool TelescopeManager::SearchSpectra(bool echo) {
	//Allocate memory for search scores. Note that we now use (and reuse) central memory storage for the
	//peptide scores for a spectrum. Once the spectrum analysis is over, this memory is used for a different scan
	//and the scores are lost.
	if(echo) cout << "Allocating score memory, size per thread: " << scans.maxScoreCount << "...";
	fim.AllocateScoreMemory(scans.maxScoreCount);
	if(echo) cout << "Done" << endl;

	//The actual search starts here.
	if(echo) cout << "Begin search of " << scans.Size() << " scans." << endl;
	start_time = chrono::high_resolution_clock::now();
	fim.ScoreSpectrum(scans);
	end_time = std::chrono::high_resolution_clock::now();
	duration_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

	if (echo) {
		cout << "End search" << endl;
		cout << scans.Size() << " spectra searched on " << params.threads << " threads." << endl;
		cout << "Duration: " << duration_milliseconds.count() << " ms." << endl;
		cout << (double)duration_milliseconds.count() / scans.Size() << " ms average per scan." << endl;
	}

	return true;
}

bool TelescopeManager::SetParams(const string& fn) {
	return params.ReadParams(fn);
}