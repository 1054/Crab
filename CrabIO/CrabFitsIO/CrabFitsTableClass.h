/* 
    CrabFitsTableClass created on 2017-10-07, aiming at reading and processing UVFITS.
 
 */

#ifndef CRABFITSTABLECLASS_H
#define CRABFITSTABLECLASS_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream> // std::cout
#include <iomanip> // std::atoi
#include <fstream> //
#include "CrabFitsIO.h"


class CrabFitsTableClass
{
public:
    CrabFitsTableClass(const char * FilePath, int xtension = 0);
    ~CrabFitsTableClass();
    std::vector<double> readFitsTableDataBlock(long RowNumber);
protected:
    FILE * m_FilePtr;
    std::ifstream m_FileStream;
    char * m_FilePath;
    int    m_FitsExtNumber;
    char * m_Header;
    long   m_HeaderSize;
    long   m_HeaderPos;
    char * m_Header0; // extension 0 main header
    long   m_HeaderSize0; // extension 0 main header
    long   m_HeaderPos0; // extension 0 main header
    int    m_Key_BITPIX;
    std::vector<long> m_Key_NAXIS;
};



#endif
