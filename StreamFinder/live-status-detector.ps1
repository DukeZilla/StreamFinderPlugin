# ESSENTIAL COMPONENT FOR THE STREAM FINDER PLUGIN | ROCKET LEAGUE BAKKESMOD
# By P as in Papi

echo "Stream Finder Debug Log"

class TwitchAuthToken {
 [string]$tokenName = "Stream Finder Plugin"
 [string]$redirectURL = "https://localhost"
 [string]$clientID = "4xeemdr46tyggrk8nuwmcw1l44ln63"
 [string]$clientSecret = "7igcrwskulfppl0jozcm6j9q4qe72y"
 [string]$accessToken
 [datetime]$tokenCollected
 [datetime]$tokenExpiration
 [string]$tokenType = "bearer"
 [string]$authenticationHeader
}

Function global:Get-TwitchAccessToken {
$twitchUri = "https://id.twitch.tv/oauth2/token"
$twitchUri += '?client_id='+[uri]::EscapeDataString("4xeemdr46tyggrk8nuwmcw1l44ln63")
$twitchUri += '&client_secret='+[uri]::EscapeDataString("7igcrwskulfppl0jozcm6j9q4qe72y")
$twitchUri += '&grant_type=client_credentials'
$response = Invoke-WebRequest -Method Post -Uri $twitchUri -UseBasicParsing
$accessTokenImport = ConvertFrom-Json -InputObject $response.Content
[TwitchAuthToken]$twitchAccessToken = [TwitchAuthToken]::New()
$twitchAccessToken.accessToken = $accessTokenImport.access_token
$twitchAccessToken.tokenCollected = Get-date
$twitchAccessToken.tokenExpiration = (Get-date).AddSeconds($accessTokenImport.expires_in)
$twitchAccessToken.tokenType = $accessTokenImport.token_type
$twitchAccessToken.authenticationHeader = "Authorization: Bearer "+$accessTokenImport.access_token
return $twitchAccessToken
}

$twitchUri = "https://id.twitch.tv/oauth2/token"
$twitchUri += '?client_id='+[uri]::EscapeDataString("4xeemdr46tyggrk8nuwmcw1l44ln63")
$twitchUri += '&client_secret='+[uri]::EscapeDataString("7igcrwskulfppl0jozcm6j9q4qe72y")
$twitchUri += '&grant_type=client_credentials'

Function Get-AuthenticationHeaderTwitch {
$accessToken = Get-TwitchAccessToken
$authHeadTwitch = New-Object "System.Collections.Generic.Dictionary[[String],[String]]"
$authHeadTwitch = @{}
$authHeadTwitch.Add("Authorization","Bearer "+$accessToken.accesstoken)
$authHeadTwitch.Add("Client-ID", "4xeemdr46tyggrk8nuwmcw1l44ln63")
return $authHeadTwitch
}

iwr -method post -uri $twitchUri -UseBasicParsing
$i = -1

# Discord webhook linke to variable
$p = (pwd).path
$discord_webhook = gc $p\discord-webhook.txt
$test = gc $p\test-webhook.txt -erroraction silentlycontinue

function Test_Webhook { # For testing the discord webhook
$trim02 = get-date | out-string # Log file
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
write-host "Test Complete!"
del $p\test-webhook.txt
exit
}

if ($test -eq "Test-Webhook") {Test_Webhook}

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

