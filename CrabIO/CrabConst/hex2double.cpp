/*
 
 compile: 

     clang++ hex2double.cpp -o hex2double_mac
     clang++ hex2double.cpp -o hex2double_linux_x86_64
 
 Note: 
     
     assuming little endian
 
 
 reference:
     
     http://stackoverflow.com/questions/21323099/convert-a-hexadecimal-to-a-float-and-viceversa-in-c
 */


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <memory.h>


using namespace std;

int main(int argc, char **argv)
{
    if(argc>1)
    {
        // char myString[]="ae47e17a14aef33f";
        uint64_t HexVar;
        double FltVar;
        sscanf(argv[1],"%lx",&HexVar); // TODO: check input
        //<DEBUG>printf("%ld\n", sizeof(HexVar));
        
        int SetLittleEndian = 0; // TODO
        
        if(SetLittleEndian) {
            FltVar = *((double*)&HexVar);
            printf("%f\n", FltVar);
        } else {
            uint64_t HexVarSwapd;
            unsigned char *ChrVar = (unsigned char *)&HexVar;
            unsigned char *ChrVarSwapd = (unsigned char *)&HexVarSwapd;
            for(int i=0; i<sizeof(double); i++) {
                memcpy(ChrVarSwapd+i,ChrVar+sizeof(double)-1-i,1);
            }
            FltVar = *((double*)&HexVarSwapd);
            printf("%f\n", FltVar);
        }
        
    } else {
        //printf("Usage: hex2double 3ff3ae147ae147ae => 1.23\n");
        printf("Usage: hex2double ae47e17a14aef33f => 1.23\n");
    }
}

