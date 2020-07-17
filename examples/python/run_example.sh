#!/usr/bin/env bash

protoc --python_out=. PythonFuncs.proto
openffi -c --idl PythonFuncs.proto -f python -t python

# TODO: run examples