#!/bin/bash
# 

clang++ -std=c++11 -Dexprtk_disable_string_capabilities -I.. ../michi2_DataClass.cpp ../michi2_MinPack.cpp ../michi2_Constraint.cpp main.cpp -o test

#-Dexprtk_enable_debugging

#./test "3+5"
#./test "LIB3INDEX > 10"


cp \
~/Cloud/Github/Crab.Toolkit.michi2/data/make_lib_SED/02_Make_Lib_SED/lib.BC03.Padova1994.BaSeL.Z0.0190.EBV.SED \
~/Cloud/Github/Crab.Toolkit.michi2/data/make_lib_SED/02_Make_Lib_SED/lib.DL07.2010.03.18.spec.2.0.HiExCom.SED \
~/Cloud/Github/Crab.Toolkit.michi2/data/make_lib_SED/02_Make_Lib_SED/lib.DL07.2010.03.18.spec.2.0.LoExCom.SED \
~/Cloud/Github/Crab.Toolkit.michi2/data/make_lib_SED/02_Make_Lib_SED/lib.MullaneyAGN.SED \
~/Cloud/Github/Crab.Toolkit.michi2/data/make_lib_SED/02_Make_Lib_SED/RadioPowerlaw.Single.lib.SED
.

