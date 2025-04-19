/* -----------------------------------------------------------------
   AY3891x Library Example Sketch 8: Check Orig or Clone
   https://github.com/Andy4495/AY3891x

   Check the chip type: is it the original AY-3-8910 or YM2149 (clone)

   This sketch writes decimal 31 (0x1f) to register 1 and reads it back:
     - If decimal 15 (0x0f) is returned, it is probably an AY-3-891x
     - If decimal 31 (0x1f) is returned, it is probably a YM2149 (or other clone)
   Note that these are the only two chips tested for; if you are checking
   another chip, then the results are even more unreliable.

   Based on method described in this blog: 
   http://blog.tynemouthsoftware.co.uk/2023/01/testing-ay-3-8910-ym2149-sound-card-for-rc2014-and-minstrel-4D.html
   which, per the above blog, is based on a suggestion from Ed Brindley (https://github.com/electrified)

   This may not be a reliable method to confidently confirm the chip type.

   2025-Apr-18 - Andy4495 - Original

*/

#include "AY3891x.h"

// Be sure to use the correct pin numbers for your setup.
// Note that on ATmega328: 17=A3, 16=A2, 15=A1, 14=A0
//          DA7, DA6, DA5, DA4, DA3, DA2, DA1, DA0, BDIR, BC2, BC1
AY3891x psg( 17,   8,   7,   6,   5,   4,   2,   3,   16,  15,  14);

void setup() {
  byte value;
  Serial.begin(38400);
  Serial.println("");
  Serial.println("AY-3-891x Sound Chip Library Example 8: Check Type.");

  psg.begin();
  // psg.setAddress(TheChipsAddress);   // Only need this for special-ordered with non-default address.

  Serial.println("");
  Serial.println("Writing 31 (0x1f) to register 1.");
  value = psg.writeThenRead(1, 0x1f);
  Serial.println("Reading back from register 1.");
  /// value = psg.read(1);
  if (value == 0x1f) {
    Serial.println("Register 1 contains 31. This is probably a YM2149.");
  } else {
    if (value == 0xf) {
      Serial.println("Register 1 contains 15. This is probably an AY-3-891x.");
    } else {
      Serial.print("Unexpected value read from register 1: "); 
      Serial.print(value);
      Serial.println(". Type of chip is unknown.");
    }
  }
}

void loop() {

}
