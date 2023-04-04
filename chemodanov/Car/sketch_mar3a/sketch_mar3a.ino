#include <Servo.h>

Servo servo;
Servo servo1;

int analogPin0 = A0;//right
int val0 = 0;
int analogPin1 = A1;//left
int val1 = 0;

int regular_speed = 500;
int angle_speed = 1000;
int max_speed = 1000;
int difference = 990;

bool first = true;

int timer_speed = 100;

bool stop = false;

char diraction;

void difference_angle(int &difference){
  if (analogPin0 < 500 && analogPin1 < 500){
    return difference = 1 * abs(analogPin0 - analogPin1);
  }else  return difference = 1 * abs(analogPin0 - analogPin1);
}

void regular (int &speed){
  if ((speed < max_speed)){
    speed += 10;
  return speed;
  } else {
    speed = max_speed;
    return speed;
  }
}

void PWM(int a, int b, int c, int d, int speed, char turn, bool drift){
  for (int i = 0; i < 1000; i++){
    if (i < speed){
      if (turn == 'b'){
        digitalWrite(a, HIGH);
        digitalWrite(b, HIGH);
        digitalWrite(c, HIGH);
        digitalWrite(d, HIGH);
      }
      if (turn == 'f'){
        digitalWrite(a, HIGH);
        digitalWrite(b, LOW);
        digitalWrite(c, HIGH);
        digitalWrite(d, LOW);
      }
      if (turn == 'r'){
        digitalWrite(a, HIGH);
        digitalWrite(b, HIGH);
        if (drift){
        digitalWrite(c, HIGH);
        digitalWrite(d, LOW);
        } else {
        digitalWrite(c, LOW);
        digitalWrite(d, LOW);
        }
      }
      if (turn == 'l'){
        if (drift){
        digitalWrite(a, HIGH);
        digitalWrite(b, LOW);
        } else {
        digitalWrite(a, LOW);
        digitalWrite(b, LOW);          
        }
        digitalWrite(c, HIGH);
        digitalWrite(d, HIGH);
      }
    } else {
      digitalWrite(a, LOW);
      digitalWrite(b, LOW);
      digitalWrite(c, LOW);
      digitalWrite(d, LOW);
    }
  }
}

void _PWM (int speed, char turn, int difference){
  for (int i = 0; i < 1000; i++){
    if (i < speed){
      if (turn == 'l'){
        digitalWrite(6, HIGH);
        digitalWrite(7, LOW);
        if (i < speed - difference){
          digitalWrite(5, HIGH);
          digitalWrite(4, LOW);
        } else {
          
          digitalWrite(5, LOW);
          digitalWrite(4, LOW);   

        }
      }
      if (turn == 'r'){
        digitalWrite(5, HIGH);
        digitalWrite(4, LOW);
        if (i < speed - difference){
          digitalWrite(6, HIGH);
          digitalWrite(7, LOW);
        } else {
          
          digitalWrite(6, LOW);
          digitalWrite(7, LOW);          
        }
      }
    } else {
      digitalWrite(6, LOW);
      digitalWrite(7, LOW);
      digitalWrite(5, LOW);
      digitalWrite(4, LOW);      
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(5, INPUT);
  pinMode(4, INPUT);
  pinMode(11, INPUT);
}

void loop() {
  //Serial.print(stop);
  bool doubleClick;
  bool button = digitalRead(11);
  stop = true;
  
  //Serial.print(" A0 = ");
  
  Serial.print(analogRead(0));//left
  Serial.print(',');
  Serial.print(analogRead(1));//right
  Serial.print(',');
  Serial.print(timer_speed);
  Serial.print(',');
  Serial.print(regular_speed);
  Serial.print(',');
  Serial.print(angle_speed);
  Serial.println();

  difference_angle(difference);

  if (analogRead(0) > 700 && analogRead(1) > 700){
    regular(regular_speed);
    PWM(6,7,5,4,regular_speed,'f', false);
    diraction = 'f';
    first = true;    
  }
  // if (analogRead(0) > 700 && analogRead(1) < 700){
  //    PWM(6,7,5,4,400,'l', true); 
  // }
  // if (analogRead(0) < 700 && analogRead(1) > 700){
  //   PWM(6,7,5,4,400,'r', true);
  // }
  // if (analogRead(0) < 700 && analogRead(1) < 700){
  //   PWM(6,7,5,4,400,'b', false);
  // }
  if (analogRead(0) > 700 && analogRead(1) < 700) {
    _PWM(angle_speed,'l',difference);
    diraction = 'l';
    regular_speed = 500;
    first = true;   
  }
  if (analogRead(0) < 700 && analogRead(1) > 700) {
    _PWM(angle_speed,'r',difference);
    diraction = 'r';
    regular_speed = 500;
    first = true;   
  }
  if (analogRead(0) < 700 && analogRead(1) < 700){
    // if (first){
    //   for (int i = 0; i < 10; i++){
    //     first = false;
    //     PWM(6,7,5,4,300,'b', true);
    //   }
    //   } else {
        if (diraction == 'r'){
          _PWM(angle_speed,'r',difference);
          regular_speed = 500;          
        } else if (diraction == 'l'){
          _PWM(angle_speed,'l',difference);
          regular_speed = 500;
        } else if (diraction == 'f'){
          PWM(6,7,5,4,400,'b', false);
          regular_speed = 500;
        }
    //}
  }
}
