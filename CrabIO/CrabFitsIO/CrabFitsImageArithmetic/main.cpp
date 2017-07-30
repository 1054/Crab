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
 
     clang++ main.cpp CrabFitsIO.cpp -o CrabFitsImageArithmetic_linux_x86_64
     clang++ -I/usr/include/malloc/ main.cpp CrabFitsIO.cpp -o CrabFitsImageArithmetic_mac; cp -i CrabFitsImageArithmetic_mac ~/Cloud/Github/DeepFields.SuperDeblending/Softwares/ds9_mac/
 
 Last update:
     
     2017-02-26   copied from CrabFitsImageCut/CrabFitsImageArithmetic
     2017-05-12   -replace-nan
     2017-07-20   also remove NAXIS1 and NAXIS2 in the mainHeader (but not recompiled yet)
 
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "CrabFitsIO.h"
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
    char *cstrFilePath = NULL; char *cstrExtNumber = (char *)"0";
    char *cstrFilePathRefImage = NULL; char *cstrExtNumberRefImage = (char *)"0";
    char *cstrOperator = NULL;
    char *cstrNumValue = NULL; double dblNumValue = 0.0;
    char *cstrNewFilePath = NULL;
    int   iRemoveNaN = 0; // in default we keep NaN values
    int   iCopyWcs = 0; // in default we do not search Wcs keywords, if the user gives '-copy-wcs' keyword, then we search the whole fits header and look for Wcs keywords and copy into the output fits file.
    char *cstrReplaceNaN = (char *)"0.0"; // the value to replace for NaN pixels
    
    int debug = 0; // <TODO><DEBUG>
    
    for(int i=1; i<argc; i++) {
        if(strncmp(argv[i],"-ext",4)==0 && i<argc-1) {
            i++;
            if(strcmp(cstrExtNumber,"0")==0) {cstrExtNumber = argv[i]; continue;}
            if(strcmp(cstrExtNumberRefImage,"0")==0) {cstrExtNumberRefImage = argv[i]; continue;}
        }
        if(strcasecmp(argv[i],"-remove-nan")==0 ) {
            iRemoveNaN = 1; continue;
        }
        if(strcasecmp(argv[i],"-replace-nan")==0 ) {
            iRemoveNaN = 1; if(i+1<=argc-1) { cstrReplaceNaN = argv[i+1]; } i++; continue;
        }
        if(strcasecmp(argv[i],"-debug")==0 ) {
            debug = 1; continue;
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
        std::cout << "CrabFitsImageArithmetic: " << cstrFilePath << " extension=" << atoi(cstrExtNumber) << std::endl;
        //
        // read Wcs fits header <20170730>
        if(iCopyWcs>0) {
            extNumerWcsHeader = 0;
            while(extNumerWcsHeader==0 || cstrWcsHeader!=NULL) {
                std::cout << "CrabFitsImageArithmetic: copy wcs from fits extension " << extNumerWcsHeader << std::endl;
                errStatus = readFitsHeader(cstrFilePath,extNumerWcsHeader,&cstrWcsHeader,&posWcsHeader,&lenWcsHeader);
                if(cstrWcsHeader!=NULL) {
                    if(cstrWcsCTYPE1==NULL) {cstrWcsCTYPE1 = extKeyword("CTYPE1",cstrWcsHeader); if(cstrWcsCTYPE1!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CTYPE1 = " << cstrWcsCTYPE1 << std::endl;} }
                    if(cstrWcsCTYPE2==NULL) {cstrWcsCTYPE2 = extKeyword("CTYPE2",cstrWcsHeader); if(cstrWcsCTYPE2!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CTYPE2 = " << cstrWcsCTYPE2 << std::endl;} }
                    if(cstrWcsCRPIX1==NULL) {cstrWcsCRPIX1 = extKeyword("CRPIX1",cstrWcsHeader); if(cstrWcsCRPIX1!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CRPIX1 = " << cstrWcsCRPIX1 << std::endl;} }
                    if(cstrWcsCRPIX2==NULL) {cstrWcsCRPIX2 = extKeyword("CRPIX2",cstrWcsHeader); if(cstrWcsCRPIX2!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CRPIX2 = " << cstrWcsCRPIX2 << std::endl;} }
                    if(cstrWcsCRVAL1==NULL) {cstrWcsCRVAL1 = extKeyword("CRVAL1",cstrWcsHeader); if(cstrWcsCRVAL1!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CRVAL1 = " << cstrWcsCRVAL1 << std::endl;} }
                    if(cstrWcsCRVAL2==NULL) {cstrWcsCRVAL2 = extKeyword("CRVAL2",cstrWcsHeader); if(cstrWcsCRVAL2!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CRVAL2 = " << cstrWcsCRVAL2 << std::endl;} }
                    if(cstrWcsCDELT1==NULL) {cstrWcsCDELT1 = extKeyword("CDELT1",cstrWcsHeader); if(cstrWcsCDELT1!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CDELT1 = " << cstrWcsCDELT1 << std::endl;} }
                    if(cstrWcsCDELT2==NULL) {cstrWcsCDELT2 = extKeyword("CDELT2",cstrWcsHeader); if(cstrWcsCDELT2!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CDELT2 = " << cstrWcsCDELT2 << std::endl;} }
                    if(cstrWcsCROTA1==NULL) {cstrWcsCROTA1 = extKeyword("CROTA1",cstrWcsHeader); if(cstrWcsCROTA1!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CROTA1 = " << cstrWcsCROTA1 << std::endl;} }
                    if(cstrWcsCROTA2==NULL) {cstrWcsCROTA2 = extKeyword("CROTA2",cstrWcsHeader); if(cstrWcsCROTA2!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CROTA2 = " << cstrWcsCROTA2 << std::endl;} }
                    if(cstrWcsCUNIT1==NULL) {cstrWcsCUNIT1 = extKeyword("CUNIT1",cstrWcsHeader); if(cstrWcsCUNIT1!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CUNIT1 = " << cstrWcsCUNIT1 << std::endl;} }
                    if(cstrWcsCUNIT2==NULL) {cstrWcsCUNIT2 = extKeyword("CUNIT2",cstrWcsHeader); if(cstrWcsCUNIT2!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CUNIT2 = " << cstrWcsCUNIT2 << std::endl;} }
                    if(cstrWcsCD1_1==NULL) {cstrWcsCD1_1 = extKeyword("CD1_1",cstrWcsHeader); if(cstrWcsCD1_1!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CD1_1 = " << cstrWcsCD1_1 << std::endl;} }
                    if(cstrWcsCD1_2==NULL) {cstrWcsCD1_2 = extKeyword("CD1_2",cstrWcsHeader); if(cstrWcsCD1_2!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CD1_2 = " << cstrWcsCD1_2 << std::endl;} }
                    if(cstrWcsCD2_1==NULL) {cstrWcsCD2_1 = extKeyword("CD2_1",cstrWcsHeader); if(cstrWcsCD2_1!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CD2_1 = " << cstrWcsCD2_1 << std::endl;} }
                    if(cstrWcsCD2_2==NULL) {cstrWcsCD2_2 = extKeyword("CD2_2",cstrWcsHeader); if(cstrWcsCD2_2!=NULL) {std::cout << "CrabFitsImageArithmetic: copy wcs CD2_2 = " << cstrWcsCD2_2 << std::endl;} }
                }
                extNumerWcsHeader++;
            }
        }
        //
        // read fits header
        errStatus = readFitsHeader(cstrFilePath,extNumber,&cstrHeader,&posHeader,&lenHeader);
        std::cout << "CrabFitsImageArithmetic: readFitsHeader " << cstrFilePath << " extension=" << extNumber << " headersize=" << strlen(cstrHeader) << std::endl;
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
            // prepare new image
            long newImWidth = oldImWidth;
            long newImHeight = oldImHeight;
            //
            // create new image array
            double *newImage = (double *)malloc(newImWidth*newImHeight*sizeof(double));
            //
            // remove NaN
            if(0==iRemoveNaN) {
                for(int i=0; i<newImWidth*newImHeight; i++) { newImage[i] = NAN; } // need cmath.h
            } else {
                double dblReplaceNaN = atof(cstrReplaceNaN);
                if(debug>0) {
                    std::cout << "DEBUG: removing all NaN values by filling " << dblReplaceNaN << std::endl;
                }
                for(int i=0; i<newImWidth*newImHeight; i++) { newImage[i] = dblReplaceNaN; } // need cmath.h
            }
            //
            // check operator
            std::string sstrOperator = std::string(cstrOperator);
            if(sstrOperator.find("*")!=std::string::npos ||
               sstrOperator.find("time")!=std::string::npos ||
               sstrOperator.find("multipl")!=std::string::npos ) {
                sstrOperator = "multiplies";
            } else if(sstrOperator.find("+")!=std::string::npos ||
                      sstrOperator.find("add")!=std::string::npos ||
                      sstrOperator.find("plus")!=std::string::npos ) {
                sstrOperator = "adds";
            } else if(sstrOperator.find("-")!=std::string::npos ||
                      sstrOperator.find("subtract")!=std::string::npos ||
                      sstrOperator.find("minus")!=std::string::npos ) {
                sstrOperator = "subtracts";
            } else if(sstrOperator.find("/")!=std::string::npos ||
                      sstrOperator.find("divide")!=std::string::npos ) {
                sstrOperator = "divides";
            } else if(sstrOperator.find("^")!=std::string::npos ||
                      sstrOperator.find("power")!=std::string::npos ) {
                sstrOperator = "power of";
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
                // check
                std::cout << "CrabFitsImageArithmetic: computing " << cstrFilePath << " extension=" << extNumber << " " << sstrOperator << " " << dblNumValue << " " << std::endl;
                //
                // copy image pixel by pixel
                for(int jj=0; jj<=(newImHeight-1); jj++) {
                    for(int ii=0; ii<=(newImWidth-1); ii++) {
                        long kk = long(ii)+long(jj)*newImWidth;
                        if(NAN != oldImage[kk] && oldImage[kk] == oldImage[kk]) {
                            if("multiplies" == sstrOperator) {
                                newImage[kk] = oldImage[kk] * dblNumValue;
                            } else if("adds" == sstrOperator) {
                                newImage[kk] = oldImage[kk] + dblNumValue;
                            } else if("subtracts" == sstrOperator) {
                                newImage[kk] = oldImage[kk] - dblNumValue;
                            } else if("divides" == sstrOperator) {
                                newImage[kk] = oldImage[kk] / dblNumValue;
                            } else if("power of" == sstrOperator) {
                                newImage[kk] = pow(oldImage[kk], dblNumValue);
                            }
                        }
                    }
                }
            } else {
                //
                // if the input NumValue is RefImage
                cstrFilePathRefImage = (char *)strRefImage.c_str();
                //
                // check
                std::cout << "CrabFitsImageArithmetic: " << cstrFilePathRefImage << " extension=" << atoi(cstrExtNumberRefImage) << std::endl;
                //
                // read fits header
                errStatus = readFitsHeader(cstrFilePathRefImage,extNumberRefImage,&cstrHeaderRefImage,&posHeaderRefImage,&lenHeaderRefImage);
                std::cout << "CrabFitsImageArithmetic: readFitsHeader " << cstrFilePathRefImage << " extension=" << extNumberRefImage << " headersize=" << strlen(cstrHeaderRefImage) << std::endl;
                //
                // read fits Naxis
                char *cstrNAXIS1RefImage = extKeyword("NAXIS1",cstrHeaderRefImage);
                char *cstrNAXIS2RefImage = extKeyword("NAXIS2",cstrHeaderRefImage);
                if(cstrNAXIS1RefImage!=NULL && cstrNAXIS2RefImage!=NULL) {
                    //
                    // read fits Naxis
                    long refImWidth = atol(cstrNAXIS1RefImage);
                    long refImHeight = atol(cstrNAXIS2RefImage);
                    //
                    // check -- if the input NumValue is RefImage
                    std::cout << "CrabFitsImageArithmetic: computing " << cstrFilePath << " extension=" << extNumber << " " << sstrOperator << " " << cstrFilePathRefImage << " extension=" << extNumberRefImage << std::endl;
                    //
                    // read ref image
                    double *refImage = readFitsImage(cstrFilePathRefImage,extNumberRefImage);
                    std::cout << "CrabFitsImageArithmetic: readFitsImage " << cstrFilePathRefImage << " extension=" << extNumberRefImage << " imagesize=" << refImWidth << "x" << refImHeight << " bitpix=" << -long(sizeof(double))*8 << std::endl;
                    //
                    // check ref image size
                    if(refImWidth!=oldImWidth || refImHeight!=oldImHeight) {
                        std::cout << "Error! The two input fits images have different dimensions!" << std::endl;
                        return -1;
                    }
                    //
                    // copy image pixel by pixel
                    for(int jj=0; jj<=(newImHeight-1); jj++) {
                        for(int ii=0; ii<=(newImWidth-1); ii++) {
                            long kk = long(ii)+long(jj)*newImWidth;
                            if(NAN != oldImage[kk] && oldImage[kk] == oldImage[kk]) {
                                if("multiplies" == sstrOperator) {
                                    newImage[kk] = oldImage[kk] * refImage[kk];
                                } else if("adds" == sstrOperator) {
                                    newImage[kk] = oldImage[kk] + refImage[kk];
                                } else if("subtracts" == sstrOperator) {
                                    newImage[kk] = oldImage[kk] - refImage[kk];
                                } else if("divides" == sstrOperator) {
                                    newImage[kk] = oldImage[kk] / refImage[kk];
                                } else if("power of" == sstrOperator) {
                                    newImage[kk] = pow(oldImage[kk], refImage[kk]);
                                }
                            }
                        }
                    }
                } else {
                    std::cout << "Error! Failed to read NAXIS1 and NAXIS2 from the second input fits file!" << std::endl;
                    return -1;
                }
            }
            if(1) {
                //
                // also modify BITPIX
                if(debug>0) {
                    std::cout << "DEBUG: old BITPIX = " << extKeyword("BITPIX",cstrHeader) << std::endl;
                }
                char *newBITPIX = new char(4);
                memset(newBITPIX,'\0',4);
                sprintf(newBITPIX,"%ld",-long(sizeof(double))*8);
                errStatus = modKeyword("BITPIX",newBITPIX,cstrHeader);
                if(debug>0) {
                    std::cout << "DEBUG: new BITPIX = " << extKeyword("BITPIX",cstrHeader) << std::endl;
                }
                //
                // copy Wcs fits header <20170730>
                if(iCopyWcs>0) {
                    //std::cout << "DEBUG: addKeyword" << std::endl;
                    if(cstrWcsCTYPE1) {addKeyword("CTYPE1",cstrWcsCTYPE1,&cstrHeader);}
                    if(cstrWcsCTYPE2) {addKeyword("CTYPE2",cstrWcsCTYPE2,&cstrHeader);}
                    if(cstrWcsCRPIX1) {addKeyword("CRPIX1",cstrWcsCRPIX1,&cstrHeader);}
                    if(cstrWcsCRPIX2) {addKeyword("CRPIX2",cstrWcsCRPIX2,&cstrHeader);}
                    if(cstrWcsCRVAL1) {addKeyword("CRVAL1",cstrWcsCRVAL1,&cstrHeader);}
                    if(cstrWcsCRVAL2) {addKeyword("CRVAL2",cstrWcsCRVAL2,&cstrHeader);}
                    if(cstrWcsCDELT1) {addKeyword("CDELT1",cstrWcsCDELT1,&cstrHeader);}
                    if(cstrWcsCDELT2) {addKeyword("CDELT2",cstrWcsCDELT2,&cstrHeader);}
                    if(cstrWcsCROTA1) {addKeyword("CROTA1",cstrWcsCROTA1,&cstrHeader);}
                    if(cstrWcsCROTA2) {addKeyword("CROTA2",cstrWcsCROTA2,&cstrHeader);}
                    if(cstrWcsCUNIT1) {addKeyword("CUNIT1",cstrWcsCUNIT1,&cstrHeader);}
                    if(cstrWcsCUNIT2) {addKeyword("CUNIT2",cstrWcsCUNIT2,&cstrHeader);}
                    if(cstrWcsCD1_1) {addKeyword("CD1_1",cstrWcsCD1_1,&cstrHeader);}
                    if(cstrWcsCD1_2) {addKeyword("CD1_2",cstrWcsCD1_2,&cstrHeader);}
                    if(cstrWcsCD2_1) {addKeyword("CD2_1",cstrWcsCD2_1,&cstrHeader);}
                    if(cstrWcsCD2_2) {addKeyword("CD2_2",cstrWcsCD2_2,&cstrHeader);}
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
                    tempHeader = mainHeader;
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
                        }
                        if(0==strncmp(tempHeader,"END      ",9)){
                            for(int tempCounter=0; tempCounter<80; tempCounter++){strncpy(tempHeader+tempCounter," ",1);} continue;
                        }
                        tempHeader++;
                    }
                    tempHeader = cstrHeader;
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
                    if(debug>0) {
                        std::cout << "CrabFitsImageArithmetic: Input " << cstrFilePath << std::endl;
                        std::cout << "CrabFitsImageArithmetic: Output " << cstrNewFilePath << std::endl;
                    }
                }
                //
                // now save new fits
                // const char *newFilePath = NULL;
                // newFilePath = "/Users/dliu/Programming/QtProgram/CrabIO/CrabFitsIO/CrabFitsImageCut/new.fits";
                // std::cout << "CrabFitsImageArithmetic: Opening " << newFilePath << " to write." << std::endl;
                if(debug>0) {
                    std::cout << "CrabFitsImageArithmetic: Opening " << cstrNewFilePath << " to write" << std::endl;
                }
                errStatus = writeFitsD(newImage,cstrHeader,cstrNewFilePath);
                if(errStatus==0) {
                    if(debug>0) {
                        std::cout << "CrabFitsImageArithmetic: Well done!" << std::endl;
                    }
                    std::cout << "CrabFitsImageArithmetic: File has been saved to " << cstrNewFilePath << std::endl;
                } else {
                    std::cout << "CrabFitsImageArithmetic: Oooh! Failed! We are sorry ... (error code " << errStatus << ")" << std::endl;
                }
                //
                //
                free(newImage); newImage = NULL;
            }
        } else {
            std::cout << "CrabFitsImageArithmetic: Error! Fits header of extension " << extNumber << " does not contain NAXIS1 and NAXIS2!" << std::endl;
        }
    } else {
        std::cout << "Usage: " << std::endl;
        std::cout << "    CrabFitsImageArithmetic FITS_IMAGE.fits 'divides' -0.5 FITS_IMAGE_OUTPUT.fits" << std::endl;
        std::cout << "    CrabFitsImageArithmetic image_sci.fits '*' 0.0 image_zero.fits -remove-nan" << std::endl;
    }
    return 0;
}
