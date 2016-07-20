/*
 compile:
     g++ -pthread main.cpp -o michi2_v02_test
     g++ -pthread main.cpp -o michi2_v02_201507
     g++ -pthread main.cpp -o michi2_v02_201509
     g++ -pthread main.cpp -o michi2_v02_201509_DL07
 deploy:
     \cp a.out ../../06_LVG_Synthesis/03_Minimize_Chi2/michi2_v02
 test: 
     ./michi2_v02 -obs flux-co-ngc1068.dat -lib lib.lvg lib.lvg -out out.csv
*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "michi2_v02.cpp"

const std::string currentDateTime();

const char *InfoRedshift = "";

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
        // michi2(argv[1],argv[2]);
        // michi2("flux-co.dat","lib.lvg");
        // m2chi2("flux-co.dat","lib.lvg","lib.lvg");
        // m2chi2("flux-co.dat","flower.lvg","flower.lvg");
        const char *FileObs = "flux-co.dat";
        const char *FileLib = "";
        const char *FileLi2 = "";
        const char *FileLi3 = "";
        const char *FileLi4 = "";
        const char *FileOut = "michi2-out.csv";
        InfoRedshift = "";
        for(int i=0; i<argc; i++) {
            if(0==strncmp(argv[i],"-obs",4)) {
                for(int j=i+1; j<i+2; j++) { if(0!=strncmp(argv[j],"-",1)) { FileObs=argv[j]; break; } else { break; } }
            }
            if(0==strncmp(argv[i],"-lib",4)) {
                for(int j=i+1; j<i+8; j++) {
                    if(0!=strncmp(argv[j],"-",1)) {
                        if(0==strlen(FileLib)) { FileLib=argv[j]; continue; }
                        if(0==strlen(FileLi2)) { FileLi2=argv[j]; continue; }
                        if(0==strlen(FileLi3)) { FileLi3=argv[j]; continue; }
                        if(0==strlen(FileLi4)) { FileLi4=argv[j]; continue; }
                    } else { break; }
                }
            }
            if(0==strncmp(argv[i],"-out",4)) {
                for(int j=i+1; j<i+2; j++) { if(0!=strncmp(argv[j],"-",1)) { FileOut=argv[j]; break; } else { break; } }
            }
            if(0==strncmp(argv[i],"-redshift",9) || 0==strcmp(argv[i],"-z")) {
                for(int j=i+1; j<i+2; j++) { if(strspn(argv[j],"+-.eE0123456789")==strlen(argv[j])) { InfoRedshift=argv[j]; break; } else { break; } }
            }
        }
        std::cout << "Please make sure that you already have these files under current directory:" << std::endl;
        std::cout << "\t" << FileObs << "\n\t" << FileLib;
        if(0<strlen(FileLi2)){std::cout << "\n\t" << FileLi2;}
        if(0<strlen(FileLi3)){std::cout << "\n\t" << FileLi3;}
        if(0<strlen(FileLi4)){std::cout << "\n\t" << FileLi4;}
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << "We will output to:" << std::endl << "\t" << FileOut << std::endl;
        std::cout << std::endl;
        std::cout << "OK, let's go! Begin at " << currentDateTime() << std::endl;
        std::cout << std::endl;
        
        if(0==strlen(FileLi4)) {
            if(0==strlen(FileLi3)) {
                if(0==strlen(FileLi2)) {
                    michi2(FileObs,FileLib,FileOut);
                } else {
                    m2chi2(FileObs,FileLib,FileLi2,FileOut);
                }
            } else {
                m3chi2(FileObs,FileLib,FileLi2,FileLi3,FileOut);
            }
        } else {
            m4chi2(FileObs,FileLib,FileLi2,FileLi3,FileLi4,FileOut);
        }
        
//        if(argc>=2) {
//            michi2("flux-co.dat","lib.lvg",argv[1]);
//            // <TODO> // m2chi2("flux-co.dat","lib.lvg","lib.lvg",argv[1]);
//        } else {
//            michi2("flux-co.dat","lib.lvg");
//            // <TODO> // m2chi2("flux-co.dat","lib.lvg","lib.lvg");
//        }
        
        std::cout << std::endl;
        std::cout << "Finally! End at " << currentDateTime() << std::endl;
        std::cout << std::endl;
    } else
    {
        
        std::cout << std::endl;
        std::cout << "Usage: \n\t michi2_v02 -obs flux-co.dat -lib lib.lvg -out output.csv" << std::endl;
        std::cout << "       \n\t michi2_v02 -obs flux-co.dat -lib lib.lvg lib.lvg -out output.csv" << std::endl;
        std::cout << "       \n\t michi2_v02 -obs flux-co.dat -lib Star.SED DL07Lo.SED DL07Hi.SED -out output.csv" << std::endl;
        // std::cout << "Version: \n\t michi2_v02 " << "2014-08-22 Orme des Merisiers" << " copyleft " << std::endl;
        std::cout << "Version: \n\t michi2_v02 " << "2015-04-09 Orme des Merisiers" << std::endl;
        std::cout << std::endl;
        
        /*
         
        std::cout << "michi2" << std::endl;
        std::cout << "  aim: this small code computes the chi-square between observed dataset and library dataset. Datasets must contain two independent variables, the first Var1 is used to match the obs and lib, the second Var2 is used to calculated chi-square. " << std::endl;
        std::cout << "  use: michi2 -obs \"flux-co.dat\" -lib \"flower.lvg\"" << std::endl;
        std::cout << "  use: m2chi2 -obs \"flux-co.dat\" -lib \"flower.lvg\" \"flower.lvg\"" << std::endl;
        std::cout << std::endl;
         
         */
    }
    return 0;
}



// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
// http://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d %X %Z", &tstruct);
    return buf;
}

