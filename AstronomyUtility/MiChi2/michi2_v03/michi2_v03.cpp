/* 
 
 TODO: 20140801:
       now the basic function works, which is computing the chi2 between obs dataset and lib dataset, and there can be two cross-matching lib datasets.
       what we need to do then is to achieve the shell argument i/o. e.g. michi2 -obs "flux.dat" -lib "lvg.lib" -out "dl.out", or when just type the
       michi2 in shell without arguments, it will as "Please input obs dataset: " << ....
 
 TODO: 20140801:
       a problem should be noticed: sometimes LMA (michi2MinPack for m2chi2) gives one negative a1 and one positive a2. How to solve this???
 
 DONE: 20141112:
       fixed a stdout typo in michi2DataClass::michi2DataClass(const char *InputFile) // exam data info //
 
 DONE: 20141113:
       fixed the intial value of aCOE in void michi2MinPack::init() // iniCOE //
 
 DONE: 20150309
       make pthread 2 instead of 4 in case not burning the computer
 
 TODO: 20150409
       bug: when like f0=0, df=99, a1 will always be 0. how to correct this?
 
 DONE: 20150721
       m3chi2parallel done
 
 DONE: 20150911
       m4chi2parallel done
 
 DONE: 20150912
       lock i2=i1 in m2chi2, and
       lock i3=i2 in m3chi2, and
       lock i4=i3 in m4chi2, suitable for DL07 cold+warm dust lib
 
 DONE: 20160714-20160718
       arbitrary LIB components

 
 */

#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <iostream>     // std::cout, std::endl
#include <iomanip>      // std::setw
#include <sstream>
#include <fstream>      // std::ifstream, std::ofstream
#include "CrabTableReadColumn.cpp"
#include "CrabStringReadColumn.cpp"
#include "CrabTableReadInfo.cpp"
#include "michi2_DataClass.cpp"
#include "michi2_MinPack.cpp"
#include "michi2_Constraint.h"
#define  LockDL07    // if define this, then we will lock the last two components. Suitable for DL07 cold+warm dust lib. 
//#undef   LockDL07    //

using namespace std;

extern const char* InfoRedshift;

extern int NumbParallel;

extern std::vector<ConstraintStructure*> Constraints;

typedef struct {std::vector<double> j0; std::vector<double> f0; std::vector<double> df; std::vector<double> f1;} MahDAT;

MahDAT michi2MatchObs(michi2DataClass *DCOBS, michi2DataClass *DCLIB, int debug = 0);

double michi2GetChiSquare(std::vector<double> f1, std::vector<double> f0, std::vector<double> df, double *a1);

double michi2GetReducedChiSquare(std::vector<double> f1, std::vector<double> f0, std::vector<double> df, double *a1);

double michi2VecMean(std::vector<double> vec);

void michi2(const char *InputObsFile, const char *InputLibFile, const char *OutputTable = NULL);

void m2chi2(const char *InputObsFile, const char *InputLibFile1, const char *InputLibFile2, const char *OutputTable = NULL);

void *m2chi2parallel(void *params);

void m3chi2(const char *InputObsFile, const char *InputLibFile1, const char *InputLibFile2, const char *InputLibFile3, const char *OutputTable = NULL);

void *m3chi2parallel(void *params);

void m4chi2(const char *InputObsFile, const char *InputLibFile1, const char *InputLibFile2, const char *InputLibFile3, const char *InputLibFile4, const char *OutputTable = NULL);

void *m4chi2parallel(void *params);

void mnchi2(std::vector<std::string> InputObsList, std::vector<std::string> InputLibList, std::vector<std::string> OutputTableList);

void *mnchi2parallel(void *params);




MahDAT michi2MatchObs(michi2DataClass *DCOBS, michi2DataClass *DCLIB, int debug)
{
    MahDAT MatchedDAT;
    //<TODO><20160718>//for(int j=0; j<DCLIB->XNum; j++) {
    //<TODO><20160718>//    if(DCLIB->Y[j]<=0.0) {return MatchedDAT;} // <TODO> do not allow negative LIB
    //<TODO><20160718>//}
//    
//    for(int j=0; j<DCLIB->XNum; j++) {
//        DCLIB->Matched[j]=0;
//        for(int k=0; k<DCOBS->XNum; k++) {
//            DCOBS->Matched[k]=0;
//            // std::cout << "michi2MatchObs: " << DCLIB->XStr[j] << "==" << DCOBS->XStr[k];
//            if(DCLIB->XStr[j]==DCOBS->XStr[k]) { // <TODO> How to match the FXLIB and FXOBS? String compare? Tolerent double value compare?
//                DCLIB->Matched[j]=1;
//                DCOBS->Matched[k]=1;
//                MatchedDAT.j0.push_back(DCOBS->X[k]); // obs
//                MatchedDAT.f0.push_back(DCOBS->Y[k]); // obs
//                MatchedDAT.df.push_back(DCOBS->YErr[k]); // obs unc
//                MatchedDAT.f1.push_back(DCLIB->Y[j]); // lib
//                // std::cout << " got it! " << DCOBS->X[k] << " " << DCOBS->Y[k] << " " << DCLIB->Y[j];
//            }
//            // std::cout << std::endl;
//        }
//    }
//
    double LibRangeXMin = DCLIB->X[0];
    double LibRangeXMax = DCLIB->X[0];
    for(int j=0; j<DCLIB->XNum; j++) {
        if(DCLIB->X[j]<LibRangeXMin){LibRangeXMin=DCLIB->X[j];}
        if(DCLIB->X[j]>LibRangeXMax){LibRangeXMax=DCLIB->X[j];}
    }

    // int debug = 0; // <Debug> // <20160718> now put in argument

    double OnePlusRedshift = 1.0; //
    if(strlen(InfoRedshift)>0) { OnePlusRedshift = 1.0+atof(InfoRedshift); }

    for(int k=0; k<DCOBS->XNum; k++) {
        DCOBS->Matched[k]=0; // <TODO> we will always find a match for obs.
        if(debug) { std::cout << "michi2MatchObs: debugging: we are trying to match obs " << DCOBS->XStr[k] << "/" << OnePlusRedshift << " within lib xrange " << LibRangeXMin << " " << LibRangeXMax << std::endl; }
        if(DCOBS->X[k]/OnePlusRedshift>=LibRangeXMin && DCOBS->X[k]/OnePlusRedshift<=LibRangeXMax) {
            DCOBS->Matched[k]=1; // <TODO> we will always find a match for obs.
            // now we need to find a value from library
            double tmpdiffvalue = -1.0;
            double mindiffvalue = -1.0;
            long   mindiffindex = -1;
            for(int j=0; j<DCLIB->XNum; j++) {
                tmpdiffvalue = (DCOBS->X[k]/OnePlusRedshift - DCLIB->X[j]);
                if(tmpdiffvalue>=0.0 && mindiffvalue<0.0) { mindiffvalue=tmpdiffvalue; mindiffindex=j; } // initial value
                if(tmpdiffvalue>=0.0 && tmpdiffvalue<mindiffvalue) { mindiffvalue=tmpdiffvalue; mindiffindex=j; } // compare value
            }
            // once got matched, we do linear interpolate
            if(mindiffindex>=0 && mindiffindex<DCLIB->XNum-1) {
                if(debug) { std::cout << "michi2MatchObs: debugging: got matched lib between " << DCLIB->X[mindiffindex] << " " << DCLIB->X[mindiffindex+1] << std::endl; }
                int j = mindiffindex;
                double f1_l1 = DCLIB->Y[j];
                double f1_l2 = DCLIB->Y[j+1]; // e.g. we are matching ObsX = 24.0, LibX = 23.95, then we interpolate LibX(23.95,24.15)=>ObsX(24.00)
                double f1_a1 = (DCOBS->X[k]/OnePlusRedshift - DCLIB->X[j]); // as the e.g., a1 =  0.05
                double f1_a2 = (DCLIB->X[j+1] - DCOBS->X[k]/OnePlusRedshift); // as the e.g., a2 =  0.15
                double f1_ai = (f1_a1+f1_a2); // then f1 = f1_l1 * 75% + f1_l2 * 25%
                double f1_li = f1_l1*(f1_a2/f1_ai)+f1_l2*(f1_a1/f1_ai); // <TODO> bi-linear interpolation!
                DCLIB->Matched[j]=1;
                DCOBS->Matched[k]=1;
                MatchedDAT.j0.push_back(DCOBS->X[k]/OnePlusRedshift); // obs
                MatchedDAT.f0.push_back(DCOBS->Y[k]); // obs
                MatchedDAT.df.push_back(DCOBS->YErr[k]); // obs unc
                // <TODO> test using nearest value, no interpolation
                // <TODO> MatchedDAT.f1.push_back(DCLIB->Y[j]); if(debug) { std::cout << "michi2MatchObs: debugging: got matched lib value " << DCLIB->Y[j] << " comparing to obs " << DCOBS->Y[k] << std::endl; }
                MatchedDAT.f1.push_back(f1_li);
                if(debug) { std::cout << "michi2MatchObs: debugging: got matched lib value " << f1_li << " (interpolated near " << DCLIB->YStr[j] << ") " << " comparing to obs " << DCOBS->Y[k] << std::endl; }
                // std::cout << "michi2MatchObs: debugging: got it! " << DCOBS->X[k]/OnePlusRedshift << " " << DCOBS->Y[k] << " " << DCLIB->Y[j];
            } else if(mindiffindex==DCLIB->XNum-1) {
                int j = mindiffindex;
                DCLIB->Matched[j]=1;
                DCOBS->Matched[k]=1;
                MatchedDAT.j0.push_back(DCOBS->X[k]/OnePlusRedshift); // obs
                MatchedDAT.f0.push_back(DCOBS->Y[k]); // obs
                MatchedDAT.df.push_back(DCOBS->YErr[k]); // obs unc
                MatchedDAT.f1.push_back(DCLIB->Y[j]); // lib
                if(debug) { std::cout << "michi2MatchObs: debugging: got matched lib value " << DCLIB->Y[j] << " (" << DCLIB->YStr[j] << ") " << " comparing to obs " << DCOBS->Y[k] << std::endl; }
                // std::cout << "michi2MatchObs: debugging: got it! " << DCOBS->X[k]/OnePlusRedshift << " " << DCOBS->Y[k] << " " << DCLIB->Y[j];
            } else {
                // <TODO> <ERROR!>
            }
        } else {
            DCOBS->Matched[k]=1;
            MatchedDAT.j0.push_back(DCOBS->X[k]/OnePlusRedshift); // obs
            MatchedDAT.f0.push_back(DCOBS->Y[k]); // obs
            MatchedDAT.df.push_back(DCOBS->YErr[k]); // obs unc
            MatchedDAT.f1.push_back(0.0); // lib <TODO>
            // MatchedDAT.f1.push_back(std::numeric_limits<double>::quiet_NaN()); // lib <TODO> what if lib does not cover full obs xrange ?
            if(debug) { std::cout << "michi2MatchObs: debugging: no matched lib found for the given obs xvalue!" << std::endl; }        }
    }
    
    return MatchedDAT;
}





