namespace TelescopeRTS
{
  partial class Form1
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      btnSelectParams = new System.Windows.Forms.Button();
      fileDialog = new System.Windows.Forms.OpenFileDialog();
      lblParamsStatus = new System.Windows.Forms.Label();
      btnSelectRaw = new System.Windows.Forms.Button();
      lblRawStatus = new System.Windows.Forms.Label();
      btnRun = new System.Windows.Forms.Button();
      rtbResults = new System.Windows.Forms.RichTextBox();
      splitMain = new System.Windows.Forms.SplitContainer();
      lblThreadsCaption = new System.Windows.Forms.Label();
      numThreadCount = new System.Windows.Forms.NumericUpDown();
      lstSearchEngine = new System.Windows.Forms.ListBox();
      btnExportLog = new System.Windows.Forms.Button();
      btnStop = new System.Windows.Forms.Button();
      lblScanRateCaption = new System.Windows.Forms.Label();
      numScanRate = new System.Windows.Forms.NumericUpDown();
      splitOutputAndLog = new System.Windows.Forms.SplitContainer();
      splitStatsAndResults = new System.Windows.Forms.SplitContainer();
      lblScansQueuedValue = new System.Windows.Forms.Label();
      lblScansQueuedCaption = new System.Windows.Forms.Label();
      lblSlowestSearchValue = new System.Windows.Forms.Label();
      lblFastestSearchValue = new System.Windows.Forms.Label();
      lblSlowestSearchCaption = new System.Windows.Forms.Label();
      lblFastestSearchCaption = new System.Windows.Forms.Label();
      lblAvgLagTimeValue = new System.Windows.Forms.Label();
      lblAvgLagTimeCaption = new System.Windows.Forms.Label();
      lblActiveThreadsValue = new System.Windows.Forms.Label();
      lblActiveThreadsCaption = new System.Windows.Forms.Label();
      lblAvgSearchTimeValue = new System.Windows.Forms.Label();
      lblMatchedScansValue = new System.Windows.Forms.Label();
      lblTotalScansValue = new System.Windows.Forms.Label();
      lblAvgSearchTimeCaption = new System.Windows.Forms.Label();
      lblMatchedScansCaption = new System.Windows.Forms.Label();
      lblTotalScansCaption = new System.Windows.Forms.Label();
      pnlPaging = new System.Windows.Forms.Panel();
      btnPageBack100 = new System.Windows.Forms.Button();
      btnPageForward100 = new System.Windows.Forms.Button();
      lblPageIndicator = new System.Windows.Forms.Label();
      btnPageNext = new System.Windows.Forms.Button();
      btnPagePrev = new System.Windows.Forms.Button();
      rtbLog = new System.Windows.Forms.RichTextBox();
      ((System.ComponentModel.ISupportInitialize)splitMain).BeginInit();
      splitMain.Panel1.SuspendLayout();
      splitMain.Panel2.SuspendLayout();
      splitMain.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)numThreadCount).BeginInit();
      ((System.ComponentModel.ISupportInitialize)numScanRate).BeginInit();
      ((System.ComponentModel.ISupportInitialize)splitOutputAndLog).BeginInit();
      splitOutputAndLog.Panel1.SuspendLayout();
      splitOutputAndLog.Panel2.SuspendLayout();
      splitOutputAndLog.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)splitStatsAndResults).BeginInit();
      splitStatsAndResults.Panel1.SuspendLayout();
      splitStatsAndResults.Panel2.SuspendLayout();
      splitStatsAndResults.SuspendLayout();
      pnlPaging.SuspendLayout();
      SuspendLayout();
      //
      // Params/raw-file selection and run controls
      //
      // btnSelectParams
      //
      btnSelectParams.Location = new System.Drawing.Point(5, 6);
      btnSelectParams.Margin = new System.Windows.Forms.Padding(5, 6, 5, 6);
      btnSelectParams.Name = "btnSelectParams";
      btnSelectParams.Size = new System.Drawing.Size(120, 40);
      btnSelectParams.TabIndex = 0;
      btnSelectParams.Text = "Select Params";
      btnSelectParams.UseVisualStyleBackColor = true;
      btnSelectParams.Click += btnSelectParams_Click;
      //
      // fileDialog (shared by both "Select Params" and "Select Raw")
      //
      fileDialog.FileName = "openFileDialog1";
      //
      // lblParamsStatus
      //
      lblParamsStatus.AutoSize = true;
      lblParamsStatus.Location = new System.Drawing.Point(135, 14);
      lblParamsStatus.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
      lblParamsStatus.Name = "lblParamsStatus";
      lblParamsStatus.Size = new System.Drawing.Size(95, 25);
      lblParamsStatus.TabIndex = 1;
      lblParamsStatus.Text = "Not Ready";
      //
      // btnSelectRaw
      //
      btnSelectRaw.Enabled = false;
      btnSelectRaw.Location = new System.Drawing.Point(5, 58);
      btnSelectRaw.Margin = new System.Windows.Forms.Padding(5, 6, 5, 6);
      btnSelectRaw.Name = "btnSelectRaw";
      btnSelectRaw.Size = new System.Drawing.Size(120, 40);
      btnSelectRaw.TabIndex = 2;
      btnSelectRaw.Text = "Select Raw";
      btnSelectRaw.UseVisualStyleBackColor = true;
      btnSelectRaw.Click += btnSelectRaw_Click;
      //
      // lblRawStatus
      //
      lblRawStatus.AutoSize = true;
      lblRawStatus.Location = new System.Drawing.Point(135, 66);
      lblRawStatus.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
      lblRawStatus.Name = "lblRawStatus";
      lblRawStatus.Size = new System.Drawing.Size(95, 25);
      lblRawStatus.TabIndex = 3;
      lblRawStatus.Text = "Not Ready";
      //
      // btnRun
      //
      btnRun.Enabled = false;
      btnRun.Location = new System.Drawing.Point(5, 107);
      btnRun.Name = "btnRun";
      btnRun.Size = new System.Drawing.Size(120, 40);
      btnRun.TabIndex = 4;
      btnRun.Text = "Run!";
      btnRun.UseVisualStyleBackColor = true;
      btnRun.Click += btnRun_Click;
      //
      // rtbResults (paged table of per-scan search results)
      //
      rtbResults.Dock = System.Windows.Forms.DockStyle.Fill;
      rtbResults.Font = new System.Drawing.Font("Courier New", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, 0);
      rtbResults.Location = new System.Drawing.Point(0, 64);
      rtbResults.Name = "rtbResults";
      rtbResults.Size = new System.Drawing.Size(1373, 576);
      rtbResults.TabIndex = 5;
      rtbResults.Text = "";
      //
      // splitMain: top-level layout, controls panel vs. everything else
      //
      splitMain.Dock = System.Windows.Forms.DockStyle.Fill;
      splitMain.Location = new System.Drawing.Point(0, 0);
      splitMain.Name = "splitMain";
      splitMain.Orientation = System.Windows.Forms.Orientation.Horizontal;
      //
      // splitMain.Panel1
      //
      splitMain.Panel1.Controls.Add(lblThreadsCaption);
      splitMain.Panel1.Controls.Add(numThreadCount);
      splitMain.Panel1.Controls.Add(lstSearchEngine);
      splitMain.Panel1.Controls.Add(btnExportLog);
      splitMain.Panel1.Controls.Add(btnStop);
      splitMain.Panel1.Controls.Add(lblScanRateCaption);
      splitMain.Panel1.Controls.Add(numScanRate);
      splitMain.Panel1.Controls.Add(btnSelectParams);
      splitMain.Panel1.Controls.Add(btnRun);
      splitMain.Panel1.Controls.Add(lblParamsStatus);
      splitMain.Panel1.Controls.Add(lblRawStatus);
      splitMain.Panel1.Controls.Add(btnSelectRaw);
      //
      // splitMain.Panel2
      //
      splitMain.Panel2.Controls.Add(splitOutputAndLog);
      splitMain.Size = new System.Drawing.Size(1668, 1004);
      splitMain.SplitterDistance = 160;
      splitMain.TabIndex = 6;
      //
      // lblThreadsCaption
      //
      lblThreadsCaption.AutoSize = true;
      lblThreadsCaption.Location = new System.Drawing.Point(716, 115);
      lblThreadsCaption.Name = "lblThreadsCaption";
      lblThreadsCaption.Size = new System.Drawing.Size(207, 25);
      lblThreadsCaption.TabIndex = 11;
      lblThreadsCaption.Text = "Threads (Telescope Only)";
      //
      // numThreadCount: worker thread count for Telescope runs (ignored for Comet, which is single-threaded)
      //
      numThreadCount.Location = new System.Drawing.Point(636, 113);
      numThreadCount.Maximum = new decimal(new int[] { 20, 0, 0, 0 });
      numThreadCount.Minimum = new decimal(new int[] { 1, 0, 0, 0 });
      numThreadCount.Name = "numThreadCount";
      numThreadCount.Size = new System.Drawing.Size(74, 31);
      numThreadCount.TabIndex = 10;
      numThreadCount.Value = new decimal(new int[] { 1, 0, 0, 0 });
      //
      // lstSearchEngine: choice of search engine (Telescope vs. Comet)
      //
      lstSearchEngine.FormattingEnabled = true;
      lstSearchEngine.ItemHeight = 25;
      lstSearchEngine.Items.AddRange(new object[] { "Telescope", "Comet" });
      lstSearchEngine.Location = new System.Drawing.Point(1472, 93);
      lstSearchEngine.Name = "lstSearchEngine";
      lstSearchEngine.Size = new System.Drawing.Size(184, 54);
      lstSearchEngine.TabIndex = 9;
      //
      // btnExportLog
      //
      btnExportLog.Location = new System.Drawing.Point(510, 107);
      btnExportLog.Name = "btnExportLog";
      btnExportLog.Size = new System.Drawing.Size(120, 40);
      btnExportLog.TabIndex = 8;
      btnExportLog.Text = "Export Log";
      btnExportLog.UseVisualStyleBackColor = true;
      btnExportLog.Click += btnExportLog_Click;
      //
      // btnStop (stub: currently only disables itself, see Form1.cs)
      //
      btnStop.Enabled = false;
      btnStop.Location = new System.Drawing.Point(384, 107);
      btnStop.Name = "btnStop";
      btnStop.Size = new System.Drawing.Size(120, 40);
      btnStop.TabIndex = 7;
      btnStop.Text = "Stop!";
      btnStop.UseVisualStyleBackColor = true;
      btnStop.Click += btnStop_Click;
      //
      // lblScanRateCaption
      //
      lblScanRateCaption.AutoSize = true;
      lblScanRateCaption.Location = new System.Drawing.Point(236, 115);
      lblScanRateCaption.Name = "lblScanRateCaption";
      lblScanRateCaption.Size = new System.Drawing.Size(152, 25);
      lblScanRateCaption.TabIndex = 6;
      lblScanRateCaption.Text = "scans per second.";
      //
      // numScanRate: target streaming rate (Hz) for the simulated real-time run
      //
      numScanRate.Enabled = false;
      numScanRate.Location = new System.Drawing.Point(135, 113);
      numScanRate.Maximum = new decimal(new int[] { 500000, 0, 0, 0 });
      numScanRate.Minimum = new decimal(new int[] { 1, 0, 0, 0 });
      numScanRate.Name = "numScanRate";
      numScanRate.Size = new System.Drawing.Size(95, 31);
      numScanRate.TabIndex = 5;
      numScanRate.Value = new decimal(new int[] { 20, 0, 0, 0 });
      //
      // splitOutputAndLog: stats+results (top) vs. the message log (bottom)
      //
      splitOutputAndLog.Dock = System.Windows.Forms.DockStyle.Fill;
      splitOutputAndLog.Location = new System.Drawing.Point(0, 0);
      splitOutputAndLog.Name = "splitOutputAndLog";
      splitOutputAndLog.Orientation = System.Windows.Forms.Orientation.Horizontal;
      //
      // splitOutputAndLog.Panel1
      //
      splitOutputAndLog.Panel1.Controls.Add(splitStatsAndResults);
      //
      // splitOutputAndLog.Panel2
      //
      splitOutputAndLog.Panel2.Controls.Add(rtbLog);
      splitOutputAndLog.Size = new System.Drawing.Size(1668, 840);
      splitOutputAndLog.SplitterDistance = 640;
      splitOutputAndLog.TabIndex = 7;
      //
      // splitStatsAndResults: summary stat labels (left) vs. paged results (right)
      //
      splitStatsAndResults.Dock = System.Windows.Forms.DockStyle.Fill;
      splitStatsAndResults.Location = new System.Drawing.Point(0, 0);
      splitStatsAndResults.Name = "splitStatsAndResults";
      //
      // splitStatsAndResults.Panel1
      //
      splitStatsAndResults.Panel1.Controls.Add(lblScansQueuedValue);
      splitStatsAndResults.Panel1.Controls.Add(lblScansQueuedCaption);
      splitStatsAndResults.Panel1.Controls.Add(lblSlowestSearchValue);
      splitStatsAndResults.Panel1.Controls.Add(lblFastestSearchValue);
      splitStatsAndResults.Panel1.Controls.Add(lblSlowestSearchCaption);
      splitStatsAndResults.Panel1.Controls.Add(lblFastestSearchCaption);
      splitStatsAndResults.Panel1.Controls.Add(lblAvgLagTimeValue);
      splitStatsAndResults.Panel1.Controls.Add(lblAvgLagTimeCaption);
      splitStatsAndResults.Panel1.Controls.Add(lblActiveThreadsValue);
      splitStatsAndResults.Panel1.Controls.Add(lblActiveThreadsCaption);
      splitStatsAndResults.Panel1.Controls.Add(lblAvgSearchTimeValue);
      splitStatsAndResults.Panel1.Controls.Add(lblMatchedScansValue);
      splitStatsAndResults.Panel1.Controls.Add(lblTotalScansValue);
      splitStatsAndResults.Panel1.Controls.Add(lblAvgSearchTimeCaption);
      splitStatsAndResults.Panel1.Controls.Add(lblMatchedScansCaption);
      splitStatsAndResults.Panel1.Controls.Add(lblTotalScansCaption);
      //
      // splitStatsAndResults.Panel2
      //
      splitStatsAndResults.Panel2.Controls.Add(rtbResults);
      splitStatsAndResults.Panel2.Controls.Add(pnlPaging);
      splitStatsAndResults.Size = new System.Drawing.Size(1668, 640);
      splitStatsAndResults.SplitterDistance = 291;
      splitStatsAndResults.TabIndex = 6;
      //
      // lblScansQueuedValue
      //
      lblScansQueuedValue.AutoSize = true;
      lblScansQueuedValue.Location = new System.Drawing.Point(188, 214);
      lblScansQueuedValue.Name = "lblScansQueuedValue";
      lblScansQueuedValue.Size = new System.Drawing.Size(22, 25);
      lblScansQueuedValue.TabIndex = 15;
      lblScansQueuedValue.Text = "0";
      //
      // lblScansQueuedCaption
      //
      lblScansQueuedCaption.AutoSize = true;
      lblScansQueuedCaption.Location = new System.Drawing.Point(16, 214);
      lblScansQueuedCaption.Name = "lblScansQueuedCaption";
      lblScansQueuedCaption.Size = new System.Drawing.Size(129, 25);
      lblScansQueuedCaption.TabIndex = 14;
      lblScansQueuedCaption.Text = "Scans Queued:";
      //
      // Run summary statistics labels (caption/value pairs)
      //
      // lblSlowestSearchValue
      //
      lblSlowestSearchValue.AutoSize = true;
      lblSlowestSearchValue.Location = new System.Drawing.Point(188, 171);
      lblSlowestSearchValue.Name = "lblSlowestSearchValue";
      lblSlowestSearchValue.Size = new System.Drawing.Size(22, 25);
      lblSlowestSearchValue.TabIndex = 13;
      lblSlowestSearchValue.Text = "0";
      //
      // lblFastestSearchValue
      //
      lblFastestSearchValue.AutoSize = true;
      lblFastestSearchValue.Location = new System.Drawing.Point(188, 146);
      lblFastestSearchValue.Name = "lblFastestSearchValue";
      lblFastestSearchValue.Size = new System.Drawing.Size(22, 25);
      lblFastestSearchValue.TabIndex = 12;
      lblFastestSearchValue.Text = "0";
      //
      // lblSlowestSearchCaption
      //
      lblSlowestSearchCaption.AutoSize = true;
      lblSlowestSearchCaption.Location = new System.Drawing.Point(16, 171);
      lblSlowestSearchCaption.Name = "lblSlowestSearchCaption";
      lblSlowestSearchCaption.Size = new System.Drawing.Size(134, 25);
      lblSlowestSearchCaption.TabIndex = 11;
      lblSlowestSearchCaption.Text = "Slowest Search:";
      //
      // lblFastestSearchCaption
      //
      lblFastestSearchCaption.AutoSize = true;
      lblFastestSearchCaption.Location = new System.Drawing.Point(16, 146);
      lblFastestSearchCaption.Name = "lblFastestSearchCaption";
      lblFastestSearchCaption.Size = new System.Drawing.Size(127, 25);
      lblFastestSearchCaption.TabIndex = 10;
      lblFastestSearchCaption.Text = "Fastest Search:";
      //
      // lblAvgLagTimeValue
      //
      lblAvgLagTimeValue.AutoSize = true;
      lblAvgLagTimeValue.Location = new System.Drawing.Point(188, 121);
      lblAvgLagTimeValue.Name = "lblAvgLagTimeValue";
      lblAvgLagTimeValue.Size = new System.Drawing.Size(22, 25);
      lblAvgLagTimeValue.TabIndex = 9;
      lblAvgLagTimeValue.Text = "0";
      //
      // lblAvgLagTimeCaption
      //
      lblAvgLagTimeCaption.AutoSize = true;
      lblAvgLagTimeCaption.Location = new System.Drawing.Point(16, 121);
      lblAvgLagTimeCaption.Name = "lblAvgLagTimeCaption";
      lblAvgLagTimeCaption.Size = new System.Drawing.Size(124, 25);
      lblAvgLagTimeCaption.TabIndex = 8;
      lblAvgLagTimeCaption.Text = "Avg Lag Time:";
      //
      // lblActiveThreadsValue
      //
      lblActiveThreadsValue.AutoSize = true;
      lblActiveThreadsValue.Location = new System.Drawing.Point(188, 96);
      lblActiveThreadsValue.Name = "lblActiveThreadsValue";
      lblActiveThreadsValue.Size = new System.Drawing.Size(22, 25);
      lblActiveThreadsValue.TabIndex = 7;
      lblActiveThreadsValue.Text = "0";
      //
      // lblActiveThreadsCaption
      //
      lblActiveThreadsCaption.AutoSize = true;
      lblActiveThreadsCaption.Location = new System.Drawing.Point(16, 96);
      lblActiveThreadsCaption.Name = "lblActiveThreadsCaption";
      lblActiveThreadsCaption.Size = new System.Drawing.Size(131, 25);
      lblActiveThreadsCaption.TabIndex = 6;
      lblActiveThreadsCaption.Text = "Active Threads:";
      //
      // lblAvgSearchTimeValue
      //
      lblAvgSearchTimeValue.AutoSize = true;
      lblAvgSearchTimeValue.Location = new System.Drawing.Point(188, 72);
      lblAvgSearchTimeValue.Name = "lblAvgSearchTimeValue";
      lblAvgSearchTimeValue.Size = new System.Drawing.Size(22, 25);
      lblAvgSearchTimeValue.TabIndex = 5;
      lblAvgSearchTimeValue.Text = "0";
      //
      // lblMatchedScansValue
      //
      lblMatchedScansValue.AutoSize = true;
      lblMatchedScansValue.Location = new System.Drawing.Point(188, 47);
      lblMatchedScansValue.Name = "lblMatchedScansValue";
      lblMatchedScansValue.Size = new System.Drawing.Size(22, 25);
      lblMatchedScansValue.TabIndex = 4;
      lblMatchedScansValue.Text = "0";
      //
      // lblTotalScansValue
      //
      lblTotalScansValue.AutoSize = true;
      lblTotalScansValue.Location = new System.Drawing.Point(188, 22);
      lblTotalScansValue.Name = "lblTotalScansValue";
      lblTotalScansValue.Size = new System.Drawing.Size(22, 25);
      lblTotalScansValue.TabIndex = 3;
      lblTotalScansValue.Text = "0";
      //
      // lblAvgSearchTimeCaption
      //
      lblAvgSearchTimeCaption.AutoSize = true;
      lblAvgSearchTimeCaption.Location = new System.Drawing.Point(16, 71);
      lblAvgSearchTimeCaption.Name = "lblAvgSearchTimeCaption";
      lblAvgSearchTimeCaption.Size = new System.Drawing.Size(148, 25);
      lblAvgSearchTimeCaption.TabIndex = 2;
      lblAvgSearchTimeCaption.Text = "Avg Search Time:";
      //
      // lblMatchedScansCaption
      //
      lblMatchedScansCaption.AutoSize = true;
      lblMatchedScansCaption.Location = new System.Drawing.Point(16, 46);
      lblMatchedScansCaption.Name = "lblMatchedScansCaption";
      lblMatchedScansCaption.Size = new System.Drawing.Size(135, 25);
      lblMatchedScansCaption.TabIndex = 1;
      lblMatchedScansCaption.Text = "Matched Scans:";
      //
      // lblTotalScansCaption
      //
      lblTotalScansCaption.AutoSize = true;
      lblTotalScansCaption.Location = new System.Drawing.Point(16, 21);
      lblTotalScansCaption.Name = "lblTotalScansCaption";
      lblTotalScansCaption.Size = new System.Drawing.Size(103, 25);
      lblTotalScansCaption.TabIndex = 0;
      lblTotalScansCaption.Text = "Total Scans:";
      //
      // pnlPaging: results paging controls, docked above rtbResults
      //
      pnlPaging.Controls.Add(btnPageBack100);
      pnlPaging.Controls.Add(btnPageForward100);
      pnlPaging.Controls.Add(lblPageIndicator);
      pnlPaging.Controls.Add(btnPageNext);
      pnlPaging.Controls.Add(btnPagePrev);
      pnlPaging.Dock = System.Windows.Forms.DockStyle.Top;
      pnlPaging.Location = new System.Drawing.Point(0, 0);
      pnlPaging.Name = "pnlPaging";
      pnlPaging.Size = new System.Drawing.Size(1373, 64);
      pnlPaging.TabIndex = 6;
      //
      // btnPageBack100 ("<<": jump back 100 pages)
      //
      btnPageBack100.Location = new System.Drawing.Point(3, 3);
      btnPageBack100.Name = "btnPageBack100";
      btnPageBack100.Size = new System.Drawing.Size(48, 48);
      btnPageBack100.TabIndex = 4;
      btnPageBack100.Text = "<<";
      btnPageBack100.UseVisualStyleBackColor = true;
      btnPageBack100.Click += btnPageBack100_Click;
      //
      // btnPageForward100 (">>": jump forward 100 pages)
      //
      btnPageForward100.Location = new System.Drawing.Point(291, 3);
      btnPageForward100.Name = "btnPageForward100";
      btnPageForward100.Size = new System.Drawing.Size(48, 48);
      btnPageForward100.TabIndex = 3;
      btnPageForward100.Text = ">>";
      btnPageForward100.UseVisualStyleBackColor = true;
      btnPageForward100.Click += btnPageForward100_Click;
      //
      // lblPageIndicator ("current/max" page display)
      //
      lblPageIndicator.Font = new System.Drawing.Font("Segoe UI", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, 0);
      lblPageIndicator.Location = new System.Drawing.Point(111, 12);
      lblPageIndicator.Name = "lblPageIndicator";
      lblPageIndicator.Size = new System.Drawing.Size(120, 26);
      lblPageIndicator.TabIndex = 2;
      lblPageIndicator.Text = "0/0";
      lblPageIndicator.TextAlign = System.Drawing.ContentAlignment.TopCenter;
      //
      // btnPageNext (">": next page)
      //
      btnPageNext.Location = new System.Drawing.Point(237, 3);
      btnPageNext.Name = "btnPageNext";
      btnPageNext.Size = new System.Drawing.Size(48, 48);
      btnPageNext.TabIndex = 1;
      btnPageNext.Text = ">";
      btnPageNext.UseVisualStyleBackColor = true;
      btnPageNext.Click += btnPageNext_Click;
      //
      // btnPagePrev ("<": previous page)
      //
      btnPagePrev.Location = new System.Drawing.Point(57, 3);
      btnPagePrev.Name = "btnPagePrev";
      btnPagePrev.Size = new System.Drawing.Size(48, 48);
      btnPagePrev.TabIndex = 0;
      btnPagePrev.Text = "<";
      btnPagePrev.UseVisualStyleBackColor = true;
      btnPagePrev.Click += btnPagePrev_Click;
      //
      // rtbLog: running log/message panel
      //
      rtbLog.Dock = System.Windows.Forms.DockStyle.Fill;
      rtbLog.Location = new System.Drawing.Point(0, 0);
      rtbLog.Name = "rtbLog";
      rtbLog.Size = new System.Drawing.Size(1668, 196);
      rtbLog.TabIndex = 0;
      rtbLog.Text = "";
      //
      // Form1
      //
      AutoScaleDimensions = new System.Drawing.SizeF(10F, 25F);
      AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      ClientSize = new System.Drawing.Size(1668, 1004);
      Controls.Add(splitMain);
      Margin = new System.Windows.Forms.Padding(5, 6, 5, 6);
      Name = "Form1";
      Text = "Form1";
      splitMain.Panel1.ResumeLayout(false);
      splitMain.Panel1.PerformLayout();
      splitMain.Panel2.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)splitMain).EndInit();
      splitMain.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)numThreadCount).EndInit();
      ((System.ComponentModel.ISupportInitialize)numScanRate).EndInit();
      splitOutputAndLog.Panel1.ResumeLayout(false);
      splitOutputAndLog.Panel2.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)splitOutputAndLog).EndInit();
      splitOutputAndLog.ResumeLayout(false);
      splitStatsAndResults.Panel1.ResumeLayout(false);
      splitStatsAndResults.Panel1.PerformLayout();
      splitStatsAndResults.Panel2.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)splitStatsAndResults).EndInit();
      splitStatsAndResults.ResumeLayout(false);
      pnlPaging.ResumeLayout(false);
      ResumeLayout(false);
    }

    #endregion

    private System.Windows.Forms.Button btnSelectParams;
    private System.Windows.Forms.OpenFileDialog fileDialog;
    private System.Windows.Forms.Label lblParamsStatus;
    private System.Windows.Forms.Button btnSelectRaw;
    private System.Windows.Forms.Label lblRawStatus;
    private System.Windows.Forms.Button btnRun;
    private System.Windows.Forms.RichTextBox rtbResults;
    private System.Windows.Forms.SplitContainer splitMain;
    private System.Windows.Forms.Label lblThreadsCaption;
    private System.Windows.Forms.NumericUpDown numThreadCount;
    private System.Windows.Forms.NumericUpDown numScanRate;
    private System.Windows.Forms.Label lblScanRateCaption;
    private System.Windows.Forms.SplitContainer splitStatsAndResults;
    private System.Windows.Forms.Button btnStop;
    private System.Windows.Forms.Label lblTotalScansCaption;
    private System.Windows.Forms.Label lblAvgSearchTimeCaption;
    private System.Windows.Forms.Label lblMatchedScansCaption;
    private System.Windows.Forms.Label lblAvgSearchTimeValue;
    private System.Windows.Forms.Label lblMatchedScansValue;
    private System.Windows.Forms.Label lblTotalScansValue;
    private System.Windows.Forms.Panel pnlPaging;
    private System.Windows.Forms.Label lblPageIndicator;
    private System.Windows.Forms.Button btnPageNext;
    private System.Windows.Forms.Button btnPagePrev;
    private System.Windows.Forms.Button btnPageForward100;
    private System.Windows.Forms.Button btnPageBack100;
    private System.Windows.Forms.Label lblActiveThreadsCaption;
    private System.Windows.Forms.Label lblActiveThreadsValue;
    private System.Windows.Forms.Label lblAvgLagTimeValue;
    private System.Windows.Forms.Label lblAvgLagTimeCaption;
    private System.Windows.Forms.Label lblSlowestSearchValue;
    private System.Windows.Forms.Label lblFastestSearchValue;
    private System.Windows.Forms.Label lblSlowestSearchCaption;
    private System.Windows.Forms.Label lblFastestSearchCaption;
    private System.Windows.Forms.Button btnExportLog;
    private System.Windows.Forms.ListBox lstSearchEngine;
    private System.Windows.Forms.SplitContainer splitOutputAndLog;
    private System.Windows.Forms.RichTextBox rtbLog;
    private System.Windows.Forms.Label lblScansQueuedValue;
    private System.Windows.Forms.Label lblScansQueuedCaption;
  }
}
