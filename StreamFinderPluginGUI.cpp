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
static char PlugVerMain[1024] = "Plugin Version 1.30.1 | Build 671";

// Buffers
static char bufferBoi[1024]; // For the discord webhook
static char buffer00[1024]; // For the Permanent blacklist
static char buffer01[1024]; // For the Lobby list
static char buffer05[1024]; // For the Search Bar
static char link00[1024] = "https://github.com/streamlink/windows-builds/releases";
static char link01[1024] = "https://www.videolan.org/vlc/";
static char streamlinkBuf00[1024]; // For the Streamlink recording status
static char path00[1024]; // For the streamlink path file
static char path01[1024]; // For the ffmpeg path file
static char TwtchUsrs[1024]; // Plugin version from txt file
static char comboNames00[1024]; // For the resume recording list combo
static char streamStatus[1024]; // Is the streamer live?
static char AboutInfoBuf[1024]; // For about pop up
static char RecPath[1024]; // Recordings path
static char VidName[1024]; // Video renaming

// Video Manager Text Strings
static std::string StartVidTxt = "Start Video";
static std::string CopyVidTxt = "Copy";
static std::string MoveVidTxt = "Move";
static std::string RenVidTxt = "Rename";
static std::string DelVidTxt = "Delete";
static std::string CloudTxt = "Upload to Cloud";
static std::string ffmpegTxt = "Convert Format";
static std::string InfoVidTxt = "Information";

// Pointers
static std::vector<char*> tokens;

// Flags
std::once_flag oneflag;

// For reading powershell output
static inline std::string buffer02{}; // Not live
static inline std::string buffer03{}; // Player is live
static inline std::string buffer04{}; // All live streamers caught
static inline std::string buffer06{}; // Recording session
static inline std::string PlugVerOld{}; // Reading version.txt

// Mini GUIs
static void WebhookGUI(bool* p_open);

// Pop ups
static void UpdateNotif(bool* p_open);

// Bools
static bool webhook_window = false; // Bool to toggle the window
static bool fb_window = false; // Bool to toggle the window
static bool streamlink = false; // Bool to toggle streamlink
static bool YouNeedToUpdate = false;

// int
static int selectednames = 0;
static int callcount = 0;

// Name spaces
namespace fs = std::filesystem;

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
    AboutPop();
    ImGui::TextUnformatted("If you have any issues or feedback message me on Discord ----> papi.gg ");
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
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
        ImGui::InputText("###Link", bufferBoi, _countof(bufferBoi));
        ImGui::PopStyleColor();
        DiscSaveNotif();
        ImGui::SameLine();
        HookTestNotif();
        ImGui::Separator();
        ImGui::Text("STREAM FINDER 101");
        ImGui::Separator();
        ImGui::Text("DISCORD WEBHOOK:");
        ImGui::BulletText("- How to set up the discord webhook -");
        ImGui::BulletText("1.) Open Discord and head to your personal server, or create one.");
        ImGui::BulletText("2.) Open your server, right click on a channel and click \"Edit Channel\"");
        ImGui::BulletText("3.) Click \"Intergrations,\" then click \"Create Webhook\" Once you have created your webhook,\n"
            "copy the webhook URL and paste it in the text box above.");
        ImGui::Text("Once the setup is complete, you can press the \"Test Webhook\" button to test your webhook.");
    }
    ImGui::End();
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

