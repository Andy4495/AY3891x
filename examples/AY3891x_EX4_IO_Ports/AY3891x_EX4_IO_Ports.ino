/* -----------------------------------------------------------------
   AY3891x Library Example Sketch 4: I/O Ports
   https://github.com/Andy4495/AY3891x

   This example sketch demonstrates the use of the I/O ports
   on the AY-3-891X. Note that the AY-3-8910 has two 8-bit I/O
   ports, the AY-3-8912 has one 8-bit I/O port, and the AY-3-8913
   does not have any I/O ports.

   12/21/26 - A.T. - Original

*/

#include "AY3891x.h"

// Be sure to use the correct pin numbers for your setup.
//          DA7, DA6, DA5, DA4, DA3, DA2, DA1, DA0, BDIR, BC2, BC1
AY3891x psg( A3,   8,   7,   6,   5,   4,   2,   3,   A2,  A1,  A0);

void setup() {

  Serial.begin(38400);
  Serial.println("");
  Serial.println("AY-3-891x Sound Chip Library Example 4: I/O Ports.");

  psg.begin();
  // psg.setAddress(TheChipsAddress);   // Only need this for special-ordered chips with non-default address.

  // By default, the I/O ports are set up as inputs.
  // There are internal pull-ups connected to the input ports, so if the pins
  // are left floating, they should read as zero when set to input.
  psg.write(AY3891x::Enable_Reg, 0x3F);  // Disable audio, I/O ports to input mode
  Serial.println("Input Mode has internal pullups.");
  Serial.print("Input Port A read value: 0x");
  Serial.println(psg.read(AY3891x::IO_Port_A_Reg), HEX);
  Serial.print("Input Port B read value: 0x");
  Serial.println(psg.read(AY3891x::IO_Port_B_Reg), HEX);
  delay(1000);

  // Set lines to output with value 0xA5 on Port A and 0x5A on Port B
  // Measure with a DMM or scope to test
  Serial.println("Setting ports to OUTPUT...");
  delay(1000);
  psg.write(AY3891x::Enable_Reg, 0xFF); // Ports to output mode, audio disabled
  Serial.println("Writing 0xA5 to A and 0x5A to B.");
  psg.write(AY3891x::IO_Port_A_Reg, 0xA5);
  psg.write(AY3891x::IO_Port_B_Reg, 0x5A);
}

void loop() {

}
