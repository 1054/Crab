source ~/Cloud/Github/Crab.Toolkit.michi2/bin/bin_setup.bash

module avail
module load anaconda

srun -N1 -n20 --pty bash

michi2-run-fitting-5-components 1.6481

michi2_read_lib_SEDs.py fit_5.out 1 obj_1
michi2_read_lib_SEDs.py fit_5.out 2 obj_2

cd obj_1; michi2-topcat-plot-lib-SEDs-of-fitting-5-components; cd ..
cd obj_2; michi2-topcat-plot-lib-SEDs-of-fitting-5-components; cd ..

