#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "Gw3thVUJaHrqwo4-tYCi9daLwkOUwZCO"; // Ваш токен Blynk

char ssid[] = "Xiaomi_329A"; // Название вашей точки доступа
char pass[] = "85418563432"; // Пароль к точке доступа

BLYNK_WRITE(V0) // Вызовется при обновлении виртуального порта V0
{
 int pinValue = param.asInt(); // Считываем новое значение порта
  Serial.print("a");
  Serial.println(pinValue);

 
}

BLYNK_WRITE(V1) // Вызовется при обновлении виртуального порта V0
{
 int pinValue = param.asInt(); // Считываем новое значение порта
  Serial.print("b");
  Serial.println(pinValue);

 
}

BLYNK_WRITE(V2) // Вызовется при обновлении виртуального порта V0
{
 int pinValue = param.asInt(); // Считываем новое значение порта
  Serial.print("c");
  Serial.println(pinValue);

 
}

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass); // Инициализация и подключение к Blynk
 
}

void loop() {
  Blynk.run();
  if (Serial.available() > 1) {   //BLYNK
    char key = Serial.read();
    bool val = Serial.parseInt();
    switch (key) {
      case 'd': Blynk.virtualWrite(V0, val);
      break;
      case 'e': Blynk.virtualWrite(V1, val);
      break;
      case 'f': Blynk.virtualWrite(V2, val);
                
      break;
    }
  }
}
