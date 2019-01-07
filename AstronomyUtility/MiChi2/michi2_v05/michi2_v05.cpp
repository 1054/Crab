/* 
 
 HIST: 20180224
       Metropolis Hasting Algorithm
       https://ysquared2.wordpress.com/2014/03/27/metropolis-hasting-algorithm-an-example-with-cc-and-r/
 
 */



#include "michi2_v05.h"
#include "spline.cpp"

extern vector<double> spline(vector<double> &x, vector<double> &y, vector<double> &output_x);



//const char *InfoRedshift = "";

int NumbParallel = 2; // number of parallel subprocesses

double Sampling = 0.8; // sampling fraction of the whole parater space

//double GlobalMinimumChisq = nan(""); // GlobalMinimumChisq

//std::vector<michi2Constraint *> Constraints;

std::vector<FilterCurveXY *> FilterCurves;

michi2ParallelPool *ppPool = new michi2ParallelPool();

std::default_random_engine RandomNumberGenerator;



/*
  MatchedObsLibStruct is a subroutine to match Obs and Lib.
  Obs and Lib all are 1D function, with two 1D arrays X and Y. But the sampling in X is different.
  This subroutine matches Lib X array to Obs X array and spline the Lib Y array at each Obs X array.
  We multiply OnePlusRedshift to Lib X array before the match.
*/
MatchedObsLibStruct michi2MatchObs(michi2DataClass *DCOBS, michi2DataClass *DCLIB, int debug)
{
    // This function will find the Y value in LIB whose X value matches the X value of each OBS
    
    MatchedObsLibStruct MatchedDAT;
    //std::cout << "michi2MatchObs: MatchedDAT=0x" << std::hex << (size_t)&MatchedDAT << std::dec << std::endl;
    
    // check DCOBS
    if(DCOBS->X.size()!=DCOBS->XNum || DCLIB->X.size()!=DCLIB->Y.size()) {
        std::cout << "michi2MatchObs: Error! Checking OBS and LIB data failed! OBS->X.size()=" << DCOBS->X.size() << " OBS->XNum=" << DCOBS->XNum << " OBS->Y.size()=" << DCOBS->Y.size() << " OBS->YNum=" << DCOBS->YNum << std::endl;
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
    //if(strlen(InfoRedshift)>0) { OnePlusRedshift = 1.0+atof(InfoRedshift); }
    if(std::isnan(DCOBS->XNorm)) {DCOBS->XNorm = 1.0;}
    
    
    for(int k=0; k<DCOBS->XNum; k++) {
        DCOBS->Matched[k]=0; // <TODO> we will always find a match for obs.
        if(debug>=2) { std::cout << "michi2MatchObs: debugging: we are trying to match obs X=" << DCOBS->XStr[k] << "(XNorm=" << DCOBS->XNorm << ", X*XNorm=" << DCOBS->X[k]*DCOBS->XNorm << " within lib X range " << LibRangeXMin << " " << LibRangeXMax << std::endl; }
        if(DCOBS->X[k]*DCOBS->XNorm>=LibRangeXMin && DCOBS->X[k]*DCOBS->XNorm<=LibRangeXMax) {
            DCOBS->Matched[k]=1; // <TODO> we will always find a match for obs.
            // now we need to find a value from library
            double tmpdiffvalue = -1.0;
            double mindiffvalue = -1.0;
            long   mindiffindex = -1;
            for(int j=0; j<DCLIB->XNum; j++) {
                tmpdiffvalue = (DCOBS->X[k]*DCOBS->XNorm - DCLIB->X[j]); // go search for the left side of OBS data point on X axis
                if(tmpdiffvalue>=0.0 && mindiffvalue<0.0) { mindiffvalue=tmpdiffvalue; mindiffindex=j; } // initial value
                if(tmpdiffvalue>=0.0 && tmpdiffvalue<mindiffvalue) { mindiffvalue=tmpdiffvalue; mindiffindex=j; } // compare value
            }
            // once got matched, we do linear interpolate
            // <20170930> or apply filter curve if user has input the filter curve ascii files
            if(mindiffindex>=0 && mindiffindex<=DCLIB->XNum-1) {
                int j = mindiffindex;
                int MatchedLibI = j;
                double MatchedLibY = DCLIB->Y[j];
                if(debug>=2) { std::cout << "michi2MatchObs: debugging: got matched lib Y value " << MatchedLibY << " at X between " << DCLIB->X[mindiffindex] << " " << DCLIB->X[mindiffindex+1] << std::endl; }
                // do linear interpolate to get more accurate MatchedLibY
                if(j<DCLIB->XNum-1) {
                    double f1_l1 = DCLIB->Y[j]; // left-side LIB data point
                    double f1_l2 = DCLIB->Y[j+1]; // right-side LIB data point, e.g. if we are matching ObsX = 24.0, LibX = 23.95, then we interpolate LibX(23.95,24.15)=>ObsX(24.00)
                    double f1_a1 = (DCOBS->X[k]*DCOBS->XNorm - DCLIB->X[j]); // as the e.g., a1 =  0.05
                    double f1_a2 = (DCLIB->X[j+1] - DCOBS->X[k]*DCOBS->XNorm); // as the e.g., a2 =  0.15
                    double f1_ai = (f1_a1+f1_a2); // then f1 = f1_l1 * 75% + f1_l2 * 25%
                    double f1_li = f1_l1*(f1_a2/f1_ai)+f1_l2*(f1_a1/f1_ai); // <TODO> bi-linear interpolation!
                    MatchedLibY = f1_li;
                    if(debug>=2) { std::cout << "michi2MatchObs: debugging: interpolated accurate lib Y value " << MatchedLibY << " at obs X " << DCOBS->X[k]*DCOBS->XNorm << std::endl; }
                }
                //
                // <20170930>
                // see whether user has input filter curve ascii files
                // the user should input as many filter curve ascii files as the OBS data point number
                std::string TempFilterCurveFilePath; TempFilterCurveFilePath.clear();
                std::vector<double> FilterCurveX; FilterCurveX.clear();
                std::vector<double> FilterCurveY; FilterCurveY.clear();
                if(debug>=2) { std::cout << "michi2MatchObs: debugging: FilterCurves.size() " << FilterCurves.size() << " k " << k << std::endl; }
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
                if(debug>=2) { std::cout << "michi2MatchObs: debugging: FilterCurveX.size() " << FilterCurveX.size() << " FilterCurveY.size() " << FilterCurveY.size() << " DCOBS->FilterCurveFilePath.size() " << DCOBS->FilterCurveFilePath.size() << std::endl; }
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
                                            if(debug>=2) { std::cout << "michi2MatchObs: debugging: applying filter curve from file: FilterCurveX = " << FilterCurveFileLineStrList[0] << ", FilterCurveY = " << FilterCurveFileLineStrList[1] << std::endl;  }
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
                        FilterCurveX[i1] = FilterCurveX[i1] * DCOBS->XNorm; // apply the same XNorm as OBS, for example 1/(1+z).
                        if(debug>=3) { std::cout << "michi2MatchObs: debugging: applying filter curve FilterCurveX = " << FilterCurveX[i1] << ", FilterCurveY = " << FilterCurveY[i1] << std::endl;  }
                    }
                    //
                    std::vector<double> FilterCurveY_Matched;
                    //for(int i1=0; i1<DCLIB->X.size(); i1++) {
                        //if(DCLIB->X[i1]>=FilterCurveX.front()*DCOBS->XNorm && DCLIB->X[i1]<=FilterCurveX.back()*DCOBS->XNorm) {
                        //    FilterCurveY_Matched[i1] = 1.0;
                        //} else {
                        //    FilterCurveY_Matched[i1] = 0.0;
                        //}
                    //}
                    //std::cout << "michi2MatchObs: debugging: FilterCurveY_Matched=0x" << std::hex << (size_t)&FilterCurveY_Matched << std::dec << std::endl;
                    
                    FilterCurveY_Matched = spline(FilterCurveX, FilterCurveY, DCLIB->X);
                    //double FilterCurveY_Matched_Total = 0.0;
                    //for(int i1=0; i1<DCLIB->X.size(); i1++) {
                    //    FilterCurveY_Matched_Total += FilterCurveY_Matched[i1];
                    //    //std::cout << "michi2MatchObs: debugging: splining filter curve value " << FilterCurveY_Matched[i1] << std::endl;
                    //}
                    //std::cout << "michi2MatchObs: debugging: FilterCurveY_Matched=0x" << std::hex << (size_t)&FilterCurveY_Matched << std::dec << std::endl;
                    //std::cout << "michi2MatchObs: debugging: DCLIB=0x" << std::hex << (size_t)&DCLIB << std::dec << std::endl;
                    //std::cout << "michi2MatchObs: debugging: DCLIB->X=0x" << std::hex << (size_t)&DCLIB->X << std::dec << std::endl;
                    //std::cout << "michi2MatchObs: debugging: DCLIB->Y=0x" << std::hex << (size_t)&DCLIB->Y << std::dec << std::endl;
                    //std::cout << "michi2MatchObs: debugging: DCLIB->FilterCurveFilePath=0x" << std::hex << (size_t)&DCLIB->FilterCurveFilePath << std::dec << std::endl;
                    //std::cout << "michi2MatchObs: debugging: DCLIB->TPAR=0x" << std::hex << (size_t)&DCLIB->TPAR << std::dec << std::endl;
                    //std::cout << "michi2MatchObs: debugging: DCOBS->FilterCurveFilePath=0x" << std::hex << (size_t)&DCOBS->FilterCurveFilePath << std::dec << std::endl;
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
                            FilterCurveX[i1] = FilterCurveX[i1] / DCOBS->XNorm; // restore back to original FilterCurveX
                        }
                        FilterCurves.resize(k+1);
                        FilterCurves[k] = new FilterCurveXY();
                        FilterCurves[k]->X.assign(FilterCurveX.begin(), FilterCurveX.end());
                        FilterCurves[k]->Y.assign(FilterCurveY.begin(), FilterCurveY.end());
                        FilterCurves[k]->Name = TempFilterCurveFilePath;
                        if(debug>=2) { std::cout << "michi2MatchObs: debugging: stored filter curve into FilterCurves[" << k << "]" << std::endl; }
                    }
                }
                if(debug>=3) { std::cout << "michi2MatchObs: debugging: DCLIB " << DCLIB << " DCOBS " << DCOBS << std::endl; }
                if(debug>=3) { std::cout << "michi2MatchObs: debugging: DCLIB->Matched["<<j<<"] " << DCLIB->Matched[j] << " DCOBS->Matched["<<k<<"] " << DCOBS->Matched[k] << std::endl; }
                //
                // save into MatchedDAT data structure
                DCLIB->Matched[j]=1;
                DCOBS->Matched[k]=1;
                MatchedDAT.j0.push_back(DCOBS->X[k]*DCOBS->XNorm); // obs
                MatchedDAT.f0.push_back(DCOBS->Y[k]); // obs
                MatchedDAT.df.push_back(DCOBS->YErr[k]); // obs unc
                MatchedDAT.f1.push_back(MatchedLibY);
                if(debug>=1) { std::cout << "michi2MatchObs: debugging: got matched lib Y value " << MatchedLibY << " comparing to obs Y value " << DCOBS->Y[k] << " at obs X value " << DCOBS->X[k] << "*" << DCOBS->XNorm << "=" << DCOBS->X[k]*DCOBS->XNorm << std::endl; }
                
                
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
            //if(debug>=2) {std::cout << "michi2MatchObs: debugging: FilterCurves=0x" << std::hex << (size_t)&FilterCurves << std::dec << std::endl;}
        } else {
            DCOBS->Matched[k]=1;
            MatchedDAT.j0.push_back(DCOBS->X[k]*DCOBS->XNorm); // obs
            MatchedDAT.f0.push_back(DCOBS->Y[k]); // obs
            MatchedDAT.df.push_back(DCOBS->YErr[k]); // obs unc
            MatchedDAT.f1.push_back(0.0); // lib <TODO>
            // MatchedDAT.f1.push_back(std::numeric_limits<double>::quiet_NaN()); // lib <TODO> what if lib does not cover full obs xrange ?
            if(debug>=1) { std::cout << "michi2MatchObs: debugging: no matched lib found for the given obs X value " << DCOBS->X[k] << "*" << DCOBS->XNorm << "=" << DCOBS->X[k]*DCOBS->XNorm << std::endl; }
        }
    }
    
    //if(debug>=2) {std::cout << "michi2MatchObs: debugging: FilterCurves=0x" << std::hex << (size_t)&FilterCurves << std::dec << std::endl;}
    
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

