/* -----------------------------------------------------------------
   AY3891x Library
   https://github.com/Andy4495/AY3891x

   12/21/20 - A.T. - Original

*/

#ifndef AY3891x_H
#define AY3891x_H

#include "Arduino.h"

class AY3891x {
public:
  enum {NO_PIN = 255};

  enum {
         // Sound chip register addresses.
         // Registers that use <8 bits fill them lsb first
         ChA_Tone_Period_Fine_Reg   = 0,  // 8 bits
         ChA_Tone_Period_Coarse_Reg = 1,  // 4 bits
         ChB_Tone_Period_Fine_Reg   = 2,  // 8 bits
         ChB_Tone_Period_Coarse_Reg = 3,  // 4 bits
         ChC_Tone_Period_Fine_Reg   = 4,  // 8 bits
         ChC_Tone_Period_Coarse_Reg = 5,  // 4 bits
         Noise_Period_Reg           = 6,  // 5 bits
         Enable_Reg                 = 7,  // 8 bits
         ChA_Amplitude              = 8,  // 5 bits
         ChB_Amplitude              = 9,  // 5 bits
         ChC_Amplitude              = 10, // 5 bits
         Env_Period_Fine_Reg        = 11, // 8 bits
         Env_Period_Coarse_Reg      = 12, // 8 bits
         Env_Shape_Cycle            = 13, // 4 bits
         IO_Port_A_Reg              = 14, // 8 bits
         IO_Port_B_Reg              = 15  // 8 bits
      };

  // All pins connected to microcontroller
  AY3891x(byte  DA7,  byte DA6, byte DA5, byte DA4, byte DA3, byte DA2, byte DA1, byte DA0,
          byte  BDIR, byte BC2, byte BC1,
          byte  A9,   byte A8,
          byte  reset, byte clock);

  // Minimal pins connected to microcontroller
  // Assumes:
  // - BC2 tied HIGH
  // - A8 tied HIGH or left unconnected
  // - A9 tied LOW or left unconnected
  // - RESET tied high or to an external reset circuit
  // - CLOCK connected to an independent clock source
  AY3891x(byte  DA7,  byte DA6, byte DA5, byte DA4, byte DA3, byte DA2, byte DA1, byte DA0,
          byte  BDIR, byte BC1);

  // For other combinations of pin connections, use the "All pins" constructor
  // and use "AY3891::NO_PIN" for pins that are not tied to the microcontroller.

  void begin();
  void write(byte regAddr, byte data);
  byte read(byte regAddr);
  void resetChip();
  byte findChipAddress();
  void setChipAddress(byte address);
  byte getChipAddress();

private:
  enum {NUM_DA_LINES = 8};
  enum {INACTIVE, LATCH_ADDR, READ_DATA, WRITE_DATA};
  byte _DA_pin[NUM_DA_LINES];
  byte _BDIR_pin, _BC2_pin, _BC1_pin;
  byte _A9_pin, _A8_pin;
  byte _reset_pin, _clock_pin;
  byte _chipAddress; // This is typically 0b0000, but can be mask-programmed at factory for any 4-bit value.

  void daPinsInput();
  void daPinsOutput(byte data);
  void latchAddressMode(byte regAddr);
  void setMode(byte mode);
};

#endif
