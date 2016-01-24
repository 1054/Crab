#ifndef H_CrabTableReadColumn
#define H_CrabTableReadColumn
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "CrabStringClean.cpp"
#include "CrabStringUnicode.cpp"
#include "CrabStringFindWholeWord.cpp"

using namespace std;



string ctrcstrtrim(const string &t, int trimflag = 0);

long ctrcstrfindwholeword(const string &t, const string &strsearch, const string &BreakupMark = " ");

string ctrcstrfindwholeword(const string &t, size_t pos, const string &BreakupMark = " ");

std::vector<double> ctrcstrtodouble(std::vector<string> strVec);

std::vector<std::string> CrabTableReadColumn(const char *InputFile, const char *InputColHead, int *OutputRowNumber = NULL, const char *CommentMark = "#");

std::vector<std::string> CrabTableReadColumn(const char *InputFile, int InputColNumber, int *OutputRowNumber = NULL, const char *CommentMark = "#");

double *CrabTableReadColumnF(const char *InputFile, int InputColNumber, int *OutputRowNumber = NULL, const char *CommentMark = "#");






string ctrcstrtrim(const string &t, int trimflag)
{
    string str = t;
    string strspace = " "; // trim space
    size_t found;
    // trim leading space
    if(0==trimflag || 2==trimflag) {
        found = str.find_first_not_of(strspace);
        if (found != string::npos) {
            if(found>0) {
                // std::cout << found << std::endl;
                str.erase(0,found);
            }
        }
        else
            str.clear(); // str is all whitespace
    }
    // trim trailing space
    if(0==trimflag || 2==trimflag) {
        found = str.find_last_not_of(strspace);
        if (found != string::npos) {
            if(found<str.length()-1) {
                // std::cout << found << std::endl;
                str.erase(found+1);
            }
        }
        else
            str.clear(); // str is all whitespace
    }
    return str;
}



long ctrcstrfindwholeword(const string &t, const string &strsearch, const string &BreakupMark)
{
    size_t foundpos = 0;
    // std::cout << strsearch << " " << strsearch.length() << " " << t.find(strsearch) << std::endl;
    while(string::npos!=foundpos) {
        foundpos = t.find(strsearch,foundpos);
        // std::cout << t << "\n" << strsearch << "\n" << foundpos << t.substr(foundpos-1,BreakupMark.length()) << t.substr(foundpos+strsearch.length(),BreakupMark.length()) << std::endl;
        // std::cout << (BreakupMark==t.substr(foundpos-1,BreakupMark.length())) << std::endl;
        // std::cout << (BreakupMark==t.substr(foundpos+strsearch.length(),BreakupMark.length())) << std::endl;
        // sleep(1.0);
        // find keyname as a whole word
        if(string::npos!=foundpos) {
            if(BreakupMark==t.substr(foundpos-1,BreakupMark.length()) &&
               BreakupMark==t.substr(foundpos+strsearch.length(),BreakupMark.length())) { break; }
        } else {
            break;
        }
        foundpos++;
    }
    return foundpos;
}


string ctrcstrfindwholeword(const string &t, size_t pos, const string &BreakupMark)
{
    string foundstr;
    if(0==pos) {
        size_t foundposL = 0;
        size_t foundposR = t.find(BreakupMark)-1;
        foundstr = t.substr(foundposL,foundposR-foundposL+1);
    } else if(t.length()-1==pos) {
        size_t foundposL = t.rfind(BreakupMark)+1;
        size_t foundposR = t.length()-1;
        foundstr = t.substr(foundposL,foundposR-foundposL+1);
    } else if(0<pos && t.length()-1>pos) {
        string strL = t.substr(0,pos);
        string strR = t.substr(pos);
        size_t foundposL = strL.rfind(BreakupMark)+1;
        size_t foundposR = strR.find(BreakupMark)-1+pos;
        foundstr = t.substr(foundposL,foundposR-foundposL+1);
    }
    return foundstr;
}


std::vector<double> ctrcstrtodouble(std::vector<string> strVec)
{
    std::vector<double> fVec;
    for(int i=0; i<strVec.size(); i++) {
        std::string str = strVec[i];
        double f = 0.0;
        if(str.length()>0 && str!="*") {
            std::istringstream ssm(str); // stream used for the conversion
            ssm >> f;
        }
        fVec.push_back(f);
    }
    return fVec;
}






