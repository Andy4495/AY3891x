/* -----------------------------------------------------------------
   AY3891x Library Example Sketch 5: Control PSG with Serial Commands
   https://github.com/Andy4495/AY3891x

   This sketch allows you to send commands to the AY-3-891x chip
   through the serial port. It makes it easy to experiment with
   how to control various sounds from the chip.

   This makes use of SerialCommands library by Pedro Tiago Pereira. The
   SerialCommands library can be downloaded through the Library Manager
   in the Arduino IDE or directly from https://github.com/ppedro74/Arduino-SerialCommands

   There is hardware-specific code to have the microcontroller
   generate the 1 MHz clock. If you are not using an Atmega328 or 32U4
   device, then you will need to provide your own code or clock signal
   to generate the required clock, and comment out
   // #define HARDWARE_GENERATED_CLOCK

   Curently supported commands (set end-of-line to Newline in Serial Monitor)
     enable all [tones|noises]
     enable tone a|b|c
     enable noise a|b|c
     disable
     write register value
     read register
     middlec
     gunshot
     explosion

   The integer values "register" and "value" need to be entered in decimal,
   but are echoed back on the terminal in hex.

   Anytime the code tries to parse an invalid command, it gives a rather
   unhelpful message "Unrecognized command" along with the first word in
   the command line entered.

   This code automatically configures a middle C tone on channels A and B
   for ease of testing. 

   12/31/20 - A.T. - Original
   08/24/21 - A.T. - Support PROGMEM for Notes[] array

*/

#include <Arduino.h>
#include "SerialCommands.h"
#include "AY3891x.h"
#include "AY3891x_sounds.h"

char buffer[32];
SerialCommands serial_commands_(&Serial, buffer, sizeof(buffer), "\n", " ");

// Be sure to use the correct pin numbers for your setup.
//          DA7, DA6, DA5, DA4, DA3, DA2, DA1, DA0, BDIR, BC2, BC1
AY3891x psg( A3,   8,   7,   6,   5,   4,   2,   3,   A2,  A1,  A0);

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

//This is the default handler, and gets called when no other command matches.
void cmd_unrecognized(SerialCommands* sender, const char* cmd)
{
	sender->GetSerial()->print(F("Unrecognized command ["));
	sender->GetSerial()->print(cmd);
	sender->GetSerial()->println(F("]"));
}

