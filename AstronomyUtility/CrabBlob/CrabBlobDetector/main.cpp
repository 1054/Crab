/* 
 
 a semi-automatic blob detecting program
 
 input 1: *.fits file --- fits image
 
 
 
 Please compile like this:
 
     clang++ -I/usr/include/malloc/ -I../ -I../../../CrabIO/CrabFitsIO -I../../../CrabIO/CrabImage -I../../../CrabIO/CrabString -I../../../CrabIO/CrabTable -L../ ../../../CrabIO/CrabFitsIO/CrabFitsIO.cpp ../../../CrabIO/CrabImage/CrabImage.cpp main.cpp -o CrabBlobDetector_mac
 
 
 Initialized:
     
     2017-04-09
 
 Last update:
 
     2017-04-09
     2017-04-10 "-number-limit"
 
 
 
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cstring>
#include <cmath>
#include <algorithm> // for sort()
#include <fstream>
#include <iostream>
#include <iomanip>
#include "CrabFitsIO.h"
#include "CrabImage.h"
#include "CrabTableReadColumn.cpp"
#include "CrabTableReadInfo.cpp"
#include "CrabBlob.cpp"
//#define DEF_Version "2017-04-09 @ MPIA"
#define DEF_Version "2017-04-10 @ MPIA"

using namespace std;






int main(int argc, char **argv)
{
    /**
     // Note that argv[0] is the path and name of the program itself.
     **/
    char *cstrInput1 = NULL;
    char *cstrInput2 = (char *)"none";
    char *cstrInput3 = (char *)"none";
    char *cstrExtNumber = (char *)"0";
    char *cstrDetectThreshold = (char *)"nan";
    char *cstrDeblendLevel = (char *)"1";
    char *cstrNumberLimit = (char *)"12";
    int   intHeaderComment = 1; // if 1 we use commented two line header, if 0 we use uncommented single line header // <Note> since 2015-12-22, we use uncommented header as default. // <Note> since 2017-03-04, we use commented header as default.
    
    int debug = 0; // <TODO><DEBUG>
    
    for(int i=1; i<argc; i++) {
        // read extension parameter
        if(strncasecmp(argv[i],"-ext",4)==0 && i<argc-1) {
            i++; cstrExtNumber = argv[i]; continue;
        }
        // read -header-no-comment option
        if(strncasecmp(argv[i],"-header-no",10)==0) { // -header-no-comment
            intHeaderComment = 0; continue;
        }
        // read -header-in-comment option
        if(strncasecmp(argv[i],"-header-in",10)==0) { // -header-in-comment
            intHeaderComment = 1; continue;
        }
        // read -detect-threshold option
        if(strncasecmp(argv[i],"-detect-threshold",17)==0 && i<argc-1) {
            i++; cstrDetectThreshold = argv[i]; continue;
        }
        // read -deblend-level option
        if(strncasecmp(argv[i],"-deblend-level",14)==0 && i<argc-1) {
            i++; cstrDeblendLevel = argv[i]; continue;
        }
        // read -number-limit option
        if(strncasecmp(argv[i],"-number-limit",13)==0 && i<argc-1) {
            i++; cstrNumberLimit = argv[i]; continue;
        }
        // read debug parameter
        if(strncasecmp(argv[i],"-debug",6)==0) {
            debug++; continue;
        }
        // read input fits file path
        if(cstrInput1==NULL && i<=argc-1) {
            cstrInput1 = argv[i]; continue;
        }
        if(cstrInput2==NULL && i<=argc-1) {
            cstrInput2 = argv[i]; continue;
        }
        if(cstrInput3==NULL && i<=argc-1) {
            cstrInput3 = argv[i]; continue;
        }
    }
    //
    //
    if(cstrInput1!=NULL && cstrInput2!=NULL && cstrInput3!=NULL)
    {
        int   errStatus = 0;
        int   extNumber = 0;  if(cstrExtNumber!=NULL) {extNumber=atoi(cstrExtNumber);}
        char *cstrHeader = NULL;
        long  posHeader = 0;
        long  lenHeader = 0;
        //
        // check
        std::cout << "# CrabBlobDetector: version " << DEF_Version << std::endl;
        // std::cout << "# CrabBlobDetector: " << cstrInput1 << " extension=" << extNumber << std::endl;
        //
        // read fits header
        errStatus = readFitsHeader(cstrInput1,extNumber,&cstrHeader,&posHeader,&lenHeader);
        std::cout << "# CrabBlobDetector: readFitsHeader " << cstrInput1 << " extension=" << extNumber << " headersize=" << strlen(cstrHeader) << std::endl;
        //
        // read fits Naxis
        char *cstrNAXIS1 = extKeyword("NAXIS1",cstrHeader);
        char *cstrNAXIS2 = extKeyword("NAXIS2",cstrHeader);
        if(cstrNAXIS1==NULL || cstrNAXIS2==NULL) {
            std::cout << "Error! Failed to read NAXIS1 and NAXIS2 from the input fits file \"" << cstrInput1 << "\"!" << std::endl;
            return -1;
        }
        //
        // read info file
        // -- so that we can apply parameter tuning
        int aperTYPE = 1; // 1: circle; 2: ellipse; 3: TODO;
        std::string tempTYPE;
        if(strncasecmp(cstrInput2,"none",strlen("none"))!=0 && strncasecmp(cstrInput2,"null",strlen("null"))!=0) {
            tempTYPE = CrabTableReadInfo(cstrInput2,"AperType");
            std::transform(tempTYPE.begin(), tempTYPE.end(), tempTYPE.begin(), ::tolower);
            if(tempTYPE.find("ellip")!= std::string::npos) {
                aperTYPE = 2;
                std::cout << "# CrabBlobDetector: apertype " << aperTYPE << " " << tempTYPE << std::endl;
            }
        }
        //
        // read fits Naxis in long format number
        long longWidth = atol(cstrNAXIS1);
        long longHeight = atol(cstrNAXIS2);
        //
        // read inintial blob parameter file (if input) <TODO>
        int aperN = 0, aperNX = 0, aperNY = 0, aperNR = 0, aperNE = 0, aperNT = 0;
        double *aperX = NULL;
        double *aperY = NULL;
        double *aperR = NULL; // aperture radius or major axis
        double *aperE = NULL; // aperture elliptic b/a
        double *aperT = NULL; // aperture rotate angle "theta"
        if(strncasecmp(cstrInput3,"none",strlen("none"))!=0 && strncasecmp(cstrInput3,"null",strlen("null"))!=0) {
            aperX = CrabTableReadColumnF(cstrInput3,1,&aperNX);
            aperY = CrabTableReadColumnF(cstrInput3,2,&aperNY);
            aperR = CrabTableReadColumnF(cstrInput3,3,&aperNR); // aperture radius or major axis
            if(1==aperTYPE) {
                aperE = (double *)malloc(aperNR*sizeof(double)); // aperture elliptic b/a
                aperT = (double *)malloc(aperNR*sizeof(double)); // aperture rotate angle "theta"
                for(int tempI = 0; tempI < aperNR; tempI++) {
                    aperE[tempI] = 1.0; aperT[tempI] = 0.0;
                }
            } else {
                if(2==aperTYPE) {
                    aperE = CrabTableReadColumnF(cstrInput3,4,&aperNE); // aperture elliptic b/a
                    aperT = CrabTableReadColumnF(cstrInput3,5,&aperNT); // aperture rotate angle "theta"
                    if(NULL==aperE) {
                        std::cout << "# CrabBlobDetector: Error! The column 4 of " << cstrInput3 << " is invalid! Abort!" << std::endl;
                        return -1;
                    }
                    if(NULL==aperT) {
                        std::cout << "# CrabBlobDetector: Error! The column 5 of " << cstrInput3 << " is invalid! Abort!" << std::endl;
                        return -1;
                    }
                }
            }
            // check aper file rows -- columns should have the same row count
            if(aperNX>0 && aperNX==aperNY && aperNY==aperNR) {
                aperN = aperNX;
            } else {
                std::cout << "# CrabBlobDetector: Error! Aperture list in " << cstrInput3 << " is invalid! Abort!" << std::endl;
                return -1;
            }
        }
        // loop each aper <TODO>
        //for(int aperK=0; aperK<aperN; aperK++) {
        //}
        
        
        
        //
        // before loop print the column header
        if(intHeaderComment>0) {
            // if intHeaderComment==1 we use commented two line header, if 0 we use uncommented single line header
            std::cout << "# " << std::endl;
            std::cout << "#"
            << setw(11) << "x"
            << setw(12) << "y"
            << setw(12) << "radius"
            << std::flush;
            std::cout << std::endl;
            std::cout << "# " << std::endl;
        } else {
            // if intHeaderComment==1 we use commented two line header, if 0 we use uncommented single line header
            std::cout << "# " << std::endl;
            std::cout << setw(12) << "x"
            << setw(12) << "y"
            << setw(12) << "radius"
            << std::flush;
            std::cout << std::endl;
        }

        
        
        
        //
        // boxlize the input fits image
        long box_size[2] = {1000,1000};
        long box_buff[2] = {100,100};
        box_size[0] = longWidth; box_size[1] = longHeight; // <TODO> do not boxlize the image
        box_buff[0] = 0; box_buff[1] = 0; // <TODO> do not boxlize the image
        
        for (long box_j = 0; box_j < longHeight; box_j+=box_size[1] ) {
            for (long box_i = 0; box_i < longWidth; box_i+=box_size[0] ) {
                
                //
                // prepare box rect
                long   iRectI0=0, iRectJ0=0, iRectI1=0, iRectJ1=0, iRectNX=0, iRectNY=0;
                double dRectI0=0, dRectJ0=0, dRectI1=0, dRectJ1=0;
                
                iRectI0 = box_i - box_buff[0];
                iRectJ0 = box_j - box_buff[1];
                iRectI1 = box_i + box_size[0]-1 + box_buff[0];
                iRectJ1 = box_j + box_size[1]-1 + box_buff[1];
                iRectNX = iRectI1 - iRectI0 + 1;
                iRectNY = iRectJ1 - iRectJ0 + 1;
                
                //
                // then calulate image crop rect (really needed for very big image -- your memory will not allow to load very big image)
                long oldLi=0,oldLj=0,oldUi=0,oldUj=0,newLi=0,newLj=0,newUi=0,newUj=0;
                CrabImageCalcCropRect(longWidth,longHeight,iRectI0,iRectJ0,iRectI1,iRectJ1,&oldLi,&oldLj,&oldUi,&oldUj,&newLi,&newLj,&newUi,&newUj);
                
                //
                // then do the image crop
                double *aperImage = NULL;
                CrabImageCopyCropRect(cstrInput1,&aperImage,iRectI0,iRectJ0,iRectI1,iRectJ1,oldLi,oldLj,oldUi,oldUj,newLi,newLj,newUi,newUj,extNumber);
                
                //
                // check the image size
                if(debug>=1) {
                    std::cout << "DEBUG: -------------------------" << std::endl;
                    std::cout << "DEBUG: iBox " << box_i << " " << box_j << std::endl;
                    std::cout << "DEBUG: Rect " << box_i-box_buff[0] << " " << box_j-box_buff[1] << std::endl;
                    std::cout << "DEBUG: Size " << iRectNX << " " << iRectNY << std::endl;
                    std::cout << "DEBUG: Data[0,0]=" << aperImage[0] << std::endl;
                    std::cout << "DEBUG: Data[" << iRectNX-1 << "," << iRectNY-1 << "]=" << aperImage[iRectNX*iRectNY-1] << std::endl;
                    std::cout << "DEBUG: Data[" << (iRectNX-1)/2 << "," << (iRectNY-1)/2 << "]=" << aperImage[iRectNX*((iRectNY-1)/2)+((iRectNX-1)/2)] << std::endl;
                    if(box_j+box_size[1] >= longHeight && box_i+box_size[0] >= longWidth) {
                        std::cout << "DEBUG: -------------------------" << std::endl;
                    }
                }
                
                // prepare blob_list
                double *input_image_data = aperImage;
                long input_image_size[2] = {iRectNX,iRectNY};
                std::vector<blob_class> blob_list;
                double input_detect_threshold = std::stod(cstrDetectThreshold);
                double input_deblend_level = std::stod(cstrDeblendLevel);
                //blob_detect(input_image_data, input_image_size, blob_list, 0.01);
                
                // prepare pixel_pool
                std::vector<long> input_pixel_pool;
                for(long jpix=0; jpix<input_image_size[1]; jpix++) {
                    for(long ipix=0; ipix<input_image_size[0]; ipix++) {
                        input_pixel_pool.push_back(jpix*input_image_size[0] + ipix);
                    }
                }
                
                // run blob find
                int input_number_limit = atoi(cstrNumberLimit);
                for(long kpix=0; kpix<input_number_limit; kpix++) {
                    // detect one "blob_item" at each time, append to "blob_list".
                    blob_class blob_item;
                    blob_item.set_debug_level(debug);
                    blob_item.init(input_image_data,input_image_size,input_pixel_pool);
                    if(input_detect_threshold==input_detect_threshold){
                        blob_item.set_detect_threshold(input_detect_threshold,true); // set absolute detect threshold
                        //std::cout << "Setting absolute detecting threshold " << input_detect_threshold << std::endl;
                    }
                    blob_item.set_deblend_level(input_deblend_level); // set deblending level,
                    int check_settle = blob_item.settle();
                    if(0==check_settle) {
                        blob_item.grow();
                        //
                        // compute rms of the whole image
                        std::vector<double> output_pixel_vec;
                        double output_pixel_rms = NAN;
                        double output_pixel_val = NAN;
                        for(long kkpix = 0; kkpix<blob_item.pixel_pool->size(); kkpix++) {
                            output_pixel_val = blob_item.image_data[blob_item.pixel_pool->at(kkpix)];
                            if(output_pixel_val==output_pixel_val) {
                                output_pixel_vec.push_back(output_pixel_val);
                            }
                        }
                        output_pixel_rms = blob_item.getsigma(output_pixel_vec);
                        std::cout << "Computing rms = " << output_pixel_rms << std::endl;
                        std::cout << "Computing xc = " << blob_item.xc.at(0)+1 << std::endl; // +1 accounts for ds9 coordinate system starting from 1 instead of 0.
                        std::cout << "Computing yc = " << blob_item.yc.at(0)+1 << std::endl; // +1 accounts for ds9 coordinate system starting from 1 instead of 0.
                        std::cout << "Computing fc = " << blob_item.fc.at(0) << std::endl;
                        std::cout << "Computing snr = " << blob_item.fc.at(0)/output_pixel_rms << std::endl;
                        std::cout << "--" << std::endl;
                        //
                        blob_list.push_back(blob_item);
                    }
                }
                
                // print blobs as ds9.reg
                blob_print_polygon(blob_list, "ds9.reg");
                
            }
            
        }
        
        //
        // free
        if(aperX) {free(aperX);} aperX=NULL;
        if(aperY) {free(aperY);} aperY=NULL;
        if(aperR) {free(aperR);} aperR=NULL;
        if(aperE) {free(aperE);} aperE=NULL;
        if(aperT) {free(aperT);} aperT=NULL;
        
    } else {
        
        //
        // print usage
        std::cout << "Usage: "
                  << "\n    CrabBlobDetector aaa.fits -detect-threshold 0.0005 -deblend-level 10 -number-limit 10"
                  << "\n        -detect-threshold is in pixel unit"
                  << "\n        -number-limit is the maximum blob number"
                  << "\n"
                  << "Version: \n    " << DEF_Version
                  << std::endl;
        
    }
    return 0;
}
