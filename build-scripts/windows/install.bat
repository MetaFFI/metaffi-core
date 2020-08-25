@echo off & setlocal
powershell -ExecutionPolicy ByPass -command ". '"%cd%\scripts.ps1"'; install %1;"
