// Plugin created by P as in Papi
// Credit to the bakkesmod programming community for help!
#include "pch.h"
#include "StreamFinderPlugin.h"
#include <windows.h>  
#include <iostream>
#include <string>
#include <tchar.h>
#include <fstream>
#include <shlobj_core.h>
//#include <Lmcons.h>

using namespace std;

bool StreamFinderEnabled = true;

BAKKESMOD_PLUGIN(StreamFinderPlugin, "Stream Finder Plugin", plugin_version, PERMISSION_ALL)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void StreamFinderPlugin::onLoad()
{
	_globalCvarManager = cvarManager;
	cvarManager->log("Hello I'm CoolPlugin B)");
	cvarManager->registerCvar("stream_finder_enabled", "1", "Enable Plugin", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		StreamFinderEnabled = cvar.getBoolValue();
			});
	cvarManager->log("Stream Finder Plugin loaded!");
	//gameWrapper->LoadToastTexture("sfimg", gameWrapper->GetDataFolder() / "StreamFinder" / "stream_finder_icon.png");
	cvarManager->registerNotifier("sf_toast", [this](std::vector<std::string> args) {
	gameWrapper->Toast("Stream Finder Plugin", "Plugin is active!", "sfimg", 5.0, ToastType_OK);
	}, "", PERMISSION_ALL);
	cvarManager->executeCommand("cool_toast");
	this->LoadHooks();

	// For initiating the stream detector
	string str00 = "set shell = wscript.createobject(\"wscript.shell\")";
	string str01 = "appData = shell.ExpandEnvironmentStrings(\"%APPDATA%\")";
	string str02 = "shell.run appData + \"\\bakkesmod\\bakkesmod\\data\\StreamFinder\\streamfinder.bat\", 0";
	string str03 = "wscript.quit";
	ofstream outfile00;
	outfile00.open("C:\\Windows\\Temp\\stream-finder.vbs");
	outfile00 << str00 << endl;
	outfile00 << str01 << endl;
	outfile00 << str02 << endl;
	outfile00 << str03 << endl;
	outfile00.close();

	// For opening the stream finder directory
	string str04 = "set shell = wscript.createobject(\"wscript.shell\")";
	string str05 = "appData = shell.ExpandEnvironmentStrings(\"%APPDATA%\")";
	string str06 = "shell.run appData + \"\\bakkesmod\\bakkesmod\\data\\StreamFinder\"";
	string str07 = "wscript.quit";
	ofstream outfile01;
	outfile01.open("C:\\Windows\\Temp\\directory.vbs");
	outfile01 << str04 << endl;
	outfile01 << str05 << endl;
	outfile01 << str06 << endl;
	outfile01 << str07 << endl;
	outfile01.close();

	// For the update button
	string str08 = "set shell = wscript.createobject(\"wscript.shell\")";
	string str09 = "appData = shell.ExpandEnvironmentStrings(\"%APPDATA%\")";
	string str10 = "shell.run appData + \"\\bakkesmod\\bakkesmod\\data\\StreamFinder\\update.bat\", 0";
	string str11 = "wscript.quit";
	ofstream outfile02;
	outfile02.open("C:\\Windows\\Temp\\update.vbs");
	outfile02 << str08 << endl;
	outfile02 << str09 << endl;
	outfile02 << str10 << endl;
	outfile02 << str11 << endl;
	outfile02.close();

	// For crash prevention
	string str12 = "set shell = wscript.createobject(\"wscript.shell\")";
	string str13 = "appData = shell.ExpandEnvironmentStrings(\"%APPDATA%\")";
	string str14 = "shell.run appData + \"\\bakkesmod\\bakkesmod\\data\\StreamFinder\\IfExistCheck.bat\", 0";
	string str15 = "wscript.quit";
	ofstream outfile03;
	outfile03.open("C:\\Windows\\Temp\\IfExistCheck.vbs");
	outfile03 << str12 << endl;
	outfile03 << str13 << endl;
	outfile03 << str14 << endl;
	outfile03 << str15 << endl;
	outfile03.close();

	STARTUPINFO startupInfo;
	PROCESS_INFORMATION pi;
	memset(&startupInfo, 0, sizeof(STARTUPINFO));
	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.wShowWindow = false;
	// Get path for each computer, non-user specific and non-roaming data.
	// Append product-specific path
	TCHAR tcsCommandLine[] = _T("start ""C:\\Windows\\Temp\\IfExistCheck.vbs""");
	CreateProcessW(L"C:\\Windows\\System32\\wscript.exe", tcsCommandLine, NULL, NULL, TRUE, 0, NULL, NULL, (LPSTARTUPINFOW)&startupInfo, &pi);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	// This solution is used to prevent the program from kicking the player out of the Rocket League window.

	//char username[UNLEN+1];
	//DWORD username_len = UNLEN+1;
	//GetUserName(username, &username_len);

	//auto cvar = cvarManager->registerCvar("template_cvar", "hello-cvar", "just a example of a cvar");
	//auto cvar2 = cvarManager->registerCvar("template_cvar2", "0", "just a example of a cvar with more settings", true, true, -10, true, 10 );

	//cvar.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
	//	cvarManager->log("the cvar with name: " + cvarName + " changed");
	//	cvarManager->log("the new value is:" + newCvar.getStringValue());
	//});

	//cvar2.addOnValueChanged(std::bind(&StreamFinderPlugin::YourPluginMethod, this, _1, _2));

	// enabled decleared in the header
	//enabled = std::make_shared<bool>(false);
	//cvarManager->registerCvar("TEMPLATE_Enabled", "0", "Enable the TEMPLATE plugin", true, true, 0, true, 1).bindTo(enabled);

	//cvarManager->registerNotifier("NOTIFIER", [this](std::vector<std::string> params){FUNCTION();}, "DESCRIPTION", PERMISSION_ALL);
	//cvarManager->registerCvar("CVAR", "DEFAULTVALUE", "DESCRIPTION", true, true, MINVAL, true, MAXVAL);//.bindTo(CVARVARIABLE);
	//gameWrapper->HookEvent("FUNCTIONNAME", std::bind(&TEMPLATE::FUNCTION, this));
	//gameWrapper->HookEventWithCallerPost<ActorWrapper>("FUNCTIONNAME", std::bind(&StreamFinderPlugin::FUNCTION, this, _1, _2, _3));
	//gameWrapper->RegisterDrawable(bind(&TEMPLATE::Render, this, std::placeholders::_1));


	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", [this](std::string eventName) {
	//	cvarManager->log("Your hook got called and the ball went POOF");
	//});
	// You could also use std::bind here
	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", std::bind(&StreamFinderPlugin::YourPluginMethod, this);

}

