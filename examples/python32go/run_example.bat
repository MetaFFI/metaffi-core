@echo off

rem compile openffi
echo Generating GoFuncsOpenFFIGuest.[so,dylib,dll] and GoFuncsOpenFFIGuest.py by compiling GoFuncs.proto with openffi
openffi -c --idl GoFuncs.proto -f python3 -t

rem run examples
echo Run Example Functions
python3 run.py

rem if there's an unexpected error - print there's an error!
if %ERRORLEVEL% GEQ 1 echo Example Failed && exit /b 1

echo ==== Examples executed successfully ====