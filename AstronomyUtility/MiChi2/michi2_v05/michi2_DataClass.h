#ifndef H_michi2DataClass
#define H_michi2DataClass

#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <string.h>
#include <string>
#include <vector>
#include <deque>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono> // std::chrono, Timer1, Timer2, TimeSpan
//#include <regex> gcc >=4.9 (CentOS >=8)
//#include <iterator>
//#include "CrabStringClean.cpp"
//#include "CrabStringReadColumn.cpp"
//#include "CrabTableReadColumn.cpp"
//#include "CrabTableReadInfo.cpp"
//#include "CrabTableGetLineCount.cpp"
//#include "michi2_ParallelPool.h"
//#include "michi2_Constraint.h"

using namespace std;

//extern std::string CrabStringTrim(const string &t, int trimflag);



/* Struct Data Class */
/*
 -> XStr    the X array in string format, full size
 -> YStr    the Y array in string format, full size
 -> X       the X array that matches obs, the same size as obs data
 -> Y       the Y array that matches obs, the same size as obs data
 -> CVAR    the column number of variable X and Y
 -> CPAR    the column number of parameter 1,2,3,...
 -> NVAR    the number count of variable, should be 2 because there are only two variable X and Y
 -> NPAR    the number count of parameters, can be any >=1, e.g. 2 if we have T_kin and n_H_2 when doing LVG modelling
 */
class michi2DataClass {
public:
    std::vector<double> X;    std::vector<double> Y;    std::vector<double>  YErr;     std::vector<int>  Matched;
    std::vector<string> XStr; std::vector<string> YStr; std::vector<string>  YErrStr;  long XCol; long YCol; long YErrCol; long XNum; long YNum;
    std::vector<std::vector<double> > FVAR;             std::vector<long>    NVAR;     std::vector<long> CVAR;
    std::vector<std::vector<double> > FPAR;             std::vector<long>    NPAR;     std::vector<long> CPAR;   std::vector<string> TPAR;
    std::ifstream                    *FileStream;       std::string          FilePath;
    std::vector<std::vector<double> > CHISQ4PARAMS; // chisq for each PAR
    std::vector<std::vector<double> > VALUE4PARAMS; // value for each PAR
    double YNorm; // Normalization factor multiplied to Y, obtained after each fitting to the data. It may change when looping each model.
    double XNorm; // Normalization factor multiplied to X, obtained after each fitting to the data. It may change when looping each model.
    std::vector<long> XMatchIndex; // <20190126> since all lib templates have the same X grid, we can store the obs-lib matching index in this XMatchIndex so that we do not run michi2MatchObs for each lib template.
    double XMin, XMax;
    long CurrentDataBlockIndex;
    long TotalDataBlockNumber;
    long CurrentDataLineIndex;
    long TotalDataLineNumber;
    double CurrentDataBlockIndexAsDouble; // for exprtk::...<double>
    bool DataBlockIndexIsFreezed;
    std::vector<double> CurrentParameterValues; // a copy of FPAR[iPar][0] of CurrentDataBlockIndex. will be updated when getDataBlock() or loadDataBlock().
    
    struct DataBlockBufferStruct {std::vector<double> X; std::vector<double> Y; std::vector<std::vector<double> > FPAR; long DataBlockIndex;}; // buffer for each LIB
    std::deque<DataBlockBufferStruct> DataBlockBuffer; // LIB index, data block index.
    int DataBlockBufferLimit = 100;
    
