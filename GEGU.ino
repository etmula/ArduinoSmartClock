
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

#define  STROBE_TM D5
#define  CLOCK_TM D6
#define  DIO_TM D7
//RTC
WidgetRTC rtc;

//Constructor object
TM1638plus tm(STROBE_TM, CLOCK_TM , DIO_TM);
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
}
