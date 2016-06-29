/*
 
 
 Please compile like this:
 
     clang++ CrabFitsIO.cpp pdbi-uvt-to-fits.cpp -o pdbi-uvt-to-fits
     clang++ CrabFitsIO.cpp pdbi-uvt-to-fits.cpp -o pdbi-uvt-to-fits-1024-linux-x86-64
 
 
 Last update:
 
     2015-03-25
     2016-06-26 UVTHEADSIZE
 
 
 */

#include <stdio.h>
#include <stdlib.h>     /* atoi */
#include <string.h>
#include <vector>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "CrabFitsIO.h"
//#define UVTHEADSIZE 512
#define UVTHEADSIZE 1024

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
    int Nchan = 0;
    int Nstoke = 0;
    int Nheader = 0;
    std::string strInputMap;
    std::string strOutputMap;
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
    if(Vargc>=1) {
        if(strOutputMap.empty()) {strOutputMap = strInputMap+".fits";}
        // open and read file
        long fpSize = 0;
        long fdSize = 0;
        std::ifstream fp;
        std::vector<float> fd;
        char *fdptr = NULL;
        float *fdvar = NULL;
        bool isLittleEndian = false; // <TODO> 
        fp.open(strInputMap.c_str(),ios::in|ios::binary);
        if(fp.is_open()) {
            int jZero=0, iZero=0;
            int j=0; 
            int jHead=UVTHEADSIZE;
            int jStok=1;
            int jChan=1;
            if(Nheader>0){jHead=Nheader;} //<TODO> uvtable header size in bytes
            if(Nstoke>0){jStok=Nstoke;} //<TODO> uvtable stokes number
            if(Nchan>0){jChan=Nchan;} //<TODO> uvtable channel number
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
                fdptr = new char[sizeof(double)];
                fp.seekg(0,ios::end); fpSize=fp.tellg(); fdSize=fpSize-jHead; // get file size and data block size
                fp.seekg(jHead,ios::beg); // seek to the begining of data block
                while(!fp.eof() && j*jNCol*sizeof(double)<fdSize) { // <corrected><20150227><dzliu> eof does not always right.
                    for(int i=0; i<jNCol; i++) { 
                        for(int k=0; k<sizeof(double); k++) {
                            if(isLittleEndian) {
                                fp.read(fdptr+sizeof(double)-1-k,sizeof(char));
                            } else {
                                fp.read(fdptr+k,sizeof(char));
                            }
                        }
                        ddvar = (double *)fdptr;
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
                fdptr = new char[sizeof(float)];
                fp.seekg(0,ios::end); fpSize=fp.tellg(); fdSize=fpSize-jHead; // get file size and data block size
                fp.seekg(jHead,ios::beg); // seek to the begining of data block
                while(!fp.eof() && j*jNCol*sizeof(float)<fdSize) { // <corrected><20150227><dzliu> eof does not always right.
                    for(int i=0; i<jNCol; i++) { // gildas uv table format: u,v,w,date,date,n1,n2,chan1real,chan1imagine,chan1weight,cha2...
                        for(int k=0; k<sizeof(float); k++) {
                            if(isLittleEndian) {
                                fp.read(fdptr+sizeof(float)-1-k,sizeof(char));
                            } else {
                                fp.read(fdptr+k,sizeof(char));
                            }
                        }
                        fdvar = (float *)fdptr;
                        fd.push_back(*fdvar);
                        if((*fdvar)==0) {iZero++;} else {iZero=0;} // remove the trailing zero zero
                        // debug
                        // if(i<=1 && j<=1) {
                        //     std::cout << (*fdvar) << std::endl;
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

