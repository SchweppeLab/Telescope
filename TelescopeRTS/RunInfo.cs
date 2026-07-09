using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Permissions;
using System.Text;
using System.Threading.Tasks;

namespace TelescopeRTS
{
  internal class RunInfo
  {
    public double memory = 0;
    public double pepMemory = 0;
    public int peptidoforms = 0;
    public int scanCount = 0;

    //Run to run changes
    public int Hz = 0;
    public int scansSearched = 0;
    public int scansWaiting = 0;
    public double sumWaitTime = 0;
    public int maxThreadCount = 0;
    public double sumSearchTime = 0;
    public double queueTime = 0;
    public double searchTime = 0;

    public void Clear()
    {
      Hz = 0;
      scansSearched = 0;
      scansWaiting = 0;
      maxThreadCount = 0;
      sumSearchTime = 0;
      sumWaitTime = 0;
      queueTime = 0;
      searchTime = 0;
    }

    public string Report()
    {
      string s = Environment.NewLine+ "========= RUN ==========" + Environment.NewLine;
      s += "Search Peptidoforms: " + peptidoforms + Environment.NewLine;
      s += "Peptidoform Memory: " + pepMemory + " Gb"+ Environment.NewLine;
      s += "Index Memory: " + memory + " Gb" + Environment.NewLine;
      s += "Scan Count: " + scanCount + Environment.NewLine;
      s += "Speed: " + Hz + " scans per second" + Environment.NewLine;
      s += "Realized Speed: " + scanCount / (queueTime / 1000) + " Hz" + Environment.NewLine;

      s += "Scans searched: " + scansSearched.ToString() + Environment.NewLine;
      s += "Search time: " + searchTime/1000 + " seconds" + Environment.NewLine;
      s += "Search Speed: " + scansSearched / (searchTime / 1000) + " Hz" + Environment.NewLine;
      s += "Avg MS search time: " + (sumSearchTime / scansSearched).ToString() + " micro seconds" + Environment.NewLine;
      s += "Max concurrent threads: " + maxThreadCount.ToString() + Environment.NewLine;
      s += "Scans deferred: " + scansWaiting.ToString() + Environment.NewLine;
      s += "Wait time: " + (sumWaitTime/ scansSearched).ToString() + " micro seconds" + Environment.NewLine;
      return s;
    }
  }
}
