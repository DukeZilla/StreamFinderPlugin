#include "pch.h"
#include <fstream>
#include <iostream>
#include <string>   
#include "StreamFinderPlugin.h"
#include <string.h>
#include <tchar.h>
#include <shlobj_core.h>
#include "ImGui/imgui_internal.h"
#include "tlhelp32.h"
#include <windows.h>
#include <shellapi.h>

using namespace std; 

// Plugin Settings Window code here

// PLUGIN VERSION
static char PlugVerMain[1024] = "Plugin Version 1.24 | Build 556";

// Buffers
static char bufferBoi[1024]; // For the discord webhook
static char buffer00[1024]; // For the Permanent blacklist
static char buffer01[1024]; // For the Lobby list
static char buffer05[1024]; // For the Search Bar
static char link00[1024] = "https://github.com/streamlink/windows-builds/releases/tag/5.0.1-1";
static char link01[1024] = "https://www.videolan.org/vlc/";
static char streamlinkBuf00[1024]; // For the Streamlink recording status
static char path00[1024]; // For the streamlink path file
static char path01[1024]; // For the ffmpeg path file
static char PlugVerOld[1024]; // Plugin version from txt file
static char TwtchUsrs[1024]; // Plugin version from txt file
static char comboNames00[1024];

// Pointers
static std::vector<char*> tokens;

// Flags
std::once_flag oneflag;

// For reading powershell output
static inline std::string buffer02{}; // Not live
static inline std::string buffer03{}; // Player is live
static inline std::string buffer04{}; // All live streamers caught
static inline std::string buffer06{}; // Recording sessions

// Mini GUIs
static void WebhookGUI(bool* p_open);
static void FileBrowserGUI(bool* p_open);

// Bools
static bool webhook_window = false; // Bool to toggle the window
static bool fb_window = false; // Bool to toggle the window
static bool streamlink = false; // Bool to toggle streamlink

// int
static int selectednames = 0;

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
    if (webhook_window)     WebhookGUI(&webhook_window);
    if (fb_window)     FileBrowserGUI(&fb_window);
    ImGui::Text("%s", PlugVerMain); // Plugin Version
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
    ImGui::TextUnformatted("----------------------------------------------------------------O");
    if (ImGui::Button("Discord Webhook Setup", ImVec2(0, 0))) {
        webhook_window = true;
        discbufferfunc();
        if (!webhook_window) {
            WebhookGUI(&webhook_window);
        }
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Setup your Discord Webhook");
    }
     
    ImGui::TextUnformatted("----------------------------------------------------------------O");
    static char keybindBuf[64] = "F7";
    if (ImGui::Button("Set Keybind")) {
        cvarManager->setBind(keybindBuf, "togglemenu " + menuTitle_);
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Set your keybind for the Stream Finder GUI");
    }

    ImGui::SameLine();
    ImGui::PushItemWidth(60);
    ImGui::InputText("##keybind", keybindBuf, IM_ARRAYSIZE(keybindBuf));
    ImGui::PopItemWidth();
    ImGui::TextUnformatted("----------------------------------------------------------------O");

    UpdateButton();

    ImGui::TextUnformatted("----------------------------------------------------------------O");
    ImGui::TextUnformatted("Plugin made by P as in Papi   |");
    ImGui::SameLine();
    Credits();
    ImGui::TextUnformatted("If there are any issues message me on Discord ----> Papi#8196 ");
}

/// <summary>
/// Mini GUIs
/// </summary>

void StreamFinderPlugin::WebhookGUI(bool* p_open) {
    if (ImGui::Begin("Discord Webhook Setup", p_open))
    {
        ImGui::TextUnformatted("DISCORD WEBHOOK LINK");
        ImGui::Separator();
        ImGui::TextUnformatted("To receive notifications of live streamers on Discord, create a channel webhook and paste it here:");
        ImGui::InputText("###Link", bufferBoi, _countof(bufferBoi));
        DiscSaveNotif();
        ImGui::SameLine();
        HookTestNotif();
        ImGui::Separator();
        ImGui::Text("STREAM FINDER 101");
        ImGui::Separator();
        ImGui::Text("DISCORD WEBHOOK:");
        ImGui::BulletText("- How to set up the discord webhook -");
        ImGui::BulletText("1.) Open Discord and head to your personal server, if you don't have one, start your own server.");
        ImGui::BulletText("2.) Once you are in your server, right click on the channel you wish to receive stream notifications with and click \"Edit Channel\"");
        ImGui::BulletText("3.) Click \"Intergrations\" and then click \"Create Webhook\" Once you have created your webhook,\n"
            "copy the webhook URL and paste it in the text box above.");
        ImGui::Text("Once you have the discord webhook link all set up, you may press the \"Test Webhook\" button to test your webhook.");
    }
    ImGui::End();
}

