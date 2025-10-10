#include "DataLoader.h"
#include "DBaser.h"
#include "DBManager.h"
#include "FIManager.h"
#include "FISpectrum.h"
#include "ResultsExporter.h"
#include "SpectrumGenerator.h"

#include "GlobalDefinitions.h"

#include <algorithm>
#include <chrono>

using namespace dbaser;
using namespace std;

typedef struct sTop {
	size_t index=0;
	float score=0;
	double eValue = 9999;
} sTop;

typedef struct sPrecursor {
	double mass = 0;
	int charge = 0;
	string peptide;
} sPrecursor;

void addPrecursor(const string& pep, double mass, int charge);
void addSpectrum(const size_t& index);
int  customScan(string fn);   //returns the maximum number of peptides scores to be computed for the spectrum.
void eValue(sTop* arr, size_t count);
void init();
void linearRegression2(double& slope, double& intercept, int& iMaxXcorr, int& iStartXcorr, int& iNextXcorr, double& rSquared);
bool sortScores(const sTop& a, const sTop& b);

vector<sPrecursor> precursors;  //list of all candidate precursor ions; allows for multiple precursors for a single spectrum
//vector<FISpectrum> scans;           //the scan data
double AA[128] = { 0 };

//For XCorr preprocessing: Make one per thread for multithreading
//FastXCorr* fx;

int histogram[HISTOSZ];
int histogramCount;
double dCummulative[HISTOSZ];