void StreamFinderPlugin::DiscSaveNotif()
{
    if (ImGui::Button("Save Changes", ImVec2(0, 0))) {
        std::ofstream webhookchange(gameWrapper->GetDataFolder() / "StreamFinder" / "cache" / "discord-webhook.txt");
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
        std::ofstream permachange(gameWrapper->GetDataFolder() / "StreamFinder" / "cache" / "permanent-blacklist.txt");
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
        std::ofstream test(gameWrapper->GetDataFolder() / "StreamFinder" / "cache" / "test-webhook.txt");
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
        ImGui::TextUnformatted("Webhook test initiated! Notification should be sent in less than 10 seconds.");
        if (ImGui::Button("Okay.", ImVec2(410, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void StreamFinderPlugin::SearchButton()
{
    if (ImGui::Button("Search")) {
        std::ofstream search(gameWrapper->GetDataFolder() / "StreamFinder" / "cache" / "names.txt");
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
        if (ImGui::Button("Okay.", ImVec2(110, 0))) {
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
            islivebuf();
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

void StreamFinderPlugin::AboutPop()
{
    if (ImGui::Button("About", ImVec2(0, 0))) {
        GetAbout();
        ImGui::OpenPopup("About Stream Finder");
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Versions & Credits");
    }

    ImGui::SetNextWindowContentSize(ImVec2(500, 213));
    if (ImGui::BeginPopupModal("About Stream Finder", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (ImGui::BeginChild("##About", ImVec2(0, 175), true)) {

            // Titles
            ImGui::Columns(2, "ColumnsAbout");

            ImGui::Text("%s", AboutInfoBuf); ImGui::NextColumn();

            // Today
            ImGui::BulletText("Testers:");
            ImGui::Text("BumpoTheClown, GettusRektus, Turtle, \n"
                "FourEyesOptic, Daboodeedabodah, \n"
                "DtewSquared, rage10b, FreezerBurn_33, \n"
                "QS3V3N, Unlivedmetal, and Sinan Enginist.");
            ImGui::Text("------------------------------------------------------O");
            ImGui::BulletText("Big thanks to:");
            ImGui::Text("JerryTheBee, Vync, ItsBranK, Martinn, \n"
                "eightfold, All of the Testers \n"
                "and the bakkesmod programming \n"
                "community for help, ideas, and debugging!"); ImGui::NextColumn();
        }
        ImGui::EndChild();
        ImGui::Text("  - - - - - - - - Made With Love by P as in Papi | Boom Squad Exclusive Plugin - - - - - - - - -");
        ImGui::Separator();
        if (ImGui::Button("Close", ImVec2(500, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

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
            std::ofstream streamlinkpath(gameWrapper->GetDataFolder() / "StreamFinder" / "cache" / "Path-streamlink.txt");
            streamlinkpath << std::string(path00) << endl;
            streamlinkpath.close();
            std::ofstream ffmpegpath(gameWrapper->GetDataFolder() / "StreamFinder" / "cache" / "Path-ffmpeg.txt");
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
    GetOldVer();
    int equalOrNot = strcmp(PlugVerMain, PlugVerOld.c_str());
    if (equalOrNot == 1) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
        ImGui::Text("UPDATE AVAILABLE");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        if (ImGui::Button("Update Plugin", ImVec2(0, 0))) {
            ImGui::OpenPopup("Update Notification");
        }

        if (ImGui::BeginPopupModal("Update Notification", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::TextUnformatted("Are you sure you want to update? \n"
                "- Requires restart");
            if (ImGui::Button("Yes", ImVec2(120, 0))) {
                STARTUPINFO startupInfo;
                PROCESS_INFORMATION pi;
                memset(&startupInfo, 0, sizeof(STARTUPINFO));
                startupInfo.cb = sizeof(STARTUPINFO);
                startupInfo.wShowWindow = false;
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
}

/// <summary>
/// Buffer Functions
/// </summary>

void StreamFinderPlugin::discbufferfunc() // Used for the discord tab
{
    std::string discstring;
    std::ifstream webhook(gameWrapper->GetDataFolder() / "StreamFinder" / "cache" / "discord-webhook.txt");
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
    std::ifstream perma(gameWrapper->GetDataFolder() / "StreamFinder" / "cache" / "permanent-blacklist.txt");
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
    std::ifstream temp(gameWrapper->GetDataFolder() / "StreamFinder" / "cache" / "names.txt");
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
    std::ifstream ifpath(gameWrapper->GetDataFolder() / "StreamFinder" / "cache" / "Path-streamlink.txt");
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
    std::ifstream ifpath2(gameWrapper->GetDataFolder() / "StreamFinder" / "cache" / "Path-ffmpeg.txt");
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
    std::ifstream listpath(gameWrapper->GetDataFolder() / "StreamFinder" / "cache" / "Combo-Info00.txt");
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
    auto logpath = gameWrapper->GetDataFolder() / "StreamFinder" / "cache" / "PeaceOfMind.txt";
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
    auto logpath01 = gameWrapper->GetDataFolder() / "StreamFinder" / "cache" / "Session-Blacklist.txt";
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
    auto logpath02 = gameWrapper->GetDataFolder() / "StreamFinder" / "cache" / "livestreamlog.txt";
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
    auto recpath = gameWrapper->GetDataFolder() / "StreamFinder" / "cache" / "streamlink-session.txt";
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

void StreamFinderPlugin::islivebuf() {
    std::string line05;
    std::string IsThisStreamerLive;
    std::ifstream getpathplz(gameWrapper->GetDataFolder() / "StreamFinder" / "cache" / "rec-status.txt");
    while (getline(getpathplz, line05))
    {
        if (line05.empty()) {
            break;
        }
        IsThisStreamerLive += line05 + '\n';
    }
    memset(&streamStatus, 0, _countof(streamStatus)); // init char array
    strncpy_s(streamStatus, IsThisStreamerLive.c_str(), _countof(streamStatus)); // print text to the array/buffer
    getpathplz.close();
}

void StreamFinderPlugin::GetOldVer() {
    auto verpath = gameWrapper->GetDataFolder() / "StreamFinder" / "version.txt";
    std::ifstream verstrm(verpath, std::ios::binary);
    verstrm.unsetf(std::ios::skipws);
    
    std::streampos size02;

    verstrm.seekg(0, std::ios::end);
    size02 = verstrm.tellg();
    verstrm.seekg(0, std::ios::beg);

    std::istream_iterator<char> start(verstrm), end;
    PlugVerOld.reserve(size02);
    PlugVerOld.insert(PlugVerOld.cbegin(), start, end);

    verstrm.close();
}

void StreamFinderPlugin::GetAbout() // Used for the blacklist
{
    std::string line07;
    std::string aboutstr;
    std::ifstream abouttxt(gameWrapper->GetDataFolder() / "StreamFinder" / "about.txt");
    while (getline(abouttxt, line07))
    {
        if (line07.empty()) {
            break;
        }
        aboutstr += line07 + '\n';
    }
    memset(&AboutInfoBuf, 0, _countof(AboutInfoBuf)); // init char array
    strncpy_s(AboutInfoBuf, aboutstr.c_str(), _countof(AboutInfoBuf)); // print text to the array/buffer
    abouttxt.close();
}

void StreamFinderPlugin::RecPathBuf() // Recordings path
{
    std::string recstring;
    std::ifstream recdir(gameWrapper->GetDataFolder() / "StreamFinder" / "cache" / "rec-path.txt");
    getline(recdir, recstring);
    // do outside of render func
    memset(&RecPath, 0, _countof(RecPath)); // init char array
    strncpy_s(RecPath, recstring.c_str(), _countof(RecPath)); // print text to the array/buffer
    recdir.close();
}

/// <summary>
/// Functions
/// </summary>
/// 

void StreamFinderPlugin::OpenChat() {
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
    wsprintfW(tcsCommandLine, L"start /c ""%s\\bakkesmod\\bakkesmod\\data\\StreamFinder\\Open-Chat.bat""", w_app_data_path);
    free(w_app_data_path);
    CreateProcessW(L"C:\\Windows\\System32\\cmd.exe", tcsCommandLine, nullptr, nullptr, TRUE, 0, nullptr, nullptr, (LPSTARTUPINFOW)&startupInfo, &pi);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    // This solution is used to prevent the program from kicking the player out of the Rocket League window.
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
    // This process starting solution is used to prevent the program from kicking the player out of the Rocket League window.
    OpenRecDir();
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

void StreamFinderPlugin::ProcessStatus() {
    if (IsProcessRunning(L"streamlink.exe")) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
        ImGui::Text("RECORDING IN PROGRESS!");
        ImGui::PopStyleColor();
        ImGui::Text("%s", streamStatus);
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 0, 0, 255));
        if (ImGui::Button("Stop")) {
            ImGui::OpenPopup("Streamlink Stop");
        }
        ImGui::PopStyleColor();

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

        ImGui::SameLine();

        if (ImGui::Button("View Recording")) {
            ViewSession();
        }

        if (ImGui::Button("Visit Streamer", ImVec2(150, 0))) {
            OpenChat();
        }
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
        ImGui::Text("No recordings in progress.");
        ImGui::PopStyleColor();
        ImGui::Text("%s", streamStatus);
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(165, 0, 0, 255));
        if (ImGui::Button("Stop")) {
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
        if (ImGui::Button("View Recent", ImVec2(150, 0))) {
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

void StreamFinderPlugin::InstallChck() {
    ImGui::TextUnformatted("Plugin made by P as in Papi   |");
}

void StreamFinderPlugin::RefreshVids() {
    static float padding = 16.0f;
    static float thumbnailSize = 128.0f;
    float cellSize = thumbnailSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;

    int imgWidth;
    int imgHeight;
    if (*previewSize_ == 0) {
        imgWidth = 192;
        imgHeight = 108;
    }
    else if (*previewSize_ == 1) {
        imgWidth = 240;
        imgHeight = 135;
    }
    else {
        imgWidth = 288;
        imgHeight = 162;
    }
    const int gridChildHeight = imgHeight + 56;

    const int moveButtonPadding = 8;

    wchar_t* w_app_data_path;
    size_t sz = 0;
    errno_t err = _wdupenv_s(&w_app_data_path, &sz, L"APPDATA");
    wchar_t ThumbPath[2048]{ 0 };
    wchar_t RecPath[2048]{ 0 };
    wsprintfW(ThumbPath, L"%s\\bakkesmod\\bakkesmod\\data\\StreamFinder\\Recordings\\thumbnails", w_app_data_path);
    wsprintfW(RecPath, L"%s\\bakkesmod\\bakkesmod\\data\\StreamFinder\\Recordings", w_app_data_path);
    free(w_app_data_path);

    for (const auto& entry01 : fs::directory_iterator(ThumbPath)) {
        const auto& path01 = entry01.path();
        std::string imgFile = path01.filename().string();
        std::string filenameStr = '"' + imgFile + '"';
        std::ofstream bl(gameWrapper->GetDataFolder() / "StreamFinder" / "a-path.txt");
        bl << ThumbPath << std::endl;
        bl << filenameStr << std::endl;
        bl.close();

        ImGui::BeginGroup();
        
        const auto& myImage = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "StreamFinder" / "Recordings" / "thumbnails" / filenameStr, true, true);

        if (auto pTex = myImage->GetImGuiTex()) {
            auto rect = myImage->GetSizeF();
            ImGui::Image(pTex, { (float)imgWidth, (float)imgHeight });
        }

        ImGui::SameLine((float)moveButtonPadding);

        ImGui::Text(imgFile.c_str());
        if (ImGui::Button("View", ImVec2(-FLT_MIN, 0.0f))) {
            OpenRecDir();
        }

        if (ImGui::BeginPopupContextItem("Video Context Menu"))
        {
            if (ImGui::Selectable(StartVidTxt.c_str())) // "Start Video"
            {
                ViewSession();
            }

            if (ImGui::Selectable(CopyVidTxt.c_str())) // "Copy"
            {
                OpenRecDir();
            }

            if (ImGui::Selectable(MoveVidTxt.c_str())) // "Move"
            {
                OpenRecDir();
            }

            if (ImGui::Selectable(RenVidTxt.c_str())) // "Rename"
            {
                if (ImGui::BeginPopupModal("Rename Video", NULL, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::BulletText("Video Name:");
                    ImGui::InputText("##VidName", VidName, _countof(VidName));

                    ImGui::Separator();

                    if (ImGui::Button("Save", ImVec2(140, 0))) {
                        std::ofstream videopath(gameWrapper->GetDataFolder() / "StreamFinder" / "Recordings" / "test.txt");
                        videopath << std::string(path00) << endl;
                        videopath.close();
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Close", ImVec2(140, 0))) {
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }
            }

            if (ImGui::Selectable(DelVidTxt.c_str())) // "Delete"
            {
                OpenRecDir();
            }

            if (ImGui::Selectable(CloudTxt.c_str())) // "Upload To Cloud"
            {
                OpenRecDir();
            }

            if (ImGui::Selectable(ffmpegTxt.c_str())) // "Convert Format"
            {
                OpenRecDir();
            }

            if (ImGui::Selectable(InfoVidTxt.c_str())) // "Video Information"
            {
                OpenRecDir();
            }

            ImGui::EndPopup();
        }
        ImGui::EndGroup();
    }
}

void StreamFinderPlugin::VideoManager() {
    ImGui::Text("Video Manager");
    ImGui::Separator();
    /*
    int newPreviewSize = *previewSize_;
    ImGui::TextUnformatted("Preview size: "); ImGui::SameLine();
    ImGui::RadioButton("small", &newPreviewSize, 0); ; ImGui::SameLine();
    ImGui::RadioButton("medium", &newPreviewSize, 1); ; ImGui::SameLine();
    ImGui::RadioButton("large", &newPreviewSize, 2);
    if (newPreviewSize != *previewSize_) {
        *previewSize_ = newPreviewSize;
        previewSizeCVar->setValue(newPreviewSize);
    }
    */
    ImGui::Text("Recordings Directory Path");
    ImGui::InputText("###RecPath", RecPath, _countof(RecPath));

    if (ImGui::Button("Save", ImVec2(140, 0))) {
        std::ofstream recpath(gameWrapper->GetDataFolder() / "StreamFinder" / "rec-path.txt");
        recpath << std::string(RecPath) << endl;
        recpath.close();
        ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button("Open Recordings Folder")) {
        OpenRecDir();
    }

    /*
    ImGui::SameLine();

    if (ImGui::Button("Refresh List", ImVec2(140, 0))) {
        RefreshVids();
    }
    */
}



/// <summary>
/// Rendered Tabs
/// </summary>

void StreamFinderPlugin::renderLoggingTab() {
    if (ImGui::BeginTabItem("Logging")) {
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

        if (ImGui::BeginChild("##Logging", ImVec2(0, 0), true)) {
                // Titles
                ImGui::Columns(3, "Columns");
                ImGui::Separator();
                ImGui::Text("All Streamers Caught"); ImGui::NextColumn();
                ImGui::Text("Current Lobby Status"); ImGui::NextColumn();
                ImGui::Text("Streamers Caught Today"); ImGui::NextColumn();
                ImGui::Separator();

                // All Streamers
                ImGui::Text("%s", buffer04.c_str()); ImGui::NextColumn();

                // Current lobby
                ImGui::Text("%s", buffer02.c_str()); ImGui::NextColumn();

                // Today
                ImGui::Text("%s", buffer03.c_str()); ImGui::NextColumn();
                ImGui::Separator();
        }
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
}

void StreamFinderPlugin::renderStreamlinkTab() {
    if (ImGui::BeginTabItem("Streamlink")) {
        if (ImGui::BeginChild("##Recorder", ImVec2(280, 0), true)) {
            ImGui::TextUnformatted("Streamlink Recorder");
            ImGui::Separator();
            //if (ImGui::Checkbox("Enable Streamlink", &streamlink)) {

            //}
            ImGui::TextUnformatted("-------------------------------------------------------O");
            ImGui::Text("Recorder:");
            ImGui::SameLine();
            ProcessStatus(); // This function also contains the Stop Recording ffbutton
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
            ImGui::TextUnformatted("-------------------------------------------------------O");
            //ImGui::Text("Programs Installed:");
        }

        ImGui::EndChild();
        ImGui::SameLine();

        if (ImGui::BeginChild("##Recordings", ImVec2(0, 0), true)) {
            VideoManager();

            ImGui::TextUnformatted("-------------------------------------------------------------------------O");

            if (ImGui::CollapsingHeader("Help")) {
                ImGui::Text("STREAM FINDER 101");
                ImGui::Separator();
                ImGui::Text("STREAMLINK:");
                ImGui::Text("Install Streamlink to automatically record Twitch Streams in the background with no performance impact!");
                ImGui::Text("- How to install Streamlink -");
                ImGui::BulletText("Click the website below and download the setup file. (Recommended file that ends with \"py311-x86_64.exe\")");
                if (ImGui::Button(link00)) {
                    const wchar_t* url00 = L"https://github.com/streamlink/windows-builds/releases";
                    const wchar_t* action00 = L"Open";
                    ShellExecute(NULL, action00, url00, NULL, NULL, SW_SHOWNORMAL);
                }
                ImGui::BulletText("After the setup, you should be all set... it's that easy!");
                ImGui::Text("- Useful Information -");
                ImGui::BulletText("With Streamlink, you can record streams with no resource-heavy recorders or with websites, it will \n"
                "record these twitch streams in the background during your gameplay once a streamer is found!");
                ImGui::BulletText("When a recording is in session it will tell you in the status in the \"Streamlink Recorder\" block, \n"
                "or a bakkesmod toast (notification) will pop up on the top right hand of your screen. \n"
                "Be sure to have bakkesmod notifications turned on!");
                ImGui::BulletText("The plugin will also use FFmpeg to convert your recordings to mp4, which is useful for editing videos. \n"
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
            if (ImGui::Button("Open Stream Finder Folder")) {
                system("C:\\Windows\\Temp\\directory.vbs");
            }
        }
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
}

// Do ImGui rendering here

void StreamFinderPlugin::Render()
{
    ImGui::SetNextWindowSizeConstraints(ImVec2(950, 700), ImVec2(FLT_MAX, FLT_MAX));
	if (ImGui::Begin(pluginNiceName_.c_str(), &isWindowOpen_, ImGuiWindowFlags_None)) {
        ImGui::Text("%s", PlugVerMain); // Plugin Version
        UpdateNotif();
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
    islivebuf();
    RecPathBuf();

    ComboBuf00();

    isWindowOpen_ = true;
}

// Called when window is closed
void StreamFinderPlugin::OnClose()
{
	isWindowOpen_ = false;
    std::ofstream cin(gameWrapper->GetDataFolder() / "StreamFinder" / "cache" / "rec-status.txt");
    cin << " " << std::endl;
}

void StreamFinderPlugin::ToggleMenu()
{
    cvarManager->executeCommand("togglemenu " + GetMenuName());
}

// Jesus is Lord.
