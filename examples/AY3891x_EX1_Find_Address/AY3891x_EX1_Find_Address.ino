/* -----------------------------------------------------------------
   AY3891x Library Example Sketch 1: Find Chip Address
   https://github.com/Andy4495/AY3891x

   The AY-3-8910 chip typically has a binary address of 01 0000 xxxx,
   where xxxx represents the specific register you are addressing.
   It is possible to request a different mask-programmed address.
   This example code determines the chip's address by writing a value
   to one of the chips registers and reads it back to see if it matches.
   If it matches, then we found the address. If not, then the next address
   is checked.

   This sketch can also be used to see if a chip is connected properly
   and functioning. If no address is found, then there may be a problem
   with the chip itself or the connection to the microcontroller.

   12/21/26 - A.T. - Original

*/

#include "AY3891x.h"

AY3891x psg(4, 5, 6, 7, 8, 10, 11, 12, 2, A5, 3);

void setup() {
  byte value;
  Serial.begin(38400);
  Serial.println("");
  Serial.println("AY-3-891x Sound Chip Library Example 1: Find Chip Address.");

  psg.begin();

  // Find chip address
  byte foundAddress;
  foundAddress = psg.findChipAddress();
  Serial.print("Default Chip Address: ");
  Serial.println(psg.getChipAddress(), HEX);

  if (foundAddress == 0xFF) {
    Serial.println("ERROR: Unable to detect chip address, check hardware.");
  }
  else {
    Serial.print("Detected Chip Address: ");
    Serial.println(foundAddress, HEX);
    delay(500);

    psg.setChipAddress(foundAddress);
    Serial.println("Testing chip by writing to a register.");
    psg.write(AY3891x::ChA_Tone_Period_Fine_Reg, 0x5A);
    Serial.println("Write vale to Tone Period Fine register: 0x5A");
    delay(500);
    value = psg.read(AY3891x::ChA_Tone_Period_Fine_Reg);
    Serial.print("Read value from Tone Period Fine register: 0x");
    Serial.println(value, HEX);
    delay(500);
  }
}

void loop() {

}
