cd .. 
$p = (pwd).path 
$source = "https://github.com/DukeZilla/StreamFinderPlugin/archive/refs/heads/main.zip" 
$destination = "$env:USERPROFILE\Downloads\StreamFinderPlugin.zip" 
Invoke-WebRequest $source -OutFile $destination 
cd \ 
cd $env:USERPROFILE\Downloads 
Expand-Archive -LiteralPath "$env:USERPROFILE\Downloads\StreamFinderPlugin.zip" -Force 
cd StreamFinderPlugin 
cd StreamFinderPlugin-main 
cd data 
cd StreamFinder 
copy "live-status-detector.ps1" "$p\StreamFinder" 
copy "logger.ps1" "$p\StreamFinder" 
copy "log.bat" "$p\StreamFinder"
copy "IfExistCheck.bat" "$p\StreamFinder"
copy "Twitch-Token.psm1" "$p\StreamFinder"
copy "update.bat" "$p\StreamFinder" 
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
exit 
