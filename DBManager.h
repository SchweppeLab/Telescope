#ifndef _DBMANAGER_H
#define _DBMANAGER_H

#include "DB.h"
#include "GlobalDefinitions.h"
#include <algorithm>

typedef struct DBMPepIndex {
	unsigned int dbIndex = 0;
	unsigned short start = 0;  //could probably be unsigned int with no hit to performance due to padding.
	char len = 0;
} DBMPepIndex;

typedef struct DBMPepMod {
	double mass = 0;
	std::vector<size_t> maskIndex;   //maps to a modification table
} DBMPepMod;

typedef struct DBMPeptide {
	double mass = 0;
	std::vector<DBMPepIndex> instances;
	std::vector<DBMPepMod> mods;
} DBMPeptide;

typedef struct DBMModDef {
	std::string sites;       //n,c for peptide termini; #,$ for protein termini
	std::string description; //optional
	double mass = 0;
	int maxPerPeptide = 0;
} DBMModDef;

typedef struct DBMStatMad {
	double mass = 0;
	std::string description;
} DBMStatMod;


/// <summary>
/// The class wraps DBaser, a simple FASTA file parser, and manages a list of peptides obtained
/// for database search analysis.
/// </summary>
class DBManager {
public:

	void AddStaticMod(std::string sites, double mass, std::string description = "");
	void AddVariableMod(std::string sites, double mass, int maxPerPeptide, std::string description = "");
	bool CheckStaticMod(char aa, double& mass, std::string& description);
	size_t DigestPeptides(std::string site, std::string except, bool cterm, bool semi);
	std::string GetModDescription(const char& index);
	double GetModMass(const char& index);
	std::string GetPeptideSequence(const size_t& index, int maskIndex=-1);
	char* GetPepSeq(const size_t& index);
	std::string GetProteinName(const size_t& index);
	std::string& GetProteinSeq(const size_t& index);

	std::string& ModMask(const int& index);
	DBMPeptide& Peptide(const size_t& index);

	bool ReadFASTA(const char* fn);
	bool ReadFASTA(std::string fn);

	void SetMaxModsPerPeptide(int i);

	size_t SizePeptide();
	size_t SizeProtein();

	//I'm of two minds regarding this array. It holds the amino acid masses. It is therefore
	//a convenient set of values to quickly reference publicly. But it also means it they can be changed
	//at the wrong times, so consider making private with an accessor.
	double aa[128] = { 0 };
	int maxPepLen = 40;
	int maxMC = 1;
	double minPepMass = 600.0;
	double maxPepMass = 4000.0;
	int minPepLen = 6;
	int maxMods = 3;
	size_t totalPeptidoforms = 0;

protected:
private:

	void AddPeptide(std::string& pep, double mass, size_t dbIndex, size_t start, size_t end, bool hasMod);
	void AddPeptideMod(std::string& pep, size_t pepIndex, double mass, double modMass, size_t dbIndex, size_t start, size_t end, std::string mStr, size_t mCount, size_t startAA);
	void BuildModSet();
	void Init();

	db_ns::DB db;
	
	std::map<std::string, size_t> mPeptide; //map makes looking up peptide sequences fast.
	std::map<std::string, size_t> mMod; //map makes looking up peptide sequences fast.
	std::map<std::string, size_t>::iterator it;
	std::vector<DBMPeptide> peptides;     //all the peptides in the database
	std::vector<std::string> modMask;
	std::vector<double> modMass;

	DBMStatMad staticMods[128];
	std::vector<DBMModDef> varMods;
	int* varModCount;
	std::vector<size_t> aaMods[128];  //each amino acid site has its own set of indexes to possible mods.

	size_t modCount = 0;

	static bool sortMass(const DBMPeptide& a, const DBMPeptide& b);

};


#endif