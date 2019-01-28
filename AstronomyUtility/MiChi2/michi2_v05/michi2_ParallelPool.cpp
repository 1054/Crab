#include "michi2_ParallelPool.h"
//#include "CrabStringClean.cpp"
//#include "CrabStringReadInfo.cpp"
//#include "CrabStringReadColumn.cpp"
//#include "CrabTableReadColumn.cpp"
//#include "CrabTableGetLineCount.cpp"


michi2ParallelPool::michi2ParallelPool()
{
    this->clear();
}



michi2ParallelPool::~michi2ParallelPool()
{
    this->clear();
    /*
     for(int i=0; i<this->SDOUTList.size(); i++) {
     SDOUTList[i]->close();
     SDOUTList[i] = NULL;
     }
     */
}



void michi2ParallelPool::clear()
{
    this->SDOBSList.clear();
    this->SDLIBList.clear();
    this->SDOUTList.clear();
    this->NumbOfAllLoops = 0;
    this->IdPool.clear();
    this->IdDone.clear();
    this->SubIdListPool.clear();
    this->SubIdListDone.clear();
    this->ParamsList.clear();
    this->DebugLevel = 0;
    this->NumbParallel = 2;
    this->nObs = 0;
    this->nLib = 0;
    this->nRow = 0;
    this->nModel = 0;
    this->nTotal = 0;
}



