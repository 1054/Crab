#include "michi2_Constraint.h"
#include "integrate.cpp"



michi2Constraint::michi2Constraint() {
    this->clear();
};



michi2Constraint::michi2Constraint(std::string input_arg) {
    this->clear();
    int parse_ok = this->parse(input_arg, NULL, std::vector<michi2DataClass *>());
    if(parse_ok<0) { exit (EXIT_FAILURE); } // 2018-01-10 exit if failed to understand the constraint
};



michi2Constraint::michi2Constraint(std::vector<std::string> input_arg) {
    this->clear();
    int parse_ok = this->parse(input_arg);
    if(parse_ok<0) { exit (EXIT_FAILURE); } // 2018-01-10 exit if failed to understand the constraint
};



michi2Constraint::michi2Constraint(const char *input_arg1, const char *input_arg2, const char *input_arg3, const char *input_arg4, const char *input_arg5) {
    this->clear();
    std::vector<std::string> input_args;
    input_args.push_back(input_arg1);
    input_args.push_back(input_arg2);
    input_args.push_back(input_arg3);
    input_args.push_back(input_arg4);
    input_args.push_back(input_arg5);
    int parse_ok = this->parse(input_args);
    if(parse_ok<0) { exit (EXIT_FAILURE); } // 2018-01-10 exit if failed to understand the constraint
};



michi2Constraint::~michi2Constraint() {
    this->clear();
};



void michi2Constraint::clear() {
    fromLIB=0; toLIB=0; // if value>0 then it's LIB id; if =-1 then use full SED.
    fromPAR=0; toPAR=0; // if value>0 then it's PAR id; if =-1 then use integration; if=0 then use LIB index.
    fromFactor=1; toFactor=1; // the factor to multiply to from/to data (obsolete).
    fromMathPower=std::nan(""); toMathPower=std::nan(""); // the factor to the power of the from/to data. Added since 2018-01-10.
    fromMultiplication=std::nan(""); toMultiplication=std::nan(""); // the factor to multiply to from/to data.
    fromAddition=std::nan(""); toAddition=std::nan(""); // the value to add to from/to data (after multiplication).
    fromLowerX=0; toLowerX=0; // the affected X range of from/to data.
    fromUpperX=0; toUpperX=0; // the affected X range of from/to data.
    fromListOfValues.clear(); toListOfValues.clear();
    OperatorType=0; OperatorTypeStr=""; // OperatorType: 0 "="; 1 ">="; -1 "<="; 2 ">"; -2 "<";
    // for example
    // 1 1 -1 2 1 means lib1 par1 <= lib2 par1, which is the case for LVG two component fitting, first component should have colder temperature.
    // 4 -1 0 -1 -1 means lib4(2e5,2.002e5) integration*2.5 = full(8,1000) integration
    // note that the left hand variable is the TO variable
    this->ConstraintStr = "";
    this->ExpressionStr = "";
    this->Variables.clear();
    this->VariablesValueStrs.clear();
    this->VariablesLibIndices.clear();
    this->Assignments.clear();
    this->AssignmentsLibIndices.clear();
    this->ConstraintType = michi2Constraint::EnumConstraintType::UNKNOWN;
    this->SDOBS = NULL;
    this->SDLIBS.clear();
    //this->Expression;
    //this->SymbolTable;
    //this->Parser;
    //this->SymbolList.clear();
};



// Extract double numbers from a given string
// loop to extract all possible double numbers
std::vector<double> michi2Constraint::extractStringDouble(std::string InputStr) {
    double TempDbl = 0.0;
    std::string TempStr = InputStr+" "; // in case the string got empty
    std::string ExctStr = ""; // extracted str
    std::vector<double> OutputDbl;
    while (std::string::npos != TempStr.find_first_of(".+-Ee0123456789")) {
        TempStr = TempStr.substr(TempStr.find_first_of(".+-Ee0123456789"));
        size_t TempPos = TempStr.find_first_not_of(".+-Ee0123456789");
        if(std::string::npos != TempPos) {
            ExctStr = TempStr.substr(0,TempPos);
            TempStr = TempStr.substr(TempPos);
        } else {
            ExctStr = TempStr;
        }
        if(ExctStr!="" && ExctStr!="+" && ExctStr!="-" && ExctStr!=".") {
            //<DEBUG>//
            //std::cout << "michi2Constraint::extractStringDouble() DEBUG: ExctStr = " << ExctStr << std::endl;
            if(0 == ExctStr.find_first_of(".+-0123456789")) {
                //std::cout << "DEBUG: ExctStr = " << ExctStr << std::endl;
                size_t TempPos2 = ExctStr.find_first_not_of(".+-0123456789");
                if(TempPos2 > 0) {
                    ExctStr = ExctStr.substr(0,TempPos2);
                }
                TempDbl = std::stod(ExctStr.c_str());
                OutputDbl.push_back(TempDbl);
            }
            ExctStr="";
        }
    }
    return OutputDbl;
}




