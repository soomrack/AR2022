#include <DHT.h>

//цифровой сигнал
#define DHTPIN             2//контакт подклчения датчика температуры
#define RELAY              4//контакт реле
#define WATER_PUMP         5//помпа
#define LIGHT_DIODE        6//лента
#define FAN                7//вентилятор

//аналоговый сигнал
#define SENSOR_LIGHT       A0//сенсор освещения
#define SENSOR_WATER       A1//сенсор влажности

#define DAY                8//время начала дня
#define NIGHT              24//время начала ночи

#define TIME_WORKING_LIGHT 2//время работы светодиодов
#define TIME_WORKING_PUMP  4//время работы помпы
#define TIME_WORKING_FAN   2//время работы вентилятора

#define DHTTYPE DHT21       //DHT 21  (AM2301)
DHT dht(DHTPIN, DHTTYPE);   //инициализация dht датчика

//Variables
float hum;  //Stores humidity value
float temp; //Stores temperature value
int time;   //Текущее время

std::string LIGHTING_SIGNAL = " ";
std::string FAN_SIGNAL = " ";
std::string RELAY_SIGNAL = " ";
std::string PUMP_SIGNAL = " ";

void StartSystem(int& t) {
  bool state_night = ((t / 100) < 24 && (t / 100) < 8) ? true : false;
  if (!state_night) {
    ControllSystem(t, state_night);
  }
}

void ControllSystem (const bool state) {
  Lighting(state);
  Pump();
  Fan();
}

int ResetTime(int& t) {
  t = (t > 24) ? 0 : t;
  return t;
}

void Lighting(const bool state) {
  LIGHTING_SIGNAL = (analogRead(SENSOR_LIGHT) > 1000) ? "HIGH" : "LOW";
  digitalWrite(LIGHT_DIODE, LIGHTING_SIGNAL);
}

void Pump(int& t) {
  PUMP_SIGNAL = (analogRead(SENSOR_WATER) >= 500) ? "LOW" : "HIGH";
  digitalWrite(PUMP, PUMP_SIGNAL);
}

void Fan(int& t) {
  FAN_SIGNAL = (analogRead(SENSOR_WATER) >= 500) ? "HIGH" : "LOW";
  digitalWrite(FAN, FAN_SIGNAL);
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  
  pinMode(DHTPIN, INPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(WATER_PUMP, OUTPUT);
  pinMode(LIGHT_DIODE, OUTPUT);
  pinMode(FAN, OUTPUT);

  time = 0;
}

void loop() {
  StartSystem(time);
  delay(10);
  time++;
  
  time = ResetTime(time);
}
