/*
 
 ;
 ; convert ra dec in seximal system to decimal system
 ;
 
 Please compile like this:
 
 clang++ CrabStringClean.cpp radec2degree.cpp -o radec2degree_mac
 
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
        if(strRA.length()>=5 && strDec.length()>=5) {
            // define all strings
            std::string strRA_sign;
            std::string strRA_hour;
            std::string strRA_min;
            std::string strRA_sec;
            std::string strDec_sign;
            std::string strDec_deg;
            std::string strDec_min;
            std::string strDec_sec;
            // analyze first byte
            if(strRA.find_first_of("-")==0) { strRA_sign = "-1"; strRA = strRA.substr(1,strRA.length()-1); } else if(strRA.find_first_of("+")==0) { strRA_sign = "+1"; strRA = strRA.substr(1,strRA.length()-1); } else { strRA_sign = "+1"; }
            if(strDec.find_first_of("-")==0) { strDec_sign = "-1"; strDec = strDec.substr(1,strDec.length()-1); } else if(strDec.find_first_of("+")==0) { strDec_sign = "+1"; strDec = strDec.substr(1,strDec.length()-1); } else { strDec_sign = "+1"; }
            if(debug>0) {
                std::cout << strRA_sign << std::endl;
                std::cout << strDec_sign << std::endl;
                //std::cout << strRA << std::endl;
                //std::cout << strDec << std::endl;
            }
            // prepare posSeperator
            size_t posSep = std::string::npos;
            // analyze RA hour
            posSep = std::string::npos;
            if(posSep==std::string::npos) { posSep = strRA.find(":"); }
            if(posSep==std::string::npos) { posSep = strRA.find("h"); }
            if(posSep==std::string::npos) { std::cout << "radec2degree: Error! Right Angle hour value is invalid!" << std::endl; return -1; }
            if(debug>0) { std::cout << " strRA=" << strRA << " posSep=" << posSep << std::endl; }
            strRA_hour = strRA.substr(0,posSep);
            strRA = strRA.substr(posSep+1,strRA.length()-2);
            // analyze Dec degree
            posSep = std::string::npos;
            if(posSep==std::string::npos) { posSep = strDec.find(":"); }
            if(posSep==std::string::npos) { posSep = strDec.find("d"); }
            if(posSep==std::string::npos) { std::cout << "radec2degree: Error! Declination degree value is invalid!" << std::endl; return -1; }
            if(debug>0) { std::cout << " strDec=" << strDec << " posSep=" << posSep << std::endl; }
            strDec_deg = strDec.substr(0,posSep);
            strDec = strDec.substr(posSep+1,strDec.length()-2);
            // debug RA hour and Dec degree
            if(debug>0) {
                std::cout << strRA_hour << std::endl;
                std::cout << strDec_deg << std::endl;
                //std::cout << strRA << std::endl;
                //std::cout << strDec << std::endl;
            }
            // analyze RA min
            posSep = std::string::npos;
            if(posSep==std::string::npos) { posSep = strRA.find(":"); }
            if(posSep==std::string::npos) { posSep = strRA.find("m"); }
            if(posSep==std::string::npos) { std::cout << "radec2degree: Error! Right Acent minute value is invalid!" << std::endl; return -1; }
            strRA_min = strRA.substr(0,posSep);
            strRA = strRA.substr(posSep+1,strRA.length()-2);
            // analyze Dec min
            posSep = std::string::npos;
            if(posSep==std::string::npos) { posSep = strDec.find(":"); }
            if(posSep==std::string::npos) { posSep = strDec.find("m"); }
            if(posSep==std::string::npos) { std::cout << "radec2degree: Error! Declination minute value is invalid!" << std::endl; return -1; }
            strDec_min = strDec.substr(0,posSep);
            strDec = strDec.substr(posSep+1,strDec.length()-2);
            // debug RA min and Dec min
            if(debug>0) {
                std::cout << strRA_min << std::endl;
                std::cout << strDec_min << std::endl;
                //std::cout << strRA << std::endl;
                //std::cout << strDec << std::endl;
            }
            // analyze RA sec
            posSep = strRA.find("s");
            if(posSep!=std::string::npos) { strRA_sec = strRA.substr(0,posSep); } else { strRA_sec = strRA; }
            // analyze Dec sec
            posSep = strDec.find("s");
            if(posSep!=std::string::npos) { strDec_sec = strDec.substr(0,posSep); } else { strDec_sec = strDec; }
            // debug RA sec and Dec sec
            if(debug>0) {
                std::cout << strRA_sec << std::endl;
                std::cout << strDec_sec << std::endl;
                //std::cout << strRA << std::endl;
                //std::cout << strDec << std::endl;
            }
            // now convert from hour:min:sec,degree:min:sec to degree,degree
            double dbRA_sign = atof(strRA_sign.c_str());
            double dbRA_hour = atof(strRA_hour.c_str());
            double dbRA_min = atof(strRA_min.c_str());
            double dbRA_sec = atof(strRA_sec.c_str());
            double dbDec_sign = atof(strDec_sign.c_str());
            double dbDec_deg = atof(strDec_deg.c_str());
            double dbDec_min = atof(strDec_min.c_str());
            double dbDec_sec = atof(strDec_sec.c_str());
            double dbRA_degrees = dbRA_sign * ( (dbRA_sec/60.0 + dbRA_min)/60.0 + (dbRA_hour) ) / 24.0 * 360.0;
            double dbDec_degrees = dbDec_sign * ( (dbDec_sec/60.0 + dbDec_min)/60.0 + (dbDec_deg) );
            std::cout << std::fixed << std::setprecision(10) << dbRA_degrees << " " << std::fixed << setprecision(10) << dbDec_degrees << std::endl;
            
        } else {
            std::cout << "radec2degree: Error! Input RA Dec has invalid format!" << std::endl;
        }
        
    } else {
        std::cout << "radec2degree +10:00:57.132 -02:20:11.83" << std::endl;
    }
    
    return 0;
}


