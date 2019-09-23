
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
//private settings
//auth[]
//WiFi ssid pass
//const int birthdays[0:month 1:day][familyNo]
//const char *family[familyNo]
#include "local_settings.h"

//RTC
WidgetRTC rtc;
void setup()
{

  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
}
void loop()
{
  Blynk.run();
}
BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
}
