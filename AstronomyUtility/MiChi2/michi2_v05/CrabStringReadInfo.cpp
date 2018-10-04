#ifndef H_CrabStringReadInfo
#define H_CrabStringReadInfo
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "CrabStringClean.cpp"
#include "CrabStringUnicode.cpp"
#include "CrabStringFindWholeWord.cpp"

using namespace std;



std::string CrabStringReadInfo(const char *InputText, const char *KeyName, const char *CommentMark = "#");
std::string CrabStringReadInfo(std::vector<std::string> InputTexts, std::string KeyName, std::string CommentMark = "#");





std::string CrabStringReadInfo(const char *InputText, const char *InputKeyName, const char *CommentMark)
{
    // http://www.cplusplus.com/forum/beginner/70805/
    std::string OutputText;
    int debug = 0;
    if(strlen(InputKeyName)>0 && strlen(InputText)>0) {
        std::wstring line = CrabStringWideString(InputText); // From utf8 char * to utf16 wstring
        std::wstring keyname = CrabStringWideString(InputKeyName); // From utf8 char * to utf16 wstring
        // std::wstring commark(CommentMark,CommentMark+strlen(CommentMark)); // From char * to wstring: http://stackoverflow.com/questions/2573834/c-convert-string-or-char-to-wstring-or-wchar-t
        std::wstring commark = CrabStringWideString(CommentMark); // From utf8 char * to utf16 wstring
        std::wstring keyvalue;
        std::wstring keycomment;
        
        if(debug) { std::cout << "DEBUG:InputText==" << InputText << std::endl; }
        if(debug) { std::cout << "DEBUG:InputKeyName==" << InputKeyName << std::endl; }
        if(debug) { std::cout << "DEBUG:CommentMark==" << CommentMark << std::endl; }
        
        line = CrabStringTrim(line);
        size_t foundpos1 = CrabStringFindWholeWord(line,keyname,L" ");
        size_t foundpos2 = CrabStringFindWholeWord(line,keyname,L"=");
        size_t foundpos = string::npos;
        if(foundpos1!=string::npos) {foundpos = foundpos1;}
        if(foundpos2!=string::npos) {foundpos = foundpos2;}
        if(foundpos!=string::npos) {
            if(debug) { std::cout << "DEBUG:"; csu_print_wstr(line); std::wcout << std::endl; }
            // size_t foundpos = line.find_first_not_of(keyname); // find keyname as a whole word // <Corrected><20150201><DzLIU> Read Galfit COMP_11 COMP_1 Problem
            // wstring trimline = line;                           // find keyname as a whole word // <Corrected><20150201><DzLIU> Read Galfit COMP_11 COMP_1 Problem
            // trimline.erase(0,foundpos);                        // find keyname as a whole word // <Corrected><20150201><DzLIU> Read Galfit COMP_11 COMP_1 Problem
            // <TODO><20150201><DzLIU> Still can not deal with this: "Happy Birthday = 2010" but keyname is "Happy".
            wstring trimline = line;
            trimline.erase(0,foundpos+keyname.length());
            if(debug) { std::cout << "DEBUG:"; csu_print_wstr(trimline); std::wcout << std::endl; } // <Debug><20150201><DzLIU> Read Galfit COMP_11 COMP_1 Problem
            trimline = CrabStringTrim(trimline);
            if(debug) { std::cout << "DEBUG:"; csu_print_wstr(trimline); std::wcout << std::endl; } // <Debug><20150201><DzLIU> Read Galfit COMP_11 COMP_1 Problem
            if(0==trimline.find(L"=")) { // find keyname as a whole word
                trimline.erase(0,1);
                foundpos = trimline.find(commark); // find comment
                if(string::npos!=foundpos) { // no need to say 0<=foundpos because foundpos is unsigned.
                    keycomment = trimline;
                    keycomment.erase(0,foundpos);
                    trimline.erase(foundpos);
                }
                keyvalue = CrabStringTrim(trimline);
                OutputText = csu_wstr_to_utf8(keyvalue);
                // [show full information] std::cout << keyname << " = " << keyvalue << std::endl;
                // std::wcout << keyvalue << std::endl; // <TODO> Verbose
            }
            if(debug) { std::cout << "DEBUG:ReadKeyValue==" << OutputText << std::endl; }
        }
    }
    return OutputText;
}







std::string CrabStringReadInfo(std::vector<std::string> InputTexts, std::string InputKeyName, std::string CommentMark)
{
    // http://www.cplusplus.com/forum/beginner/70805/
    std::string OutputText;
    int debug = 0;
    if(InputKeyName.length()>0 && InputTexts.size()>0) {
        std::string keyname = InputKeyName;
        std::string commark = CommentMark;
        for (int i=0; i<InputTexts.size(); i++) {
            std::string line = InputTexts[i];
            std::string keyvalue;
            std::string keycomment;
            
            if(debug) { std::cout << "DEBUG:Line==" << line << std::endl; }
            if(debug) { std::cout << "DEBUG:KeyName==" << keyname << std::endl; }
            if(debug) { std::cout << "DEBUG:CommentMark==" << commark << std::endl; }
            
            size_t foundpos1 = CrabStringFindWholeWord(line,keyname," ");
            size_t foundpos2 = CrabStringFindWholeWord(line,keyname,"=");
            size_t foundpos = string::npos;
            if(foundpos1!=string::npos) {foundpos = foundpos1;}
            if(foundpos2!=string::npos) {foundpos = foundpos2;}
            if(foundpos!=string::npos) {
                if(debug) { std::cout << "DEBUG:" << line << std::endl; }
                std::string trimline = line;
                trimline.erase(0,foundpos+keyname.length());
                trimline = CrabStringTrim(trimline);
                if(0==trimline.find("=")) { // find keyname as a whole word
                    trimline.erase(0,1);
                    foundpos = trimline.find(commark); // find comment
                    if(string::npos!=foundpos) { // no need to say 0<=foundpos because foundpos is unsigned.
                        keycomment = trimline;
                        keycomment.erase(0,foundpos);
                        trimline.erase(foundpos);
                    }
                    keyvalue = CrabStringTrim(trimline);
                    OutputText = keyvalue;
                }
                if(debug) { std::cout << "DEBUG:KeyValue==" << OutputText << std::endl; }
            }
        }
    }
    return OutputText;
}






#endif
