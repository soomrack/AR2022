#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define M1_DIR 4
#define M1_PWM 5
#define M2_DIR 7
#define M2_PWM 6
#define BUTTON 12
#define BUZZER 10

#define SENSOR_LEFT A1
#define SENSOR_RIGHT A2
#define SENSOR_DATA 500

#define CHENGES_SPEED 180
#define MAX_SPEED 255

#define K1 0.65
#define K2 0.02

#define GRAY 850
#define DELAY_SWITCH 500
#define BUZZER_ESPIRAL 100
#define DATA_SPEED 9600
#define MAX_RANGE_RANDOM 50

int I = 0;
int prevEr = 0;
int err = 0;
int _state = 0;
int time = 0;

void SetupPinMode() {//функция объявления всех подключаемых компонентов 
  InitMotors();
  InitButton();
  InitBuzzer();
}

void InitMotors() {//объявление моторов
  pinMode(M1_DIR, OUTPUT);
  pinMode(M1_PWM, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
  pinMode(M2_PWM, OUTPUT);
}

void InitButton() {//объявление кнопок
  pinMode(BUTTON, INPUT_PULLUP);
}

void InitBuzzer() {//объявление зуммера
  pinMode(BUZZER, OUTPUT);
}

bool ButtonState(int pin) {//функция нажатия клавиши с обработкой состояний
  bool flag;
  bool btnState = !digitalRead(pin);
  if (btnState && !flag) flag = true;  // обработчик нажатия
  if (!btnState && flag) flag = false; // обработчик отпускания
  BuzzerVoice(DELAY_SWITCH);
  return flag;
}

void BuzzerVoice(int t) {//функция голоса пищалки
  digitalWrite(BUZZER, true);
  delay(t);
  digitalWrite(BUZZER, false);
  delay(t);
}

bool MoreNumber(float power) {//функция проверки максимального значения
  bool flag;                  //передаваемого на мотор для функции спирали
  if (power < 255) flag = false;
  else flag = true;
  return flag;
}

void Motors(int Speed1, int Speed2) {//функция работы моторов
  if(Speed1 > MAX_SPEED) Speed1 = MAX_SPEED;
  if(Speed1 < -MAX_SPEED) Speed1 = -MAX_SPEED;
  if(Speed2 > MAX_SPEED) Speed2 = MAX_SPEED;
  if(Speed2 < -MAX_SPEED) Speed2 = -MAX_SPEED;

  if(Speed1 > 0) {
    digitalWrite(M1_DIR, 1);
    analogWrite(M1_PWM, Speed1);
  }
  else {
    digitalWrite(M1_DIR, 0);
    analogWrite(M1_PWM, -Speed1);
  }

  if(Speed2 > 0) {
    digitalWrite(M2_DIR, 1);
    analogWrite(M2_PWM, Speed2);
  }
  else {
    digitalWrite(M2_DIR, 0);
    analogWrite(M2_PWM, -Speed2);
  }
}

void Stop() {//остановка моторов
  Motors(0, 0);
}

int LineSensorRead(int SensorPin) {//чтение данных с датчика по аналог
  if(SensorPin < A0 || SensorPin > A3)
    return -1;

  int Data = analogRead(SensorPin);
  return Data;
}

void Print(float mR, float mL, int sR, int sL) {
  Serial.print(mR);
  Serial.print(',');
  Serial.print(mL);
  Serial.print(',');
  Serial.print(LineSensorRead(SENSOR_RIGHT));
  Serial.print(',');
  Serial.print(LineSensorRead(SENSOR_LEFT));
  Serial.print(',');
  Serial.print(I);
  Serial.println();
}

void BotState() {//_state
  int sensor;
  float motor_right, motor_left;
  bool state_b = false;
  if ( ButtonState(BUTTON) && _state != 0) _state = 0;
  if ( ButtonState(BUTTON) && _state != 1) _state = 1;
  if ((_state == 1 || _state == 3) && LineSensorRead(SENSOR_LEFT) < SENSOR_DATA && LineSensorRead(SENSOR_RIGHT) < SENSOR_DATA) _state = 2;
  if ((_state == 1 || _state == 2)) _state = 3;
  switch (_state) {//кейс состояний
    case 0:
      Stop();
      //Serial.print(0);
      break;
    case 1:
      if (LineSensorRead(SENSOR_LEFT) < SENSOR_DATA && LineSensorRead(SENSOR_RIGHT) < SENSOR_DATA) _state = 2;
      if (LineSensorRead(SENSOR_LEFT) > SENSOR_DATA || LineSensorRead(SENSOR_RIGHT) > SENSOR_DATA) _state = 3;
      //Serial.print(1);
      break;
    case 2:
      MoveEspiral(_state, motor_right, motor_left, time);
      //Serial.print(2);
      break;
    case 3:
      MoveBot(sensor, motor_right, motor_left, _state);
      //Serial.print(3);
      break;
    default :
      break;
  }
}

void MoveEspiral(int& state, float& power_r, float& power_l, int& time) {
  float r = power_r;
  float l = power_l;
  if (r == 0 && l == 0) {
    r = rand() % MAX_RANGE_RANDOM;
    l = rand() % MAX_RANGE_RANDOM;
  }
  if (LineSensorRead(SENSOR_LEFT) > 500 || LineSensorRead(SENSOR_RIGHT) > 500) state = 3;
  if (MoreNumber(r)) r++;
  if (MoreNumber(l)) l++;
  if (MoreNumber(r) && MoreNumber(l)) time++;
  else if (MoreNumber(r) && MoreNumber(l) && time == 4) state = 0;
}

void MoveBot(int& sensor, float& motor_right, float& motor_left, int& state, bool state_b) {
  sensor = LineSensorRead(SENSOR_LEFT) - LineSensorRead(SENSOR_RIGHT);
  err = sensor - GRAY;//вычисляемая ошибка для I
  motor_right = CHENGES_SPEED - K1 * sensor - I;//расчет скорости для правой пары колес
  motor_left = CHENGES_SPEED + K1 * sensor + I;//расчет скорости для левой пары колес
  Motors(motor_right, motor_left);
  Integrator(I, err, prevEr);
  prevEr = err - I;//сохраняем ошибку - I
  if (ButtonState(BUTTON)) { 
    state = 0;
    state_b = true;
  }
  if (ButtonState(LineSensorRead(SENSOR_LEFT) < 500 && LineSensorRead(SENSOR_RIGHT))) state = 2;
  Print(motor_right,motor_left,LineSensorRead(SENSOR_RIGHT),LineSensorRead(SENSOR_LEFT));
}

void Integrator(int& I, int err, int prevEr) {//интегртор
  I += K2*(err - prevEr);
}

void setup() {
  SetupPinMode();
  Serial.begin(DATA_SPEED);
}

void loop() {
  BotState();
}
