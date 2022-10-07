# STREAM FINDER PLUGIN FOR BAKKESMOD ROCKET LEAGUE 
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
$p = (pwd).path
$i = -1

Get-Content discord-webhook.txt | Foreach-Object{
	$discord_webhook = $_.Split('')
	New-Variable -Name $discord_webhook[0] -Value $discord_webhook[1]
}

function webhook_error {
Add-Type -AssemblyName System.Windows.Forms
$global:balmsg = New-Object System.Windows.Forms.NotifyIcon
$path = (Get-Process -id $pid).Path
$balmsg.Icon = [System.Drawing.Icon]::ExtractAssociatedIcon($path)
$balmsg.BalloonTipIcon = [System.Windows.Forms.ToolTipIcon]::Warning
$balmsg.BalloonTipText = 'Discord webhook not found, recommended that you include a discord webhook link in the discord-webhook.txt file.'
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
		break
	}
	
	$name = $name.replace(" ", "_")
	
	if ($name -match "ttv") { # Split "ttv" from player name
		$name = $name -split "ttv"
		echo 'ttv name split'
	}
		$name = $name -replace('-', '_')
		$name = $name -replace ('\W', '')

	# Breakers
	if ($i -eq  10) { # to prevent from looping more than necessary
		echo 'Search halted, reason: Max number break'
		$names > blacklist.txt
		echo "----------------------------------------------------------0"
		echo "Stream search ended."
		break
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
		echo "$old_name has been found as a bot"
		echo "Search skipped."
		nameloop
	}
	
	echo "Tested name = ""$name"""

	$key00 = "null"
	SessionBlacklist
	
	streamsearch
	echo "Searched $old_name"
	echo "=-=-=-=-=-=-=-=-=-=-=-=-=-="
	$name = $name.trim('')
	$name = $name.trim(' ')
	$name = $name | where{$_ -ne ""}
	$split_name = $name -split "_"
	$y = $split_name.count
	
	if ($y -ne 1) {
		$status = "research"
	} else {
		$status = "skip"
	}
	$y = $y-1
	
	$ignore = @('in',
				'on',
				'twitch',
				'ttv',
				'is',
				'live',
				'tv',
				'the',
				'as',
				'at',
				'_',
				'',
				' ',
				'of')
	
	if ($status -eq "research") {
		echo "No live streamer found, researching..."
		echo "Initiated split searching"
		research
		}
	$name = $old_name # Last resort
	streamsearch
	echo "=-=-=-=-=-=-=-=-=-=-=-=-=-="
	write-host "No live streamers detected." -foregroundcolor red
	echo "=-=-=-=-=-=-=-=-=-=-=-=-=-="
	echo "Search on $old_name has been terminated."
	
	if ($old_name -like "*twitch*") {$pass = "rocket"}
	if ($old_name -like "*tv*") {$pass = "rocket"}
	if ($old_name -like "*ttv*") {$pass = "rocket"}
	
	if ($pass -eq "rocket") { # Peace of mind notification
	Add-Type -AssemblyName System.Windows.Forms
	$global:balmsg = New-Object System.Windows.Forms.NotifyIcon
	$path = (Get-Process -id $pid).Path
	$balmsg.Icon = [System.Drawing.Icon]::ExtractAssociatedIcon($path)
	$balmsg.BalloonTipIcon = [System.Windows.Forms.ToolTipIcon]::Error
	$balmsg.BalloonTipText = "Player ""$old_name"" is NOT live on twitch."
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
	echo "=-=-=-=-=-=-=-=-=-=-=-=-=-="
	write-host "Live channel found! ===> $old_name" -foregroundcolor green
	start sound.vbs
	echo "Sound played ;)"
	WinBallon
	
	$started_at = (ConvertFrom-Json ($requestRAW00)).Data -match "Rocket League" | select -property broadcaster_login, is_live, game_name, started_at | where{$_.is_live -match "True"} | select started_at
	$split00 = $live_status | select -expandproperty broadcaster_login # Isolating the broadcaster's name
	$trim00 = $split00 | out-string
	$time00 = $started_at | select -expandproperty started_at # Receiving initial time stamp
	echo "Stream started at $time00"
	$time01 = [DateTime]::UtcNow | get-date -Format "MM/dd/yyyy HH:mm:ss" # Converting user's time to UTC time in order to correspond with Twitch's UTC time frame
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
	if ($char0 -eq 1 ) {
	$t0 = "0$t0"
	}
	if ($char1 -eq 1 ) {
	$t1 = "0$t1"
	}
	if ($char2 -eq 1 ) {
	$t2 = "0$t2"
	}
	
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
	if ($discord_webhook -eq "*INSERT DISCORD WEBHOOK HERE*") {
		webhook_error
	} Else {
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

function research { # Increase search accuracy
	for ($x = 0;$x -le $y;$x++) {
		$rcount = $x+1
		echo "--------------O"
		echo "Split search instance #$rcount"
		$name = $split_name -split "_" | select -index $x
		if ($ignore -eq $name) {
			echo "Unnecassary string to search"
			echo "Skipping ""$name"""
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
		write-host "Session blacklist expired, file has been recreated."
		$session_blacklist = gc $p\Session-Blacklist.txt -erroraction silentlycontinue
	}
	if ($session_blacklist -contains $old_name) { # to prevent from sending a notification of the same live streamer for the entire day
	echo "Player: $old_name will be blacklisted until tomorrow"
	echo "Search skipped."
	nameloop
	}
	if ($key00 -eq "blacklist") {
	echo "$old_name" >> Session-Blacklist.txt
	write-host "$old_name was added to the session blacklist"
	}
}

nameloop
exit

# Extras

# $quotes = "``````"
# https://krookedskull.com/things-to-remember/powershell-accessing-the-twitch-api-v2/
# https://www.dennisrye.com/post/howto-get-twitch-streams-with-powershell/
# https://github.com/cog1to/libctwitch/blob/master/example/twitch-remote.c

# Jesus is Lord.