int michi2RandomIndexNearbyMinimumPoint(std::vector<double> vec, double RandomRadius)
{
    // find minimum point, then randomly choose a nearby index
    // we use Gaussian random
    int output_index = -1;
    double tmp_minchisq = nan("");
    int tmp_argwhere_minchisq = -1; // find miminum chisq element index
    std::vector<int> tmp_argwhere_nanvalue; // find the NaN element index
    for (int m=0; m<vec.size(); m++) { // loop to find the minimum chisq element index or the first NaN element index
        // for each value of k-th PAR in j-th LIB for i-th OBS
        if(!std::isnan(vec[m])) { // if it is non-NaN, then its chisq will be considered
            if(!std::isnan(tmp_minchisq)) {
                if(vec[m]<tmp_minchisq) { tmp_minchisq = vec[m]; tmp_argwhere_minchisq = tmp_argwhere_nanvalue.size()-1; } // make tmp_argwhere_minchisq the index in the tmp_argwhere_nanvalue array.
            } else { tmp_minchisq = vec[m]; tmp_argwhere_minchisq = tmp_argwhere_nanvalue.size()-1; } // make tmp_argwhere_minchisq the index in the tmp_argwhere_nanvalue array.
        } else { tmp_argwhere_nanvalue.push_back(m); } // store all NaN element index
    }
    /*
    if(0==tmp_argwhere_nanvalue.size()) { // if there are no NaN element, this means all this parameter values have been looped. Move to next iObs.
        iLibSubIdList.clear();
        break;
    }
     */
    /*
     // generate random index only in NaN area
    if(!std::isnan(tmp_minchisq)) { // if found the minimum chisq element index, then we compute a random number around it assuming a normal distribution
        std::normal_distribution<float> tmp_distrib(float(tmp_argwhere_minchisq)+0.5, float(tmp_argwhere_nanvalue.size())/RandomRadius); // <TODO> width=N/3.0, if N = 5.0 then width = 1.4, see http://www.cplusplus.com/reference/random/normal_distribution/
        iLibSubIdList[k] = tmp_argwhere_nanvalue[(int)(tmp_distrib(RandomNumberGenerator)) % tmp_argwhere_nanvalue.size()];
    } else { // if not valid chisq minimum element, then randomly take a NaN element
        iLibSubIdList[k] = tmp_argwhere_nanvalue[(int)(std::rand() % tmp_argwhere_nanvalue.size())]; // std::rand()%XXX constraints the max number to be XXX-1, otherwise is RANDOM_MAX which is maybe 65535
    }
     */
    if(!std::isnan(tmp_minchisq)) { // if found the minimum chisq element index, then we compute a random number around it assuming a normal distribution
        //std::normal_distribution<float> tmp_distrib(float(tmp_argwhere_minchisq)+0.5, float(tmp_argwhere_nanvalue.size())/RandomRadius);
        std::normal_distribution<float> tmp_distrib(float(tmp_argwhere_minchisq)+0.5, RandomRadius);
        output_index = (int)(tmp_distrib(RandomNumberGenerator)) % vec.size();
    } else { // if not valid chisq minimum element, then randomly take a NaN element
        output_index = (int)(std::rand() % vec.size());
    }
    //
    return output_index;
}

int michi2FindIndexOfMinimumValue(std::vector<double> vec)
{
    int tmp_argwhere_min = -1;
    double tmp_min = nan("");
    for (int i=0; i<vec.size(); i++) {
        if(!std::isnan(vec[i])) {
            if(std::isnan(tmp_min)) {tmp_argwhere_min = i; tmp_min = vec[i];}
            else if(vec[i] < tmp_min) {tmp_argwhere_min = i; tmp_min = vec[i];}
        }
    }
    return tmp_argwhere_min;
}


