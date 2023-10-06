#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "includes/nlohmann/json.hpp"
#include "internal/message.h"
#include "internal/messagememory.h"
#include "internal/aes.h"
#include "internal/requests.h"
#include "internal/tools.h"
#include "internal/display.h"
#include "internal/colors.h"

using json = nlohmann::json;

const int MCPP_VERSION(0);
const int CONFIG_VERSION(9);

const int THEME_VERSION(1);
const int LANGUAGE_VERSION(1);

const int MSG_MAX_SIZE(490);

int main(int argc, char const *argv[])
{
    std::cout << "Starting MicaClient++..." << std::endl;
    title("MicaClient++ - Starting...");
    int exitUpdateCode(0), exitSendCode(0);
    std::vector<std::string> args(argv, argv + argc);

    std::string cfgFilePath("config.json");
    bool moderatormode(false);

    if (!arguments(args, cfgFilePath, moderatormode))
        return 100;

    std::ifstream f(cfgFilePath);
    json data = json::parse(f);
    f.close();
    // std::cout << data << std::endl;

    std::string serverurl, username, token, genkey, cfgPath;
    bool encryptenabled;
    Message::messageSettings msgsettings;
    MessageMemory::memorySettings memsettings;

    try
    {
        int cfgv = data["version"].get<int>();
        int cfgsv = data["subversion"].get<int>();
        if (cfgv != MCPP_VERSION || cfgsv != CONFIG_VERSION)
        {
            std::cout << BOLD RED_NORMAL_COLOR "ERROR: CONFIG VERSION IS NOT COMPATIBLE" NORMAL << std::endl
                      << "Please update your config to be able to use it with MicaClient++ v" << MCPP_VERSION << "." << CONFIG_VERSION << ", (found v" << cfgv << "." << cfgsv << ")" << std::endl;
            return 7;
        }
        genkey = data["generalKey"].get<std::string>();
        encryptenabled = data["enableEncryption"].get<bool>();
        serverurl = data["server"].get<std::string>();
        username = data["username"].get<std::string>();
        token = data["token"].get<std::string>();
        cfgPath = data["mcppConfigPath"].get<std::string>();
        msgsettings = Message::messageSettings(data["settings"]["displayDeletedMessages"].get<bool>(),
                                               data["settings"]["displayOfflineMessages"].get<bool>(),
                                               genkey, data["settings"]["showMessageDateTime"].get<bool>(),
                                               username, moderatormode, encryptenabled,
                                               data["blockUnVerifiedUsers"].get<bool>(),
                                               data["blockedUsers"],
                                               "",
                                               MSG_MAX_SIZE);
        memsettings = MessageMemory::memorySettings(data["settings"]["backupMessages"].get<bool>(), cfgPath);
    }
    catch (std::out_of_range &e)
    {
        std::cout << "JSON ERROR : " << e.what() << '\n';
        return 1;
    }

    json themeData;
    try
    {
        std::string themefpath = data["themeFile"].get<std::string>();
        std::ifstream f(themefpath);
        std::cout << "Loading theme (" << themefpath << ")..." << std::endl;
        themeData = json::parse(f);
        int themev = themeData["compatibilityVersion"].get<int>();
        if (themev != THEME_VERSION)
        {
            std::cout << BOLD RED_NORMAL_COLOR "ERROR: THEME VERSION IS NOT COMPATIBLE" NORMAL << std::endl
                      << "Please update your theme to be able to use it with MicaClient++ theme manager v" << THEME_VERSION << ", (found v" << themev << ")" << std::endl;
            return 7;
        }
        f.close();
    }
    catch (std::out_of_range &e)
    {
        std::cout << "JSON THEME ERROR : " << e.what() << '\n';
        return 1;
    }
    json languageData;
    try
    {
        std::string languagefpath = data["languageFile"].get<std::string>();
        std::ifstream f(languagefpath);
        std::cout << "Loading language (" << languagefpath << ")..." << std::endl;
        languageData = json::parse(f);
        int langv = languageData["compatibilityVersion"].get<int>();
        if (langv != LANGUAGE_VERSION)
        {
            std::cout << BOLD RED_NORMAL_COLOR "ERROR: LANGUAGE VERSION IS NOT COMPATIBLE" NORMAL << std::endl
                      << "Please update your language to be able to use it with MicaClient++ theme manager v" << LANGUAGE_VERSION << ", (found v" << langv << ")" << std::endl;
            return 7;
        }
        title("MicaClient++ - " + languageData["window"]["gentitle"].get<std::string>());
        f.close();
    }
    catch (std::out_of_range &e)
    {
        std::cout << "JSON LANGAGE ERROR : " << e.what() << '\n';
        return 1;
    }

    MessageMemory mem;
    if (memsettings.backup)
    {

        std::ifstream bf(cfgPath + "backup.json");
        if (!bf.is_open())
        {
            std::cout << "No backup found" << std::endl;
        }
        else
        {
            json backupData = json::parse(bf);
            mem.importMemory(backupData);
        }
    }

    std::string input;
    int resarg(0);
    while (true)
    {
        exitUpdateCode = getServerUpdate(serverurl, mem, memsettings);
        if (exitUpdateCode == 0)
        {
            clearScreen();
            mem.print(themeData, data, msgsettings);
        }
        else
        {
            std::cout << "UPDATE ERROR " << exitUpdateCode << std::endl;
            return exitUpdateCode;
        }
        if (msgsettings.channel != "") {
            std::cout << PURPLE_NORMAL_COLOR "(" BOLD << msgsettings.channel << NORMAL PURPLE_NORMAL_COLOR ") " NORMAL;
        }
        std::cout << username << " > ";
        std::cout << printStyle(themeData["messageInput"]["style"]);
        std::cin.clear();
        std::getline(std::cin, input);
        std::cout << NORMAL;
        resarg = getArguments(languageData, themeData, mem, msgsettings, serverurl, data, username, token, input, moderatormode, exitUpdateCode, exitSendCode);
        if (resarg < 0)
            break;
        input = "";
    }

    clearScreen();
    std::cout << "Exiting MicaClient++..." << std::endl;

    return 0;
}