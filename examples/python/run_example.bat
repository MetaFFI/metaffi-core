@echo off

rem compile openffi
echo Generating PythonFuncs_openffi_guest.py and PythonFuncs_openffi_host.py by compiling PythonFuncs.proto with openffi
openffi -c --idl PythonFuncs.proto -f python3 -t

rem run examples
echo Running Example Functions
python run.py

rem if there's an unexpected error - print there's an error!
if %ERRORLEVEL% GEQ 1 echo Example Failed && exit /b 1

echo ==== Examples executed successfully ====