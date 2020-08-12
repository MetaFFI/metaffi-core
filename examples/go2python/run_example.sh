#!/usr/bin/env bash

# compile protobuf
echo Generating PythonFuncs_pb2.py and PythonFuncs.pb.go by compiling PythonFuncs.proto with protoc
protoc --python_out=. --go_out=. PythonFuncs.proto

# compile openffi
echo Generating PythonFuncsOpenFFIGuest.py and PythonFuncsOpenFFIHost.[so,dylib,dll] by compiling PythonFuncs.proto with openffi
openffi -c --idl PythonFuncs.proto -f go -t

# build and run examples
echo Building Examples Host
go build -o examples

echo Run Example Functions
./examples

# if there's an unexpected error - print there's an error!
exit_code=$?
if [[ $exit_code -ne 0 ]]; then
    echo Example failed!
    exit $exit_code
fi

echo ==== Examples executed successfully ====