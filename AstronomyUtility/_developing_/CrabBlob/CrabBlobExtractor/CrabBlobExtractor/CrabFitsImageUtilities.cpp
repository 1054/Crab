#include "CrabFitsImageUtilities.h"

CrabFitsImageUtilities::CrabFitsImageUtilities()
{
}


QImage CrabFitsImageUtilities::data2qimage(double *data, long dimension[2], const char *mode, uchar *data_uchar)
{
    long width = dimension[0];
    long height = dimension[1];
    data_uchar = data2image(data, width*height, mode);
    QImage qimg = QImage(data_uchar, width, height, QImage::Format_ARGB32);
    return qimg;
}


uchar *CrabFitsImageUtilities::data2image(double *data, long width, long height, const char *mode)
{
    return data2image(data,width*height,mode);
}


uchar *CrabFitsImageUtilities::data2image(double *data, long size, const char *mode)
{
    long i=0,j=0;
    uchar *img;
    QString smode(mode);
    img = (uchar *)malloc(4*size*sizeof(uchar)); // 4bytes 32bits  QImage::Format_ARGB32
    double pixvalue = 0.0;
    double maxvalue = max_element_no_nan(data,size);
    double minvalue = min_element_no_nan(data,size);
    double scalefactor = 0.0;
    double intercept   = 0.0;
    qDebug() << "CrabFitsImageUtilities::data2image()" << "max =" << maxvalue << "min =" << minvalue;
    if(smode.isEmpty() || smode.startsWith("lin"))
    {
        scalefactor = 254.5/(maxvalue-minvalue);
        intercept = -scalefactor*minvalue;
        for(i=0,j=0;i<4*size;i+=4,j++)
        {
            pixvalue = scalefactor*data[j] + intercept;
            img[i]   = (uchar)(pixvalue);
            img[i+1] = (uchar)(pixvalue);
            img[i+2] = (uchar)(pixvalue);
            img[i+3] = 0xFF;
        }
    }
    else if(smode.startsWith("anti-lin"))
    {
        scalefactor = 254.5/(minvalue-maxvalue);//anti-color
        intercept = -scalefactor*maxvalue;
        for(i=0,j=0;i<4*size;i+=4,j++)
        {
            pixvalue = scalefactor*data[j] + intercept;
            img[i]   = (uchar)(pixvalue);
            img[i+1] = (uchar)(pixvalue);
            img[i+2] = (uchar)(pixvalue);
            img[i+3] = 0xFF;
        }
    }
    else if(smode.startsWith("log"))
    {
        double zerobase = minvalue>0.0?-minvalue*0.999:-minvalue*1.001; if(minvalue==0.0) {zerobase=maxvalue*0.000001;}
        scalefactor = 254.5/(double)(log10(maxvalue+zerobase));
        intercept = 0.0;//-scalefactor*(double)log10(minvalue+zerobase);
        qDebug() << "scalefactor =" << scalefactor << "intercept =" << intercept << "zerobase =" << zerobase;
        for(i=0,j=0;i<4*size;i+=4,j++)
        {
            pixvalue = (double)(scalefactor*log10(data[j]+zerobase) + intercept); if(pixvalue<0.0) {pixvalue=0.0;}
            img[i]   = (uchar)(pixvalue); //blue   0x00=None 0xFF=Blue
            img[i+1] = (uchar)(pixvalue); //green  0x00=None 0xFF=Green
            img[i+2] = (uchar)(pixvalue); //red    0x00=None 0xFF=Red
            img[i+3] = 0xFF; //alpha  0x00=transparent 0xFF=non-transparent
            //when QImage reads uchar * on a little-endian pc, the order is inversed.
            //<TODO: big-endian, swap bytes!>
            //<TODO> debug
            //if(fabs(data[j]+42.9895)<1e-5){
            //    qDebug()<<"data[j]"<<data[j]<<"log10(data[j]+zerobase)"<<log10(data[j]+zerobase)<<"pixvalue"<<pixvalue<<"img[i]"<<img[i];
            //}
        }
    }
    else if(smode.startsWith("anti-log"))
    {
        double zerobase = minvalue>0.0?-minvalue*0.999:-minvalue*1.001; if(minvalue==0.0) {zerobase=maxvalue*0.000001;}
        scalefactor = 254.5/(double)(-log10(maxvalue+zerobase));//anti-color
        intercept = 254.5;
        for(i=0,j=0;i<4*size;i+=4,j++)
        {
            pixvalue = (double)(scalefactor*log10(data[j]+zerobase) + intercept); if(pixvalue<0.0) {pixvalue=0.0;}
            img[i]   = (uchar)(pixvalue);
            img[i+1] = (uchar)(pixvalue);
            img[i+2] = (uchar)(pixvalue);
            img[i+3] = 0xFF;
        }
    }
    return img;
}



