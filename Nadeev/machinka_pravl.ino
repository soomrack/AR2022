#define motor_right_DIR 4
#define motor_right_PWM 5
#define motor_left_DIR 7
#define motor_left_PWM 6
#define Sensor A1

int Kp = 2.97;
int reference_value = 710;
int spd = 255;
int U = 0;
int moment_out_of_line = 0;
int time_out_of_line = 2;

int button_read(int ButtonPin) {
  int Data = digitalRead(ButtonPin);
  if (ButtonPin == 12) Data = !Data;
  return Data;
}
int line_sensor_read(int SensorPin) {
  int Data = analogRead(SensorPin);
  return Data;
}

int timer() {
  if (moment_out_of_line == 0) {
    moment_out_of_line = millis() / 1000;
  };
  return (millis() / 1000 - moment_out_of_line);
}

void InitMotors()  //инициализация моторов
{
  pinMode(motor_right_DIR, OUTPUT);
  pinMode(motor_right_PWM, OUTPUT);
  pinMode(motor_left_DIR, OUTPUT);
  pinMode(motor_left_PWM, OUTPUT);
}
void motors(int speed_right, int speed_left) {
  if (speed_right > 255) speed_right = 255;
  if (speed_right < -255) speed_right = -255;
  if (speed_left > 255) speed_left = 255;
  if (speed_left < -255) speed_left = -255;
  if (speed_right > 0) {
    digitalWrite(motor_right_DIR, 1);
    analogWrite(motor_right_PWM, speed_right);
  } else {
    digitalWrite(motor_right_DIR, 0);
    analogWrite(motor_right_PWM, -speed_right);
  }
  if (speed_left > 0) {
    digitalWrite(motor_left_DIR, 1);
    analogWrite(motor_left_PWM, speed_left);
  } else {
    digitalWrite(motor_left_DIR, 0);
    analogWrite(motor_left_PWM, -speed_left);
  }
}

void inactive(), forward_to_the_line(), line_movement(), line_search();  // States

void (*statefunc)() = inactive;  // State pointer

void inactive() {
  motors(0, 0);
  if (button_read(12)) {
    statefunc = forward_to_the_line;
  };
};
void forward_to_the_line() {
  motors(spd / 3, spd / 3);
  if ((line_sensor_read(Sensor) > reference_value)) {
    statefunc = line_movement;
  };
};
void line_movement() {
  U = Kp * (line_sensor_read(Sensor) - reference_value);
  motors(spd - U, spd + U);
  if ((line_sensor_read(Sensor) < 600)) {
    timer();
  } else {
    moment_out_of_line = 0;
  };
  if (timer() > time_out_of_line) {
    statefunc = line_search;
    U = 30;
  };
};
void line_search() {
  U = U + 0.5;
  motors(spd, U);
  delay(10);
  if ((line_sensor_read(Sensor) > reference_value)) {
    statefunc = line_movement;
  };
  if (U == spd) {
    statefunc = inactive;
    U = 0;
  };
};

void setup() {
  pinMode(12, INPUT_PULLUP);
  InitMotors();
  Serial.begin(9600);
}

void loop() {
  (*statefunc)();
}