double michi2GetChiSquare(std::vector<double> f1, std::vector<double> f0, std::vector<double> df, double *a1)
{
    // set a1 = sum((f0/df)*(f1/df)) / sum((f1/df)**2) # normalization factor
    double sum1 = 0.0;
    double sum2 = 0.0;
    double f_a1 = 0.0;
    for(int k=0; k<f1.size(); k++) {
        if(f0[k]>0.0 && df[k]>0.0) {
            sum1 += (f0[k]/df[k])*(f1[k]/df[k]);
            sum2 += (f1[k]/df[k])*(f1[k]/df[k]);
        }
    }
    f_a1 = sum1 / sum2;
    *a1 = f_a1;
    // set chi2 = sum((f_a1*f1-f0)**2/df**2)
    double chi2 = 0.0;
    for(int k=0; k<f1.size(); k++) {
        if(f0[k]>0.0 && df[k]>0.0) {
            chi2 += pow((f_a1*f1[k]-f0[k])/(df[k]),2.0);
        }
    }
    // 
    return chi2;
}

double michi2GetReducedChiSquare(std::vector<double> f1, std::vector<double> f0, std::vector<double> df, double *a1)
{
    double chi2 = michi2GetChiSquare(f1, f0, df, a1);
    // Degree of parameter freedom
    int dog2 = 0;
    for(int k=0; k<f1.size(); k++) {
        if(f0[k]>0.0 && df[k]>0.0) {
            dog2++;}}
    // we have only one free parameter a1 in one-component situation
    dog2 = dog2 - 1 - 1;
    // reduce chi2 by N-n-1
    double ReducedChi2 = chi2 / double(dog2); // 
    return ReducedChi2;
}

double michi2VecMean(std::vector<double> vec)
{
    double vecMean = 0.0;
    for(int j=0; j<vec.size(); j++) {
        vecMean += vec[j];
    }
    vecMean = vecMean/(double)vec.size();
    return vecMean;
}
























void michi2(const char *InputObsFile, const char *InputLibFile, const char *OutputTable)
{
    // read obs data structure
    michi2DataClass *SDOBS = new michi2DataClass(InputObsFile);
    // read lib data structure
    michi2DataClass *SDLIB = new michi2DataClass(InputLibFile);
    // prepare output file
    if(NULL==OutputTable) { OutputTable = "michi2.csv"; }
    // prepare
    long i0 = 0;
    std::ofstream SDOUT(OutputTable);
    SDOUT << "#" << std::setw(7) << "i" << std::setw(15) << "chi2" << std::setw(10) << "i1" << std::setw(15) << "a1";
    for(int ipar=0; ipar<SDLIB->TPAR.size(); ipar++) { SDOUT << std::setw(15) << SDLIB->TPAR[ipar]; }
    SDOUT << std::endl << "#" << std::endl;
    // read lib data item
    // -- we need to know which item we read -- i = ((ipar1*npar1+ipar2)*npar2+ipar3) -- then read nvar1 lines
    for(long i=0; i<SDLIB->XNum*SDLIB->YNum; i+=SDLIB->XNum)
    {
        ///// read line
        ///std::vector<std::string> StBlock = michi2GetDataBlock(&SDLIB->LibFileStream,i+1,SDLIB->XNum);
        
        // get data block
        SDLIB->getDataBlock(i+1,SDLIB->XNum);
        // match data block
        MahDAT SDDAT = michi2MatchObs(SDOBS,SDLIB);
        // check data block
        if(SDDAT.f1.empty()) {continue;}
        
        // min chi2 by 1 component fit
        std::vector<std::vector<double> > fLIB;
        fLIB.push_back(SDDAT.f1);
        std::vector<double> fOBS = SDDAT.f0;
        std::vector<double> eOBS = SDDAT.df;
        michi2MinPack *MPACK = new michi2MinPack(fLIB,fOBS,eOBS);
        std::vector<double> f1 = SDLIB->Y;
        double a1 = MPACK->aCOE[0];
        double chi2 = 0.0; for(int j=0;j<MPACK->chi2.size();j++) {chi2+=MPACK->chi2[j];}
        delete MPACK;
        
        // test data
        int debug = 0; // <TODO><DEBUG>
        if(debug) {
            std::cout << "michi2: " << std::setw(7) << "j0" << std::setw(12) << "f0" << std::setw(12) << "df" << std::setw(15) << "f1" << std::endl;
            for(int j=0; j<SDDAT.f0.size(); j++) {
                std::cout << "michi2: " << std::setw(7) << SDDAT.j0[j] << std::setw(12) << SDDAT.f0[j] << std::setw(12) << SDDAT.df[j] << std::setw(15) << SDDAT.f1[j] << std::endl;
            }
        }
        double r_chi2 = chi2 / (double)(SDDAT.f0.size()-1-1);
        std::cout << "michi2: " << std::setw(7) << "i0 = " << i0 << std::setw(12) << "chi2 = " << std::setw(10) << chi2 << std::setw(15) << "r_chi2 = " << std::setw(10) << r_chi2 << std::setw(12) << "a1 = " << std::setw(10) << a1 << std::endl;
        
//        // calc chi2 <TODO> <Delete>
//        chi2 = michi2GetChiSquare(SDDAT.f1,SDDAT.f0,SDDAT.df,&a1);
//        r_chi2 = michi2GetReducedChiSquare(SDDAT.f1,SDDAT.f0,SDDAT.df,&a1);
//        std::cout << "michi2: " << std::setw(7) << "i0 = " << i0 << std::setw(12) << "chi2 = " << std::setw(10) << chi2 << std::setw(15) << "r_chi2 = " << std::setw(10) << r_chi2 << std::setw(12) << "a1 = " << std::setw(10) << a1 << std::endl;
//        if(debug) {
//            std::cout << std::endl;
//        }
        
        // check parameter and output results
        SDOUT << std::setw(8) << i0 << std::setw(15) << chi2 << std::setw(10) << i << std::setw(15) << a1;
        for(int ipar=0; ipar<SDLIB->FPAR.size(); ipar++) { SDOUT << std::setw(15) << SDLIB->FPAR[ipar][0]; }
        SDOUT << std::endl;
        
        // next
        i0++;
        
        // 
        // if(i>3000) {break;} // <TODO><DEBUG>
        
    }
    std::cout << "michi2: well done! 100%!" << std::endl;
    
    //
    SDOUT.close();
}
















/* --- pthread ---- */
struct m2chi2parallelParams {
    michi2DataClass *SDOBS; michi2DataClass *SDLIB1; michi2DataClass *SDLIB2;
    const char *InputLibFile1; const char *InputLibFile2; long i1begin; long i1end; long i1; long i2; long i0;
    std::ofstream *SDOUT;
};
pthread_mutex_t m2chi2parallelMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t m2chi2parallelCondition = PTHREAD_COND_INITIALIZER;
long m2chi2parallelProgress = 0;
/* ---------------- */

void m2chi2(const char *InputObsFile, const char *InputLibFile1, const char *InputLibFile2, const char *OutputTable)
{
    // read obs data structure
    michi2DataClass *SDOBS = new michi2DataClass(InputObsFile);
    // read lib data structure
    michi2DataClass *SDLIB1 = new michi2DataClass(InputLibFile1);
    michi2DataClass *SDLIB2 = new michi2DataClass(InputLibFile2);
    // prepare output file
    if(NULL==OutputTable) { OutputTable = "m2chi2.csv"; }
    // prepare
    long i0 = 0;
    std::ofstream SDOUT(OutputTable);
    SDOUT << "#" << std::setw(7) << "i0" << std::setw(15) << "chi2" << std::setw(10) << "i1" << std::setw(15) << "a1" << std::setw(10) << "i2" << std::setw(15) << "a2"; // fitting data will write to pStream
    for(int ipar=0; ipar<SDLIB1->TPAR.size(); ipar++) { SDOUT << std::setw(15) << SDLIB1->TPAR[ipar]; }
    for(int ipar=0; ipar<SDLIB2->TPAR.size(); ipar++) { SDOUT << std::setw(15) << SDLIB2->TPAR[ipar]; }
    SDOUT << std::endl << "#" << std::endl;
    std::cout << "processing: " << std::endl;
    // parallel <New><20141211><20141212>
    m2chi2parallelProgress = 0;
    std::vector<struct m2chi2parallelParams *> m2chi2parallelParams;
    // <TODO><debug>
    // <TODO><debug> SDLIB1->YNum = 800;
    // std::cout << "progress: " << std::flush;
    // read lib item
    for(long i1=0; i1<(SDLIB1->XNum)*(SDLIB1->YNum); i1+=(SDLIB1->XNum)*long(float(SDLIB1->YNum)/2.0)) // <20150309> make pthread 2
    {
        std::cout << "looping " << i1 << " / " << (SDLIB1->XNum)*(SDLIB1->YNum) << "   ";
        struct m2chi2parallelParams *pParams = new struct m2chi2parallelParams; pParams->i0=0; pParams->i1=0; pParams->i2=0;
        std::cout << pParams << std::endl; // check that the param strcut is well set
        m2chi2parallelParams.push_back(pParams);
        pParams->SDOBS = SDOBS; pParams->SDLIB1 = SDLIB1; pParams->InputLibFile1 = InputLibFile1; pParams->InputLibFile2 = InputLibFile2; pParams->i1begin = i1; pParams->i1end = i1+(SDLIB1->XNum)*long(float(SDLIB1->YNum)/2.0); if((pParams->i1end)>(SDLIB1->XNum)*(SDLIB1->YNum)){pParams->i1end = (SDLIB1->XNum)*(SDLIB1->YNum);} pParams->SDOUT = &SDOUT;
            // <20150309> make pthread 2
        pthread_t thread1; pthread_create(&thread1, NULL, &m2chi2parallel, pParams); 
        // <TODO><DEBUG>
        // if(i1>=500) {sleep(60);} else {sleep(2);}
        sleep(5);
    }
    // show initial info for all threads
    std::cout << std::setw(8) << std::right << m2chi2parallelProgress << "||";
    for(int ip=0; ip<m2chi2parallelParams.size(); ip++) {
        std::cout << std::setw(15) << std::right << m2chi2parallelParams[ip]->i1begin << "|" << std::setw(8) << std::right << 0 << "||";
    }
    std::cout << std::endl;
    // wait for all threads
    while(m2chi2parallelProgress<(SDLIB1->XNum)*(SDLIB1->YNum)) {
        std::cout << std::setw(8) << std::right << m2chi2parallelProgress << "||";
        for(int ip=0; ip<m2chi2parallelParams.size(); ip++) {
            std::cout << std::setw(15) << std::right << m2chi2parallelParams[ip]->i1 << "|" << std::setw(8) << std::right << m2chi2parallelParams[ip]->i2 << "||";
        }
        std::cout << std::endl;
        sleep(3);
    }
    // clean
    for(int ip=0; ip<m2chi2parallelParams.size(); ip++) {
        delete m2chi2parallelParams[ip]; m2chi2parallelParams[ip]=NULL;
    }

    //
    std::cout << "    100%" << "||" << std::endl;
    SDOUT.close();
}

