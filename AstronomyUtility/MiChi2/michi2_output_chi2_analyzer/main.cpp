/*
 compile:
     g++ main.cpp -o michi2_chi2_analyzer
 test:
     ./michi2_output_chi2_analyzer out.out
*/

#include <stdio.h>
#include <string.h>
#include <vector>
#include <algorithm>    // std::transform
#include <string>
#include <iostream>     // std::cout, std::endl
#include <iomanip>      // std::setw
#include <time.h>
#include "../michi2_v04/CrabTableReadColumn.cpp"
#include "../michi2_v04/CrabTableReadInfo.cpp"
#include "../michi2_v04/integrate.cpp"


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
    if(argc>1)
    {
        
        std::string michi2_output_file(argv[1]);
        std::string michi2_output_info = michi2_output_file;
        michi2_output_info.append(".info");
        std::cout << "Hello!" << std::endl << std::endl;
        std::cout << "michi2_output_file = " << michi2_output_file << std::endl;
        std::cout << "michi2_output_info = " << michi2_output_info << std::endl;
        
        std::string File_Obs = CrabTableReadInfo(michi2_output_info.c_str(), "OBS");
        std::cout << "OBS" << " = " << File_Obs << std::endl;
        std::string File_Lib;
        std::vector<std::string> File_Libs;
        int i=1;
        while (true) {
            std::string Name_Lib = std::string("LIB") + std::to_string(i);
            File_Lib = CrabTableReadInfo(michi2_output_info.c_str(), Name_Lib.c_str());
            if(File_Lib.empty()) {break;} else {
                std::cout << Name_Lib << " = " << File_Lib << std::endl;
                File_Libs.push_back(File_Lib);
            }
            i++;
        }
        
    } else
    {
        
        std::cout << std::endl;
        std::cout << "Usage: \n";
        std::cout << "       michi2_output_chi2_analyzer fit.out\n";
        std::cout << "       \n";
        std::cout << std::endl;
        
    }
    return 0;
}

