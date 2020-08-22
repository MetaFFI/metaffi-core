@echo off & setlocal
powershell -ExecutionPolicy Bypass -command ". '"%cd%\scripts.ps1"'; build_installer_package %1;"
