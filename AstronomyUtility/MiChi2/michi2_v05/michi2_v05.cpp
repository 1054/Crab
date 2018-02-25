/* 
 
 HIST: 20180224
       Metropolis Hasting Algorithm
       https://ysquared2.wordpress.com/2014/03/27/metropolis-hasting-algorithm-an-example-with-cc-and-r/
 
 */



#include "michi2_v05.h"
#include "spline.cpp"
#include "integrate.cpp"
extern vector<double> spline(vector<double> &x, vector<double> &y, vector<double> &output_x);
extern double integrate(vector<double> &x, vector<double> &y, vector<double> &xrange);
extern double integrate_LIR(vector<double> &x, vector<double> &y, vector<double> &xrange);



const char *InfoRedshift = "";

int NumbParallel = 2; // number of parallel subprocesses

double GlobalMinimumChisq = std::nan(""); // GlobalMinimumChisq

std::vector<michi2Constraint *> Constraints;

std::vector<FilterCurveXY *> FilterCurves;



MatchedObsLibStruct michi2MatchObs(michi2DataClass *DCOBS, michi2DataClass *DCLIB, int debug)
{
    // This function will find the Y value in LIB whose X value matches the X value of each OBS
    
    MatchedObsLibStruct MatchedDAT;
    //std::cout << "michi2MatchObs: MatchedDAT=0x" << std::hex << (size_t)&MatchedDAT << std::endl;
    
    // check DCOBS
    if(DCOBS->X.size()!=DCOBS->XNum || DCLIB->X.size()!=DCLIB->Y.size()) {
        std::cout << "michi2MatchObs: checking OBS and LIB data failed! OBS->X.size()=" << DCOBS->X.size() << " OBS->XNum=" << DCOBS->XNum << " OBS->Y.size()=" << DCOBS->Y.size() << " OBS->YNum=" << DCOBS->YNum << std::endl;
        return MatchedDAT;
    }
    
    
    // get LIB X range
    double LibRangeXMin = DCLIB->X[0];
    double LibRangeXMax = DCLIB->X[0];
    for(int j=0; j<DCLIB->XNum; j++) {
        if(DCLIB->X[j]<LibRangeXMin){LibRangeXMin=DCLIB->X[j];}
        if(DCLIB->X[j]>LibRangeXMax){LibRangeXMax=DCLIB->X[j];}
    }

    //debug = 3; // <Debug> // <20160718> now put in argument

    double OnePlusRedshift = 1.0; //
    if(strlen(InfoRedshift)>0) { OnePlusRedshift = 1.0+atof(InfoRedshift); }

    
    for(int k=0; k<DCOBS->XNum; k++) {
        DCOBS->Matched[k]=0; // <TODO> we will always find a match for obs.
        if(debug>=2) { std::cout << "michi2MatchObs: debugging: we are trying to match obs X=" << DCOBS->XStr[k] << "/" << OnePlusRedshift << "=" << DCOBS->X[k]/OnePlusRedshift << " within lib X range " << LibRangeXMin << " " << LibRangeXMax << std::endl; }
        if(DCOBS->X[k]/OnePlusRedshift>=LibRangeXMin && DCOBS->X[k]/OnePlusRedshift<=LibRangeXMax) {
            DCOBS->Matched[k]=1; // <TODO> we will always find a match for obs.
            // now we need to find a value from library
            double tmpdiffvalue = -1.0;
            double mindiffvalue = -1.0;
            long   mindiffindex = -1;
            for(int j=0; j<DCLIB->XNum; j++) {
                tmpdiffvalue = (DCOBS->X[k]/OnePlusRedshift - DCLIB->X[j]); // go search for the left side of OBS data point on X axis
                if(tmpdiffvalue>=0.0 && mindiffvalue<0.0) { mindiffvalue=tmpdiffvalue; mindiffindex=j; } // initial value
                if(tmpdiffvalue>=0.0 && tmpdiffvalue<mindiffvalue) { mindiffvalue=tmpdiffvalue; mindiffindex=j; } // compare value
            }
            // once got matched, we do linear interpolate
            // <20170930> or apply filter curve if user has input the filter curve ascii files
            if(mindiffindex>=0 && mindiffindex<=DCLIB->XNum-1) {
                int j = mindiffindex;
                int MatchedLibI = j;
                double MatchedLibY = DCLIB->Y[j];
                if(debug>=1) { std::cout << "michi2MatchObs: debugging: got matched lib Y value " << MatchedLibY << " at X between " << DCLIB->X[mindiffindex] << " " << DCLIB->X[mindiffindex+1] << std::endl; }
                // do linear interpolate to get more accurate MatchedLibY
                if(j<DCLIB->XNum-1) {
                    double f1_l1 = DCLIB->Y[j]; // left-side LIB data point
                    double f1_l2 = DCLIB->Y[j+1]; // right-side LIB data point, e.g. if we are matching ObsX = 24.0, LibX = 23.95, then we interpolate LibX(23.95,24.15)=>ObsX(24.00)
                    double f1_a1 = (DCOBS->X[k]/OnePlusRedshift - DCLIB->X[j]); // as the e.g., a1 =  0.05
                    double f1_a2 = (DCLIB->X[j+1] - DCOBS->X[k]/OnePlusRedshift); // as the e.g., a2 =  0.15
                    double f1_ai = (f1_a1+f1_a2); // then f1 = f1_l1 * 75% + f1_l2 * 25%
                    double f1_li = f1_l1*(f1_a2/f1_ai)+f1_l2*(f1_a1/f1_ai); // <TODO> bi-linear interpolation!
                    MatchedLibY = f1_li;
                    if(debug>=1) { std::cout << "michi2MatchObs: debugging: interpolated accurate lib Y value " << MatchedLibY << " at obs X " << DCOBS->X[k]/OnePlusRedshift << std::endl; }
                }
                //
                // <20170930>
                // see whether user has input filter curve ascii files
                // the user should input as many filter curve ascii files as the OBS data point number
                std::string TempFilterCurveFilePath; TempFilterCurveFilePath.clear();
                std::vector<double> FilterCurveX; FilterCurveX.clear();
                std::vector<double> FilterCurveY; FilterCurveY.clear();
                if(FilterCurves.size()>k) {
                    if(FilterCurves[k]) {
                        // re-use global variable
                        //FilterCurveX.resize(FilterCurves[k]->X.size()); FilterCurveX.assign(FilterCurves[k]->X.begin(), FilterCurves[k]->X.end());
                        //FilterCurveY.resize(FilterCurves[k]->Y.size()); FilterCurveY.assign(FilterCurves[k]->Y.begin(), FilterCurves[k]->Y.end());
                        FilterCurveX.clear(); FilterCurveX.assign(FilterCurves[k]->X.begin(), FilterCurves[k]->X.end());
                        FilterCurveY.clear(); FilterCurveY.assign(FilterCurves[k]->Y.begin(), FilterCurves[k]->Y.end());
                        if(debug>=1) { std::cout << "michi2MatchObs: debugging: applying filter curve from FilterCurves[" << k << "]" << std::endl; }
                    }
                }
                if((FilterCurveX.size()==0 || FilterCurveY.size()==0) && DCOBS->FilterCurveFilePath.size()>k) {
                    if(!DCOBS->FilterCurveFilePath[k].empty()) {
                        if(DCOBS->FilterCurveFilePath[k].find("none")==std::string::npos &&
                           DCOBS->FilterCurveFilePath[k].find("null")==std::string::npos &&
                           DCOBS->FilterCurveFilePath[k].find("None")==std::string::npos &&
                           DCOBS->FilterCurveFilePath[k].find("Null")==std::string::npos) {
                            TempFilterCurveFilePath = DCOBS->FilterCurveFilePath[k];
                            //if(debug) { std::cout << "michi2MatchObs: debugging: set TempFilterCurveFilePath = DCOBS->FilterCurveFilePath[" << k << "]" << std::endl; }
                            // read filter curve file, assuming two columns ascii file
                            if(!TempFilterCurveFilePath.empty()) {
                                if(debug>=1) { std::cout << "michi2MatchObs: debugging: applying filter curve from file \"" << TempFilterCurveFilePath << "\"" << std::endl; }
                                std::ifstream FilterCurveFileStream(TempFilterCurveFilePath.c_str());
                                if(FilterCurveFileStream.is_open()) {
                                    std::string FilterCurveFileLine;
                                    while(getline(FilterCurveFileStream,FilterCurveFileLine)) {
                                        if (FilterCurveFileLine[0]=='#') continue;
                                        std::stringstream FilterCurveFileLineStrStream(FilterCurveFileLine);
                                        std::string FilterCurveFileLineStr;
                                        std::vector<std::string> FilterCurveFileLineStrList;
                                        while (getline(FilterCurveFileLineStrStream,FilterCurveFileLineStr,' ')) {
                                            FilterCurveFileLineStrList.push_back(FilterCurveFileLineStr);
                                        } // https://stackoverflow.com/questions/9435385/split-a-string-using-c11
                                        if(FilterCurveFileLineStrList.size()>=2) {
                                            if(debug>=3) { std::cout << "michi2MatchObs: debugging: applying filter curve from file: FilterCurveX = " << FilterCurveFileLineStrList[0] << ", FilterCurveY = " << FilterCurveFileLineStrList[1] << std::endl;  }
                                            double temp_x1 = atof(FilterCurveFileLineStrList[0].c_str());
                                            double temp_y1 = atof(FilterCurveFileLineStrList[1].c_str());
                                            FilterCurveX.push_back(temp_x1);
                                            FilterCurveY.push_back(temp_y1);
                                            //if(debug>=3) { std::cout << "michi2MatchObs: debugging: applying filter curve from file: FilterCurveX = " << FilterCurveX[FilterCurveX.size()-1] << ", FilterCurveY = " << FilterCurveY[FilterCurveY.size()-1] << std::endl;  }
                                        }
                                        FilterCurveFileLineStrStream.clear();
                                    }
                                    FilterCurveFileStream.close();
                                } else {
                                    cout << std::endl;
                                    cout << "Error! Failed to open the filter curve file \"" << TempFilterCurveFilePath << "\"!" << std::endl;
                                    cout << std::endl;
                                }
                            }
                        }
                    }
                }
                if(FilterCurveX.size()>0 && FilterCurveY.size()>0 && FilterCurveX.size() == FilterCurveY.size()) {
                    // apply filter curve to the LIB data
                    // spline FilterCurveX FilterCurveY LIBX FilterFactorY
                    if(debug>=2) { std::cout << "michi2MatchObs: debugging: splining filter curve" << std::endl; }
                    for(int i1=0; i1<FilterCurveX.size(); i1++) {
                        FilterCurveX[i1] = FilterCurveX[i1] / OnePlusRedshift;
                        if(debug>=3) { std::cout << "michi2MatchObs: debugging: applying filter curve FilterCurveX = " << FilterCurveX[i1] << ", FilterCurveY = " << FilterCurveY[i1] << std::endl;  }
                    }
                    //
                    std::vector<double> FilterCurveY_Matched;
                    //for(int i1=0; i1<DCLIB->X.size(); i1++) {
                        //if(DCLIB->X[i1]>=FilterCurveX.front()/OnePlusRedshift && DCLIB->X[i1]<=FilterCurveX.back()/OnePlusRedshift) {
                        //    FilterCurveY_Matched[i1] = 1.0;
                        //} else {
                        //    FilterCurveY_Matched[i1] = 0.0;
                        //}
                    //}
                    //std::cout << "michi2MatchObs: debugging: FilterCurveY_Matched=0x" << std::hex << (size_t)&FilterCurveY_Matched << std::endl;
                    
                    FilterCurveY_Matched = spline(FilterCurveX, FilterCurveY, DCLIB->X);
                    //double FilterCurveY_Matched_Total = 0.0;
                    //for(int i1=0; i1<DCLIB->X.size(); i1++) {
                    //    FilterCurveY_Matched_Total += FilterCurveY_Matched[i1];
                    //    //std::cout << "michi2MatchObs: debugging: splining filter curve value " << FilterCurveY_Matched[i1] << std::endl;
                    //}
                    //std::cout << "michi2MatchObs: debugging: FilterCurveY_Matched=0x" << std::hex << (size_t)&FilterCurveY_Matched << std::endl;
                    //std::cout << "michi2MatchObs: debugging: DCLIB=0x" << std::hex << (size_t)&DCLIB << std::endl;
                    //std::cout << "michi2MatchObs: debugging: DCLIB->X=0x" << std::hex << (size_t)&DCLIB->X << std::endl;
                    //std::cout << "michi2MatchObs: debugging: DCLIB->Y=0x" << std::hex << (size_t)&DCLIB->Y << std::endl;
                    //std::cout << "michi2MatchObs: debugging: DCLIB->FilterCurveFilePath=0x" << std::hex << (size_t)&DCLIB->FilterCurveFilePath << std::endl;
                    //std::cout << "michi2MatchObs: debugging: DCLIB->TPAR=0x" << std::hex << (size_t)&DCLIB->TPAR << std::endl;
                    //std::cout << "michi2MatchObs: debugging: DCOBS->FilterCurveFilePath=0x" << std::hex << (size_t)&DCOBS->FilterCurveFilePath << std::endl;
                    //
                    if(debug>=2) { std::cout << "michi2MatchObs: debugging: splining filter curve done" << std::endl; }
                    if(DCLIB->X.front()<FilterCurveX.back() && DCLIB->X.back()>FilterCurveX.front()) {
                        if(debug>=2) { std::cout << "michi2MatchObs: debugging: calculating filter curve integral" << std::endl;  }
                        double FilterCurveIntegrated1 = 0.0; // \int R(\lambda) f(\lambda) d\lambda
                        double FilterCurveIntegrated2 = 0.0; // \int R(\lambda) d\lambda
                        for(int i1=0; i1<DCLIB->X.size(); i1++) {
                            if(DCLIB->X[i1]>=FilterCurveX.front() && DCLIB->X[i1]<=FilterCurveX.back()) {
                                FilterCurveIntegrated1 += DCLIB->Y[i1] * FilterCurveY_Matched[i1];
                                FilterCurveIntegrated2 += FilterCurveY_Matched[i1];
                                if(debug>=3) { std::cout << "michi2MatchObs: debugging: calculating filter curve integral: DCLIB->Y[i1] * FilterCurveY_Matched[i1] = " << DCLIB->Y[i1] << " * " << FilterCurveY_Matched[i1] << std::endl;  }
                            }
                        }
                        if(debug>=2) { std::cout << "michi2MatchObs: debugging: calculating filter curve integral done" << std::endl; }
                        if(debug>=1) { std::cout << "michi2MatchObs: debugging: the lib Y value before applying filter curve: " << MatchedLibY << std::endl; }
                        if(FilterCurveIntegrated2>0) {
                            MatchedLibY = FilterCurveIntegrated1 / FilterCurveIntegrated2; // now applied filter curve (aka transmission curve)
                        } else {
                            MatchedLibY = 0.0;
                        }
                        if(debug>=1) { std::cout << "michi2MatchObs: debugging: the lib Y value after applying filter curve: " << MatchedLibY << std::endl; }
                    }
                    //
                    // store into global variable FilterCurves for re-using
                    if(FilterCurves.size()<=k) {
                        for(int i1=0; i1<FilterCurveX.size(); i1++) {
                            FilterCurveX[i1] = FilterCurveX[i1] * OnePlusRedshift;
                        }
                        FilterCurves.resize(k+1);
                        FilterCurves[k] = new FilterCurveXY();
                        FilterCurves[k]->X.assign(FilterCurveX.begin(), FilterCurveX.end());
                        FilterCurves[k]->Y.assign(FilterCurveY.begin(), FilterCurveY.end());
                        FilterCurves[k]->Name = TempFilterCurveFilePath;
                        if(debug>=1) { std::cout << "michi2MatchObs: debugging: stored filter curve into FilterCurves[" << k << "]" << std::endl; }
                    }
                }
                //
                // save into MatchedDAT data structure
                DCLIB->Matched[j]=1;
                DCOBS->Matched[k]=1;
                MatchedDAT.j0.push_back(DCOBS->X[k]/OnePlusRedshift); // obs
                MatchedDAT.f0.push_back(DCOBS->Y[k]); // obs
                MatchedDAT.df.push_back(DCOBS->YErr[k]); // obs unc
                MatchedDAT.f1.push_back(MatchedLibY);
                if(debug>=1) { std::cout << "michi2MatchObs: debugging: got matched lib Y value " << MatchedLibY << " comparing to obs Y value " << DCOBS->Y[k] << " at obs X value " << DCOBS->X[k] << "/" << OnePlusRedshift << "=" << DCOBS->X[k]/OnePlusRedshift << std::endl; }
                // std::cout << "michi2MatchObs: debugging: got it! " << DCOBS->X[k]/OnePlusRedshift << " " << DCOBS->Y[k]/OnePlusRedshift << " " << DCLIB->Y[j];
                //if(debug>=2) {std::cout << "michi2MatchObs: debugging: FilterCurves=0x" << std::hex << (size_t)&FilterCurves << std::endl;}
                
                
                // <TODO><20171001> program stacked here when jumping out of this level
                // michi2_v04_test(80272,0x7000075bd000) malloc: *** error for object 0x7fec0fc04628: incorrect checksum for freed object - object was probably modified after being freed.
                // why?
                // SOLVED!
                // THE BUG IS IN spline.cpp !!!
                // because the code is pthread, even with Qt Creator debugging, I could not find out where is the bug. FINALLY BY EYE AND WITH A LOT OF EFFORT!
                
                
            } else {
                // no nearest LIB data found to OBS data? <TODO>
                if(debug>=1) { std::cout << "michi2MatchObs: debugging: got no matched lib data within lib X range " << LibRangeXMin << " " << LibRangeXMax << "? (This should not happen!)" << std::endl; }
            }
            //if(debug>=2) {std::cout << "michi2MatchObs: debugging: FilterCurves=0x" << std::hex << (size_t)&FilterCurves << std::endl;}
        } else {
            DCOBS->Matched[k]=1;
            MatchedDAT.j0.push_back(DCOBS->X[k]/OnePlusRedshift); // obs
            MatchedDAT.f0.push_back(DCOBS->Y[k]); // obs
            MatchedDAT.df.push_back(DCOBS->YErr[k]); // obs unc
            MatchedDAT.f1.push_back(0.0); // lib <TODO>
            // MatchedDAT.f1.push_back(std::numeric_limits<double>::quiet_NaN()); // lib <TODO> what if lib does not cover full obs xrange ?
            if(debug>=1) { std::cout << "michi2MatchObs: debugging: no matched lib found for the given obs X value " << DCOBS->X[k] << "/" << OnePlusRedshift << "=" << DCOBS->X[k]/OnePlusRedshift << std::endl; }
        }
    }
    
    //if(debug>=2) {std::cout << "michi2MatchObs: debugging: FilterCurves=0x" << std::hex << (size_t)&FilterCurves << std::endl;}
    
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










































/* 20160714 mnchi2 */
/* --- pthread ---- */
pthread_mutex_t mnchi2parallelMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t mnchi2parallelCondition = PTHREAD_COND_INITIALIZER;
long mnchi2parallelProgress = 0;
/* ---------------- */

void mnchi2(std::vector<std::string> InputObsList, std::vector<std::string> InputLibList, std::vector<std::string> OutputTableList, std::vector<std::string> InputFilterCurveList, int DebugLevel)
{
    //
    // check input obs data file list
    // and store basic information into 'SDOBSList',
    // also check 'InputFilterCurveList' for each obs data file.
    if(0==InputObsList.size()) {
        std::cout << "Error! mnchi2 InputObsList is empty!" << std::endl; return;
    }
    std::vector<michi2DataClass *> SDOBSList; SDOBSList.clear();
    long NumbObs = InputObsList.size();
    for(int i=0; i<InputObsList.size(); i++) {
        michi2DataClass *SDOBS = new michi2DataClass(InputObsList.at(i).c_str(), DebugLevel);
        if(InputFilterCurveList.size()>i) {
            SDOBS->readFilterCurveListFile(InputFilterCurveList.at(i).c_str());
        }
        SDOBSList.push_back(SDOBS);
    }
    //
    // check input lib data file list
    // and store basic information into 'SDLIBList'.
    if(0==InputLibList.size()) {
        std::cout << "Error! mnchi2 InputLibList is empty!" << std::endl; return;
    }
    std::vector<michi2DataClass *> SDLIBList; SDLIBList.clear();
    long NumbLib = InputLibList.size(), NumbLibPar = 0, NumbLibVar = 0, NumbLibMulti = 1;
    for(int i=0; i<InputLibList.size(); i++) {
        michi2DataClass *SDLIB = new michi2DataClass(InputLibList.at(i).c_str(), DebugLevel);
        SDLIBList.push_back(SDLIB);
        NumbLibPar += SDLIB->TPAR.size(); // SDLIB->TPAR is the list of parameter title in a LIB file, so its size() is the number of parameters to be fit in a LIB file.
        NumbLibVar += SDLIB->YNum; // SDLIB->YNum is the number of all independent models in one LIB file, while SDLIB->XNum is the number of wavelength or any X axis in one model.
        NumbLibMulti = NumbLibMulti * SDLIB->YNum; // This gives the number of all model combinations among all LIB files.
    }
    // check output table data file list
    // set default output table data file name for each input obs data file
    // and prepare output file pointer
    if(0==OutputTableList.size()) {
        std::cout << "Error! mnchi2 OutputTableList is empty!" << std::endl; return;
    }
    //std::vector<std::ofstream*> SDOUTList; SDOUTList.clear();
    //
    //
    //
    //
    //
    //
    //
    //
    //
    // 20180224
    // Now we have read the input data file and all lib files.
    // We have a 'NumbLibPar'-dimension parameter space,
    // and we will follow Metropolis Hasting Algorithm to find the best-fits.
    // We will:
    //   1. Choose a starting point in the 'NumbLibPar'-dimension parameter space.
    //   2. Assume a posterior PDF, which is 'exp(-chisq)' here.
    //   3. Build a Markov chain Transition Matrix Trans_matrix, which has a dimension of 'NumbLibPar'x'NumbLibPar', and is all 0.5 here, which means jumping from one state to another has an equal chance.
    //   4. Build sampling functions for 'NumbLibPar' parameters, here we choose uniform sampling.
    //   5. Sample one set of 'NumbLibPar' parameters, denoted as param(t). 
    //   6. Check if the Markov chain converges:
    //        In default, the converging needs: param(t+1) * Trans_matrix(t=>t+1) = param(t) * Trans_matrix(t+1=>t), but this does not work high dimension. Metropolis algorithm is an improvement. It needs: chisq(t+1) * param(t+1) * Trans_matrix(t=>t+1) * acceptance(t+1) = param(t) * Trans_matrix(t+1=>t) * acceptance(t), where acceptance(t+1) = param(t) * Trans_matrix(t+1=>t) and acceptance(t) = param(t+1) * Trans_matrix(t=>t+1). However, it converges too slowly. Metropolis-Hasting algorithm improved the acceptance rate: acceptance(t+1) = param(t) * Trans_matrix(t+1=>t) / min([param(t+1) * Trans_matrix(t=>t+1), param(t) * Trans_matrix(t+1=>t)]), and acceptance(t) = param(t+1) * Trans_matrix(t=>t+1) / min([param(t+1) * Trans_matrix(t=>t+1), param(t) * Trans_matrix(t+1=>t)]). If acceptance(t+1) > random([0,1]), then go to state param(t+1), otherwise keep state param(t). Repeat 5 and 6.
    //    7. Gibbs Sampling algorithm: m 维的一个样本跳转到另一个样本的过程，可以拆解为 m 个子过程，每一个子过程对应一个维度。这时概率转移矩阵是 m 个子概率转移矩阵之积，即 (p = \prod_{i=k}^{m} p_k )
    //
    // (forget about the above steps)
    // (I do not think a Markov chain with a uniform sampling for all parameters will save time)
    // (I should just try my own half-interval search)
    // We will:
    //    1. For each param, select first, last and middle values, make '3*NumLib' combinations.
    //
    //
    //
    //
    // loop SDOBS
    for(int i=0; i<InputObsList.size(); i++) {
        //
        // prepare output file names
        if(OutputTableList.size()<i) {
            OutputTableList.push_back(InputObsList.at(i).insert(0,"mnchi2-").append(".out"));
        }
        //
        // write info file
        // output the input obs and lib files
        std::string OutputInfoFile = OutputTableList.at(i); OutputInfoFile.append(".info");
        std::ofstream OutputInfoFileStream(OutputInfoFile.c_str());
        OutputInfoFileStream << "OBS = " << InputObsList.at(i) << std::endl;
        OutputInfoFileStream << "NLIB = " << InputLibList.size() << std::endl;
        for(long iLib=0; iLib<InputLibList.size(); iLib++) {
            OutputInfoFileStream << "LIB" << iLib+1 << " = " << InputLibList.at(iLib) << std::endl;
        }
        OutputInfoFileStream << "OUT = " << OutputTableList.at(i) << std::endl;
        if(strlen(InfoRedshift)>0) {
            OutputInfoFileStream << "REDSHIFT = " << InfoRedshift << std::endl;
        }
        OutputInfoFileStream.close();
        //
        // write chisq table file
        // output i0, chi2, iN, aN column headers
        std::ofstream SDOUT(OutputTableList.at(i).c_str());
        SDOUT << "#" << std::setw(7) << "i0" << std::setw(15) << "chi2"; // write the output file header line
        std::ostringstream tmpstr;
        for(long iLib=0; iLib<InputLibList.size(); iLib++) {
            tmpstr.str(""); tmpstr << "i" << iLib+1; SDOUT << std::setw(10) << tmpstr.str();
            tmpstr.str(""); tmpstr << "a" << iLib+1; SDOUT << std::setw(15) << tmpstr.str();
        }
        //
        // loop SDLIB, output paramN column headers
        for(long iLib=0; iLib<SDLIBList.size(); iLib++) {
            michi2DataClass *SDLIB = SDLIBList[iLib]; // can not use 'SDLIBList.at(i)' <BUG><fixed><20160727><dzliu>
            for(long iLibPar=0; iLibPar<SDLIB->TPAR.size(); iLibPar++) {
                SDOUT << std::setw(15) << SDLIB->TPAR[iLibPar]; // write each title of parameter to be fit in each LIB file
            }
        }
        SDOUT << std::endl << "#" << std::endl;
        SDOUT.close();
        //
        //
        //
        //
        //
        //
        // prepare looping
        struct mnchi2parallelParams *ppParams = NULL; // this pointer points to a previously fitted 'pParams' struct.
        //
        // start looping
        while (true) {
            //
            // create subprocess structure
            struct mnchi2parallelParams *pParams = new struct mnchi2parallelParams;
            //
            // all subprocesses share the same OBS data structure
            pParams->SDOBSList = SDOBSList;
            //
            // all subprocesses share the same LIB data structure
            pParams->SDLIBList = SDLIBList;
            //
            // prepare model combinations and fit to the data
            // we first prepare 3 lists of lists,
            // we use an example to explain this
            // if we have 3 LIB files,
            // in the 1st LIB file, we have 2 parameters, 1st par has 10 values and 2nd par has 3 values, and we want to loop the [first,middle,last] values of both 1st and 2nd parameters.
            // in the 2nd LIB file, we have 4 parameters, which have 10,2,2,3 values respectively, and we want to loop the [first(,middle),last] values of each parameter.
            // in the 3rd LIB file, we have 6 parameters, which have 10,2,2,3,1,1 values respectively, and we want to loop the [first(,middle)(,last)] values of each parameter.
            // then, iLibParListListList is a list with NLIB dimension
            //       iLibParListListList = [ iLibParListList1, iLibParListList2, iLibParListList3 ]
            //       iLibParListList1 = [ iLibParList1, iLibParList2 ] // Lib1
            //                            iLibParList1 = [ i1_par1, i2_par1, i3_par1 ]
            //                            iLibParList2 = [ i1_par2, i2_par2, i3_par2 ]
            //       iLibParListList2 = [ iLibParList1, iLibParList2, iLibParList3, iLibParList4 ] // Lib2
            //                            iLibParList1 = [ i1_par1, i2_par1, i3_par1 ]
            //                            iLibParList2 = [ i1_par2, i2_par2 ]
            //                            iLibParList3 = [ i1_par3, i2_par3 ]
            //                            iLibParList4 = [ i1_par4, i2_par4, i3_par4 ]
            //       iLibParListList3 = [ iLibParList1, iLibParList2, iLibParList3, iLibParList4, iLibParList5, iLibParList6 ] // Lib3
            //                            iLibParList1 = [ i1_par1, i2_par1, i3_par1 ]
            //                            iLibParList2 = [ i1_par2, i2_par2 ]
            //                            iLibParList3 = [ i1_par3, i2_par3 ]
            //                            iLibParList4 = [ i1_par4, i2_par4, i3_par4 ]
            //                            iLibParList5 = [ i1_par5 ]
            //                            iLibParList6 = [ i1_par6 ]
            //
            pParams->iLibParListListList.clear();
            pParams->chisqListListList.clear();
            for(long iLib=0; iLib<SDLIBList.size(); iLib++) {
                // loop each lib, determine the range of each parameter to fit
                std::vector<std::vector<long> > iLibParListList1;
                std::vector<std::vector<double> > chisqListList1;
                //
                michi2DataClass *SDLIB = SDLIBList[iLib]; // can not use 'SDLIBList.at(i)' <BUG><fixed><20160727><dzliu>
                //
                for(long iLibPar=0; iLibPar<SDLIB->TPAR.size(); iLibPar++) {
                    // loop each parameter in a lib, determine the range of this parameter to fit the data
                    std::vector<long> iLibParList1;
                    std::vector<double> chisqList1;
                    //
                    if(ppParams) {
                        // if previously fitted, then we first find the least-chisq parameter range, then select first, middle and last values in that range for the next fitting
                        // we allow multiple local minima
                        for (long iLocalMinima=0; iLocalMinima<chisqListListList[iLib][iLibPar].size(); iLocalMinima+=3) {
                            if((ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+0] > ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+1]) &&
                               (ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+2] > ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+1])) {
                                // valley, adjust the middle point randomly to +1/4 or -1/4
                                long iLocalLeft = ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+1];
                                long iLocalRight = ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+2];
                                if((iLocalRight-iLocalLeft)/4>0) {
                                    long iLocalMiddle = iLocalLeft + ((rand()%2+1)*2-3)*(iLocalRight-iLocalLeft)/4; // (rand()%2+1) => 1 to 2
                                }
                            } else if((ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+0] <= ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+1]) &&
                                      (ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+2] <= ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+1])) {
                                // hill, split into two local minima
                                //
                            } else if((ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+0] > ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+1]) &&
                                      (ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+2] <= ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+1])) {
                                // slope, right is lower, so we adjust left edge to previous middle point and set a new middle point
                                long iLocalLeft = ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+1];
                                long iLocalRight = ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+2];
                                if(iLocalLeft < iLocalRight-1) {
                                    iLibParListList1.push_back(iLocalLeft);
                                    iLibParListList1.push_back((iLocalRight+iLocalLeft)/2);
                                    iLibParListList1.push_back(iLocalRight);
                                    chisqList1.push_back(ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+1]);
                                    chisqList1.push_back(std::nan(""));
                                    chisqList1.push_back(ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+2]);
                                } else {
                                    // if left >= right-1, it means no middle point can be set. freeze this parameter.
                                    iLibParListList1.push_back(iLocalLeft);
                                    iLibParListList1.push_back(iLocalLeft);
                                    iLibParListList1.push_back(iLocalRight);
                                    chisqList1.push_back(ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+1]);
                                    chisqList1.push_back(ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+1]);
                                    chisqList1.push_back(ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+2]);
                                }
                            } else if((ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+0] <= ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+1]) &&
                                      (ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+2] > ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+1])) {
                                // slope, left is lower, so we adjust right edge to previous middle point and set a new middle point
                                long iLocalLeft = ppParams->iLibParListListList[iLib][iLibPar][iLocalMinima+0];
                                long iLocalRight = ppParams->iLibParListListList[iLib][iLibPar][iLocalMinima+1];
                                if(iLocalLeft < iLocalRight-1) {
                                    iLibParListList1.push_back(iLocalLeft);
                                    iLibParListList1.push_back((iLocalRight+iLocalLeft)/2);
                                    iLibParListList1.push_back(iLocalRight);
                                    chisqList1.push_back(ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+0]);
                                    chisqList1.push_back(std::nan(""));
                                    chisqList1.push_back(ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+1]);
                                } else {
                                    // if left >= right-1, it means no middle point can be set. freeze this parameter.
                                    iLibParListList1.push_back(iLocalLeft);
                                    iLibParListList1.push_back(iLocalRight);
                                    iLibParListList1.push_back(iLocalRight);
                                    chisqList1.push_back(ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+0]);
                                    chisqList1.push_back(ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+1]);
                                    chisqList1.push_back(ppParams->chisqListListList[iLib][iLibPar][iLocalMinima+1]);
                                }
                            }
                        }
                    } else {
                        // we fit 3 values for a parameter: first, middle and last.
                        long iLocalLeft = 0;
                        long iLocalRight = SDLIB->NPAR[iLibPar]-1;
                        iLibParListList1.push_back(iLocalLeft);
                        iLibParListList1.push_back((iLocalRight+iLocalLeft)/2);
                        iLibParListList1.push_back(iLocalRight);
                        chisqList1.push_back(std::nan(""));
                        chisqList1.push_back(std::nan(""));
                        chisqList1.push_back(std::nan(""));
                    }
                }
                //
                pParams->iLibParListListList.push_back(iLibParListList1);
            }
        }
        //
        //
        //
        // loop SDLIB, fit model to the data
        
    }
    //
    //
    //
    //
    //
    // <TODO> old code before 20180224
    // prepare to run parallel processes <New><20141211><20141212><20160714><20160718>
    mnchi2parallelProgress = 0;
    std::vector<struct mnchi2parallelParams *> mnchi2parallelParams;
    std::cout << "processing: " << std::endl;
    int iPara = NumbParallel; // the number of parallel processes
    long iLoop = 0;
    long iStep = long((float(NumbObs*NumbLibMulti)+float(iPara)-1)/float(iPara)); // iPara is the paralle process number, allowed to be input with arg '-parallel'. //<NOTE> +float(iPara)-1 to make sure we cover all the ranges.
    while(iLoop >= 0 && iLoop < NumbObs*NumbLibMulti) {
        // determine the loop range of each process,
        // and if current process is the last process, set the step to be exactly the number of loops left.
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
            michi2DataClass *SDLIB = new michi2DataClass(InputLibList.at(i).c_str(), DebugLevel);
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
            //michi2DataClass *SDLIB = SDLIBList[iLib];
            //iTemp = iTemp / SDLIB->YNum;
            pParams->idLIBList.push_back(0); // controls the parallel processes
            pParams->ibLIBList.push_back(0); // controls the parallel processes
            pParams->ieLIBList.push_back(0); // controls the parallel processes
            pParams->irLIBList.push_back(true); // controls the parallel processes
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
        pParams->idOBS = pParams->ibOBS; // index of OBS list
        pParams->irOBS = true; // 是否进位
        for(long iLib = 0; iLib < pParams->nLib ; iLib++) {
            pParams->idLIBList[iLib] = pParams->ibLIBList[iLib]; // index of LIB list
            pParams->irLIBList[iLib] = true; // 是否进位。一个LIB文件包含了多个Templates，我们遍历每一个Template与其它LIB中的Templates的组合，所以该变量判断是否该LIB中当前Template已经与所有其它LIB中的Templates都组合过了，若是，则进位，即切换至该LIB种的下一个Template，与其它LIB中的Templates进行组合。
        }
        // print info and prepare next loop
        if(DebugLevel>=1) {
            std::cout << "looping " << iLoop+1 << "-" << iLoop+iStep-1+1 << "/" << NumbLibMulti;
            std::cout << " OBS " << pParams->ibOBS+1 << "-" << pParams->ieOBS+1;
            for(long iLib = 0; iLib < SDLIBList.size(); iLib++) {
                std::cout << " LIB" << iLib+1 << " " << pParams->ibLIBList[iLib]+1 << "-" << pParams->ieLIBList[iLib]+1 << "/" << pParams->SDLIBList[iLib]->YNum;
            }
        }
        std::cout << std::endl;
        iLoop = iLoop + iStep;
        // setup OUT list
        //pParams->SDOUTList = SDOUTList;
        pParams->InputLibList = InputLibList;
        pParams->OutputTableList = OutputTableList;
        // set debug level
        pParams->DebugLevel = DebugLevel;
        //
        // <20150309> make pthread
        pthread_t thread1; pthread_create(&thread1, NULL, &mnchi2parallel, pParams);
        //
        // sleep for a few seconds to allow the subprocess to be created.
        //if(i1>=500) {sleep(60);} else {sleep(2);}
        //sleep(5);// before 20180114
        sleep(3);
    }
    // show initial info for all threads
    // and print progress
    int ndigits = 0;
    //std::cout << std::setw(8) << std::right << mnchi2parallelProgress << "||";
    for(long ip=0; ip<mnchi2parallelParams.size(); ip++) {
        std::cout << "[" << mnchi2parallelProgress << "/" << NumbObs*NumbLibMulti << "]"; //<20180130>
        // print progress first line
        ndigits = (int)log10((double)(mnchi2parallelParams[ip]->iEnd+1)) + 1;
        //std::cout << std::setw(15) << std::right << mnchi2parallelParams[ip]->iBegin << "|" << mnchi2parallelParams[ip]->nObs*mnchi2parallelParams[ip]->nRow << "|"; //<20180116>
        std::cout << std::setw(4+ndigits) << std::right << mnchi2parallelParams[ip]->iBegin << "|" << std::setw(ndigits) << std::left << mnchi2parallelParams[ip]->iEnd+1 << "|"; //<20180116>
        for(long iLib = 0; iLib < mnchi2parallelParams[ip]->nLib; iLib++) {
            ndigits = (int)log10((double)(mnchi2parallelParams[ip]->SDLIBList[iLib]->YNum)) + 1;
            std::cout << std::setw(ndigits) << std::right << 0 << "|" << std::setw(ndigits) << std::left << mnchi2parallelParams[ip]->SDLIBList[iLib]->YNum << "|";
        }
    }
    std::cout << std::endl;
    // wait for all threads
    // and print progress
    while(mnchi2parallelProgress <= NumbObs*NumbLibMulti) {
        std::cout << "[" << mnchi2parallelProgress << "/" << NumbObs*NumbLibMulti << "]"; //<20180116>
        // print progress
        for(long ip=0; ip<mnchi2parallelParams.size(); ip++) {
            ndigits = (int)log10((double)(mnchi2parallelParams[ip]->iEnd+1)) + 1;
            //std::cout << std::setw(15) << std::right << mnchi2parallelParams[ip]->i << "|" << mnchi2parallelParams[ip]->nObs*mnchi2parallelParams[ip]->nRow << "|"; //<20180116>
            std::cout << std::setw(4+ndigits) << std::right << mnchi2parallelParams[ip]->i << "|" << std::setw(ndigits) << std::left << mnchi2parallelParams[ip]->iEnd+1 << "|"; //<20180116>
            for(long iLib = 0; iLib < mnchi2parallelParams[ip]->nLib; iLib++) {
                ndigits = (int)log10((double)(mnchi2parallelParams[ip]->SDLIBList[iLib]->YNum)) + 1;
                std::cout << std::setw(ndigits) << std::right << mnchi2parallelParams[ip]->idLIBList[iLib] << "|" << std::setw(ndigits) << std::left << mnchi2parallelParams[ip]->SDLIBList[iLib]->YNum << "|";
            }
        }
        std::cout<< std::endl;
        if(mnchi2parallelProgress >= NumbObs*NumbLibMulti) {
            break;
        } else {
            sleep(4 + rand() % 4); // sleep for 4 to 8 seconds
            continue;
        }
    }
    // all threads finished
    // print progress of 100%
    ndigits = (int)log10((double)(mnchi2parallelParams[0]->iEnd+1)) + 1;
    std::cout << std::setw(4+ndigits) << std::right << "100%" << "||" << std::endl; //<20180116>
    // clean
    for(long ip=0; ip<mnchi2parallelParams.size(); ip++) {
        delete mnchi2parallelParams[ip]; mnchi2parallelParams[ip]=NULL;
    }
}

