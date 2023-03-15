//A0-LIGHT
//A1-WATER

//2-температура?
//4-реле
//5-помпа
//6-светодиод
//7-ветилятор
#include <DHT.h>

#define DHTPIN 2            //what pin we're connected to
#define DHTTYPE DHT21       //DHT 21  (AM2301)
DHT dht(DHTPIN, DHTTYPE);   //Initialize DHT sensor for normal 16mhz Arduino

//Variables
float hum;  //Stores humidity value
float temp; //Stores temperature value
int timer = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  
  
  int sensorLight = analogRead(A0);
  int sensorWater = analogRead(A1);
  //Serial.println(timer);

  //bool sensorTemp;

  //pinMode(5, OUTPUT);
  pinMode(2, INPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);

  //Serial.println(sensorWater);
  digitalWrite(4, LOW);

  //if (sensorLight > 1000){
  //if (timer < 10)
  //digitalWrite(6, HIGH);
  //  else if (timer < 20)
  //    digitalWrite(6, LOW);
  //  else timer = 0;
  //} else digitalWrite(6, LOW);

  if (sensorLight > 1000)
  {
    digitalWrite(6, HIGH);
    delay(5000);
  }
  else
    digitalWrite(6, LOW);

  
  if (sensorWater >= 500){
    digitalWrite(5, HIGH);
    digitalWrite(7, LOW);
  }   
  else {
    digitalWrite(5, LOW);
    digitalWrite(7, HIGH);
  } 

  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print("%,  Temperature: ");
  Serial.print(temp);
  Serial.println(" Celsius");
  
  //delay(2000); //Delay 2 sec.

  timer++;
}
