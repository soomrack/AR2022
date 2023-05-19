#include <TroykaDHT.h>
DHT dht(2, DHT21); //датчик темп. и влаги подключен в порт D2
//порты управления реле:
#define VENT 7
#define HEAT 4
#define LIGHT 6
#define PUMP 5
//порты датчиков:
#define LIGHT_SENSOR A0
#define HUMIDITY_SENSOR A1
//задаем требуемые характеристики
#define REQUIRED_HUMIDITY 50 //влажность
#define REQUIRED_LIGHT 100 //свет
#define LOW_TEMPERATURE 18 //низкая температура
#define HIGH_TEMPERATURE 24 //высокая температура

void setup() {
pinMode(HEAT, OUTPUT);
pinMode(VENT, OUTPUT);
pinMode(LIGHT, OUTPUT);
pinMode(PUMP, OUTPUT);
Serial.begin(9600);
}

enum Regime { day,
night
};
enum Regime regime;
unsigned int TIME = 0; //секунды с начала дня(если начинаем в 8 утра), 16*60*60 - начало ночи
int CURRENT_TEMPERATURE;
int CURRENT_AIR_HUMIDITY;

void dht_sensor() { // проверяем работает ли датчик температуры и влажности
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

void light() { //включаем свет если темно днем
if (regime == day) {
if (analogRead(LIGHT_SENSOR) < REQUIRED_LIGHT) digitalWrite(LIGHT, HIGH);
else digitalWrite(LIGHT, LOW);
} else digitalWrite(LIGHT, LOW);//ночью не включаем
}


enum Regime_heat {
heat,
no_heat,
temperature_ok
};
Regime_heat regime_heat = temperature_ok;

void heating() { //нагреваем по-немногу
int heat_time = 10*60; //время нагрева
int wait_time = 10*60; //время остановки для прогрева всей теплицы, а также нагрева при остывании. если после ожидания температура все еще низкая, нагрев начнется снова
dht.read();
if (regime_heat = temperature_ok){
if (CURRENT_TEMPERATURE < LOW_TEMPERATURE) {
regime_heat = heat;
}}
if (regime_heat == heat) {
if (heat_time > 0) {
digitalWrite(VENT, 1); //нагрев
digitalWrite(HEAT, 1);
heat_time--;
} else {
digitalWrite(VENT, 1);//ожидание без нагрева, тепло разносится вентилятором
digitalWrite(HEAT, 0);
regime_heat = no_heat;
heat_time = 10*60;
}
}
if (regime_heat == no_heat) {
if (wait_time > 0) {
wait_time --;
} else {
wait_time = 10*60;
regime_heat == temperature_ok;

}
}
}

void cooling() {  //охлаждение, если температура выше 24
if (CURRENT_TEMPERATURE > HIGH_TEMPERATURE){
  digitalWrite(VENT, 1);}
  else {digitalWrite(VENT, 0);} //если температура в пределах нормы - не охлаждаем
}



enum Regime_pump {
irrigation,
no_irrigation,
humidity_ok
};
Regime_pump regime_pump = humidity_ok;



void watering() {//полив по-немногу
int watering_time = 15; //включаем на 15 сек
int dead_time = 60; // ждем 60 сек
if (regime_pump == humidity_ok) {
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
regime_pump = no_irrigation;
watering_time = 15;
}
}
if (regime_pump == no_irrigation) {
if (dead_time > 0) {
dead_time --;
} else {
dead_time = 60;
regime_pump == humidity_ok; //снова переходим в режим проверки влажности почвы
}
}
}


void work() {
light();
heating();
cooling();
watering();
}


void loop() {
delay(1000);
TIME++;
if (TIME >= 24*60*60) TIME = 0;
if (TIME < 16*60*60) {regime == day;}
else regime == night;
work();
}