void *mnchi2parallel(void *params)
{
    struct mnchi2parallelParams *pParams = (struct mnchi2parallelParams *)params;
    int debug = pParams->DebugLevel;
    if(debug>=1) {std::cout << "mnchi2parallel: Making parallel process " << pParams << std::endl;}
    std::vector<std::string> pStrings; pStrings.resize(pParams->nObs);
    //
    //std::vector<std::stringstream*> pStreams;
    //for(long iObs = 0; iObs < pParams->nObs; iObs++) {
    //    std::stringstream pStream;
    //    pStreams.push_back(&pStream);
    //}
    //std::vector<std::stringstream> pStreams; pStreams.resize(pParams->nObs);
    //
    // check loop begin status
    if(pParams->i != pParams->iBegin) {
        std::cout << "mnchi2parallel: Error! pParams->i != pParams->iBegin! This should not happen!" << std::endl;
    }
    //
    //std::cout << "DEBUG: pParams=0x" << std::hex << (size_t)pParams << std::endl;
    //std::cout << "DEBUG: &pParams->SDOBSList=0x" << std::hex << (size_t)&pParams->SDOBSList << std::endl;
    //std::cout << "DEBUG: &pParams->SDLIBList=0x" << std::hex << (size_t)&pParams->SDLIBList << std::endl;
    //std::cout << "DEBUG: &pParams->InputLibList=0x" << std::hex << (size_t)&pParams->InputLibList << std::endl;
    //std::cout << "DEBUG: &pParams->OutputTableList=0x" << std::hex << (size_t)&pParams->OutputTableList << std::endl;
    ////
    //// check loop rounding status
    //pParams->irOBS = true;
    //for(long iLib = 0; iLib < pParams->nLib; iLib++) {
    //    pParams->irLIBList[iLib] = true;
    //}
    //
    // prepare the list of michi2DataClass
    michi2DataClass *SDOBS = NULL;
    std::vector<michi2DataClass *> SDLIBS; SDLIBS.resize(pParams->nLib);
    //
    // loop
    while (pParams->i <= pParams->iEnd) {
        if(debug>=1) {
            //std::cout << "mnchi2parallel: Looping " << pParams->i << "/" << pParams->iEnd << " OBS" << pParams->idOBS+1; //<20180116>
            std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": Looping " << pParams->i << "/" << pParams->iEnd+1 << " OBS" << pParams->idOBS+1; //<20180116>
            for(long iLib = 0; iLib < pParams->nLib; iLib++) {
                std::cout << " LIB" << iLib+1 << " data block " << pParams->idLIBList[iLib]+1 << "/" << pParams->SDLIBList[iLib]->YNum;
            } std::cout << std::endl;
        }
        //
        // check constraints <Added><20160719><dzliu>
        bool ConstraintOK = true;
        if(Constraints.size()>0) {
            for(int iCon = 0; iCon < Constraints.size(); iCon++) {
                michi2Constraint *TempConstraint = Constraints[iCon];
                ConstraintOK = TempConstraint->check(pParams,debug-1);
                if(!ConstraintOK) {break;}
            }
        }
        //
        // if passed the constraint check
        // <20180110> moved the reading of OBS data block and LIB data block after the constraint, which can speed up a lot!
        if(ConstraintOK) {
            if(debug>=2) {std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": Passed the INDEX constraint check!" << std::endl;}
            //
            // get OBS data block
            if(pParams->irOBS) {
                if(debug>=3) {std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": Reading OBS " << pParams->idOBS+1 << " data" << std::endl;}
                SDOBS = pParams->SDOBSList.at(pParams->idOBS);
                pParams->irOBS = false;
            }
            //
            // get LIB data block
            for(long iLib = 0; iLib < pParams->nLib; iLib++) {
                if(pParams->irLIBList[iLib]) {
                    //std::cout << "DEBUG: SDLIBS[" << iLib << "]=0x" << std::hex << (size_t)SDLIBS[iLib] << ", " << "pParams->SDLIBList[" << iLib << "]=0x" << std::hex << (size_t)pParams->SDLIBList[iLib] << std::endl;
                    SDLIBS[iLib] = pParams->SDLIBList[iLib];
                    //std::cout << "DEBUG: SDLIBS[" << iLib << "]=0x" << std::hex << (size_t)SDLIBS[iLib] << std::endl;
                    SDLIBS[iLib]->getDataBlock(pParams->idLIBList[iLib] * SDLIBS[iLib]->XNum + 1, SDLIBS[iLib]->XNum);
                    if(debug>=3) {std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": Reading LIB" << iLib+1 << " data block " << pParams->idLIBList[iLib]+1 << "/" << pParams->SDLIBList[iLib]->YNum << std::endl;}
                    pParams->irLIBList[iLib] = false;
                }
            }
            //
            // check constraints again, now with SDLIBS and SDOBS
            if(Constraints.size()>0) {
                for(int iCon = 0; iCon < Constraints.size(); iCon++) {
                    michi2Constraint *TempConstraint = Constraints[iCon];
                    ConstraintOK = TempConstraint->check(SDLIBS,debug-1);
                    if(!ConstraintOK) {break;}
                }
            }
            //
            // if passed the second constraint check
            if(ConstraintOK) {
                if(debug>=2) {std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": Passed the LIB PAR constraint check!" << std::endl;}
                //
                // prepare MinPack input data structure
                std::vector<std::vector<double> > fLIB; fLIB.clear();
                std::vector<double> fOBS;
                std::vector<double> eOBS;
                std::vector<double> aFIT; aFIT.resize(pParams->nLib);
                //
                // match LIB to OBS data block
                // match LIB to OBS data block <updated><20170930> apply filter curves to fLIB at each wOBS, if the running mode is SED.
                if(debug>=2) {std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": Matching LIBs to OBS." << std::endl;}
                for(long iLib = 0; iLib < pParams->nLib; iLib++) {
                    if(debug>=3) {std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": Matching LIB" << iLib+1 << " to OBS" << pParams->idOBS+1 << std::endl;}
                    MatchedObsLibStruct SDDAT = michi2MatchObs(SDOBS, SDLIBS[iLib], debug-2); // set debug>=3 to debug this.
                    if(debug>=3) {std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": Matched LIB" << iLib+1 << " to OBS" << pParams->idOBS+1 << std::endl;}
                    // check data block
                    if(SDDAT.f0.size()>0) {
                        // min chi2 by n components
                        fLIB.push_back(SDDAT.f1);
                        if(iLib == (pParams->nLib-1)) {
                            fOBS = SDDAT.f0; eOBS = SDDAT.df;
                        }
                    } else {
                        std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": Error! Failed to read data block from LIB" << iLib+1 << " " << pParams->InputLibList.at(iLib) << std::endl;
                        return(NULL);
                    }
                }
                //
                // do MinPack
                if(fLIB.size()>0) {
                    //
                    michi2MinPack *MPACK = new michi2MinPack(fLIB,fOBS,eOBS);
                    if(debug>=2) {std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": Initializing MPACK." << std::endl;}
                    //
                    //
                    // check constraints on LIB coefficients <Added><20160719><dzliu>
                    if(Constraints.size()>0) {
                        for(int iCon = 0; iCon < Constraints.size(); iCon++) {
                            michi2Constraint *TempConstraint = Constraints[iCon];
                            //
                            // set constraint for michi2MinPack.
                            // 20180114: using new method for evaluating the user input equation
                            //
                            // Note that only constraints to the LIB NORM (normalization of some library) will be applied here.
                            // Other constraints to the LIB INDEX etc. are applied above, see the code
                            //     ConstraintOK = TempConstraint->check(pParams,debug-1);
                            //
                            // Case 1:
                            //     LIB5.NORM = LIR(8,1000)
                            // Case 2:
                            //     LIB5.NORM = "((10^LIB3_PAR3)+(10^LIB4_PAR3))/3750/10^2.4"
                            //
                            if(1==1 &&
                               TempConstraint->fromLIB<=0 &&
                               TempConstraint->toLIB>0 &&
                               TempConstraint->toLIB<=pParams->nLib &&
                               !TempConstraint->fromEquation.empty() &&
                               !TempConstraint->fromVariable.empty() &&
                               !TempConstraint->toEquation.empty() &&
                               !TempConstraint->toVariable.empty() ) {
                                //
                                // check if the destination is the normalization (NORM) of some LIB or not.
                                std::vector<double> TempVariableValues;
                                if(TempConstraint->toVariable[0] == "NORM" || TempConstraint->toVariable[0] == "NORMALIZATION") {
                                    //
                                    // print debug info
                                    if(debug>=2) {std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": Setting constraint on LIB" << TempConstraint->toLIB << " normalization!" << std::endl;}
                                    //
                                    // evaluate some variables, e.g., LIB*_PAR*,
                                    for(int iVar=0; iVar<TempConstraint->fromVariable.size(); iVar++) {
                                        //
                                        // print debug info
                                        if(debug>=3) {std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": Parsing variable " << std::flush;
                                            std::cout << TempConstraint->fromVariable[iVar] << std::endl;}
                                        //
                                        // determine the value of each variable in the equation must have a value
                                        // each variable in the equation must have a value
                                        std::string sVar = TempConstraint->fromVariable[iVar];
                                        double dVar = std::nan("");
                                        //
                                        // if the variable is LIR*
                                        if(sVar.find("LIR")==0) {
                                            // then compute LIR
                                            double LibIntegrated[pParams->nLib];
                                            double LibIntegratedTotal = 0.0;
                                            std::vector<double> LibIntegrateRange; LibIntegrateRange.push_back(8.0); LibIntegrateRange.push_back(1000.0);
                                            for(long iLib=0; iLib < pParams->nLib; iLib++) {
                                                LibIntegrated[iLib] = integrate_LIR(SDLIBS[iLib]->X, SDLIBS[iLib]->Y, LibIntegrateRange);
                                                LibIntegratedTotal += LibIntegrated[iLib];
                                                // print debug info
                                                if(debug>=3) {std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": debugging: calculating integral: LIB" << iLib+1 << " LIR(" <<LibIntegrateRange[0] << "," << LibIntegrateRange[1] << ") = " << LibIntegrated[iLib] << std::endl;}
                                            }
                                            // now we got the LIR as the variable value
                                            dVar = LibIntegratedTotal;
                                            // print debug info
                                            if(debug>=3) {std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": debugging: setting constraint variable " << sVar << " = " << dVar << std::endl;}
                                        }
                                        //
                                        // else if the variable is LIB*_PAR*
                                        else if(sVar.find("LIB")==0 && sVar.find("PAR")>0) {
                                            if(debug>=3) {std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": debugging: setting constraint variable " << sVar << std::endl;}
                                            std::regex t_regex_1("LIB([0-9]+)_PAR([0-9]+).*");
                                            std::smatch t_match_1;
                                            if(std::regex_search(sVar,t_match_1,t_regex_1) && t_match_1.size()>2) {
                                                int t_lib_number = std::atoi(t_match_1.str(1).c_str());
                                                int t_par_number = std::atoi(t_match_1.str(2).c_str());
                                                // now we got the LIB*_PAR* as the variable value
                                                if(t_par_number <= SDLIBS[t_lib_number-1]->FPAR.size()) {
                                                    dVar = SDLIBS[t_lib_number-1]->FPAR[t_par_number-1][0];
                                                } else {
                                                    std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": Error! The parameter number " << t_par_number << " exceeds the max parameter number " << SDLIBS[t_lib_number-1]->FPAR.size() << " in LIB" << t_lib_number << "!" << std::endl;
                                                    exit (EXIT_FAILURE);
                                                }
                                                // print debug info
                                                if(debug>=3) {std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": debugging: setting constraint variable " << sVar << " = " << dVar << std::endl;}
                                            }
                                        }
                                        TempVariableValues.push_back(dVar);
                                    }
                                    
                                    //
                                    // Now we set MPACK->constrain().
                                    //
                                    if(debug>=3) {std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": Setting MPACK->constrain()" << std::endl;}
                                    MPACK->constrain(TempConstraint->toLIB, TempConstraint->fromEquation, TempConstraint->fromVariable, TempVariableValues);
                                }
                            }
                            //
                            // set constraints:
                            // using the old method
                            else {
                                if(TempConstraint->fromLIB==-1 &&
                                   TempConstraint->fromPAR==-3 &&
                                   TempConstraint->toLIB>0 &&
                                   TempConstraint->toLIB<=pParams->nLib &&
                                   TempConstraint->toPAR==-1 )
                                {//
                                    // print debug info
                                    if(debug>=2) {std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": Setting constraint on LIB" << TempConstraint->toLIB << " normalization ... (old method)" << std::endl;}
                                    //
                                    // set constraint: LIB5 NORM EQ SED LIR(8,1000), i.e., fixing the normalization of LIB5 to the vLv(8,1000), i.e., qIR=250
                                    // -- NOTE vLv is not INT! vLv is the integration of \int x*SED dx, but INT is just the integration of \int SED dx.
                                    //
                                    //<DEBUG><20171001> std::cout << "DEBUG: setting constraint: LIB" << TempConstraint->toLIB << "NORM EQ SED vLv(8,1000)*1.061619121e-06, i.e. q_IR = 2.4, 10^q_IR = S_IR(8-1000)/(3.75e12Wm-2)/(S_(1.4GHz)/1Wm-2Hz-1)." << std::endl;
                                    //
                                    // in this case, LIB5 is not fitted but its normalization is fully deteremined by other LIBs (or by the full SED vLv)
                                    double LibIntegrated[pParams->nLib];
                                    double LibIntegratedTotal = 0.0;
                                    std::vector<double> LibIntegrateRange; LibIntegrateRange.push_back(TempConstraint->fromLowerX); LibIntegrateRange.push_back(TempConstraint->fromUpperX);
                                    // now we calculate the vLv(8,1000) of each LIB
                                    for(long iLib=0; iLib < pParams->nLib; iLib++) {
                                        LibIntegrated[iLib] = integrate_LIR(SDLIBS[iLib]->X, SDLIBS[iLib]->Y, LibIntegrateRange);
                                        LibIntegratedTotal += LibIntegrated[iLib];
                                        //<DEBUG><20171001>
                                        if(debug>=4) {
                                            std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": debugging: calculating integral: LIB" << iLib+1 << " INT(" <<TempConstraint->fromLowerX << "," << TempConstraint->fromUpperX << ") = " << LibIntegrated[iLib] << std::endl;
                                        }
                                    }
                                    //
                                    // then we calculate the coefficients of each LIB's vLv(8,1000) comparing to LIB5's vLv(8,1000)
                                    // and we set LibConstrainNumbers and LibConstrainFactors, that is,
                                    // the normalization of the destination LIB, TempConstraint->toLIB, depends on
                                    // the list of LIBs in LibConstrainNumbers, and the factors are in LibConstrainFactors.
                                    std::vector<int> LibConstrainNumbers;
                                    std::vector<double> LibConstrainFactors;
                                    for(long iLib=0; iLib < pParams->nLib; iLib++) {
                                        if(iLib!=TempConstraint->toLIB-1) {
                                            double TempCoefficient = 1.0;
                                            if(!std::isnan(TempConstraint->toMultiplication)) { TempCoefficient = TempCoefficient / TempConstraint->toMultiplication; }
                                            if(!std::isnan(TempConstraint->fromMultiplication)) { TempCoefficient = TempCoefficient * TempConstraint->fromMultiplication; }
                                            LibConstrainNumbers.push_back(iLib+1);
                                            //<before><20180110>//LibConstrainFactors.push_back( LibIntegrated[iLib] / (TempConstraint->toMultiplication/TempConstraint->fromMultiplication - LibIntegrated[TempConstraint->toLIB-1]) );
                                            LibConstrainFactors.push_back( LibIntegrated[iLib] / (1.0 / TempCoefficient - LibIntegrated[TempConstraint->toLIB-1]) ); //<20180110>//
                                            // -- NOTE
                                            // e.g., if we want
                                            //      S_(1.4GHz) = 1.061619121e-6 * S_(IR,8-1000), assuming the radio SED templates are normalized to S_(1.4GHz) = 1 mJy
                                            // then we need
                                            //      TempConstraint->toMultiplication is 1, TempConstraint->fromMultiplication = 1.061619121e-6.
                                            // and we have
                                            //      a1 * LibInt1 + a2 * LibInt2 + a3 * LibInt3 = S_(IR,8-1000)
                                            // so we get
                                            //      a1 * LibInt1 + a2 * LibInt2 + a3 * LibInt3 = S_(1.4GHz) / 1.061619121e-6
                                            //      a1 * LibInt1 + a2 * LibInt2 + a3 * LibInt3 = a3 / 1.061619121e-6
                                            //      a3 * (1/1.061619121e-6 - LibInt3) = a1 * LibInt1 + a2 * LibInt2
                                            //      a3  = a1 * LibInt1/((1/1.061619121e-6)-LibInt3) + a2 * LibInt2/((1/1.061619121e-6)-LibInt3)
                                            //
                                            //<DEBUG><20171001> std::cout << "DEBUG: calculating integral: LIB" << iLib+1 << " INT(" <<TempConstraint->fromLowerX << "," << TempConstraint->fromUpperX << ") = " << LibIntegrated[iLib] << ", " << "LibConstrainFactor = " << LibConstrainFactors[LibConstrainFactors.size()-1] << std::endl;
                                        } else {
                                            //<DEBUG><20171001> std::cout << "DEBUG: calculating integral: LIB" << iLib+1 << " INT(" <<TempConstraint->fromLowerX << "," << TempConstraint->fromUpperX << ") = " << LibIntegrated[iLib] << std::endl;
                                        }
                                    }
                                    //
                                    if(debug>=3) {std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": Setting MPACK->constrain()" << std::endl;}
                                    MPACK->constrain(TempConstraint->toLIB, LibConstrainNumbers, LibConstrainFactors);
                                    //
                                    break;
                                }
                            }
                        }
                    }
                    //
                    if(debug>=2) {std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": Calling MPACK->fit()" << std::endl;}
                    //
                    MPACK->fit();
                    //
                    // check parameter and write output results to stream
                    double chi2 = 0.0; for(int j=0;j<MPACK->chi2.size();j++) {chi2+=MPACK->chi2[j];}
                    std::stringstream pStream;
                    pStream << std::setw(8) << pParams->i << std::setw(15) << chi2;
                    for(long iLib = 0; iLib < pParams->nLib; iLib++) {
                        aFIT[iLib] = MPACK->aCOE[iLib];
                        pStream << std::setw(10) << pParams->idLIBList[iLib] * pParams->SDLIBList[iLib]->XNum << std::setw(15) << aFIT.at(iLib);
                    }
                    for(long iLib = 0; iLib < pParams->nLib; iLib++) {
                        //michi2DataClass *SDLIB = pParams->SDLIBList[iLib];
                        michi2DataClass *SDLIB = SDLIBS.at(iLib);
                        for(long iLibPar=0; iLibPar<SDLIB->FPAR.size(); iLibPar++) {
                            pStream << std::setw(15) << SDLIB->FPAR[iLibPar][0];
                        }
                    }
                    delete MPACK; MPACK = NULL;
                    pStream << std::endl;
                    pStrings[pParams->idOBS] += pStream.str();
                    if(debug>=2) {std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": Done MPACK" << pStream.str();
                        std::cout << std::endl;}
                }
            } else {
                // failed to pass the constraint check
                if(debug>=3) {std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": Did not pass the second constraint check!" << std::endl;}
            }
        } else {
            // failed to pass the constraint check
            if(debug>=3) {std::cout << "mnchi2parallel: subprocess " << pParams->iBegin << ": Did not pass the constraint check!" << std::endl;}
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
    //
    // check other processes, wait in order
    // <20180116> moved pthread_mutex_lock inside the loop, and added sleep time.
    for(;;) {
        // lock mutex
        pthread_mutex_lock(&mnchi2parallelMutex);
        std::cout << "mnchi2parallel: current subprocess iBegin=" << pParams->iBegin << " iEnd=" << pParams->iEnd << ": subprocess now locked! checking current waiting subprocess " << mnchi2parallelProgress << std::endl;
        // test if now we are ok to write result file
        if(pParams->iBegin==mnchi2parallelProgress) {
            // thread ok to write!
            std::cout << "mnchi2parallel: current subprocess iBegin=" << pParams->iBegin << " iEnd=" << pParams->iEnd << ": got ya! subprocess now being processed for output!" << std::endl;
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
            //std::cout << std::endl;
            //pthread_cond_signal(&mnchi2parallelCondition); // send finish signal //<20180116><DZLIU> I do the subprocess waiting by my self, using the number 'mnchi2parallelProgress', so I do not need this cond_wait().
            // unlock mutex
            pthread_mutex_unlock(&mnchi2parallelMutex); // unlock mutex // <BUG><20180116><DZLIU>: must unlock before break!!
            break;
        } else {
            // wait while other earlier threads to write result file
            //pthread_cond_wait(&mnchi2parallelCondition, &mnchi2parallelMutex); //<20180116><DZLIU> I do the subprocess waiting by my self, using the number 'mnchi2parallelProgress', so I do not need this cond_wait().
            std::cout << "mnchi2parallel: current subprocess iBegin=" << pParams->iBegin << " iEnd=" << pParams->iEnd << ": subprocess now still waiting for subprocess " << mnchi2parallelProgress << std::endl;
            // unlock mutex
            pthread_mutex_unlock(&mnchi2parallelMutex);
            std::cout << "mnchi2parallel: current subprocess iBegin=" << pParams->iBegin << " iEnd=" << pParams->iEnd << ": subprocess now unlocked!" << std::endl;
            // sleep
            int random_sleep_seconds = rand() % 10;
            std::cout << "mnchi2parallel: current subprocess iBegin=" << pParams->iBegin << " iEnd=" << pParams->iEnd << ": sleeping for " << 5 + random_sleep_seconds << " seconds ..." << std::endl;
            sleep(5 + random_sleep_seconds);
            continue;
        }
    }
    //
    // lock mutex
    pthread_mutex_lock(&mnchi2parallelMutex);
    //
    // update mnchi2parallelProgress
    //<20180116> now lock mutex when modify it!
    //mnchi2parallelProgress = pParams->i; // <BUG><20180116><DZLIU>
    mnchi2parallelProgress = pParams->iEnd+1; // <FIX><20180116><DZLIU>
    std::cout << "mnchi2parallel: current subprocess iBegin=" << pParams->iBegin << " iEnd=" << pParams->iEnd << ": subprocess now finished! ready to check next subprocess " << mnchi2parallelProgress << std::endl;
    //
    // unlock mutex
    pthread_mutex_unlock(&mnchi2parallelMutex);
    //
    // return
    return(NULL);
}
























