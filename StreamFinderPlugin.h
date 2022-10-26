#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "bakkesmod/wrappers/GuiManagerWrapper.h"
#include <sstream>
#include <fstream>

#include "version.h"

#pragma comment( lib, "pluginsdk.lib" )

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class StreamFinderPlugin : public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow, public BakkesMod::Plugin::PluginWindow
{

	std::shared_ptr<bool> enabled;

	// Inherited via PluginSettingsWindow
	
	void RenderSettings() override;
	std::string GetPluginName() override;

	// Inherited via PluginWindow

	bool isWindowOpen_ = false;
	bool isMinimized_ = false;
	std::string menuTitle_ = "StreamFinderPlugin";	
	std::string pluginNiceName_ = "Stream Finder Plugin";
	
public:

	virtual void onLoad() override;
	virtual void onUnload() override;

	void LoadHooks();
	/*
	void RenderDrawable(CanvasWrapper canvas);
	void OpenScoreboard(std::string eventName);
	void CloseScoreboard(std::string eventName);
	*/

	//void LaunchQuickNoteModal();
	void HandleGameStart(std::string eventName);
	void HandleMainMenu(std::string eventName);
	//void HandlePlayerAdded(std::string eventName);

#pragma region GUI
	
	void Render() override;
	std::string GetMenuName() override;
	std::string GetMenuTitle() override;
	bool ShouldBlockInput() override;
	bool IsActiveOverlay() override;
	void OnOpen() override;
	void OnClose() override;
	void SetImGuiContext(uintptr_t ctx) override;
	//void RenderEditNoteModal();
	void OpenMenu();
	void CloseMenu();
	void ToggleMenu(); 

	std::vector<std::string> GetPlayersNames();

private:

	void renderWebhookTab();
	void renderBlacklistsTab();
	void renderExtrasTab();
	void discbufferfunc();
	void permabufferfunc();
	void tempbufferfunc();
	void logbufferfunc();
	void UpdateNotif();
	void DiscSaveNotif();
	void ListSaveNotif();
	void HookTestNotif();
	void Log(std::string msg);
	bool isWindowOpen = false;
	bool shouldBlockInput = false;
	std::string menuTitle = "Stream Finder Plugin";
	bool openQuickNote = false;
	std::string playersNoteToEdit = "";
#pragma endregion GUI
};