std::vector<std::string> CrabTableReadColumn(const char *InputFile, const char *InputColHead, int *OutputRowNumber, const char *CommentMark)
{
    //
    std::vector<std::string> OutputArray; // OutputArray.push_back(superman);
    int debug = 0;
    //
    if(strlen(InputColHead)>0 && strlen(InputFile)>0) {
        std::wstring line;
        std::wstring trline;
        // std::wstring colname(InputColHead); // From char * to string: http://stackoverflow.com/questions/2573834/c-convert-string-or-char-to-string-or-wchar-t
        std::wstring colname = CrabStringWideString(InputColHead); // From utf8 char * to utf16 wstring
        std::wstring colvalue;
        // std::wstring commark(CommentMark); // From char * to string: http://stackoverflow.com/questions/2573834/c-convert-string-or-char-to-string-or-wchar-t
        std::wstring commark = CrabStringWideString(CommentMark); // From utf8 char * to utf16 wstring
        
        if(debug) { std::cout << "DEBUG:InputFile==" << InputFile << std::endl; }
        if(debug) { std::cout << "DEBUG:ColumnHead==" << InputColHead << std::endl; }
        if(debug) { std::cout << "DEBUG:CommentMark==" << CommentMark << std::endl; }
        
        long colpos = -1;
        long colcnt = 0;
        std::ifstream backstory (InputFile); std::string backline; // Do not use wstring to read file! <TODO>
        if (backstory.is_open()) {
            while (backstory.good()) {
                // get this line
                getline(backstory,backline);
                // if(debug) { std::cout << "DEBUG:" << backline << std::endl; }
                line = CrabStringWideString(backline); // From utf8 char * to utf16 wstring
                // if(debug) { std::wcout << "DEBUG:" << line.length() << std::endl; }
                trline = CrabStringTrim(line);
                // if(debug) { std::wcout << "DEBUG:" << line.length() << std::endl; }
                
                // whether this is empty line?
                if(trline.empty()) {
                    if(OutputArray.size()>0) {break;} else {continue;}
                }
                // whether this is comment line?
                if(0==trline.find(commark)) {
                    continue;
                }
                // whether this is header line? (we take the first non-comment line as the header line.)
                if(-1==colpos) {
                    if(debug) {
                        char* locale = setlocale(LC_ALL, "");
                        std::wcout << "CrabStringReadColumn: Debuging: " << "locale = " << locale << std::endl;
                        locale = std::setlocale(LC_ALL,"zh_CN.UTF-8");
                        std::wcout << "CrabStringReadColumn: Debuging: " << "locale = " << locale << std::endl;
                        std::wcout << "CrabStringReadColumn: Debuging: " << colname << std::endl;
                        std::wcout << "CrabStringReadColumn: Debuging: " << line << std::endl;
                        std::wcout << "CrabStringReadColumn: Debuging: " << line.find(L"测试测试") << " " << string::npos << std::endl;
                        std::setlocale(LC_ALL,"");
                        std::cout << "CrabTableReadColumn: Column Header pos " << colpos << std::endl;
                    }
                    colpos = CrabStringFindWholeWord(line,colname);
                    if(debug) {
                        std::cout << "CrabTableReadColumn: Column Header pos " << colpos << std::endl;
                    }
                    if(string::npos==colpos) {
                        std::cout << "CrabTableReadColumn: Column Header " << InputColHead << " was not found!" << std::endl;
                        break;
                    }
                }
                // whether this is content line? (we read table content by matching the vertical position!)
                if(colpos>=0) {
                    for(int j=0; j<colname.length(); j++) {
                        colvalue = CrabStringFindWholeWord(line,colpos+j);
                        if(!colvalue.empty()){
                            if(debug) { std::wcout << "CrabStringReadColumn: Debuging: j="<< j << " colvalue=" << colvalue << std::endl; }
                            colvalue = CrabStringTrim(colvalue);
                            OutputArray.push_back(csu_wstr_to_utf8(colvalue));
                            colcnt++;
                            // std::cout << colvalue << std::endl; // <TODO> Verbose
                            // std::cout << colvalue << "(" << colpos << ":" << colpos+colname.length()-1 << ")" << std::endl;
                            break;
                        }
                    }
                }
                // if user give *OutputRowNumber then only read limited rows
                if(OutputRowNumber) {
                    if(*OutputRowNumber>0) {
                        if(*OutputRowNumber<colcnt) {
                            break;
                        }
                    }
                }
            }
            backstory.close();
            // output how many rows are read
            if(OutputRowNumber) {
                *OutputRowNumber = colcnt;
            } else {
                OutputRowNumber = new int;
                *OutputRowNumber = colcnt;
            }
        }
        else {
            std::cout << "CrabTableReadColumn: Unable to open file! Please check " << InputFile << std::endl;
        }
    }
    if(debug) {
        for(int i=0; i<OutputArray.size(); i++) {
            std::cout << OutputArray[i] << std::endl;
        }
    }
    return OutputArray;
}