/*
 long michi2FindClosestValue(std::vector<long> const& vec, long input_value)
{
    // see https://stackoverflow.com/questions/8647635/elegant-way-to-find-closest-value-in-a-vector-from-above
    long tmp_argwhere_min = -1;
    long tmp_min = vec[0];
    for (int i=0; i<vec.size(); i++) {
        if(vec[i]==input_value) {tmp_argwhere_min = i; tmp_min = vec[i]; return tmp_min;} // if there is a same value, return it
    }
    for (int i=0; i<vec.size(); i++) {
        if(std::abs(vec[i]-input_value) < std::abs(tmp_min-input_value)) {tmp_argwhere_min = i; tmp_min = vec[i];}
    }
    return tmp_min;
}
 // replaced by ppPool->findIndexOfClosestIdInPool()
 */


/*
long michi2FindIndexOfClosestValue(std::vector<long> vec, long input_value)
{
    auto const it = std::lower_bound(vec.begin(), vec.end(), input_value);
    if (it == vec.end()) {
        return vec.back();
    }
    return std::distance(vec.begin(), it);
}
 // replaced by ppPool->findIndexOfClosestIdInPool()
 */









































/*
  20160714 mnchi2
  This is the main subroutine to achieve multi-thread model fitting.
*/
/* --- pthread ---- */
pthread_mutex_t mnchi2parallelMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t mnchi2parallelCondition = PTHREAD_COND_INITIALIZER;
//long mnchi2parallelProgress = 0;
/* ---------------- */

