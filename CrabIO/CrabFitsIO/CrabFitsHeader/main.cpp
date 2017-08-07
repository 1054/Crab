/*
 
 Please compile like this:
 
     clang++ ../CrabFitsIO.cpp main.cpp -o CrabFitsHeader_mac_v20170807
     g++ ../CrabFitsIO.cpp main.cpp -o CrabFitsHeader_linux_x86_64
     #-static-libstdc++
 
 Last update:
     
     2014-12-30
     
     2016-10-29 add "-ext" argument
 
 */

#include <stdio.h>
#include <stdlib.h> /* atoi */
#include <string.h>
#include <iostream>
#include <iomanip>
#include "../CrabFitsIO.h"

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
    char *cstrFilePath = NULL; char *cstrExtNumber = (char *)"-1";
    
    for(int i=1; i<argc; i++) {
        if(strncmp(argv[i],"-ext",4)==0 && i<argc-1) {
            i++; cstrExtNumber = argv[i]; continue;
        }
        if(cstrFilePath==NULL && i<=argc-1) { 
            cstrFilePath = argv[i]; continue; 
        }
    }
    
    if(argc>1)
    {
        // <DEBUG>
        // const char *aaa = readFitsHeader(argv[1],100);
        // std::cout << (aaa==0) << std::endl << (aaa==NULL) << std::endl;
        // return 0;
        //
        int extNumber = atoi(cstrExtNumber);
        const char *cstrHeader = readFitsHeader(cstrFilePath,extNumber);
        // if no "-ext" argument then we read all extensions
        while(cstrHeader!=NULL) {
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
            if(-1==atoi(cstrExtNumber)) {
                if(-1==extNumber) {extNumber++;}
                extNumber++; cstrHeader = readFitsHeader(cstrFilePath,extNumber);
            } else {
                cstrHeader = NULL;
            }
        }
    } else {
        std::cout << "CrabFitsHeader 1.fits" << std::endl;
    }
    return 0;
}
