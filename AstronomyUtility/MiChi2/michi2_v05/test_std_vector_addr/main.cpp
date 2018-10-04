#include <stdio.h>
#include <string.h>
#include <vector>
#include <algorithm>    // std::transform
#include <string>
#include <iostream>     // std::cout, std::endl
#include <iomanip>      // std::setw
#include <ctime>

using namespace std;



int main(int argc, char **argv)
{
    if(argc>0)
    {
        
        // test std::vector
        std::vector<double> t1;
        //t1.reserve(3);
        t1.push_back(1.0); t1.push_back(2.0); t1.push_back(3.0);
        std::cout << std::hex << "0x" << (size_t)(t1.data()) << std::endl;
        
        t1[2] = 99.0;
        std::cout << std::hex << "0x" << (size_t)(t1.data()) << std::endl;
        
        std::vector<double> t2;
        t2.push_back(7.0); t2.push_back(8.0); t2.push_back(9.0);
        t1 = t2;
        std::cout << std::hex << "0x" << (size_t)(t1.data()) << std::endl;
        
        
        
    }
    
    return 0;
}
            