void michi2ParallelPool::initialize(std::vector<michi2DataClass *> SDOBSList,
                                    std::vector<michi2DataClass *> SDLIBList,
                                    std::vector<std::ofstream *> SDOUTList,
                                    std::vector<michi2Constraint *> Constraints,
                                    int NumbParallel,
                                    int DebugLevel
                                    )
{
    std::cout << "michi2ParallelPool::initialize() ..." << std::endl;
    this->clear();
    this->SDOBSList = SDOBSList;
    this->SDLIBList = SDLIBList;
    this->SDOUTList = SDOUTList;
    this->nObs = this->SDOBSList.size();
    this->nLib = this->SDLIBList.size();
    for (int i=0; i<this->SDOBSList.size(); i++) { this->iObsPool.push_back(i); }
    //
    // prepare vectors for looping each OBS-LIB-model combination
    std::vector<int> iLoopSubIdList; iLoopSubIdList.resize(1 + this->nLib, 0); // one Id of OSB, then many Ids for each LIB.
    std::vector<int> iLoopSubIdNumb; iLoopSubIdNumb.push_back(this->nObs); for(int i=0; i<this->nLib; i++) {iLoopSubIdNumb.push_back(this->SDLIBList[i]->YNum);} //
    long iLoopIdPoolNumb = 1; for(int i=0; i<this->nLib; i++) {iLoopIdPoolNumb = iLoopIdPoolNumb * this->SDLIBList[i]->YNum;} iLoopIdPoolNumb = iLoopIdPoolNumb * this->nObs; //
    if(DebugLevel>=3) { std::cout << "michi2ParallelPool::initialize() iLoopSubIdNumb = "; for(int i=0; i<iLoopSubIdNumb.size(); i++) {std::cout << iLoopSubIdNumb[i] << " ";} std::cout << std::endl; }
    //
    //
    // check constraints on INDEX, to see if any LIB INDEX has been assigned by other LIB INDEX so that we freeze them.
    // determine this->SubIdFree
    this->SubIdFree.clear(); this->SubIdFree.resize(1 + this->nLib, true); // boolean value to mark whether the INDEX is free or not free (i.e., depends on constraints with other INDEX or constants).
    this->FreeConstraints.clear(); // Constraints on INDEX which are assignments
    this->FreezedConstraints.clear(); // Constraints on INDEX which are not assignments
    this->OtherConstraints.clear(); // Constraints not on INDEX
    for(int iCon = 0; iCon < Constraints.size(); iCon++) {
        if(DebugLevel>=4) { std::cout << "michi2ParallelPool::initialize() checking constraint: \"" << Constraints[iCon]->ConstraintStr << "\"" << "(Type: " << (int)(Constraints[iCon]->ConstraintType) << ")" << std::endl; }
        if(Constraints[iCon]->ConstraintType == michi2Constraint::EnumConstraintType::INDEX && Constraints[iCon]->IsAssignment == true) {
            if(DebugLevel>=4) { std::cout << "michi2ParallelPool::initialize() got a freezed INDEX constraint: \"" << Constraints[iCon]->ConstraintStr << "\""; }
            // if the constraint is assigning a value to one LIB as the INDEX, then we mark the LIB INDEX not free
            for (int ick=0; ick<Constraints[iCon]->AssignmentsLibIndices.size(); ick++) {
                this->SubIdFree[1+Constraints[iCon]->AssignmentsLibIndices[ick]] = false; // this->SubIdFree[0] is for OBS, this->SubIdFree[1:nLib] is for each LIB. so here is "AssignmentsLibIndices[ick]+1".
                if(DebugLevel>=4) { std::cout << " ... will assign to LIB" << Constraints[iCon]->AssignmentsLibIndices[ick]+1 << "_INDEX ... "; }
            }
            this->FreezedConstraints.push_back(Constraints[iCon]); // append to this->FreezedConstraints
            if(DebugLevel>=4) { std::cout << std::endl; }
        } else if(Constraints[iCon]->ConstraintType == michi2Constraint::EnumConstraintType::INDEX && Constraints[iCon]->IsAssignment == false) {
            // if this constraint is a boolean check to INDEX, then we still set the LIB INDEX to be free, and we will check them in the loop
            if(DebugLevel>=4) { std::cout << "michi2ParallelPool::initialize() got a free INDEX constraint: \"" << Constraints[iCon]->ConstraintStr << "\""; }
            this->FreeConstraints.push_back(Constraints[iCon]); // append to this->FreeConstraints
            if(DebugLevel>=4) { std::cout << std::endl; }
        } else if(Constraints[iCon]->ConstraintType == michi2Constraint::EnumConstraintType::PAR && Constraints[iCon]->IsAssignment == false) {
            // if this constraint is a boolean check to LIB PAR, then we still also check them in the loop
            if(DebugLevel>=4) { std::cout << "michi2ParallelPool::initialize() got a LIB PAR constraint: \"" << Constraints[iCon]->ConstraintStr << "\""; }
            this->FreeConstraints.push_back(Constraints[iCon]); // also append to this->FreeConstraints
            if(DebugLevel>=4) { std::cout << std::endl; }
        } else {
            if(DebugLevel>=4) { std::cout << "michi2ParallelPool::initialize() put this constaint for later check: \"" << Constraints[iCon]->ConstraintStr << "\"" << std::endl; }
            this->OtherConstraints.push_back(Constraints[iCon]); // append to this->OtherConstraints, which will be hand over to subprocesses
        }
    }
    if(DebugLevel>=3) { std::cout << "michi2ParallelPool::initialize() done with INDEX constaints!" << std::endl; }
    //
    // loop each LIB and multiply the model numbers to get a total combination number.
    this->nRow = 1; // product of all LIB rows (models), including those in freezed LIBs.
    for (int i=0; i<this->SDLIBList.size(); i++) {
        if(this->SDLIBList[i]->YNum > 0) { // count all rows including freezed and free rows (one row is one model)
            this->nRow = this->nRow * this->SDLIBList[i]->YNum; // YNum is the number of models in each LIB
        }
    }
    this->nModel = 1; // product of all free LIB rows (models)
    for (int i=0; i<this->SDLIBList.size(); i++) {
        if(this->SDLIBList[i]->YNum > 0 && this->SubIdFree[1+i]) { // only count free models, SubIdFree[1:nLib] are for LIBs.
            this->nModel = this->nModel * this->SDLIBList[i]->YNum; // YNum is the number of models in each LIB
        }
    }
    this->nTotal = this->nObs * this->nModel; // count total OSB * free-LIB-model combinations
    if(DebugLevel>=2) { std::cout << "michi2ParallelPool::initialize() this->nObs = " << this->nObs << std::endl; }
    if(DebugLevel>=2) { std::cout << "michi2ParallelPool::initialize() this->nRow = " << this->nRow << std::endl; }
    if(DebugLevel>=2) { std::cout << "michi2ParallelPool::initialize() this->nModel = " << this->nModel << std::endl; }
    if(DebugLevel>=2) { std::cout << "michi2ParallelPool::initialize() this->nTotal = " << this->nTotal << std::endl; }
    //
    //
    // loop each OBS-LIB-model combination
    int DebugPerLoopNumber = this->nTotal/20; // if(DebugLevel>=4) {DebugPerLoopNumber = 1;}
    int DebugPerLoopDigits = (int)(std::ceil(log10((double)(this->nTotal))));
    int DebugPerLoopDigits2 = (int)(std::ceil(log10((double)(iLoopIdPoolNumb))));
    std::stringstream DebugPerLoopInfo; DebugPerLoopInfo.clear();
    long iLoop = 0;
    long long iPool = this->convertSubIdListToId(iLoopSubIdList);
    while(iLoop >= 0 && iLoop < this->nTotal) {
        //
        // print loop info
        if(DebugLevel>=1 && iLoop%DebugPerLoopNumber==0) {
            DebugPerLoopInfo.str(""); // clear debug info
            DebugPerLoopInfo << "michi2ParallelPool::initialize() iLoop = " << setw(DebugPerLoopDigits) << std::right << iLoop << "/" << std::left << this->nTotal << " ";
            DebugPerLoopInfo << " iPool = " << setw(DebugPerLoopDigits2) << std::right << iPool << "/" << std::left << iLoopIdPoolNumb << " ";
            for(int k=0; k<iLoopSubIdList.size(); k++) {
                int ndigits = (int)(std::ceil(log10((double)(iLoopSubIdNumb[k]))));
                DebugPerLoopInfo << "[" << k << "]=" << std::setw(ndigits) << std::right << iLoopSubIdList[k] << "/" << iLoopSubIdNumb[k];
                if(k<iLoopSubIdList.size()-1) {DebugPerLoopInfo << " ";}
            }
            std::cout << DebugPerLoopInfo.str() << std::endl;
        }
        
        //
        // update current data block index for each LIB
        for(int i = 0; i < this->SDLIBList.size(); i++) { // iLoopSubIdList[0] is not for LIB, iLoopSubIdList[1:nLib] are for LIB.
            this->SDLIBList[i]->CurrentDataBlockIndex = iLoopSubIdList[1+i]; //<20181002><dzliu> do not replace by loadDataBlock().
            this->SDLIBList[i]->CurrentDataBlockIndexAsDouble = (double)(iLoopSubIdList[1+i]); //<20181002><dzliu> do not replace by loadDataBlock().
            //this->SDLIBList[i]->loadDataBlock(iLoopSubIdList[i+1], DebugLevel-2); //<20181002b><dzliu> speed up by replacing calling loadDataBlock() with directy manipulating SDLIBList[i]->CurrentDataBlockIndexAsDouble
        }
        
        //
        // apply freezed constraints on INDEX (i.e., assignment constraints)
        for(int i=0; i<this->FreezedConstraints.size(); i++) {
            //this->FreezedConstraints[i]->recompile(); //<20181002> no need to recompile now, exprtk issue has been fixed. see test under "test_michi2Constraint_expr2/" folder.
            this->FreezedConstraints[i]->evaluate(); // evaluate the expression, this->SDLIBList[j]->CurrentDataBlockIndexAsDouble will be updated
            for(int j=0; j<this->FreezedConstraints[i]->AssignmentsLibIndices.size(); j++) {
                int k = this->FreezedConstraints[i]->AssignmentsLibIndices[j]; if(k<0) {continue;} // the LIB index which will be assigned an INDEX
                long assigned_index = (long)(this->SDLIBList[k]->CurrentDataBlockIndexAsDouble); // the new INDEX computed by user constraint
                this->SDLIBList[k]->CurrentDataBlockIndex = assigned_index; //<20181002><dzliu> replaced by loadDataBlock().
                //this->SDLIBList[k]->loadDataBlock(assigned_index, DebugLevel-2); //<20181002b><dzliu> speed up by replacing calling loadDataBlock() with directy manipulating SDLIBList[i]->CurrentDataBlockIndexAsDouble
                iLoopSubIdList[1+k] = this->SDLIBList[k]->CurrentDataBlockIndex; // iLoopSubIdList[0] is OBS, iLoopSubIdList[1:nLib] is for LIBs!
                if(DebugLevel>=2 && iLoop%DebugPerLoopNumber==0) {
                    std::cout << DebugPerLoopInfo.str() << " ... constraint \"" << this->FreezedConstraints[i]->ExpressionStr << "\"" << " is applied to LIB" << k+1 << "_INDEX ... " << this->SDLIBList[k]->CurrentDataBlockIndex << std::endl;
                }
            }
        }
        //iLoop = this->convertSubIdListToId(iLoopSubIdList);
        
        //
        // check free constraints on INDEX or LIB_PAR (i.e., non-assignment constraints)
        bool ConstraintOK = true;
        if(1) { // <TODO> we either check it here or check it later inside each subprocess
            for(int i = 0; i < this->FreeConstraints.size(); i++) {
                if(ConstraintOK) {
                    //this->FreeConstraints[i]->recompile(); //<20181002> no need to recompile now, exprtk issue has been fixed. see test under "test_michi2Constraint_expr2/" folder.
                    for(int j=0; j<this->FreeConstraints[i]->VariablesLibIndices.size(); j++) {
                        //this->SDLIBList[j]->loadParameterValues();
                        int k = this->FreeConstraints[i]->VariablesLibIndices[j];
                        this->SDLIBList[k]->updateCurrentParameterValues(iLoopSubIdList[1+k]);
                    }
                    ConstraintOK = (this->FreeConstraints[i]->evaluate() > 0);
                    if(DebugLevel>=2 && iLoop%DebugPerLoopNumber==0) {
                        std::cout << DebugPerLoopInfo.str() << " ... constraint \"" << this->FreeConstraints[i]->ExpressionStr << "\" is being evaluated ... ";
                        if(ConstraintOK) {std::cout << "passed!" << std::endl;} else {std::cout << "BLOCKED!" << std::endl;}
                    }
                    if(DebugLevel>=3 && iLoop%DebugPerLoopNumber==0) {
                        std::cout << this->FreeConstraints[i]->printVariablesWithValues() << std::endl;
                    }
                }
            }
        }
        if(ConstraintOK) { // add to Pool
            iPool = this->convertSubIdListToId(iLoopSubIdList);
            this->IdPool.push_back(iPool);
            this->SubIdListPool.push_back(iLoopSubIdList);
            this->NumbOfAllLoops++;
            //
            // print loop info again
            if(DebugLevel>=0 && iLoop%DebugPerLoopNumber==0) {
                DebugPerLoopInfo.str(""); // clear debug info
                DebugPerLoopInfo << "michi2ParallelPool::initialize() iLoop = " << setw(DebugPerLoopDigits) << std::right << iLoop << "/" << std::left << this->nTotal << " ";
                DebugPerLoopInfo << " iPool = " << setw(DebugPerLoopDigits2) << std::right << iPool << "/" << std::left << iLoopIdPoolNumb << " ";
                for(int k=0; k<iLoopSubIdList.size(); k++) {
                    int ndigits = (int)(std::ceil(log10((double)(iLoopSubIdNumb[k]))));
                    DebugPerLoopInfo << "[" << k << "]=" << std::setw(ndigits) << std::right << iLoopSubIdList[k] << "/" << iLoopSubIdNumb[k];
                    if(k<iLoopSubIdList.size()-1) {DebugPerLoopInfo << " ";}
                }
                std::cout << DebugPerLoopInfo.str() << " ... added into Pool!" << std::endl;
            }
        }
        
        //
        // adding one to the last SubId and update iLoop for next loop
        iLoopSubIdList = this->increaseSubIdList(iLoopSubIdList, iLoopSubIdNumb, this->SubIdFree, 1);
        iLoop++;
        
        //
        // go to next loop
        
    }
    
    
    if(DebugLevel>=4) {
        int ndigits = (int)(std::ceil(log10((double)(this->IdPool[-1]))));
        for(int i=0; i<this->IdPool.size(); i+=10) {
            if(i==0) {std::cout << "michi2ParallelPool::initialize() IdPool = ";}
            else {std::cout << "                                          ";}
            for(int j=0; j<10; j++) {
                if(i+j < this->IdPool.size()) {
                    std::cout << std::setw(ndigits) << std::right << this->IdPool[i+j] << " ";
                }
            }
            std::cout << std::endl;
        }
    }
    
    // createProcesses
    this->createProcesses(NumbParallel, DebugLevel);
    
}



