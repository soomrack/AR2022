#include <DHT.h>
#include <APDS9930.h>

#define TIME_NIGHT = 86400          // ночь = [16*60*60, 24*60*60]
#define TIME_DAY = 57600            // день = [0,16*60*60]
#define F_TIME_COOLER = 7200        // время автономного включения куллера 
#define S_TIME_COOLER = 21600       // время автономного включения куллера 
#define INTERVAL_TIME_COOLER = 3600 // интервал в котором включается куллер

#define DHTPIN             2        // контакт подклчения датчика температуры
#define RELAY              4        // контакт реле
#define WATER_PUMP         5        // помпа
#define LIGHT_DIODE        6        // лента
#define FAN                7        // вентилятор
#define HEATER             8        // обогреватель

#define DHTTYPE DHT21               // DHT 21  (AM2301)

int TIME_SECONDS = 0;               // день = [0,16*60*60] ночь = [16*60*60, 24*60*60] секунд
int STATE_DAY = 0;

// объявляем объект с параметрами
DHT DHT_F(DHTPIN, DHTTYPE);
DHT DHT_S(DHTPIN, DHTTYPE);
APDS9930 apds;

struct Status {
  bool status_vent;
  bool status_air_humidity;
  bool status_dirt_humidity;
  bool status_heat;
  bool status_light;
} STATUS = {false, false, false, false, false};

struct Climate {
  int min_dirt_humidity;
  int max_dirt_humidity;
  int min_air_humidity;
  int max_air_humidity;
  int min_air_temp;
  int max_air_temp;
  int min_light;
  int max_light;
} CLIMATE = {30, 50, 40, 50, 15, 24, 5000, 12000};

enum TimeOfDay {
  DAY,
  NIGHT
};

// полив
void irrigate() {
  if (STATUS.status_dirt_humidity && STATUS.status_air_humidity) {
    digitalWrite(WATER_PUMP, HIGH);
  } else {
    digitalWrite(WATER_PUMP, LOW);
  }
}

// обогреватель
void heater() {
  if (STATUS.status_heat) {
    digitalWrite(HEATER, HIGH);
  } else {
    digitalWrite(HEATER, LOW);
  }
}

// обдув
void fan() {
  if( STATUS.status_vent
        && STATUS.status_air_humidity
        && STATUS.status_heat ) {
        digitalWrite(FAN, HIGH);
    } else {
        digitalWrite(FAN, LOW);
    }
}

// освещение
void light() {
  if (STATUS.status_light) {
    digitalWrite(LIGHT_DIODE, HIGH);
  } else {
    digitalWrite(LIGHT_DIODE, LOW);
  }
}

// реализация таймера
void timer() {
  TIME_SECONDS++;
  STATE_DAY = TIME_SECONDS > TIME_DAY ? 1 : 0;
  if (TIME_SECONDS >= TIME_NIGHT) TIME_SECONDS = 0;
  delay(1000);
}

// влючение вентиляции два раза в день на 1 час
void periodic_ventilation() {
  if (F_TIME_COOLER < TIME_SECONDS 
        && TIME_SECONDS < (F_TIME_COOLER + INTERVAL_TIME_COOLER)) { STATUS.status_vent = true; return;}
  if (S_TIME_COOLER < TIME_SECONDS 
        && TIME_SECONDS < (S_TIME_COOLER + INTERVAL_TIME_COOLER)) { STATUS.status_vent = true; return;}
  STATUS.status_vent = false;
}

void control_temperature() {
  // считываем температуру в цельсиях
  float t = DHT_F.readTemperature();
  if (t < CLIMATE.min_air_temp) {
    STATUS.status_heat = true; 
    STATUS.status_vent = false; 
    return;
  }
  if (t > CLIMATE.max_air_temp) {
    STATUS.status_vent = true; 
    STATUS.status_heat = false; 
    return;
  }
}

void control_dirt_humidity() {
  // считываем влажность
  float h = DHT_S.readHumidity(); 
  if (h < CLIMATE.min_dirt_humidity) {
    STATUS.status_dirt_humidity = true; 
    STATUS.status_heat = false;
    return;
  }
  if (h > CLIMATE.max_dirt_humidity) {
    STATUS.status_dirt_humidity = false; 
    STATUS.status_heat = true;
    return;
  }
}

void control_air_humidity() {
  // считываем влажность
  float h = DHT_F.readHumidity();
  if (h < CLIMATE.min_air_humidity) {
    STATUS.status_air_humudity = true;
    STATUS.status_vent = false;
    return;
  }
  if (h > CLIMATE.max_air_humidity) {
    STATUS.status_air_humudity = false;
    STATUS.status_vent = true;
    return;
  }
}

void control_light() {
  // считываем освещение
  flaot l = apds.readAmbientLightLux();
  STATUS.status_light = (STATE_DAY == TimeOfDay::DAY 
                          && l < CLIMATE.min_light) ? true : false;
}

void setup() {
  Serial.begin(9600);
  dht.begin();                     // запускаем датчик
  apds.init();                     // Инициируем работу датчика
  apds.enableLightSensor(false);   // Разрешаем режим определения освещённости:
}

void loop() {
  timer();

  periodic_ventilation();
  control_temperature();
  control_dirt_humidity();
  control_air_humidity();
  control_light();

  irrigate();
  fan();
  heater();
  light();
}
