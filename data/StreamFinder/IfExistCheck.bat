@echo off
break off
title Crash Prevention FileD
cls
goto main

:main
IF EXIST PeaceOfMind.txt echo "Pom Exists"
IF %ERRORLEVEL%==1 (echo " " >> PeaceOfMind.txt)
IF EXIST Session-Blacklist.txt echo "Ses Exists"
IF %ERRORLEVEL%==1 (echo " " >> Session-Blacklist.txt)
IF EXIST livestreamlog.txt echo "Log Exists"
IF %ERRORLEVEL%==1 (echo " " >> livestreamlog.txt)
pause
exit