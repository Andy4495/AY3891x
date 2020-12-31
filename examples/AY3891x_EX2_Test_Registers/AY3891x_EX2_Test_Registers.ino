/* -----------------------------------------------------------------
   AY3891x Library Example Sketch 2: Test Registers
   https://github.com/Andy4495/AY3891x

   Write a value to each of the registers and read it back to see if
   it matches.

   These values do not necessarily program anything useful on the chip;
   it is just a test to see that all the registers work.

   12/21/20 - A.T. - Original

*/

#include "AY3891x.h"

// Be sure to use the correct pin numbers for your setup.
//          DA7, DA6, DA5, DA4, DA3, DA2, DA1, DA0, BDIR, BC2, BC1
AY3891x psg( A3,   8,   7,   6,   5,   4,   2,   3,   A2,  A1,  A0);

void setup() {
  byte value;
  Serial.begin(38400);
  Serial.println("");
  Serial.println("AY-3-891x Sound Chip Library Example 2: Test Regsiters.");

  psg.begin();
  // psg.setAddress(TheChipsAddress);   // Only need this for special-ordered with non-default address.

  Serial.println("");
  Serial.println("Note that registers 0x1, 0x3, 0x5, 0xD are only 4 bits wide, and");
  Serial.println("registers 0x6, 0x8, 0x9, 0xA are only 5 bits wide.");
  Serial.println("");

  for (byte i = AY3891x::ChA_Tone_Period_Fine_Reg; i <= AY3891x:: IO_Port_B_Reg; i++) {
    Serial.print("Write value 0x");
    Serial.print(0x50 | i, HEX);
    Serial.print(" to register:   0x");
    Serial.println(i, HEX);
    psg.write(i, 0x50 | i);
    value = psg.read(i);
    Serial.print("Read value  0x");
    Serial.print(value, HEX);
    Serial.print(" from register: 0x");
    Serial.println(i, HEX);
  }
}

void loop() {

}
