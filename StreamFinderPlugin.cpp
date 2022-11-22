// Plugin created by P as in Papi
// Credit to the bakkesmod programming community for help!
#include "pch.h"
#include "StreamFinderPlugin.h"
#include <windows.h>  
#include <iostream>
#include <string>
#include <tchar.h>
#include "stdio.h";
#include <fstream>
#include <shlobj_core.h>
#include "ImGui/imgui_internal.h"
#include <Lmcons.h>
#include "tlhelp32.h"

using namespace std;

bool StreamFinderEnabled = true;

BAKKESMOD_PLUGIN(StreamFinderPlugin, "Stream Finder Plugin", plugin_version, PERMISSION_ALL)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void StreamFinderPlugin::onLoad()
{
	_globalCvarManager = cvarManager;
	cvarManager->registerCvar("stream_finder_enabled", "1", "Enable Plugin", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		StreamFinderEnabled = cvar.getBoolValue();
			});

	cvarManager->log("Stream Finder Plugin loaded!");
	gameWrapper->LoadToastTexture("sfimg", gameWrapper->GetDataFolder() / "StreamFinder" / "stream_finder_icon.png");
	cvarManager->registerNotifier("sf_toast", [this](std::vector<std::string> args) {
	gameWrapper->Toast("Stream Finder Plugin", "Plugin is active!", "sfimg", 5.0, ToastType_OK);
	}, "", PERMISSION_ALL);
	cvarManager->registerNotifier("rec_toast", [this](std::vector<std::string> args) {
		gameWrapper->Toast("Stream Finder Recorder", "A recording is still in progress!", "sfimg", 5.0, ToastType_Error);
		}, "", PERMISSION_ALL);
	cvarManager->executeCommand("sf_toast");

	this->LoadHooks();
	
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION pi;
	memset(&startupInfo, 0, sizeof(STARTUPINFO));
	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.wShowWindow = false;
	// Get path for each computer, non-user specific and non-roaming data.
	// Append product-specific path
	wchar_t* w_app_data_path;
	size_t sz = 0;
	errno_t err = _wdupenv_s(&w_app_data_path, &sz, L"APPDATA");
	wchar_t tcsCommandLine[2048]{ 0 };
	wsprintfW(tcsCommandLine, L"start \"%s\\bakkesmod\\bakkesmod\\data\\StreamFinder\\FileCheck.vbs\"", w_app_data_path);
	free(w_app_data_path);
	CreateProcessW(L"C:\\Windows\\System32\\wscript.exe", tcsCommandLine, nullptr, nullptr, FALSE, 0, nullptr, nullptr, (LPSTARTUPINFOW)&startupInfo, &pi);
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

	//enabled decleared in the header
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
gameWrapper->HookEvent("Function OnlineGameJoinGame_X.JoiningBase.IsJoiningGame", std::bind(&StreamFinderPlugin::Refresh, this, std::placeholders::_1)); // State of joining a game
gameWrapper->HookEvent("Function TAGame.GFxShell_TA.LeaveMatch", std::bind(&StreamFinderPlugin::Refresh, this, std::placeholders::_1)); // Called when you leave the match
gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded", std::bind(&StreamFinderPlugin::Refresh, this, std::placeholders::_1)); // End of game
gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded", std::bind(&StreamFinderPlugin::RecNotif, this, std::placeholders::_1)); // End of game
//gameWrapper->HookEvent("Function TAGame.GFxData_MainMenu_TA.MainMenuAdded", std::bind(&StreamFinderPlugin::Refresh, this, std::placeholders::_1)); // Called when you enter the menu
gameWrapper->HookEvent("Function TAGame.GFxData_MainMenu_TA.MainMenuAdded", std::bind(&StreamFinderPlugin::RecNotif, this, std::placeholders::_1)); // Called when you enter the menu
//gameWrapper->HookEvent("Function TAGame.GFxData_MainMenu_TA.MainMenuAdded", std::bind(&StreamFinderPlugin::HandleMainMenu, this, std::placeholders::_1)); // Called when you enter the menu
//gameWrapper->HookEvent("Function GameEvent_Soccar_TA.Active.StartRound", std::bind(&StreamFinderPlugin::HandleGameStart, this, std::placeholders::_1)); // Start of kickoff
//gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.BeginState", std::bind(&StreamFinderPlugin::HandleGameStart, this, std::placeholders::_1)); // Start of goal replay
//gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnOvertimeUpdated", std::bind(&StreamFinderPlugin::HandleGameStart, this, std::placeholders::_1)); // Start of overtime
//gameWrapper->HookEvent("Function TAGame.GameEvent_TA.AddCar", std::bind(&StreamFinderPlugin::HandlePlayerAdded, this, std::placeholders::_1)); // Called when a car spawns
//gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnAllTeamsCreated", std::bind(&StreamFinderPlugin::HandleGameStart, this, std::placeholders::_1)); // Once all teams are formed
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
	wchar_t* w_app_data_path;
	size_t sz = 0;
	errno_t err = _wdupenv_s(&w_app_data_path, &sz, L"APPDATA");
	wchar_t tcsCommandLine[2048]{ 0 };
	wsprintfW(tcsCommandLine, L"start \"%s\\bakkesmod\\bakkesmod\\data\\StreamFinder\\stream-finder.vbs\"", w_app_data_path);
	free(w_app_data_path);
	CreateProcessW(L"C:\\Windows\\System32\\wscript.exe", tcsCommandLine, nullptr, nullptr, TRUE, 0, nullptr, nullptr, (LPSTARTUPINFOW)&startupInfo, &pi);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	// This solution is used to prevent the program from kicking the player out of the Rocket League window.
}

