/*
 
 Fits image blob class
 
*/

#ifndef CRABFITSIMAGEBLOBCLASS_H
#define CRABFITSIMAGEBLOBCLASS_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip> // std::cout
#include <cmath> // NAN, std::isnan
#include <algorithm> // std::sort

class CrabFitsImageBlobClass
{
public:
    CrabFitsImageBlobClass(double *image_data, int size_x, int size_y, int pos_x, int pos_y);
    void clear_variables();
    double compute_pixel_chain_median();
    double compute_pixel_rms_noise();
    
protected:
    class PixelCoordinateClass {
    public:
        PixelCoordinateClass(CrabFitsImageBlobClass *p, int pos_x = 0, int pos_y = 0);
        ~PixelCoordinateClass();
        void set_coordinate(int pos_x, int pos_y);
        void set_value(double value);
        int x() { return mx; }
        int y() { return my; }
        int index() { return mindex; }
        double value() { return mvalue; }
        int nearest_upper_pos_x() { return mNearestUpperPosX; }
        int nearest_upper_pos_y() { return mNearestUpperPosY; }
        int nearest_lower_pos_x() { return mNearestLowerPosX; }
        int nearest_lower_pos_y() { return mNearestLowerPosY; }
        int nearest_pos_x() { return mNearestPosX; }
        int nearest_pos_y() { return mNearestPosY; }
        std::vector<PixelCoordinateClass *> nearest_brighter_pixels() { return mNearestBrighterPixels; }
        std::vector<PixelCoordinateClass *> nearest_fainter_pixels() { return mNearestFainterPixels; }
        void determine_nearest_pixels(int determine_type = 0, double threshold = NAN);
    private:
        CrabFitsImageBlobClass *mparent;
        int mx = 0; int my = 0; // pixel x y coordinate, 1-based
        int mindex = -1; // this is the index of the ImageData, 0-based
        double mvalue = NAN;
        double mNearestUpperVal = NAN; // see determine_nearest_pixel()
        double mNearestLowerVal = NAN; // see determine_nearest_pixel()
        double mNearestVal = NAN; // see determine_nearest_pixel()
        int mNearestUpperPosX = 0; // 1-based, see determine_nearest_pixel()
        int mNearestUpperPosY = 0; // 1-based, see determine_nearest_pixel()
        int mNearestLowerPosX = 0; // 1-based, see determine_nearest_pixel()
        int mNearestLowerPosY = 0; // 1-based, see determine_nearest_pixel()
        int mNearestPosX = 0; // 1-based, see determine_nearest_pixel()
        int mNearestPosY = 0; // 1-based, see determine_nearest_pixel()
        PixelCoordinateClass *mNearestValuePixel;
        PixelCoordinateClass *mNearestBrighterPixel;
        PixelCoordinateClass *mNearestFainterPixel;
        std::vector<PixelCoordinateClass *> mNearestBrighterPixels;
        std::vector<PixelCoordinateClass *> mNearestFainterPixels;
    };
    void find_maxima(int pos_x, int pos_y);
    void iteratively_grow(int pos_x, int pos_y);
    PixelCoordinateClass grow(PixelCoordinateClass pos, int direction = 0, double threshold = NAN, bool mask = false); // direction +1 (go uphill), -1 (go downhill), or 0 (cloest value).
    
private:
    double *ImageData;
    int ImageSizeX;
    int ImageSizeY;
    int mMaximaPosX;
    int mMaximaPosY;
    double ImageDataRmsNoise;
    std::vector<PixelCoordinateClass > PixelChain;
};


#endif // CRABFITSIMAGEBLOB_H
