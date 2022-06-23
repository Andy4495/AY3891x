# AY3891x Library

[![Arduino Compile Sketches](https://github.com/Andy4495/AY3891x/actions/workflows/arduino-compile-sketches.yml/badge.svg)](https://github.com/Andy4495/AY3891x/actions/workflows/arduino-compile-sketches.yml)
[![Check Markdown Links](https://github.com/Andy4495/AY3891x/actions/workflows/CheckMarkdownLinks.yml/badge.svg)](https://github.com/Andy4495/AY3891x/actions/workflows/CheckMarkdownLinks.yml)

Arduino library and chiptunes player for General Instrument AY-3-8910 / AY-3-8912 Programmable Sound Generator (PSG) chip, including clones like the YM2149.

The AY-3-8913 variant has a different programming interface which uses a Chip Select signal. *The 8913 variant is not currently supported by the library. A future iteration of this library may include support.*

The AY-3-8910 and AY-3-8912 have the same programming interface. The 8910 has two 8-bit I/O ports and the 8912 has a single 8-bit I/O port (and correspondingly fewer pins). These general purpose I/O pins are not related to the sound generation functions. This library has only been tested with the 8910 variant of the chip, but should also work with the 8912.

Note that other libraries and PSG-related projects typically discuss the need to use fast pin switching in order to meet the strict signal timing requirements when using the PSG, meaning that they don't use the platform-agnostic `digitalWrite()` function.  This ends up creating non-portable, processor specific code. While the PSG has tight timing requirements, it is possible to still use `digitalWrite()` by cycling through an extra state when reading and writing the chip registers. This library uses the generic `digitalWrite()` function instead of direct port manipulation, and should therefore work across most, if not all, processors supported by the Arduino and Energia IDEs, so long as enough I/O pins are available for the interface to the PSG.

There are several [variants][3] of the chip from other manufacturers, examples of which are the Yamaha YM2149F and YM3439 and Toshiba T7766A. These variants are probably compatible with this library but have not been tested.

I created an Arduino-AY38910 chiptunes player board, see the [README][9] in the `extras/hardware` folder.

## Usage

*Refer to the sketches in the `examples` folder.*

1. Include the library header file:  

    ```C++
    #include "AY3891x.h"
    ```

2. (optional) Include definitions for eight octaves of musical notes:

    ```C++
    #include "AY3891x_sounds.h" 
    ```

    It is not strictly necessary to use this header file, but you can include it to make playing notes a little easier. If you aren't using it, then it will save a little flash memory by not including the file. It is used by example programs 3 and 5.

    Note that as of version 1.1.0 of the library, the `Notes[]` array in AY3891x_sounds.h is defined as `PROGMEM`. This will require minor changes to any sketches which include this file and use the `Notes[]` array. See example programs 3 and 5.

3. Instantiate the object. There are two forms of the constructor. The first one supports all of the chip's interface pins. The second one allows for a slightly simplified format when using only the minimal pins necessary to interface with the chip.

    ```C++
    AY3891x(byte DA7, byte DA6, byte DA5, byte DA4, 
            byte DA3, byte DA2, byte DA1, byte DA0,
            byte BDIR, byte BC2, byte BC1,
            byte A9, byte A8,
            byte reset, byte clock);
    ```

    ```C++
    AY3891x(byte DA7, byte DA6, byte DA5, byte DA4, 
            byte DA3, byte DA2, byte DA1, byte DA0,
            byte BDIR, byte BC2, byte BC1);
    ```

    ```C++
    // Example constructor using the simpler form
    AY3891x psg(4, 5, 6, 7, 8, 10, 11, 12, 2, A5, 3);
    ```

    Use the enumeration `AY3891::NO_PIN` if any of the pins required by the constructor are not connected.

4. Initialize the object.

    ```C++
    psg.begin();
    ```

5. Use the read() and write() methods to program the chip:

    ```C++
    psg.write(AY3891x::Enable_Reg, 0x3F);  
    psg.read(AY3891x::IO_Port_A_Reg;
    ```

6. See the library source code and example sketches for other available methods and usage.

## Example Sketches

**EX1 - Find Address**  
The AY-3-8910 chip typically has a binary address of `01 0000 xxxx`,
where `xxxx` represents the specific register you are addressing.
It is possible to request a different mask-programmed address from the factory.
This example code determines the chip's address by writing a value
to one of the chip's registers and reads it back to see if it matches.
If it matches, then we found the address. If not, then the next address
is checked.

**EX2 - Test Registers**  
Writes a value to each of the chip's registers and reads it back to see if it matches.

**EX3 - Simple Tone**  
This sketch will play through the middle-octave notes C4 to C5.
Note that an external clock is required for the chip's audio functions to work. This can be [generated by the microcontroller][2] or other hardware clock circuit.

**EX4 - I/O Ports**  
This sketch demonstrates the use of the I/O ports on the AY-3-8910 or -8912.

**EX5 - Serial Commands**  
This sketch allows you to send commands to the AY-3-891x chip through the serial port. It makes it easy to experiment with the various sound generation capabilities of the chip. This example depends on the [SerialCommands][10] library.

**EX6 - Chiptunes Flash**  
This examples plays a converted YM file which is compiled into the program as a byte array of the data values to write to the 14 audio
registers on the sound chip.

Since the audio data is compiled into the program, there is a relatively small limit on the duration of the chiptune to be played. The sketch itself takes 1632 bytes, leaving about 29,000 bytes on an Atmega328. Fourteen bytes of data are written 50 times a second, meaning that the largest data array can store about 41 seconds of music.

See this [README][6] for details on finding and converting YM files for use with this sketch.

**EX7 - Chiptunes SD**  
This example sketch plays YM files which are stored on an SD card. The sketch sequentially goes through each file in the root directory. Press the button defined as `NEXT_BUTTON` to advance to the next song on the card.

See this [README][6] for details on finding and converting YM files for use with this sketch.

## References

+ AY-3-891x [datasheet][1]
+ Info from the Synth DIY [wiki][4]
+ [Summary][5] of the registers (simplified info from datasheet)
+ YMduino [code][8] which provided inspiration for chiptunes example sketches 6 and 7.

## License

The software and other files in this repository are released under what is commonly called the [MIT License][100]. See the file [`LICENSE.txt`][101] in this repository.

[1]: http://map.grauw.nl/resources/sound/generalinstrument_ay-3-8910.pdf
[2]: https://www.instructables.com/Arduino-MIDI-Chiptune-Synthesizer/
[3]: https://en.wikipedia.org/wiki/General_Instrument_AY-3-8910#Variants
[4]: https://sdiy.info/wiki/General_Instrument_AY-3-8910
[5]: ./Register-Summary.md
[6]: ./extras/tools/README.md
[8]: https://github.com/eqy/ymduino
[9]: ./extras/hardware/README.md
[10]: https://github.com/ppedro74/Arduino-SerialCommands
[100]: https://choosealicense.com/licenses/mit/
[101]: ./LICENSE.txt
[200]: https://github.com/Andy4495/AY3891x

[//]: # (Dead link from older version of README: ymduino project page https://homes.cs.washington.edu/~eqy/ymduino.html)
