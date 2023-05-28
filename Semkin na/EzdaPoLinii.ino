#include <avr/io.h>
#include <avr/interrupt.h>

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
#define LESS_WHITE 500

int I = 0;
int prevEr = 0;
int err = 0;
int _state = 0;
int time = 0;

void SetupPinMode() {
  pinMode(M1_DIR, OUTPUT);
  pinMode(M1_PWM, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
  pinMode(M2_PWM, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
}

bool ButtonState(int pin) {
  static bool flag = false;
  bool btnState = !digitalRead(pin);
  if (btnState && !flag) {
    flag = true;
    return true; // кнопка нажата
  } else if (!btnState && flag) {
    flag = false;
  }
  return false; // кнопка не нажата
}

void BuzzerVoice(int t) {
  digitalWrite(BUZZER, HIGH);
  delay(t);
  digitalWrite(BUZZER, LOW);
  delay(t);
}

bool MoreNumber(float power) {
  return power >= MAX_SPEED;
}

void Motors(int Speed1, int Speed2) {
  if (Speed1 > MAX_SPEED) Speed1 = MAX_SPEED;
  if (Speed1 < -MAX_SPEED) Speed1 = -MAX_SPEED;
  if (Speed2 > MAX_SPEED) Speed2 = MAX_SPEED;
  if (Speed2 < -MAX_SPEED) Speed2 = -MAX_SPEED;

  if (Speed1 > 0) {
    digitalWrite(M1_DIR, HIGH);
    analogWrite(M1_PWM, Speed1);
  } else {
    digitalWrite(M1_DIR, LOW);
    analogWrite(M1_PWM, -Speed1);
  }

  if (Speed2 > 0) {
    digitalWrite(M2_DIR, HIGH);
    analogWrite(M2_PWM, Speed2);
  } else {
    digitalWrite(M2_DIR, LOW);
    analogWrite(M2_PWM, -Speed2);
  }
}

void Stop() {
  Motors(0, 0);
}

int LineSensorRead(int SensorPin) {
  if (SensorPin < A0 || SensorPin > A3)
    return -1;

  return analogRead(SensorPin);
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

void BotState() {
  int sensor;
  float motor_right, motor_left;
  bool state_b = false;

  if (ButtonState(BUTTON) && _state != 0) {
    _state = 0;
    BuzzerVoice(DELAY_SWITCH);
  }
  if (ButtonState(BUTTON) && _state != 1) {
    _state = 1;
    BuzzerVoice(DELAY_SWITCH);
  }
  if ((_state == 1 || _state == 3) && LineSensorRead(SENSOR_LEFT) < SENSOR_DATA && LineSensorRead(SENSOR_RIGHT) < SENSOR_DATA) _state = 2;
  if ((_state == 1 || _state == 2)) _state = 3;

  switch (_state) {
    case 0:
      Stop();
      Serial.print(0);
      Serial.println();
      break;
    case 1:
      if (LineSensorRead(SENSOR_LEFT) < SENSOR_DATA + 100 && LineSensorRead(SENSOR_RIGHT) < SENSOR_DATA + 100) _state = 2;
      if (LineSensorRead(SENSOR_LEFT) > SENSOR_DATA + 100 || LineSensorRead(SENSOR_RIGHT) > SENSOR_DATA + 100) _state = 3;
      Serial.print(1);
      Serial.println();
      break;
    case 2:
      MoveEspiral(_state, motor_right, motor_left, time);
      Serial.print(2);
      Serial.println();
      break;
    case 3:
      if (LineSensorRead(SENSOR_LEFT) < SENSOR_DATA + 100 && LineSensorRead(SENSOR_RIGHT) < SENSOR_DATA + 100) {
        _state = 2;
        MoveEspiral(_state, motor_right, motor_left, time);
      } else {
        MoveBot(sensor, motor_right, motor_left, _state);
        Serial.print(LineSensorRead(SENSOR_LEFT));
        Serial.print(',');
        Serial.print(LineSensorRead(SENSOR_RIGHT));
        Serial.print(',');
        Serial.print(3);
        Serial.println();
      }
      break;
    default:
      break;
  }
}

float Inc(int LineL, int LineR) {
  if (LineL < LESS_WHITE && LineR < LESS_WHITE) {
    return 1;
  } else {
    return 0;
  }
}

void MoveEspiral(int& _state, float& power_r, float& power_l, int& time) {
  float r = power_r;
  float l = power_l;
  if (r == 0 && l == 0) {
    r = random(0, MAX_RANGE_RANDOM);
    l = random(0, MAX_RANGE_RANDOM);
  }
  if (LineSensorRead(SENSOR_LEFT) > LESS_WHITE || LineSensorRead(SENSOR_RIGHT) > LESS_WHITE) _state = 3;

  if (r > l) {
    l = MAX_SPEED;
    r -= 10;
    if (r < 0) r = 0;
  }
  if (l > r) {
    r = MAX_SPEED;
    l -= 10;
    if (l < 0) l = 0;
  }
  if (MoreNumber(r) && MoreNumber(l)) time++;
  else if (MoreNumber(r) && MoreNumber(l) && time == 4) _state = 0;
  Motors(r, l);
}

void MoveBot(int& sensor, float& motor_right, float& motor_left, int& _state) {
  sensor = LineSensorRead(SENSOR_LEFT) - LineSensorRead(SENSOR_RIGHT);
  err = sensor - GRAY;
  motor_right = CHENGES_SPEED - K1 * sensor - I;
  motor_left = CHENGES_SPEED + K1 * sensor + I;
  Motors(motor_right, motor_left);
  Integrator(I, err, prevEr);
  prevEr = err - I;

  if (LineSensorRead(SENSOR_LEFT) < LESS_WHITE && LineSensorRead(SENSOR_RIGHT) < LESS_WHITE) _state = 2;
}

void Integrator(int& I, int err, int prevEr) {
  I += K2 * (err - prevEr);
}

void setup() {
  SetupPinMode();
  Serial.begin(DATA_SPEED);
}

void loop() {
  BotState();
}
