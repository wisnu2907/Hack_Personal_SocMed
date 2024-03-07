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
        ModbusClient ModClient2 = new ModbusClient();
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
                ModClient.SerialPort = cPort1.Text;
                ModClient.Baudrate = int.Parse(txtBaudrate.Text);
                ModClient.Parity = System.IO.Ports.Parity.None;
                ModClient.ConnectionTimeout = 1000;
                ModClient.Connect();
                ModClient2.SerialPort = cPort2.Text;
                ModClient2.Baudrate = int.Parse(txtBaudrate.Text);
                ModClient2.Parity = System.IO.Ports.Parity.None;
                ModClient2.ConnectionTimeout = 1000;
                ModClient2.Connect();
                timerPoll.Start();
                lblStatus.Text = "Connected";
                btnConnect.Text = "Disconnect";
            } else
            {
                timerPoll.Stop();
                timer1.Stop();
                ModClient.Disconnect();
                ModClient2.Disconnect();
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

            ModClient.UnitIdentifier = 10;
            ModClient.WriteSingleRegister(1, int.Parse(tReg2_2.Text));
            ModClient.WriteSingleRegister(2, int.Parse(tReg3_2.Text));

        }

        private void btn_1_Click(object sender, EventArgs e)
        {
            try
            {
                if (btn_1.Text == "1")
                {
                    ModClient.UnitIdentifier = 11;
                    ModClient.WriteSingleRegister(0, 1);
                    ModClient2.UnitIdentifier = 10;
                    ModClient2.WriteSingleRegister(0, 1);
                    IsReading = true;
                    timer1.Start();
                    btn_1.Text = "0";
                } else
                {
                    ModClient.UnitIdentifier = 11;
                    ModClient.WriteSingleRegister(0, 0);
                    ModClient.WriteSingleRegister(3, 1);
                    ModClient2.UnitIdentifier = 10;
                    ModClient2.WriteSingleRegister(0, 0);
                    ModClient2.WriteSingleRegister(3, 1);
                    timer1.Stop();
                    btn_1.Text = "1";
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
            if (ModClient.Connected == true && ModClient2.Connected == true)
            {
                //read 1st slave
                ModClient.UnitIdentifier = 11;
                int[] vals = ModClient.ReadHoldingRegisters(0, 6);
                tReg1_1.Text = vals[0].ToString();
                tReg5_1.Text = vals[4].ToString();
                tReg6_1.Text = vals[5].ToString();

                //read 2nd slave
                ModClient2.UnitIdentifier = 10;
                int[] vals2 = ModClient2.ReadHoldingRegisters(0, 6);
                tReg1_2.Text = vals2[0].ToString();
                tReg5_2.Text = vals2[4].ToString();
                tReg6_2.Text = vals2[5].ToString();
            }
        }

        private void btnReset_Click(object sender, EventArgs e)
        {
            ModClient.UnitIdentifier = 11;
            ModClient.WriteSingleRegister(3, 1);

            ModClient2.UnitIdentifier = 10;
            ModClient2.WriteSingleRegister(3, 1);
        }

        private void bStop_Click(object sender, EventArgs e)
        {
            ModClient.UnitIdentifier = 11;
            ModClient.WriteSingleRegister(1, 0);
            ModClient.WriteSingleRegister(2, 0);

            ModClient2.UnitIdentifier = 10;
            ModClient2.WriteSingleRegister(1, 0);
            ModClient2.WriteSingleRegister(2, 0);
        }

        private void label3_Click_1(object sender, EventArgs e)
        {

        }

        private void label19_Click(object sender, EventArgs e)
        {

        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }
    }
}
