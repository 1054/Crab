#ifndef H_michi2Constraint
#define H_michi2Constraint

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <algorithm>    // std::transform, std::minmax_element
#include <string>       // std::string, std::stod
#include <iostream>     // std::cout, std::endl
#include <sstream>      // std::ostringstream
#include <iomanip>      // std::setw
#include <regex>        // std::regex
#include "exprtk/exprtk.hpp"
#include "michi2_DataClass.h"

using namespace std;

// setup exprtk
typedef exprtk::symbol_table<double>  michi2_Constraint_symbol_table_t;
typedef exprtk::expression<double>    michi2_Constraint_expression_t;
typedef exprtk::parser<double>        michi2_Constraint_parser_t;
typedef exprtk::details::variable_node<double> michi2_Constraint_variable_node_t;
typedef exprtk::details::vector_node<double> michi2_Constraint_vector_node_t; // http://www.partow.net/programming/exprtk/doxygen/classexprtk_1_1details_1_1vector__node.html
typedef exprtk::details::vector_holder<double> michi2_Constraint_vector_holder_t; // http://www.partow.net/programming/exprtk/doxygen/classexprtk_1_1details_1_1vector__holder.html
//typedef exprtk::symbol_table<long>  michi2_Constraint_symbol_table_long_int_type;
//typedef exprtk::expression<long>    michi2_Constraint_expression_long_int_type;
//typedef exprtk::parser<long>        michi2_Constraint_parser_long_int_type;
typedef typename michi2_Constraint_parser_t::dependent_entity_collector::symbol_t michi2_Constraint_symbol_t;
//michi2_Constraint_symbol_table_t symbol_table;
//std::vector<double> value_table;

// global variable InfoRedshift
//extern const char *InfoRedshift;

// extern functions
extern double integrate(vector<double> const&x, vector<double> const&y, vector<double> const&xrange);
extern double integrate_LIR(vector<double> const&x, vector<double> const&y, vector<double> const&xrange);



/*
struct ConstraintStructure {
    // OBSOLETE SINCE 20180110
    int fromLIB=0; int toLIB=0; // if value>0 then it's LIB id; if =-1 then use full SED.
    int fromPAR=0; int toPAR=0; // if value>0 then it's PAR id; if =-1 then use integration; if=0 then use LIB index.
    double fromFactor=1; double toFactor=1; // the factor to multiply to from/to data.
    double fromLowerX=0; double toLowerX=0; // the affected X range of from/to data.
    double fromUpperX=0; double toUpperX=0; // the affected X range of from/to data.
    int OperatorType=0; std::string OperatorTypeStr=""; // OperatorType: 0 "="; 1 ">="; -1 "<="; 2 ">"; -2 "<";
    // for example
    // 1 1 -1 2 1 means lib1 par1 <= lib2 par1, which is the case for LVG two component fitting, first component should have colder temperature.
    // 4 -1 0 -1 -1 means lib4(2e5,2.002e5) integration*2.5 = full(8,1000) integration
    // note that the left hand variable is the TO variable
};
*/



class michi2Constraint {
public:
    int fromLIB, toLIB; // if value>0 then it's LIB id; if =-1 then use full SED.
    int fromPAR, toPAR; // if value>0 then it's PAR id; if =-1 then use integration; if=0 then use LIB index.
    std::string fromType, toType; // string
    double fromFactor; double toFactor; // the factor to multiply to from/to data.
    double fromMathPower; double toMathPower; // the factor to multiply to from/to data.
    double fromMultiplication; double toMultiplication; // the factor to multiply to from/to data.
    double fromAddition; double toAddition; // the value to add to from/to data (after multiplication).
    double fromLowerX; double toLowerX; // the affected X range of from/to data.
    double fromUpperX; double toUpperX; // the affected X range of from/to data.
    std::string fromEquation, toEquation; // we can input equation as the contraint
    std::string fromExpression, toExpression; // we can input expression as the contraint
    std::vector<std::string> fromVariable, toVariable; // when inputting equation as the constraint, we need to extract the variables
    std::vector<double> fromListOfValues, toListOfValues; // we can also use a list of values as the constraint
    int OperatorType; std::string OperatorTypeStr; // OperatorType: 0 "="; 1 ">="; -1 "<="; 2 ">"; -2 "<";
    // for example
    // 1 1 -1 2 1 means lib1 par1 <= lib2 par1, which is the case for LVG two component fitting, first component should have colder temperature.
    // 4 -1 0 -1 -1 means lib4(2e5,2.002e5) integration*2.5 = full(8,1000) integration
    // note that the left hand variable is the TO variable
    
