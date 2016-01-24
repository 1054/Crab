/*
 compile:
 clang++ lumdist.cpp -o lumdist
 deploy:
 lumdist
 update:
 2014-10-30
 */

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <time.h>
#include "CrabConstLumDist.cpp"

using namespace std;

const std::string currentDateTime();

int main(int argc, char **argv)
{
    /**
     // You’ll notice that argv[0] is the path and name of the program itself.
     std::cout << "CrabTableReadInfo: Input " << argc-1 << " arguments." << std::endl;
     for (int i = 0; i < argc; ++i) {
     std::cout << argv[i] << std::endl;
     }
     **/
    if(argc>1)
    {
        const char* str_H0 = "";
        const char* str_OM = "";
        const char* str_LM = "";
        const char* str_rz = "";
        int verbose = 0;
        // read the first non -XXX arg as redshift
        for(int i=1; i<argc; i++) {
            if(0<strlen(argv[i]) && 0!=strncmp(argv[i],"-",1)) {
                str_rz = argv[i]; break;
            }
        }
        // read -h0 -omega -lambda
        for(int i=1; i<argc; i++) {
            if(0==strncmp(argv[i],"-h0",3) || 0==strncmp(argv[i],"-hubble",7)) {
                for(int j=i+1; j<i+2; j++) { if(0!=strncmp(argv[j],"-",1)) { str_H0=argv[j]; break; } else { break; } }
            }
            if(0==strncmp(argv[i],"-omega",6) || 0==strncmp(argv[i],"-matter",7)) {
                for(int j=i+1; j<i+2; j++) { if(0!=strncmp(argv[j],"-",1)) { str_OM=argv[j]; break; } else { break; } }
            }
            if(0==strncmp(argv[i],"-lambda",7) || 0==strncmp(argv[i],"-vaccum",7)) {
                for(int j=i+1; j<i+2; j++) { if(0!=strncmp(argv[j],"-",1)) { str_LM=argv[j]; break; } else { break; } }
            }
            if(0==strncmp(argv[i],"-verbose",8)) {
                verbose = 1;
            }
            if(0==strncmp(argv[i],"-simple",8)) {
                verbose = -1;
            }
        }
        // std::cout << "lumdist " << "H0=" << str_H0 << " Omega_M=" << str_OM << " Lambda0=" << str_LM << std::endl;
        
        double var_H0 = 73.0;
        double var_OM = 0.27;
        double var_LM = 0.73;
        double var_rz = atof(str_rz);
        
        if(strlen(str_H0)>0) { var_H0 = atof(str_H0); }
        if(strlen(str_OM)>0) { var_OM = atof(str_OM); }
        if(strlen(str_LM)>0) { var_LM = atof(str_LM); }
        
        if(verbose>=0) {
            std::cout << "lumdist " << var_rz << " H0=" << var_H0 << " Omega_M=" << var_OM << " Lambda0=" << var_LM << std::endl;
        }
        
        double lumdistance = lumdist(var_rz,var_H0,var_OM,var_LM,verbose);
        
        if(verbose>=0) {
            std::cout << "lumdist = " << lumdistance << " Mpc" << std::endl;
        } else {
            std::cout << lumdistance << std::endl;
        }
        
    } else {
        
        std::cout << "Usage: lumdist z [-simple] -h0 73 -matter 0.27 -vaccum 0.73 [-verbose]" << std::endl;
    }
    return 0;
}