int main(int argc, char* argv[]) {

	//initialize the test applicaiton
	init();
	long long bytes = 0;  //used at various stages to compute memory usage.

	//For timings:
	chrono::steady_clock::time_point start_time, end_time;
	chrono::milliseconds duration_milliseconds;
	chrono::microseconds duration_microseconds;

	//Set up our search space parameters
	cout << "Digesting FASTA file...";
	DBManager dbm;
	dbm.minPepMass = MINPEPMASS;
	dbm.maxPepMass = MAXPEPMASS;
	dbm.minPepLen = MINPEPLEN;
	dbm.maxPepLen = MAXPEPLEN;
	dbm.maxMC = MAXMC;
	//dbm.AddVariableMod("STY", 79.966331, 2, "Phosphorylation");
	dbm.AddVariableMod("M", 15.9949, 2, "Oxidation");
	//dbm.AddXL("K", "BS3");
	dbm.maxMods = MAXMODS;
	//dbm.maxModsXL = 2;
	dbm.ReadFASTA(argv[1]);
	dbm.DigestPeptides("KR","P",true,SEMIENZYME);
	cout << "Done" << endl;
	cout << "Proteins: " << dbm.SizeProtein() << endl;
	cout << "Peptides: " << dbm.SizePeptide() << endl;
	size_t realCount = 0;
	bytes = dbm.SizePeptide() * sizeof(DBMPeptide);
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
	//cout << dbm.totalPeptidoformsXL << " crosslinked Peptidoforms." << endl;

	/*SpectrumGenerator sg(&dbm);
	sg.echo = true;
	sg.GenerateSpectrum("PEPTIDEK",2);
	sg.GenerateSpectrum("VVSAAHCYKSR", 8, "FINAAKIITHPR",5,4,0);
	exit(1);*/

	//This is where we create the fragment ion index. The FIManager is a controller class that coordinates
	//several subclasses involved in the management of the indexed search. It needs two inputs at construction:
	//A DBManager object, and the number of threads for computation. Then, set the bin size desired and generate
	//the peptide list and the index.
	FIManager fim(&dbm,THREADS);
	fim.SetBinSize(BINSIZE);
	cout << "Parsing peptides in FIManager...";
	start_time = chrono::high_resolution_clock::now();
	fim.GeneratePeptideMap();
	//fim.GeneratePeptideMap("K");  //For crosslinking, add the amino acids that can be linked.
	end_time = std::chrono::high_resolution_clock::now();
	duration_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	cout << "Done" << endl;
	cout << "Duration: " << duration_milliseconds.count() << " ms." << endl;
	
	cout << "Threaded fragment ions...";
	start_time = chrono::high_resolution_clock::now();
	fim.GenerateIndex();
	end_time = std::chrono::high_resolution_clock::now();
	duration_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	cout << "Done" << endl;
	cout << "Duration: " << duration_milliseconds.count() << " ms." << endl;

	//Load all spectra to analyze using a DataLoader. This not only opens the spectra, but also
	//does any processing (e.g., Xcorr transformation) prior to analysis. Note that when reading the
	//spectra, the fragment ion index is required to determine the peptide indexes to search.
	cout << "Processing Scans...";
	start_time = chrono::high_resolution_clock::now();
	DataLoader scans(fim.fii,THREADS);
	scans.dbm = &dbm; //temporary
	string fn = argv[2];
	scans.ReadSpectra(fn);  	//Load spectra
	end_time = std::chrono::high_resolution_clock::now();
	duration_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	cout << "Duration: " << duration_milliseconds.count() << " ms." << endl;
	cout << (double)duration_milliseconds.count() / scans.Size() << " ms average per scan." << endl;

	//Calculate the approximate memory useage. Note that the precursor sizes are not fully calculated
	bytes = 0;
	for (size_t a = 0;a < scans.Size();a++) {
		bytes += scans[a].Capacity() * sizeof(FIPeak);
	}
	cout << "Done" << endl;
	cout << "Scan count: " << scans.Size() << " consuming " << (double)bytes / 1073741824 << " Gb." << endl;


	//Allocate memory for search scores. Note that we now use (and reuse) central memory storage for the
	//peptide scores for a spectrum. Once the spectrum analysis is over, this memory is used for a different scan
	//and the scores are lost.
	cout << "Allocating score memory, size per thread: " << scans.maxScoreCount << " and " << scans.maxScoreCountXL << "...";
	fim.AllocateScoreMemory(scans.maxScoreCount, scans.maxScoreCountXL);
	cout << "Done" << endl;

	//The actual search starts here.
	cout << "Begin threaded search of " << scans.Size() << " scans." << endl;
	bytes = 0;
	start_time = chrono::high_resolution_clock::now();
	fim.ScoreSpectrum(scans);
	//fim.fii->ScoreSpectrumXLFirst(scans[0]);
	//fim.fii->ScoreSpectrumXLSecond(scans[0]);
	//fim.fii->ScoreSpectrumXLThird(scans[0]);
	end_time = std::chrono::high_resolution_clock::now();
	duration_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

	cout << "End search" << endl;
	cout << scans.Size() << " spectra searched on " << THREADS << " threads." << endl;
	cout << "Duration: " << duration_milliseconds.count() << " ms." << endl;
	cout << (double)duration_milliseconds.count() / scans.Size() << " ms average per scan." << endl;

	//cout << "Before top scores" << endl;
	//for (size_t a = 0;a < 10;a++) {
	//	size_t scoreIndex = scans[0].precursor[0].ts[a].index + scans[0].precursor[0].pepOffsetXL;
	//	size_t pepIndex = fim.fii->peptidesXL[scoreIndex].peptideIndex;
	//	float score = scans[0].precursor[0].ts[a].score;
	//	printf("%d\t%s\t%.4f\n", (int)a, dbm.GetPeptideSequence(pepIndex).c_str(), score);
	//}
	//cout << "After top scores" << endl;


	//Report first and last 20 PSMs
	for (size_t a = 0;a < 20 && a<scans.Size();a++) {
		size_t topIndex = scans[a].precursor[0].ts[0].index;
		double topScore = scans[a].precursor[0].ts[0].score;
		//printf("Precursor %d is %s, TopPeptide: %s\t%.2lf\n", (int)a,precursors[a].peptide.c_str(),db[peptides[topIndex].instances[0].dbIndex].sequence.substr(peptides[topIndex].instances[0].start, peptides[topIndex].instances[0].len).c_str(), topScore * 0.005);
		printf("Spectrum %d is %s, TopPeptide: %s\t%.4lf\t%.4lf\t%d\t%.6lf vs. %.6lf\n", (int)a, scans[a].precursor[0].peptide.c_str(), dbm.GetPeptideSequence(fim.fii->peptides[topIndex].peptideIndex, fim.fii->peptides[topIndex].maskIndex).c_str(), topScore,topScore * 0.005,(int)topIndex,scans[a].precursor[0].mass, fim.fii->peptides[topIndex].mass);
		if (fim.fii->pepArrSzXL > 0 && scans[a].precursor[0].scoreCountXL>0) {
			topIndex = scans[a].precursor[0].tsXL[0].indexA;
			size_t topIndexB = scans[a].precursor[0].tsXL[0].indexB;
			topScore = scans[a].precursor[0].tsXL[0].scoreA + scans[a].precursor[0].tsXL[0].scoreB;
			topScore = round(topScore * 5) / 1000.0;  // round to 3 decimal points like Comet
			if (scans[a].precursor[0].tsXL[0].scoreA < 0 || scans[a].precursor[0].tsXL[0].scoreB<0) continue;
			printf("  SpectrumXL %d is %s, TopXL: %s-%s\t%.4lf\n", (int)a, scans[a].precursor[0].peptide.c_str(), dbm.GetPeptideSequence(fim.fii->peptidesXL[topIndex].peptideIndex).c_str(), dbm.GetPeptideSequence(fim.fii->peptidesXL[topIndexB].peptideIndex).c_str(), topScore);
			printf("  Mass: %.6lf vs. %.6lf\t%d\n", scans[a].precursor[0].mass, fim.fii->peptidesXL[topIndex].mass + fim.fii->peptidesXL[topIndexB].mass,(int)topIndex);
		}
	}

	if (scans.Size() > 20) {
		for (size_t a = scans.Size() - 20;a < scans.Size();a++) {
			size_t topIndex = scans[a].precursor[0].topIndex + scans[a].precursor[0].pepOffset;
			double topScore = scans[a].precursor[0].topScore;
			//printf("Precursor %d is %s, TopPeptide: %s\t%.2lf\n", (int)a, precursors[a].peptide.c_str(), db[peptides[topIndex].instances[0].dbIndex].sequence.substr(peptides[topIndex].instances[0].start, peptides[topIndex].instances[0].len).c_str(), topScore * 0.005);
			printf("Spectrum %d is %s, TopPeptide: %s\t%.4lf\t%d\n", (int)a, scans[a].precursor[0].peptide.c_str(), dbm.GetPeptideSequence(fim.fii->peptides[topIndex].peptideIndex, fim.fii->peptides[topIndex].maskIndex).c_str(), topScore * 0.005, (int)topIndex);
			if (fim.fii->pepArrSzXL > 0 && scans[a].precursor[0].scoreCountXL > 0) {
				topIndex = scans[a].precursor[0].tsXL[0].indexA;
				size_t topIndexB = scans[a].precursor[0].tsXL[0].indexB;
				topScore = scans[a].precursor[0].tsXL[0].scoreA + scans[a].precursor[0].tsXL[0].scoreB;
				topScore = round(topScore*5) / 1000.0;  // round to 3 decimal points like Comet
				if (scans[a].precursor[0].tsXL[0].scoreA < 0 || scans[a].precursor[0].tsXL[0].scoreB < 0) continue;
				printf("  SpectrumXL %d is %s, TopXL: %s-%s\t%.4lf\n", (int)a, scans[a].precursor[0].peptide.c_str(), dbm.GetPeptideSequence(fim.fii->peptidesXL[topIndex].peptideIndex).c_str(), dbm.GetPeptideSequence(fim.fii->peptidesXL[topIndexB].peptideIndex).c_str(), topScore);
				printf("  Mass: %.6lf vs. %.6lf\t%d\n", scans[a].precursor[0].mass, fim.fii->peptidesXL[topIndex].mass + fim.fii->peptidesXL[topIndexB].mass, (int)topIndex);
			}
		}
	}


	//FILE* f = fopen("results.txt", "wt");
	//for (size_t a = 0;a < scans.Size();a++) {
	//	size_t topIndex = scans[a].precursor[0].ts[0].index;
	//	double topScore = scans[a].precursor[0].ts[0].score;
	//	double ts = topScore * 0.005;
	//	ts = round(ts * 1000.0) / 1000.0;  // round to 3 decimal points like Comet
	//	fprintf(f, "%d\t%.4lf\t%s\n", scans[a].scanNumber, ts, dbm.GetPeptideSequence(fim.fii->peptides[topIndex].peptideIndex, fim.fii->peptides[topIndex].maskIndex).c_str());
	//
	//	if (fim.fii->pepArrSzXL > 0 && scans[a].precursor[0].scoreCountXL > 0) {
	//		topIndex = scans[a].precursor[0].tsXL[0].indexA;
	//		size_t topIndexB = scans[a].precursor[0].tsXL[0].indexB;
	//		topScore = scans[a].precursor[0].tsXL[0].scoreA + scans[a].precursor[0].tsXL[0].scoreB;
	//		topScore = round(topScore * 5) / 1000.0;  // round to 3 decimal points like Comet
	//		if (scans[a].precursor[0].tsXL[0].scoreA < 0 || scans[a].precursor[0].tsXL[0].scoreB < 0) continue;
	//		printf("  SpectrumXL %d is %s, TopXL: %s-%s\t%.4lf\n", scans[a].scanNumber, scans[a].precursor[0].peptide.c_str(), dbm.GetPeptideSequence(fim.fii->peptidesXL[topIndex].peptideIndex).c_str(), dbm.GetPeptideSequence(fim.fii->peptidesXL[topIndexB].peptideIndex).c_str(), topScore);
	//		printf("  Mass: %.6lf vs. %.6lf\t%d\n", scans[a].precursor[0].mass, fim.fii->peptidesXL[topIndex].mass + fim.fii->peptidesXL[topIndexB].mass+ 138.068074, (int)topIndex);
	//	}
	//
	//}
	//fclose(f);

	//Export Results Properly
	ResultsExporter re;
	re.dbm = &dbm;
	re.fii = fim.fii;
	re.Write("output.pep.xml", scans);

	//cout << scans[50000].scanNumber << ": " << endl;
	//for (size_t a = 0;a < scans[50000].precursor[0].ts.Size();a++) {
	//	size_t index = scans[50000].precursor[0].ts[a].index;
	//	cout << "\t" << a + 1 << "\t" << scans[50000].precursor[0].ts[a].score << "\t" << dbm.GetPeptideSequence(fim.fii->peptides[index].peptideIndex, fim.fii->peptides[index].maskIndex) << endl;
	//}

	//Report all the PSMs from a spectrum. Next step is to expand this to compute e-value.
	//cout << "Top 10 peptides (out of " << scans.back().precursor[0].scoreCount << ") from last scan : " << endl;
	//start_time = chrono::high_resolution_clock::now();
	//sTop* best = new sTop[scans.back().precursor[0].scoreCount];
	//for (size_t a = 0;a < scans.back().precursor[0].scoreCount;a++) {
	//	best[a].index = a + scans.back().precursor[0].pepOffset;
	//	best[a].score = scans.back().precursor[0].scores[a]*0.005;
	//}
	//sort(best, best + scans.back().precursor[0].scoreCount, sortScores);
	//eValue(best, scans.back().precursor[0].scoreCount);
	//end_time = std::chrono::high_resolution_clock::now();
	//auto duration_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
	//cout << "Duration: " << duration_microseconds.count() << " us." << endl;
	//for (size_t a = 0;a<10 && a < scans.back().precursor[0].scoreCount;a++) {
	//	if (best[a].score <= 0) break;
	//	printf("PSM %d: %s\t%.2lf\t%E\n", (int)a+1, dbm.GetPeptideSequence(fii->peptides[best[a].index].peptideIndex,fii->peptides[best[a].index].maskIndex).c_str(), best[a].score, best[a].eValue);
	//}


	//clean up memory.
	cout << "Start memory cleanup...";
	cout << "Done" << endl;
	return 0;

}

