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
static char bufferBoi[1024]; // For the discord webhook
static char buffer00[1024]; // For the Permanent blacklist
static char buffer01[1024]; // For the Lobby list
static char buffer02[1024]; // For the log file

std::string StreamFinderPlugin::GetPluginName() {
	return pluginNiceName_;
}

// Render the plugin settings here
// This will show up in bakkesmod when the plugin is loaded at
//  f2 -> plugins -> StreamFinderPlugin
// static char str0[128] = "Insert link";

////////////////////////
// Main Window
////////////////////////

void StreamFinderPlugin::RenderSettings() {
    ImGui::TextUnformatted("PRE-ALPHA Version 0.96 | This plugin is still under development.");
	CVarWrapper enableCvar = cvarManager->getCvar("stream_finder_enabled");
	if (!enableCvar) {
		return;
	}

	bool enabled = enableCvar.getBoolValue();
	if (ImGui::Checkbox("Enable Plugin", &enabled)) {
		enableCvar.setValue(enabled);
	}

	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Enable The Stream Finder Plugin");
	}

    if (ImGui::Button("Open Stream Finder GUI")) {
        gameWrapper->Execute([this](GameWrapper* gw) {
            cvarManager->executeCommand("togglemenu " + menuTitle_);
            });
       
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Stream Finder Graphical User Interface Window");
    }
     
    ImGui::TextUnformatted("------------------------------------------------------O");
    static char keybindBuf[64] = "F7";
    if (ImGui::Button("Set Keybind")) {
        cvarManager->setBind(keybindBuf, "togglemenu " + menuTitle_);
    }

    ImGui::SameLine();
    ImGui::PushItemWidth(60);
    ImGui::InputText("##keybind", keybindBuf, IM_ARRAYSIZE(keybindBuf));
    ImGui::PopItemWidth();
    ImGui::TextUnformatted("------------------------------------------------------O");

    UpdateNotif();

    ImGui::TextUnformatted("------------------------------------------------------O");
    ImGui::TextUnformatted("Plugin made by P as in Papi | Special thanks to the bakkesmod programming community for help!");
}

/// <summary>
/// Buttons
/// </summary>