/*
 recognize variable from an expression,
 for example, inputting "LIB3.PAR3^2 * 300.0", then we will take "LIB3" "PAR3" as variables.
 see 'exprtk' grammar here: https://github.com/ArashPartow/exprtk/blob/master/readme.txt
 basic types:
 (00) Types: Scalar, Vector, String
 (01) Basic operators: +, -, *, /, %, ^
 (02) Assignment: :=, +=, -=, *=, /=, %=
 (03) Equalities & Inequalities: =, ==, <>, !=, <, <=, >, >=
 (04) Logic operators: and, mand, mor, nand, nor, not, or, shl, shr, xnor, xor, true, false
 (05) Functions: abs, avg, ceil, clamp, equal, erf, erfc, exp, expm1, floor, frac, log, log10, log1p, log2, logn, max, min, mul, ncdf, nequal, root, round, roundn, sgn, sqrt, sum, swap, trunc
 (06) Trigonometry: acos, acosh, asin, asinh, atan, atanh, atan2, cos, cosh, cot, csc, sec, sin, sinc, sinh, tan, tanh, hypot, rad2deg, deg2grad, deg2rad, grad2deg
 (07) Control structures: if-then-else, ternary conditional, switch-case, return-statement
 (08) Loop statements: while, for, repeat-until, break, continue
 (09) String processing: in, like, ilike, concatenation
 (10) Optimisations: constant-folding, simple strength reduction and dead code elimination
 (11) Calculus: numerical integration and differentiation
 
 EXAMPLE EXPRESSIONS
 (01) sqrt(1 - (3 / x^2))
 (02) clamp(-1, sin(2 * pi * x) + cos(y / 2 * pi), +1)
 (03) sin(2.34e-3 * x)
 (04) if(((x[2] + 2) == 3) and ((y + 5) <= 9),1 + w, 2 / z)
 (05) inrange(-2,m,+2) == if(({-2 <= m} and [m <= +2]),1,0)
 (06) ({1/1}*[1/2]+(1/3))-{1/4}^[1/5]+(1/6)-({1/7}+[1/8]*(1/9))
 (07) a * exp(2.2 / 3.3 * t) + c
 (08) z := x + sin(2.567 * pi / y)
 (09) u := 2.123 * {pi * z} / (w := x + cos(y / pi))
 (10) 2x + 3y + 4z + 5w == 2 * x + 3 * y + 4 * z + 5 * w
 (11) 3(x + y) / 2.9 + 1.234e+12 == 3 * (x + y) / 2.9 + 1.234e+12
 (12) (x + y)3.3 + 1 / 4.5 == [x + y] * 3.3 + 1 / 4.5
 (13) (x + y[i])z + 1.1 / 2.7 == (x + y[i]) * z + 1.1 / 2.7
 (14) (sin(x / pi) cos(2y) + 1) == (sin(x / pi) * cos(2 * y) + 1)
 (15) 75x^17 + 25.1x^5 - 35x^4 - 15.2x^3 + 40x^2 - 15.3x + 1
 (16) (avg(x,y) <= x + y ? x - y : x * y) + 2.345 * pi / x
 (17) while (x <= 100) { x -= 1; }
 (18) x <= 'abc123' and (y in 'AString') or ('1x2y3z' != z)
 (19) ((x + 'abc') like '*123*') or ('a123b' ilike y)
 (20) sgn(+1.2^3.4z / -5.6y) <= {-7.8^9 / -10.11x }
 
 */
std::vector<std::string> michi2Constraint::recognizeVariables(std::string input_expression_str, int debug)
{
    // identify and store the variables in an expression
    std::string TempConstraintStr = input_expression_str;
    std::vector<std::string> OutputVariableList;
    std::regex t_regex("([a-zA-Z_]+[a-zA-Z_0-9]*)");
    if(debug>=1) {std::cout << "michi2Constraint::recognizeVariables() input expression = \"" << input_expression_str << "\"" << std::endl;}
    auto t_els_begin = std::sregex_iterator(TempConstraintStr.begin(), TempConstraintStr.end(), t_regex); // find the pattern repeatedly, see https://stackoverflow.com/questions/25330564/repeating-subpatterns-regex-in-c
    auto t_els_end = std::sregex_iterator();
    for (std::sregex_iterator t_els_iter = t_els_begin; t_els_iter != t_els_end; ++t_els_iter) {
        std::smatch t_match = *t_els_iter;
        if(t_match.size()>1) {
            if(debug>=1) {std::cout << "michi2Constraint::recognizeVariables() recognized variable: " << t_match.str(1) << std::endl;}
            if(std::find(OutputVariableList.begin(), OutputVariableList.end(), t_match.str(1)) == OutputVariableList.end()) {
                OutputVariableList.push_back(t_match.str(1)); // store the variable, but note to store only unique variables
            }
        }
    }
    //
    //<TODO>
    // Alternatively we can use
    //   std::vector<std::string> variable_list;
    //   bool collect_ok = exprtk::collect_variables(input_expression_str, variable_list);
    //   if(collect_ok) {this->Variables = variable_list;}
    //
    this->Variables = OutputVariableList;
    this->VariablesValueStrs.clear();
    return OutputVariableList;
}



/*
 Prepare exprtk symbol_table by setting values to each recognized variable
 The values will be get from SDLIBS.
 */
