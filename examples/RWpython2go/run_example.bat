@echo off

rem compile protobuf
echo Generating GoUtils.pb.go and GoUtils_pb2.py by compiling GoUtils.proto with protoc
protoc --go_out=. --python_out=. GoUtils.proto

rem compile openffi
echo Generating GoUtilsOpenFFIGuest.[so,dylib,dll] and GoUtilsOpenFFIGuest.py by compiling GoUtils.proto with openffi
openffi -c --idl GoUtils.proto -f python -t

rem run examples
echo Run Example Functions
python3.8 run.py

rem if there's an unexpected error - print there's an error!
if %ERRORLEVEL% GEQ 1 echo Example Failed && exit /b 1

echo ==== Examples executed successfully ====