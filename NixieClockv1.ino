
#include "define.h"
#include "Arduino.h"
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


uint8_t hours = 0;
uint8_t minutes = 0;
uint8_t seconds = 0;

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
  NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600);
  
  // Turn modem on
  WiFi.forceSleepWake();
  delay(1);

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
  hours = timeClient.getHours();
  minutes = timeClient.getMinutes();
  seconds = timeClient.getSeconds();

  // Turn modem off
  WiFi.disconnect();
  WiFi.forceSleepBegin();
  delay(1); //For some reason the modem won't go to sleep unless you do a delay
}

void display(uint8_t char3, uint8_t char2, uint8_t char1, uint8_t char0) {
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

  if (hours == 0 && minutes == 0 && seconds == 0) {
    get_internet_time();
    display(hours/10, hours%10, minutes/10, minutes%10);
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
      display(hours/10, hours%10, minutes/10, minutes%10);
    } else
      seconds++;
  }
  
  delay(1000);

}
