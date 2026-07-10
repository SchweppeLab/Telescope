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
using static System.Net.WebRequestMethods;

namespace TelescopeRTS
{

  public partial class Form1 : Form
  {
    Telescope TS;
    CometSearchManagerWrapper CS;

    List<Spectrum> spectra = new List<Spectrum>();
    int count = 0;

    TaskFactory tf = new TaskFactory();

    object lockObject = new object();
    object lockResult = new object();

    int threadCount = 20;
    bool[] threads = new bool[20];

    double matchTime = 0;
    int matchCount = 0;
    double allTime = 0;
    int allCount = 0;
    double lagTime = 0;
    int lagCount = 0;
    double minMatchTime = 0;
    double maxMatchTime = 0;
    List<TResult> results = new List<TResult>();
    
    int curPage = 1;
    int maxPage = 1;
    int threadUse = 0;

    bool doneQueueing = false;

    RunInfo runInfo = new RunInfo();

    Stopwatch globalWatch = new Stopwatch(); 
    ConcurrentQueue<ScanQueueItem> scanQueue = new ConcurrentQueue<ScanQueueItem>();

    public Form1()
    {
      InitializeComponent();
      for (int i = 0; i < threadCount; i++) threads[i] = false;
      searchListBox.SelectedIndex = 0;
    }

    private void button1_Click(object sender, EventArgs e)
    {
      button1.Enabled = false;
      searchListBox.Enabled = false;
      openFileDialog1 = new OpenFileDialog();
      openFileDialog1.Filter = "Telescope Params (*.params)|*.params|All Files (*.*)|*.*";
      if (openFileDialog1.ShowDialog() == DialogResult.OK) // For Windows Forms
      {
        label1.Text = "initializing...please be patient, may take up to several minutes...";
        label1.Update();

        string dbFile = openFileDialog1.FileName;

        if (searchListBox.SelectedIndex == 0)
        {
          TS = new Telescope();
          TS.Init(dbFile);

          runInfo.peptidoforms = TS.GetPeptidoformCount();
          runInfo.pepMemory = TS.GetMemUse(true);
          runInfo.memory = TS.GetMemUse(false);
          threadCount = 20;
        }
        else
        {
          CS = new CometSearchManagerWrapper();
          CometParamsParser cometParams = new CometParamsParser();
          cometParams.ReadFile(dbFile);

          foreach (ParamTuple param in cometParams.paramTuples)
          {
            if (!cometParams.SetParam(CS, param))
            {
              Log("Comet::InitializeSearch() called, but failed to set '" + param.Name + "' parameter to '" + param.Value + "'.");
            }
          }

          if (!cometParams.SetEnzyme(CS, "Trypsin KR P 1 1"))
          {
            Log("Comet::InitializeSearch() called, but failed to SetEnzyme.");
          }

          CS.InitializeSingleSpectrumSearch();
          Log("Comet::InitializeSearch() success.");
          threadCount = 1;
        }

        label1.Text = "Ready: " + dbFile;
        button2.Enabled = true;
      }
      else
      {
        searchListBox.Enabled = true;
        button1.Enabled = true;
      }
    }

    private void button2_Click(object sender, EventArgs e)
    {
      button2.Enabled = false;
      openFileDialog1 = new OpenFileDialog();
      openFileDialog1.Filter = "Thermo RAW (*.raw)|*.raw|All Files (*.*)|*.*";
      if (openFileDialog1.ShowDialog() == DialogResult.OK) // For Windows Forms
      {
        label2.Text = "reading...";
        label2.Update();

        FileReader fr = new FileReader();
        Spectrum s = fr.ReadSpectrum(openFileDialog1.FileName);
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
        label2.Text = "Ready: " + spectra.Count + " MS/MS scans from " + openFileDialog1.FileName;
        runInfo.scanCount = spectra.Count;

        button3.Enabled = true;
        numericUpDown1.Enabled = true;
      }
      else
      {
        button2.Enabled = true;
      }
    }

