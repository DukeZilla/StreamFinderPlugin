echo off
break off
PUSHD "%CD%"
CD /D "%~dp0"
cls
goto main

:main
cd Recordings
set Path=%CD%
for /f "delims=" %%g in ('dir /b/a-d/od/t:c') do set LastVid=%%g
cd \
call PROGRA~1\VideoLAN\VLC\vlc "%Path%\%LastVid%"
exit