@echo off

rem compile openffi
echo Generating GoFuncsOpenFFIGuest.[so,dylib,dll] and GoFuncsOpenFFIHost.go by compiling GoFuncs.proto with openffi
openffi -c --idl GoFuncs.proto -f go -t

rem build and run examples
echo Building Example Functions
go build -o examples

rem run examples
echo Run Example Functions
examples.exe

rem if there's an unexpected error - print there's an error!
if %ERRORLEVEL% GEQ 1 echo Example Failed && exit /b 1

echo ==== Examples executed successfully ====