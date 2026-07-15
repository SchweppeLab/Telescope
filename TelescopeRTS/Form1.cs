using System;
using System.Collections;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.DirectoryServices;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading;
using System.Threading.Channels;
using System.Threading.Tasks;
using System.Timers;
using System.Windows.Forms;
using CometWrapper;
using Nova.Data;
using Nova.Io.Read;
using TelescopeSharp;
using ThermoFisher.CommonCore.Data;
using ThermoFisher.CommonCore.Data.Business;
using ThermoFisher.CommonCore.Data.Interfaces;

namespace TelescopeRTS
{

  // Main window for the real-time-search (RTS) demo: lets the user pick a search-engine
  // params file (Telescope or Comet) and a Thermo raw file, then streams the raw file's
  // MS/MS spectra at a configurable rate to simulate a live acquisition feed while a pool
  // of worker threads searches each scan and the UI reports throughput/timing statistics.
  public partial class Form1 : Form
  {
    // Native Telescope search engine instance (used when "Telescope" is the selected search engine).
    Telescope telescope;
    // Comet search engine instance, accessed through the managed CometWrapper (used when "Comet" is selected).
    CometSearchManagerWrapper cometSearchManager;

    // MS/MS spectra loaded from the selected raw file, filtered to those eligible for searching.
    List<Spectrum> spectra = new List<Spectrum>();
    // Number of spectra streamed/queued so far during the current run.
    int queuedScanCount = 0;

    TaskFactory taskFactory = new TaskFactory();

    // Guards activeThreadCount (slot claim/release itself is handled by
    // searchSlotSemaphore/freeSearchSlots below, not this lock).
    object threadLock = new object();
    // Guards searchResults and the aggregate timing statistics below.
    object resultsLock = new object();

    // Number of concurrent search worker "slots": user-configurable (numThreadCount) for
    // Telescope, always 1 for Comet (its search manager is not safe to call concurrently).
    int maxConcurrentThreads = 20;
    // Bounds concurrent search work to maxConcurrentThreads; a permit becomes available
    // the instant a slot is released, so awaiting it needs no polling. Recreated at the
    // start of each run to match that run's maxConcurrentThreads.
    SemaphoreSlim searchSlotSemaphore = new SemaphoreSlim(0);
    // Pool of available per-thread slot indices (0..maxConcurrentThreads-1), handed out
    // in SpectrumMonitor and returned by RunSearchLoop when a slot's chain of scans
    // finishes. The index itself matters: Telescope's native Search() uses it to pick a
    // per-slot scratch buffer, so two concurrent searches must never share one.
    ConcurrentQueue<int> freeSearchSlots = new ConcurrentQueue<int>();
    // Every task SpectrumMonitor dispatches (one per slot hand-off, running RunSearchLoop),
    // so a run can wait for all of them - and everything they chain onto themselves - to
    // truly finish before it's considered complete. Without this, a straggler from one run
    // can still be mid-search when the next run recreates searchSlotSemaphore/freeSearchSlots,
    // and its eventual slot release lands in the new run's pool instead of the old one -
    // handing that slot index out twice within the new run and corrupting both searches.
    ConcurrentBag<Task> dispatchedSearchTasks = new ConcurrentBag<Task>();

    // Aggregate timing/counters used to compute the run statistics shown in the UI.
    double sumMatchedSearchTime = 0;
    int matchedScanCount = 0;
    double sumAllSearchTime = 0;
    int allProcessedCount = 0;
    double sumLagTime = 0;
    int lagSampleCount = 0;
    double minSearchTime = 0;
    double maxSearchTime = 0;
    List<TResult> searchResults = new List<TResult>();

    // Paging state for the results display (rtbResults), 20 results per page.
    int currentResultsPage = 1;
    int maxResultsPage = 1;
    int activeThreadCount = 0;

    RunInfo runInfo = new RunInfo();

