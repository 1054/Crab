/*
 
 Please compile like this:
 
     clang++ main.cpp -o CrabTableReadInfo_linux_x86_64
 
     clang++ main.cpp -o CrabTableReadInfo_mac
 
 
 Last update:
     
     2015-02-01 correct trimline.erase(0,foundpos-1);
 
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CrabTableReadInfo.cpp"

int main(int argc, char **argv)
{
    /**
    // You’ll notice that argv[0] is the path and name of the program itself.
    std::cout << "CrabTableReadInfo: Input " << argc-1 << " arguments." << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::cout << argv[i] << std::endl;
    }
    **/
    if(argc==3)
    {
        std::string GotKeyValue = CrabTableReadInfo(argv[1],argv[2]);
        std::cout << GotKeyValue;
        std::cout << std::endl;
    }
    else if(argc==4)
    {
        std::string GotKeyValue = CrabTableReadInfo(argv[1],argv[2],argv[3]);
        std::cout << GotKeyValue;
        std::cout << std::endl;
    }
    else {
        std::cout << "Usage: " << std::endl << "       CrabTableReadInfo CenA.ini RA" << std::endl << std::endl;
    }
    return 0;
}
