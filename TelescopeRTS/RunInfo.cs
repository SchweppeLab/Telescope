using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Permissions;
using System.Text;
using System.Threading.Tasks;

namespace TelescopeRTS
{
  // Aggregated statistics for a single search run: search-space info captured once when
  // the params file is loaded, plus run-to-run counters that reset each run (see Clear()).
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

    public string searchAlg = string.Empty;
    public string dataFile = string.Empty;

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

    // Builds a human-readable summary of this run for the log/export.
    public string Report()
    {
      string report = Environment.NewLine+ "========= RUN ==========" + Environment.NewLine;
      report += "Search Algorithm: " + searchAlg + Environment.NewLine;
      report += "Spectral Data File: " + dataFile + Environment.NewLine;

      report += "Search Peptidoforms: " + peptidoforms + Environment.NewLine;
      report += "Peptidoform Memory: " + pepMemory + " Gb"+ Environment.NewLine;
      report += "Index Memory: " + memory + " Gb" + Environment.NewLine;
      report += "Scan Count: " + scanCount + Environment.NewLine;
      report += "Speed: " + Hz + " scans per second" + Environment.NewLine;
      report += "Realized Speed: " + scanCount / (queueTime / 1000) + " Hz" + Environment.NewLine;

      report += "Scans searched: " + scansSearched.ToString() + Environment.NewLine;
      report += "Search time: " + searchTime/1000 + " seconds" + Environment.NewLine;
      report += "Search Speed: " + scansSearched / (searchTime / 1000) + " Hz" + Environment.NewLine;
      report += "Avg MS search time: " + (sumSearchTime / scansSearched).ToString() + " micro seconds" + Environment.NewLine;
      report += "Max concurrent threads: " + maxThreadCount.ToString() + Environment.NewLine;
      report += "Peak Scans Waiting: " + scansWaiting.ToString() + Environment.NewLine;
      report += "Wait time: " + (sumWaitTime/ scansSearched).ToString() + " micro seconds" + Environment.NewLine;
      return report;
    }
  }
}
