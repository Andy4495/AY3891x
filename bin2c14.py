# From https://stackoverflow.com/questions/8707183/script-tool-to-convert-file-to-c-c-source-code-array
# This file is not part of the MIT license covering the other files in this repository
# Updated to write 14-byte lines, since the data is grouped in 14 bytes.

import sys
from functools import partial

if len(sys.argv) < 2:
  sys.exit('Usage: %s file' % sys.argv[0])
print("char a[] = {")
n = 0
with open(sys.argv[1], "rb") as in_file:
  for c in iter(partial(in_file.read, 1), b''):
    print("0x%02X," % ord(c), end='')
    n += 1
    if n % 14 == 0:
      print("")
print("};")
