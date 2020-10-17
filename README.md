# Nightwolf Corruptor
A command-line tool for corrupting files.

## Argument list
* -file \<file name> - Set the input file name **(required)**
* -output \<file name> - Set the output file name **(default: 'outfile\<*random number\>*')**
* -start \<unsigned number> - Set the start byte **(default: 0)**
* -end \<unsigned number> - Set the end byte **(default: last byte)**
* -every \<positive number> - Corrupt every \<number\> bytes **(default: 1)**
* -seed \<unsigned number> - Set the random seed **(default: 0 - automatic seed)**
* -redirect \<file name> - redirect output to a file

### Corruption types 
#### 🔷 NOTE: Only one of them is executed at once!
* -add \<number> - add the number to every corrupted byte (negative numbers subtract)
* -shift \<number> - Shift corrupted byte by \<number\> bytes forward (negative numbers shift backwards)
* -replace \<number 1> \<number 2> - Replace number 1 with number 2 in corrupted bytes
* -random \<byte 1> \<byte 2> - Randomize corrupted bytes in a byte 1\* to byte 2\* range

\* byte 1 and byte 2 are numbers between 0 and 255