    std::string ConstraintStr;
    std::string ExpressionStr;
    michi2_Constraint_expression_t Expression;
    michi2_Constraint_symbol_table_t SymbolTable;
    michi2_Constraint_parser_t Parser;
    //michi2_Constraint_expression_long_int_type Expression_long_int_type;
    //michi2_Constraint_symbol_table_long_int_type SymbolTable_long_int_type;
    //std::deque<michi2_Constraint_symbol_t> SymbolList; // the retrieved symbol list, see https://github.com/ArashPartow/exprtk, [SECTION 16 - EXPRESSION DEPENDENTS]
    //michi2_Constraint_parser_t Parser;
    std::vector<std::string> Variables;
    std::vector<std::string> VariablesValueStrs;
    std::vector<std::string> Assignments;
    std::vector<long> AssignmentsLibIndices;
    std::vector<long> VariablesLibIndices;
    //std::vector<long> AssignmentsLibParIndices;
    
    
    enum class EnumConstraintType {UNKNOWN=1, INDEX=2, NORM=4, PAR=8, INT=16, SED=32, REDSHIFT=64};
    EnumConstraintType ConstraintType;
    bool IsAssignment;
    
    michi2DataClass * SDOBS;
    std::vector<michi2DataClass *> SDLIBS;
    
    //double TEST_LIB_INDEX;
    //std::vector<double> LIB_NORM;
    //std::vector<double> LIB_INDEX;
    //std::vector<std::vector<double> > LIB_PAR;
    
    michi2Constraint();
    
    michi2Constraint(std::string input_arg);
    
    michi2Constraint(std::vector<std::string> input_arg);
    
    michi2Constraint(const char *input_arg1, const char *input_arg2, const char *input_arg3, const char *input_arg4, const char *input_arg5);
    
    ~michi2Constraint();
    
    void clear();
    
    
    // Extract double numbers from a given string
    // loop to extract all possible double numbers
    std::vector<double> extractStringDouble(std::string InputStr);
    
    void setExpression(std::string input_expression, int debug = 0); 
    
    std::vector<std::string> recognizeVariables(std::string input_expression, int debug = 0);
        
    michi2_Constraint_symbol_table_t compileSymbolTable(std::string input_expression, std::vector<std::string> input_variables, michi2DataClass *SDOBS, std::vector<michi2DataClass *> SDLIBS, int debug = 0);
    
    void recompile(int debug = 0);
    
    michi2_Constraint_expression_t solveExpression(std::string input_expression, michi2_Constraint_symbol_table_t input_symbol_table, int debug = 0);
    
    void resolve(int debug = 0);
    
    std::string getVariableValueAsStr(std::string var_name);
    
    double getVariableValue(std::string var_name);
    size_t getVariableAddress(std::string var_name);
    std::string printVariablesWithValues(std::string delimeter = std::string("\n"), std::string prefix = std::string(""));
    std::string printVariablesWithAddressesAndValues(std::string delimeter = std::string("\n"), std::string prefix = std::string(""));
    
    void printSymbols(std::vector<michi2_Constraint_symbol_t> symbols, int debug = 0);
    
    int parse(std::vector<std::string> input_args, int verbose = 0);
        
    int parse(std::string input_str, michi2DataClass *SDOBS = NULL, std::vector<michi2DataClass *> SDLIBS = std::vector<michi2DataClass *>(), int verbose = 0);
    
    double evaluate();
    
};






/*
 exprtk user defined function to compute SED integration vLv etc.
 */