michi2_Constraint_symbol_table_t michi2Constraint::compileSymbolTable(std::string input_expression_str, std::vector<std::string> input_variables, michi2DataClass *SDOBS, std::vector<michi2DataClass *> SDLIBS, int debug)
{
    this->SymbolTable.clear();
    if(debug>=2) {std::cout << "michi2Constraint::compileSymbolTable() input expression: \"" << input_expression_str << "\"" << std::endl;}
    if(debug>=2) {std::cout << "michi2Constraint::compileSymbolTable() input variables: "; for(int i=0; i<input_variables.size(); i++) {std::cout << input_variables[i] << " ";}  std::cout << std::endl;}
    // determine the value of each variable in the equation must have a value
    // each variable in the equation must have a value
    this->Variables.clear();
    this->VariablesValueStrs.clear();
    this->VariablesLibIndices.clear();
    for(int i=0; i<input_variables.size(); i++) {
        std::string var_name = input_variables[i];
        std::string var_value_str;
        //if(debug>=3) {printf("michi2Constraint::compileSymbolTable() strncmp(var_name.c_str(),\"LIB\",3): %d\n", strncmp(var_name.c_str(),"LIB",3));}
        //if(debug>=3) {printf("michi2Constraint::compileSymbolTable() var_name.c_str() = %s\n", var_name.c_str());}
        if(debug>=2) {std::cout << "michi2Constraint::compileSymbolTable() getting value for " << var_name << std::endl;}
        
        //
        // check whether the variable is an assignment (derivative) or not
        bool var_is_an_assignment = false;
        std::regex t_regex_0a("\\b"+var_name+"\\s*:="); // "var_name := ...."
        std::smatch t_match_0a;
        if(std::regex_search(var_name, t_match_0a, t_regex_0a)) {
            var_is_an_assignment = true;
            this->IsAssignment = true;
        }
        //
        // compile var
        bool var_ok = false;
        //
        // if the variable is LIB*
        std::regex t_regex_0("^LIB[0-9]+", std::regex_constants::icase); // "LIB[0-9]+.*"
        std::smatch t_match_0;
        if(std::regex_search(var_name, t_match_0, t_regex_0)) {
            if(!var_ok) {
                std::regex t_regex("^LIB([0-9]+)_*X$", std::regex_constants::icase); // like "LIB4"
                std::smatch t_match;
                bool t_ok = std::regex_search(var_name, t_match, t_regex);
                if(t_ok && t_match.size()>1) {
                    long iLib = std::stoi(t_match.str(1))-1;
                    if(SDLIBS.size() > iLib) {
                        //std::vector<double> *dVar = &(SDLIBS[iLib]->X);
                        // add constant vector to the exprtk symbol_table
                        this->SymbolTable.add_vector(var_name, SDLIBS[iLib]->X); var_ok = true; var_value_str = "std::vector<double>"; // <20181003><BUG><FIXING><DZLIU> the problem is likely that exprtk does not work well with std::vector, we had better pass the double * array with size.
                        //this->SymbolTable.add_vector(var_name, (double *)(&(SDLIBS[iLib]->X[0])), SDLIBS[iLib]->X.size()); var_ok = true; var_value_str = "double["+std::to_string(SDLIBS[iLib]->X.size())+"]"; // <20181003><BUG><FIXING><DZLIU> see above notes.
                        this->Variables.push_back(var_name); this->VariablesValueStrs.push_back(var_value_str); this->VariablesLibIndices.push_back(iLib);
                        //
                        if(this->ConstraintType == michi2Constraint::EnumConstraintType::UNKNOWN) {this->ConstraintType = michi2Constraint::EnumConstraintType::INT;}
                        //
                        // debug
                        if(debug>=2) {
                            std::cout << "michi2Constraint::compileSymbolTable() input SDLIBS["<<iLib<<"] = 0x" << std::hex << (size_t)(SDLIBS[iLib]) << std::dec << std::endl;
                            std::cout << "michi2Constraint::compileSymbolTable() checking registered vector " << var_name;
                            std::cout << " addr 0x" << std::hex << (size_t)(&(SDLIBS[iLib]->X[0])) << std::dec;
                            std::cout << ", size " << SDLIBS[iLib]->X.size();
                            std::cout << ", values:";
                            for(int k=0; k<SDLIBS[iLib]->X.size(); k++) {
                                std::cout << " " << SDLIBS[iLib]->X[k];
                                if(k>10) {break;}
                            }
                            std::cout << std::endl;
                            //
                            std::cout << "michi2Constraint::compileSymbolTable() checking symbol_table.get_vector(" << var_name << ")";
                            michi2_Constraint_vector_holder_t *vec_holder = this->SymbolTable.get_vector(var_name);
                            michi2_Constraint_vector_node_t vec_node = michi2_Constraint_vector_node_t(vec_holder); // see this->getVariableAsStr();
                            std::cout << " addr 0x" << std::hex << (size_t)(&(vec_holder->data()[0])) << std::dec;
                            std::cout << ", size " << vec_node.size();
                            std::cout << ", size " << (*vec_holder).size();
                            std::cout << ", values:";
                            for(int k=0; k<vec_node.size(); k++) {
                                std::cout << " " << vec_holder->data()[k];
                                if(k>10) {break;}
                            }
                            std::cout << std::endl;
                        }
                    } else {
                        std::cout << "michi2Constraint::compileSymbolTable() Error! Input SDLIBS has wrong size!" << std::endl;
                        exit (EXIT_FAILURE);
                    }
                }
            }
            if(!var_ok) {
                std::regex t_regex("^LIB([0-9]+)_*Y$", std::regex_constants::icase); // like "LIB4"
                std::smatch t_match;
                bool t_ok = std::regex_search(var_name, t_match, t_regex);
                if(t_ok && t_match.size()>1) {
                    long iLib = std::stoi(t_match.str(1))-1;
                    if(SDLIBS.size() > iLib) {
                        //std::vector<double> *dVar = &(SDLIBS[iLib]->Y);
                        // add constant vector to the exprtk symbol_table
                        this->SymbolTable.add_vector(var_name, SDLIBS[iLib]->Y); var_ok = true; var_value_str = "std::vector<double>"; // <20181003><BUG><FIXING><DZLIU> the problem is likely that exprtk does not work well with std::vector, we had better pass the double * array with size.
                        //this->SymbolTable.add_vector(var_name, (double *)(&(SDLIBS[iLib]->Y[0])), SDLIBS[iLib]->Y.size()); var_ok = true; var_value_str = "double["+std::to_string(SDLIBS[iLib]->Y.size())+"]"; // <20181003><BUG><FIXING><DZLIU> see above notes.
                        this->Variables.push_back(var_name); this->VariablesValueStrs.push_back(var_value_str); this->VariablesLibIndices.push_back(iLib);
                        //
                        if(this->ConstraintType == michi2Constraint::EnumConstraintType::UNKNOWN) {this->ConstraintType = michi2Constraint::EnumConstraintType::INT;}
                        //
                        // debug
                        if(debug>=2) {
                            std::cout << "michi2Constraint::compileSymbolTable() input SDLIBS["<<iLib<<"] = 0x" << std::hex << (size_t)(SDLIBS[iLib]) << std::dec << std::endl;
                            std::cout << "michi2Constraint::compileSymbolTable() checking registered vector " << var_name;
                            std::cout << " addr 0x" << std::hex << (size_t)(&(SDLIBS[iLib]->Y[0])) << std::dec;
                            std::cout << ", size " << SDLIBS[iLib]->Y.size();
                            std::cout << ", values:";
                            for(int k=0; k<SDLIBS[iLib]->Y.size(); k++) {
                                std::cout << " " << SDLIBS[iLib]->Y[k];
                                if(k>10) {break;}
                            }
                            std::cout << std::endl;
                            //
                            std::cout << "michi2Constraint::compileSymbolTable() checking symbol_table.get_vector(" << var_name << ")";
                            michi2_Constraint_vector_holder_t *vec_holder = this->SymbolTable.get_vector(var_name);
                            michi2_Constraint_vector_node_t vec_node = michi2_Constraint_vector_node_t(vec_holder); // see this->getVariableAsStr();
                            std::cout << " addr 0x" << std::hex << (size_t)(&(vec_holder->data()[0])) << std::dec;
                            std::cout << ", size " << vec_node.size();
                            std::cout << ", size " << (*vec_holder).size();
                            std::cout << ", values:";
                            for(int k=0; k<vec_node.size(); k++) {
                                std::cout << " " << vec_holder->data()[k];
                                if(k>10) {break;}
                            }
                            std::cout << std::endl;
                        }
                    } else {
                        std::cout << "michi2Constraint::compileSymbolTable() Error! Input SDLIBS has wrong size!" << std::endl;
                        exit (EXIT_FAILURE);
                    }
                }
            }
            if(!var_ok) {
                std::regex t_regex("^LIB([0-9]+)_*(NORM|NORMALIZATION)$", std::regex_constants::icase); // like "LIB4_NORM"
                std::smatch t_match;
                bool t_ok = std::regex_search(var_name, t_match, t_regex);
                if(t_ok && t_match.size()>1) {
                    long iLib = std::stoi(t_match.str(1))-1;
                    if(SDLIBS.size() > iLib) {
                        // add constant variable to the exprtk symbol_table
                        //double dVar = SDLIBS[iLib]->YNorm;
                        //this->SymbolTable.add_constant(var_name, dVar); var_ok = true; var_value_str = std::to_string(dVar);
                        //
                        // add variable to the exprtk symbol_table
                        this->SymbolTable.add_variable(var_name, SDLIBS[iLib]->YNorm); var_ok = true; var_value_str = std::to_string(SDLIBS[iLib]->YNorm);
                        this->Variables.push_back(var_name); this->VariablesValueStrs.push_back(var_value_str); this->VariablesLibIndices.push_back(iLib);
                        //
                        if(this->ConstraintType == michi2Constraint::EnumConstraintType::UNKNOWN) {this->ConstraintType = michi2Constraint::EnumConstraintType::NORM;}
                    } else {
                        std::cout << "michi2Constraint::compileSymbolTable() Error! Input SDLIBS has wrong size!" << std::endl;
                        exit (EXIT_FAILURE);
                    }
                }
            }
            if(!var_ok) {
                std::regex t_regex("^LIB([0-9]+)_*(I|INDEX)$", std::regex_constants::icase); // like "LIB4_INDEX"
                std::smatch t_match;
                bool t_ok = std::regex_search(var_name, t_match, t_regex);
                if(t_ok && t_match.size()>1) {
                    long iLib = std::stoi(t_match.str(1))-1;
                    if(SDLIBS.size() > iLib) {
                        if(SDLIBS[iLib]->CurrentDataBlockIndex >= 0) {
                            // add constant variable to the exprtk symbol_table
                            //long dVar = SDLIBS[iLib]->CurrentDataBlockIndex;
                            //this->SymbolTable.add_constant(var_name, dVar); var_ok = true; var_value_str = std::to_string(dVar);
                            //
                            //add variable to the exprtk symbol_table
                            this->SymbolTable.add_variable(var_name, SDLIBS[iLib]->CurrentDataBlockIndexAsDouble); var_ok = true; var_value_str = std::to_string(SDLIBS[iLib]->CurrentDataBlockIndex);
                            //<20181002><TEST>this->TEST_LIB_INDEX = SDLIBS[iLib]->CurrentDataBlockIndexAsDouble;
                            //<20181002><TEST>this->SymbolTable.add_variable(var_name, this->TEST_LIB_INDEX); var_ok = true; var_value_str = std::to_string(SDLIBS[iLib]->CurrentDataBlockIndex);
                            this->Variables.push_back(var_name); this->VariablesValueStrs.push_back(var_value_str); this->VariablesLibIndices.push_back(iLib);
                            //
                            if(this->ConstraintType == michi2Constraint::EnumConstraintType::UNKNOWN) {this->ConstraintType = michi2Constraint::EnumConstraintType::INDEX;}
                        } else {
                            std::cout << "michi2Constraint::compileSymbolTable() Error! LIB" << iLib+1 << " has not been loaded yet! No model selected!" << std::endl;
                            exit (EXIT_FAILURE);
                        }
                    } else {
                        std::cout << "michi2Constraint::compileSymbolTable() Error! Input SDLIBS has wrong size!" << std::endl;
                        exit (EXIT_FAILURE);
                    }
                }
            }
            if(!var_ok) {
                std::regex t_regex("^LIB([0-9]+)_*PAR([0-9]+)$", std::regex_constants::icase); // like "LIB4_PAR3"
                std::smatch t_match;
                bool t_ok = std::regex_search(var_name, t_match, t_regex);
                if(t_ok && t_match.size()>2) {
                    long iLib = std::stoi(t_match.str(1))-1; // iLib index starts from 0 while LIB(Number) starts from 1.
                    long iPar = std::stoi(t_match.str(2))-1; // iPar index starts from 0 while PAR(Number) starts from 1.
                    if(SDLIBS.size() > iLib) {
                        if(SDLIBS[iLib]->CurrentDataBlockIndex >= 0) {
                            if(SDLIBS[iLib]->FPAR[iPar].size() <= 0) {
                                // if the data block has not been loaded yet, load it
                                SDLIBS[iLib]->loadDataBlock(SDLIBS[iLib]->CurrentDataBlockIndex); // this will update SDLIBS[iLib]->CurrentParameterValues[iPar] (which is the first value of the iPar-th parameter in the iLib-th LIB) and SDLIBS[iLib]->FPAR[iPar] (which is a vector of values with the size of X array size for the iPar-th parameter in the iLib-th LIB).
                            }
                            // add constant variable to the exprtk symbol_table
                            //double dVar = SDLIBS[iLib]->FPAR[iPar][0];
                            //this->SymbolTable.add_constant(var_name, dVar); var_ok = true; var_value_str = std::to_string(dVar);
                            //
                            // add variable to the exprtk symbol_table
                            //<20181002><dzliu>this->SymbolTable.add_variable(var_name, SDLIBS[iLib]->FPAR[iPar][0]); var_ok = true; var_value_str = std::to_string(SDLIBS[iLib]->FPAR[iPar][0]);
                            this->SymbolTable.add_variable(var_name, SDLIBS[iLib]->CurrentParameterValues[iPar]); var_ok = true; var_value_str = std::to_string(SDLIBS[iLib]->CurrentParameterValues[iPar]);
                            this->Variables.push_back(var_name); this->VariablesValueStrs.push_back(var_value_str); this->VariablesLibIndices.push_back(iLib);
                            //
                            if(this->ConstraintType == michi2Constraint::EnumConstraintType::UNKNOWN) {this->ConstraintType = michi2Constraint::EnumConstraintType::PAR;}
                        } else {
                            std::cout << "michi2Constraint::compileSymbolTable() Error! LIB" << iLib+1 << " PAR" << iPar+1 << " has not been loaded yet! No model selected!" << std::endl;
                            exit (EXIT_FAILURE);
                        }
                    } else {
                        std::cout << "michi2Constraint::compileSymbolTable() Error! Input SDLIBS has wrong size!" << std::endl;
                        exit (EXIT_FAILURE);
                    }
                }
            }
            /*
             replaced by direct function call in the expression
             e.g., SED(LIB3_X,LIB3_Y)
             e.g., SED(LIB3_X,LIB3_Y,8,1000)
             
            if(!var_ok) {
                std::regex t_regex("^LIB([0-9]+)_*(INT|INTEGRATION|SED)$", std::regex_constants::icase); // like "LIB4_INT"
                std::smatch t_match;
                bool t_ok = std::regex_search(var_name, t_match, t_regex);
                if(t_ok && t_match.size()>2) {
                    long iLib = std::stoi(t_match.str(1))-1;
                    if(SDLIBS.size() > iLib) {
                        auto iXMinMax = std::minmax_element(SDLIBS[iLib]->X.begin(), SDLIBS[iLib]->X.end());
                        vector<double> iXRange; iXRange.push_back(*(iXMinMax.first)); iXRange.push_back(*(iXMinMax.second)); // see https://en.cppreference.com/w/cpp/algorithm/minmax_element , https://stackoverflow.com/questions/9874802/how-can-i-get-the-max-or-min-value-in-a-vector
                        double dVar = integrate(SDLIBS[iLib]->X, SDLIBS[iLib]->Y, iXRange);
                        // add constant variable to the exprtk symbol_table
                        this->SymbolTable.add_constant(var_name, dVar); var_ok = true; var_value_str = std::to_string(dVar);
                        this->Variables.push_back(var_name); this->VariablesValueStrs.push_back(var_value_str); this->VariablesLibIndices.push_back(iLib);
                    } else {
                        std::cout << "michi2Constraint::compileSymbolTable() Error! Input SDLIBS has wrong size!" << std::endl;
                        exit (EXIT_FAILURE);
                    }
                }
            }
            if(!var_ok) {
                std::regex t_regex("^LIB([0-9]+)_*(INT|INTEGRATION|SED)_+([0-9.eE]+)_+([0-9.eE]+)$", std::regex_constants::icase); // like "LIB4_INT_8_1000"
                std::smatch t_match;
                bool t_ok = std::regex_search(var_name, t_match, t_regex);
                if(t_ok && t_match.size()>4) {
                    long iLib = std::stoi(t_match.str(1))-1;
                    double iX1 = std::stod(t_match.str(3));
                    double iX2 = std::stod(t_match.str(4));
                    if(SDLIBS.size() > iLib) {
                        std::vector<double> iXRange; iXRange.push_back(iX1); iXRange.push_back(iX2);
                        double dVar = integrate(SDLIBS[iLib]->X, SDLIBS[iLib]->Y, iXRange);
                        // add constant variable to the exprtk symbol_table
                        this->SymbolTable.add_constant(var_name, dVar); var_ok = true; var_value_str = std::to_string(dVar);
                        this->Variables.push_back(var_name); this->VariablesValueStrs.push_back(var_value_str); this->VariablesLibIndices.push_back(iLib);
                    } else {
                        std::cout << "michi2Constraint::compileSymbolTable() Error! Input SDLIBS has wrong size!" << std::endl;
                        exit (EXIT_FAILURE);
                    }
                }
            }
             */
        }
        else {
            if(!var_ok) {
                std::regex t_regex("^z$", std::regex_constants::icase); // like "LIB4"
                std::smatch t_match;
                bool t_ok = std::regex_search(var_name, t_match, t_regex);
                if(t_ok && t_match.size()>1) {
                    if(SDOBS) {
                        // add vector to the exprtk symbol_table
                        this->SymbolTable.add_variable(var_name, SDOBS->XNorm); var_ok = true; var_value_str = std::to_string(SDOBS->XNorm);
                        this->Variables.push_back(var_name); this->VariablesValueStrs.push_back(var_value_str); this->VariablesLibIndices.push_back(-2); // <TODO> -2 for OBS
                        //
                        if(this->ConstraintType == michi2Constraint::EnumConstraintType::UNKNOWN) {this->ConstraintType = michi2Constraint::EnumConstraintType::REDSHIFT;}
                    }
                }
            }
        }
        //
        // check if we parsed the variable correctly or not
        if(!var_ok) {
            if(debug>=1) {std::cout << "michi2Constraint::compileSymbolTable() unknown variable " << var_name << std::endl;}
        } else {
            if(debug>=1) {std::cout << "michi2Constraint::compileSymbolTable() compiled variable " << var_name << " = " << var_value_str << std::endl;}
        }
    }
    //
    // add all constants
    this->SymbolTable.add_constants();
    //
    // store
    if(SDLIBS.size()>0) {
        this->SDLIBS = SDLIBS;
    }
    if(SDOBS) {
        this->SDOBS = SDOBS;
    }
    //
    // return
    return this->SymbolTable;
}


