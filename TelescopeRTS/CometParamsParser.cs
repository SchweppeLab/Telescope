using System;
using System.Collections.Generic;
using System.Drawing.Printing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CometWrapper;

namespace TelescopeRTS
{
  public class DoubleRange : DoubleRangeWrapper
  {
    //pu
  }
  public class IntRange : IntRangeWrapper { }

  internal class CometParamsParser : ParamsParser
  {
    //0 = int, 1 = double, 2=string,3=var mod special case, 4=int int, 5=double double
    Dictionary<string, int> cometParams = new Dictionary<string, int>();

    public CometParamsParser()
    {
      cometParams.Add("add_Cterm_peptide", 1);
      cometParams.Add("add_Cterm_protein", 1);
      cometParams.Add("add_Nterm_peptide", 1);
      cometParams.Add("add_Nterm_protein", 1);
      cometParams.Add("add_A_alanine", 1);
      cometParams.Add("add_B_user_amino_acid", 1);
      cometParams.Add("add_C_cysteine", 1);
      cometParams.Add("add_D_aspartic_acid", 1);
      cometParams.Add("add_E_glutamic_acid", 1);
      cometParams.Add("add_F_phenylalanine", 1);
      cometParams.Add("add_G_glycine", 1);
      cometParams.Add("add_H_histidine", 1);
      cometParams.Add("add_I_isoleucine", 1);
      cometParams.Add("add_J_user_amino_acid", 1);
      cometParams.Add("add_K_lysine", 1);
      cometParams.Add("add_L_leucine", 1);
      cometParams.Add("add_M_methionine", 1);
      cometParams.Add("add_N_asparagine", 1);
      cometParams.Add("add_O_pyrrolysine", 1);
      cometParams.Add("add_P_proline", 1);
      cometParams.Add("add_Q_glutamine", 1);
      cometParams.Add("add_R_arginine", 1);
      cometParams.Add("add_S_serine", 1);
      cometParams.Add("add_T_threonine", 1);
      cometParams.Add("add_U_selenocysteine", 1);
      cometParams.Add("add_V_valine", 1);
      cometParams.Add("add_W_tryptophan", 1);
      cometParams.Add("add_X_user_amino_acid", 1);
      cometParams.Add("add_Y_tyrosine", 1);
      cometParams.Add("add_Z_user_amino_acid", 1);
      cometParams.Add("allowed_missed_cleavage", 0);
      cometParams.Add("clear_mz_range", 5);
      cometParams.Add("clip_nterm_methionine", 0);
      cometParams.Add("database_name", 2);
      cometParams.Add("decoy_prefix", 2);
      cometParams.Add("decoy_search", 0);
      cometParams.Add("digest_mass_range", 5);
      cometParams.Add("equal_I_and_L", 0);
      cometParams.Add("fragindex_max_fragmentmass", 1);
      cometParams.Add("fragindex_min_fragmentmass", 1);
      cometParams.Add("fragindex_min_ions_report", 0);
      cometParams.Add("fragindex_min_ions_score", 0);
      cometParams.Add("fragindex_num_spectrumpeaks", 0);
      cometParams.Add("fragindex_skipreadprecursors", 0);
      cometParams.Add("fragment_bin_offset", 1);
      cometParams.Add("fragment_bin_tol", 1);
      cometParams.Add("isotope_error", 0);
      cometParams.Add("max_duplicate_proteins", 0);
      cometParams.Add("max_fragment_charge", 0);
      cometParams.Add("max_precursor_charge", 0);
      cometParams.Add("max_variable_mods_in_peptide", 0);
      cometParams.Add("min_precursor_charge", 0);
      cometParams.Add("minimum_intensity", 1);
      cometParams.Add("minimum_peaks", 0);
      cometParams.Add("ms_level", 0);
      cometParams.Add("num_output_lines", 0);
      cometParams.Add("num_enzyme_termini", 0);
      cometParams.Add("num_threads", 0);
      cometParams.Add("output_mzidentmlfile", 0);
      cometParams.Add("output_pepxmlfile", 0);
      cometParams.Add("output_percolatorfile", 0);
      cometParams.Add("output_sqtfile", 0);
      cometParams.Add("output_txtfile", 0);
      cometParams.Add("override_charge", 0);
      cometParams.Add("peptide_mass_tolerance_lower", 1);
      cometParams.Add("peptide_mass_tolerance_upper", 1);
      cometParams.Add("peptide_mass_units", 0);
      cometParams.Add("peptide_length_range", 4);
      cometParams.Add("percentage_base_peak", 1);
      cometParams.Add("precursor_charge", 4);
      cometParams.Add("precursor_tolerance_type", 0);
      cometParams.Add("remove_precursor_peak", 0);
      cometParams.Add("require_variable_mod", 0);
      cometParams.Add("sample_enzyme_number", 0);
      cometParams.Add("search_enzyme_number", 0);
      cometParams.Add("search_enzyme2_number", 0);
      cometParams.Add("spectrum_batch_size", 0);
      cometParams.Add("theoretical_fragment_ions", 0);
      cometParams.Add("use_A_ions", 0);
      cometParams.Add("use_B_ions", 0);
      cometParams.Add("use_C_ions", 0);
      cometParams.Add("use_NL_ions", 0);
      cometParams.Add("use_X_ions", 0);
      cometParams.Add("use_Y_ions", 0);
      cometParams.Add("use_Z_ions", 0);
      cometParams.Add("use_Z1_ions", 0);
      cometParams.Add("variable_mod01", 3);
      cometParams.Add("variable_mod02", 3);
      cometParams.Add("variable_mod03", 3);
      cometParams.Add("variable_mod04", 3);
      cometParams.Add("variable_mod05", 3);
      cometParams.Add("variable_mod06", 3);
      cometParams.Add("variable_mod07", 3);
      cometParams.Add("variable_mod08", 3);
      cometParams.Add("variable_mod09", 3);
      cometParams.Add("variable_mod10", 3);
      cometParams.Add("variable_mod11", 3);
      cometParams.Add("variable_mod12", 3);
      cometParams.Add("variable_mod13", 3);
      cometParams.Add("variable_mod14", 3);
      cometParams.Add("variable_mod15", 3);
    }

