#include "ResultsExporter.h"

using namespace std;

ResultsExporter::ResultsExporter() {
}

ResultsExporter::~ResultsExporter() {
  dbm = nullptr;
  fii = nullptr;
  params = nullptr;
}

CnpxModificationInfo ResultsExporter::CreateModificationInfo(const std::string& peptide, const int& modIndex, const int& maskIndex) {
  //modifications
  //if there are modifications, set up an array that has the additional masses at each position
  char* mods = new char[peptide.size() + 2];
  for (size_t b = 0;b < peptide.size();b++) mods[b] = -1;
  if (modIndex > -1) {
    string mask = dbm->ModMask(maskIndex);
    for (size_t c = 0;c < mask.size();c += 2) {
      //TODO: process the position for the special cases of n- and c-termini
      mods[mask[c]] = mask[c + 1];
    }
  }

  string modPep;
  string stID;
  double stMass;
  CnpxModificationInfo mi;
  bool hasMod = false;
  for (size_t b = 0;b < peptide.size();b++) {
    modPep += peptide[b];
    
    //Static mods
    if (dbm->CheckStaticMod(peptide[b], stMass, stID)) {
      hasMod = true;
      CnpxModAminoAcidMass maam;
      maam.position = (int)b + 1;
      maam.staticMass = stMass;
      maam.mass = dbm->aa[peptide[b]];
      mi.mod_aminoacid_mass.push_back(maam);
    }

    //Var mods
    if (mods[b] > -1) {
      hasMod = true;
      CnpxModAminoAcidMass maam;
      maam.position = (int)b + 1;
      maam.variable = dbm->GetModMass(mods[b]);
      maam.mass = dbm->aa[peptide[b]] + maam.variable;
      modPep += "[" + to_string((int)(maam.mass + 0.5)) + "]";
      mi.mod_aminoacid_mass.push_back(maam);
    }
  }
  if (hasMod) mi.modified_peptide = modPep;
  delete[] mods;

  return mi;
}

CnpxSearchHit ResultsExporter::CreateSearchHit(const ScoreStruct& ss) {
  unsigned int pepIndex = fii->peptides[ss.index].peptideIndex; 
  int modIndex = fii->peptides[ss.index].modIndex;
  int maskIndex = fii->peptides[ss.index].maskIndex;
  
  CnpxSearchHit sh;
  sh.calc_neutral_pep_mass = fii->peptides[ss.index].mass;
  sh.peptide = dbm->GetPeptideSequence(pepIndex);

  //Add scores
  sh.addSearchScore("xcorr", to_string(ss.score));

  //Protein information
  sh.num_tot_proteins = (int)dbm->Peptide(pepIndex).instances.size();
  for (size_t b = 0;b < dbm->Peptide(pepIndex).instances.size();b++) {
    DBMPepIndex* pi = &dbm->Peptide(pepIndex).instances[b];
    string seq = dbm->GetProteinSeq(pi->dbIndex);
    if (b == 0) {
      sh.protein = dbm->GetProteinName(pi->dbIndex);
      sh.peptide_start_pos = (int)pi->start + 1;
      if (sh.peptide_start_pos > 1) sh.peptide_prev_aa = seq[pi->start - 1];
      else sh.peptide_prev_aa = "-";
      if (pi->start + pi->len < seq.size() - 1) sh.peptide_next_aa = seq[pi->start + pi->len];
      else sh.peptide_next_aa = "-";
    } else {
      sh.alternative_protein.emplace_back();
      sh.alternative_protein.back().protein = dbm->GetProteinName(pi->dbIndex);
      sh.alternative_protein.back().peptide_start_pos = (int)pi->start + 1;
      if (sh.peptide_start_pos > 1) sh.alternative_protein.back().peptide_prev_aa = seq[pi->start - 1];
      else sh.alternative_protein.back().peptide_prev_aa = "-";
      if (pi->start + pi->len < seq.size() - 1) sh.alternative_protein.back().peptide_next_aa = seq[pi->start + pi->len];
      else sh.alternative_protein.back().peptide_next_aa = "-";
    }
  }

  //Modification info
  CnpxModificationInfo mi = CreateModificationInfo(sh.peptide,modIndex,maskIndex);
  if (mi.mod_aminoacid_mass.size() > 0) sh.modification_info.push_back(mi);

  return sh;
}

