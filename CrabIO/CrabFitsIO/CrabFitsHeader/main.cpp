/*
 
 Please compile like this:
 
     clang++ main.cpp CrabFitsIO.cpp -o CrabFitsHeader_linux_x86_64
 
 Last update:
     
     2014-12-30
 
 */

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include "CrabFitsIO.h"

using namespace std;

// extern char *readFitsHeader(const char *FilePath);

int main(int argc, char **argv)
{
    /**
    // You’ll notice that argv[0] is the path and name of the program itself.
    std::cout << "CrabTableReadInfo: Input " << argc-1 << " arguments." << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::cout << argv[i] << std::endl;
    }
    **/
    if(argc>1)
    {
        // <DEBUG>
        // const char *aaa = readFitsHeader(argv[1],100);
        // std::cout << (aaa==0) << std::endl << (aaa==NULL) << std::endl;
        // return 0;
        //
        int extNumber = 0;
        const char *cstrHeader = readFitsHeader(argv[1],extNumber);
        while(cstrHeader!=0) {
            std::string sstrHeader(cstrHeader);
            for(int i=0; i<strlen(cstrHeader); i+=80) { //
                std::string subHeader = sstrHeader.substr(i,80);
                if(subHeader.find_first_not_of(' ') != std::string::npos) {
                    int lineNumber = (int)i/80;
                    // std::cout << ":" << std::setw(3) << left << extNumber << ":" << std::setw(5) << left << lineNumber << subHeader << std::endl;
                    std::cout << subHeader << std::endl;
                }
            }
            std::cout << std::endl;
            extNumber++; cstrHeader = readFitsHeader(argv[1],extNumber);
        }
    } else {
        std::cout << "CrabFitsHeader 1.fits" << std::endl;
    }
    return 0;
}
