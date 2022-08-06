#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"

#pragma comment( lib, "pluginsdk.lib" )

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class StreamFinderPlugin : public BakkesMod::Plugin::BakkesModPlugin 
{

	//std::shared_ptr<bool> enabled;

	// Inherited via PluginSettingsWindow
	/*
	void RenderSettings() override;
	std::string GetPluginName() override;
	void SetImGuiContext(uintptr_t ctx) override;
	*/

	// Inherited via PluginWindow
	/*

	bool isWindowOpen_ = false;
	bool isMinimized_ = false;
	std::string menuTitle_ = "StreamFinderPlugin";

	virtual void Render() override;
	virtual std::string GetMenuName() override;
	virtual std::string GetMenuTitle() override;
	virtual void SetImGuiContext(uintptr_t ctx) override;
	virtual bool ShouldBlockInput() override;
	virtual bool IsActiveOverlay() override;
	virtual void OnOpen() override;
	virtual void OnClose() override;
	
	*/
public:

	virtual void onLoad() override;
	virtual void onUnload() override;

	void LoadHooks();
	void RenderDrawable(CanvasWrapper canvas);
	void OpenScoreboard(std::string eventName);
	void CloseScoreboard(std::string eventName);
	void LaunchQuickNoteModal();
	void HandleGameStart(std::string eventName);
	void HandleGameEnd(std::string eventName);
	void HandleGameLeave(std::string eventName);
	void HandlePlayerAdded(std::string eventName);

#pragma region GUI

	void RenderEditNoteModal();
	void OpenMenu();
	void CloseMenu();
	void ToggleMenu();
	std::vector<std::string> GetPlayersNames();

private:
	void Log(std::string msg);
	bool isWindowOpen = false;
	bool shouldBlockInput = false;
	std::string menuTitle = "Stream Finder Plugin";
	bool openQuickNote = false;
	std::string playersNoteToEdit = "";
};