void michi2ParallelPool::createProcesses(
                                         int NumbParallel,
                                         int DebugLevel
                                         )
{
    std::cout << "michi2ParallelPool::createProcesses("<<NumbParallel<<") ..." << std::endl;
    
    // check this->nObs
    if(this->nObs <= 0) {
        std::cout << "michi2ParallelPool::createProcesses() Error! Please run michi2ParallelPool::initialize() first!" << std::endl;
        return;
    }
    
    // initialize pParams for NumbParallel processes
    if(NumbParallel>0) {
        this->NumbParallel = NumbParallel;
    }
    if(this->NumbParallel > this->nTotal) {
        this->NumbParallel = this->nTotal; // if there are not so many OBS*LIB combination as NumbParallel, we limit NumbParallel to the number of OBS*LIB
    }
    this->ParamsList.clear();
    for(int iParallel=0; iParallel<this->NumbParallel; iParallel++) {
        //
        // create subprocess structure
        if(DebugLevel>=2) { std::cout << "michi2ParallelPool: Creating new struct mnchi2parallelParams" << std::endl; }
        struct mnchi2parallelParams *pParams = new struct mnchi2parallelParams;
        //
        // all subprocesses share the same OBS data structure (only for reading purpose)
        pParams->SDOBSList = this->SDOBSList;
        //
        // prepare separated LIB data structure, otherwise error occurs
        pParams->SDLIBList.clear();
        pParams->SDLIBList.resize(this->SDLIBList.size());
        for(int i=0; i<this->SDLIBList.size(); i++) {
            if(DebugLevel>=2) { std::cout << "michi2ParallelPool: Reading SDLIB " << i+1 << " from " << "\"" << SDLIBList[i]->FilePath << "\"" << std::endl; }
            michi2DataClass *SDLIB = new michi2DataClass(this->SDLIBList[i]->FilePath.c_str(), DebugLevel-1);
            SDLIB->loadDataBlock(0, DebugLevel-2); // intialize with 0-th DataBlock // <TODO> now implemented inside `new michi2DataClass()`
            pParams->SDLIBList[i] = SDLIB;
            if(DebugLevel>=5) { std::cout << "michi2ParallelPool: Read SDLIB " << i+1 << ", SDLIB = 0x" << std::hex << (size_t)(SDLIB) << std::dec << ", &(SDLIB->X[0]) = 0x" << std::hex << (size_t)(&(SDLIB->X[0])) << std::dec << ", SDLIB->X.size() = " << SDLIB->X.size() << ", &(SDLIB->Y[0]) = 0x" << std::hex << (size_t)(&(SDLIB->Y[0])) << std::dec << ", SDLIB->Y.size() = " << SDLIB->Y.size() << std::endl; }
            if(DebugLevel>=5) { std::cout << "michi2ParallelPool: Read SDLIB " << i+1 << ", pParams->SDLIBList["<<i<<"] = 0x" << std::hex << (size_t)(pParams->SDLIBList[i]) << std::dec << ", &(pParams->SDLIBList["<<i<<"]->X[0]) = 0x" << std::hex << (size_t)(&(pParams->SDLIBList[i]->X[0])) << std::dec << ", pParams->SDLIBList["<<i<<"]->X.size() = " << pParams->SDLIBList[i]->X.size() << ", &(pParams->SDLIBList["<<i<<"]->Y[0]) = 0x" << std::hex << (size_t)(&(pParams->SDLIBList[i]->Y[0])) << std::dec << ", pParams->SDLIBList["<<i<<"]->Y.size() = " << pParams->SDLIBList[i]->Y.size() << std::endl; }
        }
        //
        // prepare CHISQ array for each parameter of each SDLIB of each SDOBS (4-level vector)
        this->CHISQ4PARAMS.clear();
        this->VALUE4PARAMS.clear();
        this->ARGMINPARAMS.clear();
        this->CHISQ4PARAMS.resize(this->SDOBSList.size());
        this->VALUE4PARAMS.resize(this->SDOBSList.size());
        this->ARGMINPARAMS.resize(this->SDOBSList.size());
        for(int i=0; i<pParams->SDOBSList.size(); i++) {
            // for i-th OBS
            this->CHISQ4PARAMS[i].clear();
            this->CHISQ4PARAMS[i].resize(pParams->SDLIBList.size());
            this->VALUE4PARAMS[i].clear();
            this->VALUE4PARAMS[i].resize(pParams->SDLIBList.size());
            this->ARGMINPARAMS[i].clear();
            this->ARGMINPARAMS[i].resize(pParams->SDLIBList.size());
            for(int j=0; j<pParams->SDLIBList.size(); j++) {
                // for j-th LIB
                this->CHISQ4PARAMS[i][j].resize(pParams->SDLIBList[j]->NPAR.size());
                this->VALUE4PARAMS[i][j].resize(pParams->SDLIBList[j]->NPAR.size());
                this->ARGMINPARAMS[i][j].resize(pParams->SDLIBList[j]->NPAR.size());
                //pParams->SDLIBList[j]->loadParameterValues(); // load parameter value array and initialize chis-quare arrays, this requires reading data block and may take some time -- <20181002><BUG><FIXED><DZLIU> should not be done here! should be done when newing SDLIB class! if we do it here we also need to call loadDataBlock() again to update SDLIB->X and SDLIB->Y to full data block size. -- after 5 hours of bug finding...
                for(int k=0; k<pParams->SDLIBList[j]->NPAR.size(); k++) {
                    // for k-th PAR
                    this->CHISQ4PARAMS[i][j][k].resize(pParams->SDLIBList[j]->NPAR[k]);
                    this->VALUE4PARAMS[i][j][k].resize(pParams->SDLIBList[j]->NPAR[k]);
                    this->ARGMINPARAMS[i][j][k] = -1;
                    for(int m=0; m<pParams->SDLIBList[j]->NPAR[k]; m++) {
                        // for m-th value of this PAR
                        this->CHISQ4PARAMS[i][j][k][m] = nan("");
                        this->VALUE4PARAMS[i][j][k][m] = pParams->SDLIBList[j]->VALUE4PARAMS[k][m];
                    }
                }
            }
        }
        //
        // initialize nLib, nObs, nRow
        pParams->nLib = this->nLib;
        pParams->nObs = this->nObs;
        pParams->nRow = this->nRow;
        pParams->nModel = this->nModel;
        pParams->nTotal = this->nTotal;
        //
        // DebugLevel
        pParams->DebugLevel = DebugLevel;
        //
        // Link back to this Pool
        pParams->Pool = this;
        //
        // store into global array
        this->ParamsList.push_back(pParams);
    }
}



