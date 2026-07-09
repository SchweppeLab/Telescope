using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TelescopeRTS
{
  internal class ParamTuple
  {
    public string Name { get; set; }
    public string Value { get; set; }
  }

  internal class ParamsParser
  {

    public List<ParamTuple> paramTuples = new List<ParamTuple>();

    public ParamsParser() { }

    private ParamTuple ParseLine(string line)
    {
      string name = string.Empty;
      string value = string.Empty;
      bool left = true;
      for (int i = 0; i < line.Length; i++)
      {
        if (line[i] == '#') break;
        else if (line[i] == '=') left = false;
        else
        {
          if (left) name += line[i];
          else value += line[i];
        }
      }

      name = name.Trim();
      value = value.Trim();

      if (name.Length > 0 && value.Length > 0)
      {
        ParamTuple p = new ParamTuple();
        p.Name = name;
        p.Value = value;
        return p;
      }
      return null;
    }

    public bool ReadFile(string path)
    {
      paramTuples.Clear();
      try
      {
        using (StreamReader reader = new StreamReader(path))
        {
          string line;
          while ((line = reader.ReadLine()) != null)
          {
            ParamTuple p = ParseLine(line);
            if (p != null) paramTuples.Add(p);
          }
        }
        //ErrorLog.Warn("ParamsParser.ReadFile() sucess: " + path);
        return true;
      }
      catch { } //TODO: determine how and where to catch issues reading the params file.
      return false;
    }

    //returns true if replacing a param, false if adding a param
    public bool UpdateParam(string name, string value)
    {
      ParamTuple p = new ParamTuple();
      p.Name = name;
      p.Value = value;
      return UpdateParam(p);
    }

    public bool UpdateParam(ParamTuple param)
    {
      int i;
      for (i = 0; i < paramTuples.Count; i++)
      {
        if (param.Name == paramTuples[i].Name) break;
      }
      if (i == paramTuples.Count)
      {
        paramTuples.Add(param);
        return false;
      }
      paramTuples[i].Value = param.Value;
      return true;
    }


  }
}
