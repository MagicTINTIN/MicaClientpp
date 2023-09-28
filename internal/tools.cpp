#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#ifdef _WIN32
    #include <windows.h>
#elif defined(__linux__)
    #include <unistd.h>
    #include <sys/ioctl.h>
#endif
#include "tools.h"

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

int getWidth()
{
    int width(30);
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
#elif defined(__linux__)
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    width = w.ws_col;
#endif
    return width;
}

void createLine(char c, int size = getWidth())
{
    std::cout << std::string(size, c) << std::endl;
}

void showHelp()
{
    clearScreen();
    createLine('#');
    std::cout << "Here is the list of commands" << std::endl
              << WHITE << ON_IBLACK << "/h" << RESET << " or " << WHITE << ON_IBLACK << "/help" << RESET << " - To show this message" << std::endl
              << WHITE << ON_IBLACK << "/p x Message" << RESET << " - To send a private message to the group x (only people that will have a 'x' section in discussionGroupKeys in config.json with the corect Key will be able to decrypt the message)" << std::endl
              << WHITE << ON_IBLACK << "/u Message..." << RESET << " - To send an unsafe message (no encryption)" << std::endl
              << std::endl
              << "Press [ENTER] to go back to chat" << std::endl;
    std::cin.get();
}

bool isEncryptedMessage(const std::string &str)
{
    return str.find_first_not_of("0123456789abcdef") == std::string::npos;
}