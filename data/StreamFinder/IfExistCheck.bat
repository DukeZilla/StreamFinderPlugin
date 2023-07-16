@echo off
break off
title File Check
PUSHD "%CD%"
CD /D "%~dp0"
set back=%cd%
cls
goto main

:main
:: ABOUT FILE
echo Stream Finder Files Information > about.txt
echo. >> about.txt
echo ------------------------------------------------O >> about.txt
echo. >> about.txt

:: - Plugin Version
for /f "usebackq delims=" %%x in (`type version.txt`) do (set plugin=%%x)
echo "%plugin%" >> about.txt
echo. >> about.txt

:: - Live Status Detector Version
for /f "usebackq tokens=1,* delims=|" %%g in (`type live-status-detector.ps1 ^| findstr /ic:"Detector Version"`) do (set detect=^"Live Status%%h)
echo %detect% >> about.txt
echo. >> about.txt

:: - Logger Version
for /f "usebackq tokens=1,* delims=|" %%g in (`type logger.ps1 ^| findstr /ic:"Stream FInder Logger"`) do (set log=^"Logger%%h)
echo %log% >> about.txt
echo. >> about.txt

:: - Updater Version
for /f "usebackq tokens=5,6,7 delims= " %%g in (`type update.bat ^| findstr /ic:"Stream FInder Plugin Updater"`) do (set updater="%%g %%h %%i")
echo %updater% >> about.txt
:: END OF ABOUT FILE

:: Folders
IF EXIST Recordings (echo "Recordings folder exists") else (mkdir Recordings)
IF EXIST Recordings\thumbnails (echo "Thumbnails exist") else (mkdir Recordings\thumbnails)
IF EXIST cache (echo "Cache folder exists") else (mkdir cache)

:: Text Files
IF EXIST cache\PeaceOfMind.txt (echo "Pom Exists") else (echo For logging >> cache\PeaceOfMind.txt)
IF EXIST cache\Session-Blacklist.txt (echo "Ses Exists") else (echo Streamers found today will be logged here >> cache\Session-Blacklist.txt)
IF EXIST cache\livestreamlog.txt (echo "Log Exists") else (echo Live Streamers Will be logged here. >> cache\livestreamlog.txt)
IF EXIST cache\streamlink-session.txt (echo "Rec Exists") else (echo None. >> cache\streamlink-session.txt)

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

echo "VBS files done.

:: Updating the update button
echo "Updating the updater button ex dee"
echo $updater = iwr -uri https://raw.githubusercontent.com/DukeZilla/StreamFinderPlugin/main/data/StreamFinder/update.bat > temp.ps1
echo $updater = $updater.content >> temp.ps1
echo echo "$updater" ^| Out-File -Encoding Ascii "update.bat" >> temp.ps1
powershell.exe -executionpolicy bypass -f "temp.ps1"
del temp.ps1

:: Recordings path
cd Recordings
echo %cd% > "%back%\cache\rec-path.txt"
cd ..

:: Updating file paths
echo "Updating file paths"
echo "Streamlink... this may take a minute"
cd \ & where /r . streamlink.exe > "%back%\cache\Path-00.txt" & cd %back% & findstr /ic:"streamlink" "cache\Path-00.txt" > "%back%\cache\Path-streamlink.txt"
echo "ffmpeg... this may take another minute"
cd \ & where /r . ffmpeg.exe > "%back%\cache\Path-01.txt" & cd %back% & findstr /ic:"streamlink" "cache\Path-01.txt" > "%back%\cache\Path-ffmpeg.txt"
del cache\Path-00Path-00.txt
del cache\Path-01.txt
echo DONE!
timeout 1 > nul
exit