int michi2ParallelPool::convertIdPoolToIdObs(long long iPool, int debug)
{
    // convertIdPoolToIdObs
    // we will call this->convertIdToSubIdList() which output a SubIdList which is a combination of SubId in OBS and all LIBs.
    std::vector<int> SubIdList = this->convertIdToSubIdList(iPool, std::vector<bool>(), debug);
    int iObs = SubIdList[0]; // copy second to last elements
    return iObs;
}

std::vector<int> michi2ParallelPool::convertIdPoolToIdLibList(long long iPool, int debug)
{
    // convertIdPoolToIdLibList
    // we will call this->convertIdToSubIdList() which output a SubIdList which is a combination of SubId in OBS and all LIBs.
    std::vector<int> SubIdList = this->convertIdToSubIdList(iPool, std::vector<bool>(), debug);
    std::vector<int> iLib; iLib.assign(SubIdList.begin()+1, SubIdList.end()); // // copy second to last elements
    //iLib.resize(SubIdList.size()-1);
    //for(int i=0; i<SubIdList.size()-1; i++) {
    //    iLib[i] = SubIdList[i+1]; // copy second to last elements
    //}
    return iLib;
}

long long michi2ParallelPool::convertIdLibListToIdPool(std::vector<int> iLib, int iObs, int debug)
{
    // convertIdLibListToIdPool
    // we will call this->convertIdToSubIdList() which output a SubIdList which is a combination of SubId in OBS and all LIBs.
    std::vector<int> SubIdList;
    SubIdList.push_back(iObs);
    // then extend SubIdList with iLib
    //SubIdList.reserve(SubIdList.size() + distance(iLib.begin(), iLib.end())); // reserve() is optional - just to improve performance -- see https://stackoverflow.com/questions/313432/c-extend-a-vector-with-another-vector
    SubIdList.insert(SubIdList.end(), iLib.begin(), iLib.end());
    return this->convertSubIdListToId(SubIdList, std::vector<bool>(), debug);
}



