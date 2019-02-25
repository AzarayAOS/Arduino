#include <iarduino_DHT.h> // Подключаем библиотеку для работы с датчиком DHT
iarduino_DHT sensor(2);   // Объявляем объект, указывая номер вывода, к которому подключён модуль
                          // СКЕТЧ РАБОТАЕТ, КАК С МОДУЛЕМ DHT-11, ТАК И С МОДУЛЕМ DHT-22.
void setup(){
  Serial.begin(9600);     // Инициируем передачу данных в монитор последовательного порта, на скорости 9600 бод
  delay(1000);            // Приостанавливаем выполнение скетча на 1 секунду, для перехода датчика в активное состояние
}
void loop(){                 Serial.print  ("CEHCOP B KOMHATE: ");
  switch(sensor.read()){  // Читаем показания датчика
    case DHT_OK:             Serial.println((String) sensor.hum + "% - " + sensor.tem + "*C"); break;
    case DHT_ERROR_CHECKSUM: Serial.println("HE PABEHCTBO KC");                                break;
    case DHT_ERROR_DATA:     Serial.println("OTBET HE COOTBETCTB. CEHCOPAM 'DHT'");            break;
    case DHT_ERROR_NO_REPLY: Serial.println("HET OTBETA");                                     break;
    default:                 Serial.println("ERROR");                                          break;
  } delay(2000);          // Приостанавливаем выполнение скетча на 2 секунды, между выводами показаний
}
