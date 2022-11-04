@echo off
break off
title Crash Prevention FileD
cls
goto main

:main
IF EXIST PeaceOfMind.txt (echo "Pom Exists") else (echo " " >> PeaceOfMind.txt)
IF EXIST Session-Blacklist.txt (echo "Ses Exists") else (echo " " >> Session-Blacklist.txt)
IF EXIST livestreamlog.txt (echo "Log Exists") else (echo " " >> livestreamlog.txt)
taskkill /im cmd.exe -f
exit
