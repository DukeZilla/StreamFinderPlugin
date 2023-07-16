# COMPONENT FOR THE STREAM FINDER PLUGIN | ROCKET LEAGUE BAKKESMOD
# By P as in Papi

echo "Stream Finder Logger | Version 0.97.1"

Import-Module C:Twitch-Token.psm1

$p = (pwd).path
$i = -1

function lobbylog {
	$i++
	
	# Name search refinement
	$names = gc $p\cache\names.txt
	$botlist = gc $p\cache\botlist.txt
	$old_name = $names | select -index $i
	$session_blacklist = gc $p\cache\Session-Blacklist.txt -erroraction silentlycontinue
	$blacklist = gc $p\cache\blacklist-log.txt -erroraction silentlycontinue
	$perma_blacklist = gc $p\cache\permanent-blacklist.txt -erroraction silentlycontinue
	$name = $names | select -index $i
	
	echo "----------------------------------------------------------0"
	echo "Instance #$i"
	
	# Null Value Check / Breaker
	$null_check = $name
    if ($null -eq $null_check) { # at the end of the list the loop will break
		echo "Search halted, reason: null value break"
		echo "Reached the end of name list."
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
		$name = $name.trim() # Remove blank space
		$name = $name | out-string
		$name = $name.trim() # Remove blank space
		$global:specChar = $name -replace '[^a-zA-Z0-9\/!.@#$%^&*()-_=+?|;:]', '' # Removing unique special characters

	# Breakers
	if ($i -eq  10) { # to prevent from looping more than necessary
		echo 'Search halted, reason: Max number break'
		echo "----------------------------------------------------------0"
		echo "Stream log ended."
		exit
	}

	if ($blacklist -contains $old_name) { # to prevent from sending a notification of the same live streamer for one game
		echo "$old_name has been temporarily blacklisted"
		lobbylog
	}
	
	
	if ($i -eq 0) {
		echo "i is equal to 0"
		$time = get-date -format "t"
		echo "Time of logging: $time" | Out-File -Encoding Ascii "cache\PeaceOfMind.txt"
	}
	
	if ($botlist -contains $old_name) { # to prevent from searching bots
		echo "$old_name has been identified as a bot" | Out-File -Append -Encoding Ascii "cache\PeaceOfMind.txt"
		lobbylog
	}
	
	if ($perma_blacklist -contains $old_name) { # to prevent from sending a notification of the same live streamer forever
		echo "$old_name is permanetaly blacklisted"
		lobbylog
	}
	
	if ($null -eq $old_name) {
	echo "No live streamers found." | Out-File -Append -Encoding Ascii "cache\PeaceOfMind.txt"
	lobbylog
	}
	
	echo "Old name:$old_name"
	echo "Name:$name"
	echo "SpecChar:$specChar"
	
	$requestRAW03 = Invoke-WebRequest -Headers (Get-AuthenticationHeaderTwitch) -UseBasicParsing -Uri "https://api.twitch.tv/helix/users?login=$specChar"
	$id = (ConvertFrom-Json ($requestRAW03)).Data | select -expandproperty id
	if ($null -eq $id) {
	echo "$old_name has no twitch channel found" | Out-File -Append -Encoding Ascii "cache\PeaceOfMind.txt"
	lobbylog
	}
	
	$requestRAW00 = Invoke-WebRequest -Headers (Get-AuthenticationHeaderTwitch) -UseBasicParsing -Uri "https://api.twitch.tv/helix/search/channels?query=$specChar" # Searching the player's name through twitch database
	$live_status = (ConvertFrom-Json ($requestRAW00)).Data -match "Rocket League" | select -property broadcaster_login, is_live, game_name | where{$_.is_live -match "True"} # Determining whether streamer is live or not
	if ($live_status -like "*True*") { # Stream information management function | Discord webhook operations
	echo "$old_name is LIVE on Twitch!" | Out-File -Append -Encoding Ascii "cache\PeaceOfMind.txt"
	lobbylog
	} else {
	echo "$old_name is not live on twitch." | Out-File -Append -Encoding Ascii "cache\PeaceOfMind.txt"
	lobbylog
	}

	$requestRAW04 = Invoke-WebRequest -Headers (Get-AuthenticationHeaderTwitch) -UseBasicParsing -Uri "https://api.twitch.tv/helix/videos?user_id=$id"
	$json_request = (ConvertFrom-Json ($requestRAW04)).Data | select -expandproperty published_at
	$last_live = $json_request | select -index 0
	$date_now = [DateTime]::UtcNow
	$time_sum = [datetime]$date_now -[datetime]$last_live
	$tD = $time_sum | select -expandproperty days

	if ($tD -ne 0) {
	echo "$old_name was last live $tD days ago" | Out-File -Append -Encoding Ascii "cache\PeaceOfMind.txt"
	lobbylog
	}

	$tM = $time_sum | select -expandproperty minutes
	if ($tM -ne 0) {
	echo "$old_name was last live $tM minutes ago" | Out-File -Append -Encoding Ascii "cache\PeaceOfMind.txt"
	lobbylog
	}
	lobbylog
}

lobbylog
exit