std::vector<int> michi2ParallelPool::convertIdToSubIdList(long long iPool, std::vector<bool> SubIdFree, int debug)
{
    std::vector<int> SubIdList;
    SubIdList.resize(1 + this->SDLIBList.size(), 0); // "1 +" considers one index for SDOBS in SDOBSList, then other SubIds are for parameters in each SDLIB // SubIdList[1:nLib] is a list of indices for the k-th model in each LIB file // default value is 0 <TODO> -1
    
    // check this->nObs
    if(this->nObs <= 0 || this->nLib <= 0) {
        std::cout << "michi2ParallelPool::convertIdToSubIdList() Error! No Obs or Lib data loaded! Please run michi2ParallelPool::initialize() first!" << std::endl; return SubIdList;
    }
    
    // setup SubIdFree
    /*if(SubIdFree.size()==0) {
        if(this->SubIdFree.size() > 0) {
            SubIdFree = this->SubIdFree;
        } else {
            SubIdFree.resize(1 + this->SDLIBList.size(), true); // SubIdFree[0] is for OBS and SubIdFree[1:nLib] is for LIBs.
        }
    }*/
    SubIdFree.resize(1 + this->SDLIBList.size(), true); // <TODO> will always set to true
    
    // count free LIBs
    long long NumbFreeModel = 1;
    for(int j=0; j<this->SDLIBList.size(); j++) {
        if(SubIdFree[1+j]) { // SubIdFree[0] is for OBS and SubIdFree[1:nLib] is for LIBs.
            NumbFreeModel = NumbFreeModel * this->SDLIBList[j]->YNum;
        }
    }
    
    // iObs is the index of the OBS file (because each OBS file only contain 1 source).
    SubIdList[0] = int(iPool / NumbFreeModel);
    
    // prepare for counting indices for LIBs per OBS
    long long iLoopPerObs = iPool % NumbFreeModel; // iLIB for each OBS, e.g., LIB1[10] LIB2[8] LIB3[4] 9 7 3
    
    // compute the index for each LIB per OBS
    long long NumbLibMulti = NumbFreeModel;
    for(int j=0; j<this->SDLIBList.size(); j++) {
        if(SubIdFree[1+j]) { // SubIdFree[0] is for OBS and SubIdFree[1:nLib] is for LIBs.
            NumbLibMulti = NumbLibMulti / this->SDLIBList[j]->YNum;
            SubIdList[1+j] = int(iLoopPerObs / NumbLibMulti);
            iLoopPerObs = iLoopPerObs % NumbLibMulti;
            // for example, we have three libs:
            // LIB1[10], LIB2[4], LIB3[6]
            // iLib 0 == indices {0 0 0}
            // iLib 1 == indices {0 0 1}
            // iLib 5 == indices {0 0 5}
            // iLib 6 == indices {0 1 0}
        }
    }
    return SubIdList;
}

