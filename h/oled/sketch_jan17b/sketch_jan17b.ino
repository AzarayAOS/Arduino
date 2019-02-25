#include <Wire.h>
const byte DS3231 = 0x68; // I2C адрес таймера DS3231

void setup() {
  Wire.begin(); 
  Serial.begin(9600);
}

void loop() {
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
  
  printDateTime(t); // выводим дату и время
  delay(1000); // пауза 1 секунда
}

// разбирает считанный массив и выводит дату и время 
void printDateTime(byte *arr) {
  if (arr[4]<10) Serial.print("0"); // дополнение нулями слева для выравнивания
  Serial.print(arr[4], HEX); // выводим дату
  Serial.print(".");
  if (arr[5]<10) Serial.print("0"); 
  Serial.print(arr[5], HEX); // выводим месяц
  Serial.print(".20"); // 2000-ые годы подразумеваются
  Serial.print(arr[6], HEX); // выводим год
  Serial.print(" ");
  if (arr[2]<10) Serial.print("0"); 
  Serial.print(arr[2], HEX); // выводим час
  Serial.print(":");
  if (arr[1]<10) Serial.print("0"); 
  Serial.print(arr[1], HEX); // выводим минуты
  Serial.print(":");
  if (arr[0]<10) Serial.print("0"); 
  Serial.println(arr[0], HEX); // выводим секунды  
}
