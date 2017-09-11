#ifndef CRABFITSIMAGEUTILITIES_H
#define CRABFITSIMAGEUTILITIES_H

#include <QImage>
#include <QByteArray>
#include <QList>
#include <QMap>
#include <QDebug>
#include <math.h>

class CrabFitsImageUtilities
{
public:
    CrabFitsImageUtilities();
    static QImage data2qimage(double *data, long dimension[2], const char *mode=NULL, uchar *data_uchar=NULL);
    static uchar *data2image(double *data, long width, long height, const char *mode=NULL);
    static uchar *data2image(double *data, long size, const char *mode=NULL);
    static double max_element(double *data, long size);
    static double min_element(double *data, long size);
    static double max_element_no_nan(double *data, long size, long *found_index=NULL);
    static double min_element_no_nan(double *data, long size, long *found_index=NULL);
    static double *copy_image_no_nan(double *data, long width, long height, long istart=-1, long jstart=-1, long iend=-1, long jend=-1);
    static QMap<QString,double> curve_of_growth(double *data, long width, long height, double xstart, double ystart, double max_distance=-1, double pos_angle=0.0, double delta_angle=0.0);
};

#endif // CRABFITSIMAGEUTILITIES_H
