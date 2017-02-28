/*
 
 Please compile like this:
 
     clang++ main.cpp CrabFitsIO.cpp -o CrabFitsHeadEditor_mac
 
 */

#include <stdio.h>
#include <fstream>
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
    char *cstrFilePath = NULL; char *cstrExtNumber = (char *)"0";
    char *cstrKeyName = NULL;
    char *cstrKeyValue = NULL;
    for(int i=1; i<argc; i++) {
        if(strncmp(argv[i],"-ext",4)==0 && i<argc-1) { i++; cstrExtNumber = argv[i]; continue; }
        if(cstrFilePath==NULL && i<=argc-1) { cstrFilePath = argv[i]; continue; }
        if(cstrKeyName==NULL && i<=argc-1) { cstrKeyName = argv[i]; continue; }
        if(cstrKeyValue==NULL && i<=argc-1) { cstrKeyValue = argv[i]; continue; }
    }
    if(cstrExtNumber==NULL) {cstrExtNumber = "0";}
    std::cout << "CrabFitsHeadEditor" << " " << cstrFilePath << " " << cstrKeyName << " " << cstrKeyValue << " " << "extension=" << atoi(cstrExtNumber) << std::endl;
    //
    //
    if(cstrFilePath!=NULL && cstrKeyName!=NULL && cstrKeyValue!=NULL)
    {
        // <DEBUG>
        // const char *aaa = readFitsHeader(argv[1],100);
        // std::cout << (aaa==0) << std::endl << (aaa==NULL) << std::endl;
        // return 0;
        //
        // read whole header
        int   extNumber = 0;  if(cstrExtNumber!=NULL) {extNumber=atoi(cstrExtNumber);}
        char *cstrHeader = NULL;
        long  posHeader = 0;
        long  lenHeader = 0;
        int   errStatus = readFitsHeader(cstrFilePath,extNumber,&cstrHeader,&posHeader,&lenHeader);
        while(cstrHeader!=NULL) {
            //
            //
            std::cout << "XTENSION" << extNumber << "-----------------------------------------------------------------------" << std::endl;
            //
            // print old keyvalue
            std::string sstrHeader(cstrHeader);
            for(int i=0; i<strlen(cstrHeader); i+=80) { //
                std::string subHeader = sstrHeader.substr(i,80);
                if(subHeader.find_first_not_of(' ') != std::string::npos) {
                    int lineNumber = (int)i/80;
                    // std::cout << ":" << std::setw(3) << left << extNumber << ":" << std::setw(5) << left << lineNumber << subHeader << std::endl;
                    if(subHeader.find(cstrKeyName) != std::string::npos) {
                        std::cout << subHeader << std::endl;
                    }
                }
            }
            //
            // modify the keyvalue
            int errCode = modKeyword(cstrKeyName,cstrKeyValue,cstrHeader);
            //
            // print new keyvalue
            sstrHeader = std::string(cstrHeader);
            for(int i=0; i<strlen(cstrHeader); i+=80) { //
                std::string subHeader = sstrHeader.substr(i,80);
                if(subHeader.find_first_not_of(' ') != std::string::npos) {
                    int lineNumber = (int)i/80;
                    // std::cout << ":" << std::setw(3) << left << extNumber << ":" << std::setw(5) << left << lineNumber << subHeader << std::endl;
                    if(subHeader.find(cstrKeyName) != std::string::npos) {
                        std::cout << subHeader << std::endl;
                    }
                }
            }
            //
            // <TODO> how to write the file?
            // rewrite
            std::ofstream fp(cstrFilePath,std::ios::in|std::ios::out|std::ios::binary);
            fp.seekp(posHeader,std::ios::beg);
            fp.write(cstrHeader,lenHeader);
            fp.close();
            //
            // read next extension header
            if(cstrHeader!=NULL) { free(cstrHeader); }
            extNumber++;
            errStatus = readFitsHeader(cstrFilePath,extNumber,&cstrHeader,&posHeader,&lenHeader);
        }
    } else {
        std::cout << "CrabFitsHeader 1.fits KeyName KeyValue -extension 3" << std::endl;
    }
    return 0;
}