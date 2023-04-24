#include <avr/io.h>
#include <avr/interrupt.h>

#define M1_DIR 4
#define M1_PWM 5
#define M2_DIR 7
#define M2_PWM 6
#define BUTTON12 12

#define SENSOR_LEFT A2
#define SENSOR_RIGHT A1

#define CHENGES_SPEED 180
#define MAX_SPEED 255

#define K1 0.65
#define K2 0.02

#define gray 850

int I = 0;
int prevEr = 0;
int err = 0;

void SetupPinMode() {
  InitMotors();
  InitButton();
}

void InitMotors() {
  pinMode(M1_DIR, OUTPUT);
  pinMode(M1_PWM, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
  pinMode(M2_PWM, OUTPUT);
}

void InitButton() {
  pinMode(BUTTON12, INPUT_PULLUP);
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
}

void Stop() {
  Motors(0, 0);
}

int integrator(int I, int input) {
  I += K2*(err - prevEr);
  return I;
}

bool ButtonState(int pin) {
  bool btnState = !digitalRead(BUTTON12);
  if (btnState && !flag) flag = true;  // обработчик нажатия
  if (!btnState && flag) flag = false; // обработчик отпускания
  return flag;
}

void setup() {
  SetupPinMode();
  Serial.begin(9600);
}

void loop() {
  int sensor;
  float motor_right, motor_left;
  while(1) {
    sensor = LineSensorRead(SENSOR_LEFT) - LineSensorRead(SENSOR_RIGHT);
    err = sensor - gray; 
    motor_right = 180 - K1 * sensor - I;// + LineSensorRead(A2))/2
    motor_left = 180 + K1 * sensor + I;
    Motors(motor_right, motor_left);
    I = integrator(I, sensor);
    prevEr = err - I;
    Print(motor_right,motor_left,LineSensorRead(SENSOR_RIGHT),LineSensorRead(SENSOR_LEFT));
  }
  Stop();
}
