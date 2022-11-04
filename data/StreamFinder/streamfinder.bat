@echo off
break off
set __compat_layer=runasinvoker
PUSHD "%CD%"
CD /D "%~dp0"
goto main

:main
title Stream Finder Plugin
@echo off
cls
:run
echo.
echo.
powershell.exe -executionpolicy bypass -f "live-status-detector.ps1"
echo.
exit
