kill -name "RocketLeague" -force
cd ..
cd ..
$p = (pwd).path
$source = "https://github.com/DukeZilla/StreamFinderPlugin/archive/refs/heads/main.zip"
$destination = "$env:USERPROFILE\Downloads\StreamFinderPlugin.zip"
Invoke-WebRequest $source -OutFile $destination
cd \
cd $env:USERPROFILE\Downloads
echo "Confirm the prompt below to update."
Expand-Archive -LiteralPath "$env:USERPROFILE\Downloads\StreamFinderPlugin.zip" -Force
cd StreamFinderPlugin
cd StreamFinderPlugin-main
cd plugins
copy "StreamFinderPlugin.dll" "$p\plugins"
cd ..
cd data
cd StreamFinder
copy "live-status-detector.ps1" "$p\data\StreamFinder"
cd $env:USERPROFILE\Downloads
rmdir StreamFinderPlugin
del StreamFinderPlugin.zip
echo " "
echo "Update for the Stream Finder Plugin is finished, you may start Rocket League."
echo " "
pause
exit