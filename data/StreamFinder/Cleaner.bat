echo off
break off
title Cleaning Lady
PUSHD "%CD%"
CD /D "%~dp0"
goto main

:main
IF EXIST cache (echo "Cache folder exists") else (mkdir cache)
move a-path.txt cache
move blacklist-log.txt cache
move blacklist.txt cache
move botlist.txt cache
move Combo-Info00.txt cache
move detector-transcript.txt cache
move discord-webhook.txt cache
move file.txt cache
move livestreamlog.txt cache
move names.txt cache
move Path-ffmpeg.txt cache
move Path-streamlink.txt cache
move PeaceOfMind.txt cache
move permanent-blacklist.txt cache
move rec-path.txt cache
move rec-status.txt cache
move Session-Blacklist.txt cache
move streamlink-info.txt cache
move streamlink-session.txt cache
echo Me terminar, gracias!
timeout 1 > nul
exit