    private async void button3_Click(object sender, EventArgs e)
    {
      numericUpDown1.Enabled = false;
      button3.Enabled = false;
      button4.Enabled = true;
      nudThreads.Enabled = false;

      count = 0;
      //richTextBox1.Text = string.Empty;
      results.Clear();
      curPage = 1;
      maxPage = 1;

      runInfo.Clear();
      runInfo.Hz = (int)numericUpDown1.Value;

      double interval = 1.0 / (int)numericUpDown1.Value * 1000; //milliseconds
      long frequency = Stopwatch.Frequency;
      long tpm = frequency / 1000; //ticks-per-millisecond.

      allCount = 0;
      matchCount = 0;
      allTime = 0;
      matchTime = 0;
      lagTime = 0;
      lagCount = 0;
      minMatchTime = 0;
      maxMatchTime = 0;

      if (searchListBox.SelectedIndex == 0)
      {
        threadCount = (int)nudThreads.Value;
      }
      else
      {
        threadCount = 1;
      }
      for (int a = 0; a < threadCount; a++) threads[a] = false;
      threadUse = 0;

      System.Timers.Timer myTimer = new System.Timers.Timer(500);
      myTimer.Elapsed += UpdateStatsEvent;
      myTimer.AutoReset = true;
      myTimer.Enabled = true;

      doneQueueing = false;
      globalWatch.Start();

      //Start the spectrum monitor: this looks for spectra waiting to be passed to threads
      Task monTask = Task.Run(SpectrumMonitor);

      //Start the spectrum streamer: this queues up spectra at user-defined intervals
      long searchTime = 0;
      await Task.Run(() =>
      {
        Stopwatch stopwatch = new Stopwatch();
        stopwatch.Start();
        long startTicks = stopwatch.ElapsedTicks;
        long lastTicks = startTicks;
        long elapsedTicks = 0;
        searchTime = globalWatch.ElapsedTicks;

        while (true)
        {
          long currentTicks = stopwatch.ElapsedTicks;
          elapsedTicks += currentTicks - lastTicks;
          lastTicks = currentTicks;

          if (elapsedTicks >= tpm * interval)
          {
            elapsedTicks -= (long)(tpm * interval);
            if (count < spectra.Count)
            {
              //Queue up the scan
              scanQueue.Enqueue(new ScanQueueItem(count++, globalWatch.ElapsedTicks));
            }
            else break;
            //AnalyzeSpectrum();
          }
        }

        long endTicks = stopwatch.ElapsedTicks;
        elapsedTicks += endTicks - startTicks;
        lock (lockResult)
        {
          runInfo.queueTime = (double)elapsedTicks / frequency * 1000;
        }
        doneQueueing = true;
      });
      Task.WaitAll();
      monTask.Wait();
      long searchElapsedTicks =globalWatch.ElapsedTicks - searchTime;
      lock (lockResult)
      {
        runInfo.searchTime = (double)searchElapsedTicks / frequency * 1000;
      }

      Log("Done queueing spectra: " + scanQueue.Count.ToString());

      myTimer.Stop();
      myTimer.Dispose();

      globalWatch.Stop();
      UpdateStats();
      Log(runInfo.Report());

      numericUpDown1.Enabled = true;
      button3.Enabled = true;
      button4.Enabled = false;
      nudThreads.Enabled = true;
    }

    private async Task SpectrumMonitor()
    {
      while (!doneQueueing || !scanQueue.IsEmpty)
      {
        int tIndex = -1;
        lock (lockObject)
        {
          for (int a = 0; a < threadCount; a++)
          {
            if (!threads[a])
            {
              tIndex = a;
              threads[a] = true;
              threadUse++;
              break;
            }
          }
        }

        if (tIndex > -1)
        {
          //if thread was available, score the next scan in the queue, otherwise return the thread to the pool
          if (scanQueue.TryDequeue(out ScanQueueItem res))
          {
            if (searchListBox.SelectedIndex == 0)
            {
              Task t = tf.StartNew(() => ScoreSpectrum(tIndex, res));
            }
            else
            {
              Task t = tf.StartNew(() => ScoreCometSpectrum(tIndex, res));
            }
          }
          else
          {
            lock (lockObject)
            {
              threads[tIndex] = false;
              threadUse--;
            }
          }
        }

        //wait 5ms before checking for more spectra
        await Task.Delay(1);
      }
    }

    private void Log(string msg)
    {
      rtbMessage.AppendText(msg + Environment.NewLine);
    }

