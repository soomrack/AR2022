#define LF 5  //LeftFront
#define LB 4  //LeftBack
#define RF 6  //RightFront
#define RB 7  //RightBack
#define CS A0 //ColorSensor
#define Black 700 //Black Color 

void setup ()  {
  pinMode (LF, OUTPUT);
  pinMode (LB, OUTPUT);
  pinMode (RF, OUTPUT);
  pinMode (RB, OUTPUT);
  pinMode(CS, INPUT);
}

int PID(float input, float setpoint, float kp, float ki, float kd, float dt) { // Функция расчета управляющего сигнала
  float err = setpoint - input; // Расчет ошибки регулирования
  static float integral = 0, prevErr = 0;
  integral =+ err * dt; // Расчет интегральной составляющей
  float D = (err - prevErr) / dt; // Расчет дифференциальной составляющей
  prevErr = err;
  return (err * kp + integral * ki + D * kd); // Формирование управляющего сигнала
}

void left(int Speed) { // Функция управления левым мотором
  analogWrite(LF,Speed);
  digitalWrite(LB,0);
}

void right(int Speed) { // Функция управления правым мотором
  analogWrite(RF,Speed);
  digitalWrite(RB,0);
}

void loop()  {

  left(80-PID(analogRead(CS),Black, 0.17, 0.065, 0.015, 0.01)); // Speed = 80-PID(analogRead(CS),black, 0.17, 0.065, 0.015, 0.01)
  right(80+PID(analogRead(CS),Black, 0.17, 0.065, 0.015, 0.01));
  delay(10); // Период региулирования dt в миллисекундах
}
