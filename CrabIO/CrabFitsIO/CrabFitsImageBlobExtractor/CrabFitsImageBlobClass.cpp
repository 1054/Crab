/*
 
 Fits image blob class
 
 */

#include "CrabFitsImageBlobClass.h"


using namespace std;


CrabFitsImageBlobClass::CrabFitsImageBlobClass(double *image_data, int size_x, int size_y, int pos_x, int pos_y)
{
    if(image_data != NULL) {
        this->ImageData = image_data;
        this->ImageSizeX = size_x;
        this->ImageSizeY = size_y;
        this->find_maxima(pos_x, pos_y);
        this->iteratively_grow(this->mMaximaPosX, this->mMaximaPosY);
    }
}


void CrabFitsImageBlobClass::clear_variables()
{
    PixelChain.clear();
}



CrabFitsImageBlobClass::PixelCoordinateClass::PixelCoordinateClass(CrabFitsImageBlobClass *p, int pos_x, int pos_y) : mparent(p)
{
    mNearestValuePixel = new PixelCoordinateClass(mparent);
    mNearestBrighterPixel = new PixelCoordinateClass(mparent);
    mNearestFainterPixel = new PixelCoordinateClass(mparent);
    set_coordinate(pos_x, pos_y);
}

CrabFitsImageBlobClass::PixelCoordinateClass::~PixelCoordinateClass()
{
    for (int i = 0; i < mNearestBrighterPixels.size(); i++) { delete mNearestBrighterPixels[i]; }
    for (int i = 0; i < mNearestFainterPixels.size(); i++) { delete mNearestFainterPixels[i]; }
    mNearestBrighterPixels.clear();
    mNearestFainterPixels.clear();
    delete mNearestValuePixel;
    delete mNearestBrighterPixel;
    delete mNearestFainterPixel;
}

void CrabFitsImageBlobClass::PixelCoordinateClass::set_coordinate(int pos_x, int pos_y)
{
    mx = pos_x; my = pos_y; mindex = -1; mvalue = NAN;
    if(mx>0 && my>0) { mindex = (my-1) * mparent->ImageSizeX + (mx-1); mvalue = mparent->ImageData[mindex]; }
}

void CrabFitsImageBlobClass::PixelCoordinateClass::set_coordinate(double value)
{
    mvalue = value;
}

