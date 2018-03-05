#include "michi2_DataClass.h"
#include "CrabStringClean.cpp"
#include "CrabStringReadColumn.cpp"
#include "CrabTableReadColumn.cpp"
#include "CrabTableReadInfo.cpp"
#include "CrabTableGetLineCount.cpp"


michi2DataClass::michi2DataClass(const char *InputFile, int verbose)
{
    //
    // This Class is used for not only LIB file but also OBS file.
    //
    if(1==1) { // <TODO> Check File Exists!
        this->FilePath = std::string(InputFile,InputFile+strlen(InputFile));
        this->FileStream.open(InputFile);
    }
    // std::cout << "michi2DataClass: Good! Let's Go!" << std::endl;
    // prepare vectors
    std::string              StTEMP;                           // Temporary String
    std::vector<std::string> StCVAR; std::vector<long> InCVAR; // Col Number of Each Variable
    std::vector<std::string> StNVAR; std::vector<long> InNVAR; // Row  Count of Each Variable
    std::vector<std::string> StCPAR; std::vector<long> InCPAR; // Col Number of Each Parameter
    std::vector<std::string> StNPAR; std::vector<long> InNPAR; // Row  Count of Each Parameter
    std::vector<std::string> StTPAR;                           // Col  Title of Each Parameter
    // read data info
    std::string StLine;
    for(int i=1; i<=2; i++) { // <TODO><LIMIT> Support only =< 2 Variables (Limit <= 2 Dimensions)
        std::string StColCVAR1 = michi2sprint("# CVAR",i," ");
    }
    while (std::getline(this->FileStream, StLine))
    {
        std::istringstream SsLine(StLine);
        if
        if(!StLine.compare(0,StColCVAR1.length(),StColCVAR1)==0) {  }
    }
    for(int i=1; i<=2; i++) { // <TODO><LIMIT> Support only =< 2 Variables (Limit <= 2 Dimensions)
        if(verbose>=3) {std::cout << "michi2DataClass: CrabTableReadInfo " << michi2sprint("# CVAR",i,"") << std::flush;}
        StTEMP = CrabTableReadInfo(InputFile,michi2sprint("# CVAR",i,""));
        if(verbose>=3) {std::cout << ": " << StTEMP << std::endl;}
        if(!StTEMP.empty()) { StCVAR.push_back(StTEMP); InCVAR.push_back(michi2stoi(StTEMP)); } // else {break;}
        //
        if(verbose>=3) {std::cout << "michi2DataClass: CrabTableReadInfo " << michi2sprint("# NVAR",i,"") << std::flush;}
        StTEMP = CrabTableReadInfo(InputFile,michi2sprint("# NVAR",i,""));
        if(verbose>=3) {std::cout << ": " << StTEMP << std::endl;}
        if(!StTEMP.empty()) { StNVAR.push_back(StTEMP); InNVAR.push_back(michi2stoi(StTEMP)); } // else {break;}
        //
    }
    for(int i=1; i<=16; i++) { // <TODO><LIMIT> Support only <= 16 Parameters <20180131> 6 --> 16
        if(verbose>=3) {std::cout << "michi2DataClass: CrabTableReadInfo " << "# CPAR" << i << std::flush;}
        StTEMP = CrabTableReadInfo(InputFile,michi2sprint("# CPAR",i,""));
        if(verbose>=3) {std::cout << ": " << StTEMP << std::endl;}
        if(!StTEMP.empty()) { StCPAR.push_back(StTEMP); InCPAR.push_back(michi2stoi(StTEMP)); } // else {break;}
        //
        if(verbose>=3) {std::cout << "michi2DataClass: CrabTableReadInfo " << "# NPAR" << i << std::flush;}
        StTEMP = CrabTableReadInfo(InputFile,michi2sprint("# NPAR",i,""));
        if(verbose>=3) {std::cout << ": " << StTEMP << std::endl;}
        if(!StTEMP.empty()) { StNPAR.push_back(StTEMP); InNPAR.push_back(michi2stoi(StTEMP)); } // else {break;}
        //
        if(verbose>=3) {std::cout << "michi2DataClass: CrabTableReadInfo " << "# TPAR" << i << std::flush;}
        StTEMP = CrabTableReadInfo(InputFile,michi2sprint("# TPAR",i,""));
        if(verbose>=3) {std::cout << ": " << StTEMP << std::endl;}
        if(!StTEMP.empty()) { StTPAR.push_back(StTEMP); } // else {break;}
        //
    }
    // print error info
    if(this->FilePath.find("Lib")==0 && InNPAR.empty()) {
        std::cout << "michi2DataClass: Error! \"NPAR\" was not found in the header of LIB file \"" << InputFile << "\"" << std::endl;
        exit (EXIT_FAILURE);
    }
    if(this->FilePath.find("Lib")==0 && StTPAR.empty()) {
        std::cout << "michi2DataClass: Error! \"TPAR\" was not found in the header of LIB file \"" << InputFile << "\"" << std::endl;
        exit (EXIT_FAILURE);
    }
    // print debug info
    if(!InNPAR.empty()) {
        if(verbose>=2) {
            std::cout << "michi2DataClass: Read " << InNPAR.size() << " parameters: " << std::flush;
            for(int iPar_1=0; iPar_1<StTPAR.size(); iPar_1++) {
                std::cout << StTPAR[iPar_1] << ", " << std::flush;
            }
            std::cout << "from the file " << InputFile << std::endl;
        }
    }
    // std::cout << "michi2DataClass: Good! Let's Go!" << std::endl;
    // exam data info
    if(InNVAR.empty()) { // if no header info, then directly read this data file! (obs.dat) <TODO>
        InNVAR.push_back(CrabTableGetLineCount(InputFile));  InCVAR.push_back(1);  InNVAR.push_back(1);  InCVAR.push_back(2);
        if(verbose>=1) {
            std::cout << "michi2DataClass: We will take column " << InCVAR[0] << " as X, column " << InCVAR[1] << " as Y, and column " << InCVAR[1]+1 << " as YErr if possible, from the file " << InputFile << std::endl;
        }
        if(verbose>=2) { std::cout << "michi2DataClass: Reading column " << InCVAR[0] << std::endl; }
        this->XStr = CrabTableReadColumn(InputFile,InCVAR[0]);
        if(verbose>=2) { std::cout << "michi2DataClass: Reading column " << InCVAR[1] << std::endl; }
        this->YStr = CrabTableReadColumn(InputFile,InCVAR[1]);
        if(verbose>=2) { std::cout << "michi2DataClass: Reading column " << InCVAR[1]+1 << std::endl; }
        this->YErrStr = CrabTableReadColumn(InputFile,InCVAR[1]+1);
        if(this->XStr.empty()||this->YStr.empty()) { std::cout << "michi2DataClass: Error! Could not determine NVAR1 and NVAR2 from " << InputFile << "!" << std::endl; return; }
        this->X = michi2stod(this->XStr);
        this->Y = michi2stod(this->YStr);
        this->YErr = michi2stod(this->YErrStr);
        this->Matched.resize(this->Y.size()); this->Matched.assign(this->Y.size(),0); // * 0.0;
        
    } else {
        if(InNVAR.size()!=2) { std::cout << "michi2DataClass: Error! Could not determine NVAR1 and NVAR2 from " << InputFile << "!" << std::endl; return; }
        // std::cout << "michi2DataClass: We will take column " << InCVAR[0] << " as X, column " << InCVAR[1] << " as Y, and column " << InCVAR[1]+1 << " as YErr if possible, from the file " << InputFile << std::endl;
        if(verbose>=1) {
            std::cout << "michi2DataClass: We will take column " << InCVAR[0] << " as X, column " << InCVAR[1] << " as Y, according to the header in the file " << InputFile << std::endl;
        }
    }
    // save
    this->CVAR = InCVAR;
    this->NVAR = InNVAR;
    this->CPAR = InCPAR;
    this->NPAR = InNPAR;
    this->TPAR = StTPAR;
    this->XNum = InNVAR[0];
    this->YNum = InNVAR[1];
    this->XCol = InCVAR[0];
    this->YCol = InCVAR[1];
    this->YErrCol = InCVAR[1]+1;
    
    // std::cout << "DEBUG: michi2DataClass this->X=0x" << std::hex << (size_t)&this->X << std::endl;
    // std::cout << "DEBUG: michi2DataClass this->XNum=0x" << std::hex << (size_t)&this->XNum << std::endl;
    // std::cout << "DEBUG: michi2DataClass this->Y=0x" << std::hex << (size_t)&this->Y << std::endl;
    // std::cout << "DEBUG: michi2DataClass this->YNum=0x" << std::hex << (size_t)&this->YNum << std::endl;
}


