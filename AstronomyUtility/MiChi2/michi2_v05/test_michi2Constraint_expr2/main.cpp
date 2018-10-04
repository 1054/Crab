

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
        
        michi2DataClass *SDLIB1 = new michi2DataClass("lib.BC03.Padova1994.BaSeL.Z0.0190.EBV.SED", debug);
        michi2DataClass *SDLIB2 = new michi2DataClass("lib.MullaneyAGN.SED", debug);
        michi2DataClass *SDLIB3 = new michi2DataClass("lib.DL07.2010.03.18.spec.2.0.HiExCom.SED", debug);
        michi2DataClass *SDLIB4 = new michi2DataClass("lib.DL07.2010.03.18.spec.2.0.LoExCom.SED", debug);
        michi2DataClass *SDLIB5 = new michi2DataClass("RadioPowerlaw.Single.lib.SED", debug);
        SDLIB3->loadDataBlock(10); SDLIB3->YNorm = 1.0;
        SDLIB4->loadDataBlock(10); SDLIB4->YNorm = 1.0;
        SDLIB5->loadDataBlock(0); SDLIB5->YNorm = 1.0;
        
        std::vector<michi2DataClass *> SDLIBS;
        SDLIBS.push_back(SDLIB1);
        SDLIBS.push_back(SDLIB2);
        SDLIBS.push_back(SDLIB3);
        SDLIBS.push_back(SDLIB4);
        SDLIBS.push_back(SDLIB5);
        
        michi2Constraint *TempConstraint = new michi2Constraint();
        
        TempConstraint->parse(expression, NULL, SDLIBS, debug);
        
        std::cout << "Value: " << TempConstraint->Expression.value() << std::endl;
        std::cout << "Checking constraint: \"" << TempConstraint->ConstraintStr << "\"" << std::endl;
        std::cout << "Checking variables: " << std::endl;
        std::cout << TempConstraint->printVariablesWithAddressesAndValues("\n", "    ") << std::endl;
        std::cout << "    " << "&LIB3_X[0] = 0x" << std::hex << (size_t)(&(SDLIB3->X[0])) << std::endl;
        std::cout << "    " << "&LIB3_Y[0] = 0x" << std::hex << (size_t)(&(SDLIB3->Y[0])) << std::endl;
        std::cout << std::endl;
        
        
        
        
        
        // do some changes 1
        SDLIB3->YNorm = 2.0;
        SDLIB4->YNorm = 2.0;
        //TempConstraint->TEST_LIB_INDEX = 90;
        //SDLIBS[2]->CurrentDataBlockIndex = 90;
        //SDLIBS[2]->CurrentDataBlockIndexAsDouble = 90.0;
        
        //TempConstraint->compileSymbolTable(TempConstraint->ExpressionStr, TempConstraint->Variables, NULL, SDLIBS);
        //TempConstraint->recompile();
        //TempConstraint->solveExpression(TempConstraint->ExpressionStr, TempConstraint->SymbolTable);
        
        std::cout << std::endl;
        std::cout << "Value: " << TempConstraint->Expression.value() << std::endl;
        std::cout << "Checking constraint: \"" << TempConstraint->ConstraintStr << "\"" << std::endl;
        std::cout << "Checking variables: " << std::endl;
        std::cout << TempConstraint->printVariablesWithAddressesAndValues("\n", "    ") << std::endl;
        std::cout << "    " << "&LIB3_X[0] = 0x" << std::hex << (size_t)(&(SDLIB3->X[0])) << std::endl;
        std::cout << "    " << "&LIB3_Y[0] = 0x" << std::hex << (size_t)(&(SDLIB3->Y[0])) << std::endl;
        std::cout << std::endl;
        
        
        
        
        
        // do some changes 2
        SDLIB3->loadDataBlock(SDLIB3->YNum-1, 1);
        SDLIB4->loadDataBlock(SDLIB4->YNum-1, 1);
        SDLIB3->YNorm = 1.0;
        SDLIB4->YNorm = 1.0;
        
        //TempConstraint->solveExpression(TempConstraint->ExpressionStr, TempConstraint->SymbolTable);
        //TempConstraint->resolve();
        
        std::cout << std::endl;
        std::cout << "Value: " << TempConstraint->Expression.value() << std::endl;
        std::cout << "Checking constraint: \"" << TempConstraint->ConstraintStr << "\"" << std::endl;
        std::cout << "Checking variables: " << std::endl;
        std::cout << TempConstraint->printVariablesWithAddressesAndValues("\n", "    ") << std::endl;
        std::cout << "    " << "&LIB3_X[0] = 0x" << std::hex << (size_t)(&(SDLIB3->X[0])) << std::endl;
        std::cout << "    " << "&LIB3_Y[0] = 0x" << std::hex << (size_t)(&(SDLIB3->Y[0])) << std::endl;
        std::cout << std::endl;
        
        
        
        
        
        // do some changes 3
        SDLIB3->loadDataBlock(SDLIB3->YNum-1, 1);
        SDLIB4->loadDataBlock(SDLIB4->YNum-1, 1);
        SDLIB3->YNorm = 2.0;
        SDLIB4->YNorm = 2.0;
        
        //TempConstraint->solveExpression(TempConstraint->ExpressionStr, TempConstraint->SymbolTable);
        
        std::cout << std::endl;
        std::cout << "Value: " << TempConstraint->Expression.value() << std::endl;
        std::cout << "Checking constraint: \"" << TempConstraint->ConstraintStr << "\"" << std::endl;
        std::cout << "Checking variables: " << std::endl;
        std::cout << TempConstraint->printVariablesWithAddressesAndValues("\n", "    ") << std::endl;
        std::cout << "    " << "&LIB3_X[0] = 0x" << std::hex << (size_t)(&(SDLIB3->X[0])) << std::endl;
        std::cout << "    " << "&LIB3_Y[0] = 0x" << std::hex << (size_t)(&(SDLIB3->Y[0])) << std::endl;
        std::cout << std::endl;
        
        
        
        
        
        
        // do some changes 4
        SDLIB3->loadDataBlock(5);
        SDLIB4->loadDataBlock(5);
        SDLIB3->YNorm = 2.0;
        SDLIB4->YNorm = 2.0;
        
        //TempConstraint->solveExpression(TempConstraint->ExpressionStr, TempConstraint->SymbolTable);
        
        std::cout << std::endl;
        std::cout << "Value: " << TempConstraint->Expression.value() << std::endl;
        std::cout << "Checking constraint: \"" << TempConstraint->ConstraintStr << "\"" << std::endl;
        std::cout << "Checking variables: " << std::endl;
        std::cout << TempConstraint->printVariablesWithAddressesAndValues("\n", "    ") << std::endl;
        std::cout << "    " << "&LIB3_X[0] = 0x" << std::hex << (size_t)(&(SDLIB3->X[0])) << std::endl;
        std::cout << "    " << "&LIB3_Y[0] = 0x" << std::hex << (size_t)(&(SDLIB3->Y[0])) << std::endl;
        std::cout << std::endl;
        
        
        
        
        // test std::vector
        std::vector t1;
        //t1.reserve(3);
        t1.push_back(1.0); t1.push_back(2.0); t1.push_back(3.0);
        std::cout << std::hex << "0x" << (size_t)(t1.data()) << std::endl;
        
        t1[2] = 99.0;
        std::cout << std::hex << "0x" << (size_t)(t1.data()) << std::endl;
        
        std::vector t2;
        t2.push_back(7.0); t2.push_back(8.0); t2.push_back(9.0);
        t1 = t2;
        std::cout << std::hex << "0x" << (size_t)(t1.data()) << std::endl;
        
        
        
    }
    
    return 0;
}
            