void mnchi2(std::vector<std::string> InputObsList,
            std::vector<std::string> InputLibList,
            std::vector<std::string> OutputTableList,
            std::vector<double>      InputRedshiftList,
            std::vector<std::string> InputConstraintList,
            std::vector<std::string> InputFilterCurveList,
            int DebugLevel)
{
    //
    // read input obs data file list
    // and store basic information into 'SDOBSList',
    // also check 'InputFilterCurveList' for each obs data file.
    //
    if(0==InputObsList.size()) {
        std::cout << "Error! mnchi2 InputObsList is empty!" << std::endl; return;
    }
    std::vector<michi2DataClass *> SDOBSList; SDOBSList.clear();
    int NumbObs = InputObsList.size();
    for(int i=0; i<InputObsList.size(); i++) {
        michi2DataClass *SDOBS = new michi2DataClass(InputObsList.at(i).c_str(), DebugLevel-1);
        if(InputFilterCurveList.size()>i) {
            SDOBS->readFilterCurveListFile(InputFilterCurveList.at(i).c_str());
        }
        if(InputRedshiftList.size()>i) {
            SDOBS->XNorm = 1.0/(1.0+InputRedshiftList[i]); // apply XNorm for each OBS if there is an InputRedshift paired with it.
            SDOBS->YNorm = (1.0+InputRedshiftList[i]); // redshift to rest-frame, assuming flux density unit is mJy (erg s-1 cm-2 Hz-1), Y' = Y * (1+z) and X' = X / (1+z).
        }
        SDOBSList.push_back(SDOBS);
    }
    //
    // read input lib data file list
    // and store basic information into 'SDLIBList'.
    //
    if(0==InputLibList.size()) {
        std::cout << "Error! mnchi2 InputLibList is empty!" << std::endl; exit (EXIT_FAILURE); // return;
    }
    std::vector<michi2DataClass *> SDLIBList; SDLIBList.clear();
    //int NumbLib = InputLibList.size(), NumbLibPar = 0, NumbLibVar = 0, NumbLibMulti = 1;
    for(int i=0; i<InputLibList.size(); i++) {
        michi2DataClass *SDLIB = new michi2DataClass(InputLibList.at(i).c_str(), DebugLevel-1);
        bool CheckLibParConsistency = SDLIB->checkParameterConsistency(DebugLevel);
        if(!CheckLibParConsistency) {std::cout << "Error! mnchi2 InputLibList["<<i<<"] \"" << InputLibList.at(i) << "\" has invalid format!" << std::endl; exit (EXIT_FAILURE);}
        SDLIB->XNorm = 1.0;
        SDLIB->YNorm = 1.0;
        SDLIBList.push_back(SDLIB);
        //NumbLibPar += SDLIB->TPAR.size(); // SDLIB->TPAR is the list of parameter title in a LIB file, so its size() is the number of parameters to be fit in a LIB file.
        //NumbLibVar += SDLIB->YNum; // SDLIB->YNum is the number of all independent models in one LIB file, while SDLIB->XNum is the number of wavelength or any X axis in one model.
        //NumbLibMulti = NumbLibMulti * SDLIB->YNum; // This gives the number of all model combinations among all LIB files.
    }
    //
    // check output table data file list
    // and prepare output file pointer
    // and loop SDOUTList and write info and chisq table file header
    //
    if(0==OutputTableList.size()) {
        std::cout << "Error! mnchi2 OutputTableList is empty!" << std::endl; return;
    }
    if(InputObsList.size()!=OutputTableList.size()) {
        std::cout << "Error! mnchi2 InputObsList and OutputTableList do not match in size!" << std::endl; return;
    }
    std::vector<std::ofstream*> SDOUTList; SDOUTList.clear();
    for(int i=0; i<OutputTableList.size(); i++) {
        std::ofstream *SDOUT = new ofstream(OutputTableList.at(i).c_str(), std::ofstream::out); // std::ofstream SDOUT(pParams->OutputTableList.at(i).c_str(), std::ofstream::out | std::ofstream::app);
        //
        // write info file
        // output the input obs and lib files
        std::string OutputInfoFile = OutputTableList[i]; OutputInfoFile.append(".info");
        std::ofstream OutputInfoFileStream(OutputInfoFile.c_str());
        if(DebugLevel>=1) {std::cout << "mnchi2: writing into " << OutputInfoFile << std::endl; }
        if(i<InputObsList.size()) {
            OutputInfoFileStream << "OBS = " << InputObsList[i] << std::endl;
        } else {
            OutputInfoFileStream << "OBS = " << "N/A" << std::endl;
        }
        OutputInfoFileStream << "NLIB = " << InputLibList.size() << std::endl;
        for(int j=0; j<InputLibList.size(); j++) {
            OutputInfoFileStream << "LIB" << j+1 << " = " << InputLibList[j] << std::endl;
        }
        OutputInfoFileStream << "OUT = " << OutputTableList[i] << std::endl;
        if(i<InputRedshiftList.size()) {
            OutputInfoFileStream << "REDSHIFT = " << std::to_string(InputRedshiftList[i]) << std::endl;
        }
        OutputInfoFileStream.close();
        //
        // write chisq table file
        // output i0, chi2, iN, aN column headers
        if(DebugLevel>=0) {std::cout << "mnchi2: writing into " << OutputTableList.at(i).c_str() << std::endl; }
        *SDOUT << "#" << std::setw(10) << "i0" << std::setw(15) << "chi2"; // write the output file header line
        std::ostringstream tmpstr;
        for(int j=0; j<InputLibList.size(); j++) {
            tmpstr.str(""); tmpstr << "i" << j+1; *SDOUT << std::setw(10) << tmpstr.str();
            tmpstr.str(""); tmpstr << "a" << j+1; *SDOUT << std::setw(15) << tmpstr.str();
        }
        //
        // loop SDLIB, output paramN column headers
        for(int j=0; j<SDLIBList.size(); j++) {
            for(int iLibPar=0; iLibPar<SDLIBList[j]->TPAR.size(); iLibPar++) {
                *SDOUT << std::setw(15) << SDLIBList[j]->TPAR[iLibPar]; // write each title of parameter to be fit in each LIB file
            }
        }
        *SDOUT << std::endl << "#" << std::endl;
        SDOUT->flush();
        SDOUTList.push_back(SDOUT);
    }
    //
    // Make constraints
    //
    if(DebugLevel>=0) {std::cout << "mnchi2: making constraints " << std::endl; }
    std::vector<michi2Constraint *> Constraints; Constraints.clear();
    for(int i=0; i<InputConstraintList.size(); i++) {
        if(!InputConstraintList[i].empty()) {
            if(i==0) {
                for(int j=0; j<SDLIBList.size(); j++) { SDLIBList[j]->loadDataBlock(0); } // initialize everything with first LIB model. -- no need to be done here. now implemented when `new michi2Constraint`.
            }
            std::string TempConstraintStr = InputConstraintList[i];
            michi2Constraint *TempConstraint = new michi2Constraint();
            TempConstraint->parse(TempConstraintStr, SDOBSList[0], SDLIBList, DebugLevel-1); // initialize everything with first OBS and first LIB model.
            if(DebugLevel>=2) {std::cout << "mnchi2: parsing constraint " << i+1 << " \"" << TempConstraint->ConstraintStr << "\""; }
            //
            // set DataBlockIndexIsFreezed if the LIB INDEX depends on user input constraints
            if(TempConstraint->ConstraintType == michi2Constraint::EnumConstraintType::INDEX) {
                for(int k=0; k<TempConstraint->AssignmentsLibIndices.size(); k++) {
                    SDLIBList[TempConstraint->AssignmentsLibIndices[k]]->DataBlockIndexIsFreezed = true;
                    if(DebugLevel>=2) {std::cout << " ... LIB" << TempConstraint->AssignmentsLibIndices[k]+1 << "INDEX will be freezed!";}
                }
            }
            if(DebugLevel>=2) {std::cout << std::endl;}
            //
            Constraints.push_back(TempConstraint);
            //
            InputConstraintList[i] = TempConstraintStr;
        }
    }
    //
    // test function SED()
    //
    if(DebugLevel>=2) {
        //std::cout << "mnchi2: testing SED<double>() ... " << std::endl;
        //SED<double> *SED_func_test = new SED<double>();
        //double var_test_123 = (*SED_func_test).SED(SDLIBList[2]->X, SDLIBList[2]->Y);
    }
    //
    // initialize global variable michi2ParallelPool *ppPool
    //
    ppPool->initialize(SDOBSList, SDLIBList, SDOUTList, Constraints, NumbParallel, DebugLevel-1);
    //
    // make pthread
    //
    for (int ip=0; ip<ppPool->NumbParallel; ip++) {
        //
        // <20150309> make pthread
        //if(DebugLevel>=4) { std::cout << "mnchi2: &(ppPool->ParamsList["<<ip<<"]) = 0x" << std::hex << (size_t)((ppPool->ParamsList[ip])) << std::dec << std::endl; }
        pthread_t thread1; pthread_create(&thread1, NULL, &mnchi2parallel, (void *)(ppPool->ParamsList[ip]));
        //
        // sleep for a few seconds to allow the subprocess to be created.
        //if(i1>=500) {sleep(60);} else {sleep(2);}
        //sleep(5);// before 20180114
        //sleep(3);// before 20180206
        if(DebugLevel>=1) { std::cout << "mnchi2: sleeping for 2 seconds" << std::endl; }
        usleep(2000);
    }
    
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
    //
    //
    //
    
    
    
    
    int NumbOfActLoops;
    if(Sampling >= 1.0) {
        NumbOfActLoops = (int) ( Sampling );
    } else {
        NumbOfActLoops = (int) ( Sampling * (double)(ppPool->NumbOfAllLoops) );
    }
    
    //
    // wait for all threads and print progress
    //
    while((ppPool->IdDone.size() <= NumbOfActLoops) &&
          (ppPool->IdPool.size()>0) )
    {
        //
        // print progress
        std::stringstream progress_info;
        std::stringstream progress_info_prefix;
        if(DebugLevel>=0) {
            int ndigits = (int)(std::ceil(log10( NumbOfActLoops )));
            progress_info_prefix << "[" << std::setw(ndigits) << std::right << ppPool->IdDone.size() << "/" << std::left << NumbOfActLoops << "]"; //<20180116>
            for(int ip=0; ip<ppPool->NumbParallel; ip++) {
                if(ip==0) { progress_info << progress_info_prefix.str(); }
                else { progress_info << std::setw(progress_info_prefix.str().size()) << " "; }
                progress_info << " 0x" << std::hex << (size_t)(ppPool->ParamsList[ip]) << std::dec;
                progress_info << " " << ppPool->ParamsList[ip]->LoopInfoStr;
                progress_info << "\n"; // if(ip<ppPool->NumbParallel-1) {progress_info << "\n";} // print new line except for the last line
            }
        }
        //
        // print parameters
        /*
        if(DebugLevel>=1) {
            for(int i=0; i<ppPool->nObs; i++) {
                progress_info << " PAR {";
                for(int j=0; j<ppPool->nLib; j++) {
                    if(j>0) {progress_info << "|";}
                    for(int k=0; k<ppPool->SDLIBList[j]->NPAR.size(); k++) {
                        int ndigits3 = (int)(std::ceil(log10((double)(ppPool->SDLIBList[j]->NPAR[k]))));
                        if(k>0) {progress_info << ",";}
                        progress_info << std::setw(ndigits3) << std::right << ppPool->ARGMINPARAMS[i][j][k] << "/" << std::setw(ndigits3) << std::left << ppPool->SDLIBList[j]->NPAR[k];
                    }
                }
                progress_info << "}";
            }
        }
         */
        if(DebugLevel>=1) {
            for(int i=0; i<ppPool->nObs; i++) {
                progress_info << std::setw(progress_info_prefix.str().size()) << " ";
                progress_info << " --- FITTED PARAMS {";
                double progress_chisq = nan("");
                int line_break_width = 120;
                int line_break_counter = (long)(std::ceil((double)(progress_info.str().size()) / (double)(line_break_width))) * line_break_width;
                for(int j=0; j<ppPool->nLib; j++) {
                    for(int k=0; k<ppPool->SDLIBList[j]->NPAR.size(); k++) {
                        //int ndigits3 = (int)(std::ceil(log10((double)( *std::max_element( ppPool->VALUE4PARAMS[i][j][k].begin(), ppPool->VALUE4PARAMS[i][j][k].end() ) ))));
                        progress_info << ppPool->SDLIBList[j]->TPAR.at(k) << " = "; // PAR title
                        if(ppPool->ARGMINPARAMS[i][j][k] >= 0) {
                            //progress_info << std::setw(ndigits3) << std::right << ppPool->VALUE4PARAMS[i][j][k].at(ppPool->ARGMINPARAMS[i][j][k]);
                            progress_info << ppPool->VALUE4PARAMS[i][j][k].at(ppPool->ARGMINPARAMS[i][j][k]);
                            progress_chisq = ppPool->CHISQ4PARAMS[i][j][k].at(ppPool->ARGMINPARAMS[i][j][k]);
                        } else {
                            //progress_info << std::setw(ndigits3) << std::right << -1;
                            progress_info << -1;
                        }
                        if(k<ppPool->SDLIBList[j]->NPAR.size()-1) {progress_info << ", ";} // print ", " after each PAR
                        if(k==ppPool->SDLIBList[j]->NPAR.size()-1 && j<ppPool->nLib-1) {progress_info << "; ";} // print "; " after all PARs for each LIB
                        if(progress_info.str().size() > line_break_counter) { // insert possible line break here
                            progress_info << "\n" << std::setw(progress_info_prefix.str().size() + std::string(" --- FITTED PARAMS {").size()) << " "; // line break
                            line_break_counter+=line_break_width;
                        }
                    }
                }
                progress_info << "}";
                progress_info << "\n";
                progress_info << std::setw(progress_info_prefix.str().size()) << " ";
                progress_info << " --- CHISQ {" << progress_chisq << "}";
            }
        }
        std::cout << "\n" << progress_info.str() << std::endl;
        //
        // sleep
        if((ppPool->IdDone.size() <= NumbOfActLoops) &&
           (ppPool->IdPool.size()>0) )
        {
            if(DebugLevel>=1) {
                sleep(1); // sleep for 1 second
            } else {
                sleep(4 + rand() % 4); // sleep for 4 to 8 seconds
            }
        } else {
            break;
        }
    }
    //
    // when all threads finished, print a progress of 100%
    //
    int ndigitsfinal = (int)(std::ceil(log10(NumbOfActLoops))) * 2 + 1;
    std::cout << "[" << std::setw(ndigitsfinal) << std::right << "100%" << "]" << std::endl;
    
    
    //
    // TODO:
    // output ppPool->CHISQ4PARAMS and ppPool->VALUE4PARAMS
    //
    
}