    private void ScoreCometSpectrum(int tIndex, ScanQueueItem sci)
    {
      long frequency = Stopwatch.Frequency;
      TResult res = new TResult();
      res.waitTime = ((double)(globalWatch.ElapsedTicks - sci.ticks)) / frequency * 1000000;
      
      Stopwatch lagwatch = new Stopwatch();
      lagwatch.Start();

      List<string> peptideSequences = new List<string>();
      List<string> proteinIdentifiers = new List<string>();
      List<List<FragmentWrapper>> matchedFragments;
      List<ScoreWrapper> searchScores;

      bool bSearch = false;
      Spectrum s = spectra[sci.scanIndex];
      double[] mz = new double[s.Count];
      double[] intensity = new double[s.Count];
      int i = 0;
      foreach (SpecDataPoint p in s.DataPoints)
      {
        mz[i] = p.Mz;
        intensity[i++] = p.Intensity;
      }

      double preMz = 0;
      int preZ = 0;
      double dt = 0;
      double dtLag = 0;
 
      res.scanNumber = s.ScanNumber;
      if (s.Precursors.Count > 0)
      {
        preMz = s.Precursors[0].MonoisotopicMz;
        if (preMz == 0) preMz = s.Precursors[0].IsolationMz;
        preZ = s.Precursors[0].Charge;
      }
      if (preMz == 0) goto FREETHREAD;

      bSearch = true;
      Stopwatch stopwatch = new Stopwatch();
      stopwatch.Start();
      bool searchSuccess = CS.DoSingleSpectrumSearchMultiResults(
          1,
          preZ,
          preMz,
          mz,
          intensity,
          s.Count,
          out peptideSequences,
          out proteinIdentifiers,
          out matchedFragments,
          out searchScores);
      stopwatch.Stop();
      dt = (double)stopwatch.ElapsedTicks / frequency * 1000000;
      res.searchTime = dt;

      if (searchSuccess && searchScores.Count > 0)
      {
        res.peptide = peptideSequences[0];
        res.protein = proteinIdentifiers[0].Substring(0, 21);
        res.score = searchScores[0].xCorr;
      }

    FREETHREAD:
      lagwatch.Stop();
      dtLag = (double)lagwatch.ElapsedTicks / frequency * 1000000;

      lock (lockResult)
      {
        allCount++;
        allTime += dt;
        lagTime += dtLag;
        lagCount++;
        if (bSearch)
        {
          matchCount++;
          matchTime += dt;
          if (minMatchTime == 0)
          {
            minMatchTime = dt;
            maxMatchTime = dt;
          }
          else
          {
            if (dt < minMatchTime) minMatchTime = dt;
            if (dt > maxMatchTime) maxMatchTime = dt;
          }
          res.lagTime = dtLag;
        }
        results.Add(res);
        runInfo.sumWaitTime += res.waitTime;
        runInfo.sumSearchTime += dt;
        runInfo.scansSearched++;

      }

      //Try grabbing another scan, or free the thread if there is none.
      if (scanQueue.TryDequeue(out ScanQueueItem sq))
      {
        if (searchListBox.SelectedIndex == 0)
        {
          Task t = tf.StartNew(() => ScoreSpectrum(tIndex, sq));
        }
        else
        {
          Task t = tf.StartNew(() => ScoreCometSpectrum(tIndex, sq));
        }
      }
      else
      {
        lock (lockObject)
        {
          if (threadUse > runInfo.maxThreadCount) runInfo.maxThreadCount = threadUse;
          threads[tIndex] = false;
          threadUse--;
        }
      }

    }

    private void ScoreSpectrum(int tIndex, ScanQueueItem sci)
    {
      long frequency = Stopwatch.Frequency;
      TResult res = new TResult();
      res.waitTime =((double)(globalWatch.ElapsedTicks - sci.ticks)) / frequency * 1000000;

      Stopwatch lagwatch = new Stopwatch();
      lagwatch.Start();

      bool bSearch = false;
      Spectrum s = spectra[sci.scanIndex];
      double[] mz = new double[s.Count];
      double[] intensity = new double[s.Count];
      int i = 0;
      foreach (SpecDataPoint p in s.DataPoints)
      {
        mz[i] = p.Mz;
        intensity[i++] = p.Intensity;
      }

      double preMz = 0;
      int preZ = 0;
      double dt = 0;
      double dtLag = 0;

      res.scanNumber = s.ScanNumber;
      if (s.Precursors.Count > 0)
      {
        preMz = s.Precursors[0].MonoisotopicMz;
        if (preMz == 0) preMz = s.Precursors[0].IsolationMz;
        preZ = s.Precursors[0].Charge;
      }
      if (preMz == 0) goto FREETHREAD;

      bSearch = true;
      Stopwatch stopwatch = new Stopwatch();
      stopwatch.Start();
      TScore score = TS.Search(tIndex, mz, intensity, preMz, preZ);
      stopwatch.Stop();
      dt = (double)stopwatch.ElapsedTicks / frequency * 1000000;
      res.searchTime = dt;
      res.peptide = score.peptide;
      res.protein = score.protein;
      res.score = score.score;

    FREETHREAD:
      lagwatch.Stop();
      dtLag = (double)lagwatch.ElapsedTicks / frequency * 1000000;

      lock (lockResult)
      {
        allCount++;
        allTime += dt;
        lagTime += dtLag;
        lagCount++;
        if (bSearch)
        {
          matchCount++;
          matchTime += dt;
          if (minMatchTime == 0)
          {
            minMatchTime = dt;
            maxMatchTime = dt;
          }
          else
          {
            if (dt < minMatchTime) minMatchTime = dt;
            if (dt > maxMatchTime) maxMatchTime = dt;
          }
          res.lagTime = dtLag;
        }
        results.Add(res);
        runInfo.sumWaitTime += res.waitTime;
        runInfo.sumSearchTime += dt;
        runInfo.scansSearched++;

      }

      //Try grabbing another scan, or free the thread if there is none.
      if (scanQueue.TryDequeue(out ScanQueueItem sq))
      {
        if (searchListBox.SelectedIndex == 0)
        {
          Task t = tf.StartNew(() => ScoreSpectrum(tIndex, sq));
        }
        else
        {
          Task t = tf.StartNew(() => ScoreCometSpectrum(tIndex, sq));
        }
      }
      else
      {
        lock (lockObject)
        {
          if (threadUse > runInfo.maxThreadCount) runInfo.maxThreadCount = threadUse;
          threads[tIndex] = false;
          threadUse--;
        }
      }
    }

