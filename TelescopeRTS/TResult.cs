using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TelescopeRTS
{
  internal class TResult
  {
    public int scanNumber = 0;
    public double lagTime = 0;
    public double searchTime = 0;
    public double waitTime = 0;
    public double score = 0;
    public string peptide = string.Empty;
    public string protein = string.Empty;
  }
}
