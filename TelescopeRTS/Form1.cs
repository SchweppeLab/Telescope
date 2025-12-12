using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using Nova.Io.Read;
using Nova.Data;

using TelescopeSharp;
using System.Diagnostics;
using System.Timers;
using ThermoFisher.CommonCore.Data.Interfaces;
using System.Runtime.CompilerServices;
using ThermoFisher.CommonCore.Data;

namespace TelescopeRTS
{

  public partial class Form1 : Form
  {
    Telescope TS;
    List<Spectrum> spectra = new List<Spectrum>();
    int count = 0;

    TaskFactory tf = new TaskFactory();

    object lockObject = new object();
    object lockResult = new object();
    bool killMe = false;

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

    public Form1()
    {
      InitializeComponent();
      for (int i = 0; i < threadCount; i++) threads[i] = false;
    }

    private void button1_Click(object sender, EventArgs e)
    {
      button1.Enabled = false;
      openFileDialog1 = new OpenFileDialog();
      openFileDialog1.Filter = "Telescope Params (*.params)|*.params|All Files (*.*)|*.*";
      if (openFileDialog1.ShowDialog() == DialogResult.OK) // For Windows Forms
      {
        label1.Text = "initializing...";
        label1.Update();

        string dbFile = openFileDialog1.FileName;
        TS = new Telescope();
        TS.Init(dbFile);

        label1.Text = "Ready: " + dbFile;
        button2.Enabled = true;
      }
      else
      {
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

      count = 0;
      richTextBox1.Text = string.Empty;
      killMe = false;
      results.Clear();
      curPage = 1;
      maxPage = 1;

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


      System.Timers.Timer myTimer = new System.Timers.Timer(500);
      myTimer.Elapsed += UpdateStats;
      myTimer.AutoReset = true;
      myTimer.Enabled = true;

      await Task.Run(() =>
      {
        Stopwatch stopwatch = new Stopwatch();
        stopwatch.Start();
        long startTicks = stopwatch.ElapsedTicks;
        long lastTicks = startTicks;
        long elapsedTicks = 0;

        while (!killMe)
        {
          long currentTicks = stopwatch.ElapsedTicks;
          elapsedTicks += currentTicks - lastTicks;
          lastTicks = currentTicks;

          if (elapsedTicks >= tpm * interval)
          {
            elapsedTicks -= (long)(tpm * interval);
            AnalyzeSpectrum();
          }
        }
      });
      Task.WaitAll();

      myTimer.Stop();
      myTimer.Dispose();

      numericUpDown1.Enabled = true;
      button3.Enabled = true;
    }

    private void AnalyzeSpectrum()//Object source, ElapsedEventArgs e)
    {
      if (count < spectra.Count)
      {
        int tIndex = -1;
        while (tIndex < 0)
        {
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
            //if (tIndex < 0) System.Threading.Thread.Sleep(1);
          }
          if (tIndex < 0)
          {
            lock (lockResult)
            {
              //  richTextBox1.Text += "No threads" + Environment.NewLine;
              richTextBox1.Update();
            }
            //System.Threading.Thread.Sleep(1);
          }
        }
        Task t = tf.StartNew(() => ScoreSpectrum(tIndex, count++));
        //lock (lockResult)
        //{
        //  richTextBox1.Text += "Task sent." + Environment.NewLine;
        //  richTextBox1.Update();
        //}
      }
      else
      {
        killMe = true;
      }
    }

    private void ScoreSpectrum(int tIndex, int sIndex)
    {
      long frequency = Stopwatch.Frequency;
      Stopwatch lagwatch = new Stopwatch();
      lagwatch.Start();

      bool bSearch = false;
      Spectrum s = spectra[sIndex];
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
      TResult res = new TResult();
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
          if(minMatchTime == 0)
          {
            minMatchTime = dt;
            maxMatchTime = dt;
          } else
          {
            if(dt<minMatchTime) minMatchTime = dt;
            if(dt>maxMatchTime) maxMatchTime = dt;
          }
          res.lagTime = dtLag;
        }
        results.Add(res);

      }
      lock (lockObject)
      {
        threads[tIndex] = false;
        threadUse--;
        //lock (lockResult)
        //{
        //  richTextBox1.Text += "free " + tIndex.ToString() + Environment.NewLine;
        //}
      }
    }

    private void UpdateResults()
    {
      richTextBox1.Text = string.Format("{0,-" + 8 + "} {1,-" + 8 + "} {2,-" + 8 + "} {3,-" + 40 + "} {4,-" + 12 + "}", "SCAN", "TIME(us)", "SCORE", "PEPTIDE", "PROTEIN") + Environment.NewLine;
      string str = new string('=', 75);
      richTextBox1.Text += str + Environment.NewLine;
      int start = (curPage - 1) * 20;
      for (int i = start; i < start + 20; i++)
      {
        if (i == results.Count) break;
        richTextBox1.Text += string.Format("{0,-" + 8 + "} {1,-" + 8 + "} {2,-" + 8 + "} {3,-" + 40 + "} {4,-" + 12 + "}", results[i].scanNumber.ToString(), results[i].searchTime.ToString("F2"), results[i].score.ToString("F2"), results[i].peptide, results[i].protein) + Environment.NewLine;
      }
    }

    private void UpdateStats(Object source, ElapsedEventArgs e)
    {
      lock (lockResult)
      {
        label7.Text = allCount.ToString();
        label8.Text = matchCount.ToString();
        label9.Text = (matchTime / matchCount).ToString("F2") + " us";
        label14.Text = (lagTime / lagCount).ToString("F2") + " us";
        label17.Text = minMatchTime.ToString("F2") + " us";
        label18.Text = maxMatchTime.ToString("F2") + " us";
        lock (lockObject)
        {
          label12.Text = threadUse.ToString();
        }

        maxPage = results.Count / 20 + 1;
        label10.Text = curPage.ToString() + "/" + maxPage.ToString();
      }
    }

    private void button4_Click(object sender, EventArgs e)
    {
      killMe = true;
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
  }
}
