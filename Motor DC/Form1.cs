using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using EasyModbus;

namespace Motor_DC
{
    public partial class Form1 : Form
    {
        ModbusClient ModClient = new ModbusClient();
        bool IsReading = false;
        public Form1()
        {
            InitializeComponent();
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void label3_Click(object sender, EventArgs e)
        {

        }

        private void btnConnect_Click(object sender, EventArgs e)
        {
            if(btnConnect.Text == "Connect")
            {
                ModClient.SerialPort = txtPort1.Text;
                ModClient.Baudrate = int.Parse(txtBaudrate.Text);
                ModClient.Parity = System.IO.Ports.Parity.None;
                ModClient.ConnectionTimeout = 1000;
                ModClient.Connect();
                timerPoll.Start();
                lblStatus.Text = "Connected";
                btnConnect.Text = "Disconnect";
            } else
            {
                timerPoll.Stop();
                timer1.Stop();
                ModClient.Disconnect();
                lblStatus.Text = "Disconnected";
                btnConnect.Text = "Connect";
            }
        }

        private void label4_Click(object sender, EventArgs e)
        {

        }

        private void timerPoll_Tick(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void tReg3_2_TextChanged(object sender, EventArgs e)
        {

        }

        private void bSend_Click(object sender, EventArgs e)
        {
            ModClient.UnitIdentifier = 11;
            ModClient.WriteSingleRegister(1, int.Parse(tReg2_1.Text));
            ModClient.WriteSingleRegister(2, int.Parse(tReg3_1.Text));

        }

        private void btn_1_Click(object sender, EventArgs e)
        {
            try
            {
                if (btn_1.Text == "1")
                {
                    ModClient.UnitIdentifier = 11;
                    ModClient.WriteSingleRegister(0, 1);
                    IsReading = true;
                    timer1.Start();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Failed to write: " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void tReg1_1_Load(object sender, EventArgs e)
        {

        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (ModClient.Connected == true)
            {
                //read 1st slave
                ModClient.UnitIdentifier = 11;
                int[] vals = ModClient.ReadHoldingRegisters(0, 6);
                tReg1_1.Text = vals[0].ToString();
                tReg4_1.Text = vals[3].ToString();
                tReg5_1.Text = vals[4].ToString();
                tReg6_1.Text = vals[5].ToString();
            }
        }

        private void btnReset_Click(object sender, EventArgs e)
        {
            ModClient.UnitIdentifier = 11;
            ModClient.WriteSingleRegister(3, 1);
        }

        private void bStop_Click(object sender, EventArgs e)
        {
            ModClient.UnitIdentifier = 11;
            ModClient.WriteSingleRegister(1, 0);
            ModClient.WriteSingleRegister(2, 0);
        }

        private void pictureBox1_Click(object sender, EventArgs e)
        {

        }

        private void pictureBox1_Click_1(object sender, EventArgs e)
        {

        }
    }
}
