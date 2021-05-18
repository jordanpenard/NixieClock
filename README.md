# Nixie Clock v1

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
On the software side, the only thing that needs doing is setting your WIFI SSID and password in `define.h`
