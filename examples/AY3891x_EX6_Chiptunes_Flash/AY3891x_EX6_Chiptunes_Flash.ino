/* ---------------------------------------------------------------------
   AY3891x Library Example Sketch 6: Play chiptunes data stored in flash
   https://github.com/Andy4495/AY3891x

   Music Player using AY3891x Library.

   This example plays YM file which is compiled into the program
   as a byte array of the data values to write to the 14 audio
   registers on the sound chip.

   Needs specially-formatted header file containing a pre-defined
   array of data values. See README available at link above for more info.

   There is hardware-specific code to have the microcontroller
   generate an approximate 2 MHz clock. If you are not using an Atmega328 or
   32U4 device, then you will need to provide your own code or clock signal
   to generate the required clock, and comment out
   // #define HARDWARE_GENERATED_CLOCK

   02/08/21 - A.T. - Original
   07/25/22 - Andy4495 - Implement non-blocking delay

*/

#include <Arduino.h>
#include "AY3891x.h"
#include "chiptunes.h"

// Be sure to use the correct pin numbers for your setup.
//          DA7, DA6, DA5, DA4, DA3, DA2, DA1, DA0, BDIR, BC2, BC1
AY3891x psg( A3,   8,   7,   6,   5,   4,   2,   3,   A2,  A1,  A0);

size_t index = 0;
unsigned long prev_micros;
// Write the data at a rate of 50 Hz <-> every 20000 us
#define INTERVAL 20000


#define HARDWARE_GENERATED_CLOCK  // Comment this line if not using supported microcontroller
#ifdef HARDWARE_GENERATED_CLOCK
// The following code generates an approximately 2 MHz 50% duty cycle output to be used
// as the clock signal for the AY-3-891x chip.
// Note that the following code is hardware-specific. It works on certain Atmega
// chips (including Arduino UNO), but will not work on all microcontrollers
// without modification
static const byte clkOUT = 9;
const byte DIVISOR = 3; // Set for approximate 2 MHz clock
static void clockSetup()
{
  TCCR1A = (1 << COM1A0);
  TCCR1B = (1 << WGM12) | (1 << CS10);
  TCCR1C = 0;
  TIMSK1 = 0;
  OCR1AH = 0;
  OCR1AL = DIVISOR;
}
#endif

void setup()
{
#ifdef HARDWARE_GENERATED_CLOCK
  // Hardware-specific microcontroller code to generate a clock signal for the AY-3-891x chip
  pinMode(clkOUT, OUTPUT);
  digitalWrite(clkOUT, LOW);
  clockSetup();
#endif

  psg.begin();
  // psg.setAddress(TheChipsAddress);   // Only need this for special-ordered chips with non-default address.
}

void loop()
{
  byte i;

  if (micros() - prev_micros > INTERVAL)
  {
    prev_micros = micros();

    for (i= 0; i < 14; i++) {
      psg.write(i, pgm_read_byte(&psg_data[index++]));
    }

    if (index >= sizeof(psg_data)) {
      psg.write(AY3891x::Enable_Reg, MIXER_NOISES_DISABLE | MIXER_TONES_DISABLE | psg.read(AY3891x::Enable_Reg));
      while (1);
      // Or... you can re-start the song:
      //  index = 0;
    }
  }

}
