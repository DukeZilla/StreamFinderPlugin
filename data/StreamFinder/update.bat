@echo off
break off
set __compat_layer=runasinvoker
PUSHD "%CD%"
CD /D "%~dp0"
goto main

:main
title Stream Finder Plugin Updater Version 1.3
@echo off
cls
color 5
echo    _____ __                               _______           __         
echo   / ___// /_________  ____ _____ ___     / ____(_)___  ____/ /__  _____
echo   \__ \/ __/ ___/ _ \/ __ `/ __ `__ \   / /_  / / __ \/ __  / _ \/ ___/
echo  ___/ / /_/ /  /  __/ /_/ / / / / / /  / __/ / / / / / /_/ /  __/ /    
echo /____/\__/_/   \___/\__,_/_/ /_/ /_/  /_/   /_/_/ /_/\__,_/\___/_/  
echo.
echo -----------------------------------------------------------------------------------O
echo.
echo By P as in Papi
echo.
echo.

( echo function start_notif {
  echo Add-Type -AssemblyName System.Windows.Forms
  echo $global:balmsg = New-Object System.Windows.Forms.NotifyIcon
  echo $path = (Get-Process -id $pid^).Path
  echo $balmsg.Icon = [System.Drawing.Icon]::ExtractAssociatedIcon($path^)
  echo $balmsg.BalloonTipIcon = [System.Windows.Forms.ToolTipIcon]::Info
  echo $balmsg.BalloonTipText = "Update starting..."
  echo $balmsg.BalloonTipTitle = "Stream Finder Plugin"
  echo $balmsg.Visible = $true
  echo $balmsg.ShowBalloonTip(60000^)
  echo }
  echo start_notif
  echo cd .. 
  echo $p = (pwd^).path 
  echo Write-host "Downloading Stream Finder Plugin"
  echo $source = "https://github.com/DukeZilla/StreamFinderPlugin/archive/refs/heads/main.zip" 
  echo $destination = "$env:USERPROFILE\Downloads\StreamFinderPlugin.zip" 
  echo Invoke-WebRequest $source -OutFile $destination 
  echo cd \ 
  echo cd $env:USERPROFILE\Downloads 
  echo Write-host "Extracting files to Downloads folder"
  echo Expand-Archive -LiteralPath "$env:USERPROFILE\Downloads\StreamFinderPlugin.zip" -Force 
  echo cd StreamFinderPlugin 
  echo cd StreamFinderPlugin-main 
  echo cd data 
  echo cd StreamFinder 
  echo Write-host "Starting update..."
  echo copy "updater.ps1" "$p\StreamFinder" 
  echo exit ) > confirmer.ps1 
 
echo.
powershell.exe -executionpolicy bypass -f "confirmer.ps1"
if %errorlevel%==1 (
	color 4
	echo An error has occurred when attempting to run:
	echo "confirmer.ps1"
	echo.
	timeout 5
	exit
)
powershell.exe -executionpolicy bypass -f "updater.ps1"
if %errorlevel%==1 (
	color 4
	echo An error has occurred when attempting to run:
	echo "updater.ps1"
	echo.
	timeout 5
	exit
)
echo. 
del updater.ps1
del confirmer.ps1
color 0a
echo Update for the Stream Finder Plugin is finished!
echo. 
timeout 5
exit
