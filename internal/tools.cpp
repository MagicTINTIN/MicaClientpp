#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <map>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <sys/ioctl.h>
#endif
#include "tools.h"
#include "colors.h"

/* ESCAPE SEQUENCES */

bool regexWishBoundaries(std::string &text, const std::string &word, const std::string &replacement)
{
    std::string::size_type pos = text.find(word);
    bool found = false;

    while (pos != std::string::npos)
    {
        // Check if the match ends with a word boundary
        if ((pos + word.length() == text.length() || !isalnum(text[pos + word.length()])) &&
            (pos == 0 || (!isalnum(text[pos - 1] && text[pos - 1] != '@'))))
        {
            // Replace the occurrence with the specified replacement
            text.replace(pos, word.length(), replacement);
            found = true;
        }

        // Search for the next occurrence
        pos = text.find(word, pos + replacement.length() - 1);
    }

    return found;
}

std::map<std::string, char> const escapeSequenceMap = {
    {"&#039;", '\''},
    {"&#92;", '\\'},
    {"&quot;", '\"'},
    {"&amp;", '&'},
    {"&apos;", '\''},
    {"&lt;", '<'},
    {"&gt;", '>'},
    {"&nbsp;", ' '}
    // Add more escape sequences as needed
};

bool arguments(std::vector<std::string> &args, std::string &cfgPath, bool &moderator)
{
    for (size_t i = 0; i < args.size(); ++i)
    {
        if (args[i] == "--moderator" || args[i] == "-m")
            moderator = true;
        else if (args[i] == "--cfg")
        {
            if (i + 1 < args.size())
            {
                cfgPath = args[i + 1];
                std::cout << "Loading external config file (" << cfgPath << ")..." << std::endl;
            }
            else
            {
                std::cout << RED_NORMAL_COLOR BOLD << "--cfg needs to be followed by a path to a config file" << NORMAL << std::endl;
                return false;
            }
        }
    }
    return true;
}

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

void oldCreateLine(char c, int size = getWidth())
{
    std::cout << std::string(size, c) << std::endl;
}

std::string createLineString(char c, int size = getWidth())
{
    return std::string(size, c);
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

bool isEncryptedMessage(const std::string &str)
{
    return str.find_first_not_of("0123456789abcdef") == std::string::npos;
}

std::string urlEncode(const std::string &input)
{
    std::ostringstream encoded;
    encoded.fill('0');
    encoded << std::hex;

    for (char c : input)
    {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            encoded << c;
        }
        else if (c == ' ')
        {
            encoded << '+';
        }
        else
        {
            encoded << '%' << std::setw(2) << int(static_cast<unsigned char>(c));
        }
    }

    return encoded.str();
}

time_t getTimestamp(std::string t)
{
    struct tm tm;
    if (strptime(t.c_str(), "%Y-%m-%d %H:%M:%S", &tm) != NULL)
        return mktime(&tm);
    else
        return -1;
}

void ReplaceStringInPlace(std::string &s, const std::string &search, const std::string &replace)
{
    size_t pos(0);
    while ((pos = s.find(search, pos)) != std::string::npos)
    {
        s.replace(pos, search.length(), replace);
        pos += replace.length();
    }
}

void escapeBackslash(std::string &s)
{
    auto it = std::find(s.begin(), s.end(), '\\');
    while (it != s.end())
    {
        auto it2 = s.insert(it, '\\');

        // skip over the slashes we just inserted
        it = std::find(it2 + 2, s.end(), '\\');
    }
}

char getCharacterFromEscapeSequence(const std::string &escapeSequence)
{
    auto it = escapeSequenceMap.find(escapeSequence);
    if (it != escapeSequenceMap.end())
    {
        return it->second;
    }
    else
    {
        // Return a default character (you can modify this as needed)
        return '?'; // For example, return '?' for unknown escape sequences
    }
}

void replaceEscapeSequences(std::string &s)
{
    size_t found = s.find("&");
    while (found != std::string::npos)
    {
        size_t end = s.find(";", found);
        if (end != std::string::npos && end - found < 6)
        {
            std::string escapeSequence = s.substr(found, end - found + 1);
            char character = getCharacterFromEscapeSequence(escapeSequence);
            s.replace(found, end - found + 1, 1, character);
        }
        found = s.find("&", found + 1);
    }
}