void cmd_enable(SerialCommands* sender)
{
  // "enable all"        -- enables all tone and noise generators
  // "enable all tones"  -- enables all tone generators
  // "enable all noises" -- enables all noise generators
  // "enable tone a"     -- enables tone generator A
  // "enable tone b"     -- enables tone generator A
  // "enable tone c"     -- enables tone generator A
  // "enable noise a"    -- enables noise generator A
  // "enable noise b"    -- enables noise generator B
  // "enable noise c"    -- enables noise generator C
  enum{ALL, TONE, NOISE, TONES, NOISES, CHANA, CHANB, CHANC, NONE};
  char* param1 = sender->Next();
  char* param2 = sender->Next();
  int p1, p2;
  byte value;

  if (param1 == NULL) p1 = NONE;
  else
  {
    if (strncmp(param1, "all", 3) == 0) p1 = ALL;
    else
    {
      if (strncmp(param1, "tone", 4) == 0) p1 = TONE;
      else
      {
        if (strncmp(param1, "noise", 5) == 0) p1 = NOISE;
        else p1 = NONE;
      }
    }
  }

  if (param2 == NULL) p2 = NONE;
  else
  {
    if (strncmp(param2, "tones", 5) == 0) p2 = TONES;
    else
    {
      if (strncmp(param2, "noises", 6) == 0) p2 = NOISES;
      else
      {
        if (strncmp(param2, "a", 1) == 0) p2 = CHANA;
        else
        {
          if (strncmp(param2, "b", 1) == 0) p2 = CHANB;
          else
          {
            if (strncmp(param2, "c", 1) == 0) p2 = CHANC;
            else p2 = NONE;
          }
        }
      }
    }
  }

  switch (p1)
  {
    case ALL:
      switch (p2)
      {
        case NONE:
          sender->GetSerial()->println(F("Enabling all tone and noise generators."));
          value = psg.read(AY3891x::Enable_Reg);
          psg.write(AY3891x::Enable_Reg, MIXER_INPUTS_DISABLE & value);
          break;
        case TONES:
          sender->GetSerial()->println(F("Enabling all tone generators."));
          value = psg.read(AY3891x::Enable_Reg);
          psg.write(AY3891x::Enable_Reg, (MIXER_INPUTS_DISABLE | MIXER_NOISES_DISABLE) & value);
          break;
        case NOISES:
          sender->GetSerial()->println(F("Enabling all noise generators."));
          value = psg.read(AY3891x::Enable_Reg);
          psg.write(AY3891x::Enable_Reg, (MIXER_INPUTS_DISABLE |MIXER_TONES_DISABLE) & value);
          break;
        default:
          cmd_unrecognized(sender, buffer);
      }
      break;
    case TONE:
      switch (p2)
      {
        case CHANA:
          sender->GetSerial()->println(F("Enabling Channel A tone generator."));
          value = psg.read(AY3891x::Enable_Reg);
          psg.write(AY3891x::Enable_Reg, (MIXER_INPUTS_DISABLE | MIXER_NOISES_DISABLE | MIXER_TONE_B_DISABLE | MIXER_TONE_C_DISABLE) & value);
          break;
        case CHANB:
          sender->GetSerial()->println(F("Enabling Channel B tone generator."));
          value = psg.read(AY3891x::Enable_Reg);
          psg.write(AY3891x::Enable_Reg, (MIXER_INPUTS_DISABLE | MIXER_NOISES_DISABLE | MIXER_TONE_A_DISABLE | MIXER_TONE_C_DISABLE) & value);
          break;
        case CHANC:
          sender->GetSerial()->println(F("Enabling Channel C tone generator."));
          value = psg.read(AY3891x::Enable_Reg);
          psg.write(AY3891x::Enable_Reg, (MIXER_INPUTS_DISABLE | MIXER_NOISES_DISABLE | MIXER_TONE_A_DISABLE | MIXER_TONE_B_DISABLE) & value);
          break;
        default:
          cmd_unrecognized(sender, buffer);
          break;
      }
      break;
    case NOISE:
      switch (p2)
      {
        case CHANA:
          sender->GetSerial()->println(F("Enabling Channel A noise generator."));
          value = psg.read(AY3891x::Enable_Reg);
          psg.write(AY3891x::Enable_Reg, (MIXER_INPUTS_DISABLE | MIXER_TONES_DISABLE | MIXER_NOISE_B_DISABLE | MIXER_NOISE_C_DISABLE) & value);
          break;
        case CHANB:
          sender->GetSerial()->println(F("Enabling Channel B noise generator."));
          value = psg.read(AY3891x::Enable_Reg);
          psg.write(AY3891x::Enable_Reg, (MIXER_INPUTS_DISABLE | MIXER_TONES_DISABLE | MIXER_NOISE_A_DISABLE | MIXER_NOISE_C_DISABLE) & value);
          break;
        case CHANC:
          sender->GetSerial()->println(F("Enabling Channel C noise generator."));
          value = psg.read(AY3891x::Enable_Reg);
          psg.write(AY3891x::Enable_Reg, (MIXER_INPUTS_DISABLE | MIXER_TONES_DISABLE | MIXER_NOISE_A_DISABLE | MIXER_NOISE_B_DISABLE) & value);
          break;
        default:
          cmd_unrecognized(sender, buffer);
          break;
      }
      break;
    default:
      cmd_unrecognized(sender, buffer);
      break;
  }
}

void cmd_disable(SerialCommands* sender)
{
  // "disable"        -- disables all tone and noise generators
  byte value;

  sender->GetSerial()->println(F("Disabling all tones and noises."));

  value = psg.read(AY3891x::Enable_Reg);
  psg.write(AY3891x::Enable_Reg, MIXER_NOISES_DISABLE | MIXER_TONES_DISABLE | value);
}

