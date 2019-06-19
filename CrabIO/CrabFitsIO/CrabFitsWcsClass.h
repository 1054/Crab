/*
 
 This code CrabFitsWcs aims at providing a class for astronomical fits image world coordinate system (WCS) calculation.
 
*/
#ifndef CRABFITSWCSCLASS_H
#define CRABFITSWCSCLASS_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip> // std::cout
#include <cmath> // NAN, std::isnan

class CrabFitsWcsClass
{
public:
    CrabFitsWcsClass(char *cstrFilePath = NULL, int intExtNumber = 0, int debug = 0);
    void clear_variables();
    int read_from_fits_file(char *cstrFilePath, int intExtNumber = 0, int debug = 0);
    int print_wcs_info();
    
private:
    std::string FilePath;
    int ExtNumber;
    std::string Header;
    std::string CTYPE1;
    std::string CTYPE2;
    double CRPIX1;
    double CRPIX2;
    double CRVAL1;
    double CRVAL2;
    double CDELT1;
    double CDELT2;
    double CROTA1;
    double CROTA2;
    std::string CUNIT1;
    std::string CUNIT2;
    double CD1_1;
    double CD1_2;
    double CD2_1;
    double CD2_2;
};


#endif // CRABFITSWCSCLASS_H
