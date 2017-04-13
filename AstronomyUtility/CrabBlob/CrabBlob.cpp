#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <numeric>
#include <algorithm> // std::sort
#include <iterator>
#include <cmath>
#include <CrabBlob.h>
#define PI 3.14159265

using namespace std;

void blob_class::init(double* input_image_data, long input_image_size[2], std::vector<long> &input_pixel_pool, double input_detect_threshold = 0.02, double input_deblend_level = 1.0)
{
    this->image_size[0] = input_image_size[0];
    this->image_size[1] = input_image_size[1];
    this->image_data = input_image_data;
    this->pixel_pool = &input_pixel_pool;
    this->set_detect_threshold(input_detect_threshold,false); // set relative detect threshold
    this->set_deblend_level(input_deblend_level);
    this->debug_level = 0;
    if(this->debug_level>=1) {
        std::cout << "DEBUG: CrabBlob: blob_class::init() &input_pixel_pool = " << &input_pixel_pool << ", this->pixel_pool = " << this->pixel_pool << std::endl;
    }
    //int check_ok = this->settle();
    //input_pixel_pool = this->pixel_pool;
};

void blob_class::set_debug_level(int input_debug_level)
{
    if(input_debug_level>0) {
        this->debug_level = input_debug_level;
    } else {
        this->debug_level = 0;
    }
}

void blob_class::set_detect_threshold(double input_detect_threshold, bool absolute_value)
{
    if(absolute_value) {
        this->growth_limit = input_detect_threshold;
    } else {
        this->growth_ratio = input_detect_threshold;
        this->growth_limit = NAN;
    }
};

void blob_class::set_deblend_level(double input_deblend_level)
{
    this->growth_level = 2.0/input_deblend_level;
};

int blob_class::plant(double* input_image_data, long input_image_size[2], std::vector<long> &input_pixel_pool, double input_detect_threshold = 0.02, double input_deblend_level = 1.0) {
    // same as "this->init()"
    this->image_size[0] = input_image_size[0];
    this->image_size[1] = input_image_size[1];
    this->image_data = input_image_data;
    this->pixel_pool = &input_pixel_pool;
    this->set_detect_threshold(input_detect_threshold);
    this->set_deblend_level(input_deblend_level);
    int check_ok = 0;
    check_ok = settle();
    check_ok = grow();
    //input_pixel_pool = this->pixel_pool;
    return check_ok;
}

int blob_class::settle() {
    // find local maxima and settle there
    if(!this->image_data) {return -1;}
    if(this->pixel_pool->size()<=0) {return -1;}
    // find local maxima
    this->xc.push_back(this->pixel_pool->at(0) % this->image_size[0]);
    this->yc.push_back(this->pixel_pool->at(0) / this->image_size[0]);
    this->fc.push_back(this->image_data[this->pixel_pool->at(0)]);
    for(long ipix=0; ipix<this->pixel_pool->size(); ipix++) {
        double temp_pixel_value = this->image_data[this->pixel_pool->at(ipix)];
        if(fc.at(0) != fc.at(0) ||
           temp_pixel_value > fc.at(0))
        {
            fc.at(0) = this->image_data[this->pixel_pool->at(ipix)];
            xc.at(0) = this->pixel_pool->at(ipix) % this->image_size[0];
            yc.at(0) = this->pixel_pool->at(ipix) / this->image_size[0];
        }
    }
    if(this->debug_level>=1) {
        std::cout << "DEBUG: CrabBlob: blob_class::settle()" << std::flush;
        std::cout << " xc = " << xc.at(0) << std::flush;
        std::cout << ", yc = " << yc.at(0) << std::flush;
        std::cout << ", fc = " << fc.at(0) << std::flush;
        std::cout << ", growth_limit = " << growth_limit << std::flush;
        std::cout << std::endl;
    }
    if(growth_limit==growth_limit) {
        if(fc.at(0)<growth_limit) {
            // oops, local maxima does not meet the absolute detect threshold!
            return -3;
        }
    }
    return 0;
    //return grow();
}

double blob_class::getpix(long xinput, long yinput) {
    // get pixel by input subscript (i,j) from the 1D data array of a 2D image [i,j]
    if(!this->image_data) {return NAN;}
    if(this->pixel_pool->size()<=0) {return NAN;}
    long xindex = this->image_size[0]*yinput + xinput;
    return this->image_data[xindex];
}

