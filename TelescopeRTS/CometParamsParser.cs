using System;
using System.Collections.Generic;
using System.Drawing.Printing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CometWrapper;

namespace TelescopeRTS
{
  // Thin wrappers so DoubleRangeWrapper/IntRangeWrapper (from CometWrapper) can be
  // constructed directly as TelescopeRTS types.
  public class DoubleRange : DoubleRangeWrapper { }
  public class IntRange : IntRangeWrapper { }

  // Translates a Comet .params file into calls on CometSearchManagerWrapper.SetParam(),
  // using a lookup table of each known parameter's value type (see paramValueTypes).
  internal class CometParamsParser : ParamsParser
  {
    // Maps each recognized Comet parameter name to how its value should be parsed:
    // 0 = int, 1 = double, 2 = string, 3 = variable-mod special case (8 fields),
    // 4 = int-int range, 5 = double-double range.
    Dictionary<string, int> paramValueTypes = new Dictionary<string, int>();

    public CometParamsParser()
    {
      paramValueTypes.Add("add_Cterm_peptide", 1);
      paramValueTypes.Add("add_Cterm_protein", 1);
      paramValueTypes.Add("add_Nterm_peptide", 1);
      paramValueTypes.Add("add_Nterm_protein", 1);
      paramValueTypes.Add("add_A_alanine", 1);
      paramValueTypes.Add("add_B_user_amino_acid", 1);
      paramValueTypes.Add("add_C_cysteine", 1);
      paramValueTypes.Add("add_D_aspartic_acid", 1);
      paramValueTypes.Add("add_E_glutamic_acid", 1);
      paramValueTypes.Add("add_F_phenylalanine", 1);
      paramValueTypes.Add("add_G_glycine", 1);
      paramValueTypes.Add("add_H_histidine", 1);
      paramValueTypes.Add("add_I_isoleucine", 1);
      paramValueTypes.Add("add_J_user_amino_acid", 1);
      paramValueTypes.Add("add_K_lysine", 1);
      paramValueTypes.Add("add_L_leucine", 1);
      paramValueTypes.Add("add_M_methionine", 1);
      paramValueTypes.Add("add_N_asparagine", 1);
      paramValueTypes.Add("add_O_pyrrolysine", 1);
      paramValueTypes.Add("add_P_proline", 1);
      paramValueTypes.Add("add_Q_glutamine", 1);
      paramValueTypes.Add("add_R_arginine", 1);
      paramValueTypes.Add("add_S_serine", 1);
      paramValueTypes.Add("add_T_threonine", 1);
      paramValueTypes.Add("add_U_selenocysteine", 1);
      paramValueTypes.Add("add_V_valine", 1);
      paramValueTypes.Add("add_W_tryptophan", 1);
      paramValueTypes.Add("add_X_user_amino_acid", 1);
      paramValueTypes.Add("add_Y_tyrosine", 1);
      paramValueTypes.Add("add_Z_user_amino_acid", 1);
      paramValueTypes.Add("allowed_missed_cleavage", 0);
      paramValueTypes.Add("clear_mz_range", 5);
      paramValueTypes.Add("clip_nterm_methionine", 0);
      paramValueTypes.Add("database_name", 2);
      paramValueTypes.Add("decoy_prefix", 2);
      paramValueTypes.Add("decoy_search", 0);
      paramValueTypes.Add("digest_mass_range", 5);
      paramValueTypes.Add("equal_I_and_L", 0);
      paramValueTypes.Add("fragindex_max_fragmentmass", 1);
      paramValueTypes.Add("fragindex_min_fragmentmass", 1);
      paramValueTypes.Add("fragindex_min_ions_report", 0);
      paramValueTypes.Add("fragindex_min_ions_score", 0);
      paramValueTypes.Add("fragindex_num_spectrumpeaks", 0);
      paramValueTypes.Add("fragindex_skipreadprecursors", 0);
      paramValueTypes.Add("fragment_bin_offset", 1);
      paramValueTypes.Add("fragment_bin_tol", 1);
      paramValueTypes.Add("isotope_error", 0);
      paramValueTypes.Add("max_duplicate_proteins", 0);
      paramValueTypes.Add("max_fragment_charge", 0);
      paramValueTypes.Add("max_precursor_charge", 0);
      paramValueTypes.Add("max_variable_mods_in_peptide", 0);
      paramValueTypes.Add("min_precursor_charge", 0);
      paramValueTypes.Add("minimum_intensity", 1);
      paramValueTypes.Add("minimum_peaks", 0);
      paramValueTypes.Add("ms_level", 0);
      paramValueTypes.Add("num_output_lines", 0);
      paramValueTypes.Add("num_enzyme_termini", 0);
      paramValueTypes.Add("num_threads", 0);
      paramValueTypes.Add("output_mzidentmlfile", 0);
      paramValueTypes.Add("output_pepxmlfile", 0);
      paramValueTypes.Add("output_percolatorfile", 0);
      paramValueTypes.Add("output_sqtfile", 0);
      paramValueTypes.Add("output_txtfile", 0);
      paramValueTypes.Add("override_charge", 0);
      paramValueTypes.Add("peptide_mass_tolerance_lower", 1);
      paramValueTypes.Add("peptide_mass_tolerance_upper", 1);
      paramValueTypes.Add("peptide_mass_units", 0);
      paramValueTypes.Add("peptide_length_range", 4);
      paramValueTypes.Add("percentage_base_peak", 1);
      paramValueTypes.Add("precursor_charge", 4);
      paramValueTypes.Add("precursor_tolerance_type", 0);
      paramValueTypes.Add("remove_precursor_peak", 0);
      paramValueTypes.Add("require_variable_mod", 0);
      paramValueTypes.Add("sample_enzyme_number", 0);
      paramValueTypes.Add("search_enzyme_number", 0);
      paramValueTypes.Add("search_enzyme2_number", 0);
      paramValueTypes.Add("spectrum_batch_size", 0);
      paramValueTypes.Add("theoretical_fragment_ions", 0);
      paramValueTypes.Add("use_A_ions", 0);
      paramValueTypes.Add("use_B_ions", 0);
      paramValueTypes.Add("use_C_ions", 0);
      paramValueTypes.Add("use_NL_ions", 0);
      paramValueTypes.Add("use_X_ions", 0);
      paramValueTypes.Add("use_Y_ions", 0);
      paramValueTypes.Add("use_Z_ions", 0);
      paramValueTypes.Add("use_Z1_ions", 0);
      paramValueTypes.Add("variable_mod01", 3);
      paramValueTypes.Add("variable_mod02", 3);
      paramValueTypes.Add("variable_mod03", 3);
      paramValueTypes.Add("variable_mod04", 3);
      paramValueTypes.Add("variable_mod05", 3);
      paramValueTypes.Add("variable_mod06", 3);
      paramValueTypes.Add("variable_mod07", 3);
      paramValueTypes.Add("variable_mod08", 3);
      paramValueTypes.Add("variable_mod09", 3);
      paramValueTypes.Add("variable_mod10", 3);
      paramValueTypes.Add("variable_mod11", 3);
      paramValueTypes.Add("variable_mod12", 3);
      paramValueTypes.Add("variable_mod13", 3);
      paramValueTypes.Add("variable_mod14", 3);
      paramValueTypes.Add("variable_mod15", 3);
    }

