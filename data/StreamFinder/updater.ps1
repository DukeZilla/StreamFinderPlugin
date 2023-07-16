$p = (pwd).path
cd $p
start-transcript -path "$p\cache\transcript-updater.txt"
echo " "

write-host "Logs:"
$rl = (get-process -name RocketLeague -erroraction silentlycontinue).path
cmd.exe /c taskkill /t /im rocketleague.exe /f
if (get-process -name steam) {
	$steam = (get-process -name steam -erroraction silentlycontinue).path
	cmd.exe /c taskkill /t /im steam.exe /f
	}
sleep 1
cd .. 
cd ..
$p = (pwd).path 
cd \ 
cd $env:USERPROFILE\Downloads 
cd StreamFinderPlugin 
cd StreamFinderPlugin-main 
cd data 
cd StreamFinder 
copy "live-status-detector.ps1" "$p\data\StreamFinder" 
write-host "Copied live-status-detector.ps1"
copy "logger.ps1" "$p\data\StreamFinder" 
write-host "Copied logger.ps1"
copy "log.bat" "$p\data\StreamFinder"
write-host "Copied log.bat"
copy "IfExistCheck.bat" "$p\data\StreamFinder"
write-host "Copied IfExistCheck.bat"
copy "Twitch-Token.psm1" "$p\data\StreamFinder"
write-host "Copied Twitch-Token.psm1"
copy "update.bat" "$p\data\StreamFinder"
write-host "Copied update.bat"
copy "Streamlink-Recorder.bat" "$p\data\StreamFinder"
write-host "Copied Streamlink-Recorder.bat"
copy "stream_finder_icon.png" "$p\data\StreamFinder"
write-host "Copied stream_finder_icon.png"
copy "FileCheck.vbs" "$p\data\StreamFinder"
write-host "Copied FileCheck.vbs"
copy "View-Recording.bat" "$p\data\StreamFinder"
write-host "Copied View-Recording.bat"
copy "ffmpeg-converter.bat" "$p\data\StreamFinder"
write-host "Copied ffmpeg-converter.bat"
copy "Check-Update.bat" "$p\data\StreamFinder"
write-host "Copied Check-Update.bat"
copy "Open-Chat.bat" "$p\data\StreamFinder"
write-host "Copied open-chat.bat"
copy "Cleaner.bat" "$p\data\StreamFinder"
write-host "Copied Cleaner.bat"
copy "version.txt" "$p\data\StreamFinder"
write-host "Copied version.txt"
cd ..
cd ..
cd Plugins
copy "StreamFinderPlugin.dll" "$p\plugins"
write-host "Updated StreamFinderPlugin.dll"
cd $env:USERPROFILE\Downloads 
rmdir StreamFinderPlugin -Recurse -Force
del StreamFinderPlugin.zip 
cd "$p\data\StreamFinder"
start FileCheck.vbs
function update_notif {
Add-Type -AssemblyName System.Windows.Forms
$global:balmsg = New-Object System.Windows.Forms.NotifyIcon
$path = (Get-Process -id $pid).Path
$balmsg.Icon = [System.Drawing.Icon]::ExtractAssociatedIcon($path)
$balmsg.BalloonTipIcon = [System.Windows.Forms.ToolTipIcon]::Info
$balmsg.BalloonTipText = "Update complete!"
$balmsg.BalloonTipTitle = "Stream Finder Plugin"
$balmsg.Visible = $true
$balmsg.ShowBalloonTip(60000)
}
update_notif
if ($null -ne $steam) {start-process $steam -erroraction silentlycontinue}
if ($null -ne $rl) {start-process $rl -erroraction silentlycontinue}
exit 
