// переменные таймеров
#define pompa 9 // пин помпы полива
#define priv_for 5 // пин привод вперед
#define priv_rev 6 // пин привод зад
#define svet 7 // пин реле света
#define dat_svet 14 // вход датчика света
#define svetlo 600 // значение датчика когда светло (0-1023)
#define temp 600 // значение датчика темпратуры (0-1023)
#define temp_dat 15 // вход датчика температуры 
#define btn1 3
#define btn2 4
#define btn3 8
uint32_t myTimer1, myTimer2, tmr, tmr1, tmr2, tmr3;
bool flag_priv;
bool flag_poliv;
bool flag_svet;
bool btn_1;
bool btn_2;
bool btn_3;
bool flag1;
bool flag2;
bool flag3;
void setup() {
  Serial.begin(9600);
  pinMode(pompa, OUTPUT);
  pinMode(svet, OUTPUT);
  pinMode(priv_for, OUTPUT);
  pinMode(priv_rev, OUTPUT);
  pinMode(dat_svet, INPUT);
  pinMode(temp_dat, INPUT);
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(btn3, INPUT_PULLUP);
  }
void loop() {  // вызов полива
if (millis() - tmr >= 1800000) {
  if (millis() - myTimer1 >= 28800000) {   // таймер на 8 часов
    myTimer1 = millis();              
    flag_poliv = 1;
    poliv();  
  }
  
  if (analogRead(dat_svet) < svetlo) { // опрос датчика света
    flag_svet = 1;
    fanar();
  }
    else {
      flag_svet = 0;
      fanar();
    }
  
  if  (analogRead(temp_dat) > temp) {   // опрос датчика температуры
    flag_priv = 1;
    temper(); 
  }
    else {
      flag_priv = 0;
      temper(); 
    }
}
   btn_1 = !digitalRead(3); 
 if (btn_1 == 1 && flag1 == 0 && millis() - tmr1 >= 200) {
  tmr1 = millis();
  flag1 = 1;
 }
 if (btn_1 == 0 && flag1 == 1 && millis() - tmr1 >= 200) {
  tmr1 = millis();
  flag1 = 0;
  flag_svet = !flag_svet;
  fanar();
 }

  btn_2 = !digitalRead(3); 
 if (btn_2 == 1 && flag2 == 0 && millis() - tmr2 >= 200) {
  tmr2 = millis();
  flag2 = 1;
 }
 if (btn_2 == 0 && flag2 == 1 && millis() - tmr2 >= 200) {
  tmr2 = millis();
  flag2 = 0;
  flag_poliv = !flag_poliv;
  poliv();
 }

  btn_3 = !digitalRead(3); 
 if (btn_3 == 1 && flag3 == 0 && millis() - tmr3 >= 200) {
  tmr3 = millis();
  flag3 = 1;
 }
 if (btn_3 == 0 && flag3 == 1 && millis() - tmr3 >= 200) {
  tmr3 = millis();
  flag3 = 0;
  flag_priv = !flag_priv;
  temper();
 }
   
   if (Serial.available() > 1) {   //BLYNK
    char key = Serial.read();
    bool val = Serial.parseInt();
    switch (key) {
      case 'a': flag_svet = val; fanar(); Serial.print("d"); Serial.print(val);
      break;
      case 'b': poliv(); Serial.print("e"); Serial.print(val);
      break;
      case 'c': flag_priv = val; temper(); Serial.print("f"); Serial.print(val);       
      break;
    }
  }
}

void fanar() {  // Свет
  if (flag_svet) {digitalWrite(svet, flag_svet); Serial.print("d"); Serial.print(flag_svet);}
  else {digitalWrite(svet, flag_svet); Serial.print("d"); Serial.print(flag_svet);}
}

void poliv() {            // полив
    myTimer2 = myTimer1;
    if (flag_poliv) {
    digitalWrite(pompa, true); // вкл полив
    Serial.print("e"); Serial.print(flag_poliv);
      if (millis() - myTimer2 >= 10800000) {   // таймер на 3 часа
        myTimer1 = millis();              // сброс таймера
        digitalWrite(pompa, false); // выкл полив
        flag_poliv = 0;
        Serial.print("e"); Serial.print(flag_poliv);
      } 
    }  
} 

void temper() {        // форточка
  bool pologenie;
  if (flag_priv == 1 && pologenie == 0) {
    digitalWrite(priv_for, true);
    digitalWrite(priv_rev, false);
    pologenie = 1;
    Serial.print("f"); Serial.print(pologenie);
  }
  if (flag_priv == 0 && pologenie == 1) {
    digitalWrite(priv_for, false);
    digitalWrite(priv_rev, true);
    pologenie = 0;
    Serial.print("f"); Serial.print(pologenie);
  }
}
