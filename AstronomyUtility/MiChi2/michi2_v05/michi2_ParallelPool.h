#ifndef H_michi2ParallelPool
#define H_michi2ParallelPool

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include "michi2_DataClass.h"
#include "michi2_Constraint.h"

using namespace std;





/* Struct For Parallel */
class michi2ParallelPool {
public:
    std::vector<michi2DataClass *>  SDOBSList;
    std::vector<michi2DataClass *>  SDLIBList;
    std::vector<std::ofstream *>    SDOUTList;
    
    long NumbOfAllLoops; // total number of loops.
    std::vector<std::vector<int> > SubIdListPool; // pool for SubIdLists (iLibs).
    std::vector<long long> IdPool; // list of unprocessed Ids, where Id can be converted to SubIdList by convertIdToSubIdList().
    std::vector<std::vector<int> > SubIdListDone; // list of processed SubIdLists (iLibs).
    std::vector<long long> IdDone; // list of processed Ids, where Id can be converted to SubIdList by convertIdToSubIdList().
    std::vector<int> iObsPool; // pool of OBS indices
    
    std::vector<michi2Constraint *> FreeConstraints; // free INDEX
    std::vector<michi2Constraint *> FreezedConstraints; // freezed INDEX
    std::vector<michi2Constraint *> OtherConstraints; // other NORM, PAR, etc.
    
    std::vector<bool> SubIdFree; // a list of boolean values indicating which SubId is free. SubId means iObs, iLib1, iLib2, ...
    
    int nObs;
    int nLib;
    //std::vector<long> nLibModels;
    long nRow; long nModel; long nTotal; // nModel counts only free models (product of model number in all free LIBs). nTotal = nObs * nModel. 
    int DebugLevel;
    
    int NumbParallel;
    std::vector<struct mnchi2parallelParams *> ParamsList; // each element is a pParams struct for one subprocess.
    
    std::vector<std::vector<std::vector<std::vector<double> > > > CHISQ4PARAMS; // CHISQ for each LIB parameter in each LIB for each OBS
    std::vector<std::vector<std::vector<std::vector<double> > > > VALUE4PARAMS; // parameter value for each LIB parameter in each LIB for each OBS
    
    std::vector<std::vector<std::vector<int > > > ARGMINPARAMS; // minimum arg of each param
    
    michi2ParallelPool();
    ~michi2ParallelPool();
    void clear();
    void initialize(std::vector<michi2DataClass *> SDOBSList,
                    std::vector<michi2DataClass *> SDLIBList,
                    std::vector<std::ofstream *> SDOUTList,
                    std::vector<michi2Constraint *> Constraints,
                    int NumbParallel = 2,
                    int DebugLevel = 0
                    );
    void createProcesses(int NumbParallel,
                         int DebugLevel = 0
                         );
    
    int convertIdPoolToIdObs(long long iPool, int debug = 0);
    std::vector<int> convertIdPoolToIdLibList(long long iPool, int debug = 0);
    long long convertIdLibListToIdPool(std::vector<int> iLib, int iObs = 0, int debug = 0);
    
    std::vector<int> convertIdToSubIdList(long long iPool, std::vector<bool> SubIdFree = std::vector<bool>(), int debug = 0);
    long long convertSubIdListToId(std::vector<int> SubIdList, std::vector<bool> SubIdFree = std::vector<bool>(), int debug = 0);
    
    std::vector<int> increaseSubIdList(std::vector<int> SubIdList, std::vector<int> SubIdNumb, std::vector<bool> SubIdFree = std::vector<bool>(), int increment = 1);
    
    long findIndexOfClosestIdInPool(long long iPool, int direction = 1, int debug = 0);
    long long findClosestIdInPool(long long iPool);
    bool removeIdInPool(long long iPool);
    void printIdPool();
    
};






#endif
