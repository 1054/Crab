#include "myfitsImage.h"
#define pi 3.141592653589793

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*System Analyze Functions.*/
//NaN ：Not a number.
//IEEE对符点数的标准化中有一个规定：指数 is 255,底数不为0时，是Nan(Not a Number)表示不是一个浮点值.
//http://blog.csdn.net/minyangchina/archive/2009/10/15/4676989.aspx
int isFloatNaN(float f)
{
    //float fNaN;
    //int *iNaN=(int *)&fNaN;
    int  *i=(int *)&f;
    char *c=(char *)&f;
    unsigned char c0,c1,c2,c3;  //Here, we MUST use unsigned char, make sure >>2 is filled by 00 not 11.
                                //otherwise using char makes 11000000>>2==11110000.
    c0=*(c+0); c1=*(c+1); c2=*(c+2); c3=*(c+3);

    if(isLittleEndian())
    {   //0x0000C07F=00000000 00000000 11000000 01111111
        if(c2>>6==0x03 && c3==0x7F)
            return 1;
        else
            return 0;
    }
    else
    {
        //0x7FC00000=01111111 11000000 00000000 00000000
        if(c0==0x7F && c1>>6==0x03)
            return 1;
        else
            return 0;
    }

    return *i==0x7FC00000;
    //return *(c+1)==127    //0x7F=127
}

