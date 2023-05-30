#include <DHT.h>
#include <DHT_U.h>
#define LIGHT_SENS A0     // датчик света
#define WET_SENS A1       // датчик влажности почвы
#define TEMP_SENS 2       // датчик температуры
#define FORGE 4           // печь?
#define PUMP 5            // помпа
#define UV 6              // лампы
#define FAN 7             // вентилятор
#define DHTPIN TEMP_SENS  // what digital pin we're connected to(Вывод к которому подключен датчик, в данном случае  пин D2)
#define DHTTYPE DHT22     // DHT 22 (AM2302), AM2321

int TEMP_OPTIM = 21;                                                 // оптимальная температура теплицы
int TEMP_DELTA = 5;                                                  // разброс температуры теплицы
unsigned long TEMP_SENS_DELAY = 5;                                   // время между срабатываниями датчика температуры (не меньше 5 сек)
float VENTILATION_START = 19;                                        //время начала проветривания
float VENTILATION_END = 19.25;                                       // время окончания проветривания
float START_TIME = 12.5;                                             // время запуска теплицы
float LIGHT_HOURS_PER_DAY = 12;                                      // продолжительность светового дня
float LIGHT_SUNRISE_TIME = 8;                                        // время начала светового дня
float LIGHT_SUNSET_TIME = LIGHT_SUNRISE_TIME + LIGHT_HOURS_PER_DAY;  // время окончания светового дня
int LIGHT_LIMIT = 500;                                               // оптимальный уровень освещённости
int LIGHT_ARRAY[200];                                                // массив значений показаний датчика освещённости
int WET_OPTIM = 1000;                                                // оптимальная влажность почвы

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(LIGHT_SENS, INPUT);
  pinMode(WET_SENS, INPUT);
  pinMode(TEMP_SENS, INPUT);
  pinMode(PUMP, OUTPUT);
  pinMode(UV, OUTPUT);
  pinMode(FAN, OUTPUT);
  digitalWrite(FAN, LOW);
  digitalWrite(FORGE, LOW);
  for (int i = 0; i < 200; i++) {
    LIGHT_ARRAY[i] = LIGHT_LIMIT;
  };
}

int temp_sens_check() {
  if (millis() - TEMP_SENS_DELAY > 5000) {
    TEMP_SENS_DELAY = millis();
    float temp = dht.readTemperature();
    return temp;
  };
}


int light_sens_check() {
  int j;
  if (j < 200) {
    LIGHT_ARRAY[j] = analogRead(LIGHT_SENS);
    j++;
  } else {
    LIGHT_ARRAY[0] = analogRead(LIGHT_SENS);
    j = 1;
  };
  int sum;
  for (int k = 0; k < 200; k++)
    sum += LIGHT_ARRAY[k];
  return sum / 200;
}


void heater_off(), heat(), cooling(), ventilation(), pump_on(), pump_off(), light_on(), light_off();  // состояния


void (*statefunc_heater)() = heater_off;  // State pointer
void (*statefunc_light)() = light_off;
void (*statefunc_pump)() = pump_off;


float time_of_day() {
  uint32_t sec = millis() / 1000ul;                 // полное количество секунд
  float timeHours = (START_TIME + (sec / 3600ul));  // часы
  if (timeHours > 24)
    timeHours -= 24;
  int timeMins = (sec % 3600ul) / 60ul;  // минуты
  float result = timeHours + timeMins / 60;
  return (result);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void heater_off() {
  digitalWrite(FORGE, LOW);
  digitalWrite(FAN, LOW);
  if (temp_sens_check() < (TEMP_OPTIM - TEMP_DELTA)) {
    statefunc_heater = heat;
  };
  if (temp_sens_check() > (TEMP_OPTIM + TEMP_DELTA)) {
    statefunc_heater = cooling;
  };
  if ((time_of_day() > VENTILATION_START) && (time_of_day() < VENTILATION_END)) {
    statefunc_heater = ventilation;
  };
};


void heat() {
  digitalWrite(FORGE, HIGH);
  digitalWrite(FAN, HIGH);
  if (temp_sens_check() > TEMP_OPTIM) {
    statefunc_heater = heater_off;
  };
};


void cooling() {
  digitalWrite(FORGE, LOW);
  digitalWrite(FAN, HIGH);
  if (temp_sens_check() < TEMP_OPTIM) {
    statefunc_heater = heater_off;
  };
};


void ventilation() {
  digitalWrite(FORGE, LOW);
  digitalWrite(FAN, HIGH);
  if ((time_of_day() < VENTILATION_START) || (time_of_day() > VENTILATION_END)) {
    statefunc_heater = heater_off;
  };
};


void light_off() {
  digitalWrite(UV, LOW);
  if ((time_of_day() > LIGHT_SUNRISE_TIME) && (time_of_day() < LIGHT_SUNSET_TIME))
    if (light_sens_check() < LIGHT_LIMIT) {
      statefunc_light = light_on;
    };
};


void light_on() {
  digitalWrite(UV, HIGH);
  if ((time_of_day() < LIGHT_SUNRISE_TIME) || (time_of_day() > LIGHT_SUNSET_TIME) || (light_sens_check() > LIGHT_LIMIT)) {
    statefunc_light = light_off;
  };
};


void pump_off() {
  digitalWrite(PUMP, LOW);
  if (analogRead(WET_SENS) < (WET_OPTIM)) {
    statefunc_pump = pump_on;
  }
}

void pump_on() {
  const long pump_cycle = 15 * 60 * 1000;
  int time_work = millis() % (pump_cycle);
  if (time_work < 5 * 1000) {
    digitalWrite(PUMP, HIGH);
  } else {
    digitalWrite(PUMP, LOW);
  }
  if (analogRead(WET_SENS) > (WET_OPTIM)) {
    statefunc_pump = pump_off;
  }
}

void loop() {
  (*statefunc_heater)();
  (*statefunc_light)();
  (*statefunc_pump)();
  delay(20);
}