void *m2chi2parallel(void *params)
{
    struct m2chi2parallelParams *pParams = (struct m2chi2parallelParams *)params;
    std::stringstream pStream;
    // read lib data structure
    michi2DataClass *SDLIB1 = new michi2DataClass(pParams->InputLibFile1);
    michi2DataClass *SDLIB2 = new michi2DataClass(pParams->InputLibFile2);
    pParams->SDLIB1 = SDLIB1;
    pParams->SDLIB2 = SDLIB2;
    pParams->i1 = 0;
    pParams->i2 = 0;
    pParams->i0 = 0;
    for(pParams->i1=pParams->i1begin; pParams->i1<pParams->i1end && pParams->i1<(pParams->SDLIB1->XNum)*(pParams->SDLIB1->YNum); pParams->i1+=(SDLIB1->XNum))
    {
        // get data block
        pParams->SDLIB1->getDataBlock(pParams->i1+1,pParams->SDLIB1->XNum);
        // match data block
        MahDAT SDDAT1 = michi2MatchObs(pParams->SDOBS,pParams->SDLIB1);
        // <TODO><Debug>
        // std::cout << "m2chi2parallel: i1begin=" << pParams->i1begin << " ? m2chi2parallelProgress=" << m2chi2parallelProgress << std::endl;
        // check data block
        if(!SDDAT1.f1.empty()) {
            // loop the second lib
            // <added><20150912><dzliu> // lock the fourth lib to the third lib, i.e. DL07 cold/warm.
#ifdef LockDL07
            for(pParams->i2=pParams->i1; pParams->i2<pParams->i1+1; pParams->i2+=1)
#else
            for(pParams->i2=0; pParams->i2<(pParams->SDLIB2->XNum)*(pParams->SDLIB2->YNum); pParams->i2+=(pParams->SDLIB2->XNum))
#endif
            {
                // <TODO><Debug>
                // std::cout << i1 << " " << i2 << " / " << pParams->SDLIB2->XNum*pParams->SDLIB2->YNum << std::endl;
                // get data block
                pParams->SDLIB2->getDataBlock(pParams->i2+1,pParams->SDLIB2->XNum);
                // match data block
                MahDAT SDDAT2 = michi2MatchObs(pParams->SDOBS,pParams->SDLIB2);
                // <TODO><Debug>
                // std::cout << i1 << " " << i2 << " / " << pParams->SDLIB2->XNum*pParams->SDLIB2->YNum << std::endl;
                // check data block
                if(SDDAT2.f1.empty()) {continue;}
                // min chi2 by 2 components
                std::vector<std::vector<double> > fLIB;
                fLIB.push_back(SDDAT1.f1);
                fLIB.push_back(SDDAT2.f1);
                std::vector<double> fOBS = SDDAT2.f0;
                std::vector<double> eOBS = SDDAT2.df;
                michi2MinPack *MPACK = new michi2MinPack(fLIB,fOBS,eOBS);
                std::vector<double> f1 = pParams->SDLIB1->Y;
                std::vector<double> f2 = pParams->SDLIB2->Y;
                double a1 = MPACK->aCOE[0];
                double a2 = MPACK->aCOE[1];
                double chi2 = 0.0; for(int j=0;j<MPACK->chi2.size();j++) {chi2+=MPACK->chi2[j];}
                delete MPACK;
                // check parameter and print results
                /*
                 std::cout << std::setw(6) << m2chi2parallelProI0 << std::setw(15) << chi2 << std::setw(10) << i1 << std::setw(15) << a1 << std::setw(10) << i2 << std::setw(15) << a2;
                 for(int ipar=0; ipar<pParams->SDLIB1->FPAR.size(); ipar++) { std::cout << std::setw(15) << pParams->SDLIB1->FPAR[ipar][0]; }
                 for(int ipar=0; ipar<pParams->SDLIB2->FPAR.size(); ipar++) { std::cout << std::setw(15) << pParams->SDLIB2->FPAR[ipar][0]; }
                 std::cout << std::endl;
                 */
                // check parameter and output results to stream
                pStream << std::setw(8) << pParams->i0 << std::setw(15) << chi2 << std::setw(10) << pParams->i1 << std::setw(15) << a1 << std::setw(10) << pParams->i2 << std::setw(15) << a2;
                for(int ipar=0; ipar<pParams->SDLIB1->FPAR.size(); ipar++) { pStream << std::setw(15) << pParams->SDLIB1->FPAR[ipar][0]; }
                for(int ipar=0; ipar<pParams->SDLIB2->FPAR.size(); ipar++) { pStream << std::setw(15) << pParams->SDLIB2->FPAR[ipar][0]; }
                pStream << std::endl;
                pParams->i0++;
            }
        }
    }
    // lock mutex
    pthread_mutex_lock(&m2chi2parallelMutex);
    std::cout << "m2chi2parallel: i1begin=" << pParams->i1begin << " ? m2chi2parallelProgress=" << m2chi2parallelProgress << " now locked!" << std::endl;
    for(;;) {
        // test if now we are ok to write result file
        if(pParams->i1begin==m2chi2parallelProgress) {
            // thread ok to write!
            std::cout << "m2chi2parallel: i1begin=" << pParams->i1begin << " ? m2chi2parallelProgress=" << m2chi2parallelProgress << " got ya!";
            if(!pStream.str().empty()) {*pParams->SDOUT<<pStream.str();}
            std::cout << std::endl;
            pthread_cond_signal(&m2chi2parallelCondition); // send finish signal
            break;
            // pthread_mutex_unlock(&m2chi2parallelMutex); // unlock mutex
            // delete SDLIB1; delete SDLIB2; return(NULL);
        } else {
            // wait while other earlier threads to write result file
            pthread_cond_wait(&m2chi2parallelCondition, &m2chi2parallelMutex);
            std::cout << "m2chi2parallel: i1begin=" << pParams->i1begin << " ? m2chi2parallelProgress=" << m2chi2parallelProgress << " now waiting" << std::endl;
        }
    }
    // unlock mutex
    // <TODO><Delete> m2chi2parallelProgress += pParams->SDLIB1->XNum;
    m2chi2parallelProgress = pParams->i1end;
    pthread_mutex_unlock(&m2chi2parallelMutex);
    std::cout << "m2chi2parallel: i1begin=" << pParams->i1begin << " ? m2chi2parallelProgress=" << m2chi2parallelProgress << " now unlocked!" << std::endl;
    // finish at last! <TODO><Debug> should not happen
    if(pParams->i1begin<=m2chi2parallelProgress) {delete SDLIB1; delete SDLIB2; return(NULL);}
    return(NULL);
}









/* --- pthread ---- */
struct m3chi2parallelParams {
    michi2DataClass *SDOBS; michi2DataClass *SDLIB1; michi2DataClass *SDLIB2; michi2DataClass *SDLIB3;
    const char *InputLibFile1; const char *InputLibFile2; const char *InputLibFile3; long i1begin; long i1end; long i1; long i2; long i3; long i0;
    std::ofstream *SDOUT;
};
pthread_mutex_t m3chi2parallelMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t m3chi2parallelCondition = PTHREAD_COND_INITIALIZER;
long m3chi2parallelProgress = 0;
/* ---------------- */

