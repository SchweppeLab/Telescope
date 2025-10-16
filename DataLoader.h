#ifndef _DATALOADER_H
#define _DATALOADER_H

#include "FastXCorr.h"
#include "FISpectrum.h"
#include "FragmentIonIndex.h"
#include "GlobalDefinitions.h"
#include "MSReader.h"
#include "ParamsManager.h"
#include "Threading.h"
#include "ThreadPool.h"

/// <summary>
/// Structure for processing spectra on separate threads
/// </summary>
struct sSpectrumStruct {
	Mutex* mutex = nullptr;
	bool* thread = nullptr;
	FISpectrum* scan = nullptr;
	sSpectrumStruct(FISpectrum* s) {
		scan = s;
	}
	~sSpectrumStruct() {
		Threading::LockMutex(*mutex);
		*thread = false;
		Threading::UnlockMutex(*mutex);
		scan = NULL;
		thread = NULL;
		mutex = NULL;
	}
};

/// <summary>
/// Class for reading in spectral data files. The underlying library is MSToolkit, which allows for mzML (preferred),
/// mzXML, MGF, etc., and under the right conditions, Thermo raw files. Spectra are currently read from disk in their
/// entirety, then processed in multithreaded fashion. More efficiency could be achieve by multithreaded processing
/// while loading scans, as each scan becomes fully loaded. This will require some additional thread-safe engineering.
/// 
/// On a side note, Telescope could do a better job profiling the loading and processing to make clearer the expected
/// computation times for each. In particular, knowing the processing time independent of any disk reading time is
/// critical to real-time applications using Telescope.
/// </summary>
class DataLoader {
public:
	DataLoader();
	~DataLoader();

	FISpectrum& operator[](const size_t& index);

	bool Initialize(DBManager* d, FragmentIonIndex* f, ParamsManager* p);
	bool ReadSpectra(const std::string& fn);
	size_t Size();

	//This obscure data member is used to identify the largest number of peptides
	//that will be scored against any one spectrum. It is used to buffer a score array
	//that will then have sufficient memory to be used with any spectrum.
	size_t maxScoreCount = 0;

protected:
private:

	void Allocate();
	void Deallocate();
	static void ProcessSpectrum(FISpectrum& s, int tIndex);
	static void ProcessSpectrumProc(sSpectrumStruct* s);

	DBManager* dbm = nullptr;
	FragmentIonIndex* fii = nullptr;
	ParamsManager* params = nullptr;
	std::vector<FISpectrum> scans;

	//array of xcorr transformers, one per thread
	static FastXCorr* xcorr; 
	static Mutex mutexThreads;
	static bool* activeThread;
	static size_t threads;
};


#endif