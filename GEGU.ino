
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
//LED,7segLED and Buttons Library
#include <TM1638plus.h>

//private settings
//auth[]
//WiFi ssid pass
//const int birthdays[0:month 1:day][familyNo]
//const char *family[familyNo]
#include "local_settings.h"

//settings
#define  STROBE_TM D5
#define  CLOCK_TM D6
#define  DIO_TM D7


//values
char buf[100];
//viretualPin data[PinNo]
String data[40];
int random_temp = random(10);

//RTC
WidgetRTC rtc;

//Constructor object
TM1638plus tm(STROBE_TM, CLOCK_TM , DIO_TM);

//functions
void emailsendMode(int buttonNo);
void randomMode(TM1638plus device);
void birthdayIlluminate(TM1638plus device, unsigned long mil, int familyNo);
void informationdisplayMode(int buttonNo);
void datestampMode(int buttonNo);
//display LED when not Birthday
void displayLED(TM1638plus device);
//display time like " 00  00 "
void displayTime(TM1638plus device, int ho, int mi);
//display date like "19-09-23"
void displayDate(TM1638plus device, int ye, int mo, int da);
//display string last 8 charactors
void displayString(TM1638plus device,String text);
//overwrite all data to viretualPin
void virtualwrite();

void setup()
{

  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
  tm.reset();
}
void loop()
{
  Blynk.run();
}
BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
  Blynk.syncVirtual(V0, V1, V2, V3, V4, V5, V6, V7, V10, V11, V12, V13, V20, V21, V22, V35, V36, V37);
  
}

void randomMode(TM1638plus device){
  while(device.readButtons() != 0){
    device.displayASCII(0,random(48,58));
    device.displayASCII(1,random(48,58));
    device.displayASCII(2,random(48,58));
    device.displayASCII(3,random(48,58));
    device.displayASCII(4,random(48,58));
    device.displayASCII(5,random(48,58));
    device.displayASCII(6,random(48,58));
    device.displayASCII(7,random(48,58));
    delay(100);
  }
  randomSeed(now());
  device.reset();
  for(int i = 7;i >= 0;i--){
    device.displayASCII(i,random(48,58));
    delay(100);
  }
  while(device.readButtons() == 0){delay(1);}
}


void birthdayIlluminate(TM1638plus device, unsigned long mil, int familyNo){
  int firework_frequency = 10000;
  int firework_speed = 100;
  
//LED firework
  if(random_temp * int(firework_frequency / 10) < mil % firework_frequency   &&   mil % firework_frequency < random_temp * int(firework_frequency / 10) + 1000){
    for(int i = 0;i < 4;i++){
      bool isOn = i * firework_speed < mil % 1000  &&  mil % 1000 < (i + 1) * firework_speed;
      device.setLED(4 + i, isOn);
      device.setLED(3 - i, isOn);
    }
  }else{
    displayLED(device);
  }
    //ランダムリセット
  if(mil % 10000 > random_temp * 1000 + 990){random_temp = random(10);}
  
//birthday message
  if(mil % 10000 >9000){
    device.displayText(family[familyNo]);
  }else if(mil % 10000 > 8000){
    device.displayText("birthdAy");
  }else if(mil % 10000 > 7000){
    device.displayText(" HAPPY  ");
  }else{
    displayTime(device, hour(), minute());
  }
  
}



void informationdisplayMode(TM1638plus device, int buttonNo){
  int displayno = 0;
  int timer = millis();
  String info_data[4] = {data[10], data[11], data[12], data[13]};
  //display info select menu
  tm.reset();
  for(int i = 0;i < sizeof(info_data)/sizeof(String);i++){
    if(i != displayno) device.display7Seg(i,0b01011100);
    else  device.display7Seg(i,0b01100011);
  }
  //Wait until the button is released
  while(device.readButtons() != 0){delay(1);}
  
  while(1){
    if(device.readButtons() == uint8_t(pow(2, buttonNo))){
      //displayno setting
      displayno++;
      if(displayno ==  sizeof(info_data)/sizeof(String)){
        displayno = 0;
      }
      //display like ooPo
      for(int i = 0;i < 8;i++){
        if(i < sizeof(info_data)/sizeof(String)){
        if(i != displayno) device.display7Seg(i,0b01011100);
        else  device.display7Seg(i,0b01100011);
        }else{
          device.display7Seg(i,0);
      }
      }
      //Wait until the button is released
      while(device.readButtons() != 0){delay(1);}
      //timer reset
      timer = millis();
    //display info
    }else if(millis() > timer + 2 * 1000){
      displayString(device,info_data[displayno]);
    //finish
    }
    if(millis() > timer + 15 * 1000 || 
            (device.readButtons() != uint8_t(pow(2, buttonNo)) && device.readButtons() != 0)){
      return;
    }
    delay(1);
  }
}



