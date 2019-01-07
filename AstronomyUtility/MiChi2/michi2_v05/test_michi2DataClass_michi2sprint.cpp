#include <stdio.h>
#include <iostream>
#include <iomanip> // std::setfill, std::setw
#include <cstdlib>
#include <string>
#include <vector>
#include <clocale> // <20160725><dzliu> modified from <locale> to <clocale> for mac.
#include <wchar.h>

// g++ -std=c++11 test_michi2DataClass_michi2sprint.cpp -o test_michi2DataClass_michi2sprint

using namespace std;

const char *michi2sprint(const char* wsPrefix, long wi, const char* wsSuffix);

const char *michi2sprint(const char* wsPrefix, long wi, const char* wsSuffix)
{    std::string input_str_1 = wsPrefix;
    std::string input_str_2 = std::to_string(wi); // C++11
    std::string input_str_3 = wsSuffix;
    std::string output_str = input_str_1 + input_str_2 + input_str_3;
    return output_str.c_str();
}


int main()
{
    int i=5;
    printf(michi2sprint("# TPAR",i,""));
    printf("\n");
    return 0;
}