static bool IsProcessRunning(const wchar_t* processName) {
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry)) {

			wcout << entry.szExeFile << endl;

			if (!_wcsicmp(entry.szExeFile, processName))
				exists = true;
		}

	CloseHandle(snapshot);
	return exists;
}

void StreamFinderPlugin::Refresh(std::string eventName)
{	
	std::ofstream bl(gameWrapper->GetDataFolder() / "StreamFinder" / "blacklist-log.txt");
	bl << " " << std::endl;
	bl.close();

	if (IsProcessRunning(L"streamlink.exe")) {
		cvarManager->executeCommand("rec_toast");
	}
} 

void StreamFinderPlugin::RecNotif(std::string eventName)
{
	cvarManager->executeCommand("sf_toast");

	if (IsProcessRunning(L"streamlink.exe")) {
		ModalWrapper modal = gameWrapper->CreateModal("Streamlink Recorder");
		modal.SetBody("A stream recording is currently in progress...");
		modal.AddButton("Okay.", true, [&] {
			});
		modal.AddButton("Stop Recording", true, [&] {
			STARTUPINFO startupInfo;
			PROCESS_INFORMATION pi;
			memset(&startupInfo, 0, sizeof(STARTUPINFO));
			startupInfo.cb = sizeof(STARTUPINFO);
			startupInfo.wShowWindow = false;
			// Get path for each computer, non-user specific and non-roaming data.
			// Append product-specific path
			wchar_t* w_app_data_path;
			size_t sz = 0;
			errno_t err = _wdupenv_s(&w_app_data_path, &sz, L"APPDATA");
			wchar_t tcsCommandLine[2048]{ 0 };
			wsprintfW(tcsCommandLine, L"start \"%s\\bakkesmod\\bakkesmod\\data\\StreamFinder\\stop-recording.vbs\"", w_app_data_path);
			free(w_app_data_path);
			CreateProcessW(L"C:\\Windows\\System32\\wscript.exe", tcsCommandLine, nullptr, nullptr, FALSE, 0, nullptr, nullptr, (LPSTARTUPINFOW)&startupInfo, &pi);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			// This solution is used to prevent the program from kicking the player out of the Rocket League window.
			});
		modal.AddButton("View Stream", true, [&] {
			STARTUPINFO startupInfo;
			PROCESS_INFORMATION pi;
			memset(&startupInfo, 0, sizeof(STARTUPINFO));
			startupInfo.cb = sizeof(STARTUPINFO);
			startupInfo.wShowWindow = false;
			// Get path for each computer, non-user specific and non-roaming data.
			// Append product-specific path
			wchar_t* w_app_data_path;
			size_t sz = 0;
			errno_t err = _wdupenv_s(&w_app_data_path, &sz, L"APPDATA");
			wchar_t tcsCommandLine[2048]{ 0 };
			wsprintfW(tcsCommandLine, L"start /c ""%s\\bakkesmod\\bakkesmod\\data\\StreamFinder\\View-Recording.bat""", w_app_data_path);
			free(w_app_data_path);
			CreateProcessW(L"C:\\Windows\\System32\\cmd.exe", tcsCommandLine, nullptr, nullptr, TRUE, 0, nullptr, nullptr, (LPSTARTUPINFOW)&startupInfo, &pi);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			// This solution is used to prevent the program from kicking the player out of the Rocket League window.
			});
		modal.ShowModal();
	}
}


void StreamFinderPlugin::onUnload()
{
}


// https://cplusplus.com/doc/tutorial/files/
// https://stackoverflow.com/questions/224225/create-an-application-without-a-window