long long michi2ParallelPool::convertSubIdListToId(std::vector<int> SubIdList, std::vector<bool> SubIdFree, int debug)
{
    long long iLoop = -1;
    //int debug = 0;
    if(debug>=1) {std::cout << "michi2ParallelPool::convertSubIdListToId() DEBUG " << std::endl;}
    if(debug>=1) {std::cout << "michi2ParallelPool::convertSubIdListToId() DEBUG this->nLib = " << this->nLib << " this->SDLIBList.size() = " << this->SDLIBList.size() << std::endl;}
    
    // check this->nObs
    if(this->nObs <= 0 || this->nLib <= 0) {
        std::cout << "michi2ParallelPool::convertSubIdListToId() Error! No Obs or Lib data loaded! Please run michi2ParallelPool::initialize() first!" << std::endl;
        return -1;
    }
    if(this->SDLIBList.size() != this->nLib) {
        std::cout << "michi2ParallelPool::convertSubIdListToId() Error! The input this->SDLIBList.size() and this->nLib do not match! Please run michi2ParallelPool::initialize() first!" << std::endl;
        return -1;
    }
    if(SubIdList.size() != 1 + this->nLib) {
        std::cout << "michi2ParallelPool::convertSubIdListToId() Error! The input SubIdList has wrong size! It should equal number of LIB plus one for OBS." << std::endl;
        return -1;
    }
    if(debug>=1) {std::cout << "michi2ParallelPool::convertSubIdListToId() DEBUG " << std::endl;}
    
    // setup SubIdFree
    /*if(SubIdFree.size()==0) {
        if(this->SubIdFree.size() > 0) {
            SubIdFree = this->SubIdFree;
        } else {
            SubIdFree.resize(1 + this->SDLIBList.size(), true);
        }
    }*/
    SubIdFree.resize(1 + this->SDLIBList.size(), true); // <TODO> will always set to true
    
    // SubIdList: 0 3 5 2 means SDOBSList[0], SDLIB1[3], SDLIB2[5], SDLIB3[2], where SDLIB1 is SDLIBList[0], etc.
    // iLoop = SubIdList[3] + LIB3YNum*SubIdList[2] + LIB3YNum*LIB2YNum * SubIdList[1] + LIB3YNum*LIB2YNum*LIB1YNum * SubIdList[0]
    //<20190128> Note that the SubIdList for parallel id is different from the SubIdList for Library parameter id,
    //<20190128> for parallel id slower-changed item is placed left of faster-changed item,
    //<20190128> e.g., assuming one SDOBS, 3 SDLIB, which have NPAR 2,4,3 respectively, then we have:
    //<20190128> iPool=0 means SDOBSList[0], SDLIB1[0], SDLIB2[0], SDLIB3[0]
    //<20190128> iPool=1 means SDOBSList[0], SDLIB1[0], SDLIB2[0], SDLIB3[1]
    //<20190128> iPool=2 means SDOBSList[0], SDLIB1[0], SDLIB2[0], SDLIB3[2]
    //<20190128> iPool=3 means SDOBSList[0], SDLIB1[0], SDLIB2[1], SDLIB3[0]
    //<20190128> iPool=4 means SDOBSList[0], SDLIB1[0], SDLIB2[1], SDLIB3[1]
    //<20190128> iPool=5 means SDOBSList[0], SDLIB1[0], SDLIB2[1], SDLIB3[2]
    //<20190128> iPool=6 means SDOBSList[0], SDLIB1[0], SDLIB2[2], SDLIB3[0]
    //<20190128> iPool=7 means SDOBSList[0], SDLIB1[0], SDLIB2[2], SDLIB3[1]
    //<20190128> iPool=8 means SDOBSList[0], SDLIB1[0], SDLIB2[2], SDLIB3[2]
    //<20190128> iPool=9 means SDOBSList[0], SDLIB1[0], SDLIB2[3], SDLIB3[0]
    //<20190128> iPool=10 means SDOBSList[0], SDLIB1[0], SDLIB2[3], SDLIB3[1]
    //<20190128> iPool=11 means SDOBSList[0], SDLIB1[0], SDLIB2[3], SDLIB3[2]
    //<20190128> iPool=12 means SDOBSList[0], SDLIB1[1], SDLIB2[0], SDLIB3[0]
    //<20190128> iPool=13 means SDOBSList[0], SDLIB1[1], SDLIB2[0], SDLIB3[1]
    //<20190128> iPool=14 means SDOBSList[0], SDLIB1[1], SDLIB2[0], SDLIB3[2] -- iPool = i3 + i2*(3) + i1*(4*3) + i0
    iLoop = 0;
    long long NumbLibMulti = 1;
    for(int j=0; j<this->nLib; j++) {
        int k = this->nLib - j; // if j=0, nLib=3, then NumbLibMulti=1, k=3-0=3, iLoop=0+1*SubIdList[3].
                                // if j=1, nLib=3, then NumbLibMulti=1*this->SDLIBList[3]->YNum, k=3-1=2, iLoop=1*SubIdList[3]+1*this->SDLIBList[3]->YNum*SubIdList[2].
                                // until j=nLib-1, k=1, jump out of loop and do a last addition outside.
        
        if(SubIdFree.at(k)) { // k loops from nLib to 1, SubIdFree[nLib:1] is for LIBs and SubIdFree[0] is for OBS.
            if(debug>=1) {std::cout << "michi2ParallelPool::convertSubIdListToId() DEBUG j = " << j << " k = " << k << std::endl;}
            iLoop = iLoop + NumbLibMulti * SubIdList.at(k);
            if(debug>=1) {std::cout << "michi2ParallelPool::convertSubIdListToId() DEBUG j = " << j << " k = " << k << " iLoop = iLoop + " << NumbLibMulti << "*" << SubIdList.at(k) << std::endl;}
            if(debug>=1) {std::cout << "michi2ParallelPool::convertSubIdListToId() DEBUG j = " << j << " k = " << k << " LIB"<<k<<"YNum = " << SDLIBList.at(k-1)->YNum << std::endl;}
            if(debug>=1) {std::cout << "michi2ParallelPool::convertSubIdListToId() DEBUG j = " << j << " k = " << k << " NumbLibMulti = " << NumbLibMulti << std::endl;}
            NumbLibMulti = NumbLibMulti * (this->SDLIBList.at(k-1)->YNum); // k-1 is the LIB index for k-th LIB. <BUG><FIXED><20181001><DZLIU>
            if(debug>=1) {std::cout << "michi2ParallelPool::convertSubIdListToId() DEBUG j = " << j << " k = " << k << " NumbLibMulti = " << NumbLibMulti << std::endl;}
            if(debug>=1) {std::cout << "michi2ParallelPool::convertSubIdListToId() DEBUG j = " << j << " k = " << k << " iLoop = " << iLoop << std::endl;}
        }
    }
    if(debug>=1) {std::cout << "michi2ParallelPool::convertSubIdListToId() DEBUG iLoop = iLoop + " << NumbLibMulti << "*" << SubIdList.at(0) << std::endl;}
    if(SubIdFree.at(0)) {iLoop = iLoop + NumbLibMulti * SubIdList.at(0);}
    if(debug>=1) {std::cout << "michi2ParallelPool::convertSubIdListToId() DEBUG END" << std::endl;}
    return iLoop;
}


