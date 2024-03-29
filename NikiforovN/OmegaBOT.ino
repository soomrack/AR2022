#define left_light_sensor A0  // подключения портов
#define right_light_sensor A1 
#define M2_DIR 4  // в скобках правый мотор справа, левый мотор слева
#define M2_PWM 5
#define M1_DIR 7
#define M1_PWM 6


int State = 0;  // переменная состояния (0 - робот стоит, 1 - робот начинает движение)
int avgspeed = 100;  // среднее значение скорости движения робота
int kP = 4.15;  // коэффициент P-регулятора
int last_detected = 0;  // переменная, определяющая датчик, который последний раз видел линию (0 - левый, 1 - правый)
int spiral_speed = 0;  // скорость при движении по спирали

int sensor_left = analogRead(left_light_sensor);  //считываем показания левого датчика в переменную
int sensor_right = analogRead(right_light_sensor);  //считываем показания правого датчика в переменную
int sensor_left_converted = map (sensor_left, 533, 917, 100, 0);  //преобразуем значения с левого датчика в диапазон от 0 до 100 (0-черный, 100-белый), значения 533 и 917 соответсвуют показаниям левого датчика на черном и белом соответственно
int sensor_right_converted = map (sensor_right, 355, 868, 100, 0);  //преобразуем значения с правого датчика в диапазон от 0 до 100 (0-черный, 100-белый), значения 355 и 868 соответсвуют показаниям левого датчика на черном и белом соответственно
int correction_left = sensor_left_converted * kP;  //переменная корректировки по левому датчику
int correction_right = sensor_right_converted * kP;  //переменная корректировки по правому датчику
enum State { standb_by, keep_line, search_line};  // заводим массив с типом данных enum State, в который вписываем название состояний
enum State curr_state = standb_by;  // заводим переменную такого типа

void setup()
{
  Serial.begin(9600);  // скорость работы сериал порта
  pinMode(12, INPUT_PULLUP); // порт кнопки с pullup для устранения возможного дребезга
  InitMotors();  //запускаем подпрограмму, объявляющую порты двигателя
}

void InitMotors()  // порты для управления двигателями
{
  pinMode(M1_DIR, OUTPUT); 
  pinMode(M1_PWM, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
  pinMode(M2_PWM, OUTPUT);
}

void Motors(int Speed1, int Speed2)  //подпрограмма управления двигателями
{
  if (Speed1 > 255) Speed1 = 255;  // ограничения по входным скоростям (ограничение ШИМ 255 = 100% скорость вращения двигателя), speed_left
  if (Speed1 < -255) Speed1 = -255;
  if (Speed2 > 255) Speed2 = 255;
  if (Speed2 < -255) Speed2 = -255;

  if (Speed1 > 0) 
  {
    digitalWrite(M1_DIR, 1);
    analogWrite(M1_PWM, Speed1);
  }
  else 
  {
    digitalWrite(M1_DIR, 0);
    analogWrite(M1_PWM, -Speed1);
  }

  if (Speed2 > 0)  //аналогичное управление для второго мотора
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


void (*robot_actions[])(void)={&state_stand_by, &state_keep_line, &state_search_line};

void state_keep_line(){
    Motors(avgspeed - correction_left + correction_right, avgspeed - correction_right + correction_left);  //запуск моторов с учетом корректировок
    if (sensor_left_converted < 50 and sensor_right_converted > 50){  // левый датчик на черном
      last_detected = 1;
    }
    if (sensor_right_converted < 50 and sensor_left_converted > 50){  // правый датчик на черном
      last_detected = 2;
    }
    if (sensor_left_converted > 50 and sensor_right_converted > 50){  // условие для потери линии, т.е. оба датчика видят белый цвет
      curr_state = search_line;
    }
}

void state_stand_by(){
  Motors(0,0);
}

void entering_line(){
  if (last_detected == 1){
    Motors(100,0);
    delay(500);
    Motors(0,0);
    delay(500);
    return;
  }
  if (last_detected == 2){
    Motors(0,100);
    delay(500);
    Motors(0,0);
    delay(500);
    return;
  }
  
}

void state_search_line (int sensor_left_converted, int sensor_right_converted){  //функция для поиска линии движением по спирали
  if (last_detected == 0){  // Линия не замечена
    if (sensor_left_converted > 50){
      curr_state = keep_line;
    }
  }
  if (last_detected == 1){  // Левый датчик засек линию,значит она где-то слева, двигаем против часовой стрелки до момента детектирования линии правым датчиком
    if (sensor_right_converted < 50){
      entering_line();
      curr_state = keep_line;
      spiral_speed = 0;
    }
    else {
      Motors(140 + spiral_speed, -40 + spiral_speed);
      delay(100);
      spiral_speed += 1;
    }
  }
  
  if (last_detected == 2){  // Правый датчик засек линию,значит она где-то справа, двигаем по часовой стрелки до момента детектирования линии левым датчиком
    if (sensor_left_converted < 50){
      entering_line();
      curr_state = keep_line;
      spiral_speed = 0;
    }
    else{
      Motors(-40 + spiral_speed, 140 + spiral_speed);
      delay(100);
      spiral_speed += 1;
    }
  }
}


void loop() {
  sensor_left = analogRead(left_light_sensor);   //считываем показания левого датчика в переменную
  sensor_right = analogRead(right_light_sensor);   //считываем показания правого датчика в переменную
  sensor_left_converted = map (sensor_left, 533, 917, 100, 0);  //преобразуем значения с левого датчика в диапазон от 0 до 100 (0-черный, 100-белый), значения 533 и 917 соответсвуют показаниям левого датчика на черном и белом соответственно
  sensor_right_converted = map (sensor_right, 355, 868, 100, 0);  //преобразуем значения с правого датчика в диапазон от 0 до 100 (0-черный, 100-белый), значения 355 и 868 соответсвуют показаниям левого датчика на черном и белом соответственно
  correction_left = sensor_left_converted * kP;  //переменная корректировки по левому датчику
  correction_right = sensor_right_converted * kP;  //переменная корректировки по правому датчику
  
  int button = digitalRead(12);  //считываем показания с кнопки
  if (button == 0 and curr_state == standb_by){
    curr_state == keep_line;
    delay(300);
    return;
  }
  if (button == 0 and curr_state == keep_line){
    curr_state = standb_by;
    delay(300);
    return;
  }
  
  robot_actions[curr_state]();
}