void cmd_write(SerialCommands* sender)
{
  char* param1 = sender->Next();  // Register
  char* param2 = sender->Next();  // Value
  byte p1, p2;

  if (param1 == NULL) cmd_unrecognized(sender, buffer);
  else
  {
    p1 = atoi(param1);
    if (p1 > AY3891x::IO_Port_B_Reg) cmd_unrecognized(sender, buffer);
    else
    {
      if (param2 == NULL) cmd_unrecognized(sender, buffer);
      else
      {
        p2 = atoi(param2);
        sender->GetSerial()->print(F("Writing register 0x"));
        sender->GetSerial()->print(p1, HEX);
        sender->GetSerial()->print(F(" with value 0x"));
        sender->GetSerial()->println(p2, HEX);
        psg.write(p1, p2);
      }
    }
  }
}

void cmd_read(SerialCommands* sender)
{
  char* param1 = sender->Next();  // Register
  byte p1;
  if (param1 == NULL) cmd_unrecognized(sender, buffer);
  else
  {
    p1 = atoi(param1);
    if (p1 > AY3891x::IO_Port_B_Reg) cmd_unrecognized(sender, buffer);
    else
    {
      sender->GetSerial()->print(F("Reading value from register 0x"));
      sender->GetSerial()->print(p1, HEX);
      sender->GetSerial()->print(F(": 0x"));
      sender->GetSerial()->println(psg.read(p1), HEX);
    }
  }
}

void cmd_middlec(SerialCommands* sender)
{
  sender->GetSerial()->println("Enable Middle C on channels A and B");
  psg.write(AY3891x::Enable_Reg, MIXER_NOISES_DISABLE | MIXER_TONE_C_DISABLE);    // Disable the noise, enable tones on A and B
  psg.write(AY3891x::ChA_Amplitude, 0x04); // Lower amplitude
  psg.write(AY3891x::ChB_Amplitude, 0x08); // Mid amplitude
  psg.write(AY3891x::ChA_Tone_Period_Coarse_Reg, pgm_read_word(&Notes[C_4]) >> 8);
  psg.write(AY3891x::ChA_Tone_Period_Fine_Reg, pgm_read_word(&Notes[C_4]) & TONE_GENERATOR_FINE);
  psg.write(AY3891x::ChB_Tone_Period_Coarse_Reg, pgm_read_word(&Notes[C_4]) >> 8);
  psg.write(AY3891x::ChB_Tone_Period_Fine_Reg, pgm_read_word(&Notes[C_4]) & TONE_GENERATOR_FINE);
}

void cmd_gunshot(SerialCommands* sender)
{
  sender->GetSerial()->println("Playing gunshot sound on A, B, C");
  // Register programming values adopted from AY-3-8910 Data Manual, Secion 6.2
  psg.write(AY3891x::Noise_Period_Reg, 0xF); // Set Noise period to mid-value
  psg.write(AY3891x::Enable_Reg, MIXER_TONES_DISABLE); // Enable noise only on A,B,C
  psg.write(AY3891x::ChA_Amplitude, 0x10); // Set full amplitude range under direct control of Envelope Generator
  psg.write(AY3891x::ChB_Amplitude, 0x10);
  psg.write(AY3891x::ChC_Amplitude, 0x10);
  psg.write(AY3891x::Env_Period_Coarse_Reg, 4); // Set envlope period
  psg.write(AY3891x::Env_Period_Fine_Reg, 0);
  psg.write(AY3891x::Env_Shape_Cycle, 0);  // Envelope decay, one cycle only
}

