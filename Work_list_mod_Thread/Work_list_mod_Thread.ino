#include <iarduino_OLED_txt.h>                         // Подключаем библиотеку iarduino_OLED_txt.
iarduino_OLED_txt myOLED(0x78);                        // Объявляем объект myOLED, указывая адрес дисплея на шине I2C: 0x78 (если учитывать бит RW=0).
                                                       //
extern uint8_t SmallFontRus[];                         // Подключаем шрифт SmallFontRus.
#include<Thread.h>
   
#include <iarduino_DHT.h> // Подключаем библиотеку для работы с датчиком DHT
iarduino_DHT sensor(2);   // Объявляем объект, указывая номер вывода, к которому подключён модуль
                          // СКЕТЧ РАБОТАЕТ, КАК С МОДУЛЕМ DHT-11, ТАК И С МОДУЛЕМ DHT-22.


#include <i2cmaster.h>
#include <Wire.h>
const byte DS3231 = 0x68; // I2C адрес таймера DS3231
#include <SD.h> 

bool flag;
Thread DisplaySeen=Thread(); // создаём поток параллельного выполнения для отображения ан экран
Thread DisplayCalculate=Thread(); // для подсчёта 
Thread WriteToFile=Thread();    // запись в файл





float refresh_rate = 5000.0;
int hex_to_dec(int hex)
{
  int dec=0;
dec=(int)(hex/16)*10;
dec+=hex%16;
return dec;
}      
//String StrData="";
//String StrTime="";
//int countround=0;     // количество записей

 bool flagThread=false;
 String newString="";
  String HumTem="HT";
  String Hum="T";
  String Tem="T";
  String DataTime="DT";
  String Temper="Tm";
  String StrData="Sd";
  String StrTime="ST";

   int countround=0;     // количество записей


void WriteFile()
{

//========= SD КАРТА ===========


newString=DataTime+" "+HumTem+" "+Temper;
  
  // Строка которую будем записывать на карту
 
 
 // Открываем файл и записываем строку
 // В одно время можно открывать только один файл
 // Если файла нет то он будет создан
 File logFile = SD.open("LOG.csv", FILE_WRITE);


  if (logFile) 
  {
    logFile.println(newString);
    logFile.close();
    Serial.println(newString);
  } 
  else 
  {
    Serial.println("LOG.csv");
    Serial.println("Couldn't open log file");
  }
  countround++;
  
}



void Display()
{
  Wire.beginTransmission(DS3231); // начинаем обмен с DS3231
  Wire.write(byte(0x00)); // записываем адрес регистра, с которого начинаем чтение!!!
  Wire.endTransmission(); // завершаем передачу

  byte t[7]; // массив для хранения даты и времени
  int i = 0; // индекс текущего элемента массива
  Wire.beginTransmission(DS3231); // начинаем обмен с DS3231
  Wire.requestFrom(DS3231, 7); // запрашиваем 7 байтов у DS3231
  while(Wire.available()) { // пока есть данные от DS3231
    t[i] = Wire.read(); // читаем 1 байт и сохраняем в массив t
    i++; // инкрементируем индекс элемента массива
  }
  Wire.endTransmission(); // завершаем обмен
  
 //Serial.println(printDateTime(t)); // выводим секунды  ; // выводим дату и время
 DataTime=printDateTime(t);



  
  //
   myOLED.clrScr();                                   // Чистим экран.
    myOLED.print( "Дата: " +StrData   ,      0, 0);    // Выводим текст начиная с 0 столбца 0 строки.
    myOLED.print( "Время: "+ StrTime   ,      0, 1);    // Выводим текст начиная с 0 столбца 1 строки.
    myOLED.print( "Hum:  "+Hum +"%"    , 0, 3);
    myOLED.print( "Tem:  "+Tem     , 0, 4);
    myOLED.print( "IR_s: "+ Temper , 0, 5);    // Выводим текст по центру 4 строки.
    myOLED.print( "Количество записей "  , 0, 6);    // Выводим текст по центру 6 строки.
    myOLED.print( countround  , 3, 7);    // Выводим текст по центру 7 строки.
    delay(1000);
 
}

