@echo off

rem make sure python-strings-utils is installed
pip3 install python-string-utils

rem compile openffi
echo Generating PythonStringUtilsOpenFFIGuest.py and PythonStringUtilsOpenFFIHost.[so,dylib,dll] by compiling PythonStringUtils.proto with openffi
openffi -c --idl PythonStringUtils.proto -f go -t

rem build and run examples
echo Building Examples Host
go build -o examples

echo Run Example Functions
./examples

rem if there's an unexpected error - print there's an error!
if %ERRORLEVEL% GEQ 1 echo Example Failed && exit /b 1

echo ==== Examples executed successfully ====