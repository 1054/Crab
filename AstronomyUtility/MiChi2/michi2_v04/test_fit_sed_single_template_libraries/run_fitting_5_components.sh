#!/bin/bash
#

set -e

if [[ -f ../michi2_v04_mac ]]; then rm ../michi2_v04_mac; fi

bash -c "cd ../; ./do_Compile"

pwd

Redshift=1.6481
Parallel=3

cp flux_obsframe_without_radio.dat flux_obsframe.dat

../michi2_v04_mac \
                -obs flux_obsframe.dat \
                -redshift $Redshift \
                -lib lib.BC03.Padova1994.BaSeL.Z0.0190.EBV.SED \
                     lib.MullaneyAGN.SED \
                     lib.DL07.HiExCom.SED \
                     lib.DL07.LoExCom.SED \
                     RadioPowerlaw.Single.lib.SED \
                -out fit_5.out \
                -parallel $Parallel \
                -constraint LIB3 INDEX EQ LIB4 INDEX \
                -constraint LIB5 NORM EQ SED "((LIB3*(10^LIB3_PAR3))+(LIB4*(10^LIB4_PAR3)))*40.31970/3750/10^2.4" \
                -filter filter.list \
                -debug -debug -debug -debug
                # -debug -debug -debug -debug
             

