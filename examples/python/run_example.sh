#!/usr/bin/env bash

# compile protobuf
echo Generating PythonFuncs_pb2.py by compiling PythonFuncs.proto with protoc
protoc --python_out=. PythonFuncs.proto

# compile openffi
echo Generating PythonFuncs_openffi_guest.py and PythonFuncs_openffi_host.py by compiling PythonFuncs.proto with openffi
openffi -c --idl PythonFuncs.proto -f python -t

# run examples
echo Running Example Functions
python3.7 run.py

# if there's an unexpected error - print there's an error!
exit_code=$?
if [[ $exit_code -ne 0 ]]; then
    echo Example failed!
    exit $exit_code
fi

echo ==== Examples executed successfully ====