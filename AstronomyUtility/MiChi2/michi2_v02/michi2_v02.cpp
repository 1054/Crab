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
#define  LockDL07    // if define this, then we will lock the last two components. Suitable for DL07 cold+warm dust lib. 
//#undef   LockDL07    //

using namespace std;

extern const char* InfoRedshift;

typedef struct {std::vector<double> j0; std::vector<double> f0; std::vector<double> df; std::vector<double> f1;} MahDAT;

MahDAT michi2MatchObs(michi2DataClass *DCOBS, michi2DataClass *DCLIB);

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




MahDAT michi2MatchObs(michi2DataClass *DCOBS, michi2DataClass *DCLIB)
{
    MahDAT MatchedDAT;
    for(int j=0; j<DCLIB->XNum; j++) {
        if(DCLIB->Y[j]<=0.0) {return MatchedDAT;} // <TODO> do not allow negative LIB
    }
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
    
    
    int debug = 0; // <TODO><Debug>
    
    
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
























