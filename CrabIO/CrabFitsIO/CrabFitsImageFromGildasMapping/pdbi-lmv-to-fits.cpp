/*
 
 
 Please compile like this:
 
     clang++ CrabFitsIO.cpp pdbi-lmv-to-fits.cpp -o pdbi-lmv-to-fits
 
 
 Last update:
     
     2015-03-25
         add argument -header to allow user to define header byte size e.g. 512 or 1024 (default is 512)
         add argument -image to allow user to define image nx ny size e.g. 1024 1024
         fixed CDELT1 not negative bug
 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "CrabFitsIO.h"


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
    
    int Vargc = 0;
    std::string strInputMap;
    std::string strOutputMap;
    std::string strPointingRA;
    std::string strPointingDec;
    std::string strPixScaleX;
    std::string strPixScaleY;
    long        lonHeaderBytes = 512;
    long        lonImageSizeX = 128;
    long        lonImageSizeY = 128;
    for(int i=1; i<argc; i++) {
        if( (0==strcmp(argv[i],"-h")) ||
            (0==strcmp(argv[i],"-help")) ||
           (0==strcmp(argv[i],"--help")) ) {
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
        } else if( (0==strcmp(argv[i],"-pointing")) ||
                  (0==strcmp(argv[i],"-radec")) ||
                  (0==strcmp(argv[i],"--pointing")) ||
                  (0==strcmp(argv[i],"--radec")) ) {
            if((i+2)>(argc-1)) {
                std::cout << "Error! Pointing ra dec invalid! Please set it like this: -pointing 150.000 02.000 " << std::endl;
                return -1;
            } else {
                strPointingRA = std::string(argv[i+1]);
                strPointingDec = std::string(argv[i+2]);
                i++; i++;
            }
        } else if( (0==strcmp(argv[i],"-pixscale")) ||
                  (0==strcmp(argv[i],"--pixscale")) ) {
            if((i+1)>(argc-1)) {
                std::cout << "Error! Pixscale invalid! Please set it like this: -pixscale 1.0 " << std::endl;
                return -1;
            } else {
                double PixScaleVar = strtod(argv[i+1],NULL);
                ostringstream PixScaleBuf; PixScaleBuf << (-PixScaleVar/3600.0);
                strPixScaleX = PixScaleBuf.str();
                PixScaleBuf.str(""); PixScaleBuf.clear(); PixScaleBuf << (PixScaleVar/3600.0);
                strPixScaleY = PixScaleBuf.str();
                i++; 
            }
        } else if( (0==strcmp(argv[i],"-cdelt")) ||
                  (0==strcmp(argv[i],"--cdelt")) ) {
            if((i+2)>(argc-1)) {
                std::cout << "Error! CDELT x y invalid! Please set it like this: -cdelt 1.0 0.9 " << std::endl;
                return -1;
            } else {
                strPixScaleX = std::string(argv[i+1]);
                strPixScaleY = std::string(argv[i+2]);
                i++; i++;
            }
        } else if( (0==strcmp(argv[i],"-header")) ||
                   (0==strcmp(argv[i],"-headersize")) ||
                   (0==strcmp(argv[i],"--header")) ||
                   (0==strcmp(argv[i],"--headersize")) ) {
            if((i+1)>(argc-1)) {
                std::cout << "Error! header size invalid! Please set it like this: -headersize 512 " << std::endl;
                return -1;
            } else {
                lonHeaderBytes = atol(argv[i+1]);
                i++; 
            }
        } else if( (0==strcmp(argv[i],"-image")) ||
                   (0==strcmp(argv[i],"-imagesize")) ||
                   (0==strcmp(argv[i],"--image")) ||
                   (0==strcmp(argv[i],"--imagesize")) ) {
            if((i+2)>(argc-1)) {
                std::cout << "Error! header size invalid! Please set it like this: -imagesize 1024 1024 " << std::endl;
                return -1;
            } else {
                lonImageSizeX = atol(argv[i+1]);
                lonImageSizeY = atol(argv[i+2]);
                i++; i++;
            }
        } else {
            if(strInputMap.empty()) {strInputMap = std::string(argv[i]); Vargc++; continue;}
            if(strOutputMap.empty()) {strOutputMap = std::string(argv[i]); Vargc++; continue;}
        }
    }
    if(Vargc>=1) {
        if(strOutputMap.empty()) {strOutputMap = strInputMap+".fits";}
        // open and read file
        std::ifstream fp;
        std::vector<float> fd;
        float *fdvar = NULL;
        char *fdptr = NULL;
        char *sHeader = (char *)malloc(sizeof(char)*(lonHeaderBytes+1));
        memset(sHeader,0x0,sizeof(char)*(lonHeaderBytes+1));
        bool isLittleEndian = false; // <TODO> 
        fp.open(strInputMap.c_str(),ios::in|ios::binary);
        if(fp.is_open()) {
            // find last of 20 20 20 20
            fdptr = new char[sizeof(float)];
            // fp.seekg(512,ios::beg);
            fp.read(sHeader,lonHeaderBytes*sizeof(char));
            for(int j=0; j<lonImageSizeY; j++) {
                for(int i=0; i<lonImageSizeX; i++) {
                    for(int k=0; k<sizeof(float); k++) {
                        if(isLittleEndian) {
                            fp.read(fdptr+sizeof(float)-1-k,sizeof(char));
                        } else {
                            fp.read(fdptr+k,sizeof(char));
                        }
                    }
                    fdvar = (float *)fdptr;
                    fd.push_back(*fdvar);
                    // debug
                    // if(i<=1 && j<=1) {
                    //     std::cout << (*fdvar) << std::endl;
                    //     break;
                    // }
                }
            }
            fp.close();
            // write data to fits
            if(fd.size() == lonImageSizeX*lonImageSizeY) {
                char *header = generateFitsHeaderFS(lonImageSizeX,lonImageSizeY);
                // if(header) { printf("debug header %d (strlen=%d)\n", header, strlen(header)); }
                
                // <Added><20150309><dzliu> add pointing info: CRPIX1 CRPIX2 CRVAL1 CRVAL2 CDELT1 CDELT2 CROT2
                // GILDAS UV TABLE HEADER FORMAT (headersize=512)
                // +048 number of axis 1 freq (int)
                // +052 number of visibilities (int)
                // +064 Reference Pixel frequency (double)
                // +072 central frequency (double) -- very important to the uvmap!
                // +340 2000 probably J2000 (float)
                // +424 Resolution in Velocity (float)
                // +428 Offset in Velocity (float)
                // +296 Source Name
                // +388 Line Name
                // +456 Noise
                // +308 30ba2b10 29fa0440 f50580de 56e1a43f   44d3bf3e 9f871040 5798cd8f 239fe73f 0000fa44 24000000 03000000
                //               2.077769          1.288127   0.374659 2.258278      0.0   1.8095   2000.0
                // +352 30ba2b10 29fa0440 f50580de 56e1a43f   00000000 00000000 01000000 02000000
                // +384 30000000 LineName
                //
                // GILDAS UV TABLE HEADER FORMAT (headersize=1024)
                // +080 number of axis 1 freq (int)
                // +088 number of visibilities (int)
                // +168 Reference Pixel frequency (double)
                // +176 central frequency (double) -- very important to the uvmap!
                // +576 2000 probably J2000 (float)
                // +656 Resolution in Velocity (float)
                // +660 Offset in Velocity (float)
                // ---> then the Rest Frequency in the header is computed = Reso Freq * 2.99792458e5 / Reso Velo. 
                // 
                // char *sCRPIX1_raw = (char *)malloc(sizeof(char)*(10+1));
                // memset(sCRPIX1_raw,0x0,sizeof(char)*(10+1));
                // for(int k=0; k<sizeof(float); k++) { if(isLittleEndian) { strncpy(sCRPIX1_raw+sizeof(float)-1-k,sHeader+128+k,1); } else { strncpy(sCRPIX1_raw+k,sHeader+128+k,1); } }
                // float *fCRPIX1 = (float *)sCRPIX1_raw;
                // 
                if(!strPointingRA.empty() && !strPointingDec.empty()) {
                    addKeyword("CTYPE1","'RA---TAN'",&header);
                    addKeyword("CTYPE2","'DEC--TAN'",&header);
                    addKeyword("EQUINOX","2000.0",&header);
                    char strCRPIX1[20]; char strCRPIX2[20];
                    int cokCRPIX1 = sprintf(strCRPIX1,"%15.3f",float(lonImageSizeX+2)/2.0); // <TODO> +2)/2.0 +1)/2.0 // "65"
                    int cokCRPIX2 = sprintf(strCRPIX2,"%15.3f",float(lonImageSizeY+2)/2.0); // <TODO> +2)/2.0 +1)/2.0 // "65"
                    // printf("\ndebug: NAXIS %ld %ld\n\n",lonImageSizeX,lonImageSizeY);
                    // printf("\ndebug: CRPIX %s %s (%d %d)\n\n",strCRPIX1,strCRPIX2,cokCRPIX1,cokCRPIX2);
                    addKeyword("CRPIX1",strCRPIX1,&header); // printf("\ndebug:%d\n%s\n",header,header);
                    addKeyword("CRPIX2",strCRPIX2,&header); // printf("\ndebug:%d\n%s\n",header,header);
                    addKeyword("CRVAL1",strPointingRA.c_str(),&header);
                    addKeyword("CRVAL2",strPointingDec.c_str(),&header);
                }
                if(!strPixScaleX.empty() && !strPixScaleY.empty()) {
                    addKeyword("CDELT1",strPixScaleX.c_str(),&header);
                    addKeyword("CDELT2",strPixScaleY.c_str(),&header);
                    addKeyword("CROTA","0",&header);
                }
                
                // int checked = writeFitsFS(fd.data(),128,128,strOutputMap.c_str());
                int checked = writeFitsF(fd.data(),header,strOutputMap.c_str());
                std::cout << "pdbi-lmv-to-fits: successfully saved to " << strOutputMap << std::endl;
            }
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

