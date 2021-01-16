# AY-3-891x Register Summary
Register numbers "Rn" are in Octal in datasheet.

#### Registers R0 - R5: Tone Generator Control  
Register R0  - 0x0 - Channel A Tone Fine  
Register R1  - 0x1 - Channel A Tone Coarse (lower 5 bits)  
Register R2  - 0x2 - Channel B Tone Fine  
Register R3  - 0x3 - Channel B Tone Coarse (lower 5 bits)  
Register R4  - 0x4 - Channel C Tone Fine  
Register R5  - 0x5 - Channel C Tone Coarse (lower 5 bits)  
Clock is divided by 16, and then further divided by 12-bit Tone Course/Fine value to create tone frequency.  
With 1 MHz clock, a tone divider of 239 (0xE7) will generate middle C (C_4).

#### Register R6 - 0x6: Noise Generator Control (lower 5 bits)
Clock is divided by 16, then further divided by the 5-bit value in R6.  
Note that all values, including zero, produce noise if the noise channel is enabled.

#### Register R7 - 0x7: Mixer Control, I/O Direction
Audio control is active LOW; I/O LOW is INPUT mode.  
Coming out of chip reset, noise and tones are enabled and I/O is set to INPUT mode.  
```
b7 - Port B Output/Input*  
b6 - Port A Output/Input*  
b5 - Noise Channel C*  
b4 - Noise Channel B*  
b3 - Noise Channel A*  
b2 - Tone Channel C*  
b1 - Tone Channel B*  
b0 - Tone Channel A*  
```

#### Register R10 - R12: Amplitude Control
Register R10 - 0x8 - Channel A  
Register R11 - 0x9 - Channel B  
Register R12 - 0xA - Channel C  
```
b7-b5 not used
b4 - 1 = envelope mode, 0 = use b3-b0 for constant amplitude level
```

#### Registers R13 - R14: Envelope Period Control
Register R13 - 0xB - Course (upper bits)  
Register R14 - 0xC - Fine (lower bits)  
Clock is divided by 256, then divided by 16 bit value in R13-R14.  

#### Register R15 - 0xD:- Envelope Shape Control
```
b7 - b4 not used
b3 - Continue
b2 - Attack
b1 - Alternate
b0 - Hold
```
```
b00xx - \___________
b01xx - /|__________
b1000 - \|\|\|\|\|\|
b1001 - \___________ (appears to be same effect as b00xx)
b1010 - \/\/\/\/\/\/
b1011 - \|TTTTTTTTTT (T represents full output level)
b1100 - /|/|/|/|/|/|
b1101 - /TTTTTTTTTTT (T represents full output level)
b1110 - /\/\/\/\/\/\ (similar to b1010, but opposite order)
b1111 - /|__________ (appears to be same effect as b01xx)
```

#### Registers R16 - R17: I/O Port Data
Register R16 - 0xE - Port A  
Register R17 - 0xF - Port B  
