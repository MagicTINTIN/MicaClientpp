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
#include "internal/importer.h"
#include "internal/themes.h"
#include <chrono>
#include <thread>

using json = nlohmann::json;

const int MCPP_VERSION(1);
const int CONFIG_VERSION(1);
const int PROGRAM_VERSION(3);

const int THEME_VERSION(3);
const int LANGUAGE_VERSION(1);

const std::string VERSION(std::to_string(MCPP_VERSION) + "." + std::to_string(CONFIG_VERSION) + "." + std::to_string(PROGRAM_VERSION));

const int MSG_MAX_SIZE(MsgSettings::msgmaxsize);

int main(int argc, char const *argv[])
{
    std::cout << "Starting MicaClient++..." << std::endl;
    title("MicaClient++ - Starting...");
    displayLogo();
    int exitUpdateCode(0), exitSendCode(0);
    std::vector<std::string> args(argv, argv + argc);

    std::string cfgFilePath("config.json");
    bool moderatormode(false);

    int processArgs = arguments(args, cfgFilePath, moderatormode, VERSION);
    if (processArgs < 0)
        return 100;
    else if (processArgs == 1)
        return 0;

    json themeData, languageData, data;
    std::string serverurl, username, token, genkey, cfgPath, input;
    bool encryptenabled;
    Message::messageSettings msgsettings;
    MessageMemory::memorySettings memsettings;
    MessageMemory mem;

    // import data contained in json files
    int icfg = configImporter(MCPP_VERSION, CONFIG_VERSION, MSG_MAX_SIZE, cfgFilePath, data, encryptenabled, moderatormode, genkey, serverurl, username, token, cfgPath, msgsettings, memsettings);
    if (icfg > 0)
        return icfg;
    icfg = themeImporter(THEME_VERSION, themeData, data);
    if (icfg > 0)
        return icfg;
    icfg = languageImporter(LANGUAGE_VERSION, languageData, data);
    if (icfg > 0)
        return icfg;
    icfg = backupImporter(memsettings, mem, cfgPath);
    if (icfg > 0)
        return icfg;

    //make the logo appears for x seconds
    std::this_thread::sleep_for(std::chrono::seconds(themeData["displayLogoDuration"].get<int>()));

    int resarg(0);
    while (true)
    {
        exitUpdateCode = getServerUpdate(serverurl, mem, memsettings);
        if (exitUpdateCode == 0)
        {
            clearScreen();
            mem.print(languageData, themeData, data, msgsettings);
        }
        else
        {
            std::cout << "UPDATE ERROR " << exitUpdateCode << std::endl;
            return exitUpdateCode;
        }

        themeVariables tv = themeVariables(false, msgsettings.channel != "", false, msgsettings.modmsg, msgsettings.pseudo, "", "", msgsettings.channel, "", "X");
        themeProcessPrint(languageData, themeData, "prompt", tv);
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