void cmd_explosion(SerialCommands* sender)
{
  sender->GetSerial()->println("Playing explosion sound on A, B, C");
  // Register programming values adopted from AY-3-8910 Data Manual, Secion 6.2
  psg.write(AY3891x::Noise_Period_Reg, 0x0); // Set Noise period to max value
  psg.write(AY3891x::Enable_Reg, MIXER_TONES_DISABLE); // Enable noise only on A,B,C
  psg.write(AY3891x::ChA_Amplitude, 0x10); // Set full amplitude range under direct control of Envelope Generator
  psg.write(AY3891x::ChB_Amplitude, 0x10);
  psg.write(AY3891x::ChC_Amplitude, 0x10);
  psg.write(AY3891x::Env_Period_Coarse_Reg, 0x1c); // Set envlope period
  psg.write(AY3891x::Env_Period_Fine_Reg, 0);
  psg.write(AY3891x::Env_Shape_Cycle, 0);  // Envelope decay, one cycle only
}

SerialCommand cmd_enable_("enable", cmd_enable);
SerialCommand cmd_disable_("disable", cmd_disable);
SerialCommand cmd_write_("write", cmd_write);
SerialCommand cmd_read_("read", cmd_read);
SerialCommand cmd_middlec_("middlec", cmd_middlec);
SerialCommand cmd_gunshot_("gunshot", cmd_gunshot);
SerialCommand cmd_explosion_("explosion", cmd_explosion);

void setup()
{
#ifdef HARDWARE_GENERATED_CLOCK
  // Hardware-specific microcontroller code to generate a clock signal for the AY-3-891x chip
  pinMode(clkOUT, OUTPUT);
  digitalWrite(clkOUT, LOW);
  clockSetup();
#endif
	Serial.begin(38400);

	serial_commands_.SetDefaultHandler(cmd_unrecognized);
	serial_commands_.AddCommand(&cmd_enable_);
  serial_commands_.AddCommand(&cmd_disable_);
  serial_commands_.AddCommand(&cmd_write_);
  serial_commands_.AddCommand(&cmd_read_);
  serial_commands_.AddCommand(&cmd_middlec_);
  serial_commands_.AddCommand(&cmd_gunshot_);
  serial_commands_.AddCommand(&cmd_explosion_);

  psg.begin();
  // psg.setAddress(TheChipsAddress);   // Only need this for special-ordered chips with non-default address.
  // Set up a tone on channel A and B to make testing easier.
  // By default, the tone and noise generators are enabled when the chip comes out of reset, I/O pins are set to INPUT
  psg.write(AY3891x::Enable_Reg, MIXER_NOISES_DISABLE);    // Disable the noise
  psg.write(AY3891x::ChA_Amplitude, 0x04); // Lower amplitude
  psg.write(AY3891x::ChB_Amplitude, 0x08); // Mid amplitude
  Serial.print(F("Configuring note: C4 (middle C)"));
  psg.write(AY3891x::ChA_Tone_Period_Coarse_Reg, pgm_read_word(&Notes[C_4]) >> 8);
  psg.write(AY3891x::ChA_Tone_Period_Fine_Reg, pgm_read_word(&Notes[C_4]) & TONE_GENERATOR_FINE);
  psg.write(AY3891x::ChB_Tone_Period_Coarse_Reg, pgm_read_word(&Notes[C_4]) >> 8);
  psg.write(AY3891x::ChB_Tone_Period_Fine_Reg, pgm_read_word(&Notes[C_4]) & TONE_GENERATOR_FINE);

  Serial.println(F(""));
  Serial.println(F("AY-3-891x Sound Chip Library Example 5: Serial Commands."));
  Serial.println(F(""));
  Serial.println(F("Available commands:"));
  Serial.println(F("  enable all [tones|noises]"));
  Serial.println(F("  enable tone a|b|c"));
  Serial.println(F("  enable noise a|b|c"));
  Serial.println(F("  disable"));
  Serial.println(F("  write register value"));
  Serial.println(F("  read register"));
  Serial.println(F("  middlec"));
  Serial.println(F("  gunshot"));
  Serial.println(F("  explosion"));
  Serial.println(F("Enter command: "));
}

void loop()
{
	serial_commands_.ReadSerial();
}
