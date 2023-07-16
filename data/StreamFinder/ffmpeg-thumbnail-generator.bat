@echo off
break off
title Thumbnail Generator
PUSHD "%CD%"
CD /D "%~dp0"
set back=%cd%
SETLOCAL ENABLEDELAYEDEXPANSION
cd cache
for /f "usebackq tokens=1,* delims=" %%x in (Path-ffmpeg.txt) do (set ffmpeg=%%x)
cd ..
IF EXIST Recordings\thumbnails (echo Directories Exist) else (mkdir Recordings\thumbnails)
cd Recordings\thumbnails
set thumbDir=%cd%
cd ..
cls
goto main

:main
for /f "delims=" %%A in ('dir /b/a-d') do (set ExtVid=%%A
set noExtVid=%%~nA
call "!ffmpeg!" -y -i "!ExtVid!" -ss 00:00:15.000 -vframes 1 "!thumbDir!\!noExtVid!.png"
)
endlocal
exit