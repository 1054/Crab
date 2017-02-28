/*
 
 
 Please compile like this:
 
     clang++ CrabFitsIO.cpp pdbi-uvt-to-fits.cpp -o pdbi-uvt-to-fits
     clang++ CrabFitsIO.cpp pdbi-uvt-to-fits.cpp -o pdbi-uvt-to-fits-1024-linux-x86-64
     clang++ -std=c++11 CrabFitsIO.cpp pdbi-uvt-to-fits.cpp -o pdbi-uvt-to-fits-linux-x86-64
 
 
 Last update:
 
     2015-03-25
     2016-06-26 UVTHEADSIZE
     2016-07-04 auto determine header size
 
 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>     /* atoi */
#include <stdint.h>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "CrabFitsIO.h"

using namespace std;

bool isFloat(string s) { // http://stackoverflow.com/questions/447206/c-isfloat-function
    istringstream iss(s);
    float dummy;
    iss >> noskipws >> dummy;
    return iss && iss.eof();     // Result converted to bool
}

int main(int argc, char **argv)
{
    /**
     // You’ll notice that argv[0] is the path and name of the program itself.
     std::cout << "CrabTableReadInfo: Input " << argc-1 << " arguments." << std::endl;
     for (int i = 0; i < argc; ++i) {
     std::cout << argv[i] << std::endl;
     }
     **/
    
    int Vargc = 0;
    int Nchan = 0;
    int Nstoke = 0;
    int Nheader = 0;
    int Verbose = 3; // <TODO> Verbose
    std::string strInputMap;
    std::string strOutputMap;
    int         intHeaderType = 1; // lonHeaderBytes = 512*intHeaderType
    int         intStokesNumb = 1;
    uint64_t    lonHeaderBytes = 0;
    uint64_t    lonNAXIS1 = 0;
    uint64_t    lonNAXIS2 = 0;
    double      dblCDELT1 = 0.0;
    double      dblCDELT2 = 0.0;
    double      dblCRPIX1 = 0.0;
    double      dblCRPIX2 = 0.0;
    double      dblCRVAL1 = 0.0;
    double      dblCRVAL2 = 0.0;
    for(int i=1; i<argc; i++) {
        if( (0==strcmp(argv[i],"-h")) ||
           (0==strcmp(argv[i],"-help")) ||
           (0==strcmp(argv[i],"--help")) ) {
            std::cout << std::endl;
            std::cout << "pdbi-uvt-to-fits: " << std::endl;
            std::cout << std::endl;
            std::cout << "    This code will convert a PdBI uvt data to a fits image, " << std::endl;
            std::cout << "    which has a dimension of 10xN. The 10 columns of x-axis " << std::endl;
            std::cout << "    are u, v, scan, date, time, ant, ant, Re, Im, and Weight " << std::endl;
            std::cout << "    respectively. The number of rows of y-axis is the number " << std::endl;
            std::cout << "    of all visibilities. " << std::endl;
            std::cout << "    The *.uvt data are the output of GILDAS/MAPPING e.g. " << std::endl;
            std::cout << "        MAPPING> write uv temp-data" << std::endl;
            std::cout << "    which will write a temp-data.uvt file, then we run" << std::endl;
            std::cout << "        > pdbi-uvt-to-fits temp-data.uvt" << std::endl;
            std::cout << "    which will output a temp-data.uvt.fits image. " << std::endl;
            std::cout << std::endl;
            std::cout << "    TODO: only support uvt data with 512 or 1024 bytes header. Default is -header 1024." << std::endl;
            std::cout << "    TODO: only support uvt data with float values. This will not change according to GILDAS." << std::endl;
            std::cout << std::endl;
            return -5;
        } else {
            if(0==strcmp(argv[i],"-polar") || 0==strcmp(argv[i],"-polarization") || 0==strcmp(argv[i],"-stokes")) {
                if((i+1)<argc){i++;if(atoi(argv[i])>0){Nstoke=atoi(argv[i]);continue;}else{Nstoke=-1;}}else{Nstoke=-1;}
                if(-1==Nstoke){std::cout << "pdbi-uvt-to-fits: Error! Please set valid number of -polar!" << std::endl;return -1;}
            }
            if(0==strcmp(argv[i],"-chan") || 0==strcmp(argv[i],"-channel")) {
                if((i+1)<argc){i++;if(atoi(argv[i])>0){Nchan=atoi(argv[i]);continue;}else{Nchan=-1;}}else{Nchan=-1;}
                if(-1==Nchan){std::cout << "pdbi-uvt-to-fits: Error! Please set valid number of -channel!" << std::endl;return -1;}
            }
            if(0==strcmp(argv[i],"-header")) {
                if((i+1)<argc){i++;if(atoi(argv[i])>0){Nheader=atoi(argv[i]);continue;}else{Nheader=-1;}}else{Nheader=-1;}
                if(-1==Nheader){std::cout << "pdbi-uvt-to-fits: Error! Please set valid number of -header!" << std::endl;return -1;}
            }
            Vargc++;
            if(strInputMap.empty()) {strInputMap = std::string(argv[i]); continue;}
            if(strOutputMap.empty()) {strOutputMap = std::string(argv[i]); continue;}
        }
    }
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    if(Vargc>=1) {
        //
        // set default output file
        if(strOutputMap.empty()) {strOutputMap = strInputMap+".fits";}
        //
        // prepare variables
        std::ifstream fp;
        std::vector<float> fd;
        char  *strHeader = NULL;
        float *fltVarPtr = NULL;
        long  *lonVarPtr = NULL;
        int   *intVarPtr = NULL;
        char  *strVarPtr = NULL;
        long   fpSize = 0;
        long   fdSize = 0;
        bool   isLittleEndian = false; // <TODO>
        //
        // open uvt binary file
        fp.open(strInputMap.c_str(),ios::in|ios::binary);
        if(fp.is_open()) {
            //
            // prepare to read header
            strVarPtr = new char[8]; strVarPtr[8-1] = 0x0;
            intVarPtr = new int;
            lonVarPtr = new long;
            //
            // read header
            if(0==lonHeaderBytes) {
                // auto determine header bytes
                lonHeaderBytes = 512;
                strHeader = (char *)malloc(sizeof(char)*(lonHeaderBytes+1));
                memset(strHeader,0x0,sizeof(char)*(lonHeaderBytes+1));
                fp.read(strHeader,lonHeaderBytes*sizeof(char));
                // auto determine header type
                memcpy(strVarPtr,strHeader+0xE8,7); // memcpy(strVarPtr,strHeader+0x164,7);
                if(0==strcmp(strVarPtr,"UV-DATA")) {
                    intHeaderType = 1;
                    // header type 1    --- header size 512   --- 0xE8  "UV-DATA"
                    // header type n>=2 --- header size 512*n --- 0x164 "UV-DATA" --- n is indicated by byte 0x18
                } else {
                    memset(strVarPtr,0x0,8);
                    memcpy(strVarPtr,strHeader+0x18,1); // 0x18 is the mark byte --- <TODO> make sure of this!
                    intHeaderType = (int)(*strVarPtr);  // 0x18 is the mark byte
                }
                // read the rest header bytes
                for(int j=1; j<intHeaderType; j++) {
                    free(strHeader); lonHeaderBytes+=512;
                    strHeader = (char *)malloc(sizeof(char)*(lonHeaderBytes+1));
                    fp.seekg(0,ios::beg);
                    fp.read(strHeader,lonHeaderBytes*sizeof(char));
                }
                // print debug message
                if(Verbose>=1) {
                    std::cout << "Header type is " << intHeaderType << " Code " << strVarPtr << std::endl; // <DEBUG>
                }
            } else {
                // otherwise if user input lonHeaderBytes
                intHeaderType = lonHeaderBytes/512;
                strHeader = (char *)malloc(sizeof(char)*(lonHeaderBytes+1));
                memset(strHeader,0x0,sizeof(char)*(lonHeaderBytes+1));
                fp.read(strHeader,lonHeaderBytes*sizeof(char)); // fp.seekg(512,ios::beg);
                // auto determine header type
                memcpy(strVarPtr,strHeader+0x18,1); // 0x18 is the mark byte --- <TODO> make sure of this!
                intHeaderType = (int)(*strVarPtr); // 0x18 is the mark byte
                // print debug message
                if(Verbose>=1) {
                    std::cout << "Header type is " << intHeaderType << " Code " << strVarPtr << std::endl; // <DEBUG>
                }
            }
            free(strVarPtr);
            //
            // check header format
            if((std::string(strHeader+0x0,strHeader+0x0+12) != std::string("GILDAS<UVFIL")) &&
               (std::string(strHeader+0x0,strHeader+0x0+12) != std::string("GILDAS-UVFIL"))) {
                if(Verbose>=1) {
                    std::cout << "Checking header starting string " << std::string(strHeader+0x0,strHeader+0x0+12) << std::endl; // <DEBUG>
                }
                std::cout << "Error! header is not starting with \"GILDAS-UVFIL\"! Wrong file? " << std::endl;
                return -1;
            }
            //
            // Check NAXIS1 NAXIS2
            if(lonNAXIS1<=0 || lonNAXIS2<=0) {
                // NAXIS1 NAXIS2 are 4 byte int for header type 1, while 8 byte long for header type >=2.
                if(Verbose>=1) {
                    std::cout << "Reading NAXIS1 NAXIS2 from header " << std::endl;
                }
                if(Verbose>=2) {
                    // std::cout << "Reading NAXIS1 NAXIS2 " << "sizeof(long)=" << sizeof(long) << std::endl; // what if sizeof(long)==4? -- So now use uint64_t
                    std::cout << "Reading NAXIS1 NAXIS2 " << "sizeof(uint64_t)=" << sizeof(uint64_t) << std::endl;
                }
                if(1==intHeaderType) {
                    strVarPtr = strHeader+0x30;
                    memcpy(&lonNAXIS1,strVarPtr,4); strVarPtr+=4;
                    memcpy(&lonNAXIS2,strVarPtr,4); strVarPtr+=4;
                } else {
                    strVarPtr = strHeader+0x50;
                    memcpy(&lonNAXIS1,strVarPtr,8); strVarPtr+=8;
                    memcpy(&lonNAXIS2,strVarPtr,8); strVarPtr+=8;
                }
                if(Verbose>=1) {
                    std::cout << "Checking NAXIS1 NAXIS2 " << lonNAXIS1 << " " << lonNAXIS2 << std::endl;
                }
                if(lonNAXIS1<=0 || lonNAXIS2<=0) {
                    std::cout << "Error! NAXIS1 NAXIS2 are not given or determined! " << std::endl;
                    return -1;
                }
            }
            //
            // Check CRPIX1 CRVAL1 CDELT1 CRPIX2 CRVAL2 CDELT2
            if(dblCRPIX1<=0 || dblCRPIX2<=0) {
                std::cout << "Reading CRPIX CRVAL CDELT from header " << std::endl;
                // CRPIX CRVAL CDELT are 8 byte double for all header types.
                if(1==intHeaderType) {
                    strVarPtr = strHeader+0x40;
                } else {
                    strVarPtr = strHeader+0xA8;
                }
                memcpy(&dblCRPIX1,strVarPtr,8); strVarPtr+=8;
                memcpy(&dblCRVAL1,strVarPtr,8); strVarPtr+=8;
                memcpy(&dblCDELT1,strVarPtr,8); strVarPtr+=8;
                memcpy(&dblCRPIX2,strVarPtr,8); strVarPtr+=8;
                memcpy(&dblCRVAL2,strVarPtr,8); strVarPtr+=8;
                memcpy(&dblCDELT2,strVarPtr,8); strVarPtr+=8;
                if(Verbose>=1) {
                    std::cout << "Checking CRPIX1 CRVAL1 CDELT1 " << dblCRPIX1 << " " << dblCRVAL1 << " " << dblCDELT1 << " " << std::endl;
                    std::cout << "Checking CRPIX2 CRVAL2 CDELT2 " << dblCRPIX2 << " " << dblCRVAL2 << " " << dblCDELT2 << " " << std::endl;
                }
                if(dblCRPIX1<=0 || dblCRPIX2<=0) {
                    std::cout << "Error! CRPIX1 CRPIX2 are not given or determined! " << std::endl;
                    return -1;
                }
            }
            //
            // Check Stokes
            if(intHeaderType>1 && intStokesNumb==1) {
                // <TODO> DO NOT KNOW WHERE THE OLD 512 BYTE HEADER STORES THE STOKES NUMBER
                // <TODO> seems that (most?) PdBI data are only single stokes.
                // <TODO> So we only read stokes number from header for header type n>=2 data.
                strVarPtr = strHeader+0x2E0;
                memcpy(&intStokesNumb,strVarPtr,4); strVarPtr+=4;
                if(Verbose>=1) {
                    std::cout << "Checking Stokes " << intStokesNumb << std::endl;
                    std::cout << "Checking Stokes " << intStokesNumb << std::endl;
                }
                if(intStokesNumb<=0) {
                    std::cout << "Error! Stokes is not given or determined! " << std::endl;
                    return -1;
                }
            }
            //
            //
            // return 0;
            //
            //
            //
            //
            //
            //
            //
            //
            //
            //
            //
            //
            //
            //
            //
            //
            //
            //
            //
            //
            int jZero=0, iZero=0;
            int j=0; 
            int jHead=lonHeaderBytes;
            int jStok=intStokesNumb;
            int jChan=(lonNAXIS1-7)/3; // gildas uv table format
            if(Nheader>0){jHead=Nheader;} //<TODO> User input UV table header size in bytes
            if(Nstoke>0){jStok=Nstoke;} //<TODO> User input UV table stokes number
            if(Nchan>0){jChan=Nchan;} //<TODO> User input UV table channel number
            int jNCol = 7+3*jChan*jStok; // gildas uv table format: 

            // gildas uv table format: u,v,w,date,date,n1,n2,Chan1_Polar1_Re,Chan1_Polar1_Im,Chan1_Polar1_Wt,
            //                                               Chan1_Polar2_Re,Chan1_Polar2_Im,Chan1_Polar2_Wt,
            //                                               Chan2_Polar1_Re,Chan2_Polar1_Im,Chan2_Polar1_Wt,
            //                                               Chan2_Polar2_Re,Chan2_Polar2_Im,Chan2_Polar2_Wt,
            //                                               Chan3_Polar1_Re,Chan3_Polar1_Im,Chan3_Polar1_Wt,
            //                                               Chan3_Polar2_Re,Chan3_Polar2_Im,Chan3_Polar2_Wt,
            //                                               ...

            std::cout << "pdbi-uvt-to-fits: uv table header size is " << jHead << " bytes" << std::endl;
            std::cout << "pdbi-uvt-to-fits: uv table polar number is " << jStok << "" << std::endl;
            std::cout << "pdbi-uvt-to-fits: uv table channel number is " << jChan << "" << std::endl;
            std::cout << "pdbi-uvt-to-fits: uv table column number is " << jNCol << "" << std::endl;


            // read gildas uv table 
            if(1==0) { // <TODO> -- choose double type or float type -- generally gildas uv table is float type
                // double type -- one visibility row contains 10*8 = 80 bytes
                double *ddvar = NULL;
                strVarPtr = new char[sizeof(double)];
                fp.seekg(0,ios::end); fpSize=fp.tellg(); fdSize=fpSize-jHead; // get file size and data block size
                fp.seekg(jHead,ios::beg); // seek to the begining of data block
                while(!fp.eof() && j*jNCol*sizeof(double)<fdSize) { // <corrected><20150227><dzliu> eof does not always right.
                    for(int i=0; i<jNCol; i++) { 
                        for(int k=0; k<sizeof(double); k++) {
                            if(isLittleEndian) {
                                fp.read(strVarPtr+sizeof(double)-1-k,sizeof(char));
                            } else {
                                fp.read(strVarPtr+k,sizeof(char));
                            }
                        }
                        ddvar = (double *)strVarPtr;
                        fd.push_back((float)(*ddvar));
                        if((*ddvar)==0) {iZero++;} else {iZero=0;} // remove the trailing zero zero
                        // debug
                        // if(i<=1 && j<=1) {
                        //     std::cout << (*ddvar) << std::endl;
                        //     break;
                        // }
                    }
                    if(iZero>0) {jZero++;} else {jZero=0;} // remove the trailing zero zero
                    j++;
                }
                fp.close();
            } else {
                // float type -- one visibility row contains 10*4 = 40 bytes
                // std::cout << "sizeof(float) = " << sizeof(float) << std::endl;
                strVarPtr = new char[sizeof(float)];
                fp.seekg(0,ios::end); fpSize=fp.tellg(); fdSize=fpSize-jHead; // get file size and data block size
                fp.seekg(jHead,ios::beg); // seek to the begining of data block
                while(!fp.eof() && j*jNCol*sizeof(float)<fdSize) { // <corrected><20150227><dzliu> eof does not always right.
                    for(int i=0; i<jNCol; i++) { // gildas uv table format: u,v,w,date,date,n1,n2,chan1real,chan1imagine,chan1weight,cha2...
                        for(int k=0; k<sizeof(float); k++) {
                            if(isLittleEndian) {
                                fp.read(strVarPtr+sizeof(float)-1-k,sizeof(char));
                            } else {
                                fp.read(strVarPtr+k,sizeof(char));
                            }
                        }
                        fltVarPtr = (float *)strVarPtr;
                        fd.push_back(*fltVarPtr);
                        if((*fltVarPtr)==0) {iZero++;} else {iZero=0;} // remove the trailing zero zero
                        // debug
                        // if(i<=1 && j<=1) {
                        //     std::cout << (*fltVarPtr) << std::endl;
                        //     break;
                        // }
                    }
                    if(iZero>0) {jZero++;} else {jZero=0;} // remove the trailing zero zero
                    //<debug><20150227><dzliu>// if(j==73799){std::cout << j << " " << j*jNCol << " " << fdSize << " " << fp.tellg() << " " << fp.eof() << std::endl;}
                    //<debug><20150227><dzliu>// if(j==73800){std::cout << j << " " << j*jNCol << " " << fdSize << " " << fp.tellg() << " " << fp.eof() << std::endl;}
                    j++;
                }
                fp.close();
            }
            // remove the trailing zero zero
            if(jZero>0) {
                std::cout << "pdbi-uvt-to-fits: we found " << jZero << " trailing zero zero and will trim them." << std::endl;
                for(int i=0; i<jZero*jNCol; i++) {
                    fd.pop_back();
                }
                j-=jZero;
            }
            // write data to fits
            if(fd.size() == j*jNCol) {
                int checked = writeFitsFS(fd.data(),jNCol,j,strOutputMap.c_str());
                std::cout << "pdbi-uvt-to-fits: successfully read " << j << " visibilities from " << strInputMap << std::endl;
                std::cout << "pdbi-uvt-to-fits: successfully saved to " << strOutputMap << std::endl;
            }
        }
    } else {
        std::cout << "pdbi-uvt-to-fits -header 1024 -chan 64 -polar 1 aaa.uvt aaa.fits" << std::endl;
    }
    return 0;
}