michi2DataClass::~michi2DataClass()
{
    if(this->FileStream.is_open()) { this->FileStream.close(); }
    FilterCurveFilePath.clear(); FilePath.clear();
    X.clear(); Y.clear();
}


const char *michi2DataClass::michi2sprint(const char* wsPrefix, long wi, const char* wsSuffix)
{
    /*
    std::ostringstream wssm; // stream used for the conversion
    wssm << wsPrefix;
    wssm << wi;
    wssm << wsSuffix;
    return wssm.str().c_str();
     */
    std::string input_str_1 = wsPrefix;
    std::string input_str_2 = std::to_string(wi); // C++11
    std::string input_str_3 = wsSuffix;
    std::string output_str = input_str_1 + input_str_2 + input_str_3;
    return output_str.c_str();
    // BUG 20180114: found on linux gcc > 5 only:
    // stringstream.str() will return a temporary string object, which will be destroyed after the function call. When we want to manipulate on this string object (for example, to create corresponding C string), we must be very careful about this trap that might cause unexpected results. -- https://www.ibm.com/developerworks/community/blogs/5894415f-be62-4bc0-81c5-3956e82276f3/entry/discover_the_traps_when_using_stringstream_str?lang=en
}


int michi2DataClass::michi2stoi(string str)
{
    std::istringstream ssm(str);
    int i = 0;
    ssm >> i;
    return i;
}


