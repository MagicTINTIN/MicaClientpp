
#include <string>
#include <iomanip>
#include <iostream>
#include "message.h"
#include "messagememory.h"
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
    std::cout << BOLD UNDERLINED << "Here is the list of commands" << std::endl
              << std::endl
              << WHITE_NORMAL_COLOR << BLACK_NORMAL_BACKGROUND << "/h" << NORMAL << " or " << WHITE_NORMAL_COLOR << BLACK_NORMAL_BACKGROUND << "/help" << NORMAL << " - To show this message" << std::endl
              << WHITE_NORMAL_COLOR << BLACK_NORMAL_BACKGROUND << "/p x Message" << NORMAL << " - To send a private message to the group x (only people that will have a 'x' section in discussionGroupKeys in config.json with the corect Key will be able to decrypt the message)" << std::endl
              << WHITE_NORMAL_COLOR << BLACK_NORMAL_BACKGROUND << "/u Message..." << NORMAL << " - To send an unsafe message (no encryption)" << std::endl
              << std::endl;

    if (moderator)
    {
        createLine();
        std::cout << BOLD UNDERLINED << "Here is the list of MODERATORS commands" << std::endl
                  << std::endl
                  << WHITE_NORMAL_COLOR << BLACK_NORMAL_BACKGROUND << "/delmsg x" << NORMAL << " or " << WHITE_NORMAL_COLOR << BLACK_NORMAL_BACKGROUND << "/d x" << NORMAL << " - To delete the message with the id x" << std::endl
                  << std::endl;
    }
    std::cout << std::endl
              << "Press " << REVERSED BLINK << "[ENTER]" << NORMAL << " to go back to chat" << std::endl;
    std::cin.get();
}

int showReplying(MessageMemory &mem, int id, Message::messageSettings &msgs)
{
    Message msg(mem.getMessageByID(id));
    if (msg.getID() < 0)
        return 1;
    msg.printReply(msgs);
    std::cout << THIN "Replying to " << msg.getAuthor() << NORMAL " | " << msgs.pseudo << " > ";
    return 0;
}