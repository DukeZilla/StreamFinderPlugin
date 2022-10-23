@echo off
break off
set __compat_layer=runasinvoker
PUSHD "%CD%"
CD /D "%~dp0"
goto main

:main
title Stream Finder Plugin Powershell Version 5.1
@echo off
cls
:run
echo.
echo.
powershell.exe -executionpolicy bypass -f "updater.ps1"
if %errorlevel%==1 (
	echo Error file not found 404
	echo "updater.ps1"
	goto fin
)
echo.
:exit
:fin
echo.
echo SCRIPT TERMINATED.
echo.
echo Do you wish to reinitiate this program?
echo.
set /p choice=Type (Y/N): 
if %choice%==Y cls & color 7 & goto run
if %choice%==y cls & color 7 & goto run 
if %choice%==N exit
if %choice%==n exit
echo.
color 0c
echo - Error: Incorrect Selection -
echo.
pause
cls
color 7
goto fin