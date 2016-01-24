/*
 
 CrabImage
 
 
 
 
 
 First code:
       
       2015-01-29
 
 
 Last update:
 
       2015-02-01 correct Li Lj Ui Uj
 
 
 */

#include "CrabImage.h"


void CrabImageConvertCen2Rect(char *cstrCenX, char *cstrCenY, char *cstrCenRX, char *cstrCenRY, long *iRectI0, long *iRectJ0, long *iRectI1, long *iRectJ1, double *dRectI0, double *dRectJ0, double *dRectI1, double *dRectJ1)
{
    //
    // convert from CenX,CenY,CenR to Rect(I1,J1)(I2,J2)
    // CenX CenY are the astro image coordinate starting from 1.0
    // Rect I J are the array index coordinate starting from 0
    // dRectI dRectJ are the fractional part of real rect vertex
    // e.g. real rect lower left image coord should be double(iRectI+dRectI+1)
    if( cstrCenX!=NULL && cstrCenY!=NULL && cstrCenRX!=NULL && cstrCenRY!=NULL ) {
        long iCenX = atol(cstrCenX); long iCenY = atol(cstrCenY);
        long iCenRX = atol(cstrCenRX); long iCenRY = atol(cstrCenRY);
        double dCenX = atof(cstrCenX) - double(iCenX);
        double dCenY = atof(cstrCenY) - double(iCenY);
        double dCenRX = atof(cstrCenRX) - double(iCenRX);
        double dCenRY = atof(cstrCenRY) - double(iCenRY);
        *dRectI0 = dCenX - dCenRX; *dRectJ0 = dCenY - dCenRY;
        *dRectJ0 = dCenX + dCenRX; *dRectJ1 = dCenY + dCenRY;
        *iRectI0 = iCenX-1-iCenRX; *iRectJ0 = iCenY-1-iCenRY;
        *iRectI1 = iCenX-1+iCenRX; *iRectJ1 = iCenY-1+iCenRY;
    }
}


void CrabImageConvertCen2Rect(double doubCenX, double doubCenY, double doubCenRX, double doubCenRY, long *iRectI0, long *iRectJ0, long *iRectI1, long *iRectJ1, double *dRectI0, double *dRectJ0, double *dRectI1, double *dRectJ1)
{
    //
    // convert from CenX,CenY,CenR to Rect(I1,J1)(I2,J2)
    // CenX CenY are the astro image coordinate starting from 1.0
    // Rect I J are the array index coordinate starting from 0
    // dRectI dRectJ are the fractional part of real rect vertex
    // e.g. real rect lower left image coord should be double(iRectI+dRectI+1)
    if( 1 == 1 ) {
        long iCenX = long(doubCenX); long iCenY = long(doubCenY);
        long iCenRX = long(doubCenRX); long iCenRY = long(doubCenRY);
        double dCenX = double(doubCenX) - double(iCenX);
        double dCenY = double(doubCenY) - double(iCenY);
        double dCenRX = double(doubCenRX) - double(iCenRX);
        double dCenRY = double(doubCenRY) - double(iCenRY);
        *dRectI0 = dCenX - dCenRX; *dRectJ0 = dCenY - dCenRY;
        *dRectJ0 = dCenX + dCenRX; *dRectJ1 = dCenY + dCenRY;
        *iRectI0 = iCenX-1-iCenRX; *iRectJ0 = iCenY-1-iCenRY;
        *iRectI1 = iCenX-1+iCenRX; *iRectJ1 = iCenY-1+iCenRY;
    }
}