int michi2DataClass::michi2wstoi(wstring wstr)
{
    std::wistringstream wssm(wstr);
    int wi = 0;
    wssm >> wi;
    return wi;
}


std::vector<double> michi2DataClass::michi2stod(std::vector<string> strVec)
{
    std::vector<double> iVec;
    for(int i=0; i<strVec.size(); i++) {
        std::string str = strVec[i];
        std::istringstream ssm(str); // stream used for the conversion
        double di = 0.0;
        ssm >> di;
        iVec.push_back(di);
    }
    return iVec;
}


std::vector<double> michi2DataClass::michi2wstod(std::vector<wstring> wstrVec)
{
    std::vector<double> wiVec;
    for(int i=0; i<wstrVec.size(); i++) {
        std::wstring wstr = wstrVec[i];
        std::wistringstream wssm(wstr); // stream used for the conversion
        double wi = 0.0;
        wssm >> wi;
        wiVec.push_back(wi);
    }
    return wiVec;
}

std::vector<std::string> michi2DataClass::getDataBlock(long lineNumber, long lineCount, int debug)
{
    // debug
    // int debug = 0;
    // prepare to read
    std::string backline;
    // skip header
    long backpoint = 0;
    long backshift = 0;
    long backwidth = 0;
    std::ifstream *backstory = &(this->FileStream);
    if (backstory->is_open()) {
        // backpoint = backstory->tellg(); // std::wcout << "DEBUG: " << "backpoint=" << backpoint << std::endl;
        backstory->seekg(0);
        while(backstory->good()) {
            // read line and test whether starting with #
            backshift = backstory->tellg(); // std::wcout << "DEBUG: " << "backshift=" << backshift << std::endl;
            getline(*backstory,backline);
            if(backline.find("#")!=0) { // not starting with #
                // get data line width // <TODO> EACH DATA LINE SHOULD HAVE SAME BYTE WIDTH !
                backwidth = backstory->tellg(); backwidth -= backshift; // std::wcout << "DEBUG: " << backwidth << std::endl;
                break;
            }
        }
        // backstory.seekg(backpoint); // rewind to former status
        backstory->seekg(backshift); // rewind to the beginning of data blocks
    }
    // prepare text block
    std::vector<std::string> textblock;
    // seek to line i
    backstory->seekg(backshift+(lineNumber-1)*backwidth);
    // debug
    if(debug) { std::cout << "getDataBlock: " << "seeking to line " << lineNumber << " offset " << backshift << "+" << (lineNumber-1) << "*" << backwidth << "=" << backshift+(lineNumber-1)*backwidth << std::endl; }
    // read lines
    for(int j=0; j<lineCount; j++) {
        getline(*backstory,backline);
        textblock.push_back(backline);
        if(debug) { std::cout << "getDataBlock: " << backline << std::endl; }
        
    }   if(debug) { std::cout << "getDataBlock: " << std::endl; } // <TODO><DEBUG>
    // read data <TODO>
    std::vector<std::string>  StFX = CrabStringReadColumn(textblock,this->XCol);
    std::vector<std::string>  StFY = CrabStringReadColumn(textblock,this->YCol);
    std::vector<double>       DbFX = michi2stod(StFX);
    std::vector<double>       DbFY = michi2stod(StFY);
    // save data <TODO>
    this->X = DbFX;
    this->Y = DbFY;
    this->XStr = StFX;
    this->YStr = StFY;
    this->Matched.resize(this->Y.size()); this->Matched.assign(this->Y.size(),0); // * 0.0;
    // read parameter data <TODO>
    this->FPAR.clear();
    for(int j=0; j<this->CPAR.size(); j++) {
        std::vector<std::string>  StPAR = CrabStringReadColumn(textblock,this->CPAR[j]);
        std::vector<double>       DbPAR = michi2stod(StPAR);
        this->FPAR.push_back(DbPAR);
    }
    // return
    return textblock;
}