void addPrecursor(const string& pep, double mass, int charge) {
	precursors.emplace_back();
	precursors.back().charge = charge;
	precursors.back().mass = mass;
	precursors.back().peptide = pep;
}

//void addSpectrum(const size_t& index) {
//	size_t scanIndex = scans.size();
//	scans.emplace_back();
//	
//	string pep = precursors[index].peptide;
//	char zLimit = precursors[index].charge - 1;
//	if (zLimit == 0) zLimit = 1;
//	if (zLimit > 3) zLimit = 3;
//
//	//b-ions
//	double m = 0;
//	for (size_t b = 0;b < pep.size() - 1;b++) {
//		m += AA[pep[b]];
//
//		for (char z = 0;z < zLimit;z++) {
//			double mz = (m + (z + 1) * PROTON) / (z + 1);
//			//setting m/z limit on ions
//			if (mz < fii->maxMZ) {
//				FIPeak p;
//				p.mz = mz;
//				p.fIndex = mz * fii->invBinSize;
//				p.value = rand() % 100 + 1;
//				scans[scanIndex].AddPeak(p);
//			}
//		}
//	}
//
//	//y-ions
//	m = 18.01056466;
//	for (size_t b = pep.size() - 1;b > 0;b--) {
//		m += AA[pep[b]];
//
//		for (char z = 0;z < zLimit;z++) {
//			double mz = (m + (z + 1) * PROTON) / (z + 1);
//			//setting m/z limit on ions
//			if (mz < fii->maxMZ) {
//				FIPeak p;
//				p.mz = mz;
//				p.fIndex = mz * fii->invBinSize;
//				p.value = rand() % 100 + 1;
//				scans[scanIndex].AddPeak(p);
//			}
//		}
//	}
//
//	for (size_t b = 0;b < NOISE;b++) {
//		FIPeak p;
//		p.fIndex = rand() % fii->maxBin;
//		p.mz = p.fIndex * BINSIZE;
//		p.value = rand() % 20 + 1;
//		scans[scanIndex].AddPeak(p);
//	}
//
//	if(XCORR) fx->ProcessSpectrum(scans[scanIndex]);
//}

