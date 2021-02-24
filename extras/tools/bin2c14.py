# Program to create a C header file defining the sound chip data from a YM file.
# Use this to create the header file used by EX6 in the library. 
# See info at the following site for specifics on how to use.
#   https://github.com/Andy4495/AY3891x
#
#    02/05/21 - A.T. - Original


import sys
from functools import partial

if len(sys.argv) < 2:
  sys.exit('Usage: %s filename [maximum_number_of_bytes]' % sys.argv[0])
print("const byte PROGMEM psg_data[] = {")
n = 0
if len(sys.argv) == 3:
  maxbytes = int(sys.argv[2])
else:
  maxbytes = 999999
with open(sys.argv[1], "rb") as in_file:
  for c in iter(partial(in_file.read, 1), b''):
    print("0x%02X," % ord(c), end='')
    n += 1
    if n % 14 == 0:
      print("")
      if n > maxbytes:
        break
print("};")