void michi2Constraint::recompile(int debug)
{
    this->compileSymbolTable(this->ConstraintStr, this->Variables, this->SDOBS, this->SDLIBS, debug);
    this->solveExpression(this->ConstraintStr, this->SymbolTable, debug);
}



/*
 Set exprtk expression
 */
void michi2Constraint::setExpression(std::string input_expression_str, int debug)
{
    // check whether the expression conatins "=", if yes, change it to exprtk style ":="
    std::string TempConstraintStr = input_expression_str;
    TempConstraintStr = std::regex_replace(TempConstraintStr, std::regex("\\b(LIB[0-9]+)[\\s\\.]+(PAR[0-9]+|I|INDEX|NORM|NORMALIZATION|X|Y)\\b", std::regex_constants::icase), "$1_$2"); // compatible issue. this is mainly for backward-compatiblity with michi2_v04 five-element constraint.
    TempConstraintStr = std::regex_replace(TempConstraintStr, std::regex("\\b(LIB[0-9]+)\\b", std::regex_constants::icase), "($1_NORM * SED($1_X,$1_Y))"); // compatible issue. this is mainly for backward-compatiblity with michi2_v04 five-element constraint.
    TempConstraintStr = std::regex_replace(TempConstraintStr, std::regex("\\b(VALUE)\\b", std::regex_constants::icase), ""); // compatible issue. this is mainly for backward-compatiblity with michi2_v04 five-element constraint.
    TempConstraintStr = std::regex_replace(TempConstraintStr, std::regex("([\\w\\d])\\s*(=)\\s*", std::regex_constants::icase), "$1 := ");
    TempConstraintStr = std::regex_replace(TempConstraintStr, std::regex("\\s*\\b(GE)\\b\\s*", std::regex_constants::icase), " >= ");
    TempConstraintStr = std::regex_replace(TempConstraintStr, std::regex("\\s*\\b(GT)\\b\\s*", std::regex_constants::icase), " > ");
    TempConstraintStr = std::regex_replace(TempConstraintStr, std::regex("\\s*\\b(LE)\\b\\s*", std::regex_constants::icase), " <= ");
    TempConstraintStr = std::regex_replace(TempConstraintStr, std::regex("\\s*\\b(LT)\\b\\s*", std::regex_constants::icase), " < ");
    TempConstraintStr = std::regex_replace(TempConstraintStr, std::regex("\\s*\\b(NE)\\b\\s*", std::regex_constants::icase), " != ");
    TempConstraintStr = std::regex_replace(TempConstraintStr, std::regex("\\s*\\b(EQ)\\b\\s*", std::regex_constants::icase), " := ");
    //
    // initialize ConstraintType and others
    this->ConstraintType = michi2Constraint::EnumConstraintType::UNKNOWN;
    this->AssignmentsLibIndices.clear();
    this->VariablesLibIndices.clear();
    //this->AssignmentsLibParIndices.clear();
    this->IsAssignment = false;
    //
    // check INDEX, apply round() function to make it integer.
    // because exprtk can only deal with either all integer or all double. here clearly we need exprtk to use double for other constraints, so INDEX are acually used as double in constraints. so in the constraint string we round double INDEX to integer.
    std::regex t_regex_2("\\b(LIB)([0-9]+)_*(I|INDEX)\\b", std::regex_constants::icase);
    std::smatch t_match_2;
    std::string t_str_2 = "";
    while(std::regex_search(TempConstraintStr, t_match_2, t_regex_2)) {
        std::regex t_regex_3("^\\s*:=", std::regex_constants::icase); // if it is an assignment, then do not apply round()
        std::smatch t_match_3;
        std::string t_str_3 = t_match_2.suffix();
        //std::cout << "michi2Constraint::setExpression() DEBUG: t_match_2.str(0) " << t_match_2.str(0) << std::endl;
        //std::cout << "michi2Constraint::setExpression() DEBUG: t_match_2.str(2) " << t_match_2.str(2) << std::endl;
        //std::cout << "michi2Constraint::setExpression() DEBUG: t_match_2.suffix() " << t_match_2.suffix() << std::endl; // should not edit TempConstraintStr at this moment!
        if(std::regex_search(t_str_3, t_match_3, t_regex_3)) {
            t_str_2 += t_match_2.prefix();
            t_str_2 += t_match_2.str(0); // whole matched str
            //this->IsAssignment = true;
            //this->AssignmentsLibIndices.push_back(std::stoi(t_match_2.str(2))-1); // -1 is because user input LIBN the number N starts from 1, while as an index in SDLIBS or SDLIBList it should -1.
        } else {
            t_str_2 += t_match_2.prefix();
            t_str_2 += std::regex_replace(t_match_2.str(0), t_regex_2, "round($1$2_$3)"); // add a round() function
        }
        this->ConstraintType = michi2Constraint::EnumConstraintType::INDEX; // if this expression is assigning a value to an INDEX, then set this->ConstraintType to INDEX and set this->IsAssignment = true;
        TempConstraintStr = t_match_2.suffix();
    }
    t_str_2 += TempConstraintStr;
    TempConstraintStr = t_str_2; // now we changed all non-assignment (not at the left-hand of :=) LIBX_INDEX --> round(LIBX_INDEX)
    //
    // store into this class
    this->ConstraintStr = TempConstraintStr;
    if(debug>=1) {std::cout << "michi2Constraint::setExpression() ConstraintStr = \"" << TempConstraintStr << "\"" << std::endl;}
}



