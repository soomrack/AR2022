#define STOP      0 
#define RUN       1 
#define BACK      2 
#define LEFT      3 
#define RIGHT     4 
int a1 = 6;
int a2 = 7;
int b1 = 8;
int b2 = 9;
int sensor1 = 5; 
int sensor2 = 4; 
int sensor3 = 3; 
int sensor4 = 2; 
int leftPWM = 10;
int rightPWM = 11;
void setup()
{
  Serial.begin(9600);
  pinMode(a1, OUTPUT);
  pinMode(a2, OUTPUT);
  pinMode(b1, OUTPUT);
  pinMode(b2, OUTPUT);
  pinMode(leftPWM, OUTPUT);
  pinMode(rightPWM, OUTPUT);
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  pinMode(sensor4, INPUT);
}
void loop() 
{
  tracing();       
}
void Work(int c,int value)
{
  analogWrite(leftPWM, value);  
  analogWrite(rightPWM, value);
  switch(c)
  {
     case RUN:
      Serial.println("RUN"); 
      digitalWrite(a1, HIGH);
      digitalWrite(a2, LOW);
      digitalWrite(b1, HIGH);
      digitalWrite(b2, LOW);
      break;
     case BACK:
      Serial.println("BACK"); 
      digitalWrite(a1, LOW);
      digitalWrite(a2, HIGH);
      digitalWrite(b1, LOW);
      digitalWrite(b2, HIGH);
      break;
     case LEFT:
      Serial.println("LEFT"); 
      digitalWrite(a1, HIGH);
      digitalWrite(a2, LOW);
      digitalWrite(b1, LOW);
      digitalWrite(b2, HIGH);
      break;
     case RIGHT:
      Serial.println("RIGHT"); 
      digitalWrite(a1, LOW);
      digitalWrite(a2, HIGH);
      digitalWrite(b1, HIGH);
      digitalWrite(b2, LOW);
      break;
     default:
      Serial.println("STOP"); 
      digitalWrite(a1, LOW);
      digitalWrite(a2, LOW);
      digitalWrite(b1, LOW);
      digitalWrite(b2, LOW);
  }
}
void tracing()
{
  int d[4];
  d[0] = digitalRead(5);
  d[1] = digitalRead(4);
  d[2] = digitalRead(3);
  d[3] = digitalRead(2);
  if(!d[0] && !d[1] && !d[2] && !d[3]) 
  {
    Work(RUN, 200);
  }
  if(d[0] || d[1]) 
  {
    Work(RIGHT, 150);
  }
  if(d[2] || d[3]) 
  {
    Work(LEFT, 150);
  }
  if(d[0] && d[3]) 
  {
    Work(STOP, 0);
    while(1);
  }
  Serial.print(d[0]);
  Serial.print("---");
  Serial.print(d[1]);
  Serial.print("---");
  Serial.print(d[2]);
  Serial.print("---");
  Serial.println(d[3]);
}