template <typename T>
struct SED
: public exprtk::igeneric_function<T>
{
    typedef typename exprtk::igeneric_function<T>::parameter_list_t parameter_list_t;
    typedef typename exprtk::igeneric_function<T>::generic_type generic_type; // typedef type_store<T> generic_type // type_store -- see -- http://www.partow.net/programming/exprtk/doxygen/structexprtk_1_1type__store.html
    typedef typename generic_type::vector_view vector_t;
    typedef typename generic_type::scalar_view scalar_t;
    
    SED()
    : exprtk::igeneric_function<T>("T|TT|VV|VVTT")
    {}
    // see https://github.com/ArashPartow/exprtk/blob/master/readme.txt,
    // define a parameter sequences VV or VVTT, V is vector, T is scalar, S is string and ? is any type and * is any number of any types.
    
    inline T operator() (const std::size_t& ps_index, parameter_list_t parameters)
    {
        // if "T" then ps_index = 0
        // if "TT" then ps_index = 1
        // if "VV" then ps_index = 2
        // if "VVTT" then ps_index = 3
        int debug = 0;
        if(debug>=1) {std::cout << "SED() operator() parameters.size()=" << parameters.size() << std::endl;}
        std::vector<double> x;
        std::vector<double> y;
        std::vector<double> xrange;
        size_t nx = -1;
        size_t ny = -1;
        if(parameters.size()==1) {
            generic_type& gt1 = parameters[0]; // only one scalar
            if (generic_type::e_scalar == gt1.type) {
                scalar_t t1(gt1);
                double x1 = t1();
                return x1; // return this scalar. this is mainly for backward-compatiblity with michi2_v04 five-element constraint.
            }
        }
        if(parameters.size()>=2) {
            generic_type& gt1 = parameters[0]; // X array
            generic_type& gt2 = parameters[1]; // Y array
            if (generic_type::e_vector == gt1.type &&
                generic_type::e_vector == gt2.type) {
                nx = gt1.size;
                ny = gt2.size;
                //double *vx = (double *)(gt1.data); // vector_t is exprtk::igeneric_function<T>::generic_type::vector_view, which is just type_store<T>. struct type_store has `void* data;` and `std::size_t size;`.
                //double *vy = (double *)(gt2.data); // vector_t is exprtk::igeneric_function<T>::generic_type::vector_view, which is just type_store<T>. struct type_store has `void* data;` and `std::size_t size;`.
                //
                //for(int k=0; k<nx; k++) { vx[k] = *((double *)(gt1.data)+k); x.push_back(vx[k]); }
                //for(int k=0; k<ny; k++) { vy[k] = *((double *)(gt2.data)+k); y.push_back(vy[k]); }
                //
                // from exprtk.hpp, ,
                // we can see that
                //    class generic_function_node : public expression_node<T>
                //        virtual bool init_branches()
                //            ts.data = vi->vds().data(); // ts is type_store, vds is vector_data_store
                // and
                //            vi = dynamic_cast<vector_interface<T>*>(arg_list_[i]))
                //
                vector_t vx(gt1);
                vector_t vy(gt2);
                x.resize(nx);
                y.resize(ny);
                std::vector<double> x2(vx.begin(), vx.begin()+nx); x = x2;
                std::vector<double> y2(vy.begin(), vy.begin()+ny); y = y2;
                //
                if(debug>=1) {
                    std::cout << "SED() operator() checking symbol_table parameter_list[0]";
                    std::cout << " addr 0x" << std::hex << (size_t)(&gt1) << std::dec;
                    std::cout << ", parameter_list[0].data = 0x" << std::hex << (size_t)(gt1.data) << std::dec;
                    std::cout << ", size " << nx;
                    std::cout << ", values:";
                    for(int k=0; k<nx; k++) {
                        if((k>=0 && k<=5) || (k>=20 && k<=25) || (k>=100 && k<=105) || (k>=120 && k<=125) || (k>=220 && k<=225) || (k>=300 && k<=305)) {
                            std::cout << " " << vx[k];
                        }
                    }
                    std::cout << std::endl;
                }
                //
                if(debug>=1) {
                    std::cout << "SED() operator() checking symbol_table parameter_list[1]";
                    std::cout << " addr 0x" << std::hex << (size_t)(&gt2) << std::dec;
                    std::cout << ", parameter_list[1].data = 0x" << std::hex << (size_t)(gt2.data) << std::dec;
                    std::cout << ", size " << ny;
                    std::cout << ", values:";
                    for(int k=0; k<ny; k++) {
                        if((k>=0 && k<=5) || (k>=20 && k<=25) || (k>=100 && k<=105) || (k>=120 && k<=125) || (k>=220 && k<=225) || (k>=300 && k<=305)) {
                            std::cout << " " << vy[k];
                        }
                    }
                    std::cout << std::endl;
                }
                //
                // see "exprtk.hpp", "class vector_view", "inline data_ptr_t data() const", "return data_;".
                //
                // <DEBUG>
                if(debug>=1) {
                    std::cout << "SED() operator() x vector addr 0x" << std::hex << (size_t)(&x[0]) << std::dec << ", size " << x.size() << ", vx array addr 0x" << std::hex << (size_t)(vx.begin()) << std::dec << " size " << nx << std::endl;
                    std::cout << "SED() operator() y vector addr 0x" << std::hex << (size_t)(&y[0]) << std::dec << ", size " << y.size() << ", vy array addr 0x" << std::hex << (size_t)(vy.begin()) << std::dec << " size " << ny << std::endl;
                    std::cout << "SED() operator() " << setw(12) << "x" << " " << setw(12) << "y" << setw(12) << "vx" << " " << setw(12) << "vy" << std::endl;
                    for(int k=0; k<ny; k++) {
                        if((k>=0 && k<=5) || (k>=20 && k<=25) || (k>=100 && k<=105) || (k>=120 && k<=125) || (k>=220 && k<=225) || (k>=300 && k<=305)) {std::cout << "SED() operator() " << setw(12) << x.at(k) << " " << setw(12) << y.at(k) << setw(12) << vx[k] << " " << setw(12) << vy[k] << std::endl;}
                    }
                }
            } else {
                if(debug>=1) {
                    //
                    // <DEBUG> if input two scalar values, then adding them as a test
                    if (generic_type::e_scalar == gt1.type &&
                        generic_type::e_scalar == gt2.type) {
                        //double t1 = *((double *)(gt1.data));
                        //double t2 = *((double *)(gt2.data));
                        scalar_t t1(gt1);
                        scalar_t t2(gt2);
                        double x1 = t1();
                        double x2 = t2();
                        std::cout << "SED() x1 x2 " << x1 << " " << x2 << std::endl;
                        return x1+x2;
                    }
                }
            }
        }
        if(parameters.size()>=4) {
            generic_type& gt3 = parameters[2]; // X range lower boundary
            generic_type& gt4 = parameters[3]; // X range upper boundary
            if (generic_type::e_scalar == gt3.type &&
                generic_type::e_scalar == gt4.type) {
                //double x1 = static_cast<double>(scalar_t(gt3));
                //double x2 = static_cast<double>(scalar_t(gt4));
                //double x1 = *((double *)(gt3.data));
                //double x2 = *((double *)(gt4.data));
                scalar_t t1(gt3);
                scalar_t t2(gt4);
                double x1 = t1();
                double x2 = t2();
                xrange.push_back(x1);
                xrange.push_back(x2);
                if(debug>=1) {std::cout << "SED() integrating over " << x1 << " " << x2 << std::endl;}
            }
        }
        if(x.size()>0 && y.size()==x.size() && xrange.size()==0) {
            xrange.push_back(8.0);
            xrange.push_back(1000.0);
        }
        if(x.size()>0 && y.size()==x.size() && xrange.size()>=2) {
            return integrate_LIR(x, y, xrange);
        }
        
        return nan(""); // T(0);
        
        //
        // and to make it work with user defined function,
        // we need to override explicitly this function with these two arguments.
        // all others will not work.
        // -- dzliu 20180930
    }
    
};





