#define M1_DIR 4
#define M1_PWM 5  
#define M2_DIR 7
#define M2_PWM 6

#define BUTTOM 12

#define MAX_SPEED 255
#define CLOCK_LIM 120

/* Значения датчика цвета:
 * 280-320 - белый цвет
 * 700-850 - черный
 */

int speedR = 80, speedL = 80; /* Скорость робота */
int p = 0, i = 0, d = 0, U = 0;
int iMax = 20;
int sensorValueL, sensorValueR; /* Данные с датчиков */
float kp = 0.6, ki = 0.01, kd = 2;/* Коэффициент пидрегулятора */
float eOld = 0;
int error; /* ошибка */
bool isButton = false;
bool stateMove = false, stateSearch = false;
/* Движение по спирали */
long long int clock = 0;
long long int clockLimit = CLOCK_LIM;
int speedSearchL = MAX_SPEED, speedSearchR = 0;


/* Считывание данных с датчиков света */
int LightSensorRead(int SensorPin)
{
  if(SensorPin < A0 || SensorPin > A3)
    return -1;

  int Data = analogRead(SensorPin);
  return Data;
}

int ButtonSignalRead(int ButtonPin)
{
  if(ButtonPin != BUTTOM)
    return -1;

  int Data = digitalRead(ButtonPin);
  return Data;
}

/* Инициалтизация кнопки */
void InitButton() {
  pinMode(BUTTOM, INPUT_PULLUP);
}

/* Инициализация пинов для управления двигателями */
void InitMotors()
{
  pinMode(M1_DIR, OUTPUT);
  pinMode(M1_PWM, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
  pinMode(M2_PWM, OUTPUT);
}

/* Запись скорости в моторы */
void Motors(int Speed1, int Speed2)
{
  if(Speed1 > MAX_SPEED) Speed1 = MAX_SPEED;
  if(Speed1 < -MAX_SPEED) Speed1 = -MAX_SPEED;
  if(Speed2 > MAX_SPEED) Speed2 = MAX_SPEED;
  if(Speed2 < -MAX_SPEED) Speed2 = -MAX_SPEED;

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

//Ехать вперед
void MoveForward(int Speed)
{
  Motors(Speed, Speed);
}

/* Установка скорости с учетом работы регулятора */
void MoveForward_U(int SpeedL, int SpeedR, int influens){
  int resL = SpeedL + influens;
  int resR = SpeedR - influens;
  Motors(resL, resR);
}


/* Остановка моторов */
void Stop()
{
  Motors(0, 0);
}

void setup() {
  InitMotors();
  InitButton();
  Serial.begin(9600);
}

/* Функция для состояния "езда по линии" */
void stateMoveLine() {
  kp = 0.6;
  error = sensorValueL - sensorValueR;
  p = error * kp;

  i = i + error*ki;
  if (abs(i) > iMax){
    if (i < 0) {
      i = -iMax;
    } else {
      i = iMax;
    }
  }
  d = (error / eOld) * kd;
  U = p + i + d;
  eOld = error; 

  MoveForward_U(speedL, speedR, U);  
}

void checkButtonValue(){
  isButton = ButtonSignalRead(BUTTOM);
  if (!isButton) {
    stateMove = !stateMove;
    delay(1000);
  }
}

void searchLine(){
  kp = 0.9;
  clock++;
  Motors(speedSearchL, speedSearchR);
  if (clock > clockLimit){
    clock = 0;
    speedSearchL -= 30;
    clockLimit = clockLimit * 1.4;
    if (speedSearchL < 130) 
    {
      speedSearchL = MAX_SPEED;
      
      if (!speedSearchR){
        speedSearchR = 120;
      } else {
        speedSearchR += 10;
      }
    }
  }
  //speedL -= 10;
}

void loop() {
  checkButtonValue();
  stateSearch = false;
  
  if (sensorValueL < 600 && sensorValueR < 600 && stateMove){
    stateSearch = true;
  }
  if (stateMove){
    if (stateSearch){
      searchLine();
    } else {
      stateMoveLine();
    }
  } else {
    Stop();
  }

  sensorValueL = LightSensorRead(A0); 
  sensorValueR = LightSensorRead(A1); 
}