    // Tracks elapsed time since the current run started, used to timestamp queued scans
    // and to compute overall search/queueing durations.
    Stopwatch runStopwatch = new Stopwatch();
    // Producer/consumer hand-off for queued scans: the streaming loop writes, SpectrumMonitor
    // reads via ReadAllAsync (and worker tasks read directly via TryRead to grab the next
    // scan without going through the monitor). Unlike a plain ConcurrentQueue, awaiting the
    // reader is a true signal - no fixed-interval polling to notice a newly queued item.
    // Recreated at the start of each run since Complete() is a one-way, one-shot signal.
    Channel<ScanQueueItem> scanChannel = Channel.CreateUnbounded<ScanQueueItem>();

    public Form1()
    {
      InitializeComponent();
      lstSearchEngine.SelectedIndex = 0;
    }

    // Prompts for a Telescope .params file (or a Comet params file, depending on the
    // selected search engine) and initializes the corresponding search engine.
    private void btnSelectParams_Click(object sender, EventArgs e)
    {
      btnSelectParams.Enabled = false;
      lstSearchEngine.Enabled = false;
      fileDialog = new OpenFileDialog();
      fileDialog.Filter = "Telescope Params (*.params)|*.params|All Files (*.*)|*.*";
      if (fileDialog.ShowDialog() == DialogResult.OK) // For Windows Forms
      {
        lblParamsStatus.Text = "initializing...please be patient, may take up to several minutes...";
        lblParamsStatus.Update();

        string dbFile = fileDialog.FileName;

        if (lstSearchEngine.SelectedIndex == 0)
        {
          telescope = new Telescope();
          telescope.Init(dbFile);

          runInfo.peptidoforms = telescope.GetPeptidoformCount();
          runInfo.pepMemory = telescope.GetMemUse(true);
          runInfo.memory = telescope.GetMemUse(false);
          maxConcurrentThreads = 20;
        }
        else
        {
          cometSearchManager = new CometSearchManagerWrapper();
          CometParamsParser cometParams = new CometParamsParser();
          cometParams.ReadFile(dbFile);

          foreach (ParamTuple param in cometParams.paramTuples)
          {
            if (!cometParams.SetParam(cometSearchManager, param))
            {
              Log("Comet::InitializeSearch() called, but failed to set '" + param.Name + "' parameter to '" + param.Value + "'.");
            }
          }

          if (!cometParams.SetEnzyme(cometSearchManager, "Trypsin KR P 1 1"))
          {
            Log("Comet::InitializeSearch() called, but failed to SetEnzyme.");
          }

          cometSearchManager.InitializeSingleSpectrumSearch();
          Log("Comet::InitializeSearch() success.");
          maxConcurrentThreads = 1;
        }

        runInfo.searchAlg = lstSearchEngine.GetItemText(lstSearchEngine.SelectedItem);
        lblParamsStatus.Text = "Ready: " + dbFile;
        btnSelectRaw.Enabled = true;
      }
      else
      {
        lstSearchEngine.Enabled = true;
        btnSelectParams.Enabled = true;
      }
    }

    // Prompts for a Thermo .raw file, reads MS2 spectra from it (filtering out ones that
    // are too sparse, unassigned precursor charge, or outside the expected mass range),
    // and stores them for the run.
    private void btnSelectRaw_Click(object sender, EventArgs e)
    {
      btnSelectRaw.Enabled = false;
      fileDialog = new OpenFileDialog();
      fileDialog.Filter = "Thermo RAW (*.raw)|*.raw|All Files (*.*)|*.*";
      if (fileDialog.ShowDialog() == DialogResult.OK) // For Windows Forms
      {
        lblRawStatus.Text = "reading...";
        lblRawStatus.Update();

        FileReader fr = new FileReader();
        Spectrum s = fr.ReadSpectrum(fileDialog.FileName);
        while (s.ScanNumber > 0)
        {
          if (s.MsLevel != 2)
          {
            s = fr.ReadSpectrum();
            continue;
          }
          if (s.Count < 25)
          {
            s = fr.ReadSpectrum();
            continue;
          }
          if (s.Precursors.Count == 0)
          {
            s = fr.ReadSpectrum();
            continue;
          }
          if (s.Precursors[0].IsolationMz * s.Precursors[0].Charge - s.Precursors[0].Charge < 800 ||
            s.Precursors[0].IsolationMz * s.Precursors[0].Charge - s.Precursors[0].Charge > 4000)
          {
            s = fr.ReadSpectrum();
            continue;
          }
          spectra.Add(s);
          s = fr.ReadSpectrum();
        }
        lblRawStatus.Text = "Ready: " + spectra.Count + " MS/MS scans from " + fileDialog.FileName;
        runInfo.scanCount = spectra.Count;
        runInfo.dataFile = fileDialog.FileName;

        btnRun.Enabled = true;
        numScanRate.Enabled = true;
      }
      else
      {
        btnSelectRaw.Enabled = true;
      }
    }

