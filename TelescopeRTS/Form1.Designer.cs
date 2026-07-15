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
      grpAlgorithm = new System.Windows.Forms.GroupBox();
      lstSearchEngine = new System.Windows.Forms.ListBox();
      btnLockAlgorithm = new System.Windows.Forms.Button();
      grpParameters = new System.Windows.Forms.GroupBox();
      grpRawData = new System.Windows.Forms.GroupBox();
      grpRunControls = new System.Windows.Forms.GroupBox();
      lblScanRateCaption = new System.Windows.Forms.Label();
      numScanRate = new System.Windows.Forms.NumericUpDown();
      lblThreadsCaption = new System.Windows.Forms.Label();
      numThreadCount = new System.Windows.Forms.NumericUpDown();
      btnStop = new System.Windows.Forms.Button();
      grpExport = new System.Windows.Forms.GroupBox();
      btnExportLog = new System.Windows.Forms.Button();
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
      grpAlgorithm.SuspendLayout();
      grpParameters.SuspendLayout();
      grpRawData.SuspendLayout();
      grpRunControls.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)numScanRate).BeginInit();
      ((System.ComponentModel.ISupportInitialize)numThreadCount).BeginInit();
      grpExport.SuspendLayout();
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
      // btnSelectParams
      // 
      btnSelectParams.Location = new System.Drawing.Point(15, 25);
      btnSelectParams.Margin = new System.Windows.Forms.Padding(5, 6, 5, 6);
      btnSelectParams.Name = "btnSelectParams";
      btnSelectParams.Size = new System.Drawing.Size(230, 44);
      btnSelectParams.TabIndex = 0;
      btnSelectParams.Text = "Select Params File";
      btnSelectParams.UseVisualStyleBackColor = true;
      btnSelectParams.Click += btnSelectParams_Click;
      // 
      // fileDialog
      // 
      fileDialog.FileName = "openFileDialog1";
      // 
      // lblParamsStatus
      // 
      lblParamsStatus.AutoSize = true;
      lblParamsStatus.Location = new System.Drawing.Point(15, 80);
      lblParamsStatus.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
      lblParamsStatus.MaximumSize = new System.Drawing.Size(230, 0);
      lblParamsStatus.Name = "lblParamsStatus";
      lblParamsStatus.Size = new System.Drawing.Size(95, 25);
      lblParamsStatus.TabIndex = 1;
      lblParamsStatus.Text = "Not Ready";
      // 
      // btnSelectRaw
      // 
      btnSelectRaw.Location = new System.Drawing.Point(15, 25);
      btnSelectRaw.Margin = new System.Windows.Forms.Padding(5, 6, 5, 6);
      btnSelectRaw.Name = "btnSelectRaw";
      btnSelectRaw.Size = new System.Drawing.Size(230, 44);
      btnSelectRaw.TabIndex = 0;
      btnSelectRaw.Text = "Select Raw File";
      btnSelectRaw.UseVisualStyleBackColor = true;
      btnSelectRaw.Click += btnSelectRaw_Click;
      // 
      // lblRawStatus
      // 
      lblRawStatus.AutoSize = true;
      lblRawStatus.Location = new System.Drawing.Point(15, 80);
      lblRawStatus.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
      lblRawStatus.MaximumSize = new System.Drawing.Size(230, 0);
      lblRawStatus.Name = "lblRawStatus";
      lblRawStatus.Size = new System.Drawing.Size(95, 25);
      lblRawStatus.TabIndex = 1;
      lblRawStatus.Text = "Not Ready";
      // 
      // btnRun
      // 
      btnRun.Location = new System.Drawing.Point(303, 25);
      btnRun.Name = "btnRun";
      btnRun.Size = new System.Drawing.Size(137, 44);
      btnRun.TabIndex = 4;
      btnRun.Text = "Run!";
      btnRun.UseVisualStyleBackColor = true;
      btnRun.Click += btnRun_Click;
      // 
      // rtbResults
      // 
      rtbResults.Dock = System.Windows.Forms.DockStyle.Fill;
      rtbResults.Font = new System.Drawing.Font("Courier New", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, 0);
      rtbResults.Location = new System.Drawing.Point(0, 64);
      rtbResults.Name = "rtbResults";
      rtbResults.Size = new System.Drawing.Size(1299, 576);
      rtbResults.TabIndex = 5;
      rtbResults.Text = "";
      // 
      // splitMain
      // 
      splitMain.Dock = System.Windows.Forms.DockStyle.Fill;
      splitMain.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
      splitMain.IsSplitterFixed = true;
      splitMain.Location = new System.Drawing.Point(0, 0);
      splitMain.Name = "splitMain";
      splitMain.Orientation = System.Windows.Forms.Orientation.Horizontal;
      // 
      // splitMain.Panel1
      // 
      splitMain.Panel1.Controls.Add(grpAlgorithm);
      splitMain.Panel1.Controls.Add(grpParameters);
      splitMain.Panel1.Controls.Add(grpRawData);
      splitMain.Panel1.Controls.Add(grpRunControls);
      splitMain.Panel1.Controls.Add(grpExport);
      // 
      // splitMain.Panel2
      // 
      splitMain.Panel2.Controls.Add(splitOutputAndLog);
      splitMain.Size = new System.Drawing.Size(1578, 1144);
      splitMain.SplitterDistance = 175;
      splitMain.TabIndex = 6;
      // 
      // grpAlgorithm
      // 
      grpAlgorithm.Controls.Add(lstSearchEngine);
      grpAlgorithm.Controls.Add(btnLockAlgorithm);
      grpAlgorithm.Location = new System.Drawing.Point(10, 8);
      grpAlgorithm.Name = "grpAlgorithm";
      grpAlgorithm.Size = new System.Drawing.Size(220, 155);
      grpAlgorithm.TabIndex = 0;
      grpAlgorithm.TabStop = false;
      grpAlgorithm.Text = "1. Algorithm";
      // 
      // lstSearchEngine
      // 
      lstSearchEngine.FormattingEnabled = true;
      lstSearchEngine.ItemHeight = 25;
      lstSearchEngine.Items.AddRange(new object[] { "Telescope", "Comet" });
      lstSearchEngine.Location = new System.Drawing.Point(15, 25);
      lstSearchEngine.Name = "lstSearchEngine";
      lstSearchEngine.Size = new System.Drawing.Size(190, 54);
      lstSearchEngine.TabIndex = 0;
      // 
      // btnLockAlgorithm
      // 
      btnLockAlgorithm.Location = new System.Drawing.Point(15, 95);
      btnLockAlgorithm.Name = "btnLockAlgorithm";
      btnLockAlgorithm.Size = new System.Drawing.Size(190, 44);
      btnLockAlgorithm.TabIndex = 1;
      btnLockAlgorithm.Text = "Lock Algorithm";
      btnLockAlgorithm.UseVisualStyleBackColor = true;
      btnLockAlgorithm.Click += btnLockAlgorithm_Click;
      // 
      // grpParameters
      // 
      grpParameters.Controls.Add(btnSelectParams);
      grpParameters.Controls.Add(lblParamsStatus);
      grpParameters.Location = new System.Drawing.Point(240, 8);
      grpParameters.Name = "grpParameters";
      grpParameters.Size = new System.Drawing.Size(260, 155);
      grpParameters.TabIndex = 1;
      grpParameters.TabStop = false;
      grpParameters.Text = "2. Parameters";
      // 
      // grpRawData
      // 
      grpRawData.Controls.Add(btnSelectRaw);
      grpRawData.Controls.Add(lblRawStatus);
      grpRawData.Location = new System.Drawing.Point(510, 8);
      grpRawData.Name = "grpRawData";
      grpRawData.Size = new System.Drawing.Size(260, 155);
      grpRawData.TabIndex = 2;
      grpRawData.TabStop = false;
      grpRawData.Text = "3. Raw Data";
      // 
      // grpRunControls
      // 
      grpRunControls.Controls.Add(lblScanRateCaption);
      grpRunControls.Controls.Add(numScanRate);
      grpRunControls.Controls.Add(lblThreadsCaption);
      grpRunControls.Controls.Add(numThreadCount);
      grpRunControls.Controls.Add(btnRun);
      grpRunControls.Controls.Add(btnStop);
      grpRunControls.Location = new System.Drawing.Point(780, 8);
      grpRunControls.Name = "grpRunControls";
      grpRunControls.Size = new System.Drawing.Size(460, 155);
      grpRunControls.TabIndex = 3;
      grpRunControls.TabStop = false;
      grpRunControls.Text = "4. Run";
      // 
      // lblScanRateCaption
      // 
      lblScanRateCaption.AutoSize = true;
      lblScanRateCaption.Location = new System.Drawing.Point(105, 33);
      lblScanRateCaption.Name = "lblScanRateCaption";
      lblScanRateCaption.Size = new System.Drawing.Size(152, 25);
      lblScanRateCaption.TabIndex = 1;
      lblScanRateCaption.Text = "scans per second.";
      // 
      // numScanRate
      // 
      numScanRate.Enabled = false;
      numScanRate.Location = new System.Drawing.Point(15, 30);
      numScanRate.Maximum = new decimal(new int[] { 500000, 0, 0, 0 });
      numScanRate.Minimum = new decimal(new int[] { 1, 0, 0, 0 });
      numScanRate.Name = "numScanRate";
      numScanRate.Size = new System.Drawing.Size(80, 31);
      numScanRate.TabIndex = 0;
      numScanRate.Value = new decimal(new int[] { 20, 0, 0, 0 });
      // 
      // lblThreadsCaption
      // 
      lblThreadsCaption.AutoSize = true;
      lblThreadsCaption.Location = new System.Drawing.Point(105, 78);
      lblThreadsCaption.Name = "lblThreadsCaption";
      lblThreadsCaption.Size = new System.Drawing.Size(201, 25);
      lblThreadsCaption.TabIndex = 3;
      lblThreadsCaption.Text = "threads (Telescope only)";
      // 
      // numThreadCount
      // 
      numThreadCount.Location = new System.Drawing.Point(15, 75);
      numThreadCount.Maximum = new decimal(new int[] { 20, 0, 0, 0 });
      numThreadCount.Minimum = new decimal(new int[] { 1, 0, 0, 0 });
      numThreadCount.Name = "numThreadCount";
      numThreadCount.Size = new System.Drawing.Size(80, 31);
      numThreadCount.TabIndex = 2;
      numThreadCount.Value = new decimal(new int[] { 1, 0, 0, 0 });
      // 
      // btnStop
      // 
      btnStop.Enabled = false;
      btnStop.Location = new System.Drawing.Point(303, 80);
      btnStop.Name = "btnStop";
      btnStop.Size = new System.Drawing.Size(137, 44);
      btnStop.TabIndex = 5;
      btnStop.Text = "Stop!";
      btnStop.UseVisualStyleBackColor = true;
      btnStop.Click += btnStop_Click;
      // 
      // grpExport
      // 
      grpExport.Controls.Add(btnExportLog);
      grpExport.Location = new System.Drawing.Point(1250, 8);
      grpExport.Name = "grpExport";
      grpExport.Size = new System.Drawing.Size(160, 155);
      grpExport.TabIndex = 4;
      grpExport.TabStop = false;
      grpExport.Text = "Export";
      // 
      // btnExportLog
      // 
      btnExportLog.Location = new System.Drawing.Point(15, 55);
      btnExportLog.Name = "btnExportLog";
      btnExportLog.Size = new System.Drawing.Size(130, 44);
      btnExportLog.TabIndex = 0;
      btnExportLog.Text = "Export Log";
      btnExportLog.UseVisualStyleBackColor = true;
      btnExportLog.Click += btnExportLog_Click;
      // 
      // splitOutputAndLog
      // 
      splitOutputAndLog.Dock = System.Windows.Forms.DockStyle.Fill;
      splitOutputAndLog.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
      splitOutputAndLog.IsSplitterFixed = true;
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
      splitOutputAndLog.Size = new System.Drawing.Size(1578, 965);
      splitOutputAndLog.SplitterDistance = 640;
      splitOutputAndLog.TabIndex = 7;
      // 
      // splitStatsAndResults
      // 
      splitStatsAndResults.Dock = System.Windows.Forms.DockStyle.Fill;
      splitStatsAndResults.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
      splitStatsAndResults.IsSplitterFixed = true;
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
      splitStatsAndResults.Size = new System.Drawing.Size(1578, 640);
      splitStatsAndResults.SplitterDistance = 275;
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
      // pnlPaging
      // 
      pnlPaging.Controls.Add(btnPageBack100);
      pnlPaging.Controls.Add(btnPageForward100);
      pnlPaging.Controls.Add(lblPageIndicator);
      pnlPaging.Controls.Add(btnPageNext);
      pnlPaging.Controls.Add(btnPagePrev);
      pnlPaging.Dock = System.Windows.Forms.DockStyle.Top;
      pnlPaging.Location = new System.Drawing.Point(0, 0);
      pnlPaging.Name = "pnlPaging";
      pnlPaging.Size = new System.Drawing.Size(1299, 64);
      pnlPaging.TabIndex = 6;
      // 
      // btnPageBack100
      // 
      btnPageBack100.Location = new System.Drawing.Point(3, 3);
      btnPageBack100.Name = "btnPageBack100";
      btnPageBack100.Size = new System.Drawing.Size(48, 48);
      btnPageBack100.TabIndex = 4;
      btnPageBack100.Text = "<<";
      btnPageBack100.UseVisualStyleBackColor = true;
      btnPageBack100.Click += btnPageBack100_Click;
      // 
      // btnPageForward100
      // 
      btnPageForward100.Location = new System.Drawing.Point(291, 3);
      btnPageForward100.Name = "btnPageForward100";
      btnPageForward100.Size = new System.Drawing.Size(48, 48);
      btnPageForward100.TabIndex = 3;
      btnPageForward100.Text = ">>";
      btnPageForward100.UseVisualStyleBackColor = true;
      btnPageForward100.Click += btnPageForward100_Click;
      // 
      // lblPageIndicator
      // 
      lblPageIndicator.Font = new System.Drawing.Font("Segoe UI", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, 0);
      lblPageIndicator.Location = new System.Drawing.Point(111, 12);
      lblPageIndicator.Name = "lblPageIndicator";
      lblPageIndicator.Size = new System.Drawing.Size(120, 26);
      lblPageIndicator.TabIndex = 2;
      lblPageIndicator.Text = "0/0";
      lblPageIndicator.TextAlign = System.Drawing.ContentAlignment.TopCenter;
      // 
      // btnPageNext
      // 
      btnPageNext.Location = new System.Drawing.Point(237, 3);
      btnPageNext.Name = "btnPageNext";
      btnPageNext.Size = new System.Drawing.Size(48, 48);
      btnPageNext.TabIndex = 1;
      btnPageNext.Text = ">";
      btnPageNext.UseVisualStyleBackColor = true;
      btnPageNext.Click += btnPageNext_Click;
      // 
      // btnPagePrev
      // 
      btnPagePrev.Location = new System.Drawing.Point(57, 3);
      btnPagePrev.Name = "btnPagePrev";
      btnPagePrev.Size = new System.Drawing.Size(48, 48);
      btnPagePrev.TabIndex = 0;
      btnPagePrev.Text = "<";
      btnPagePrev.UseVisualStyleBackColor = true;
      btnPagePrev.Click += btnPagePrev_Click;
      // 
      // rtbLog
      // 
      rtbLog.Dock = System.Windows.Forms.DockStyle.Fill;
      rtbLog.Location = new System.Drawing.Point(0, 0);
      rtbLog.Name = "rtbLog";
      rtbLog.Size = new System.Drawing.Size(1578, 321);
      rtbLog.TabIndex = 0;
      rtbLog.Text = "";
      // 
      // Form1
      // 
      AutoScaleDimensions = new System.Drawing.SizeF(10F, 25F);
      AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      ClientSize = new System.Drawing.Size(1578, 1144);
      Controls.Add(splitMain);
      Margin = new System.Windows.Forms.Padding(5, 6, 5, 6);
      Name = "Form1";
      Text = "Form1";
      splitMain.Panel1.ResumeLayout(false);
      splitMain.Panel2.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)splitMain).EndInit();
      splitMain.ResumeLayout(false);
      grpAlgorithm.ResumeLayout(false);
      grpParameters.ResumeLayout(false);
      grpParameters.PerformLayout();
      grpRawData.ResumeLayout(false);
      grpRawData.PerformLayout();
      grpRunControls.ResumeLayout(false);
      grpRunControls.PerformLayout();
      ((System.ComponentModel.ISupportInitialize)numScanRate).EndInit();
      ((System.ComponentModel.ISupportInitialize)numThreadCount).EndInit();
      grpExport.ResumeLayout(false);
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
    private System.Windows.Forms.GroupBox grpAlgorithm;
    private System.Windows.Forms.Button btnLockAlgorithm;
    private System.Windows.Forms.GroupBox grpParameters;
    private System.Windows.Forms.GroupBox grpRawData;
    private System.Windows.Forms.GroupBox grpRunControls;
    private System.Windows.Forms.GroupBox grpExport;
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
