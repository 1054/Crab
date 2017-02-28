/*
 
 
 Please compile like this:
 
 clang++ CrabFitsFloat2Bytes.cpp -o CrabFitsFloat2Bytes_mac
 
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
    
    if(argc>=2) {
        char *strNumber = argv[1];
        float varNumber = atof(strNumber);
        unsigned char *ptrNumber = (unsigned char *)&varNumber;
        for(int i=0; i<sizeof(float); i++) {
            std::cout << hex << uppercase << setfill('0') << setw(2) << (unsigned int)(*ptrNumber) << " " << std::flush;
            ptrNumber++;
        }
        std::cout << std::endl;
    } else {
        std::cout << "Usage: CrabFitsFloat2Bytes 3.1415926" << std::endl;
    }
    return 0;
}


