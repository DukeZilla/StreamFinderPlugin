set shell = wscript.createobject("wscript.shell")
appData = shell.ExpandEnvironmentStrings("%APPDATA%")
file00 = Chr(34) & appData & "\bakkesmod\bakkesmod\data\StreamFinder\IfExistCheck.bat" & Chr(34)
shell.run file00, 0
wscript.quit