echo "Stream search beginning."

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
	
	echo "----------------------------------------------------------0"
	echo "Instance #$i"
	
	# Null Value Check / Breaker
	$null_check = $name
    if ($null -eq $null_check) { # at the end of the list the loop will break
		echo "Search halted, reason: null value break"
		echo "Reached the end of name list."
		$names > blacklist.txt
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

		$name = $name -replace('-', '_')
		$name = $name -replace ('\W', '')

	# Breakers
	if ($i -eq  10) { # to prevent from looping more than necessary
		echo 'Search halted, reason: Max number break'
		$names > blacklist.txt
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
	
	streamsearch
	echo "Searched $old_name"
	echo "=-=-=-=-=-=-=-=-=-=-=-=-=-="
	
	$amount = "check"
	
	function validation {
		if ($amount -eq "pass") {break}
		
		if ($name -match "twitch") { # Split "twitch" from player name
			$name = $name -split "twitch"
			echo 'twitch name split'
		}
		
		$global:amount = "pass"
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
	
	if ($old_name -like "*twitch*") {validation}
	if ($old_name -like "*live*") {validation}
	if ($old_name -like "*ttv*") {validation}
	if ($old_name -like "*tv*") {validation}
	if ($old_name -like "*tiktok*") {validation}
	if ($old_name -like "*yt*") {validation}
	if ($old_name -like "*youtube*") {validation}
	if ($old_name -like "*t.tv*") {validation}
		
	$name = $old_name # Last resort
	streamsearch
	
	echo "=-=-=-=-=-=-=-=-=-=-=-=-=-="
	write-host "No live streamers detected." -foregroundcolor red
	echo "=-=-=-=-=-=-=-=-=-=-=-=-=-="
	echo "Search on $old_name has been terminated."
	
	if ($old_name -like "*twitch*") {$pass = "rocket"}
	if ($old_name -like "*live*") {$pass = "rocket"}
	if ($old_name -like "*tv*") {$pass = "rocket"}
	if ($old_name -like "*ttv*") {$pass = "rocket"}
	
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

function streamsearch { # The stream finder itself
$requestRAW00 = Invoke-WebRequest -Headers (Get-AuthenticationHeaderTwitch) -UseBasicParsing -Uri https://api.twitch.tv/helix/search/channels?query=$name
$live_status = (ConvertFrom-Json ($requestRAW00)).Data -match "Rocket League" | select -property broadcaster_login, is_live, game_name | where{$_.is_live -match "True"}
if ($live_status -like "*True*") { # Discord Bot notification operations \ webhooks
	$old_name = $restore
	echo "=-=-=-=-=-=-=-=-=-=-=-=-=-="
	write-host "Live channel found! ===> $old_name" -foregroundcolor green
	start sound.vbs
	echo "Sound played ;)"
	WinBallon
	
	$started_at = (ConvertFrom-Json ($requestRAW00)).Data -match "Rocket League" | select -property broadcaster_login, is_live, game_name, started_at | where{$_.is_live -match "True"} | select started_at
	$split00 = $live_status | select -expandproperty broadcaster_login # Isolating the broadcaster's name
	$trim00 = $split00 | out-string
	
	$power_version = $PSVersionTable.PSVersion | select -expandproperty Major
	if ($power_version -eq "7") {Psv7} else {Psv5}
	
	echo "Stream started at $time00"
	echo "Current UTC time: $time01"
	$tsum = [datetime]$time01 -[datetime]$time00 # Time sum
	$t0 = $tsum | select -expandproperty hours # Isolating outputs
	$t1 = $tsum | select -expandproperty minutes
	$t2 = $tsum | select -expandproperty seconds
	$t3 = $tsum | select -expandproperty days
	if ($t3 -ne 0) { # If the streamer was streaming for more than 24 hours
	for ($x = 1;$x -le $t3;$x++) {
		write-host "Streamer has been live for more than 24 hours!"
		$t0 = $t0 + 24
		}
	}
	$char0 = $t0 | measure -character | select -expandproperty characters
	$char1 = $t1 | measure -character | select -expandproperty characters
	$char2 = $t2 | measure -character | select -expandproperty characters
	if ($char0 -eq 1 ) {$t0 = "0$t0"}
	if ($char1 -eq 1 ) {$t1 = "0$t1"}
	if ($char2 -eq 1 ) {$t2 = "0$t2"}
	$timestamp = "$t0`:$t1`:$t2"
	echo "timestamp: $timestamp"
	$twitch_username = $trim00.trim('')
	$trim02 = get-date | out-string # Log file
	$date = $trim02.trim('')
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
		$warning = "Hurry in!"
	} else {
		$vod = "True"
		$warning = ""
	}
	
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

	iwr -uri $url -method Post -body ($payload | ConvertTo-Json) -ContentType 'Application/Json' # Sending live notification to discord
	if ($discord_webhook -eq "*INSERT DISCORD WEBHOOK HERE*") {webhook_error} Else {
		echo "Discord notification sent!"
		echo "Discord url = $url"
	}
	
	echo "$twitch_username was found live on $date" >> livestreamlog.txt
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
	if ($string_i -eq '[a-z]') {$global:search_pass = "ignore"}
	if ($string_i -eq '[0-9]') {$global:search_pass = "ignore"}
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
	$d00 = get-date -format "dd"
	if (-not(Test-Path -path $p\Session-Blacklist.txt)) { # Check if file exists
		$d00 > Session-Blacklist.txt
		write-host "Session blacklist created."
	}
	$session_blacklist = gc $p\Session-Blacklist.txt
	$d01 = $session_blacklist | select -index 0
	if (-not($d01 -eq $d00)) { # Check date
		$d00 > Session-Blacklist.txt
		write-host "Session blacklist expired, list reset done."
		$session_blacklist = gc $p\Session-Blacklist.txt -erroraction silentlycontinue
	}
	if ($session_blacklist -contains $old_name) { # to prevent from sending a notification of the same live streamer for the entire day
	echo "Player: $old_name will be blacklisted until tomorrow"
	echo "Search skipped."
	nameloop
	}
	if ($key00 -eq "blacklist") {
	echo "$old_name" >> Session-Blacklist.txt
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
exit

# Extras

# $quotes = "``````"
# https://krookedskull.com/things-to-remember/powershell-accessing-the-twitch-api-v2/
# https://www.dennisrye.com/post/howto-get-twitch-streams-with-powershell/
# https://github.com/cog1to/libctwitch/blob/master/example/twitch-remote.c

# Jesus is Lord.
