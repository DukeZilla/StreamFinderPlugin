echo off
break off
title Open Chat
PUSHD "%CD%"
CD /D "%~dp0"
goto main

:main
cd cache
for /f "usebackq tokens=1,* delims=" %%x in (streamlink-info.txt) do (set %%x)
start %TwitchLink%
exit