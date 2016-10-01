#!/bin/bash
#

deploy_current_dir=$(pwd -P)
deploy_script_dir=$(dirname "${BASH_SOURCE[0]}")
deploy_upper_dir=$(dirname "${deploy_script_dir}")

echo "Welcome"
echo "Deploying michi2 into ${deploy_current_dir} ..."

# 
# Copy files
# 
ls "${deploy_script_dir}/"*.sm
ls "${deploy_upper_dir}/lib_SED/"*.zip


