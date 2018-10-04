#ifndef H_michi2MinPack
#define H_michi2MinPack

#include <stdio.h>
#include <math.h>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <numeric>
#include <cmath>
#include <regex>
#include <thread>
#include "exprtk/exprtk.hpp"
//#include "CrabTableReadColumn.cpp"
//#include "CrabStringReadColumn.cpp"
//#include "CrabTableReadInfo.cpp"
//#include "CrabTableGetLineCount.cpp"
#include "michi2_DataClass.h"
#include "michi2_Constraint.h"

using namespace std;

// setup exprtk
//typedef exprtk::symbol_table<double> michi2MinPack_symbol_table_t;
//typedef exprtk::expression<double>     michi2MinPack_expression_t;
//typedef exprtk::parser<double>             michi2MinPack_parser_t;



/* global variables */

extern thread_local std::vector<double> michi2MinPack_fOBS;

extern thread_local std::vector<double> michi2MinPack_eOBS;

extern thread_local std::vector<double> michi2MinPack_aCOE;

extern thread_local std::vector<std::vector<double> > michi2MinPack_fLIB;

extern thread_local long michi2MinPack_ncount;

extern thread_local int michi2MinPack_debug;

extern thread_local std::vector<michi2Constraint *> michi2MinPack_Constraints;

/*
struct michi2MinPack_constraint {
    int to=-1; std::vector<int> from;
    std::vector<double> multiplication;
    double addition=0.0;
}; //<Added><20171001> allow to constraint aCOE of LIB, for example, lock LIB1 normalization coefficient = LIB2 normalization coefficient * 100, this is useful in locking radio SED to IR(8-1000um) via the IR-radio correlation.

struct michi2MinPack_constraint_expression {
    int to=-1;
    std::vector<int> from;
    std::string Equation;
    std::vector<std::string> Variable;
    std::vector<double> Value;
}; //<Added><20180114> allow to constraint aCOE of LIB in a more flexible way, for example, lock LIB5 normalization coefficient = (10^LIB3_PAR3+10^LIB4_PAR3)/3750/10^2.4, this is useful in locking radio SED to IR(8-1000um) via the IR-radio correlation.

extern thread_local std::vector<michi2MinPack_constraint *> michi2MinPack_constraints; //<Added><20171001>

extern thread_local std::vector<michi2MinPack_constraint_expression *> michi2MinPack_constraint_expressions; //<Added><20180114>
*/




/* global function */

void michi2MinPack_func(const int *m, const int *n, const double *x, double *fvec, int *iflag);



/* 
   Struct Min Pack
   based on cminpack
*/

class michi2MinPack {
public:
    std::vector<double> fOBS;
    std::vector<double> eOBS;
    std::vector<std::vector<double> > fLIB;
    std::vector<double> aCOE;
    std::vector<double> chi2;
    std::vector<michi2Constraint *> Constraints;
    michi2MinPack(
                  std::vector<std::vector<double> > Input_fLIB,
                  std::vector<double> Input_fOBS,
                  std::vector<double> Input_eOBS,
                  std::vector<michi2Constraint *> NormalizationConstraints,
                  int Input_debug = 0
                  );
    void func(const int *m,
              const int *n,
              const double *x,
              double *fvec,
              int *iflag);
    void init(
              std::vector<std::vector<double> > Input_fLIB,
              std::vector<double> Input_fOBS,
              std::vector<double> Input_eOBS,
              std::vector<michi2Constraint *> NormalizationConstraints,
              int Input_debug = 0
              );
    /*
    void constrain(
                   int toLib,
                   std::vector<int> fromLibs,
                   std::vector<double> multiplication_factors
                   );
    void constrain(
                   int toLib,
                   int fromLib,
                   double multiplication_factor
                   );
    void constrain(
                   int toLib,
                   std::string Equation,
                   std::vector<std::string> Variable,
                   std::vector<double> Value
                   );
     */
    //void func(const int *m, const int *n, const double *x, double *fvec, int *iflag);
    void fit(int Input_debug = 0);
    double mean(std::vector<double> data);
};











#endif
