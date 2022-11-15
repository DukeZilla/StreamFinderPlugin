echo off
break off
title Stream Finder Plugin ^| Streamlink Recorder
PUSHD "%CD%"
CD /D "%~dp0"
set back=%cd%
cd %appdata%
cd ..
cd local
set local=%cd%
cd %back%
cls
goto main

:main
set DateTime=%DATE:~-4%.%DATE:~4,2%.%DATE:~7,2% %TIME:~0,2%.%TIME:~3,2%.%TIME:~6,2%
for /f "usebackq tokens=1,* delims=" %%x in (streamlink-info.txt) do (set %%x)
IF EXIST Recordings (cd Recordings) else (mkdir Recordings & cd Recordings)
call %local%\Streamlink\bin\streamlink -o "%TwitchUsr% (%DateTime%).ts" "%TwitchLink%" 1080p,best
exit