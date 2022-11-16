@echo off
break off
title Crash Prevention File
PUSHD "%CD%"
CD /D "%~dp0"
set back=%cd%
cls
goto main

:main
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
	
IF EXIST stop-recording.vbs (echo "Stream Finder vbs Exists") else ( echo set shell = wscript.createobject("wscript.shell"^) > stop-recording.vbs
	echo shell.run "taskkill /im streamlink.exe -f", 0 >> stop-recording.vbs
	echo wscript.quit >> stop-recording.vbs
	)

IF EXIST recordings-dir.vbs (echo "Rec dir vbs Exists") else ( echo set shell = wscript.createobject("wscript.shell"^) > recordings-dir.vbs
	echo appData = shell.ExpandEnvironmentStrings("%APPDATA%"^) >> recordings-dir.vbs
	echo dir00 = Chr(34^) ^& appData ^& "\bakkesmod\bakkesmod\data\StreamFinder\Recordings" ^& Chr(34^) >> recordings-dir.vbs
	echo shell.run dir00 >> recordings-dir.vbs
	echo wscript.quit >> recordings-dir.vbs
	)

:: Path
IF EXIST Path-streamlink.txt (echo "streamlink Path Exists") else (cd \ & where /r . streamlink.exe > "%back%\Path-00.txt" & cd %back% & findstr /ic:"streamlink" "Path-00.txt" > Path-streamlink.txt)
IF EXIST Path-ffmpeg.txt (echo "ffmpeg Path Exists") else (cd \ & where /r . ffmpeg.exe > "%back%\Path-01.txt" & cd %back% & findstr /ic:"streamlink" "Path-01.txt" > Path-ffmpeg.txt)
del Path-00.txt
del Path-01.txt

taskkill /im cmd.exe -f
exit