/*
 exprtk user defined function to compute direct integration of Lv.
 */
template <typename T>
struct INT
: public exprtk::igeneric_function<T>
{
    typedef typename exprtk::igeneric_function<T>::parameter_list_t parameter_list_t;
    typedef typename exprtk::igeneric_function<T>::generic_type generic_type;
    typedef typename generic_type::vector_view vector_t;
    typedef typename generic_type::scalar_view scalar_t;
    //
    INT()
    : exprtk::igeneric_function<T>("TT|VV|VVTT")
    {}
    //
    inline T operator()(const std::size_t& ps_index, parameter_list_t parameters)
    {
        int debug = 0;
        if(debug>=1) {std::cout << "INT() operator()" << std::endl;}
        std::vector<double> x;
        std::vector<double> y;
        std::vector<double> xrange;
        if(parameters.size()>=2) {
            generic_type& gt1 = parameters[0]; // X array
            generic_type& gt2 = parameters[1]; // Y array
            if (generic_type::e_vector == gt1.type &&
                generic_type::e_vector == gt2.type) {
                vector_t vx = gt1; vector_t vy = gt2;
                x.resize(vx.size()); y.resize(vy.size());
                for (int k=0; k<vx.size(); k++) { x[k] = vx[k]; y[k] = vy[k]; } // see above
            }
        }
        if(parameters.size()>=4) {
            generic_type& gt3 = parameters[2]; // X range lower boundary
            generic_type& gt4 = parameters[3]; // X range upper boundary
            if (generic_type::e_scalar == gt3.type &&
                generic_type::e_scalar == gt4.type) {
                scalar_t t1(gt3); scalar_t t2(gt4); double x1 = t1(); double x2 = t2(); xrange.push_back(x1); xrange.push_back(x2);
                if(debug>=1) {std::cout << "INT() integrating over " << x1 << " " << x2 << std::endl;}
            }
        }
        if(x.size()>0 && y.size()==x.size() && xrange.size()==0) {
            xrange.push_back(8.0);
            xrange.push_back(1000.0);
        }
        if(x.size()>0 && y.size()==x.size() && xrange.size()>=2) {
            return integrate(x, y, xrange);
        }
        return nan(""); // T(0);
    }
};



