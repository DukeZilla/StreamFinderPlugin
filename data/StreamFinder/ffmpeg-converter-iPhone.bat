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
call "%ffmpeg%" -i "%tsVid%.mp4" -vcodec libx264 -profile:v main -level 3.1 -preset medium -crf 23 -x264-params ref=4 -acodec copy -movflags +faststart "%tsVid% Copy.mov"
pause
exit