    private void UpdateResults()
    {
      richTextBox1.Text = string.Format("{0,-" + 8 + "} {1,-" + 8 + "} {2,-" + 8 + "} {3,-" + 12 + "} {4,-" + 40 + "} {5,-" + 12 + "}", "SCAN", "TIME(us)", "DELAY(us)", "SCORE", "PEPTIDE", "PROTEIN") + Environment.NewLine;
      string str = new string('=', 95);
      richTextBox1.Text += str + Environment.NewLine;
      int start = (curPage - 1) * 20;
      for (int i = start; i < start + 20; i++)
      {
        if (i == results.Count) break;
        richTextBox1.Text += string.Format("{0,-" + 8 + "} {1,-" + 8 + "} {2,-" + 8 + "} {3,-" + 12 + "} {4,-" + 40 + "} {5,-" + 12 + "}", results[i].scanNumber.ToString(), results[i].searchTime.ToString("F2"), results[i].waitTime.ToString("F2"),results[i].score.ToString("F2"), results[i].peptide, results[i].protein) + Environment.NewLine;
      }
    }

    private void UpdateStatsEvent(Object source, ElapsedEventArgs e)
    {
      UpdateStats();
    }

    private void UpdateStats()
    {
      lock (lockResult)
      {
        label7.Text = count.ToString();
        label8.Text = matchCount.ToString();
        label9.Text = (matchTime / matchCount).ToString("F2") + " us";
        label14.Text = (lagTime / lagCount).ToString("F2") + " us";
        label17.Text = minMatchTime.ToString("F2") + " us";
        label18.Text = maxMatchTime.ToString("F2") + " us";
        lock (lockObject)
        {
          label12.Text = threadUse.ToString();
          //label19.Text = scanQueue.Count.ToString();
        }

        maxPage = results.Count / 20 + 1;
        label10.Text = curPage.ToString() + "/" + maxPage.ToString();
      }
    }

    private void button4_Click(object sender, EventArgs e)
    {
      //find a good way to stop the analysis. it is currently broken.
      button4.Enabled = false;
    }

    private void button5_Click(object sender, EventArgs e)
    {
      if (curPage > 1) curPage--;
      label10.Text = curPage.ToString() + "/" + maxPage.ToString();
      UpdateResults();
    }

    private void button6_Click(object sender, EventArgs e)
    {
      curPage++;
      if (curPage > maxPage) curPage = maxPage;
      label10.Text = curPage.ToString() + "/" + maxPage.ToString();
      UpdateResults();
    }

    private void button7_Click(object sender, EventArgs e)
    {
      curPage += 100;
      if (curPage > maxPage) curPage = maxPage;
      label10.Text = curPage.ToString() + "/" + maxPage.ToString();
      UpdateResults();
    }

    private void button8_Click(object sender, EventArgs e)
    {
      curPage -= 100;
      if (curPage < 1) curPage = 1;
      label10.Text = curPage.ToString() + "/" + maxPage.ToString();
      UpdateResults();
    }

    private void button9_Click(object sender, EventArgs e)
    {
      string timestamp = DateTime.Now.ToString("yyyy-MM-dd_HH-mm-ss");
      string fileName = $"log_{timestamp}.txt";
      using (StreamWriter sw = new StreamWriter(fileName))
      {
        sw.WriteLine(runInfo.Report());
      }
    }

  }
}