void Calcualte()
{
//String newString="";
  //String HumTem="";
  //String Hum="";
  //String Tem="";
  //String DataTime="";
  //String Temper="";

   //===== ТЕМПЕРАТУРА И ВЛАЖНОСТЬ ===========
                  
  //Serial.print  ("CEHCOP B KOMHATE: ");
  switch(sensor.read()){  // Читаем показания датчика
    case DHT_OK:             {//Serial.println((String) sensor.hum + "% - " + sensor.tem + "*C"); 
                              HumTem=(String) sensor.hum+" "+sensor.tem; 
                              Hum=(String) sensor.hum;
                              Tem=sensor.tem;
                              break;}
    case DHT_ERROR_CHECKSUM: Serial.println("HE PABEHCTBO KC");                                break;
    case DHT_ERROR_DATA:     Serial.println("OTBET HE COOTBETCTB. CEHCOPAM 'DHT'");            break;
    case DHT_ERROR_NO_REPLY: Serial.println("HET OTBETA");                                     break;
    default:                 Serial.println("ERROR");                                          break;
  } 

//===== ДАТА И ВРЕМЯ ===========
  
  Wire.beginTransmission(DS3231); // начинаем обмен с DS3231
  Wire.write(byte(0x00)); // записываем адрес регистра, с которого начинаем чтение!!!
  Wire.endTransmission(); // завершаем передачу

  byte t[7]; // массив для хранения даты и времени
  int i = 0; // индекс текущего элемента массива
  Wire.beginTransmission(DS3231); // начинаем обмен с DS3231
  Wire.requestFrom(DS3231, 7); // запрашиваем 7 байтов у DS3231
  while(Wire.available()) { // пока есть данные от DS3231
    t[i] = Wire.read(); // читаем 1 байт и сохраняем в массив t
    i++; // инкрементируем индекс элемента массива
  }
  Wire.endTransmission(); // завершаем обмен
  
 //Serial.println(printDateTime(t)); // выводим секунды  ; // выводим дату и время
 DataTime=printDateTime(t);

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
 
  //Serial.print("Celcius: ");
  //Serial.println(celcius);
  Temper=celcius;

/*
//========= SD КАРТА ===========


newString=DataTime+" "+HumTem+" "+Temper;
  
  // Строка которую будем записывать на карту
 
 
 // Открываем файл и записываем строку
 // В одно время можно открывать только один файл
 // Если файла нет то он будет создан
 File logFile = SD.open("LOG.csv", FILE_WRITE);


  if (logFile) 
  {
    logFile.println(newString);
    logFile.close();
    Serial.println(newString);
  } 
  else 
  {
    Serial.println("LOG.csv");
    Serial.println("Couldn't open log file");
  }
  countround++;*/
  
}



// Назначаем пины
int CS_pin = 10;
int pow_pin = 8; // Если вы используете SD Shield  
// разбирает считанный массив и выводит дату и время 
String printDateTime(byte *arr) {
  String strreturn="";
  
  if (arr[4]<10) strreturn="0"; // дополнение нулями слева для выравнивания
  strreturn=strreturn+hex_to_dec(arr[4]); // выводим дату
  strreturn=strreturn+".";
  if (arr[5]<10) strreturn=strreturn+"0"; 
  strreturn=strreturn+hex_to_dec(arr[5]); // выводим месяц
  strreturn=strreturn+".20"; // 2000-ые годы подразумеваются
  strreturn=strreturn+hex_to_dec(arr[6]); // выводим год

  StrData=strreturn;
  
  //strreturn=strreturn+"_";
  strreturn="";
  if (arr[2]<10) strreturn=strreturn+"0"; 
  
  strreturn=hex_to_dec(arr[2]); // выводим час
  strreturn=strreturn+":";
  if (arr[1]<10) strreturn=strreturn+"0"; 
  strreturn=strreturn+hex_to_dec(arr[1]); // выводим минуты
  strreturn=strreturn+":";
  if (arr[0]<10) strreturn=strreturn+"0"; 
  strreturn=strreturn+hex_to_dec(arr[0]);

  StrTime=strreturn;

  strreturn=StrData+"_"+StrTime;


   return strreturn;
}

void DisplTRest()
{

  
  //
    myOLED.clrScr();                                   // Чистим экран.
    myOLED.print( "Обновление экрана:"    ,      0, 0);    // Выводим текст начиная с 0 столбца 0 строки.
    myOLED.print( " 5 сек"    ,      3, 1);    // Выводим текст начиная с 0 столбца 0 строки.
    myOLED.print( "Запись на SD: ", OLED_C, 3);    // Выводим текст по центру 2 строки.
     myOLED.print( " каждые 60 сек", 3, 4);    // Выводим текст по центру 2 строки.
    
    delay(5000);                                       // Ждём 3 секунды.
    
   
    


}



                   
void setup(){                                          //
    myOLED.begin();                                    // Инициируем работу с дисплеем.
    myOLED.setFont(SmallFontRus);                      // Указываем шрифт который требуется использовать для вывода цифр и текста.
//  myOLED.setCoding(TXT_UTF8);                        // Указываем кодировку текста в скетче. Если на дисплее не отображается Русский алфавит, то ...
                                                      // раскомментируйте функцию setCoding и замените параметр TXT_UTF8, на TXT_CP866 или TXT_WIN1251.
  Serial.begin(9600);     // Инициируем передачу данных в монитор последовательного порта, на скорости 9600 бод
  Wire.begin();

    Serial.println("Initializing Card");
  //Назначаем пин CS_pin выходом
  pinMode(CS_pin, OUTPUT);

  //Если мы используем шилд то назначаем выходом пин для питания шилда
  pinMode(pow_pin, OUTPUT);  
  digitalWrite(pow_pin, HIGH);

  // Проверяем доступность карты
  if (!SD.begin(CS_pin)) {
  Serial.println("Card Failure");
  return;
  }
  Serial.println("Card Ready");


  
  DisplaySeen.onRun(Display);
  DisplaySeen.setInterval(5000);

  DisplayCalculate.onRun(Calcualte);
  DisplayCalculate.setInterval(3000);
  
  WriteToFile.onRun(WriteFile);
  WriteToFile.setInterval(10000);

  //if((SD.exists("LOG.csv"))){;}
 // else
  {
  File logFile = SD.open("LOG.csv", FILE_WRITE);
 
 if (logFile) {
 logFile.println("Date/Time Humidity temperature IR_sensor");
 logFile.close();
 Serial.println("Date/Time Humidity temperature IR_sensor");
 } else {
 Serial.println("LOG.csv");
 Serial.println("Couldn't open log file");
 }
  }

  i2c_init(); //Initialise the i2c bus
  PORTC = (1 << PORTC4) | (1 << PORTC5);//enable pullups
  delay(1000);            // Приостанавливаем выполнение скетча на 1 секунду, для перехода датчика в активное состояние

 
   DisplTRest();


  Calcualte();
  delay(1000);            // Приостанавливаем выполнение скетча на 1 секунду, для перехода датчика в активное состояние
}                                                     

