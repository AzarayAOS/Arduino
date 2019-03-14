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
        const int DT = 60;      // через сколько секунд делать запись в таблицу
        PrivateFontCollection pfc = new PrivateFontCollection();
        private SerialManager serialManager = new SerialManager();
        private bool connected = false;
        private string sketchVersion = "2.0";
        private byte UpdateTimer = 6;       // перменная для регулярного обновления данных с устройства
        private  uint TableUpdate = 100;    // переменная для регулярного добавления записей в тадлицу
        //private string FileWriteSave;       // путь к каталогу, куда регулярно надо занасить данные
        
        


        private string[] inforstr;


        

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {            
            
            dtpCustom.Value = DateTime.Now;
            

            textBox1.Text = Application.StartupPath.ToString() + "\\result.csv";
            textBox2.Text = Application.StartupPath.ToString()+"\\";
            saveFileDialog1.Filter = "CSV файл(*.csv)|*.csv;*.CSV|Все файлы(*.*)|*.*";
            //saveFileDialog1.InitialDirectory = Application.StartupPath.ToString();

            
            
            addAvailableComPorts();
            updateUI();
        }
        /// <summary>
        /// Запись данных в таблицу
        /// </summary>
        private void SetTablice()
        {
            dataGridView1.Rows.Add(
                dataGridView1.Rows.Count.ToString(), 
                DateTime.Now.ToString("yyyy.MM.dd"), 
                DateTime.Now.ToString("HH:mm"), 
                inforstr[0], 
                inforstr[1], 
                inforstr[2]);

            // перевод фокуса на последнюю занесённую в таблицу строку
            dataGridView1.FirstDisplayedCell = dataGridView1.Rows[dataGridView1.Rows.Count - 1].Cells[0];


        }


        /// <summary>
        /// Регулятная запись данных в файл с текущей датой
        /// </summary>
        void FileWriteTimmer()
        {

            string temp="";

            try
            {
                string FileNameData = DateTime.Now.ToString("dd.MM.yyyy") + ".csv";
                FileInfo fInfo = new FileInfo(textBox2.Text + FileNameData);

                if (!fInfo.Exists)
                {
                    FileStream fsTemp = new FileStream(textBox2.Text + FileNameData, FileMode.Append);
                    StreamWriter streamWriterTemp = new StreamWriter(fsTemp);
                    streamWriterTemp.WriteLine("Time Humidity Temperature IR_Sensor");
                    streamWriterTemp.Close();
                    fsTemp.Close();
                }


                FileStream fs = new FileStream(textBox2.Text + FileNameData, FileMode.Append);
                StreamWriter streamWriter = new StreamWriter(fs);

                temp = DateTime.Now.ToString("HH:mm").ToString() + " "
                    + inforstr[0] + " "
                    + inforstr[1] + " "
                    + inforstr[2];
                temp = temp.Replace('.', ',');

                streamWriter.WriteLine(temp);

                streamWriter.Close();
                fs.Close();

            }
            catch (Exception)
            {
                MessageBox.Show("Ошибка при добавлении записис в файл в "+ DateTime.Now.ToString("HH:mm:ss").ToString()+"!");
            }

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
                btConnect.Text = "РАЗЪЕДЕНИТЬ";
                toolStripStatusLabel1.Text = "Эскиз программы v" + sketchVersion;
                toolStripStatusLabel2.Text = "Эскиз программы датчиков v" + serialManager.getSketchVersion();
                btGet.Enabled = true;
                btSetSystem.Enabled = true;
                btSetCustom.Enabled = true;
                dtpCustom.Enabled = true;
                cbSerialPorts.Enabled = false;
            }
            else
            {
                btConnect.Text = "СОЕДЕНИТЬ";
                toolStripStatusLabel1.Text = "Разъединенно";
                toolStripStatusLabel2.Text = "Нет информации";
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


            if(TableUpdate>=DT)
            {
                SetTablice();

                if (checkBox1.Checked) 
                    FileWriteTimmer();

                TableUpdate = 0;
                progressBar1.Value = 0;
            }
            TableUpdate += 5;
           // progressBar1.PerformStep();
            //SetTablice();


        }

        private void btConnect_Click(object sender, EventArgs e)
        {
            System.Threading.Thread.Sleep(1500);
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
                    //checkBox1.Enabled = false;
                    UpdateTimer = 6;
                    progressBar1.Value = 0;
                    groupBox5.Enabled = false;
                    
                    updateUI();
                }
                else serialManager.disconnect();
            }

            else
            {
                timer1.Enabled = false;
                timer2.Enabled = false;
                //checkBox1.Enabled = true;
                groupBox5.Enabled = true;
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

        private void button1_Click(object sender, EventArgs e)
        {
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                textBox1.Text = saveFileDialog1.FileName;
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            //FileStream fs = new FileStream(textBox1.Text, FileMode.Append);
            //StreamWriter streamWriter = new StreamWriter(fs);
            string filewrite;
            try
            {
                FileInfo fInfo = new FileInfo(textBox1.Text);
                bool flag = !fInfo.Exists;
                if (flag)
                {

                    FileStream fsTemp = new FileStream(textBox1.Text, FileMode.Append);
                    StreamWriter streamWriterTemp = new StreamWriter(fsTemp);
                    streamWriterTemp.WriteLine("Number Date Time Humidity Temperature IR_Sensor");
                    streamWriterTemp.Close();
                    fsTemp.Close();
                }

                FileStream fs = new FileStream(textBox1.Text, FileMode.Append);
                StreamWriter streamWriter = new StreamWriter(fs);

                string temp;

                for (int j = 0; j < dataGridView1.Rows.Count-1; j++)
                {
                    filewrite = "";
                    for (int i = 0; i < dataGridView1.Rows[j].Cells.Count; i++)
                    {
                        //streamWriter.Write(dataGridView1.Rows[j].Cells[i].Value + " ");

                        // заменяем все точки в строках на запятые, кроме в ячейке "дата" перед записью в файл
                        temp = dataGridView1.Rows[j].Cells[i].Value.ToString();
                        if (i != 1) temp = temp.Replace('.', ',');

                        //filewrite = filewrite + dataGridView1.Rows[j].Cells[i].Value + " ";
                        filewrite = filewrite + temp + " ";
                    }
                    //filewrite = filewrite.Replace('.', ',');
                    streamWriter.WriteLine(filewrite);
                }

                streamWriter.Close();
                fs.Close();
                MessageBox.Show(flag? "Файл успешно сохранен": "Файл успешно дописан");

            }
            catch (Exception)
            {

                MessageBox.Show("Ошибка при сохранении файла!");
            }

        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (folderBrowserDialog1.ShowDialog() == DialogResult.OK)
                textBox2.Text = folderBrowserDialog1.SelectedPath+"\\";
        }
    }
}
