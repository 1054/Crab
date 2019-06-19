/*
 
 This code CrabFitsWcs aims at providing a class for astronomical fits image world coordinate system (WCS) calculation.
 
 */

#include "CrabFitsWcsClass.h"
#include "CrabFitsIO.h"


using namespace std;


CrabFitsWcsClass::CrabFitsWcsClass(char *cstrFilePath, int intExtNumber, int debug)
{
    if(cstrFilePath != NULL) {
        this->read_from_fits_file(cstrFilePath, intExtNumber, debug);
    }
}


void CrabFitsWcsClass::clear_variables()
{
    FilePath = "";
    Header = "";
    CTYPE1 = "";
    CTYPE2 = "";
    CRPIX1 = NAN;
    CRPIX2 = NAN;
    CRVAL1 = NAN;
    CRVAL2 = NAN;
    CDELT1 = NAN;
    CDELT2 = NAN;
    CROTA1 = NAN;
    CROTA2 = NAN;
    CUNIT1 = "";
    CUNIT2 = "";
    CD1_1 = NAN;
    CD1_2 = NAN;
    CD2_1 = NAN;
    CD2_2 = NAN;
}


int CrabFitsWcsClass::read_from_fits_file(char *cstrFilePath, int intExtNumber, int debug)
{
    int errStatus = 0;
    //
    // clear variables
    this->clear_variables();
    //
    // make sure the input is not NULL
    if(cstrFilePath != NULL) {
        //
        // store file path and fits extension number
        FilePath = std::string(cstrFilePath);
        ExtNumber = intExtNumber;
        //
        // init variables
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
        // read Wcs fits header
        while(ExtNumber==0 || cstrWcsHeader==NULL) {
            //
            if(debug>=1) {
                std::cout << "CrabFitsWcs: reading wcs from fits extension " << ExtNumber << std::endl;
            }
            //
            errStatus = readFitsHeader(FilePath.c_str(), ExtNumber, &cstrWcsHeader, &posWcsHeader, &lenWcsHeader);
            //
            if(cstrWcsHeader!=NULL) {
                if(cstrWcsCTYPE1==NULL) {cstrWcsCTYPE1 = extKeyword("CTYPE1",cstrWcsHeader,1); if(cstrWcsCTYPE1!=NULL) {this->CTYPE1 = std::string(cstrWcsCTYPE1); if (debug>=1) {std::cout << "CrabFitsWcs: read wcs CTYPE1 = " << cstrWcsCTYPE1 << std::endl;}}}
                if(cstrWcsCTYPE2==NULL) {cstrWcsCTYPE2 = extKeyword("CTYPE2",cstrWcsHeader,1); if(cstrWcsCTYPE2!=NULL) {this->CTYPE2 = std::string(cstrWcsCTYPE2); if(debug>=1) {std::cout << "CrabFitsWcs: read wcs CTYPE2 = " << cstrWcsCTYPE2 << std::endl;}} }
                if(cstrWcsCRPIX1==NULL) {cstrWcsCRPIX1 = extKeyword("CRPIX1",cstrWcsHeader,1); if(cstrWcsCRPIX1!=NULL) {this->CRPIX1 = std::stod(cstrWcsCRPIX1); if(debug>=1) {std::cout << "CrabFitsWcs: read wcs CRPIX1 = " << cstrWcsCRPIX1 << std::endl;}} }
                if(cstrWcsCRPIX2==NULL) {cstrWcsCRPIX2 = extKeyword("CRPIX2",cstrWcsHeader,1); if(cstrWcsCRPIX2!=NULL) {this->CRPIX2 = std::stod(cstrWcsCRPIX2); if(debug>=1) {std::cout << "CrabFitsWcs: read wcs CRPIX2 = " << cstrWcsCRPIX2 << std::endl;}} }
                if(cstrWcsCRVAL1==NULL) {cstrWcsCRVAL1 = extKeyword("CRVAL1",cstrWcsHeader,1); if(cstrWcsCRVAL1!=NULL) {this->CRVAL1 = std::stod(cstrWcsCRVAL1); if(debug>=1) {std::cout << "CrabFitsWcs: read wcs CRVAL1 = " << cstrWcsCRVAL1 << std::endl;}} }
                if(cstrWcsCRVAL2==NULL) {cstrWcsCRVAL2 = extKeyword("CRVAL2",cstrWcsHeader,1); if(cstrWcsCRVAL2!=NULL) {this->CRVAL2 = std::stod(cstrWcsCRVAL2); if(debug>=1) {std::cout << "CrabFitsWcs: read wcs CRVAL2 = " << cstrWcsCRVAL2 << std::endl;}} }
                if(cstrWcsCDELT1==NULL) {cstrWcsCDELT1 = extKeyword("CDELT1",cstrWcsHeader,1); if(cstrWcsCDELT1!=NULL) {this->CDELT1 = std::stod(cstrWcsCDELT1); if(debug>=1) {std::cout << "CrabFitsWcs: read wcs CDELT1 = " << cstrWcsCDELT1 << std::endl;}} }
                if(cstrWcsCDELT2==NULL) {cstrWcsCDELT2 = extKeyword("CDELT2",cstrWcsHeader,1); if(cstrWcsCDELT2!=NULL) {this->CDELT2 = std::stod(cstrWcsCDELT2); if(debug>=1) {std::cout << "CrabFitsWcs: read wcs CDELT2 = " << cstrWcsCDELT2 << std::endl;}} }
                if(cstrWcsCROTA1==NULL) {cstrWcsCROTA1 = extKeyword("CROTA1",cstrWcsHeader,1); if(cstrWcsCROTA1!=NULL) {this->CROTA1 = std::stod(cstrWcsCROTA1); if(debug>=1) {std::cout << "CrabFitsWcs: read wcs CROTA1 = " << cstrWcsCROTA1 << std::endl;}} }
                if(cstrWcsCROTA2==NULL) {cstrWcsCROTA2 = extKeyword("CROTA2",cstrWcsHeader,1); if(cstrWcsCROTA2!=NULL) {this->CROTA2 = std::stod(cstrWcsCROTA2); if(debug>=1) {std::cout << "CrabFitsWcs: read wcs CROTA2 = " << cstrWcsCROTA2 << std::endl;}} }
                if(cstrWcsCUNIT1==NULL) {cstrWcsCUNIT1 = extKeyword("CUNIT1",cstrWcsHeader,1); if(cstrWcsCUNIT1!=NULL) {this->CUNIT1 = std::string(cstrWcsCUNIT1); if(debug>=1) {std::cout << "CrabFitsWcs: read wcs CUNIT1 = " << cstrWcsCUNIT1 << std::endl;}} }
                if(cstrWcsCUNIT2==NULL) {cstrWcsCUNIT2 = extKeyword("CUNIT2",cstrWcsHeader,1); if(cstrWcsCUNIT2!=NULL) {this->CUNIT2 = std::string(cstrWcsCUNIT2); if(debug>=1) {std::cout << "CrabFitsWcs: read wcs CUNIT2 = " << cstrWcsCUNIT2 << std::endl;}} }
                if(cstrWcsCD1_1==NULL) {cstrWcsCD1_1 = extKeyword("CD1_1",cstrWcsHeader,1); if(cstrWcsCD1_1!=NULL) {this->CD1_1 = std::stod(cstrWcsCD1_1); if(debug>=1) {std::cout << "CrabFitsWcs: read wcs CD1_1 = " << cstrWcsCD1_1 << std::endl;}} }
                if(cstrWcsCD1_2==NULL) {cstrWcsCD1_2 = extKeyword("CD1_2",cstrWcsHeader,1); if(cstrWcsCD1_2!=NULL) {this->CD1_2 = std::stod(cstrWcsCD1_2); if(debug>=1) {std::cout << "CrabFitsWcs: read wcs CD1_2 = " << cstrWcsCD1_2 << std::endl;}} }
                if(cstrWcsCD2_1==NULL) {cstrWcsCD2_1 = extKeyword("CD2_1",cstrWcsHeader,1); if(cstrWcsCD2_1!=NULL) {this->CD2_1 = std::stod(cstrWcsCD2_1); if(debug>=1) {std::cout << "CrabFitsWcs: read wcs CD2_1 = " << cstrWcsCD2_1 << std::endl;}} }
                if(cstrWcsCD2_2==NULL) {cstrWcsCD2_2 = extKeyword("CD2_2",cstrWcsHeader,1); if(cstrWcsCD2_2!=NULL) {this->CD2_2 = std::stod(cstrWcsCD2_2); if(debug>=1) {std::cout << "CrabFitsWcs: read wcs CD2_2 = " << cstrWcsCD2_2 << std::endl;}} }
                //
                // if got all needed keywords then break.
                if(cstrWcsCTYPE1!=NULL && cstrWcsCTYPE2!=NULL &&
                   cstrWcsCRPIX1!=NULL && cstrWcsCRPIX2!=NULL &&
                   cstrWcsCRVAL1!=NULL && cstrWcsCRVAL2!=NULL) {
                    // store fits header
                    this->Header = std::string(cstrWcsHeader);
                    // check more
                    if((cstrWcsCDELT1!=NULL && cstrWcsCDELT2!=NULL) ||
                       (cstrWcsCD1_1!=NULL && cstrWcsCD1_2!=NULL &&
                        cstrWcsCD2_1!=NULL && cstrWcsCD2_2!=NULL)
                      ) {
                        errStatus = 0;
                        break; // jump out of the while loop
                    } else {
                        errStatus = -3;
                        if(debug>=1) {std::cout << "CrabFitsWcs: found CTYPE, CRPIX and CRVAL but not CDELT or CDX_X from fits extension " << ExtNumber << std::endl;}
                    }
                } else {
                    errStatus = -2;
                    if(debug>=1) {std::cout << "CrabFitsWcs: could not find CTYPE, CRPIX or CRVAL from fits extension " << ExtNumber << std::endl;}
                }
            } else {
                errStatus = -1;
                if(debug>=1) {std::cout << "CrabFitsWcs: could not read fits extension " << ExtNumber << std::endl;}
            }
            ExtNumber++;
        }
    }
    return errStatus;
}