/*
 Prepare exprtk expression and parser, with the inputs from the above two functions.
 */
michi2_Constraint_expression_t michi2Constraint::solveExpression(std::string input_expression_str, michi2_Constraint_symbol_table_t input_symbol_table, int debug)
{
    this->ExpressionStr.clear();
    //
    // check whether the expression has user defined function SED()
    std::regex t_regex_2(".*\\b(vLv|LIR|SED)\\s*\\(.*\\).*", std::regex_constants::icase);
    std::smatch t_match_2;
    bool t_ok_2 = std::regex_search(input_expression_str, t_match_2, t_regex_2);
    if(t_ok_2 && t_match_2.size()>1) {
        SED<double> *SED_func = new SED<double>();
        input_symbol_table.add_function(t_match_2.str(1), *SED_func);
        if(debug>=1) {std::cout << "michi2Constraint::solveExpression() added function " << t_match_2.str(1) << "()!" << std::endl;}
    }
    //
    // check whether the expression has user defined function INT()
    std::regex t_regex_2b(".*\\b(INTEGRATE|INT)\\s*\\(.*\\).*", std::regex_constants::icase);
    std::smatch t_match_2b;
    bool t_ok_2b = std::regex_search(input_expression_str, t_match_2b, t_regex_2b);
    if(t_ok_2b && t_match_2b.size()>1) {
        INT<double> *INT_func = new INT<double>();
        input_symbol_table.add_function(t_match_2b.str(1), *INT_func);
        if(debug>=1) {std::cout << "michi2Constraint::solveExpression() added function " << t_match_2b.str(1) << "()!" << std::endl;}
    }
    //
    // check whether the expression has user defined function test()
    /*
    std::regex t_regex_3("\\b(test)\\s*\\(.*\\)", std::regex_constants::icase);
    std::smatch t_match_3;
    bool t_ok_3 = std::regex_search(input_expression_str, t_match_3, t_regex_3);
    if(t_ok_3 && t_match_3.size()>1) {
        test<double> test_3;
        input_symbol_table.add_function("test", test_3);
        if(debug>=1) {std::cout << "michi2Constraint::solveExpression() added function test()!" << std::endl;}
    }
    */
    //
    // check whether the expression has user defined function test()
    /*
    std::regex t_regex_32("\\b(test2)\\s*\\(.*\\)", std::regex_constants::icase);
    std::smatch t_match_32;
    bool t_ok_32 = std::regex_search(input_expression_str, t_match_32, t_regex_32);
    if(t_ok_32 && t_match_32.size()>1) {
        test2<double> *test_32 = new test2<double>();
        input_symbol_table.add_function("test2", *test_32);
        if(debug>=1) {std::cout << "michi2Constraint::solveExpression() added function test2()!" << std::endl;}
        //
        // 20180930 dzliu found the problem (after several hours of checking) -- when creating user defined functions, we need `new` it!
        // for example, we defined a function named test2(), as written in the header file of this cpp,
        // then if we just create it
        //   test2<double> test_2;
        // and
        //   input_symbol_table.add_function("test2", test_2);
        // then it will be dead if jumping out of if environment or a loop!
        // we must `new` it like
        //   test2<double> *test_32 = new test2<double>();
        //   input_symbol_table.add_function("test2", test_32);
        // !!!
    }
    */
    //
    // register symbol table
    this->Expression.register_symbol_table(input_symbol_table);
    if(debug>=1) {
        std::cout << "michi2Constraint::solveExpression() registered symbol table!" << std::endl;
        if(debug>=2) {
            std::vector<std::string> check_variable_list;
            if (exprtk::collect_variables(input_expression_str, check_variable_list)) {
                std::cout << "michi2Constraint::solveExpression() check_variable_list: ";
                for (std::string var : check_variable_list) {
                    std::transform(var.begin(), var.end(), var.begin(), [](const char& c){return std::toupper(c);} );
                    std::cout << var << " ";
                } std::cout << std::endl;
            }
            std::deque<std::string> check_function_list;
            if (exprtk::collect_functions(input_expression_str, input_symbol_table, check_function_list)) {
                std::cout << "michi2Constraint::solveExpression() check_function_list: ";
                for (std::string var : check_function_list) {
                    std::transform(var.begin(), var.end(), var.begin(), [](const char& c){return std::toupper(c);} );
                    std::cout << var << " ";
                } std::cout << std::endl;
            }
        }
    }
    //
    // setup parser
    //michi2_Constraint_parser_t parser; /* parser.enable_unknown_symbol_resolver(); */ /* see https://github.com/ArashPartow/exprtk */
    this->Parser.dec().collect_assignments() = true; // set to collect only assignments // The assignments will only consist of variable types and as such will not contain symbols denoting functions.
    if (this->Parser.compile(input_expression_str, this->Expression)) {
        // retrieve symbol list, see https://github.com/ArashPartow/exprtk,
        std::deque<michi2_Constraint_symbol_t> symbols;
        this->Parser.dec().assignment_symbols(symbols);
        // collect assgnments (derivatives)
        if(debug>=2) {std::cout << "michi2Constraint::solveExpression() collect_assignments: "; }
        this->Assignments.clear();
        this->AssignmentsLibIndices.clear();
        if(symbols.size()>0) {this->IsAssignment = true;}
        for(int k=0; k<symbols.size(); k++) {
            std::string t_symbol = symbols[k].first;
            std::transform(t_symbol.begin(), t_symbol.end(), t_symbol.begin(), [](const char& c){return std::toupper(c);} );
            this->Assignments.push_back(t_symbol); if(debug>=2) {std::cout << t_symbol << " ";}
            // try to get LIB index
            std::regex t_regex_5a("^LIB([0-9]+)", std::regex_constants::icase); std::smatch t_match_5a;
            bool t_ok_5a = std::regex_search(t_symbol, t_match_5a, t_regex_5a);
            if(t_ok_5a) {
                int icka = std::stoi(t_match_5a.str(1))-1;
                this->AssignmentsLibIndices.push_back(icka);
                if(debug>=2) {std::cout << t_symbol << "(LIB"<<icka+1<<") ";}
            } else {
                this->AssignmentsLibIndices.push_back(-1); // each element of this->Assignments will have a corresponding this->AssignmentsLibIndices
            }
            // check if this assignment is LIB_INDEX
            std::regex t_regex_4a(".*_(I|INDEX)$", std::regex_constants::icase); std::smatch t_match_4a;
            bool t_ok_4a = std::regex_search(t_symbol, t_match_4a, t_regex_4a);
            if(t_ok_4a) { this->ConstraintType = michi2Constraint::EnumConstraintType::INDEX; }
            // check if this assignment is LIB_PAR
            std::regex t_regex_4b(".*_PAR[0-9]+$", std::regex_constants::icase); std::smatch t_match_4b;
            bool t_ok_4b = std::regex_search(t_symbol, t_match_4b, t_regex_4b);
            if(t_ok_4b) { this->ConstraintType = michi2Constraint::EnumConstraintType::PAR; }
            // check if this assignment is LIB_NORM
            std::regex t_regex_4c(".*_(NORM|NORMALIZATION)$", std::regex_constants::icase); std::smatch t_match_4c;
            bool t_ok_4c = std::regex_search(t_symbol, t_match_4c, t_regex_4c);
            if(t_ok_4c) { this->ConstraintType = michi2Constraint::EnumConstraintType::NORM; }
        }
        if(debug>=2) {std::cout << std::endl;}
        if(debug>=1) {std::cout << "michi2Constraint::solveExpression() succeeded!" << std::endl;}
    } else {
        printf("michi2Constraint::solveExpression() Error! Failed to parse the input constraint expression \"%s\"!\n", input_expression_str.c_str());
        printf("michi2Constraint::solveExpression() Error message from exprtk: \"%s\"\n", this->Parser.error().c_str());
        exit (EXIT_FAILURE);
    }
    //
    // store into this class
    this->ExpressionStr = input_expression_str;
    //
    // return
    return this->Expression;
}