void StreamFinderPlugin::UpdateNotif()
{
    if (ImGui::Button("Update Plugin", ImVec2(0, 0))) {
        ImGui::OpenPopup("Update Stream Finder");
    }
    ImGui::SameLine();
    ImGui::TextUnformatted("No need to restart Rocket League.");

    if (ImGui::BeginPopupModal("Update Stream Finder", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("Are you sure you want to update?");
        if (ImGui::Button("Yes", ImVec2(120, 0))) {
            STARTUPINFO startupInfo;
            PROCESS_INFORMATION pi;
            memset(&startupInfo, 0, sizeof(STARTUPINFO));
            startupInfo.cb = sizeof(STARTUPINFO);
            startupInfo.wShowWindow = false;
            // Get path for each computer, non-user specific and non-roaming data.
            // Append product-specific path
            TCHAR tcsCommandLine[] = _T("start ""\\Windows\\Temp\\update.vbs""");
            CreateProcessW(L"C:\\Windows\\System32\\wscript.exe", tcsCommandLine, NULL, NULL, TRUE, 0, NULL, NULL, (LPSTARTUPINFOW)&startupInfo, &pi);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            cvarManager->log("Update launched, see you on the other side!.");
            HookTestNotif();
            // This solution is used to prevent the program from kicking the player out of the Rocket League window.
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("No", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void StreamFinderPlugin::DiscSaveNotif()
{
    if (ImGui::Button("Save Changes", ImVec2(0, 0))) {
        std::ofstream webhookchange(gameWrapper->GetDataFolder() / "StreamFinder" / "discord-webhook.txt");
        webhookchange << std::string(bufferBoi) << endl;
        webhookchange.close();
        ImGui::OpenPopup("Discord Webhook");
    }

    if (ImGui::BeginPopupModal("Discord Webhook", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("Webhook saved!");
        if (ImGui::Button("Okay.", ImVec2(135, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void StreamFinderPlugin::ListSaveNotif()
{
    if (ImGui::Button("Save Changes", ImVec2(0, 0))) {
        std::ofstream permachange(gameWrapper->GetDataFolder() / "StreamFinder" / "permanent-blacklist.txt");
        permachange << std::string(buffer00) << endl;
        permachange.close();
        ImGui::OpenPopup("Blacklist");
    }

    if (ImGui::BeginPopupModal("Blacklist", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("Changes has been saved!");
        if (ImGui::Button("Okay.", ImVec2(135, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void StreamFinderPlugin::HookTestNotif()
{
    if (ImGui::Button("Test Webhook", ImVec2(0, 0))) {
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
        HookTestNotif();
        // This solution is used to prevent the program from kicking the player out of the Rocket League window.
        ImGui::OpenPopup("Webhook Test");
    }

    if (ImGui::BeginPopupModal("Webhook Test", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("Webhook test initiated!");
        if (ImGui::Button("Okay.", ImVec2(135, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

/// <summary>
/// Buffer Functions
/// </summary>

void StreamFinderPlugin::discbufferfunc() // Used for the discord tab
{
    std::string discstring;
    std::ifstream webhook(gameWrapper->GetDataFolder() / "StreamFinder" / "discord-webhook.txt");
    getline(webhook, discstring);
    // do outside of render func
    memset(&bufferBoi, 0, _countof(bufferBoi)); // init char array
    strncpy_s(bufferBoi, discstring.c_str(), _countof(bufferBoi)); // print text to the array/buffer
    webhook.close();
}

void StreamFinderPlugin::permabufferfunc() // Used for the blacklist
{
    std::string line00;
    std::string permastring;
    std::ifstream perma(gameWrapper->GetDataFolder() / "StreamFinder" / "permanent-blacklist.txt");
    while (getline(perma, line00))
    {
        if (line00.empty()) {
            break;
        }
        permastring += line00 + '\n';
    }
    // do outside of render func
    memset(&buffer00, 0, _countof(buffer00)); // init char array
    strncpy_s(buffer00, permastring.c_str(), _countof(buffer00)); // print text to the array/buffer
    perma.close();
}

void StreamFinderPlugin::tempbufferfunc() // Used for the "Current Lobby List"
{
    std::string line01;
    std::string tempstring;
    std::ifstream temp(gameWrapper->GetDataFolder() / "StreamFinder" / "names.txt");
    while (getline(temp, line01))
    {
        if (line01.empty()) {
            break;
        }
        tempstring += line01 + '\n';
    }
    // do outside of render func
    memset(&buffer01, 0, _countof(buffer01)); // init char array
    strncpy_s(buffer01, tempstring.c_str(), _countof(buffer01)); // print text to the array/buffer
    temp.close();
}

void StreamFinderPlugin::logbufferfunc() // Used for the peace of mind feature
{
    std::string line02;
    std::string logstring;
    std::ifstream logstrm(gameWrapper->GetDataFolder() / "StreamFinder" / "PeaceOfMind.json");
    while (getline(logstrm, line02))
    {
        if (line02.empty()) {
            break;
        }
        logstring += line02 + '\n';
    }
    // do outside of render func
    memset(&buffer02, 0, _countof(buffer02)); // init char array
    strncpy_s(buffer02, logstring.c_str(), _countof(buffer02)); // print text to the array/buffer
    logstrm.close();
}

/// <summary>
/// Rendered Tabs
/// </summary>

void StreamFinderPlugin::renderWebhookTab() {
    if (ImGui::BeginTabItem("Discord Webhook Setup")) {
        ImGui::Separator();
        ImGui::TextUnformatted("DISCORD WEBHOOK LINK");
        ImGui::Separator();
        ImGui::TextUnformatted("To receive notifications of live streamers, create a channel webhook and paste it here:");
        ImGui::InputText("###Link", bufferBoi, _countof(bufferBoi));
        DiscSaveNotif();
        ImGui::SameLine();
        HookTestNotif();
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Help"))
        {
            ImGui::Text("STREAM FINDER 101");
            ImGui::Separator();
            ImGui::Text("DISCORD WEBHOOK:");
            ImGui::BulletText("- How to set up the discord webhook -");
            ImGui::BulletText("1.) Open Discord and head to your personal server, if you don't have one, start your own server.");
            ImGui::BulletText("2.) Once you are in your server, right click on the channel you wish to receive stream notifications with and click \"Edit Channel\"");
            ImGui::BulletText("3.) Click \"Intergrations\" and then click \"Create Webhook\" Once you have created your webhook,\n"
                "copy the webhook URL and paste it in the text box above.");
            ImGui::BulletText("More info can be found on the discord support site.");
            ImGui::Text("Once you have the discord webhook link all set up, you may press the \"Test Webhook\" button to test your webhook.");
            ImGui::Separator();
        }

        ImGui::EndTabItem();
    }
}

void StreamFinderPlugin::renderBlacklistsTab() {
    if (ImGui::BeginTabItem("Blacklist")) {
        ImGui::Separator();
        ImGui::TextUnformatted("BLACKLIST");
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Blacklist", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Current Lobby"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Blacklist"), 1);
            ImGui::TextUnformatted("Enter names of players you wish to be permanently blacklisted here.");
            ImGui::Separator();
            ImGui::InputTextMultiline("###List", buffer00, _countof(buffer00));
            ListSaveNotif();
        }

        if (ImGui::CollapsingHeader("Current Lobby")) {
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Blacklist"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Current Lobby"), 1);
            ImGui::TextUnformatted("Players in your current lobby will appear here.");
            ImGui::Separator();
            ImGui::InputTextMultiline("###List", buffer01, _countof(buffer01));
            if (ImGui::Button("Refresh")) {
                tempbufferfunc();
            }
        }

        ImGui::Separator();

        if (ImGui::CollapsingHeader("Help"))
        {
            ImGui::Text("STREAM FINDER 101");
            ImGui::Separator();
            ImGui::Text("BLACKLIST INFO:");
            ImGui::BulletText("- Information on blacklists -");
            ImGui::BulletText("Permanent Blacklist: Whatever names are included in this list will be completely ignored by the program indefinitely,.");
            ImGui::BulletText("Be sure the names are typed exactly the same as the projected RL name.");
            ImGui::BulletText("The \"Current Lobby\" list refreshes everytime you toggle the GUI, or by pressing the refresh button.");
            ImGui::BulletText("Side note: All bot names have been blacklisted by default to prevent false positives.");
            ImGui::Separator();
        }

        ImGui::EndTabItem();
    }
}

void StreamFinderPlugin::renderExtrasTab() {
    if (ImGui::BeginTabItem("Misc")) {
        ImGui::Separator();
        ImGui::TextUnformatted("MISC");
        ImGui::Separator();
        ImGui::TextUnformatted("Current Stream Finder Log:");
        ImGui::TextUnformatted("Streamers who were found will be logged here.");
        ImGui::InputTextMultiline("###Misc", buffer02, _countof(buffer02));
        ImGui::Separator();

        if (ImGui::Button("Open Stream Finder Folder")) {
            system("C:\\Windows\\Temp\\directory.vbs");
        }

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Opens the Stream Finder directory folder");
        }

        ImGui::Separator();

        if (ImGui::CollapsingHeader("Help")) {
            ImGui::Text("STREAM FINDER 101");
            ImGui::Separator();
            ImGui::Text("MISC:");
            ImGui::BulletText("The program will also log all the stream finding events in a text file: \"livestreamlog.txt\"");
            ImGui::BulletText("If you ever want to see the stream finder folder you can click the button above."); \
            ImGui::BulletText("If there are any bugs please sumbit a ticket on the stream finder github page!");
            //ImGui::BulletText("You may also modify the sound effect, be sure the sound file name is modified in \"sound.vbs\"");
            ImGui::Separator();
        }

        ImGui::EndTabItem();
    }
}

// Do ImGui rendering here

void StreamFinderPlugin::Render()
{
    ImGui::SetNextWindowSizeConstraints(ImVec2(750, 550), ImVec2(FLT_MAX, FLT_MAX));
	if (ImGui::Begin(pluginNiceName_.c_str(), &isWindowOpen_, ImGuiWindowFlags_None)) {
        if (ImGui::BeginTabBar("#Tab Bar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_NoTooltip)) {
            ImGui::TextUnformatted("Stream Finder Settings");
            renderBlacklistsTab();
            renderWebhookTab();
            renderExtrasTab();
            ImGui::EndTabBar();
        }
	}

	ImGui::End();

	if (!isWindowOpen_)
	{
		cvarManager->executeCommand("togglemenu " + menuTitle_);
	}
}

// Name of the menu that is used to toggle the window.
std::string StreamFinderPlugin::GetMenuName()
{
    return menuTitle_;
}

// Title to give the menu
std::string StreamFinderPlugin::GetMenuTitle()
{
    return pluginNiceName_;
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
    discbufferfunc();
    logbufferfunc();
    permabufferfunc();
    tempbufferfunc();
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
