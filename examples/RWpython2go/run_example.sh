#!/usr/bin/env bash

# compile protobuf
echo Generating GoUtils.pb.go and GoUtils_pb2.py by compiling GoUtils.proto with protoc
protoc --go_out=. --python_out=. GoUtils.proto

# compile openffi
echo Generating GoUtilsOpenFFIGuest.[so,dylib,dll] and GoUtilsOpenFFIGuest.py by compiling GoUtils.proto with openffi
openffi -c --idl GoUtils.proto -f python -t

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