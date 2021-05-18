# Nixie Clock v1

The hardware was designed with EasyEda and is accessible here : https://oshwlab.com/jordan038/nixie-clock
The software was coded on the Arduino IDE

## Schematic
[Schematic_Nixie_clock_v1.pdf](https://github.com/jordanpenard/NixieClock/raw/main/Schematic_Nixie_clock_v1.pdf)

## Gerber
[Gerber_Nixie_clock_v1.zip](https://github.com/jordanpenard/NixieClock/raw/main/Gerber_Nixie_clock_v1.zip)

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
- NTPClient by Fabrice Weinberg

Required board :
- esp8266 (https://arduino.esp8266.com/stable/package_esp8266com_index.json)

You will need to set your WIFI SSID and password in `define.h`
