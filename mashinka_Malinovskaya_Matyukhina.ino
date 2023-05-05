#define M1_DIR 4
#define M1_PWM 5
#define M2_DIR 7
#define M2_PWM 6
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

//Ехать вперед
void MoveForward(int Speed)
{
  Motors(Speed, Speed);
}

void Stop()
{
  Motors(0, 0);
}

void MoveForwardByDelay(int Speed, int Time)
{
  MoveForward(Speed);
  delay(Time);
  Stop();
}

void MoveRight(int Speed)
{
  Motors(-Speed, Speed);
}

void MoveRightByDelay(int Speed, int Time)
{
  MoveRight(Speed);
  delay(Time);
  Stop();
}

void MoveBack(int Speed)
{
  Motors(-Speed, -Speed);
}

void MoveBackByDelay(int Speed, int Time)
{
  MoveBack(Speed);
  delay(Time);
  Stop();
}


void setup()
{
  InitMotors();
  Serial.begin(9600);
}
float ERR[10] = {0,0,0,0,0,0,0,0,0,0};
uint8_t i;
void loop()
{
  while ( ButtonRead(12) )
  {i++; if(i>=10){i=0;}
  
    
    int Y = analogRead(A2);
    Serial.println(Y); 
    int X = 120; //пост скорость 
    float KP = 2; //коэф.про.
    float kD = 2; //коэфф. диф.
    float kI = 0.015;//коэфф. инт. 
    int YP = 650; //осв
    
    ERR[i] = Y - YP ;
    float SUM    = 0; for(auto j:ERR){SUM+=j;}
    float D = (ERR[i]-ERR[(i+9)%10]) * kD; 
    float I = SUM * kI;
    float U = ERR[i]*KP;
    float RX = X +U/2+D+I;
    float LX = X - U/2-D-I;   
    Motors(LX,RX);
 
  }
}

  

  
