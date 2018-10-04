/*
 compile:
     g++ -pthread main.cpp -o michi2_v05
     clang++ -std=c++11 -pthread main.cpp -o michi2_v05
 
 test:
     ./michi2_v05 \
                  -obs flux-SED.dat \
                  -lib lib.lvg lib.lvg lib.lvg \
                  -out out.out \
                  -constraint LIB1 INDEX EQ LIB2 INDEX \
                  -constraint LIB3 NORM EQ LIB2 LIR
*/

#include <stdio.h>
#include <string.h>
#include <vector>
#include <algorithm>    // std::transform
#include <string>
#include <iostream>     // std::cout, std::endl
#include <iomanip>      // std::setw
#include <sstream>      // std::istringstream
#include <sys/types.h>  // mkdir
#include <sys/stat.h>   // stat, S_ISDIR, mkdir
//#include <time.h>
#include "michi2_v05.h" // Constraints, Sampling, NumbParallel
#include "currentdatetime.cpp"
extern const std::string currentDateTime();

using namespace std;





/*
 * some useful subroutines
 */

bool is_file_exist(const char *fileName)
{
    // check file existence
    // from https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
    std::ifstream infile(fileName);
    return infile.good();
}

bool is_dir_exist(const char *DirPath)
{
    // check folder existence
    // see https://stackoverflow.com/questions/12510874/how-can-i-check-if-a-directory-exists
    struct stat s = {0};
    if(0==stat(DirPath, &s) && S_ISDIR(s.st_mode)) {return true;}
    return false;
}

const char kPathSeparator =
#ifdef _WIN32
'\\';
#else
'/';
#endif

