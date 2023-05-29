#define Dr_F 5 // Drive Forward  Выдвижение привода
#define Dr_R 6 // Drive Reverse Задвижение привода
#define Light 7 // Освещение
#define Pump 9 // Насос полива
#define Light_S 14 // Light Sensor Датчик света
#define Day 600 // Значение датчика при высокой освещенности 
#define Temp 600 // Temperature Значение датчика необходимой температуры 
#define Temp_S 15 // Temperature Sensor Датчик температуры 
uint32_t myTimer1, myTimer2;
bool Drive;
void setup() {
  pinMode(Pump, OUTPUT);
  pinMode(Dr_F, OUTPUT);
  pinMode(Dr_R, OUTPUT);
  pinMode(Light_S, INPUT);
  pinMode(Temp_S, INPUT);
  }
void loop() {
  if (millis() - myTimer1 >= 28800000) {   // Значение таймера на 8 часов для ожидания
    myTimer1 = millis();              // Сброс таймера
    Watering();  // Вызов полива
  }
  if (analogRead(Light_S) < Day) {   // Опрос датчика света
    digitalWrite(Light, true); // Включить освещение
  }
    else {
      digitalWrite(Light, false); // Выключить освещение
    }
  if  (analogRead(Temp_S) > Temp) {   // Опрос датчика температуры
    Temper(); 
    Drive = 1; 
  }
    else {
      Temper(); 
      Drive = 0;
    }
}
void Watering() {            // Полив
  myTimer2 = myTimer1;
  digitalWrite(Pump, true); // Включить полив
  if (millis() - myTimer2 >= 10800000) {   // Значение таймера на 3 часа для полива
    myTimer1 = millis();              // Сброс таймера
    digitalWrite(Pump, false); // Выключить полив
  }
  
}

void Temper() {        // Дверца
  bool Position;
  if (Drive == 1 && Position == 0) {
    digitalWrite(Dr_F, true);
    digitalWrite(Dr_R, false);
    Position = 1;
  }
  if (Drive == 0 && Position == 1) {
    digitalWrite(Dr_F, false);
    digitalWrite(Dr_R, true);
    Position = 0;
  }
}