    long HeaderLines;
    long HeaderBytes;
    // FVAR[0] = X,    FVAR[1] = Y
    // FPAR[0] = PAR1, FPAR[1] = PAR2, ...
    std::vector<string> FilterCurveFilePath; // <added><20171001>
    michi2DataClass(const char * InputFile, int verbose = 0);
    ~michi2DataClass();
    const char *michi2sprint(const char* wsPrefix, long wi, const char* wsSuffix);
    int michi2stoi(string str);
    int michi2wstoi(wstring wstr);
    double michi2stod(string strVec);
    std::vector<double> michi2stod(std::vector<string> strVec);
    std::vector<double> michi2wstod(std::vector<wstring> wstrVec);
    std::vector<std::string> split(const std::string &s);
    std::vector<std::string> getDataBlock(long lineNumber, long lineCount, int debug = 0); // this function loads data block into this->X, this->Y
    std::vector<std::string> getDataBlockQuickQuick(long lineNumber, long lineCount, int debug = 0); // this function DOES load data block into this->X, this->Y
    bool checkParameterConsistency(int debug = 1);
    int setDataBlockBufferLimit(int InputDataBlockBufferLimit);
    int loadFromDataBlockBuffer(long InputDataBlockIndex);
    int pushToDataBlockBuffer();
    int loadDataBlock(long InputDataBlockIndex, int debug = 0);
    long getParameterLineSpacing(long iParameter, int debug = 0);
    std::vector<double> getParameterValues(int iPar, int debug = 0); // get values for one parameter
    void loadParameterValues(std::vector<int> iPars = std::vector<int>(), int debug = 0);
    void updateCurrentParameterValues(long iModel, int debug = 0);
    std::vector<int> convertIdToSubIdList(long IdModel);
    long convertSubIdListToId(std::vector<int> SubIdList);
    std::vector<std::string> readFilterCurveListFile(const char * InputFile);
};



/* Struct For Parallel */
struct mnchi2parallelParams {
    std::vector<michi2DataClass *> SDOBSList;
    std::vector<michi2DataClass *> SDLIBList;
    std::vector<std::ofstream *> SDOUTList;
    std::vector<std::string> InputObsList; // InputObsList file names
    std::vector<std::string> InputLibList; // InputLibList file names
    std::vector<std::string> OutputTableList;
    
    //long iiPool; //  current index in Pool->IdPool[] -- Pool[iiPool] = iPool.
    long long iPool; //  corresponding Id in Pool->IdPool[]. SubIdList = convertIdToSubIdList(iPool).
    int iObs; // OBS index in SDOBSList
    std::vector<int> iLib; // list of LIB index in SDLIBList. iPool = convertSubIdListToId(SubIdList) = convertLibIdListToId(iLib, iObs).
    
    long nObs; long nLib; long nRow; long nModel; long nTotal;
    int DebugLevel=0;
    
    //michi2ParallelPool *Pool; // Pool of indices of OBS and LIB data array
    void *Pool; // Pool of indices of OBS and LIB data array
    std::stringstream LoopInfo;
    std::string LoopInfoStr;
    
    
    
    // example (pseudo code):
    // mnchi2parallelIndex.i = 0
    // mnchi2parallelIndex.InputObsList = {"my-obs1.dat" "my-obs2.dat"}
    // mnchi2parallelIndex.iObsList = {0 -1} // use "my-obs1.dat"
    // mnchi2parallelIndex.InputLibList = {"component1.lib" "component2.lib" "component3.lib"}
    // mnchi2parallelIndex.iLibList = {3 5 0} // use model at row 4, 6 and 1 of each "component*.lib".
    
    /*
    long  ibOBS;     //    begin index of OBS list
    long  idOBS;     //          index of OBS list
    long  ieOBS;     //      end index of OBS list
    bool  irOBS;     // rounding index of OBS list 就是计算是否进位
    std::vector<long> ibLIBList; //    begin index of LIB list
    std::vector<long> idLIBList; //          index of LIB list
    std::vector<long> ieLIBList; //      end index of LIB list
    std::vector<bool> irLIBList; // rounding index of LIB list 就是计算是否进位
    
    std::vector<std::vector<std::vector<double> > > chisqListListList; // the fitted chisq list of each Par in Par list of each Lib in Lib list
    std::vector<std::vector<std::vector<long> > > iLibParListListList; // the looping index list of each Par in Par list of each Lib in Lib list
    std::vector<std::vector<long> > iLibParBeginList;  // begin of the index of each Par in each Lib <20180224>
    std::vector<std::vector<long> > iLibParStepList;  // step of the index of each Par in each Lib <20180224>
    std::vector<std::vector<long> > iLibParEndList;  // end of the index of each Par in each Lib <20180224>
    
    long iBegin; long iEnd;
    */
};





#endif
