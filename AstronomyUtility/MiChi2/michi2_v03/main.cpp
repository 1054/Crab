/*
 compile:
     g++ -pthread main.cpp -o michi2_v02_test
     g++ -pthread main.cpp -o michi2_v02_201507
     g++ -pthread main.cpp -o michi2_v02_201509
     g++ -pthread main.cpp -o michi2_v02_201509_DL07
     clang++ -std=c++11 -pthread main.cpp -o michi2_v03_201607
     clang++ -std=c++11 -pthread main.cpp -o michi2_v03_20160727
 deploy:
     \cp a.out ../../06_LVG_Synthesis/03_Minimize_Chi2/michi2_v03
 test: 
     ./michi2_v03 -obs flux-co-ngc1068.dat -lib lib.lvg lib.lvg lib.lvg -out out.out
     ./michi2_v03_201607  -obs test/flux-co-ngc1068.dat  -lib test/lib_big_co.lvg test/lib_big_co.lvg   -out test/out.out   -constraint "LIB1" "PAR3" "<" LIB2 par2
     ./michi2_v03_201607  -obs test/flux-co-ngc1068.dat  -lib test/lib_big_co.lvg test/lib_big_co.lvg   -out test/out.out   -constraint LIB1 INDEX EQ LIB2 INDEX
*/

#include <stdio.h>
#include <string.h>
#include <vector>
#include <algorithm>    // std::transform
#include <string>
#include <iostream>     // std::cout, std::endl
#include <iomanip>      // std::setw
#include <time.h>
#include "michi2_Constraint.h"
#include "michi2_v03.cpp"

using namespace std;

const std::string currentDateTime();

std::vector<double> extractStringDouble(std::string InputStr);

const char *InfoRedshift = "";

int NumbParallel = 2; // number of parallel subprocesses

