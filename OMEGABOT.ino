
#define left1 5
#define left2 4
#define right1 6
#define right2 7
#define SR A0
#define avrgGrey 600 

void setup ()  {
  pinMode (left1, OUTPUT);
  pinMode (left2, OUTPUT);
  pinMode (right1, OUTPUT);
  pinMode (right2, OUTPUT);
  pinMode(SR, INPUT);
}

int PID(float input, float setpoint, float kp, float ki, float kd, float dt) { // функция расчета управляющего сигнала
  float err = setpoint - input; // расчет ошибки регулирования
  static float integral = 0, prevErr = 0;
  integral =+ err * dt; // расчет интегральной составляющей
  float D = (err - prevErr) / dt; // расчет дифференциальной составляющей
  prevErr = err;
  return (err * kp + integral * ki + D * kd); // формирование управляющего сигнала
}

void left(int Speed) { // функция управления левым мотором
  analogWrite(left1,Speed);
  digitalWrite(left2,0);
}

void right(int Speed) { // функция управления правым мотором
  analogWrite(right1,Speed);
  digitalWrite(right2,0);
}

void loop()  {

  left(80-PID(analogRead(SR),avrgGrey, 0.17, 0.065, 0.015, 0.01)); // Speed = 80-PID(analogRead(SR),avrgGrey, 0.17, 0.065, 0.015, 0.01)
  right(80+PID(analogRead(SR),avrgGrey, 0.17, 0.065, 0.015, 0.01));
  delay(10); // период региулирования dt в миллисекундах
}
