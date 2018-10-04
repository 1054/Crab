#ifndef H_michi2
#define H_michi2

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>    // std::transform, std::find
//#include <math.h>
#include <cstdlib>      // std::rand
#include <cmath>        // std::nan, std::isnan
#include <ctime>        // std::time
#include <random>       // std::normal_distribution
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <chrono>       // std::chrono
#include <iterator>     // std::distance
#include <iostream>     // std::cout, std::endl
#include <iomanip>      // std::setw, std::setprecision
#include <sstream>      // std::istringstream
#include <fstream>      // std::ifstream, std::ofstream
#include <regex>        // std::regex
#include "michi2_DataClass.h"
#include "michi2_MinPack.h"
#include "michi2_Constraint.h"
#include "michi2_ParallelPool.h"
//#include "spline.cpp"
//#include "integrate.cpp"
//#include "currentdatetime.cpp"

using namespace std;



//extern const char *InfoRedshift;

extern int NumbParallel; // number of parallel subprocesses

extern double Sampling;

//extern double GlobalMinimumChisq; // GlobalMinimumChisq




//extern const std::string currentDateTime();
//extern vector<double> spline(vector<double> &x, vector<double> &y, vector<double> &output_x);
//extern double integrate(vector<double> &x, vector<double> &y, vector<double> &xrange);
//extern double integrate_LIR(vector<double> &x, vector<double> &y, vector<double> &xrange);



//extern std::vector<michi2Constraint *> Constraints;

typedef struct {std::vector<double> X; std::vector<double> Y; std::string Name;} FilterCurveXY;

extern std::vector<FilterCurveXY *> FilterCurves;



typedef struct {std::vector<double> j0; std::vector<double> f0; std::vector<double> df; std::vector<double> f1;} MatchedObsLibStruct;

MatchedObsLibStruct michi2MatchObs(michi2DataClass *DCOBS, michi2DataClass *DCLIB, int debug = 0);



double michi2GetChiSquare(std::vector<double> f1, std::vector<double> f0, std::vector<double> df, double *a1);

double michi2GetReducedChiSquare(std::vector<double> f1, std::vector<double> f0, std::vector<double> df, double *a1);

double michi2VecMean(std::vector<double> vec);

int michi2RandomIndexNearbyMinimumPoint(std::vector<double> vec, double RandomRadius = 3.0);

int michi2FindIndexOfMinimumValue(std::vector<double> vec);

long michi2FindIndexOfClosestValue(std::vector<long> vec, long input_value);


//extern long mnchi2parallelProgress;

void mnchi2(std::vector<std::string> InputObsList,
            std::vector<std::string> InputLibList,
            std::vector<std::string> OutputTableList,
            std::vector<double> InputRedshiftList,
            std::vector<std::string> InputFilterCurveList,
            std::vector<std::string> InputConstraintList,
            int DebugLevel = 0);

void *mnchi2parallel(void *params);









#endif
