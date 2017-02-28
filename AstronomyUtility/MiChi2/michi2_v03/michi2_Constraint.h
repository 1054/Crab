#ifndef michi2_ConstraintStructure
#define michi2_ConstraintStructure

struct ConstraintStructure {
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

#endif
