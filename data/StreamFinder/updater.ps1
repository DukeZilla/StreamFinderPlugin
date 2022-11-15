$rl = (get-process -name RocketLeague -erroraction silentlycontinue).path
kill -name RocketLeague -Force -erroraction silentlycontinue
if (get-process -name steam) {
	$steam = (get-process -name steam -erroraction silentlycontinue).path
	kill -name Steam -force
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
copy "logger.ps1" "$p\data\StreamFinder" 
copy "log.bat" "$p\data\StreamFinder"
copy "IfExistCheck.bat" "$p\data\StreamFinder"
copy "Twitch-Token.psm1" "$p\data\StreamFinder"
copy "update.bat" "$p\data\StreamFinder"
copy "Streamlink-Recorder.bat" "$p\data\StreamFinder"
copy "stream_finder_icon.png" "$p\data\StreamFinder"
copy "FileCheck.vbs" "$p\data\StreamFinder"
copy "View-Recording.bat" "$p\data\StreamFinder"
copy "version.txt" "$p\data\StreamFinder"
cd ..
cd ..
cd Plugins
copy "StreamFinderPlugin.dll" "$p\plugins"
cd $env:USERPROFILE\Downloads 
rmdir StreamFinderPlugin -Recurse -Force
del StreamFinderPlugin.zip 
cd "$p\data\StreamFinder"
if (-not(gc livestreamlog.txt -erroraction silentlycontinue)) {echo " " | Out-File -Append -Encoding Ascii "livestreamlog.txt"}
if (-not(gc Session-blacklist.txt -erroraction silentlycontinue)) {echo " " | Out-File -Append -Encoding Ascii "Session-blacklist.txt"}
if (-not(gc PeaceOfMind.txt -erroraction silentlycontinue)) {echo " " | Out-File -Append -Encoding Ascii "PeaceOfMind.txt"}
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
