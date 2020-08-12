@echo off

rem compile protobuf
echo Generating PythonFuncs_pb2.py and PythonFuncs.pb.go by compiling PythonFuncs.proto with protoc
protoc --python_out=. --go_out=. PythonFuncs.proto

rem compile openffi
echo Generating PythonFuncsOpenFFIGuest.py and PythonFuncsOpenFFIHost.[so,dylib,dll] by compiling PythonFuncs.proto with openffi
openffi -c --idl PythonFuncs.proto -f go -t

rem build and run examples
echo Building Examples Host
go build -o examples

echo Run Example Functions
examples.exe

rem if there's an unexpected error - print there's an error!
if %ERRORLEVEL% GEQ 1 echo Example Failed && exit /b 1

echo ==== Examples executed successfully ====