int isLittleEndian()
{
    int isLittleEndian=1;

    /*decide the system's endian.*/
    union{short s; char c[2];}decideEndian;

    //TODO: what if sizeof(short) != 2 ??
    if(sizeof(short) == 2)
    {
        decideEndian.s = 0x0102;
        if(decideEndian.c[0] == 1 && decideEndian.c[1] == 2) //big enidan
            isLittleEndian=0;
        else if(decideEndian.c[0] == 2 && decideEndian.c[1] == 1) //little endian
            isLittleEndian=1;
    }

    return isLittleEndian;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*Moving Image to make 0,0 to be center.(For FFT)*/
//This would be used for FFT. By assuming the image is periodic, we can move and make the coordi-
//nary 0,0 to be at the original center of the image.
//Note that the nnX and nnY must be 2 odd numbers.
float *imageCenteringF(float *f, int nnX, int nnY)
{
    //nnX and nnY MUST both be odd number.
    float *g;
    int i=0,j=0,m=0;
    int Delta1=0,Delta2=0,Delta3=0,Delta4=0;
    g = (float *)malloc(nnX*nnY*sizeof(float));
    if(g==NULL) return NULL;
    if(nnX%2!=0 && nnY%2!=0)
    {
        Delta1 = nnX*(nnY-1)/2 + (nnX-1)/2;
        Delta2 = nnX*(nnY-1)/2 - (nnX-1)/2;
        Delta3 =-nnX*(nnY-1)/2 + (nnX-1)/2;
        Delta4 =-nnX*(nnY-1)/2 - (nnX-1)/2;
        for(j=0,m=0l;j<=(nnY-1)/2;j++)
        {
            for(i=0;i<=(nnX-1)/2;i++,m++)
            {
                g[m+Delta1]=f[m];
                g[m+Delta1]=f[m];
            }
            m-=1;
            for(i=(nnX-1)/2;i<(nnX-1);i++,m++)
            {
                g[m+Delta2]=f[m];
                g[m+Delta2]=f[m];
            }
            m+=1;
        }
        m-=nnX;
        for(j=(nnY-1)/2;j<(nnY-1);j++)
        {
            for(i=0;i<=(nnX-1)/2;i++,m++)
            {
                g[m+Delta3]=f[m];
                g[m+Delta3]=f[m];
            }
            m-=1;
            for(i=(nnX-1)/2;i<(nnX-1);i++,m++)
            {
                g[m+Delta4]=f[m];
                g[m+Delta4]=f[m];
            }
            m+=1;
        }
        m=0l;
        return g;
    }
    else
    {
        free((void *)g);
        return NULL;
    }
}

double *imageCenteringD(double *f, int nnX, int nnY)
{
    //nnX and nnY MUST both be odd number.
    double *g;
    int i=0,j=0,m=0,destroy=0;
    int Delta1=0,Delta2=0,Delta3=0,Delta4=0;
    g = (double *)malloc(nnX*nnY*sizeof(double));
    if(g==NULL) return NULL;
    if(nnX%2!=0 && nnY%2!=0)
    {
        Delta1 = nnX*(nnY-1)/2 + (nnX-1)/2;
        Delta2 = nnX*(nnY-1)/2 - (nnX-1)/2;
        Delta3 =-nnX*(nnY-1)/2 + (nnX-1)/2;
        Delta4 =-nnX*(nnY-1)/2 - (nnX-1)/2;
        for(j=0,m=0l;j<=(nnY-1)/2;j++)
        {
            for(i=0;i<=(nnX-1)/2;i++,m++)
            {
                g[m+Delta1]=f[m];
                g[m+Delta1]=f[m];
            }
            m-=1;
            for(i=(nnX-1)/2;i<(nnX-1);i++,m++)
            {
                g[m+Delta2]=f[m];
                g[m+Delta2]=f[m];
            }
            m+=1;
        }
        m-=nnX;
        for(j=(nnY-1)/2;j<(nnY-1);j++)
        {
            for(i=0;i<=(nnX-1)/2;i++,m++)
            {
                g[m+Delta3]=f[m];
                g[m+Delta3]=f[m];
            }
            m-=1;
            for(i=(nnX-1)/2;i<(nnX-1);i++,m++)
            {
                g[m+Delta4]=f[m];
                g[m+Delta4]=f[m];
            }
            m+=1;
        }
        m=0l;
        if(destroy==1) free((void *)f);
        return g;
    }
    else
    {
        free((void *)g);
        return NULL;
    }
}

float *imageCenteringD2F(double *f, int nnX, int nnY)
{
    //nnX and nnY MUST both be odd number.
    float *g;
    int i=0,j=0,m=0,destroy=0;
    int Delta1=0,Delta2=0,Delta3=0,Delta4=0;
    g = (float *)malloc(nnX*nnY*sizeof(float));
    if(g==NULL) return NULL;
    if(nnX%2!=0 && nnY%2!=0)
    {
        Delta1 = nnX*(nnY-1)/2 + (nnX-1)/2;
        Delta2 = nnX*(nnY-1)/2 - (nnX-1)/2;
        Delta3 =-nnX*(nnY-1)/2 + (nnX-1)/2;
        Delta4 =-nnX*(nnY-1)/2 - (nnX-1)/2;
        for(j=0,m=0l;j<=(nnY-1)/2;j++)
        {
            for(i=0;i<=(nnX-1)/2;i++,m++)
            {
                g[m+Delta1]=(float)f[m];
                g[m+Delta1]=(float)f[m];
            }
            m-=1;
            for(i=(nnX-1)/2;i<(nnX-1);i++,m++)
            {
                g[m+Delta2]=(float)f[m];
                g[m+Delta2]=(float)f[m];
            }
            m+=1;
        }
        m-=nnX;
        for(j=(nnY-1)/2;j<(nnY-1);j++)
        {
            for(i=0;i<=(nnX-1)/2;i++,m++)
            {
                g[m+Delta3]=(float)f[m];
                g[m+Delta3]=(float)f[m];
            }
            m-=1;
            for(i=(nnX-1)/2;i<(nnX-1);i++,m++)
            {
                g[m+Delta4]=(float)f[m];
                g[m+Delta4]=(float)f[m];
            }
            m+=1;
        }
        m=0l;
        if(destroy==1) free((void *)f);
        return g;
    }
    else
    {
        free((void *)g);
        return NULL;
    }
}

float *imageCenteringReplaceF(float *f, int nnX, int nnY) //will replace the input array.
{
    //nnX and nnY MUST both be odd number.
    float *g;
    int i=0,j=0,m=0,destroy=0;
    int Delta1=0,Delta2=0,Delta3=0,Delta4=0;
    g = (float *)malloc(nnX*nnY*sizeof(float));
    if(g==NULL) return NULL;
    destroy = 1;
    if(nnX%2!=0 && nnY%2!=0)
    {
        Delta1 = nnX*(nnY-1)/2 + (nnX-1)/2;
        Delta2 = nnX*(nnY-1)/2 - (nnX-1)/2;
        Delta3 =-nnX*(nnY-1)/2 + (nnX-1)/2;
        Delta4 =-nnX*(nnY-1)/2 - (nnX-1)/2;
        for(j=0,m=0l;j<=(nnY-1)/2;j++)
        {
            for(i=0;i<=(nnX-1)/2;i++,m++)
            {
                g[m+Delta1]=f[m];
                g[m+Delta1]=f[m];
            }
            m-=1;
            for(i=(nnX-1)/2;i<(nnX-1);i++,m++)
            {
                g[m+Delta2]=f[m];
                g[m+Delta2]=f[m];
            }
            m+=1;
        }
        m-=nnX;
        for(j=(nnY-1)/2;j<(nnY-1);j++)
        {
            for(i=0;i<=(nnX-1)/2;i++,m++)
            {
                g[m+Delta3]=f[m];
                g[m+Delta3]=f[m];
            }
            m-=1;
            for(i=(nnX-1)/2;i<(nnX-1);i++,m++)
            {
                g[m+Delta4]=f[m];
                g[m+Delta4]=f[m];
            }
            m+=1;
        }
        m=0l;
        if(destroy==1) free((void *)f);
        return g;
    }
    else
    {
        free((void *)g);
        return NULL;
    }
}

double *imageCenteringReplaceD(double *f, int nnX, int nnY) //will replace the input array.
{
    //nnX and nnY MUST both be odd number.
    double *g;
    int i=0,j=0,m=0,destroy=0;
    int Delta1=0,Delta2=0,Delta3=0,Delta4=0;
    g = (double *)malloc(nnX*nnY*sizeof(double));
    if(g==NULL) return NULL;
    destroy = 1;
    if(nnX%2!=0 && nnY%2!=0)
    {
        Delta1 = nnX*(nnY-1)/2 + (nnX-1)/2;
        Delta2 = nnX*(nnY-1)/2 - (nnX-1)/2;
        Delta3 =-nnX*(nnY-1)/2 + (nnX-1)/2;
        Delta4 =-nnX*(nnY-1)/2 - (nnX-1)/2;
        for(j=0,m=0l;j<=(nnY-1)/2;j++)
        {
            for(i=0;i<=(nnX-1)/2;i++,m++)
            {
                g[m+Delta1]=f[m];
                g[m+Delta1]=f[m];
            }
            m-=1;
            for(i=(nnX-1)/2;i<(nnX-1);i++,m++)
            {
                g[m+Delta2]=f[m];
                g[m+Delta2]=f[m];
            }
            m+=1;
        }
        m-=nnX;
        for(j=(nnY-1)/2;j<(nnY-1);j++)
        {
            for(i=0;i<=(nnX-1)/2;i++,m++)
            {
                g[m+Delta3]=f[m];
                g[m+Delta3]=f[m];
            }
            m-=1;
            for(i=(nnX-1)/2;i<(nnX-1);i++,m++)
            {
                g[m+Delta4]=f[m];
                g[m+Delta4]=f[m];
            }
            m+=1;
        }
        m=0l;
        if(destroy==1) free((void *)f);
        return g;
    }
    else
    {
        free((void *)g);
        return NULL;
    }
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*Type Casting Program.*/
char *itoaL(int n)  //convert integer to char * .
{
    int i=0;
    char *ch1;
    char *ch2;
    char *ch3;
    int size3=0;
    ch1 = (char *)malloc(25*sizeof(char));  //max len <= 24.
    size3 = sprintf(ch1,"%d",n);
    ch2 = strtrim(ch1);
    ch3 = (char *)malloc((size3+1)*sizeof(char));
    for(i=0;i<size3;i++)
    {
        *(ch3+i) = *(ch2+i);
    }
    *(ch3+size3)='\0';
    free((void *)ch1);
    ch1 = NULL;
    ch2 = NULL;
    return ch3;
}

char *strtrim(char *str)  //trim the blank spaces at the head and tail of a string.
{
    char *head = str;
    char *tail = str + strlen(str); //tail now point to the end mark of a string '\0'.
    while(*head==' ')
    {
        head++;
    }
    tail--;
    while(*tail==' ')
    {
        *tail = '\0';
        tail--;
    }
    return head;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*Image Data Type Casting Program.*/
//convert float array to double array. (creating a new one.)
double *imageFloat2Double(float *f, int nnN)
{
    int i=0;
    double *d;
    d = (double *)malloc(nnN*sizeof(double));          //malloc
    for(i=0;i<nnN;i++)
    {
        d[i]=(double)f[i];
    }
    return d;
}

float *imageDouble2Float(double *d, int nnN)
{
    int i=0;
    float *f;
    f = (float *)malloc(nnN*sizeof(float));            //malloc
    for(i=0;i<nnN;i++)
    {
        f[i]=(float)d[i];
    }
    return f;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*Image Cutting and Filling Program.*/
float *imageCutEdge(float *oldimg, int OldWidth, int OldHeight)
{
    int NewWidth = OldWidth + OldWidth%2 -1;
    int NewHeight = OldHeight + OldHeight%2 -1;
    return imageCut(oldimg,OldWidth,OldHeight,0,0,NewWidth,NewHeight);
}

float *imageCut(float *oldimg, int OldWidth, int OldHeight, int i0, int j0, int NewWidth, int NewHeight)
{
    //i0,j0 starts from left-bottom vertex.
    int i,j,j1,j7,j8;
    float fvalue;
    float *newimg;
    newimg = (float *)malloc(NewWidth*NewHeight*sizeof(float));
    j8=OldHeight;
    for(j=0;j<NewHeight;j++)
    {
        j1 = (j+j0)*OldWidth;
        j7 = j*NewWidth;
        for(i=0;i<NewWidth;i++)
        {
            fvalue = oldimg[j1+i+i0];
            //TODO: test NAN.
            if(isFloatNaN(fvalue))
                fvalue=0.0;
            newimg[j7+i] = fvalue;
        }
    }

    return newimg;
}

float *imageCutCF(float *oldimg, int OldWidth, int OldHeight, int Oldic, int Oldjc, int NewWidth, int NewHeight)
{   //this program can cut and fill automatically.
    //ic,jc stars from the center point.
    //会调整使得NewWidth,NewHeight为奇数
    int i0,j0;
    NewWidth = NewWidth + NewWidth%2 - 1;    //make NewWidth,NewHeight to be odd.
    NewHeight = NewHeight + NewHeight%2 - 1;
    i0=Oldic-(NewWidth-1)/2;
    j0=Oldjc-(NewHeight-1)/2;
    if(i0>=0 && j0>=0&& (j0+NewHeight)<=OldHeight && (i0+NewWidth)<=OldWidth)
    {
        return imageCut(oldimg,OldWidth,OldHeight,i0,j0,NewWidth,NewHeight);
    }
    else if(i0<0 && j0<0&& (j0+NewHeight)>OldHeight && (i0+NewWidth)>OldWidth)
    {
        return imageFill(oldimg,OldWidth,OldHeight,i0,j0,NewWidth,NewHeight);
    }
    else
    {
        printf("imageCut: not accomplished for non-center-symmetric cut or fill.\n");
        return NULL;
    }
}

float *imageFill(float *oldimg, int OldWidth, int OldHeight, int i0, int j0, int NewWidth, int NewHeight)
{
    //i0,j0 starts from left-bottom vertex.
    int i,j,i1,j1;
    float fvalue;
    float *newimg;
    newimg = (float *)malloc(NewWidth*NewHeight*sizeof(float));
    for(j=0;j<NewHeight;j++)
    {
        j1 = (j+j0);
        for(i=0;i<NewWidth;i++)
        {
            i1=i+i0;
            if(j1>=0 && j1<OldHeight && i1>=0 && i1<OldWidth)
            {
                fvalue = oldimg[j1*OldWidth+i1];
            }
            else
            {
                fvalue = 0.0;
            }
            //TODO: test NAN.
            newimg[j*NewWidth+i] = fvalue;
        }
    }

    return newimg;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*Image Rebin Program.*/
//This program has a same effect with IDL.astrolib's frebin.
float *imageRebin(float *oldimg, int OldWidth, int OldHeight, int NewWidth, int NewHeight)
{
    int   i,j;
    int   X0,Y0,X1,Y1,X2,Y2;
    float x,y,dx,dy,f1,f2,f3,f4,fu,fv,fvalue;
    float *newimg;

    newimg = (float *)malloc(NewWidth*NewHeight*sizeof(float));

    dx = ((float)OldWidth-1.0)/((float)NewWidth-1.0);
    dy = ((float)OldHeight-1.0)/((float)NewHeight-1.0);

    /*四个顶点*/
    newimg[0]=oldimg[0]; //lbot
    newimg[NewWidth-1]=oldimg[OldWidth-1]; //rbot
    newimg[(NewHeight-1)*NewWidth]=oldimg[(OldHeight-1)*OldWidth]; //ltop
    newimg[NewHeight*NewWidth-1]=oldimg[OldHeight*OldWidth-1]; //rtop

    /*上下左右边界*/
    j=0; Y0=0; //first line boundary.
    for(i=1;i<NewWidth-1;i++)
    {
        x=0.0+i*dx;
        X1=(int)x; X2=X1+1; //X1-X2 == -1.0
        fu=x-(float)X1;
        newimg[j*NewWidth+i]=fu*oldimg[Y0*OldWidth+X1]+(1.0-fu)*oldimg[Y0*OldWidth+X2];
    }
    j=NewHeight-1; Y0=OldHeight-1; //last line boundary.
    for(i=1;i<NewWidth-1;i++)
    {
        x=0.0+i*dx;
        X1=(int)x; X2=X1+1; //X1-X2 == -1.0
        fu=x-(float)X1;
        newimg[j*NewWidth+i]=fu*oldimg[Y0*OldWidth+X1]+(1.0-fu)*oldimg[Y0*OldWidth+X2];
    }
    i=0; X0=0; //left side boundary.
    for(j=1;j<NewHeight-1;j++)
    {
        y=0.0+j*dy;
        Y1=(int)y; Y2=Y1+1; //Y1-Y2 == -1.0
        fv=y-(float)Y1;
        newimg[j*NewWidth+i]=fv*oldimg[Y1*OldWidth+X0]+(1.0-fv)*oldimg[Y2*OldWidth+X0];
    }
    i=NewWidth-1; X0=OldWidth-1; //right side boundary.
    for(j=1;j<NewHeight-1;j++)
    {
        y=0.0+j*dy;
        Y1=(int)y; Y2=Y1+1; //Y1-Y2 == -1.0
        fv=y-(float)Y1;
        newimg[j*NewWidth+i]=fv*oldimg[Y1*OldWidth+X0]+(1.0-fv)*oldimg[Y2*OldWidth+X0];
    }

    /*内点*/
    for(j=1;j<NewHeight-1;j++)
    {
        for(i=1;i<NewWidth-1;i++)
        {
            x=0.0+i*dx; y=0.0+j*dy;
            X1=(int)x; X2=X1+1; //X1-X2 == -1.0
            Y1=(int)y; Y2=Y1+1; //Y1-Y2 == -1.0
            /*Bilinear Interpolation.*/
            //suppose i,j are integers and u,v are fractions.
            //f(i+u,j+v) = (1-u)(1-v)*f(i, j)
            //           +   u  (1-v)*f(i+1,j)
            //           + (1-u)  v  *f(i,j+1)
            //           +   u    v  *f(i+1,j+1)
            //And be careful with the index of the data array.
            fu=x-(float)X1; fv=y-(float)Y1;
            f1 = oldimg[Y1*OldWidth+X1]; //bot-left
            f2 = oldimg[Y1*OldWidth+X2]; //bot-right
            f3 = oldimg[Y2*OldWidth+X1]; //top-left
            f4 = oldimg[Y2*OldWidth+X2]; //top-right
            fvalue = (1.0-fu)*(1.0-fv)*f1 + fu*(1.0-fv)*f2 + (1.0-fu)*fv*f3 + fu*fv*f4;
            newimg[j*NewWidth+i]=fvalue;
        }
    }

    return newimg;

}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
float *imageRotate(float *img, int Width, int Height, float degree, int CenterPointX, int CenterPointY)
{
    int i,j;
    int nnX,nnY;
    float theta;
    float *imgR;
    float p0,q0;
    float p,q;
    float fpq;
    float sinA,cosA,tanA,ctgA;

    nnX = Width;
    nnY = Height;
    theta = degree/180.0*pi;
    //degree can not be 0,90,180,270,360, otherwise tanA or ctgA will get an NAN.
    sinA=(float)sin(theta);cosA=(float)cos(theta);tanA=(float)tan(theta);ctgA=1.0/tanA;
    p0=(float)CenterPointX;q0=(float)CenterPointY;

    imgR = (float *)malloc(nnX*nnY*sizeof(float));

    for(j=0;j<nnY;j++)
    {
        for(i=0;i<nnX;i++)
        {
            //(p,q)->rotate->(i,j)
            p = (i/sinA + j/cosA - p0/sinA - q0/cosA)/(tanA+ctgA) + p0;
            q = (j/sinA - i/cosA - q0/sinA + p0/cosA)/(ctgA+tanA) + q0;
            if(p>=0.0 && q>=0.0 && p<(float)nnX-1.0 && q<(float)nnY-1.0)
            {
                fpq = biliInter(img,nnX,nnY,p,q);
                imgR[j*nnX+i] = fpq;
            }
            else if(p==(float)nnX-1.0 || q==(float)nnY-1.0)
            {
                imgR[j*nnX+i] = img[nnY*nnX-1];
            }
            else
            {
                imgR[j*nnX+i] = -0.0; //<TODO:>shoule we use NAN
            }
        }
    }
    return imgR;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
float biliInter(float *data, int nnX, int nnY, float fx, float fy)
{
    //Here fx, fy are NOT in the "natural data coordinate" system starts with 1.0, 1.0.
    //Here fx, fy are in the "digital data coordinate" system starts with 0.0, 0.0.
    int nx,ny;
    int m1=0l, m2=0l, m3=0l, m4=0l;
    float fu=0.0,fv=0.0;
    float f1=0.0,f2=0.0,f3=0.0,f4=0.0,fvalue=0.0;
    nx = (int)fx;
    ny = (int)fy;
    fu = fx - (float)nx;
    fv = fy - (float)ny;
    //fx should be in [0,nnX-1)
    //fy should be in [0,nnY-1)
    /*Consider inner region, exclude the boundaries and outer region.*/
    if(nx>0 && ny>0 && nx<nnX && ny<nnY)
    {
        /*Bilinear Interpolation.*/
        //suppose i,j are integers and u,v are fractions.
        //f(i+u,j+v) = (1-u)(1-v)*f( i , j )
        //           +   u  (1-v)*f(i+1, j )
        //           + (1-u)  v  *f( i ,j+1)
        //           +   u    v  *f(i+1,j+1)
        //And be careful with the index of the data array.
        m1 = ny*nnX + nx; //m1 =    ny *nnX  + nx;
        m2 = m1 + 1  ;    //m2 =    ny *nnX  + (nx+1);
        m3 = m1 + nnX;    //m3 = (ny+1)*nnX  + nx;
        m4 = m3 + 1  ;    //m4 = (ny+1)*nnX  + (nx+1);
        f1 = data[m1];
        f2 = data[m2];
        f3 = data[m3];
        f4 = data[m4];
        fvalue = (1.0-fu)*(1.0-fv)*f1 + fu*(1.0-fv)*f2 + (1.0-fu)*fv*f3 + fu*fv*f4;
    }
    return fvalue;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*Image Statistical Analysze Program.*/
float imageTotalF(float *img, int Width, int Height)
{
    int i=0;
    float total=0.0;
    total=0.0;
    for(i=0;i<Width*Height;i++)
    {
        total += img[i];
    }
    return total;
}


