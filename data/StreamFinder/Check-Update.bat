@echo off
break off
title Stream Finder Plugin Update Check
PUSHD "%CD%"
CD /D "%~dp0"
goto main

:main

( echo $version = iwr -uri "https://raw.githubusercontent.com/DukeZilla/StreamFinderPlugin/main/data/StreamFinder/version.txt"
  echo $version = $version.content
  echo $version = $version.trim('')
  echo echo "$version" ^| Out-File -Encoding Ascii "version.txt"
  echo exit ) > update-check.ps1 
  
powershell.exe -executionpolicy bypass -f "update-check.ps1"
if %errorlevel%==1 (
	color 4
	echo An error has occurred when attempting to run:
	echo "update-check.ps1"
	echo.
	timeout 5
)
del update-check.ps1
exit
