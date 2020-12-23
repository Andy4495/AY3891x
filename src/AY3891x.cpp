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
  _clock_pin = clock; /// Controlling this pin is not supported at this time.
  _chipAddress = 0; // Assume address 0 unless manually changed
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
          _chipAddress = 0; // Assume address 0 unless manually changed
}

// For other combinations of pin connections, use the "All pins" constructor
// and use "AY3891::NO_PIN" for pins that are not tied to the microcontroller.

void AY3891x::begin() {

  // DA line configuration depends on whether we are setting address,
  // reading, or writing to the chip. So set the DA lines to INPUT mode
  // (high impedance) and let the specific read/write commands set the
  // proper pinModes.
  daPinsInput();

  if (_BDIR_pin  != NO_PIN) pinMode(_BDIR_pin,  OUTPUT);
  if (_BC2_pin   != NO_PIN) pinMode(_BC2_pin,   OUTPUT);
  if (_BC1_pin   != NO_PIN) pinMode(_BC1_pin,   OUTPUT);
  // Set up A8 and A9 to proper levels.
  // Pins have internal pullup (A8), or pulldown (A9) so need
  // to use OUTPUT mode only when pulling to opposite level.
  if (_A9_pin    != NO_PIN) pinMode(_BDIR_pin,  INPUT);
  if (_A8_pin    != NO_PIN) pinMode(_BDIR_pin,  INPUT);
  // Reset the chip so it is ready to program.
  resetChip();
  /// Clock is not currently supported, so configure it as high impedance
  if (_clock_pin != NO_PIN) pinMode(_clock_pin, INPUT);
}

void AY3891x::latchAddressMode(byte regAddr) {
  // Latch address steps:
  // 1. Set bus mode to INACTIVE
  // 2. Configure DA pins to pinMode OUTPUT
  // 3. Put register address on DA pins
  // 4. Set bus mode to LATCH_ADDR
  // 5. Set bus mode to INACTIVE
  // Leave DA pins in OUTPUT mode
  setMode(INACTIVE);
  daPinsOutput(_chipAddress | regAddr); // Register address is 4 lsb
  setMode(LATCH_ADDR);
  // delay is not needed here because code is slow enough (300 ns)
  setMode(INACTIVE);

}

void AY3891x::write(byte regAddr, byte data) {
  // Write to chip register:
  // 1. Latch the register address
  // 2. Set DA pins to output
  // 3. Set bits on DA pins
  // 4. Set bus mode to WRITE_DATA
  // 5. delay 1800 ns (3 us)
  // 6. Set BC to INACTIVE
  // 7. Set BA pins to pinMode INPUT (high impedance)
  latchAddressMode(regAddr);
  daPinsOutput(data);
  setMode(WRITE_DATA);
  delayMicroseconds(3);
  setMode(INACTIVE);
  daPinsInput();
}

byte AY3891x::read(byte regAddr) {
  // Read from chip register:
  // 1. Latch the address register
  // 2. Set DA pins to input mode
  // 3. Set bus mode to READ_DATA
  // 4. Read the data on DA pins
  // 5. Set bus mode to INACTIVE
  byte returnData = 0;

  latchAddressMode(regAddr);
  daPinsInput();
  setMode(READ_DATA);
  for (byte i = 0; i < NUM_DA_LINES; i++) {
    returnData = returnData | (digitalRead(_DA_pin[i]) << i);
  }
  setMode(INACTIVE);
  return returnData;
}

void AY3891x::daPinsInput() {
  byte i;

  for (i = 0; i < NUM_DA_LINES; i++) {
    if (_DA_pin[i] != NO_PIN) pinMode(_DA_pin[i], INPUT);
  }
}

void AY3891x::daPinsOutput(byte data) {
  byte i;

  for (i = 0; i < NUM_DA_LINES; i++) {
    if (_DA_pin[i] != NO_PIN) pinMode(_DA_pin[i], OUTPUT);
  }

  for (i = 0; i < NUM_DA_LINES; i++) {
    if (_DA_pin[i] != NO_PIN) {
      digitalWrite(_DA_pin[i], data & 0x01);
      data = data >> 1;
    }
  }
}


void AY3891x::resetChip() {
  if (_reset_pin != NO_PIN) {
    pinMode(_reset_pin, INPUT);
    digitalWrite(_reset_pin, LOW);
    pinMode(_reset_pin, OUTPUT);
    delayMicroseconds(6);        // From datasheet: tRW 5us (min)
    pinMode(_reset_pin, INPUT);
  }
}

// Since it is possible to order the AY-3-891x chip with a mask programmed
// address other than 0, include a function to try all 16 possible setAddresses
// and return the valied address.
byte AY3891x::findChipAddress() {
  byte testAddress = 0x00;
  byte readValue;
  byte addressFound = 0xFF;  // 0xFF indicates address check failed.
  byte prevAddress;

  prevAddress = getChipAddress();

  for(byte i = 0; i < 0x10; i++) {
    setChipAddress(testAddress);
    write(Env_Period_Fine_Reg, 0x7A);
    readValue = read(Env_Period_Fine_Reg);
    if (readValue == 0x7A) {
      addressFound = 0;
      break;
    }
    testAddress = testAddress + 0x10;
  }

  if (addressFound != 0xFF) addressFound = testAddress;

  setChipAddress(prevAddress);

  return addressFound;
}

void AY3891x::setChipAddress(byte address) {
  // Note that only the 4 msb are used for the address.
  // The lower 4 bits are used to select the register.
  _chipAddress = address & 0xF0;
}

byte AY3891x::getChipAddress() {
  return _chipAddress;
}

void AY3891x::setMode(byte mode) {
  switch (mode) {
    case INACTIVE:   // 0b010
      if (_BDIR_pin != NO_PIN) digitalWrite(_BDIR_pin, LOW);
      if (_BC2_pin  != NO_PIN) digitalWrite(_BC2_pin,  HIGH);
      if (_BC1_pin  != NO_PIN) digitalWrite(_BC1_pin,  LOW);
      break;

    case LATCH_ADDR: // 0b111
      if (_BDIR_pin != NO_PIN) digitalWrite(_BDIR_pin, HIGH);
      if (_BC2_pin  != NO_PIN) digitalWrite(_BC2_pin,  HIGH);
      if (_BC1_pin  != NO_PIN) digitalWrite(_BC1_pin,  HIGH);
      break;

    case READ_DATA:  // 0b011
      if (_BDIR_pin != NO_PIN) digitalWrite(_BDIR_pin, LOW);
      if (_BC2_pin  != NO_PIN) digitalWrite(_BC2_pin,  HIGH);
      if (_BC1_pin  != NO_PIN) digitalWrite(_BC1_pin,  HIGH);
      break;

    case WRITE_DATA: // 0b110
      if (_BDIR_pin != NO_PIN) digitalWrite(_BDIR_pin, HIGH);
      if (_BC2_pin  != NO_PIN) digitalWrite(_BC2_pin,  HIGH);
      if (_BC1_pin  != NO_PIN) digitalWrite(_BC1_pin,  LOW);
      break;

    default:
      // Set pins the same as INACTIVE mode 0b010
      if (_BDIR_pin != NO_PIN) digitalWrite(_BDIR_pin, LOW);
      if (_BC2_pin  != NO_PIN) digitalWrite(_BC2_pin,  HIGH);
      if (_BC1_pin  != NO_PIN) digitalWrite(_BC1_pin,  LOW);
      break;
  }
}