void michi2Constraint::resolve(int debug)
{
    // setup parser
    //michi2_Constraint_parser_t parser; /* parser.enable_unknown_symbol_resolver(); */ /* see https://github.com/ArashPartow/exprtk */
    //parser.dec().collect_assignments() = true; // set to collect only assignments // The assignments will only consist of variable types and as such will not contain symbols denoting functions.
    //parser.compile(this->ExpressionStr, this->Expression);
    if(!this->ExpressionStr.empty()) {
        this->Parser.compile(this->ExpressionStr, this->Expression);
        //this->Parser.register_local_vars(this->Expression);
        //this->Parser.register_return_results(this->Expression);
    }
}




void michi2Constraint::printSymbols(std::vector<michi2_Constraint_symbol_t> symbols, int debug)
{
    // retrieve symbol list, see https://github.com/ArashPartow/exprtk,
    if(symbols.size()>0) {
        for (std::size_t i = 0; i < symbols.size(); i++) {
            michi2_Constraint_symbol_t& symbol = symbols[i];
            switch (symbol.second) {
                case michi2_Constraint_parser_t::e_st_variable :
                    if(debug>=1) {std::cout << "michi2Constraint::printSymbols() " << symbol.first << std::endl;}
                    break;
                case michi2_Constraint_parser_t::e_st_vector   :
                    if(debug>=1) {std::cout << "michi2Constraint::printSymbols() " << symbol.first << std::endl;}
                    break;
                case michi2_Constraint_parser_t::e_st_string   :
                    if(debug>=1) {std::cout << "michi2Constraint::printSymbols() " << symbol.first << std::endl;}
                    break;
                case michi2_Constraint_parser_t::e_st_function :
                    if(debug>=1) {std::cout << "michi2Constraint::printSymbols() " << symbol.first << std::endl;}
                    break;
                case michi2_Constraint_parser_t::e_st_unknown :
                    if(debug>=1) {std::cout << "michi2Constraint::printSymbols() " << symbol.first << std::endl;}
                    break;
                case michi2_Constraint_parser_t::e_st_vecelem :
                    if(debug>=1) {std::cout << "michi2Constraint::printSymbols() " << symbol.first << std::endl;}
                    break;
                case michi2_Constraint_parser_t::e_st_local_variable :
                    if(debug>=1) {std::cout << "michi2Constraint::printSymbols() " << symbol.first << std::endl;}
                    break;
                case michi2_Constraint_parser_t::e_st_local_vector :
                    if(debug>=1) {std::cout << "michi2Constraint::printSymbols() " << symbol.first << std::endl;}
                    break;
                case michi2_Constraint_parser_t::e_st_local_string :
                    if(debug>=1) {std::cout << "michi2Constraint::printSymbols() " << symbol.first << std::endl;}
                    break;
                default:
                    break;
            }
        }
    }
    /*
     retrieve symbol list, see https://github.com/ArashPartow/exprtk,
     Note: The 'symbol_t' type is a std::pair comprising of the symbol name
     (std::string) and the associated type of the symbol as denoted by the
     cases in the switch statement.
     */
}
















