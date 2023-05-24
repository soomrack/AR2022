int M1_DIR = 4;
int M1_PWM = 5;
int M2_DIR = 6;
int M2_PWM = 7;
int left_sensor A0;
int right_sensor A1;

int ButtonRead(int ButtonPin)
{
  int Data = digitalRead(ButtonPin);
  if(ButtonPin == 12) Data = !Data;
  return Data;
}


void InitMotaors()
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


void Forward(int Speed)
{
  Motors(Speed, Speed);
}

void Stop()
{
  Motors(0, 0);
}

void ForwardByDelay(int Speed, int Time)
{
  Forward(Speed);
  delay(Time);
  Stop();
}

void Right(int Speed)
{
  Motors(-Speed, Speed);
}

void RightByDelay(int Speed, int Time)
{
  Right(Speed);
  delay(Time);
  Stop();
}

void Back(int Speed)
{
  Motors(-Speed, -Speed);
}

void BackByDelay(int Speed, int Time)
{
  Back(Speed);
  delay(Time);
  Stop();
}


void find_way (int sensor_left_converted, int sensor_right_converted){
  if (last_detected == 0)
  {                                                        
    if (sensor_left_converted > 50)
    {
      State = 1;
    }
  }
  if (last_detected == 1)
  {                                          
    if (sensor_right_converted < 50)
    {
      entering_line();
      State = 1;
      spiral_speed = 0;
    }
    else 
    {
      Motors(140 + spiral_speed, -40 + spiral_speed);
      delay(100);
      spiral_speed++;
    }
  }
  if (last_detected == 2)
  { 
    if (sensor_left_converted < 50)
    {
      entering_line();
      State = 1;
      spiral_speed = 0;
    }
    else
    {
      Motors(-40 + spiral_speed, 140 + spiral_speed);
      delay(100);
      spiral_speed++;
    }
  }
  
void loop() 
{
  int sensor_left = analogRead(left_light_sensor);
  int sensor_right = analogRead(right_light_sensor);
  int sensor_left_converted = map (sensor_left, 533, 917, 100, 0);
  int sensor_right_converted = map (sensor_right, 355, 868, 100, 0);
  int button = digitalRead(12);
  if (button == 0 and State == 0)
  {
    tone(9, 104, 1000);
    State = 1;
    delay(300);
  }
  else if (button == 0 and State == 1)
  {
    tone(9, 104, 1000);
    State = 0;
    delay(300);
  }
  if (State == 1)
  { 
    int correction_left = sensor_left_converted * kP;
    int correction_right = sensor_right_converted * kP;
    Motors(avgspeed - correction_left + correction_right, avgspeed - correction_right + correction_left);

    if (sensor_left_converted < 50 and sensor_right_converted > 50){
      last_detected = 1;
    }
    if (sensor_right_converted < 50 and sensor_left_converted > 50){
      last_detected = 2;
    }
    if (sensor_left_converted > 50 and sensor_right_converted > 50){
     { 
      tone(9, 104, 1000);
      State = 2;
     }
    }
  }
  
  if (State == 0)
  { 
    Stop(0);
  }
  if (State == 2)
  { 
    find_way(sensor_left_converted, sensor_right_converted);
  }
}
