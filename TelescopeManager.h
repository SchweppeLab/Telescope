#ifndef _TELESCOPEMANAGER_H
#define _TELESCOPEMANAGER_H

#include "DataLoader.h"
#include "DBManager.h"
#include "FIManager.h"
#include "GlobalDefinitions.h"
#include "ParamsManager.h"
#include "ResultsExporter.h"

#include <chrono>

class TelescopeManager {
public:
	//TelescopeManager();
	//~TelescopeManager();

	int Launch(bool echo=true);
	bool SetParams(const std::string& fn);

protected:
private:

	bool ExportResults(const std::string& fn,bool echo);
	void Init();

	bool ProcessDB(bool echo);
	bool ProcessIndex(bool echo);
	bool ProcessPeptideMap(bool echo);
	bool ProcessSpectra(const std::string& fn, bool echo);
	bool SearchSpectra(bool echo);

	DataLoader scans;
	DBManager dbm;
	FIManager fim;
	ParamsManager params;

	//For timings:
	std::chrono::steady_clock::time_point start_time, end_time;
	std::chrono::milliseconds duration_milliseconds;
	std::chrono::microseconds duration_microseconds;

};


#endif