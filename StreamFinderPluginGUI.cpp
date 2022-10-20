#include "pch.h"
#include <fstream>
#include <iostream>
#include "StreamFinderPlugin.h"
#include <string.h>
#include <tchar.h>
#include <shlobj_core.h>
#include "ImGui/imgui_internal.h"

using namespace std;

// Plugin Settings Window code here
static void StreamFinderGUI(bool* p_open);
static char bufferBoi[1024]; // For the discord webhook
static char buffer00[1024]; // For the Permanent blacklist
static char buffer01[1024]; // For the Temporary blacklist
static char buffer02[1024]; // For the Session blacklist
static char buffer03[1024]; // For the log file

std::string StreamFinderPlugin::GetPluginName() {
	return "Stream Finder Plugin";
}

// Render the plugin settings here
// This will show up in bakkesmod when the plugin is loaded at
//  f2 -> plugins -> StreamFinderPlugin
// static char str0[128] = "Insert link";

void StreamFinderPlugin::RenderSettings() {
    static bool show_sf_window = false;
    if (show_sf_window)           StreamFinderGUI(&show_sf_window);
    ImGui::TextUnformatted("PRE ALPHA Version 0.9.30 | This Plugin is still under development");

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

    if (ImGui::Button("Open Stream Finder GUI")) {
        discbufferfunc();
        permabufferfunc();
        tempbufferfunc();
        sessionbufferfunc();
        logbufferfunc();
        show_sf_window = true;
        if (!show_sf_window) {
            StreamFinderGUI(&show_sf_window);
        }
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Stream Finder Settings Manager");
    }
     
    ImGui::TextUnformatted("------------------------------------------------------O");

    if (ImGui::CollapsingHeader("Help"))
    {
        ImGui::Text("SETTINGS MANAGER 101");
        ImGui::Separator();
        ImGui::Text("DISCORD WEBHOOK:");
        ImGui::BulletText("- How to set up the discord webhook -");
        ImGui::BulletText("1.) Open Discord and head to your personal server, if you don't have one, start your own server.");
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

void StreamFinderPlugin::StreamFinderGUI(bool* p_open)
{
 ImGui::SetNextWindowSizeConstraints(ImVec2(500, 500), ImVec2(FLT_MAX, FLT_MAX));
     if (ImGui::Begin("Stream Finder Plugin", p_open)) {
         if (ImGui::BeginTabBar("#Tab Bar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_NoTooltip)) {
             ImGui::TextUnformatted("Stream Finder Settings");
             renderWebhookTab();
             renderBlacklistsTab();
             renderExtrasTab();
             ImGui::EndTabBar();
         }
     }
     ImGui::End();
}

void StreamFinderPlugin::discbufferfunc() 
{
    std::string discstring;
    std::ifstream webhook(gameWrapper->GetDataFolder() / "StreamFinder" / "discord-webhook.txt");
    getline(webhook, discstring);
    // do outside of render func
    memset(&bufferBoi, 0, _countof(bufferBoi)); // init char array
    strncpy_s(bufferBoi, discstring.c_str(), _countof(bufferBoi)); // print text to the array/buffer
    webhook.close();
}

void StreamFinderPlugin::permabufferfunc()
{
    std::string permastring;
    std::ifstream perma(gameWrapper->GetDataFolder() / "StreamFinder" / "permanent-blacklist.txt");
    getline(perma, permastring);
    // do outside of render func
    memset(&buffer00, 0, _countof(buffer00)); // init char array
    strncpy_s(buffer00, permastring.c_str(), _countof(buffer00)); // print text to the array/buffer
    perma.close();
}

void StreamFinderPlugin::tempbufferfunc()
{
    std::string tempstring;
    std::ifstream temp(gameWrapper->GetDataFolder() / "StreamFinder" / "blacklist.txt");
    getline(temp, tempstring);
    // do outside of render func
    memset(&buffer01, 0, _countof(buffer01)); // init char array
    strncpy_s(buffer01, tempstring.c_str(), _countof(buffer01)); // print text to the array/buffer
    temp.close();
}

void StreamFinderPlugin::sessionbufferfunc()
{
    std::string sessionstring;
    std::ifstream sessions(gameWrapper->GetDataFolder() / "StreamFinder" / "session-blacklist.txt");
    getline(sessions, sessionstring);
    // do outside of render func
    memset(&buffer02, 0, _countof(buffer02)); // init char array
    strncpy_s(buffer02, sessionstring.c_str(), _countof(buffer02)); // print text to the array/buffer
    sessions.close();
}

void StreamFinderPlugin::logbufferfunc()
{
    std::string logstring;
    std::ifstream log(gameWrapper->GetDataFolder() / "StreamFinder" / "livestreamlog.txt");
    getline(log, logstring);
    // do outside of render func
    memset(&buffer03, 0, _countof(buffer03)); // init char array
    strncpy_s(buffer03, logstring.c_str(), _countof(buffer03)); // print text to the array/buffer
    log.close();
}

void StreamFinderPlugin::renderWebhookTab() {
    if (ImGui::BeginTabItem("Discord Webhook")) {
        ImGui::Separator();
        ImGui::TextUnformatted("DISCORD WEBHOOK LINK");
        ImGui::Separator();
        ImGui::TextUnformatted("To receive notifications, create a webhook and paste it here:");
        ImGui::InputText("###Link", bufferBoi, _countof(bufferBoi));

        if (ImGui::Button("Save")) {
            std::ofstream webhookchange(gameWrapper->GetDataFolder() / "StreamFinder" / "discord-webhook.txt");
            webhookchange << std::string(bufferBoi) << endl;
            webhookchange.close();
        }
        ImGui::SameLine();
        if (ImGui::Button("Test Webhook")) {
            std::ofstream test(gameWrapper->GetDataFolder() / "StreamFinder" / "test-webhook.txt");
            test << "Test-Webhook" << std::endl;
            test.close();
            STARTUPINFO startupInfo;
            PROCESS_INFORMATION pi;
            memset(&startupInfo, 0, sizeof(STARTUPINFO));
            startupInfo.cb = sizeof(STARTUPINFO);
            startupInfo.wShowWindow = false;
            // Get path for each computer, non-user specific and non-roaming data.
            // Append product-specific path
            TCHAR tcsCommandLine[] = _T("start ""\\Windows\\Temp\\stream-finder.vbs""");
            CreateProcessW(L"C:\\Windows\\System32\\wscript.exe", tcsCommandLine, NULL, NULL, TRUE, 0, NULL, NULL, (LPSTARTUPINFOW)&startupInfo, &pi);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            cvarManager->log("Stream Detector Test Launched.");
            // This solution is used to prevent the program from kicking the player out of the Rocket League window.
        }
        ImGui::EndTabItem;
    }
}

void StreamFinderPlugin::renderBlacklistsTab() {
    if (ImGui::BeginTabItem("Blacklists")) {
        ImGui::Separator();
        ImGui::TextUnformatted("BLACKLISTS");
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Permanent Blacklist")) {
            ImGui::TextUnformatted("Enter names you wish to be permanetly blacklisted here.");
            ImGui::Separator();
            ImGui::InputTextMultiline("###List", buffer00, _countof(buffer00));
            if (ImGui::Button("Save")) {
                std::ofstream permachange(gameWrapper->GetDataFolder() / "StreamFinder" / "permanent-blacklist.txt");
                permachange << std::string(buffer00) << endl;
                permachange.close();
            }
        }
        if (ImGui::CollapsingHeader("Temporary Blacklist")) {
            ImGui::TextUnformatted("These names are blacklisted for 1 game.");
            ImGui::Separator();
            ImGui::InputTextMultiline("###List", buffer01, _countof(buffer01));
            if (ImGui::Button("Save")) {
                std::ofstream tempchange(gameWrapper->GetDataFolder() / "StreamFinder" / "blacklist.txt");
                tempchange << std::string(buffer01) << endl;
                tempchange.close();
            }
        }
        if (ImGui::CollapsingHeader("Session Blacklist")) {
            ImGui::TextUnformatted("These names are blacklisted until the day is over.");
            ImGui::Separator();
            ImGui::InputTextMultiline("###List", buffer02, _countof(buffer02));
            if (ImGui::Button("Save")) {
                std::ofstream sessionchange(gameWrapper->GetDataFolder() / "StreamFinder" / "blacklist.txt");
                sessionchange << std::string(buffer02) << endl;
                sessionchange.close();
            }
        }
        if (ImGui::Button("Clear all temporary blacklists")) {

        }
        ImGui::EndTabItem;
    }
}

void StreamFinderPlugin::renderExtrasTab() {
    if (ImGui::BeginTabItem("Extras")) {
        ImGui::Separator();
        ImGui::TextUnformatted("EXTRAS");
        ImGui::Separator();
        ImGui::TextUnformatted("Live Stream Log File");
        ImGui::InputTextMultiline("###List", buffer03, _countof(buffer03));
        ImGui::Separator();
        if (ImGui::Button("Open Stream Finder Folder")) {
            system("C:\\Windows\\Temp\\directory.vbs");
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Opens the Stream Finder directory folder");
        }
        ImGui::EndTabItem;
    }
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