std::vector<int> michi2ParallelPool::increaseSubIdList(std::vector<int> SubIdList, std::vector<int> SubIdNumb, std::vector<bool> SubIdFree, int increment)
{
    //std::vector<int> SubIdList2;
    //SubIdList2 = this->increaseSubIdList(SubIdList, SubIdNumb, SubIdFree, increment);
    int debug = 0;
    //
    // setup SubIdFree
    if(SubIdFree.size()==0) {
        if(this->SubIdFree.size() > 0) {
            SubIdFree = this->SubIdFree;
        } else {
            SubIdFree.resize(1 + this->SDLIBList.size(), true);
        }
    } // here SubIdFree will take effect on the increment of SubIds
    //
    for(int i = SubIdList.size()-1; i >= 0; i--) {
        if(SubIdFree[i]) { // only free SubIds are updated
            SubIdList[i]+=increment; // adding one to the last SubId
            break;
        }
    }
    for(int i = SubIdList.size()-1; i >= 1; i--) {
        if(SubIdFree[i]) { // only free SubIds are updated
            if(SubIdList[i] >= SubIdNumb[i]) {
                // if overlow, find the left-side free element
                for(int j = i-1; j >= 0; j--) {
                    if(SubIdFree[j]) { // only free SubIds are updated
                        SubIdList[j]++;
                        SubIdList[i] = 0;
                        if(debug>=1) {std::cout << "michi2ParallelPool::increaseSubIdList() DEBUG i " << i << " j " << j << " SubIdList"; for(int k=0; k<SubIdList.size(); k++) {std::cout << " " << SubIdList[k] << "/" << SubIdNumb[k] << "/" << SubIdFree[k];} std::cout << std::endl;}
                        break;
                    }
                }
            }
        }
        if(debug>=1) {std::cout << "michi2ParallelPool::increaseSubIdList() DEBUG i " << i << " SubIdList"; for(int k=0; k<SubIdList.size(); k++) {std::cout << " " << SubIdList[k] << "/" << SubIdNumb[k] << "/" << SubIdFree[k];} std::cout << std::endl;}
    }
    if(SubIdList[0] >= SubIdNumb[0]) {
        // overflow! <TODO>
    }
    return SubIdList;
}




bool michi2ParallelPool::removeIdInPool(long long iPool)
{
    if(this->IdPool.size() > 0 && this->SubIdListPool.size() == this->IdPool.size()) {
        for(int i=0; i<this->IdPool.size(); i++) {
            // this->NumbOfAllLoops == this->IdPool.size() + this->IdDone.size() == this->SubIdListPool.size() + this->SubIdListDone.size()
            if(this->IdPool.at(i) == iPool) {
                this->SubIdListPool.erase(this->SubIdListPool.begin() + i);
                this->IdPool.erase(this->IdPool.begin() + i);
                return true;
            }
        }
    } else {
        //std::cout << "michi2ParallelPool::removeIdInPool() Error! Pool not initialized!" << std::endl; exit (EXIT_FAILURE);
    }
    return false;
}


