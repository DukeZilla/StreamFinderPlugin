#include "pch.h"
#include <fstream>
#include <iostream>
#include "StreamFinderPlugin.h"
#include <string>
#include <tchar.h>
#include <shlobj_core.h>
#include "ImGui/imgui_internal.h"

using namespace std;

// Plugin Settings Window code here

std::string StreamFinderPlugin::GetPluginName() {
	return "Stream Finder Plugin";
}

// Render the plugin settings here
// This will show up in bakkesmod when the plugin is loaded at
//  f2 -> plugins -> StreamFinderPlugin
// static char str0[128] = "Insert link";

void StreamFinderPlugin::RenderSettings() {
    bool show_sf_window = false;
    ImGui::TextUnformatted("PRE ALPHA Version 0.9.23 | This Plugin is still under development");

	CVarWrapper enableCvar = cvarManager->getCvar("stream_finder_enabled");

	if (!enableCvar) {
		return;
	}

	bool enabled = enableCvar.getBoolValue();

	if (ImGui::Checkbox("Enable Plugin", &enabled)) {
		enableCvar.setValue(enabled);
	}

	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Toggle Stream Finder Plugin");
	}

    if (ImGui::Button("Open Setting Manager")) {
       
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Stream Finder Settings Manager");
    }

    ImGui::TextUnformatted("------------------------------------------------------O");

    if (ImGui::Button("Open Stream Finder Folder")) {
        system("C:\\Windows\\Temp\\directory.vbs");
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Opens the Stream Finder directory folder");
    }
     
    ImGui::TextUnformatted("------------------------------------------------------O");

    if (ImGui::CollapsingHeader("Help"))
    {
        ImGui::Text("SETTINGS MANAGER 101");
        ImGui::Separator();
        ImGui::Text("DISCORD WEBHOOK:");
        ImGui::BulletText("- How to set up the discord webhook -");
        ImGui::BulletText("1.) Open Discord and head to your personal server");
        ImGui::BulletText("2.) Right click on the channel you wish to receive stream notifications with and click \"Edit Channel\"");
        ImGui::BulletText("3.) Click \"Intergrations\" and then click \"Create Webhook Once you have created your webhook,\n"
            "copy the webhook URL and paste it in the text box");
        ImGui::BulletText("More info can be found on the discord support site.");
        ImGui::Separator();

        ImGui::Text("BLACKLIST INFO:");
        ImGui::BulletText("- Information on blacklists -");
        ImGui::BulletText("Temporary Blacklist: This will blacklist a player for 1 game to prevent notification spam.");
        ImGui::BulletText("Session Blacklist: If a streamer is found, they will be blacklisted for the entire day.\n"
            "The session blacklist will expire after the day is over. (Another spam prevention)");
        ImGui::BulletText("Permanent Blacklist: Whatever names are included in this list will be completely ignored by the program indefinitely,.");
        ImGui::BulletText("Bot list: All bot names have been blacklisted by default to prevent false positives.");
        ImGui::Separator();
        ImGui::Text("EXTRA INFO:");
        ImGui::BulletText("The program will also log all the stream finding events in a text file: \"livestreamlog.txt\"");
        ImGui::BulletText("If you ever want to see the stream finder folder you can click the button above.");\
        ImGui::BulletText("If there are any bugs please sumbit a ticket on the stream finder github page!");
        //ImGui::BulletText("You may also modify the sound effect, be sure the sound file name is modified in \"sound.vbs\"");
        ImGui::Separator();
    }

    ImGui::TextUnformatted("Plugin made by P as in Papi | Special thanks to the bakkesmod programming community for help!");
}

void StreamFinderPlugin::GuiWindow() 
{
    ImGui::SetNextWindowSizeConstraints(ImVec2(500, 500), ImVec2(FLT_MAX, FLT_MAX));
    if (ImGui::Begin("###Stream Finder Plugin", &isWindowOpen)) {
        if (ImGui::BeginTabBar("#Tab Bar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_NoTooltip)) {
            renderWebhookTab();
            renderBlacklistsTab();
            renderLiveLogTab();
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}

void StreamFinderPlugin::renderWebhookTab() {
    char buf[50];
    ImGui::TextUnformatted("DISCORD WEBHOOK LINK");
    ImGui::Separator();
    ImGui::TextUnformatted("To receive notifications, create a webhook and paste it here:");
    ImGui::InputText("Link", buf, 50);
    if (ImGui::Button("Save")) {

    }
    if (ImGui::Button("Test Webhook")) {
    
    }
}

void StreamFinderPlugin::renderBlacklistsTab() {
    ImGui::TextUnformatted("BLACKLISTS");
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Permanent Blacklist")) {}
    if (ImGui::CollapsingHeader("Temporary Blacklist")) {}
    if (ImGui::CollapsingHeader("Session Blacklist")) {}
    if (ImGui::Button("Clear all temporary blacklists")) {

    }
}

void StreamFinderPlugin::renderLiveLogTab() {
    ImGui::TextUnformatted("LOGGING");
    ImGui::Separator();
}

// Do ImGui rendering here

void StreamFinderPlugin::Render()
{
    if (!this->isWindowOpen) {
        cvarManager->executeCommand("togglemenu " + GetMenuName());

        return;
    }

	if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, ImGuiWindowFlags_None))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	ImGui::End();

	if (!isWindowOpen_)
	{
		cvarManager->executeCommand("togglemenu " + GetMenuName());
	}
}

// Name of the menu that is used to toggle the window.
std::string StreamFinderPlugin::GetMenuName()
{
	return "StreamFinderPlugin";
}

// Title to give the menu
std::string StreamFinderPlugin::GetMenuTitle()
{
	return menuTitle_;
}

// Don't call this yourself, BM will call this function with a pointer to the current ImGui context
void StreamFinderPlugin::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
    ImGui::GetIO().ConfigWindowsResizeFromEdges = true;
}

// Should events such as mouse clicks/key inputs be blocked so they won't reach the game
bool StreamFinderPlugin::ShouldBlockInput()
{
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

// Return true if window should be interactive
bool StreamFinderPlugin::IsActiveOverlay()
{
	return true;
}

// Called when window is opened
void StreamFinderPlugin::OnOpen()
{
	isWindowOpen_ = true;
}

// Called when window is closed
void StreamFinderPlugin::OnClose()
{
	isWindowOpen_ = false;
}

void StreamFinderPlugin::ToggleMenu()
{
    cvarManager->executeCommand("togglemenu " + GetMenuName());
}
