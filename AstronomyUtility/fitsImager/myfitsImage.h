//
// snail is a kind of very small animal.
// This program aims to deal with simple image operation.
// Using code C, but with a file name of *.cpp.
//

#ifndef myfitsIMAGE_H
#define myfitsIMAGE_H
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <math.h>


/*System Analyze Functions.*/
int isFloatNaN(float f);
int isLittleEndian();

/*Type Casting Program.*/
char *itoaL(int n);  //convert integer to char * .
char *strtrim(char *str);   //trim the blank spaces at the head and tail of a string.

/*Image Data Type Casting Program.*/
double *imageFloat2Double(float *f, int nnN);
float  *imageDouble2Float(double *d, int nnN);

/*Moving Image to make 0,0 to be center.(For FFT)*/
float  *imageCentering(float *f, int nnX, int nnY);
double *imageCenteringD(double *f, int nnX, int nnY);
float  *imageCenteringD2F(double *f, int nnX, int nnY);
float  *imageCenteringReplaceF(float *f, int nnX, int nnY);
double *imageCenteringReplaceD(double *f, int nnX, int nnY);

/*Image Cutting and Filling Program.*/
float *imageCutEdge(float *oldimg, int OldWidth, int OldHeight);
float *imageCut(float *oldimg, int OldWidth, int OldHeight, int i0, int j0, int NewWidth, int NewHeight);
float *imageCutCF(float *oldimg, int OldWidth, int OldHeight, int ic, int jc, int NewWidth, int NewHeight);
float *imageFill(float *oldimg, int OldWidth, int OldHeight, int i0, int j0, int NewWidth, int NewHeight);

/*Image Rebin Program.*/
float *imageRebin(float *oldimg, int OldWidth, int OldHeight, int NewWidth, int NewHeight);

/*Image Rotate Program.*/
float *imageRotate(float *img, int Width, int Height, float degree, int CenterPointX, int CenterPointY);
float  biliInter(float *data, int nnX, int nnY, float fx, float fy);

/*Image Statistic Program.*/
float imageTotalF(float *img, int Width, int Height);


#endif // myfitsIMAGE_H