long michi2ParallelPool::findIndexOfClosestIdInPool(long long iPool, int direction, int debug)
{
    if(this->IdPool.size() > 0) { // if Pool has been initialized
        // this->NumbOfAllLoops == this->IdPool.size() + this->IdDone.size() == this->SubIdListPool.size() + this->SubIdListDone.size()
        
        // long ibegin = 0;
        // long iend = this->IdPool.size()-1;
        // if(direction < 0) {
        //     ibegin = this->IdPool.size()-1;
        //     iend = 0;
        // }
        // long istep = direction;
        // long iloop;
        
        // if this->CHISQ4PARAMS.size() > 0, we do a CHISQ4PARAMS check
        // 20190126: try optimize this by not checking CHISQ4PARAMS here but in "michi2_v05.cpp". so here I applied && 1 == 0
        /*
         if(this->CHISQ4PARAMS.size() > 0 && 1 == 0) {
            for(iloop = ibegin; iloop <= iend; iloop += istep) {
                if(direction > 0) { if(this->IdPool.at(iloop) < iPool) { continue; }
                } else { if(this->IdPool.at(iloop) > iPool) { continue; } }
                // if we find a cloest Id, then also check CHISQ4PARAMS
                bool iok = true;
                std::vector<int> iSubIdList = this->convertIdToSubIdList(iPool, std::vector<bool>(), debug);
                int i = iSubIdList[0]; if(i<0) { return -1; } // invalid OBS index
                //std::cout << "michi2ParallelPool::findIndexOfClosestIdInPool() iPool = " << iPool << ", CHISQ4PARAMS.size() = " << CHISQ4PARAMS.size() << std::endl;
                //std::cout << "michi2ParallelPool::findIndexOfClosestIdInPool() iPool = " << iPool << ", CHISQ4PARAMS["<<i<<"].size() = " << CHISQ4PARAMS.at(i).size() << std::endl;
                for (int j=0; j<this->CHISQ4PARAMS.at(i).size(); j++) { // for j-th LIB
                    //std::cout << "michi2ParallelPool::findIndexOfClosestIdInPool() iPool = " << iPool << ", CHISQ4PARAMS["<<i<<"]["<<j<<"].size() = " << CHISQ4PARAMS.at(i).at(j).size() << ", iSubIdList["<<1+j<<"] = " << iSubIdList[1+j] << std::endl;
                    if(iSubIdList[1+j]<0) { return -1; } // invalid LIB model index
                    std::vector<int> iLibSubIdList = this->SDLIBList[j]->convertIdToSubIdList(iSubIdList[1+j]); // taking the (iSubIdList[1+j])-th model from the j-th LIB. // convert model index to the SubIdList in j-th LIB.
                    //std::cout << "michi2ParallelPool::findIndexOfClosestIdInPool() iPool = " << iPool << ", CHISQ4PARAMS["<<i<<"]["<<j<<"].size() = " << CHISQ4PARAMS.at(i).at(j).size() << ", iSubIdList["<<1+j<<"] = " << iSubIdList[1+j] << ", iLibSubIdList.size() = " << iLibSubIdList.size() << std::endl;
                    for (int k=0; k<this->CHISQ4PARAMS[i][j].size(); k++) {
                        if(iok && !std::isnan(this->CHISQ4PARAMS[i][j][k][iLibSubIdList[k]]) && this->CHISQ4PARAMS[i][j][k][iLibSubIdList[k]] > 999999.0) {
                            iok = false; break;
                        }
                    }
                    if(!iok) break; // as long as we find a 999999.9 chisq, we exclude this 'this->IdPool.at(iloop)' and goto next 'iloop'.
                }
                if(iok) { return iloop; }
            }
        } else {
            for(long iloop = ibegin; iloop <= iend; iloop += istep) {
                if(direction > 0) { if(this->IdPool.at(iloop) < iPool) { continue; }
                } else { if(this->IdPool.at(iloop) > iPool) { continue; } }
                //
                // if we find a cloest Id, then return
                return iloop;
            }
        }
         */
        //
        // 20190126: optimize this. we must make sure this->IdPool is monochromatically increasing.
        // std::lower_bound: returns the first value that does not compare less
        // std::upper_bound: returns the first value that compares strictly greater
        long ifind;
        auto lowerbound = std::lower_bound(this->IdPool.begin(), this->IdPool.end(), iPool);
        if (lowerbound == this->IdPool.end()) {
            ifind = this->IdPool.size()-1;
        } else {
            ifind = (long)(lowerbound - this->IdPool.begin());
        }
        // also check left size value to see if it is closer or not
        if(ifind-1 >= 0) {
            if(std::abs(this->IdPool[ifind-1] - iPool) < std::abs(this->IdPool[ifind] - iPool)) {
                ifind = ifind - 1;
            }
        }
        return ifind;
    } else {
        //std::cout << "michi2ParallelPool::findIndexOfClosestIdInPool() Error! Pool not initialized!" << std::endl; exit (EXIT_FAILURE);
    }
    //std::cout << "michi2ParallelPool::findIndexOfClosestIdInPool() iPool = " << iPool << ", return -1" << std::endl;
    return -1;
}


long long michi2ParallelPool::findClosestIdInPool(long long iPool)
{
    if(this->IdPool.size() > 0) {
        //
        // 20190126: optimize this. we must make sure this->IdPool is monochromatically increasing.
        //long iloop = findIndexOfClosestIdInPool(iPool, 1);
        //if(iloop < 0) { iloop = findIndexOfClosestIdInPool(iPool, -1); }
        //if(iloop >= 0) { return this->IdPool.at(iloop); }
        long iloop = findIndexOfClosestIdInPool(iPool);
        return this->IdPool.at(iloop);
    } else {
        //std::cout << "michi2ParallelPool::findClosestIdInPool() Error! Pool not initialized!" << std::endl; exit (EXIT_FAILURE);
    }
    return -1;
}


void michi2ParallelPool::printIdPool()
{
    if(this->IdPool.size() > 0) {
        int ndigits = (int)(std::ceil(log10((double)(this->IdPool.back()))));
        for(int i=0; i<this->IdPool.size(); i+=10) {
            if(i==0) {std::cout << "IdPool = ";}
                else {std::cout << "         ";}
            for(int j=0; j<10; j++) {
                if(i+j < this->IdPool.size()) {
                    std::cout << std::setw(ndigits) << std::right << this->IdPool[i+j] << " ";
                }
            }
            std::cout << std::endl;
        }
    } else {
        std::cout << "IdPool = EMPTY";
    }
}