    // Streams the loaded spectra at the user-specified rate (numScanRate, in scans/second),
    // simulating a real-time acquisition feed, while a pool of worker tasks (SpectrumMonitor)
    // pulls scans off the queue and searches them as they arrive.
    private async void btnRun_Click(object sender, EventArgs e)
    {
      numScanRate.Enabled = false;
      btnRun.Enabled = false;
      btnStop.Enabled = true;
      numThreadCount.Enabled = false;

      queuedScanCount = 0;
      //richTextBox1.Text = string.Empty;
      searchResults.Clear();
      currentResultsPage = 1;
      maxResultsPage = 1;

      runInfo.Clear();
      runInfo.Hz = (int)numScanRate.Value;

      double interval = 1.0 / (int)numScanRate.Value * 1000; //milliseconds
      long frequency = Stopwatch.Frequency;
      long ticksPerMillisecond = frequency / 1000;

      allProcessedCount = 0;
      matchedScanCount = 0;
      sumAllSearchTime = 0;
      sumMatchedSearchTime = 0;
      sumLagTime = 0;
      lagSampleCount = 0;
      minSearchTime = 0;
      maxSearchTime = 0;

      if (lstSearchEngine.SelectedIndex == 0)
      {
        maxConcurrentThreads = (int)numThreadCount.Value;
      }
      else
      {
        maxConcurrentThreads = 1;
      }
      searchSlotSemaphore = new SemaphoreSlim(maxConcurrentThreads, maxConcurrentThreads);
      freeSearchSlots = new ConcurrentQueue<int>(Enumerable.Range(0, maxConcurrentThreads));
      dispatchedSearchTasks = new ConcurrentBag<Task>();
      activeThreadCount = 0;

      System.Timers.Timer statsTimer = new System.Timers.Timer(500);
      statsTimer.Elapsed += UpdateStatsEvent;
      statsTimer.AutoReset = true;
      statsTimer.Enabled = true;

      scanChannel = Channel.CreateUnbounded<ScanQueueItem>();
      runStopwatch.Start();

      //Start the spectrum monitor: this looks for spectra waiting to be passed to threads
      Task monitorTask = Task.Run(SpectrumMonitor);

      //Start the spectrum streamer: this queues up spectra at user-defined intervals
      long searchStartTicks = 0;
      await Task.Run(() =>
      {
        Stopwatch stopwatch = new Stopwatch();
        stopwatch.Start();
        long startTicks = stopwatch.ElapsedTicks;
        long lastTicks = startTicks;
        long elapsedTicks = 0;
        searchStartTicks = runStopwatch.ElapsedTicks;

        while (true)
        {
          long currentTicks = stopwatch.ElapsedTicks;
          elapsedTicks += currentTicks - lastTicks;
          lastTicks = currentTicks;

          if (elapsedTicks >= ticksPerMillisecond * interval)
          {
            elapsedTicks -= (long)(ticksPerMillisecond * interval);
            if (queuedScanCount < spectra.Count)
            {
              //Queue up the scan
              scanChannel.Writer.TryWrite(new ScanQueueItem(queuedScanCount++, runStopwatch.ElapsedTicks));
            }
            else break;
            //AnalyzeSpectrum();
          }
        }

        long endTicks = stopwatch.ElapsedTicks;
        elapsedTicks += endTicks - startTicks;
        lock (resultsLock)
        {
          runInfo.queueTime = (double)elapsedTicks / frequency * 1000;
        }
        scanChannel.Writer.Complete();
      });
      Task.WaitAll();
      monitorTask.Wait();
      // Wait for every dispatched slot's full chain of scans (see RunSearchLoop) to
      // actually finish - not just for the queue to be drained - before the run is
      // considered complete, so no straggler can still be running when the next run
      // recreates searchSlotSemaphore/freeSearchSlots.
      await Task.WhenAll(dispatchedSearchTasks);
      long searchElapsedTicks = runStopwatch.ElapsedTicks - searchStartTicks;
      lock (resultsLock)
      {
        runInfo.searchTime = (double)searchElapsedTicks / frequency * 1000;
      }

      Log("Done queueing spectra.");

      statsTimer.Stop();
      statsTimer.Dispose();

      runStopwatch.Stop();
      UpdateStats();
      Log(runInfo.Report());

      numScanRate.Enabled = true;
      btnRun.Enabled = true;
      btnStop.Enabled = false;
      numThreadCount.Enabled = true;
    }

