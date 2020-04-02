#include <EEPROM.h>              // Чтение запись всякой хуйни в память
#include <SoftwareSerial.h>      // Подключение библиотеки последовательного порта
#include <OneWire.h>
#include <DallasTemperature.h>
#include <GyverTM1637.h>

SoftwareSerial BTSerial(11, 10);
GyverTM1637 disp(19, 18);

#define SENSOR_PIN 8              // датчик температуры
#define FAN_PIN 6                 // Пин МОСФЕТА на вентиляторы
#define TEMPERATURE_PRECISION 9   // Расширение бит точности измерения температуры

int sensor_temp, maxtemp, hysteresis, auto_fan;

unsigned long sendTimer;

OneWire oneWire(SENSOR_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress Thermometer;

// Обхявление глобальных переменных //
String input_string = "";        // Служебнная переменная для определения других команд и их значений

// Установка основных параметров подключениея
void setup() {
  Serial.begin(9600);             // Старт  серийного порта отладки
  BTSerial.begin(36400);          // Старт серийного порта Bluetooth
  sensors.begin();
  sensors.getAddress(Thermometer, 0);
  sensors.setResolution(Thermometer, TEMPERATURE_PRECISION);

  disp.clear();
  for(int i = 0; i < 50;i++){
    disp.display(0, random(9));
    disp.display(1, random(9));
    disp.display(2, random(9));
    disp.display(3, random(9));
    disp.brightness(random(7));
    delay(50);
  }
  disp.clear();
  disp.brightness(7);  // яркость, 0 - 7 (минимум - максимум)
  
  pinMode(FAN_PIN, OUTPUT);       // Подключение мосфета
  
  maxtemp = EEPROM.read(0);     // Чтение максимальной температуры
  hysteresis = EEPROM.read(1);  // Чтение гистерезиса
}

// Основной цикл программы //
void loop() {
  getBluetooth(); // Получение данных с Bluetooth
  getSerial();    // Получение данных с Serial
  // Опрос температуры, управление вентилятором, отправка данных
  if (millis() - sendTimer > 1000) {
    sensors.requestTemperatures();
    sensor_temp = sensors.getTempC(Thermometer);
    byte a1 = sensor_temp%100/10;
    byte a2 = sensor_temp%10/1;

    disp.display(1, a1);
    disp.display(2, a2);
    
    disp.displayByte(3, 0x63);
    Cooling();
    sendData();
    sendTimer = millis();
  }
}
// Отправка данных
void sendData() {
  BTSerial.print(sensor_temp);
  BTSerial.print("|");
  BTSerial.print(maxtemp);
  BTSerial.print("|");
  BTSerial.print(hysteresis);
  Serial.print(sensor_temp);
  Serial.print("|");
  Serial.print(maxtemp);
  Serial.print("|");
  Serial.println(hysteresis);
}

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
  if (sensor_temp >= maxtemp && auto_fan == 0) {
    digitalWrite(FAN_PIN, HIGH);
    delay(10);
  }
  //
  if (sensor_temp <= maxtemp - hysteresis) {
    digitalWrite(FAN_PIN, LOW);
    delay(10);
  }
  if (auto_fan == 1)
  {
    digitalWrite(FAN_PIN, HIGH);
  }
}

void getSerial() {
  while (Serial.available() > 0) {
    // Запись байтов в переменную с буфера последовательного порта поступаемых с телефона
    char c = Serial.read();
    Serial.write(c);
    // Проверка символа перевода строки (CL) в конце сообщения
    if (c == '\n') {
      Command();                // Выполение комманды
      input_string = "";        // После выполенения команды стирать сообщение
    }
    // Проверка символа в конце сообщения
    else if (c == '/') {
      Command();
      input_string = "";
    } else input_string += c;
  }
}

void getBluetooth() {
  while (BTSerial.available() > 0) {
    // Запись байтов в переменную с буфера последовательного порта поступаемых с телефона
    char c = BTSerial.read();
    Serial.print(c);
    // Проверка символа перевода строки (CL) в конце сообщения
    if (c == '\n') {
      Command();                // Выполение комманды
      input_string = "";        // После выполенения команды стирать сообщение
    }
    // Проверка символа в конце сообщения
    else if (c == '/') {
      Command();
      input_string = "";
    } else input_string += c;
  }
}
