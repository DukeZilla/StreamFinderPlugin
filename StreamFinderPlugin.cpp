#include "pch.h"
#include "StreamFinderPlugin.h"
#include <iostream>
#include <string>
#include <fstream>
using namespace std;


BAKKESMOD_PLUGIN(StreamFinderPlugin, "Stream Finder Plugin", plugin_version, PERMISSION_ALL)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void StreamFinderPlugin::onLoad()
{
	_globalCvarManager = cvarManager;
	cvarManager->log("Plugin loaded!");

	//cvarManager->registerNotifier("my_aweseome_notifier", [&](std::vector<std::string> args) {
	cvarManager->log("Hello notifier!");
	//}, "", 0);
	this->LoadHooks();

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
gameWrapper->HookEvent("Function GameEvent_TA.Countdown.BeginState", std::bind(&StreamFinderPlugin::HandleGameStart, this, std::placeholders::_1));
gameWrapper->HookEvent("Function TAGame.Team_TA.EventPlayerAdded", std::bind(&StreamFinderPlugin::HandlePlayerAdded, this, std::placeholders::_1));
gameWrapper->HookEvent("Function OnlineGameJoinGame_X.JoiningBase.IsJoiningGame", std::bind(&StreamFinderPlugin::HandleGameStart, this, std::placeholders::_1));
gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnAllTeamsCreated", std::bind(&StreamFinderPlugin::HandleGameStart, this, std::placeholders::_1));
gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded", std::bind(&StreamFinderPlugin::HandleGameEnd, this, std::placeholders::_1));
gameWrapper->HookEvent("Function TAGame.GFxShell_TA.LeaveMatch", std::bind(&StreamFinderPlugin::HandleGameLeave, this, std::placeholders::_1));
}

void StreamFinderPlugin::HandleGameStart(std::string eventName)
{

	std::vector<std::string> StreamFinderPlugin::GetPlayersNames()
	{
		std::vector<std::string> names;

		ServerWrapper server = gameWrapper->GetCurrentGameState();
		if (server)
		{
			auto pris = server.GetPRIs();
			for (auto pri : pris) {
				if (!pri) continue;
				names.push_back(pri.GetPlayerName().ToString());
			};
		}

		return names;

		std::vector<std::string> playersNames = GetPlayersNames();
		for (std::string name : playersNames)
		{
			cvarManager->log("playerNames");
		}
	}
}

void StreamFinderPlugin::onUnload()
{
}