void CrabImageCalcCropRect(long oldImWidth, long oldImHeight, long iRectI0, long iRectJ0, long iRectI1, long iRectJ1, long *oldLi, long *oldLj, long *oldUi, long *oldUj, long *newLi, long *newLj, long *newUi, long *newUj, int debug)
{
    //
    // calculate the image crop rect so that the crop rect does not exceed the old image range
    if(oldImWidth>0 && oldImHeight>0) {
        //
        // define new image size
        long newImWidth = iRectI1 - iRectI0 + 1;
        long newImHeight = iRectJ1 - iRectJ0 + 1;
        //
        // map the old image coordinate to new image coordinate
        *oldLi=0;*oldUi=0;*oldLj=0;*oldUj=0; // should not exceed old image range
        *newLi=0;*newUi=0;*newLj=0;*newUj=0; // should not exceed new image range
        if(iRectI0<0) {*oldLi=0;*newLi=-iRectI0;} else {*oldLi=iRectI0;*newLi=0;}
        if(iRectJ0<0) {*oldLj=0;*newLj=-iRectJ0;} else {*oldLj=iRectJ0;*newLj=0;}
        if(iRectI1<oldImWidth) {*oldUi=iRectI1;*newUi=newImWidth-1;} else {*oldUi=oldImWidth-1;*newUi=newImWidth-1-iRectI1+oldImWidth-1;}
        if(iRectJ1<oldImHeight) {*oldUj=iRectJ1;*newUj=newImHeight-1;} else {*oldUj=oldImHeight-1;*newUj=newImHeight-1-iRectJ1+oldImHeight-1;}
        if(debug>0) {
            std::cout << "DEBUG: CrabImageCalcCropRect: " << "oldImage["<<*oldLi<<":"<<*oldUi<<","<<*oldLj<<":"<<*oldUj<<"]" << std::endl;
            std::cout << "DEBUG: CrabImageCalcCropRect: " << "newImage["<<*newLi<<":"<<*newUi<<","<<*newLj<<":"<<*newUj<<"]" << std::endl;
        }

    }
}


void CrabImageCopyCropRect(double *oldImage, double **newImage, long iRectI0, long iRectJ0, long iRectI1, long iRectJ1, long oldLi, long oldLj, long oldUi, long oldUj, long newLi, long newLj, long newUi, long newUj, int debug)
{
    //
    //
    if(oldImage!=NULL) {
        //
        // define new image size
        long newImWidth = iRectI1 - iRectI0 + 1;
        long newImHeight = iRectJ1 - iRectJ0 + 1;
        //
        // create new image array
        (*newImage) = (double *)malloc(newImWidth*newImHeight*sizeof(double));
        for(int i=0; i<(newImWidth)*(newImHeight); i++) { (*newImage)[i] = NAN; } // need cmath.h
        //
        // copy image pixel by pixel
        for(int jj=0; jj<=(newUj-newLj); jj++) {
            for(int ii=0; ii<=(newUi-newLi); ii++) {
                (*newImage)[(newLi+ii)+(newLj+jj)*newImWidth] = oldImage[ii+jj*(newUi-newLi+1)];
            }
        }
    }
}


void CrabImageCopyCropRect(char *oldImageFits, double **newImage, long iRectI0, long iRectJ0, long iRectI1, long iRectJ1, long oldLi, long oldLj, long oldUi, long oldUj, long newLi, long newLj, long newUi, long newUj, int extNumber, int debug)
{
    //
    //
    if(oldImageFits!=NULL) {
        //
        // define new image size
        long newImWidth = iRectI1 - iRectI0 + 1;
        long newImHeight = iRectJ1 - iRectJ0 + 1;
        //
        // create new image array
        (*newImage) = (double *)malloc(newImWidth*newImHeight*sizeof(double));
        for(int i=0; i<(newImWidth)*(newImHeight); i++) { (*newImage)[i] = NAN; } // need cmath.h
        //
        // read old image by block <Updated><20141117><DzLIU>
        double *oldImage = readFitsImageBlock(oldImageFits,oldLi,oldLj,(newUi-newLi+1),(newUj-newLj+1),extNumber); // <Updated><20141117><DzLIU>
        //
        if(debug>0) {
            std::cout << "DEBUG: " << "readFitsImageBlock "<<oldLi<<" "<<oldLj<<" "<<(newUi-newLi+1)<<" "<<(newUj-newLj+1)<<" "<<extNumber << std::endl;
            std::cout << "DEBUG: " << "readFitsImageBlock "<< oldImage << "->[0] = " << oldImage[0] << std::endl;
            std::cout << "DEBUG: " << "readFitsImageBlock "<< oldImage << "->[1] = " << oldImage[1] << std::endl;
        }
        // copy image pixel by pixel
        for(int jj=0; jj<=(newUj-newLj); jj++) {
            for(int ii=0; ii<=(newUi-newLi); ii++) {
                (*newImage)[(newLi+ii)+(newLj+jj)*newImWidth] = oldImage[ii+jj*(newUi-newLi+1)];
            }
        }
        // free old image memory <Added><20160111><DzLIU>
        free(oldImage); oldImage=NULL; 
    }
}





