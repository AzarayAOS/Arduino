using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Drawing.Text;
using System.IO.Ports;

namespace RTCSetup
{
    public partial class Form1 : Form
    {

        PrivateFontCollection pfc = new PrivateFontCollection();
        private SerialManager serialManager = new SerialManager();
        private bool connected = false;
        private string sketchVersion = "1.0";
        private byte UpdateTimer = 6;
        private  uint TableUpdate = 100;

        private string[] inforstr;


        string StrHum;
        string Strtemp;
        string Strir_temp;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {            
            Stream fontStream = this.GetType().Assembly.GetManifestResourceStream("RTCSetup.Resources.digital-7.ttf");
            byte[] fontdata = new byte[fontStream.Length];
            fontStream.Read(fontdata, 0, (int)fontStream.Length);
            fontStream.Close();
            unsafe
            {
                fixed (byte* pFontData = fontdata)
                {
                    pfc.AddMemoryFont((System.IntPtr)pFontData, fontdata.Length);
                }
            }
            lbSystemTime.Font = new Font(pfc.Families.First(), 30, FontStyle.Regular);
            //lbRTCTime.Font = new Font(pfc.Families.First(), 25, FontStyle.Regular);

            //label1.Font= new Font(pfc.Families.First(), 25, FontStyle.Regular);

            //label2.Font= new Font(pfc.Families.First(), 25, FontStyle.Regular);
            //label3.Font = new Font(pfc.Families.First(), 25, FontStyle.Regular);
            //label4.Font = new Font(pfc.Families.First(), 25, FontStyle.Regular);
            //label5.Font = new Font(pfc.Families.First(), 25, FontStyle.Regular);
            dtpCustom.Value = DateTime.Now;
            // lbRTCTime.Text = "unknown";



            progressBar1.Step =100*( timer2.Interval / 1000)/60;
            addAvailableComPorts();
            updateUI();
        }

        private void SetTablice()
        {
            dataGridView1.Rows.Add(
                dataGridView1.Rows.Count.ToString(), 
                DateTime.Now.ToString("yyyy.MM.dd"), 
                DateTime.Now.ToString("HH:mm"), 
                inforstr[0], 
                inforstr[1], 
                inforstr[2]);


            dataGridView1.FirstDisplayedCell = dataGridView1.Rows[dataGridView1.Rows.Count - 1].Cells[0];


        }


        private void addAvailableComPorts()
        {
            foreach (string comPort in serialManager.getAvailablePorts())
                cbSerialPorts.Items.Add(comPort);

            if (cbSerialPorts.Items.Count > 0) cbSerialPorts.SelectedIndex = 0;
        }

        private void updateUI()
        {
            if (connected)
            {
                btConnect.Text = "DISCONNECT";
                toolStripStatusLabel1.Text = "Connected with sketch v" + sketchVersion;
                btGet.Enabled = true;
                btSetSystem.Enabled = true;
                btSetCustom.Enabled = true;
                dtpCustom.Enabled = true;
                cbSerialPorts.Enabled = false;
            }
            else
            {
                btConnect.Text = "CONNECT";
                toolStripStatusLabel1.Text = "Disconnected";
                btGet.Enabled = false;
                btSetSystem.Enabled = false;
                btSetCustom.Enabled = false;
                dtpCustom.Enabled = false;
                cbSerialPorts.Enabled = true;
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            //lbSystemTime.Text = DateTime.Now.ToString("dd/MM/yyyy HH:mm:ss");

            //string info = serialManager.getInfo();
           
            inforstr = serialManager.getInfo().Split(' ');

            label3.Text = inforstr[0];
            label4.Text = inforstr[1];
            label5.Text = inforstr[2];
            //progressBar1.Value = 0;
            UpdateTimer = 6;


            if(TableUpdate>=60)
            {
                SetTablice();
                TableUpdate = 0;
                progressBar1.Value = 0;
            }
            TableUpdate += 5;
           // progressBar1.PerformStep();
            //SetTablice();


        }

        private void btConnect_Click(object sender, EventArgs e)
        {
            if (!connected)
            {
                string serialPortName = (string)cbSerialPorts.SelectedItem;
                bool result = serialManager.connect(serialPortName);
               
                if (result) 
                {
                    result = serialManager.handshake();
                    
                } 
                else return;

                if(result) 
                {
                    
                    connected = true;
                    timer1.Enabled = true;
                    timer2.Enabled = true;
                    
                    updateUI();
                }
                else serialManager.disconnect();
            }

            else
            {
                timer1.Enabled = false;
                timer2.Enabled = false;
                serialManager.disconnect();
                connected = false;
                updateUI();
            }
        }

        private void btGet_Click(object sender, EventArgs e)
        {
            lbRTCTime.Text = serialManager.getRTCTime();
        }

        private void btSetSystem_Click(object sender, EventArgs e)
        {
            serialManager.setRTCTime(DateTime.Parse(lbSystemTime.Text));
        }

        private void btSetCustom_Click(object sender, EventArgs e)
        {
            serialManager.setRTCTime(dtpCustom.Value);
        }

        private void timer2_Tick(object sender, EventArgs e)
        {
            //progressBar1.PerformStep();
            UpdateTimer--;
            label6.Text = "Обновление через: " + UpdateTimer.ToString() + " сек.";
            progressBar1.PerformStep();

        }
    }
}
