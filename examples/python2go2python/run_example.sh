#!/usr/bin/env bash

# compile protobuf
echo Generating GoFuncs.pb.go and GoFuncs_pb2.py by compiling GoFuncs.proto with protoc
protoc --go_out=. --python_out=. GoFuncs.proto

# compile openffi

# compile go to python
echo Generating PythonFuncsOpenFFIGuest.py and PythonFuncsOpenFFIHost.[so,dylib,dll] by compiling PythonFuncs.proto with openffi
openffi -c --idl PythonFuncs.proto -f go -t

# compile python to go
echo Generating GoFuncsOpenFFIGuest.[so,dylib,dll] and GoFuncsOpenFFIGuest.py by compiling GoFuncs.proto with openffi
openffi -c --idl GoFuncs.proto -f python -t


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