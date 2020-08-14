#!/usr/bin/env bash

# compile openffi
echo Generating GoFuncsOpenFFIGuest.[so,dylib,dll] and GoFuncsOpenFFIGuest.py by compiling GoFuncs.proto with openffi
openffi -c --idl GoFuncs.proto -f python3 -t

# run examples
echo Run Example Functions
python3.8 run.py

# if there's an unexpected error - print there's an error!
exit_code=$?
if [[ $exit_code -ne 0 ]]; then
    echo Example failed!
    exit $exit_code
fi

echo ==== Examples executed successfully ====