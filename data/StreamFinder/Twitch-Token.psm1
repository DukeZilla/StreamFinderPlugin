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