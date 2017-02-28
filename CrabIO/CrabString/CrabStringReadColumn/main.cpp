#include <stdio.h>
#include <string>
#include <vector>
#include "../CrabStringReadColumn.cpp"

int main(int argc, char **argv)
{
    /**
    // You’ll notice that argv[0] is the path and name of the program itself.
    std::cout << "CrabTableReadInfo: Input " << argc-1 << " arguments." << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::cout << argv[i] << std::endl;
    }
    **/
    if(argc>=1)
    {
        std::vector<std::wstring> tmpVec;
        tmpVec.push_back(wstring(L"   E_UP "));
        tmpVec.push_back(wstring(L"  123   "));
        tmpVec.push_back(wstring(L"   456  "));
        tmpVec.push_back(wstring(L"    789 "));
        tmpVec.push_back(wstring(L"    1   "));
        tmpVec.push_back(wstring(L"     1  "));
        tmpVec.push_back(wstring(L"      0 "));
        CrabStringReadColumn(tmpVec,"E_UP");
        std::wcout << std::endl << std::endl;
        CrabStringReadColumn(tmpVec,1);
        std::wcout << std::endl << std::endl;
        CrabStringReadColumn(tmpVec,2);
    } else
    {
        std::cout << "CrabStringReadColumn(StringVector,\"E_UP\")" << std::endl;
    }
    return 0;
}