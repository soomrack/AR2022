#include <DHT.h>

//цифровой сигнал
#define DHTPIN 2            //контакт подклчения датчика температуры
#define RELAY 4             //контакт реле
#define WATER_PUMP 5        //помпа
#define LIGHT_DIODE 6       //лента
#define FAN 7               //вентилятор

//аналоговый сигнал
#define SENSOR_LIGHT A0     //сенсор освещения
#define SENSOR_WATER A1     //сенсор влажности

#define DHTTYPE DHT21       //DHT 21  (AM2301)
DHT dht(DHTPIN, DHTTYPE);   //инициализация dht датчика

//Variables
float hum;  //Stores humidity value
float temp; //Stores temperature value
std::string LIGHTING_SIGNAL = " ";
std::string FAN_SIGNAL = " ";
std::string RELAY_SIGNAL = " ";
std::string PUMP_SIGNAL = " ";

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  
  
  int sensorLight = analogRead(SENSOR_LIGHT);
  int sensorWater = analogRead(SENSOR_WATER);

  pinMode(DHTPIN, INPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(WATER_PUMP, OUTPUT);
  pinMode(LIGHT_DIODE, OUTPUT);
  pinMode(FAN, OUTPUT);

  LIGHTING_SIGNAL = (SENSOR_LIGHT > 1000) ? "HIGH" : "LOW";
  PUMP_SIGNAL = (SENSOR_WATER >= 500) ? "HIGH" : "LOW";
  FAN_SIGNAL = (SENSOR_WATER >= 500) ? "LOW" : "HIGH";

  digitalWrite(RELAY, RELAY_SIGNAL);
  digitalWrite(PUMP, PUMP_SIGNAL);
  digitalWrite(FAN, FAN_SIGNAL);
  digitalWrite(LIGHT_DIODE, LIGHTING_SIGNAL);
}