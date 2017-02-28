/*
 last update:
     2015-06-15 add argument BreakupMark
     2015-06-15 create this branch -- to read topcat "text" format data table -- which is just the MySQL format table
 
 compile:
     clang++ main.cpp -o rfile_sep
 
 test:
     ./rfile_sep catalog_separated.txt YERR_GALFIT_CHEB_B
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
    std::string strColNumb = "";
    std::string strComment = "#";
    std::string strBreakup = "|";
    for (int i=0; i<argc; ++i)
    {
        if(i==1) {strFileUrl = argv[i];}
        if(i==2) {strColName = argv[i];}
        if(i==3) {strColNumb = argv[i];}
        if(i==4) {strComment = argv[i];}
        if(i==5) {strBreakup = argv[i];}
    }
    int debug = 0;
    if(debug){
        std::cout << "debug: strColNumb=\"" << strColNumb <<"\"" << std::endl;
        std::cout << "debug: strComment=\"" << strComment <<"\"" << std::endl;
        std::cout << "debug: strBreakup=\"" << strBreakup <<"\"" << std::endl;
    }
    if(strFileUrl.length()>0 && strColName.length()>0)
    {
        int ColumnNumber = 0;
        if(atoi(strColNumb.c_str())>0){ColumnNumber=atoi(strColNumb.c_str());}
        std::vector<std::string> ColumnContents = CrabTableReadColumn(strFileUrl.c_str(),strColName.c_str(),&ColumnNumber,strComment.c_str(),strBreakup.c_str()); // test to read column as input
        for(int i=0; i<ColumnNumber; i++) {
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
        std::cout << "Usage: " << std::endl;
        std::cout << "    rfile_sep FilePath ColumnName [ColumnNumber [CommentMark [BreakupMark]]]" << std::endl << std::endl;
        std::cout << "Original version: " << std::endl;
        std::cout << "    CrabTableReadColumn FilePath ColumnName [ColumnNumber [CommentMark [BreakupMark]]]" << std::endl << std::endl;
        std::cout << "Last update: " << std::endl;
        std::cout << "    2015-06-15 Gif-sur-Yvette" << std::endl << std::endl;
    }
    return 0;
}
