@echo off
break off
set __compat_layer=runasinvoker
PUSHD "%CD%"
CD /D "%~dp0"
goto main

:main
title Stream Finder Plugin Updater
@echo off
cls

( echo kill -name "RocketLeague" -force 
  echo cd .. 
  echo cd .. 
  echo $p = (pwd^).path 
  echo $source = "https://github.com/DukeZilla/StreamFinderPlugin/archive/refs/heads/main.zip" 
  echo $destination = "$env:USERPROFILE\Downloads\StreamFinderPlugin.zip" 
  echo Invoke-WebRequest $source -OutFile $destination 
  echo cd \ 
  echo cd $env:USERPROFILE\Downloads 
  echo echo "Confirm the prompt below to update." 
  echo Expand-Archive -LiteralPath "$env:USERPROFILE\Downloads\StreamFinderPlugin.zip" -Force 
  echo cd StreamFinderPlugin 
  echo cd StreamFinderPlugin-main 
  echo cd plugins 
  echo copy "StreamFinderPlugin.dll" "$p\plugins" 
  echo cd .. 
  echo cd data 
  echo cd StreamFinder 
  echo copy "live-status-detector.ps1" "$p\data\StreamFinder" 
  echo cd $env:USERPROFILE\Downloads 
  echo rmdir StreamFinderPlugin 
  echo del StreamFinderPlugin.zip 
  echo exit ) >> updater.ps1 
  
echo.
powershell.exe -executionpolicy bypass -f "updater.ps1"
if %errorlevel%==1 (
	color 4
	echo An error has occurred when attempting to run:
	echo "updater.ps1"
)
echo. 
del updater.ps1
color 0a
echo "Update for the Stream Finder Plugin is finished, you may start Rocket League." 
echo. 
pause 
exit
