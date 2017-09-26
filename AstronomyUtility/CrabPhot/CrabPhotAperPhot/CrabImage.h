/*
 
 CrabImage
 
 
 
 
 
 First code:
 
 2015-01-29
 
 
 Last update:
 
 2015-02-01 correct Li Lj Ui Uj
 
 
 */

#ifndef CRABIMAGE_H
#define CRABIMAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <malloc.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include "CrabFitsIO.h"

using namespace std;

void CrabImageConvertCen2Rect(char *cstrCenX, char *cstrCenY, char *cstrCenRX, char *cstrCenRY, long *iRectI0, long *iRectJ0, long *iRectI1, long *iRectJ1, double *dRectI0=NULL, double *dRectJ0=NULL, double *dRectI1=NULL, double *dRectJ1=NULL);

void CrabImageConvertCen2Rect(double doubCenX, double doubCenY, double doubCenRX, double doubCenRY, long *iRectI0, long *iRectJ0, long *iRectI1, long *iRectJ1, double *dRectI0=NULL, double *dRectJ0=NULL, double *dRectI1=NULL, double *dRectJ1=NULL);

void CrabImageCalcCropRect(long oldImWidth, long oldImHeight, long iRectI0, long iRectJ0, long iRectI1, long iRectJ1, long *oldLi, long *oldLj, long *oldUi, long *oldUj, long *newLi, long *newLj, long *newUi, long *newUj, int debug=0);

void CrabImageCopyCropRect(double *oldImage, double **newImage, long iRectI0, long iRectJ0, long iRectI1, long iRectJ1, long oldLi, long oldLj, long oldUi, long oldUj, long newLi, long newLj, long newUi, long newUj, int debug=0);

void CrabImageCopyCropRect(char *oldImageFits, double **newImage, long iRectI0, long iRectJ0, long iRectI1, long iRectJ1, long oldLi, long oldLj, long oldUi, long oldUj, long newLi, long newLj, long newUi, long newUj, int extNumber=0, int debug=0);

#endif // CRABIMAGE_H
