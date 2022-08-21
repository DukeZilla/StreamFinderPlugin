set shell = wscript.createobject("wscript.shell")
appData = shell.ExpandEnvironmentStrings("%APPDATA%")
shell.run appData + "\bakkesmod\bakkesmod\data\StreamFinder\streamfinder.bat", 0
wscript.quit