    // Configures Comet's enzyme settings from a "Name BreakAA NoBreakAA Offset MissedCleavages" string.
    public bool SetEnzyme(CometSearchManagerWrapper comet, string enzymeSpec)
    {
      string[] parts = enzymeSpec.Split(' ');
      if (parts.Length == 5)
      {
        var enzymeInfo = new EnzymeInfoWrapper();
        enzymeInfo.set_SearchEnzymeName(parts[0]); //Trypsin
        enzymeInfo.set_SearchEnzymeBreakAA(parts[1]); //KR
        enzymeInfo.set_SearchEnzymeNoBreakAA(parts[2]); //P
        enzymeInfo.set_SearchEnzymeOffSet(int.Parse(parts[3])); //1 (c-term)
        enzymeInfo.set_AllowedMissedCleavge(int.Parse(parts[4])); //1
        return comet.SetParam("[COMET_ENZYME_INFO]", enzymeSpec, enzymeInfo);
      }
      return false;
    }

    // Looks up the given parameter's expected type in paramValueTypes, parses its value
    // accordingly, and forwards it to CometSearchManagerWrapper.SetParam().
    public bool SetParam(CometSearchManagerWrapper comet, ParamTuple param)
    {
      int paramType;
      int intValue;
      int intValue2;
      double doubleValue;
      double doubleValue2;
      string[] parts;

      if (!paramValueTypes.TryGetValue(param.Name, out paramType)) paramType = -1;

      switch (paramType)
      {
        case 0:
          if (int.TryParse(param.Value, out intValue))
          {
            return comet.SetParam(param.Name, param.Value, intValue);
          }
          break;
        case 1:
          if (double.TryParse(param.Value, out doubleValue))
          {
            return comet.SetParam(param.Name, param.Value, doubleValue);
          }
          break;
        case 2:
          return comet.SetParam(param.Name, param.Value, param.Value);
        case 3:
          parts = param.Value.Split(' ');
          if (parts.Length == 8)
          {
            var varModsWrapper = new VarModsWrapper();
            varModsWrapper.set_VarModMass(double.Parse(parts[0]));
            varModsWrapper.set_VarModChar(parts[1]);
            varModsWrapper.set_BinaryMod(int.Parse(parts[2]));
            varModsWrapper.set_MaxNumVarModAAPerMod(int.Parse(parts[3]));
            varModsWrapper.set_VarModTermDistance(int.Parse(parts[4]));
            varModsWrapper.set_WhichTerm(int.Parse(parts[5]));
            varModsWrapper.set_RequireThisMod(int.Parse(parts[6]));
            varModsWrapper.set_VarNeutralLoss(double.Parse(parts[7]));
            return comet.SetParam(param.Name, param.Value, varModsWrapper);
          }
          break;
        case 4:
          parts = param.Value.Split(' ');
          if (parts.Length == 2)
          {
            if (Int32.TryParse(parts[0], out intValue) && Int32.TryParse(parts[1], out intValue2))
            {
              IntRange intRange = new IntRange();
              intRange.set_iStart(intValue);
              intRange.set_iEnd(intValue2);
              return comet.SetParam(param.Name, param.Value, intRange);
            }
          }
          break;
        case 5:
          parts = param.Value.Split(' ');
          if (parts.Length == 2)
          {
            if (double.TryParse(parts[0], out doubleValue) && double.TryParse(parts[1], out doubleValue2))
            {
              DoubleRange doubleRange = new DoubleRange();
              doubleRange.set_dStart(doubleValue);
              doubleRange.set_dEnd(doubleValue2);
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