    // Hands queued scans off to worker tasks as both a scan and a free search slot
    // become available, until scanChannel is completed and drained. Both waits
    // (for a new scan, and for a free slot) are true async signals - awaiting
    // scanChannel.Reader.ReadAllAsync() resumes the instant a scan is written, and
    // searchSlotSemaphore.WaitAsync() resumes the instant a slot is released - so
    // there is no fixed-interval polling anywhere in this hand-off. Every dispatched
    // task is recorded in dispatchedSearchTasks so btnRun_Click can wait for the
    // entire run to truly finish, not just for scanChannel to be drained.
    private async Task SpectrumMonitor()
    {
      await foreach (ScanQueueItem item in scanChannel.Reader.ReadAllAsync())
      {
        await searchSlotSemaphore.WaitAsync();
        freeSearchSlots.TryDequeue(out int slotIndex);
        lock (threadLock)
        {
          activeThreadCount++;
        }

        // Explicit local copies before the closure captures them, so there is no ambiguity
        // about per-iteration freshness inside an async state machine's loop.
        int capturedSlotIndex = slotIndex;
        ScanQueueItem capturedItem = item;
        Task t = taskFactory.StartNew(() => RunSearchLoop(capturedSlotIndex, capturedItem));
        dispatchedSearchTasks.Add(t);
      }
    }

    // Runs for the lifetime of one search slot: scores the given scan, then keeps
    // pulling and scoring the next available scan on this same task (reusing the same
    // threadIndex) until none remain, only then returning the slot to the pool. Looping
    // in place - rather than starting a new Task per chained scan, as before - means the
    // task SpectrumMonitor dispatches doesn't complete until every scan chained onto this
    // slot is truly finished, so awaiting it is a reliable "is this slot done" signal.
    private void RunSearchLoop(int threadIndex, ScanQueueItem item)
    {
      while (true)
      {
        if (lstSearchEngine.SelectedIndex == 0)
        {
          ScoreSpectrum(threadIndex, item);
        }
        else
        {
          ScoreCometSpectrum(threadIndex, item);
        }

        if (!scanChannel.Reader.TryRead(out ScanQueueItem nextItem)) break;
        item = nextItem;
      }

      lock (threadLock)
      {
        if (activeThreadCount > runInfo.maxThreadCount) runInfo.maxThreadCount = activeThreadCount;
        activeThreadCount--;
      }
      freeSearchSlots.Enqueue(threadIndex);
      searchSlotSemaphore.Release();
    }

    // Appends a line to the log/message panel (rtbLog).
    private void Log(string msg)
    {
      rtbLog.AppendText(msg + Environment.NewLine);
    }