double blob_class::getmean(std::vector<double> v) {
    // compute mean
    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    double mean = sum / v.size();
    return mean;
}

double blob_class::getsigma(std::vector<double> v) {
    // compute sigma
    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    double m = sum / v.size();
    double accum = 0.0;
    for(long k=0; k<v.size(); k++) {
        accum += (v.at(k) - m) * (v.at(k) - m);
    }
    double stdev = sqrt(accum / (v.size()-1));
    return stdev;
}

double blob_class::gettheta(long xinput, long yinput) {
    // get the position angle, which is used for determining grow direction
    if(!this->image_data) {return NAN;}
    if(this->xc.size()<=0) {return NAN;}
    if(this->yc.size()<=0) {return NAN;}
    double temp_theta = NAN;
    //if(this->xd.size()>0 && this->yd.size()>0) {
    //    temp_theta = std::atan2(yinput-getmean(this->yd), xinput-getmean(this->xd)); // http://en.cppreference.com/w/cpp/numeric/math/atan2
    //} else {
    //    temp_theta = std::atan2(yinput-yc.at(0), xinput-xc.at(0)); // http://en.cppreference.com/w/cpp/numeric/math/atan2
    //}
    temp_theta = std::atan2(yinput-yc.at(0), xinput-xc.at(0)); // http://en.cppreference.com/w/cpp/numeric/math/atan2
    temp_theta = temp_theta/PI;
    return temp_theta;
}

int blob_class::addpix(long xinput, long yinput, long insert_position = -1) {
    // add pixel to the "this->x" "this->y" "this->f" arrays
    // and mask the added pixel in the "this->pixel_pool" array, so that other blob_class will not touch those masked pixels. The "this->pixel_pool" array is supposed to be shared with other blob_class.
    if(!this->image_data) {return -1;}
    if(this->pixel_pool->size()<=0) {return -1;}
    if(xinput>=0 && xinput<this->image_size[0] && yinput>=0 && yinput<this->image_size[1]) {
        if(insert_position == -1 || insert_position>=f.size()) {
            this->x.push_back(xinput);
            this->y.push_back(yinput);
            this->f.push_back(this->getpix(xinput,yinput));
            this->theta.push_back(this->gettheta(xinput,yinput));
        } else {
            this->x.insert(this->x.begin()+insert_position, xinput);
            this->y.insert(this->y.begin()+insert_position, yinput);
            this->f.insert(this->f.begin()+insert_position, this->getpix(xinput,yinput));
            this->theta.insert(this->theta.begin()+insert_position, this->gettheta(xinput,yinput));
        }
        this->maskpix(xinput,yinput);
    } else {
        return -2;
    }
    return 0;
}

int blob_class::rmpix(long pindex) {
    // remove pixel from the "this->x" "this->y" "this->f" arrays (dump into "this->xd" "this->yd" "this->fd" arrays)
    // which are supposed to be inside the blob perimeter.
    if(!this->image_data) {return -1;}
    if(this->pixel_pool->size()<=0) {return -1;}
    if(pindex>=0 && pindex<this->f.size()) {
        this->xd.push_back(this->x.at(pindex));
        this->yd.push_back(this->y.at(pindex));
        this->fd.push_back(this->f.at(pindex));
        this->x.erase(this->x.begin()+pindex);
        this->y.erase(this->y.begin()+pindex);
        this->f.erase(this->f.begin()+pindex);
        this->theta.erase(this->theta.begin()+pindex);
    } else {
        return -2;
    }
    return 0;
}

int blob_class::maskpix(long xinput, long yinput) {
    // mask the input pixel in the "this->pixel_pool" array, so that other blob_class will not touch those masked pixels. The "this->pixel_pool" array is supposed to be shared with other blob_class.
    if(!this->image_data) {return -1;}
    if(this->pixel_pool->size()<=0) {return -1;}
    long xindex = this->image_size[0]*yinput + xinput;
    for(long ipix=0; ipix<this->pixel_pool->size(); ipix++) {
        if(this->pixel_pool->at(ipix) == xindex) {
            this->pixel_pool->erase(this->pixel_pool->begin()+ipix);
            break;
        }
    }
    return 0;
}