double CrabFitsImageUtilities::max_element(double *data, long size)
{
    long i=0;
    double max=data[0];
    for(i=0;i<size;i++)
        if(data[i]>max) max=data[i];
    return max;
}


double CrabFitsImageUtilities::min_element(double *data, long size)
{
    long i=0;
    double min=data[0];
    for(i=0;i<size;i++)
        if(data[i]<min) min=data[i];
    return min;
}



double CrabFitsImageUtilities::max_element_no_nan(double *data, long size, long *found_index)
{
    long i=0;
    double max=NAN;
    for(i=0;i<size;i++) {
        if(!isnan(data[i]) && (isnan(max) || data[i]>max)) {
            if(found_index) {*found_index=i;}
            max=data[i];
        }
    }
    return max;
}


double CrabFitsImageUtilities::min_element_no_nan(double *data, long size, long *found_index)
{
    long i=0;
    double min=NAN;
    for(i=0;i<size;i++) {
        if(!isnan(data[i]) && (isnan(min) || data[i]<min)) {
            if(found_index) {*found_index=i;}
            min=data[i];
        }
    }
    return min;
}


double *CrabFitsImageUtilities::copy_image_no_nan(double *data, long width, long height, long istart, long jstart, long iend, long jend)
{
    long i=0, j=0, newi=0, newj=0;
    if(istart==-1) {istart=0;}
    if(jstart==-1) {jstart=0;}
    if(iend==-1) {iend=width-1;}
    if(jend==-1) {jend=height-1;}
    if(istart<0) {istart=0;}
    if(jstart<0) {jstart=0;}
    if(iend>width-1) {iend=width-1;}
    if(jend>height-1) {jend=height-1;}
    if(iend<0) {return NULL;}
    if(jend<0) {return NULL;}
    if(istart>width-1) {return NULL;}
    if(jstart>height-1) {return NULL;}
    if((istart+1)*(jstart+1)>width*height) {return NULL;}
    if((iend+1)*(jend+1)>width*height) {return NULL;}
    long newwidth = iend-istart+1;
    long newheight = jend-jstart+1;
    double *newdata = (double *)malloc(sizeof(double)*newwidth*newheight);
    memset(newdata,0x0,sizeof(double)*newwidth*newheight);
    for(j=jstart,newj=0;j<=jend;j++,newj++) {
        for(i=istart,newi=0;i<=iend;i++,newi++) {
            newdata[newj*newwidth+newi]=data[j*width+i];
        }
    }
    return newdata;
}