void m3chi2(const char *InputObsFile, const char *InputLibFile1, const char *InputLibFile2, const char *InputLibFile3, const char *OutputTable)
{
    // read obs data structure
    michi2DataClass *SDOBS = new michi2DataClass(InputObsFile);
    // read lib data structure
    michi2DataClass *SDLIB1 = new michi2DataClass(InputLibFile1);
    michi2DataClass *SDLIB2 = new michi2DataClass(InputLibFile2);
    michi2DataClass *SDLIB3 = new michi2DataClass(InputLibFile3);
    // prepare output file
    if(NULL==OutputTable) { OutputTable = "m3chi2.csv"; }
    // prepare
    long i0 = 0;
    std::ofstream SDOUT(OutputTable);
    SDOUT << "#" << std::setw(7) << "i0" << std::setw(15) << "chi2" << std::setw(10) << "i1" << std::setw(15) << "a1" << std::setw(10) << "i2" << std::setw(15) << "a2" << std::setw(10) << "i3" << std::setw(15) << "a3"; // fitting data will write to pStream
    for(int ipar=0; ipar<SDLIB1->TPAR.size(); ipar++) { SDOUT << std::setw(15) << SDLIB1->TPAR[ipar]; }
    for(int ipar=0; ipar<SDLIB2->TPAR.size(); ipar++) { SDOUT << std::setw(15) << SDLIB2->TPAR[ipar]; }
    for(int ipar=0; ipar<SDLIB3->TPAR.size(); ipar++) { SDOUT << std::setw(15) << SDLIB3->TPAR[ipar]; }
    SDOUT << std::endl << "#" << std::endl;
    std::cout << "processing: " << std::endl;
    // parallel <New><20141211><20141212>
    m3chi2parallelProgress = 0;
    std::vector<struct m3chi2parallelParams *> m3chi2parallelParams;
    // <TODO><debug>
    // <TODO><debug> SDLIB1->YNum = 800;
    // std::cout << "progress: " << std::flush;
    // read lib item
    for(long i1=0; i1<(SDLIB1->XNum)*(SDLIB1->YNum); i1+=(SDLIB1->XNum)*long(float(SDLIB1->YNum)/2.0)) // <20150309> make pthread 2
    {
        std::cout << "looping " << i1 << " / " << (SDLIB1->XNum)*(SDLIB1->YNum) << "   ";
        struct m3chi2parallelParams *pParams = new struct m3chi2parallelParams; pParams->i0=0; pParams->i1=0; pParams->i2=0; pParams->i3=0;
        std::cout << pParams << std::endl; // check that the param strcut is well set
        m3chi2parallelParams.push_back(pParams);
        pParams->SDOBS = SDOBS; pParams->SDLIB1 = SDLIB1; pParams->InputLibFile1 = InputLibFile1; pParams->InputLibFile2 = InputLibFile2; pParams->InputLibFile3 = InputLibFile3; pParams->i1begin = i1; pParams->i1end = i1+(SDLIB1->XNum)*long(float(SDLIB1->YNum)/2.0); if((pParams->i1end)>(SDLIB1->XNum)*(SDLIB1->YNum)){pParams->i1end = (SDLIB1->XNum)*(SDLIB1->YNum);} pParams->SDOUT = &SDOUT;
        // <20150309> make pthread 2
        pthread_t thread1; pthread_create(&thread1, NULL, &m3chi2parallel, pParams);
        // <TODO><DEBUG>
        // if(i1>=500) {sleep(60);} else {sleep(2);}
        sleep(5);
    }
    // show initial info for all threads
    std::cout << std::setw(8) << std::right << m3chi2parallelProgress << "||";
    for(int ip=0; ip<m3chi2parallelParams.size(); ip++) {
        std::cout << std::setw(15) << std::right << m3chi2parallelParams[ip]->i1begin << "|" << std::setw(8) << std::right << 0 << "|" << std::setw(8) << std::right << 0 << "||";
    }
    std::cout << std::endl;
    // wait for all threads
    while(m3chi2parallelProgress<(SDLIB1->XNum)*(SDLIB1->YNum)) {
        std::cout << std::setw(8) << std::right << m3chi2parallelProgress << "||";
        for(int ip=0; ip<m3chi2parallelParams.size(); ip++) {
            std::cout << std::setw(15) << std::right << m3chi2parallelParams[ip]->i1 << "|" << std::setw(8) << std::right << m3chi2parallelParams[ip]->i2 << "|" << std::setw(8) << std::right << m3chi2parallelParams[ip]->i3 << "||";
        }
        std::cout << std::endl;
        sleep(3);
    }
    // clean
    for(int ip=0; ip<m3chi2parallelParams.size(); ip++) {
        delete m3chi2parallelParams[ip]; m3chi2parallelParams[ip]=NULL;
    }
    
    //
    std::cout << "    100%" << "||" << std::endl;
    SDOUT.close();
}

void *m3chi2parallel(void *params)
{
    struct m3chi2parallelParams *pParams = (struct m3chi2parallelParams *)params;
    std::stringstream pStream;
    // read lib data structure
    michi2DataClass *SDLIB1 = new michi2DataClass(pParams->InputLibFile1);
    michi2DataClass *SDLIB2 = new michi2DataClass(pParams->InputLibFile2);
    michi2DataClass *SDLIB3 = new michi2DataClass(pParams->InputLibFile3);
    pParams->SDLIB1 = SDLIB1;
    pParams->SDLIB2 = SDLIB2;
    pParams->SDLIB3 = SDLIB3;
    pParams->i1 = 0;
    pParams->i2 = 0;
    pParams->i3 = 0;
    pParams->i0 = 0;
    for(pParams->i1=pParams->i1begin; pParams->i1<pParams->i1end && pParams->i1<(pParams->SDLIB1->XNum)*(pParams->SDLIB1->YNum); pParams->i1+=(SDLIB1->XNum))
    {
        // get data block
        pParams->SDLIB1->getDataBlock(pParams->i1+1,pParams->SDLIB1->XNum);
        // match data block
        MahDAT SDDAT1 = michi2MatchObs(pParams->SDOBS,pParams->SDLIB1);
        // <TODO><Debug>
        // std::cout << "m3chi2parallel: i1begin=" << pParams->i1begin << " ? m3chi2parallelProgress=" << m3chi2parallelProgress << std::endl;
        // check data block
        if(!SDDAT1.f1.empty()) {
            // loop the second lib
            for(pParams->i2=0; pParams->i2<(pParams->SDLIB2->XNum)*(pParams->SDLIB2->YNum); pParams->i2+=(pParams->SDLIB2->XNum))
            {
                // check parameter and print results
                /*
                 std::cout << std::setw(6) << pParams->i2;
                 std::cout << std::endl;
                 */
                // <TODO><Debug>
                // std::cout << i1 << " " << i2 << " / " << pParams->SDLIB2->XNum*pParams->SDLIB2->YNum << std::endl;
                // get data block
                pParams->SDLIB2->getDataBlock(pParams->i2+1,pParams->SDLIB2->XNum);
                // match data block
                MahDAT SDDAT2 = michi2MatchObs(pParams->SDOBS,pParams->SDLIB2);
                // <TODO><Debug>
                // std::cout << i1 << " " << i2 << " / " << pParams->SDLIB2->XNum*pParams->SDLIB2->YNum << std::endl;
                // check data block
                if(SDDAT2.f1.empty()) {continue;}
                if(!SDDAT2.f1.empty()) {
                    // loop the second lib
                    // <added><20150912><dzliu> // lock the fourth lib to the third lib, i.e. DL07 cold/warm.
#ifdef LockDL07
                    for(pParams->i3=pParams->i2; pParams->i3<pParams->i2+1; pParams->i3+=1)
#else
                    for(pParams->i3=0; pParams->i3<(pParams->SDLIB3->XNum)*(pParams->SDLIB3->YNum); pParams->i3+=(pParams->SDLIB3->XNum))
#endif
                    {
                        // <TODO><Debug>
                        // std::cout << i1 << " " << i2 << " / " << pParams->SDLIB2->XNum*pParams->SDLIB2->YNum << std::endl;
                        // get data block
                        pParams->SDLIB3->getDataBlock(pParams->i3+1,pParams->SDLIB3->XNum);
                        // match data block
                        MahDAT SDDAT3 = michi2MatchObs(pParams->SDOBS,pParams->SDLIB3);
                        // <TODO><Debug>
                        // std::cout << i1 << " " << i2 << " / " << pParams->SDLIB2->XNum*pParams->SDLIB2->YNum << std::endl;
                        // check data block
                        if(SDDAT3.f1.empty()) {continue;}
                        // min chi2 by 3 components
                        std::vector<std::vector<double> > fLIB;
                        fLIB.push_back(SDDAT1.f1);
                        fLIB.push_back(SDDAT2.f1);
                        fLIB.push_back(SDDAT3.f1);
                        std::vector<double> fOBS = SDDAT3.f0;
                        std::vector<double> eOBS = SDDAT3.df;
                        michi2MinPack *MPACK = new michi2MinPack(fLIB,fOBS,eOBS);
                        std::vector<double> f1 = pParams->SDLIB1->Y;
                        std::vector<double> f2 = pParams->SDLIB2->Y;
                        std::vector<double> f3 = pParams->SDLIB3->Y;
                        double a1 = MPACK->aCOE[0];
                        double a2 = MPACK->aCOE[1];
                        double a3 = MPACK->aCOE[2];
                        double chi2 = 0.0; for(int j=0;j<MPACK->chi2.size();j++) {chi2+=MPACK->chi2[j];}
                        delete MPACK;
                        // check parameter and output results to stream
                        pStream << std::setw(8) << pParams->i0 << std::setw(15) << chi2 << std::setw(10) << pParams->i1 << std::setw(15) << a1 << std::setw(10) << pParams->i2 << std::setw(15) << a2 << std::setw(10) << pParams->i3 << std::setw(15) << a3;
                        for(int ipar=0; ipar<pParams->SDLIB1->FPAR.size(); ipar++) { pStream << std::setw(15) << pParams->SDLIB1->FPAR[ipar][0]; }
                        for(int ipar=0; ipar<pParams->SDLIB2->FPAR.size(); ipar++) { pStream << std::setw(15) << pParams->SDLIB2->FPAR[ipar][0]; }
                        for(int ipar=0; ipar<pParams->SDLIB3->FPAR.size(); ipar++) { pStream << std::setw(15) << pParams->SDLIB3->FPAR[ipar][0]; }
                        pStream << std::endl;
                        pParams->i0++;
                    }
                }
            }
        }
    }
    // lock mutex
    pthread_mutex_lock(&m3chi2parallelMutex);
    std::cout << "m3chi2parallel: i1begin=" << pParams->i1begin << " ? m3chi2parallelProgress=" << m3chi2parallelProgress << " now locked!" << std::endl;
    for(;;) {
        // test if now we are ok to write result file
        if(pParams->i1begin==m3chi2parallelProgress) {
            // thread ok to write!
            std::cout << "m3chi2parallel: i1begin=" << pParams->i1begin << " ? m3chi2parallelProgress=" << m3chi2parallelProgress << " got ya!";
            if(!pStream.str().empty()) {*pParams->SDOUT<<pStream.str();}
            std::cout << std::endl;
            pthread_cond_signal(&m3chi2parallelCondition); // send finish signal
            break;
            // pthread_mutex_unlock(&m3chi2parallelMutex); // unlock mutex
            // delete SDLIB1; delete SDLIB2; delete SDLIB3; return(NULL);
        } else {
            // wait while other earlier threads to write result file
            pthread_cond_wait(&m3chi2parallelCondition, &m3chi2parallelMutex);
            std::cout << "m3chi2parallel: i1begin=" << pParams->i1begin << " ? m3chi2parallelProgress=" << m3chi2parallelProgress << " now waiting" << std::endl;
        }
    }
    // unlock mutex
    // <TODO><Delete> m3chi2parallelProgress += pParams->SDLIB1->XNum;
    m3chi2parallelProgress = pParams->i1end;
    pthread_mutex_unlock(&m3chi2parallelMutex);
    std::cout << "m3chi2parallel: i1begin=" << pParams->i1begin << " ? m3chi2parallelProgress=" << m3chi2parallelProgress << " now unlocked!" << std::endl;
    // finish at last! <TODO><Debug> should not happen
    if(pParams->i1begin<=m3chi2parallelProgress) {delete SDLIB1; delete SDLIB2; delete SDLIB3; return(NULL);}
    return(NULL);
}









