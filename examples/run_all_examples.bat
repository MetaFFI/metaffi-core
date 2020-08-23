@echo off

echo 'Skipping Go->Go example (experimental. Go cannot load a Go C-Shared library)'
rem cd go
rem run_example.bat
rem cd ..
echo =======================================
echo 'Running Python3->Python3'
cd python3
run_example.bat
cd ..
echo =======================================
echo 'Running Go->Python3'
cd go2python3
run_example.bat
cd ..
echo =======================================
echo 'Running Python3->Go'
cd python32go
run_example.bat
cd ..
echo =======================================
echo 'Running Real World Go->Python3'
cd RWgo2python3
run_example.bat
cd ..
echo =======================================
echo 'Running Real World Python3->Go'
cd RWpython32go
run_example.bat
cd ..
echo =======================================
echo Done!

