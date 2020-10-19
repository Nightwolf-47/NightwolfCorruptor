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
* -replace \<byte 1> \<byte 2> - Replace byte 1\* with byte 2\* in corrupted bytes
* -random \<byte 1> \<byte 2> - Randomize corrupted bytes in a byte 1 to byte 2 range

**Unsigned number**, **number** and **byte** can be written as a hexadecimal number (prepend it with *0x*) or as an octal number (prepend it with *0*), otherwise the numbers are decimal. 
In **\-every** and **\-seed** arguments the numbers are always decimal.

\* byte 1 and byte 2 are numbers between 0 and 255

## Compilation
Compiling is simple as NWC only has 2 .cpp files and 1 header file, but here are lines of code that you can paste to your terminal/cmd.exe to compile this program.
The code should compile with C++11 standard, but C++14 or newer is recommended.
### Windows (MinGW)
```
g++ -Wall -std=c++14 -O2 -s -o nwc.exe main.cpp nwc.cpp
```
### Linux (GCC/Clang)
```
g++ -Wall -std=c++14 -O2 -s -o nwc main.cpp nwc.cpp
```