    // Searches one spectrum against the Comet engine and records timing/result stats,
    // then either picks up the next queued scan on this same thread or frees the slot.
    private void ScoreCometSpectrum(int threadIndex, ScanQueueItem item)
    {
      long frequency = Stopwatch.Frequency;
      TResult res = new TResult();
      res.waitTime = ((double)(runStopwatch.ElapsedTicks - item.queuedAtTicks)) / frequency * 1000000;

      Stopwatch lagStopwatch = new Stopwatch();
      lagStopwatch.Start();

      List<string> peptideSequences = new List<string>();
      List<string> proteinIdentifiers = new List<string>();
      List<List<FragmentWrapper>> matchedFragments;
      List<ScoreWrapper> searchScores;

      bool matched = false;
      Spectrum s = spectra[item.scanIndex];
      double[] mz = new double[s.Count];
      double[] intensity = new double[s.Count];
      int i = 0;
      foreach (SpecDataPoint p in s.DataPoints)
      {
        mz[i] = p.Mz;
        intensity[i++] = p.Intensity;
      }

      double precursorMz = 0;
      int precursorCharge = 0;
      double searchTimeMicros = 0;
      double lagTimeMicros = 0;

      res.scanNumber = s.ScanNumber;
      if (s.Precursors.Count > 0)
      {
        precursorMz = s.Precursors[0].MonoisotopicMz;
        if (precursorMz == 0) precursorMz = s.Precursors[0].IsolationMz;
        precursorCharge = s.Precursors[0].Charge;
      }
      if (precursorMz == 0) goto FREETHREAD;

      matched = true;
      Stopwatch searchStopwatch = new Stopwatch();
      searchStopwatch.Start();
      bool searchSuccess = cometSearchManager.DoSingleSpectrumSearchMultiResults(
          1,
          precursorCharge,
          precursorMz,
          mz,
          intensity,
          s.Count,
          out peptideSequences,
          out proteinIdentifiers,
          out matchedFragments,
          out searchScores);
      searchStopwatch.Stop();
      searchTimeMicros = (double)searchStopwatch.ElapsedTicks / frequency * 1000000;
      res.searchTime = searchTimeMicros;

      if (searchSuccess && searchScores.Count > 0)
      {
        res.peptide = peptideSequences[0];
        res.protein = proteinIdentifiers[0].Substring(0, 21);
        res.score = searchScores[0].xCorr;
      }

    FREETHREAD:
      lagStopwatch.Stop();
      lagTimeMicros = (double)lagStopwatch.ElapsedTicks / frequency * 1000000;

      lock (resultsLock)
      {
        allProcessedCount++;
        sumAllSearchTime += searchTimeMicros;
        sumLagTime += lagTimeMicros;
        lagSampleCount++;
        if (matched)
        {
          matchedScanCount++;
          sumMatchedSearchTime += searchTimeMicros;
          if (minSearchTime == 0)
          {
            minSearchTime = searchTimeMicros;
            maxSearchTime = searchTimeMicros;
          }
          else
          {
            if (searchTimeMicros < minSearchTime) minSearchTime = searchTimeMicros;
            if (searchTimeMicros > maxSearchTime) maxSearchTime = searchTimeMicros;
          }
          res.lagTime = lagTimeMicros;
        }
        searchResults.Add(res);
        runInfo.sumWaitTime += res.waitTime;
        runInfo.sumSearchTime += searchTimeMicros;
        runInfo.scansSearched++;

      }
    }

