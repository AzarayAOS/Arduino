#include <DHT.h>            // другая библиотека для работы с датчиком DHT11 и DHT22
#include <DHT_U.h>          // в которой автоматически выбирается подключенный датчик 
                            // и коректно вычисляются температура и влажность
#include <i2cmaster.h>

#define BUFFER_SIZE 20      // размеры буфера для связи с ПС
#define VERSION     "2.0"   // версия подпрограммы Arduino

#define DHTPIN 2                               // Пин к которому подключен датчик
#define DHTTYPE DHT22                          // Используемый датчик DHT 22 (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);                      // Инициализируем датчик

const byte DS3231 = 0x68; // I2C адрес таймера DS3231


// Назначаем пины
int CS_pin = 10;
int pow_pin = 8; // Если вы используете SD Shield

// Буфер для входящих данных
char serial_buffer[BUFFER_SIZE];
int buffer_position;

//===== ТЕМПЕРАТУРА И ВЛАЖНОСТЬ ===========
// функция получени температуры и влажности с датчика DHT11/22
String DHT_Get()
{
  String HumTem="";
  
  float h,tm;
  h = dht.readHumidity();                 // Считывание влажности в процентах
  tm = dht.readTemperature();              // Считывание температуры в градусах цельсия

  //Serial.println("h: "+(String)h+" tm: "+(String)tm);
  if (isnan(h) || isnan(tm))                     // Проверяем, получилось считать данные 
    {
      //Serial.println("Read error DHT22");    // Выводим текст
      return;                                  
    }
  // Вычислить тепловой индекс в градусах Цельсия (isFahreheit = false)
  float hic = dht.computeHeatIndex(tm, h, false);
  //Serial.println("h: "+(String)h+" tm: "+(String)tm+" hic: "+(String)hic);
  HumTem=(String)h+" "+(String)tm;
  return HumTem;
}

//========= ДАтчик с ИК метра ==========
// функция получени температуры с датчика ИК-датчика
double IR_Get()
{

  int dev = 0x5A<<1;
  int data_low = 0;
  int data_high = 0;
  int pec = 0;
 
  i2c_start_wait(dev+I2C_WRITE);
  i2c_write(0x07);
 
  // читать
  i2c_rep_start(dev+I2C_READ);
  data_low = i2c_readAck(); //Прочитайте 1 байт, а затем отправьте подтверждение
  data_high = i2c_readAck(); //Прочитайте 1 байт, а затем отправьте подтверждение
  pec = i2c_readNak();
  i2c_stop();
 
  //Это преобразовывает старший и младший байты вместе и обрабатывает температуру, MSB - бит ошибки и игнорируется для временных
  double tempFactor = 0.02; // 0,02 градуса на LSB (разрешение измерения MLX90614)
  double tempData = 0x0000; // обнулять данные
  int frac; // данные после десятичной точки
 
  // Это маскирует бит ошибки старшего байта, затем перемещает его влево на 8 бит и добавляет младший байт.
  tempData = (double)(((data_high & 0x007F) << 8) + data_low);
  tempData = (tempData * tempFactor)-0.01;
 
  float celcius = tempData - 273.15;
 
  //Serial.print("Celcius: ");
  //Serial.println(celcius);

  return celcius;
}

//==============START================  

void setup() {
  
  Serial.begin(57600);

  buffer_position = 0;

//Назначаем пин CS_pin выходом
    pinMode(CS_pin, OUTPUT);
    //Если мы используем шилд то назначаем выходом пин для питания шилда
    pinMode(pow_pin, OUTPUT);  
    digitalWrite(pow_pin, HIGH);
  i2c_init(); //Инициализация шины i2c
  PORTC = (1 << PORTC4) | (1 << PORTC5);//enable pullups
  delay(3000);            // Приостанавливаем выполнение скетча на 3 секунду, для перехода датчика в активное состояние 
}

void loop() {
  
  // Ожидание поступающих данных по последовательному порту
  if (Serial.available() > 0) {
    
    // Читаем входящий символ
    char incoming_char = Serial.read();
    
    // Конец линии?
    if(incoming_char == '\n') {
      
      // Разбор команды
      
      // ## 
      if(serial_buffer[0] == '#' && serial_buffer[1] == '#')
        Serial.println("!!");
        
      // ?V
      else if(serial_buffer[0] == '?' && serial_buffer[1] == 'V')
        Serial.println(VERSION);

      // ?T 
      else if(serial_buffer[0] == '?' && serial_buffer[1] == 'T') {        
       /* DateTime now = RTC.now();
        char time_string[20];
        sprintf(time_string, "%02d/%02d/%d %02d:%02d:%02d", 
          now.day(), now.month(), now.year(),
          now.hour(), now.minute(), now.second());
          Serial.println(time_string);*/
      }

      // !T 
      else if(serial_buffer[0] == '!' && serial_buffer[1] == 'T') {

        String time_string = String(serial_buffer);
        /*int day = time_string.substring(2, 4).toInt();
        int month = time_string.substring(4, 6).toInt();        
        int year = time_string.substring(6, 10).toInt();
        int hour = time_string.substring(10, 12).toInt();
        int minute = time_string.substring(12, 14).toInt();
        int second = time_string.substring(14, 16).toInt();
        DateTime set_time = DateTime(year, month, day, hour, minute, second);
        RTC.adjust(set_time);*/
        Serial.println("OK");
      }// ?D
      else if(serial_buffer[0] == '?' && serial_buffer[1] == 'D')
        Serial.println(DHT_Get()+" "+(String)IR_Get());
      
      // Сбросить буфер
      buffer_position = 0;
    }
    
    // возврат каретки, ничего не делать
    else if(incoming_char == '\r');
    
    // Нормальный символ
    else {
      
      // Буфер заполнен, нам нужно сбросить его
      if(buffer_position == BUFFER_SIZE - 1) buffer_position = 0;

      // Сохраняем символ в буфере и перемещаем индекс
      serial_buffer[buffer_position] = incoming_char;
      buffer_position++;      
    }
  }    
}
