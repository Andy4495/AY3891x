Finding and Converting YM Files for the Chiptunes Examples
==========================================================

The AY3891x [library][7] includes two example sketches ([EX6][11], [EX7][12]) that play complex [chiptunes][8] on the AY-3-8910  [Programmable Sound Generator][10] (PSG) and its clones and variants.

These sketches in particular are designed to work with "YM Files". [YM Files][9] are an audio format that lists the data sent to the 14 audio programming registers in the supported PSG chips. The data in the file is typically meant to be written at a rate of 50 Hz. At this rate, it takes 700 bytes of data for each second of audio.

This library does not include any YM files because I could not find any clear licensing and/or copyright information that would allow me to redistribute the files. This README will therefore explain where to find the files and the conversion process to make them usable by the example scripts.

1. **Find YM Files**  
  [This Github repo][6] (see the `data` directory) and [this web site][5] contain a large number of YM files. Others can be found with your favorite search engine. [This web page][4] contains a couple already-converted YM files (and also includes a more brief explanation of converting a raw, compressed YM file to a usable file).  

  Note that one additional step ("Convert to Header File") is required to get converted files to work with [Example 6][11] in this library.

2. **Decompress**  
  Original YM files are typically stored in an LHA-compressed format. LHA files have the ascii text `-lh5-` embedded in the 3rd through 7th bytes of the file (use a binary file reader to see this). The numeral (e.g. `5`) represents the compression method and can differ from file to file. There may be LHA decompressors for Windows, but I used the "lhasa" package on Linux. I ran the decompressor in a VirtualBox VM on Windows, but it should also work using WSL.
```
$ lha xw=./extract-directory filename.ym
```
  The output of the lha command will tell you the name and location of your "melted" (extracted) file. Note that the extracted file name may be different than the compressed file name.

3. **Interleave**  
  While the uncompressed file is binary, it will have `YM5!LeOnArD!` along with some other descriptive text in ASCII in the first bytes of the file (use a binary file reader to see this). This uncompressed binary file has the audio programming registers stored in a non-interleaved format for more efficient compression. These values now need to be interleaved for more efficient playback.  

  There are two different tools available to interleave. Either one can be used depending on your setup.

  ***Method 1:*** `ymextract_x64` (Linux program, available [here][4])
```
$ ymextract_x64 decompressed-filename output-filename.bin  
```

  ***Method 2:***  `decoder.py` (Python script, available [here][2])
```
$ python decoder.py filename.bin force_interleaved
```
  The `decoder.py` script creates a file named `outputfile` in the current directory.  

  This binary output file created by either `ymextract_x64` or `decoder.py` contains sound register data listed in groups of 14 (you may be able to discern a 14-byte pattern when viewing the file with a binary file reader).  

  At this point, the interleaved file can be copied to an SD card and used with  [Example 7][12] from this library.

4. **Convert to Header File**  
  In order to work with [Example 6][11], the interleaved file needs to be converted to a C array and placed in a header file. Use the `bin2c14.py` [python script][13] included in the `extras/tools` directory of this library to convert to a header file.
```
$ python bin2c14.py outputfile 28000 >chiptunes.h
```
  Since [Example 6][11] compiles the audio data into the program itself, there is a limited amount of memory available. For an Arduino UNO or Mini, 28000 bytes is about the most you can fit with a basic player program. This allows for about 40 seconds of music. Hence, the above example has a parameter of `28000`, which tells to script to stop filling the array once it has reached a size of 28000.

References
----------
+ [ymduino][1] project
+ [ymduino code][2], which served as inspiration for my chiptunes example code. Also contains the decoder.py script mentioned above.
+ Yamaha YM2149 [datasheet][3] -- clone of AY-3-8910
+ [YM Playback][4] -- Some sample YM files, along with ymextract converter/extractor program
+ [Genesis8][5] -- source of YM file downloads
+ [YM Jukebox][6] -- large source of YM file downloads

[1]: https://homes.cs.washington.edu/~eqy/ymduino.html
[2]: https://github.com/eqy/ymduino
[3]: http://www.ym2149.com/ym2149.pdf
[4]: http://www.waveguide.se/?article=ym-playback-on-the-ymz284
[5]: http://www.genesis8bit.fr/frontend/music.php
[6]: https://github.com/nguillaumin/ym-jukebox/tree/master/data
[7]: https://github.com/Andy4495/AY3891x
[8]: https://en.wikipedia.org/wiki/Chiptune
[9]: http://www.vgmpf.com/Wiki/index.php?title=YM
[10]: https://en.wikipedia.org/wiki/Programmable_sound_generator
[11]: ../../examples/AY3891x_EX6_Chiptunes_Flash
[12]: ../../examples/AY3891x_EX7_Chiptunes_SD
[13]: ./bin2c14.py
