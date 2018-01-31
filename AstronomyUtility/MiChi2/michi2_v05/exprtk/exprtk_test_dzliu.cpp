#include <stdio.h>
#include <string.h>
#include <vector>
#include <algorithm>    // std::transform
#include <string>
#include <iostream>     // std::cout, std::endl
#include <iomanip>      // std::setw
#include <time.h>
#include <sstream>
#include <numeric>
#include <cmath>
#include <regex>
#include "exprtk.hpp"

using namespace std;



template <typename T>
double calc_test(std::string expression_string) {
    //
    // setup exprtk
    typedef exprtk::symbol_table<T> symbol_table_t;
    typedef exprtk::expression<T>     expression_t;
    typedef exprtk::parser<T>             parser_t;
    symbol_table_t symbol_table;
    //
    //std::string expression_string = "((LIB3*(10^LIB3_PAR3))+(LIB4*(10^LIB4_PAR3)))/3750/(10^2.4)";
    //
    // add variable to the exprtk symbol_table
    T LIB3 = T(0.000000);
    T LIB3_PAR3 = T(3.110672);
    T LIB4 = T(8.916278);
    T LIB4_PAR3 = T(2.071430);
    symbol_table.add_variable("LIB3", LIB3);
    symbol_table.add_variable("LIB3_PAR3", LIB3_PAR3);
    symbol_table.add_variable("LIB4", LIB4);
    symbol_table.add_variable("LIB4_PAR3", LIB4_PAR3);
    //
    // evaluate the constraint expression
    //symbol_table.add_constants();
    expression_t expression;
    expression.register_symbol_table(symbol_table);
    parser_t parser;
    if (!parser.compile(expression_string, expression)){
        printf("michi2MinPack_func() Error! Failed to parse the input constraint equation \"%s\"\n!", expression_string.c_str());
        exit (EXIT_FAILURE);
    }
    //
    // print debug info
    printf("michi2MinPack_func() debugging: value = %f\n", double(expression.value()));
    
    return double(expression.value());
}




int main(int argc, char **argv)
{
    double a = calc_test<double>(argv[1]);
    return 0;
}
