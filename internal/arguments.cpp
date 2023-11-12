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
#include "themes.h"

using json = nlohmann::json;

/* DELETED ARGUMENT FUNCTION */

int deleteArg(json &lang, json &theme, std::string &input, std::string const &serverurl, std::string &username, std::string &token, int &exitSendCode)
{
    if (input.rfind("/d ", 0) == 0)
        replaceStringInPlace(input, "/d ", "");
    else if (input.rfind("/delmsg ", 0) == 0)
        replaceStringInPlace(input, "/delmsg ", "");
    else
    {
        userError(theme, "/d" + lang["common"]["and"].get<std::string>() + "/delmsg" + lang["errors"]["idneeded"].get<std::string>());
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

int replyArg(json &lang, json &theme, json config, MessageMemory &mem, std::string &input, std::string const &serverurl, Message::messageSettings &msgsettings, std::string &username, std::string &token, int &exitSendCode)
{
    std::string idtoreply("");
    bool safemode(msgsettings.securemsg);
    if (input.rfind("/r ", 0) == 0)
    {
        replaceStringInPlace(input, "/r ", "");
        idtoreply = input;
    }
    else if (input.rfind("/ru ", 0) == 0)
    {
        replaceStringInPlace(input, "/ru ", "");
        idtoreply = input;
        safemode = false;
    }
    else if (input.rfind("/r", 0) == 0)
    {
        if (input.rfind("/ru", 0) == 0)
            safemode = false;
        clearScreen();
        mem.print(lang, theme, config, msgsettings, true);
        std::cout << lang["commands"]["reply"]["enterid"].get<std::string>();
        std::cout << printStyle(theme["replyInput"]["style"]);
        std::getline(std::cin, input);
        std::cout << NORMAL;
        idtoreply = input;
    }
    else
    {
        userError(theme, lang["errors"]["usehr"].get<std::string>());
        std::cin.get();
        return 1;
    }

    clearScreen();
    mem.print(lang, theme, config, msgsettings);

    int intidtoreply;
    if (idtoreply.size() == 0)
    {
        userError(theme, lang["errors"]["missingId"].get<std::string>());
        std::cin.get();
        return 2;
    }
    else
    {
        try
        {
            intidtoreply = std::stoi(idtoreply);
        }
        catch (...)
        {
            userError(theme, lang["errors"]["invalidID"].get<std::string>());
            std::cin.get();
            return 2;
        }
    }

    Message::isgroupmessage igmreply;
    if (showReplying(lang, theme, config, mem, intidtoreply, msgsettings, igmreply) > 0)
    {
        userError(theme, lang["errors"]["msgnotfound"].get<std::string>());
        std::cin.get();
        return 2;
    }
    std::cout << printStyle(theme["messageInput"]["style"]);
    std::getline(std::cin, input);
    std::cout << NORMAL;
    if (input.length() > 0 && input.find_first_not_of(' ') != std::string::npos)
    {
        if (safemode || igmreply.visible)
        {
            std::string privatereplyprefix("");
            unsigned char decryptedText[MsgSettings::msgmaxsize] = {};
            unsigned char key[40] = "";
            unsigned char encryptedText[MsgSettings::encryptedmaxsize] = {};

            std::copy(input.cbegin(), input.cend(), decryptedText);
            if (igmreply.visible)
            {
                privatereplyprefix = "团" + igmreply.groupname;
                std::copy(igmreply.key.cbegin(), igmreply.key.cend(), key);
            }
            else
                std::copy(msgsettings.generalkey.cbegin(), msgsettings.generalkey.cend(), key);
            AES(decryptedText, key, encryptedText);

            std::string encryptedInput(reinterpret_cast<char *>(encryptedText));
            exitSendCode = sendMessage(serverurl, privatereplyprefix + "答" + idtoreply + "护" + encryptedInput, username, token);
        }
        else
            exitSendCode = sendMessage(serverurl, +"答" + idtoreply + "护" + input, username, token);
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
    replaceStringInPlace(input, "/u ", "");
    if (input.length() > 0 && input.find_first_not_of(' ') != std::string::npos)
    {
        exitSendCode = sendMessage(serverurl, input, username, token);
        if (exitSendCode != 0)
        {
            std::cout << "SEND UNSAFE ERROR " << exitSendCode << std::endl;
            return -exitSendCode;
        }
        return 3;
    }
    return 0;
}

/* PRIVATE GROUP MESSAGE SEND */

int pSendArg(json &lang, json &theme, MessageMemory &mem, json &config, std::string &input, std::string const &serverurl, Message::messageSettings &msgsettings, std::string &username, std::string &token, int &exitSendCode)
{
    if (!msgsettings.securemsg)
    {
        userError(theme, lang["errors"]["featnotavailable"].get<std::string>());
        std::cin.get();
        return 2;
    }
    std::string privategroupname("");
    if (input.rfind("/p ", 0) == 0)
    {
        replaceStringInPlace(input, "/p ", "");
        privategroupname = input;
    }
    else if (input.rfind("/p", 0) == 0)
    {
        std::cout << lang["commands"]["group"]["entergroup"].get<std::string>();
        std::cout << printStyle(theme["groupInput"]["style"]);
        std::getline(std::cin, input);
        std::cout << NORMAL;
        privategroupname = input;
    }
    else
    {
        userError(theme, lang["errors"]["userhpg"].get<std::string>());
        std::cin.get();
        return 1;
    }

    privategroup privateg = findPrivateGroup(config, privategroupname);
    if (!privateg.found)
    {
        userError(theme, lang["errors"]["groupnotfound"].get<std::string>());
        std::cin.get();
        return 2;
    }

    clearScreen();
    mem.print(lang, theme, config, msgsettings);
    themeVariables tv = themeVariables(false, msgsettings.channel != "", true, msgsettings.modmsg, msgsettings.pseudo, "", "", msgsettings.channel, privategroupname, "X");
    themeProcessPrint(lang, theme, "prompt", tv);
    std::cout << printStyle(theme["messageInput"]["style"]);
    std::getline(std::cin, input);
    std::cout << NORMAL;
    if (input.length() > 0 && input.find_first_not_of(' ') != std::string::npos)
    {
        unsigned char decryptedText[MsgSettings::msgmaxsize] = {};
        unsigned char key[40] = "";
        unsigned char encryptedText[MsgSettings::encryptedmaxsize] = {};

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
        unsigned char decryptedText[MsgSettings::msgmaxsize] = {};
        unsigned char key[40] = "";
        unsigned char encryptedText[MsgSettings::encryptedmaxsize] = {};

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

/* GROUP VISION COMMAND */

int groupArg(json &lang, json &theme, std::string &input, Message::messageSettings &msettings, json &config)
{
    if (!msettings.securemsg)
    {
        userError(theme, lang["errors"]["featnotavailable"].get<std::string>());
        std::cin.get();
        return 2;
    }

    std::string privategroupname("");

    if (input.rfind("/g ", 0) == 0)
    {
        replaceStringInPlace(input, "/g ", "");
        privategroupname = input;
    }
    else
    {
        title("MicaClient++ - General");
        msettings.channel = "";
        return 0;
    }

    privategroup privateg = findPrivateGroup(config, privategroupname);
    if (!privateg.found)
    {
        userError(theme, lang["errors"]["groupnotfound"].get<std::string>());
        std::cin.get();
        return 2;
    }

    title("MicaClient++ - (" + privategroupname + ")");
    msettings.channel = privategroupname;
    return 0;
}

/* PRIVATE GROUP CHANNEL MESSAGE SEND */

int pChannelSendArg(json &lang, json &theme, MessageMemory &mem, json &config, std::string &input, std::string const &serverurl, Message::messageSettings &msgsettings, std::string &username, std::string &token, int &exitSendCode)
{
    if (!msgsettings.securemsg)
    {
        userError(theme, lang["errors"]["featnotavailable"].get<std::string>());
        std::cin.get();
        return 2;
    }
    std::string privategroupname(msgsettings.channel);

    privategroup privateg = findPrivateGroup(config, privategroupname);
    if (!privateg.found)
    {
        userError(theme, lang["errors"]["groupnotfound"].get<std::string>());
        std::cin.get();
        return 2;
    }

    if (input.length() > 0 && input.find_first_not_of(' ') != std::string::npos)
    {
        unsigned char decryptedText[MsgSettings::msgmaxsize] = {};
        unsigned char key[40] = "";
        unsigned char encryptedText[MsgSettings::encryptedmaxsize] = {};

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