@echo off
copy scripts.ps1 ..>NUL
cd..
echo powershell -ExecutionPolicy ByPass -command ". '"%cd%\scripts.ps1"'; uninstall;" > remove.bat
echo del scripts.ps1 >> remove.bat
echo del remove.bat >> remove.bat
start cmd /c remove.bat /wait
echo Uninstalled OpenFFI successfully!