void emailsendMode(TM1638plus device, int buttonNo){
  tm.reset();
  //display like E3
  tm.display7Seg(1,0b01111001);
  tm.display7Seg(2,0b01001111);
  //Wait until the button is released
  while(device.readButtons() != 0){delay(1);}
  unsigned long timer = millis();
  while(1){
    delay(1);
    //pushed Button again 
    if(device.readButtons() == uint8_t(pow(2, buttonNo))){
      data[buttonNo] = String(now());
      virtualwrite();
      //send email
      Blynk.email("{DEVICE_OWNER_EMAIL}","{DEVICE_NAME}#GEGU","This mail was sent by {DEVICE_NAME}.");
      //animate sending email
      for(int i = 1;i < 7;i++){
        device.reset();
        device.display7Seg(i,0b01111001);
        device.display7Seg(i + 1,0b01001111);
        delay(300);
      }
      device.reset();
      return;
    //pushed other button or timeout
    }else if(millis() - timer > 5000 || tm.readButtons() != 0){
      device.reset();
      return;
    }
  }
}


void datestampMode(TM1638plus device, int buttonNo){
  displayDate(device, year(data[buttonNo].toInt()), month(data[buttonNo].toInt()), day(data[buttonNo].toInt()));
  
  //Wait until the button is released
  while(device.readButtons() != 0){delay(1);}
  
  unsigned long timer = millis();
  while(1){
    delay(1);
    uint8_t value = device.readButtons();
    if(value == uint8_t(pow(2, buttonNo))){
      data[buttonNo] = String(now());
      virtualwrite();
      displayDate(device, year(), month(), day());
      delay(3000);
      device.reset();
      return;
    }else if(millis() - timer > 5000 || value != 0){
      device.reset();
      return;
    }
  }
    
}

void displayLED(TM1638plus device){
  device.setLED(0, now() > (data[0].toInt() + 1 * 60*60*24 - data[0].toInt() % 60*60*24));
  device.setLED(1, now() > (data[1].toInt() + 7 * 60*60*24 - data[1].toInt() % 60*60*24));
  device.setLED(2, now() > (data[2].toInt() + 30 * 60*60*24 - data[2].toInt() % 60*60*24));
  device.setLED(3, now() > (data[3].toInt() + 30 * 60*60*24 - data[3].toInt() % 60*60*24));
  if(now() < data[30].toInt() + 5 * 60*60){
    if(data[30].toInt() + 10 * 60){
      device.setLED(4, millis() % 1000 < 500);
    }else{
      device.setLED(4, true);
    }
  }
  if(now() < data[31].toInt() + 5 * 60*60){
    if(data[30].toInt() + 10 * 60){
      device.setLED(5, millis() % 1000 < 500);
    }else{
      device.setLED(5,true);
    }
  }
  if(now() < data[32].toInt() + 5 * 60*60){
    if(data[30].toInt() + 10 * 60){
      device.setLED(6, millis() % 1000 < 500);
    }else{
      device.setLED(6,true);
    }
  }
  if(data[13].toInt() >= 3){
    if(data[13].toInt() >= 6){
      tm.setLED(7, millis() % 1000 < 500);
    }else{
      tm.setLED(7,true);
    }
    
  }
}

void displayTime(TM1638plus device, int ho, int mi){
  sprintf(&buf[0],"%02d%02d", hour(), minute());
  tm.display7Seg(0,0);
  tm.displayASCII(1,buf[0]);
  tm.displayASCII(2,buf[1]);
  tm.display7Seg(3,0);
  tm.display7Seg(4,0);
  tm.displayASCII(5,buf[2]);
  tm.displayASCII(6,buf[3]);
  tm.display7Seg(7,0);
}

