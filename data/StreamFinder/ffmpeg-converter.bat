@echo off
break off
title Convert
PUSHD "%CD%"
CD /D "%~dp0"
set back=%cd%
for /f "usebackq tokens=1,* delims=" %%x in (Path-ffmpeg.txt) do (set ffmpeg=%%x)
cd Recordings
set recDir=%cd%
cls
goto main

:main
for /f "delims=" %%g in ('dir /b/a-d/od/t:c') do set LastVid=%%g
for %%f  in ("%LastVid%") do set tsVid=%%~nf
call "%ffmpeg%" -i "%tsVid%.ts" -c copy "%tsVid%.mp4"
IF %ERRORLEVEL% == 0 DEL "%LastVid%"
exit