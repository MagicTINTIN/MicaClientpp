
#include <string>
#include <iomanip>
#include <iostream>
#include <fstream>
#include "message.h"
#include "messagememory.h"
#include "colors.h"
#include "tools.h"
#include "display.h"
#include "../includes/nlohmann/json.hpp"
#include "importer.h"
using json = nlohmann::json;

int configImporter(int const &MCPP_VERSION, int const &CONFIG_VERSION, int const &MSG_MAX_SIZE,
                   std::string &cfgFilePath, json &data,
                   bool &encryptenabled, bool &moderatormode,
                   std::string &genkey, std::string &serverurl, std::string &username, std::string &token, std::string &cfgPath,
                   Message::messageSettings &msgsettings, MessageMemory::memorySettings &memsettings)
{
    std::ifstream f(cfgFilePath);
    data = json::parse(f);
    f.close();
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
    return 0;
}

int themeImporter(int const &THEME_VERSION,
                  json &themeData, json &data)
{
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
    return 0;
}

int languageImporter(int const &LANGUAGE_VERSION,
                  json &languageData, json &data)
{
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
    return 0;
}

int backupImporter(MessageMemory::memorySettings &memsettings, MessageMemory &mem, std::string &cfgPath)
{
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
    return 0;
}