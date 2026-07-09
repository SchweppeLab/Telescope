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
      button1 = new System.Windows.Forms.Button();
      openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
      label1 = new System.Windows.Forms.Label();
      button2 = new System.Windows.Forms.Button();
      label2 = new System.Windows.Forms.Label();
      button3 = new System.Windows.Forms.Button();
      richTextBox1 = new System.Windows.Forms.RichTextBox();
      splitContainer1 = new System.Windows.Forms.SplitContainer();
      searchListBox = new System.Windows.Forms.ListBox();
      button9 = new System.Windows.Forms.Button();
      button4 = new System.Windows.Forms.Button();
      label3 = new System.Windows.Forms.Label();
      numericUpDown1 = new System.Windows.Forms.NumericUpDown();
      splitContainer3 = new System.Windows.Forms.SplitContainer();
      splitContainer2 = new System.Windows.Forms.SplitContainer();
      label18 = new System.Windows.Forms.Label();
      label17 = new System.Windows.Forms.Label();
      label16 = new System.Windows.Forms.Label();
      label15 = new System.Windows.Forms.Label();
      label14 = new System.Windows.Forms.Label();
      label13 = new System.Windows.Forms.Label();
      label12 = new System.Windows.Forms.Label();
      label11 = new System.Windows.Forms.Label();
      label9 = new System.Windows.Forms.Label();
      label8 = new System.Windows.Forms.Label();
      label7 = new System.Windows.Forms.Label();
      label6 = new System.Windows.Forms.Label();
      label5 = new System.Windows.Forms.Label();
      label4 = new System.Windows.Forms.Label();
      panel1 = new System.Windows.Forms.Panel();
      button8 = new System.Windows.Forms.Button();
      button7 = new System.Windows.Forms.Button();
      label10 = new System.Windows.Forms.Label();
      button6 = new System.Windows.Forms.Button();
      button5 = new System.Windows.Forms.Button();
      rtbMessage = new System.Windows.Forms.RichTextBox();
      label19 = new System.Windows.Forms.Label();
      label20 = new System.Windows.Forms.Label();
      ((System.ComponentModel.ISupportInitialize)splitContainer1).BeginInit();
      splitContainer1.Panel1.SuspendLayout();
      splitContainer1.Panel2.SuspendLayout();
      splitContainer1.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)numericUpDown1).BeginInit();
      ((System.ComponentModel.ISupportInitialize)splitContainer3).BeginInit();
      splitContainer3.Panel1.SuspendLayout();
      splitContainer3.Panel2.SuspendLayout();
      splitContainer3.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)splitContainer2).BeginInit();
      splitContainer2.Panel1.SuspendLayout();
      splitContainer2.Panel2.SuspendLayout();
      splitContainer2.SuspendLayout();
      panel1.SuspendLayout();
      SuspendLayout();
      // 
      // button1
      // 
      button1.Location = new System.Drawing.Point(5, 6);
      button1.Margin = new System.Windows.Forms.Padding(5, 6, 5, 6);
      button1.Name = "button1";
      button1.Size = new System.Drawing.Size(120, 40);
      button1.TabIndex = 0;
      button1.Text = "Select Params";
      button1.UseVisualStyleBackColor = true;
      button1.Click += button1_Click;
      // 
      // openFileDialog1
      // 
      openFileDialog1.FileName = "openFileDialog1";
      // 
      // label1
      // 
      label1.AutoSize = true;
      label1.Location = new System.Drawing.Point(135, 14);
      label1.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
      label1.Name = "label1";
      label1.Size = new System.Drawing.Size(95, 25);
      label1.TabIndex = 1;
      label1.Text = "Not Ready";
      // 
      // button2
      // 
      button2.Enabled = false;
      button2.Location = new System.Drawing.Point(5, 58);
      button2.Margin = new System.Windows.Forms.Padding(5, 6, 5, 6);
      button2.Name = "button2";
      button2.Size = new System.Drawing.Size(120, 40);
      button2.TabIndex = 2;
      button2.Text = "Select Raw";
      button2.UseVisualStyleBackColor = true;
      button2.Click += button2_Click;
      // 
      // label2
      // 
      label2.AutoSize = true;
      label2.Location = new System.Drawing.Point(135, 66);
      label2.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
      label2.Name = "label2";
      label2.Size = new System.Drawing.Size(95, 25);
      label2.TabIndex = 3;
      label2.Text = "Not Ready";
      // 
      // button3
      // 
      button3.Enabled = false;
      button3.Location = new System.Drawing.Point(5, 107);
      button3.Name = "button3";
      button3.Size = new System.Drawing.Size(120, 40);
      button3.TabIndex = 4;
      button3.Text = "Run!";
      button3.UseVisualStyleBackColor = true;
      button3.Click += button3_Click;
      // 
      // richTextBox1
      // 
      richTextBox1.Dock = System.Windows.Forms.DockStyle.Fill;
      richTextBox1.Font = new System.Drawing.Font("Courier New", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, 0);
      richTextBox1.Location = new System.Drawing.Point(0, 64);
      richTextBox1.Name = "richTextBox1";
      richTextBox1.Size = new System.Drawing.Size(1373, 576);
      richTextBox1.TabIndex = 5;
      richTextBox1.Text = "";
      // 
      // splitContainer1
      // 
      splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
      splitContainer1.Location = new System.Drawing.Point(0, 0);
      splitContainer1.Name = "splitContainer1";
      splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
      // 
      // splitContainer1.Panel1
      // 
      splitContainer1.Panel1.Controls.Add(searchListBox);
      splitContainer1.Panel1.Controls.Add(button9);
      splitContainer1.Panel1.Controls.Add(button4);
      splitContainer1.Panel1.Controls.Add(label3);
      splitContainer1.Panel1.Controls.Add(numericUpDown1);
      splitContainer1.Panel1.Controls.Add(button1);
      splitContainer1.Panel1.Controls.Add(button3);
      splitContainer1.Panel1.Controls.Add(label1);
      splitContainer1.Panel1.Controls.Add(label2);
      splitContainer1.Panel1.Controls.Add(button2);
      // 
      // splitContainer1.Panel2
      // 
      splitContainer1.Panel2.Controls.Add(splitContainer3);
      splitContainer1.Size = new System.Drawing.Size(1668, 1004);
      splitContainer1.SplitterDistance = 160;
      splitContainer1.TabIndex = 6;
      // 
      // searchListBox
      // 
      searchListBox.FormattingEnabled = true;
      searchListBox.ItemHeight = 25;
      searchListBox.Items.AddRange(new object[] { "Telescope", "Comet" });
      searchListBox.Location = new System.Drawing.Point(1472, 93);
      searchListBox.Name = "searchListBox";
      searchListBox.Size = new System.Drawing.Size(184, 54);
      searchListBox.TabIndex = 9;
      // 
      // button9
      // 
      button9.Location = new System.Drawing.Point(510, 107);
      button9.Name = "button9";
      button9.Size = new System.Drawing.Size(120, 40);
      button9.TabIndex = 8;
      button9.Text = "Export Log";
      button9.UseVisualStyleBackColor = true;
      button9.Click += button9_Click;
      // 
      // button4
      // 
      button4.Enabled = false;
      button4.Location = new System.Drawing.Point(384, 107);
      button4.Name = "button4";
      button4.Size = new System.Drawing.Size(120, 40);
      button4.TabIndex = 7;
      button4.Text = "Stop!";
      button4.UseVisualStyleBackColor = true;
      button4.Click += button4_Click;
      // 
      // label3
      // 
      label3.AutoSize = true;
      label3.Location = new System.Drawing.Point(236, 115);
      label3.Name = "label3";
      label3.Size = new System.Drawing.Size(152, 25);
      label3.TabIndex = 6;
      label3.Text = "scans per second.";
      // 
      // numericUpDown1
      // 
      numericUpDown1.Enabled = false;
      numericUpDown1.Location = new System.Drawing.Point(135, 113);
      numericUpDown1.Maximum = new decimal(new int[] { 500000, 0, 0, 0 });
      numericUpDown1.Minimum = new decimal(new int[] { 1, 0, 0, 0 });
      numericUpDown1.Name = "numericUpDown1";
      numericUpDown1.Size = new System.Drawing.Size(95, 31);
      numericUpDown1.TabIndex = 5;
      numericUpDown1.Value = new decimal(new int[] { 20, 0, 0, 0 });
      // 
      // splitContainer3
      // 
      splitContainer3.Dock = System.Windows.Forms.DockStyle.Fill;
      splitContainer3.Location = new System.Drawing.Point(0, 0);
      splitContainer3.Name = "splitContainer3";
      splitContainer3.Orientation = System.Windows.Forms.Orientation.Horizontal;
      // 
      // splitContainer3.Panel1
      // 
      splitContainer3.Panel1.Controls.Add(splitContainer2);
      // 
      // splitContainer3.Panel2
      // 
      splitContainer3.Panel2.Controls.Add(rtbMessage);
      splitContainer3.Size = new System.Drawing.Size(1668, 840);
      splitContainer3.SplitterDistance = 640;
      splitContainer3.TabIndex = 7;
      // 
      // splitContainer2
      // 
      splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
      splitContainer2.Location = new System.Drawing.Point(0, 0);
      splitContainer2.Name = "splitContainer2";
      // 
      // splitContainer2.Panel1
      // 
      splitContainer2.Panel1.Controls.Add(label19);
      splitContainer2.Panel1.Controls.Add(label20);
      splitContainer2.Panel1.Controls.Add(label18);
      splitContainer2.Panel1.Controls.Add(label17);
      splitContainer2.Panel1.Controls.Add(label16);
      splitContainer2.Panel1.Controls.Add(label15);
      splitContainer2.Panel1.Controls.Add(label14);
      splitContainer2.Panel1.Controls.Add(label13);
      splitContainer2.Panel1.Controls.Add(label12);
      splitContainer2.Panel1.Controls.Add(label11);
      splitContainer2.Panel1.Controls.Add(label9);
      splitContainer2.Panel1.Controls.Add(label8);
      splitContainer2.Panel1.Controls.Add(label7);
      splitContainer2.Panel1.Controls.Add(label6);
      splitContainer2.Panel1.Controls.Add(label5);
      splitContainer2.Panel1.Controls.Add(label4);
      // 
      // splitContainer2.Panel2
      // 
      splitContainer2.Panel2.Controls.Add(richTextBox1);
      splitContainer2.Panel2.Controls.Add(panel1);
      splitContainer2.Size = new System.Drawing.Size(1668, 640);
      splitContainer2.SplitterDistance = 291;
      splitContainer2.TabIndex = 6;
      // 
      // label18
      // 
      label18.AutoSize = true;
      label18.Location = new System.Drawing.Point(188, 171);
      label18.Name = "label18";
      label18.Size = new System.Drawing.Size(22, 25);
      label18.TabIndex = 13;
      label18.Text = "0";
      // 
      // label17
      // 
      label17.AutoSize = true;
      label17.Location = new System.Drawing.Point(188, 146);
      label17.Name = "label17";
      label17.Size = new System.Drawing.Size(22, 25);
      label17.TabIndex = 12;
      label17.Text = "0";
      // 
      // label16
      // 
      label16.AutoSize = true;
      label16.Location = new System.Drawing.Point(16, 171);
      label16.Name = "label16";
      label16.Size = new System.Drawing.Size(134, 25);
      label16.TabIndex = 11;
      label16.Text = "Slowest Search:";
      // 
      // label15
      // 
      label15.AutoSize = true;
      label15.Location = new System.Drawing.Point(16, 146);
      label15.Name = "label15";
      label15.Size = new System.Drawing.Size(127, 25);
      label15.TabIndex = 10;
      label15.Text = "Fastest Search:";
      // 
      // label14
      // 
      label14.AutoSize = true;
      label14.Location = new System.Drawing.Point(188, 121);
      label14.Name = "label14";
      label14.Size = new System.Drawing.Size(22, 25);
      label14.TabIndex = 9;
      label14.Text = "0";
      // 
      // label13
      // 
      label13.AutoSize = true;
      label13.Location = new System.Drawing.Point(16, 121);
      label13.Name = "label13";
      label13.Size = new System.Drawing.Size(124, 25);
      label13.TabIndex = 8;
      label13.Text = "Avg Lag Time:";
      // 
      // label12
      // 
      label12.AutoSize = true;
      label12.Location = new System.Drawing.Point(188, 96);
      label12.Name = "label12";
      label12.Size = new System.Drawing.Size(22, 25);
      label12.TabIndex = 7;
      label12.Text = "0";
      // 
      // label11
      // 
      label11.AutoSize = true;
      label11.Location = new System.Drawing.Point(16, 96);
      label11.Name = "label11";
      label11.Size = new System.Drawing.Size(131, 25);
      label11.TabIndex = 6;
      label11.Text = "Active Threads:";
      // 
      // label9
      // 
      label9.AutoSize = true;
      label9.Location = new System.Drawing.Point(188, 72);
      label9.Name = "label9";
      label9.Size = new System.Drawing.Size(22, 25);
      label9.TabIndex = 5;
      label9.Text = "0";
      // 
      // label8
      // 
      label8.AutoSize = true;
      label8.Location = new System.Drawing.Point(188, 47);
      label8.Name = "label8";
      label8.Size = new System.Drawing.Size(22, 25);
      label8.TabIndex = 4;
      label8.Text = "0";
      // 
      // label7
      // 
      label7.AutoSize = true;
      label7.Location = new System.Drawing.Point(188, 22);
      label7.Name = "label7";
      label7.Size = new System.Drawing.Size(22, 25);
      label7.TabIndex = 3;
      label7.Text = "0";
      // 
      // label6
      // 
      label6.AutoSize = true;
      label6.Location = new System.Drawing.Point(16, 71);
      label6.Name = "label6";
      label6.Size = new System.Drawing.Size(148, 25);
      label6.TabIndex = 2;
      label6.Text = "Avg Search Time:";
      // 
      // label5
      // 
      label5.AutoSize = true;
      label5.Location = new System.Drawing.Point(16, 46);
      label5.Name = "label5";
      label5.Size = new System.Drawing.Size(135, 25);
      label5.TabIndex = 1;
      label5.Text = "Matched Scans:";
      // 
      // label4
      // 
      label4.AutoSize = true;
      label4.Location = new System.Drawing.Point(16, 21);
      label4.Name = "label4";
      label4.Size = new System.Drawing.Size(103, 25);
      label4.TabIndex = 0;
      label4.Text = "Total Scans:";
      // 
      // panel1
      // 
      panel1.Controls.Add(button8);
      panel1.Controls.Add(button7);
      panel1.Controls.Add(label10);
      panel1.Controls.Add(button6);
      panel1.Controls.Add(button5);
      panel1.Dock = System.Windows.Forms.DockStyle.Top;
      panel1.Location = new System.Drawing.Point(0, 0);
      panel1.Name = "panel1";
      panel1.Size = new System.Drawing.Size(1373, 64);
      panel1.TabIndex = 6;
      // 
      // button8
      // 
      button8.Location = new System.Drawing.Point(3, 3);
      button8.Name = "button8";
      button8.Size = new System.Drawing.Size(48, 48);
      button8.TabIndex = 4;
      button8.Text = "<<";
      button8.UseVisualStyleBackColor = true;
      button8.Click += button8_Click;
      // 
      // button7
      // 
      button7.Location = new System.Drawing.Point(291, 3);
      button7.Name = "button7";
      button7.Size = new System.Drawing.Size(48, 48);
      button7.TabIndex = 3;
      button7.Text = ">>";
      button7.UseVisualStyleBackColor = true;
      button7.Click += button7_Click;
      // 
      // label10
      // 
      label10.Font = new System.Drawing.Font("Segoe UI", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, 0);
      label10.Location = new System.Drawing.Point(111, 12);
      label10.Name = "label10";
      label10.Size = new System.Drawing.Size(120, 26);
      label10.TabIndex = 2;
      label10.Text = "0/0";
      label10.TextAlign = System.Drawing.ContentAlignment.TopCenter;
      // 
      // button6
      // 
      button6.Location = new System.Drawing.Point(237, 3);
      button6.Name = "button6";
      button6.Size = new System.Drawing.Size(48, 48);
      button6.TabIndex = 1;
      button6.Text = ">";
      button6.UseVisualStyleBackColor = true;
      button6.Click += button6_Click;
      // 
      // button5
      // 
      button5.Location = new System.Drawing.Point(57, 3);
      button5.Name = "button5";
      button5.Size = new System.Drawing.Size(48, 48);
      button5.TabIndex = 0;
      button5.Text = "<";
      button5.UseVisualStyleBackColor = true;
      button5.Click += button5_Click;
      // 
      // rtbMessage
      // 
      rtbMessage.Dock = System.Windows.Forms.DockStyle.Fill;
      rtbMessage.Location = new System.Drawing.Point(0, 0);
      rtbMessage.Name = "rtbMessage";
      rtbMessage.Size = new System.Drawing.Size(1668, 196);
      rtbMessage.TabIndex = 0;
      rtbMessage.Text = "";
      // 
      // label19
      // 
      label19.AutoSize = true;
      label19.Location = new System.Drawing.Point(188, 214);
      label19.Name = "label19";
      label19.Size = new System.Drawing.Size(22, 25);
      label19.TabIndex = 15;
      label19.Text = "0";
      // 
      // label20
      // 
      label20.AutoSize = true;
      label20.Location = new System.Drawing.Point(16, 214);
      label20.Name = "label20";
      label20.Size = new System.Drawing.Size(129, 25);
      label20.TabIndex = 14;
      label20.Text = "Scans Queued:";
      // 
      // Form1
      // 
      AutoScaleDimensions = new System.Drawing.SizeF(10F, 25F);
      AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      ClientSize = new System.Drawing.Size(1668, 1004);
      Controls.Add(splitContainer1);
      Margin = new System.Windows.Forms.Padding(5, 6, 5, 6);
      Name = "Form1";
      Text = "Form1";
      splitContainer1.Panel1.ResumeLayout(false);
      splitContainer1.Panel1.PerformLayout();
      splitContainer1.Panel2.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)splitContainer1).EndInit();
      splitContainer1.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)numericUpDown1).EndInit();
      splitContainer3.Panel1.ResumeLayout(false);
      splitContainer3.Panel2.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)splitContainer3).EndInit();
      splitContainer3.ResumeLayout(false);
      splitContainer2.Panel1.ResumeLayout(false);
      splitContainer2.Panel1.PerformLayout();
      splitContainer2.Panel2.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)splitContainer2).EndInit();
      splitContainer2.ResumeLayout(false);
      panel1.ResumeLayout(false);
      ResumeLayout(false);
    }

    #endregion

    private System.Windows.Forms.Button button1;
    private System.Windows.Forms.OpenFileDialog openFileDialog1;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Button button2;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.Button button3;
    private System.Windows.Forms.RichTextBox richTextBox1;
    private System.Windows.Forms.SplitContainer splitContainer1;
    private System.Windows.Forms.NumericUpDown numericUpDown1;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.SplitContainer splitContainer2;
    private System.Windows.Forms.Button button4;
    private System.Windows.Forms.Label label4;
    private System.Windows.Forms.Label label6;
    private System.Windows.Forms.Label label5;
    private System.Windows.Forms.Label label9;
    private System.Windows.Forms.Label label8;
    private System.Windows.Forms.Label label7;
    private System.Windows.Forms.Panel panel1;
    private System.Windows.Forms.Label label10;
    private System.Windows.Forms.Button button6;
    private System.Windows.Forms.Button button5;
    private System.Windows.Forms.Button button7;
    private System.Windows.Forms.Button button8;
    private System.Windows.Forms.Label label11;
    private System.Windows.Forms.Label label12;
    private System.Windows.Forms.Label label14;
    private System.Windows.Forms.Label label13;
    private System.Windows.Forms.Label label18;
    private System.Windows.Forms.Label label17;
    private System.Windows.Forms.Label label16;
    private System.Windows.Forms.Label label15;
    private System.Windows.Forms.Button button9;
    private System.Windows.Forms.ListBox searchListBox;
    private System.Windows.Forms.SplitContainer splitContainer3;
    private System.Windows.Forms.RichTextBox rtbMessage;
    private System.Windows.Forms.Label label19;
    private System.Windows.Forms.Label label20;
  }
}

