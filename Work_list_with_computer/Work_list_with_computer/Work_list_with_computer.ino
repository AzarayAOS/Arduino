//#include <iarduino_DHT.h> // Подключаем библиотеку для работы с датчиком DHT
//iarduino_DHT sensor(2);   // Объявляем объект, указывая номер вывода, к которому подключён модуль
                            // СКЕТЧ РАБОТАЕТ, КАК С МОДУЛЕМ DHT-11, ТАК И С МОДУЛЕМ DHT-22.

#include <DHT.h>            // другая библиотека для работы с датчиком DHT11 и DHT22
#include <DHT_U.h>          // в которой автоматически выбирается подключенный датчик 
                            // и коректно вычисляются температура и влажность

#include <i2cmaster.h>


#define DHTPIN 2                               // Пин к которому подключен датчик
#define DHTTYPE DHT22                          // Используемый датчик DHT 22 (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);                      // Инициализируем датчик

const byte DS3231 = 0x68; // I2C адрес таймера DS3231


// Назначаем пины
int CS_pin = 10;
int pow_pin = 8; // Если вы используете SD Shield  


//==============START================  
void setup() 
{
    Serial.begin(9600);     // Инициируем передачу данных в монитор последовательного порта, на скорости 9600 бод
    dht.begin(); 

    
    
    //Назначаем пин CS_pin выходом
    pinMode(CS_pin, OUTPUT);


    //Если мы используем шилд то назначаем выходом пин для питания шилда
    pinMode(pow_pin, OUTPUT);  
    digitalWrite(pow_pin, HIGH);


  i2c_init(); //Initialise the i2c bus
  PORTC = (1 << PORTC4) | (1 << PORTC5);//enable pullups
  delay(3000);            // Приостанавливаем выполнение скетча на 3 секунду, для перехода датчика в активное состояние

 
}

void loop() 
{

  String newString="";
  String HumTem="";
  String Hum="";
  String Tem="";
  String Temper="";

  //===== ТЕМПЕРАТУРА И ВЛАЖНОСТЬ ===========



  float h,tm;
  h = dht.readHumidity();                 // Считывание влажности в процентах
  tm = dht.readTemperature();              // Считывание температуры в градусах цельсия

  //Serial.println("h: "+(String)h+" tm: "+(String)tm);
  if (isnan(h) || isnan(tm))                     // Проверяем, получилось считать данные 
    {
      Serial.println("Read error DHT22");    // Выводим текст
      return;                                  
    }
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(tm, h, false);
  Serial.println("h: "+(String)h+" tm: "+(String)tm+" hic: "+(String)hic);
  HumTem=(String)h+" "+(String)hic;
  Hum=(String)h;
  Tem=(String)hic;

  
  //========= ДАтчик с ИК метра ==========


  int dev = 0x5A<<1;
  int data_low = 0;
  int data_high = 0;
  int pec = 0;
 
  i2c_start_wait(dev+I2C_WRITE);
  i2c_write(0x07);
 
  // read
  i2c_rep_start(dev+I2C_READ);
  data_low = i2c_readAck(); //Read 1 byte and then send ack
  data_high = i2c_readAck(); //Read 1 byte and then send ack
  pec = i2c_readNak();
  i2c_stop();
 
  //This converts high and low bytes together and processes temperature, MSB is a error bit and is ignored for temps
  double tempFactor = 0.02; // 0.02 degrees per LSB (measurement resolution of the MLX90614)
  double tempData = 0x0000; // zero out the data
  int frac; // data past the decimal point
 
  // This masks off the error bit of the high byte, then moves it left 8 bits and adds the low byte.
  tempData = (double)(((data_high & 0x007F) << 8) + data_low);
  tempData = (tempData * tempFactor)-0.01;
 
  float celcius = tempData - 273.15;
 
  Serial.print("Celcius: ");
  Serial.println(celcius);
  Temper=celcius;

  delay(5000);                                       // Ждём 5 секунду.
}
