using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;
using System.Windows.Forms;
using System.Threading;

namespace RTCSetup
{
    class SerialManager
    {
        private SerialPort serialPort;

        public string[] getAvailablePorts()
        {
            return SerialPort.GetPortNames();
        }

        public bool connect(string serialPortName)
        {
            serialPort = new SerialPort(serialPortName, 57600);
            serialPort.NewLine = "\r\n";
            serialPort.ReadTimeout = 500;

            try
            {
                serialPort.Open();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Невозможно открыть последовательный порт: " + ex.Message, "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
            return true;
        }

        // Первое рукопожатие с устройством:
        // когда он получил ##, он должен ответить с !!
        public bool handshake()
        {
            serialPort.WriteLine("##");

            string response = "";
            try
            {
                response = serialPort.ReadLine();
            }
            catch (Exception)
            {
                MessageBox.Show("Нет данных из последовательного порта", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
            
            if(response.Equals("!!")) return true;
            MessageBox.Show("Неизвестное устройство", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
            return false;
        }

        // Получить версию бегущего эскиза
        // команда:? V
        // ответ: <version> (например, «1.0»)
        public string getSketchVersion()
        {
            serialPort.WriteLine("?V");

            string response = "";
            try
            {
                response = serialPort.ReadLine();
            }
            catch (Exception)
            {
                MessageBox.Show("Нет данных из последовательного порта", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            return response;
        }

        // Получить фактическую дату и время RTC
        // команда:? T
        // ответ: дата и время в формате дд / мм / гггг чч: мм: сс
        public string getRTCTime()
        {
            serialPort.WriteLine("?T");

            string response = "";
            try
            {
                response = serialPort.ReadLine();
            }
            catch (Exception)
            {
                MessageBox.Show("Нет данных из последовательного порта", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return "UNKNOWN";
            }
            return response;
        }




        /// <summary>
        /// Получить информацию с датчиков, команда ?D
        /// </summary>
        /// <returns>возвращает строку с ответом</returns>
        public string getInfo()
        {
            serialPort.WriteLine("?D");
            string response = "";

            try
            {
                response = serialPort.ReadLine();
            }
            catch (Exception)
            {
                MessageBox.Show("Нет данных из последовательного порта", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return "UNKNOWN";
            }

            
            return response;
        }

        // Установка даты и времени RTC
        // команда:! TddMMyyyyhhmmss
        // ответ: ОК
        public bool setRTCTime(DateTime time)
        {
            // Wait until we're 200ms before the next second
            while (DateTime.Now.Millisecond != 850)
                Thread.Sleep(1);

            string command = "!T" + time.AddSeconds(1).ToString("ddMMyyyyHHmmss");
            serialPort.WriteLine(command);

            string response = "";
            try
            {
                response = serialPort.ReadLine();
            }
            catch (Exception)
            {
                MessageBox.Show("Нет данных из последовательного порта", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }

            if (response.Equals("OK"))
            {
                MessageBox.Show("RTC установлен!", "Операция успешна", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return true;
            }
            MessageBox.Show("Неправильный ответ от RTC", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
            return false;
        }

        public void disconnect()
        {
            serialPort.Close();
        }
    }
}
