#include "nwc.hpp"
#include <cstdlib> //atexit() function
//Nightwolf Corruptor Version 1.1
//Requires a C++11 compiler (C++17 on systems other than Windows, Linux/Android and untested Mac OS X)

NWC_Class nwc;

std::string redfilename;
FILE* printstream=nullptr;

void redirectexit(void) //Works only when redirectprintf was called
{
    if(printstream!=nullptr)
        fclose(stdout);
}

void redirectprintf(std::string filename) //Redirect printf to a file
{
    if(printstream!=nullptr)
        return;
    printstream = freopen(filename.c_str(),"w",stdout);
    if(printstream==nullptr)
    {
        printf("ERROR: Redirection to %s has failed\n",filename.c_str());
        fclose(stdout);
        exit(NWCE_RDFAIL); //Redirection failure
    }
    atexit(redirectexit);
}

void printArgs()
{
    printf("List of arguments: \n");
    printf("-file *file name* - set the input file name (required)\n");
    printf("-start *unsigned number* - set the start byte (default 0 -> first byte)\n");
    printf("-end *unsigned number* - set the end byte (default -> always last byte)\n");
    printf("-every *positive number* - corrupt every X-th byte\n");
    printf("-seed *unsigned number* - set the RNG seed, 0 is a random seed\n");
    printf("-output *file name* - set the output file name (default 'outfile*random number*')\n");
    printf("-add *number* - add (or remove if negative) the number to every corrupted byte\n");
    printf("-shift *number* - shift corrupted byte by *number* bytes to the right (negative numbers shift backwards)\n");
    printf("-replace *byte 1* *byte 2* - replace byte 1 in corrupted bytes with byte 2\n");
    printf("-random *byte 1* *byte 2* - randomize corrupted bytes in byte 1 to byte 2 range\n");
    printf("--help - print this list\n");
}

std::string everybytestr(NWC_unsigned num)
{
    if(num == 1)
        return "";
    std::string numstr=std::to_string(num);
    if((num % 10 > 3 || num % 10 == 0) || (num >= 11 && num <= 13))
        return numstr+"th ";
    else if(num % 10 == 3)
        return numstr+"rd ";
    else if(num % 10 == 2)
        return numstr+"nd ";
    else if(num % 10 == 1)
        return numstr+"st ";
    else
        return numstr+"th "; //should not appear
}

void printSettings()
{
    printf("File name: %s\n",nwc.filename.c_str());
    if(!nwc.output.empty())
        printf("Output file name: %s\n",nwc.output.c_str());
    printf("Start byte: %llu\n",nwc.startbyte);
    if(nwc.endbyte==NWC_EOFEND)
        printf("End byte: End of the file\n");
    else
        printf("End byte: %llu\n",nwc.endbyte);
    printf("Corrupt every %sbyte\n",everybytestr(nwc.everyxbyte).c_str());
    switch(nwc.cotype.type)
    {
    case CTPE_ADD:
        if(nwc.cotype.val_1<0)
            printf("Corruption type: Substract %lld from byte\n",-nwc.cotype.val_1);
        else
            printf("Corruption type: Add %lld to byte\n",nwc.cotype.val_1);
        break;
    case CTPE_SHIFT:
        printf("Corruption type: Shift %lld bytes\n",nwc.cotype.val_1);
        break;
    case CTPE_REPLACE:
        printf("Corruption type: Replace %lld with %lld\n",nwc.cotype.val_1,nwc.cotype.val_2);
        break;
    case CTPE_RANDOM:
        printf("Corruption type: Randomize byte in range %lld-%lld\n",nwc.cotype.val_1,nwc.cotype.val_2);
        if(nwc.randseed==0)
            printf("Seed: Automatic\n");
        else
            printf("Seed: %llu",nwc.randseed);
        break;
    default:
        break;
    }
    printf("\n");
}

void unknownArg(std::string argstr)
{
    printf("ERROR: Unknown argument '%s'\n\n",argstr.c_str());
    exit(NWCE_UNKARG);
}

void argstart(std::string argstr)
{
    try
    {
        nwc.startbyte = std::stoull(argstr,nullptr,0);
    }
    catch(std::invalid_argument& e)
    {
        printf("ERROR: -start: '%s' is not a number\n",argstr.c_str());
        exit(NWCE_NANERR);
    }
    catch(std::out_of_range& e)
    {
        printf("ERROR: -start: '%s' is out of range ( 0-%llu )\n",argstr.c_str(),ULLONG_MAX);
        exit(NWCE_OORERR);
    }
}