std::vector<std::string> CrabTableReadColumn(const char *InputFile, int InputColNumber, int *OutputRowNumber, const char *CommentMark)
{
    //
    std::vector<std::string> OutputArray; // OutputArray.push_back(superman);
    int debug = 0;
    //
    if(InputColNumber>0 && strlen(InputFile)>0) {
        std::wstring line;
        std::wstring trline;
        std::wstring colname;
        std::wstring colvalue;
        std::wstring commark = CrabStringWideString(CommentMark); // From utf8 char * to utf16 wstring
        
        if(debug) { std::cout << "DEBUG:InputFile==" << InputFile << std::endl; }
        if(debug) { std::cout << "DEBUG:ColumnNumb==" << InputColNumber << std::endl; }
        if(debug) { std::cout << "DEBUG:CommentMark==" << CommentMark << std::endl; }
        
        long colpos = -1;
        long colcnt = 0;
        std::ifstream backstory (InputFile); std::string backline; // Do not use wstring to read file! <TODO>
        if (backstory.is_open()) {
            while (backstory.good()) {
                getline(backstory,backline);
                // if(debug) { std::wcout << "DEBUG:" << backline.length() << std::endl; }
                line = CrabStringWideString(backline); // From utf8 char * to utf16 wstring
                // if(debug) { std::wcout << "DEBUG:" << line.length() << std::endl; }
                trline = CrabStringTrim(line);
                // if(debug) { std::wcout << "DEBUG:" << line.length() << std::endl; }
                
                // whether this is empty line?
                if(trline.empty()) {
                    if(OutputArray.size()>0) {break;} else {continue;}
                }
                // whether this is comment line?
                if(0==trline.find(commark)) {
                    continue;
                }
                // whether this is content line? (we read String content by counting the column number!)
                if(1==1) {
                    // vector<string> debugTmpArr;
                    colpos = 0;
                    for(long j=0; j<line.length(); j++) {
                        colvalue = CrabStringFindWholeWord(line,j);
                        if(!colvalue.empty()){
                            // if(debug) { std::wcout << "CrabStringReadColumn: Debuging: j="<< j << " colvalue=" << colvalue << std::endl; }
                            colpos--;
                            if((-colpos)==InputColNumber) {
                                colpos = j; break;
                            } else {
                                j+=colvalue.length();
                            }
                        }
                    }
                    if(colpos<0) {
                        std::cout << "CrabStringReadColumn: Column Number is too large in file " << InputFile << std::endl;
                        // std::cout << "CrabStringReadColumn: Debugging ";
                        // for(int debuggi=0; debuggi<debugTmpArr.size(); debuggi++) { std::cout << debugTmpArr[debuggi] << " "; }
                        // std::cout << std::endl;
                 /* } else if(colpos==0) {
                        std::cout << "CrabStringReadColumn: Empty line! <TODO> Stop or not?" << std::endl; // <Modified><20150201><DzLIU>
                    } else if(colpos>0) {
                        OutputArray.push_back(csu_wstr_to_utf8(colvalue));
                        colcnt++;
                        // std::cout << colvalue << std::endl; // <TODO> Verbose
                        // std::cout << colvalue << "(" << colpos << ":" << colpos+colname.length()-1 << ")" << std::endl;
                    }
                 */
                    } else if(colpos>=0) {
                        OutputArray.push_back(csu_wstr_to_utf8(colvalue));
                        colcnt++;
                        // std::cout << colvalue << std::endl; // <TODO> Verbose
                        // std::cout << colvalue << "(" << colpos << ":" << colpos+colname.length()-1 << ")" << std::endl;
                    }
                    // if user give *OutputRowNumber then only read limited rows
                    if(OutputRowNumber) {
                        if(*OutputRowNumber>0) {
                            if(*OutputRowNumber<colcnt) {
                                break;
                            }
                        }
                    }
                }
            }
            backstory.close();
            // output how many rows are read
            if(OutputRowNumber) {
                *OutputRowNumber = colcnt;
            } else {
                OutputRowNumber = new int;
                *OutputRowNumber = colcnt;
            }
        }
        else {
            std::cout << "CrabTableReadColumn: InputFile is invalid or InputNumber is not positive! Please check " << InputFile << std::endl;
        }
    }
    if(debug) {
        for(int i=0; i<OutputArray.size(); i++) {
            std::cout << OutputArray[i] << std::endl;
        }
    }
    return OutputArray;
}


double *CrabTableReadColumnF(const char *InputFile, int InputColNumber, int *OutputRowNumber, const char *CommentMark)
{
    std::vector<std::string> sctable = CrabTableReadColumn(InputFile, InputColNumber, OutputRowNumber, CommentMark);
    if(!sctable.empty()) {
        std::vector<double> dctable = ctrcstrtodouble(sctable);
        double *fctable = new double[sctable.size()];
        for(int iic=0; iic<sctable.size(); iic++) { fctable[iic] = dctable[iic]; }
        return fctable;
    }
    return NULL;
}



#endif