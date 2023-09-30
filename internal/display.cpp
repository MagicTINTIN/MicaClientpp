
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

void showHelp(bool moderator)
{
    clearScreen();
    createLine();
    std::cout << BOLD UNDERLINED << "Here is the list of commands" NORMAL << std::endl
              << std::endl
              << WHITE_NORMAL_COLOR BLACK_NORMAL_BACKGROUND "/h" NORMAL " or " WHITE_NORMAL_COLOR BLACK_NORMAL_BACKGROUND "/help" NORMAL " - To show this message" << std::endl
              << std::endl
              << "To reply to a message type " WHITE_NORMAL_COLOR BLACK_NORMAL_BACKGROUND "/r x" NORMAL ITALIC " (with x the message ID to reply)." NORMAL << std::endl
              << "If you don't know it, only type " WHITE_NORMAL_COLOR BLACK_NORMAL_BACKGROUND "/r" NORMAL ", then choose the ID of the message you want to reply" << std::endl
              << "Then, press [ENTER], and you will be able to write and send your answer ! " << std::endl
              << THIN "You can use " BLACK_NORMAL_BACKGROUND "/ru x" NORMAL THIN " or " BLACK_NORMAL_BACKGROUND "/ru" NORMAL THIN " to relpy in unsafe mode" << std::endl
              << std::endl
              << WHITE_NORMAL_COLOR BLACK_NORMAL_BACKGROUND "/p x Message" NORMAL " - To send a private message to the group x (only people that will have a 'x' section in discussionGroupKeys in config.json with the corect Key will be able to decrypt the message)" << std::endl
              << std::endl
              << WHITE_NORMAL_COLOR BLACK_NORMAL_BACKGROUND "/u Message..." NORMAL " - To send an unsafe message (no encryption)" << std::endl

              << std::endl;

    if (moderator)
    {
        createLine();
        std::cout << BOLD UNDERLINED "Here is the list of MODERATORS commands" NORMAL << std::endl
                  << std::endl
                  << WHITE_NORMAL_COLOR BLACK_NORMAL_BACKGROUND "/delmsg x" NORMAL " or " WHITE_NORMAL_COLOR BLACK_NORMAL_BACKGROUND "/d x" NORMAL " - To delete the message with the id x" << std::endl
                  << std::endl;
    }
    std::cout << std::endl
              << "Press " REVERSED BLINK "[ENTER]" NORMAL " to go back to chat" << std::endl;
    std::cin.get();
}

int showReplying(json config, MessageMemory &mem, int id, Message::messageSettings &msgs, Message::isgroupmessage &igm)
{
    Message msg(mem.getMessageByID(id));
    if (msg.getID() < 0)
        return 1;
    igm = msg.isGroupContent(config);
    msg.printReply(msgs, igm);
    if (igm.visible)
        std::cout << THIN "Replying to " << msg.getAuthor() << NORMAL " | " << msgs.pseudo << THIN " -> " NORMAL PURPLE_NORMAL_COLOR "(" << igm.groupname << ")" NORMAL " > ";
    else
        std::cout << THIN "Replying to " << msg.getAuthor() << NORMAL " | " << msgs.pseudo << " > ";
    return 0;
}

int getArguments(MessageMemory &mem,
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
        showHelp(moderatormode);
    }
    else if (moderatormode && (input.rfind("/d", 0) == 0))
    {
        return deleteArg(input, serverurl, username, token, exitSendCode);
    }
    else if (input.rfind("/r", 0) == 0)
    {
        return replyArg(config, mem, input, serverurl, msgsettings, username, token, exitSendCode);
    }
    else if (input.rfind("/u ", 0) == 0)
    {
        return uSendArg(input, serverurl, username, token, exitSendCode);
    }
    else if (input.rfind("/p", 0) == 0)
    {
        return pSendArg(mem, config, input, serverurl, msgsettings, username, token, exitSendCode);
    }
    else if (input.rfind("/g", 0) == 0)
    {
        return groupArg(input, msgsettings, config);
    }
    else
    {
        if (msgsettings.channel != "")
            return pChannelSendArg(mem, config, input, serverurl, msgsettings, username, token, exitSendCode);
        else
            return sendArg(msgsettings, input, serverurl, username, token, exitSendCode);
    }
    return 0;
}