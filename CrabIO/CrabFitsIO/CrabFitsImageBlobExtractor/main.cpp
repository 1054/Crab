/*
 
 ; 
 ; Extract blobs in an input fits image.
 ; 
 ; A blob is defined as a group of pixels forming a hill or valley like surface in 3D.
 ;
 ; 2 input arguments: InputImage, output file name.
 ;
 ;
 
 Please compile like this:
 
     clang++ -std=c++11 -I.. main.cpp ../CrabFitsIO.cpp ../CrabFitsWcsClass.cpp -o CrabFitsImageBlobExtractor_mac
 
 Last update:
     
     2019-06-18   adapted from previously developed CrabBlobExtractor
 
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath> // NAN, std::isnan
#include <fstream>
#include <iostream>
#include <iomanip>
#include <chrono> // for high_resolution_clock
#include "CrabFitsIO.h"
#include "CrabFitsWcsClass.h"
#include "CrabFitsImageBlobClass.h"

using namespace std;

// extern char *readFitsHeader(const char *FilePath);

int main(int argc, char **argv)
{
    /*
     Read user input arguments.
     Note that argv[0] is the path and name of the program itself.
    */
    char *cstrFilePath = NULL; std::string FilePath;
    char *cstrExtNumber = NULL; int ExtNumber = 0;
    char *cstrOutputName = NULL; std::string OutputName;
    
    int debug = 0; // <TODO><DEBUG>
    
    for(int i=1; i<argc; i++) {
        if(strncmp(argv[i],"-ext",4)==0 && i<argc-1) {
            if(cstrExtNumber==NULL) {i++; cstrExtNumber = argv[i]; ExtNumber = atoi(cstrExtNumber); continue;}
            continue;
        }
        if(strcasecmp(argv[i],"-debug")==0 ) {
            debug++; continue;
        }
        if(cstrFilePath==NULL && i<=argc-1) { cstrFilePath = argv[i]; FilePath = std::string(cstrFilePath); continue; }
        if(cstrOutputName==NULL && i<=argc-1) { cstrOutputName = argv[i]; OutputName = std::string(cstrOutputName); continue; }
    }
    //
    // debug elapsed time, record starting time
    auto timeit_start = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> timeit_elapsed = std::chrono::high_resolution_clock::now() - timeit_start;
    if(debug>=2) {
        std::cout << "Elapsed time: " << timeit_elapsed.count() << " s" << std::endl;
    }
    //
    // check user input
    if(cstrFilePath!=NULL && cstrOutputName!=NULL)
    {
        //
        // Wcs
        CrabFitsWcsClass *fitsWcs = new CrabFitsWcsClass(cstrFilePath);
        if(debug>=1) {
            fitsWcs->print_wcs_info();
        }
        //
        // debug elapsed time
        if(debug>=2) {
            timeit_elapsed = std::chrono::high_resolution_clock::now() - timeit_start;
            std::cout << "DEBUG: elapsed time: " << timeit_elapsed.count() << " s" << std::endl;
        }
        //
        // read fits header
        int errStatus = 0;
        long posHeader = -1;
        long lenHeader = -1;
        char *cstrHeader = NULL;
        errStatus = readFitsHeader(cstrFilePath,ExtNumber,&cstrHeader,&posHeader,&lenHeader);
        std::cout << "CrabFitsImageBlobExtractor: readFitsHeader \"" << cstrFilePath << "\" extension=" << ExtNumber << " headersize=" << strlen(cstrHeader) << std::endl;
        //
        // debug elapsed time
        if(debug>=2) {
            timeit_elapsed = std::chrono::high_resolution_clock::now() - timeit_start;
            std::cout << "DEBUG: elapsed time: " << timeit_elapsed.count() << " s" << std::endl;
        }
        //
        // read fits Naxis
        char *cstrNAXIS1 = extKeyword("NAXIS1",cstrHeader);
        char *cstrNAXIS2 = extKeyword("NAXIS2",cstrHeader);
        if(cstrNAXIS1!=NULL && cstrNAXIS2!=NULL) {
            //
            // read fits Naxis
            long ImageWidth = atol(cstrNAXIS1);
            long ImageHeight = atol(cstrNAXIS2);
            //
            // read fits image
            double *ImageData = readFitsImage(cstrFilePath,ExtNumber);
            std::cout << "CrabFitsImageBlobExtractor: readFitsImage \"" << cstrFilePath << "\" extension=" << ExtNumber << " imagesize=" << ImageWidth << "x" << ImageHeight << std::endl;
            //
            // debug elapsed time
            if(debug>=2) {
                timeit_elapsed = std::chrono::high_resolution_clock::now() - timeit_start;
                std::cout << "DEBUG: elapsed time: " << timeit_elapsed.count() << " s" << std::endl;
            }
            //
            // find maxima
            //while()
            if(1) {
                //CrabFitsImageBlobClass FindBlob(ImageData, ImageWidth, ImageHeight, 41, 210);
                //CrabFitsImageBlobClass FindBlob(ImageData, ImageWidth, ImageHeight, 85, 166);
                CrabFitsImageBlobClass FindBlob(ImageData, ImageWidth, ImageHeight, 95, 172);
                
            }
            //
            // debug elapsed time
            if(debug>=2) {
                timeit_elapsed = std::chrono::high_resolution_clock::now() - timeit_start;
                std::cout << "DEBUG: elapsed time: " << timeit_elapsed.count() << " s. All done." << std::endl;
            }
        } else {
            std::cout << "CrabFitsImageBlobExtractor: Error! Fits header of extension " << ExtNumber << " does not contain NAXIS1 and NAXIS2!" << std::endl;
        }
    } else {
        std::cout << "Aim: " << std::endl;
        std::cout << "    This code finds blobs in an input fits image." << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Usage: " << std::endl;
        std::cout << "    CrabFitsImageBlobExtractor FITS_IMAGE.fits OUTPUT_TEXT_FILE.txt" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Last update:" << std::endl;
        std::cout << "    2019-06-17 Heidelberg" << std::endl;
    }
    return 0;
}