void argend(std::string argstr)
{
    try
    {
        nwc.endbyte = std::stoull(argstr,nullptr,0);
    }
    catch(std::invalid_argument& e)
    {
        printf("ERROR: -end: '%s' is not a number\n",argstr.c_str());
        exit(NWCE_NANERR);
    }
    catch(std::out_of_range& e)
    {
        printf("ERROR: -end: '%s' is out of range ( 0-%llu )\n",argstr.c_str(),ULLONG_MAX);
        exit(NWCE_OORERR);
    }
}

void argevery(std::string argstr)
{
    try
    {
        nwc.everyxbyte = std::stoull(argstr);
    }
    catch(std::invalid_argument& e)
    {
        printf("ERROR: -every(byte): '%s' is not a number\n",argstr.c_str());
        exit(NWCE_NANERR);
    }
    catch(std::out_of_range& e)
    {
        printf("ERROR: -every(byte): '%s' is out of range ( 1-%llu )\n",argstr.c_str(),ULLONG_MAX);
        exit(NWCE_OORERR);
    }

    if(nwc.everyxbyte==0)
    {
        printf("WARNING: -every(byte): 0 is not a valid option (minimum is 1), changing to 1\n");
        nwc.everyxbyte=1;
    }
}

void argadd(std::string argstr)
{
    try
    {
        nwc.cotype.val_1 = std::stoll(argstr,nullptr,0);
    }
    catch(std::invalid_argument& e)
    {
        printf("ERROR: -add: '%s' is not a number\n",argstr.c_str());
        exit(NWCE_NANERR);
    }
    catch(std::out_of_range& e)
    {
        printf("ERROR: -add: '%s' is out of range ( %lld-%lld )\n",argstr.c_str(),LLONG_MIN,LLONG_MAX);
        exit(NWCE_OORERR);
    }
    nwc.cotype.type = CTPE_ADD;
}

void argshift(std::string argstr)
{
    try
    {
        nwc.cotype.val_1 = std::stoll(argstr,nullptr,0);
    }
    catch(std::invalid_argument& e)
    {
        printf("ERROR: -shift: '%s' is not a number\n",argstr.c_str());
        exit(NWCE_NANERR);
    }
    catch(std::out_of_range& e)
    {
        printf("ERROR: -shift: '%s' is out of range ( %lld-%lld )\n",argstr.c_str(),LLONG_MIN,LLONG_MAX);
        exit(NWCE_OORERR);
    }
    nwc.cotype.type = CTPE_SHIFT;
}

bool argreplace(std::string argstr)
{
    if(nwc.cotype.filled>1)
    {
        nwc.cotype.val_1 = 0;
        nwc.cotype.val_2 = 0;
        nwc.cotype.filled = 0;
    }

    try
    {
        if(nwc.cotype.filled==0)
        {
            nwc.cotype.val_1 = std::stoll(argstr,nullptr,0);
        }
        else
        {
            nwc.cotype.val_2 = std::stoll(argstr,nullptr,0);
        }
    }
    catch(std::invalid_argument& e)
    {
        printf("ERROR: -replace: '%s' is not a number in value %d\n",argstr.c_str(),nwc.cotype.filled+1);
        exit(NWCE_NANERR);
    }
    catch(std::out_of_range& e)
    {
        printf("ERROR: -replace: '%s' is out of range ( 0-255 ) in value %d\n",argstr.c_str(),nwc.cotype.filled+1);
        exit(NWCE_OORERR);
    }
    if(nwc.cotype.val_1>255 || nwc.cotype.val_2>255 || nwc.cotype.val_1<0 || nwc.cotype.val_2<0)
    {
        printf("ERROR: -replace: '%s' is out of range ( 0-255 ) in value %d\n",argstr.c_str(),nwc.cotype.filled+1);
        exit(NWCE_OORERR);
    }
    nwc.cotype.type = CTPE_REPLACE;
    nwc.cotype.filled++;
    if(nwc.cotype.filled>1)
        return true;
    else
        return false;
}