//int customScan(string fn) {
//	FIPrecursor pre;
//	double max = 0;
//
//	char str[256];
//	char* tok;
//	FILE* f = fopen(fn.c_str(), "rt");
//	fgets(str, 256, f);
//	tok = strtok(str, "\n\r");
//	pre.peptide = tok;
//	fgets(str, 256, f);
//	tok = strtok(str, "\n\r");
//	pre.charge = atoi(tok);
//	fgets(str, 256, f);
//	tok = strtok(str, "\n\r");
//	pre.mass = atof(tok);
//	fgets(str, 256, f);
//	tok = strtok(str, "\n\r");
//	max = atof(tok);
//
//	scans.emplace_back();
//	while (!feof(f)) {
//		if (fgets(str, 256, f) == NULL) continue;
//		if (strlen(str) < 2) continue;
//		tok = strtok(str, "\t \n\r");
//		FIPeak pk;
//		double mz = atof(tok);
//		pk.mz = mz;
//		pk.fIndex = (size_t)(mz * fii->invBinSize);
//		tok = strtok(NULL, "\t \n\r");
//		pk.value = atof(tok);//(int)(atof(tok) / max*100);
//		if (mz<MAXMZ && pk.value > 0) scans.back().AddPeak(pk);
//	}
//	fclose(f);
//	
//	double err = pre.mass / 1e6*PPM;
//	double min = pre.mass - err;
//	max = pre.mass + err;
//	size_t index = fii->FindPeptideIndex(min); //should return the first index below the desired mass
//	while (fii->peptides[index].mass < min) index++;
//	pre.pepOffset = index;
//	scans.back().precursor.push_back(pre);
//
//	int count = 1;
//	while (fii->peptides[index++].mass < max) count++;
//	scans.back().precursor.back().scoreCount = count;
//
//	//This needs to be improved for cases where there are no peptides in the index
//	//that fulfill the precursor mass requirements.
//	//if (fii->pepArrSzXL > 0 && pre.mass > MINPEPMASS * 2) {
//	//	min = pre.mass / 2;
//	//	max = pre.mass - MINPEPMASS;
//	//	min -= (min / 1e6 * PPM);
//	//	max += (max / 1e6 * PPM);
//	//	index = fii->FindPeptideIndexXL(min);
//	//	while (fii->peptidesXL[index].mass < min) index++;
//	//	scans.back().precursor.back().pepOffsetXL = index;
//	//	count = 1;
//	//	while (fii->peptidesXL[index++].mass < max) count++;
//	//	scans.back().precursor.back().scoreCountXL = count;
//	//}
//
//	if (XCORR) fx->ProcessSpectrum(scans.back());
//
//	//for (size_t a = 0;a < scans.back().Size();a++) {
//	//	cout << scans.back()[a].fIndex << "\t" << scans.back()[a].fIndex * BINSIZE << "\t" << scans.back()[a].value << endl;
//	//}
//
//	return count;
//
//}

