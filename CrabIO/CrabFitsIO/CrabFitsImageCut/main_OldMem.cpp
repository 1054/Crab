/*
 
 ; 
 ; Cut or pad the InputImage to the new size NewSize.
 ;
 ; Only integer NewSize allowed! Only integer CentreXY allowed!
 ;
 ; If the InputImage is smaller than the NewSize, then we will pad the new image with NaN value.
 ; If the InputImage is larger than the NewSize, then we will cut the input image to make the new image.
 ;
 ; Note that both padding and cutting can be performed simutaneously.
 ; 
 ; E.g. CentreXY is close to the left edge of the InputImage, and far from the right edge, and NewSize is large,
 ;      then we will pad the left and cut the right of the InputImage, and return the new image.
 ;
 ; If an CentreXY is given, then use it as the centre of the new image.
 ;
 ; CornerIJ is i1:i2 j1:j2
 ;
 
 Please compile like this:
 
     clang++ main.cpp CrabFitsIO.cpp -o CrabFitsImageCrop_mac
 
 */

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "CrabFitsIO.h"

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
    char *cstrFilePath = NULL; char *cstrExtNumber = (char *)"0";
    char *cstrNewFilePath = NULL;
    char *cstrCenX = NULL;
    char *cstrCenY = NULL;
    char *cstrCenRX = NULL;
    char *cstrCenRY = NULL;
    char *cstrRectI0 = NULL;
    char *cstrRectJ0 = NULL;
    char *cstrRectI1 = NULL;
    char *cstrRectJ1 = NULL;
    long  iCenX=0, iCenY=0, iCenRX=0, iCenRY=0, iRectI0=0, iRectJ0=0, iRectI1=0, iRectJ1=0;
    
    int debug = 0;
    
    for(int i=1; i<argc; i++) {
        if(strncmp(argv[i],"-ext",4)==0 && i<argc-1) {
            i++; cstrExtNumber = argv[i]; continue;
        }
        if(strncmp(argv[i],"-cen",4)==0 && i<argc-3) {
            i++; cstrCenX = argv[i];
            i++; cstrCenY = argv[i];
            i++; cstrCenRX = argv[i];
            if(i<argc-1) { if(strspn(argv[i+1],"+-.eE0123456789")==strlen(argv[i+1])) {i++; cstrCenRY=argv[i];} else {cstrCenRY=cstrCenRX;} } else {cstrCenRY=cstrCenRX;} continue;            
        }
        if(strncmp(argv[i],"-rec",4)==0 && i<argc-4) {
            i++; cstrRectI0 = argv[i];
            i++; cstrRectJ0 = argv[i];
            i++; cstrRectI1 = argv[i];
            i++; cstrRectJ1 = argv[i]; continue;
        }
        if(strncmp(argv[i],"-out",4)==0 && i<argc-1) {
            i++; cstrNewFilePath = argv[i]; continue;
        }
        if(cstrFilePath==NULL && i<=argc-1) { cstrFilePath = argv[i]; continue; }
    }
    //
    //
    if(cstrFilePath!=NULL)
    { 
        int   errStatus = 0;
        int   extNumber = 0;  if(cstrExtNumber!=NULL) {extNumber=atoi(cstrExtNumber);}
        char *cstrHeader = NULL;
        long  posHeader = 0;
        long  lenHeader = 0;
        //
        // check
        std::cout << "CrabFitsImageCrop: " << cstrFilePath << " extension=" << atoi(cstrExtNumber) << std::endl;
        //
        // read fits header
        errStatus = readFitsHeader(cstrFilePath,extNumber,&cstrHeader,&posHeader,&lenHeader);
        std::cout << "CrabFitsImageCrop: readFitsHeader " << cstrFilePath << " extension=" << extNumber << " headersize=" << strlen(cstrHeader) << std::endl;
        //
        // read fits Naxis
        char *cstrNAXIS1 = extKeyword("NAXIS1",cstrHeader);
        char *cstrNAXIS2 = extKeyword("NAXIS2",cstrHeader);
        if(cstrNAXIS1!=NULL && cstrNAXIS2!=NULL) {
            //
            // read fits Naxis
            long oldImWidth = atol(cstrNAXIS1);
            long oldImHeight = atol(cstrNAXIS2);
            //
            // read fits image
            double *oldImage = readFitsImage(cstrFilePath,extNumber);
            std::cout << "CrabFitsImageCrop: readFitsImage " << cstrFilePath << " extension=" << extNumber << " imagesize=" << oldImWidth << "x" << oldImHeight << std::endl;
            //
            // convert from CenX,CenY,CenR to Rect(I1,J1)(I2,J2)
            if( (cstrRectI0==NULL || cstrRectJ0==NULL || cstrRectI1==NULL || cstrRectJ1==NULL) && cstrCenX!=NULL && cstrCenY!=NULL && cstrCenRX!=NULL && cstrCenRY!=NULL ) {
                // only when rect(i1,j1,i2,j2) is invlid, and cen(x,y,radius) is valid.
                iCenX = atol(cstrCenX); iCenY = atol(cstrCenY); iCenRX = atol(cstrCenRX); iCenRY = atol(cstrCenRY);
                iRectI0 = iCenX-1-iCenRX; iRectJ0 = iCenY-1-iCenRY;
                iRectI1 = iCenX-1+iCenRX; iRectJ1 = iCenY-1+iCenRY;
            } else {
                //
                iRectI0 = atol(cstrRectI0); iRectJ0 = atol(cstrRectJ0);
                iRectI1 = atol(cstrRectI1); iRectJ1 = atol(cstrRectJ1);
            }
            //
            // check Rect
            if( iRectI1>=0 && iRectI0<oldImWidth && iRectJ1>=0 && iRectJ0<oldImHeight && \
               ((iRectI0<iRectI1 && iRectJ0<iRectJ1) || (iRectI0==iRectI1 && iRectJ0<iRectJ1) || (iRectI0<iRectI1 && iRectJ0==iRectJ1)) ) {
                //
                // check
                std::cout << "CrabFitsImageCrop: cropFitsImage " << cstrFilePath << " extension=" << extNumber << " rect(i0,j0,i1,j1)=(" << iRectI0 << "," << iRectJ0 << "," << iRectI1 << "," << iRectJ1 << ")" << std::endl;
                //
                // define new image size
                long newImWidth = iRectI1 - iRectI0 + 1;
                long newImHeight = iRectJ1 - iRectJ0 + 1;
                //
                // create new image array
                double *newImage = (double *)malloc(newImWidth*newImHeight*sizeof(double));
                for(int i=0; i<newImWidth*newImHeight; i++) { newImage[i] = NAN; } // need cmath.h
                if(debug>0) {
                std::cout << "DEBUG: " << oldImWidth << "x" << oldImHeight << " " << oldImage[1173-1+(269-1)*oldImWidth] << std::endl;
                std::cout << "DEBUG: " << newImWidth << "x" << newImHeight << " " << newImage[100+100*newImWidth] << std::endl;
                }
                //
                // map the old image coordinate to new image coordinate
                long oldLi=0,oldUi=0,oldLj=0,oldUj=0;
                long newLi=0,newUi=0,newLj=0,newUj=0;
                if(iRectI0<0) {oldLi=0;newLi=-iRectI0;} else {oldLi=iRectI0;newLi=0;}
                if(iRectJ0<0) {oldLj=0;newLj=-iRectJ0;} else {oldLj=iRectJ0;newLj=0;}
                if(iRectI1<oldImWidth) {oldUi=iRectI1;newUi=newImWidth-1;} else {oldUi=oldImWidth-1;newUi=newImWidth-1-iRectI1+oldImWidth-1;}
                if(iRectJ1<oldImHeight) {oldUj=iRectJ1;newUj=newImHeight-1;} else {oldUj=oldImHeight-1;newUj=newImHeight-1-iRectJ1+oldImHeight-1;}
                if(debug>0) {
                std::cout << "DEBUG: " << "oldImage["<<oldLi<<":"<<oldUi<<","<<oldLj<<":"<<oldUj<<"]" << std::endl;
                std::cout << "DEBUG: " << "newImage["<<newLi<<":"<<newUi<<","<<newLj<<":"<<newUj<<"]" << std::endl;
                }
                //
                // copy image pixel by pixel
                for(int jj=0; jj<=(newUj-newLj); jj++) {
                    for(int ii=0; ii<=(newUi-newLi); ii++) {
                        newImage[(newLi+ii)+(newLj+jj)*newImWidth] = oldImage[(oldLi+ii)+(oldLj+jj)*oldImWidth];
                    }
                }
                if(debug>0) {
                std::cout << "DEBUG: " << "oldImage["<<oldLi<<":"<<oldUi<<","<<oldLj<<":"<<oldUj<<"]" << std::endl;
                std::cout << "DEBUG: " << "newImage["<<newLi<<":"<<newUi<<","<<newLj<<":"<<newUj<<"]" << std::endl;
                }
                //
                // modify header Keywords
                char *newNAXIS1 = (char *)malloc(10*sizeof(char)); sprintf(newNAXIS1,"%0ld",newImWidth);
                char *newNAXIS2 = (char *)malloc(10*sizeof(char)); sprintf(newNAXIS2,"%0ld",newImHeight);
                errStatus = modKeyword("NAXIS1",newNAXIS1,cstrHeader); free(newNAXIS1); newNAXIS1=NULL;
                errStatus = modKeyword("NAXIS2",newNAXIS2,cstrHeader); free(newNAXIS2); newNAXIS2=NULL;
                if(debug>0) {
                std::cout << "DEBUG: new NAXIS1 = " << extKeyword("NAXIS1",cstrHeader) << std::endl;
                std::cout << "DEBUG: new NAXIS2 = " << extKeyword("NAXIS2",cstrHeader) << std::endl;
                }
                char *cstrCRPIX1 = extKeyword("CRPIX1",cstrHeader);
                char *cstrCRPIX2 = extKeyword("CRPIX2",cstrHeader);
                if(cstrCRPIX1!=NULL && cstrCRPIX2!=NULL) {
                    if(debug>0) {
                    std::cout << "DEBUG: old CRPIX1 = " << cstrCRPIX1 << std::endl;
                    std::cout << "DEBUG: old CRPIX2 = " << cstrCRPIX2 << std::endl;
                    }
                    double dbCRPIX1 = atof(cstrCRPIX1);
                    double dbCRPIX2 = atof(cstrCRPIX2);
                    dbCRPIX1 = dbCRPIX1 - iRectI0;
                    dbCRPIX2 = dbCRPIX2 - iRectJ0;
                    char *newCRPIX1 = (char *)malloc(20*sizeof(char)); sprintf(newCRPIX1,"%0.3f",dbCRPIX1);
                    char *newCRPIX2 = (char *)malloc(20*sizeof(char)); sprintf(newCRPIX2,"%0.3f",dbCRPIX2);
                    errStatus = modKeyword("CRPIX1",newCRPIX1,cstrHeader); free(newCRPIX1); newCRPIX1=NULL;
                    errStatus = modKeyword("CRPIX2",newCRPIX2,cstrHeader); free(newCRPIX2); newCRPIX2=NULL;
                    if(debug>0) {
                    std::cout << "DEBUG: new CRPIX1 = " << extKeyword("CRPIX1",cstrHeader) << std::endl;
                    std::cout << "DEBUG: new CRPIX2 = " << extKeyword("CRPIX2",cstrHeader) << std::endl;
                    }
                    //
                    // also modify BITPIX
                    if(debug>0) {
                    std::cout << "DEBUG: old BITPIX = " << extKeyword("BITPIX",cstrHeader) << std::endl;
                    }
                    char *newBITPIX = (char *)"-64";
                    errStatus = modKeyword("BITPIX",newBITPIX,cstrHeader);
                    if(debug>0) {
                    std::cout << "DEBUG: new BITPIX = " << extKeyword("BITPIX",cstrHeader) << std::endl;
                    }
                    //
                    // and remember to copy the main header when (extNumber>0)
                    if(extNumber>0) {
                        char *mainHeader = readFitsHeader(cstrFilePath,0);
                        char *newBITPIX = (char *)"-64"; errStatus = modKeyword("BITPIX",newBITPIX,mainHeader);
                        char *fullHeader = (char *)malloc((strlen(mainHeader)+strlen(cstrHeader)+1)*sizeof(char));
                        memset(fullHeader,'\0',(strlen(mainHeader)+strlen(cstrHeader)+1));
                        strcpy(fullHeader,mainHeader);
                        strcat(fullHeader,cstrHeader);
                        free(cstrHeader); cstrHeader = fullHeader;
                    }
                    //
                    // prepare output file path
                    if(cstrNewFilePath==NULL) {
                        std::string strNewFilePath(cstrFilePath);
                        size_t posFileNameSuffix = strNewFilePath.find_last_of(".fits");
                        if(std::string::npos!=posFileNameSuffix) {
                            strNewFilePath = strNewFilePath.substr(0,posFileNameSuffix-4) + "_Cropped.fits";
                        } else {
                            strNewFilePath = strNewFilePath + "_Cropped.fits";
                        }
                        cstrNewFilePath = (char *)malloc(sizeof(char)*(strlen(strNewFilePath.c_str())+1));
                        memset(cstrNewFilePath,'\0',(strlen(strNewFilePath.c_str())+1));
                        strcpy(cstrNewFilePath,strNewFilePath.c_str());
                        if(debug>0) {
                            std::cout << "CrabFitsImageCrop: Input " << cstrFilePath << std::endl;
                            std::cout << "CrabFitsImageCrop: Output " << cstrNewFilePath << std::endl;
                        }
                    }
                    //
                    // now save new fits
                    // const char *newFilePath = NULL;
                    // newFilePath = "/Users/dliu/Programming/QtProgram/CrabIO/CrabFitsIO/CrabFitsImageCut/new.fits";
                    // std::cout << "CrabFitsImageCrop: Opening " << newFilePath << " to write." << std::endl;
                    if(debug>0) {
                    std::cout << "CrabFitsImageCrop: Opening " << cstrNewFilePath << " to write" << std::endl;
                    }
                    errStatus = writeFitsD(newImage,cstrHeader,cstrNewFilePath);
                    if(errStatus==0) {
                        if(debug>0) {
                        std::cout << "CrabFitsImageCrop: Well done!" << std::endl;
                        }
                        std::cout << "CrabFitsImageCrop: File has been saved to " << cstrNewFilePath << std::endl;
                    } else {
                        std::cout << "CrabFitsImageCrop: Oooh! Failed! We are sorry ... (error code " << errStatus << ")" << std::endl;
                    }
                } else {
                    std::cout << "CrabFitsImageCrop: Error! Fits header of extension " << extNumber << " does not contain CRPIX1 and CRPIX2!" << std::endl;
                }
                //
                //
                free(newImage); newImage = NULL;
            } else {
                std::cout << "CrabFitsImageCrop: Error! Please input valid cen(x,y,radius) or rect(i0,j0,i1,j1)!" << std::endl;
                return -1;
            }
        } else {
            std::cout << "CrabFitsImageCrop: Error! Fits header of extension " << extNumber << " does not contain NAXIS1 and NAXIS2!" << std::endl;
        }
    } else {
        std::cout << "CrabFitsImageCrop_mac Arp193_HST_7219_21_NIC_NIC2_F110W_drz.fits -ext 1 -cen 1173 269 200" << std::endl;
    }
    return 0;
}