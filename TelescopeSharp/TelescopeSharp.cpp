#include "TelescopeSharp.h"

using namespace TelescopeSharp;

Telescope::Telescope() {
	dbm = new DBManager();
	params = new ParamsManager();
	fim = new FIManager();
	mm = new FIMemoryManager();
	xcorr = new FastXCorr();
}

Telescope::~Telescope() {
	delete dbm;
	delete params;
	delete fim;
	delete mm;
	delete xcorr;
}

double Telescope::GetMemUse(bool peptidoform) {
	long long bytes = 0;
	if (peptidoform) {
		bytes = dbm->SizePeptide() * sizeof(DBMPeptide);
		for (size_t a = 0;a < dbm->SizePeptide();a++) {
			bytes += dbm->Peptide(a).instances.size() * sizeof(DBMPepIndex);
			bytes += dbm->Peptide(a).mods.size() * sizeof(DBMPepMod);
			for (size_t b = 0;b < dbm->Peptide(a).mods.size();b++) {
				bytes += dbm->Peptide(a).mods[b].maskIndex.size() * sizeof(size_t);
			}
		}
	} else {
		bytes = fim->memUse;
	}
	return (double)bytes / 1073741824;
}

int Telescope::GetPeptidoformCount() {
	return dbm->totalPeptidoforms;
}

bool Telescope::Init(String^ pFile) {
	std::string s = marshal_as<std::string>(pFile);
	bool ret = params->ReadParams(s);
	if (!ret) return false;

	fim->Initialize(dbm, params);
	ret = ProcessDB();
	if (!ret) return false;

	xcorr->Initialize(params);

	fim->GeneratePeptideMap();
	fim->GenerateIndex();

	size_t indexA = 0;
	size_t indexB = 0;
	maxScoreCount = 0;
	for (size_t a = 0;a < fim->fii.SizePeptide();a++) {
		double mass = fim->fii.peptides[a].mass;
		double mErr = mass / 1e6 * (params->ppm*2);
		double min = mass - mErr;
		double max = mass + mErr;
		while (indexA < a && fim->fii.peptides[indexA].mass < min) indexA++;
		if (indexB < a) indexB = a;
		while (indexB < fim->fii.pepArrSz && fim->fii.peptides[indexB].mass < max) indexB++;
		if ((indexB - indexA) > maxScoreCount) {
			maxScoreCount = indexB - indexA + 1;
		}
	}
	maxScoreCount += 10; //Add a cushion
	mm->AllocateScores(params->threads, (size_t)maxScoreCount);

	return true;
}

bool Telescope::ProcessDB() {

	//Set parameters - TODO: Pass parameters object
	dbm->minPepMass = params->minPepMass;
	dbm->maxPepMass = params->maxPepMass;
	dbm->minPepLen = params->minPepLen;
	dbm->maxPepLen = params->maxPepLen;
	dbm->maxMC = params->maxMC;
	dbm->maxMods = params->maxMods;
	for (size_t a = 0;a < params->mods.size();a++) {
		if (params->mods[a].variable) {
			dbm->AddVariableMod(params->mods[a].sites, params->mods[a].mass, params->mods[a].maxPerPeptide, params->mods[a].description);
		} else {
			dbm->AddStaticMod(params->mods[a].sites, params->mods[a].mass, params->mods[a].description);
		}
	}

	//Read the fasta file
	if (!dbm->ReadFASTA(params->fastaFile)) return false;

	//Digest the database to find all peptides
	dbm->DigestPeptides("KR", "P", true, params->semiEnzyme);

	return true;
}

TScore^ Telescope::Search(int thread, array<double>^ mz, array<double>^ intensity, double preMz, int preZ) {
	FIPrecursor p;
	FISpectrum s;

	double invBinSize = 1 / params->binSize;

	//Process precursor
	int charge = preZ;
	if (charge == 0) charge = 3;
	double mass = preMz * charge - (charge * PROTON);
	if (mass + PROTON<params->minPepMass || mass + PROTON>params->maxPepMass) goto FORMATSCORE; //M+H to match Comet...

	//The peptide mass tolerance boundaries here replicate how Comet computes them (which is M+H space...)
	double mzErr = preMz / 1e6 * params->ppm;
	double mzMin = preMz - mzErr;
	double mzMax = preMz + mzErr;
	double min = mzMin * charge - (charge * PROTON);
	double max = mzMax * charge - (charge * PROTON);
	size_t index = fim->fii.FindPeptideIndex(min); //should return the first index below the desired mass
	while (fim->fii.peptides[index].mass < min) index++;

	p.charge = charge;
	p.mass = mass;
	p.peptide = "";
	p.pepOffset = index;
	s.precursor.push_back(p);
	s.precursor.back().ts.Init(params->psmCount);
	int count = 0;
	while (index < fim->fii.pepArrSz && fim->fii.peptides[index++].mass < max) count++;
	s.precursor.back().scoreCount = count;

	//Process spectrum peaks
	s.Allocate(mz->Length);
	for (int a = 0;a < mz->Length;a++) {
		if (mz[a] > params->maxMZ) break; //assuming mz values are in order from low to high.
		FIPeak p;
		p.mz = mz[a];
		p.value = (float)intensity[a];
		p.fIndex = (size_t)(p.mz * invBinSize + 1);
		s.AddPeak(p);
	}

	//Xcorr, if requested
	if(params->xcorr)	xcorr->ProcessSpectrum(s);

	//Do search
	fim->fii.ScoreSpectrum(s, mm->scores[thread]);


FORMATSCORE:

	TScore^ t = gcnew TScore();
	if (s.precursor.size() == 0) return t;
	if (s.precursor[0].ts[0].score > 0) {
		size_t index = fim->fii.peptides[s.precursor[0].ts[0].index].peptideIndex;
		size_t mIndex = fim->fii.peptides[s.precursor[0].ts[0].index].maskIndex;
		t->peptide = marshal_as<String^>(dbm->GetPeptideSequence(index, mIndex));
		t->protein = marshal_as<String^>(dbm->GetProteinNameFromPeptideIndex(index));
		t->score = s.precursor[0].ts[0].score;
	}
	return t;

}