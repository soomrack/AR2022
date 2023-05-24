#include<TroykaDHT.h>

#define DHT_SENSOR_PIN 2 
#define HEAT 4
#define PUMP 5
#define LED 6 
#define FAN 7 

#define LIGHT A0
#define SOIL_HUM A1

DHT dht(DHT_SENSOR_PIN, DHT21);

void seyup(){
  Serial.begin(9600);
  pinMode(SOIL_HUM,INPUT);
  pinMode(LIGHT,INPUT);
  pinMode(HEAT,OUTPUT);
  pinMode(LED,OUTPUT);
  pinMode(PUMP,OUTPUT);
  pinMode(FAN,OUTPUT);
  
  dht.begin();
  digitalWrite(FAN, HIGH);
}

void loop(){
  dht.read();
  if (dht.getState()==DHT_OK){
    Serial.println(dht.getTemperatureC());
    // Serial.print("");
    // Serial.println(dht.getHumidity());
  }
  if (analogRead(LIGHT)>500)
   digitalWrite(LED,HIGH);
  else
   digitalWrite(LED,LOW);

  if (dht.getTemperatureC()<28)
   digitalWrite(HEAT,HIGH);
  else
   digitalWrite(HEAT,LOW);

  //if (analogRead(SOIL_HUM)>500)
  // digitalWrite(PUMP,HIGH);
  //else
  // digitalWrite(PUMP,LOW);
}