void displayDate(TM1638plus device, int ye, int mo, int da){
  sprintf(&buf[0],"%04d%02d%02d",ye,mo,da);
  tm.displayASCII(0,buf[2]);
  tm.displayASCII(1,buf[3]);
  tm.displayASCII(2,0b0101101);
  tm.displayASCII(3,buf[4]);
  tm.displayASCII(4,buf[5]);
  tm.displayASCII(5,0b0101101);
  tm.displayASCII(6,buf[6]);
  tm.displayASCII(7,buf[7]);
}

void displayString(TM1638plus device,String text){
  if(text.length() <= 8){
  for(int i = 0;i < 8;i++){
    if(i < text.length()){
      device.displayASCII(7 - i,text[text.length() - (i + 1)]);
    }else{
      device.display7Seg(7 - i,0);
    }
  }
  }else{
    int start = millis()%((text.length()+8)*500)/500;
    for(int i = 0; i < 8; i++){
      if(start + i < 8){
        device.display7Seg(i,0);
      }else if(start-8 + i >= text.length()){
        device.display7Seg(i,0);
      }else{
        device.displayASCII(i,text[start-8+i]);
      }
    }
  }
}



void virtualwrite(){
  Blynk.virtualWrite(V0,data[0]);
  Blynk.virtualWrite(V1,data[1]);
  Blynk.virtualWrite(V2,data[2]);
  Blynk.virtualWrite(V3,data[3]);
  Blynk.virtualWrite(V4,data[4]);
  Blynk.virtualWrite(V5,data[5]);
  Blynk.virtualWrite(V6,data[6]);
  Blynk.virtualWrite(V7,data[7]);
  Blynk.virtualWrite(V10,data[10]);
  Blynk.virtualWrite(V11,data[11]);
  Blynk.virtualWrite(V12,data[12]);
  Blynk.virtualWrite(V13,data[13]);
  Blynk.virtualWrite(V20,data[20]);
  Blynk.virtualWrite(V21,data[21]);
  Blynk.virtualWrite(V22,data[22]);
  Blynk.virtualWrite(V30,data[30]);
  Blynk.virtualWrite(V31,data[31]);
  Blynk.virtualWrite(V32,data[32]);
  Blynk.virtualWrite(V35,data[30]);
  Blynk.virtualWrite(V36,data[31]);
  Blynk.virtualWrite(V37,data[32]);
}

BLYNK_WRITE(V0){
  data[0] = param.asStr();
}
BLYNK_WRITE(V1){
  data[1] = param.asStr();
}
BLYNK_WRITE(V2){
  data[2] = param.asStr();
}
BLYNK_WRITE(V3){
  data[3] = param.asStr();
}
BLYNK_WRITE(V4){
  data[4] = param.asStr();
}
BLYNK_WRITE(V5){
  data[5] = param.asStr();
}
BLYNK_WRITE(V6){
  data[6] = param.asStr();
}
BLYNK_WRITE(V7){
  data[7] = param.asStr();
}
BLYNK_WRITE(V10){
  data[10] = param.asStr();
}
BLYNK_WRITE(V11){
  data[11] = param.asStr();
}
BLYNK_WRITE(V12){
  data[12] = param.asStr();
}
BLYNK_WRITE(V13){
  data[13] = param.asStr();
}
BLYNK_WRITE(V20){
  data[20] = param.asStr();
}
BLYNK_WRITE(V21){
  data[21] = param.asStr();
}
BLYNK_WRITE(V22){
  data[22] = param.asStr();
}
BLYNK_WRITE(V30){
  data[30] = now();
  virtualwrite();
}
BLYNK_WRITE(V31){
  data[31] = now();
  virtualwrite();
}
BLYNK_WRITE(V32){
  data[32] = now();
  virtualwrite();
}
BLYNK_WRITE(V35){
  data[30] = param.asStr();
}
BLYNK_WRITE(V36){
  data[31] = param.asStr();
}
BLYNK_WRITE(V37){
  data[32] = param.asStr();
}
