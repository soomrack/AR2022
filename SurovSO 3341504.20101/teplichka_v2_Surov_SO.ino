#include <TroykaDHT.h>
DHT dht(2, DHT21);  //датчик темп. и влаги подключен в порт D2
//порты управления реле:
#define VENT 7
#define HEAT 4
#define LIGHT 6
#define PUMP 5
//порты датчиков:
#define LIGHT_SENSOR A0
#define HUMIDITY_SENSOR A1
//задать необходимые поддерживаемые характеристики
#define REQUIRED_HUMIDITY 50


void setup() {
  pinMode(HEAT, OUTPUT);
  pinMode(VENT, OUTPUT);
  pinMode(LIGHT, OUTPUT);
  pinMode(PUMP, OUTPUT);
  Serial.begin(9600);
  delay(2000);
}

enum Regime { day,
              night
            };
enum Regime regime;
unsigned int TIME = 0;  //секунды с начала дня, 16*60*60 - начало ночи
//обозначаем глобальные переменные для показаний датчика темп. и влажности воздуха
int CURRENT_TEMPERATURE = 20;
int CURRENT_AIR_HUMIDITY = 50;

void dht_sensor() {
  dht.read();
  switch (dht.getState()) {
    case DHT_OK:
      CURRENT_TEMPERATURE = dht.getTemperatureC();
      CURRENT_AIR_HUMIDITY = dht.getHumidity();
      break;
    case DHT_ERROR_CHECKSUM:
      Serial.println("Checksum error");
      break;
    case DHT_ERROR_TIMEOUT:
      Serial.println("Time out error");
      break;
    case DHT_ERROR_NO_REPLY:
      Serial.println("Sensor not connected");
      break;
  }
}

void light() {
  if (regime == day) {
    if (analogRead(LIGHT_SENSOR) < 100) digitalWrite(LIGHT, HIGH);
    else digitalWrite(LIGHT, LOW);
  } else digitalWrite(LIGHT, LOW);
}

void vent() {
  boolean vent_on = LOW;                       // переменная тригер включения вентилятора, для цикла for ниже
  for (int i = 2; i <= 12; i = i + 2) {  // проветривать по расписанию, каждые 2 часа, цикл for реализует перебор времен, в которые может быть проветривание
    if (((i * 60 * 60) - (15 * 60)) < TIME < (i * 60 * 60)) {
      vent_on = HIGH;
    }
  }
  digitalWrite(VENT, vent_on);
}

void heat() {
  dht.read();// включать обогрев и вентилятор если холодно по датчику температуры воздуха, включать только вентилятор, если жарко
  if (CURRENT_TEMPERATURE < 25) {
    digitalWrite(VENT, 1);
    digitalWrite(HEAT, 1);
  }
  if (CURRENT_TEMPERATURE > 35) {
    digitalWrite(VENT, 1);
    digitalWrite(HEAT, 0);
  } else {
    digitalWrite(VENT, 0);
    digitalWrite(HEAT, 0);
  }
}

enum Regime_pump {
  irrigation,
  hold,
  humidity_ok
};
Regime_pump regime_pump = humidity_ok;



void pump() {
  static int dead_time = 60;
  static int watering_time = 15; 
  if (regime_pump == humidity_ok) {
    int dead_time = 60;
    int watering_time = 15;
    if (analogRead(HUMIDITY_SENSOR) < REQUIRED_HUMIDITY) {
      regime_pump = irrigation;
    }
  }
  if (regime_pump == irrigation) {
    if (watering_time > 0) {
      digitalWrite(PUMP, HIGH);
      watering_time--;
    } else {
      digitalWrite(PUMP, LOW);
      regime_pump = hold;
      watering_time = 15;
    }
  }
  if (regime_pump == hold) {
    if (dead_time > 0) {
      dead_time -= 1;
    } else {
      dead_time = 60;
      regime_pump == humidity_ok;
    }
  }
}


void regime_day() {
  light();
  vent();
  heat();
  pump();
}

void regime_night() {
  light();
  vent();
  heat();
  pump();
}


void loop() {
  delay(1000);
  TIME++;
  if (TIME >= 24 * 60 * 60) TIME = 0;
  regime = TIME < 16 * 60 * 60 ? day : night;
  if (regime == day) regime_day();
  if (regime == night) regime_night();
}