    // Searches one spectrum against the Telescope engine and records timing/result stats,
    // then either picks up the next queued scan on this same thread or frees the slot.
    private void ScoreSpectrum(int threadIndex, ScanQueueItem item)
    {
      long frequency = Stopwatch.Frequency;
      TResult res = new TResult();
      res.waitTime = ((double)(runStopwatch.ElapsedTicks - item.queuedAtTicks)) / frequency * 1000000;

      Stopwatch lagStopwatch = new Stopwatch();
      lagStopwatch.Start();

      bool matched = false;
      Spectrum s = spectra[item.scanIndex];
      double[] mz = new double[s.Count];
      double[] intensity = new double[s.Count];
      int i = 0;
      foreach (SpecDataPoint p in s.DataPoints)
      {
        mz[i] = p.Mz;
        intensity[i++] = p.Intensity;
      }

      double precursorMz = 0;
      int precursorCharge = 0;
      double searchTimeMicros = 0;
      double lagTimeMicros = 0;

      res.scanNumber = s.ScanNumber;
      if (s.Precursors.Count > 0)
      {
        precursorMz = s.Precursors[0].MonoisotopicMz;
        if (precursorMz == 0) precursorMz = s.Precursors[0].IsolationMz;
        precursorCharge = s.Precursors[0].Charge;
      }
      if (precursorMz == 0) goto FREETHREAD;

      matched = true;
      Stopwatch searchStopwatch = new Stopwatch();
      searchStopwatch.Start();
      TScore score = telescope.Search(threadIndex, mz, intensity, precursorMz, precursorCharge);
      searchStopwatch.Stop();
      searchTimeMicros = (double)searchStopwatch.ElapsedTicks / frequency * 1000000;
      res.searchTime = searchTimeMicros;
      res.peptide = score.peptide;
      res.protein = score.protein;
      res.score = score.score;

    FREETHREAD:
      lagStopwatch.Stop();
      lagTimeMicros = (double)lagStopwatch.ElapsedTicks / frequency * 1000000;

      lock (resultsLock)
      {
        allProcessedCount++;
        sumAllSearchTime += searchTimeMicros;
        sumLagTime += lagTimeMicros;
        lagSampleCount++;
        if (matched)
        {
          matchedScanCount++;
          sumMatchedSearchTime += searchTimeMicros;
          if (minSearchTime == 0)
          {
            minSearchTime = searchTimeMicros;
            maxSearchTime = searchTimeMicros;
          }
          else
          {
            if (searchTimeMicros < minSearchTime) minSearchTime = searchTimeMicros;
            if (searchTimeMicros > maxSearchTime) maxSearchTime = searchTimeMicros;
          }
          res.lagTime = lagTimeMicros;
        }
        searchResults.Add(res);
        runInfo.sumWaitTime += res.waitTime;
        runInfo.sumSearchTime += searchTimeMicros;
        runInfo.scansSearched++;

      }
    }

    // Fixed-width header row shared by the on-screen results view and the exported log.
    private static string FormatResultsHeader()
    {
      return string.Format("{0,-" + 8 + "} {1,-" + 8 + "} {2,-" + 8 + "} {3,-" + 12 + "} {4,-" + 40 + "} {5,-" + 12 + "}", "SCAN", "TIME(us)", "DELAY(us)", "SCORE", "PEPTIDE", "PROTEIN");
    }

    // Fixed-width row for one search result, matching FormatResultsHeader's column widths.
    private static string FormatResultRow(TResult result)
    {
      return string.Format("{0,-" + 8 + "} {1,-" + 8 + "} {2,-" + 8 + "} {3,-" + 12 + "} {4,-" + 40 + "} {5,-" + 12 + "}", result.scanNumber.ToString(), result.searchTime.ToString("F2"), result.waitTime.ToString("F2"), result.score.ToString("F2"), result.peptide, result.protein);
    }

    // Tab-delimited header row for the exported results (see btnExportLog_Click).
    private static string FormatResultsHeaderTsv()
    {
      return string.Join("\t", "SCAN", "TIME(us)", "DELAY(us)", "SCORE", "PEPTIDE", "PROTEIN");
    }

    // Tab-delimited row for one search result, matching FormatResultsHeaderTsv's columns.
    private static string FormatResultRowTsv(TResult result)
    {
      return string.Join("\t",
        result.scanNumber.ToString(),
        result.searchTime.ToString("F2"),
        result.waitTime.ToString("F2"),
        result.score.ToString("F2"),
        result.peptide,
        result.protein);
    }

