echo off
break off
PUSHD "%CD%"
CD /D "%~dp0"
cls
goto main

:main
cd cache
tasklist /fi "ImageName eq streamlink.exe" /fo csv 2>NUL | find /I "streamlink.exe">NUL
if "%ERRORLEVEL%"=="0" echo Streamlink is Recording! > status.txt
if "%ERRORLEVEL%"=="1" echo No recordings in progress > status.txt
exit