/* 
 
 a semi-automatic aper phot program
 
 input 1: *.fits file --- the galaxy image 
 input 2: *.info file --- the galaxy info (Name,z,dL)
 input 3: *.aper file --- the apertures (x,y,rad)
 
 
 
 Please compile like this:
 
     clang++ main.cpp CrabFitsIO.cpp CrabImage.cpp -o CrabPhotAperPhot_mac
     clang++ main.cpp CrabFitsIO.cpp CrabImage.cpp -o CrabPhotAperPhot_linux_x86_64
 
     clang++ main.cpp CrabFitsIO.cpp CrabPhotUI.cpp -I/Users/dliu/Code/Qt/5.2.1/5.2.1/clang_64/include/ -I/Users/dliu/Code/Qt/5.2.1/5.2.1/clang_64/include/QtCore -I/Users/dliu/Code/Qt/5.2.1/5.2.1/clang_64/include/QtGui -I/Users/dliu/Code/Qt/5.2.1/5.2.1/clang_64/include/QtWidgets -L/Users/dliu/Code/Qt/5.2.1/5.2.1/clang_64/lib -lQtCore.framework -o CrabPhotAperPhot_mac
     
     clang++ -I/usr/include/malloc/ main.cpp CrabFitsIO.cpp CrabImage.cpp -o CrabPhotAperPhot_mac # 2017-02-28
     clang++ -I/usr/include/malloc/ main.cpp CrabFitsIO.cpp CrabImage.cpp -o CrabPhotAperPhot_mac # 2017-03-04
 
 
 Initialized:
     
     2015-01-29
 
 Last update:
 
     2015-03-23 add rms
     2015-12-18 add stddev, median
     2015-12-21 add arg -header-no-comment
     2015-12-22 set arg -header-no-comment as the default choice
     2017-02-28 set arg -header-no-comment as the default choice. Use strncasecmp instead of strncmp. Added #include <clocale>.
     2017-03-04 set arg -header-in-comment as the default choice.
 
 
 
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <algorithm> // for sort()
#include <fstream>
#include <iostream>
#include <iomanip>
#include "CrabFitsIO.h"
#include "CrabImage.h"
#include "CrabTableReadColumn.cpp"
#include "CrabTableReadInfo.cpp"
//#define DEF_Version "2015-12-21"
//#define DEF_Version "2015-12-22"
//#define DEF_Version "2016-01-11"
//#define DEF_Version "2016-04-30"
//#define DEF_Version "2017-02-28"
#define DEF_Version "2017-03-04"

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
        std::cout << "# CrabPhotAperPhot: version " << DEF_Version << std::endl;
        // std::cout << "# CrabPhotAperPhot: " << cstrInput1 << " extension=" << extNumber << std::endl;
        //
        // read fits header
        errStatus = readFitsHeader(cstrInput1,extNumber,&cstrHeader,&posHeader,&lenHeader);
        std::cout << "# CrabPhotAperPhot: readFitsHeader " << cstrInput1 << " extension=" << extNumber << " headersize=" << strlen(cstrHeader) << std::endl;
        //
        // read fits Naxis
        char *cstrNAXIS1 = extKeyword("NAXIS1",cstrHeader);
        char *cstrNAXIS2 = extKeyword("NAXIS2",cstrHeader);
        if(cstrNAXIS1!=NULL && cstrNAXIS2!=NULL) {
            //
            // read info file <added><20160430><dzliu>
            // -- so that we can apply parameter tuning, e.g. elliptical aperture
            int aperTYPE = 1; // 1: circle; 2: ellipse; 3: TODO;
            std::string tempTYPE;
            if(strncasecmp(cstrInput2,"none",strlen("none"))!=0 && strncasecmp(cstrInput2,"null",strlen("null"))!=0) {
                tempTYPE = CrabTableReadInfo(cstrInput2,"AperType");
                std::transform(tempTYPE.begin(), tempTYPE.end(), tempTYPE.begin(), ::tolower);
                if(tempTYPE.find("ellip")!= std::string::npos) {
                    aperTYPE = 2;
                    std::cout << "# CrabPhotAperPhot: apertype " << aperTYPE << " " << tempTYPE << std::endl;
                }
            }
            //
            // read fits Naxis
            long longWidth = atol(cstrNAXIS1);
            long longHeight = atol(cstrNAXIS2);
            //
            // read aper details
            int aperN = 0, aperNX = 0, aperNY = 0, aperNR = 0, aperNE = 0, aperNT = 0;
            double *aperX = CrabTableReadColumnF(cstrInput3,1,&aperNX);
            double *aperY = CrabTableReadColumnF(cstrInput3,2,&aperNY);
            double *aperR = CrabTableReadColumnF(cstrInput3,3,&aperNR); // aperture radius or major axis
            double *aperE = NULL; // aperture elliptic b/a
            double *aperT = NULL; // aperture rotate angle "theta"
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
                        std::cout << "# CrabPhotAperPhot: Error! The column 4 of " << cstrInput3 << " is invalid! Abort!" << std::endl;
                        return -1;
                    }
                    if(NULL==aperT) {
                        std::cout << "# CrabPhotAperPhot: Error! The column 5 of " << cstrInput3 << " is invalid! Abort!" << std::endl;
                        return -1;
                    }
                }
            }
            // check aper file rows -- columns should have the same row count
            if(aperNX>0 && aperNX==aperNY && aperNY==aperNR) {
                aperN = aperNX;
            } else {
                std::cout << "# CrabPhotAperPhot: Error! Aperture list in " << cstrInput3 << " is invalid! Abort!" << std::endl;
                return -1;
            }
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
                std::cout << setw(13) << "npix"
                          << setw(13) << "sum"
                          << setw(13) << "min"
                          << setw(13) << "max"
                          << setw(13) << "mean"
                          << setw(13) << "rms"
                          << std::flush;
                std::cout << setw(13) << "npix_int"
                          << setw(13) << "sum_int"
                          << setw(13) << "mean_int"
                          << setw(13) << "median_int"
                          << setw(13) << "stddev_int"
                          << setw(13) << "rms_int"
                          << setw(13) << "sumabs_int"
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
                std::cout << setw(13) << "npix"
                          << setw(13) << "sum"
                          << setw(13) << "min"
                          << setw(13) << "max"
                          << setw(13) << "mean"
                          << setw(13) << "rms"
                          << std::flush;
                std::cout << setw(13) << "npix_int"
                          << setw(13) << "sum_int"
                          << setw(13) << "mean_int"
                          << setw(13) << "median_int"
                          << setw(13) << "stddev_int"
                          << setw(13) << "rms_int"
                          << setw(13) << "sumabs_int"
                          << std::flush;
                std::cout << std::endl;
            }
            //
            // loop each aper to do the photometry
            for(int aperK=0; aperK<aperN; aperK++) {
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
                double *aperImaRD = (double *)malloc(aperLength*aperLength*sizeof(double)); // distance array
                double *aperImaRU = (double *)malloc(aperLength*aperLength*sizeof(double)); // pixvalue array (no fractional part)
                double *aperImaRV = (double *)malloc(aperLength*aperLength*sizeof(double)); // pixvalue array
                double *aperMask1 = (double *)malloc(aperLength*aperLength*sizeof(double)); // mask of pix in aperture circle 1 (0 means out, 1 means in, fractional means at the edge)
                double  aperSumM1 = 0.0; // sum pix numb in aperture circle 1 (no fractional part)
                double  aperSumN1 = 0.0; // sum pix numb in aperture circle 1
                double  aperSumS1 = 0.0; // sum pix flux in aperture circle 1 (no fractional part)
                double  aperSumF1 = 0.0; // sum pix flux in aperture circle 1
                double  aperRmsS1 = 0.0; // root-mean-square (abbreviated "RMS" and sometimes called the quadratic mean (no fractional part)
                double  aperRmsF1 = 0.0; // root-mean-square (abbreviated "RMS" and sometimes called the quadratic mean
                double  aperVaiS1 = 0.0; // Variance (no fractional part)
                double  aperVaiF1 = 0.0; // Variance
                double  aperSaiS1 = 0.0; // Standard Deviation (no fractional part)
                double  aperSaiF1 = 0.0; // Standard Deviation
                double  aperMedS1 = 0.0; // median (no fractional part)
                double  aperMedF1 = 0.0; // median
                double  aperMeaS1 = 0.0; // mean (no fractional part)
                double  aperMeaF1 = 0.0; // mean
                double  aperSabS1 = 0.0; // sum of absolute values (no fractional part)
                double  aperSabF1 = 0.0; // sum of absolute values
                double  aperMaxS1 = NAN; // max pix flux in aperture circle 1 (no fractional part)
                double  aperMaxF1 = NAN; // max pix flux in aperture circle 1
                double  aperMinS1 = NAN; // min pix flux in aperture circle 1 (no fractional part)
                double  aperMinF1 = NAN; // min pix flux in aperture circle 1
                for(long tempK=0; tempK<aperLength*aperLength; tempK++) {
                    aperImaRU[tempK] = NAN;
                    aperImaRV[tempK] = NAN;
                    if(aperImage[tempK]==aperImage[tempK]) { // <TODO> check non NAN
                        aperImaRX[tempK] = (double(tempK % aperLength)) - (double(aperRadius)) - (double(aperX[aperK])-long(aperX[aperK])); // convert global coordinate to small image cut coordinate, center is aperX,aperY. <corrected><20150220><dzliu> fractional difference
                        aperImaRY[tempK] = (double(tempK / aperLength)) - (double(aperRadius)) - (double(aperY[aperK])-long(aperY[aperK])); // convert global coordinate to small image cut coordinate, center is aperX,aperY. <corrected><20150220><dzliu> fractional difference
                        aperImaRD[tempK] = sqrt(aperImaRX[tempK]*aperImaRX[tempK] + aperImaRY[tempK]*aperImaRY[tempK]);
                        // <modified><20160430><dzliu> elliptic
                        // double tempDIST = aperImaRD[tempK]-aperR[aperK]; // circular radius
                        double tempDIST = 0.0; // the distance difference between aperture and current pixel along a same line of sight from center 0,0
                        double tempTANG = 0.0; // the angle of current pixel line of sight from center 0,0 in degree, against +X axis, compared to the ds9 rotate angle aperT[aperK] (against +X axis as well <TODO>).
                        if(aperImaRX[tempK]!=0.0) {
                            if(aperImaRY[tempK]>=0.0) {
                                tempTANG = atan(aperImaRY[tempK]/aperImaRX[tempK])*180.0/3.14159265;
                            } else {
                                tempTANG = atan(aperImaRY[tempK]/aperImaRX[tempK])*180.0/3.14159265+180.0;
                            }
                        } else {
                            if(aperImaRY[tempK]==0.0) {
                                tempTANG = 1999;
                            } else {
                                if(aperImaRY[tempK]>0.0) {
                                    tempTANG = 90.0;
                                } else {
                                    tempTANG = 90.0+180.0;
                                }
                            }
                        }
                        if(tempTANG>1000) { // this is exactly the center pixel
                            tempDIST = 999;
                        } else {
                            tempTANG = tempTANG - aperT[aperK]; // <TODO> what if one use Position Angle PA that againsts +Y axis?
                            tempDIST = sqrt(pow(aperR[aperK]*cos(tempTANG/180.0*3.14159265),2)+pow(aperR[aperK]*aperE[aperK]*sin(tempTANG/180.0*3.14159265),2));
                        }
                        //<DEBUG> std::cout << "# DEBUG" << std::endl;
                        //<DEBUG> std::cout << setw(12) << tempTANG << setw(12) << tempDIST << std::endl;
                        //<DEBUG> std::cout << "# DEBUG" << std::endl;
                        //<DEBUG> 20160430 elliptic shape has no problem.

                        tempDIST = aperImaRD[tempK] - tempDIST; // then compute the distance difference between current pixel and aperture border pixel, if positive then means current pixel is out of the aperture

                        if(tempDIST<=0.0) { // (sum ignoring fractional part)
                            aperImaRU[tempK] = aperImage[tempK];
                            aperSumS1 += aperImage[tempK];
                            aperSabS1 += std::abs(aperImage[tempK]);
                            // <modified><20151218><dzliu> if(aperImage[tempK]>0.0) {aperRmsS1 += aperImage[tempK]*aperImage[tempK];} // do RMS for both positive and negative pixels
                            aperRmsS1 += aperImage[tempK]*aperImage[tempK];
                            aperSumM1 += 1.0;
                            // <debug><20150220><dzliu> std::cout << setw(10) << aperImage[tempK] << setw(10) << aperImaRD[tempK] << setw(10) << aperImaRX[tempK] << setw(10) << aperImaRY[tempK] << std::endl;
                            if(aperMinS1!=aperMinS1) {aperMinS1 = aperImage[tempK];} else if(aperImage[tempK]<aperMinS1) {aperMinS1 = aperImage[tempK];}
                            if(aperMaxS1!=aperMaxS1) {aperMaxS1 = aperImage[tempK];} else if(aperImage[tempK]>aperMaxS1) {aperMaxS1 = aperImage[tempK];}
                        }
                        if(tempDIST<=-0.5) { // (sum considering fractional part)
                            // these pixels are fully within the aperture
                            aperMask1[tempK] = 1.0;
                            aperImaRV[tempK] = aperImage[tempK];
                            aperSumF1 += aperImage[tempK];
                            aperSabF1 += std::abs(aperImage[tempK]);
                            // <modified><20151218><dzliu> if(aperImage[tempK]>0.0) {aperRmsF1 += aperImage[tempK]*aperImage[tempK];} // do RMS for both positive and negative pixels
                            aperRmsF1 += aperImage[tempK]*aperImage[tempK];
                            aperSumN1 += 1.0;
                            // <debug><20150220><dzliu> std::cout << setw(10) << aperImage[tempK] << setw(10) << aperImaRD[tempK] << setw(10) << aperImaRX[tempK] << setw(10) << aperImaRY[tempK] << std::endl;
                            if(aperMinF1!=aperMinF1) {aperMinF1 = aperImage[tempK];} else if(aperImage[tempK]<aperMinF1) {aperMinF1 = aperImage[tempK];}
                            if(aperMaxF1!=aperMaxF1) {aperMaxF1 = aperImage[tempK];} else if(aperImage[tempK]>aperMaxF1) {aperMaxF1 = aperImage[tempK];}
                        } else {
                            // these pixels are at the border of current aperture
                            // how to consider those pixels near within 0.5 pixel of the aperture circle?
                            // we sum their weighted pixel values
                            if(tempDIST>-0.5 && tempDIST<=+0.5) {
                                // <modified><20160430><dzliu> elliptic
                                // aperMask1[tempK] = (1.0-aperImaRD[tempK]+aperR[aperK]-0.5);
                                aperMask1[tempK] = (1.0-(tempDIST)-0.5);
                                aperSumF1 += aperImage[tempK] * aperMask1[tempK];
                                // <modified><20151218><dzliu> if(aperImage[tempK]>0.0) {aperRmsF1 += aperImage[tempK] * aperImage[tempK] * aperMask1[tempK];}
                                aperRmsF1 += aperImage[tempK]*aperImage[tempK]*aperMask1[tempK]*aperMask1[tempK];
                                aperSumN1 += aperMask1[tempK];
                            } else {
                                aperMask1[tempK] = 0.0;
                            }
                        }
                    }
                }
                if(debug) {
                    std::cout << "DEBUG: aperR[" << aperK << "]=" << aperR[aperK] << " fSumPix=" << aperSumF1 << std::endl;
                }
                //
                // now sort pixel value array and compute mean, rms, variance, stddev, and median
                aperMeaS1 = aperSumS1/aperSumM1; // mean of the pixel values (no fractional part)
                aperMeaF1 = aperSumF1/aperSumN1; // mean of the pixel values
                aperRmsS1 = sqrt(aperRmsS1/aperSumM1); // <added><20150323><dzliu> compute rms = sqrt((x1^2+x2^2+...)/N) (no fractional part)
                aperRmsF1 = sqrt(aperRmsF1/aperSumN1); // <added><20150323><dzliu> compute rms = sqrt((x1^2+x2^2+...)/N)
                sort(aperImaRU, aperImaRU+aperLength*aperLength);
                sort(aperImaRV, aperImaRV+aperLength*aperLength);
                long tempI=0,tempJ=0;
                for(long tempK=0; tempK<aperLength*aperLength; tempK++) {
                    if(aperImaRU[tempK]==aperImaRU[tempK]) { // (no fractional part)
                        aperVaiS1 += (aperImaRU[tempK]-aperMeaS1)*(aperImaRU[tempK]-aperMeaS1); // The Variance is defined as the average of the squared differences from the Mean.
                        tempI += 1; if(tempI==long(double(aperSumM1+1)/2.0)) { aperMedS1 = aperImaRU[tempK]; } // The median
                    }
                    if(aperImaRV[tempK]==aperImaRV[tempK]) {
                        aperVaiF1 += (aperImaRV[tempK]-aperMeaF1)*(aperImaRV[tempK]-aperMeaF1); // The Variance is defined as the average of the squared differences from the Mean.
                        tempJ += 1; if(tempJ==long(double(aperSumN1+1)/2.0)) { aperMedF1 = aperImaRV[tempK]; } // The median
                    }
                }
                aperVaiS1 = aperVaiS1/aperSumM1; // The Variance is defined as the average of the squared differences from the Mean. (no fractional part)
                aperVaiF1 = aperVaiF1/aperSumN1; // The Variance is defined as the average of the squared differences from the Mean.
                aperSaiS1 = sqrt(aperVaiS1); // Standard Deviation is the square root of the Variance. (no fractional part)
                aperSaiF1 = sqrt(aperVaiF1); // Standard Deviation is the square root of the Variance.
                //
                // output: x y rad NPIX FPIX FMIN FMAX
                std::cout << std::fixed
                          << setw(12) << setprecision(5) << aperX[aperK]
                          << setw(12) << setprecision(5) << aperY[aperK]
                          << setw(12) << setprecision(5) << aperR[aperK]
                          << std::flush;
                std::cout << resetiosflags(ios::fixed|ios::showpoint)
                          << setw(13) << aperSumN1
                          << setw(13) << aperSumF1
                          << setw(13) << aperMinF1
                          << setw(13) << aperMaxF1
                          << setw(13) << aperMeaF1
                          << setw(13) << aperRmsF1
                          << std::flush;
                std::cout << setw(13) << aperSumM1
                          << setw(13) << aperSumS1
                          << setw(13) << aperMeaS1
                          << setw(13) << aperMedS1
                          << setw(13) << aperSaiS1
                          << setw(13) << aperRmsS1
                          << setw(13) << aperSabS1
                          << std::flush; // show the integer values
                std::cout << std::endl;
                //
                // free
                free(aperImaRX); aperImaRX=NULL;
                free(aperImaRY); aperImaRY=NULL;
                free(aperImaRD); aperImaRD=NULL;
                free(aperImaRU); aperImaRU=NULL;
                free(aperImaRV); aperImaRV=NULL;
                free(aperMask1); aperMask1=NULL;
                free(aperImage); aperImage=NULL;
            }
            // 
            // free
            free(aperX); aperX=NULL;
            free(aperY); aperY=NULL;
            free(aperR); aperR=NULL;
        }
        std::cout << std::endl;
        
    } else {
        // print usage
        std::cout << "Usage: "
                  << "\n    CrabPhot NGC1068_PACS160.fits NGC1068_Info.txt NGC1068_Apertures.txt -ext 1"
                  << "\n    CrabPhot NGC1068_PACS160.fits NGC1068_Info.txt NGC1068_Apertures.txt -ext 1 -header-no-comment"
                  << "\n    CrabPhot NGC1068_PACS160.fits NGC1068_Info.txt NGC1068_Apertures.txt -ext 1 -header-in-comment"
                  << "\n    (It is ok to provide a non-exist NGC1068_Info.txt if you have no galaxy information e.g. redshift, distance, etc.)"
                  << "\n    (The aperture list file NGC1068_Apertures.txt should be ascii character space separated column aligned table.)"
                  << "\n    (The default output format is header-no-comment, the first line will be the column headers.)"
                  << "\n    (If set the output format to header-in-comment, then the 4th line will be the column headers, and starts with a #.)"
                  << "\n"
                  << "Version: \n    " << DEF_Version
                  << std::endl;
    }
    return 0;
}
