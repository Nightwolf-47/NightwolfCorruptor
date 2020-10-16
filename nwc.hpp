#ifndef NWC_HPP_INCLUDED
#define NWC_HPP_INCLUDED

//Some of these headers may be used by the main.cpp file
#include <cstdio> //printf
#include <string> //string stuff
#include <climits> //ULLONG_MAX
#include <fstream> //File I/O
#include <random> //RNG

const int ARG_NONE = 0;
const int ARG_FILE = 1;
const int ARG_START = 2;
const int ARG_END = 3;
const int ARG_EVERY = 4;
const int ARG_ADD = 5;
const int ARG_SHIFT = 6;
const int ARG_REPLACE = 7;
const int ARG_OUTPUT = 8;
const int ARG_RANDOM = 9;
const int ARG_SEED = 10;
const int ARG_REDIRECT = 147;

const int CTPE_NONE = 0;
const int CTPE_ADD = 1;
const int CTPE_SHIFT = 2;
const int CTPE_REPLACE = 3;
const int CTPE_RANDOM = 4;

//NWC Exit codes
const int NWCE_NORMAL = 0; //Normal exit
const int NWCE_NEARGS = 100; //Not enough arguments
const int NWCE_UNKARG = 101; //Unknown argument
const int NWCE_NANERR = 102; //"Not A Number" error
const int NWCE_OORERR = 103; //Out of range error
const int NWCE_NOFILE = 104; //No input file
const int NWCE_NOCORR = 105; //No corruption type
const int NWCE_IEMPTY = 106; //Input file empty
const int NWCE_INOPEN = 107; //Input file can't be opened
const int NWCE_ONOPEN = 108; //Output file can't be opened
const int NWCE_RDFAIL = 147; //File redirection failure

//NWC Critical Exit codes (Executed only when a bug is present)
const int NWCE_CRITAT = 200; //Wrong argument type (argument parser error)
const int NWCE_CRITID = 201; //Input data is empty (file read/size check error)

static std::random_device rd;
static std::mt19937 mt(rd());

typedef unsigned long long NWC_unsigned;
typedef long long NWC_number;

const NWC_unsigned NWC_EOFEND = ULLONG_MAX; //Always end corruption at the end of file (default setting)

NWC_unsigned nwc_getfilesize(std::string filename); //Get size of the specified file (no error checking)

struct NWC_CoType
{
    int type=CTPE_NONE; //Corruption type
    NWC_number val_1=0; //First number
    NWC_number val_2=0; //Second number
    int filled=0; //How many numbers are filled (0-2)
};

class NWC_Class
{
    public:
    std::string filename; //Input file name
    NWC_unsigned startbyte=0; //Start corruption byte
    NWC_unsigned endbyte=NWC_EOFEND; //End corruption byte (default: last byte)
    NWC_unsigned everyxbyte=1; //Corrupt every X byte
    NWC_unsigned randseed=0; //RNG seed (for "-random" parameter)
    NWC_CoType cotype; //Corruption data
    std::string output; //Output file name
    void run(); //Corrupt the file
};

#endif // NWC_HPP_INCLUDED
