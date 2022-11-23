# ESSENTIAL COMPONENT FOR THE STREAM FINDER PLUGIN | ROCKET LEAGUE BAKKESMOD
# By P as in Papi

echo "Stream Finder | Detector Version 1.57"

Import-Module C:Twitch-Token.psm1

# Discord webhook linke to variable
$p = (pwd).path
cd $env:APPDATA
cd ..
cd "Local\Streamlink\bin"
$global:streamlink_path = (pwd).path
cd $p

$discord_webhook = gc $p\discord-webhook.txt
$test = gc $p\test-webhook.txt -erroraction silentlycontinue

function test_notif {
Add-Type -AssemblyName System.Windows.Forms
$global:balmsg = New-Object System.Windows.Forms.NotifyIcon
$path = (Get-Process -id $pid).Path
$balmsg.Icon = [System.Drawing.Icon]::ExtractAssociatedIcon($path)
$balmsg.BalloonTipIcon = [System.Windows.Forms.ToolTipIcon]::Info
$balmsg.BalloonTipText = 'Test notification sent.'
$balmsg.BalloonTipTitle = "Stream Finder Plugin"
$balmsg.Visible = $true
$balmsg.ShowBalloonTip(60000)
}

function Test_Webhook { # For testing the discord webhook
if ($discord_webhook -eq "*INSERT DISCORD WEBHOOK HERE*") {
	webhook_error
	exit}

$trim02 = get-date | out-string 
$date = $trim02.trim('')

# v Discord Notification Message v
$url = "$discord_webhook"
$content00 = "**TEST NOTIFICATION**" 
$content01 = "Title: This notification is a test to let you know that the discord webhook function is working."
$content02 = "Date found: $date" 
$content03 = "Timestamp: 9:23:01"
$content04 = "Views: 73"
$content05 = "VODs: N/A"
$content06 = "Language: en"
$content07 = "Come say hi! ---> https://www.twitch.tv"
$payload = [PSCustomObject]@{ 
content = "$content00
-------------------------------------------------------O
__*Stream Information*__
- $content01
- $content02
- $content03
- $content04
- $content05
- $content06
-------------------------------------------------------O
- $content07 " } 
# ^ Discord Notification Message ^

iwr -uri $url -method Post -body ($payload | ConvertTo-Json) -ContentType 'Application/Json' # Sending live notification to discord
test_notif
start $p\sound.vbs
write-host "Test Complete!"
del $p\test-webhook.txt
exit
}

function webhook_error {
Add-Type -AssemblyName System.Windows.Forms
$global:balmsg = New-Object System.Windows.Forms.NotifyIcon
$path = (Get-Process -id $pid).Path
$balmsg.Icon = [System.Drawing.Icon]::ExtractAssociatedIcon($path)
$balmsg.BalloonTipIcon = [System.Windows.Forms.ToolTipIcon]::Warning
$balmsg.BalloonTipText = 'Discord webhook not found, recommended to setup your discord webhook in the Stream Finder settings.'
$balmsg.BalloonTipTitle = "Stream Finder Plugin Error"
$balmsg.Visible = $true
$balmsg.ShowBalloonTip(60000)
}

if ($test -eq "Test-Webhook") {Test_Webhook}

echo "Stream search beginning."
$global:stream = 0
$i = -1

# Logging
echo "set shell = wscript.createobject(`"wscript.shell`")" > startlog.vbs
echo "shell.run `"log.bat`", 0" >> startlog.vbs
echo "wscript.quit" >> startlog.vbs
start-process startlog.vbs

function nameloop {
	$i++
	
	# Name search refinement
	$names = gc $p\names.txt
	$botlist = gc $p\botlist.txt
	$old_name = $names | select -index $i
	$restore = $old_name
	$session_blacklist = gc $p\Session-Blacklist.txt -erroraction silentlycontinue
	$blacklist = gc $p\blacklist.txt -erroraction silentlycontinue
	$perma_blacklist = gc $p\permanent-blacklist.txt -erroraction silentlycontinue
	$name = $names | select -index $i
	if ($old_name -eq "Search-Request") {nameloop}
	
	echo "----------------------------------------------------------0"
	echo "Instance #$i"
	
	# Null Value Check / Breaker
	$null_check = $name
    if ($null -eq $null_check) { # at the end of the list the loop will break
		echo "Search halted, reason: null value break"
		echo "Reached the end of name list."
		$names > blacklist.txt
		$names > blacklist-log.txt
		echo "----------------------------------------------------------0"
		echo "Stream search ended."
		exit
	}
	
	$name = $name.replace(" ", "_")
	
	if ($name -match "ttv") { # Split "ttv" from player name
		$name = $name -split "ttv"
		echo 'ttv name split'
	}
	
	if ($name -match "t.tv") { # Split "t.tv" from player name
		$name = $name -split "t.tv"
		echo 't.tv name split'
	}

		$name = $name -replace('-', '_') # Changing Dashes to underscores
		$name = $name -replace ('\W', '') # Removing common special characters
		$global:specChar = $name -replace '[^a-zA-Z0-9\/.!@#$%^&*()-_=+?|;:]', '' # Removing unique special characters

	# Breakers
	if ($i -eq  10) { # to prevent from looping more than necessary
		echo 'Search halted, reason: Max number break'
		$names > blacklist.txt
		$names > blacklist-log.txt
		echo "----------------------------------------------------------0"
		echo "Stream search ended."
		exit
	}
	if ($perma_blacklist -contains $old_name) { # to prevent from sending a notification of the same live streamer forever
		echo "Player: $old_name is permanetaly blacklisted"
		echo "Search skipped."
		nameloop
	}
	if ($blacklist -contains $old_name) { # to prevent from sending a notification of the same live streamer for one game
		echo "Player: $old_name has been temporarily blacklisted"
		echo "Search skipped."
		nameloop
	}
	
	if ($botlist -contains $old_name) { # to prevent from searching bots
		echo "$old_name has been identified as a bot"
		echo "Search skipped."
		nameloop
	}
	
	echo "Tested name = ""$name"""

	$key00 = "null"
	SessionBlacklist
	
	$global:strmsrch = $name
	streamsearch
	echo "Searched $old_name"
	echo "=-=-=-=-=-=-=-=-=-=-=-=-=-="
	
	$global:amount = "pass"
	if ($old_name -like "*twitch.tv*") {$global:amount = "check"}
	if ($old_name -like "*twitch*") {$global:amount = "check"}
	if ($old_name -like "*live*") {$global:amount = "check"}
	if ($old_name -like "*ttv*") {$global:amount = "check"}
	if ($old_name -like "*tv*") {$global:amount = "check"}
	if ($old_name -like "*t.tv*") {$global:amount = "check"}
	if ($old_name -like "*tiktok*") {$global:amount = "check"}
	if ($old_name -like "*yt*") {$global:amount = "check"}
	if ($old_name -like "*youtube*") {$global:amount = "check"}
	if ($global:amount -eq "check") {validation}
	$global:amount = "pass"
	
	echo "-------------------O" # Method 3
	echo "Special character removal"
	write-host "Researching ""$specChar""" -foregroundcolor yellow
	$global:strmsrch = $specChar
	streamsearch	
		
	$global:strmsrch = $old_name # Last resort
	streamsearch
	
	echo "=-=-=-=-=-=-=-=-=-=-=-=-=-="
	write-host "No live streamers detected." -foregroundcolor red
	echo "=-=-=-=-=-=-=-=-=-=-=-=-=-="
	echo "Search on $old_name has been terminated."
	
	if ($old_name -like "*twitch*") {$pass = "rocket"}
	if ($old_name -like "*live*") {$pass = "rocket"}
	if ($old_name -like "*tv*") {$pass = "rocket"}
	if ($old_name -like "*ttv*") {$pass = "rocket"}
	if ($names -contains "Search-Request") {$pass = "rocket"}
	
	if ($pass -eq "rocket") { # Peace of mind notification
	Add-Type -AssemblyName System.Windows.Forms
	$global:balmsg = New-Object System.Windows.Forms.NotifyIcon
	$path = (Get-Process -id $pid).Path
	$balmsg.Icon = [System.Drawing.Icon]::ExtractAssociatedIcon($path)
	$balmsg.BalloonTipIcon = [System.Windows.Forms.ToolTipIcon]::Error
	$balmsg.BalloonTipText = "Player ""$old_name"" is NOT live."
	$balmsg.BalloonTipTitle = "Stream Finder Plugin"
	$balmsg.Visible = $true
	$balmsg.ShowBalloonTip(60000)
	}
	++$x
	$pass = ''
	
	nameloop
}

function validation {
	write-host "Validation process..."	
	
	if ($name -match "twitch") { # Split "twitch" from player name
		$name = $name -split "twitch"
		echo 'twitch name split'
	}
	
	$name = $name.trim('')
	$name = $name.trim(' ')
	$name = $name | where{$_ -ne ""}
	$split_name00 = $name -split "_"
	$split_name01 = $old_name -split "\W"
	$y00 = $split_name00.count
	$y01 = $split_name01.count
	$y00 = $y00-1
	$y01 = $y01-1
	
	echo "No live streamer found, researching..."
	echo "Initiated split searching"
	research
}

function streamsearch { # The stream finder itself
	echo "Streamsearch function on $strmsrch"
	$requestRAW00 = Invoke-WebRequest -Headers (Get-AuthenticationHeaderTwitch) -UseBasicParsing -Uri https://api.twitch.tv/helix/search/channels?query=$strmsrch # Searching the player's name through twitch database
	$live_status = (ConvertFrom-Json ($requestRAW00)).Data -match "Rocket League" | select -property broadcaster_login, is_live, game_name | where{$_.is_live -match "True"} # Determining whether streamer is live or not

	if ($live_status -like "*True*") { # Stream information management function | Discord webhook operations
	$stream = $stream+1
	$old_name = $restore
	echo "=-=-=-=-=-=-=-=-=-=-=-=-=-="
	write-host "Live channel found! ===> $old_name" -foregroundcolor green
	start sound.vbs
	echo "Sound played ;)"
	WinBallon
	
	$started_at = (ConvertFrom-Json ($requestRAW00)).Data -match "Rocket League" | select -property broadcaster_login, is_live, game_name, started_at | where{$_.is_live -match "True"} | select started_at # For the timesum cmdlet
	$split00 = $live_status | select -expandproperty broadcaster_login # Isolating the broadcaster's name
	$trim00 = $split00 | out-string
	
	$power_version = $PSVersionTable.PSVersion | select -expandproperty Major # Get powershell version
	if ($power_version -eq "7") {Psv7} else {Psv5}
	
	echo "Stream started at $time00"
	echo "Current UTC time: $time01"
	$tsum = [datetime]$time01 -[datetime]$time00 # Time sum
	$t0 = $tsum | select -expandproperty hours # v Isolating outputs v
	$t1 = $tsum | select -expandproperty minutes
	$t2 = $tsum | select -expandproperty seconds
	$t3 = $tsum | select -expandproperty days
	if ($t3 -ne 0) { # If the streamer was streaming for more than 24 hours
		for ($x = 1;$x -le $t3;$x++) {
			write-host "Streamer has been live for more than 24 hours!"
			$t0 = $t0 + 24
		}
	}
	$char0 = $t0 | measure -character | select -expandproperty characters # Re-isolating / Reformating timesum outputs
	$char1 = $t1 | measure -character | select -expandproperty characters
	$char2 = $t2 | measure -character | select -expandproperty characters
	if ($char0 -eq 1 ) {$t0 = "0$t0"}
	if ($char1 -eq 1 ) {$t1 = "0$t1"}
	if ($char2 -eq 1 ) {$t2 = "0$t2"}
	$timestamp = "$t0`:$t1`:$t2" # Final 
	echo "timestamp: $timestamp"
	$twitch_username = $trim00.trim('')
	$trim02 = get-date | out-string # Log file
	$date = $trim02.trim('')
	$short_date = get-date -format "MM/dd/yyyy hh:mm:ss"
	echo "Player $old_name's twitch username => $twitch_username"
	
	# Additional Stream info for discord notification
	$requestRAW01 = Invoke-WebRequest -Headers (Get-AuthenticationHeaderTwitch) -UseBasicParsing -Uri "https://api.twitch.tv/helix/streams?user_login=$twitch_username"
	$title = (ConvertFrom-Json ($requestRAW01)).Data | select -expandproperty title
	$lang = (ConvertFrom-Json ($requestRAW01)).Data | select -expandproperty language
	$views = (ConvertFrom-Json ($requestRAW01)).Data | select -expandproperty viewer_count
	$user_id = (ConvertFrom-Json ($requestRAW01)).Data | select -expandproperty user_id
	$requestRAW02 = Invoke-WebRequest -Headers (Get-AuthenticationHeaderTwitch) -UseBasicParsing -Uri "https://api.twitch.tv/helix/videos?user_id=$user_id"
	$vod_check = (ConvertFrom-Json ($requestRAW02)).Data
	
	if ("" -eq $vod_check) {
		$vod = "False"
		$warning = "HURRY IN!"
	} else {
		$vod = "True"
		$warning = ""
	}
	$old_name =  $old_name -replace '[^a-zA-Z0-9\/.!@#$%^&*()-_=+?|;:]', ''
	
# v Discord Notification Message v
$url = "$discord_webhook"
$content00 = "**$old_name is LIVE on Twitch! $warning**" 
$content01 = "Title: $title"
$content02 = "Date found: $date" 
$content03 = "Timestamp: $timestamp"
$content04 = "Views: $views"
$content05 = "VODs: $vod"
$content06 = "Language: $lang"
$content07 = "Come say hi! ---> https://www.twitch.tv/$twitch_username"
$payload = [PSCustomObject]@{ 
content = "$content00
-------------------------------------------------------O
__*Stream Information*__
- $content01
- $content02
- $content03
- $content04
- $content05
- $content06
-------------------------------------------------------O
- $content07 " } 
# ^ Discord Notification Message ^
	
	write-host "Sending discord notification..."
	iwr -uri $url -method Post -body ($payload | ConvertTo-Json) -ContentType 'Application/Json' # Sending live notification to discord
	
	if ($discord_webhook -eq "*INSERT DISCORD WEBHOOK HERE*") {webhook_error} Else {
		echo "Discord notification sent!"
		echo "Discord url = $url"
	}
	
	$el_time = get-date -format "t"
	
	echo "$twitch_username was found live on $date" | Out-File -Append -Encoding Ascii "livestreamlog.txt"
	$global:Pom = @("$old_name is LIVE on twitch!",
					"Time found: $el_time",
					"Views: $views",
					"VODs: $vod",
					"------------------------------------------------------------O")
					
	# create function for streamlink
	
	if (gc $streamlink_path\streamlink.exe) {
	# Streamlink info
	echo "TwitchUsr=$twitch_username" | Out-File -Encoding Ascii "streamlink-info.txt"
	echo "TwitchLink=https://www.twitch.tv/$twitch_username" | Out-File -Append -Encoding Ascii "streamlink-info.txt"
	
	# Special Ses text
	echo "$twitch_username" | Out-File -Encoding Ascii "streamlink-session.txt"
	echo "Date: $short_date" | Out-File -Append -Encoding Ascii "streamlink-session.txt"
	echo "VODs: $vod" | Out-File -Append -Encoding Ascii "streamlink-session.txt"
	
	# For ImGui combo box
	echo "$twitch_username" | Out-File -Append -Encoding Ascii "Combo-Info00.txt"
	
	# Start Streamlink
	start-process record.vbs
	write-host "STREAMLINK RECORDING STARTED"
	}
	
	#End
	echo "Searched $old_name"
	echo "=-=-=-=-=-=-=-=-=-=-=-=-=-="
	echo "Search on $old_name has been terminated."
	$key00 = "blacklist"
	SessionBlacklist
	nameloop
	}
}

function ignore_string {
	if ($string_i -eq '') {$global:search_pass = "ignore"}
	if ($string_i -eq 'in') {$global:search_pass = "ignore"}
	if ($string_i -eq 'on') {$global:search_pass = "ignore"}
	if ($string_i -eq 'twitch') {$global:search_pass = "ignore"}
	if ($string_i -eq 'ttv') {$global:search_pass = "ignore"}
	if ($string_i -eq 'tv') {$global:search_pass = "ignore"}
	if ($string_i -eq 't') {$global:search_pass = "ignore"}
	if ($string_i -eq 'is') {$global:search_pass = "ignore"}
	if ($string_i -eq 'live') {$global:search_pass = "ignore"}
	if ($string_i -eq 'the') {$global:search_pass = "ignore"}
	if ($string_i -eq 'as') {$global:search_pass = "ignore"}
	if ($string_i -eq 'at') {$global:search_pass = "ignore"}
	if ($string_i -eq '_') {$global:search_pass = "ignore"}
	if ($string_i -eq ' ') {$global:search_pass = "ignore"}
	if ($string_i -eq 'of') {$global:search_pass = "ignore"}
	if ($string_i -eq 'RL') {$global:search_pass = "ignore"}
	if ($string_i -eq 'with') {$global:search_pass = "ignore"}
	if ($string_i -eq 'it') {$global:search_pass = "ignore"}
	if ($string_i -eq 'yt') {$global:search_pass = "ignore"}
	if ($string_i -eq 'tiktok') {$global:search_pass = "ignore"}
	if ($string_i -eq 'youtube') {$global:search_pass = "ignore"}
	if ($search_pass -eq "ignore") {echo "Search pass = ignore"} else {echo "Search pass = pass"}
}

function research { # Increase search accuracy
				
	for ($x = 0;$x -le $y01;$x++) { # Method 1
		$rcount = $x+1
		echo "-------------------O"
		echo "Split search instance #$rcount"
		echo "Method 1"
		$test_old_name = $split_name01 -split "\W" | select -index $x
		echo "Tested name: $test_old_name"
		$global:string_i = $test_old_name
		ignore_string
		if ($search_pass -eq "ignore") {
			echo "Unnecassary string to search"
			write-host "Skipping ""$test_old_name""" -foregroundcolor white
			$global:search_pass = "pass"
			continue
		}
		write-host "Researching ""$test_old_name""" -foregroundcolor yellow
		$global:strmsrch = $test_old_name
		streamsearch
	}
	
	for ($x = 0;$x -le $y00;$x++) { # Method 2
		$rcount = $x+1
		echo "-------------------O"
		echo "Split search instance #$rcount"
		echo "Method 2"
		$name = $split_name00 -split "_" | select -index $x
		echo "Tested name: $name"
		$global:string_i = $name
		ignore_string
		if ($search_pass -eq "ignore") {
			echo "Unnecassary string to search"
			write-host "Skipping ""$test_old_name""" -foregroundcolor white
			$global:search_pass = "pass"
			continue
		}
		write-host "Researching ""$name""" -foregroundcolor yellow
		$global:strmsrch = $name
		streamsearch
	}
}

function WinBallon { # Windows notification
	Add-Type -AssemblyName System.Windows.Forms
	$global:balmsg = New-Object System.Windows.Forms.NotifyIcon
	$path = (Get-Process -id $pid).Path
	$balmsg.Icon = [System.Drawing.Icon]::ExtractAssociatedIcon($path)
	$balmsg.BalloonTipIcon = [System.Windows.Forms.ToolTipIcon]::Warning
	$balmsg.BalloonTipText = "Player ""$old_name"" is LIVE on twitch!"
	$balmsg.BalloonTipTitle = "Stream Finder Plugin"
	$balmsg.Visible = $true
	$balmsg.ShowBalloonTip(60000)
}

function SessionBlacklist { # To prevent from repeadetely sending the same live stream notification
	$el_date = get-date -format "MM/dd/yy"
	$d00 = "Date: $el_date"
	if (-not(Test-Path -path $p\Session-Blacklist.txt)) { # Check if file exists
		echo "$d00" | Out-File -Encoding Ascii "Session-Blacklist.txt"
		write-host "Session blacklist created."
	}
	$session_blacklist = gc $p\Session-Blacklist.txt
	$d01 = $session_blacklist | select -index 0
	if (-not($d01 -eq $d00)) { # Check date
		echo "$d00"  | Out-File -Encoding Ascii "Session-Blacklist.txt"
		write-host "Session blacklist expired, list reset done."
		del "Combo-Info00.txt" -force
		$session_blacklist = gc $p\Session-Blacklist.txt -erroraction silentlycontinue
	}
	if ($session_blacklist -match $old_name) { # to prevent from sending a notification of the same live streamer for the entire day
	echo "Player: $old_name will be blacklisted until tomorrow"
	echo "Search skipped."
	nameloop
	}
	if ($key00 -eq "blacklist") {
	$Pom | Out-File -Append -Encoding Ascii "Session-Blacklist.txt"
	write-host "$old_name has been added to the session blacklist"
	}
}

function PSv5 { # Timestamp function for powershell version 5 and below
	$global:time_start = $started_at | select -expandproperty started_at # Receiving initial time stamp
	$global:split_time00 = $time_start -split "[A-Z]" | select -index 0
	$global:split_time01 = $time_start -split "[A-Z]" | select -index 1
	$global:time00 = ($split_time00,$split_time01) -join " "
	$global:time01 = [DateTime]::UtcNow | get-date -Format "yyyy-MM-dd HH:mm:ss" # Converting user's time to UTC time in order to correspond with Twitch's UTC time frame
}

function PSv7 { # Timestamp function for powershell version 7 and above
	$global:time00 = $started_at | select -expandproperty started_at # Receiving initial time stamp
	$global:time01 = [DateTime]::UtcNow | get-date -Format "MM/dd/yyyy HH:mm:ss" # Converting user's time to UTC time in order to correspond with Twitch's UTC time frame
}

nameloop
write-host "Bottom of script reached."
exit

# Extras

# $quotes = "``````"
# https://krookedskull.com/things-to-remember/powershell-accessing-the-twitch-api-v2/
# https://www.dennisrye.com/post/howto-get-twitch-streams-with-powershell/
# https://github.com/cog1to/libctwitch/blob/master/example/twitch-remote.c

# Jesus is Lord.