void check_folder_existence(const char* path)
{
    std::string sStr(path);
    size_t iPos = sStr.find_last_of(kPathSeparator);
    if(iPos==std::string::npos) {
        return; // no folder in path
    }
    std::istringstream sStream(path);
    std::string s("");
    std::string sDir("");
    while(getline(sStream, s, kPathSeparator)) {
        if(sDir.empty()) {sDir = s;} else {sDir += kPathSeparator; sDir += s;}
        if(!is_dir_exist(sDir.c_str())) {
            if(0!=mkdir(sDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {
                std::cout << "Error! Failed to create the output directory \"" << sDir << "\"! Exit!" << std::endl;
                exit (EXIT_FAILURE);
            }
        }
    }
    return;
}

















/*
 * main()
 */

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
        std::vector<std::string> FileObsList; // Obs Flux & FlueErr List
        std::vector<std::string> FileLibList; // Lib
        std::vector<std::string> FileOutList; // Out
        std::vector<std::string> InputFilterCurveList; // Input Filter Curve List
        std::vector<std::string> InputConstraintList; // Input Constraint List
        std::vector<double> InputRedshiftList; // Input Redshift List
        double InputSampling;
        int DebugLevel = 0; // DebugLevel
        //Constraints.clear();
        std::cout << "Welcome!" << std::endl << std::endl;
        int i=0;
        enum class argMode {obs, lib, out, filter, redshift, parallel, constraint, sampling, none};
        argMode arg_mode = argMode::none;
        while(i<argc) {
            if(0==strncmp(argv[i],"-obs",4)) {
                arg_mode = argMode::obs; i++;
            }
            else if(0==strncmp(argv[i],"-lib",4)) {
                arg_mode = argMode::lib; i++;
            }
            else if(0==strncmp(argv[i],"-out",4)) {
                arg_mode = argMode::out; i++;
            }
            else if(0==strncmp(argv[i],"-filter",7)) {
                arg_mode = argMode::filter; i++;
            }
            else if(0==strncmp(argv[i],"-redshift",9) || 0==strcmp(argv[i],"-z")) {
                arg_mode = argMode::redshift; i++;
            }
            else if(0==strncmp(argv[i],"-parallel",9) || 0==strcmp(argv[i],"-p")) {
                arg_mode = argMode::parallel; i++;
            }
            else if(0==strncmp(argv[i],"-constrain",10)) {
                arg_mode = argMode::constraint; InputConstraintList.push_back(std::string("")); i++;
            }
            else if(0==strncmp(argv[i],"-sampling",9)) {
                arg_mode = argMode::sampling; i++;
            }
            else if(0==strcmp(argv[i],"-debug")) {
                arg_mode = argMode::none; i++; DebugLevel++;
            }
            else {
                switch (arg_mode) {
                    case argMode::obs:
                        if(is_file_exist(argv[i])) {
                            FileObsList.push_back(argv[i]);
                        } else { std::cout << "Error! The input observation data \"" << argv[i] << "\" does not exist!" << std::endl; exit (EXIT_FAILURE); }
                        break;
                    case argMode::lib:
                        if(is_file_exist(argv[i])) {
                            FileLibList.push_back(argv[i]);
                        } else { std::cout << "Error! The input library data \"" << argv[i] << "\" does not exist!" << std::endl; exit (EXIT_FAILURE); }
                        break;
                    case argMode::out:
                        check_folder_existence(argv[i]);
                        FileOutList.push_back(argv[i]);
                        break;
                    case argMode::filter:
                        if(is_file_exist(argv[i])) {
                            InputFilterCurveList.push_back(argv[i]);
                        } else { std::cout << "Error! The input filter curve data \"" << argv[i] << "\" does not exist!" << std::endl; exit (EXIT_FAILURE); }
                        break;
                    case argMode::redshift:
                        if(strspn(argv[i],"+-.eE0123456789")==strlen(argv[i])) {
                            //InfoRedshift = argv[i];
                            InputRedshiftList.push_back(std::stod(std::string(argv[i])));
                        } else { std::cout << "Error! The input redshift " << argv[i] << " is invalid!" << std::endl; exit (EXIT_FAILURE); }
                        break;
                    case argMode::parallel:
                        if(strspn(argv[i],"0123456789")==strlen(argv[i])) {
                            NumbParallel = atoi(argv[i]);
                        } else { std::cout << "Error! The input parallel number " << argv[i] << " is invalid!" << std::endl; exit (EXIT_FAILURE); }
                        break;
                    case argMode::constraint:
                        // (obsolete)
                        // a valid constraint requires 5 arguments:
                        // LIB1 PAR1 OPERATOR LIB2 PAR2
                        // for example -constraint LIB3-DL07 PAR1-UMIN EQ LIB4-DL07 PAR1-UMIN
                        // (new)
                        // the constraint now accept a string with very large flexibility
                        //michi2Constraint *TempConstraint = new michi2Constraint(argv[i+1], argv[i+2], argv[i+3], argv[i+4], argv[i+5]);
                        //Constraints.push_back(TempConstraint);
                        InputConstraintList.back() += argv[i];
                        InputConstraintList.back() += " ";
                        break;
                    case argMode::sampling:
                        InputSampling = std::stod(std::string(argv[i]));
                        Sampling = InputSampling;
                        break;
                    case argMode::none:
                        break;
                    default:
                        arg_mode = argMode::none;
                        break;
                }
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
            std::cout << "\t" << FileOutList[iOut] << std::endl;
        }
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << "We will apply following filter curves:" << std::endl;
        for(int iFil = 0; iFil < InputFilterCurveList.size(); iFil++) {
            std::cout << "\t" << InputFilterCurveList[iFil] << std::endl;
        }
        std::cout << std::endl;
        std::cout << "We will apply following constraints:" << std::endl;
        for(int iCon = 0; iCon < InputConstraintList.size(); iCon++) {
            if(!InputConstraintList.empty()) {
                std::cout << "\t" << InputConstraintList[iCon] << std::endl;
                //michi2Constraint *TempConstraint = new michi2Constraint(InputConstraintList[iCon]);
                //Constraints.push_back();
            }
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

        //
        mnchi2(FileObsList, FileLibList, FileOutList, InputRedshiftList, InputConstraintList, InputFilterCurveList, DebugLevel);
        //
        // TODO: InputSampling
        //
        
        std::cout << std::endl;
        std::cout << "Finally! End at " << currentDateTime() << std::endl;
        std::cout << std::endl;
    } else
    {
        
        std::cout << std::endl;
        std::cout << "Usage: \n";
        std::cout << "       michi2_v05 -obs flux-co.dat -lib lib.lvg -out output.csv\n";
        std::cout << "       \n";
        std::cout << "       michi2_v05 -obs flux-co.dat -lib lib.lvg lib.lvg -out output.csv\n";
        std::cout << "       \n";
        std::cout << "       michi2_v05 -obs flux-co.dat -lib Star.SED DL07Lo.SED DL07Hi.SED -out output.csv\n";
        std::cout << "       \n";
        std::cout << "       michi2_v05 -obs flux-obsframe.dat \\\n";
        std::cout << "                  -redshift 6.3 \\\n";
        std::cout << "                  -lib Star.SED AGN.SED DL07Hi.SED DL07Lo.SED Radio.SED \\\n";
        std::cout << "                  -out output.dat \\\n";
        std::cout << "                  -constraint LIB3 INDEX EQ LIB4 INDEX \\\n";
        std::cout << "                  -constraint \"LIB3_INDEX = LIB4_INDEX\" \\\n";
        std::cout << "                  -constraint \"LIB1_PAR1 >= 0.2\" \\\n";
        std::cout << "                  -constraint \"LIB3_PAR1 >= 1.0\" \\\n";
        std::cout << "                  -constraint \"LIB5_NORM := ((LIB3_NORM*(10^LIB3_PAR3))+(LIB4_NORM*(10^LIB4_PAR3)))*40.31970/3750/10^2.4\" \\\n";
        std::cout << "                  -filter filter.list\n";
        std::cout << "                  -sampling 0.01\n";
        std::cout << "                  # (*) The option \"-constraint\" provides powerful control on what models to fit.\n";
        std::cout << "                  #   - The first constraint means that we lock the index of DL07Hi.SED and DL07Lo.SED\n";
        std::cout << "                  #       to be the same, i.e. same Umin, qPAH, etc.\n";
        std::cout << "                  #   - The second constraint is the same as the first one but in a more modern format\n";
        std::cout << "                  #       which is using exprtk package and is recommended to use.\n";
        std::cout << "                  #   - The third constraint means that we limit the 1st parameter of 1st library,\n";
        std::cout << "                  #       i.e., EBV in Star.SED, to be greater than 0.2.\n";
        std::cout << "                  #   - The fourth constraint means that we limit the 1st parameter of 3rd library,\n";
        std::cout << "                  #       i.e, Umin in DL07Hi.SED, to be greater than 1.0. Because LIB3 INDEX is\n";
        std::cout << "                  #       locked to LIB4 INDEX, this means that both libraries will have their\n";
        std::cout << "                  #       PAR1 beging constrained by this rule.\n";
        std::cout << "                  #   - The fifth constraint means that we lock the normalization of Radio.SED\n";
        std::cout << "                  #       to LIB3+LIB4 IR luminosity following the IR-radio correlation,\n";
        std::cout << "                  #       i.e., q_IR = 2.4, and S_(1.4GHz)/S_(IR,8-1000) = 1.0/3750/10^q_IR,\n";
        std::cout << "                  #       and PAR3 is log10(L_IR/(4*pi*dL^2)) for each model in each LIB3\n";
        std::cout << "                  #       and LIB4 and is pre-computed and recorded in the LIB file.\n";
        std::cout << "                  #       LIB3_NORM and LIB4_NORM will be the fitted normalization of.\n";
        std::cout << "                  #       LIB3 and LIB4, and will be updated during each fitting.\n";
        std::cout << "                  #       Therefore (NORM * 10^PAR3) represents the on-the-fly\n";
        std::cout << "                  #       fitted IR luminosity.\n";
        std::cout << "                  # (*) We can also read filter curves according to the input \"filter.list\" file,\n";
        std::cout << "                  #     which should have two columns, wavelength and filter curve file path without white space,\n";
        std::cout << "                  #     and rows should exactly correspond to \"flux-obsframe.dat\".\n";
        std::cout << "                  #     For wavelength without applicable filter curve, just put \"none\" in the second column.\n";
        std::cout << "                  #     The filter curve file path should refer to a two-column ASCII file which contains\n";
        std::cout << "                  #     obs-frame wavelength and filter transmission value normalized to 1.\n";
        std::cout << "                  # (*) We can also specify the sampling fraction by the \"-sampling\" option.\n";
        std::cout << "                  #     Once given, we will run only this fraction of all model combinations.\n";
        std::cout << "                  #     But do not worry too much about missing important points in the chi-square\n";
        std::cout << "                  #     space, because our code will shuffle around minimum chi-square points\n";
        std::cout << "                  #     to best sample the chi-square space for each parameter in each library.\n";
        std::cout << "       \n";
        std::cout << "Version: \n";
        std::cout << "         michi2_v05 " << "2018-10-XX Heidelberg" << std::endl;
        std::cout << std::endl;
        
    }
    return 0;
}


