void *mnchi2parallel(void *params)
{
    //
    // take over params as pParams
    struct mnchi2parallelParams *pParams = (struct mnchi2parallelParams *)params;
    int debug = pParams->DebugLevel;
    //
    // prepare debug info
    std::stringstream process_info;
    process_info << "mnchi2parallel: subprocess 0x" << std::hex << (size_t)(pParams) << std::dec;
    std::stringstream debug_info;
    long process_delay = 0;
    //
    if(debug>=1) {std::cout << process_info.str() << ": Debugging level " << debug << std::endl;}
    if(debug>=4) {std::cout << process_info.str() << ": pParams = 0x" << std::hex << (size_t)(pParams) << std::dec << std::endl;}
    if(debug>=1) {for(int i=0; i<pParams->SDOBSList.size(); i++) {std::cout << process_info.str() << ": Connected to pParams->SDOBSList["<<i<<"] = 0x" << std::hex << (size_t)(pParams->SDOBSList[i]) << std::dec << std::endl;}}
    if(debug>=1) {for(int i=0; i<pParams->SDLIBList.size(); i++) {std::cout << process_info.str() << ": Connected to pParams->SDLIBList["<<i<<"] = 0x" << std::hex << (size_t)(pParams->SDLIBList[i]) << std::dec << std::endl;}}
    if(debug>=2) {for(int i=0; i<pParams->SDLIBList.size(); i++) {std::cout << process_info.str() << ": Connected to pParams->SDLIBList["<<i<<"]->X.data() = 0x" << std::hex << (size_t)(pParams->SDLIBList[i]->X.data()) << std::dec << " size " << pParams->SDLIBList[i]->X.size() << " pParams->SDLIBList["<<i<<"]->Y.data() = 0x" << std::hex << (size_t)(pParams->SDLIBList[i]->Y.data()) << std::dec << " size " << pParams->SDLIBList[i]->Y.size() << std::endl;}} // std::vector::data() is supported since c++11.
    if(debug>=2) {for(int i=0; i<pParams->SDLIBList.size(); i++) {std::cout << process_info.str() << ": Connected to &(pParams->SDLIBList["<<i<<"]->X[0]) = 0x" << std::hex << (size_t)(&(pParams->SDLIBList[i]->X[0])) << std::dec << " size " << pParams->SDLIBList[i]->X.size() << " &(pParams->SDLIBList["<<i<<"]->Y[0]) = 0x" << std::hex << (size_t)(&(pParams->SDLIBList[i]->Y[0])) << std::dec << " size " << pParams->SDLIBList[i]->Y.size() << std::endl;}} // std::vector::data() is supported since c++11.
    //
    // initialize pParams
    pParams->iObs = -1; pParams->iLib.resize(pParams->SDLIBList.size(), -1);
    //
    // prepare LoopInfo stringstream
    pParams->LoopInfoStr = "<NO INFO>";
    pParams->LoopInfo.str("");
    pParams->LoopInfo << "OBS [" << pParams->iObs+1 << "/" << pParams->nObs << "]" << " ";
    pParams->LoopInfo << "LIB [";
    for(int j = 0; j < pParams->iLib.size(); j++) {
        int ndigits3 = (int)(std::ceil(log10((double)(pParams->SDLIBList[j]->YNum)))); if(j>0) {pParams->LoopInfo << "|";}
        pParams->LoopInfo << std::setw(ndigits3) << std::right << pParams->iLib[j]+1 << "/" << std::setw(ndigits3) << std::left << pParams->SDLIBList[j]->YNum;
    } pParams->LoopInfo << "]"; pParams->LoopInfoStr = pParams->LoopInfo.str();
    //
    // hand over constraint strs for each subprocess
    //pthread_mutex_lock(&mnchi2parallelMutex); // lock mutex
    std::vector<michi2Constraint *> SubprocessConstraints; SubprocessConstraints.clear();
    for(int i = 0; i < ppPool->OtherConstraints.size(); i++) { // ppPool->OtherConstraints are non-INDEX constraints.
        michi2Constraint *TempConstraint = new michi2Constraint(); // must new these constraints for each subprocess.
        std::string TempConstraintStr = ppPool->OtherConstraints[i]->ConstraintStr;
        TempConstraint->parse(TempConstraintStr, pParams->SDOBSList[0], pParams->SDLIBList, debug-1); // initialize everything with first OBS and first LIB model. // <TODO> what if iObs changed? Hope constraints are not on OBS... <TODO>
        SubprocessConstraints.push_back(TempConstraint);
    }
    std::vector<michi2Constraint *> FreezedConstraints; FreezedConstraints.clear();
    for(int i = 0; i < ppPool->FreezedConstraints.size(); i++) { // ppPool->OtherConstraints are non-INDEX constraints.
        michi2Constraint *TempConstraint = new michi2Constraint(); // must new these constraints for each subprocess.
        std::string TempConstraintStr = ppPool->FreezedConstraints[i]->ConstraintStr;
        TempConstraint->parse(TempConstraintStr, pParams->SDOBSList[0], pParams->SDLIBList, debug-1); // initialize everything with first OBS and first LIB model. // <TODO> what if iObs changed? Hope constraints are not on OBS... <TODO>
        FreezedConstraints.push_back(TempConstraint);
    }
    //pthread_mutex_unlock(&mnchi2parallelMutex); // unlock mutex
    //
    // prepare string variable for storing the output content for each Obs
    std::vector<std::string> pStrings; pStrings.resize(pParams->nObs);
    //
    // set number of active loops
    int NumbOfActLoops;
    if(Sampling >= 1.0) {
        NumbOfActLoops = (int) ( Sampling );
    } else {
        NumbOfActLoops = (int) ( Sampling * (double)(ppPool->NumbOfAllLoops) );
    }
    //
    // while loop
    while((ppPool->IdDone.size() <= NumbOfActLoops) &&
          (ppPool->IdPool.size()>0) )
    {
        std::chrono::high_resolution_clock::time_point Timer1 = std::chrono::high_resolution_clock::now();
        std::chrono::high_resolution_clock::time_point Timer2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double,std::milli> TimeSpan = Timer2 - Timer1;
        if(debug>=1) {std::cout << process_info.str() << ": Starting IdPool at time 0" << std::endl;}
        //
        // prepare iObs
        int iObs = pParams->iObs;
        //
        // choose a random iObs from iObsPool if not ready
        if(iObs<0) { iObs = ppPool->iObsPool[std::rand() % ppPool->iObsPool.size()]; }
        //
        // prepare iLib (IdLibList), which is a list of indices for models in each LIB1, LIB2, LIB3, ...
        std::vector<int> iLib; iLib.resize(pParams->SDLIBList.size(), -1);
        //
        // lock mutex
        pthread_mutex_lock(&mnchi2parallelMutex); // lock mutex for writing ppPool->CHISQ4PARAMS
        if(debug>=2) {std::cout << process_info.str() << ": pthread mutex locked for analyzing chisq array!" << std::endl;}
        //
        // analyze chisq array for each parameter in each LIB for current iObs
        while (true) {
            int i = iObs;
            // for i-th OBS
            //bool HasProcessedAll = true; // no, this is not ture, even we have all valid values for each PAR, we still have not looped all combinations between PARs.
            for (int j=0; j<ppPool->CHISQ4PARAMS[i].size(); j++) {
                // for j-th LIB
                if(pParams->SDLIBList[j]->DataBlockIndexIsFreezed) {
                    iLib[j] = 0;
                    continue;
                }
                std::vector<int> iLibSubIdList; iLibSubIdList.resize(pParams->SDLIBList[j]->NPAR.size(), -1);
                for (int k=0; k<ppPool->CHISQ4PARAMS[i][j].size(); k++) {
                    // for k-th PAR
                    // check if has processed all
                    int nProbed = 0;
                    for(int m=0; m<ppPool->CHISQ4PARAMS[i][j][k].size(); m++) { // loop values for k-th PAR
                        if(std::isnan(ppPool->CHISQ4PARAMS[i][j][k][m])) {
                            //HasProcessedAll = false;
                        } else {
                            nProbed++; // count number of probed grid points in the chisq space for a given PAR
                        }
                    }
                    // analyze chisq array for each parameter
                    // first find the minimum chisq point, if all are NaN then it will set NaN to ppPool->ARGMINPARAMS[i][j][k]
                    ppPool->ARGMINPARAMS[i][j][k] = michi2FindIndexOfMinimumValue(ppPool->CHISQ4PARAMS[i][j][k]);
                    // then determine a radius for next step finding a Gaussian random index nearby the minimum chisq point, the radius is larger if the chisq space is less probed
                    double RandomRadius = (double)(ppPool->CHISQ4PARAMS[i][j][k].size())/1.0; // (double)(ppPool->CHISQ4PARAMS[i][j][k].size())/std::sqrt((double)(nProbed)+1.0); // if size = 10, nProbed = 0, then RandomRadius = 10/sqrt(1) ~ 10; // if size = 10, nProbed = 1, then RandomRadius = 10/sqrt(2) ~ 7; // if size = 10, nProbed = 5, then RandomRadius = 10/sqrt(6) ~ 4;
                    if(RandomRadius<3.0) {RandomRadius = 3.0;} // set a minimum random radius
                    // then find an element nearby the minimum chisq point which will be fitted in this loop
                    iLibSubIdList[k] = michi2RandomIndexNearbyMinimumPoint(ppPool->CHISQ4PARAMS[i][j][k], RandomRadius);
                    // setting the selected model chisq to 999999.9 so that other processes will not try to fit it at the same time
                    if(std::isnan(ppPool->CHISQ4PARAMS[i][j][k][iLibSubIdList[k]])) {
                        ppPool->CHISQ4PARAMS[i][j][k][iLibSubIdList[k]] = 999999.9;
                    }
                    // -> this will mark 999999.9 for iLibSubIdList[k]-th value of each PAR in each free LIB.
                }
                // convert SubIdList to the model index in j-th LIB
                iLib[j] = pParams->SDLIBList[j]->convertSubIdListToId(iLibSubIdList); // taking the (iLib[j])-th model from the j-th LIB.
                if(debug>=4) {std::cout << process_info.str() << ": analyzing chisq for LIB" << j+1 << " IdModel " << iLib[j] << " SubIdList "; for(int ick=0; ick<iLibSubIdList.size(); ick++) {std::cout << iLibSubIdList[ick] << "/" << pParams->SDLIBList[j]->NPAR[ick] << " ";} std::cout << std::endl;}
            }
            /*
            if(HasProcessedAll) { // if jump to here, then it means there are no unprocessed parameter for this iObs. Remove this iObs from ppPool->iObsPool and pick up another iObs from ppPool->iObsPool to fit.
                ppPool->iObsPool.erase(std::remove(ppPool->iObsPool.begin(), ppPool->iObsPool.end(), iObs), ppPool->iObsPool.end()); // remove iObs from iObsPool. See https://stackoverflow.com/questions/3385229/c-erase-vector-element-by-value-rather-than-by-position
                if(ppPool->iObsPool.size() > 0) {
                    iObs = ppPool->iObsPool.at(std::rand() % ppPool->iObsPool.size());
                    continue;
                } else {
                    iObs = -1; // no iObs to loop! all finished!
                    break; // finished chi-square analysis
                }
            }
            */
            // iObs is not changed and iLib has been newly determined from the above chi-square analysis. Now let's proceed!
            break; // finished chi-square analysis
        }
        //
        // unlock mutex
        pthread_mutex_unlock(&mnchi2parallelMutex); // unlock mutex
        if(debug>=2) {std::cout << process_info.str() << ": pthread mutex unlocked!" << std::endl;}
        //
        // update current data block index for LIBs
        for(int i = 0; i < iLib.size(); i++) {
            pParams->SDLIBList[i]->CurrentDataBlockIndex = iLib[i];
            pParams->SDLIBList[i]->CurrentDataBlockIndexAsDouble = (double)(iLib[i]);
        }
        //
        // parse INDEX constraints
        if(debug>=1) {std::cout << process_info.str() << ": parsing INDEX constraints" << std::endl;}
        for(int i = 0; i < FreezedConstraints.size(); i++) { // FreezedConstraints are INDEX constraints.
            //FreezedConstraints[i]->recompile(debug-1); //<20181002> no need to recompile now, exprtk issue has been fixed. see test under "test_michi2Constraint_expr2/" folder.
            FreezedConstraints[i]->evaluate(); // evaluate the expression, this->SDLIBList[j]->CurrentDataBlockIndexAsDouble will be updated
            for(int j = 0; j < FreezedConstraints[i]->AssignmentsLibIndices.size(); j++) {
                int k = FreezedConstraints[i]->AssignmentsLibIndices[j];
                pParams->SDLIBList[k]->CurrentDataBlockIndex = (long)(pParams->SDLIBList[k]->CurrentDataBlockIndexAsDouble);
                iLib[k] = pParams->SDLIBList[k]->CurrentDataBlockIndex;
            }
        }
        //
        // convert (iLib, iObs) pair to iPool for fitting the data and model
        if(debug>=1) {std::cout << process_info.str() << ": ppPool->convertIdLibListToIdPool(iLib,iObs), iLib=["; for(int j=0; j<iLib.size(); j++) {std::cout << iLib[j]; if(j<iLib.size()-1) {std::cout << ",";}} std::cout << "], iObs=" << iObs << std::endl;}
        long long iPool = ppPool->convertIdLibListToIdPool(iLib, iObs); // convert paired OBS and LIBs indices to a global pool index
        if(iPool<0) {iPool = ppPool->convertIdLibListToIdPool(iLib, iObs, 1); } // debug it if we get negative iPool! 
        if(debug>=1) {std::cout << process_info.str() << ": ppPool->convertIdLibListToIdPool(iLib,iObs), iLib=["; for(int j=0; j<iLib.size(); j++) {std::cout << iLib[j]; if(j<iLib.size()-1) {std::cout << ",";}} std::cout << "], iObs=" << iObs << ", iPool = " << iPool << std::endl;}
        //
        // find a nearest iPool in ppPool->IdPool
        ////long iPool2 = michi2FindClosestValue(ppPool->IdPool, iPool); // this can prevent from repeated processing
        ////int iObs2 = ppPool->convertIdPoolToIdObs(iPool2);
        ////std::vector<int> iLib2 = ppPool->convertIdPoolToIdLibList(iPool2); // this is buggy because of the freezed SubIds can not be computed well
        //long argWhere2 = michi2FindIndexOfClosestValue(ppPool->IdPool, iPool); // this can prevent from repeated processing
        pthread_mutex_lock(&mnchi2parallelMutex); // lock mutex for calling ppPool->findIndexOfClosestIdInPool() to query ppPool->IdPool
        if(debug>=2) {std::cout << process_info.str() << ": pthread mutex locked for querying ppPool->IdPool!" << std::endl;}
        if(debug>=1) {std::cout << process_info.str() << ": ppPool->findIndexOfClosestIdInPool(iPool=" << iPool << ")" << std::endl;}
        long argWhere2 = ppPool->findIndexOfClosestIdInPool(iPool); // we only process what are in the Pool
        long long iPool2; std::vector<int> SubIdList2; int iObs2; std::vector<int> iLib2;
        if(debug>=1) {std::cout << process_info.str() << ": ppPool->findIndexOfClosestIdInPool(iPool=" << iPool << ") returned " << argWhere2 << std::endl;}
        if(argWhere2 >= 0) {
            iPool2 = ppPool->IdPool.at(argWhere2);
            SubIdList2 = ppPool->SubIdListPool.at(argWhere2);
            iObs2 = SubIdList2.front();
            iLib2.resize(SubIdList2.size()-1); iLib2.assign(SubIdList2.begin()+1, SubIdList2.end());
        } else {
            iPool2 = iPool;
            iObs2 = iObs;
            iLib2 = iLib;
        }
        pthread_mutex_unlock(&mnchi2parallelMutex); // unlock mutex
        if(debug>=2) {std::cout << process_info.str() << ": pthread mutex unlocked!" << std::endl;}
        if(debug>=2) {
            std::cout << "mnchi2parallel: DEBUG: iObs = " << iObs << ", iLib = "; for(int k=0; k<iLib.size(); k++) {std::cout << iLib[k] << " ";} std::cout << ", iPool = " << iPool << std::endl;
            std::cout << "mnchi2parallel: DEBUG: iObs2 = " << iObs2 << ", iLib2 = "; for(int k=0; k<iLib2.size(); k++) {std::cout << iLib2[k] << " ";} std::cout << ", iPool2 = " << iPool2 << std::endl;
            //std::cout << "mnchi2parallel: DEBUG: ppPool->IdPool = "; for(int k=0; k<ppPool->IdPool.size(); k++) {std::cout << ppPool->IdPool[k] << " ";} std::cout << std::endl;
        }
        if(iPool2 != iPool) { // this can happen if user has some constraints which filtered out some IdPool.
            // mark current iPool CHISQ to 999999.9 so that other processes will not try to run it. we will update CHISQ after each loop.
            pthread_mutex_lock(&mnchi2parallelMutex); // lock mutex for writing ppPool->CHISQ4PARAMS, if iPool2 != iPool, i.e., iPool did not pass the constraints.
            // for i-th OBS
            for (int i=iObs2, j=0; j<ppPool->CHISQ4PARAMS[i].size(); j++) {
                // for j-th LIB
                std::vector<int> iLibSubIdList = pParams->SDLIBList[j]->convertIdToSubIdList(iLib2[j]);
                for (int k=0; k<ppPool->CHISQ4PARAMS[i][j].size(); k++) {
                    // for k-th PAR
                    if(std::isnan(ppPool->CHISQ4PARAMS[i][j][k][iLibSubIdList[k]])) {
                        ppPool->CHISQ4PARAMS[i][j][k][iLibSubIdList[k]] = 999999.9;
                    }
                }
            }
            pthread_mutex_unlock(&mnchi2parallelMutex); // unlock mutex
            pParams->iPool = iPool2;
            pParams->iObs = iObs2;
            pParams->iLib = iLib2;
        } else {
            pParams->iPool = iPool;
            pParams->iObs = iObs;
            pParams->iLib = iLib;
        }
        //
        Timer2 = std::chrono::high_resolution_clock::now(); TimeSpan = Timer2 - Timer1;
        if(debug>=1) {std::cout << process_info.str() << ": Determined a pParams->iPool " << pParams->iPool << " at time 0+" << TimeSpan.count() << " ms" << std::endl;}
        //
        //
        //
        //
        // update LoopInfo stringstream
        pParams->LoopInfoStr = "<NO INFO>";
        pParams->LoopInfo.str("");
        pParams->LoopInfo << "OBS [" << pParams->iObs+1 << "/" << pParams->nObs << "]" << " ";
        pParams->LoopInfo << "LIB [";
        for(int j = 0; j < pParams->iLib.size(); j++) {
            int ndigits3 = (int)(std::ceil(log10((double)(pParams->SDLIBList[j]->YNum)))); if(j>0) {pParams->LoopInfo << "|";}
            pParams->LoopInfo << std::setw(ndigits3) << std::right << pParams->iLib[j]+1 << "/" << std::setw(ndigits3) << std::left << pParams->SDLIBList[j]->YNum;
        } pParams->LoopInfo << "]"; pParams->LoopInfoStr = pParams->LoopInfo.str();
        //
        //
        //
        //
        //
        //
        // print debug info
        if(debug>=1) {
            // print progress
            debug_info.str("");
            debug_info << process_info.str() << ": Processing OBS" << pParams->iObs+1;
            for(int i=0; i<pParams->nLib; i++) {
                debug_info << " LIB" << i+1 << " data block " << pParams->iLib[i]+1 << "/" << pParams->SDLIBList[i]->YNum;
            }
            debug_info << "\n";
            std::cout << debug_info.str() << std::flush;
        }
        //
        //
        //
        // get LIB data block
        for(int i=0; i<pParams->nLib; i++) {
            if(debug>=2) {std::cout << process_info.str() << ": Reading LIB" << i+1 << " data block " << pParams->iLib[i]+1 << "/" << pParams->SDLIBList[i]->YNum << ", pParams->SDLIBList[i]->X.data() = 0x" << std::hex << (size_t)(pParams->SDLIBList[i]->X.data()) << std::dec << ", pParams->SDLIBList[i]->Y.data() = 0x" << std::hex << (size_t)(pParams->SDLIBList[i]->Y.data()) << std::dec << std::endl;}
            //pParams->SDLIBList[i]->getDataBlockQuickQuick(pParams->iLib[i] * pParams->SDLIBList[i]->XNum + 1, pParams->SDLIBList[i]->XNum); // replaced by loadDataBlock()
            pParams->SDLIBList[i]->loadDataBlock(pParams->iLib[i], debug-1); // this will update pParams->SDLIBList[i]->FPAR[NPAR][:], pParams->SDLIBList[i]->X[:] and pParams->SDLIBList[i]->Y[:]
            if(debug>=2) {std::cout << process_info.str() << ": Read LIB" << i+1 << " data block " << pParams->iLib[i]+1 << "/" << pParams->SDLIBList[i]->YNum << ", pParams->SDLIBList[i]->X.data() = 0x" << std::hex << (size_t)(pParams->SDLIBList[i]->X.data()) << std::dec << ", pParams->SDLIBList[i]->Y.data() = 0x" << std::hex << (size_t)(pParams->SDLIBList[i]->Y.data()) << std::dec << std::endl;}
        }
        //
        // check constraints again <Added><20160719><dzliu> <20181001><dzliu>
        std::vector<michi2Constraint *> NormalizationConstraints; NormalizationConstraints.clear();
        bool ConstraintOK = true;
        for(int i = 0; i < SubprocessConstraints.size(); i++) { // SubprocessConstraints are from ppPool->OtherConstraints, which are LIB PAR constraints
            if(SubprocessConstraints[i]->ConstraintType == michi2Constraint::EnumConstraintType::NORM) { // we will hand over NORM constraints to MPACK
                SubprocessConstraints[i]->recompile(debug-1); //<20181002><TODO> seems still need to recompile here
                double ConstraintValue = SubprocessConstraints[i]->evaluate();
                NormalizationConstraints.push_back(SubprocessConstraints[i]);
            } else { // check other constraints, e.g., on LIB PARs
                SubprocessConstraints[i]->recompile(debug-1); //<20181002><TODO> seems still need to recompile here
                double ConstraintValue = SubprocessConstraints[i]->evaluate();
                if(ConstraintOK) {ConstraintOK = (ConstraintValue>0);}
                if(debug>=3) {
                    debug_info.str("");
                    debug_info << process_info.str() << ": Checking SubprocessConstraints["<<i<<"]: \"" << SubprocessConstraints[i]->ConstraintStr << "\" ... " << ConstraintValue;
                }
                if(debug>=4) {
                    for(int ick=0; ick<SubprocessConstraints[i]->Variables.size(); ick++) {
                        debug_info << " ... " << SubprocessConstraints[i]->Variables[ick] << " = " << SubprocessConstraints[i]->VariablesValueStrs[ick];
                    }
                }
                if(debug>=3) {
                    debug_info << "\n";
                    std::cout << debug_info.str() << std::flush;
                }
            }
            //if(!ConstraintOK) {break;}
        }
        if(!ConstraintOK) {
            //
            // did not pass the constraint check, skip this model
            if(debug>=3) {std::cout << process_info.str() << ": Did not pass the constraint check! Skip!" << std::endl;}
            Timer2 = std::chrono::high_resolution_clock::now(); TimeSpan = Timer2 - Timer1;
            if(debug>=1) {std::cout << process_info.str() << ": Time spent in this loop is " << TimeSpan.count() << " ms" << std::endl;}
            //
            // remove current pParams->iPool from ppPool->IdPool
            bool iremoveok = false;
            pthread_mutex_lock(&mnchi2parallelMutex); // lock mutex for calling ppPool->removeIdInPool() to remove pParams->iPool from ppPool->IdPool
            if(debug>=2) {std::cout << process_info.str() << ": pthread mutex locked for removing iPool which did not pass the constrains from Pool!" << std::endl;}
            iremoveok = ppPool->removeIdInPool(pParams->iPool);  // remove pParams->iPool from ppPool->IdPool due to failed passing user constraints // note that ppPool->CHISQ4PARAMS[i][j][iLibSubIdList[j]] now is still 999999.9
            pthread_mutex_unlock(&mnchi2parallelMutex); // unlock mutex
            if(debug>=2) {std::cout << process_info.str() << ": pthread mutex unlocked!" << std::endl;}
            //
            // check
            if(iremoveok) {
                if(debug>=3) {std::cout << process_info.str() << ": Erased IdPool " << pParams->iPool << " ppPool->IdPool.size() " << ppPool->IdPool.size() << std::endl;}
                //if(debug>=3) {std::cout << process_info.str() << ": Erased IdPool " << pParams->iPool << " michi2FindClosestValue(ppPool->IdPool, pParams->iPool) " << michi2FindClosestValue(ppPool->IdPool, pParams->iPool) << std::endl;}
            } else {
                std::cout << process_info.str() << ": Error! Failed to remove iPool " << pParams->iPool << " from ppPool->IdPool!" << std::endl;
                exit (EXIT_FAILURE);
            }
            //
            // continue
            continue;
        } else {
            if(debug>=3) {std::cout << process_info.str() << ": Passed the constraint check!" << std::endl;}
            if(debug>=1) {Timer2 = std::chrono::high_resolution_clock::now(); TimeSpan = Timer2 - Timer1; std::cout << process_info.str() << ": Time spent so far is " << TimeSpan.count() << " ms" << std::endl;}
        }
        //
        //
        //
        // prepare MinPack input data structure
        std::vector<std::vector<double> > fLIB; fLIB.clear();
        std::vector<double> fOBS;
        std::vector<double> eOBS;
        std::vector<double> aFIT; aFIT.resize(pParams->nLib);
        //
        // match LIB to OBS data block
        // match LIB to OBS data block <updated><20170930> apply filter curves to fLIB at each wOBS, if the running mode is SED.
        if(debug>=2) {std::cout << process_info.str() << ": Matching LIBs to OBS." << std::endl;}
        for(int i=0; i<pParams->nLib; i++) {
            if(debug>=3) {std::cout << process_info.str() << ": Matching LIB" << i+1 << " to OBS" << pParams->iObs+1 << std::endl;}
            MatchedObsLibStruct SDDAT = michi2MatchObs(pParams->SDOBSList[pParams->iObs],
                                                       pParams->SDLIBList[i],
                                                       debug-1);
                                                       // set debug>=3 to debug this.
            if(debug>=3) {std::cout << process_info.str() << ": Matched LIB" << i+1 << " to OBS" << pParams->iObs+1 << std::endl;}
            // check data block
            if(SDDAT.f0.size()>0) {
                // min chi2 by n components
                fLIB.push_back(SDDAT.f1);
                if(i == (pParams->nLib-1)) {
                    fOBS = SDDAT.f0; eOBS = SDDAT.df;
                }
            } else {
                std::cout << process_info.str() << ": Error! Failed to read data block from LIB" << i+1 << " \"" << pParams->SDLIBList[i]->FilePath << "\"" << std::endl;
                return(NULL);
            }
        }
        //
        Timer2 = std::chrono::high_resolution_clock::now(); TimeSpan = Timer2 - Timer1;
        if(debug>=1) {std::cout << process_info.str() << ": Start MPACK at time 0+" << TimeSpan.count() << " ms" << std::endl;}
        //
        // do MinPack
        //
        michi2MinPack *MPACK = new michi2MinPack(fLIB, fOBS, eOBS, NormalizationConstraints, debug-3);
        if(debug>=3) {std::cout << process_info.str() << ": Initializing MPACK." << std::endl;}
        //
        // call MPACK->fit()
        if(debug>=3) {std::cout << process_info.str() << ": Calling MPACK->fit()" << std::endl;}
        int t0 = std::time(NULL);
        MPACK->fit();
        int t1 = std::time(NULL);
        if(debug>=3) {std::cout << process_info.str() << ": Finished MPACK->fit() in " << t1 - t0 << " seconds" << std::endl;}
        //
        // check parameter and write output results to stream
        double chi2 = 0.0; for(int j=0;j<MPACK->chi2.size();j++) {chi2+=MPACK->chi2[j];}
        std::stringstream pStream;
        std::stringstream pHeader;
        pStream << std::setw(10+1) << pParams->iPool << std::setw(15) << chi2;
        pHeader << std::setw(10+1) << "IdPool" << std::setw(15) << "chi2";
        for(int i=0; i<pParams->nLib; i++) {
            aFIT[i] = MPACK->aCOE[i];
            pStream << std::setw(10) << pParams->iLib[i] * pParams->SDLIBList[i]->XNum << std::setw(15) << aFIT.at(i);
            pHeader << std::setw(10) << "i"+std::to_string(i+1) << std::setw(15) << "a"+std::to_string(i+1);
            pParams->SDLIBList[i]->YNorm = aFIT[i];
        }
        for(int i=0; i<pParams->nLib; i++) {
            for(int iLibPar=0; iLibPar<pParams->SDLIBList[i]->FPAR.size(); iLibPar++) {
                pStream << std::setw(15) << pParams->SDLIBList[i]->FPAR[iLibPar][0];
                pHeader << std::setw(15) << pParams->SDLIBList[i]->TPAR[iLibPar];
            }
        }
        delete MPACK; MPACK = NULL;
        pStream << std::endl;
        pHeader << std::endl;
        //pStrings[pParams->iObs] += pStream.str();
        pStrings[pParams->iObs] = pStream.str();
        //
        Timer2 = std::chrono::high_resolution_clock::now(); TimeSpan = Timer2 - Timer1;
        if(debug>=1) {std::cout << process_info.str() << ": Done MPACK at time 0+" << TimeSpan.count() << " ms" << std::endl;}
        if(debug>=2) {std::cout << process_info.str() << ": " << pHeader.str() << std::flush;}
        if(debug>=2) {std::cout << process_info.str() << ": " << pStream.str() << std::flush;}
        
        
        
        //
        // write to chisq table file and update ppPool->CHISQ4PARAMS
        pthread_mutex_lock(&mnchi2parallelMutex); // lock mutex for updating ppPool->CHISQ4PARAMS
        if(debug>=2) {std::cout << process_info.str() << ": pthread mutex locked for updating chisq array, removing iPool from Pool and writing to file!" << std::endl;}
        if(debug>=1) {std::cout << process_info.str() << ": Now writing to file!" << std::endl;}
        for(int i = pParams->iObs; i <= pParams->iObs; i++) { // for i = iObs only
            if(!pStrings[i].empty()) {
                *(ppPool->SDOUTList[i]) << pStrings[pParams->iObs];
                ppPool->SDOUTList[i]->flush();
            }
            if(debug>=1) {std::cout << process_info.str() << ": Now updating chisq array!" << std::endl;}
            for(int j = 0; j < pParams->nLib; j++) {
                std::vector<int> iLibSubIdList = pParams->SDLIBList[j]->convertIdToSubIdList(pParams->iLib[j]);
                for(int k = 0; k < pParams->SDLIBList[j]->NPAR.size(); k++) {
                    //for(int m=0; m < pParams->SDLIBList[j]->NPAR[k]; m++) {}
                    if(std::isnan(ppPool->CHISQ4PARAMS[i][j][k][iLibSubIdList[k]])) {
                        ppPool->CHISQ4PARAMS[i][j][k][iLibSubIdList[k]] = chi2;
                    } else if(ppPool->CHISQ4PARAMS[i][j][k][iLibSubIdList[k]] > chi2) {
                        ppPool->CHISQ4PARAMS[i][j][k][iLibSubIdList[k]] = chi2; // this will update every j LIB and k PAR.
                    }
                }
            }
            if(debug>=1) {std::cout << process_info.str() << ": Now updated chisq array!" << std::endl;}
        }
        ppPool->removeIdInPool(pParams->iPool); // remove pParams->iPool from ppPool->IdPool after finished this loop.
        if(debug>=1) {std::cout << process_info.str() << ": Finished pParams->iPool " << pParams->iPool << ", ppPool->IdPool.size() " << ppPool->IdPool.size() << std::endl;}
        ppPool->IdDone.push_back(pParams->iPool); // add to iLibDone
        ppPool->SubIdListDone.push_back(ppPool->convertIdToSubIdList(pParams->iPool));
        pthread_mutex_unlock(&mnchi2parallelMutex); // unlock mutex
        if(debug>=2) {std::cout << process_info.str() << ": pthread mutex unlocked!" << std::endl;}
        //
        Timer2 = std::chrono::high_resolution_clock::now(); TimeSpan = Timer2 - Timer1;
        if(debug>=1) {std::cout << process_info.str() << ": Finished IdPool at time 0+" << TimeSpan.count() << " ms" << std::endl << std::endl;}
        if(debug>=2) {process_delay += 2*debug;}
        if(debug>=1) {std::cout << process_info.str() << ": Sleeping for " << process_delay << " seconds." << std::endl << std::endl << std::endl; sleep(process_delay);}
        
    }
    //
    // return
    return(NULL);
}
























