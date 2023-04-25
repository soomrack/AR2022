#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define M1_DIR 4
#define M1_PWM 5
#define M2_DIR 7
#define M2_PWM 6
#define BUTTON 12
#define BUZZER 10

#define SENSOR_LEFT A2
#define SENSOR_RIGHT A1

#define CHENGES_SPEED 180
#define MAX_SPEED 255

#define K1 0.65
#define K2 0.02

#define GRAY 850
#define DELAY_SWITCH 500
#define BUZZER_ESPIRAL 100

int I = 0;
int prevEr = 0;
int err = 0;
int _state = 0;

void SetupPinMode() {
  InitMotors();
  InitButton();
  InitBuzzer();
  return;
}

void InitMotors() {
  pinMode(M1_DIR, OUTPUT);
  pinMode(M1_PWM, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
  pinMode(M2_PWM, OUTPUT);
  return;
}

void InitButton() {
  pinMode(BUTTON, INPUT_PULLUP);
  return;
}

void InitBuzzer() {
  pinMode(BUZZER, OUTPUT);
  return;
}

bool ButtonState(int pin) {
  bool btnState = !digitalRead(pin);
  if (btnState && !flag) flag = true;  // обработчик нажатия
  if (!btnState && flag) flag = false; // обработчик отпускания
  BuzzerVoice(DELAY_SWITCH);
  return flag;
}

void BuzzerVoice(int t) {
  digitalWrite(BUZZER, true);
  delay(t);
  digitalWrite(BUZZER, false);
  delay(t);
}

bool MoreNumber(float power) {
  bool flag;
  if (power < 255) flag = false;
  else flag = true;
  return flag;
}

void Motors(int Speed1, int Speed2) {
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
  return;
}

void Stop() {
  Motors(0, 0);
  return;
}

int LineSensorRead(int SensorPin) {
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
  return;
}

void BotState() {//_state
  int sensor;
  float motor_right, motor_left;
  int time;
  if (!ButtonState(BUTTON) && LineSensorRead(SENSOR_LEFT) < 500 && LineSensorRead(SENSOR_RIGHT) < 500) _state = 0;
  if ( ButtonState(BUTTON) && LineSensorRead(SENSOR_LEFT) < 500 && LineSensorRead(SENSOR_RIGHT) < 500) _state = 1;
  if ((_state == 1 || _state == 3) && LineSensorRead(SENSOR_LEFT) < 500 && LineSensorRead(SENSOR_RIGHT) < 500) _state = 2;
  if ((_state == 1 || _state == 2)) _state = 3;
  switch (_state) {
    case 0:
      Stop();
      break;
    case 1:
      if (LineSensorRead(SENSOR_LEFT) < 500 && LineSensorRead(SENSOR_RIGHT) < 500) _state = 2;
      if (LineSensorRead(SENSOR_LEFT) > 500 || LineSensorRead(SENSOR_RIGHT) > 500) _state = 3;
      break;
    case 2:
      MoveEspiral(_state);
      BuzzerVoice(BUZZER_ESPIRAL);
      break;
    case 3:
      MoveBot(sensor, motor_right, motor_left);
      break;
    default
      break;
  }
  return;
}

void MoveEspiral(int& state, float& power_r, float& power_l, int& time) {
  float r = power_r;
  float l = power_l;
  if (r == 0 && l == 0) {
    r = rand() % 50;
    l = rand() % 50;
  }
  if (LineSensorRead(SENSOR_LEFT) > 500 || LineSensorRead(SENSOR_RIGHT) > 500) state = 3;
  if (MoreNumber(r)) r++;
  if (MoreNumber(l)) l++;
  if (MoreNumber(r) && MoreNumber(l)) time++;
  else if (MoreNumber(r) && MoreNumber(l) && time == 4) state = 0;
  return;
}

void MoveBot(int& sensor, float& motor_right, float& motor_left) {
  sensor = LineSensorRead(SENSOR_LEFT) - LineSensorRead(SENSOR_RIGHT);
  err = sensor - GRAY; 
  motor_right = 180 - K1 * sensor - I;// + LineSensorRead(A2))/2
  motor_left = 180 + K1 * sensor + I;
  Motors(motor_right, motor_left);
  I = integrator(I, sensor);
  prevEr = err - I;
  Print(motor_right,motor_left,LineSensorRead(SENSOR_RIGHT),LineSensorRead(SENSOR_LEFT));
  return;
}

int integrator(int I, int input) {
  I += K2*(err - prevEr);
  return I;
}

void setup() {
  SetupPinMode();
  Serial.begin(9600);
}

void loop() {
  BotState();
}
