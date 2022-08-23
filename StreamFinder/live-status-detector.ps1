# STREAM FINDER PLUGIN FOR BAKKESMOD ROCKET LEAGUE

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

if ($discord_webhook -eq $null) {
Add-Type -AssemblyName System.Windows.Forms
$global:balmsg = New-Object System.Windows.Forms.NotifyIcon
$path = (Get-Process -id $pid).Path
$balmsg.Icon = [System.Drawing.Icon]::ExtractAssociatedIcon($path)
$balmsg.BalloonTipIcon = [System.Windows.Forms.ToolTipIcon]::Warning
$balmsg.BalloonTipText = 'Discord webhook not found, in order to get notifications of live streamers, include a webhook link in the discord-webhook.txt file.'
$balmsg.BalloonTipTitle = "Stream Finder Plugin Error"
$balmsg.Visible = $true
$balmsg.ShowBalloonTip(60000)
}

echo "Stream search beginning."
function nameloop {
	$i++
	
	# Name search refinement
	$names = gc $p\names.txt
	$old_name = $names | select -index $i
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
	#if ($name -match "live") { # Split "live" from player name
	#	$name = $name -split "\slive\s"
	#	echo 'live name split'
	#}
	#if ($name -match "twitch") { # Split "twitch" from player name
	#	$name = $name -split "\stwitch\s"
	#	echo 'live name split'
	#}
	echo "Tested name = ""$name"""
	# Breakers
	if ($i -eq  10) { # to prevent from looping more than necessary
		echo 'Search halted, reason: Max number break'
		$names > blacklist.txt
		echo "----------------------------------------------------------0"
		echo "Stream search ended."
		break
	}
	if ($perma_blacklist -contains $old_name) { # to prevent from sending a notification of the same live streamer for one game
		echo "Player name $old_name is permanetaly blacklisted"
		echo "Search skipped."
		nameloop
	}
	if ($blacklist -contains $old_name) { # to prevent from sending a notification of the same live streamer for one game
		echo "Player name $old_name has been temporarily blacklisted"
		echo "Search skipped."
		nameloop
	}
	
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
	
	if ($status -eq "research") {
		echo "Initiated split searching"
		for ($x = 0;$x -le $y;$x++) {
			$rcount = $x+1
			echo "Split search instance #$rcount"
			$name = $split_name -split "_" | select -index $x
			write-host "Researching $name" -foregroundcolor yellow
			streamsearch
		}
	}
	write-host "No live streamers detected." -foregroundcolor red
	echo "=-=-=-=-=-=-=-=-=-=-=-=-=-="
	echo "Search on $old_name has been terminated."
	nameloop
}

function streamsearch {
	$requestRAW = Invoke-WebRequest -Headers (Get-AuthenticationHeaderTwitch) -UseBasicParsing -Uri https://api.twitch.tv/helix/search/channels?query=$name
	$live_status = (ConvertFrom-Json ($requestRAW)).Data -match "Rocket League" | select -property broadcaster_login, is_live, game_name | where{$_.is_live -match "True"}
	if ($live_status -like "*True*") { # Discord Bot send notifications via webhook to server
		write-host "Live streamer found! => $old_name" -foregroundcolor green
		$split00 = $live_status | select -expandproperty broadcaster_login # Isolating the broadcaster's name
		$trim00 = $split00 | out-string
		$twitch_username = $trim00.trim('')
		echo "Player $old_name's twitch username => $twitch_username"
		$url = "$discord_webhook"
		$content = "Player $old_name is Live on Twitch! Come say hi. https://www.twitch.tv/$twitch_username"
		$payload = [PSCustomObject]@{ # Sending live notification to discord
		content = $content
		}
		iwr -uri $url -method Post -body ($payload | ConvertTo-Json) -ContentType 'Application/Json'
		echo "Discord notification sent!"
		echo "Discord url = $url"
		$trim01 = get-date | out-string # Log file
		$date = $trim01.trim('')
		echo "Streamer $twitch_username was found live on $date" >> livestreamlog.txt
		echo "Searched $old_name"
		echo "Search on $old_name has been terminated."
		nameloop
	}
}

nameloop
exit

# LINKS

# client id: 4xeemdr46tyggrk8nuwmcw1l44ln63
# secret id: 7igcrwskulfppl0jozcm6j9q4qe72y

# https://krookedskull.com/things-to-remember/powershell-accessing-the-twitch-api-v2/
# https://www.dennisrye.com/post/howto-get-twitch-streams-with-powershell/
# https://github.com/cog1to/libctwitch/blob/master/example/twitch-remote.c

# cd "C:\Users\dan_r\Desktop\Script Center\Projects\StreamFinderPlugin"