std::vector<ConstraintStructure*> Constraints; // --> ./michi2_v03_201607 -obs test/... terminated by signal SIGSEGV (Address boundary error)



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
        std::vector<std::string> FileObsList;
        std::vector<std::string> FileLibList;
        std::vector<std::string> FileOutList;
        std::cout << "Welcome!" << std::endl << std::endl;
        int i=0;
        while(i<argc) {
            if(0==strncmp(argv[i],"-obs",4))
            {
                i++;
                while(i<argc) {
                    if(0!=strncmp(argv[i],"-",1)) {
                        FileObsList.push_back(argv[i]);
                    } else {break;}
                    i++;
                }
            }
            else if(0==strncmp(argv[i],"-lib",4))
            {
                i++;
                while(i<argc) {
                    if(0!=strncmp(argv[i],"-",1)) {
                        FileLibList.push_back(argv[i]);
                    } else {break;}
                    i++;
                }
            }
            else if(0==strncmp(argv[i],"-lib",4))
            {
                i++;
                while(i<argc) {
                    if(0!=strncmp(argv[i],"-",1)) {
                        FileLibList.push_back(argv[i]);
                    } else {break;}
                    i++;
                }
            }
            else if(0==strncmp(argv[i],"-out",4))
            {
                i++;
                while(i<argc) {
                    if(0!=strncmp(argv[i],"-",1)) {
                        FileOutList.push_back(argv[i]);
                    } else {break;}
                    i++;
                }
            }
            else if(0==strncmp(argv[i],"-redshift",9) || 0==strcmp(argv[i],"-z"))
            {
                i++;
                if(i<argc) {
                    if(strspn(argv[i],"+-.eE0123456789")==strlen(argv[i])) {
                        InfoRedshift = argv[i]; i++;
                    } else {
                        std::cout << "Error! Input redshift is invalid!" << std::endl;
                    }
                }
            }
            else if(0==strncmp(argv[i],"-parallel",9) || 0==strcmp(argv[i],"-p"))
            {
                i++;
                if(i<argc) {
                    if(strspn(argv[i],"0123456789")==strlen(argv[i])) {
                        NumbParallel = atoi(argv[i]); i++;
                    } else {
                        std::cout << "Error! Input parallel is invalid!" << std::endl;
                    }
                }
            }
            else if(0==strncmp(argv[i],"-constrain",10) || 0==strcmp(argv[i],"-s"))
            {
                // i++;
                // a valid constraint requires 5 arguments:
                // LIB1 PAR1 OPERATOR LIB2 PAR2
                // for example -constraint LIB3-DL07 PAR1-UMIN EQ LIB4-DL07 PAR1-UMIN
                if((i+5)<argc) {
                    ConstraintStructure *TempConstraint = new ConstraintStructure();
                    std::string TempConstraintStr;
                    std::string TempConstraintStrCopy;
                    std::cout << "Input Constraint:";
                    for(int j=1; j<=5; j++) {
                        TempConstraintStr = argv[i+j];
                        std::transform(TempConstraintStr.begin(),TempConstraintStr.end(),TempConstraintStr.begin(),::toupper);
                        //std::cout << " " << TempConstraintStr;
                        if(j==1) {TempConstraintStrCopy=TempConstraintStr;} // TempConstraintStrCopy is used as previous container.
                        //
                        // if input "lib3" LIB id, then set constraint from/to LIB 3 (number starting from 1).
                        // if input "full" then set constraint from/to full SED.
                        //
                        // if input "index" then set constraint on index.
                        // if input "integration" then set constraint on integration.
                        // if input "par3" LIB PAR id, then set constraint on LIB PAR.
                        //
                        if (TempConstraintStr=="SED" || (std::string::npos!=TempConstraintStr.find("FULL"))) {
                            // it's FULL SED INTEGRAT mode, set constraint with integration of full SED
                            if(j==4) {TempConstraint->fromLIB = -1;}
                            else if(j==1) {TempConstraint->toLIB = -1;}
                        } else if (TempConstraintStr=="L" || TempConstraintStr=="INT" || (std::string::npos!=TempConstraintStr.find("INTEGRAT"))) {
                            // it's INTEGRAT mode, set constraint with integration of full SED or LIB SED
                            if(j==5) {TempConstraint->fromPAR = -1; TempConstraint->fromFactor = 1.0; TempConstraint->fromLowerX = 0.0; TempConstraint->fromUpperX = 0.0;}
                            else if(j==2) {TempConstraint->toPAR = -1; TempConstraint->toFactor = 1.0; TempConstraint->toLowerX = 0.0; TempConstraint->toUpperX = 0.0;}
                            // now read integration normalization factor if available
                            std::vector<double> TempConstraintDbl = extractStringDouble(TempConstraintStr);
                            if(TempConstraintDbl.size()>=1) {
                                if(j==5) {TempConstraint->fromFactor = TempConstraintDbl[0]; }
                                else if(j==2) {TempConstraint->toFactor = TempConstraintDbl[0]; }
                            } TempConstraintDbl.clear();
                            // now read integration x range if available
                            TempConstraintDbl = extractStringDouble(TempConstraintStrCopy); // if it is INTEGRAT mode, then we also need to read LIB X range
                            if(TempConstraintDbl.size()>=3) {
                                if(j==5) {TempConstraint->fromLowerX = TempConstraintDbl[1]; TempConstraint->fromUpperX = TempConstraintDbl[2];}
                                else if(j==2) {TempConstraint->toLowerX = TempConstraintDbl[1]; TempConstraint->toUpperX = TempConstraintDbl[2];}
                            } TempConstraintDbl.clear();
                        }  else if (TempConstraintStr=="F" || (std::string::npos!=TempConstraintStr.find("FLUX"))) {
                            // it's FLUX mode, set constraint with the flux at X position of full SED or LIB SED
                            if(j==5) {TempConstraint->fromPAR = -2; TempConstraint->fromFactor = 1.0; TempConstraint->fromLowerX = 0.0; TempConstraint->fromUpperX = 0.0;}
                            else if(j==2) {TempConstraint->toPAR = -2; TempConstraint->toFactor = 1.0; TempConstraint->toLowerX = 0.0; TempConstraint->toUpperX = 0.0;}
                            // now read FLUX normalization factor if available
                            std::vector<double> TempConstraintDbl = extractStringDouble(TempConstraintStr);
                            if(TempConstraintDbl.size()>=1) {
                                if(j==5) {TempConstraint->fromFactor = TempConstraintDbl[0]; }
                                else if(j==2) {TempConstraint->toFactor = TempConstraintDbl[0]; }
                            } TempConstraintDbl.clear();
                            // now read FLUX X position if available
                            TempConstraintDbl = extractStringDouble(TempConstraintStrCopy); // if it is FLUX mode, then we also need to read LIB X position
                            if(TempConstraintDbl.size()>=2) {
                                if(j==5) {TempConstraint->fromLowerX = TempConstraintDbl[1]; TempConstraint->fromUpperX = TempConstraintDbl[1];}
                                else if(j==2) {TempConstraint->toLowerX = TempConstraintDbl[1]; TempConstraint->toUpperX = TempConstraintDbl[1];}
                            } TempConstraintDbl.clear();
                        } else if (TempConstraintStr=="I" || (std::string::npos!=TempConstraintStr.find("INDEX"))) {
                            // it's INDEX mode, set constraint on LIB index rather then LIB PAR
                            if(j==5) {TempConstraint->fromPAR = 0;}
                            else if(j==2) {TempConstraint->toPAR = 0;}
                        } else if(std::string::npos != TempConstraintStr.find_first_of("0123456789")) {
                            // it's normal mode, set constraint on some PAR of some LIB
                            TempConstraintStr = TempConstraintStr.substr(TempConstraintStr.find_first_of("0123456789"));
                            if(j==1){TempConstraint->toLIB = std::stoi(TempConstraintStr);}
                            else if(j==2){TempConstraint->toPAR = std::stoi(TempConstraintStr);}
                            else if(j==4){TempConstraint->fromLIB = std::stoi(TempConstraintStr);}
                            else if(j==5){TempConstraint->fromPAR = std::stoi(TempConstraintStr);}
                        } else if(j==3) {
                            if     (std::string::npos != TempConstraintStr.find("=")  ) {TempConstraint->OperatorType=0;  TempConstraint->OperatorTypeStr="EQ";}
                            else if(std::string::npos != TempConstraintStr.find(">=") ) {TempConstraint->OperatorType=1;  TempConstraint->OperatorTypeStr="GE";}
                            else if(std::string::npos != TempConstraintStr.find("<=") ) {TempConstraint->OperatorType=-1; TempConstraint->OperatorTypeStr="LE";}
                            else if(std::string::npos != TempConstraintStr.find(">")  ) {TempConstraint->OperatorType=2;  TempConstraint->OperatorTypeStr="GT";}
                            else if(std::string::npos != TempConstraintStr.find("<")  ) {TempConstraint->OperatorType=-2; TempConstraint->OperatorTypeStr="LT";}
                            else if(std::string::npos != TempConstraintStr.find("EQ") ) {TempConstraint->OperatorType=0;  TempConstraint->OperatorTypeStr="EQ";}
                            else if(std::string::npos != TempConstraintStr.find("GE") ) {TempConstraint->OperatorType=1;  TempConstraint->OperatorTypeStr="GE";}
                            else if(std::string::npos != TempConstraintStr.find("LE") ) {TempConstraint->OperatorType=-1; TempConstraint->OperatorTypeStr="LE";}
                            else if(std::string::npos != TempConstraintStr.find("GT") ) {TempConstraint->OperatorType=2;  TempConstraint->OperatorTypeStr="GT";}
                            else if(std::string::npos != TempConstraintStr.find("LT") ) {TempConstraint->OperatorType=-2; TempConstraint->OperatorTypeStr="LT";}
                            else {
                                std::cout << std::endl;
                                std::cout << "Error! The input constraint operator " << argv[i+j] << " could not be understood!" << std::endl;
                                std::cout << std::endl;
                                return -1;
                            }
                        } else {
                            std::cout << std::endl;
                            std::cout << "Error! The input constraint argument " << argv[i+j] << " could not be understood!" << std::endl;
                            std::cout << std::endl;
                            return -1;
                        }
                        TempConstraintStrCopy = TempConstraintStr;
                    }
                    std::cout << std::endl;
                    std::cout << "\t";
                    //
                    std::cout << "LIB" << TempConstraint->toLIB;
                    if(TempConstraint->toPAR<=0) {std::cout << "[" << TempConstraint->toLowerX << ":" << TempConstraint->toUpperX << "]";}
                    std::cout << " ";
                    if(TempConstraint->toPAR>0) {std::cout << "PAR" << TempConstraint->toPAR << "*" << TempConstraint->fromFactor;}
                    else if(TempConstraint->toPAR==0) {std::cout << "INDEX";}
                    else if(TempConstraint->toPAR==-1) {std::cout << "INTEGRATION" << "*" << TempConstraint->toFactor;}
                    else if(TempConstraint->toPAR==-2) {std::cout << "FLUX" << "*" << TempConstraint->toFactor;}
                    //
                    std::cout << " ";
                    std::cout << " " << TempConstraint->OperatorTypeStr << " ";
                    std::cout << " ";
                    //
                    std::cout << "LIB" << TempConstraint->fromLIB;
                    if(TempConstraint->fromPAR<=0) {std::cout << "[" << TempConstraint->fromLowerX << ":" << TempConstraint->fromUpperX << "]";}
                    std::cout << " ";
                    if(TempConstraint->fromPAR>0) {std::cout << "PAR" << TempConstraint->fromPAR << "*" << TempConstraint->fromFactor;}
                    else if(TempConstraint->fromPAR==0) {std::cout << "INDEX";}
                    else if(TempConstraint->fromPAR==-1) {std::cout << "INTEGRATION" << "*" << TempConstraint->fromFactor;}
                    else if(TempConstraint->fromPAR==-2) {std::cout << "FLUX" << "*" << TempConstraint->fromFactor;}
                    //
                    std::cout << std::endl;
                    std::cout << std::endl;
                    Constraints.push_back(TempConstraint);
                    i+=5;
                } else {
                    std::cout << std::endl;
                    std::cout << "Error! The input constraint should have 5 arguments, e.g. lib3 par3 < lib2 par3." << std::endl;
                    std::cout << "Examples:" << std::endl;
                    std::cout << "    -constraint lib2 par3 eq lib3 par3 # e.g. DL07 cold+warm SED, cold Umin should be equal to warm Umin." << std::endl;
                    std::cout << "    -constraint lib1 par1 lt lib2 par1 # e.g. LVG two-component CO SLED, cold T_kin should be less than warm T_kin." << std::endl;
                    std::cout << "Notes:" << std::endl;
                    std::cout << "    We can set several constraints for example" << std::endl;
                    std::cout << "    -constraint lib1 par1 le lib2 par1 -constraint lib1 par2 le lib2 par2" << std::endl;
                    std::cout << "    This is for LVG two-component CO SLED, both T_kin and n_H2 are set to be smaller in colder component." << std::endl;
                    // <TODO><20160719><dzliu> std::cout << "    -constraint \"lib4[200000]\" \"flux\" eq \"full[8:1000]\" \"integration*0.0005\" # e.g. set radio 200000um flux be the 8-1000 integration times 0.0005." << std::endl;
                    // <TODO><20160719><dzliu>
                    // ITS TOO HARD TO IMPLEMENT SO MANY THINGS
                    // See notes in michi2_v03.cpp
                    //
                    std::cout << std::endl;
                    return -1;
                }
                i++;
            }
            else
            {
                i++;
            }
        }
        std::cout << "Please make sure that you already have these files under current directory:" << std::endl;
        for(int iObs = 0; iObs < FileObsList.size(); iObs++) {
            std::cout << "\t" << FileObsList[iObs];
        }
        std::cout << std::endl;
        for(int iLib = 0; iLib < FileLibList.size(); iLib++) {
            std::cout << "\t" << FileLibList[iLib];
        }
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << "We will output to:" << std::endl;
        for(int iOut = 0; iOut < FileOutList.size(); iOut++) {
            std::cout << "\t" << FileOutList[iOut];
        }
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << "OK, let's go! Begin at " << currentDateTime() << std::endl;
        std::cout << std::endl;
        
        // if(0==strlen(FileLi4)) {
        //     if(0==strlen(FileLi3)) {
        //         if(0==strlen(FileLi2)) {
        //             michi2(FileObs,FileLib,FileOut);
        //         } else {
        //             m2chi2(FileObs,FileLib,FileLi2,FileOut);
        //         }
        //     } else {
        //         m3chi2(FileObs,FileLib,FileLi2,FileLi3,FileOut);
        //     }
        // } else {
        //     m4chi2(FileObs,FileLib,FileLi2,FileLi3,FileLi4,FileOut);
        // }

        //<TODO><UNCOMMNET>//
        mnchi2(FileObsList,FileLibList,FileOutList);
        //<TODO><UNCOMMNET>//
        
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
        std::cout << "Usage: \n\t michi2_v03 -obs flux-co.dat -lib lib.lvg -out output.csv" << std::endl;
        std::cout << "       \n\t michi2_v03 -obs flux-co.dat -lib lib.lvg lib.lvg -out output.csv" << std::endl;
        std::cout << "       \n\t michi2_v03 -obs flux-co.dat -lib Star.SED DL07Lo.SED DL07Hi.SED -out output.csv" << std::endl;
        // std::cout << "Version: \n\t michi2_v03 " << "2014-08-22 Orme des Merisiers" << " copyleft " << std::endl;
        // std::cout << "Version: \n\t michi2_v03 " << "2015-04-09 Orme des Merisiers" << std::endl;
        std::cout << "Version: \n\t michi2_v03 " << "2016-07-14 Nanjing" << std::endl;
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


// Extract double numbers from a given string
// loop to extract all possible double numbers
std::vector<double> extractStringDouble(std::string InputStr) {
    double TempDbl = 0.0;
    std::string TempStr = InputStr+" "; // in case the string got empty
    std::string ExctStr = ""; // extracted str
    std::vector<double> OutputDbl;
    while (std::string::npos != TempStr.find_first_of(".+-Ee0123456789")) {
        TempStr = TempStr.substr(TempStr.find_first_of(".+-Ee0123456789"));
        size_t TempPos = TempStr.find_first_not_of(".+-Ee0123456789");
        if(std::string::npos != TempPos) {
            ExctStr = TempStr.substr(0,TempPos);
            TempStr = TempStr.substr(TempPos);
        } else {
            ExctStr = TempStr;
        }
        if(ExctStr!="") {
            if(0 == ExctStr.find_first_of(".+-0123456789")) {
                //std::cout << "DEBUG: ExctStr = " << ExctStr << std::endl;
                TempDbl = std::stod(ExctStr.c_str());
                OutputDbl.push_back(TempDbl);
            }
            ExctStr="";
        }
    }
    return OutputDbl;
}

