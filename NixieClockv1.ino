
#include "define.h"
#include "Arduino.h"
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


uint8_t hours = 2;
uint8_t minutes = 0;
uint8_t seconds = 0;
bool is_dst = false;

void setup() {
  Serial.begin(115200);

  pinMode(ShiftClk, OUTPUT);
  pinMode(OutEn, OUTPUT);  
  pinMode(ShiftIn, OUTPUT);
  pinMode(Latch, OUTPUT);
  pinMode(Clear, OUTPUT);

  digitalWrite(ShiftClk, LOW);  
  digitalWrite(OutEn, LOW);  
  digitalWrite(ShiftIn, LOW);  
  digitalWrite(Latch, LOW);  
  digitalWrite(Clear, HIGH);  
}

void get_internet_time() {

  // Define NTP Client to get time
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "pool.ntp.org", 0);
  
  // Turn modem on
  WiFi.forceSleepWake();
  delay(100);

  // Connect to WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Get time
  timeClient.begin();
  timeClient.update();

  // Get a time structure
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);
  hours = ptm->tm_hour;
  minutes = ptm->tm_min;
  seconds = ptm->tm_sec;

  // Compute DST
  //  DST (GMT+1 durring summer time) starts on the last Sunday of March and ends on the last Sunday of October
  // True conditions :
  //  April to Sept
  //  March and (mday-wday)>=25
  //  October and (mday-wday)<25
  if( ( (ptm->tm_mon > 2) && (ptm->tm_mon < 9) )
   || ( (ptm->tm_mon == 2) && ((ptm->tm_mday - ptm->tm_wday) >= 25) )
   || ( (ptm->tm_mon == 9) && ((ptm->tm_mday - ptm->tm_wday) < 25) ) )
    is_dst = true;
  else
    is_dst = false;

  Serial.println((String)"Internet time : " + hours + (String)":" + minutes + (String)":" + seconds);
  Serial.println((String)"DST : " + is_dst);
  Serial.println((String)"tm_mon : " + ptm->tm_mon);
  Serial.println((String)"tm_mday : " + ptm->tm_mday);
  Serial.println((String)"tm_wday : " + ptm->tm_wday);

  // Turn modem off
  WiFi.disconnect();
  WiFi.forceSleepBegin();
  delay(1); //For some reason the modem won't go to sleep unless you do a delay
}

void display() {
  uint8_t adjusted_hours = hours;
  if(is_dst) {
    adjusted_hours++;
  }
  
  const uint8_t char3 = adjusted_hours/10;
  const uint8_t char2 = adjusted_hours%10;
  const uint8_t char1 = minutes/10;
  const uint8_t char0 = minutes%10;
  const uint16_t buff = ((char3 & 0xF) << 12) | ((char2 & 0xF) << 8) | ((char1 & 0xF) << 4) | (char0 & 0xF);

  // Value to send to the shift registers
  Serial.println(buff, HEX);

  digitalWrite(Latch, LOW);

  // Shift data through
  for(uint8_t i = 0; i < 16; i++) {
    digitalWrite(ShiftIn, (buff >> (15-i)) & 0x01);  
    delayMicroseconds(SHIFT_HALF_PERIOD_US);
    digitalWrite(ShiftClk, HIGH);  
    delayMicroseconds(SHIFT_HALF_PERIOD_US);
    digitalWrite(ShiftClk, LOW);  
  }

  digitalWrite(Latch, HIGH);
}

void loop() {

  if (hours == 2 && minutes == 0 && seconds == 0) {
    get_internet_time();
    display();
  } else {
    // Add 1 second to the clock
    if (seconds >= 59) {
      if (minutes >= 59) {
        if (hours >= 23) {
          hours = 0;
        } else
          hours++;
        minutes = 0;
      } else
        minutes++;
      seconds = 0;
      display();
    } else
      seconds++;
  }
  
  delay(1000);

}
