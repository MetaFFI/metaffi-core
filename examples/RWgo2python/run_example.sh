#!/usr/bin/env bash

# make sure python-strings-utils is installed
pip3 install python-string-utils

# compile protobuf
echo Generating PythonStringUtils_pb2.py and PythonStringUtils.pb.go by compiling PythonStringUtils.proto with protoc
protoc --python_out=. --go_out=. PythonStringUtils.proto

# compile openffi
echo Generating PythonStringUtilsOpenFFIGuest.py and PythonStringUtilsOpenFFIHost.[so,dylib,dll] by compiling PythonStringUtils.proto with openffi
openffi -c --idl PythonStringUtils.proto -f go -t

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