/*
 exprtk user defined function to test exprtk user defined function.
 */
template <typename T>
struct test : public exprtk::ifunction<T>
{
    typedef typename exprtk::igeneric_function<T>::parameter_list_t parameter_list_t;
    typedef typename exprtk::igeneric_function<T>::generic_type generic_type;
    typedef typename generic_type::scalar_view scalar_t;
    typedef typename generic_type::vector_view vector_t;
    typedef typename generic_type::string_view string_t;
    
    //using exprtk::igeneric_function<T>::operator();
    
     test()
     : exprtk::ifunction<T>(2)
     {
     }
     
     T operator()(const T& v1, const T& v2)
     {
         return (v1 + v2);
     }
};



/*
 exprtk user defined function to test exprtk user defined function.
 */
template <typename T>
struct test2 : public exprtk::igeneric_function<T>
{
    //typedef exprtk::type_store<T> generic_type;
    //typedef typename generic_type::parameter_list parameter_list_t;
    typedef typename exprtk::igeneric_function<T>::parameter_list_t parameter_list_t;
    typedef typename exprtk::igeneric_function<T>::generic_type generic_type;
    typedef typename generic_type::scalar_view scalar_t;
    typedef typename generic_type::vector_view vector_t;
    typedef typename generic_type::string_view string_t;
    
    //using exprtk::igeneric_function<T>::operator();
    
    test2()
    {}
    // see https://github.com/ArashPartow/exprtk/blob/master/readme.txt,
    // see exprtk_test.cpp
    
    inline T operator() (parameter_list_t parameters)
    {
        generic_type& gt1 = parameters[0]; // X
        generic_type& gt2 = parameters[1]; // Y
        scalar_t x(gt1);
        scalar_t y(gt2);
        //std::cout << "test() " << x() << " " << y() << std::endl;
        //return x()+y();
        // to convert scalar_t to double,
        // we can just
        //    scalar_t x(gt1);
        //    double xvalue = x();
        // this is read from exprtk.hpp, struct type_view, struct scalar_view, inline value_t& operator() (), return v_.
        // -- dzliu 20180930
        return T(x()+y()); // nan(""); // T(0);
    }
    
};







#endif
