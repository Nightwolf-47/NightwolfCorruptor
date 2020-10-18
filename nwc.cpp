#include "nwc.hpp"

#ifdef _WIN32 //Windows 32/64-bit

#include <windows.h>
NWC_unsigned nwc_getfilesize(std::string filename)
{
    NWC_unsigned filesize;
    WIN32_FILE_ATTRIBUTE_DATA fadata;
    GetFileAttributesExA((LPCSTR)filename.c_str(),GetFileExInfoStandard,&fadata);
    filesize = (NWC_unsigned)fadata.nFileSizeLow | ((NWC_unsigned)fadata.nFileSizeHigh << 32);
    return filesize;
}

#elif defined (__unix__) || (defined (__APPLE__) && defined (__MACH__)) //*nix and Mac OS X

#include <sys/stat.h>
#include <unistd.h>
NWC_unsigned nwc_getfilesize(std::string filename)
{
    NWC_unsigned filesize;
    struct stat st;
    stat(filename.c_str(),&st);
    filesize = st.st_size;
    return filesize;
}

#else //Other systems (requires a C++17 compiler and a working <filesystem> library)

#include <filesystem>
NWC_unsigned nwc_getfilesize(std::string filename)
{
    NWC_unsigned filesize;
    std::filesystem::path ipath;
    ipath = filename;
    filesize = std::filesystem::file_size(ipath);
    return filesize;
}

#endif // _WIN32

int randomint(int imin,int imax)
{
    std::uniform_int_distribution<int> dist(imin,imax);
    return dist(mt);
}

char limittochar(int val)
{
    return (char)((unsigned)val % 256);
}

void NWC_Class::run()
{
    if(filename.empty())
    {
        printf("ERROR: No base file (-file) set, aborting...\n");
        exit(NWCE_NOFILE);
    }
    if(output.empty())
    {
        output = "outfile"+std::to_string(randomint(1,INT_MAX));
        printf("INFO: Output file will be %s\n",output.c_str());
    }
    if(everyxbyte==0) //everyxbyte has to be at least 1, as 0 would cause an infinite loop
    {
        everyxbyte=1;
    }
    if(cotype.type==CTPE_NONE)
    {
        printf("ERROR: No corruption type (add/shift/replace/random) set, aborting...\n");
        exit(NWCE_NOCORR);
    }
    if(randseed!=0)
        mt.seed(randseed);
    std::vector<char> filevec;
    std::ifstream ifs(filename,std::ios::binary);
    if(ifs.is_open())
    {
        NWC_unsigned length = nwc_getfilesize(filename);
        if(length==0)
        {
            printf("ERROR: Input file is empty\n");
            exit(NWCE_IEMPTY);
        }
        filevec.resize(length);
        ifs.read(filevec.data(),filevec.size()); //Fastest way to load a file to a std::vector<char>
        ifs.close();
    }
    else
    {
        printf("ERROR: Input file can't be opened\n");
        exit(NWCE_INOPEN);
    }

    if(filevec.size()==0)
    {
        printf("CRITICAL ERROR: Input file data is empty\n");
        printf("Please report this error to the developer (Nightwolf-47)\n");
        exit(NWCE_CRITID);
    }
    if(endbyte==NWC_EOFEND) //If endbyte is set to the end of file, set it to the last byte in the file
    {
        endbyte=filevec.size()-1;
    }

    endbyte = std::min(endbyte,(NWC_unsigned)filevec.size()-1);

    for(NWC_unsigned byte=startbyte; byte<=endbyte; byte+=everyxbyte)
    {
        char& bytechar=filevec[byte];
        switch(cotype.type)
        {
        case CTPE_ADD: //Add val_1 to byte. If the resulting value is higher than 255 or lower than 0, it wraps around
            bytechar=limittochar((int)bytechar+(cotype.val_1 % 256)); //val_1 is converted to a value in range (-255,255) to prevent integer overflow
            break;
        case CTPE_SHIFT: //Swap current byte with byte that's val_1 bytes forward from it (this SHIFT implementation might change later)
            if(cotype.val_1<0 && byte+cotype.val_1<0)
            {
                break;
            }
            else if(byte+cotype.val_1<=endbyte && byte+cotype.val_1>=startbyte)
            {
                std::swap(bytechar,filevec[byte+cotype.val_1]);
            }
            break;
        case CTPE_REPLACE: //Replace byte with val_2 if it has val_1 value
            if(cotype.val_1==(NWC_number)bytechar)
                bytechar=cotype.val_2;
            break;
        case CTPE_RANDOM: //Randomize byte in range between val_1 and val_2
            if(cotype.val_1>cotype.val_2)
                std::swap(cotype.val_1,cotype.val_2);
            bytechar = (char)(randomint(cotype.val_1,cotype.val_2));
            break;
        default:
            break;
        }
    }

    std::ofstream ofs(output,std::ios::binary);
    if(ofs.is_open())
    {
        ofs.write(filevec.data(),filevec.size());
        ofs.close();
    }
    else
    {
        printf("ERROR: Output file can't be opened\n");
        exit(NWCE_ONOPEN);
    }
}