std::vector<std::string> michi2DataClass::getDataBlockQuickQuick(long lineNumber, long lineCount, int debug)
{
    // debug
    // int debug = 0;
    // prepare to read
    std::string backline;
    // skip header
    long backpoint = 0;
    long backshift = 0;
    long backwidth = 0;
    std::ifstream *backstory = &(this->FileStream);
    if (backstory->is_open()) {
        // backpoint = backstory->tellg(); // std::wcout << "DEBUG: " << "backpoint=" << backpoint << std::endl;
        backstory->seekg(0);
        while(backstory->good()) {
            // read line and test whether starting with #
            backshift = backstory->tellg(); // std::wcout << "DEBUG: " << "backshift=" << backshift << std::endl;
            getline(*backstory,backline);
            if(backline.find("#")!=0) { // not starting with #
                // get data line width // <TODO> EACH DATA LINE SHOULD HAVE SAME BYTE WIDTH !
                backwidth = backstory->tellg(); backwidth -= backshift; // std::wcout << "DEBUG: " << backwidth << std::endl;
                break;
            }
        }
        // backstory.seekg(backpoint); // rewind to former status
        backstory->seekg(backshift); // rewind to the beginning of data blocks
    }
    // prepare text block
    std::vector<std::string> textblock;
    // seek to line i
    backstory->seekg(backshift+(lineNumber-1)*backwidth);
    // debug
    if(debug) { std::cout << "getDataBlock: " << "seeking to line " << lineNumber << " offset " << backshift << "+" << (lineNumber-1) << "*" << backwidth << "=" << backshift+(lineNumber-1)*backwidth << std::endl; }
    // read lines
    this->XStr.clear(); this->YStr.clear();
    this->X.clear(); this->Y.clear();
    std::vector<std::string> TmpStrVec;
    for(int j=0; j<lineCount; j++) {
        getline(*backstory,backline);
        textblock.push_back(backline);
        TmpStrVec = split(backline, ' ');
        this->XStr.push_back(TmpStrVec[this->XCol-1]);
        this->YStr.push_back(TmpStrVec[this->YCol-1]);
        if(debug) { std::cout << "getDataBlock: " << backline << std::endl; }
    }   if(debug) { std::cout << "getDataBlock: " << std::endl; } // <TODO><DEBUG>
    this->X = michi2stod(this->XStr);
    this->Y = michi2stod(this->YStr);
    // return
    return textblock;
}


std::vector<std::string> michi2DataClass::split(std::string const& original, char separator)
{
    std::vector<std::string> results;
    std::string::const_iterator start = original.begin();
    std::string::const_iterator end = original.end();
    std::string::const_iterator next = std::find( start, end, separator );
    while ( next != end ) {
        results.push_back( std::string( start, next ) );
        start = next + 1;
        next = std::find( start, end, separator );
    }
    results.push_back( std::string( start, next ) );
    return results;
}



std::vector<std::string> michi2DataClass::readFilterCurveListFile(const char * InputFile)
{
    // The FilterCurveListFile should contains two columns,
    // first is wavelength, same as input OBS file,
    // second is the filter curve file path for each wavelength.
    //std::cout << "michi2DataClass::readFilterCurveListFile()" << std::endl;
    //int ReadRowNumber = 0;
    this->FilterCurveFilePath = CrabTableReadColumn(InputFile, 2); // we use two white space to separate columns, read the second column.
    if(this->FilterCurveFilePath.size()!=this->X.size()) {this->FilterCurveFilePath.clear();}
    if(this->FilterCurveFilePath.size()==0) {this->FilterCurveFilePath.resize(this->X.size());}
    for(int i=0; i<this->FilterCurveFilePath.size(); i++) {
        if(!this->FilterCurveFilePath.empty()) {
            //std::cout << "this->FilterCurveFilePath[" << i << "] = \"" << this->FilterCurveFilePath[i] << "\"" << std::endl;
            //this->FilterCurveFilePath[i] = std::regex_replace(this->FilterCurveFilePath[i], std::regex("^ +$"), ""); // this requires gcc >=4.9 (CentOS >=8)
            this->FilterCurveFilePath[i] = CrabStringTrim(this->FilterCurveFilePath[i]);
            if(this->FilterCurveFilePath.at(i).front() == '"' && this->FilterCurveFilePath.at(i).back() == '"') {
                this->FilterCurveFilePath[i].erase( 0, 1 ); // erase the first character
                this->FilterCurveFilePath[i].erase( this->FilterCurveFilePath[i].size() - 1 ); // erase the last character
            }
            if(this->FilterCurveFilePath[i].find("/")!=0) {
                std::string InputFileStr(InputFile);
                if (InputFileStr.find_last_of("/")!=std::string::npos) {
                    this->FilterCurveFilePath[i] = InputFileStr.substr(0,InputFileStr.find_last_of("/")+1).append(this->FilterCurveFilePath[i]);
                }
            }
        }
        //std::cout << "this->FilterCurveFilePath[" << i << "] = \"" << this->FilterCurveFilePath[i] << "\"" << std::endl;
    }
    return this->FilterCurveFilePath;
}