int blob_class::grow() {
    //
    // grow from the local maxima to outer fainter pixels
    //
    if(!this->image_data) {return -1;}
    if(this->pixel_pool->size()<=0) {return -1;}
    if(fc.size()<=0) {return -1;}
    //
    // if no perimeter pixel yet, set the planted position as the initial perimeter pixels
    if(f.size()<=0) {
        addpix(xc.at(0),yc.at(0)); rmpix(0); // do this so that the central pixel goes into dump arrays "this->xd" "this->yd" "this->fd" which are for duplication check
        addpix(xc.at(0)-1,yc.at(0));
        addpix(xc.at(0)-1,yc.at(0)+1);
        addpix(xc.at(0),yc.at(0)+1);
        addpix(xc.at(0)+1,yc.at(0)+1);
        addpix(xc.at(0)+1,yc.at(0));
        addpix(xc.at(0)+1,yc.at(0)-1);
        addpix(xc.at(0),yc.at(0)-1);
        addpix(xc.at(0)-1,yc.at(0)-1);
    }
    //
    // grow starting from xc,yc, eat as far as possible
    long i = 0;
    long count = 0;
    // growth_ratio = 0.03; // go as far as 45% peak flux
    // growth_level = 2.0; // deblend different sources by checking the raising of flux when growing.
    // growth_limit = 0.004; // go as far as absolute pixel flux limit
    while (i<f.size()) {
        theta.at(i) = gettheta(x.at(i), y.at(i));
        //std::cout << "DEBUG: CrabBlob: blob_class::grow() theta = " << theta.at(i) << ", i = " << i << std::endl;
        // we grow twice, along both X and Y axis
        long grow1 = -1; // along -X
        long grow2 = +1; // along +Y
        if(theta.at(i)>=-0.5 && theta.at(i)<0.5) {grow1=+1;}
        if(theta.at(i)>=-1.0 && theta.at(i)<0.0) {grow2=-1;}
        long growx = x.at(i)+grow1;
        long growy = y.at(i)+grow2;
        long basex = x.at(i);
        long basey = y.at(i);
        double growx_pixval = getpix(growx,basey); double growx_theta = gettheta(growx,basey);
        double growy_pixval = getpix(basex,growy); double growy_theta = gettheta(basex,growy);
        bool growx_check = false;
        bool growy_check = false;
        long grown = 0;
        long dupli = 0;
        //
        // print debug info
        if(this->debug_level>=1) {
            std::cout << "DEBUG: CrabBlob: blob_class::grow() basex = " << basex+1 << ", basey = " << basey+1 << ", growX = " << grow1 << " (" << growx_pixval << ")" << ", growY = " << grow2 << " (" << growy_pixval << "), theta = " << theta.at(i) << std::endl;
        }
        //
        // grow along X
        if(growx>=0 && growx<image_size[0]) {
            if(this->growth_limit == this->growth_limit) {
                // check by growth_limit if non NAN
                growx_check = (growx_pixval >= growth_limit &&
                               growx_pixval <= growth_limit * growth_level + f.at(i) );
            } else {
                // otherwise check by growth_ratio
                growx_check = (growx_pixval >= growth_ratio * fc.at(0) &&
                               growx_pixval <= growth_ratio * fc.at(0) * growth_level + f.at(i) );
            }
            if(growx_pixval == growx_pixval &&
               growx_check == 1)
            {
                int duplicated = 0;
                for(long j=0; j<f.size(); j++) {
                    if(x[j]==growx && y[j]==basey) {duplicated = 1;}
                }
                for(long j=0; j<fd.size(); j++) {
                    if(xd[j]==growx && yd[j]==basey) {duplicated = 1;}
                }
                if(0==duplicated) {
                    long insert_position = -1;
                    for(long j=0; j<theta.size(); j++) {
                        if(theta.at(j)<growx_theta) {insert_position = j; break;}
                    }
                    if(grow1>0) {
                        addpix(growx,basey,i+1);
                        //addpix(growx,basey,insert_position);
                        if(this->debug_level>=1) {
                            std::cout << "DEBUG: CrabBlob: blob_class::grow() basex = " << basex+1 << ", basey = " << basey+1 << ", grow = " << "+X" << std::endl; // +1 accounts for ds9 coordinate system starting from 1 instead of 0.
                        }
                    } else {
                        addpix(growx,basey,i+1);
                        //addpix(growx,basey,insert_position);
                        if(this->debug_level>=1) {
                            std::cout << "DEBUG: CrabBlob: blob_class::grow() basex = " << basex+1 << ", basey = " << basey+1 << ", grow = " << "-X" << std::endl; // +1 accounts for ds9 coordinate system starting from 1 instead of 0.
                        }
                    }
                    //x.insert(x.begin()+i+1, growx);
                    //y.insert(y.begin()+i+1, basey);
                    //f.insert(f.begin()+i+1, getpix(growx,basey));
                    grown++;
                } else {
                    dupli++;
                }
            }
        }
        //
        // grow along Y
        if(growy>=0 && growy<image_size[1]) {
            if(this->growth_limit == this->growth_limit) {
                // check by growth_limit if non NAN
                growy_check = (growy_pixval >= growth_limit &&
                               growy_pixval <= growth_limit * growth_level + f.at(i) );
            } else {
                // otherwise check by growth_ratio
                growy_check = (growy_pixval >= growth_ratio * fc.at(0) &&
                               growy_pixval <= growth_ratio * fc.at(0) * growth_level + f.at(i) );
            }
            if(growy_pixval == growy_pixval &&
               growy_check)
            {
                int duplicated = 0;
                for(long j=0; j<f.size(); j++) {
                    if(x[j]==basex && y[j]==growy) {duplicated = 1;}
                }
                for(long j=0; j<fd.size(); j++) {
                    if(xd[j]==basex && yd[j]==growy) {duplicated = 1;}
                }
                if(0==duplicated) {
                    long insert_position = -1;
                    for(long j=0; j<theta.size(); j++) {
                        if(theta.at(j)<growy_theta) {insert_position = j; break;}
                    }
                    if(grow2>0) {
                        addpix(basex,growy,i+1);
                        //addpix(basex,growy,insert_position);
                        if(this->debug_level>=1) {
                            std::cout << "DEBUG: CrabBlob: blob_class::grow() basex = " << basex+1 << ", basey = " << basey+1 << ", grow = " << "+Y" << std::endl; // +1 accounts for ds9 coordinate system starting from 1 instead of 0.
                        }
                    } else {
                        addpix(basex,growy,i+1);
                        //addpix(basex,growy,insert_position);
                        if(this->debug_level>=1) {
                            std::cout << "DEBUG: CrabBlob: blob_class::grow() basex = " << basex+1 << ", basey = " << basey+1 << ", grow = " << "-Y" << std::endl; // +1 accounts for ds9 coordinate system starting from 1 instead of 0.
                        }
                    }
                    //x.insert(x.begin()+i+1, basex);
                    //y.insert(y.begin()+i+1, growy);
                    //f.insert(f.begin()+i+1, getpix(basex,growy));
                    grown++;
                } else {
                    dupli++;
                }
            }
        }
        
        if(grown>=1) {
            rmpix(i); i--;
        }
        
        if(dupli==2) {
            rmpix(i); i--;
        }
        
        if(this->debug_level>=1) {
            std::cout << "DEBUG: CrabBlob: blob_class::grow() x = " << std::flush;
            for(long ipix=0; ipix<f.size(); ipix++) {
                if(ipix>0) {std::cout << ", ";}
                std::cout << std::setw(10) << x.at(ipix)+1 << std::flush; // +1 accounts for ds9 coordinate system starting from 1 instead of 0.
            }
            std::cout << std::endl;
            
            std::cout << "DEBUG: CrabBlob: blob_class::grow() y = " << std::flush;
            for(long ipix=0; ipix<f.size(); ipix++) {
                if(ipix>0) {std::cout << ", ";}
                std::cout << std::setw(10) << y.at(ipix)+1 << std::flush; // +1 accounts for ds9 coordinate system starting from 1 instead of 0.
            }
            std::cout << std::endl;
            
            std::cout << "DEBUG: CrabBlob: blob_class::grow() f = " << std::flush;
            for(long ipix=0; ipix<f.size(); ipix++) {
                if(ipix>0) {std::cout << ", ";}
                std::cout << std::setw(10) << f.at(ipix) << std::flush;
            }
            std::cout << std::endl;
            
            std::cout << "DEBUG: CrabBlob: blob_class::grow() t = " << std::flush;
            for(long ipix=0; ipix<f.size(); ipix++) {
                if(ipix>0) {std::cout << ", ";}
                std::cout << std::setw(10) << theta.at(ipix) << std::flush;
            }
            std::cout << std::endl;
        }
        
        i++;
        count+=grown;
        
        // if(count>=1) {break;} // <TODO><DEBUG>
    }
    return count;
}




















