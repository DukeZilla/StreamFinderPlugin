@echo off
break off
title Crash Prevention File
PUSHD "%CD%"
CD /D "%~dp0"
set back=%cd%
cls
goto main

:main
echo Stream Finder Files Information > about.txt
echo. >> about.txt
echo ------------------------------------------------O >> about.txt
echo. >> about.txt

:: Plugin Version
for /f "usebackq delims=" %%x in (`type version.txt`) do (set plugin=%%x)
echo "%plugin%" >> about.txt
echo. >> about.txt

:: Live Status Detector Version
for /f "usebackq tokens=1,* delims=|" %%g in (`type live-status-detector.ps1 ^| findstr /ic:"Detector Version"`) do (set detect=^"Live Status%%h)
echo %detect% >> about.txt
echo. >> about.txt

:: Logger Version
for /f "usebackq tokens=1,* delims=|" %%g in (`type logger.ps1 ^| findstr /ic:"Stream FInder Logger"`) do (set log=^"Logger%%h)
echo %log% >> about.txt
echo. >> about.txt

:: Updater Version
for /f "usebackq tokens=5,6,7 delims= " %%g in (`type update.bat ^| findstr /ic:"Stream FInder Plugin Updater"`) do (set updater="%%g %%h %%i")
echo %updater% >> about.txt

:: Text Files
IF EXIST PeaceOfMind.txt (echo "Pom Exists") else (echo For logging >> PeaceOfMind.txt)
IF EXIST Session-Blacklist.txt (echo "Ses Exists") else (echo Streamers found today will be logged here >> Session-Blacklist.txt)
IF EXIST livestreamlog.txt (echo "Log Exists") else (echo Live Streamers Will be logged here. >> livestreamlog.txt)
IF EXIST streamlink-session.txt (echo "Rec Exists") else (echo None. >> streamlink-session.txt)

:: Folder
IF EXIST Recordings (echo "Recordings folder exists") else (mkdir Recordings)

:: VBS Files
del stream-finder.vbs
del record.vbs
del recordings-dir.vbs
del stop-recording.vbs

IF EXIST stream-finder.vbs (echo "Stream Finder vbs Exists") else ( echo set shell = wscript.createobject("wscript.shell"^) > stream-finder.vbs
	echo appData = shell.ExpandEnvironmentStrings("%APPDATA%"^) >> stream-finder.vbs
	echo file = Chr(34^) ^& appData ^& "\bakkesmod\bakkesmod\data\StreamFinder\streamfinder.bat" ^& Chr(34^) >> stream-finder.vbs
	echo shell.run file, 0 >> stream-finder.vbs
	echo wscript.quit >> stream-finder.vbs
	)

IF EXIST record.vbs (echo "Stream Finder vbs Exists") else ( echo set shell = wscript.createobject("wscript.shell"^) > record.vbs
	echo appData = shell.ExpandEnvironmentStrings("%APPDATA%"^) >> record.vbs
	echo file = Chr(34^) ^& appData ^& "\bakkesmod\bakkesmod\data\StreamFinder\Streamlink-Recorder.bat" ^& Chr(34^) >> record.vbs
	echo shell.run file, 0 >> record.vbs
	echo wscript.quit >> record.vbs
	)
	
IF EXIST stop-recording.vbs (echo "Stop Rec vbs Exists") else ( echo set shell = wscript.createobject("wscript.shell"^) > stop-recording.vbs
	echo appData = shell.ExpandEnvironmentStrings("%APPDATA%"^) >> stop-recording.vbs
	echo file = Chr(34^) ^& appData ^& "\bakkesmod\bakkesmod\data\StreamFinder\ffmpeg-converter.bat" ^& Chr(34^) >> stop-recording.vbs
	echo shell.run "taskkill /im streamlink.exe -f", 0 >> stop-recording.vbs
	echo wscript.sleep 2000 >> stop-recording.vbs
	echo shell.run file, 0 >> stop-recording.vbs
	echo wscript.quit >> stop-recording.vbs
	)

IF EXIST recordings-dir.vbs (echo "Rec dir vbs Exists") else ( echo set shell = wscript.createobject("wscript.shell"^) > recordings-dir.vbs
	echo appData = shell.ExpandEnvironmentStrings("%APPDATA%"^) >> recordings-dir.vbs
	echo dir00 = Chr(34^) ^& appData ^& "\bakkesmod\bakkesmod\data\StreamFinder\Recordings" ^& Chr(34^) >> recordings-dir.vbs
	echo shell.run dir00 >> recordings-dir.vbs
	echo wscript.quit >> recordings-dir.vbs
	)

:: Path
cd \ & where /r . streamlink.exe > "%back%\Path-00.txt" & cd %back% & findstr /ic:"streamlink" "Path-00.txt" > Path-streamlink.txt
cd \ & where /r . ffmpeg.exe > "%back%\Path-01.txt" & cd %back% & findstr /ic:"streamlink" "Path-01.txt" > Path-ffmpeg.txt
del Path-00.txt
del Path-01.txt

taskkill /im cmd.exe -f
exit
