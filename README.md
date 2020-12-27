AY3891x Library
====================

Arduino library for General Instrument AY-3-8910/AY-3-8912/AY-3-8913 Programmable Sound Generator chip.

All three chips have the same sound generator functionality and programming interface and only differ by the number of general purpose I/O pins available on the IC package. These I/O pins are not related to the sound generation functions.

This library has only been tested with the AY-3-8910 variant of the chip.

The AY-3-8913 variant does not have a BC2 pin, but includes a Chip Select signal. This variant is not currently supported by the library at this time (due to the differences in signal which affects both object instantiation and signaling). A future iteration of this library will likely include support.

```
Device     Package Pins  General Purpose I/O Pins
---------  ------------  ------------------------
AY-3-8910       40            16
AY-3-8912       28             8
AY-3-8913       24             0
```

Usage
-----

*Refer to the sketches in the `examples` folder.*

Constructor
<several variations>

Driving Clock from Arduino
---
From https://www.instructables.com/Arduino-MIDI-Chiptune-Synthesizer/. Code by Ian Harvey.
```
static const byte clkOUT = 9;

static const ushort DIVISOR = 7; // Set for 1MHz clock

static void clockSetup()
{
 // Timer 1 setup for Mega32U4 devices
 //
 // Use CTC mode: WGM13..0 = 0100
 // Toggle OC1A on Compare Match: COM1A1..0 = 01
 // Use ClkIO with no prescaling: CS12..0 = 001
 // Interrupts off: TIMSK0 = 0
 // OCR0A = interval value

 TCCR1A = (1 << COM1A0);
 TCCR1B = (1 << WGM12) | (1 << CS10);
 TCCR1C = 0;
 TIMSK1 = 0;
 OCR1AH = 0;
 OCR1AL = DIVISOR; // NB write high byte first

}

pinMode(clkOUT, OUTPUT);
digitalWrite(clkOUT, LOW);
clockSetup();
```




References
----------
+ AZ-3-891x [datasheet][1]

[1]: http://map.grauw.nl/resources/sound/generalinstrument_ay-3-8910.pdf
[2]:
