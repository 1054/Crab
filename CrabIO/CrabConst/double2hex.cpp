/*
 
 compile: 
     
     clang++ double2hex.cpp -o double2hex_mac
 
 note:
     
     assuming little endian
 
 
 reference:
     
     http://stackoverflow.com/questions/21323099/convert-a-hexadecimal-to-a-float-and-viceversa-in-c
 */


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <cstring>
#include <iostream>
#include <iomanip>

using namespace std;

int main(int argc, char **argv)
{
    if(argc>1)
    {
        double FltVar = atof(argv[1]);
        unsigned char *ChrVar = (unsigned char *)&FltVar;
        // std::cout << "double is " << sizeof(double) << " bytes" << std::endl;
        
        int SetLittleEndian = 0; // TODO
        
        if(SetLittleEndian) {
            for(int i=sizeof(double)-1; i>=0; i--) {
                std::cout << hex << setw(2) << setfill('0') << (int)*(ChrVar+i) << std::flush;
                // TODO: uppercase
                // TODO: space
            }
        } else {
            for(int i=0; i<sizeof(double); i++) {
                std::cout << hex << setw(2) << setfill('0') << (int)*(ChrVar+i) << std::flush;
                // TODO: uppercase
                // TODO: space
            }
        }
        std::cout << std::endl;
    } else {
        printf("Usage: double2hex 1.23 => 3f9d70a4\n");
        printf("Usage: double2hex 1.23 => a4709d3f\n");
    }
}