/* --- pthread ---- */
struct m4chi2parallelParams {
    michi2DataClass *SDOBS; michi2DataClass *SDLIB1; michi2DataClass *SDLIB2; michi2DataClass *SDLIB3; michi2DataClass *SDLIB4;
    const char *InputLibFile1; const char *InputLibFile2; const char *InputLibFile3; const char *InputLibFile4; long i1begin; long i1end; long i1; long i2; long i3; long i4; long i0;
    std::ofstream *SDOUT;
};
pthread_mutex_t m4chi2parallelMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t m4chi2parallelCondition = PTHREAD_COND_INITIALIZER;
long m4chi2parallelProgress = 0;
/* ---------------- */

void m4chi2(const char *InputObsFile, const char *InputLibFile1, const char *InputLibFile2, const char *InputLibFile3, const char *InputLibFile4, const char *OutputTable)
{
    // read obs data structure
    michi2DataClass *SDOBS = new michi2DataClass(InputObsFile);
    // read lib data structure
    michi2DataClass *SDLIB1 = new michi2DataClass(InputLibFile1); // here we only need to read SDLIB1 because we split SDLIB1 into parallel. 
    michi2DataClass *SDLIB2 = new michi2DataClass(InputLibFile2);
    michi2DataClass *SDLIB3 = new michi2DataClass(InputLibFile3);
    michi2DataClass *SDLIB4 = new michi2DataClass(InputLibFile4);
    // prepare output file
    if(NULL==OutputTable) { OutputTable = "m4chi2.csv"; }
    // prepare
    long i0 = 0;
    std::ofstream SDOUT(OutputTable);
    SDOUT << "#" << std::setw(7) << "i0" << std::setw(15) << "chi2" << std::setw(10) << "i1" << std::setw(15) << "a1" << std::setw(10) << "i2" << std::setw(15) << "a2" << std::setw(10) << "i3" << std::setw(15) << "a3" << std::setw(10) << "i4" << std::setw(15) << "a4"; // fitting data will write to pStream
    for(int ipar=0; ipar<SDLIB1->TPAR.size(); ipar++) { SDOUT << std::setw(15) << SDLIB1->TPAR[ipar]; }
    for(int ipar=0; ipar<SDLIB2->TPAR.size(); ipar++) { SDOUT << std::setw(15) << SDLIB2->TPAR[ipar]; }
    for(int ipar=0; ipar<SDLIB3->TPAR.size(); ipar++) { SDOUT << std::setw(15) << SDLIB3->TPAR[ipar]; }
    for(int ipar=0; ipar<SDLIB4->TPAR.size(); ipar++) { SDOUT << std::setw(15) << SDLIB4->TPAR[ipar]; }
    SDOUT << std::endl << "#" << std::endl;
    std::cout << "processing: " << std::endl;
    // parallel <New><20141211><20141212>
    m4chi2parallelProgress = 0;
    std::vector<struct m4chi2parallelParams *> m4chi2parallelParams;
    // make parallel
    if(SDLIB1->YNum>=2)
    {
        for(long i1=0; i1<(SDLIB1->XNum)*(SDLIB1->YNum); i1+=(SDLIB1->XNum)*long(float(SDLIB1->YNum)/2.0)) // <20150309> make pthread 2
        {
            std::cout << "looping " << i1 << " / " << (SDLIB1->XNum)*(SDLIB1->YNum) << "   ";
            struct m4chi2parallelParams *pParams = new struct m4chi2parallelParams; pParams->i0=0; pParams->i1=0; pParams->i2=0; pParams->i3=0; pParams->i4=0;
            std::cout << pParams << std::endl; // check that the param strcut is well set
            m4chi2parallelParams.push_back(pParams);
            pParams->SDOBS = SDOBS; pParams->SDLIB1 = SDLIB1;
            pParams->InputLibFile1 = InputLibFile1; pParams->InputLibFile2 = InputLibFile2; pParams->InputLibFile3 = InputLibFile3; pParams->InputLibFile4 = InputLibFile4;
            pParams->i1begin = i1; pParams->i1end = i1+(SDLIB1->XNum)*long(float(SDLIB1->YNum)/2.0);
            if((pParams->i1end)>(SDLIB1->XNum)*(SDLIB1->YNum)){pParams->i1end = (SDLIB1->XNum)*(SDLIB1->YNum);} pParams->SDOUT = &SDOUT;
            // <20150309> make pthread 2
            pthread_t thread1; pthread_create(&thread1, NULL, &m4chi2parallel, pParams);
            // <TODO><DEBUG>
            // if(i1>=500) {sleep(60);} else {sleep(2);}
            sleep(5);
        }
    } else {
        std::cout << "starting " << 0 << " / " << (SDLIB1->XNum)*(SDLIB1->YNum) << "   ";
        struct m4chi2parallelParams *pParams = new struct m4chi2parallelParams; pParams->i0=0; pParams->i1=0; pParams->i2=0; pParams->i3=0; pParams->i4=0;
        std::cout << pParams << std::endl; // check that the param strcut is well set
        m4chi2parallelParams.push_back(pParams);
        pParams->SDOBS = SDOBS; pParams->SDLIB1 = SDLIB1;
        pParams->InputLibFile1 = InputLibFile1; pParams->InputLibFile2 = InputLibFile2; pParams->InputLibFile3 = InputLibFile3; pParams->InputLibFile4 = InputLibFile4;
        pParams->i1begin = 0; pParams->i1end = (SDLIB1->XNum)*(SDLIB1->YNum);
        if((pParams->i1end)>(SDLIB1->XNum)*(SDLIB1->YNum)){pParams->i1end = (SDLIB1->XNum)*(SDLIB1->YNum);} pParams->SDOUT = &SDOUT;
        // <20150911> do not do parallel when SDLIB1 has only one NPAR
        pthread_t thread1; pthread_create(&thread1, NULL, &m4chi2parallel, pParams);
    }
    // show initial info for all threads
    std::cout << std::setw(8) << std::right << m4chi2parallelProgress << "||";
    for(int ip=0; ip<m4chi2parallelParams.size(); ip++) {
        std::cout << std::setw(15) << std::right << m4chi2parallelParams[ip]->i1begin << "|" << std::setw(8) << std::right << 0 << "|" << std::setw(8) << std::right << 0 << "|" << std::setw(8) << std::right << 0 << "||";
    }
    std::cout << std::endl;
    // wait for all threads
    while(m4chi2parallelProgress<(SDLIB1->XNum)*(SDLIB1->YNum)) {
        std::cout << std::setw(8) << std::right << m4chi2parallelProgress << "||";
        for(int ip=0; ip<m4chi2parallelParams.size(); ip++) {
            std::cout << std::setw(15) << std::right << m4chi2parallelParams[ip]->i1 << "|" << std::setw(8) << std::right << m4chi2parallelParams[ip]->i2 << "|" << std::setw(8) << std::right << m4chi2parallelParams[ip]->i3 << "|" << std::setw(8) << std::right << m4chi2parallelParams[ip]->i4 << "||";
        }
        std::cout << std::endl;
        sleep(3);
    }
    // clean
    for(int ip=0; ip<m4chi2parallelParams.size(); ip++) {
        delete m4chi2parallelParams[ip]; m4chi2parallelParams[ip]=NULL;
    }
    
    //
    std::cout << "    100%" << "||" << std::endl;
    SDOUT.close();
}

