/* -----------------------------------------------------------------
   AY3891x Library
   https://github.com/Andy4495/AY3891x

   12/21/20 - A.T. - Original
   29-Dec-2023 - Andy4495 - Release 2.0.0

   The AY3891x chip uses an unconventional microcontroller interfacing
   method. Instead of using a chip select signal, the chip requires
   specific timing and sequencing on the bus control signals (BDIR, BC2, BC1).
   In particular, the skew between the bus control signals is required
   to be less than 50ns when changing states between INACTIVE, LATCH_ADDRESS,
   READ, and WRITE (referred with symbol "tBD" and named "Associative Time
   Delay" in the datasheet). The built-in, platform-agnostic digitalWrite()
   function is much too slow to meet this timing spec when changing more than
   one pin at a time. So, to keep this library code as generic as
   possible, it is therefore required to connect all three bus
   control signals to the microcontroller, even though the AY3891
   datasheet mentions that it is possible to tie BC2 high and control
   the states with just BDIR and BC1.

   By using all three signals, it is possible to switch between the states
   by only changing one bus control signal at a time between states. This
   meets the timing constraints regardless of how long digitalWrite() actually
   takes to run, since only one signal is changing for each state change.

   So the pin configurations for each state change is as follows, with the
   pins ordered BDIR, BC2, BC1 left to right:

   LATCH_ADDRESS:
   000 (INACTIVE)-> 001 -> 000 (INACTIVE)
   READ:
   000 (INACTIVE) -> 010 (INACTIVE) -> 011 -> 010 (INACTIVE) -> (INACTIVE)
   WRITE:
   000 (INACTIVE) -> 010 (INACTIVE) -> 110 -> 010 (INACTIVE) -> (INACTIVE)

   Using two diffent INACTIVE state pin configurations allows the Library
   to change a single pin at a time between states. So while this requires
   an extra I/O pin to be used, the code is simpler and more generic.

   /// Note, the AY38913 chip variation has a chip-select signal which
       is not currently supported by this library.

*/

#include "AY3891x.h"

// Constructor with all possible pins
// ****** IMPORTANT! *****
// All three of the bus control signals (BDIR, BC1, BC2) need to be connected 
// to the processor. 
// Do not use AY3891x::NO_PIN in the constructor for these signals.
// ****** IMPORTANT! *****
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

// Constructor for minimal pins connected to microcontroller
// Assumes:
// - A8 tied HIGH or left unconnected
// - A9 tied LOW or left unconnected
// - RESET tied high or to an external reset circuit
// - CLOCK connected to an independent clock source
// ****** IMPORTANT! *****
// All three of the bus control signals (BDIR, BC1, BC2) need to be connected 
// to the processor. 
// Do not use AY3891x::NO_PIN in the constructor for these signals.
// ****** IMPORTANT! *****
AY3891x::AY3891x(byte  DA7,  byte DA6, byte DA5, byte DA4, byte DA3, byte DA2, byte DA1, byte DA0,
        byte  BDIR, byte BC2, byte BC1) {

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
          _A9_pin    = NO_PIN;
          _A8_pin    = NO_PIN;
          _reset_pin = NO_PIN;
          _clock_pin = NO_PIN;
          _chipAddress = 0; // Assume address 0 unless manually changed
}

// ****** IMPORTANT! *****
// For other combinations of pin connections, use the "All pins" constructor
// and use "AY3891::NO_PIN" for pins that are not tied to the microcontroller.
// All three of the bus control signals (BDIR, BC1, BC2) need to be connected 
// to the processor. 
// Do not use AY3891x::NO_PIN in the constructor for these signals.
// ****** IMPORTANT! *****


