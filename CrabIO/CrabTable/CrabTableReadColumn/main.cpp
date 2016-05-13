/*
 last update:
     2015-06-15 add argument BreakupMark
     2016-05-13 changed all "ColumnNumber" to "OutputRowNumber", all "strColNumb" to "strRowNumb", now input can be either "ColName" or "ColNumb"
 
 compile:
     clang++ main.cpp -o CrabTableReadColumn_linux_x86_64
     clang++ main.cpp -o CrabTableReadColumn_mac
 
 test:
     ./CrabTableReadColumn_mac catalog_separated.txt YERR_GALFIT_CHEB_B
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CrabTableReadColumn.cpp"

using namespace std;

int main(int argc, char **argv)
{
    /**
    // You’ll notice that argv[0] is the path and name of the program itself.
    std::cout << "CrabTableReadInfo: Input " << argc-1 << " arguments." << std::endl;
     **/
    std::string strFileUrl = "";
    std::string strColName = "";
    std::string strRowNumb = "#";
    std::string strComment = "#";
    std::string strBreakup = " ";
    for (int i=0; i<argc; ++i)
    {
        if(i==1) {strFileUrl = argv[i];}
        if(i==2) {strColName = argv[i];}
        if(i==3) {strRowNumb = argv[i];}
        if(i==4) {strComment = argv[i];}
        if(i==5) {strBreakup = argv[i];}
    }
    int debug = 0;
    if(debug){
        std::cout << "debug: strRowNumb=\"" << strRowNumb <<"\"" << std::endl;
        std::cout << "debug: strComment=\"" << strComment <<"\"" << std::endl;
        std::cout << "debug: strBreakup=\"" << strBreakup <<"\"" << std::endl;
    }
    if(strFileUrl.length()>0 && strColName.length()>0)
    {
        int OutputRowNumber = 0;
        if(atoi(strRowNumb.c_str())>0){OutputRowNumber=atoi(strRowNumb.c_str());}
        
        std::vector<std::string> ColumnContents;
        if(atoi(strColName.c_str())>0){
            ColumnContents = CrabTableReadColumn(strFileUrl.c_str(),atoi(strColName.c_str()),&OutputRowNumber,strComment.c_str(),strBreakup.c_str()); 
        } else {
            ColumnContents = CrabTableReadColumn(strFileUrl.c_str(),strColName.c_str(),&OutputRowNumber,strComment.c_str(),strBreakup.c_str()); 
        }
        for(int i=0; i<OutputRowNumber; i++) {
            std::cout << ColumnContents[i] << std::endl;
        }
        // std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
        // CrabTableReadColumn(argv[1],argv[2]); // test to read column as input
        // std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
        // CrabTableReadColumn(argv[1],3); // test to read column 3
        // std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
        // CrabTableReadColumn(argv[1],12); // test to read column 12
        // std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
        // CrabTableReadColumn(argv[1],30); // test to read column 30
    } else {
        // std::cout << "Usage: " << std::endl << "    CrabTableReadColumn FilePath ColName [ColumnNumber [CommentMark]]" << std::endl << std::endl << "Last update: " << std::endl << "    2014-12-17 Gif-sur-Yvette" << std::endl << std::endl;
        // std::cout << "Usage: " << std::endl << "    CrabTableReadColumn FilePath ColName [ColumnNumber [CommentMark [BreakupMark]]]" << std::endl << std::endl << "Last update: " << std::endl << "    2015-06-15 Gif-sur-Yvette" << std::endl << std::endl;
        std::cout << "Usage: " << std::endl << "    CrabTableReadColumn FilePath ColNameOrColNumb [OutputRowNumber [CommentMark [BreakupMark]]]" << std::endl << std::endl << "Last update: " << std::endl << "    2016-05-13 Nanjing" << std::endl << std::endl;
    }
    return 0;
}