bool argrandom(std::string argstr)
{
    if(nwc.cotype.filled>1)
    {
        nwc.cotype.val_1 = 0;
        nwc.cotype.val_2 = 0;
        nwc.cotype.filled = 0;
    }

    try
    {
        if(nwc.cotype.filled==0)
        {
            nwc.cotype.val_1 = std::stoll(argstr,nullptr,0);
        }
        else
        {
            nwc.cotype.val_2 = std::stoll(argstr,nullptr,0);
        }
    }
    catch(std::invalid_argument& e)
    {
        printf("ERROR: -random(ize): '%s' is not a number in value %d\n",argstr.c_str(),nwc.cotype.filled+1);
        exit(NWCE_NANERR);
    }
    catch(std::out_of_range& e)
    {
        printf("ERROR: -random(ize): '%s' is out of range ( 0-255 ) in value %d\n",argstr.c_str(),nwc.cotype.filled+1);
        exit(NWCE_OORERR);
    }

    if(nwc.cotype.val_1>255 || nwc.cotype.val_2>255 || nwc.cotype.val_1<0 || nwc.cotype.val_2<0)
    {
        printf("ERROR: -random(ize): '%s' is out of range ( 0-255 ) in value %d\n",argstr.c_str(),nwc.cotype.filled+1);
        exit(NWCE_OORERR);
    }
    nwc.cotype.type = CTPE_RANDOM;
    nwc.cotype.filled++;
    if(nwc.cotype.filled>1)
        return true;
    else
        return false;
}

void argseed(std::string argstr)
{
    try
    {
        nwc.randseed = std::stoull(argstr);
    }
    catch(std::invalid_argument& e)
    {
        printf("ERROR: -seed: '%s' is not a number\n",argstr.c_str());
        exit(NWCE_NANERR);
    }
    catch(std::out_of_range& e)
    {
        printf("ERROR: -seed: '%s' is out of range ( 0-%llu )\n",argstr.c_str(),ULLONG_MAX);
        exit(NWCE_OORERR);
    }
}

void arghelp()
{
    printArgs();
    exit(NWCE_NORMAL);
}

int main(int argc, char* argv[])
{
    std::string argstr;
    std::string repstr;
    int argmode=ARG_NONE;
    printf("Nightwolf Corruptor V1.1 is running...\n\n");
    if(argc<2)
    {
        printf("ERROR: Not enough arguments\n\n");
        printArgs();
        return NWCE_NEARGS;
    }
    for(int i=1; i<argc; i++)
    {
        argstr=argv[i];
        switch(argmode)
        {
        case ARG_NONE:
            if(argstr=="-file")
                argmode=ARG_FILE;
            else if(argstr=="-start")
                argmode=ARG_START;
            else if(argstr=="-end")
                argmode=ARG_END;
            else if(argstr=="-every"||argstr=="-everybyte")
                argmode=ARG_EVERY;
            else if(argstr=="-add")
                argmode=ARG_ADD;
            else if(argstr=="-shift")
                argmode=ARG_SHIFT;
            else if(argstr=="-replace")
                argmode=ARG_REPLACE;
            else if(argstr=="-o"||argstr=="-output")
                argmode=ARG_OUTPUT;
            else if(argstr=="-r"||argstr=="-redirect")
                argmode=ARG_REDIRECT;
            else if(argstr=="-random"||argstr=="-randomize")
                argmode=ARG_RANDOM;
            else if(argstr=="-seed")
                argmode=ARG_SEED;
            else if(argstr=="--help")
                arghelp();
            else
                unknownArg(argstr);
            break;
        case ARG_FILE:
            nwc.filename = argstr;
            argmode = ARG_NONE;
            break;
        case ARG_START:
            argstart(argstr);
            argmode = ARG_NONE;
            break;
        case ARG_END:
            argend(argstr);
            argmode = ARG_NONE;
            break;
        case ARG_EVERY:
            argevery(argstr);
            argmode = ARG_NONE;
            break;
        case ARG_ADD:
            argadd(argstr);
            argmode = ARG_NONE;
            break;
        case ARG_SHIFT:
            argshift(argstr);
            argmode = ARG_NONE;
            break;
        case ARG_REPLACE:
            if(argreplace(argstr))
                argmode = ARG_NONE;
            break;
        case ARG_OUTPUT:
            nwc.output = argstr;
            argmode = ARG_NONE;
            break;
        case ARG_REDIRECT:
            redirectprintf(argstr);
            argmode = ARG_NONE;
            break;
        case ARG_RANDOM:
            if(argrandom(argstr))
                argmode = ARG_NONE;
            break;
        case ARG_SEED:
            argseed(argstr);
            argmode = ARG_NONE;
            break;
        default: //Should never be executed
            printf("CRITICAL ERROR: Incorrect argument type %d\n",argmode);
            printf("Please report this error to the developer (Nightwolf-47)\n");
            return NWCE_CRITAT;
            break;
        }
    }
    printSettings();
    nwc.run();
    printf("Corruption successful!\n");
    redirectexit(); //In case the atexit(redirectexit) function doesn't work
    return NWCE_NORMAL;
}