void AY3891x::begin() {

  // DA line configuration depends on whether we are latching the address,
  // reading, or writing to the chip. So set the DA lines to INPUT mode
  // (high impedance) and let the specific read/write/latch commands set the
  // proper pinModes.
  daPinsInput();

  // Set up the bus control signals. These are always outputs.
  if (_BDIR_pin  != NO_PIN) pinMode(_BDIR_pin,  OUTPUT);
  if (_BC2_pin   != NO_PIN) pinMode(_BC2_pin,   OUTPUT);
  if (_BC1_pin   != NO_PIN) pinMode(_BC1_pin,   OUTPUT);
  //setMode(INACTIVE_010);
  // Need to do this one manually since we are in "unknown" state at this point
  digitalWrite(_BDIR_pin, LOW);
  digitalWrite(_BC2_pin,  HIGH);
  digitalWrite(_BC1_pin,  LOW);

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

  ///setMode(INACTIVE_000);
  mode010to000();
  daPinsOutput(_chipAddress | regAddr); // Register address is 4 lsb
  // setMode(LATCH_ADDR);
  mode000to001();
  // delay is not needed here because code is slow enough (300 ns min per datasheet)
  // setMode(INACTIVE_000);
  mode001to000();
  // setMode(INACTIVE_010); // default state is 010
  mode000to010();
}

void AY3891x::write(byte regAddr, byte data) {
  // Write to chip register:
  // 1. Latch the register address
  // 2. Set DA pins to output
  // 3. Set bits on DA pins
  // 4. Set bus mode to WRITE_DATA
  // 5. Set BC to INACTIVE
  // 6. Set BA pins to pinMode INPUT (high impedance)
  latchAddressMode(regAddr);
  daPinsOutput(data);
  // The Write Data Pulse Width tDW has a max time of 10 us per some datasheets
  // tDW = time that WRITE_DATA mode is enabled before going back to INACTIVE
  //setMode(WRITE_DATA);
  noInterrupts();
  mode010to110();
  //setMode(INACTIVE_010);
  mode110to010();
  interrupts();
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
  ///setMode(READ_DATA);
  mode010to011();
  for (byte i = 0; i < NUM_DA_LINES; i++) {
    returnData = returnData | (digitalRead(_DA_pin[i]) << i);
  }
  ///setMode(INACTIVE_010);
  mode011to010();
  return returnData;
}

byte AY3891x::writeThenRead(byte regAddr, byte data) {
  // This method writes a value and then immediately reads it back
  // without latching the address again.
  byte returnData = 0;
  latchAddressMode(regAddr);
  daPinsOutput(data);
  // The Write Data Pulse Width tDW has a max time of 10 us per some datasheets
  // tDW = time that WRITE_DATA mode is enabled before going back to INACTIVE
  //setMode(WRITE_DATA);
  noInterrupts();
  mode010to110();
  //setMode(INACTIVE_010);
  mode110to010();
  daPinsInput();
  mode010to011();
  for (byte i = 0; i < NUM_DA_LINES; i++) {
    returnData = returnData | (digitalRead(_DA_pin[i]) << i);
  }
  ///setMode(INACTIVE_010);
  mode011to010();
  interrupts();
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

// For the following mode change calls, the three
// binary digits represent the following signals:
//   BDIR BC2 BC1

// INACTIVE to LATCH_ADDR
void AY3891x::mode000to001() {
  digitalWrite(_BC1_pin,  HIGH);
}

// LATCH_ADDR to INACTIVE
void AY3891x::mode001to000() {
  digitalWrite(_BC1_pin,  LOW);
}

// INACTIVE to INACTIVE
void AY3891x::mode000to010() {
  digitalWrite(_BC2_pin,  HIGH);
}

// INACTIVE to INACTIVE
void AY3891x::mode010to000() {
  digitalWrite(_BC2_pin,  LOW);
}

// INACTIVE to WRITE_DATA
void AY3891x::mode010to110() {
  digitalWrite(_BDIR_pin,  HIGH);
}

// WRITE_DATA to INACTIVE
void AY3891x::mode110to010() {
  digitalWrite(_BDIR_pin,  LOW);
}

// INACTIVE to READ_DATA
void AY3891x::mode010to011() {
  digitalWrite(_BC1_pin,  HIGH);
}

// READ_DATA to INACTIVE
void AY3891x::mode011to010() {
  digitalWrite(_BC1_pin,  LOW);
}
