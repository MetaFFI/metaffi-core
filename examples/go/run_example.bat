@echo off

rem compile protobuf
echo Generating PythonFuncs_pb2.py by compiling PythonFuncs.proto with protoc
protoc --python_out=. PythonFuncs.proto

rem compile openffi
echo Generating PythonFuncs_openffi_guest.py and PythonFuncs_openffi_host.py by compiling PythonFuncs.proto with openffi
openffi -c --idl PythonFuncs.proto -f python -t python

rem run examples
echo Running Example Functions
python run.py

rem if there's an unexpected error - print there's an error!
if %ERRORLEVEL% GEQ 1 echo Example Failed && exit /b 1

echo ==== Examples executed successfully ====