/*
 This function parses the user input constraints
 a constraint should have five elements, for example,
 *) -constraint LIB3 INDEX EQ LIB4 INDEX
 *) -constraint LIB3 PAR3 GT LIB4 PAR3
 *) -constraint LIB3 NORM EQ EXPR (LIB3.PAR3*LIB3.NORM+LIB4.PAR3*LIB4.NORM)*0.75
 
 but since 20180930, we can also input one single string
 *) -constraint "LIB3_PAR3 > 0.5*LIB4_PAR3"
 *) -constraint "LIB5_NORM == 0.5*(LIB3_PAR3+LIB4_PAR3)"
 
 */
int michi2Constraint::parse(std::vector<std::string> input_args, int verbose)
{
    std::string TempConstraintStr;
    for(int i=0; i<input_args.size(); i++) {
        TempConstraintStr += input_args[i];
        if(i<input_args.size()-1) {TempConstraintStr += " ";}
    }
    return this->parse(TempConstraintStr, NULL, std::vector<michi2DataClass *>(), verbose);
}

int michi2Constraint::parse(std::string input_str, michi2DataClass *SDOBS, std::vector<michi2DataClass *> SDLIBS, int verbose)
{
    this->clear();
    // setExpression
    this->setExpression(input_str, verbose);
    //recognizeVariables
    this->recognizeVariables(this->ConstraintStr, verbose);
    // if SDLIBS is valid
    if(SDLIBS.size()>0) {
        // compileSymbolTable
        this->compileSymbolTable(this->ConstraintStr, this->Variables, SDOBS, SDLIBS, verbose);
        // solveExpression
        this->solveExpression(this->ConstraintStr, this->SymbolTable, verbose);
    }
    return 0;
};