    public bool SetEnzyme(CometSearchManagerWrapper comet, string s)
    {
      string[] v = s.Split(' ');
      if (v.Length == 5)
      {
        var enzymeInfo = new EnzymeInfoWrapper();
        enzymeInfo.set_SearchEnzymeName(v[0]); //Trypsin
        enzymeInfo.set_SearchEnzymeBreakAA(v[1]); //KR
        enzymeInfo.set_SearchEnzymeNoBreakAA(v[2]); //P
        enzymeInfo.set_SearchEnzymeOffSet(int.Parse(v[3])); //1 (c-term)
        enzymeInfo.set_AllowedMissedCleavge(int.Parse(v[4])); //1
        return comet.SetParam("[COMET_ENZYME_INFO]", s, enzymeInfo);
      }
      return false;
    }

    public bool SetParam(CometSearchManagerWrapper comet, ParamTuple param)
    {
      int pType;
      int iVal;
      int iVal2;
      double dVal;
      double dVal2;
      string[] v;

      if (!cometParams.TryGetValue(param.Name, out pType)) pType = -1;

      switch (pType)
      {
        case 0:
          if (int.TryParse(param.Value, out iVal))
          {
            return comet.SetParam(param.Name, param.Value, iVal);
          }
          break;
        case 1:
          if (double.TryParse(param.Value, out dVal))
          {
            return comet.SetParam(param.Name, param.Value, dVal);
          }
          break;
        case 2:
          return comet.SetParam(param.Name, param.Value, param.Value);
        case 3:
          v = param.Value.Split(' ');
          if (v.Length == 8)
          {
            var varModsWrapper = new VarModsWrapper();
            varModsWrapper.set_VarModMass(double.Parse(v[0]));
            varModsWrapper.set_VarModChar(v[1]);
            varModsWrapper.set_BinaryMod(int.Parse(v[2]));
            varModsWrapper.set_MaxNumVarModAAPerMod(int.Parse(v[3]));
            varModsWrapper.set_VarModTermDistance(int.Parse(v[4]));
            varModsWrapper.set_WhichTerm(int.Parse(v[5]));
            varModsWrapper.set_RequireThisMod(int.Parse(v[6]));
            varModsWrapper.set_VarNeutralLoss(double.Parse(v[7]));
            return comet.SetParam(param.Name, param.Value, varModsWrapper);
          }
          break;
        case 4:
          v = param.Value.Split(' ');
          if (v.Length == 2)
          {
            if (Int32.TryParse(v[0], out iVal) && Int32.TryParse(v[1], out iVal2))
            {
              IntRange intRange = new IntRange();
              intRange.set_iStart(iVal);
              intRange.set_iEnd(iVal2);
              return comet.SetParam(param.Name, param.Value, intRange);
            }
          }
          break;
        case 5:
          v = param.Value.Split(' ');
          if (v.Length == 2)
          {
            if (double.TryParse(v[0], out dVal) && double.TryParse(v[1], out dVal2))
            {
              DoubleRange doubleRange = new DoubleRange();
              doubleRange.set_dStart(dVal);
              doubleRange.set_dEnd(dVal2);
              return comet.SetParam(param.Name, param.Value, doubleRange);
            }
          }
          break;
        default:
          //ErrorLog.Warn("CometParamsParser.SetParam(): '" + param.Name + "' not recognized.");
          break;
      }

      //Malformed parameter if we reach here
      return false;
    }

  }
}
