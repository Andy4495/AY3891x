/* -----------------------------------------------------------------
   AY3891x Library
   https://github.com/Andy4495/AY3891x

   12/21/20 - A.T. - Original

*/

#include "AY3891x.h"

// All pins connected to microcontroller
AY3891x::AY3891x(byte  DA7,  byte DA6, byte DA5, byte DA4, byte DA3, byte DA2, byte DA1, byte DA0,
        byte  BDIR, byte BC2, byte BC1,
        byte  A9,   byte A8,
        byte  reset, byte clock) {

  _DA_pin[7] = DA7;
  _DA_pin[6] = DA6;
  _DA_pin[5] = DA5;
  _DA_pin[4] = DA4;
  _DA_pin[3] = DA3;
  _DA_pin[2] = DA2;
  _DA_pin[1] = DA1;
  _DA_pin[0] = DA0;
  _BDIR_pin  = BDIR;
  _BC2_pin   = BC2;
  _BC1_pin   = BC1;
  _A9_pin    = A9;
  _A8_pin    = A8;
  _reset_pin = reset;
  _clock_pin = NO_PIN; /// Controlling this pin is not supported at this time.
}

// Minimal pins connected to microcontroller
// Assumes:
// - BC2 tied HIGH
// - A8 tied HIGH or left unconnected
// - A9 tied LOW or left unconnected
// - RESET tied high or to an external reset circuit
// - CLOCK connected to an independent clock source
AY3891x::AY3891x(byte  DA7,  byte DA6, byte DA5, byte DA4, byte DA3, byte DA2, byte DA1, byte DA0,
        byte  BDIR, byte BC1) {

          _DA_pin[7] = DA7;
          _DA_pin[6] = DA6;
          _DA_pin[5] = DA5;
          _DA_pin[4] = DA4;
          _DA_pin[3] = DA3;
          _DA_pin[2] = DA2;
          _DA_pin[1] = DA1;
          _DA_pin[0] = DA0;
          _BDIR_pin  = BDIR;
          _BC2_pin   = NO_PIN;
          _BC1_pin   = BC1;
          _A9_pin    = NO_PIN;
          _A8_pin    = NO_PIN;
          _reset_pin = NO_PIN;
          _clock_pin = NO_PIN;

}

// For other combinations of pin connections, use the "All pins" constructor
// and use "AY3891::NO_PIN" for pins that are not tied to the microcontroller.

void AY3891x::begin() {
  byte i;

  /// Read datasheet: confirm whether DA lines should be output mode in all
  /// cases except when reading a value, or Hi-Z (input mode) except when
  /// writing. (Since these are address lines without a separate chip select,
  /// it seems like these would need to be output always to avoid floating
  /// values).
  /// .... DA Line setup here ...

  if (_BDIR_pin  != NO_PIN) pinMode(_BDIR_pin,  OUTPUT);
  if (_BC2_pin   != NO_PIN) pinMode(_BC2_pin,   OUTPUT);
  if (_BC1_pin   != NO_PIN) pinMode(_BC1_pin,   OUTPUT);
  /// Need to set up A8 and A9 to proper levels.
  /// Pins have internal pullup (A8), or pulldown (A9) so need
  /// to use OUTPUT mode only when pulling to opposite level.
  /// Also, may need to use these as a sort of chip-select
  if (_A9_pin    != NO_PIN) pinMode(_BDIR_pin,  OUTPUT);
  if (_A8_pin    != NO_PIN) pinMode(_BDIR_pin,  OUTPUT);
  /// Need to add a reset pin sequence and then let chips input pullup hold it high.
  if (_reset_pin != NO_PIN) pinMode(_reset_pin, OUTPUT);
  /// Clock is not supported, so this will always be set to NO_PIN
  if (_clock_pin != NO_PIN) pinMode(_clock_pin, OUTPUT);
}

void AY3891x::write(uint16_t address, byte data) {
/// Define the vaarious register names in header file
}

byte AY3891x::read(uint16_t address) {

}

void AY3891x::setAddressLines(byte address) {
  byte i;

  for (i = 0; i < NUM_DA_LINES; i++) {
    if (_DA_pin[i] != NO_PIN) pinMode(_DA_pin[i], OUTPUT);
  }
}

void AY3891x::reset() {
  if (_reset_pin != NO_PIN) {
    digitalWrite(_reset_pin, LOW);
    pinMode(_reset_pin, OUTPUT);
    delayMicroseconds(6);        // From datasheet: tRW 5us (min)
    pinMode(_reset_pin, INPUT);
  }
}

// Since it is possible to order the AY-3-891x chip with a mask programmed
// address other than 0, include a function to try all 16 possible setAddresses
// and return the valied address.
byte AY3891x::findAddress() {
  /// Imiplement address check here, and use this in one of the example sketches.
  return 0; 
}