int CrabFitsWcsClass::print_wcs_info()
{
    if(!CTYPE1.empty() && !CTYPE2.empty() &&
       !std::isnan(CRPIX1) && !std::isnan(CRPIX2) &&
       !std::isnan(CRVAL1) && !std::isnan(CRVAL2)) {
        std::cout << "WCS info: " << std::endl;
        std::cout << "  CTYPE1 = " << std::setw(18) << CTYPE1;
        std::cout << "  CTYPE2 = " << std::setw(18) << CTYPE2 << std::endl;
        std::cout << "  CUNIT1 = " << std::setw(18) << CUNIT1;
        std::cout << "  CUNIT2 = " << std::setw(18) << CUNIT2 << std::endl;
        std::cout << "  CRPIX1 = " << std::setw(18) << CRPIX1;
        std::cout << "  CRPIX2 = " << std::setw(18) << CRPIX2 << std::endl;
        std::cout << "  CRVAL1 = " << std::setw(18) << CRVAL1;
        std::cout << "  CRVAL2 = " << std::setw(18) << CRVAL2 << std::endl;
        if(!std::isnan(CDELT1) && !std::isnan(CDELT2)) {
            std::cout << "  CDELT1 = " << std::setw(18) << CDELT1;
            std::cout << "  CDELT2 = " << std::setw(18) << CDELT2 << std::endl;
            return 0;
        } else if(!std::isnan(CD1_1) && !std::isnan(CD1_2) &&
                  !std::isnan(CD2_1) && !std::isnan(CD2_2)) {
            std::cout << "  CD1_1  = " << std::setw(18) << CD1_1;
            std::cout << "  CD2_1  = " << std::setw(18) << CD2_1 << std::endl;
            std::cout << "  CD1_2  = " << std::setw(18) << CD1_2;
            std::cout << "  CD2_2  = " << std::setw(18) << CD2_2 << std::endl;
            return 0;
        }
    }
    return -1;
}


