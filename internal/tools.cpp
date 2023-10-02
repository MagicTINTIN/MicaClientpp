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
#include "colors.h"
#include "tools.h"
#include "../includes/nlohmann/json.hpp"

using json = nlohmann::json;

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

bool isEncryptedMessage(const std::string &str)
{
    return str.find_first_not_of("0123456789abcdef") == std::string::npos;
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

std::string createLineString(char c, int size)
{
    return std::string(size, c);
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

std::string jsonBackslash(std::string s)
{
    auto it = std::find(s.begin(), s.end(), '\\');
    while (it != s.end())
    {
        auto it2 = s.insert(it, '\\');

        // skip over the slashes we just inserted
        it = std::find(it2 + 2, s.end(), '\\');
    }
    return s;
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

std::vector<std::string> split(std::string s, std::string delimiter)
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
    {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

privategroup::privategroup() : found(false), name(""), key(KEYNOTFOUND), isin(false)
{
}
privategroup::privategroup(bool f, std::string n, std::string k, std::vector<std::string> v, bool i) : found(f), name(n), key(k), isin(i)
{
}

privategroup findPrivateGroup(json config, std::string name, bool isUserIn, std::string pseudo)
{
    std::string key;
    std::vector<std::string> users;
    bool foundUser(pseudo == config["username"].get<std::string>()), foundBlocked(false);
    try
    {
        if (config["discussionGroupKeys"].contains(name) &&
            config["discussionGroupKeys"][name].contains("key") &&
            config["discussionGroupKeys"][name].contains("users") &&
            config["discussionGroupKeys"][name].contains("blocked"))
        {
            key = config["discussionGroupKeys"][name]["key"].get<std::string>();
            for (auto &u : config["discussionGroupKeys"][name]["users"].items())
            {
                std::string tmppseudo = u.value().get<std::string>();
                foundUser = (foundUser || tmppseudo == pseudo || tmppseudo == "*");
                users.push_back(tmppseudo);
            }
            for (auto &u : config["discussionGroupKeys"][name]["blocked"].items())
            {
                std::string tmppseudo = u.value().get<std::string>();
                foundBlocked = (foundBlocked || tmppseudo == pseudo);
            }
        }
        else
            return privategroup();
    }
    catch (std::out_of_range &e)
    {
        return privategroup();
    }
    return privategroup(true, name, key, users, (!isUserIn || (foundUser && !foundBlocked)));
}

std::string escapeBourrinJson(const std::string &input)
{
    std::string soclean;
    for (unsigned char c : input)
    {
        if ((c >= 0x20 && c <= 0x7E) || (c >= 0xC0 && c <= 0xF0))
        {
            soclean += c;
        }
    }
    return soclean;
}

std::string escapeJson(const std::string &input)
{
    std::string escaped;
    for (unsigned char c : input)
    {
        switch (c)
        {
        case '\n':
            escaped += "\\n";
            break;
        case '\r':
            escaped += "\\r";
            break;
        case '\t':
            escaped += "\\t";
            break;
        // case '\\':
        //     escaped += "\\\\";
        //     break;
        default:
            if (c >= 0 && c <= 0x1F)
            {
                // Escape other control characters as Unicode escape sequences
                char buf[7];
                snprintf(buf, sizeof(buf), "\\u%04X", static_cast<unsigned char>(c));
                escaped += buf;
            }
            else
            {
                escaped += c;
            }
            break;
        }
    }
    return escapeBourrinJson(escaped);
}

int isUTF8ContinuationByte(unsigned char c)
{
    // In UTF-8, continuation bytes have the form 10xxxxxx
    return (c & 0xC0) == 0x80;
}

int decodeUTF8Character(unsigned char *data, unsigned char *out, size_t *index, size_t *outIndex, uint32_t *value)
{
    unsigned char tmp[6];
    // Check if the byte at the given index is a start byte for a multibyte character
    if ((data[*index] & 0xC0) == 0xC0)
    {
        // Count the number of leading '1' bits to determine the character length
        int leadingOnes = 0;
        unsigned char mask = 0x80;

        // set a temporary variable to add it to out if the char is a valid UTF-8 char
        tmp[0] = data[*index];

        while ((data[*index] & mask) == mask)
        {
            leadingOnes++;
            mask >>= 1;
        }
        // In UTF-8, multibyte characters have the form 110xxxxx, 1110xxxx, 11110xxx, etc.
        if (leadingOnes > 1)
        {
            // Initialize the character value
            *value = data[*index] & (0xFF >> (leadingOnes + 1));
            // Validate and decode the remaining bytes
            for (int i = 1; i < leadingOnes; i++)
            {
                (*index)++;
                tmp[i] = data[*index];
                if (isUTF8ContinuationByte(data[*index]))
                {
                    *value <<= 6;
                    *value |= (data[*index] & 0x3F);
                }
                else
                {
                    return 0;
                }
            }
            if (*value >= 0x80 && *value <= 0xD7FF && *value != 0xFFFE && *value != 0xFFFF)
            //if (*value >= 0x80 && *value <= 0xFF && *value != 0xFFFE && *value != 0xFFFF)
            {
                for (size_t i = 0; i < leadingOnes; i++)
                {
                    out[(*outIndex)++] = tmp[i];
                }
                return 1;
            }
            else
                return 0;
        }
    }
    // simple ascii
    else if (data[*index] >= 0x20 && data[*index] <= 0x7E)
    {
        out[(*outIndex)++] = data[*index];
        return 1;
    }
    // Not a valid character at all
    return 0;
}

std::string stringCleaner(std::string toclean)
{
    unsigned char data[toclean.size() + 1];

    std::copy(toclean.cbegin(), toclean.cend(), data);
    size_t index = 0;
    uint32_t characterValue;
    size_t dataIndex = 0; // Index for the modified data

    // Create a buffer to store the modified data without invalid UTF-8 characters
    unsigned char modifiedData[toclean.size()];

    while (index < toclean.size())
    {
        decodeUTF8Character(data, modifiedData, &index, &dataIndex, &characterValue);
        index++;
    }

    // Null-terminate the modifiedData to make it a valid C string
    modifiedData[dataIndex] = '\0';
    return reinterpret_cast<char *>(modifiedData);
}

void charsCleaner(unsigned char *data, unsigned char *modifiedData)
{
    size_t index = 0;
    uint32_t characterValue;
    size_t dataIndex = 0; // Index for the modified data

    while (index < sizeof(*data))
    {
        decodeUTF8Character(data, modifiedData, &index, &dataIndex, &characterValue);
        index++;
    }
    // Null-terminate the modifiedData to make it a valid C string
    modifiedData[dataIndex] = '\0';
}

std::string charsToStringCleaner(unsigned char *data)
{
    size_t index = 0;
    uint32_t characterValue;
    size_t dataIndex = 0; // Index for the modified data
    unsigned char modifiedData[sizeof(*data)] = "";

    while (index < sizeof(*data))
    {
        decodeUTF8Character(data, modifiedData, &index, &dataIndex, &characterValue);
        index++;
    }
    // Null-terminate the modifiedData to make it a valid C string
    modifiedData[dataIndex] = '\0';

    return reinterpret_cast<char *>(modifiedData);
}