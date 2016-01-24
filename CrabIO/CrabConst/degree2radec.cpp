/*
 
 ;
 ; convert ra dec in seximal system to decimal system
 ;
 
 Please compile like this:
 
 clang++ CrabStringClean.cpp degree2radec.cpp -o degree2radec_mac
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

std::string CrabStringTrim(const string &t, int trimflag = 0);


int main(int argc, char **argv)
{
    /**
     // You’ll notice that argv[0] is the path and name of the program itself.
     std::cout << "CrabTableReadInfo: Input " << argc-1 << " arguments." << std::endl;
     for (int i = 0; i < argc; ++i) {
     std::cout << argv[i] << std::endl;
     }
     **/
    
    char *cstrRA = NULL;
    char *cstrDec = NULL;
    
    int debug = 0;
    
    for(int i=1; i<argc; i++) {
        if(cstrRA==NULL && i<=argc-1) { cstrRA = argv[i]; continue; }
        if(cstrDec==NULL && i<=argc-1) { cstrDec = argv[i]; continue; }
    }
    
    if(cstrRA!=NULL && cstrDec!=NULL) {
        //
        if(debug>0) {
            std::cout << "RA = " << cstrRA << std::endl;
            std::cout << "Dec = " << cstrDec << std::endl;
        }
        // use std::string to operate
        std::string strRA(cstrRA);
        std::string strDec(cstrDec);
        // trim
        strRA = CrabStringTrim(strRA);
        strDec = CrabStringTrim(strDec); 
        //
        if(strRA.length()>=1 && strDec.length()>=1) {
            // define all values
            double dbRA_degrees = atof(strRA.c_str());
            double dbDec_degrees = atof(strDec.c_str());
            std::string strRA_sign = ""; 
            std::string strDec_sign = "";
            if(dbRA_degrees<0) { std::cout << "degree2radec: Error! Right Angle is negative!" << std::endl; return -1; }
            if(dbDec_degrees<0) { strDec_sign = "-"; dbDec_degrees = -dbDec_degrees; }
            // now convert from hour:min:sec,degree:min:sec to degree,degree
            double dbRA_hour = dbRA_degrees/360.0*24.0;
            double dbRA_min = (dbRA_hour-(int)dbRA_hour)*60.0;
            double dbRA_sec = (dbRA_min-(int)dbRA_min)*60.0;
            double dbDec_deg = dbDec_degrees;
            double dbDec_min = (dbDec_deg-(int)dbDec_deg)*60.0;
            double dbDec_sec = (dbDec_min-(int)dbDec_min)*60.0;
            std::cout << std::fixed << std::setw(2) << std::setfill('0') << (int)dbRA_hour << ":";
            std::cout << std::fixed << std::setw(2) << std::setfill('0') << (int)dbRA_min << ":";
            std::cout << std::fixed << std::setw(6) << std::setfill('0') << std::setprecision(3) << dbRA_sec << " ";
            std::cout << strDec_sign; 
            std::cout << std::fixed << std::setw(2) << std::setfill('0') << (int)dbDec_deg << ":";
            std::cout << std::fixed << std::setw(2) << std::setfill('0') << (int)dbDec_min  << ":";
            std::cout << std::fixed << std::setw(6) << std::setfill('0') << std::setprecision(3) << dbDec_sec << std::endl;
            
        } else {
            std::cout << "degree2radec: Error! Input RA Dec has invalid format!" << std::endl;
        }
        
    } else {
        std::cout << "degree2radec 150.2380500000 -2.3366194444" << std::endl;
    }
    
    return 0;
}


