#include <avr/io.h>
#include <avr/interrupt.h>

#define M1_DIR 4
#define M1_PWM 5
#define M2_DIR 7
#define M2_PWM 6

int gray = 850;
int time = 0;
float K1 = 0.65;
float a,b;
float K2 = 0.02;
int I = 0;
int prevEr = 0;
int err = 0;

int ButtonRead(int ButtonPin)
{
  int Data = digitalRead(ButtonPin);
  if(ButtonPin == 12) Data = !Data;
  return Data;
}

void InitMotors()
{
  pinMode(M1_DIR, OUTPUT);
  pinMode(M1_PWM, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
  pinMode(M2_PWM, OUTPUT);
}

void Motors(int Speed1, int Speed2)
{
  if(Speed1 > 255) Speed1 = 255;
  if(Speed1 < -255) Speed1 = -255;
  if(Speed2 > 255) Speed2 = 255;
  if(Speed2 < -255) Speed2 = -255;

  if(Speed1 > 0)
  {
    digitalWrite(M1_DIR, 1);
    analogWrite(M1_PWM, Speed1);
  }
  else
  {
    digitalWrite(M1_DIR, 0);
    analogWrite(M1_PWM, -Speed1);
  }

  if(Speed2 > 0)
  {
    digitalWrite(M2_DIR, 1);
    analogWrite(M2_PWM, Speed2);
  }
  else
  {
    digitalWrite(M2_DIR, 0);
    analogWrite(M2_PWM, -Speed2);
  }
}

int LineSensorRead(int SensorPin)
{
  if(SensorPin < A0 || SensorPin > A3)
    return -1;

  int Data = analogRead(SensorPin);
  return Data;
}

void Print()
{
  Serial.print(a);
  Serial.print(',');
  Serial.print(b);
  Serial.print(',');
  Serial.print(LineSensorRead(A1));
  Serial.print(',');
  Serial.print(LineSensorRead(A2));
  Serial.print(',');
  Serial.print(I);
  Serial.println();
}

void Stop()
{
  Motors(0, 0);
}

int integrator(int I, int input)
{
  I += K2*(err - prevEr);
  return I;
}

void setup()
{
  pinMode(12, INPUT_PULLUP);
  InitMotors();
  Serial.begin(9600);
}



void loop()
{
  int sensor;
  while(!ButtonRead(12)) 
  {
    sensor = LineSensorRead(A2) - LineSensorRead(A1);
    err = sensor - gray; 
    a = 180 - K1 * sensor - I;// + LineSensorRead(A2))/2
    b = 180 + K1 * sensor + I;
    Motors(a,b);
    I = integrator(I, sensor);
    prevEr = err - I;
    Print();
  }
  Stop();
  time++;
}
