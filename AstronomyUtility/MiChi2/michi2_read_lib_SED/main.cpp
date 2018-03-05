/*
 compile:
     g++ main.cpp -o michi2_chi2_analyzer
 test:
     ./michi2_read_lib_SED lib.SED 1001 245.023
*/

#include <stdio.h>
#include <string.h>
#include <vector>
#include <algorithm>    // std::transform
#include <string>
#include <iostream>     // std::cout, std::endl
#include <iomanip>      // std::setw
#include <fstream>
#include <cmath>        // std::isnan
#include <time.h>
#include "../michi2_v04/CrabStringReadInfo.cpp"
#include "../michi2_v04/CrabTableReadColumn.cpp"
#include "../michi2_v04/integrate.cpp"
#include "../michi2_v04/michi2_DataClass.cpp"


//extern std::string CrabTableReadInfo(const char *InputFile, const char *KeyName, const char *CommentMark);



int main(int argc, char **argv)
{
    /**
    // You’ll notice that argv[0] is the path and name of the program itself.
    std::cout << "CrabTableReadInfo: Input " << argc-1 << " arguments." << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::cout << argv[i] << std::endl;
    }
    **/
    if(argc>2)
    {
        
        std::string michi2_lib_file(argv[1]);
        //std::cout << "# michi2_lib_file" << " = " << michi2_lib_file << std::endl;
        
        long michi2_lib_index = std::atol(argv[2]);
        //std::cout << "# michi2_lib_index" << " = " << michi2_lib_index << std::endl;
        
        double michi2_lib_norma = std::nan("");
        if(argc>3) {
            michi2_lib_norma = (double)std::atof(argv[3]); // normalization
            //std::cout << "# michi2_lib_norma" << " = " << michi2_lib_norma << std::endl;
        }
        
        std::string michi2_out_SED_file;
        std::string michi2_out_LIR_file;
        if(argc>4) {
            michi2_out_SED_file = std::string(argv[4]); // output file name
            michi2_out_LIR_file = michi2_out_SED_file; // output file name
            michi2_out_LIR_file.append(".vLv_8_1000");
        }
        
        int i=0;
        
        int verbose = 0;
        michi2DataClass *SDLIB = new michi2DataClass(michi2_lib_file.c_str(), verbose);
        
        if(SDLIB) {
            std::vector<std::string> Data_block = SDLIB->getDataBlockQuickQuick(michi2_lib_index + 1, SDLIB->NVAR[0], verbose); // michi2_lib_index is index starting from 0, while getDataBlock() function needs input line number starting from 1.
            
            if(!michi2_out_SED_file.empty()) {
                std::ofstream michi2_out_SED_fstream(michi2_out_SED_file.c_str(), std::ofstream::out);
                michi2_out_SED_fstream << "# michi2_lib_file" << " = " << michi2_lib_file << std::endl;
                michi2_out_SED_fstream << "# michi2_lib_index" << " = " << michi2_lib_index << std::endl;
                michi2_out_SED_fstream << "# michi2_lib_norma" << " = " << michi2_lib_norma << std::endl;
                michi2_out_SED_fstream << "#" << std::setw(25-1) << "X" << " " << std::setw(25) << "Y" << std::endl;
                michi2_out_SED_fstream << std::setprecision(10) << std::scientific;
                for(i=0; i<SDLIB->X.size(); i++) {
                    michi2_out_SED_fstream << std::setw(25) << SDLIB->X[i] << " " << std::setw(25) << SDLIB->Y[i] * michi2_lib_norma << std::endl;
                }
                michi2_out_SED_fstream.close();
            } else {
                for(i=0; i<Data_block.size(); i++) {
                    std::cout << Data_block[i] << std::endl;
                    /*
                    if(std::isnan(michi2_lib_norma)) {
                        std::cout << std::setw(20) << std::setprecision(10) << std::scientific << SDLIB->X[i] << " " << std::setw(20) << SDLIB->Y[i] << std::endl;
                    } else {
                        std::cout << std::setw(20) << std::setprecision(10) << std::scientific << SDLIB->X[i] << " " << std::setw(20) << SDLIB->Y[i] * michi2_lib_norma << std::endl;
                    }
                    */
                }
            }
            
            
            if(!michi2_out_LIR_file.empty()) {
                
                std::vector<double> wIR; wIR.push_back(8.0); wIR.push_back(1000.0);
                double LIR = integrate_LIR(SDLIB->X, SDLIB->Y, wIR);
                
                std::ofstream michi2_out_LIR_fstream(michi2_out_LIR_file.c_str(), std::ofstream::out);
                michi2_out_LIR_fstream << "# michi2_lib_file" << " = " << michi2_lib_file << std::endl;
                michi2_out_LIR_fstream << "# michi2_lib_index" << " = " << michi2_lib_index << std::endl;
                michi2_out_LIR_fstream << "# michi2_lib_norma" << " = " << michi2_lib_norma << std::endl;
                michi2_out_LIR_fstream << std::setprecision(10) << std::scientific << LIR << std::endl;
                michi2_out_LIR_fstream.close();
                
                std::cout << "Output to \"" << michi2_out_SED_file << "\"" << " and \"" << michi2_out_LIR_file << "\"!" << std::endl;
                
            } else if(!michi2_out_SED_file.empty()) {
                
                std::cout << "Output to \"" << michi2_out_SED_file << "\"!" << std::endl;
            }
            
        }
        
    } else
    {
        
        std::cout << std::endl;
        std::cout << "Usage: \n";
        std::cout << "       michi2_read_lib_SED lib.SED 1001 123.456 output_SED.txt\n";
        std::cout << "                           [file] [index] [norm_factor] [out_file]\n";
        std::cout << "       \n";
        std::cout << std::endl;
        
    }
    return 0;
}