CnpxSpectrumQuery ResultsExporter::CreateSpectrumQuery(const FISpectrum& spec) {
  CnpxSpectrumQuery sq;
  sq.spectrum = to_string(spec.scanNumber);
  sq.start_scan = spec.scanNumber;
  sq.end_scan = spec.scanNumber;
  sq.precursor_neutral_mass = spec.precursor[0].mass;
  sq.assumed_charge = spec.precursor[0].charge;
  sq.index = 1;
  sq.retention_time_sec = 0;

  return sq;
}

void ResultsExporter::Initialize(DBManager* d, FragmentIonIndex* f, ParamsManager* p) {
  dbm = d;
  fii = f;
  params = p;
}

bool ResultsExporter::Write(const std::string& fn, DataLoader& scans) {

	NeoPepXMLParser p;

  CnpxMSMSPipelineAnalysis pa;
  char timebuf[80];
  time_t timeNow;
  time(&timeNow);
  strftime(timebuf, 80, "%Y-%m-%dT%H:%M:%S", localtime(&timeNow));
  pa.date.parseDateTime(timebuf);

  CnpxMSMSRunSummary rs;
  rs.base_name = "somedata.mzML";
  rs.base_name = rs.base_name.substr(0, rs.base_name.find_last_of('.'));
  string outFile = "results";
  if (rs.base_name[0] == '/') { //unix
    outFile = outFile.substr(outFile.find_last_of("/") + 1, outFile.size());
  } else { //assuming windows
    outFile = outFile.substr(outFile.find_last_of("\\") + 1, outFile.size());
  }
  rs.raw_data = "somedata";
  rs.raw_data_type = "raw";

  //Add the enzyme
  CnpxSampleEnzyme se;
  se.name = "trypsin";
  CnpxSpecificity ses;
  ses.cut = "KR";
  ses.no_cut = "P";
  ses.sense = "C";
  se.specificity.push_back(ses);
  rs.sample_enzyme.push_back(se);

  CnpxSearchSummary ss;
  ss.search_engine = "Telescope";
  ss.base_name = rs.base_name;
  ss.search_engine_version = "Unstoppable!!!";
  ss.precursor_mass_type = "monoisotopic";
  ss.fragment_mass_type = "monoisotopic";
  ss.search_id = 1;


  //processPath(params->dbFile, outPath);
  string outPath = "somewhere";

  CnpxSearchDatabase sd;
  sd.local_path = outPath;
  sd.type = "AA";
  ss.search_database.push_back(sd);

  CnpxEnzymaticSearchConstraint esc;
  esc.enzyme = "trypsin";
  esc.max_num_internal_cleavages = params->maxMC;
  esc.min_number_termini = 2;
  ss.enzymatic_search_constraint.push_back(esc);

  //TODO: Add modifications & params


  rs.search_summary.push_back(ss);
  pa.msms_run_summary.push_back(rs);

  string pepXML_fileName = fn+".pep.xml";
  pa.summary_xml = pepXML_fileName;
  p.msms_pipeline_analysis.push_back(pa);


  //Iterate over all spectra
  for (size_t a = 0;a < scans.Size();a++) {

    //Create spectrum entry
    CnpxSpectrumQuery sq = CreateSpectrumQuery(scans[a]);

    //Iterate over all top scores for this spectrum
    for (size_t b = 0;b < scans[a].precursor[0].ts.Size();b++) {
      if (scans[a].precursor[0].ts[b].score <= 0) break;

      //If there is at least one results, create search_result element in parent spectrum_query element
      if(b==0) sq.search_result.emplace_back();

      //Create search_hit element for this spectrum, update the mass difference and rank.
      CnpxSearchHit sh = CreateSearchHit(scans[a].precursor[0].ts[b]);
      sh.massdiff = sh.calc_neutral_pep_mass - sq.precursor_neutral_mass;
      if (b == 0) sh.hit_rank = 1;
      else {
        if (scans[a].precursor[0].ts[b].score < scans[a].precursor[0].ts[b - 1].score) sh.hit_rank = sq.search_result.back().search_hit.back().hit_rank + 1;
        else sh.hit_rank = sq.search_result.back().search_hit.back().hit_rank;
      }

      //Add search_hit element to parent search_result element
      sq.search_result.back().search_hit.push_back(sh);
    }

    //Add spectrum_query element to the msms_run_summary element
    sq.index = (int)p.msms_pipeline_analysis[0].msms_run_summary[0].spectrum_query.size() + 1;
    p.msms_pipeline_analysis[0].msms_run_summary[0].spectrum_query.push_back(sq);
  }

  //Write the entire xml to file.
  p.write(fn.c_str(), true);

	return true;
}