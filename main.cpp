#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <fstream>
#include "includes/nlohmann/json.hpp"
#include "internal/message.h"
#include "internal/messagememory.h"
#include "internal/aes.h"
#include "internal/requests.h"
#include "internal/tools.h"

using json = nlohmann::json;

int main(int argc, char const *argv[])
{
    int exitUpdateCode(0), exitSendCode(0);

    std::ifstream f("config.json");
    json data = json::parse(f);
    // std::cout << data << std::endl;

    std::string serverurl, geturl, posturl, username, token;
    Message::messageSettings msgsettings;
    MessageMemory::memorySettings memsettings;

    try
    {
        serverurl = data["server"].get<std::string>();
        geturl = serverurl + "msg.php?getmsg=json";
        posturl = serverurl + "/msg.php?";
        username = data["username"].get<std::string>();
        token = data["token"].get<std::string>();
        msgsettings = Message::messageSettings(data["settings"]["displayDeletedMessages"].get<bool>(), data["settings"]["displayOfflineMessages"].get<bool>());
        memsettings = MessageMemory::memorySettings(data["settings"]["backupMessages"].get<bool>());
    }
    catch (std::out_of_range &e)
    {
        std::cout << "JSON ERROR : " << e.what() << '\n';
        return 1;
    }

    MessageMemory mem;

    std::ifstream bf("backup.json");
    json backupData = json::parse(bf);

    mem.importMemory(backupData);

    std::string input;
    while (true)
    {
        exitUpdateCode = getServerUpdate(geturl, mem, memsettings);
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

        if (input == "/exit")
        {
            break; // Exit program if /exit
        }
        else
        {
            exitSendCode = sendMessage(posturl, input, username, token);
            if (exitUpdateCode != 0)
            {
                std::cout << "SEND ERROR " << exitUpdateCode << std::endl;
                return exitUpdateCode;
            }
        }
    }

    std::cout << "Exiting MicaClient++..." << std::endl;

    return 0;
}