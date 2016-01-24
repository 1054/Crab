/*
 
 compile: 
     
     clang++ hex2float.cpp -o hex2float_mac
 
 Note: 
     
     assuming little endian
 
 
 reference:
     
     http://stackoverflow.com/questions/21323099/convert-a-hexadecimal-to-a-float-and-viceversa-in-c
 */


#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>

using namespace std;

int main(int argc, char **argv)
{
    if(argc>1)
    {
        // char myString[]="0x3f9d70a4";
        uint32_t HexVar;
        float FltVar;
        sscanf(argv[1],"%x",&HexVar); // TODO: check input
        
        int SetLittleEndian = 0; // TODO
        
        if(SetLittleEndian) {
            FltVar = *((float*)&HexVar);
            printf("%f\n", FltVar);
        } else {
            uint32_t HexVarSwapd;
            unsigned char *ChrVar = (unsigned char *)&HexVar;
            unsigned char *ChrVarSwapd = (unsigned char *)&HexVarSwapd;
            for(int i=0; i<sizeof(float); i++) {
                memcpy(ChrVarSwapd+i,ChrVar+sizeof(float)-1-i,1);
            }
            FltVar = *((float*)&HexVarSwapd);
            printf("%f\n", FltVar);
        }
        
    } else {
        printf("Usage: hex2float 3f9d70a4 => 1.23\n");
        printf("Usage: hex2float a4709d3f => 1.23\n");
    }
}

