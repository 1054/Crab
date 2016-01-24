/* 
 
 a c++ tool similar to the IRAF imstat tool
 
 input 1: *.fits file --- the galaxy image

 Please compile like this:
 
     clang++ -std=c++11 main.cpp CrabFitsIO.cpp CrabImage.cpp -o CrabPhotImageStatistics_linux_x86_64
 
 Initialized:
     
     2015-11-23
 
 Last update:
 
     2015-11-23 mainly copied from CrabPhotAperPhot
     2015-11-23 TODO: could not read SPIRE photometry fits file extensions! Need to improve CrabFitsIO.cpp
     2015-11-23 checked the results with ds9 region analysis. All except fMEDIAN are consistent. Now need to check fMEDIAN.
     2015-11-23 now fMEDIAN corrected by adding a sort(vector.begin(),vector.end())
 
 
 
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <fstream>
#include <iostream>     // std::cout
#include <vector>       // std::vector
#include <numeric>      // std::accumulate
#include <algorithm>    // std::max -- http://www.cplusplus.com/reference/algorithm/max/
#include <iomanip>
#include "CrabFitsIO.h"
#include "CrabImage.h"
#include "CrabTableReadColumn.cpp"

using namespace std;

int main(int argc, char **argv)
{
    /**
     // Note that argv[0] is the path and name of the program itself.
     **/
    char *cstrInput1 = NULL;
    char *cstrExtNumber = (char *)"0";
    
    int debug = 0; // <TODO><DEBUG>
    
    for(int i=1; i<argc; i++) {
        // read extension parameter
        if(strncmp(argv[i],"-ext",4)==0 && i<argc-1) {
            i++; cstrExtNumber = argv[i]; continue;
        }
        // read input fits file path
        if(cstrInput1==NULL && i<=argc-1) {
            cstrInput1 = argv[i]; continue;
        }
    }
    //
    //
    if(cstrInput1!=NULL)
    {
        int   errStatus = 0;
        int   extNumber = 0;  if(cstrExtNumber!=NULL) {extNumber=atoi(cstrExtNumber);}
        char *cstrHeader = NULL;
        long  posHeader = 0;
        long  lenHeader = 0;
        //
        // check
        // std::cout << "# CrabPhotAperPhot: " << cstrInput1 << " extension=" << extNumber << std::endl;
        //
        // read fits header
        errStatus = readFitsHeader(cstrInput1,extNumber,&cstrHeader,&posHeader,&lenHeader);
        std::cout << "# CrabPhotAperPhot: readFitsHeader " << cstrInput1 << " extension=" << extNumber << " headersize=" << strlen(cstrHeader) << " errStatus=" << errStatus << std::endl;
        //
        // read fits Naxis
        char *cstrNAXIS1 = extKeyword("NAXIS1",cstrHeader);
        char *cstrNAXIS2 = extKeyword("NAXIS2",cstrHeader);
        if(cstrNAXIS1!=NULL && cstrNAXIS2!=NULL) {
            //
            // read fits Naxis
            long longWidth = atol(cstrNAXIS1);
            long longHeight = atol(cstrNAXIS2);
            //
            // before loop print the column header
            std::cout << "# " << std::endl;
            std::cout << "# " << cstrInput1 << std::endl;
            std::cout << "# " << std::endl;
            std::cout << "# " << setw(13) << "NPIX" << setw(15) << "MIN" << setw(15) << "MEAN" << setw(15) << "MEDIAN" << setw(15) << "MAX" << setw(15) << "SUM" << setw(15) << "STDDEV" << setw(15) << "-3STDDEV" << setw(15) << "+6STDDEV" << std::endl;
            std::cout << "# " << std::endl;
            //
            // read fits Image (TODO: for very big image, what can we do?) // mainly copied from CrabPhotAperPhot
            //
            // first set Rect (currently just use the full image)
            long   iRectI0=0, iRectJ0=0, iRectI1=0, iRectJ1=0;
            // double dRectI0=0, dRectJ0=0, dRectI1=0, dRectJ1=0;
            iRectI1 = longWidth-1; iRectJ1 = longHeight-1;
            //
            // then convert to array indices
            long oldLi=0,oldLj=0,oldUi=0,oldUj=0,newLi=0,newLj=0,newUi=0,newUj=0; // just array indices, no need to be used for now.
            CrabImageCalcCropRect(longWidth,longHeight,iRectI0,iRectJ0,iRectI1,iRectJ1,&oldLi,&oldLj,&oldUi,&oldUj,&newLi,&newLj,&newUi,&newUj);
            //
            // then do the image crop as in CrabPhotAperPhot
            double *aperImage = NULL;
            CrabImageCopyCropRect(cstrInput1,&aperImage,iRectI0,iRectJ0,iRectI1,iRectJ1,oldLi,oldLj,oldUi,oldUj,newLi,newLj,newUi,newUj,extNumber);
            //
            // then remove nan
            std::vector<double> denanArray; // std::cout << denanArray.size() << std::endl; // checked that the intialized array size is 0.
            for(int j=iRectJ0; j<iRectJ1; j++) {
                for(int i=iRectI0; i<iRectI1; i++) {
                    double dblPixValue = aperImage[j*(iRectI1-iRectI0+1)+i];
                    if(!std::isnan(dblPixValue)) {
                        denanArray.push_back(dblPixValue);
                    }
                }
            }
            //
            // prepare measurable variables
            long NPIX=0;
            double fMIN=0.0, fMEAN=0.0, fMEDIAN=0.0, fMAX=0.0, fSUM=0.0, fSQSUM=0.0, fSTDDEV=0.0, fN3STDDEV=0.0, fP6STDDEV=0.0;
            //
            // do the statistical calculation
            NPIX = denanArray.size();
            sort(denanArray.begin(),denanArray.end());                                                  // http://stackoverflow.com/questions/2114797/compute-median-of-values-stored-in-vector-c
            fMIN = *std::min_element(denanArray.begin(),denanArray.end());                              // http://stackoverflow.com/questions/10158756/using-stdmax-element-on-a-vectordouble
            fMEAN = std::accumulate(denanArray.begin(),denanArray.end(),0.0) / denanArray.size();       // http://stackoverflow.com/questions/7616511/calculate-mean-and-standard-deviation-from-a-vector-of-samples-in-c-using-boos
            fMEDIAN = (NPIX%2==0)?(denanArray[NPIX/2-1]+denanArray[NPIX/2])/2.0:denanArray[(NPIX-1)/2]; // http://stackoverflow.com/questions/2114797/compute-median-of-values-stored-in-vector-c
            fMAX = *std::max_element(denanArray.begin(),denanArray.end());                              // http://stackoverflow.com/questions/10158756/using-stdmax-element-on-a-vectordouble
            fSUM = std::accumulate(denanArray.begin(),denanArray.end(),0.0);                            // http://stackoverflow.com/questions/7616511/calculate-mean-and-standard-deviation-from-a-vector-of-samples-in-c-using-boos
            fSQSUM = std::inner_product(denanArray.begin(),denanArray.end(),denanArray.begin(),0.0);    // http://stackoverflow.com/questions/7616511/calculate-mean-and-standard-deviation-from-a-vector-of-samples-in-c-using-boos
            fSTDDEV = std::sqrt(fSQSUM / denanArray.size() - fMEAN * fMEAN);                            // http://stackoverflow.com/questions/7616511/calculate-mean-and-standard-deviation-from-a-vector-of-samples-in-c-using-boos
            fN3STDDEV = fMEAN - 3.0*fSTDDEV;
            fP6STDDEV = fMEAN + 6.0*fSTDDEV;
            //
            // check
            // std::cout << setw(15) << denanArray[0] << setw(15) << denanArray[0] << setw(15) << denanArray[0] << std::endl;
            //
            // output: MIN MEAN MEDIAN MAX SUM STDDEV -3STDDEV +6STDDEV
            std::cout << setw(15) << NPIX << setw(15) << fMIN << setw(15) << fMEAN << setw(15) << fMEDIAN << setw(15) << fMAX << setw(15) << fSUM << setw(15) << fSTDDEV << setw(15) << fN3STDDEV << setw(15) << fP6STDDEV << std::endl;
            std::cout << std::endl;
        }
        std::cout << std::endl;
        // done
    } else {
        // print usage
        std::cout << "Usage: \n    CrabPhotImageStatistics NGC1068_PACS160.fits -ext 1"
                  << std::endl;
    }
    return 0;
}
