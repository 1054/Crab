#ifndef CRABBLOB_H
#define CRABBLOB_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <vector>

using namespace std;

class blob_class
{
    public:
    double size;
    double rect[4]; // x0,y0,x1,y1
    double growth_ratio;
    double growth_limit; // detect threshold
    double growth_level; // deblend level
    std::vector<double> x; // perimeter
    std::vector<double> y; // perimeter
    std::vector<double> f; // perimeter
    std::vector<double> theta; // perimeter
    std::vector<double> xd; // inner area
    std::vector<double> yd; // inner area
    std::vector<double> fd; // inner area
    std::vector<double> fc; // center flux
    std::vector<double> xc; // center x
    std::vector<double> yc; // center y
    double* image_data;
    std::vector<long> *pixel_pool; // pool of good pixels, share between blobs
    long image_size[2];
    
    void init(double* input_image_data, long input_image_size[2], std::vector<long> &input_pixel_pool, double input_detect_threshold, double input_deblend_level);
    void set_deblend_level(double input_deblend_level);
    void set_detect_threshold(double input_detect_threshold, bool absolute_value = false);
    int plant(double* input_image_data, long input_image_size[2], std::vector<long> &input_pixel_pool, double input_detect_threshold, double input_deblend_level);
    int settle();
    int grow();
    
    double getmean(std::vector<double> v);
    double getsigma(std::vector<double> v);
    double getpix(long xinput, long yinput);
    double gettheta(long xinput, long yinput);
    int addpix(long xinput, long yinput, long insert_position);
    int rmpix(long pindex);
    int maskpix(long xinput, long yinput);
    
    int debuglevel;
};

struct blob_struct
{
    // <TODO> delete
    int size;
    int color;
    // track current line segment
    int x1;
    int x2;
    int y;
    // basic linked list
    struct blob* prev;
    struct blob* next;
    // siblings are connected segments
    struct blob* sib_p;
    struct blob* sib_n;
    // incremental average
    double center_x;
    double center_y;
    // bounding box
    int bb_x1, bb_y1, bb_x2, bb_y2;
    // single linked list for tracking all old pixels
    // struct blob* old;
};


void blob_detect(double *input_image_data, long input_image_size[2], std::vector<blob_class> &blob_list, double input_detect_threshold, double input_deblend_level);

void blob_print_polygon(std::vector<blob_class> blob_list, std::string output_file = "ds9.reg");



// 
// below are the functions for sorting one vector by another
// -- http://stackoverflow.com/questions/37368787/c-sort-one-vector-based-on-another-one
//
template <typename A, typename B>
void zip(
         std::vector<A> &a,
         std::vector<B> &b,
         std::vector<std::pair<A,B> > &zipped)
{
    for(size_t i=0; i<a.size(); ++i)
    {
        zipped.push_back(std::make_pair(a[i], b[i]));
    }
};

template <typename A, typename B>
void unzip(
           std::vector<std::pair<A,B> > &zipped,
           std::vector<A> &a,
           std::vector<B> &b)
{
    for(size_t i=0; i<a.size(); i++)
    {
        a[i] = zipped[i].first;
        b[i] = zipped[i].second;
    }
};

struct func_compare_theta {
    template <typename A, typename B>
    bool operator() (
                        std::pair<A,B> a,
                        std::pair<A,B> b)
    {
        return (a.second < b.second);
    };
};


#endif // CRABBLOB_H
