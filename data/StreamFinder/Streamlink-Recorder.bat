echo off
break off
title Stream Finder Plugin ^| Streamlink Recorder
PUSHD "%CD%"
CD /D "%~dp0"
set back=%cd%
cls
goto main

:main
set DateTime=%DATE:~-4%.%DATE:~4,2%.%DATE:~7,2%%TIME:~0,2%.%TIME:~3,2%.%TIME:~6,2%
for /f "usebackq tokens=1,* delims=" %%x in (Path-streamlink.txt) do (set streamlink=%%x)
for /f "usebackq tokens=1,* delims=" %%x in (streamlink-info.txt) do (set %%x)
IF EXIST Recordings (cd Recordings) else (mkdir Recordings & cd Recordings)
call "%streamlink%" -o "%TwitchUsr% (%DateTime%).ts" "%TwitchLink%" 1080p,best
IF %ERRORLEVEL% == 1 (cd %back% & echo Unable to record, %TwitchUsr% is not live. > rec-status.txt)
exit
