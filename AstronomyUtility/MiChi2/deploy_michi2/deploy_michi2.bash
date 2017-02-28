#!/bin/bash
#

deploy_current_dir=$(pwd -P)
deploy_script_dir=$(dirname "${BASH_SOURCE[0]}")
deploy_upper_dir=$(dirname "${deploy_script_dir}")

echo "Welcome"
echo "Deploying michi2 into ${deploy_current_dir} ..."
sleep 1.0

# 
# Copy files
# 
cp "${deploy_script_dir}"/*.sm "${deploy_script_dir}"/run*.sh .
unzip "${deploy_upper_dir}/lib_SED/FSPS.Padova.BaSeL.Z0.0190.EBV.lib.zip"
unzip "${deploy_upper_dir}/lib_SED/DL07.DuoCom.SinglePAH.lib.zip"
unzip "${deploy_upper_dir}/lib_SED/MullaneyAGN.Single.lib.SED.zip"
unzip "${deploy_upper_dir}/lib_SED/RadioPowerlaw.Single.lib.SED.zip"
if [[ $(uname) == "Darwin" ]]; then
    echo "Mac!"; sleep 1.0
    cp "${deploy_upper_dir}/michi2_v03/michi2_v03_20160727mac" .
    ln -fs "michi2_v03_20160727mac" michi2_v03
    chmod +x michi2_v03
else
    cp "${deploy_upper_dir}/michi2_v03/michi2_v03_20160829_Linux_x86_64" .
    ln -fs "michi2_v03_20160829_Linux_x86_64" michi2_v03
    chmod +x michi2_v03
fi

ln -fs "run_plotting_quintuple.sm" "run_plotting.sm"


