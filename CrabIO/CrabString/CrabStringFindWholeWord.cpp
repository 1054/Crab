/*
 
 We must be careful about codecs. 
 All files input should be utf8. 
 
 */

#ifndef H_CrabStringFindWholeWord
#define H_CrabStringFindWholeWord
#include <iostream>
#include <iomanip> // std::setfill, std::setw
#include <cstdlib>
#include <string>
#include <locale>
#include <wchar.h>

using namespace std;

long CrabStringFindWholeWord(const string &t, const string &strsearch, const string &BreakupMark = " ");

long CrabStringFindWholeWord(const wstring &t, const wstring &strsearch, const wstring &BreakupMark = L" ");

string CrabStringFindWholeWord(const string &t, size_t pos, const string &BreakupMark = " ");

wstring CrabStringFindWholeWord(const wstring &t, size_t pos, const wstring &BreakupMark = L" ");



long CrabStringFindWholeWord(const string &t, const string &strsearch, const string &BreakupMark)
{
    size_t foundpos = 0;
    while(string::npos!=foundpos) {
        foundpos = t.find(strsearch,foundpos);
        if(string::npos!=foundpos) {
            if(foundpos>=BreakupMark.length() && foundpos+strsearch.length()<t.length()-BreakupMark.length()) {
                if(BreakupMark==t.substr(foundpos-1,BreakupMark.length()) &&
                   BreakupMark==t.substr(foundpos+strsearch.length(),BreakupMark.length())) {break;}
            }
            if(foundpos==0 && foundpos+strsearch.length()<t.length()-BreakupMark.length()) {
                if(BreakupMark==t.substr(foundpos+strsearch.length(),BreakupMark.length())) {break;}
            }
            if(foundpos>=BreakupMark.length() && foundpos+strsearch.length()==t.length()-BreakupMark.length()-1) {
                if(BreakupMark==t.substr(foundpos-1,BreakupMark.length())) {break;}
            }
        } else {
            break;
        }
        foundpos++;
    }
    return foundpos;
}



long CrabStringFindWholeWord(const wstring &t, const wstring &strsearch, const wstring &BreakupMark)
{
    size_t foundpos = 0;
    int debug = 0;
    while(wstring::npos!=foundpos) {
        foundpos = t.find(strsearch,foundpos);
        if(debug) { std::wcout << "CrabStringFindWholeWord: searching " << strsearch << " got foundpos = " << foundpos << std::endl; }
        if(wstring::npos!=foundpos) {
            if(foundpos>=BreakupMark.length() && foundpos+strsearch.length()<t.length()-BreakupMark.length()) {
                if(BreakupMark==t.substr(foundpos-1,BreakupMark.length()) &&
                   BreakupMark==t.substr(foundpos+strsearch.length(),BreakupMark.length())) {break;}
            }
            if(foundpos==0 && foundpos+strsearch.length()<t.length()-BreakupMark.length()) {
                if(BreakupMark==t.substr(foundpos+strsearch.length(),BreakupMark.length())) {break;}
            }
            if(foundpos>=BreakupMark.length() && foundpos+strsearch.length()==t.length()-BreakupMark.length()-1) {
                if(BreakupMark==t.substr(foundpos-1,BreakupMark.length())) {break;}
            }
        } else {
            break;
        }
        foundpos++;
    }
    return foundpos;
}



string CrabStringFindWholeWord(const string &t, size_t pos, const string &BreakupMark)
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



wstring CrabStringFindWholeWord(const wstring &t, size_t pos, const wstring &BreakupMark)
{
    wstring foundstr;
    if(0==pos) {
        size_t foundposL = 0;
        size_t foundposR = t.find(BreakupMark)-1;
        foundstr = t.substr(foundposL,foundposR-foundposL+1);
    } else if(t.length()-1==pos) {
        size_t foundposL = t.rfind(BreakupMark)+1;
        size_t foundposR = t.length()-1;
        foundstr = t.substr(foundposL,foundposR-foundposL+1);
    } else if(0<pos && t.length()-1>pos) {
        wstring strL = t.substr(0,pos);
        wstring strR = t.substr(pos);
        size_t foundposL = strL.rfind(BreakupMark)+1;
        size_t foundposR = strR.find(BreakupMark)-1+pos;
        foundstr = t.substr(foundposL,foundposR-foundposL+1);
    }
    return foundstr;
}



#endif