void *m4chi2parallel(void *params)
{
    struct m4chi2parallelParams *pParams = (struct m4chi2parallelParams *)params;
    std::stringstream pStream;
    // read lib data structure
    michi2DataClass *SDLIB1 = new michi2DataClass(pParams->InputLibFile1);
    michi2DataClass *SDLIB2 = new michi2DataClass(pParams->InputLibFile2);
    michi2DataClass *SDLIB3 = new michi2DataClass(pParams->InputLibFile3);
    michi2DataClass *SDLIB4 = new michi2DataClass(pParams->InputLibFile4);
    pParams->SDLIB1 = SDLIB1;
    pParams->SDLIB2 = SDLIB2;
    pParams->SDLIB3 = SDLIB3;
    pParams->SDLIB4 = SDLIB4;
    pParams->i1 = 0;
    pParams->i2 = 0;
    pParams->i3 = 0;
    pParams->i4 = 0;
    pParams->i0 = 0;
    for(pParams->i1=pParams->i1begin; pParams->i1<pParams->i1end && pParams->i1<(pParams->SDLIB1->XNum)*(pParams->SDLIB1->YNum); pParams->i1+=(SDLIB1->XNum))
    {
        // get data block
        pParams->SDLIB1->getDataBlock(pParams->i1+1,pParams->SDLIB1->XNum);
        // match data block
        MahDAT SDDAT1 = michi2MatchObs(pParams->SDOBS,pParams->SDLIB1);
        // <TODO><Debug>
        // std::cout << "m4chi2parallel: i1begin=" << pParams->i1begin << " ? m4chi2parallelProgress=" << m4chi2parallelProgress << std::endl;
        // check data block
        if(!SDDAT1.f1.empty()) {
            // loop the second lib
            for(pParams->i2=0; pParams->i2<(pParams->SDLIB2->XNum)*(pParams->SDLIB2->YNum); pParams->i2+=(pParams->SDLIB2->XNum))
            {
                // check parameter and print results
                /*
                 std::cout << std::setw(6) << pParams->i2;
                 std::cout << std::endl;
                 */
                // <TODO><Debug>
                // std::cout << i1 << " " << i2 << " / " << pParams->SDLIB2->XNum*pParams->SDLIB2->YNum << std::endl;
                // get data block
                pParams->SDLIB2->getDataBlock(pParams->i2+1,pParams->SDLIB2->XNum);
                // match data block
                MahDAT SDDAT2 = michi2MatchObs(pParams->SDOBS,pParams->SDLIB2);
                // <TODO><Debug>
                // std::cout << i1 << " " << i2 << " / " << pParams->SDLIB2->XNum*pParams->SDLIB2->YNum << std::endl;
                // check data block
                if(SDDAT2.f1.empty()) {continue;}
                if(!SDDAT2.f1.empty()) {
                    // loop the thrid lib
                    for(pParams->i3=0; pParams->i3<(pParams->SDLIB3->XNum)*(pParams->SDLIB3->YNum); pParams->i3+=(pParams->SDLIB3->XNum))
                    {
                        // <TODO><Debug>
                        // std::cout << i1 << " " << i2 << " / " << pParams->SDLIB2->XNum*pParams->SDLIB2->YNum << std::endl;
                        // get data block
                        pParams->SDLIB3->getDataBlock(pParams->i3+1,pParams->SDLIB3->XNum);
                        // match data block
                        MahDAT SDDAT3 = michi2MatchObs(pParams->SDOBS,pParams->SDLIB3);
                        // <TODO><Debug>
                        // std::cout << i1 << " " << i2 << " / " << pParams->SDLIB2->XNum*pParams->SDLIB2->YNum << std::endl;
                        // check data block
                        if(SDDAT3.f1.empty()) {continue;}
                        if(!SDDAT3.f1.empty()) {
                            // loop the fourth lib
                            // <added><20150911><dzliu> // lock the fourth lib to the third lib, i.e. DL07 cold/warm.
#ifdef LockDL07
                            for(pParams->i4=pParams->i3; pParams->i4<pParams->i3+1; pParams->i4+=1)
#else
                            for(pParams->i4=0; pParams->i4<(pParams->SDLIB4->XNum)*(pParams->SDLIB4->YNum); pParams->i4+=(pParams->SDLIB4->XNum))
#endif
                            {
                                // <TODO><Debug>
                                // std::cout << i1 << " " << i2 << " / " << pParams->SDLIB2->XNum*pParams->SDLIB2->YNum << std::endl;
                                // get data block
                                pParams->SDLIB4->getDataBlock(pParams->i4+1,pParams->SDLIB4->XNum);
                                // match data block
                                MahDAT SDDAT4 = michi2MatchObs(pParams->SDOBS,pParams->SDLIB4);
                                // <TODO><Debug>
                                // std::cout << i1 << " " << i2 << " / " << pParams->SDLIB2->XNum*pParams->SDLIB2->YNum << std::endl;
                                // check data block
                                if(SDDAT4.f1.empty()) {continue;}
                                // min chi2 by 4 components
                                std::vector<std::vector<double> > fLIB;
                                fLIB.push_back(SDDAT1.f1);
                                fLIB.push_back(SDDAT2.f1);
                                fLIB.push_back(SDDAT3.f1);
                                fLIB.push_back(SDDAT4.f1);
                                std::vector<double> fOBS = SDDAT4.f0;
                                std::vector<double> eOBS = SDDAT4.df;
                                michi2MinPack *MPACK = new michi2MinPack(fLIB,fOBS,eOBS);
                                std::vector<double> f1 = pParams->SDLIB1->Y;
                                std::vector<double> f2 = pParams->SDLIB2->Y;
                                std::vector<double> f3 = pParams->SDLIB3->Y;
                                std::vector<double> f4 = pParams->SDLIB4->Y;
                                double a1 = MPACK->aCOE[0];
                                double a2 = MPACK->aCOE[1];
                                double a3 = MPACK->aCOE[2];
                                double a4 = MPACK->aCOE[3];
                                double chi2 = 0.0; for(int j=0;j<MPACK->chi2.size();j++) {chi2+=MPACK->chi2[j];}
                                delete MPACK;
                                // check parameter and output results to stream
                                pStream << std::setw(8) << pParams->i0 << std::setw(15) << chi2 << std::setw(10) << pParams->i1 << std::setw(15) << a1 << std::setw(10) << pParams->i2 << std::setw(15) << a2 << std::setw(10) << pParams->i3 << std::setw(15) << a3 << std::setw(10) << pParams->i4 << std::setw(15) << a4;
                                for(int ipar=0; ipar<pParams->SDLIB1->FPAR.size(); ipar++) { pStream << std::setw(15) << pParams->SDLIB1->FPAR[ipar][0]; }
                                for(int ipar=0; ipar<pParams->SDLIB2->FPAR.size(); ipar++) { pStream << std::setw(15) << pParams->SDLIB2->FPAR[ipar][0]; }
                                for(int ipar=0; ipar<pParams->SDLIB3->FPAR.size(); ipar++) { pStream << std::setw(15) << pParams->SDLIB3->FPAR[ipar][0]; }
                                for(int ipar=0; ipar<pParams->SDLIB4->FPAR.size(); ipar++) { pStream << std::setw(15) << pParams->SDLIB4->FPAR[ipar][0]; }
                                pStream << std::endl;
                                pParams->i0++;
                            }
                        }
                    }
                }
            }
        }
    }
    // lock mutex
    pthread_mutex_lock(&m4chi2parallelMutex);
    std::cout << "m4chi2parallel: i1begin=" << pParams->i1begin << " ? m4chi2parallelProgress=" << m4chi2parallelProgress << " now locked!" << std::endl;
    for(;;) {
        // test if now we are ok to write result file
        if(pParams->i1begin==m4chi2parallelProgress) {
            // thread ok to write!
            std::cout << "m4chi2parallel: i1begin=" << pParams->i1begin << " ? m4chi2parallelProgress=" << m4chi2parallelProgress << " got ya!";
            if(!pStream.str().empty()) {*pParams->SDOUT<<pStream.str();}
            std::cout << std::endl;
            pthread_cond_signal(&m4chi2parallelCondition); // send finish signal
            break;
            // pthread_mutex_unlock(&m4chi2parallelMutex); // unlock mutex
            // delete SDLIB1; delete SDLIB2; delete SDLIB3; delete SDLIB4; return(NULL);
        } else {
            // wait while other earlier threads to write result file
            pthread_cond_wait(&m4chi2parallelCondition, &m4chi2parallelMutex);
            std::cout << "m4chi2parallel: i1begin=" << pParams->i1begin << " ? m4chi2parallelProgress=" << m4chi2parallelProgress << " now waiting" << std::endl;
        }
    }
    // unlock mutex
    // <TODO><Delete> m4chi2parallelProgress += pParams->SDLIB1->XNum;
    m4chi2parallelProgress = pParams->i1end;
    pthread_mutex_unlock(&m4chi2parallelMutex);
    std::cout << "m4chi2parallel: i1begin=" << pParams->i1begin << " ? m4chi2parallelProgress=" << m4chi2parallelProgress << " now unlocked!" << std::endl;
    // finish at last! <TODO><Debug> should not happen
    if(pParams->i1begin<=m4chi2parallelProgress) {delete SDLIB1; delete SDLIB2; delete SDLIB3; delete SDLIB4; return(NULL);}
    return(NULL);
}








/* 20160714 mnchi2 */
/* --- pthread ---- */
struct mnchi2parallelParams {
    std::vector<michi2DataClass*> SDOBSList;
    std::vector<michi2DataClass*> SDLIBList;
    //std::vector<std::ofstream*> SDOUTList;
    std::vector<std::string> InputLibList;
    std::vector<std::string> OutputTableList;
                long  ibOBS;     //    begin index of OBS list
                long  idOBS;     //          index of OBS list
                long  ieOBS;     //      end index of OBS list
                bool  irOBS;     // rounding index of OBS list 就是计算是否进位
    std::vector<long> ibLIBList; //    begin index of LIB list
    std::vector<long> idLIBList; //          index of LIB list
    std::vector<long> ieLIBList; //      end index of LIB list
    std::vector<bool> irLIBList; // rounding index of LIB list 就是计算是否进位
    long iBegin; long iEnd; long i;
    long nObs; long nLib; long nRow;
};
pthread_mutex_t mnchi2parallelMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t mnchi2parallelCondition = PTHREAD_COND_INITIALIZER;
long mnchi2parallelProgress = 0;
/* ---------------- */

