/* -----------------------------------------------------------------
   AY3891x Library
   https://github.com/Andy4495/AY3891x

   12/21/20 - A.T. - Original
   29-Dec-2023 - Andy4495 - Release 2.0.0

*/

#ifndef AY3891x_H
#define AY3891x_H

#include "Arduino.h"
#include "AY3891x_register_defs.h"

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

  // Constructor for all pins
  // Use "AY3891::NO_PIN" for pins that are not tied to the microcontroller.
  // ****** IMPORTANT! *****
  // All three of the bus control signals (BDIR, BC1, BC2) need to be connected 
  // to the processor. 
  // Do not use AY3891x::NO_PIN in the constructor for these signals.
  // ****** IMPORTANT! *****
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
  // ****** IMPORTANT! *****
  // All three of the bus control signals (BDIR, BC1, BC2) need to be connected 
  // to the processor. 
  // Do not use AY3891x::NO_PIN in the constructor for these signals.
  // ****** IMPORTANT! *****
AY3891x(byte  DA7,  byte DA6, byte DA5, byte DA4, byte DA3, byte DA2, byte DA1, byte DA0,
          byte  BDIR, byte BC2, byte BC1);

  void begin();
  void write(byte regAddr, byte data);
  byte read(byte regAddr);
  byte writeThenRead(byte regAddr, byte data);
  void resetChip();
  byte findChipAddress();
  void setChipAddress(byte address);
  byte getChipAddress();

private:
  enum {NUM_DA_LINES = 8};
  enum {INACTIVE_000, INACTIVE_010, LATCH_ADDR, READ_DATA, WRITE_DATA};
  byte _DA_pin[NUM_DA_LINES];
  byte _BDIR_pin, _BC2_pin, _BC1_pin;
  byte _A9_pin, _A8_pin;
  byte _reset_pin, _clock_pin;
  byte _chipAddress; // This is typically 0b0000, but can be mask-programmed at factory for any 4-bit value.

  void daPinsInput();
  void daPinsOutput(byte data);
  void latchAddressMode(byte regAddr);
  inline void mode000to001();
  inline void mode001to000();
  inline void mode000to010();
  inline void mode010to000();
  inline void mode010to110();
  inline void mode110to010();
  inline void mode010to011();
  inline void mode011to010();
};

#endif
