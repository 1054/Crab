/*
 
 compile: 
     
     clang++ hex2double.cpp -o hex2double_mac
 
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

using namespace std;

int main(int argc, char **argv)
{
    if(argc>1)
    {
        // char myString[]="0x3f9d70a4";
        uint32_t HexVar;
        double FltVar;
        sscanf(argv[1],"%x",&HexVar); // TODO: check input
        
        int SetLittleEndian = 0; // TODO
        
        if(SetLittleEndian) {
            FltVar = *((double*)&HexVar);
            printf("%f\n", FltVar);
        } else {
            uint32_t HexVarSwapd;
            unsigned char *ChrVar = (unsigned char *)&HexVar;
            unsigned char *ChrVarSwapd = (unsigned char *)&HexVarSwapd;
            for(int i=0; i<sizeof(double); i++) {
                memcpy(ChrVarSwapd+i,ChrVar+sizeof(double)-1-i,1);
            }
            FltVar = *((double*)&HexVarSwapd);
            printf("%f\n", FltVar);
        }
        
    } else {
        printf("Usage: hex2double 3f9d70a4 => 1.23\n");
        printf("Usage: hex2double a4709d3f => 1.23\n");
    }
}

