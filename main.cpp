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
        msgsettings = Message::messageSettings(data["settings"]["displayDeletedMessages"].get<bool>(), data["settings"]["displayOfflineMessages"].get<bool>(), genkey, data["settings"]["showMessageDateTime"].get<bool>(), username, moderatormode);
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

        if (input.length() == 0)
            continue;
        else if (input.rfind("/exit", 0) == 0)
        {
            break; // Exit program if /exit
        }
        else if (input.rfind("/help", 0) == 0 || input.rfind("/h", 0) == 0)
        {
            showHelp(moderatormode);
        }
        else if (moderatormode && (input.rfind("/d", 0) == 0 || input.rfind("/delmsg", 0) == 0))
        {
            if (input.rfind("/d ", 0) == 0)
                ReplaceStringInPlace(input, "/d ", "");
            else if (input.rfind("/delmsg ", 0) == 0)
                ReplaceStringInPlace(input, "/delmsg ", "");
            else
            {
                std::cout << RED_NORMAL_COLOR << "/d and /delmsg take the id as argument !" << std::endl;
                std::cin.get();
                continue;
            }
            exitSendCode = delMessage(serverurl, input, username, token);

            if (exitUpdateCode != 0)
            {
                std::cout << "SEND UNSAFE ERROR " << exitUpdateCode << std::endl;
                return exitUpdateCode;
            }
        }
        else if (input.rfind("/r", 0) == 0)
        {
            std::string idtoreply("");
            bool safemode(encryptenabled);
            if (input.rfind("/r ", 0) == 0)
            {
                ReplaceStringInPlace(input, "/r ", "");
                idtoreply = input;
            }
            else if (input.rfind("/ru ", 0) == 0)
            {
                ReplaceStringInPlace(input, "/ru ", "");
                idtoreply = input;
                safemode = false;
            }
            else if (input.rfind("/ru", 0) == 0)
            {
                safemode = false;
                clearScreen();
                mem.print(msgsettings, true);
                std::cout << "Type the [ID] of the message you want to answer: ";
                std::getline(std::cin, input);
                idtoreply = input;
            }
            else if (input.rfind("/r", 0) == 0)
            {
                clearScreen();
                mem.print(msgsettings, true);
                std::cout << "Type the [ID] of the message you want to answer: ";
                std::getline(std::cin, input);
                idtoreply = input;
            }
            else
            {
                std::cout << RED_NORMAL_COLOR << "Use /help to know how to use replies" << std::endl;
                std::cin.get();
                continue;
            }

            clearScreen();
            mem.print(msgsettings);

            if (showReplying(mem, stoi(idtoreply), msgsettings) > 0)
            {
                std::cout << RED_NORMAL_COLOR << "Impossible to find this message" << std::endl;
                std::cin.get();
                continue;
            }
            std::getline(std::cin, input);
            if (input.length() > 0)
            {
                if (safemode)
                {
                    unsigned char decryptedText[490] = "";
                    unsigned char key[40] = "";
                    unsigned char encryptedText[980] = "";

                    std::copy(input.cbegin(), input.cend(), decryptedText);
                    std::copy(genkey.cbegin(), genkey.cend(), key);
                    AES(decryptedText, key, encryptedText);

                    std::string encryptedInput(reinterpret_cast<char *>(encryptedText));
                    exitSendCode = sendMessage(serverurl, "答" + idtoreply + "护" + encryptedInput, username, token);
                }
                else
                    exitSendCode = sendMessage(serverurl, "答" + idtoreply + "护" + input, username, token);
                if (exitUpdateCode != 0)
                {
                    std::cout << "SEND REPLY ERROR " << exitUpdateCode << std::endl;
                    return exitUpdateCode;
                }
            }
        }
        else if (input.rfind("/u ", 0) == 0)
        {
            ReplaceStringInPlace(input, "/u ", "");
            exitSendCode = sendMessage(serverurl, input, username, token);
            if (exitUpdateCode != 0)
            {
                std::cout << "SEND UNSAFE ERROR " << exitUpdateCode << std::endl;
                return exitUpdateCode;
            }
        }
        else if (input.rfind("/p", 0) == 0)
        {
            std::string privategroup("");
            if (input.rfind("/p ", 0) == 0)
            {
                ReplaceStringInPlace(input, "/p ", "");
                privategroup = input;
            }
            else if (input.rfind("/p", 0))
            {
                std::cout << "Type the name of the group: ";
                std::getline(std::cin, input);
                privategroup = input;
            }
            else
            {
                std::cout << RED_NORMAL_COLOR << "Use /help to know how to use private groups" << std::endl;
                std::cin.get();
                continue;
            }

            if (showReplying(mem, group, msgsettings) > 0)
            {
                std::cout << RED_NORMAL_COLOR << "Impossible to find this message" << std::endl;
                std::cin.get();
                continue;
            }
            std::getline(std::cin, input);
            if (input.length() > 0)
            {
                if (safemode)
                {
                    unsigned char decryptedText[490] = "";
                    unsigned char key[40] = "";
                    unsigned char encryptedText[980] = "";

                    std::copy(input.cbegin(), input.cend(), decryptedText);
                    std::copy(genkey.cbegin(), genkey.cend(), key);
                    AES(decryptedText, key, encryptedText);

                    std::string encryptedInput(reinterpret_cast<char *>(encryptedText));
                    exitSendCode = sendMessage(serverurl, "答" + idtoreply + "护" + encryptedInput, username, token);
                }
                else
                    exitSendCode = sendMessage(serverurl, "答" + idtoreply + "护" + input, username, token);
                if (exitUpdateCode != 0)
                {
                    std::cout << "SEND REPLY ERROR " << exitUpdateCode << std::endl;
                    return exitUpdateCode;
                }
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
                exitSendCode = sendMessage(serverurl, "护" + encryptedInput, username, token);
            }
            else
                exitSendCode = sendMessage(serverurl, input, username, token);

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