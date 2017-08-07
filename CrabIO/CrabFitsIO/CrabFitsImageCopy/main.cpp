/*
 
 ; 
 ; Copy image
 ;
 ; Copy part of the input image to the output image by giving the -rect argument.
 ;
 ; rect is i1:i2 j1:j2 with numbers starting from 0 to dimen(image)-1
 ;
 
 Please compile like this:
 
     clang++ main.cpp ../CrabFitsIO.cpp -o CrabFitsImageCopy_linux_x86_64
     clang++ main.cpp ../CrabFitsIO.cpp -o CrabFitsImageCopy_mac
 
 Last update:
     
     2017-08-03   initialized from CrabFitsImageCut
 
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <fstream>
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
    char *cstrInputFilePath = NULL; char *cstrInputExtNumber = NULL;
    char *cstrOutputFilePath = NULL; char *cstrOutputExtNumber = NULL;
    char *cstrCenX = NULL;
    char *cstrCenY = NULL;
    char *cstrCenRX = NULL;
    char *cstrCenRY = NULL;
    char *cstrInputRectI0 = NULL;
    char *cstrInputRectJ0 = NULL;
    char *cstrInputRectI1 = NULL;
    char *cstrInputRectJ1 = NULL;
    char *cstrOutputRectI0 = NULL;
    char *cstrOutputRectJ0 = NULL;
    char *cstrOutputRectI1 = NULL;
    char *cstrOutputRectJ1 = NULL;
    int InputRectI0=0, InputRectJ0=0, InputRectI1=0, InputRectJ1=0;
    int OutputRectI0=0, OutputRectJ0=0, OutputRectI1=0, OutputRectJ1=0;
    
    int debug = 0; // <TODO><DEBUG>
    
    for(int i=1; i<argc; i++) {
        if(strncmp(argv[i],"-ext",4)==0 && i<argc-1) {
            if(i>=argc-1) { std::cout << "CrabFitsImageCopy: Input Error! -ext should be followed with 1 number value!" << std::endl; return -1; }
            if(cstrInputExtNumber==NULL) {
                if(strspn(argv[i+1],"+-.eE0123456789")==strlen(argv[i+1])) { i++; cstrInputExtNumber = argv[i]; continue; }
            }
            if(cstrOutputExtNumber==NULL) {
                if(strspn(argv[i+1],"+-.eE0123456789")==strlen(argv[i+1])) { i++; cstrOutputExtNumber = argv[i]; continue; }
            }
            continue;
        }
        if(strncmp(argv[i],"-cen",4)==0 && i<argc-3) {
            i++; cstrCenX = argv[i];
            i++; cstrCenY = argv[i];
            i++; cstrCenRX = argv[i];
            if(i<argc-1) { if(strspn(argv[i+1],"+-.eE0123456789")==strlen(argv[i+1])) {i++; cstrCenRY=argv[i];} else {cstrCenRY=cstrCenRX;} } else {cstrCenRY=cstrCenRX;} continue;
        }
        if(strncmp(argv[i],"-rect",5)==0) {
            if(i>=argc-4) { std::cout << "CrabFitsImageCopy: Input Error! -rect should be followed with 4 number values!" << std::endl; return -1; }
            if(cstrInputRectI0==NULL) {
                if(strspn(argv[i+1],"+-.eE0123456789")==strlen(argv[i+1])) {i++; cstrInputRectI0 = argv[i];} else {std::cout << "CrabFitsImageCopy: Input Error! -rect should be followed with 4 number values (i0,j0,i1,j1)!" << std::endl; return -1; }
                if(strspn(argv[i+1],"+-.eE0123456789")==strlen(argv[i+1])) {i++; cstrInputRectJ0 = argv[i];} else {std::cout << "CrabFitsImageCopy: Input Error! -rect should be followed with 4 number values (i0,j0,i1,j1)!" << std::endl; return -1; }
                if(strspn(argv[i+1],"+-.eE0123456789")==strlen(argv[i+1])) {i++; cstrInputRectI1 = argv[i];} else {std::cout << "CrabFitsImageCopy: Input Error! -rect should be followed with 4 number values (i0,j0,i1,j1)!" << std::endl; return -1; }
                if(strspn(argv[i+1],"+-.eE0123456789")==strlen(argv[i+1])) {i++; cstrInputRectJ1 = argv[i];} else {std::cout << "CrabFitsImageCopy: Input Error! -rect should be followed with 4 number values (i0,j0,i1,j1)!" << std::endl; return -1; }
                continue;
            }
            if(cstrOutputRectI0==NULL) {
                if(strspn(argv[i+1],"+-.eE0123456789")==strlen(argv[i+1])) {i++; cstrOutputRectI0 = argv[i];} else {std::cout << "CrabFitsImageCopy: Input Error! -rect should be followed with 4 number values (i0,j0,i1,j1)!" << std::endl; return -1; }
                if(strspn(argv[i+1],"+-.eE0123456789")==strlen(argv[i+1])) {i++; cstrOutputRectJ0 = argv[i];} else {std::cout << "CrabFitsImageCopy: Input Error! -rect should be followed with 4 number values (i0,j0,i1,j1)!" << std::endl; return -1; }
                if(strspn(argv[i+1],"+-.eE0123456789")==strlen(argv[i+1])) {i++; cstrOutputRectI1 = argv[i];} else {std::cout << "CrabFitsImageCopy: Input Error! -rect should be followed with 4 number values (i0,j0,i1,j1)!" << std::endl; return -1; }
                if(strspn(argv[i+1],"+-.eE0123456789")==strlen(argv[i+1])) {i++; cstrOutputRectJ1 = argv[i];} else {std::cout << "CrabFitsImageCopy: Input Error! -rect should be followed with 4 number values (i0,j0,i1,j1)!" << std::endl; return -1; }
                continue;
            }
            continue;
            
        }
        if(strncmp(argv[i],"-debug",6)==0) {
            i++; debug++; continue;
        }
        //if(strncmp(argv[i],"-out",4)==0 && i<argc-1) {
        //    i++; cstrOutputFilePath = argv[i]; continue;
        //}
        //if(strncmp(argv[i],"-buf",3)==0 && i<argc-1) {
        //    i++; cstrBufferX = argv[i]; continue;
        //    if(i<argc-1) { if(strspn(argv[i+1],"+-.eE0123456789")==strlen(argv[i+1])) {i++; cstrBufferY=argv[i];} else {cstrBufferY=cstrBufferX;} } else {cstrBufferY=cstrBufferX;} continue;
        //}
        if(cstrInputFilePath==NULL && i<=argc-1) { cstrInputFilePath = argv[i]; continue; }
        if(cstrOutputFilePath==NULL && i<=argc-1) { cstrOutputFilePath = argv[i]; continue; }
    }
    //
    //
    if(cstrInputFilePath!=NULL && cstrOutputFilePath!=NULL)
    {
        //
        // get fits extension number
        int InputExtNumber = 0; if(cstrInputExtNumber!=NULL) { InputExtNumber=atoi(cstrInputExtNumber); }
        int OutputExtNumber = 0; if(cstrOutputExtNumber!=NULL) { OutputExtNumber=atoi(cstrOutputExtNumber); }
        //
        // print message
        //std::cout << "CrabFitsImageCopy: from " << cstrInputFilePath << " extension=" << InputExtNumber << " to " << cstrInputFilePath << " extension=" << OutputExtNumber << std::endl;
        //
        // <TODO>
        int   errStatus = 0;
        char *cstrInputFitsHeader = NULL;
        char *cstrOutputFitsHeader = NULL;
        long  InputFitsHeaderPos = 0;
        long  InputFitsHeaderLen = 0;
        long  OutputFitsHeaderPos = 0;
        long  OutputFitsHeaderLen = 0;
        //
        // read input fits header
        errStatus = readFitsHeader(cstrInputFilePath,InputExtNumber,&cstrInputFitsHeader,&InputFitsHeaderPos,&InputFitsHeaderLen);
        if(errStatus!=0) { std::cout << "CrabFitsImageCopy: Error! Failed to read \"" << cstrInputFilePath << "\" extension=" << InputExtNumber << "!" << std::endl; return errStatus; } else {
            std::cout << "CrabFitsImageCopy: readFitsHeader " << cstrInputFilePath << " extension=" << InputExtNumber << " headersize=" << strlen(cstrInputFitsHeader) << std::endl; }
        //
        // read output fits header
        errStatus = readFitsHeader(cstrOutputFilePath,OutputExtNumber,&cstrOutputFitsHeader,&OutputFitsHeaderPos,&OutputFitsHeaderLen);
        if(errStatus!=0) { std::cout << "CrabFitsImageCopy: Error! Failed to read \"" << cstrOutputFilePath << "\" extension=" << OutputExtNumber << "!" << std::endl; return errStatus; } else {
            std::cout << "CrabFitsImageCopy: readFitsHeader " << cstrOutputFilePath << " extension=" << OutputExtNumber << " headersize=" << strlen(cstrOutputFitsHeader) << std::endl; }
        //
        // get input and output fits NAXIS
        char *cstrInputNAXIS1 = extKeyword("NAXIS1",cstrInputFitsHeader);
        char *cstrInputNAXIS2 = extKeyword("NAXIS2",cstrInputFitsHeader);
        char *cstrOutputNAXIS1 = extKeyword("NAXIS1",cstrOutputFitsHeader);
        char *cstrOutputNAXIS2 = extKeyword("NAXIS2",cstrOutputFitsHeader);
        //
        // if input and output fits NAXIS are all good
        if(cstrInputNAXIS1!=NULL && cstrInputNAXIS2!=NULL && cstrOutputNAXIS1!=NULL && cstrOutputNAXIS2!=NULL) {
            //
            // read input fits NAXIS
            int InputImageWidth = atoi(cstrInputNAXIS1);
            int InputImageHeight = atoi(cstrInputNAXIS2);
            int OutputImageWidth = atoi(cstrOutputNAXIS1);
            int OutputImageHeight = atoi(cstrOutputNAXIS2);
            //
            // get input rect
            InputRectI0 = 0; if(cstrInputRectI0!=NULL) { InputRectI0 = atoi(cstrInputRectI0); }
            InputRectJ0 = 0; if(cstrInputRectJ0!=NULL) { InputRectJ0 = atoi(cstrInputRectJ0); }
            InputRectI1 = InputImageWidth-1; if(cstrInputRectI1!=NULL) { InputRectI1 = atoi(cstrInputRectI1); }
            InputRectJ1 = InputImageHeight-1; if(cstrInputRectJ1!=NULL) { InputRectJ1 = atoi(cstrInputRectJ1); }
            //
            // get copy data size
            int CopyDataWidth = InputRectI1-InputRectI0+1;
            int CopyDataHeight = InputRectJ1-InputRectJ0+1;
            //
            // get output rect
            OutputRectI0 = 0; if(cstrOutputRectI0!=NULL) { OutputRectI0 = atoi(cstrOutputRectI0); }
            OutputRectJ0 = 0; if(cstrOutputRectJ0!=NULL) { OutputRectJ0 = atoi(cstrOutputRectJ0); }
            OutputRectI1 = CopyDataWidth-1; if(cstrOutputRectI1!=NULL) { OutputRectI1 = atoi(cstrOutputRectI1); }
            OutputRectJ1 = CopyDataHeight-1; if(cstrOutputRectJ1!=NULL) { OutputRectJ1 = atoi(cstrOutputRectJ1); }
            //
            // if output rect is smaller than input rect, then make input rect smaller
            if((InputRectI1-InputRectI0+1)>(OutputRectI1-OutputRectI0+1)) {
                int RefineRectI1 = ((InputRectI1-InputRectI0+1)-(OutputRectI1-OutputRectI0+1));
                InputRectI1 -= RefineRectI1; CopyDataWidth -= RefineRectI1;
            }
            if((InputRectJ1-InputRectJ0+1)>(OutputRectJ1-OutputRectJ0+1)) {
                int RefineRectJ1 = ((InputRectJ1-InputRectJ0+1)-(OutputRectJ1-OutputRectJ0+1));
                InputRectJ1 -= RefineRectJ1; CopyDataHeight -= RefineRectJ1;
            }
            //
            // read CopyDataArray from the input fits image
            double *CopyDataArray = NULL;
            if(InputRectI0==0 && InputRectJ0==0 && InputRectI1==InputImageWidth-1 && InputRectJ1==InputImageHeight-1) {
                CopyDataArray = readFitsImage(cstrInputFilePath,InputExtNumber);
                if(CopyDataArray==NULL) {
                    std::cout << "CrabFitsImageCopy: Error! Failed to read the input fits image! Calling readFitsImage(\"" << cstrInputFilePath << "\"," << InputExtNumber << ") failed! Please contact 1054@gmx.com and send command line output for debug! Thank you!" << std::endl; return -1;
                }
            } else {
                CopyDataArray = readFitsImageBlock(cstrInputFilePath,InputRectI0,InputRectJ0,CopyDataWidth,CopyDataHeight,InputExtNumber);
                if(CopyDataArray==NULL) {
                    std::cout << "CrabFitsImageCopy: Error! Failed to read the input fits image! Calling readFitsImageBlock(\"" << cstrInputFilePath << "\"," << InputRectI0 << "," << InputRectJ0 << "," << CopyDataWidth << "," << CopyDataHeight << "," << InputExtNumber << ") failed! Please contact 1054@gmx.com and send command line output for debug! Thank you!" << std::endl; return -1;
                }
            }
            //
            // print message
            std::cout << "CrabFitsImageCopy: Copying InputImage Rect (" << InputRectI0 << "," << InputRectJ0 << ";" << InputRectI1 << "," << InputRectJ1 << ") to OutputImage Rect (" << OutputRectI0 << "," << OutputRectJ0 << ";" << OutputRectI1 << "," << OutputRectJ1 << ")" << std::endl;
            //
            // copy to output fits file
            errStatus = modifyFitsImage(CopyDataArray, CopyDataWidth, CopyDataHeight, OutputRectI0, OutputRectJ0, cstrOutputFilePath, OutputExtNumber, debug);
            //
            // check error status
            if(errStatus==0) {
                free(CopyDataArray); CopyDataArray = NULL;
            } else {
                std::cout << "CrabFitsImageCopy: Error! Failed to modify the output fits file! Calling modifyFitsImage(" << "CopyDataArray" << ", CopyDataWidth=" << CopyDataWidth << ", CopyDataHeight=" << CopyDataHeight << ", OutputRectI0=" << OutputRectI0 << ", OutputRectJ0=" << OutputRectJ0 << ", cstrOutputFilePath=\"" << cstrOutputFilePath << "\", OutputExtNumber=" << OutputExtNumber << ", debug=" << debug << ") failed! Please contact 1054@gmx.com and send command line output for debug! Thank you!" << std::endl; return -1;
            }
        } else {
            std::cout << "CrabFitsImageCopy: Error! Input or output fits header does not contain NAXIS1 and NAXIS2!" << std::endl; return -1;
        }
    } else {
        std::cout << "Usage: " << std::endl;
        std::cout << "    CrabFitsImageCopy input.fits -ext 3 -rect 20 20 120 120 output.fits" << std::endl;
        std::cout << "    CrabFitsImageCopy input.fits -ext 3 -rect 20 20 120 120 output.fits -ext 2 -rect 60 60 180 180" << std::endl;
    }
    return 0;
}