void StreamFinderPlugin::LoadHooks()
{
gameWrapper->HookEvent("Function GameEvent_TA.Countdown.BeginState", std::bind(&StreamFinderPlugin::HandleGameStart, this, std::placeholders::_1)); // Start of countdown
gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded", std::bind(&StreamFinderPlugin::ClearList, this, std::placeholders::_1)); // End of game
//gameWrapper->HookEvent("Function GameEvent_Soccar_TA.Active.StartRound", std::bind(&StreamFinderPlugin::HandleGameStart, this, std::placeholders::_1)); // Start of kickoff
gameWrapper->HookEvent("Function OnlineGameJoinGame_X.JoiningBase.IsJoiningGame", std::bind(&StreamFinderPlugin::ClearList, this, std::placeholders::_1)); // State of joining a game
//gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.BeginState", std::bind(&StreamFinderPlugin::HandleGameStart, this, std::placeholders::_1)); // Start of goal replay
//gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnOvertimeUpdated", std::bind(&StreamFinderPlugin::HandleGameStart, this, std::placeholders::_1)); // Start of overtime
//gameWrapper->HookEvent("Function TAGame.GameEvent_TA.AddCar", std::bind(&StreamFinderPlugin::HandlePlayerAdded, this, std::placeholders::_1)); // Called when a car spawns
//gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnAllTeamsCreated", std::bind(&StreamFinderPlugin::HandleGameStart, this, std::placeholders::_1)); // Once all teams are formed
//gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded", std::bind(&StreamFinderPlugin::HandleGameEnd, this, std::placeholders::_1)); // Called when the match ends
//gameWrapper->HookEvent("Function TAGame.GFxShell_TA.LeaveMatch", std::bind(&StreamFinderPlugin::HandleGameLeave, this, std::placeholders::_1)); // Called when you leave the match
gameWrapper->HookEvent("Function TAGame.GFxData_MainMenu_TA.MainMenuAdded", std::bind(&StreamFinderPlugin::HandleMainMenu, this, std::placeholders::_1)); // Called when you enter the menu
}

std::vector<std::string> StreamFinderPlugin::GetPlayersNames()
{
	std::vector<std::string> names;

	ServerWrapper server = gameWrapper->GetCurrentGameState();
	if (server)
	{
		auto pris = server.GetPRIs();
		for (auto pri : pris) { //for each pri in pris
			if (!pri) continue;
			names.push_back(pri.GetPlayerName().ToString()); //add the player name to the list (names)
		};
	}

	return names; //then it returns the list
}

void StreamFinderPlugin::HandleGameStart(std::string eventName)
{
	std::vector<std::string> playersNames = GetPlayersNames(); //playersNames equals to what returns GetPlayersNames()
	std::ofstream stream(gameWrapper->GetDataFolder() / "StreamFinder" / "names.txt");
	for (std::string name : playersNames)
	{
		cvarManager->log(name);
		stream << name << std::endl;
	}
	stream.close();

	STARTUPINFO startupInfo;
	PROCESS_INFORMATION pi;
	memset(&startupInfo, 0, sizeof(STARTUPINFO));
	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.wShowWindow = false;
	// Get path for each computer, non-user specific and non-roaming data.
	// Append product-specific path
	TCHAR tcsCommandLine[] = _T("start ""C:\\Windows\\Temp\\stream-finder.vbs""");
	CreateProcessW(L"C:\\Windows\\System32\\wscript.exe", tcsCommandLine, NULL, NULL, TRUE, 0, NULL, NULL, (LPSTARTUPINFOW)&startupInfo, &pi);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	cvarManager->log("Stream Detector Launched.");
	// This solution is used to prevent the program from kicking the player out of the Rocket League window.
}

void StreamFinderPlugin::ClearList(std::string eventName)
{
	std::ofstream bl(gameWrapper->GetDataFolder() / "StreamFinder" / "blacklist.txt");
	bl << " " << std::endl;
}

void StreamFinderPlugin::HandleMainMenu(std::string eventName)
{
	cvarManager->executeCommand("sf_toast");
}


void StreamFinderPlugin::onUnload()
{
}


// https://cplusplus.com/doc/tutorial/files/
// https://stackoverflow.com/questions/224225/create-an-application-without-a-window
