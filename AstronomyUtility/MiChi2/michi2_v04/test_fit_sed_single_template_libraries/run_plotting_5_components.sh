#!/bin/bash
#

source ~/Cloud/Github/Crab.Toolkit.michi2/bin/bin_setup.bash

rm -rf obj_*

cp flux_obsframe_with_radio.dat flux_obsframe.dat

michi2-plot-results-of-fitting-5-components fit_5.out

michi2_read_lib_SEDs.py fit_5.out 1 obj_x1
michi2_read_lib_SEDs.py fit_5.out 2 obj_x2
michi2_read_lib_SEDs.py fit_5.out 3 obj_x3
michi2_read_lib_SEDs.py fit_5.out 4 obj_x4

cd obj_x1; michi2-topcat-plot-lib-SEDs-of-fitting-5-components; cd ..
cd obj_x2; michi2-topcat-plot-lib-SEDs-of-fitting-5-components; cd ..
cd obj_x3; michi2-topcat-plot-lib-SEDs-of-fitting-5-components; cd ..
cd obj_x4; michi2-topcat-plot-lib-SEDs-of-fitting-5-components; cd ..

open obj_*/Plot_*.pdf
