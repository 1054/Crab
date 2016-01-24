/*
 
 
 Please compile like this:
 
 clang++ CrabFitsBytes2Int.cpp -o CrabFitsBytes2Int_mac
 
 */

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>


using namespace std;


int main(int argc, char **argv)
{
    /**
     // You’ll notice that argv[0] is the path and name of the program itself.
     std::cout << "CrabTableReadInfo: Input " << argc-1 << " arguments." << std::endl;
     for (int i = 0; i < argc; ++i) {
     std::cout << argv[i] << std::endl;
     }
     **/
    
    if(argc>sizeof(int)) {
        char *strBytes = (char *)malloc(sizeof(int)*sizeof(char));
        for(int i=0; i<sizeof(int); i++) {
            std::string tmpByte(argv[i+1]);
            if(tmpByte.find("0x")!=0) {tmpByte="0x"+tmpByte;}
            strBytes[i] = (unsigned char)(strtol(tmpByte.c_str(),NULL,0));
            //<TODO><DEBUG>//std::cout << hex << (unsigned int)strBytes[i] << std::endl;
        }
        int *varNumber = (int *)strBytes;
        std::cout << *varNumber << std::endl;
    } else {
        std::cout << "Usage: CrabFitsBytes2Int ";
        for(int i=0; i<sizeof(int); i++) {
            std::cout << "FF" << " " << std::flush;
        }
        std::cout << std::endl;
    }
    return 0;
}