double michi2Constraint::evaluate()
{
    if(!this->ExpressionStr.empty()) {
        return this->Expression.value();
    }
    return nan("");
}



/*
 get variable value as string
 see exprtk.hpp
     inline variable_ptr get_variable(const std::string& variable_name) const
     class variable_node
     inline bool is_variable_node(const expression_node<T>* node)
     class vector_node
 */
std::string michi2Constraint::getVariableValueAsStr(std::string var_name)
{
    std::ostringstream sStream;
    if(this->SymbolTable.is_variable(var_name)) {
        michi2_Constraint_variable_node_t *var_node = this->SymbolTable.get_variable(var_name);
        double var_value = (double)(var_node->value());
        sStream << var_value;
    } else if(this->SymbolTable.is_vector(var_name)) {
        michi2_Constraint_vector_holder_t *vec_holder = this->SymbolTable.get_vector(var_name);
        michi2_Constraint_vector_node_t vec_node = michi2_Constraint_vector_node_t(vec_holder); // by reading \url{http://mrpt.ual.es/reference/devel/classexprtk_1_1details_1_1vector__holder_1_1sequence__vector__impl.html} and other related pages we can find that symbol_table.get_vector() returns a vector_holder_t, and vector_holder_t is a member inside vector_node_t, and vector_node_t has value() and size() functions, but not data(). but we can directly call vector_holder_t::data()[k] to get the element.
        sStream << "double[" << vec_node.size() << "]";
        for(int k=0; k<vec_node.size(); k++) {
            sStream << " " << vec_holder->data()[k];
            if(k>10) {break;} // <TODO> max print number
        }
    } else {
        sStream << "NULL";
    }
    //this->VariablesValueStrs;
    return sStream.str();
}



/*
 get variable value as double
 see exprtk.hpp
 inline variable_ptr get_variable(const std::string& variable_name) const
 class variable_node
 inline bool is_variable_node(const expression_node<T>* node)
 class vector_node
 */
double michi2Constraint::getVariableValue(std::string var_name)
{
    if(this->SymbolTable.is_variable(var_name)) {
        michi2_Constraint_variable_node_t *var_node = this->SymbolTable.get_variable(var_name);
        double var_value = (double)(var_node->value());
        return var_value;
    }
    //this->VariablesValueStrs;
    return nan("");
}

size_t michi2Constraint::getVariableAddress(std::string var_name)
{
    if(this->SymbolTable.is_variable(var_name)) {
        michi2_Constraint_variable_node_t *var_node = this->SymbolTable.get_variable(var_name);
        return (size_t)(var_node);
    } else if(this->SymbolTable.is_vector(var_name)) {
        michi2_Constraint_vector_holder_t *vec_holder = this->SymbolTable.get_vector(var_name);
        return (size_t)(vec_holder->data());
    }
    return -1;
}



/*
 print all variable names and values as string
 see exprtk.hpp
 inline variable_ptr get_variable(const std::string& variable_name) const
 class variable_node
 */
std::string michi2Constraint::printVariablesWithValues(std::string delimeter, std::string prefix)
{
    std::ostringstream sStream;
    for (int i=0; i<this->Variables.size(); i++) {
        std::string var_name = this->Variables[i];
        //std::cout << "michi2Constraint::printVariablesWithValues() var_name" << var_name << std::endl;
        std::string var_value = this->getVariableValueAsStr(var_name);
        sStream << prefix << var_name << " = " << var_value;
        if(i<this->Variables.size()-1) {sStream << delimeter;}
    }
    return sStream.str();
}

std::string michi2Constraint::printVariablesWithAddressesAndValues(std::string delimeter, std::string prefix)
{
    std::ostringstream sStream;
    for (int i=0; i<this->Variables.size(); i++) {
        std::string var_name = this->Variables[i];
        std::string var_value = this->getVariableValueAsStr(var_name);
        sStream << prefix << var_name << " @0x" << std::hex << this->getVariableAddress(var_name) << std::dec << " = " << var_value;
        if(i<this->Variables.size()-1) {sStream << delimeter;}
    }
    return sStream.str();
}











































