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

using json = nlohmann::json;

int main(int argc, char const *argv[])
{
    int exitUpdateCode(0), exitSendCode(0);

    std::ifstream f("config.json");
    json data = json::parse(f);
    //std::cout << data << std::endl;

    std::string serverurl, geturl, posturl, username, token, genkey;
    bool encryptenabled;
    Message::messageSettings msgsettings;
    MessageMemory::memorySettings memsettings;

    try
    {
        genkey = data["generalKey"].get<std::string>();
        encryptenabled = data["enableEncryption"].get<bool>();
        serverurl = data["server"].get<std::string>();
        geturl = serverurl + "msg.php?getmsg=json";
        posturl = serverurl + "/msg.php?";
        username = data["username"].get<std::string>();
        token = data["token"].get<std::string>();
        msgsettings = Message::messageSettings(data["settings"]["displayDeletedMessages"].get<bool>(), data["settings"]["displayOfflineMessages"].get<bool>(), genkey, data["settings"]["showMessageDateTime"].get<bool>(), username);
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

        if (input.length() == 0)
            continue;
        else if (input.rfind("/exit", 0) == 0)
        {
            break; // Exit program if /exit
        }
        else if (input.rfind("/help", 0) == 0)
        {
            showHelp();
        }
        else if (input.rfind("/u ", 0) == 0)
        {
            ReplaceStringInPlace(input, "/u ", "");
            exitSendCode = sendMessage(posturl, input, username, token);
            if (exitUpdateCode != 0)
            {
                std::cout << "SEND UNSAFE ERROR " << exitUpdateCode << std::endl;
                return exitUpdateCode;
            }
        }
        else
        {
            if (encryptenabled)
            {
                unsigned char decryptedText[490] = "";
                unsigned char key[40] = "";
                unsigned char encryptedText[980] = "";

                std::copy(input.cbegin(), input.cend(), decryptedText);
                std::copy(genkey.cbegin(), genkey.cend(), key);
                AES(decryptedText, key, encryptedText);

                std::string encryptedInput(reinterpret_cast<char *>(encryptedText));
                exitSendCode = sendMessage(posturl, "护" + encryptedInput, username, token);
            }
            else
                exitSendCode = sendMessage(posturl, input, username, token);
            
            
            if (exitUpdateCode != 0)
            {
                std::cout << "SEND ERROR " << exitUpdateCode << std::endl;
                return exitUpdateCode;
            }
        }

        input = "";
    contin:;
    }

    std::cout << "Exiting MicaClient++..." << std::endl;

    return 0;
}