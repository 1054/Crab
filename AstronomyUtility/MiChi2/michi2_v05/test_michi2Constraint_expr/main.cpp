

#include <stdio.h>
#include <string.h>
#include <vector>
#include <algorithm>    // std::transform
#include <string>
#include <iostream>     // std::cout, std::endl
#include <iomanip>      // std::setw
#include <ctime>
#include "../michi2_DataClass.h" // Constraints,
#include "../michi2_Constraint.h" // Constraints,

using namespace std;

const char *InfoRedshift = NULL;

int main(int argc, char **argv)
{
    if(argc>1)
    {
        std::string expression = argv[1];
        
        int debug = 0;
        
        int t0, t1;
        
        michi2DataClass *SDLIB1 = new michi2DataClass("lib.BC03.Padova1994.BaSeL.Z0.0190.EBV.SED", debug);
        michi2DataClass *SDLIB2 = new michi2DataClass("lib.DL07.2010.03.18.spec.2.0.HiExCom.SED", debug);
        michi2DataClass *SDLIB3 = new michi2DataClass("lib.DL07.2010.03.18.spec.2.0.LoExCom.SED", debug);
        SDLIB1->loadDataBlock(10); SDLIB1->YNorm = 1.0;
        SDLIB2->loadDataBlock(1); SDLIB2->YNorm = 1.0;
        SDLIB3->loadDataBlock(30); SDLIB3->YNorm = 1.0;
        
        std::vector<michi2DataClass *> SDLIBS;
        SDLIBS.push_back(SDLIB1);
        SDLIBS.push_back(SDLIB2);
        SDLIBS.push_back(SDLIB3);
        
        t0 = std::time(NULL);
        michi2Constraint *TempConstraint = new michi2Constraint();
        t1 = std::time(NULL); std::cout << "Used " << t1 - t0 << " seconds" << std::endl;
        
        t0 = std::time(NULL);
        TempConstraint->parse(expression, NULL, SDLIBS, debug);
        t1 = std::time(NULL); std::cout << "Used " << t1 - t0 << " seconds" << std::endl;
        
        std::cout << "Value: " << TempConstraint->Expression.value() << std::endl;
        
        for(int ick=0; ick<TempConstraint->Variables.size(); ick++) {
            std::cout << "Checking constraint: " << TempConstraint->ConstraintStr << " ... " << TempConstraint->Variables[ick] << " = " << TempConstraint->VariablesValueStrs[ick] << std::endl;
        }
        
        std::cout << std::endl;
        
        
        
        
        SDLIB2->loadDataBlock(130);
        SDLIB3->loadDataBlock(130);
        //long LIB3_INDEX = 90; // no effect
        //SDLIBS[2]->CurrentDataBlockIndex = 90;
        //SDLIBS[2]->CurrentDataBlockIndexAsDouble = 90.0;
        
        
        //t0 = std::time(NULL);
        //TempConstraint->compileSymbolTable(TempConstraint->ExpressionStr, TempConstraint->Variables, NULL, SDLIBS);
        TempConstraint->recompile();
        //t1 = std::time(NULL); std::cout << "Used " << t1 - t0 << " seconds" << std::endl;
        
        //t0 = std::time(NULL);
        //TempConstraint->solveExpression(TempConstraint->ExpressionStr, TempConstraint->SymbolTable);
        //t1 = std::time(NULL); std::cout << "Used " << t1 - t0 << " seconds" << std::endl;
        
        //t0 = std::time(NULL);
        std::cout << "Value: " << TempConstraint->Expression.value() << std::endl;
        //t1 = std::time(NULL); std::cout << "Used " << t1 - t0 << " seconds" << std::endl;
        
        for(int ick=0; ick<TempConstraint->Variables.size(); ick++) {
            std::cout << "Checking constraint: " << TempConstraint->ConstraintStr << " ... " << TempConstraint->Variables[ick] << " = " << TempConstraint->VariablesValueStrs[ick] << std::endl;
        }
        
        //TempConstraint->printSymbols(debug);
        
        
        
        
        
    }
    
    return 0;
}
            