    // Renders one 20-row page of searchResults into rtbResults as a fixed-width table.
    private void UpdateResults()
    {
      rtbResults.Text = FormatResultsHeader() + Environment.NewLine;
      string separator = new string('=', 95);
      rtbResults.Text += separator + Environment.NewLine;
      int start = (currentResultsPage - 1) * 20;
      for (int i = start; i < start + 20; i++)
      {
        if (i == searchResults.Count) break;
        rtbResults.Text += FormatResultRow(searchResults[i]) + Environment.NewLine;
      }
    }

    private void UpdateStatsEvent(Object source, ElapsedEventArgs e)
    {
      UpdateStats();
    }

    // Refreshes the summary statistics labels from the current aggregate counters.
    private void UpdateStats()
    {
      lock (resultsLock)
      {
        lblTotalScansValue.Text = queuedScanCount.ToString();
        lblMatchedScansValue.Text = matchedScanCount.ToString();
        lblAvgSearchTimeValue.Text = (sumMatchedSearchTime / matchedScanCount).ToString("F2") + " us";
        lblAvgLagTimeValue.Text = (sumLagTime / lagSampleCount).ToString("F2") + " us";
        lblFastestSearchValue.Text = minSearchTime.ToString("F2") + " us";
        lblSlowestSearchValue.Text = maxSearchTime.ToString("F2") + " us";
        lock (threadLock)
        {
          lblActiveThreadsValue.Text = activeThreadCount.ToString();
          //lblScansQueuedValue.Text = scanChannel.Reader.Count.ToString();
        }

        maxResultsPage = searchResults.Count / 20 + 1;
        lblPageIndicator.Text = currentResultsPage.ToString() + "/" + maxResultsPage.ToString();
      }
    }

    private void btnStop_Click(object sender, EventArgs e)
    {
      //find a good way to stop the analysis. it is currently broken.
      btnStop.Enabled = false;
    }

    private void btnPagePrev_Click(object sender, EventArgs e)
    {
      if (currentResultsPage > 1) currentResultsPage--;
      lblPageIndicator.Text = currentResultsPage.ToString() + "/" + maxResultsPage.ToString();
      UpdateResults();
    }

    private void btnPageNext_Click(object sender, EventArgs e)
    {
      currentResultsPage++;
      if (currentResultsPage > maxResultsPage) currentResultsPage = maxResultsPage;
      lblPageIndicator.Text = currentResultsPage.ToString() + "/" + maxResultsPage.ToString();
      UpdateResults();
    }

    private void btnPageForward100_Click(object sender, EventArgs e)
    {
      currentResultsPage += 100;
      if (currentResultsPage > maxResultsPage) currentResultsPage = maxResultsPage;
      lblPageIndicator.Text = currentResultsPage.ToString() + "/" + maxResultsPage.ToString();
      UpdateResults();
    }

    private void btnPageBack100_Click(object sender, EventArgs e)
    {
      currentResultsPage -= 100;
      if (currentResultsPage < 1) currentResultsPage = 1;
      lblPageIndicator.Text = currentResultsPage.ToString() + "/" + maxResultsPage.ToString();
      UpdateResults();
    }

    // Writes the most recent run's RunInfo report, followed by the complete list of
    // search results, to a file named after the export timestamp.
    private void btnExportLog_Click(object sender, EventArgs e)
    {
      string timestamp = DateTime.Now.ToString("yyyy-MM-dd_HH-mm-ss");
      string fileName = $"log_{timestamp}.txt";
      using (StreamWriter sw = new StreamWriter(fileName))
      {
        sw.WriteLine(runInfo.Report());

        sw.WriteLine(FormatResultsHeaderTsv());
        lock (resultsLock)
        {
          foreach (TResult result in searchResults)
          {
            sw.WriteLine(FormatResultRowTsv(result));
          }
        }
      }
      Log("Exported: " + fileName);
    }

  }
}
