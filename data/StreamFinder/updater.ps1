$rl = (get-process -name RocketLeague).path -erroraction silentlycontinue
kill -name RocketLeague -Force -erroraction silentlycontinue
if (get-process -name steam) {
	$steam = (get-process -name steam).path -erroraction silentlycontinue
	kill -name Steam -force
	}
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
cd ..
cd ..
cd Plugins
copy "StreamFinderPlugin.dll" "$p\plugins"
cd $env:USERPROFILE\Downloads 
rmdir StreamFinderPlugin -Recurse -Force
del StreamFinderPlugin.zip 
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
if ($null -ne $steam) {start-process $steam} -erroraction silentlycontinue
if ($null -ne $rl) {start-process $rl} -erroraction silentlycontinue
exit 