void mnchi2(std::vector<std::string> InputObsList, std::vector<std::string> InputLibList, std::vector<std::string> OutputTableList)
{
    // check input obs data file list
    // and prepare obs data number
    if(0==InputObsList.size()) {
        std::cout << "Error! mnchi2 InputObsList is empty!" << std::endl; return;
    }
    std::vector<michi2DataClass*> SDOBSList; SDOBSList.clear();
    long NumbObs = InputObsList.size();
    for(int i=0; i<InputObsList.size(); i++) {
        michi2DataClass *SDOBS = new michi2DataClass(InputObsList.at(i).c_str());
        SDOBSList.push_back(SDOBS);
    }
    // check input lib data file list
    // and prepare lib data structure
    if(0==InputLibList.size()) {
        std::cout << "Error! mnchi2 InputLibList is empty!" << std::endl; return;
    }
    std::vector<michi2DataClass*> SDLIBList; SDLIBList.clear();
    long NumbLib = InputLibList.size(), NumbLibPar = 0, NumbLibVar = 0, NumbLibMulti = 1;
    for(int i=0; i<InputLibList.size(); i++) {
        michi2DataClass *SDLIB = new michi2DataClass(InputLibList.at(i).c_str());
        SDLIBList.push_back(SDLIB);
        NumbLibPar += SDLIB->TPAR.size(); // SDLIB->TPAR is the list of parameter title in a LIB file, so its size() is the number of parameters to be fit in a LIB file.
        NumbLibVar += SDLIB->YNum;
        NumbLibMulti = NumbLibMulti * SDLIB->YNum;
    }
    // check output table data file list
    // set default output table data file name for each input obs data file
    // and prepare output file pointer
    if(0==OutputTableList.size()) {
        std::cout << "Error! mnchi2 OutputTableList is empty!" << std::endl; return;
    }
    //std::vector<std::ofstream*> SDOUTList; SDOUTList.clear();
    for(int i=0; i<InputObsList.size(); i++) {
        if(OutputTableList.size()<i) {
            OutputTableList.push_back(InputObsList.at(i).insert(0,"mnchi2-").append(".out"));
        }
        std::ofstream SDOUT(OutputTableList.at(i).c_str());
        //SDOUTList.push_back(&SDOUT);
        SDOUT << "#" << std::setw(7) << "i0" << std::setw(15) << "chi2"; // write the output file header line
        std::ostringstream tmpstr;
        for(long iLib=0; iLib<InputLibList.size(); iLib++) {
            tmpstr.str(""); tmpstr << "i" << iLib+1; SDOUT << std::setw(10) << tmpstr.str();
            tmpstr.str(""); tmpstr << "a" << iLib+1; SDOUT << std::setw(15) << tmpstr.str();
        }
        for(long iLib=0; iLib<InputLibList.size(); iLib++) {
            michi2DataClass *SDLIB = SDLIBList.at(i);
            for(long iLibPar=0; iLibPar<SDLIB->TPAR.size(); iLibPar++) {
                SDOUT << std::setw(15) << SDLIB->TPAR[iLibPar]; // write each title of parameter to be fit in each LIB file
            }
        }
        SDOUT << std::endl << "#" << std::endl;
        SDOUT.close();
    }
    // prepare to run parallel processes <New><20141211><20141212><20160714><20160718>
    mnchi2parallelProgress = 0;
    std::vector<struct mnchi2parallelParams *> mnchi2parallelParams;
    std::cout << "processing: " << std::endl;
    int iPara = NumbParallel;
    long iLoop = 0;
    long iStep = long((float(NumbObs*NumbLibMulti)+float(iPara)-1)/float(iPara)); //<TODO> 2 is the paralle process number, will be allowed from user input. //<NOTE> +float(iPara)-1 to make sure we cover all the ranges.
    while(iLoop >= 0 && iLoop < NumbObs*NumbLibMulti) {
        // determine current loop step, in case of overflow
        if((iLoop+iStep)>NumbObs*NumbLibMulti) {iStep=NumbObs*NumbLibMulti-iLoop;}
        //std::cout << "looping " << iLoop+1 << "-" << iLoop+iStep-1+1 << " / " << NumbLibMulti << "   ";
        struct mnchi2parallelParams *pParams = new struct mnchi2parallelParams;
        //<TODO><DELETE>// pParams->i0=0;
        //<TODO><DELETE>// for(long iLib=0; iLib<NumbLib; iLib++) {pParams->iList[iLib]=0;}
        //std::cout << pParams << std::endl; // check that the param strcut is well set
        mnchi2parallelParams.push_back(pParams);
        //
        // all subprocesses share the same OBS data structure
        pParams->SDOBSList = SDOBSList;
        //
        // prepare separated LIB data structure, otherwise error occurs
        //pParams->SDLIBList = SDLIBList;
        pParams->SDLIBList.clear();
        for(int i=0; i<InputLibList.size(); i++) {
            michi2DataClass *SDLIB = new michi2DataClass(InputLibList.at(i).c_str());
            pParams->SDLIBList.push_back(SDLIB);
        }
        //
        // divide all loops into parallel sub processes
        pParams->i = iLoop;
        pParams->iBegin = iLoop;
        pParams->iEnd = iLoop+iStep-1;
        pParams->nObs = NumbObs;
        pParams->nLib = NumbLib;
        pParams->nRow = NumbLibMulti;
        //std::cout << "DEBUG: Total scan " << NumbLibMulti << " iLoop " << iLoop << " iStep " << iStep << std::endl;
        //
        // initialize the loop range of each OBS and LIB index
        //pParams->idOBS = (long(iLoop/NumbLibMulti));
        //pParams->ibOBS = (long(iLoop/NumbLibMulti));
        //pParams->ieOBS = (long((iLoop+iStep-1)/NumbLibMulti));
        //pParams->irOBS = true;
        //long iRest = iLoop%NumbLibMulti;
        //long iNext = (iLoop+iStep-1)%NumbLibMulti;
        //long iTemp = NumbLibMulti;
        for(long iLib = 0; iLib < SDLIBList.size(); iLib++) {
            //michi2DataClass *SDLIB = SDLIBList.at(iLib);
            //iTemp = iTemp / SDLIB->YNum;
            pParams->idLIBList.push_back(0); // controls the parallel processes
            pParams->ibLIBList.push_back(0); // controls the parallel processes
            pParams->ieLIBList.push_back(0); // controls the parallel processes
            pParams->irLIBList.push_back(true);              // controls the parallel processes
            //iRest = iRest%iTemp;
            //iNext = iNext%iTemp;
        }
        pParams->ibOBS = 0;
        pParams->ieOBS = 0;
        //
        // compute the rounding of each OBS and LIB index so as to
        // compute the loop range of each OBS and LIB index
        long iTemp = 0;
        while(iTemp < pParams->iBegin) {
            for(long iLib = pParams->nLib-1; iLib >= 0 ; iLib--) {
                pParams->ibLIBList[iLib]++;
                if((pParams->ibLIBList[iLib])>(pParams->SDLIBList[iLib]->YNum-1)) { // rounding overflow
                    pParams->ibLIBList[iLib] = 0; // rewind to zero or begin value
                    if(0==iLib) { // if the highest LIB got rounded, then we need to round idOBS as well.
                        pParams->ibOBS++;
                        break;
                    }
                } else {
                    break; // if no rounding overflow happens, it will break here, so only the last LIB index was increased.
                }
            }
            iTemp++;
        }
        iTemp = 0;
        while(iTemp < pParams->iEnd) {
            for(long iLib = pParams->nLib-1; iLib >= 0 ; iLib--) {
                pParams->ieLIBList[iLib]++;
                if((pParams->ieLIBList[iLib])>(pParams->SDLIBList[iLib]->YNum-1)) { // rounding overflow
                    pParams->ieLIBList[iLib] = 0; // rewind to zero or begin value
                    if(0==iLib) { // if the highest LIB got rounded, then we need to round idOBS as well.
                        pParams->ieOBS++;
                        break;
                    }
                } else {
                    break; // if no rounding overflow happens, it will break here, so only the last LIB index was increased.
                }
            }
            iTemp++;
        }
        // set initial loop values
        pParams->idOBS = pParams->ibOBS;
        pParams->irOBS = true;
        for(long iLib = 0; iLib < pParams->nLib ; iLib++) {
            pParams->idLIBList[iLib] = pParams->ibLIBList[iLib];
            pParams->irLIBList[iLib] = true;
        }
        // print info and prepare next loop
        std::cout << "looping " << iLoop+1 << "-" << iLoop+iStep-1+1 << "/" << NumbLibMulti;
        std::cout << " OBS " << pParams->ibOBS+1 << "-" << pParams->ieOBS+1;
        for(long iLib = 0; iLib < SDLIBList.size(); iLib++) {
            std::cout << " LIB" << iLib+1 << " " << pParams->ibLIBList.at(iLib)+1 << "-" << pParams->ieLIBList.at(iLib)+1 << "/" << pParams->SDLIBList.at(iLib)->YNum;
        }
        std::cout << std::endl;
        iLoop = iLoop + iStep;
        // setup OUT list
        //pParams->SDOUTList = SDOUTList;
        pParams->InputLibList = InputLibList;
        pParams->OutputTableList = OutputTableList;
        // <TODO><DEBUG>
        //sleep(15);
        // <20150309> make pthread
        pthread_t thread1; pthread_create(&thread1, NULL, &mnchi2parallel, pParams);
        // <TODO><DEBUG>
        // if(i1>=500) {sleep(60);} else {sleep(2);}
        sleep(5);
    }
    // show initial info for all threads
    //std::cout << std::setw(8) << std::right << mnchi2parallelProgress << "||";
    for(long ip=0; ip<mnchi2parallelParams.size(); ip++) {
        std::cout << std::setw(15) << std::right << mnchi2parallelParams[ip]->iBegin << "|" << mnchi2parallelParams[ip]->nObs*mnchi2parallelParams[ip]->nRow << "|";
        for(long iLib = 0; iLib < mnchi2parallelParams[ip]->nLib; iLib++) {
            std::cout << std::setw(8) << std::right << 0 << "|" << mnchi2parallelParams[ip]->SDLIBList[iLib]->YNum << "|";
        }
    }
    std::cout << std::endl;
    // wait for all threads
    while(mnchi2parallelProgress<NumbObs*NumbLibMulti) {
        //std::cout << std::setw(8) << std::right << mnchi2parallelProgress << "||";
        for(long ip=0; ip<mnchi2parallelParams.size(); ip++) {
            //std::cout << std::setw(15) << std::right << mnchi2parallelParams[ip]->i << "|" << mnchi2parallelParams[ip]->nObs*mnchi2parallelParams[ip]->nRow << "|";
            std::cout << std::setw(15) << std::right << mnchi2parallelParams[ip]->i << "|" << mnchi2parallelParams[ip]->iEnd << "|";
            for(long iLib = 0; iLib < mnchi2parallelParams[ip]->nLib; iLib++) {
                std::cout << std::setw(8) << std::right << mnchi2parallelParams[ip]->idLIBList[iLib] << "|" << mnchi2parallelParams[ip]->SDLIBList[iLib]->YNum << "|";
            }
        }
        std::cout<< std::endl;
        sleep(3);
    }
    // clean
    for(long ip=0; ip<mnchi2parallelParams.size(); ip++) {
        delete mnchi2parallelParams[ip]; mnchi2parallelParams[ip]=NULL;
    }
    //
    std::cout << std::setw(15) << std::right << "100%" << "||" << std::endl;
}

