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
  void write(uint16_t address, byte data);
  byte read(uint16_t address);
  void reset();
  byte findAddress();

private:
  enum {NUM_DA_LINES = 8};
  byte _DA_pin[NUM_DA_LINES];
  byte _BDIR_pin, _BC2_pin, _BC1_pin;
  byte _A9_pin, _A8_pin;
  byte _reset_pin, _clock_pin;

  void setAddressLines(byte address);
};

#endif
