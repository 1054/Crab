/*
 
 ; 
 ; Do arithmetic operation on InputImage. 
 ; 
 ; Similar to IRAF imarith, but should be more compatible with systems. (e.g. IRAF could not be launched when the working directory is in external hard drive.
 ;
 ; 4 arguments: InputImage, operator, number value, and output file path.
 ;
 ; In this code, we used C++ Mathematical Expression Library (http://partow.net/programming/exprtk/index.html)
 ;
 
 Please compile like this:
 
     clang++ main.cpp ../CrabFitsIO.cpp -o CrabFitsImageArithmetic_linux_x86_64
     clang++ -I/usr/include/malloc/ main.cpp ../CrabFitsIO.cpp -o CrabFitsImageArithmetic_mac; cp -i CrabFitsImageArithmetic_mac ~/Cloud/Github/DeepFields.SuperDeblending/Softwares/ds9_mac/
 
 Last update:
     
     2017-02-26   copied from CrabFitsImageCut/CrabFitsImageArithmetic
     2017-05-12   -replace-nan
     2017-07-20   also remove NAXIS1 and NAXIS2 in the mainHeader (but not recompiled yet)
     2017-07-30   -copy-wcs
     2017-09-20   found bug when run 'CrabFitsImageArithmetic fit_3_bug_20170920a.fits -ext 2 times 1.0 fit_3_bug_20170920a.model.fits -copy-wcs -debug -debug'. Should be the bug in 'addKeyword()'.
     2018-02-25   added 'sstrOperator == "=="' and 'sstrOperator == "!="'
     2019-06-14   added 'sstrOperator' ">", ">=", "<" and "<="
     2019-06-17   added '-rect'
     2019-06-17   now using 'enumOperator' to speed up
 
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <chrono>  // for high_resolution_clock
#include "../CrabFitsIO.h"
#include "exprtk.hpp"

using namespace std;

// extern char *readFitsHeader(const char *FilePath);

int main(int argc, char **argv)
{
    /**
    // You’ll notice that argv[0] is the path and name of the program itself.
    std::cout << "CrabTableReadInfo: Input " << argc-1 << " arguments." << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::cout << argv[i] << std::endl;
    }
    **/
    char *cstrFilePath = NULL; char *cstrExtNumber = NULL;
    char *cstrFilePathRefImage = NULL; char *cstrExtNumberRefImage = NULL;
    int intRectX1 = -1, intRectY1 = -1, intRectX2 = -1, intRectY2 = -1;
    int intRectX1RefImage = -1, intRectY1RefImage = -1, intRectX2RefImage = -1, intRectY2RefImage = -1;
    char *cstrOperator = NULL;
    enum enumOperatorType {adds, subtracts, multiplies, divides, power_of, equals, does_not_equal, greater_than, greater_equal, less_than, less_equal} enumOperator;
    //static const char *cstrOperatorType[] = { "+", "-", "*", "/", "^", "==", "!=", ">", ">=", "<", "<=" }; // for printing
    static const char *cstrOperatorType[] = { "adds", "subtracts", "multiplies", "divided by", "power of", "equals", "does not equal", "greater than", "greater equal", "less than", "less equal" }; // for printing
    char *cstrNumValue = NULL; double dblNumValue = 0.0;
    char *cstrNewFilePath = NULL;
    int   iRemoveNaN = 0; // in default we keep NaN values
    int   iCopyWcs = 0; // in default we do not search Wcs keywords, if the user gives '-copy-wcs' keyword, then we search the whole fits header and look for Wcs keywords and copy into the output fits file.
    char *cstrReplaceNaN = (char *)"0.0"; // the value to replace for NaN pixels
    double dblReplaceNaN = 0.0;
    
    int debug = 0; // <TODO><DEBUG>
    
    for(int i=1; i<argc; i++) {
        if(strncmp(argv[i],"-ext",4)==0 && i<argc-1) {
            if(cstrExtNumber==NULL) {i++; cstrExtNumber = argv[i]; continue;}
            if(cstrExtNumberRefImage==NULL) {i++; cstrExtNumberRefImage = argv[i]; continue;}
            continue;
        }
        if(strncmp(argv[i],"-rect",4)==0 && i<argc-4) {
            if(intRectX1==-1) {intRectX1 = atoi(argv[++i]); intRectY1 = atoi(argv[++i]); intRectX2 = atoi(argv[++i]); intRectY2 = atoi(argv[++i]); continue;}
            if(intRectX1RefImage==-1) {intRectX1RefImage = atoi(argv[++i]); intRectY1RefImage = atoi(argv[++i]); intRectX2RefImage = atoi(argv[++i]); intRectY2RefImage = atoi(argv[++i]); continue;}
            continue;
        }
        if(strcasecmp(argv[i],"-remove-nan")==0 ) {
            iRemoveNaN = 1; continue;
        }
        if(strcasecmp(argv[i],"-replace-nan")==0 ) {
            iRemoveNaN = 1; if(i<argc-1) { i++; cstrReplaceNaN = argv[i]; dblReplaceNaN = atof(cstrReplaceNaN); } continue;
        }
        if(strcasecmp(argv[i],"-debug")==0 ) {
            debug++; continue;
        }
        if(strcasecmp(argv[i],"-copy-wcs")==0 || strcasecmp(argv[i],"-copywcs")==0 ) {
            iCopyWcs = 1; continue;
        }
        if(cstrFilePath==NULL && i<=argc-1) { cstrFilePath = argv[i]; continue; }
        if(cstrOperator==NULL && i<=argc-1) { cstrOperator = argv[i]; continue; }
        if(cstrNumValue==NULL && i<=argc-1) { cstrNumValue = argv[i]; continue; }
        if(cstrNewFilePath==NULL && i<=argc-1) { cstrNewFilePath = argv[i]; continue; }
    }
    //
    // debug elapsed time, record starting time
    auto timeit_start = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> timeit_elapsed = std::chrono::high_resolution_clock::now() - timeit_start;
    if(debug>=2) {
        std::cout << "Elapsed time: " << timeit_elapsed.count() << " s" << std::endl;
    }
    //
    //
    if(cstrFilePath!=NULL && cstrOperator!=NULL && cstrNumValue!=NULL && cstrNewFilePath!=NULL)
    { 
        int   errStatus = 0;
        int   extNumber = 0;  if(cstrExtNumber!=NULL) {extNumber=atoi(cstrExtNumber);}
        int   extNumberRefImage = 0;  if(cstrExtNumberRefImage!=NULL) {extNumberRefImage=atoi(cstrExtNumberRefImage);}
        char *cstrHeader = NULL;
        char *cstrHeaderRefImage = NULL;
        long  posHeader = 0;
        long  lenHeader = 0;
        long  posHeaderRefImage = 0;
        long  lenHeaderRefImage = 0;
        //
        // Wcs
        int extNumerWcsHeader = 0;
        long posWcsHeader = 0;
        long lenWcsHeader = 0;
        char *cstrWcsHeader = NULL;
        char *cstrWcsCTYPE1 = NULL;
        char *cstrWcsCTYPE2 = NULL;
        char *cstrWcsCRPIX1 = NULL;
        char *cstrWcsCRPIX2 = NULL;
        char *cstrWcsCRVAL1 = NULL;
        char *cstrWcsCRVAL2 = NULL;
        char *cstrWcsCDELT1 = NULL;
        char *cstrWcsCDELT2 = NULL;
        char *cstrWcsCROTA1 = NULL;
        char *cstrWcsCROTA2 = NULL;
        char *cstrWcsCUNIT1 = NULL;
        char *cstrWcsCUNIT2 = NULL;
        char *cstrWcsCD1_1 = NULL;
        char *cstrWcsCD1_2 = NULL;
        char *cstrWcsCD2_1 = NULL;
        char *cstrWcsCD2_2 = NULL;
        //
        // check
        std::cout << "CrabFitsImageArithmetic: " << cstrFilePath << " extension=" << extNumber << std::endl;
        //
        // read Wcs fits header <20170730>
        if(iCopyWcs>0) {
            extNumerWcsHeader = 0;
            while(extNumerWcsHeader==0 || cstrWcsHeader!=NULL) {
                std::cout << "CrabFitsImageArithmetic: copy wcs from fits extension " << extNumerWcsHeader << std::endl;
                errStatus = readFitsHeader(cstrFilePath,extNumerWcsHeader,&cstrWcsHeader,&posWcsHeader,&lenWcsHeader);
                if(cstrWcsHeader!=NULL) {
                    if(cstrWcsCTYPE1==NULL) {cstrWcsCTYPE1 = extKeyword("CTYPE1",cstrWcsHeader,1); if(cstrWcsCTYPE1!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CTYPE1 = " << cstrWcsCTYPE1 << std::endl;} }
                    if(cstrWcsCTYPE2==NULL) {cstrWcsCTYPE2 = extKeyword("CTYPE2",cstrWcsHeader,1); if(cstrWcsCTYPE2!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CTYPE2 = " << cstrWcsCTYPE2 << std::endl;} }
                    if(cstrWcsCRPIX1==NULL) {cstrWcsCRPIX1 = extKeyword("CRPIX1",cstrWcsHeader,1); if(cstrWcsCRPIX1!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CRPIX1 = " << cstrWcsCRPIX1 << std::endl;} }
                    if(cstrWcsCRPIX2==NULL) {cstrWcsCRPIX2 = extKeyword("CRPIX2",cstrWcsHeader,1); if(cstrWcsCRPIX2!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CRPIX2 = " << cstrWcsCRPIX2 << std::endl;} }
                    if(cstrWcsCRVAL1==NULL) {cstrWcsCRVAL1 = extKeyword("CRVAL1",cstrWcsHeader,1); if(cstrWcsCRVAL1!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CRVAL1 = " << cstrWcsCRVAL1 << std::endl;} }
                    if(cstrWcsCRVAL2==NULL) {cstrWcsCRVAL2 = extKeyword("CRVAL2",cstrWcsHeader,1); if(cstrWcsCRVAL2!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CRVAL2 = " << cstrWcsCRVAL2 << std::endl;} }
                    if(cstrWcsCDELT1==NULL) {cstrWcsCDELT1 = extKeyword("CDELT1",cstrWcsHeader,1); if(cstrWcsCDELT1!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CDELT1 = " << cstrWcsCDELT1 << std::endl;} }
                    if(cstrWcsCDELT2==NULL) {cstrWcsCDELT2 = extKeyword("CDELT2",cstrWcsHeader,1); if(cstrWcsCDELT2!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CDELT2 = " << cstrWcsCDELT2 << std::endl;} }
                    if(cstrWcsCROTA1==NULL) {cstrWcsCROTA1 = extKeyword("CROTA1",cstrWcsHeader,1); if(cstrWcsCROTA1!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CROTA1 = " << cstrWcsCROTA1 << std::endl;} }
                    if(cstrWcsCROTA2==NULL) {cstrWcsCROTA2 = extKeyword("CROTA2",cstrWcsHeader,1); if(cstrWcsCROTA2!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CROTA2 = " << cstrWcsCROTA2 << std::endl;} }
                    if(cstrWcsCUNIT1==NULL) {cstrWcsCUNIT1 = extKeyword("CUNIT1",cstrWcsHeader,1); if(cstrWcsCUNIT1!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CUNIT1 = " << cstrWcsCUNIT1 << std::endl;} }
                    if(cstrWcsCUNIT2==NULL) {cstrWcsCUNIT2 = extKeyword("CUNIT2",cstrWcsHeader,1); if(cstrWcsCUNIT2!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CUNIT2 = " << cstrWcsCUNIT2 << std::endl;} }
                    if(cstrWcsCD1_1==NULL) {cstrWcsCD1_1 = extKeyword("CD1_1",cstrWcsHeader,1); if(cstrWcsCD1_1!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CD1_1 = " << cstrWcsCD1_1 << std::endl;} }
                    if(cstrWcsCD1_2==NULL) {cstrWcsCD1_2 = extKeyword("CD1_2",cstrWcsHeader,1); if(cstrWcsCD1_2!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CD1_2 = " << cstrWcsCD1_2 << std::endl;} }
                    if(cstrWcsCD2_1==NULL) {cstrWcsCD2_1 = extKeyword("CD2_1",cstrWcsHeader,1); if(cstrWcsCD2_1!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CD2_1 = " << cstrWcsCD2_1 << std::endl;} }
                    if(cstrWcsCD2_2==NULL) {cstrWcsCD2_2 = extKeyword("CD2_2",cstrWcsHeader,1); if(cstrWcsCD2_2!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CD2_2 = " << cstrWcsCD2_2 << std::endl;} }
                    //
                    // <20170920> if got all needed keywords then break. -- no, do not adopt this.
                    /* if(cstrWcsCTYPE1!=NULL && cstrWcsCTYPE2!=NULL &&
                       cstrWcsCRPIX1!=NULL && cstrWcsCRPIX2!=NULL &&
                       cstrWcsCRVAL1!=NULL && cstrWcsCRVAL2!=NULL) {
                        if(cstrWcsCDELT1!=NULL && cstrWcsCDELT2!=NULL) {
                            break;
                        } else if(cstrWcsCD1_1!=NULL && cstrWcsCD1_2!=NULL &&
                                  cstrWcsCD2_1!=NULL && cstrWcsCD2_2!=NULL) {
                            break;
                        }
                    }
                    */
                }
                extNumerWcsHeader++;
            }
        }
        //
        // debug elapsed time
        if(debug>=2) {
            timeit_elapsed = std::chrono::high_resolution_clock::now() - timeit_start;
            std::cout << "DEBUG: elapsed time: " << timeit_elapsed.count() << " s" << std::endl;
        }
        //
        // read fits header
        errStatus = readFitsHeader(cstrFilePath,extNumber,&cstrHeader,&posHeader,&lenHeader);
        std::cout << "CrabFitsImageArithmetic: readFitsHeader " << cstrFilePath << " extension=" << extNumber << " headersize=" << strlen(cstrHeader) << std::endl;
        //
        // debug elapsed time
        if(debug>=2) {
            timeit_elapsed = std::chrono::high_resolution_clock::now() - timeit_start;
            std::cout << "DEBUG: elapsed time: " << timeit_elapsed.count() << " s" << std::endl;
        }
        //
        // read fits Naxis
        char *cstrNAXIS1 = extKeyword("NAXIS1",cstrHeader);
        char *cstrNAXIS2 = extKeyword("NAXIS2",cstrHeader);
        if(cstrNAXIS1!=NULL && cstrNAXIS2!=NULL) {
            //
            // read fits Naxis
            long oldImWidth = atol(cstrNAXIS1);
            long oldImHeight = atol(cstrNAXIS2);
            //
            // read fits image <Updated><20141117><DzLIU> Now we do not read the full image !
            double *oldImage = readFitsImage(cstrFilePath,extNumber);
            std::cout << "CrabFitsImageArithmetic: readFitsImage " << cstrFilePath << " extension=" << extNumber << " imagesize=" << oldImWidth << "x" << oldImHeight << " bitpix=" << -long(sizeof(double))*8 << std::endl;
            //
            // get default computing rectangle if not input by the user (since 20190617)
            if(intRectX1 == -1) {
                intRectX1 = 0; intRectY1 = 0;
                intRectX2 = oldImWidth - 1; intRectY2 = oldImHeight - 1;
            }
            //
            // debug elapsed time
            if(debug>=2) {
                timeit_elapsed = std::chrono::high_resolution_clock::now() - timeit_start;
                std::cout << "DEBUG: elapsed time: " << timeit_elapsed.count() << " s" << std::endl;
            }
            //
            // prepare new image (new image dimension is always the same as the first input image)
            long newImWidth = oldImWidth;
            long newImHeight = oldImHeight;
            long newImPixelCount = newImWidth * newImHeight;
            //
            // check operator
            std::string sstrOperator = std::string(cstrOperator);
            int maskOperator = 0; // apply the operation on Image (0) or on ImageMask (1).
            if(sstrOperator.find("*")!=std::string::npos ||
               sstrOperator.find("time")!=std::string::npos ||
               sstrOperator.find("multipl")!=std::string::npos ) {
                enumOperator = enumOperatorType::multiplies;
            } else if(sstrOperator.find("+")!=std::string::npos ||
                      sstrOperator.find("add")!=std::string::npos ||
                      sstrOperator.find("plus")!=std::string::npos ) {
                enumOperator = enumOperatorType::adds;
            } else if(sstrOperator.find("-")!=std::string::npos ||
                      sstrOperator.find("subtract")!=std::string::npos ||
                      sstrOperator.find("minus")!=std::string::npos ) {
                enumOperator = enumOperatorType::subtracts;
            } else if(sstrOperator.find("/")!=std::string::npos ||
                      sstrOperator.find("divide")!=std::string::npos ) {
                enumOperator = enumOperatorType::divides;
            } else if(sstrOperator.find("^")!=std::string::npos ||
                      sstrOperator.find("power")!=std::string::npos ) {
                enumOperator = enumOperatorType::power_of;
            } else if(sstrOperator == "==" ||
                      sstrOperator == "eq" ||
                      sstrOperator == "equals" ) {
                enumOperator = enumOperatorType::equals;
                maskOperator = 1;
            } else if(sstrOperator == "!=" ||
                      sstrOperator == "ne" ||
                      sstrOperator == "does not equal" ) {
                enumOperator = enumOperatorType::does_not_equal;
                maskOperator = 1;
            } else if(sstrOperator == ">" ||
                      sstrOperator == "gt" ||
                      sstrOperator == "greater than" ) {
                enumOperator = enumOperatorType::greater_than;
                maskOperator = 1;
            } else if(sstrOperator == ">=" ||
                      sstrOperator == "ge" ||
                      sstrOperator == "greater equal" ) {
                enumOperator = enumOperatorType::greater_equal;
                maskOperator = 1;
            } else if(sstrOperator == "<" ||
                      sstrOperator == "lt" ||
                      sstrOperator == "less than" ) {
                enumOperator = enumOperatorType::less_than;
                maskOperator = 1;
            } else if(sstrOperator == "<=" ||
                      sstrOperator == "le" ||
                      sstrOperator == "less equal" ) {
                enumOperator = enumOperatorType::less_equal;
                maskOperator = 1;
            }
            //
            // create new image array and image mask array
            double *newImage;
            int *newImageMask;
            //
            // initialize new image array and/or image mask array
            if(maskOperator==0) {
                //
                // debug info
                if(debug>=2) {
                    std::cout << "DEBUG: allocating memory" << std::endl;
                }
                //
                // allocate memory
                newImage = (double *)malloc(newImPixelCount * sizeof(double));
                //
                // debug elapsed time
                if(debug>=2) {
                    timeit_elapsed = std::chrono::high_resolution_clock::now() - timeit_start;
                    std::cout << "DEBUG: elapsed time: " << timeit_elapsed.count() << " s" << std::endl;
                }
                //
                // //if the user does not need to remove NaN, we initialize the output image with NaN, otherwise with dblReplaceNaN.
                // we initialize the output image with the input image
                // and if the user does not need to remove NaN, we keep original values, otherwise replace NaN values with dblReplaceNaN.
                if(0==iRemoveNaN) {
                    // for(int i=0; i<newImPixelCount; i++) { newImage[i] = NAN; } // need cmath.h
                    for(int i=0; i<newImPixelCount; i++) { newImage[i] = oldImage[i]; }
                } else {
                    if(debug>=1) {
                        std::cout << "DEBUG: removing all NaN values by filling " << dblReplaceNaN << std::endl;
                    }
                    for(int i=0; i<newImPixelCount; i++) { if (oldImage[i] == oldImage[i]) { newImage[i] = oldImage[i]; } else { newImage[i] = dblReplaceNaN; } } // NAN need cmath.h. By checking oldImage[i] == oldImage[i] we tell if the value is NAN or not as (NAN == NAN) is False.
                }
            } else {
                //
                // allocate memory
                newImageMask = (int *)malloc(newImPixelCount * sizeof(int));
                for(int i=0; i<newImPixelCount; i++) { newImageMask[i] = 0; }
            }
            //
            // check NumValue (evaluate)
            std::string strRefImage = cstrNumValue;
            if(strRefImage.find(".fits")==std::string::npos) {
                exprtk::symbol_table<double> symbol_table; symbol_table.add_constants();
                exprtk::expression<double> expression; expression.register_symbol_table(symbol_table);
                exprtk::parser<double> parser; parser.compile(cstrNumValue,expression);
                dblNumValue = expression.value();
                //
                // print info -- FitsImage operating with NumValue
                std::cout << "CrabFitsImageArithmetic: computing " << cstrFilePath << " extension=" << extNumber << " rectangle=[" << "[" << intRectX1 << "," << intRectY1 << "]" << "," << "[" << intRectX2 << "," << intRectY2 << "]" << "]" << " " << cstrOperatorType[enumOperator] << " " << dblNumValue << " " << std::endl;
                //
                // debug elapsed time
                if(debug>=2) {
                    timeit_elapsed = std::chrono::high_resolution_clock::now() - timeit_start;
                    std::cout << "DEBUG: elapsed time: " << timeit_elapsed.count() << " s" << std::endl;
                }
                //
                // copy image pixel by pixel
                for(int jj=intRectY1; jj<=intRectY2; jj++) {
                    for(int ii=intRectX1; ii<=intRectX2; ii++) {
                        long kk = long(ii)+long(jj)*newImWidth;
                        if(kk < 0 || kk >= newImPixelCount) {continue;} // check overflow
                        //
                        // check if the operation is on Image or ImageMask
                        if(maskOperator==0) {
                            //
                            // check if oldImage pixel value is NaN
                            if(NAN != oldImage[kk] && oldImage[kk] == oldImage[kk]) {
                                //
                                // check Operator
                                if(enumOperatorType::multiplies == enumOperator) {
                                    newImage[kk] = oldImage[kk] * dblNumValue;
                                } else if(enumOperatorType::adds == enumOperator) {
                                    newImage[kk] = oldImage[kk] + dblNumValue;
                                } else if(enumOperatorType::subtracts == enumOperator) {
                                    newImage[kk] = oldImage[kk] - dblNumValue;
                                } else if(enumOperatorType::divides == enumOperator) {
                                    newImage[kk] = oldImage[kk] / dblNumValue;
                                } else if(enumOperatorType::power_of == enumOperator) {
                                    newImage[kk] = pow(oldImage[kk], dblNumValue);
                                }
                            }
                        } else {
                            //
                            // check Operator
                            if(enumOperatorType::equals == enumOperator) {
                                if(oldImage[kk] == dblNumValue) {
                                    newImageMask[kk] = 1;
                                }
                            } else if(enumOperatorType::does_not_equal == enumOperator) {
                                if(oldImage[kk] != dblNumValue) {
                                    newImageMask[kk] = 1;
                                }
                            } else if(enumOperatorType::greater_than == enumOperator) {
                                if(oldImage[kk] > dblNumValue) {
                                    newImageMask[kk] = 1;
                                }
                            } else if(enumOperatorType::greater_equal == enumOperator) {
                                if(oldImage[kk] >= dblNumValue) {
                                    newImageMask[kk] = 1;
                                }
                            } else if(enumOperatorType::less_than == enumOperator) {
                                if(oldImage[kk] < dblNumValue) {
                                    newImageMask[kk] = 1;
                                }
                            } else if(enumOperatorType::less_equal == enumOperator) {
                                if(oldImage[kk] <= dblNumValue) {
                                    newImageMask[kk] = 1;
                                }
                            }
                        }
                    }
                }
                //
                // debug elapsed time
                if(debug>=2) {
                    timeit_elapsed = std::chrono::high_resolution_clock::now() - timeit_start;
                    std::cout << "DEBUG: elapsed time: " << timeit_elapsed.count() << " s" << std::endl;
                }
            } else {
                //
                // if the input NumValue is RefImage
                cstrFilePathRefImage = (char *)strRefImage.c_str();
                //
                // check
                std::cout << "CrabFitsImageArithmetic: " << cstrFilePathRefImage << " extension=" << extNumberRefImage << std::endl;
                //
                // read fits header
                errStatus = readFitsHeader(cstrFilePathRefImage,extNumberRefImage,&cstrHeaderRefImage,&posHeaderRefImage,&lenHeaderRefImage);
                std::cout << "CrabFitsImageArithmetic: readFitsHeader " << cstrFilePathRefImage << " extension=" << extNumberRefImage << " headersize=" << strlen(cstrHeaderRefImage) << std::endl;
                //
                // debug elapsed time
                if(debug>=2) {
                    timeit_elapsed = std::chrono::high_resolution_clock::now() - timeit_start;
                    std::cout << "DEBUG: elapsed time: " << timeit_elapsed.count() << " s" << std::endl;
                }
                //
                // read fits Naxis
                char *cstrNAXIS1RefImage = extKeyword("NAXIS1",cstrHeaderRefImage);
                char *cstrNAXIS2RefImage = extKeyword("NAXIS2",cstrHeaderRefImage);
                if(cstrNAXIS1RefImage!=NULL && cstrNAXIS2RefImage!=NULL) {
                    //
                    // read fits Naxis
                    long refImWidth = atol(cstrNAXIS1RefImage);
                    long refImHeight = atol(cstrNAXIS2RefImage);
                    long refImPixelCount = refImWidth * refImHeight;
                    //
                    // read ref image
                    double *refImage = readFitsImage(cstrFilePathRefImage,extNumberRefImage);
                    std::cout << "CrabFitsImageArithmetic: readFitsImage " << cstrFilePathRefImage << " extension=" << extNumberRefImage << " imagesize=" << refImWidth << "x" << refImHeight << " bitpix=" << -long(sizeof(double))*8 << std::endl;
                    //
                    // get default computing rectangle for the ref image (since 20190617)
                    if(intRectX1RefImage == -1) {
                        intRectX1RefImage = 0; intRectY1RefImage = 0;
                        intRectX2RefImage = refImWidth - 1; intRectY2RefImage = refImHeight - 1;
                    }
                    //
                    // print info -- FitsImage operating with another RefImage
                    std::cout << "CrabFitsImageArithmetic: computing " << cstrFilePath << " extension=" << extNumber << " rectangle=[" << "[" << intRectX1 << "," << intRectY1 << "]" << "," << "[" << intRectX2 << "," << intRectY2 << "]" << "]" << " " << cstrOperatorType[enumOperator] << " " << cstrFilePathRefImage << " extension=" << extNumberRefImage << " rectangle=[" << "[" << intRectX1RefImage << "," << intRectY1RefImage << "]" << "," << "[" << intRectX2RefImage << "," << intRectY2RefImage << "]" << "]" << std::endl;
                    //
                    // debug elapsed time
                    if(debug>=2) {
                        timeit_elapsed = std::chrono::high_resolution_clock::now() - timeit_start;
                        std::cout << "DEBUG: elapsed time: " << timeit_elapsed.count() << " s" << std::endl;
                    }
                    //
                    // check ref image size
                    if((intRectX2RefImage-intRectX1RefImage)!=(intRectX2-intRectX1) || (intRectY2RefImage-intRectY1RefImage)!=(intRectY2-intRectY1)) {
                        std::cout << "Error! The two input fits images have different dimensions or computing rectangle sizes!" << std::endl;
                        return -1;
                    }
                    //
                    // operate on image pixel by pixel
                    for(int jj=intRectY1, jjRefImage=intRectY1RefImage; (jj<=intRectY2) && (jjRefImage<=intRectY2RefImage); jj++, jjRefImage++) {
                        for(int ii=intRectX1, iiRefImage=intRectX1RefImage; (ii<=intRectX2) && (iiRefImage<=intRectX2RefImage); ii++, iiRefImage++) {
                            long kk = long(ii)+long(jj)*newImWidth;
                            long kkRefImage=long(iiRefImage)+long(jjRefImage)*refImWidth;
                            if(kk < 0 || kk >= newImPixelCount) {continue;} // check overflow
                            if(kkRefImage < 0 || kkRefImage >= refImPixelCount) {continue;} // check overflow
                            //
                            // check if the operation is on Image or ImageMask
                            if(maskOperator==0) {
                                //
                                // check if oldImage pixel value is NaN
                                if(NAN != oldImage[kk] && oldImage[kk] == oldImage[kk]) {
                                    //
                                    // check Operator
                                    if(enumOperatorType::multiplies == enumOperator) {
                                        newImage[kk] = oldImage[kk] * refImage[kkRefImage];
                                    } else if(enumOperatorType::adds == enumOperator) {
                                        newImage[kk] = oldImage[kk] + refImage[kkRefImage];
                                    } else if(enumOperatorType::subtracts == enumOperator) {
                                        newImage[kk] = oldImage[kk] - refImage[kkRefImage];
                                    } else if(enumOperatorType::divides == enumOperator) {
                                        newImage[kk] = oldImage[kk] / refImage[kkRefImage];
                                    } else if(enumOperatorType::power_of == enumOperator) {
                                        newImage[kk] = pow(oldImage[kk], refImage[kkRefImage]);
                                    }
                                }
                            } else {
                                //
                                // check Operator
                                if(enumOperatorType::equals == enumOperator) {
                                    if(oldImage[kk] == refImage[kkRefImage]) {
                                        newImageMask[kk] = 1;
                                    }
                                } else if(enumOperatorType::does_not_equal == enumOperator) {
                                    if(oldImage[kk] != refImage[kkRefImage]) {
                                        newImageMask[kk] = 1;
                                    }
                                } else if(enumOperatorType::greater_than == enumOperator) {
                                    if(oldImage[kk] > refImage[kkRefImage]) {
                                        newImageMask[kk] = 1;
                                    }
                                } else if(enumOperatorType::greater_equal == enumOperator) {
                                    if(oldImage[kk] >= refImage[kkRefImage]) {
                                        newImageMask[kk] = 1;
                                    }
                                } else if(enumOperatorType::less_than == enumOperator) {
                                    if(oldImage[kk] < refImage[kkRefImage]) {
                                        newImageMask[kk] = 1;
                                    }
                                } else if(enumOperatorType::less_equal == enumOperator) {
                                    if(oldImage[kk] <= refImage[kkRefImage]) {
                                        newImageMask[kk] = 1;
                                    }
                                }
                            }
                        }
                    }
                } else {
                    std::cout << "Error! Failed to read NAXIS1 and NAXIS2 from the second input fits file!" << std::endl;
                    return -1;
                }
                //
                // debug elapsed time
                if(debug>=2) {
                    timeit_elapsed = std::chrono::high_resolution_clock::now() - timeit_start;
                    std::cout << "DEBUG: elapsed time: " << timeit_elapsed.count() << " s" << std::endl;
                }
            }
            if(1) {
                //
                // also modify BITPIX
                if(debug>=1) {
                    std::cout << "DEBUG: old BITPIX = " << extKeyword("BITPIX",cstrHeader) << std::endl;
                }
                char *newBITPIX = new char(4);
                memset(newBITPIX,'\0',4);
                if(maskOperator==0) {
                    sprintf(newBITPIX,"%ld",-long(sizeof(double))*8);
                    errStatus = modKeyword("BITPIX",newBITPIX,cstrHeader);
                } else {
                    sprintf(newBITPIX,"%d",int(sizeof(int))*8);
                    errStatus = modKeyword("BITPIX",newBITPIX,cstrHeader);
                }
                if(debug>=1) {
                    std::cout << "DEBUG: new BITPIX = " << extKeyword("BITPIX",cstrHeader) << std::endl;
                }
                //
                // copy Wcs fits header <20170730>
                if(iCopyWcs>0) {
                    //std::cout << "DEBUG: addKeyword" << std::endl;
                    if(cstrWcsCTYPE1) {addKeyword("CTYPE1",cstrWcsCTYPE1,&cstrHeader,NULL,1,debug-1); if(debug>=2) {std::cout << "DEBUG: addKeyword " << "CTYPE1 " << cstrWcsCTYPE1 << std::endl;} }
                    if(cstrWcsCTYPE2) {addKeyword("CTYPE2",cstrWcsCTYPE2,&cstrHeader,NULL,1,debug-1); if(debug>=2) {std::cout << "DEBUG: addKeyword " << "CTYPE2 " << cstrWcsCTYPE2 << std::endl;} }
                    if(cstrWcsCRPIX1) {addKeyword("CRPIX1",cstrWcsCRPIX1,&cstrHeader,NULL,1,debug-1); if(debug>=2) {std::cout << "DEBUG: addKeyword " << "CRPIX1 " << cstrWcsCRPIX1 << std::endl;} }
                    if(cstrWcsCRPIX2) {addKeyword("CRPIX2",cstrWcsCRPIX2,&cstrHeader,NULL,1,debug-1); if(debug>=2) {std::cout << "DEBUG: addKeyword " << "CRPIX2 " << cstrWcsCRPIX2 << std::endl;} }
                    if(cstrWcsCRVAL1) {addKeyword("CRVAL1",cstrWcsCRVAL1,&cstrHeader,NULL,1,debug-1); if(debug>=2) {std::cout << "DEBUG: addKeyword " << "CRVAL1 " << cstrWcsCRVAL1 << std::endl;} }
                    if(cstrWcsCRVAL2) {addKeyword("CRVAL2",cstrWcsCRVAL2,&cstrHeader,NULL,1,debug-1); if(debug>=2) {std::cout << "DEBUG: addKeyword " << "CRVAL2 " << cstrWcsCRVAL2 << std::endl;} }
                    if(cstrWcsCDELT1) {addKeyword("CDELT1",cstrWcsCDELT1,&cstrHeader,NULL,1,debug-1); if(debug>=2) {std::cout << "DEBUG: addKeyword " << "CDELT1 " << cstrWcsCDELT1 << std::endl;} }
                    if(cstrWcsCDELT2) {addKeyword("CDELT2",cstrWcsCDELT2,&cstrHeader,NULL,1,debug-1); if(debug>=2) {std::cout << "DEBUG: addKeyword " << "CDELT2 " << cstrWcsCDELT2 << std::endl;} }
                    if(cstrWcsCROTA1) {addKeyword("CROTA1",cstrWcsCROTA1,&cstrHeader,NULL,1,debug-1); if(debug>=2) {std::cout << "DEBUG: addKeyword " << "CROTA1 " << cstrWcsCROTA1 << std::endl;} }
                    if(cstrWcsCROTA2) {addKeyword("CROTA2",cstrWcsCROTA2,&cstrHeader,NULL,1,debug-1); if(debug>=2) {std::cout << "DEBUG: addKeyword " << "CROTA2 " << cstrWcsCROTA2 << std::endl;} }
                    if(cstrWcsCUNIT1) {addKeyword("CUNIT1",cstrWcsCUNIT1,&cstrHeader,NULL,1,debug-1); if(debug>=2) {std::cout << "DEBUG: addKeyword " << "CUNIT1 " << cstrWcsCUNIT1 << std::endl;} }
                    if(cstrWcsCUNIT2) {addKeyword("CUNIT2",cstrWcsCUNIT2,&cstrHeader,NULL,1,debug-1); if(debug>=2) {std::cout << "DEBUG: addKeyword " << "CUNIT2 " << cstrWcsCUNIT2 << std::endl;} }
                    if(cstrWcsCD1_1)  {addKeyword("CD1_1", cstrWcsCD1_1, &cstrHeader,NULL,1,debug-1); if(debug>=2) {std::cout << "DEBUG: addKeyword " << "CD1_1 "  << cstrWcsCD1_1  << std::endl;} }
                    if(cstrWcsCD1_2)  {addKeyword("CD1_2", cstrWcsCD1_2, &cstrHeader,NULL,1,debug-1); if(debug>=2) {std::cout << "DEBUG: addKeyword " << "CD1_2 "  << cstrWcsCD1_2  << std::endl;} }
                    if(cstrWcsCD2_1)  {addKeyword("CD2_1", cstrWcsCD2_1, &cstrHeader,NULL,1,debug-1); if(debug>=2) {std::cout << "DEBUG: addKeyword " << "CD2_1 "  << cstrWcsCD2_1  << std::endl;} }
                    if(cstrWcsCD2_2)  {addKeyword("CD2_2", cstrWcsCD2_2, &cstrHeader,NULL,1,debug-1); if(debug>=2) {std::cout << "DEBUG: addKeyword " << "CD2_2 "  << cstrWcsCD2_2  << std::endl;} }
                }
                //
                // and remember to copy the main header when (extNumber>0) <TODO> do we need to copy the main header??
                if(extNumber>0) {                                       // <TODO> do we need to copy the main header??
                    char *mainHeader = readFitsHeader(cstrFilePath,0);
                    char *tempHeader = NULL;
                    // char *newBITPIX = (char *)"-64";
                    // errStatus = modKeyword("BITPIX",newBITPIX,mainHeader);
                    // change "EXTEND" and remove "NAXIS" "END", so as to make sure the output fits is single frame. <Added><20150202><DzLIU>
                    // <DONE>
                    // <DONE> now compatible with galfit: use -remove-nan option! (filling all NaN values by 0.0, but note that 0.0 is not exactly 0 due to the precision of the system.
                    // <DONE>
                    errStatus = modKeyword("EXTEND","F",mainHeader);
                    tempHeader = mainHeader; // pointer to the mainHeader
                    while(*tempHeader!='\0'){
                        if(0==strncmp(tempHeader,"NAXIS   =",9)){
                            // char *tempHead80 = (char *)malloc(80*sizeof(char)); strncpy(tempHead80,tempHeader,80);
                            // std::cout << "DEBUG: " << std::endl;
                            // std::cout << "DEBUG: " << tempHead80 << std::endl;
                            // std::cout << "DEBUG: " << std::endl;
                            // free(tempHead80); tempHead80 = NULL;
                            for(int tempCounter=0; tempCounter<80; tempCounter++){strncpy(tempHeader+tempCounter," ",1);} continue;
                        }
                        if(0==strncmp(tempHeader,"NAXIS1  =",9)){
                            for(int tempCounter=0; tempCounter<80; tempCounter++){strncpy(tempHeader+tempCounter," ",1);} continue;
                        } /* 20170720 also remove NAXIS1 and NAXIS2 in the mainHeader */
                        if(0==strncmp(tempHeader,"NAXIS2  =",9)){
                            for(int tempCounter=0; tempCounter<80; tempCounter++){strncpy(tempHeader+tempCounter," ",1);} continue;
                        } /* 20170720 also remove NAXIS1 and NAXIS2 in the mainHeader */
                        if(0==strncmp(tempHeader,"BITPIX  =",9)){
                            for(int tempCounter=0; tempCounter<80; tempCounter++){strncpy(tempHeader+tempCounter," ",1);} continue;
                        } /* 20170720 also remove BITPIX in the mainHeader */
                        if(0==strncmp(tempHeader,"END      ",9)){
                            for(int tempCounter=0; tempCounter<80; tempCounter++){strncpy(tempHeader+tempCounter," ",1);} continue;
                        } /* 20170720 also remove END mark in the mainHeader */
                        if(0==strncmp(tempHeader,"CTYPE1  =",9)){
                            for(int tempCounter=0; tempCounter<80; tempCounter++){strncpy(tempHeader+tempCounter," ",1);} continue;
                        } /* 20180128 also remove CTYPE1 in the mainHeader */
                        if(0==strncmp(tempHeader,"CTYPE2  =",9)){
                            for(int tempCounter=0; tempCounter<80; tempCounter++){strncpy(tempHeader+tempCounter," ",1);} continue;
                        } /* 20180128 also remove CTYPE2 in the mainHeader */
                        if(0==strncmp(tempHeader,"CRVAL1  =",9)){
                            for(int tempCounter=0; tempCounter<80; tempCounter++){strncpy(tempHeader+tempCounter," ",1);} continue;
                        } /* 20180128 also remove CRVAL1 in the mainHeader */
                        if(0==strncmp(tempHeader,"CRVAL2  =",9)){
                            for(int tempCounter=0; tempCounter<80; tempCounter++){strncpy(tempHeader+tempCounter," ",1);} continue;
                        } /* 20180128 also remove CRVAL2 in the mainHeader */
                        if(0==strncmp(tempHeader,"CRPIX1  =",9)){
                            for(int tempCounter=0; tempCounter<80; tempCounter++){strncpy(tempHeader+tempCounter," ",1);} continue;
                        } /* 20180128 also remove CRPIX1 in the mainHeader */
                        if(0==strncmp(tempHeader,"CRPIX2  =",9)){
                            for(int tempCounter=0; tempCounter<80; tempCounter++){strncpy(tempHeader+tempCounter," ",1);} continue;
                        } /* 20180128 also remove CRPIX2 in the mainHeader */
                        if(0==strncmp(tempHeader,"CDELT1  =",9)){
                            for(int tempCounter=0; tempCounter<80; tempCounter++){strncpy(tempHeader+tempCounter," ",1);} continue;
                        } /* 20180128 also remove CDELT1 in the mainHeader */
                        if(0==strncmp(tempHeader,"CDELT2  =",9)){
                            for(int tempCounter=0; tempCounter<80; tempCounter++){strncpy(tempHeader+tempCounter," ",1);} continue;
                        } /* 20180128 also remove CDELT2 in the mainHeader */
                        if(0==strncmp(tempHeader,"CROTA1  =",9)){
                            for(int tempCounter=0; tempCounter<80; tempCounter++){strncpy(tempHeader+tempCounter," ",1);} continue;
                        } /* 20180128 also remove CROTA1 in the mainHeader */
                        if(0==strncmp(tempHeader,"CROTA2  =",9)){
                            for(int tempCounter=0; tempCounter<80; tempCounter++){strncpy(tempHeader+tempCounter," ",1);} continue;
                        } /* 20180128 also remove CROTA2 in the mainHeader */
                        if(0==strncmp(tempHeader,"CD1_1   =",9)){
                            for(int tempCounter=0; tempCounter<80; tempCounter++){strncpy(tempHeader+tempCounter," ",1);} continue;
                        } /* 20180128 also remove CD1_1 in the mainHeader */
                        if(0==strncmp(tempHeader,"CD2_2   =",9)){
                            for(int tempCounter=0; tempCounter<80; tempCounter++){strncpy(tempHeader+tempCounter," ",1);} continue;
                        } /* 20180128 also remove CD2_2 in the mainHeader */
                        if(0==strncmp(tempHeader,"CD1_2   =",9)){
                            for(int tempCounter=0; tempCounter<80; tempCounter++){strncpy(tempHeader+tempCounter," ",1);} continue;
                        } /* 20180128 also remove CD1_2 in the mainHeader */
                        if(0==strncmp(tempHeader,"CD2_1   =",9)){
                            for(int tempCounter=0; tempCounter<80; tempCounter++){strncpy(tempHeader+tempCounter," ",1);} continue;
                        } /* 20180128 also remove CD2_1 in the mainHeader */
                        tempHeader++;
                    }
                    tempHeader = cstrHeader; // pointer to the cstrHeader
                    while(*tempHeader!='\0'){
                        if(0==strncmp(tempHeader,"XTENSION=",9)){
                            for(int tempCounter=0; tempCounter<80; tempCounter++){strncpy(tempHeader+tempCounter," ",1);} continue;
                        }
                        tempHeader++;
                    }
                    /*                               <TODO> do we need to copy the main header??
                     */
                    char *fullHeader = (char *)malloc((strlen(mainHeader)+strlen(cstrHeader)+1)*sizeof(char));
                    memset(fullHeader,'\0',(strlen(mainHeader)+strlen(cstrHeader)+1));
                    strcpy(fullHeader,mainHeader);
                    strcat(fullHeader,cstrHeader);
                    free(cstrHeader); cstrHeader = fullHeader;
                    /*                               <TODO> or we simply copy the first line SIMPLE = T??
                    char *fullHeader = (char *)malloc((80+strlen(cstrHeader)+1)*sizeof(char));
                    memset(fullHeader,' ',(80+strlen(cstrHeader)+1)); fullHeader[80+strlen(cstrHeader)] = '\0'; // memset(fullHeader,'\0',(80+strlen(cstrHeader)+1));
                    strncpy(fullHeader,mainHeader,80);
                    strcat(fullHeader,cstrHeader); for(int tempCounter=0; tempCounter<80*35; tempCounter++){strncpy(fullHeader+tempCounter," ",1);}
                     free(cstrHeader); cstrHeader = fullHeader;
                     */
                }
                //
                // prepare output file path
                if(cstrNewFilePath==NULL) {
                    std::string strNewFilePath(cstrFilePath);
                    size_t posFileNameSuffix = strNewFilePath.find_last_of(".fits");
                    if(std::string::npos!=posFileNameSuffix) {
                        strNewFilePath = strNewFilePath.substr(0,posFileNameSuffix-4) + "_New.fits";
                    } else {
                        strNewFilePath = strNewFilePath + "_New.fits";
                    }
                    cstrNewFilePath = (char *)malloc(sizeof(char)*(strlen(strNewFilePath.c_str())+1));
                    memset(cstrNewFilePath,'\0',(strlen(strNewFilePath.c_str())+1));
                    strcpy(cstrNewFilePath,strNewFilePath.c_str());
                    if(debug>=1) {
                        std::cout << "CrabFitsImageArithmetic: Input " << cstrFilePath << std::endl;
                        std::cout << "CrabFitsImageArithmetic: Output " << cstrNewFilePath << std::endl;
                    }
                }
                //
                // now save new fits
                // const char *newFilePath = NULL;
                // newFilePath = "/Users/dliu/Programming/QtProgram/CrabIO/CrabFitsIO/CrabFitsImageCut/new.fits";
                // std::cout << "CrabFitsImageArithmetic: Opening " << newFilePath << " to write." << std::endl;
                //
                // debug elapsed time
                if(debug>=2) {
                    timeit_elapsed = std::chrono::high_resolution_clock::now() - timeit_start;
                    std::cout << "DEBUG: elapsed time: " << timeit_elapsed.count() << " s" << std::endl;
                }
                //
                // print progress info
                if(debug>=1) {
                    std::cout << "CrabFitsImageArithmetic: Opening " << cstrNewFilePath << " to write" << std::endl;
                }
                //
                // write image data as double float array or mask as integer array
                if(maskOperator==0) {
                    errStatus = writeFitsD(newImage,cstrHeader,cstrNewFilePath);
                } else {
                    errStatus = writeFitsI(newImageMask,cstrHeader,cstrNewFilePath);
                }
                if(errStatus==0) {
                    std::cout << "CrabFitsImageArithmetic: File has been saved to " << cstrNewFilePath << std::endl;
                } else {
                    std::cout << "CrabFitsImageArithmetic: Oooh! Failed! We are sorry ... (error code " << errStatus << ")" << std::endl;
                }
                //
                // free memory and reset pointer
                if(maskOperator==0) {
                    free(newImage); newImage = NULL;
                } else {
                    free(newImageMask); newImageMask = NULL;
                }
            }
            //
            // debug elapsed time
            if(debug>=2) {
                timeit_elapsed = std::chrono::high_resolution_clock::now() - timeit_start;
                std::cout << "DEBUG: elapsed time: " << timeit_elapsed.count() << " s. All done." << std::endl;
            }
        } else {
            std::cout << "CrabFitsImageArithmetic: Error! Fits header of extension " << extNumber << " does not contain NAXIS1 and NAXIS2!" << std::endl;
        }
    } else {
        std::cout << "Aim: " << std::endl;
        std::cout << "    This code computes the arithmetic operation between an input fits image and a number or" << std::endl;
        std::cout << "    that between two input fits images." << std::endl;
        std::cout << "Usage: " << std::endl;
        std::cout << "    1. Simply adding/subtracting/multiplying/dividing by a number:" << std::endl;
        std::cout << "    CrabFitsImageArithmetic FITS_IMAGE.fits adds 1.0 OUTPUT_IMAGE.fits" << std::endl;
        std::cout << "    CrabFitsImageArithmetic FITS_IMAGE.fits '+' 1.0 OUTPUT_IMAGE.fits" << std::endl;
        std::cout << "    CrabFitsImageArithmetic FITS_IMAGE.fits '-' 1.0 OUTPUT_IMAGE.fits" << std::endl;
        std::cout << "    CrabFitsImageArithmetic FITS_IMAGE.fits '*' 1.0 OUTPUT_IMAGE.fits" << std::endl;
        std::cout << "    CrabFitsImageArithmetic FITS_IMAGE.fits '/' 1.0 OUTPUT_IMAGE.fits" << std::endl;
        std::cout << "    2. Removing NaN pixels:" << std::endl;
        std::cout << "    CrabFitsImageArithmetic FITS_IMAGE.fits times 1.0 OUTPUT_IMAGE.fits -remove-nan" << std::endl;
        std::cout << "    3. Copying WCS:" << std::endl;
        std::cout << "    CrabFitsImageArithmetic FITS_IMAGE.fits times 1.0 OUTPUT_IMAGE.fits -remove-nan -copy-wcs" << std::endl;
        std::cout << "    4. Operating on a specified fits extension:" << std::endl;
        std::cout << "       (with the -ext argument with a 0-based number)" << std::endl;
        std::cout << "    CrabFitsImageArithmetic FITS_IMAGE.fits -ext 3 times 1.0 OUTPUT_IMAGE.fits -remove-nan -copy-wcs" << std::endl;
        std::cout << "    5. Operating on a specified rectangle area:" << std::endl;
        std::cout << "       (with the -rect argument with four 0-based numbers indicating x1,y1 and x2,y2)" << std::endl;
        std::cout << "    CrabFitsImageArithmetic FITS_IMAGE.fits -rect 244 218 248 222 adds image_5x5.fits OUTPUT_IMAGE.fits -copy-wcs" << std::endl;
        std::cout << "Last update:" << std::endl;
        std::cout << "    2019-06-17 Heidelberg" << std::endl;
    }
    return 0;
}
