
namespace Motor_DC
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.label1 = new System.Windows.Forms.Label();
            this.txtPort1 = new System.Windows.Forms.TextBox();
            this.txtBaudrate = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.btnConnect = new System.Windows.Forms.Button();
            this.s = new System.Windows.Forms.Label();
            this.lblStatus = new System.Windows.Forms.Label();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.saveFileDialog2 = new System.Windows.Forms.SaveFileDialog();
            this.label4 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.tReg6_1 = new System.Windows.Forms.Label();
            this.label13 = new System.Windows.Forms.Label();
            this.tReg5_1 = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.tReg4_1 = new System.Windows.Forms.Label();
            this.timerPoll = new System.Windows.Forms.Timer(this.components);
            this.label5 = new System.Windows.Forms.Label();
            this.tReg2_1 = new System.Windows.Forms.TextBox();
            this.tReg3_1 = new System.Windows.Forms.TextBox();
            this.btnSend = new System.Windows.Forms.Button();
            this.btn_1 = new System.Windows.Forms.Button();
            this.tReg1_1 = new System.Windows.Forms.Label();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.btnReset = new System.Windows.Forms.Button();
            this.bStop = new System.Windows.Forms.Button();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 32);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(34, 17);
            this.label1.TabIndex = 0;
            this.label1.Text = "Port";
            this.label1.Click += new System.EventHandler(this.label1_Click);
            // 
            // txtPort1
            // 
            this.txtPort1.Location = new System.Drawing.Point(84, 30);
            this.txtPort1.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.txtPort1.Name = "txtPort1";
            this.txtPort1.Size = new System.Drawing.Size(167, 22);
            this.txtPort1.TabIndex = 1;
            this.txtPort1.Text = "COM7\r\n";
            // 
            // txtBaudrate
            // 
            this.txtBaudrate.Location = new System.Drawing.Point(84, 78);
            this.txtBaudrate.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.txtBaudrate.Name = "txtBaudrate";
            this.txtBaudrate.Size = new System.Drawing.Size(167, 22);
            this.txtBaudrate.TabIndex = 3;
            this.txtBaudrate.Text = "9600";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 78);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(66, 17);
            this.label2.TabIndex = 2;
            this.label2.Text = "Baudrate";
            this.label2.Click += new System.EventHandler(this.label2_Click);
            // 
            // btnConnect
            // 
            this.btnConnect.Location = new System.Drawing.Point(15, 126);
            this.btnConnect.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btnConnect.Name = "btnConnect";
            this.btnConnect.Size = new System.Drawing.Size(223, 68);
            this.btnConnect.TabIndex = 4;
            this.btnConnect.Text = "Connect";
            this.btnConnect.UseVisualStyleBackColor = true;
            this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
            // 
            // s
            // 
            this.s.AutoSize = true;
            this.s.Location = new System.Drawing.Point(12, 579);
            this.s.Name = "s";
            this.s.Size = new System.Drawing.Size(56, 17);
            this.s.TabIndex = 5;
            this.s.Text = "Status :";
            this.s.Click += new System.EventHandler(this.label3_Click);
            // 
            // lblStatus
            // 
            this.lblStatus.AutoSize = true;
            this.lblStatus.Location = new System.Drawing.Point(81, 579);
            this.lblStatus.Name = "lblStatus";
            this.lblStatus.Size = new System.Drawing.Size(102, 17);
            this.lblStatus.TabIndex = 6;
            this.lblStatus.Text = "Not Connected";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(25, 345);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(55, 17);
            this.label4.TabIndex = 7;
            this.label4.Text = "Slave 1";
            this.label4.Click += new System.EventHandler(this.label4_Click);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(27, 451);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(42, 17);
            this.label6.TabIndex = 9;
            this.label6.Text = "Reg1";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(105, 451);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(42, 17);
            this.label7.TabIndex = 11;
            this.label7.Text = "Reg2";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(274, 451);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(42, 17);
            this.label9.TabIndex = 13;
            this.label9.Text = "Reg3";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(233, 526);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(42, 17);
            this.label11.TabIndex = 19;
            this.label11.Text = "Reg6";
            // 
            // tReg6_1
            // 
            this.tReg6_1.AutoSize = true;
            this.tReg6_1.Font = new System.Drawing.Font("Microsoft Sans Serif", 30F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tReg6_1.Location = new System.Drawing.Point(234, 469);
            this.tReg6_1.Name = "tReg6_1";
            this.tReg6_1.Size = new System.Drawing.Size(53, 58);
            this.tReg6_1.TabIndex = 18;
            this.tReg6_1.Text = "0";
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(85, 526);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(42, 17);
            this.label13.TabIndex = 17;
            this.label13.Text = "Reg5";
            // 
            // tReg5_1
            // 
            this.tReg5_1.AutoSize = true;
            this.tReg5_1.Font = new System.Drawing.Font("Microsoft Sans Serif", 30F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tReg5_1.Location = new System.Drawing.Point(85, 469);
            this.tReg5_1.Name = "tReg5_1";
            this.tReg5_1.Size = new System.Drawing.Size(53, 58);
            this.tReg5_1.TabIndex = 16;
            this.tReg5_1.Text = "0";
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(27, 526);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(42, 17);
            this.label15.TabIndex = 15;
            this.label15.Text = "Reg4";
            // 
            // tReg4_1
            // 
            this.tReg4_1.AutoSize = true;
            this.tReg4_1.Font = new System.Drawing.Font("Microsoft Sans Serif", 30F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tReg4_1.Location = new System.Drawing.Point(27, 469);
            this.tReg4_1.Name = "tReg4_1";
            this.tReg4_1.Size = new System.Drawing.Size(53, 58);
            this.tReg4_1.TabIndex = 14;
            this.tReg4_1.Text = "0";
            // 
            // timerPoll
            // 
            this.timerPoll.Interval = 1000;
            this.timerPoll.Tick += new System.EventHandler(this.timerPoll_Tick);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(129, 11);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(55, 17);
            this.label5.TabIndex = 34;
            this.label5.Text = "Slave 1";
            // 
            // tReg2_1
            // 
            this.tReg2_1.Font = new System.Drawing.Font("Microsoft Sans Serif", 30F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tReg2_1.Location = new System.Drawing.Point(84, 390);
            this.tReg2_1.Margin = new System.Windows.Forms.Padding(4);
            this.tReg2_1.Multiline = true;
            this.tReg2_1.Name = "tReg2_1";
            this.tReg2_1.Size = new System.Drawing.Size(107, 57);
            this.tReg2_1.TabIndex = 36;
            this.tReg2_1.Text = "0";
            // 
            // tReg3_1
            // 
            this.tReg3_1.Font = new System.Drawing.Font("Microsoft Sans Serif", 30F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tReg3_1.Location = new System.Drawing.Point(236, 390);
            this.tReg3_1.Margin = new System.Windows.Forms.Padding(4);
            this.tReg3_1.Multiline = true;
            this.tReg3_1.Name = "tReg3_1";
            this.tReg3_1.Size = new System.Drawing.Size(119, 57);
            this.tReg3_1.TabIndex = 37;
            this.tReg3_1.Text = "0";
            // 
            // btnSend
            // 
            this.btnSend.Location = new System.Drawing.Point(448, 127);
            this.btnSend.Margin = new System.Windows.Forms.Padding(4);
            this.btnSend.Name = "btnSend";
            this.btnSend.Size = new System.Drawing.Size(195, 66);
            this.btnSend.TabIndex = 40;
            this.btnSend.Text = "send";
            this.btnSend.UseVisualStyleBackColor = true;
            this.btnSend.Click += new System.EventHandler(this.bSend_Click);
            // 
            // btn_1
            // 
            this.btn_1.Location = new System.Drawing.Point(244, 126);
            this.btn_1.Margin = new System.Windows.Forms.Padding(4);
            this.btn_1.Name = "btn_1";
            this.btn_1.Size = new System.Drawing.Size(195, 66);
            this.btn_1.TabIndex = 43;
            this.btn_1.Text = "1";
            this.btn_1.UseVisualStyleBackColor = true;
            this.btn_1.Click += new System.EventHandler(this.btn_1_Click);
            // 
            // tReg1_1
            // 
            this.tReg1_1.AutoSize = true;
            this.tReg1_1.Font = new System.Drawing.Font("Microsoft Sans Serif", 30F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tReg1_1.Location = new System.Drawing.Point(27, 393);
            this.tReg1_1.Name = "tReg1_1";
            this.tReg1_1.Size = new System.Drawing.Size(53, 58);
            this.tReg1_1.TabIndex = 44;
            this.tReg1_1.Text = "0";
            // 
            // timer1
            // 
            this.timer1.Interval = 10;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // btnReset
            // 
            this.btnReset.Location = new System.Drawing.Point(15, 218);
            this.btnReset.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btnReset.Name = "btnReset";
            this.btnReset.Size = new System.Drawing.Size(223, 68);
            this.btnReset.TabIndex = 45;
            this.btnReset.Text = "Reset";
            this.btnReset.UseVisualStyleBackColor = true;
            this.btnReset.Click += new System.EventHandler(this.btnReset_Click);
            // 
            // bStop
            // 
            this.bStop.BackColor = System.Drawing.Color.Red;
            this.bStop.ForeColor = System.Drawing.SystemColors.ButtonFace;
            this.bStop.Location = new System.Drawing.Point(248, 220);
            this.bStop.Name = "bStop";
            this.bStop.Size = new System.Drawing.Size(190, 65);
            this.bStop.TabIndex = 46;
            this.bStop.Text = "Stop";
            this.bStop.UseVisualStyleBackColor = false;
            this.bStop.Click += new System.EventHandler(this.bStop_Click);
            // 
            // pictureBox1
            // 
            this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
            this.pictureBox1.InitialImage = null;
            this.pictureBox1.Location = new System.Drawing.Point(425, 360);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(167, 167);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.pictureBox1.TabIndex = 47;
            this.pictureBox1.TabStop = false;
            this.pictureBox1.Click += new System.EventHandler(this.pictureBox1_Click_1);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(655, 604);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.bStop);
            this.Controls.Add(this.btnReset);
            this.Controls.Add(this.tReg1_1);
            this.Controls.Add(this.btn_1);
            this.Controls.Add(this.btnSend);
            this.Controls.Add(this.tReg3_1);
            this.Controls.Add(this.tReg2_1);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.tReg6_1);
            this.Controls.Add(this.label13);
            this.Controls.Add(this.tReg5_1);
            this.Controls.Add(this.label15);
            this.Controls.Add(this.tReg4_1);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.lblStatus);
            this.Controls.Add(this.s);
            this.Controls.Add(this.btnConnect);
            this.Controls.Add(this.txtBaudrate);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.txtPort1);
            this.Controls.Add(this.label1);
            this.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.Name = "Form1";
            this.Text = "Astro_24";
            this.Load += new System.EventHandler(this.tReg1_1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtPort1;
        private System.Windows.Forms.TextBox txtBaudrate;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button btnConnect;
        private System.Windows.Forms.Label s;
        private System.Windows.Forms.Label lblStatus;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
        private System.Windows.Forms.SaveFileDialog saveFileDialog2;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label tReg6_1;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label tReg5_1;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Label tReg4_1;
        private System.Windows.Forms.Timer timerPoll;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox tReg2_1;
        private System.Windows.Forms.TextBox tReg3_1;
        private System.Windows.Forms.Button btnSend;
        private System.Windows.Forms.Button btn_1;
        private System.Windows.Forms.Label tReg1_1;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Button btnReset;
        private System.Windows.Forms.Button bStop;
        private System.Windows.Forms.PictureBox pictureBox1;
    }
}

