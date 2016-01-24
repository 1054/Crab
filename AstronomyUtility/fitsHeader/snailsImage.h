//snail is a kind of very small animal.
//This program aims to deal with simple image operation.
//Using code C, but with a file name of *.cpp.
//
#ifndef SNAILSIMAGE_H
#define SNAILSIMAGE_H


/*Snail's System and Data Judging Functions.*/
int isFloatNaN(float f);
int isLittleEndian();

/*Snail's Type Casting Program.*/
char *itoaL(int n);  //convert integer to char * .
char *strtrim(char *str);   //trim the blank spaces at the head and tail of a string.

/*Snail's Image Data Type Casting Program.*/
double *snailsFloat2Double(float *f, int nnN);
float *snailsDouble2Float(double *d, int nnN);

/*Snail's Moving Image to make 0,0 to be center.(For FFT)*/
float  *centeringImage(float *f, int nnX, int nnY);
double *centeringImageD(double *f, int nnX, int nnY);
float  *centeringImageD2F(double *f, int nnX, int nnY);
float  *centeringImageReplaceF(float *f, int nnX, int nnY);
double *centeringImageReplaceD(double *f, int nnX, int nnY);

/*Snail's Image Cutting and Filling Program.*/
float *snailsCutEdge(float *oldimg, int OldWidth, int OldHeight);
float *snailsCut(float *oldimg, int OldWidth, int OldHeight, int i0, int j0, int NewWidth, int NewHeight);
float *snailsCutCF(float *oldimg, int OldWidth, int OldHeight, int ic, int jc, int NewWidth, int NewHeight);
float *snailsFill(float *oldimg, int OldWidth, int OldHeight, int i0, int j0, int NewWidth, int NewHeight);

/*Snail's Image Rebin Program.*/
float *snailsRebin(float *oldimg, int OldWidth, int OldHeight, int NewWidth, int NewHeight);

/*Snail's Image Rotate Program.*/
float *snailsRotate(float *img, int Width, int Height, float degree, int CenterPointX, int CenterPointY);
float biliInter(float *data, int nnX, int nnY, float fx, float fy);

/*Snail's Image Statistic Program.*/
float snailsTotalF(float *img, int Width, int Height);


#endif // SNAILSIMAGE_H
