# Nixie Clock v1

![The clock](https://github.com/jordanpenard/NixieClock/raw/main/PXL_20220818_160243123.jpg)

The hardware was designed with EasyEda and is accessible here : https://oshwlab.com/jordan038/nixie-clock

The software was coded on the Arduino IDE and is available in this repo.

The 3D printed case was designed in OnShape and is accessible here : https://cad.onshape.com/documents/f923ccf06895d8b9759a02be/w/6e75daf2f2eb973f4217d7dc/e/3933878daeafba718c4affce

## Schematic
[Schematic_Nixie_clock_v1.pdf](https://github.com/jordanpenard/NixieClock/raw/main/Schematic_Nixie_clock_v1.pdf)

## Gerber
[Gerber_Nixie_clock_v1.zip](https://github.com/jordanpenard/NixieClock/raw/main/Gerber_Nixie_clock_v1.zip)

## 3D printed case
[Case_Nixie_clock_v1](https://www.thingiverse.com/thing:5468544)

## BOM
|Name                             |Designator |Footprint                        |Quantity|
|---------------------------------|-----------|---------------------------------|--------|
|IN-14                            |N1,N2,N3,N4|IN-14                            |4       |
|200                              |R1         |RESISTOR                         |1       |
|4.7k                             |R2,R3,R4,R5|RESISTOR                         |4       |
|K155ID1                          |U1,U2,U3,U4|DIP16                            |4       |
|74HC595                          |U5,U6      |DIP16                            |2       |
|WEMOS D1 MINI                    |U7         |WEMOS D1 MINI                    |1       |
|High voltage supply by valentinas|U8         |HIGH VOLTAGE SUPPLY BY VALENTINAS|1       |

## Firmware
Required libraries :
- [WifiManager v0.16.0](https://github.com/tzapu/WiFiManager)

Required board :
- esp8266 (https://arduino.esp8266.com/stable/package_esp8266com_index.json)

On the first boot, the board will create an access point with captive portal for you to setup your WiFi credentials. Those details will be stored on the board's flash and used in the future. If the board is unable to connect to the WiFi you setup, it will bring the access point up again. The access point is named `Arduino-NixieClockV1`.

The time is gathered from `time.nist.gov`, and will display as GMT+0, and will correct for DST in the summer automatically. The board will keep track of time over the period of a day and will refresh it's time from the time server previously mentioned every 24h.

If the 3 digits on the left are blank, that means the 4th digit is displaying a debug code while the board is booting :
- 0 : Waiting for WiFi to connect
- 1 : Time server DNS lookup
- 2 : Sending NTP request
- 3 : Waiting for NTP response
- 4 : Switching WiFi off