void StreamFinderPlugin::FileBrowserGUI(bool* p_open) {
    ImGui::TextUnformatted("Plugin made by P as in Papi   |");
}

/// <summary>
/// Buttons
/// </summary>

void StreamFinderPlugin::UpdateButton()
{
    if (ImGui::Button("Update Plugin", ImVec2(0, 0))) {
        ImGui::OpenPopup("Update Stream Finder");
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Keep up with updates!");
    }

    ImGui::SameLine();
    ImGui::TextUnformatted("Will restart Rocket League.");

    if (ImGui::BeginPopupModal("Update Stream Finder", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("Are you sure you want to update? \n"
                               "- For Steam users: Steam will restart.");
        if (ImGui::Button("Yes", ImVec2(120, 0))) {
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
            wsprintfW(tcsCommandLine, L"start /c ""%s\\bakkesmod\\bakkesmod\\data\\StreamFinder\\update.bat""", w_app_data_path);
            free(w_app_data_path);
            CreateProcessW(L"C:\\Windows\\System32\\cmd.exe", tcsCommandLine, nullptr, nullptr, TRUE, 0, nullptr, nullptr, (LPSTARTUPINFOW)&startupInfo, &pi);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            // This solution is used to prevent the program from kicking the player out of the Rocket League window.
            cvarManager->log("Update Started");
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("No", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void StreamFinderPlugin::Credits()
{
    if (ImGui::Button("Credits", ImVec2(0, 0))) {
        ImGui::OpenPopup("Credits");
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Showing some love.");
    }

    if (ImGui::BeginPopupModal("Credits", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::BulletText("Testers:");
        ImGui::Text("BumpoTheClown, Unlivedmetal,  \n"
            "FourEyesOptic, Daboodeedabodah, stewSquared, \n"
            "Turtle, FreezerBurn_33, rage10b, \n"
            "QS3V3N, GettusRektus, and Sinan Enginist.");
        ImGui::Separator();
        ImGui::BulletText("Big thanks to:");
        ImGui::Text("JerryTheBee, Vync, ItsBranK, Martinn, \n"
            "BumpoTheClown, GettusRektus, eightfold, the testers \n"
            "and the rest of the bakkesmod programming \n"
            "community for help, ideas, and debugging!");
        ImGui::Separator();
        ImGui::Text("- - - Made with love by P as in Papi - - -");
        if (ImGui::Button("Close", ImVec2(310, 0))) {
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
        if (ImGui::Button("Okay.", ImVec2(130, 0))) {
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
        // For initiating the stream detector
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
        cvarManager->log("Stream Detector Test Launched.");
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

void StreamFinderPlugin::SearchButton()
{
    if (ImGui::Button("Search")) {
        std::ofstream search(gameWrapper->GetDataFolder() / "StreamFinder" / "names.txt");
        search << "Search-Request" << std::endl;
        search << std::string(buffer05) << std::endl;
        search.close();
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
        cvarManager->log("Stream Detector Test Launched.");
        ImGui::OpenPopup("Search");
    }

    if (ImGui::BeginPopupModal("Search", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("Search has started");
        if (ImGui::Button("Okay.", ImVec2(130, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void StreamFinderPlugin::SearchButton2()
{
    if (ImGui::Button("Re-launch Live Detector")) {
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
        cvarManager->log("Stream Detector Test Launched.");
    }
}

void StreamFinderPlugin::StopRecording() {
    ImGui::OpenPopup("Streamlink Stop");

    if (ImGui::BeginPopupModal("Streamlink Stop", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("Do you wish to stop recording?");

        if (ImGui::Button("Yes", ImVec2(90, 0))) {
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
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("No", ImVec2(90, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void StreamFinderPlugin::ViewSession() {
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
    OpenRecDir();
}

void StreamFinderPlugin::ResumeRecording() {
    if (ImGui::Button("Resume Recording", ImVec2(150, 0))) {
        ImGui::OpenPopup("Streamlink Resume");
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Select a name you wish to resume with.");
    }

    if (ImGui::BeginPopupModal("Streamlink Resume", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("Do you wish to resume the recording session?");

        if (ImGui::Button("Yes", ImVec2(130, 0))) {
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
            wsprintfW(tcsCommandLine, L"start \"%s\\bakkesmod\\bakkesmod\\data\\StreamFinder\\record.vbs\"", w_app_data_path);
            free(w_app_data_path);
            CreateProcessW(L"C:\\Windows\\System32\\wscript.exe", tcsCommandLine, nullptr, nullptr, FALSE, 0, nullptr, nullptr, (LPSTARTUPINFOW)&startupInfo, &pi);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            cvarManager->log("Recording resumed.");
            // This solution is used to prevent the program from kicking the player out of the Rocket League window.
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("No", ImVec2(130, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

/// <summary>
/// Pop ups
/// </summary>

void StreamFinderPlugin::SetPathPopup() {
    if (ImGui::Button("Path Settings", ImVec2(150, 0))) {
        PathBuf00();
        PathBuf01();
        ImGui::OpenPopup("File Path Settings");
    }

    if (ImGui::BeginPopupModal("File Path Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::BulletText("Streamlink Path:");
        ImGui::InputText("##PathStreamlink", path00, _countof(path00));
        ImGui::BulletText("FFmpeg Path:");
        ImGui::InputText("##PathFFmpeg", path01, _countof(path01));

        ImGui::Separator();

        if (ImGui::Button("Save", ImVec2(140, 0))) {
            std::ofstream streamlinkpath(gameWrapper->GetDataFolder() / "StreamFinder" / "Path-streamlink.txt");
            streamlinkpath << std::string(path00) << endl;
            streamlinkpath.close();
            std::ofstream ffmpegpath(gameWrapper->GetDataFolder() / "StreamFinder" / "Path-ffmpeg.txt");
            ffmpegpath << std::string(path01) << endl;
            ffmpegpath.close();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Close", ImVec2(140, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void StreamFinderPlugin::UpdateNotif() {
    std::call_once(oneflag, []() {
        if (ImGui::BeginPopupModal("Update", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::BulletText("A new update for the plugin is available, do you wish to continue?");
            ImGui::BulletText("Requires Rocket League to restart.");

            if (ImGui::Button("Update", ImVec2(140, 0))) {
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
                wsprintfW(tcsCommandLine, L"start /c ""%s\\bakkesmod\\bakkesmod\\data\\StreamFinder\\update.bat""", w_app_data_path);
                free(w_app_data_path);
                CreateProcessW(L"C:\\Windows\\System32\\cmd.exe", tcsCommandLine, nullptr, nullptr, TRUE, 0, nullptr, nullptr, (LPSTARTUPINFOW)&startupInfo, &pi);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
                // This solution is used to prevent the program from kicking the player out of the Rocket League window.
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Close", ImVec2(140, 0))) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    });
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
    memset(&buffer01, 0, _countof(buffer01)); // init char array
    strncpy_s(buffer01, tempstring.c_str(), _countof(buffer01)); // print text to the array/buffer
    temp.close();
}

void StreamFinderPlugin::PathBuf00()
{
    std::string line02;
    std::string pathstr;
    std::ifstream ifpath(gameWrapper->GetDataFolder() / "StreamFinder" / "Path-streamlink.txt");
    while (getline(ifpath, line02))
    {
        if (line02.empty()) {
            break;
        }
        pathstr += line02 + '\n';
    }
    memset(&path00, 0, _countof(path00)); // init char array
    strncpy_s(path00, pathstr.c_str(), _countof(path00)); // print text to the array/buffer
    ifpath.close();
}

void StreamFinderPlugin::PathBuf01()
{
    std::string line03;
    std::string path2str;
    std::ifstream ifpath2(gameWrapper->GetDataFolder() / "StreamFinder" / "Path-ffmpeg.txt");
    while (getline(ifpath2, line03))
    {
        if (line03.empty()) {
            break;
        }
        path2str += line03 + '\n';
    }
    memset(&path01, 0, _countof(path01)); // init char array
    strncpy_s(path01, path2str.c_str(), _countof(path01)); // print text to the array/buffer
    ifpath2.close();
}

void StreamFinderPlugin::ComboBuf00()
{
    std::string namestr;
    std::ifstream listpath(gameWrapper->GetDataFolder() / "StreamFinder" / "Combo-Info00.txt");
    getline(listpath, namestr);
    listpath.close();
    char* token = std::strtok(namestr.data(), ".");
    while (token)
    {
        tokens.push_back(token);
        token = std::strtok(nullptr, ".");
    }
}

void StreamFinderPlugin::notlivelogbufferfunc()
{
    auto logpath = gameWrapper->GetDataFolder() / "StreamFinder" / "PeaceOfMind.txt";
    std::ifstream logstrm(logpath, std::ios::binary);
    logstrm.unsetf(std::ios::skipws);

    std::streampos size;

    logstrm.seekg(0, std::ios::end);
    size = logstrm.tellg();
    logstrm.seekg(0, std::ios::beg);

    std::istream_iterator<char> start(logstrm), end;
    buffer02.reserve(size);
    buffer02.insert(buffer02.cbegin(), start, end);

    logstrm.close();
}

void StreamFinderPlugin::livelogbufferfunc()
{
    auto logpath01 = gameWrapper->GetDataFolder() / "StreamFinder" / "Session-Blacklist.txt";
    std::ifstream logstrm01(logpath01, std::ios::binary);
    logstrm01.unsetf(std::ios::skipws);

    std::streampos size01;

    logstrm01.seekg(0, std::ios::end);
    size01 = logstrm01.tellg();
    logstrm01.seekg(0, std::ios::beg);

    std::istream_iterator<char> start(logstrm01), end;
    buffer03.reserve(size01);
    buffer03.insert(buffer03.cbegin(), start, end);

    logstrm01.close();
}

void StreamFinderPlugin::streamlogbufferfunc()
{
    auto logpath02 = gameWrapper->GetDataFolder() / "StreamFinder" / "livestreamlog.txt";
    std::ifstream logstrm02(logpath02, std::ios::binary);
    logstrm02.unsetf(std::ios::skipws);

    std::streampos size02;

    logstrm02.seekg(0, std::ios::end);
    size02 = logstrm02.tellg();
    logstrm02.seekg(0, std::ios::beg);

    std::istream_iterator<char> start(logstrm02), end;
    buffer04.reserve(size02);
    buffer04.insert(buffer04.cbegin(), start, end);

    logstrm02.close();
}

void StreamFinderPlugin::RecSesBuf()
{
    auto recpath = gameWrapper->GetDataFolder() / "StreamFinder" / "streamlink-session.txt";
    std::ifstream recstrm(recpath, std::ios::binary);
    recstrm.unsetf(std::ios::skipws);

    std::streampos size01;

    recstrm.seekg(0, std::ios::end);
    size01 = recstrm.tellg();
    recstrm.seekg(0, std::ios::beg);

    std::istream_iterator<char> start(recstrm), end;
    buffer06.reserve(size01);
    buffer06.insert(buffer06.cbegin(), start, end);

    recstrm.close();
}

/// <summary>
/// Functions
/// </summary>

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

void StreamFinderPlugin::ProcessStatus() {
    if (IsProcessRunning(L"streamlink.exe")) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
        ImGui::Text("RECORDING IN PROGRESS!");
        ImGui::PopStyleColor();
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 0, 0, 255));
        if (ImGui::Button("Stop Recording")) {
            StopRecording();
        }
        ImGui::PopStyleColor();
        if (ImGui::Button("View Live Stream", ImVec2(150, 0))) {
            ViewSession();
        }
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
        ImGui::Text("No recordings in progress.");
        ImGui::PopStyleColor();
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(165, 0, 0, 255));
        if (ImGui::Button("Stop Recording")) {
            ImGui::OpenPopup("Stop Streamlink");
        }
        ImGui::PopStyleColor();
        if (ImGui::BeginPopupModal("Stop Streamlink", NULL, ImGuiWindowFlags_AlwaysAutoResize)) { 
            ImGui::TextUnformatted("No recordings are in progress");

            if (ImGui::Button("Okay.", ImVec2(170, 0))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        if (ImGui::Button("View Recent Recording", ImVec2(150, 0))) {
            ViewSession();
        }
    }
    cin.get();
}

void StreamFinderPlugin::OpenRecDir() {
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
    wsprintfW(tcsCommandLine, L"start \"%s\\bakkesmod\\bakkesmod\\data\\StreamFinder\\recordings-dir.vbs\"", w_app_data_path);
    free(w_app_data_path);
    CreateProcessW(L"C:\\Windows\\System32\\wscript.exe", tcsCommandLine, nullptr, nullptr, FALSE, 0, nullptr, nullptr, (LPSTARTUPINFOW)&startupInfo, &pi);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    // This solution is used to prevent the program from kicking the player out of the Rocket League window.
}

/// <summary>
/// Rendered Tabs
/// </summary>

void StreamFinderPlugin::renderLoggingTab() {
    if (ImGui::BeginTabItem("Logging")) {
        if (ImGui::BeginChild("##Logging", ImVec2(0, 0), true)) {
            ImGui::TextUnformatted("Stream Finder Log Files");
            ImGui::SameLine();

            if (ImGui::Button("Refresh logs")) {
                buffer02.clear();
                buffer03.clear();
                buffer04.clear();

                livelogbufferfunc();
                notlivelogbufferfunc();
                streamlogbufferfunc();
            }

            ImGui::SameLine();

            SearchButton2();

            ImGui::Separator();

            if (ImGui::CollapsingHeader("Stream Finder Logs", ImGuiTreeNodeFlags_DefaultOpen)) {

                // Titles
                ImGui::Columns(3, "Columns");
                ImGui::Separator();
                ImGui::Text("All Streamers Caught"); ImGui::NextColumn();
                ImGui::Text("Current Lobby Status"); ImGui::NextColumn();
                ImGui::Text("Streamers Caught Today"); ImGui::NextColumn();
                ImGui::Separator();

                // All Stream
                ImGui::Text("%s", buffer04.c_str()); ImGui::NextColumn();

                // Current lobby
                ImGui::Text("%s", buffer02.c_str()); ImGui::NextColumn();

                // Today
                ImGui::Text("%s", buffer03.c_str()); ImGui::NextColumn();
                ImGui::Separator();
            }
        }
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
}

void StreamFinderPlugin::renderStreamlinkTab() {
    if (fb_window)     FileBrowserGUI(&fb_window);
    if (ImGui::BeginTabItem("Streamlink")) {
        if (ImGui::BeginChild("##Recorder", ImVec2(280, 0), true)) {
            ImGui::TextUnformatted("Streamlink Recorder");
            ImGui::Separator();
            //if (ImGui::Checkbox("Enable Streamlink", &streamlink)) {

            //}
            ImGui::TextUnformatted("-------------------------------------------------------O");
            ImGui::Text("Streamlink Status:");
            ImGui::SameLine();
            ProcessStatus();
            ImGui::TextUnformatted("-------------------------------------------------------O");
            ImGui::Text("Resume Last Session:");
            ResumeRecording();
            //ImGui::Combo("Today's List", &selectednames, tokens.data(), sizeof(tokens));
            ImGui::TextUnformatted("-------------------------------------------------------O");
            ImGui::Text("Last Recording Session:");
            ImGui::Text("%s", buffer06.c_str());
            ImGui::TextUnformatted("-------------------------------------------------------O");
            ImGui::Text("File Path:");
            SetPathPopup();

        }

        ImGui::EndChild();
        ImGui::SameLine();

        if (ImGui::BeginChild("##Recordings", ImVec2(0, 0), true)) {
            ImGui::TextUnformatted("Video Manager");
            ImGui::Separator();
            ImGui::TextUnformatted("COMING SOON");
            //if (ImGui::Button("File Manager")) {
            //    fb_window = true;
            //    if (!fb_window) {
            //        FileBrowserGUI(&fb_window);
            //    }
            //}

            if (ImGui::Button("Open Recordings Folder")) {
                OpenRecDir();
            }

            ImGui::TextUnformatted("-------------------------------------------------------------------------O");

            if (ImGui::CollapsingHeader("Help")) {
                ImGui::Text("STREAM FINDER 101");
                ImGui::Separator();
                ImGui::Text("STREAMLINK:");
                ImGui::Text("Install Streamlink to automatically record Twitch Streams in the background with no performance impact!");
                ImGui::Text("- How to install Streamlink -");
                ImGui::BulletText("Click the website below and download the setup file. (Recommend \"streamlink-5.0.1-1-py310-x86_64.exe\")");
                if (ImGui::Button(link00)) {
                    const wchar_t* url00 = L"https://github.com/streamlink/windows-builds/releases/tag/5.0.1-1";
                    const wchar_t* action00 = L"Open";
                    ShellExecute(NULL, action00, url00, NULL, NULL, SW_SHOWNORMAL);
                }
                ImGui::BulletText("After the setup, you should be all set... it's that quick!");
                ImGui::Text("- Useful Information -");
                ImGui::BulletText("With Streamlink, you can record streams with no resource-heavy recorders or with websites, it will \n"
                "record these twitch streams in the background during your gameplay when a streamer is found!");
                ImGui::BulletText("When a recording is in session it will tell you in the status in the \"Streamlink Recorder\" block, \n"
                "or a bakkesmod toast (notification) will pop up on the top right hand of your screen. \n"
                "Be sure to have bakkesmod notifications turned on!");
                ImGui::BulletText("The plugin will also use FFmpeg to convert your recordings to mp4, which comes with streamlink. \n"
                "This process will happen after you stop recording. You can configure the file paths in the \"File Path Settings\". \n"
                "These paths will still be automatically configured for you.");
                ImGui::BulletText("Recommended that you download VLC media player to view these recordings, VLC will allow you to view streams live.");
                if (ImGui::Button(link01)) {
                    const wchar_t* url01 = L"https://www.videolan.org/vlc/";
                    const wchar_t* action01 = L"Open";
                    ShellExecute(NULL, action01, url01, NULL, NULL, SW_SHOWNORMAL);
                }
                ImGui::Separator();
            }
        }
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
}

void StreamFinderPlugin::renderBlacklistsTab() {
    if (ImGui::BeginTabItem("Blacklist")) {
        if (ImGui::BeginChild("##Blacklist", ImVec2(0, 0), true)) {
            ImGui::TextUnformatted("Blacklisting");
            ImGui::Separator();

            if (ImGui::CollapsingHeader("Blacklist", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::GetStateStorage()->SetInt(ImGui::GetID("Current Lobby"), 0);
                ImGui::GetStateStorage()->SetInt(ImGui::GetID("Blacklist"), 1);
                ImGui::TextUnformatted("Enter names of players you wish to be permanently blacklisted here. (Must be exactly the same as display name)");
                ImGui::Separator();
                ImGui::InputTextMultiline("###List", buffer00, _countof(buffer00));
                ListSaveNotif();
            }

            if (ImGui::CollapsingHeader("Current Lobby")) {
                ImGui::GetStateStorage()->SetInt(ImGui::GetID("Blacklist"), 0);
                ImGui::GetStateStorage()->SetInt(ImGui::GetID("Current Lobby"), 1);
                ImGui::TextUnformatted("Players in your current lobby will appear here. (Copy & Paste)");
                ImGui::Separator();
                ImGui::InputTextMultiline("###List", buffer01, _countof(buffer01));
                if (ImGui::Button("Refresh")) {
                    tempbufferfunc();
                }
            }

            ImGui::Separator();

            if (ImGui::CollapsingHeader("Help")) {
                ImGui::Text("STREAM FINDER 101");
                ImGui::Separator();
                ImGui::Text("BLACKLIST INFO:");
                ImGui::BulletText("- Information on blacklists -");
                ImGui::BulletText("Blacklist: Whatever names are included in this list will be completely ignored by the program indefinitely,.");
                ImGui::BulletText("Be sure the names are typed exactly the same as the name you see on their nameplate.");
                ImGui::BulletText("The \"Current Lobby\" list refreshes everytime you toggle the GUI, or by pressing the refresh button.");
                ImGui::BulletText("Side note: All bot names have been blacklisted by default to prevent false positives.");
                ImGui::Separator();
            }
        }
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
}

void StreamFinderPlugin::renderExtrasTab() {
    if (ImGui::BeginTabItem("Search")) {
        if (ImGui::BeginChild("##Extras", ImVec2(0, 0), true)) {
            ImGui::TextUnformatted("Player Search Bar");
            ImGui::Separator();
            ImGui::Text("Still suspect that a player is live? Type the name here:");
            ImGui::InputText("Search bar", buffer05, _countof(buffer05));
            SearchButton();

            ImGui::TextUnformatted("-------------------------------------------------------------------------O");
        }
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
}

// Do ImGui rendering here

void StreamFinderPlugin::Render()
{
    ImGui::SetNextWindowSizeConstraints(ImVec2(750, 550), ImVec2(FLT_MAX, FLT_MAX));
	if (ImGui::Begin(pluginNiceName_.c_str(), &isWindowOpen_, ImGuiWindowFlags_None)) {
        if (ImGui::BeginTabBar("#Tab Bar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_NoTooltip)) {
            renderLoggingTab();
            renderStreamlinkTab();
            renderBlacklistsTab();
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
    buffer02.clear();
    buffer03.clear();
    buffer04.clear();
    buffer06.clear();

    permabufferfunc();
    tempbufferfunc();

    livelogbufferfunc();
    notlivelogbufferfunc();
    streamlogbufferfunc();
    RecSesBuf();

    ComboBuf00();

    //std::bind(&UpdateNotif, this);
    //std::thread t1(oneflag);
    //t1.join();

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
