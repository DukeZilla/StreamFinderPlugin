echo off
break off
title Open Chat
goto main

:main
for /f "usebackq tokens=1,* delims=" %%x in (streamlink-info.txt) do (set %%x)
start %TwitchLink%
exit