void loop(){ 


  // Првоеряем, пришло ли время провести новое снятие информации с датчиков
  if(DisplayCalculate.shouldRun())
    DisplayCalculate.run();
    
  
  // Проверяем, пришло ли время обновления экрана

  if(DisplaySeen.shouldRun())
    DisplaySeen.run();

  // првоеряем, пришло ли время записать данные в файл
  if(WriteToFile.shouldRun())
      WriteToFile.run();
        
/*
  //String newString="";
  //String HumTem="";
  //String Hum="";
  //String Tem="";
  //String DataTime="";
  //String Temper="";

   //===== ТЕМПЕРАТУРА И ВЛАЖНОСТЬ ===========
                  
  //Serial.print  ("CEHCOP B KOMHATE: ");
  switch(sensor.read()){  // Читаем показания датчика
    case DHT_OK:             {//Serial.println((String) sensor.hum + "% - " + sensor.tem + "*C"); 
                              HumTem=(String) sensor.hum+" "+sensor.tem; 
                              Hum=(String) sensor.hum;
                              Tem=sensor.tem;
                              break;}
    case DHT_ERROR_CHECKSUM: Serial.println("HE PABEHCTBO KC");                                break;
    case DHT_ERROR_DATA:     Serial.println("OTBET HE COOTBETCTB. CEHCOPAM 'DHT'");            break;
    case DHT_ERROR_NO_REPLY: Serial.println("HET OTBETA");                                     break;
    default:                 Serial.println("ERROR");                                          break;
  } 

//===== ДАТА И ВРЕМЯ ===========
  
  Wire.beginTransmission(DS3231); // начинаем обмен с DS3231
  Wire.write(byte(0x00)); // записываем адрес регистра, с которого начинаем чтение!!!
  Wire.endTransmission(); // завершаем передачу

  byte t[7]; // массив для хранения даты и времени
  int i = 0; // индекс текущего элемента массива
  Wire.beginTransmission(DS3231); // начинаем обмен с DS3231
  Wire.requestFrom(DS3231, 7); // запрашиваем 7 байтов у DS3231
  while(Wire.available()) { // пока есть данные от DS3231
    t[i] = Wire.read(); // читаем 1 байт и сохраняем в массив t
    i++; // инкрементируем индекс элемента массива
  }
  Wire.endTransmission(); // завершаем обмен
  
 //Serial.println(printDateTime(t)); // выводим секунды  ; // выводим дату и время
 DataTime=printDateTime(t);

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
 
  //Serial.print("Celcius: ");
  //Serial.println(celcius);
  Temper=celcius;


//========= SD КАРТА ===========


newString=DataTime+" "+HumTem+" "+Temper;
  
  // Строка которую будем записывать на карту
 
 
 // Открываем файл и записываем строку
 // В одно время можно открывать только один файл
 // Если файла нет то он будет создан
 File logFile = SD.open("LOG.csv", FILE_WRITE);


  if (logFile) 
  {
    logFile.println(newString);
    logFile.close();
    Serial.println(newString);
  } 
  else 
  {
    Serial.println("LOG.csv");
    Serial.println("Couldn't open log file");
  }
  countround++;*/
  
  /*if(flagThread)
  {
    DisplaySeen.run();
    flagThread=false;
  }
 */
 /*
 
  //
    myOLED.clrScr();                                   // Чистим экран.
    myOLED.print( "Дата: " +StrData   ,      0, 0);    // Выводим текст начиная с 0 столбца 0 строки.
    myOLED.print( "Время: "+ StrTime   ,      0, 1);    // Выводим текст начиная с 0 столбца 1 строки.
    myOLED.print( "Hum:  "+Hum +"%"    , 0, 3);
    myOLED.print( "Tem:  "+Tem     , 0, 4);
    myOLED.print( "IR_s: "+ Temper , 0, 5);    // Выводим текст по центру 4 строки.
    myOLED.print( "Количество записей "  , 0, 6);    // Выводим текст по центру 6 строки.
    myOLED.print( countround  , 3, 7);    // Выводим текст по центру 7 строки.
    */
    //delay(60000);                                       // Ждём 60 секунду.
   

    
}                                                     
