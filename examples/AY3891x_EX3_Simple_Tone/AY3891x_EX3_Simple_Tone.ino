/* -----------------------------------------------------------------
   AY3891x Library Example Sketch 3: Simple Tone Generator
   https://github.com/Andy4495/AY3891x

   This code will play through the middle-octave notes C4 to C5.
   It assumes a 1 MHz clock input to the AY-3-891x chip.

   There is hardware-specific code to have the microcontroller
   generate the 1 MHz clock. If you are not using an Atmega328 or 32U4
   device, then you will need to provide your own code or clock signal
   to generate the required clock, and comment out
   // #define HARDWARE_GENERATED_CLOCK

   12/21/20 - A.T. - Original
   08/24/21 - A.T. - Support PROGMEM for Notes[] array

*/

#include "AY3891x.h"
#include "AY3891x_sounds.h"  // Contains the divisor values for the musical notes

// Be sure to use the correct pin numbers for your setup.
//          DA7, DA6, DA5, DA4, DA3, DA2, DA1, DA0, BDIR, BC2, BC1
AY3891x psg( A3,   8,   7,   6,   5,   4,   2,   3,   A2,  A1,  A0);

#ifdef ARDUINO_ARCH_AVR
#define HARDWARE_GENERATED_CLOCK  // Comment this line if not using supported microcontroller
#ifdef HARDWARE_GENERATED_CLOCK
// The following code generates a 1 MHz 50% duty cycle output to be used
// as the clock signal for the AY-3-891x chip.
// Note that the following code is hardware-specific. It works on certain Atmega
// chips (including Arduino UNO), but will not work on all microcontrollers
// without modification
static const byte clkOUT = 9;
const byte DIVISOR = 7; // Set for 1MHz clock
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
#endif

const int notes_to_play[] = {
  C_4, D_4, E_4, F_4, G_4, A_4, B_4, C_5,
  C_4, E_4, G_4, C_5, G_4, E_4, C_4
};

void setup() {
#ifdef HARDWARE_GENERATED_CLOCK
  // Hardware-specific microcontroller code to generate a clock signal for the AY-3-891x chip
  pinMode(clkOUT, OUTPUT);
  digitalWrite(clkOUT, LOW);
  clockSetup();
#endif
  Serial.begin(38400);
  Serial.println("");
  Serial.println("AY-3-891x Sound Chip Library Example 3: Simple Tone Generator.");

  psg.begin();
  // psg.setAddress(TheChipsAddress);   // Only need this for special-ordered chips with non-default address.

  // Use less than max amplitude, in case external amp can't handle the higher level (start low and increase after testing)
  psg.write(AY3891x::ChA_Amplitude, 0x04); // Lower amplitude
  psg.write(AY3891x::ChB_Amplitude, 0x08); // Mid amplitude
  psg.write(AY3891x::Enable_Reg, ~(MIXER_TONE_A_DISABLE | MIXER_TONE_B_DISABLE));   // Enable Channel A and B tone generator output

  for (byte i = 0; i < sizeof(notes_to_play) / sizeof(notes_to_play[0]); i++) {
    Serial.print("Playing note freq: ");
    Serial.println(1000000UL/16/pgm_read_word(&Notes[notes_to_play[i]]));
    psg.write(AY3891x::ChA_Tone_Period_Coarse_Reg, pgm_read_word(&Notes[notes_to_play[i]]) >> 8);
    psg.write(AY3891x::ChA_Tone_Period_Fine_Reg, pgm_read_word(&Notes[notes_to_play[i]]) & TONE_GENERATOR_FINE);
    psg.write(AY3891x::ChB_Tone_Period_Coarse_Reg, pgm_read_word(&Notes[notes_to_play[i]]) >> 8);
    psg.write(AY3891x::ChB_Tone_Period_Fine_Reg, pgm_read_word(&Notes[notes_to_play[i]]) & TONE_GENERATOR_FINE);
    delay(1000);
  }

  psg.write(AY3891x::Enable_Reg, MIXER_ALL_DISABLED);   // Disable Tone Generators
}

void loop() {

}