void *mnchi2parallel(void *params)
{
    int debug = 0;
    struct mnchi2parallelParams *pParams = (struct mnchi2parallelParams *)params;
    if(debug>=1) {std::cout << "DEBUG: Make parallel process " << pParams << std::endl;}
    std::vector<std::string> pStrings; pStrings.resize(pParams->nObs);
    //std::vector<std::stringstream*> pStreams;
    //for(long iObs = 0; iObs < pParams->nObs; iObs++) {
    //    std::stringstream pStream;
    //    pStreams.push_back(&pStream);
    //}
    //std::vector<std::stringstream> pStreams; pStreams.resize(pParams->nObs);
    //
    // check loop begin status
    if(pParams->i != pParams->iBegin) {
        std::cout << "Error! pParams->i != pParams->iBegin! This should not happen!" << std::endl;
    }
    ////
    //// check loop rounding status
    //pParams->irOBS = true;
    //for(long iLib = 0; iLib < pParams->nLib; iLib++) {
    //    pParams->irLIBList[iLib] = true;
    //}
    //
    // prepare the list of michi2DataClass
    michi2DataClass *SDOBS = NULL;
    std::vector<michi2DataClass*> SDLIBS; SDLIBS.resize(pParams->nLib);
    //
    // loop
    while (pParams->i <= pParams->iEnd) {
        if(debug>=1) {
            std::cout << "DEBUG: Loop " << pParams->i << "/" << pParams->iEnd << " OBS" << pParams->idOBS+1;
            for(long iLib = 0; iLib < pParams->nLib; iLib++) {
                std::cout << " LIB" << iLib+1 << " data block " << pParams->idLIBList.at(iLib)+1 << "/" << pParams->SDLIBList.at(iLib)->YNum;
            } std::cout << std::endl;
        }
        //
        // get OBS data block
        if(pParams->irOBS) {
            SDOBS = pParams->SDOBSList.at(pParams->idOBS);
            //SDOBS->getDataBlock(pParams->idOBS * SDOBS->XNum + 1, SDOBS->XNum);
            //if(debug>=2) {std::cout << "DEBUG: Read OBS" << pParams->idOBS+1 << std::endl;}
            pParams->irOBS = false;
        }
        //
        // get LIB data block
        for(long iLib = 0; iLib < pParams->nLib; iLib++) {
            if(pParams->irLIBList.at(iLib)) {
                SDLIBS[iLib] = pParams->SDLIBList.at(iLib);
                SDLIBS[iLib]->getDataBlock(pParams->idLIBList.at(iLib) * SDLIBS[iLib]->XNum + 1, SDLIBS[iLib]->XNum);
                if(debug>=2) {std::cout << "DEBUG: Read LIB" << iLib+1 << " data block " << pParams->idLIBList.at(iLib)+1 << "/" << pParams->SDLIBList.at(iLib)->YNum << std::endl;}
                pParams->irLIBList[iLib] = false;
            }
        }
        //
        // prepare MinPack input data structure
        std::vector<std::vector<double> > fLIB; fLIB.clear();
        std::vector<double> fOBS;
        std::vector<double> eOBS;
        std::vector<double> aFIT; aFIT.resize(pParams->nLib);
        //
        // check constraints <Added><20160719><dzliu>
        bool ConstraintOK = true;
        if(Constraints.size()>0) {
            for(int iCon = 0; iCon < Constraints.size(); iCon++) {
                ConstraintStructure *TempConstraint = Constraints[iCon];
                if(TempConstraint->fromLIB>0 && TempConstraint->toLIB>0 && TempConstraint->fromPAR>0 && TempConstraint->toPAR>0) {
                    // set constraint: toLIB.toPAR \Operator\ fromLIB.fromPAR
                    double TempPAR1 = SDLIBS[TempConstraint->toLIB-1]->FPAR[TempConstraint->toPAR-1][0];
                    double TempPAR2 = SDLIBS[TempConstraint->fromLIB-1]->FPAR[TempConstraint->fromPAR-1][0];
                    if(TempConstraint->OperatorType == 0) {
                        ConstraintOK = (TempPAR1 == TempPAR2); // set constraint: toLIB.toPAR EQ fromLIB.fromPAR
                    } else if(TempConstraint->OperatorType == 1) {
                        ConstraintOK = (TempPAR1 >= TempPAR2); // set constraint: toLIB.toPAR GE fromLIB.fromPAR
                    } else if(TempConstraint->OperatorType == -1) {
                        ConstraintOK = (TempPAR1 <= TempPAR2); // set constraint: toLIB.toPAR LE fromLIB.fromPAR
                    } else if(TempConstraint->OperatorType == 2) {
                        ConstraintOK = (TempPAR1 > TempPAR2); // set constraint: toLIB.toPAR GT fromLIB.fromPAR
                    } else if(TempConstraint->OperatorType == -2) {
                        ConstraintOK = (TempPAR1 < TempPAR2); // set constraint: toLIB.toPAR LT fromLIB.fromPAR
                    }
                } else if(TempConstraint->fromLIB>0 && TempConstraint->toLIB>0 && TempConstraint->fromPAR==0 && TempConstraint->toPAR==0) {
                    // set constraint: toLIB.toINDEX \Operator\ toINDEX.fromINDEX
                    double TempINDEX1 = pParams->idLIBList.at(TempConstraint->toLIB-1);
                    double TempINDEX2 = pParams->idLIBList.at(TempConstraint->fromLIB-1);
                    //std::cout << "set constraint: toLIB.toINDEX EQ fromLIB.fromINDEX: " << TempINDEX1 << " " << TempINDEX2;
                    if(TempConstraint->OperatorType == 0) {
                        ConstraintOK = (TempINDEX1 == TempINDEX2); // set constraint: toLIB.toINDEX EQ fromLIB.fromINDEX
                    } else if(TempConstraint->OperatorType == 1) {
                        ConstraintOK = (TempINDEX1 >= TempINDEX2); // set constraint: toLIB.toINDEX GE fromLIB.fromINDEX
                    } else if(TempConstraint->OperatorType == -1) {
                        ConstraintOK = (TempINDEX1 <= TempINDEX2); // set constraint: toLIB.toINDEX LE fromLIB.fromINDEX
                    } else if(TempConstraint->OperatorType == 2) {
                        ConstraintOK = (TempINDEX1 > TempINDEX2); // set constraint: toLIB.toINDEX GT fromLIB.fromINDEX
                    } else if(TempConstraint->OperatorType == -2) {
                        ConstraintOK = (TempINDEX1 < TempINDEX2); // set constraint: toLIB.toINDEX LT fromLIB.fromINDEX
                    }
                    //std::cout << " " << ConstraintOK << std::endl;
                } else {
                    // set constraint: toLIB[LowerX:UpperX]*Factor = toLIB[LowerX:UpperX]*Factor
                    // <TODO><20160719><dzliu>
                    // ITS TOO HARD TO IMPLEMENT SO MANY THINGS
                    // Constraining the normalization of a LIB is not what should be constrained here.
                    // This code should only be a tool to compute minimum chi2 from OBS data with multiple LIB data.
                    // We can set constraints on LIB parameters, LIB index, but not LIB normalization!
                }
            }
        }
        //
        // match LIB to OBS data block
        if(ConstraintOK) {
            for(long iLib = 0; iLib < pParams->nLib; iLib++) {
                if(debug>=2) {std::cout << "DEBUG: MahD OBS" << pParams->idOBS+1 << " LIB" << iLib+1 << std::endl;}
                MahDAT SDDAT = michi2MatchObs(SDOBS, SDLIBS[iLib]); // , debug
                // check data block
                if(!SDDAT.f1.empty()) {
                    // min chi2 by n components
                    fLIB.push_back(SDDAT.f1);
                    if(iLib == (pParams->nLib-1)) {
                        fOBS = SDDAT.f0; eOBS = SDDAT.df;
                    }
                } else {
                    std::cout << "Error! Failed to read data block from LIB" << iLib+1 << " " << pParams->InputLibList.at(iLib) << std::endl;
                    return(NULL);
                }
            }
        }
        //
        // do MinPack
        if(fLIB.size()>0) {
            if(debug>=2) {std::cout << "DEBUG: Call MPACK" << std::endl;}
            //
            michi2MinPack *MPACK = new michi2MinPack(fLIB,fOBS,eOBS);
            //
            // check parameter and output results to stream
            double chi2 = 0.0; for(int j=0;j<MPACK->chi2.size();j++) {chi2+=MPACK->chi2[j];}
            std::stringstream pStream;
            pStream << std::setw(8) << pParams->i << std::setw(15) << chi2;
            for(long iLib = 0; iLib < pParams->nLib; iLib++) {
                aFIT[iLib] = MPACK->aCOE[iLib];
                pStream << std::setw(10) << pParams->idLIBList.at(iLib) * pParams->SDLIBList.at(iLib)->XNum << std::setw(15) << aFIT.at(iLib);
            }
            for(long iLib = 0; iLib < pParams->nLib; iLib++) {
                //michi2DataClass *SDLIB = pParams->SDLIBList.at(iLib);
                michi2DataClass *SDLIB = SDLIBS.at(iLib);
                for(long iLibPar=0; iLibPar<SDLIB->FPAR.size(); iLibPar++) {
                    pStream << std::setw(15) << SDLIB->FPAR[iLibPar][0];
                }
            }
            delete MPACK; MPACK = NULL;
            pStream << std::endl;
            pStrings[pParams->idOBS] += pStream.str();
            if(debug>=2) {std::cout << "DEBUG: Done MPACK" << pStream.str();
                std::cout << std::endl;}
        }
        //
        // compute the rounding of each LIB index idLIBList[iLib] and goto next loop
        for(long iLib = pParams->nLib-1; iLib >= 0 ; iLib--) {
            pParams->idLIBList[iLib]++;
            pParams->irLIBList[iLib] = true;
            if((pParams->idLIBList[iLib])>(pParams->SDLIBList[iLib]->YNum-1)) { // rounding overflow
                pParams->idLIBList[iLib] = 0; // rewind to zero or begin value
                if(0==iLib) { // if the highest LIB got rounded, then we need to round idOBS as well.
                    // great we have done all the LIB loops for one OBS data, move to the next OBS data.
                    pParams->idOBS++;
                    pParams->irOBS = true;
                    break;
                }
            } else {
                break; // if no rounding overflow happens, it will break here, so only the last LIB index was increased.
            }
        }
        //
        // goto next loop
        pParams->i++;
    }
    // lock mutex
    pthread_mutex_lock(&mnchi2parallelMutex);
    std::cout << "mnchi2parallel: iBegin=" << pParams->iBegin << " ? mnchi2parallelProgress=" << mnchi2parallelProgress << " now locked!" << std::endl;
    for(;;) {
        // test if now we are ok to write result file
        if(pParams->iBegin==mnchi2parallelProgress) {
            // thread ok to write!
            std::cout << "mnchi2parallel: iBegin=" << pParams->iBegin << " ? mnchi2parallelProgress=" << mnchi2parallelProgress << " got ya!";
            for(long iObs = 0; iObs < pParams->nObs; iObs++) {
                if(!pStrings[iObs].empty()) {
                    std::ofstream SDOUT(pParams->OutputTableList.at(iObs).c_str(), std::ofstream::out | std::ofstream::app);
                    SDOUT << pStrings[iObs];
                    SDOUT.close();
                    //*pParams->SDOUTList.at(iObs) << pStrings[iObs] << std::flush;
                }
                //<TODO><DELETE>// if(1) {std::cout << "DEBUG: pStrings[iObs]" << pStrings[iObs] << std::endl;}
                //pParams->SDOUTList.at(iObs)->close();
            }
            std::cout << std::endl;
            pthread_cond_signal(&mnchi2parallelCondition); // send finish signal
            break;
            // pthread_mutex_unlock(&mnchi2parallelMutex); // unlock mutex
            // delete SDLIB1; delete SDLIB2; delete SDLIB3; delete SDLIB4; return(NULL);
        } else {
            // wait while other earlier threads to write result file
            pthread_cond_wait(&mnchi2parallelCondition, &mnchi2parallelMutex);
            std::cout << "mnchi2parallel: iBegin=" << pParams->iBegin << " ? mnchi2parallelProgress=" << mnchi2parallelProgress << " now waiting" << std::endl;
        }
    }
    // unlock mutex
    pthread_mutex_unlock(&mnchi2parallelMutex);
    std::cout << "mnchi2parallel: iBegin=" << pParams->iBegin << " ? mnchi2parallelProgress=" << mnchi2parallelProgress << " now unlocked!" << std::endl;
    // update mnchi2parallelProgress
    mnchi2parallelProgress = pParams->i;
    // return
    return(NULL);
}
























