/* 
 
 a semi-automatic aper phot program
 
 input 1: *.fits file --- the galaxy image 
 input 2: *.info file --- the galaxy info (Name,z,dL)
 input 3: *.aper file --- the apertures (x,y,rad)
 
 
 
 Please compile like this:
 
     clang++ main.cpp CrabFitsIO.cpp CrabImage.cpp -o CrabPhotRingPhot_mac
 
     clang++ main.cpp CrabFitsIO.cpp CrabPhotUI.cpp -I/Users/dliu/Code/Qt/5.2.1/5.2.1/clang_64/include/ -I/Users/dliu/Code/Qt/5.2.1/5.2.1/clang_64/include/QtCore -I/Users/dliu/Code/Qt/5.2.1/5.2.1/clang_64/include/QtGui -I/Users/dliu/Code/Qt/5.2.1/5.2.1/clang_64/include/QtWidgets -L/Users/dliu/Code/Qt/5.2.1/5.2.1/clang_64/lib -lQtCore.framework -o CrabPhotRingPhot_mac
 
 Initialized:
     
     2015-01-29
 
 Last update:
 
     2015-03-23 add rms
 
 
 
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "CrabFitsIO.h"
#include "CrabImage.h"
#include "CrabTableReadColumn.cpp"

using namespace std;

int main(int argc, char **argv)
{
    /**
     // Note that argv[0] is the path and name of the program itself.
     **/
    char *cstrInput1 = NULL;
    char *cstrInput2 = NULL;
    char *cstrInput3 = NULL;
    char *cstrExtNumber = (char *)"0";
    
    int debug = 0; // <TODO><DEBUG>
    
    for(int i=1; i<argc; i++) {
        // read extension parameter
        if(strncmp(argv[i],"-ext",4)==0 && i<argc-1) {
            i++; cstrExtNumber = argv[i]; continue;
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
        std::cout << "# CrabPhotRingPhot: " << cstrInput1 << " extension=" << extNumber << std::endl;
        //
        // read fits header
        errStatus = readFitsHeader(cstrInput1,extNumber,&cstrHeader,&posHeader,&lenHeader);
        std::cout << "# CrabPhotRingPhot: readFitsHeader " << cstrInput1 << " extension=" << extNumber << " headersize=" << strlen(cstrHeader) << std::endl;
        //
        // read fits Naxis
        char *cstrNAXIS1 = extKeyword("NAXIS1",cstrHeader);
        char *cstrNAXIS2 = extKeyword("NAXIS2",cstrHeader);
        if(cstrNAXIS1!=NULL && cstrNAXIS2!=NULL) {
            //
            // read fits Naxis
            long longWidth = atol(cstrNAXIS1);
            long longHeight = atol(cstrNAXIS2);
            //
            // read aper details
            int aperN = 0, aperNX = 0, aperNY = 0, aperNR = 0;
            double *aperX = CrabTableReadColumnF(cstrInput3,1,&aperNX);
            double *aperY = CrabTableReadColumnF(cstrInput3,2,&aperNY);
            double *aperR = CrabTableReadColumnF(cstrInput3,3,&aperNR);
            if(aperNX>0 && aperNX==aperNY && aperNY==aperNR) {
                aperN = aperNX;
            } else {
                std::cout << "# CrabPhotRingPhot: Error! Aperture list in " << cstrInput3 << " is invalid! Abort!" << std::endl;
                return -1;
            }
            //
            // before loop print the column header
            std::cout << "# " << std::endl;
            std::cout << "#" << setw(11) << "x" << setw(12) << "y" << setw(12) << "radius" << setw(15) << "npix" << setw(15) << "sum" << setw(15) << "min" << setw(15) << "max" << std::flush;
            std::cout << setw(15) << "mean" << std::flush;
            std::cout << setw(15) << "rms" << std::flush;
            std::cout << setw(15) << "npix_int" << setw(15) << "sum_int" << setw(15) << "mean_int" << setw(15) << "rms_int" << std::flush;
            std::cout << std::endl;
            std::cout << "# " << std::endl;
            //
            // loop each two-aper-pair to do the photometry
            for(int aperK=1; aperK<aperN; aperK++) { // loop each two-aper-pair, so aperK starts from 1 instead of 0. 
                //
                // firstly convert aper coordinate to array index
                // we set an additional aperture radius buffer to account for fractional number
                double aperBuffer = 1.0;
                long   aperRadius = long(aperR[aperK]+aperBuffer);
                long   aperLength = aperRadius*2+1;
                long   iRectI0=0, iRectJ0=0, iRectI1=0, iRectJ1=0;
                double dRectI0=0, dRectJ0=0, dRectI1=0, dRectJ1=0;
                CrabImageConvertCen2Rect(aperX[aperK],aperY[aperK],aperRadius,aperRadius,&iRectI0,&iRectJ0,&iRectI1,&iRectJ1,&dRectI0,&dRectJ0,&dRectI1,&dRectJ1);
                //
                // then calulate image crop rect (really needed for very big image -- your memory will not allow to load very big image)
                long oldLi=0,oldLj=0,oldUi=0,oldUj=0,newLi=0,newLj=0,newUi=0,newUj=0;
                CrabImageCalcCropRect(longWidth,longHeight,iRectI0,iRectJ0,iRectI1,iRectJ1,&oldLi,&oldLj,&oldUi,&oldUj,&newLi,&newLj,&newUi,&newUj);
                //
                // then do the image crop
                double *aperImage = NULL;
                CrabImageCopyCropRect(cstrInput1,&aperImage,iRectI0,iRectJ0,iRectI1,iRectJ1,oldLi,oldLj,oldUi,oldUj,newLi,newLj,newUi,newUj,extNumber);
                // <TODO> debug
                // CrabImageCopyCropRect(cstrInput1,&aperImage,iRectI0,iRectJ0,iRectI1,iRectJ1,oldLi,oldLj,oldUi,oldUj,newLi,newLj,newUi,newUj,extNumber,debug);
                //
                // check the image size
                if(debug) {
                    // std::cout << cstrInput1 << " " << aperImage << " " << iRectI0 << " " << iRectJ0 << " " << iRectI1 << " " << iRectJ1 << " " << oldLi << " " << oldLj << " " << oldUi << " " << oldUj << " " << newLi << " " << newLj << " " << newUi << " " << newUj << " " << std::endl;
                    std::cout << "DEBUG: iRectIL=" << iRectI1 - iRectI0 + 1 << " iRectJL=" << iRectJ1 - iRectJ0 + 1 << std::endl;
                    std::cout << "DEBUG: aperRadius=" << aperRadius << " aperImage=" << aperImage << std::endl;
                    std::cout << "DEBUG: aperRadius=" << aperRadius << " fPix000=" << aperImage[0] << std::endl;
                    std::cout << "DEBUG: aperRadius=" << aperRadius << " fPix001=" << aperImage[1] << std::endl;
                    std::cout << "DEBUG: aperRadius=" << aperRadius << " fCenPix=" << aperImage[aperRadius*aperLength+aperRadius] << std::endl;
                }
                //
                // ok, now we got the cutout image aperImage, with a rect size (int(aperRad)+1)*(int(aperRad)+1), we still need to make a circle mask to select only pixels within the circle
                double *aperImaRX = (double *)malloc(aperLength*aperLength*sizeof(double));
                double *aperImaRY = (double *)malloc(aperLength*aperLength*sizeof(double));
                double *aperImaRD = (double *)malloc(aperLength*aperLength*sizeof(double)); // distabce array
                double *aperMask1 = (double *)malloc(aperLength*aperLength*sizeof(double)); // mask of pix in aperture circle 1 (0 means out, 1 means in, fractional means at the edge)
                double  aperSumM1 = 0.0; // sum pix numb in aperture circle 1 (no fractional part)
                double  aperSumN1 = 0.0; // sum pix numb in aperture circle 1
                double  aperSumS1 = 0.0; // sum pix flux in aperture circle 1 (no fractional part)
                double  aperSumF1 = 0.0; // sum pix flux in aperture circle 1
                double  aperRmsS1 = 0.0; // root-mean-square (abbreviated "RMS" and sometimes called the quadratic mean (no fractional part)
                double  aperRmsF1 = 0.0; // root-mean-square (abbreviated "RMS" and sometimes called the quadratic mean
                double  aperMaxF1 = NAN; // max pix flux in aperture circle 1
                double  aperMinF1 = NAN; // min pix flux in aperture circle 1
                for(long tempK=0; tempK<aperLength*aperLength; tempK++) {
                    if(aperImage[tempK]==aperImage[tempK]) { // <TODO> check non NAN
                        aperImaRX[tempK] = (double(tempK % aperLength)) - (double(aperRadius)) - (double(aperX[aperK])-long(aperX[aperK])); // <corrected><20150220><dzliu> fractional difference
                        aperImaRY[tempK] = (double(tempK / aperLength)) - (double(aperRadius)) - (double(aperY[aperK])-long(aperY[aperK])); // <corrected><20150220><dzliu> fractional difference
                        aperImaRD[tempK] = sqrt(aperImaRX[tempK]*aperImaRX[tempK] + aperImaRY[tempK]*aperImaRY[tempK]);
                        if(aperImaRD[tempK]>aperR[aperK-1] && aperImaRD[tempK]<=aperR[aperK]) { // (sum ignoring fractional part)
                            aperSumS1 += aperImage[tempK];
                            if(aperImage[tempK]>0.0) {aperRmsS1 += aperImage[tempK]*aperImage[tempK];}
                            aperSumM1 += 1.0;
                            // <debug><20150220><dzliu> std::cout << setw(10) << aperImage[tempK] << setw(10) << aperImaRD[tempK] << setw(10) << aperImaRX[tempK] << setw(10) << aperImaRY[tempK] << std::endl;
                        }
                        if(aperImaRD[tempK]>aperR[aperK-1]+0.5 && aperImaRD[tempK]<=aperR[aperK]-0.5) {
                            aperMask1[tempK] = 1.0;
                            aperSumF1 += aperImage[tempK];
                            if(aperImage[tempK]>0.0) {aperRmsF1 += aperImage[tempK]*aperImage[tempK];}
                            aperSumN1 += 1.0;
                            if(aperMinF1!=aperMinF1) {aperMinF1 = aperImage[tempK];} else if(aperImage[tempK]<aperMinF1) {aperMinF1 = aperImage[tempK];}
                            if(aperMaxF1!=aperMaxF1) {aperMaxF1 = aperImage[tempK];} else if(aperImage[tempK]>aperMaxF1) {aperMaxF1 = aperImage[tempK];}
                        } else {
                            if(aperImaRD[tempK]>aperR[aperK]-0.5 && aperImaRD[tempK]<=aperR[aperK]+0.5) {
                                aperMask1[tempK] = (1.0-aperImaRD[tempK]+aperR[aperK]-0.5); // <TODO> how to consider those pixels near within 0.5 pixel of the aperture circle?
                                aperSumF1 += aperImage[tempK] * aperMask1[tempK];
                                if(aperImage[tempK]>0.0) {aperRmsF1 += aperImage[tempK] * aperImage[tempK] * aperMask1[tempK];}
                                aperSumN1 += aperMask1[tempK];
                            } else {
                                if(aperImaRD[tempK]>aperR[aperK-1]-0.5 && aperImaRD[tempK]<=aperR[aperK-1]+0.5) {
                                    aperMask1[tempK] = (1.0-aperImaRD[tempK]+aperR[aperK-1]+0.5); // <TODO> how to consider those pixels near within 0.5 pixel of the aperture circle?
                                    aperSumF1 += aperImage[tempK] * aperMask1[tempK];
                                    if(aperImage[tempK]>0.0) {aperRmsF1 += aperImage[tempK] * aperImage[tempK] * aperMask1[tempK];}
                                    aperSumN1 += aperMask1[tempK];
                                } else {
                                    aperMask1[tempK] = 0.0;
                                }
                            }
                        }
                    }
                }
                if(debug) {
                    std::cout << "DEBUG: aperR[" << aperK-1 << ":" << aperK << "]=" << aperR[aperK-1] << ":" << aperR[aperK] << " fSumPix=" << aperSumF1 << std::endl;
                }
                //
                // output: x y rad NPIX FPIX FMIN FMAX
                std::cout << std::fixed << setw(12) << setprecision(5) << aperX[aperK] << setw(12) << setprecision(5) << aperY[aperK] << setw(12) << setprecision(5) << aperR[aperK] << setw(15) << setprecision(5) << aperSumN1 << std::flush;
                // std::cout.unsetf(std::ios::fixed);
                // std::cout.unsetf(std::ios::fixed);
                std::cout << resetiosflags(ios::fixed|ios::showpoint) << setw(15) << aperSumF1 << setw(15) << aperMinF1 << setw(15) << aperMaxF1 << std::flush;
                std::cout << setw(15) << aperSumF1/aperSumN1 << std::flush; // show mean pix value (incl. fractional pix value)
                aperRmsF1 = sqrt(aperRmsF1/aperSumN1); aperRmsS1 = sqrt(aperRmsS1/aperSumM1); // <added><20150323><dzliu> compute rms = sqrt((x1^2+x2^2+...)/N)
                std::cout << setw(15) << aperRmsF1 << std::flush; // show rms pix value (incl. fractional pix value)
                std::cout << setw(15) << aperSumM1 << setw(15) << aperSumS1 << setw(15) << aperSumS1/aperSumM1 << std::flush; // show the integer values
                std::cout << setw(15) << aperRmsS1 << std::flush; // show rms pix value (integer pix value)
                std::cout << std::endl;
            }
        }
        std::cout << std::endl;
        
    } else {
        // print usage
        std::cout << "Usage: \n    CrabPhotRingPhot NGC1068_PACS160.fits NGC1068_Info.txt NGC1068_Apertures.txt -ext 1" \
                  << "\n    (It is ok to provide a non-exist NGC1068_Info.txt if you have no galaxy information e.g. redshift, distance, etc.)" \
                  << "\n    (The aperture list file NGC1068_Apertures.txt should be ascii character space separated column aligned table.)" \
                  << std::endl;
    }
    return 0;
}