void blob_detect(double *input_image_data, long input_image_size[2], std::vector<blob_class> &blob_list, double input_detect_threshold, double input_deblend_level)
{
    if(!input_image_data) {return;}
    
    // build image pixel pool
    std::vector<long> input_pixel_pool;
    for(long jpix=0; jpix<input_image_size[1]; jpix++) {
        for(long ipix=0; ipix<input_image_size[0]; ipix++) {
            input_pixel_pool.push_back(jpix*input_image_size[0] + ipix);
        }
    }
    
    // Output to "blob_list"
    
    blob_class blob_item_1;
    blob_item_1.init(input_image_data,input_image_size,input_pixel_pool);
    blob_list.push_back(blob_item_1);
    
    blob_class blob_item_2;
    blob_item_2.init(input_image_data,input_image_size,input_pixel_pool);
    blob_list.push_back(blob_item_2);
    
    blob_print_polygon(blob_list);
    
}














void blob_print_polygon(std::vector<blob_class> blob_list, std::string output_file) {
    
    ofstream outfile;
    outfile.open(output_file);
    
    std::cout << "image" << std::endl;
    outfile << "image\n\n";
    for(int iblob=0; iblob<blob_list.size(); iblob++) {
        //
        // get blob_item
        blob_class blob_item = blob_list.at(iblob);
        //
        // sort blob_item
        //std::sort(std::begin(blob_item), std::end(blob_item),[](const auto& i, const auto& j) { return i.theta > j.theta; } );
        //
        // copy vectors
        std::vector<double> out_x = blob_item.x;
        std::vector<double> out_y = blob_item.y;
        std::vector<double> out_f = blob_item.f;
        std::vector<double> out_theta = blob_item.theta;
        //
        // sort vectors by theta -- http://stackoverflow.com/questions/37368787/c-sort-one-vector-based-on-another-one
        std::vector<std::pair<double,double> > out_x_zipped;
        std::vector<std::pair<double,double> > out_y_zipped;
        // Zip the vectors together
        zip(out_x, out_theta, out_x_zipped);
        zip(out_y, out_theta, out_y_zipped);
        // Sort the vector of pairs
        std::sort(out_x_zipped.begin(), out_x_zipped.end(), func_compare_theta());
        std::sort(out_y_zipped.begin(), out_y_zipped.end(), func_compare_theta());
        
        // Write the sorted pairs back to the original vectors
        unzip(out_x_zipped, out_x, out_theta);
        unzip(out_y_zipped, out_y, out_theta);
        
        // Output to ds9.reg
        std::cout << "polygon(" << std::flush;
        //outfile << "polygon(";
        for(int ipix=0; ipix<out_theta.size(); ipix++) {
            if(ipix>=1) {
                std::cout << ", ";
                //outfile << ", ";
            }
            std::cout << out_x.at(ipix)+1 << "," << out_y.at(ipix)+1;
            //outfile << out_x.at(ipix)+1 << "," << out_y.at(ipix)+1;
            // here +1 because the ds9 coordinate system starts from 1, while subscripts start from 0.
        }
        //std::cout << ") ";
        //outfile << ")\n";
        outfile << "\n";
        
        std::stringstream output_color_ss;
        output_color_ss << std::setfill('0') << std::setw(2) << std::hex << (unsigned int)(rand()%255);
        output_color_ss << std::setfill('0') << std::setw(2) << std::hex << (unsigned int)(rand()%255);
        output_color_ss << std::setfill('0') << std::setw(2) << std::hex << (unsigned int)(rand()%255);
        std::string output_color_str = output_color_ss.str();
        
        for(int ipix=0; ipix<blob_item.fd.size(); ipix++) {
            outfile << "point(" << blob_item.xd.at(ipix)+1 << "," << blob_item.yd.at(ipix)+1 << ") # point=box color=#" << output_color_str << "\n";
            // here +1 because the ds9 coordinate system starts from 1, while subscripts start from 0.
        }
        for(int ipix=0; ipix<out_theta.size(); ipix++) {
            outfile << "point(" << out_x.at(ipix)+1 << "," << out_y.at(ipix)+1 << ") # point=cross color=#" << output_color_str << "\n";
            // here +1 because the ds9 coordinate system starts from 1, while subscripts start from 0.
        }
        std::cout << std::endl;
        outfile << "\n";
    }
    std::cout << std::endl;
    outfile.close();
    std::cout << "Successfully output to \"" << output_file << "\"!" << std::endl;
}







