# Chiptunes Player Hardware

I created a chiptunes player circuit on a breadboard containing the following:

- General Instrument [AY-3-8910][1] Programmable Sound Generator
- Arduino 5V [Pro Mini][2]
- SparkFun [Mono Amplifier][3]
- SparkFun [Level Shifter][4]
- Generic [Micro SD breakout board][5] (does not include level shifters)
- Texas Instruments [TLV-2217][6] 3.3V LDO Regulator (used by the SD breakout and level shifter)

The board has several headers to supply power, connect an FTDI adapter, and to configure the audio mixing to the amplifier.

The signal connections are as follows:

```text
Signal      AY-3-8910  Arduino  Other/Notes
------      ---------  -------  -----------
Vcc (+5V)       40       VCC    TLV-2217 Vin, Amplifier PWR+, headers, Level Shifter HV
GND              1       GND    TLV-2217 GND, Level Shifter, Amplifier, SD Breakout, headers
Audio Ch A       4              Headers
Audio Ch B       3              Headers
Audio Ch C      38              Headers
BDIR            27        A2
BC2             28        A1
BC1             29        A0
RESET           23              N.C. (pulled up internal to AY-3-8910)
CLOCK           22        D9
A9              24              N.C. (pulled down internal to AY-3-8910)
A8              25              N.C. (pulled up internal to AY-3-8910)
DA7             30        A3
DA6             31        D8
DA5             32        D7
DA4             33        D6
DA3             34        D5
DA2             35        D4
DA1             36        D2
DA0             37        D3
SPI MOSI                 D11    Level Shifter, then SD Breakout MOSI
SPI MISO                 D12    Level Shifter, then SD Breakout MISO
SD CS                    D10    Level Shifter, then SD Breakout CS
SPI CLK                  D13    Level Shifter, then SD Breakout CLK
3.3V                            TLV-2217 Vout, Level Shifter LV, SD Breakout 3.3V
RXI                       D0    FTDI Header TX
TXO                       D1    FTDI Header RX
BUTTON              A4/D18/SDA  Pushbutton switch to GND
```

## Board Photos

The 3.3V regulator is mounted below the Arduino (to save space) and is not visible in the photos.

### Top

![Top of Board][7]

### Bottom

![Bottom of Board][8]

[1]: ../../README.md
[2]: https://www.sparkfun.com/products/11113
[3]: https://www.sparkfun.com/products/11044
[4]: https://www.sparkfun.com/products/12009
[5]: https://www.amazon.com/Module-Storage-Reader-Arduino-Memory/dp/B07YHGCC8C/ref=sr_1_8?dchild=1&keywords=micro-sd+breakout+board&qid=1614302508&sr=8-8
[6]: https://www.ti.com/lit/ds/symlink/tlv2217.pdf
[7]: ./ChiptunesPlayer-Top.jpg
[8]: ./ChiptunesPlayer-Bottom.jpg
[200]: https://github.com/Andy4495/AY3891x
