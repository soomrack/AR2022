#include <DHT.h>
#include <APDS9930.h>

#define TIME_NIGHT 86400          // ночь = [16*60*60, 24*60*60]
#define TIME_DAY 57600            // день = [0,16*60*60]


#define DHTPIN             2        // датчик температуры
#define RELAY              4        // контакт реле
#define WATER_PUMP         5        // помпа
#define LIGHT_DIODE        6        // лента
#define VENT                7        // вентилятор
#define HEATER             8        // обогреватель

#define DHT_TYPE DHT21               // DHT 21  (AM2301)

int TIME_SECONDS = 0;               // день = [0,16*60*60] ночь = [16*60*60, 24*60*60] секунд
int DAY_STATE = 0;

// объявляем объект с параметрами
DHT DHT_F(DHTPIN, DHT_TYPE);
DHT DHT_S(DHTPIN, DHT_TYPE);
APDS9930 apds;

struct Status {
  bool status_vent;
  bool status_air_hum;
  bool status_dirt_hum;
  bool status_heat;
  bool status_light;
};
// Выставляем флаги
struct Status STAT = {false, false, false, false, false};

struct Clim {
  int min_dirt_hum;
  int max_dirt_hum;
  int min_air_hum;
  int max_air_hum;
  int min_air_temp;
  int max_air_temp;
  int min_light;
  int max_light;
};

// Задаем параметры климата
struct Clim CLIM = {30, 50, 40, 50, 15, 24, 5000, 12000};

enum DayTime {
  DAY,
  NIGHT
};

// реализация таймера
void timer() {
  TIME_SECONDS++;
  /* День/ночь */
  DAY_STATE = TIME_SECONDS > TIME_DAY ? 1 : 0;
  /* Обновление дня */
  if (TIME_SECONDS >= TIME_NIGHT) TIME_SECONDS = 0;
  delay(1000);
}

// полив
void watering() {
  if (STAT.status_dirt_hum && STAT.status_air_hum) {
    digitalWrite(WATER_PUMP, HIGH);
  } else {
    digitalWrite(WATER_PUMP, LOW);
  }
}

// обогреватель
void heater() {
  if (STAT.status_heat) {
    digitalWrite(HEATER, HIGH);
  } else {
    digitalWrite(HEATER, LOW);
  }
}

// обдув
void blowing() {
  if( (STAT.status_vent
    && STAT.status_air_hum) || (STAT.status_vent 
    && STAT.status_heat )) {
    digitalWrite(VENT, HIGH);
  } else {
    digitalWrite(VENT, LOW);
  }
}

// освещение
void light() {
  if (STAT.status_light) {
    digitalWrite(LIGHT_DIODE, HIGH);
  } else {
    digitalWrite(LIGHT_DIODE, LOW);
  }
}

void temperature_control() {
  // считываем температуру в цельсиях
  float t = DHT_F.readTemperature();
  if (t < CLIM.min_air_temp) {
    STAT.status_heat = true; //нагреваем
    STAT.status_vent = false; 
  }
  else if (t > CLIM.max_air_temp) {
    STAT.status_vent = true; //проветриваем
    STAT.status_heat = false; 
  }
}

void dirt_humidity_control() {
  // считываем влажность
  float h = DHT_S.readHumidity(); 
  if (h < CLIM.min_dirt_hum) {
    STAT.status_dirt_hum = true;  // орошаем почву
    STAT.status_heat = false; //отключаем нагрев
  }
  else if (h > CLIM.max_dirt_hum) {
    STAT.status_dirt_hum = false; //не орошаем почву
    STAT.status_heat = true; //включаем нагрев
  }
}

void air_humidity_control() {
  // считываем влажность
  float h = DHT_F.readHumidity();
  if (h < CLIM.min_air_hum) {
    STAT.status_air_hum = true; // Воздух сухой
    STAT.status_vent = false; // Вентилятор закрыт
  }
  else if (h > CLIM.max_air_hum) {
    STAT.status_air_hum = false; // Воздух влажный
    STAT.status_vent = true; // Вентилятор открыт
  }
}

void light_control() {
  // считываем освещение
  float lv; 
  apds.readAmbientLightLux(lv);
  STAT.status_light = (DAY_STATE == DayTime::DAY 
                          && lv < CLIM.min_light) ? true : false; // определение времени суток
}

void setup() {
  Serial.begin(9600);
  DHT_S.begin();                     // запускаем датчик
  apds.init();                     // Инициируем работу датчика
  apds.enableLightSensor(false);   // Разрешаем режим определения освещённости:
}

void loop() {
  timer();
  temperature_control();
  dirt_humidity_control();
  air_humidity_control();
  light_control();

  light();
  heater();
  blowing();
  watering();
}
