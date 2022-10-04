
#include "define.h"
#include "Arduino.h"
#include <WiFiUdp.h>
#include <WiFiManager.h>


// -----------
// WiFi

WiFiManager wifiManager;

void connect_to_wifi() {

  wifiManager.setConfigPortalTimeout(180); // 3min timeout
  wifiManager.autoConnect("Arduino-NixieClockV1");

  if (WiFi.status() == WL_IDLE_STATUS)
    Serial.println("WL_IDLE_STATUS");
  else if (WiFi.status() == WL_NO_SSID_AVAIL)
    Serial.println("WL_NO_SSID_AVAIL");
  else if (WiFi.status() == WL_SCAN_COMPLETED)
    Serial.println("WL_SCAN_COMPLETED");
  else if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WL_CONNECTED");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    return;
  } else if (WiFi.status() == WL_CONNECT_FAILED)
    Serial.println("WL_CONNECT_FAILED");
  else if (WiFi.status() == WL_CONNECTION_LOST)
    Serial.println("WL_CONNECTION_LOST");
  else if (WiFi.status() == WL_DISCONNECTED)
    Serial.println("WL_DISCONNECTED");
  else
    Serial.println("WL_NO_SHIELD");

  ESP.reset();
}

// -----------
// Keeping track of time

const unsigned long intervalNTP = 86400UL; // Update the time every day
unsigned long prevNTP = 0;
unsigned long lastNTPResponse = 0;

uint32_t timeUNIX = 0;                      // The most recent timestamp received from the time server

uint32_t get_unixtimestamp() {
  return timeUNIX + (millis() - lastNTPResponse) / 1000;
}

void get_internet_time() {

  const char* ntpServerName = "time.nist.gov";

  const int NTP_PACKET_SIZE = 48;  // NTP time stamp is in the first 48 bytes of the message
  byte NTPBuffer[NTP_PACKET_SIZE];     // A buffer to hold incoming and outgoing packets

  connect_to_wifi();

  send_to_display(0xa, 0xa, 0xa, 1);

  IPAddress timeServerIP;        // The time.nist.gov NTP server's IP address
  WiFiUDP UDP;                   // Create an instance of the WiFiUDP class to send and receive UDP messages

  UDP.begin(123);

  if(!WiFi.hostByName(ntpServerName, timeServerIP)) { // Get the IP address of the NTP server
    Serial.println("DNS lookup failed. Rebooting.");
    ESP.reset();
  }

  send_to_display(0xa, 0xa, 0xa, 2);

  Serial.println((String)"Time server IP : " + timeServerIP.toString());

  memset(NTPBuffer, 0, NTP_PACKET_SIZE);  // set all bytes in the buffer to 0
  // Initialize values needed to form NTP request
  NTPBuffer[0] = 0b11100011;   // LI, Version, Mode
  // send a packet requesting a timestamp:
  UDP.beginPacket(timeServerIP, 123); // NTP requests are to port 123
  UDP.write(NTPBuffer, NTP_PACKET_SIZE);
  UDP.endPacket();

  send_to_display(0xa, 0xa, 0xa, 3);

  int i = 0;
  while (UDP.parsePacket() == 0) {
    if (i > 20)
      ESP.reset();
    delay(500);
    i++;
  }

  UDP.read(NTPBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
  // Combine the 4 timestamp bytes into one 32-bit number
  uint32_t NTPTime = (NTPBuffer[40] << 24) | (NTPBuffer[41] << 16) | (NTPBuffer[42] << 8) | NTPBuffer[43];

  // Convert NTP time to a UNIX timestamp:
  // Unix time starts on Jan 1 1970. That's 2208988800 seconds in NTP time:
  const uint32_t seventyYears = 2208988800UL;
  // subtract seventy years:
  timeUNIX = NTPTime - seventyYears;

  Serial.println((String)"NTPTime : " + NTPTime);
  Serial.println((String)"Unix epoch time : " + timeUNIX);

  lastNTPResponse = millis();
  prevNTP = get_unixtimestamp();

  send_to_display(0xa, 0xa, 0xa, 4);

  // Turn modem off
  WiFi.mode(WIFI_OFF);
}

void send_to_display(uint8_t char3, uint8_t char2, uint8_t char1, uint8_t char0) {
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

void refresh_display() {
  uint8_t hours = 2;
  uint8_t minutes = 0;
  uint8_t seconds = 0;
  bool is_dst = false;

  // Get a time structure
  time_t epochTime = get_unixtimestamp();
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
  {
    hours++;
    is_dst = true;
  }
  else
    is_dst = false;

  Serial.println((String)"Internet time : " + hours + (String)":" + minutes + (String)":" + seconds);
  Serial.println((String)"DST : " + is_dst);
  Serial.println((String)"tm_mon : " + ptm->tm_mon);
  Serial.println((String)"tm_mday : " + ptm->tm_mday);
  Serial.println((String)"tm_wday : " + ptm->tm_wday);

  send_to_display(hours/10, hours%10, minutes/10, minutes%10);
}

int get_next_minute_change_in_sec() {
  time_t epochTime = get_unixtimestamp();
  struct tm *ptm = gmtime((time_t *)&epochTime);
  return(60-(ptm->tm_sec));
}

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

  send_to_display(0xa, 0xa, 0xa, 0);
  
  get_internet_time();
}

void loop() {

  // Time to update our internet time
  if (get_unixtimestamp() - prevNTP > intervalNTP) // Request the time from the time server every day
    ESP.reset();
  
  refresh_display();
  do {
    delay(1000);
  } while (get_next_minute_change_in_sec() != 60);
}
