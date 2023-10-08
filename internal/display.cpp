#include <string>
#include <iomanip>
#include <iostream>
#include "message.h"
#include "messagememory.h"
#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <sys/ioctl.h>
#endif
#include "colors.h"
#include "tools.h"
#include "display.h"
#include "requests.h"
#include "aes.h"
#include "arguments.h"
#include "../includes/nlohmann/json.hpp"
using json = nlohmann::json;

void clearScreen()
{
#ifdef _WIN32
    // Windows
    system("cls");
#elif __linux__
    // Linux
    system("clear");
#else
    // Other or unknown OS
    printf("____________________________________\n");
    printf("Unsupported operating system.\n");
#endif
}

void createLine()
{
    std::cout << WHITE_NORMAL_BACKGROUND WHITE_NORMAL_COLOR << createLineString('_') << std::endl
              << NORMAL;
}

void showHelp(json &lang, json &theme, bool moderator)
{
    clearScreen();
    json hl = lang["commands"]["help"];
    std::cout << theme["themeName"].get<std::string>() << " v" << theme["themeVersion"].get<int>() << std::endl;
    createLine();
    std::cout << BOLD UNDERLINED << hl["title"].get<std::string>() << NORMAL << std::endl
              << std::endl
              << WHITE_NORMAL_COLOR BLACK_NORMAL_BACKGROUND "/h" NORMAL << lang["common"]["or"].get<std::string>() << WHITE_NORMAL_COLOR BLACK_NORMAL_BACKGROUND "/help" NORMAL " - " << hl["helpcmd"].get<std::string>() << std::endl
              << WHITE_NORMAL_COLOR BLACK_NORMAL_BACKGROUND "/q" NORMAL << lang["common"]["or"].get<std::string>() << WHITE_NORMAL_COLOR BLACK_NORMAL_BACKGROUND "/exit" NORMAL " - " << hl["exitcmd"].get<std::string>() << std::endl
              << std::endl
              << hl["rp1"].get<std::string>() << WHITE_NORMAL_COLOR BLACK_NORMAL_BACKGROUND "/r x" NORMAL ITALIC << hl["rp2"].get<std::string>() << NORMAL << std::endl
              << hl["rp2"].get<std::string>() << hl["rp3"].get<std::string>() << WHITE_NORMAL_COLOR BLACK_NORMAL_BACKGROUND "/r" NORMAL << hl["rp4"].get<std::string>() << std::endl
              << hl["rp5"].get<std::string>() << std::endl
              << THIN << hl["rp6"].get<std::string>() << BLACK_NORMAL_BACKGROUND "/ru x" NORMAL THIN " or " BLACK_NORMAL_BACKGROUND "/ru" NORMAL THIN << hl["rp7"].get<std::string>() << NORMAL << std::endl
              << std::endl
              << WHITE_NORMAL_COLOR BLACK_NORMAL_BACKGROUND "/p" << hl["groupname"].get<std::string>() << hl["message"].get<std::string>() << NORMAL " - " << hl["privatecmd"].get<std::string>() << std::endl
              << std::endl
              << hl["gp1"].get<std::string>() << BLACK_NORMAL_BACKGROUND "/g" << hl["groupname"].get<std::string>() << NORMAL << hl["gp2"].get<std::string>() << BLACK_NORMAL_BACKGROUND "/g" NORMAL "." << std::endl
              << hl["gp3"].get<std::string>() << std::endl
              << std::endl
              << WHITE_NORMAL_COLOR BLACK_NORMAL_BACKGROUND "/u" << hl["message"].get<std::string>() << NORMAL " - " << hl["unsafecmd"].get<std::string>() << std::endl
              << std::endl;

    if (moderator)
    {
        std::cout << NORMAL BOLD UNDERLINED << hl["modtitle"].get<std::string>() << NORMAL << std::endl
                  << std::endl
                  << WHITE_NORMAL_COLOR BLACK_NORMAL_BACKGROUND "/delmsg x" NORMAL " or " WHITE_NORMAL_COLOR BLACK_NORMAL_BACKGROUND "/d x" NORMAL " - " << hl["delcmd"].get<std::string>() << std::endl
                  << std::endl;
    }
    std::cout << std::endl
              << hl["endp1"].get<std::string>() << REVERSED BLINK << hl["endp2"].get<std::string>() << NORMAL << hl["endp3"].get<std::string>() << std::endl;
    std::cin.get();
}

int showReplying(json &lang, json &theme, json config, MessageMemory &mem, int id, Message::messageSettings &msgs, Message::isgroupmessage &igm)
{
    Message msg(mem.getMessageByID(id));
    if (msg.getID() < 0)
        return 1;
    igm = msg.isGroupContent(config);
    msg.printReply(theme, msgs, igm);
    if (igm.visible)
        std::cout << THIN "Replying to " << msg.getAuthor() << NORMAL " | " << msgs.pseudo << THIN " -> " NORMAL PURPLE_NORMAL_COLOR "(" << igm.groupname << ")" NORMAL " > ";
    else
        std::cout << THIN "Replying to " << msg.getAuthor() << NORMAL " | " << msgs.pseudo << " > ";
    return 0;
}

int getArguments(json &lang, json &theme, MessageMemory &mem,
                 Message::messageSettings &msgsettings, std::string const &serverurl, json &config,
                 std::string &username, std::string &token, std::string &input,
                 bool const &moderatormode,
                 int &exitUpdateCode, int &exitSendCode)
{
    if (input.length() == 0)
        return 0;
    else if (input.rfind("/q", 0) == 0 || input.rfind("/exit", 0) == 0)
    {
        return -1;
    }
    else if (input.rfind("/h", 0) == 0)
    {
        showHelp(lang, theme, moderatormode);
    }
    else if (moderatormode && (input.rfind("/d", 0) == 0))
    {
        return deleteArg(lang, theme, input, serverurl, username, token, exitSendCode);
    }
    else if (input.rfind("/r", 0) == 0)
    {
        return replyArg(lang, theme, config, mem, input, serverurl, msgsettings, username, token, exitSendCode);
    }
    else if (input.rfind("/u ", 0) == 0)
    {
        return uSendArg(input, serverurl, username, token, exitSendCode);
    }
    else if (input.rfind("/p", 0) == 0)
    {
        return pSendArg(lang, theme, mem, config, input, serverurl, msgsettings, username, token, exitSendCode);
    }
    else if (input.rfind("/g", 0) == 0)
    {
        return groupArg(lang, theme, input, msgsettings, config);
    }
    else
    {
        if (msgsettings.channel != "")
            return pChannelSendArg(lang, theme, mem, config, input, serverurl, msgsettings, username, token, exitSendCode);
        else
            return sendArg(msgsettings, input, serverurl, username, token, exitSendCode);
    }
    return 0;
}


void title(std::string t)
{
    std::cout << "\033]0;" << t << "\007";
}
