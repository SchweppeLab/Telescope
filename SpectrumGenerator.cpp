#include "SpectrumGenerator.h"

using namespace std;

SpectrumGenerator::SpectrumGenerator(DBManager* d) {
	dbm = d;
}

SpectrumGenerator::~SpectrumGenerator() {
	dbm = NULL;
}

double SpectrumGenerator::CalcPeptideMass(string peptide) {
	double m = 0;
	for (size_t a = 0;a < peptide.size();a++) {
		m += dbm->aa[peptide[a]];
	}
	m += 18.01056466;
	return m;
}

void SpectrumGenerator::CalcXLIons(FISpectrum& spec, string peptide, int charge, size_t site, double mass) {
	double revMass = CalcPeptideMass(peptide)+mass;
	float total = 0;
	bool bLink = false;

	char zLimit = (char)charge - 1;
	if (zLimit == 0) zLimit = 1;
	if (zLimit > 3) zLimit = 3;

	//b-ions
	double m = 0;
	for (size_t b = 0;b < peptide.size() - 1;b++) {
		m += dbm->aa[peptide[b]];
		revMass -= dbm->aa[peptide[b]];
		if (b == site) {
			bLink = true;
			m += mass;
			revMass -= mass;
		}

		for (char z = 0;z < zLimit;z++) {
			double mz = (m + (z + 1) * PROTON) / (z + 1);
			bool bSkip = false;
			if (z > 0 && mz < 400) bSkip = true;
			//setting m/z limit on ions
			if (mz > minMZ && mz < maxMZ && !bSkip) {
				FIPeak p;
				p.mz = mz;
				p.value = rand() % 100 + 1;
				total += p.value;
				if (echo) {
					cout << "b" << b + 1 << "," << (int)z + 1 << "+\t";
					if (bLink) cout << "*";
					printf("%.6lf\t%d\t%.0f", p.mz, (int)((size_t)(p.mz / 0.02)), p.value);
					if (bLink) cout << "\t" << ((m - mass) + (z + 1) * PROTON) / (z + 1) << "," << (size_t)(((m - mass) + (z + 1) * PROTON) / (z + 1)/0.02);
					cout << endl;
				}
				spec.AddPeak(p);
			}

			mz = (revMass + (z + 1) * PROTON) / (z + 1);
			bSkip = false;
			if (z > 0 && mz < 400) bSkip = true;
			//setting m/z limit on ions
			if (mz > minMZ && mz < maxMZ && !bSkip) {
				FIPeak p;
				p.mz = mz;
				p.value = rand() % 100 + 1;
				total += p.value;
				if (echo) {
					cout << "y" << peptide.size() - b - 1 << "," << (int)z + 1 << "+\t";
					if (!bLink) cout << "*";
					printf("%.6lf\t%d\t%.0f",p.mz,(int)((size_t)(p.mz / 0.02)),p.value);
					if (!bLink) cout << "\t" << ((revMass - mass) + (z + 1) * PROTON) / (z + 1) << "," << (size_t)(((revMass - mass) + (z + 1) * PROTON) / (z + 1) / 0.02);
					cout << endl;
				}
				spec.AddPeak(p);
			}
		}
	}

	if (echo) cout << "Max score for " << peptide << ": " << total << endl;
}

FISpectrum SpectrumGenerator::GenerateSpectrum(string peptide, int z) {
	return MakeSpectrum(peptide,z);
}

FISpectrum SpectrumGenerator::GenerateSpectrum(string alpha, size_t siteA, string beta, size_t siteB, int z, double xlMass) {
	return MakeXLSpectrum(alpha, siteA, beta, siteB, z, xlMass);
}

FISpectrum SpectrumGenerator::MakeSpectrum(string peptide, int charge) {
	double mass = CalcPeptideMass(peptide);

	if (echo) cout << peptide << ", " << charge << "+, " << mass << ", " << (mass + charge * PROTON) / charge << endl;

	FISpectrum spec;
	float total = 0;

	char zLimit = (char)charge - 1;
	if (zLimit == 0) zLimit = 1;
	if (zLimit > 3) zLimit = 3;

	//b-ions
	double m = 0;
	for (size_t b = 0;b < peptide.size() - 1;b++) {
		m += dbm->aa[peptide[b]];

		for (char z = 0;z < zLimit;z++) {
			double mz = (m + (z + 1) * PROTON) / (z + 1);
			if (z > 0 && mz < 400) continue;
			//setting m/z limit on ions
			if (mz>minMZ && mz < maxMZ) {
				FIPeak p;
				p.mz = mz;
				p.value = rand() % 100 + 1;
				total += p.value;
				if (echo) cout << "b" << b + 1 << "," << (int)z+1 << "+\t" << p.mz << "\t" << (size_t)(p.mz / 0.02) << "\t" << p.value << endl;
				spec.AddPeak(p);
			}
		}
	}

	//y-ions
	m = 18.01056466;
	for (size_t b = peptide.size() - 1;b > 0;b--) {
		m += dbm->aa[peptide[b]];

		for (char z = 0;z < zLimit;z++) {
			double mz = (m + (z + 1) * PROTON) / (z + 1);
			if (z > 0 && mz < 400) continue;
			//setting m/z limit on ions
			if (mz > minMZ && mz < maxMZ) {
				FIPeak p;
				p.mz = mz;
				p.value = rand() % 100 + 1;
				total += p.value;
				if (echo) cout << "y" << peptide.size()-b << "," << (int)z+1 << "+\t" << p.mz << "\t" << (size_t)(p.mz/0.02) << "\t" << p.value << endl;
				spec.AddPeak(p);
			}
		}
	}

	if (echo) cout << "Maximum score value: " << total << endl;

	return spec;
}

FISpectrum SpectrumGenerator::MakeXLSpectrum(string alpha, size_t siteA, string beta, size_t siteB, int charge, double xlMass) {
	double massA = CalcPeptideMass(alpha);
	double massB = CalcPeptideMass(beta);
	double totalMass = massA + massB + xlMass;
	if (echo) {
		printf("%s, %.6lf\n",alpha.c_str(),massA);
		printf("%s, %.6lf\n", beta.c_str(), massB);
		printf("%d+, %.6lf, %.4lf\n", charge, totalMass, (totalMass + charge * PROTON) / charge);
	}

	FISpectrum spec;
	CalcXLIons(spec, alpha, charge, siteA, massB + xlMass);
	CalcXLIons(spec, beta, charge, siteB, massA + xlMass);
	return spec;
}