QMap<QString,double> CrabFitsImageUtilities::curve_of_growth(double *data, long width, long height, double xstart, double ystart, double max_distance, double pos_angle, double delta_angle)
{
    QMap<QString,double> output_curve_of_growth;
    long i=0,j=0,dist=0;
    long istart = long(xstart-1.0); // from which pixel we start the measure of curve of growth
    long jstart = long(ystart-1.0); // from which pixel we start the measure of curve of growth
    if(istart<0 || jstart<0 || istart>width-1 || jstart>height-1) {return output_curve_of_growth;}
    double xdist = 0;
    double ydist = 0;
    double rdist = 0;
    xdist = double(dist)*cos((pos_angle+90.0)/180.0*M_PI);
    ydist = double(dist)*sin((pos_angle+90.0)/180.0*M_PI);
    i = long(round(xstart-1.0+xdist));
    j = long(round(ystart-1.0+ydist));
    int pixel_precision = 3;
    while (dist>=0) {
        if(i<0 || j<0 || i>width-1 || j>height-1) break;
        rdist = sqrt( (double(i)-(xstart-1.0))*(double(i)-(xstart-1.0)) + (double(j)-(ystart-1.0))*(double(j)-(ystart-1.0)) );
        output_curve_of_growth[QString("%1").arg(rdist,0,'f',pixel_precision)] = data[j*width+i];
        //qDebug() << "CrabFitsImageUtilities::curve_of_growth()" << "i" << i << "j" << j << "dist" << dist << "xdist" << xdist << "ydist" << ydist;
        if(dist>0 && delta_angle>0.0) {
            double step_angle = 360.0;
            if(dist==0) step_angle = 1;
            else if(dist==1) step_angle = 4;
            else if(dist==2) step_angle = 8;
            else if(dist==3) step_angle = 16;
            else if(dist==4) step_angle = 32;
            else if(dist==5) step_angle = 64;
            else if(dist==6) step_angle = 128;
            else if(dist==7) step_angle = 256;
            double loop_angle = 0.0 + delta_angle/step_angle;
            while (loop_angle<=delta_angle) {
                // delta_angle is the 'thickness' of the pos_angle
                xdist = double(dist)*cos((pos_angle+90.0+loop_angle)/180.0*M_PI);
                ydist = double(dist)*sin((pos_angle+90.0+loop_angle)/180.0*M_PI);
                i = long(round(xstart-1.0+xdist));
                j = long(round(ystart-1.0+ydist));
                if(!(i<0 || j<0 || i>width-1 || j>height-1)) {
                    rdist = sqrt( (double(i)-(xstart-1.0))*(double(i)-(xstart-1.0)) + (double(j)-(ystart-1.0))*(double(j)-(ystart-1.0)) );
                    output_curve_of_growth[QString("%1").arg(rdist,0,'f',pixel_precision)] = data[j*width+i];
                }
                //qDebug() << "CrabFitsImageUtilities::curve_of_growth()" << "i" << i << "j" << j << "dist" << dist << "xdist" << xdist << "ydist" << ydist << "pos_angle" << pos_angle+loop_angle;
                // delta_angle is the 'thickness' of the pos_angle
                xdist = double(dist)*cos((pos_angle+90.0-loop_angle)/180.0*M_PI);
                ydist = double(dist)*sin((pos_angle+90.0-loop_angle)/180.0*M_PI);
                i = long(round(xstart-1.0+xdist));
                j = long(round(ystart-1.0+ydist));
                if(!(i<0 || j<0 || i>width-1 || j>height-1)) {
                    rdist = sqrt( (double(i)-(xstart-1.0))*(double(i)-(xstart-1.0)) + (double(j)-(ystart-1.0))*(double(j)-(ystart-1.0)) );
                    output_curve_of_growth[QString("%1").arg(rdist,0,'f',pixel_precision)] = data[j*width+i];
                }
                //qDebug() << "CrabFitsImageUtilities::curve_of_growth()" << "i" << i << "j" << j << "dist" << dist << "xdist" << xdist << "ydist" << ydist << "pos_angle" << pos_angle-loop_angle;
                // next loop_angle
                loop_angle = loop_angle + delta_angle/step_angle;
            }
        }
        dist++;
        if(max_distance>0 && dist>max_distance) break;
        xdist = double(dist)*cos((pos_angle+90.0)/180.0*M_PI);
        ydist = double(dist)*sin((pos_angle+90.0)/180.0*M_PI);
        i = long(round(double(istart)+xdist));
        j = long(round(double(jstart)+ydist));
    }
    return output_curve_of_growth;
}

