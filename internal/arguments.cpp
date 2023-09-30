#include <string>
#include <iomanip>
#include <iostream>
#include "message.h"
#include "messagememory.h"
#include "colors.h"
#include "tools.h"
#include "display.h"
#include "requests.h"
#include "aes.h"
#include "arguments.h"

/* DELETED ARGUMENT FUNCTION */

int deleteArg(std::string &input, std::string const &serverurl, std::string &username, std::string &token, int &exitSendCode)
{
    if (input.rfind("/d ", 0) == 0)
        ReplaceStringInPlace(input, "/d ", "");
    else if (input.rfind("/delmsg ", 0) == 0)
        ReplaceStringInPlace(input, "/delmsg ", "");
    else
    {
        std::cout << RED_NORMAL_COLOR << "/d and /delmsg take the id as argument !" << std::endl;
        std::cin.get();
        return 1;
    }
    exitSendCode = delMessage(serverurl, input, username, token);

    if (exitSendCode != 0)
    {
        std::cout << "SEND UNSAFE ERROR " << exitSendCode << std::endl;
        return -exitSendCode;
    }
    return 3;
}

/* REPLY ARGUMENTS FONCTION */

int replyArg(json config, MessageMemory &mem, std::string &input, std::string const &serverurl, Message::messageSettings &msgsettings, std::string &username, std::string &token, int &exitSendCode)
{
    std::string idtoreply("");
    bool safemode(msgsettings.securemsg);
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
    else if (input.rfind("/r", 0) == 0)
    {
        if (input.rfind("/ru", 0) == 0)
            safemode = false;
        clearScreen();
        mem.print(config, msgsettings, true);
        std::cout << "Type the [ID] of the message you want to answer: ";
        std::getline(std::cin, input);
        idtoreply = input;
    }
    else
    {
        std::cout << RED_NORMAL_COLOR << "Use /help to know how to use replies" << std::endl;
        std::cin.get();
        return 1;
    }

    clearScreen();
    mem.print(config, msgsettings);

    if (showReplying(mem, stoi(idtoreply), msgsettings) > 0)
    {
        std::cout << RED_NORMAL_COLOR << "Impossible to find this message" << std::endl;
        std::cin.get();
        return 2;
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
            std::copy(msgsettings.generalkey.cbegin(), msgsettings.generalkey.cend(), key);
            AES(decryptedText, key, encryptedText);

            std::string encryptedInput(reinterpret_cast<char *>(encryptedText));
            exitSendCode = sendMessage(serverurl, "答" + idtoreply + "护" + encryptedInput, username, token);
        }
        else
            exitSendCode = sendMessage(serverurl, "答" + idtoreply + "护" + input, username, token);
        if (exitSendCode != 0)
        {
            std::cout << "SEND REPLY ERROR " << exitSendCode << std::endl;
            return -exitSendCode;
        }
        return 3;
    }
    return 0;
}

/* UNSAFE MESSAGE SEND */

int uSendArg(std::string &input, std::string const &serverurl, std::string &username, std::string &token, int &exitSendCode)
{
    ReplaceStringInPlace(input, "/u ", "");
    exitSendCode = sendMessage(serverurl, input, username, token);
    if (exitSendCode != 0)
    {
        std::cout << "SEND UNSAFE ERROR " << exitSendCode << std::endl;
        return -exitSendCode;
    }
    return 3;
}

/* PRIVATE GROUP MESSAGE SEND */
int pSendArg(MessageMemory &mem, json &config, std::string &input, std::string const &serverurl, Message::messageSettings &msgsettings, std::string &username, std::string &token, int &exitSendCode)
{
    std::string privategroupname("");
    if (input.rfind("/p ", 0) == 0)
    {
        ReplaceStringInPlace(input, "/p ", "");
        privategroupname = input;
    }
    else if (input.rfind("/p", 0) == 0)
    {
        std::cout << "Enter the name of the group: " PURPLE_NORMAL_COLOR;
        std::getline(std::cin, input);
        privategroupname = input;
    }
    else
    {
        std::cout << RED_NORMAL_COLOR << "Use /help to know how to use private groups" << std::endl;
        std::cin.get();
        return 1;
    }

    privategroup privateg = findPrivateGroup(config, privategroupname);
    if (!privateg.found)
    {
        std::cout << RED_NORMAL_COLOR << "Impossible to find this group" << std::endl;
        std::cin.get();
        return 2;
    }

    clearScreen();
    mem.print(config, msgsettings);
    std::cout << username << THIN " -> " NORMAL PURPLE_NORMAL_COLOR "(" << privategroupname << ")" NORMAL " > ";
    std::getline(std::cin, input);
    if (input.length() > 0)
    {
        unsigned char decryptedText[490] = "";
        unsigned char key[40] = "";
        unsigned char encryptedText[980] = "";

        std::copy(input.cbegin(), input.cend(), decryptedText);
        std::copy(privateg.key.cbegin(), privateg.key.cend(), key);
        AES(decryptedText, key, encryptedText);

        std::string encryptedInput(reinterpret_cast<char *>(encryptedText));
        exitSendCode = sendMessage(serverurl, "团" + privategroupname + "护" + encryptedInput, username, token);
        if (exitSendCode != 0)
        {
            std::cout << "SEND PRIVATE GROUP ERROR " << exitSendCode << std::endl;
            return -exitSendCode;
        }
        return 3;
    }
    return 0;
}

/* MESSAGE SEND ARGUMENT FONCTION */

int sendArg(Message::messageSettings &msgsettings, std::string &input, std::string const &serverurl, std::string &username, std::string &token, int &exitSendCode)
{
    if (msgsettings.securemsg)
    {
        unsigned char decryptedText[490] = "";
        unsigned char key[40] = "";
        unsigned char encryptedText[980] = "";

        std::copy(input.cbegin(), input.cend(), decryptedText);
        std::copy(msgsettings.generalkey.cbegin(), msgsettings.generalkey.cend(), key);
        AES(decryptedText, key, encryptedText);

        std::string encryptedInput(reinterpret_cast<char *>(encryptedText));
        exitSendCode = sendMessage(serverurl, "护" + encryptedInput, username, token);
    }
    else
        exitSendCode = sendMessage(serverurl, input, username, token);

    if (exitSendCode != 0)
    {
        std::cout << "SEND ERROR " << exitSendCode << std::endl;
        return -exitSendCode;
    }
    return 3;
}
