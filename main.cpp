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

int main(int argc, char const *argv[])
{
    int exitUpdateCode(0), exitSendCode(0);
    std::vector<std::string> args(argv, argv + argc);

    std::string cfgPath("config.json");
    bool moderatormode(false);

    if (!arguments(args, cfgPath, moderatormode))
        return 100;

    std::ifstream f(cfgPath);
    json data = json::parse(f);
    // std::cout << data << std::endl;

    std::string serverurl, username, token, genkey;
    bool encryptenabled;
    Message::messageSettings msgsettings;
    MessageMemory::memorySettings memsettings;

    try
    {
        genkey = data["generalKey"].get<std::string>();
        encryptenabled = data["enableEncryption"].get<bool>();
        serverurl = data["server"].get<std::string>();
        username = data["username"].get<std::string>();
        token = data["token"].get<std::string>();
        msgsettings = Message::messageSettings(data["settings"]["displayDeletedMessages"].get<bool>(), data["settings"]["displayOfflineMessages"].get<bool>(), genkey, data["settings"]["showMessageDateTime"].get<bool>(), username, moderatormode, encryptenabled);
        memsettings = MessageMemory::memorySettings(data["settings"]["backupMessages"].get<bool>());
    }
    catch (std::out_of_range &e)
    {
        std::cout << "JSON ERROR : " << e.what() << '\n';
        return 1;
    }

    MessageMemory mem;

    if (memsettings.backup)
    {

        std::ifstream bf("backup.json");
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
            mem.print(msgsettings);
        }
        else
        {
            std::cout << "UPDATE ERROR " << exitUpdateCode << std::endl;
            return exitUpdateCode;
        }
        std::cout << username << " > ";
        std::getline(std::cin, input);

        resarg = getArguments(mem, msgsettings, serverurl, data, username, token, input, moderatormode, exitUpdateCode, exitSendCode);
        if (resarg < 0)
            break;
        input = "";
    }

    std::cout << "Exiting MicaClient++..." << std::endl;

    return 0;
}