void CrabFitsImageBlobClass::PixelCoordinateClass::determine_nearest_pixels(int determine_type, double threshold)
{
    // find the three nearest pixels which have:
    // min of no less than
    // max of no greater than
    // and nearest value.
    //
    // example:
    //   1.2   1.8   1.9
    //   1.3   1.5   1.6
    //   1.2   1.1   1.7
    // the central pixel is current position
    // NearestUpper: "min of no less than" is 1.6
    // NearestLower: "max of no greater than" is 1.3
    // Nearest: "min of absolute difference" is 1.6
    //
    if(mparent == NULL) {return;}
    if(mparent->ImageData == NULL) {return;}
    if(mx <= 0 || my <= 0) {return;}
    //
    if(determine_type == +1) {
        mNearestUpperVal = NAN;
        mNearestUpperPosX = 0; // 1-based
        mNearestUpperPosY = 0; // 1-based
        mNearestBrighterPixel->set_coordinate(0, 0);
        for (int i = 0; i < mNearestBrighterPixels.size(); i++) { delete mNearestBrighterPixels[i]; }
        mNearestBrighterPixels.clear();
    } else if(determine_type == -1) {
        mNearestLowerVal = NAN;
        mNearestLowerPosX = 0; // 1-based
        mNearestLowerPosY = 0; // 1-based
        mNearestFainterPixel->set_coordinate(0, 0);
        for (int i = 0; i < mNearestFainterPixels.size(); i++) { delete mNearestFainterPixels[i]; }
        mNearestFainterPixels.clear();
    } else if(determine_type == 0) {
        mNearestVal = NAN;
        mNearestPosX = 0; // 1-based
        mNearestPosY = 0; // 1-based
        mNearestValuePixel->set_coordinate(0, 0);
    }
    double tCentralVal = mparent->ImageData[this->index()];
    for (int ty = my-1; ty <= my+1; ty++) {
        long tindex = (ty-1) * mparent->ImageSizeX;
        for (int tx = mx-1; tx <= mx+1; tx++) {
            // skip central pixel
            if(tx == mx && ty == my) { continue; }
            // skip diagonal pixels
            //if((tx-mx) == (ty-my)) { continue; }
            // make sure current pixel is within the image
            if(tx > 0 && tx <= mparent->ImageSizeX && ty > 0 && ty <= mparent->ImageSizeY) {
                double tCurrentVal = mparent->ImageData[tindex+(tx-1)];
                if(std::isnan(tCurrentVal)) { continue; }
                if(!std::isnan(threshold)) { if(tCurrentVal < threshold) { continue; } } // threshold is a lower limit
                if(determine_type == +1) {
                    if((tCurrentVal >= tCentralVal && std::isnan(mNearestUpperVal)) ||
                       (tCurrentVal >= tCentralVal && tCurrentVal < mNearestUpperVal)
                       ) {
                        mNearestUpperVal = tCurrentVal;
                        mNearestUpperPosX = tx;
                        mNearestUpperPosY = ty;
                        mNearestBrighterPixel.set_coordinate(tx, ty);
                    }
                    // and also find all brighter nearby pixels
                    if(tCurrentVal >= tCentralVal) {
                        PixelCoordinateClass *tPixel = new PixelCoordinateClass(mparent, tx, ty);
                        mNearestBrighterPixels.push_back(tPixel);
                    }
                }
                else if(determine_type == -1) {
                    if((tCurrentVal <= tCentralVal && std::isnan(mNearestLowerVal)) ||
                       (tCurrentVal <= tCentralVal && tCurrentVal > mNearestLowerVal)
                       ) {
                        mNearestLowerVal = tCurrentVal;
                        mNearestLowerPosX = tx;
                        mNearestLowerPosY = ty;
                        mNearestFainterPixel.set_coordinate(tx, ty);
                    }
                    // and also find all fainter nearby pixels
                    if((tCurrentVal <= tCentralVal) {
                        PixelCoordinateClass *tPixel = new PixelCoordinateClass(mparent, tx, ty);
                        mNearestFainterPixels.push_back(tPixel);
                    }
                }
                else if(determine_type == 0) {
                    if((std::isnan(mNearestVal)) ||
                       (std::fabs(tCurrentVal-tCentralVal) < std::fabs(mNearestVal-tCentralVal))
                       ) {
                        mNearestVal = tCurrentVal;
                        mNearestPosX = tx;
                        mNearestPosY = ty;
                        mNearestValuePixel.set_coordinate(tx, ty);
                    }
                }
                //std::cout << "PixelCoordinateClass::determine_nearest_pixels: mNearestLowerPosX=" << mNearestLowerPosX << ", mNearestLowerPosY=" << mNearestLowerPosY << ", mNearestLowerVal=" << mNearestLowerVal << ", tx=" << tx << ", ty=" << ty << ", tCurrentVal=" << tCurrentVal << ", tCentralVal=" << tCentralVal << ", threshold=" << threshold << std::endl;
            }
        }
    }
    return;
}


double CrabFitsImageBlobClass::compute_pixel_chain_median()
{
    // return median value of the PixelChain
    if(PixelChain.size() > 0) {
        std::vector<double> tvalues;
        for (int i = 0; i < PixelChain.size(); i++) {
            tvalues.push_back(PixelChain[i].value());
        }
        std::sort(tvalues.begin(), tvalues.end());
        return tvalues[(int)(tvalues.size()/2)];
    }
    return NAN;
}


double CrabFitsImageBlobClass::compute_pixel_rms_noise()
{
    // compute the rms noise of the whole image data
    this->ImageDataRmsNoise = NAN;
    if(ImageData != NULL && ImageSizeX > 0 && ImageSizeY > 0) {
        double sum = 0;
        int cnt = 0;
        for (int k = 0; k < ImageSizeX*ImageSizeY; k++) {
            if(std::isnan(ImageData[k])) { continue; }
            sum += std::pow(ImageData[k], 2);
            cnt++;
        }
        //std::cout << "CrabFitsImageBlobClass::compute_pixel_rms_noise() sum=" << sum << ", cnt=" << cnt << ", rms=" << std::sqrt(sum/cnt) << std::endl;
        this->ImageDataRmsNoise = std::sqrt(sum/cnt);
    }
    return this->ImageDataRmsNoise;
}



void CrabFitsImageBlobClass::find_maxima(int pos_x, int pos_y)
{
    // go uphill to find the maxima
    // pos_xy are 1-based image pixel coordinates
    PixelCoordinateClass pos(this, pos_x, pos_y);
    PixelCoordinateClass nextpos = pos;
    //
    // grow uphill
    while(nextpos.x() > 0 && nextpos.x() <= this->ImageSizeY && nextpos.y() > 0 && nextpos.y() <= this->ImageSizeY) {
        pos = nextpos;
        this->mMaximaPosX = pos.x();
        this->mMaximaPosY = pos.y();
        nextpos = this->grow(pos, +1); // threshold=NAN, mask=false
    }
}



void CrabFitsImageBlobClass::iteratively_grow(int pos_x, int pos_y)
{
    // go uphill to find the maxima
    // pos_xy are 1-based image pixel coordinates
    PixelCoordinateClass pos(this, pos_x, pos_y);
    PixelCoordinateClass nextpos = pos;
    //
    // grow downhill
    int CountIteration = 0;
    double threshold = compute_pixel_rms_noise();
    nextpos = pos;
    while(nextpos.x() > 0 && nextpos.x() <= this->ImageSizeY && nextpos.y() > 0 && nextpos.y() <= this->ImageSizeY) {
        pos = nextpos;
        PixelChain.push_back(pos);
        nextpos = this->grow(pos, -1, threshold, true); // mask=ture, i.e., mask out current pixel
        CountIteration++;
        //if(CountIteration>=35) {
        //    pos.set_coordinate(0, 0); // debug: set bad data to end the while loop
        //}
    }
}


/*
CrabFitsImageBlobClass::PixelCoordinateClass CrabFitsImageBlobClass::grow(PixelCoordinateClass pos, int direction)
{
    PixelCoordinateClass nextpos(this);
    if(this->ImageData != NULL) {
        // print current pixel coordinate and value as a debug info
        //std::cout << "ImageData[" << pos.x()-1 << "," << pos.y()-1 << "] = " << this->ImageData[pos.index()] << std::endl;
        std::cout << "image; point(" << pos.x() << "," << pos.y() << ") # point=box color=#e8139e" << std::endl;
        //
        // grow along the isoline
        // get surrounding 9 pixel values and find two cloest-value pixels
        //pos.determine_nearest_pixel();
        pos.determine_nearest_pixel( this->compute_pixel_chain_median() );
        //std::cout << "pos.nearest_lower_pos_x()=" << pos.nearest_lower_pos_x() << ", pos.nearest_lower_pos_y()=" << pos.nearest_lower_pos_y() << ", compute_pixel_chain_median()=" << compute_pixel_chain_median() << std::endl;
        //
        // mask current pixel
        this->ImageData[pos.index()] = NAN;
        //
        // prepare for the nextpos
        //nextpos.set_coordinate(pos.nearest_upper_pos_x(), pos.nearest_upper_pos_y());
        //nextpos.set_coordinate(pos.nearest_lower_pos_x(), pos.nearest_lower_pos_y());
        if(direction>0) {
            // grow uphill
            nextpos.set_coordinate(pos.nearest_upper_pos_x(), pos.nearest_upper_pos_y());
        } else {
            // grow nearest
            nextpos.set_coordinate(pos.nearest_pos_x(), pos.nearest_pos_y());
        }
        
    }
    return nextpos;
}
*/


CrabFitsImageBlobClass::PixelCoordinateClass CrabFitsImageBlobClass::grow(PixelCoordinateClass pos, int direction, double threshold, bool mask)
{
    PixelCoordinateClass nextpos(this);
    if(this->ImageData != NULL) {
        //
        // print current pixel coordinate and value as a debug info
        //std::cout << "ImageData[" << pos.x()-1 << "," << pos.y()-1 << "] = " << this->ImageData[pos.index()] << std::endl;
        //
        // write ds9.reg file
        std::cout << "image; point(" << pos.x() << "," << pos.y() << ") # point=box color=#e8139e" << std::endl;
        //
        // grow by getting surrounding 9 pixel values and find which are fainter than current pixel value but brighter than the threshold
        pos.determine_nearest_pixels( direction, threshold );
        //std::cout << "pos.nearest_lower_pos_x()=" << pos.nearest_lower_pos_x() << ", pos.nearest_lower_pos_y()=" << pos.nearest_lower_pos_y() << std::endl;
        //
        // mask current pixel
        if(mask == true) {
            this->ImageData[pos.index()] = NAN;
        }
        //
        // prepare for the nextpos
        //nextpos.set_coordinate(pos.nearest_upper_pos_x(), pos.nearest_upper_pos_y());
        //nextpos.set_coordinate(pos.nearest_lower_pos_x(), pos.nearest_lower_pos_y());
        if(direction == +1) {
            // grow uphill
            nextpos.set_coordinate(pos.nearest_upper_pos_x(), pos.nearest_upper_pos_y());
        } else if(direction == -1) {
            // grow downhill
            nextpos.set_coordinate(pos.nearest_lower_pos_x(), pos.nearest_lower_pos_y());
        } else if(direction == 0) {
            // grow nearest
            nextpos.set_coordinate(pos.nearest_pos_x(), pos.nearest_pos_y());
        }
        
    }
    return nextpos;
}