void eValue(sTop* arr, size_t count) {
	int iMaxCorr;
	int iStartCorr;
	int iNextCorr;
	double dSlope;
	double dIntercept;
	double dRSquare;
	bool bSkipXL = false;
	bool bSingletFail = false;
	double topScore = 0;

	memset(histogram, 0, sizeof(int) * HISTOSZ);
	for (size_t a = 0;a < count;a++) {
		int index = (int)(arr[a].score * 10+0.5);
		if (index < 0) index = 0;
		else if (index >= HISTOSZ) index = HISTOSZ - 1;
		histogram[index]++;
	}
	histogramCount = count;
	linearRegression2(dSlope, dIntercept, iMaxCorr, iStartCorr, iNextCorr, dRSquare);

	//diagnostics - probably temporary
	//tmpIntercept = (float)dIntercept;  // b
	//tmpSlope = (float)dSlope;  // m
	//tmpIStartCorr = (float)iStartCorr;
	//tmpINextCorr = (float)iNextCorr;
	//tmpIMaxCorr = (short)iMaxCorr;
	//tmpRSquare = dRSquare;
	dSlope *= 10.0;
	if (dSlope >= 0.0) return;

	for (size_t a = 0;a < count;a++) {
		if (arr[a].score <= 0) break;
		arr[a].eValue = pow(10.0, dSlope * arr[a].score + dIntercept);
		if (arr[a].eValue > 9999) {
			arr[a].eValue = 9999;
			break;
		}
	}

}

