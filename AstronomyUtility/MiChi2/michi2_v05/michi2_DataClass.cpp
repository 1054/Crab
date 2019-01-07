#include "michi2_DataClass.h"
#include "CrabStringClean.cpp"
#include "CrabStringReadInfo.cpp"
#include "CrabStringReadColumn.cpp"
#include "CrabTableReadColumn.cpp"
#include "CrabTableGetLineCount.cpp"


michi2DataClass::michi2DataClass(const char *InputFile, int verbose)
{
    //
    // This Class is used for not only LIB file but also OBS file.
    //
    this->FilePath = std::string(InputFile,InputFile+strlen(InputFile));
    this->FileStream = new std::ifstream();
    this->FileStream->open(InputFile);
    // std::cout << "michi2DataClass: Good! Let's Go!" << std::endl;
    //
    // prepare vectors
    std::string              StTEMP;                           // Temporary String
    std::vector<std::string> StCVAR; std::vector<long> InCVAR; // Col Number of Each Variable
    std::vector<std::string> StNVAR; std::vector<long> InNVAR; // Row  Count of Each Variable
    std::vector<std::string> StCPAR; std::vector<long> InCPAR; // Col Number of Each Parameter
    std::vector<std::string> StNPAR; std::vector<long> InNPAR; // Row  Count of Each Parameter
    std::vector<std::string> StTPAR;                           // Col  Title of Each Parameter
    //
    // read header info
    this->HeaderLines = 0;
    this->HeaderBytes = 0;
    std::vector<std::string> StHeaderLines;
    std::string StLine;
    while (std::getline(*(this->FileStream), StLine)) {
        if(StLine.compare(0,1,"#")!=0) {break;} // if we meet the first non-commented data line, then break.
        StHeaderLines.push_back(StLine); // store header lines into StHeaderLines
        this->HeaderLines++;
        this->HeaderBytes = this->FileStream->tellg();
    }
    for(int i=1; i<=2; i++) { // <TODO><LIMIT> Support only =< 2 Variables (Limit <= 2 Dimensions)
        if(verbose>=4) {std::cout << "michi2DataClass: CrabStringReadInfo " << michi2sprint("# CVAR",i,"") << std::flush;}
        StTEMP = CrabStringReadInfo(StHeaderLines,michi2sprint("# CVAR",i,""));
        if(verbose>=4) {std::cout << ": " << StTEMP << std::endl;}
        if(!StTEMP.empty()) { StCVAR.push_back(StTEMP); InCVAR.push_back(michi2stoi(StTEMP)); } // else {break;}
        //
        if(verbose>=4) {std::cout << "michi2DataClass: CrabStringReadInfo " << michi2sprint("# NVAR",i,"") << std::flush;}
        StTEMP = CrabStringReadInfo(StHeaderLines,michi2sprint("# NVAR",i,""));
        if(verbose>=4) {std::cout << ": " << StTEMP << std::endl;}
        if(!StTEMP.empty()) { StNVAR.push_back(StTEMP); InNVAR.push_back(michi2stoi(StTEMP)); } // else {break;}
        //
    }
    for(int i=1; i<=16; i++) { // <TODO><LIMIT> Support only <= 16 Parameters <20180131> 6 --> 16
        if(verbose>=4) {std::cout << "michi2DataClass: CrabStringReadInfo " << "# CPAR" << i << std::flush;}
        StTEMP = CrabStringReadInfo(StHeaderLines,michi2sprint("# CPAR",i,""));
        if(verbose>=4) {std::cout << ": " << StTEMP << std::endl;}
        if(!StTEMP.empty()) { StCPAR.push_back(StTEMP); InCPAR.push_back(michi2stoi(StTEMP)); } // else {break;}
        //
        if(verbose>=4) {std::cout << "michi2DataClass: CrabStringReadInfo " << "# NPAR" << i << std::flush;}
        StTEMP = CrabStringReadInfo(StHeaderLines,michi2sprint("# NPAR",i,""));
        if(verbose>=4) {std::cout << ": " << StTEMP << std::endl;}
        if(!StTEMP.empty()) { StNPAR.push_back(StTEMP); InNPAR.push_back(michi2stoi(StTEMP)); } // else {break;}
        //
        if(verbose>=4) {std::cout << "michi2DataClass: CrabStringReadInfo " << "# TPAR" << i << std::flush;}
        StTEMP = CrabStringReadInfo(StHeaderLines,michi2sprint("# TPAR",i,""));
        if(verbose>=4) {std::cout << ": " << StTEMP << std::endl;}
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
    //
    // std::cout << "michi2DataClass: Good! Let's Go!" << std::endl;
    //
    // exam data info
    if(InNVAR.empty()) { // if no header info, then directly read this data file! (obs.dat)
        
        InNVAR.push_back(CrabTableGetLineCount(InputFile)); InCVAR.push_back(1); // set VAR1 size to LineCount, take first column for VAR1 values.
        InNVAR.push_back(1); InCVAR.push_back(2); // set VAR2 size to 1 (only one source or model), take second column for VAR2 values.
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
        this->Matched.resize(this->X.size(), 0); // this->Matched.assign(this->Y.size(),0); // * 0.0;
        
    } else { // otherwise read the data file according to header (lib.dat)
        
        if(InNVAR.size()!=2) { std::cout << "michi2DataClass: Error! Could not determine NVAR1 and NVAR2 from " << InputFile << "!" << std::endl; return; }
        // std::cout << "michi2DataClass: We will take column " << InCVAR[0] << " as X, column " << InCVAR[1] << " as Y, and column " << InCVAR[1]+1 << " as YErr if possible, from the file " << InputFile << std::endl;
        if(verbose>=1) {
            std::cout << "michi2DataClass: We will take column " << InCVAR[0] << " as X, column " << InCVAR[1] << " as Y, according to the header in the file " << InputFile << std::endl;
        }
        this->X.clear();
        this->Y.clear();
        
    }
    //
    // save
    this->CVAR = InCVAR;
    this->NVAR = InNVAR;
    this->CPAR = InCPAR; // a list of column number
    this->NPAR = InNPAR; // a list of number, InNPAR.size() == InNVAR[1].
    this->TPAR = StTPAR; // a list of string
    this->XNum = InNVAR[0]; // X data grid number in OBS file or in each model of a LIB file
    this->YNum = InNVAR[1]; // number of models in a LIB file
    this->XCol = InCVAR[0]; // X data array column number
    this->YCol = InCVAR[1]; // Y data array column number
    this->YErrCol = InCVAR[1]+1; // YError data array column number
    //
    // initialize YNorm and XNorm
    this->YNorm = nan("");
    this->XNorm = nan("");
    //
    // initialize VALUE4PARAMS and CHISQ4PARAMS (must before this->loadParameterValues();) <20181003> now no need, I implemented own data block reading code in loadParameterValues().
    this->VALUE4PARAMS.clear(); this->CHISQ4PARAMS.clear();
    for(int i=0; i<this->NPAR.size(); i++) {
        std::vector<double> CHISQ4PAR; CHISQ4PAR.clear();
        for(int j=0; j<this->NPAR[i]; j++) {
            CHISQ4PAR.push_back(nan("")); // CHISQ for each PAR, initialize with NAN (Not A Number)
        }
        this->VALUE4PARAMS.push_back(CHISQ4PAR);
        this->CHISQ4PARAMS.push_back(CHISQ4PAR);
    }
    //
    // initialize FPAR, parameter value array -- now in this->loadDataBlock();
    //this->FVAR.clear();
    //
    // initialize by reading first data block
    if(this->X.size()==0 || this->Y.size()==0) {
        this->X.resize(this->XNum, nan("")); // then we will not change the size of it
        this->Y.resize(this->XNum, nan("")); // then we will not change the size of it
        this->CurrentParameterValues.resize(this->CPAR.size(), nan("")); // then we will not change the size of it
        this->loadParameterValues(std::vector<int>(), verbose-1); // this initialization is important for chisq analysis, and must be done before loadDataBlock(0) otherwise this->X.size() and Y.size() will be both 1.
        this->loadDataBlock(0, verbose-1); // this initialization is important for constraints evaluate()
    }
    //
    // std::cout << "DEBUG: michi2DataClass this->X=0x" << std::hex << (size_t)&this->X << std::endl;
    // std::cout << "DEBUG: michi2DataClass this->XNum=0x" << std::hex << (size_t)&this->XNum << std::endl;
    // std::cout << "DEBUG: michi2DataClass this->Y=0x" << std::hex << (size_t)&this->Y << std::endl;
    // std::cout << "DEBUG: michi2DataClass this->YNum=0x" << std::hex << (size_t)&this->YNum << std::endl;
    //
    // initialize CurrentDataBlockIndex
    this->CurrentDataBlockIndexAsDouble = nan("");
    this->CurrentDataBlockIndex = -1; // no data block selected
    this->CurrentDataLineIndex = -1; // no data block selected
    this->TotalDataBlockNumber = this->YNum;
    this->TotalDataLineNumber = this->XNum * this->YNum;
    this->DataBlockIndexIsFreezed = false; // is the index of this LIB been freezed or not. will be set by user constraints
}


michi2DataClass::~michi2DataClass()
{
    if(this->FileStream) { if(this->FileStream->is_open()) { this->FileStream->close(); } }
    FilterCurveFilePath.clear(); FilePath.clear();
    X.clear(); Y.clear();
    this->VALUE4PARAMS.clear(); this->CHISQ4PARAMS.clear();
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


double michi2DataClass::michi2stod(string str)
{
    std::istringstream ssm(str); // stream used for the conversion
    double di = 0.0;
    ssm >> di;
    return di;
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







bool michi2DataClass::checkParameterConsistency(int debug)
{
    // here we will check parameter consistency
    bool ParConOK = true;
    long CheckNVAR2 = 1;
    for(int j=0; j<this->NPAR.size(); j++) {
        //
        CheckNVAR2 = CheckNVAR2 * this->NPAR.at(j);
        //
        long lineSpacing = this->getParameterLineSpacing(j); // each parameter has a certain lineSpacing if they are well formated.
        //
        // for example:
        //                   PAR1 PAR2 PAR3
        // data block 0:        0    0    0
        // data block 1:        0    0    1
        // data block 2:        0    1    0
        // data block 3:        0    1    1
        // data block 4:        0    2    0
        // data block 5:        0    2    1
        // data block 6:        0    3    0
        // data block 7:        0    3    1
        // data block 8:        1    0    0
        // data block 9:        1    0    1
        // data block 10:       1    1    0
        // data block 11:       1    1    1
        // data block 12:       1    2    0
        // data block 13:       1    2    1
        // data block 14:       1    3    0
        // data block 15:       1    3    1
        //                     [N]  [4]  [2]
        //
        long lineNumber = 1;
        if(lineSpacing <= 0) {
            std::cout << "michi2DataClass::checkParameterConsistency() Error! getParameterLineSpacing() returned " << lineSpacing << std::endl;
            ParConOK = false;
        } else {
            std::vector<double> fval = this->getParameterValues(j);
            if(fval.size() != this->NPAR.at(j)) {
                std::cout << "michi2DataClass::checkParameterConsistency() Error! getParameterValues("<<j<<").size() != NPAR.at("<<j<<")" << std::endl;
                ParConOK = false;
            } else {
                if(debug>=1) {
                    std::cout << "michi2DataClass::checkParameterConsistency() printing " << j+1 << "-th parameter " << this->TPAR.at(j) << " values: " << std::flush;
                }
                for(int k=0; k<fval.size(); k++) { // for each k-th value of j-th parameter, check k-th value does not equal (k-1)-th value
                    if(k>=1) {
                        if(std::fabs(fval[k]-fval[k-1]) < std::fabs(fval[k])*1e-30) {
                            ParConOK = false;
                        }
                        if(debug>=1) {
                            std::cout << ", " << std::flush;
                        }
                    }
                    if(debug>=1) {
                        std::cout << fval[k] << std::flush;
                    }
                }
                if(debug>=1) {
                    std::cout << std::endl;
                }
                if(!ParConOK) {
                    std::cout << "michi2DataClass::checkParameterConsistency() Error! There are some duplicated values!!" << std::endl;
                    ParConOK = false;
                }
            }
        }
    }
    if(CheckNVAR2 != this->YNum) {
        std::cout << "michi2DataClass::checkParameterConsistency() Error! Product of NPARs != LIB YNum!" << std::endl;
        ParConOK = false;
    }
    return ParConOK;
}


int michi2DataClass::setDataBlockBufferLimit(int InputDataBlockBufferLimit)
{
    this->DataBlockBufferLimit = InputDataBlockBufferLimit;
    return 0;
}

int michi2DataClass::loadFromDataBlockBuffer(long InputDataBlockIndex)
{
    int kFound = -1;
    for (int k=0; k<this->DataBlockBuffer.size(); k++) {
        if(InputDataBlockIndex == this->DataBlockBuffer[k].DataBlockIndex) {
            kFound = k;
            // update X Y and parameter data
            std::copy(this->DataBlockBuffer[k].X.begin(), this->DataBlockBuffer[k].X.end(), this->X.begin());
            std::copy(this->DataBlockBuffer[k].Y.begin(), this->DataBlockBuffer[k].Y.end(), this->Y.begin());
            this->FPAR = this->DataBlockBuffer[k].FPAR;
            for(int j=0; j<this->CPAR.size(); j++) {
                this->CurrentParameterValues[j] = this->DataBlockBuffer[k].FPAR[j][0];
            }
            // update CurrentDataBlockNumber to current data block index (model index)
            this->CurrentDataBlockIndex = InputDataBlockIndex; // index starts from 0, each data block contains this->XNum lines.
            this->CurrentDataBlockIndexAsDouble = (double)(InputDataBlockIndex);
            this->CurrentDataLineIndex = this->CurrentDataBlockIndex * this->XNum; // index starts from 0
            // return
            return 0;
        }
    }
    return -1;
}

int michi2DataClass::pushToDataBlockBuffer()
{
    DataBlockBufferStruct tmpBuffer;
    tmpBuffer.X = this->X;
    tmpBuffer.Y = this->Y;
    tmpBuffer.FPAR = this->FPAR;
    tmpBuffer.DataBlockIndex = this->CurrentDataBlockIndex;
    this->DataBlockBuffer.push_back(tmpBuffer);
    if(this->DataBlockBuffer.size() > this->DataBlockBufferLimit) {
        this->DataBlockBuffer.pop_front();
    }
    return 0;
}

int michi2DataClass::loadDataBlock(long InputDataBlockIndex, int debug)
{
    // load the i-th model, where i = InputDataBlockIndex (starting from 0) is the data block index
    if(InputDataBlockIndex < this->YNum) {
        // check buffer
        if(0 == this->loadFromDataBlockBuffer(InputDataBlockIndex)) {
            if(debug>=2) {std::cout << "michi2DataClass::loadDataBlock() Loaded data block at index " << InputDataBlockIndex << " with " << this->X.size() << " rows from DataBlockBuffer!" << std::endl;}
        } else {
            // get data block
            this->getDataBlockQuickQuick(InputDataBlockIndex * this->XNum + 1, this->XNum, debug-1); // lineNumber = DataBlockIndex * XNum + 1
            // push to buffer
            this->pushToDataBlockBuffer();
            if(debug>=2) {std::cout << "michi2DataClass::loadDataBlock() Pushed data block at index " << InputDataBlockIndex << " with " << this->X.size() << " rows to the DataBlockBuffer! DataBlockBuffer.size() = " << this->DataBlockBuffer.size() << std::endl;}
        }
    } else {
        std::cout << "michi2DataClass::loadDataBlock() Error! Failed to load data block at index " << InputDataBlockIndex << "!" << std::endl;
        return -1;
    }
    return 0;
}

std::vector<std::string> michi2DataClass::getDataBlock(long lineNumber, long lineCount, int debug)
{
    // get data block by inputting lineNumber and lineCount -- note that the input is not data block index
    // debug
    // int debug = 0;
    // debug time spent
    std::chrono::high_resolution_clock::time_point Timer1;
    std::chrono::high_resolution_clock::time_point Timer2;
    std::chrono::duration<double,std::milli> TimeSpan;
    if(debug>=3) {Timer1 = std::chrono::high_resolution_clock::now();}
    // prepare to read
    std::string backline;
    // skip header
    long backpoint = 0;
    long backshift = 0;
    long backwidth = 0;
    std::ifstream *backstory = (this->FileStream);
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
    if(debug>=2) { std::cout << "getDataBlock: " << "seeking to line " << lineNumber << " offset " << backshift << "+" << (lineNumber-1) << "*" << backwidth << "=" << backshift+(lineNumber-1)*backwidth << std::endl; }
    // read lines
    for(int j=0; j<lineCount; j++) {
        getline(*backstory,backline);
        textblock.push_back(backline);
        if(debug>=2) { std::cout << "getDataBlock: " << backline << std::endl; }
        
    }   if(debug>=2) { std::cout << "getDataBlock: " << std::endl; } // <TODO><DEBUG>
    // read data
    std::vector<std::string>  StFX = CrabStringReadColumn(textblock,this->XCol);
    std::vector<std::string>  StFY = CrabStringReadColumn(textblock,this->YCol);
    std::vector<double>       DbFX = michi2stod(StFX);
    std::vector<double>       DbFY = michi2stod(StFY);
    // save data
    std::copy(DbFX.begin(), DbFX.end(), this->X.begin()); if(DbFX.size() < this->X.size()) {std::fill(this->X.begin()+DbFX.size(), this->X.end(), 0.0);}
    std::copy(DbFY.begin(), DbFY.end(), this->Y.begin()); if(DbFY.size() < this->Y.size()) {std::fill(this->Y.begin()+DbFY.size(), this->Y.end(), 0.0);}
    this->XStr = StFX;
    this->YStr = StFY;
    this->Matched.resize(lineCount, 0); // this->Matched.assign(this->Y.size(),0); // * 0.0;
    if(debug>=2) { std::cout << "getDataBlock: " << "read " << this->X.size() << " lines" << std::endl; }
    // read parameter data
    this->FPAR.clear();
    for(int j=0; j<this->CPAR.size(); j++) {
        std::vector<std::string>  StPAR = CrabStringReadColumn(textblock,this->CPAR[j]);
        std::vector<double>       DbPAR = michi2stod(StPAR); // each FPAR is an array with the same size as X array.
        this->FPAR.push_back(DbPAR);
        this->CurrentParameterValues[j] = DbPAR[0];
    }
    // point CurrentDataBlockNumber to current data block index (model index)
    this->CurrentDataLineIndex = lineNumber-1; // index starts from 0
    this->CurrentDataBlockIndex = this->CurrentDataLineIndex / this->XNum; // index starts from 0, each data block contains this->XNum lines.
    this->CurrentDataBlockIndexAsDouble = (double)(CurrentDataBlockIndex);
    // debug time spent
    if(debug>=3) {Timer2 = std::chrono::high_resolution_clock::now(); TimeSpan = Timer2 - Timer1; std::cout << "getDataBlock: " << "spent " << TimeSpan.count()/1000.0 << " seconds" << std::endl;}
    // return
    return textblock;
}

std::vector<std::string> michi2DataClass::getDataBlockQuickQuick(long lineNumber, long lineCount, int debug)
{
    // note -- lineNumber starts from 1 and only counts data block lines, no commented header line accounted.
    // debug
    // int debug = 0;
    // debug time spent
    std::chrono::high_resolution_clock::time_point Timer1;
    std::chrono::high_resolution_clock::time_point Timer2;
    std::chrono::duration<double,std::milli> TimeSpan;
    if(debug>=2) {Timer1 = std::chrono::high_resolution_clock::now();}
    // prepare to read
    std::string backline;
    // skip header
    long backwidth = 0;
    this->FileStream->seekg(this->HeaderBytes);
    // get the first data line
    std::getline(*(this->FileStream), backline);
    // take the first data line width as the width for all data lines // <TODO> EACH DATA LINE SHOULD HAVE SAME BYTE WIDTH !
    backwidth = (long)this->FileStream->tellg() - this->HeaderBytes;
    // prepare text block
    std::vector<std::string> textblock;
    // seek to line i
    this->FileStream->seekg(this->HeaderBytes+(lineNumber-1)*backwidth);
    // debug
    if(debug>=2) { std::cout << "getDataBlock: " << "seeking to line " << lineNumber << " offset " << this->HeaderBytes+(lineNumber-1)*backwidth << std::endl; }
    // read lines
    std::vector<std::string> TmpXStr;
    std::vector<std::string> TmpYStr;
    for(int j=0; j<lineCount; j++) {
        std::getline(*(this->FileStream), backline);
        textblock.push_back(backline);
        std::vector<std::string> TmpStrVec = split(backline);
        TmpXStr.push_back(TmpStrVec[this->XCol-1]);
        TmpYStr.push_back(TmpStrVec[this->YCol-1]);
        //if(debug) { std::cout << "getDataBlock: " << backline << std::flush; }
    }   //if(debug) { std::cout << "getDataBlock: " << std::endl; }
    std::vector<double> DbFX = michi2stod(TmpXStr);
    std::vector<double> DbFY = michi2stod(TmpYStr);
    std::copy(DbFX.begin(), DbFX.end(), this->X.begin()); if(DbFX.size() < this->X.size()) {std::fill(this->X.begin() + DbFX.size(), this->X.end(), 0.0);}
    std::copy(DbFY.begin(), DbFY.end(), this->Y.begin()); if(DbFY.size() < this->Y.size()) {std::fill(this->Y.begin() + DbFY.size(), this->Y.end(), 0.0);}
    this->XStr = TmpXStr;
    this->YStr = TmpYStr; // <TODO> may have different size than this->X
    this->Matched.resize(lineCount, 0); // this->Matched.assign(this->Y.size(), 0); // * 0.0; // <BUG><FIXED><20181001><DZLIU>
    if(debug>=2) { std::cout << "getDataBlock: " << "read " << this->X.size() << " lines" << std::endl; }
    // read parameter data
    this->FPAR.clear();
    for(int j=0; j<this->CPAR.size(); j++) {
        std::vector<std::string>  StPAR = CrabStringReadColumn(textblock,this->CPAR[j]);
        std::vector<double>       DbPAR = michi2stod(StPAR); // each FPAR is an array with the same size as X array.
        this->FPAR.push_back(DbPAR);
        this->CurrentParameterValues[j] = DbPAR[0];
    }
    // point CurrentDataBlockIndex to current data block index (model index)
    this->CurrentDataLineIndex = lineNumber-1; // index starts from 0
    this->CurrentDataBlockIndex = this->CurrentDataLineIndex / this->XNum; // index starts from 0, each data block contains this->XNum lines.
    this->CurrentDataBlockIndexAsDouble = (double)(CurrentDataBlockIndex);
    // reset YNorm
    //this->XNorm = 1.0; // reset normalization before fitting
    //this->YNorm = 1.0; // reset normalization before fitting
    // debug time spent
    if(debug>=2) {Timer2 = std::chrono::high_resolution_clock::now(); TimeSpan = Timer2 - Timer1; std::cout << "getDataBlock: " << "spent " << TimeSpan.count()/1000.0 << " seconds" << std::endl;}
    // return
    return textblock;
}

long michi2DataClass::getParameterLineSpacing(long iParameter, int debug)
{
    long lineSpacing = -1;
    
    // check valid
    if(this->NPAR.size()<=0) {
        std::cout << "michi2DataClass::getParameterLineSpacing() Error! this->NPAR.size()=0! Please initialize this class!" << std::endl;
        return -1;
    }
    if(this->NPAR.size()<=iParameter) {
        std::cout << "michi2DataClass::getParameterLineSpacing() Error! Input iParameter too large!" << std::endl;
        return -1;
    }
    
    // multiply according to the order and number of PARAMS
    // for example,
    // LIB1[3] LIB2[4] LIB3[2]: 0 0 0
    // LIB1[3] LIB2[4] LIB3[2]: 0 0 1
    // LIB1[3] LIB2[4] LIB3[2]: 0 1 0
    // LIB1[3] LIB2[4] LIB3[2]: 0 1 1
    // LIB1[3] LIB2[4] LIB3[2]: 0 2 0
    // LIB1[3] LIB2[4] LIB3[2]: 0 2 1
    // LIB1[3] LIB2[4] LIB3[2]: 0 3 0
    // LIB1[3] LIB2[4] LIB3[2]: 0 3 1
    // LIB1[3] LIB2[4] LIB3[2]: 1 0 0
    // LIB1[3] LIB2[4] LIB3[2]: 1 0 1
    // LIB1[3] LIB2[4] LIB3[2]: 1 1 0
    // LIB1[3] LIB2[4] LIB3[2]: 1 1 1
    // LIB1[3] LIB2[4] LIB3[2]: 1 2 0
    // LIB1[3] LIB2[4] LIB3[2]: 1 2 1
    // LIB1[3] LIB2[4] LIB3[2]: 1 3 0
    // LIB1[3] LIB2[4] LIB3[2]: 1 3 1
    // LIB1[3] LIB2[4] LIB3[2]: 2 0 0
    // LIB1[3] LIB2[4] LIB3[2]: 2 0 1
    // LIB1[3] LIB2[4] LIB3[2]: 2 1 0
    // LIB1[3] LIB2[4] LIB3[2]: 2 1 1
    // LIB1[3] LIB2[4] LIB3[2]: 2 2 0
    // LIB1[3] LIB2[4] LIB3[2]: 2 2 1
    // LIB1[3] LIB2[4] LIB3[2]: 2 3 0
    // LIB1[3] LIB2[4] LIB3[2]: 2 3 1
    // for LIB1 the lineSpacing is 8 (=24/3), for LIB2 is 2 (=24/3/4), for LIB3 is 1.
    //-- although note that this requires the LIB file be well formatted, with parameters listed from left to right columns from lower to higher dimensions.
    long NumbParamMulti = 1;
    for(int i=0; i<=iParameter; i++) {
        NumbParamMulti = NumbParamMulti * this->NPAR[i];
    }
    lineSpacing = this->YNum / NumbParamMulti;
    lineSpacing = lineSpacing * this->XNum; // finally multiply the X array size of each data block
    return lineSpacing;
}


std::vector<double> michi2DataClass::getParameterValues(int iPar, int debug)
{
    std::vector<double> fval;
    if(this->NPAR.size() <= iPar) { return fval; }
    std::vector<int> iPars; iPars.push_back(iPar);
    this->loadParameterValues(iPars, debug); // <TODO> this will update this->VALUE4PARAMS[iPar]
    fval.resize(this->NPAR[iPar]); fval.assign(this->VALUE4PARAMS[iPar].begin(), this->VALUE4PARAMS[iPar].end());
    return fval;
}

void michi2DataClass::loadParameterValues(std::vector<int> iPars, int debug)
{
    // we will load all parameter values for all parameters
    // check valid
    if(this->NPAR.size()<=0) {
        std::cout << "michi2DataClass::loadParameterValues() Error! Please initialize this class!" << std::endl;
        return;
    }
    if(iPars.size() == 0) {
        for(int i=0; i<this->NPAR.size(); i++) {
            iPars.push_back(i);
        }
    }
    
    // get all parameter values by reading the first line of each data block
    for(const auto& i : iPars) { // loop all PARs
        long lineNumber = 1; // starting from the first line of the first data block.
        long lineCount = 1; // read only first line of each data block. each data block usually shares one parameter value.
        long lineSpacing = this->getParameterLineSpacing(i); // for i-th PAR, each two values have certain line spacing
        
        // prepare to read LIB file data blocks
        std::string backline;
        // skip header
        long backwidth = 0;
        this->FileStream->seekg(this->HeaderBytes); // New absolute position within the stream (relative to the beginning).
        // get the first data line // <TODO> this can be optimized as this is repeated for each i in iPars
        std::getline(*(this->FileStream), backline);
        // take the first data line width as the width for all data lines // <TODO> EACH DATA LINE SHOULD HAVE SAME BYTE WIDTH !
        backwidth = (long)this->FileStream->tellg() - this->HeaderBytes;
        
        for(int j=0; j<this->NPAR[i]; j++) { // loop all values for each PAR
            
            //std::vector<std::string> textblock = getDataBlockQuickQuick(lineNumber, lineCount, debug); //
            
            if(lineNumber > 1) {
                // seek to line i
                this->FileStream->seekg(this->HeaderBytes+(lineNumber-1)*backwidth);
                // debug
                if(debug>=2) { std::cout << "michi2DataClass::loadParameterValues() getDataBlock: " << "seeking to line " << lineNumber << " offset " << this->HeaderBytes+(lineNumber-1)*backwidth << std::endl; }
                // read one line
                std::getline(*(this->FileStream), backline);
            }
            
            if(backline.empty()) {
                std::cout << "michi2DataClass::loadParameterValues() Error! Failed to get lineNumber " << lineNumber << "!" << std::endl;
                return;
            }
            
            std::vector<std::string> TmpStrVec = split(backline);
            if(TmpStrVec.size() <= this->CPAR[i]-1) {
                std::cout << "michi2DataClass::loadParameterValues() Error! Failed to get correct columns at data lineNumber " << lineNumber << "!" << std::endl;
                return;
            }
            this->VALUE4PARAMS[i][j] = michi2stod(TmpStrVec[this->CPAR[i]-1]); // for i-th Parameter, take j-th value.
            
            lineNumber = lineNumber + lineSpacing;
        }
    }
}


void michi2DataClass::updateCurrentParameterValues(long iModel, int debug)
{
    if(this->VALUE4PARAMS.size()>0) {
        if(this->VALUE4PARAMS.size() == this->NPAR.size()) {
            for (int i=0; i<this->VALUE4PARAMS.size(); i++) { // for each parameter in this model
                long lineSpacing = this->getParameterLineSpacing(i); // for i-th PAR, each two values have certain line spacing
                long lineNumber = iModel * this->XNum + 1; // the iModel-th model in this LIB corresponds to this lineNumber
                long j = (lineNumber-1) / lineSpacing; // now we know that the iModel-th model corresponds to j-th parameter.
                // for example, PAR1 = 1 in DataBlock[0:10), 2 in DataBlock[10:20), 3 in DataBlock[20:30), PAR2 = 1-10 within each DataBlock[0:10). NPAR1 = 3, NPAR2 = 10.
                // we input iModel=25, so we have lineSpacing = 10 * XNum, lineNumber = 25 * XNum + 1, long j = 2. got PAR1[2].
                // we input iModel=20, so we have lineSpacing = 10 * XNum, lineNumber = 20 * XNum + 1, long j = 2. got PAR1[2].
                this->CurrentParameterValues[i] = this->VALUE4PARAMS[i][j];
            }
        }
    }
}


std::vector<int> michi2DataClass::convertIdToSubIdList(long IdModel)
{
    // this actually converts a data block index (starting from 0) to the list of parameter index, SubIdList (starting from 0)
    // for example, the k-th data block, index is k (starting from 0), it corresponds to, e.g., Par1[3], Par2[4], Par3[2], etc.
    
    std::vector<int> SubIdList;
    
    // check valid
    if(this->NPAR.size()<=0) {
        std::cout << "michi2DataClass::convertIdToSubIdList() Error! Please initialize this class!" << std::endl;
        return SubIdList;
    }
    
    // SubIdList is a list of indices for the k-th value of each parameter in a LIB file
    SubIdList.resize(this->NPAR.size(), -1);
    
    // compute the index for each LIB per OBS
    long NumbParamMulti = this->YNum;
    long iLoop = IdModel;
    for(int j=0; j<this->NPAR.size(); j++) {
        NumbParamMulti = NumbParamMulti / this->NPAR[j];
        SubIdList[j] = long(iLoop / NumbParamMulti);
        iLoop = iLoop % NumbParamMulti;
        // for example, we have three parameters:
        // PAR1[10], PAR2[4], PAR3[6]
        // iPar 0 == indices {0 0 0}
        // iPar 1 == indices {0 0 1}
        // iPar 5 == indices {0 0 5}
        // iPar 6 == indices {0 1 0}
    }
    return SubIdList;
}


long michi2DataClass::convertSubIdListToId(std::vector<int> SubIdList)
{
    // this actually converts a list of parameter index (starting from 0), SubIdList, to the data block index (starting from 0)
    // for example, if we know the current data block corresponds to, e.g., Par1[3], Par2[4], Par3[2], then we know that it is the k-th model in this data class.
    //
    
    long IdModel = -1;
    
    // check valid
    if(this->NPAR.size()<=0) {
        std::cout << "michi2DataClass::convertSubIdListToId() Error! Please initialize this class!" << std::endl;
        return -1;
    }
    if(SubIdList.size()!=this->NPAR.size()) {
        std::cout << "michi2DataClass::convertSubIdListToId() Error! The input SubIdList has wrong size!" << std::endl;
        return -1;
    }
    
    // IdModel = SubIdList[2] + NPAR3 * SubIdList[1] + NPAR3*NPAR2 * SubIdList[0]
    IdModel = 0;
    long NumbParamMulti = 1;
    for(int j=0; j<this->NPAR.size(); j++) {
        IdModel = IdModel + NumbParamMulti * SubIdList[this->NPAR.size()-1-j]; // j=0, SubIdList[2]
        NumbParamMulti = NumbParamMulti * this->NPAR[this->NPAR.size()-1-j]; // j=0, NPAR[2]
        //std::cout << "michi2DataClass::convertSubIdListToId() IdModel " << IdModel << " SubIdList[" << this->NPAR.size()-1-j << "] " << SubIdList[this->NPAR.size()-1-j] << std::endl;
    }
    return IdModel;
}


std::vector<std::string> michi2DataClass::split(const std::string &s)
{
    // copied from
    // -- https://stackoverflow.com/questions/9435385/split-a-string-using-c11
    // -- https://stackoverflow.com/questions/29441675/splitting-strings-separately-by-line-and-by-blank-space-using-getline?rq=1
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    while (ss >> item) {
        elems.push_back(item);
    }
    return elems;
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











