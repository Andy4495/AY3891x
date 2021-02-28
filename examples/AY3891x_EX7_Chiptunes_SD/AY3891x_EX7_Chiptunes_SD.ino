/* ------------------------------------------------------------------
   AY3891x Library Example Sketch 7: Play chiptunes data from SD card
   https://github.com/Andy4495/AY3891x

   Music Player using AY3891x Library.

   This example sketch plays YM files which are stored on an SD card.
   The sketch sequentially goes through each file in the root directory.
   It does not check the format of the file.
   To create a properly formatted file, see the README at the link above.

   There is hardware-specific code to have the microcontroller
   generate an approximate 2 MHz clock. If you are not using an Atmega328 or
   32U4 device, then you will need to provide your own code or clock signal
   to generate the required clock, and comment out
   // #define HARDWARE_GENERATED_CLOCK

   02/08/21 - A.T. - Original
   02/25/21 - A.T. - Check for button on pin 18 (aka SDA or A4) to advance to next file

*/

#include <Arduino.h>
#include "AY3891x.h"
#include "SD.h"

// Be sure to use the correct pin numbers for your setup.
//          DA7, DA6, DA5, DA4, DA3, DA2, DA1, DA0, BDIR, BC2, BC1
AY3891x psg( A3,   8,   7,   6,   5,   4,   2,   3,   A2,  A1,  A0);

unsigned long prev_micros = 0;
// Write the data at a rate of 50 Hz <-> every 20000 us
#define INTERVAL 20000

#define CS   10

#define NEXT_BUTTON 18
// Time in milliseconds to wait for keybounce to settle.
// 20 ms usually works well for most buttons, adjust if necessary
#define KEYBOUNCE_DELAY 20


#define HARDWARE_GENERATED_CLOCK  // Comment this line if not using supported microcontroller
#ifdef HARDWARE_GENERATED_CLOCK
// The following code generates an approximate 2 MHz 50% duty cycle output to be used
// as the clock signal for the AY-3-891x chip.
// Note that the following code is hardware-specific. It works on certain Atmega
// chips (including Arduino UNO), but will not work on all microcontrollers
// without modification
static const byte clkOUT = 9;
const byte DIVISOR = 3; // Set for approximate 2 MHz clock
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

File root;
File current_song;
enum State {PLAYING, GETNEXT, NOMOREFILES, BUTTONRELEASE};
State state;

unsigned long last_button = 0;

void setup()
{
#ifdef HARDWARE_GENERATED_CLOCK
  // Hardware-specific microcontroller code to generate a clock signal for the AY-3-891x chip
  pinMode(clkOUT, OUTPUT);
  digitalWrite(clkOUT, LOW);
  clockSetup();
#endif

  pinMode(CS, OUTPUT);
  pinMode(NEXT_BUTTON, INPUT_PULLUP);
  Serial.begin(9600);

  psg.begin();
  // psg.setAddress(TheChipsAddress);   // Only need this for special-ordered chips with non-default address.

  Serial.println(F(""));
  Serial.println(F("AY-3-891x Sound Chip Library Example 7: Sound Chip YM data player."));
  Serial.print(F("Press button on pin "));
  Serial.print(NEXT_BUTTON);
  Serial.println(F(" to advance to next song on SD card."));
  Serial.println(F(""));

  if (!SD.begin(CS))
  {
    Serial.println(F("SD Initialization Failed: program stopped."));
    while (1); // Infinite loop here
  }
  else
  {
    Serial.println("SD Initialization OK");
    Serial.println("");
    root = SD.open("/");
    state = GETNEXT;
  }
}

void loop()
{
  int i;
  byte register_data[14];

  switch (state) {

    case PLAYING:
      if (micros() - prev_micros > INTERVAL)
      {
        prev_micros = micros();
        if (current_song.available())
        {
          for (i = 0; i < 14; i++) {
            register_data[i] = current_song.read();
          }
          for (i = 0; i < 14; i++) {
            psg.write( i, register_data[i]);
          }
        }
        else
        {
          current_song.close();
          state = GETNEXT;
        }
      }
      if (digitalRead(NEXT_BUTTON) == LOW)
      {
        if (millis() - last_button > KEYBOUNCE_DELAY)
        {
          last_button = millis();
          state = BUTTONRELEASE;
          // Turn off audio if button is pressed
          psg.write(AY3891x::Enable_Reg, MIXER_NOISES_DISABLE | MIXER_TONES_DISABLE | psg.read(AY3891x::Enable_Reg));
        }
      }
      break;

    case BUTTONRELEASE:
      if (digitalRead(NEXT_BUTTON) == HIGH)
      {
        if (millis() - last_button > KEYBOUNCE_DELAY)
        {
          last_button = millis();
          current_song.close();
          state = GETNEXT;
        }
      }
      break;

    case GETNEXT:
      current_song = root.openNextFile();

      if (!current_song)
      {
        // No more files on the SD card
        state = NOMOREFILES;
        // When no more song files, turn off audio
        psg.write(AY3891x::Enable_Reg, MIXER_NOISES_DISABLE | MIXER_TONES_DISABLE | psg.read(AY3891x::Enable_Reg));
        Serial.println("No more files found. Press button to start again.");
      }
      else
      {
        if ( (strncmp(current_song.name(), "SYSTEM", 6) == 0) || (strncmp(current_song.name(), "TRASH", 5) == 0) )
        {
          // Windows creates a folder that is difficult to remove, so just ignore it.
          Serial.print("Skipping file: ");
          Serial.println(current_song.name());
          current_song.close();
          // state will stil be GETNEXT for next iteration through loop()
        }
        else
        {
          Serial.print("File opened: ");
          Serial.println(current_song.name());
          Serial.print("File size (bytes): ");
          Serial.println(current_song.size());
          Serial.print("Duration: ");
          Serial.print(current_song.size() / 50UL / 14UL);
          Serial.println(" seconds");
          state = PLAYING;
        }
      }
      break;

    case NOMOREFILES:
      if (digitalRead(NEXT_BUTTON) == LOW)
      {
        if (millis() - last_button > KEYBOUNCE_DELAY)
        {
          last_button = millis();
          state = BUTTONRELEASE;
          root.rewindDirectory();
        }
      }
      break;

    default:
      state = NOMOREFILES;
      break;
  }
}
