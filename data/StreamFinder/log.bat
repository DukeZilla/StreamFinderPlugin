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
powershell.exe -executionpolicy bypass -f "logger.ps1"
pause
exit