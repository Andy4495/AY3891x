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

   12/21/26 - A.T. - Original

*/

#include "AY3891x.h"

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
#endif

AY3891x psg(4, 5, 6, 7, 8, 10, 11, 12, 2, A5, 3);

// Assuming a 1 MHz clock on the AY38910 chip, these represent the
// Tone Generator Control values for the notes C4 (middle C) to C4
// For a 2 MHz clock, double the divider values
// Approximate frequencies: 261,  293,  329,  349,  393,  440,  492,  534 Hz
byte  tone_dividers[]   = { 239,  213,  190,  179,  159,  142,  127,  117};  // 1 MHz clock
// byte  tone_dividers[]   = { 478,  426,  379,  358,  319,  284,  253,  235};  // 2 MHz clock
const char* note_name[] = {"C4", "D4", "E4", "F4", "G4", "A5", "B5", "C5"};

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

  psg.write(AY3891x::ChA_Amplitude,0x0F); // Max amplitude
  psg.write(AY3891x::Enable_Reg, 0x3E);   // Enable Channel A tone generator output

  for (byte i = 0; i < sizeof(tone_dividers)/sizeof(tone_dividers[0]); i++){
    Serial.print("Playing note: ");
    Serial.println(note_name[i]);
    psg.write(AY3891x::ChA_Tone_Period_Coarse_Reg, 0);   // All the dividers are < 255, so no coarse adjustment needed in this example
    psg.write(AY3891x::ChA_Tone_Period_Fine_Reg, tone_dividers[i]);
    delay(1000);
  }
}

void loop() {

}
