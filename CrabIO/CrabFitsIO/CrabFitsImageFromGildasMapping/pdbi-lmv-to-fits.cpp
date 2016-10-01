/*
 
 
 Please compile like this:

     clang++ CrabFitsIO.cpp pdbi-lmv-to-fits.cpp -o pdbi-lmv-to-fits
     clang++ CrabFitsIO.cpp pdbi-lmv-to-fits.cpp -o pdbi-lmv-to-fits-linux-x86-64
     cp pdbi-lmv-to-fits-linux-x86-64 /home/dzliu/Cloud/Github/Crab.Toolkit.PdBI/bin/bin_linux/pdbi-lmv-to-fits_linux_x86_64
 
 
 Last update:
     
     2015-03-25
         add argument -header to allow user to define header byte size e.g. 512 or 1024 (default is 512)
         add argument -image to allow user to define image nx ny size e.g. 1024 1024
         fixed CDELT1 not negative bug
     2016-07-04 auto determine header size
     2016-07-22 <TODO><20160722>
     2016-07-28 //<20160728>//for compatible with old glib 2.12
     2016-08-04 //<20160804>//do not check dblCRPIX3
     2016-08-04 //<20160804>//for pdbi *.beam file
 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

template <typename T>

const char *mesprintf(const char *strFormat, T dblVar) {
    int intlength = snprintf(NULL, 0, strFormat, dblVar);
    // printf("\nmesprintf: format = %s intlength = %d value = %g\n", strFormat, intlength, dblVar);
    char *stroutput = new char[intlength+1];
    memset(stroutput, 0x0, intlength+1);
    snprintf(stroutput, intlength+1, strFormat, dblVar);
    return stroutput;
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
    std::string strPointingRA; // in degree
    std::string strPointingDec; // in degree
    std::string strPixScaleX; // in degree
    std::string strPixScaleY; // in degree
    int         intHeaderType = 1; // lonHeaderBytes = 512*intHeaderType
    int         intStokesNumb = 1;
    uint64_t    lonHeaderBytes = 0;
    uint64_t    lonNAXIS1 = 0;
    uint64_t    lonNAXIS2 = 0;
    uint64_t    lonNAXIS3 = 0;
    double      dblCDELT1 = 0.0;
    double      dblCDELT2 = 0.0;
    double      dblCDELT3 = 0.0;
    double      dblCRPIX1 = 0.0;
    double      dblCRPIX2 = 0.0;
    double      dblCRPIX3 = 0.0;
    double      dblCRVAL1 = 0.0;
    double      dblCRVAL2 = 0.0;
    double      dblCRVAL3 = 0.0; // frequency axis
    double      dblPointingRA = 0.0; // degree
    double      dblPointingDec = 0.0; // degree
    double      dblRestFrequency = 0.0; // Rest Frequency
    double      dblOffsFrequency = 0.0; // Offset in Frequency
    double      dblResoFrequency = 0.0; // Resolution in Frequency
    double      dblPixScaleX = 0.0;
    double      dblPixScaleY = 0.0;
    long        lonImageSizeX = 0;
    long        lonImageSizeY = 0;
    for(int i=1; i<argc; i++) {
        if( (0==strcasecmp(argv[i],"-h")) ||
            (0==strcasecmp(argv[i],"-help")) ||
           (0==strcasecmp(argv[i],"--help")) ) {
            std::cout << std::endl;
            std::cout << "pdbi-lmv-to-fits: " << std::endl;
            std::cout << std::endl;
            std::cout << "    This code will convert a PdBI lmv map to a fits image. " << std::endl;
            std::cout << "    This means we can use *.lmv *.lmv-clean *.lmv-res and " << std::endl;
            std::cout << "    *.beam files as the input. " << std::endl;
            std::cout << "    These files are the output of GILDAS/MAPPING e.g. " << std::endl;
            std::cout << "        MAPPING> write dirty temp-dirty" << std::endl;
            std::cout << "    which will write a temp-dirty.lmv file, then we run" << std::endl;
            std::cout << "        > pdbi-lmv-to-fits temp-dirty.lmv" << std::endl;
            std::cout << "    which will output a temp-dirty.lmv.fits image. " << std::endl;
            std::cout << std::endl;
            std::cout << "    TODO: only support lmv data with 512 bytes header. " << std::endl;
            std::cout << "    TODO: only support lmv data with 128x128 map size. " << std::endl;
            std::cout << "    TODO: only support lmv data with float values. " << std::endl;
            std::cout << std::endl;
            return -5;
        } else if( (0==strcasecmp(argv[i],"-pointing")) ||
                   (0==strcasecmp(argv[i],"-radec")) ||
                   (0==strcasecmp(argv[i],"--pointing")) ||
                   (0==strcasecmp(argv[i],"--radec")) ) {
            if((i+2)>(argc-1)) {
                std::cout << "Error! Pointing ra dec invalid! Please set it like this: -pointing 150.000 02.000 " << std::endl;
                return -1;
            } else {
                strPointingRA = std::string(argv[i+1]);
                strPointingDec = std::string(argv[i+2]);
                if(!isFloat(strPointingRA) || !isFloat(strPointingDec)) {
                    std::cout << "Error! Pointing ra dec invalid! Please set it like this: -pointing 150.000 02.000 " << std::endl;
                    return -1;
                }
                i++; i++;
            }
        } else if( (0==strcasecmp(argv[i],"-pixscale")) ||
                  (0==strcasecmp(argv[i],"--pixscale")) ) {
            if((i+1)>(argc-1)) {
                std::cout << "Error! Pixscale invalid! Please set it like this: -pixscale 1.0 " << std::endl;
                return -1;
            } else {
                std::string PixScaleStr = std::string(argv[i+1]);
                if(!isFloat(PixScaleStr)) {
                    std::cout << "Error! Pixscale invalid! Please set it like this: -pixscale 1.0 " << std::endl;
                    return -1;
                }
                double PixScaleVar = strtod(argv[i+1],NULL);
                ostringstream PixScaleBuf; PixScaleBuf << (PixScaleVar/3600.0); //PixScaleBuf << (-PixScaleVar/3600.0);
                strPixScaleX = PixScaleBuf.str();
                //PixScaleBuf.str(""); PixScaleBuf.clear(); PixScaleBuf << (PixScaleVar/3600.0);
                strPixScaleY = PixScaleBuf.str();
                dblPixScaleX = -strtod(strPixScaleX.c_str(), NULL);//<20160728>//for compatible with old glib 2.12
                dblPixScaleY = strtod(strPixScaleY.c_str(), NULL);//<20160728>//for compatible with old glib 2.12
                //<20160728>//dblPixScaleX = -std::stod(strPixScaleX);//<20160728>//for compatible with old glib 2.12
                //<20160728>//dblPixScaleY = std::stod(strPixScaleY);//<20160728>//for compatible with old glib 2.12
                i++; 
            }
        } else if( (0==strcasecmp(argv[i],"-cdelt")) ||
                   (0==strcasecmp(argv[i],"--cdelt")) ) {
            if((i+2)>(argc-1)) {
                std::cout << "Error! CDELT x y invalid! Please set it like this: -cdelt -5E-4 1E-4 " << std::endl;
                return -1;
            } else {
                strPixScaleX = std::string(argv[i+1]);
                strPixScaleY = std::string(argv[i+2]);
                if(!isFloat(strPixScaleX) || !isFloat(strPixScaleY)) {
                    std::cout << "Error! CDELT x y invalid! Please set it like this: -cdelt -5E-4 1E-4 " << std::endl;
                    return -1;
                }
                dblPixScaleX = strtod(strPixScaleX.c_str(), NULL);//<20160728>//for compatible with old glib 2.12
                dblPixScaleY = strtod(strPixScaleY.c_str(), NULL);//<20160728>//for compatible with old glib 2.12
                //<20160728>//dblPixScaleX = std::stod(strPixScaleX);//<20160728>//for compatible with old glib 2.12
                //<20160728>//dblPixScaleY = std::stod(strPixScaleY);//<20160728>//for compatible with old glib 2.12
                i++; i++;
            }
        } else if( (0==strcasecmp(argv[i],"-header")) ||
                   (0==strcasecmp(argv[i],"-headersize")) ||
                   (0==strcasecmp(argv[i],"--header")) ||
                   (0==strcasecmp(argv[i],"--headersize")) ) {
            if((i+1)>(argc-1)) {
                std::cout << "Error! header size invalid! Please set it like this: -headersize 512 " << std::endl;
                return -1;
            } else {
                Nheader = atol(argv[i+1]);
                i++; 
            }
        } else if( (0==strcasecmp(argv[i],"-image")) ||
                   (0==strcasecmp(argv[i],"-imagesize")) ||
                   (0==strcasecmp(argv[i],"--image")) ||
                   (0==strcasecmp(argv[i],"--imagesize")) ) {
            if((i+2)>(argc-1)) {
                std::cout << "Error! header size invalid! Please set it like this: -imagesize 1024 1024 " << std::endl;
                return -1;
            } else {
                if(!isFloat(argv[i+1]) || !isFloat(argv[i+2])) {
                    std::cout << "Error! header size invalid! Please set it like this: -imagesize 1024 1024 " << std::endl;
                    return -1;
                }
                lonImageSizeX = atol(argv[i+1]);
                lonImageSizeY = atol(argv[i+2]);
                i++; i++;
            }
        } else {
            if(strInputMap.empty()) {strInputMap = std::string(argv[i]); Vargc++; continue;}
            if(strOutputMap.empty()) {strOutputMap = std::string(argv[i]); Vargc++; continue;}
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
        std::vector<std::vector<float> > fcube;
        char  *strHeader = NULL;
        float *fltVarPtr = NULL;
        long  *lonVarPtr = NULL;
        int   *intVarPtr = NULL;
        char  *strVarPtr = NULL;
        bool isLittleEndian = true; // <TODO>
        //
        // open lmv binary file
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
                if(0==strcmp(strVarPtr,"RA     ")) {
                    intHeaderType = 1;
                    // header type 1    --- header size 512   --- 0xE8  "RA     "
                    // header type n>=2 --- header size 512*n --- 0x17C "RA     " --- n is indicated by byte 0x18
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
            if((std::string(strHeader+0x0,strHeader+0x0+12) != std::string("GILDAS<IMAGE")) &&
               (std::string(strHeader+0x0,strHeader+0x0+12) != std::string("GILDAS-IMAGE"))) {
                if(Verbose>=1) {
                    std::cout << "Checking header starting string " << std::string(strHeader+0x0,strHeader+0x0+12) << std::endl; // <DEBUG>
                }
                std::cout << "Error! header is not starting with \"GILDAS<IMAGE\"! Wrong file? " << std::endl;
                return -1;
            }
            //
            // Check NAXIS1 NAXIS2 NAXIS3
            if(lonNAXIS1<=0 || lonNAXIS2<=0 || lonNAXIS3<=0) {
                // NAXIS1 NAXIS2 NAXIS3 are 4 byte int for header type 1, while 8 byte long for header type >=2.
                if(Verbose>=1) {
                    std::cout << "Reading NAXIS1 NAXIS2 NAXIS3 from header " << std::endl;
                }
                if(Verbose>=2) {
                    // std::cout << "Reading NAXIS1 NAXIS2 NAXIS3 " << "sizeof(long)=" << sizeof(long) << std::endl; // what if sizeof(long)==4? -- So now use uint64_t
                    std::cout << "Reading NAXIS1 NAXIS2 NAXIS3 " << "sizeof(uint64_t)=" << sizeof(uint64_t) << std::endl;
                }
                if(1==intHeaderType) {
                    strVarPtr = strHeader+0x30;
                    memcpy(&lonNAXIS1,strVarPtr,4); strVarPtr+=4;
                    memcpy(&lonNAXIS2,strVarPtr,4); strVarPtr+=4;
                    memcpy(&lonNAXIS3,strVarPtr,4); strVarPtr+=4;
                } else {
                    strVarPtr = strHeader+0x50;
                    memcpy(&lonNAXIS1,strVarPtr,8); strVarPtr+=8;
                    memcpy(&lonNAXIS2,strVarPtr,8); strVarPtr+=8;
                    memcpy(&lonNAXIS3,strVarPtr,8); strVarPtr+=8;
                }
                if(Verbose>=1) {
                    std::cout << "Checking NAXIS1 NAXIS2 NAXIS3 " << lonNAXIS1 << " " << lonNAXIS2 << " " << lonNAXIS3 << std::endl;
                }
                // Check valid NAXIS value
                //<20160804>// if(lonNAXIS1<=0 || lonNAXIS2<=0 || lonNAXIS3<=0) {
                //<20160804>//     std::cout << "Error! NAXIS1 NAXIS2 NAXIS3 are not given or determined! " << std::endl;
                //<20160804>//     return -1;
                //<20160804>// }
                if(lonNAXIS1<=0 || lonNAXIS2<=0) {
                    std::cout << "Error! NAXIS1 NAXIS2 are negative values! This should not happen? Exit! " << std::endl;
                    return -1;
                }
            }
            //
            // Check CRPIX1 CRVAL1 CDELT1 CRPIX2 CRVAL2 CDELT2 CRPIX3 CRVAL3 CDELT3
            if(dblCRPIX1<=0 || dblCRPIX2<=0 || dblCRPIX3<=0) {
                std::cout << "Reading CRPIX CRVAL CDELT from header " << std::endl;
                // CRPIX CRVAL CDELT are 8 byte double for all header types.
                if(1==intHeaderType) {
                    strVarPtr = strHeader+0x40; // <NOTE> different from UV table data (strHeader+0x40)
                } else {
                    strVarPtr = strHeader+0xA8; // <NOTE> different from UV table data (strHeader+0xA8) // <TODO><20160722> some need to +3*8
                }
                memcpy(&dblCRPIX1,strVarPtr,8); strVarPtr+=8;
                memcpy(&dblCRVAL1,strVarPtr,8); strVarPtr+=8;
                memcpy(&dblCDELT1,strVarPtr,8); strVarPtr+=8;
                memcpy(&dblCRPIX2,strVarPtr,8); strVarPtr+=8;
                memcpy(&dblCRVAL2,strVarPtr,8); strVarPtr+=8;
                memcpy(&dblCDELT2,strVarPtr,8); strVarPtr+=8;
                memcpy(&dblCRPIX3,strVarPtr,8); strVarPtr+=8;
                memcpy(&dblCRVAL3,strVarPtr,8); strVarPtr+=8;
                memcpy(&dblCDELT3,strVarPtr,8); strVarPtr+=8;
                if(Verbose>=1) {
                    std::cout << "Checking CRPIX1 CRVAL1 CDELT1 " << dblCRPIX1 << " " << dblCRVAL1 << " " << dblCDELT1/3.141592653589793*180.0 << " " << std::endl;
                    std::cout << "Checking CRPIX2 CRVAL2 CDELT2 " << dblCRPIX2 << " " << dblCRVAL2 << " " << dblCDELT2/3.141592653589793*180.0 << " " << std::endl;
                    std::cout << "Checking CRPIX3 CRVAL3 CDELT3 " << dblCRPIX3 << " " << dblCRVAL3 << " " << dblCDELT3 << " " << std::endl;
                }
                // Check valid pixel value
                //<20160804>// if(dblCRPIX1<=0 || dblCRPIX2<=0 || dblCRPIX3<=0) {
                //<20160804>//     std::cout << "Error! CRPIX1 CRPIX2 CRPIX3 are not given or determined! " << std::endl;
                //<20160804>//     return -1;
                //<20160804>// }
                if(dblCRPIX1<=0 || dblCRPIX2<=0 ) { //<20160804>// do not check dblCRPIX3
                    std::cout << "Error! CRPIX1 CRPIX2 are negative values! This should not happen? Exit! " << std::endl;
                    return -1;
                }
                // Further convert to string type
                if( (dblPixScaleX<-1e-30||dblPixScaleX>1e-30) && (dblPixScaleY<-1e-30||dblPixScaleY>1e-30) ) { // check non-zero
                    strPixScaleX = mesprintf("%g", dblPixScaleX);
                    strPixScaleY = mesprintf("%g", dblPixScaleY);
                } else {
                    strPixScaleX = mesprintf("%g", dblCDELT1/3.141592653589793*180.0); // <TODO><20160722> the minus sign? GILDAS always gives positive CDELT1? // <20160728> convert from GILDAS radian to degree.
                    strPixScaleY = mesprintf("%g", dblCDELT2/3.141592653589793*180.0); // <20160728> convert from GILDAS radian to degree.
                }

                // <Added><20160728> double check to deal with the "<TODO><20160722> some need to +3*8" problem
                if(dblCRPIX1<1 || dblCRPIX2<1) {
                    //
                    strVarPtr = strVarPtr-9*8; // <Added><20160728> rewind and shift 3*8 bits and read again
                    strVarPtr = strVarPtr+3*8; // <Added><20160728> double check to deal with the "<TODO><20160722> some need to +3*8" problem
                    //
                    memcpy(&dblCRPIX1,strVarPtr,8); strVarPtr+=8;
                    memcpy(&dblCRVAL1,strVarPtr,8); strVarPtr+=8;
                    memcpy(&dblCDELT1,strVarPtr,8); strVarPtr+=8;
                    memcpy(&dblCRPIX2,strVarPtr,8); strVarPtr+=8;
                    memcpy(&dblCRVAL2,strVarPtr,8); strVarPtr+=8;
                    memcpy(&dblCDELT2,strVarPtr,8); strVarPtr+=8;
                    memcpy(&dblCRPIX3,strVarPtr,8); strVarPtr+=8;
                    memcpy(&dblCRVAL3,strVarPtr,8); strVarPtr+=8;
                    memcpy(&dblCDELT3,strVarPtr,8); strVarPtr+=8;
                    if(Verbose>=1) {
                        std::cout << "Checking CRPIX1 CRVAL1 CDELT1 " << dblCRPIX1 << " " << dblCRVAL1 << " " << dblCDELT1/3.141592653589793*180.0 << " " << std::endl;
                        std::cout << "Checking CRPIX2 CRVAL2 CDELT2 " << dblCRPIX2 << " " << dblCRVAL2 << " " << dblCDELT2/3.141592653589793*180.0 << " " << std::endl;
                        std::cout << "Checking CRPIX3 CRVAL3 CDELT3 " << dblCRPIX3 << " " << dblCRVAL3 << " " << dblCDELT3 << " " << std::endl;
                    }
                    // Check valid pixel value
                    //<20160804>// if(dblCRPIX1<=0 || dblCRPIX2<=0 || dblCRPIX3<=0) {
                    //<20160804>//     std::cout << "Error! CRPIX1 CRPIX2 CRPIX3 are not given or determined! " << std::endl;
                    //<20160804>//     return -1;
                    //<20160804>// }
                    if(dblCRPIX1<=0 || dblCRPIX2<=0) {
                        std::cout << "Error! CRPIX1 CRPIX2 are negative values! This should not happen? Exit! " << std::endl;
                        return -1;
                    }
                    // Further convert to string type
                    if( (dblPixScaleX<-1e-30||dblPixScaleX>1e-30) && (dblPixScaleY<-1e-30||dblPixScaleY>1e-30) ) { // check non-zero
                        strPixScaleX = mesprintf("%g", dblPixScaleX);
                        strPixScaleY = mesprintf("%g", dblPixScaleY);
                    } else {
                        strPixScaleX = mesprintf("%g", dblCDELT1/3.141592653589793*180.0); // <TODO><20160722> the minus sign? GILDAS always gives positive CDELT1? // <20160728> convert from GILDAS radian to degree.
                        strPixScaleY = mesprintf("%g", dblCDELT2/3.141592653589793*180.0); // <20160728> convert from GILDAS radian to degree.
                    }
                }

            }
            //
            // Check Stokes
            if(intHeaderType>1 && intStokesNumb==1) {
                // <TODO> Perhaps GILDAS lmv image only have one stokes?
            }
            //
            // Check Pointing RA Dec
            if(strPointingRA.empty() || strPointingDec.empty()) {
                if(1==intHeaderType) {
                    strVarPtr = strHeader+0x160;
                } else {
                    strVarPtr = strHeader+0x24C;
                }
                memcpy(&dblPointingRA,strVarPtr,8); strVarPtr+=8;
                memcpy(&dblPointingDec,strVarPtr,8); strVarPtr+=8;
                dblPointingRA = dblPointingRA/3.141592653589793*180.0;
                dblPointingDec = dblPointingDec/3.141592653589793*180.0;
                strPointingRA = mesprintf("%0.7f",dblPointingRA);
                strPointingDec = mesprintf("%0.7f",dblPointingDec);
                if(Verbose>=1) {
                    std::cout << "Checking Pointing RA Dec " << strPointingRA << " " << strPointingDec << " " << std::endl;
                }
                if(strPointingRA.empty() || strPointingDec.empty()) {
                    std::cout << "Error! Pointing RA Dec are not given or determined! " << std::endl;
                    return -1;
                }
            }
            //
            // Check RestFrequency ResoFrequency
            if(dblRestFrequency<=0) {
                if(1==intHeaderType) {
                    strVarPtr = strHeader+0x190; // Resolution in Frequency, Offset in Frequency, Rest frequency
                } else {
                    strVarPtr = strHeader+0x278; // Resolution in Frequency, Offset in Frequency, Rest frequency
                }
                memcpy(&dblResoFrequency,strVarPtr,8); strVarPtr+=8;
                memcpy(&dblOffsFrequency,strVarPtr,8); strVarPtr+=8;
                memcpy(&dblRestFrequency,strVarPtr,8); strVarPtr+=8;
                dblResoFrequency = dblResoFrequency/1000; // make it GHz
                dblOffsFrequency = dblOffsFrequency/1000; // make it GHz
                dblRestFrequency = dblRestFrequency/1000; // make it GHz
                if(Verbose>=1) {
                    std::cout << "Checking Rest Frequency " << dblRestFrequency << " " << std::endl;
                    std::cout << "Checking Offset in Frequency " << dblOffsFrequency << " " << std::endl;
                    std::cout << "Checking Resolution in Frequency " << dblResoFrequency << " " << std::endl;
                }
                if(dblRestFrequency<0) {
                    std::cout << "Error! Rest Frequency are not given or determined! " << std::endl;
                    return -1;
                }
            }
            //
            //
            // return 0;
            //
            //
            //<20160804>// for pdbi *.beam file
            if(lonNAXIS3<=0) {
                lonNAXIS3 = 1; //<20160804>// for pdbi *.beam file
                dblCRPIX3 = 1.0;
                dblCDELT3 = 1.0;
                dblCRVAL3 = 0.0;
                dblRestFrequency = 0.0;
                dblOffsFrequency = 0.0;
                dblResoFrequency = 0.0;
                strPointingRA = "0.0";
                strPointingDec = "0.0";
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
            int jHead=lonHeaderBytes;
            int jStok=intStokesNumb;
            int jChan=lonNAXIS3; // frequency or channel axis
            lonImageSizeX=lonNAXIS1; // image x axis
            lonImageSizeY=lonNAXIS2; // image x axis
            if(Nheader>0){jHead=Nheader;} //<TODO> User input UV table header size in bytes
            if(Nstoke>0){jStok=Nstoke;} //<TODO> User input UV table stokes number
            if(Nchan>0){jChan=Nchan;} //<TODO> User input UV table channel number
            //
            // prepare fits file to write
            char *header = generateFitsHeaderFS(lonImageSizeX,lonImageSizeY);
            if(!strPointingRA.empty() && !strPointingDec.empty()) {
                addKeyword("CTYPE1","'RA---TAN'",&header);
                addKeyword("CTYPE2","'DEC--TAN'",&header);
                addKeyword("EQUINOX","2000.0",&header);
                addKeyword("CRPIX1",mesprintf("%g",dblCRPIX1),&header); // printf("\ndebug:%d\n%s\n",header,header);
                addKeyword("CRPIX2",mesprintf("%g",dblCRPIX2),&header); // printf("\ndebug:%d\n%s\n",header,header);
                addKeyword("CRVAL1",strPointingRA.c_str(),&header);
                addKeyword("CRVAL2",strPointingDec.c_str(),&header);
            }
            if(!strPixScaleX.empty() && !strPixScaleY.empty()) {
                addKeyword("CDELT1",strPixScaleX.c_str(),&header);
                addKeyword("CDELT2",strPixScaleY.c_str(),&header);
                addKeyword("CROTA","0",&header); // radio projection, no rota.
            }
            if(1) {
                addKeyword("NAXIS","3",&header);
                addKeyword("NAXIS3",mesprintf("%ld",lonNAXIS3),&header);
                addKeyword("CTYPE3","'Frequency'",&header);
                addKeyword("CRPIX3",mesprintf("%g",dblCRPIX3),&header);
                addKeyword("CRVAL3",mesprintf("%g",dblRestFrequency),&header);
                addKeyword("CDELT3",mesprintf("%g",dblResoFrequency),&header);
            }
            if(0) {
                // <DEBUG> addKeyword
                for(int k=1; k<50; k++) {
                    std::cout << "Testing addKeyword " << mesprintf("'TEST%02d'",k) << std::endl;
                    addKeyword(mesprintf("TEST%02d",k),mesprintf("'TEST%02d'",k),&header);
                }
            }
            if(Verbose>=5) {
                printf("\ndebug: NAXIS3 = %ld\n", lonNAXIS3);
                printf("\ndebug: NAXIS3 = %s\n", mesprintf("%ld",lonNAXIS3));
                printf("\ndebug:\n%s\n", header);
            }
            //
            //
            //return 0;
            //
            // print message
            if(Verbose>=1) {
                std::cout << "Writing to " << strOutputMap << std::endl;
            }
            //
            // open fits file to write
            std::ofstream fpout;
            fpout.open(strOutputMap.c_str(),std::ofstream::binary);
            fpout.write(header, strlen(header));
            //
            // read lmv file and write into fits file
            // -- strVarPtr = new char[1];
            // -- while(fp.read(strVarPtr,1)) {
            // --     fpout << strVarPtr;
            // -- } --> endian problem!
            //
            // -- (if data structure is [[[Channels]XAxis]YAxis]
            // -- long lonInPos = fp.tellg();
            // -- long lonOutPos = fpout.tellp();
            // -- strVarPtr = new char[4*lonImageSizeX*lonImageSizeY]; // <TODO> must be 4 byte float
            // -- for(int j=0; j<jChan; j++) {
            // --     fp.seekg(lonInPos+4*j);
            // --     memset(strVarPtr,0x0,4*lonImageSizeX*lonImageSizeY);
            // --     for(int k=0; k<lonImageSizeX*lonImageSizeY; k++) {
            // --         fp.read(strVarPtr+4*k, 4);
            // --     }
            // --     //fpout.seekp(lonOutPos+4*lonImageSizeX*lonImageSizeY*j, ios::cur);
            // --     fpout.write(strVarPtr,4*lonImageSizeX*lonImageSizeY);
            // -- }
            // --
            //
            strVarPtr = new char[4]; memset(strVarPtr,0x0,4);
            while(fp.read(strVarPtr,4)) {
                fpout.write(strVarPtr+3,1);
                fpout.write(strVarPtr+2,1);
                fpout.write(strVarPtr+1,1);
                fpout.write(strVarPtr+0,1);
            }
            //
            fpout.close();
            fp.close();
            //
            //
//            return 0;
//            // write data to fits
//            for(int jch=0; jch<jChan; jch++) {
//                if(fd.size() == lonImageSizeX*lonImageSizeY) {
//                    char *header = generateFitsHeaderFS(lonImageSizeX,lonImageSizeY);

//                    // if(header) { printf("debug header %d (strlen=%d)\n", header, strlen(header)); }

//                    // <Added><20150309><dzliu> add pointing info: CRPIX1 CRPIX2 CRVAL1 CRVAL2 CDELT1 CDELT2 CROT2
//                    // GILDAS UV TABLE HEADER FORMAT (headersize=512)
//                    // +048 number of axis 1 freq (int)
//                    // +052 number of visibilities (int)
//                    // +064 Reference Pixel frequency (double)
//                    // +072 central frequency (double) -- very important to the uvmap!
//                    // +340 2000 probably J2000 (float)
//                    // +424 Resolution in Velocity (float)
//                    // +428 Offset in Velocity (float)
//                    // +296 Source Name
//                    // +388 Line Name
//                    // +456 Noise
//                    // +308 30ba2b10 29fa0440 f50580de 56e1a43f   44d3bf3e 9f871040 5798cd8f 239fe73f 0000fa44 24000000 03000000
//                    //               2.077769          1.288127   0.374659 2.258278      0.0   1.8095   2000.0
//                    // +352 30ba2b10 29fa0440 f50580de 56e1a43f   00000000 00000000 01000000 02000000
//                    // +384 30000000 LineName
//                    //
//                    // GILDAS UV TABLE HEADER FORMAT (headersize=1024)
//                    // +080 number of axis 1 freq (int)
//                    // +088 number of visibilities (int)
//                    // +168 Reference Pixel frequency (double)
//                    // +176 central frequency (double) -- very important to the uvmap!
//                    // +576 2000 probably J2000 (float)
//                    // +656 Resolution in Velocity (float)
//                    // +660 Offset in Velocity (float)
//                    // ---> then the Rest Frequency in the header is computed = Reso Freq * 2.99792458e5 / Reso Velo.
//                    //
//                    // char *sCRPIX1_raw = (char *)malloc(sizeof(char)*(10+1));
//                    // memset(sCRPIX1_raw,0x0,sizeof(char)*(10+1));
//                    // for(int k=0; k<sizeof(float); k++) { if(isLittleEndian) { strncpy(sCRPIX1_raw+sizeof(float)-1-k,strHeader+128+k,1); } else { strncpy(sCRPIX1_raw+k,strHeader+128+k,1); } }
//                    // float *fCRPIX1 = (float *)sCRPIX1_raw;
//                    //
//                    if(!strPointingRA.empty() && !strPointingDec.empty()) {
//                        addKeyword("CTYPE1","'RA---TAN'",&header);
//                        addKeyword("CTYPE2","'DEC--TAN'",&header);
//                        addKeyword("EQUINOX","2000.0",&header);
//                        char strCRPIX1[20]; char strCRPIX2[20];
//                        int cokCRPIX1 = sprintf(strCRPIX1,"%15.3f",float(lonImageSizeX+2)/2.0); // <TODO> +2)/2.0 +1)/2.0 // "65"
//                        int cokCRPIX2 = sprintf(strCRPIX2,"%15.3f",float(lonImageSizeY+2)/2.0); // <TODO> +2)/2.0 +1)/2.0 // "65"
//                        // printf("\ndebug: NAXIS %ld %ld\n\n",lonImageSizeX,lonImageSizeY);
//                        // printf("\ndebug: CRPIX %s %s (%d %d)\n\n",strCRPIX1,strCRPIX2,cokCRPIX1,cokCRPIX2);
//                        addKeyword("CRPIX1",strCRPIX1,&header); // printf("\ndebug:%d\n%s\n",header,header);
//                        addKeyword("CRPIX2",strCRPIX2,&header); // printf("\ndebug:%d\n%s\n",header,header);
//                        addKeyword("CRVAL1",strPointingRA.c_str(),&header);
//                        addKeyword("CRVAL2",strPointingDec.c_str(),&header);
//                    }
//                    if(!strPixScaleX.empty() && !strPixScaleY.empty()) {
//                        addKeyword("CDELT1",strPixScaleX.c_str(),&header);
//                        addKeyword("CDELT2",strPixScaleY.c_str(),&header);
//                        addKeyword("CROTA","0",&header);
//                    }

//                    // int checked = writeFitsFS(fd.data(),128,128,strOutputMap.c_str());
//                    int checked = writeFitsF(fd.data(),header,strOutputMap.c_str());
//                    std::cout << "pdbi-lmv-to-fits: successfully saved to " << strOutputMap << std::endl;
//                }
//            }
        }
    } else {
        std::cout << "Usage: " << std::endl;
        std::cout << "    pdbi-lmv-to-fits aaa.lmv aaa.fits" << std::endl;
        std::cout << "    pdbi-lmv-to-fits aaa.lmv-clean aaa.fits" << std::endl;
        std::cout << "    pdbi-lmv-to-fits aaa.lmv-clean aaa.fits -pointing 150.000 20.000" << std::endl;
        std::cout << "    pdbi-lmv-to-fits aaa.lmv-clean aaa.fits -pointing 150.000 20.000 -pixscale 0.52" << std::endl;
        std::cout << "    pdbi-lmv-to-fits aaa.lmv-clean aaa.fits -pointing 150.000 20.000 -cdelt -0.0001444444 0.0001444444" << std::endl;
        std::cout << "    pdbi-lmv-to-fits aaa.lmv-clean aaa.fits -headersize 1024 -imagesize 1024 1024" << std::endl;
        std::cout << std::endl;
    }
    return 0;
}

