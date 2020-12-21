AY3891x Library
====================

Arduino library for General Instrument AY-3-8910/AY-3-8912/AY-3-8913 Programmable Sound Generator chip.

All three chips have the same sound generator functionality and programming interface and only differ by the number of general purpose I/O pins available on the IC package. These I/O pins are not related to the sound generation functions.

```
Device     Package Pins  General Purpose I/O Pins
---------  ------------  ------------------------
AY-3-8910       40            16
AY-3-8912       28             8
AY-3-8913       24             0
```

Usage
-----

*Refer to the sketches in the `examples` folder.*

Constructor
<several variations>




References
----------
+ AZ-3-891x [datasheet][1]

[1]: http://map.grauw.nl/resources/sound/generalinstrument_ay-3-8910.pdf
[2]:
