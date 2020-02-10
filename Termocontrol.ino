#include <EEPROM.h>              // Чтение запись всякой хуйни в память
#include <SoftwareSerial.h>      // Подключение библиотеки последовательного порта
#include <OneWire.h>

SoftwareSerial BTSerial(10,11);
// Подключение термометра
#include <DallasTemperature.h>
#define SENSOR_PIN 5              // датчик температуры
#define FAN_PIN 9                 // Пин МОСФЕТА на вентиляторы
#define TEMPERATURE_PRECISION 9   // Расширение бит точности измерения температуры

int sensor_temp,statusFan,maxtemp,hysteresis,auto_fan;

OneWire oneWire(SENSOR_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress Thermometer;

// Обхявление глобальных переменных //
String input_string = "";        // Служебнная переменная для определения других команд и их значений

// Установка основных параметров подключениея
void setup(){
  BTSerial.begin(38400);          // Старт серийного порта Bluetooth
  Serial.begin(9600);             // Старт  серийного порта отладки
  pinMode(FAN_PIN, OUTPUT);       // Подключение мосфета
  sensors.begin();
  sensors.getAddress(Thermometer, 0);
  sensors.setResolution(Thermometer, TEMPERATURE_PRECISION);
  
  maxtemp = EEPROM.read(0);     // Чтение максимальной температуры
  hysteresis = EEPROM.read(1);  // Чтение гистерезиса
}

// Основной цикл программы //
void loop(){
  sendBluetooth();
  getBluetooth();
  getSerial();
  Cooling();
  delay(500);
}

void getBluetooth(){
  while(BTSerial.available() > 0){
    // Запись байтов в переменную с буфера последовательного порта поступаемых с телефона
    char c = BTSerial.read();
    Serial.print(c);
   // Проверка символа перевода строки (CL) в конце сообщения
    if (c == '\n') { 
      Command();                // Выполение комманды
      input_string = "";        // После выполенения команды стирать сообщение
      }
    // Проверка символа в конце сообщения
    else if (c == '/'){
     Command();
     input_string = "";
    } else input_string += c;
  }  
}

// Конечная отпрвка всех переменных по Bluetooth
void sendBluetooth(){
  BTSerial.print(sensor_temp);
  BTSerial.print("|");
  BTSerial.print(maxtemp);
  BTSerial.print("|");
  BTSerial.print(hysteresis);
  }
*/

// Команды выполняемые при приёме сообщения по последовательному порту Bluetooth
void Command() {
  if (input_string.startsWith("M") == true)   //Если строка начиинается с символа - M
    {
        input_string.replace("M", "");        // Удаляем со строки определяющий символ - M
        maxtemp = input_string.toInt();    // Преобразовываем строку в целое число (int), если необходимо с плавающей запятой пишем toFloat()
        EEPROM.update(0, maxtemp);
        Serial.println("Установка максимальной температуры");
        delay(10);                          // Служебная задержка
    }
  if (input_string.startsWith("H") == true)   //Если строка начиинается с символа - H
    {
        input_string.replace("H", "");        // Удаляем со строки определяющий символ - H
        hysteresis = input_string.toInt();    // Преобразовываем строку в целое число (int), если необходимо с плавающей запятой пишем toFloat()
        EEPROM.update(1, hysteresis);
        Serial.println("Установка гистерезиса");
        delay(10);                          // Служебная задержка
    }
  if (input_string == "AUTO_OFF")   //Если строка начиинается с символа - H
    {
        auto_fan = 1;
        Serial.println("Режим постоянной работы вентиляторов");
        delay(10);                          // Служебная задержка
    }
  if (input_string == "AUTO_ON")   //Если строка начиинается с символа - H
    {
        auto_fan = 0;
        Serial.println("Режим автоматической работы вентиляторов");
        delay(10);                          // Служебная задержка
    }
}

void Cooling() {
  sensors.requestTemperatures();
  sensor_temp = sensors.getTempC(Thermometer);
  if(sensor_temp >= maxtemp && auto_fan == 0){
    digitalWrite(FAN_PIN, HIGH);
    delay(10);
  }
  //
  if(sensor_temp <= maxtemp - hysteresis){
    digitalWrite(FAN_PIN, LOW);
    delay(10);
    }
  if(auto_fan == 1) 
    {
      digitalWrite(FAN_PIN, HIGH);
    }
  
}

void getSerial(){
  while(Serial.available() > 0){
    // Запись байтов в переменную с буфера последовательного порта поступаемых с телефона
    char c = Serial.read();
    Serial.write(c);
   // Проверка символа перевода строки (CL) в конце сообщения
    if (c == '\n') { 
      Command();                // Выполение комманды
      input_string = "";        // После выполенения команды стирать сообщение
      }
    // Проверка символа в конце сообщения
    else if (c == '/'){
     Command();
     input_string = "";
    } else input_string += c;
  }  
}
