#include "CrabFitsIO.h"
#include "CrabFitsTableClass.h"



CrabFitsTableClass::CrabFitsTableClass(const char * FilePath, int xtension)
{
    if(FilePath) {
        int i=0;
        int check_ok = -1;
        m_FilePath = (char *)FilePath;
        m_FitsExtNumber = xtension;
        m_FilePtr = NULL;
        m_Header0 = NULL;
        m_Header = NULL;
        
        // open FileStream
        m_FileStream.open(FilePath, std::ifstream::binary|std::ifstream::in);
        if(!m_FileStream.is_open()) {
            std::cout << "Error! Failed to open \"" << FilePath << "\"!" << std::endl;
        }
        
        // readFitsHeader for extension 0, i.e., the main header
        check_ok = readFitsHeader(FilePath, 0, &m_Header0, &m_HeaderPos0, &m_HeaderSize0);
        if(0==check_ok) {
            std::cout << "Read " << FilePath << std::endl;
            std::cout << "     " << "extension " << 0 << std::endl;
            std::cout << "     " << "HeaderSize " << m_HeaderSize << std::endl;
            std::cout << "     " << "HeaderPos " << m_HeaderPos << std::endl;
        }
        
        if(xtension>0) {
            // readFitsHeader for extension i, i.e., the user-input extension
            check_ok = readFitsHeader(FilePath, xtension, &m_Header, &m_HeaderPos, &m_HeaderSize);
            if(0==check_ok) {
                std::cout << "Read " << FilePath << std::endl;
                std::cout << "     " << "extension " << xtension << std::endl;
                std::cout << "     " << "HeaderSize " << m_HeaderSize0 << " Bytes" << std::endl;
                std::cout << "     " << "HeaderPos " << m_HeaderPos0 << " Bytes" << std::endl;
            }
        }
        
        //
        // read header keywords
        m_Key_BITPIX = 0;
        m_Key_NAXIS.clear();
        std::string Key_Name;
        char * Key_Value;
        if(m_Header0) {
            Key_Name = "BITPIX";
            Key_Value = extKeyword(Key_Name.c_str(),m_Header0);
            if(Key_Value) {
                m_Key_BITPIX = std::atoi(Key_Value);
                std::cout << "Keyword " << "BITPIX " << m_Key_BITPIX << std::endl;
            }
            Key_Name = "NAXIS";
            Key_Value = extKeyword(Key_Name.c_str(),m_Header0);
            if(Key_Value) {
                for(i=1; i<=m_Key_BITPIX; i++) {
                    Key_Name = "NAXIS"; Key_Name.append(std::to_string(i));
                    Key_Value = extKeyword(Key_Name.c_str(),m_Header0);
                    if(Key_Value) {
                        m_Key_NAXIS.push_back(std::atol(Key_Value));
                        std::cout << "Keyword " << std::string("NAXIS")+std::to_string(i)+" " << m_Key_NAXIS[m_Key_NAXIS.size()-1] << std::endl;
                    }
                }
            }
        }
        if(m_Header) {
            Key_Name = "BITPIX";
            Key_Value = extKeyword(Key_Name.c_str(),m_Header);
            if(Key_Value) {
                m_Key_BITPIX = std::atoi(Key_Value);
                std::cout << "Keyword " << "BITPIX " << m_Key_BITPIX << std::endl;
            }
            Key_Name = "NAXIS";
            Key_Value = extKeyword(Key_Name.c_str(),m_Header);
            if(Key_Value) {
                m_Key_BITPIX = std::atoi(Key_Value);
                for(i=1; i<=m_Key_BITPIX; i++) {
                    Key_Name = "NAXIS"; Key_Name.append(std::to_string(i));
                    Key_Value = extKeyword(Key_Name.c_str(),m_Header);
                    if(Key_Value) {
                        m_Key_NAXIS.push_back(std::atol(Key_Value));
                        std::cout << "Keyword " << std::string("NAXIS")+std::to_string(i)+" " << m_Key_NAXIS[m_Key_NAXIS.size()-1] << std::endl;
                    }
                }
            }
        }
    }
}


CrabFitsTableClass::~CrabFitsTableClass()
{
    
}


std::vector<double> CrabFitsTableClass::readFitsTableDataBlock(long RowNumber)
{
    std::vector<double> output_array;
    //
    // TODO
    if(m_FileStream.is_open()) {
        m_FileStream.clear();
        if(m_Header) {
            m_FileStream.seekg(m_HeaderPos);
        } else {
            m_FileStream.seekg(m_HeaderPos0);
        }
        //cout << "State before seekg: " << in.rdstate() << endl;   // Prints 3 (11 in binary) failbit and eofbit activated.
        //in.seekg(0);
        //cout << "State after seekg: " << in.rdstate() << endl;    // Prints 2 (10 in binary) just failbit activated.

        
    }
    //
    // return output_array
    return output_array;
}