void init() {
	//Amino acid masses
	AA['A'] = 71.0371103;
	AA['C'] = 103.0091803;
	AA['D'] = 115.0269385;
	AA['E'] = 129.0425877;
	AA['F'] = 147.0684087;
	AA['G'] = 57.0214611;
	AA['H'] = 137.0589059;
	AA['I'] = 113.0840579;
	AA['K'] = 128.0949557;
	AA['L'] = 113.0840579;
	AA['M'] = 131.0404787;
	AA['N'] = 114.0429222;
	AA['P'] = 97.0527595;
	AA['Q'] = 128.0585714;
	AA['R'] = 156.1011021;
	AA['S'] = 87.0320244;
	AA['T'] = 101.0476736;
	AA['U'] = 150.9536303;
	AA['V'] = 99.0684087;
	AA['W'] = 186.0793065;
	AA['Y'] = 163.0633228;
}

void linearRegression2(double& slope, double& intercept, int& iMaxXcorr, int& iStartXcorr, int& iNextXcorr, double& rSquared) {
	double Sx, Sxy;      // Sum of square distances.
	double Mx, My;       // means
	double dx, dy;
	double b, a;
	double SumX, SumY;   // Sum of X and Y values to calculate mean.
	double SST, SSR;
	double rsq;
	double bestRSQ;
	double bestSlope;
	double bestInt;

	//double dCummulative[HISTOSZ];  // Cummulative frequency at each xcorr value.

	int i;
	int bestNC;
	int bestStart;
	int iNextCorr;    // 2nd best xcorr index
	int iMaxCorr = 0;   // max xcorr index
	int iStartCorr;
	int iNumPoints;

	// Find maximum correlation score index.
	for (i = HISTOSZ - 2; i >= 0; i--) {
		if (histogram[i] > 0)  break;
	}
	iMaxCorr = i;

	//bail now if there is no width to the distribution
	if (iMaxCorr < 3) {
		slope = 0;
		intercept = 0;
		iMaxXcorr = 0;
		iStartXcorr = 0;
		iNextXcorr = 0;
		rSquared = 0;
		return;
	}

	//More aggressive version summing everything below the max
	dCummulative[iMaxCorr - 1] = histogram[iMaxCorr - 1];
	for (i = iMaxCorr - 2; i >= 0; i--) {
		dCummulative[i] = dCummulative[i + 1] + histogram[i];
	}

	//get middle-ish datapoint as seed. Using count/10.
	for (i = 0; i < iMaxCorr; i++) {
		if (dCummulative[i] < histogramCount / 10) break;
	}
	if (i >= (iMaxCorr - 1)) iNextCorr = iMaxCorr - 2;
	else iNextCorr = i;

	// log10...and stomp all over the original...hard to troubleshoot later
	for (i = iMaxCorr - 1; i >= 0; i--) {
		histogram[i] = (int)dCummulative[i];
		dCummulative[i] = log10(dCummulative[i]);
	}

	iStartCorr = iNextCorr - 1;
	iNextCorr++;

	bool bRight = false; // which direction to add datapoint from
	bestRSQ = 0;
	bestNC = 0;
	bestSlope = 0;
	bestInt = 0;
	rsq = Mx = My = a = b = 0.0;
	while (true) {
		Sx = Sxy = SumX = SumY = 0.0;
		iNumPoints = 0;

		// Calculate means.
		for (i = iStartCorr; i <= iNextCorr; i++) {
			if (histogram[i] > 0) {
				SumY += dCummulative[i];
				SumX += i;
				iNumPoints++;
			}
		}
		if (iNumPoints > 0) {
			Mx = SumX / iNumPoints;
			My = SumY / iNumPoints;
		} else {
			Mx = My = 0.0;
		}

		// Calculate sum of squares.
		SST = 0;
		for (i = iStartCorr; i <= iNextCorr; i++) {
			dx = i - Mx;
			dy = dCummulative[i] - My;
			Sx += dx * dx;
			Sxy += dx * dy;
			SST += dy * dy;
		}
		b = Sxy / Sx;
		a = My - b * Mx;  // y-intercept

		//MH: compute R2
		SSR = 0;
		for (i = iStartCorr; i <= iNextCorr; i++) {
			dy = dCummulative[i] - (b * i + a);
			SSR += (dy * dy);
		}
		rsq = 1 - SSR / SST;

		if (rsq > 0.95 || rsq > bestRSQ) {
			if (rsq > bestRSQ || iNextCorr - iStartCorr + 1 < 8) { //keep better RSQ only if more than 8 datapoints, otherwise keep every RSQ below 8 datapoints
				bestRSQ = rsq;
				bestNC = iNextCorr;
				bestSlope = b;
				bestInt = a;
				bestStart = iStartCorr;
			}
			if (bRight) {
				if (iNextCorr < (iMaxCorr - 1)) iNextCorr++;
				else if (iStartCorr > 0) iStartCorr--;
				else break;
			} else {
				if (iStartCorr > 0) iStartCorr--;
				else if (iNextCorr < (iMaxCorr - 1)) iNextCorr++;
				else break;
			}
			bRight = !bRight;
		} else {
			break;
		}
	}

	slope = bestSlope;
	intercept = bestInt;
	iMaxXcorr = iMaxCorr;
	iStartXcorr = bestStart;
	iNextXcorr = bestNC;
	rSquared = bestRSQ;

}

bool sortScores(const sTop& a, const sTop& b) {
	return (a.score > b.score);
}