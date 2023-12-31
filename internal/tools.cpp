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
#include <codecvt>
#endif
#include "colors.h"
#include "tools.h"
#include "../includes/nlohmann/json.hpp"

using json = nlohmann::json;

/* ESCAPE SEQUENCES */

bool replaceRegexWishBoundaries(std::string &text, const std::string &word, const std::string &replacement)
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

bool regexWishBoundaries(std::string &text, const std::string &word)
{
    std::string::size_type pos = text.find(word);
    bool found = false;

    while (pos != std::string::npos && !found)
    {
        // Check if the match ends with a word boundary
        if ((pos + word.length() == text.length() || !isalnum(text[pos + word.length()])) &&
            (pos == 0 || (!isalnum(text[pos - 1] && text[pos - 1] != '@'))))
            found = true;
        pos = text.find(word, pos + word.length() - 1);
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

int arguments(std::vector<std::string> &args, std::string &cfgPath, bool &moderator, std::string const &v, bool &showAll)
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
                return -1;
            }
        }
        else if (args[i] == "--version" || args[i] == "-v")
        {
            std::cout << "MicaClient++ Version " << v << std::endl;
            return 1;
        }
        else if (args[i] == "--all")
        {
            showAll = true;
        }
        else if (args[i] == "--help" || args[i] == "-h")
        {
            std::cout << "MicaClient++ is an alternative client for Micasend" << std::endl
                      << " -m, --moderator to start it in moderator mode" << std::endl
                      << " --cfg path/to/a/config/file.json to load a specific config" << std::endl;
            return 1;
        }
    }
    return 0;
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

void replaceStringInPlace(std::string &s, const std::string &search, const std::string &replace)
{
    size_t pos(0);
    while ((pos = s.find(search, pos)) != std::string::npos)
    {
        s.replace(pos, search.length(), replace);
        pos += replace.length();
    }
}

std::string getValFromJSONSource(json &valuesource, std::string k)
{
    if (valuesource.contains(k))
        return valuesource[k].get<std::string>();
    return "";
}

void replacePrefixes(json &valuesource, std::string const prefix, std::string &s)
{
    size_t startPos = 0;
    while ((startPos = s.find(prefix, startPos)) != std::string::npos)
    {
        size_t endPos = startPos + prefix.length();
        while (isalnum(s[endPos]) && endPos < s.length())
            endPos++;

        // if last word in string
        if (endPos == std::string::npos || endPos == s.length())
        {
            endPos = s.length();
        }

        std::string keyToFind = s.substr(startPos + prefix.length(), endPos - (startPos + prefix.length()));
        std::string valueFound = getValFromJSONSource(valuesource, keyToFind);
        replaceRegexWishBoundaries(s, prefix + keyToFind, valueFound);

        startPos += valueFound.length();
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
            if (leadingOnes >= 6)
                return 0;
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
            // if (*value >= 0x80 && *value <= 0xFF && *value != 0xFFFE && *value != 0xFFFF)
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

void charsCleaner(unsigned char *data, int const &sizeofdata, unsigned char *modifiedData)
{
    size_t index = 0;
    uint32_t characterValue;
    size_t dataIndex = 0; // Index for the modified data

    while (index < sizeofdata)
    {
        decodeUTF8Character(data, modifiedData, &index, &dataIndex, &characterValue);
        index++;
    }
    // Null-terminate the modifiedData to make it a valid C string
    modifiedData[dataIndex] = '\0';
}

std::string charsToStringCleaner(unsigned char *data, int const &sizeofdata)
{
    size_t index = 0;
    uint32_t characterValue;
    size_t dataIndex = 0; // Index for the modified data
  
    unsigned char modifiedData[MsgSettings::encryptedmaxsize] = {};

    while (index < sizeofdata && data[index] != (unsigned)'\0')
    {
        decodeUTF8Character(data, modifiedData, &index, &dataIndex, &characterValue);
        index++;
    }
    // Null-terminate the modifiedData to make it a valid C string
    modifiedData[dataIndex] = '\0';

    std::string returnedString = reinterpret_cast<char *>(modifiedData);
    return returnedString;
}

std::string replaceDelimiters(std::string str, std::string const &lowerDelimiter, std::string const &upperDelimiter, std::string const &lowerReplacement, std::string const &upperReplacement) noexcept
{
    size_t firstDelPos = 0, secondDelPos = 0;

    while ((firstDelPos = str.find(lowerDelimiter, firstDelPos)) != std::string::npos)
    {
        secondDelPos = firstDelPos + 1;
        while ((secondDelPos = str.find(upperDelimiter, secondDelPos)) != std::string::npos && (secondDelPos != 0 && str[secondDelPos - 1] == '\\'))
        {
            secondDelPos += upperDelimiter.length();
            if (secondDelPos >= str.length())
                secondDelPos = std::string::npos - 1;
        }

        if (secondDelPos != std::string::npos && (secondDelPos == 0 || str[secondDelPos - 1] != '\\'))
        {
            std::string strBetweenTwoDels = str.substr(firstDelPos + lowerDelimiter.length(), secondDelPos - firstDelPos - lowerDelimiter.length());
            std::string newBetweenTwoDels = lowerReplacement + strBetweenTwoDels + upperReplacement;

            str.replace(firstDelPos, lowerDelimiter.length() + strBetweenTwoDels.length() + upperDelimiter.length(), newBetweenTwoDels);
            firstDelPos += newBetweenTwoDels.length();
        }
        firstDelPos = secondDelPos;
        if (firstDelPos >= str.length())
            firstDelPos = std::string::npos;
    }
    return str;
}

void delimitersOuterRanges(std::string &str, std::string const &lowerDelimiter, std::string const &upperDelimiter, std::vector<std::vector<size_t>> ranges) noexcept
{
    size_t firstDelPos = 0, secondDelPos = 0;

    while ((firstDelPos = str.find(lowerDelimiter, firstDelPos)) != std::string::npos)
    {
        secondDelPos = firstDelPos + 1;
        while ((secondDelPos = str.find(upperDelimiter, secondDelPos)) != std::string::npos && (secondDelPos != 0 && str[secondDelPos - 1] == '\\'))
        {
            secondDelPos += upperDelimiter.length();
            if (secondDelPos >= str.length())
                secondDelPos = std::string::npos - 1;
        }

        if (secondDelPos != std::string::npos && (secondDelPos == 0 || str[secondDelPos - 1] != '\\'))
        {
            //std::cout << "Found ! " << upperDelimiter << std::endl;
            std::string strBetweenTwoDels = str.substr(firstDelPos + lowerDelimiter.length(), secondDelPos - firstDelPos - lowerDelimiter.length());

            ranges.push_back({firstDelPos, lowerDelimiter.length() + strBetweenTwoDels.length() + upperDelimiter.length()});
        }
        firstDelPos = secondDelPos;
        if (firstDelPos >= str.length())
            firstDelPos = std::string::npos;
    }
}

void delimitersRanges(std::string &str, std::string const &lowerDelimiter, std::string const &upperDelimiter, std::vector<std::vector<size_t>> &ranges) noexcept
{
    size_t firstDelPos = 0, secondDelPos = 0;

    //std::cout << firstDelPos << " n1 " << secondDelPos << std::endl;
    while ((firstDelPos = str.find(lowerDelimiter, firstDelPos)) != std::string::npos)
    {
        //std::cout << firstDelPos << " l1 " << secondDelPos << std::endl;
        secondDelPos = firstDelPos + lowerDelimiter.length();
        while ((secondDelPos = str.find(upperDelimiter, secondDelPos)) != std::string::npos && (secondDelPos != 0 && str[secondDelPos - 1] == '\\'))
        {
            secondDelPos += upperDelimiter.length();
            if (secondDelPos >= str.length())
                secondDelPos = std::string::npos;
        }

        if (secondDelPos != std::string::npos && (secondDelPos == 0 || str[secondDelPos - 1] != '\\'))
        {
            std::string strBetweenTwoDels = str.substr(firstDelPos + lowerDelimiter.length(), secondDelPos - firstDelPos - lowerDelimiter.length());

            //std::cout << firstDelPos << " finding : " << strBetweenTwoDels << std::endl;
            ranges.push_back({firstDelPos, secondDelPos}); // lowerDelimiter.length() + strBetweenTwoDels.length()
        }

        if (firstDelPos + upperDelimiter.length() >= str.length()) {
            firstDelPos = std::string::npos;
            secondDelPos = std::string::npos;
        }
        else if (secondDelPos == std::string::npos) {
            firstDelPos = std::string::npos;
        }
        else {
            firstDelPos = secondDelPos + upperDelimiter.length();
        }
    }
}

std::string textFormatter(std::string str) noexcept
{
    //std::cout << "Old string : " << str << std::endl;
    std::string newstr;

    std::string boldDelimiter = "**";
    std::string firstItalicDelimiter = "*";
    std::string secondItalicDelimiter = "_";
    std::string underlinedDelimiter = "__";
    std::string reversedDelimiter = "`";
    std::string strikedDelimiter = "~~";

    // Bold
    bool isBold = false;
    std::vector<std::vector<size_t>> rangesBold;
    int elementBoldNb = 0;
    delimitersRanges(str, boldDelimiter, boldDelimiter, rangesBold);
    //std::cout << "ebnb : " << elementBoldNb << " sizeofrb : " << rangesBold.size() << std::endl;

    // Italic
    bool isItalic = false;
    std::vector<std::vector<size_t>> rangesItalic;
    int elementItalicNb = 0;
    delimitersRanges(str, firstItalicDelimiter, firstItalicDelimiter, rangesItalic);
    delimitersRanges(str, secondItalicDelimiter, secondItalicDelimiter, rangesItalic);

    // Underlined
    bool isUnderlined = false;
    std::vector<std::vector<size_t>> rangesUnderlined;
    int elementUnderlinedNb = 0;
    delimitersRanges(str, underlinedDelimiter, underlinedDelimiter, rangesUnderlined);

    // Reversed
    bool isReversed = false;
    std::vector<std::vector<size_t>> rangesReversed;
    int elementReversedNb = 0;
    delimitersRanges(str, reversedDelimiter, reversedDelimiter, rangesReversed);

    // Striked
    bool isStriked = false;
    std::vector<std::vector<size_t>> rangesStriked;
    int elementStrikedNb = 0;
    delimitersRanges(str, strikedDelimiter, strikedDelimiter, rangesStriked);

    size_t charnb = 0;
    bool actualiseStyle = false;
    size_t delimiterLengthLeft = 0;
    while (charnb < str.length())
    {
        // std::cout << " l3 " << charnb << std::endl;
        // std::cout << "ebnb : " << elementBoldNb << " sizeofrb : " << rangesBold.size() << std::endl;
        if (elementBoldNb < rangesBold.size() && charnb == rangesBold[elementBoldNb][isBold ? 1 : 0])
        {
            isBold = isBold ? false : true;
            if (!isBold)
                elementBoldNb++;
            actualiseStyle = true;
            delimiterLengthLeft = std::max(delimiterLengthLeft, boldDelimiter.length());
        }
        if (elementItalicNb < rangesItalic.size() && charnb == rangesItalic[elementItalicNb][isItalic ? 1 : 0])
        {
            isItalic = isItalic ? false : true;
            if (!isItalic)
                elementItalicNb++;
            actualiseStyle = true;
            delimiterLengthLeft = std::max(delimiterLengthLeft, firstItalicDelimiter.length());
        }
        if (elementUnderlinedNb < rangesUnderlined.size() && charnb == rangesUnderlined[elementUnderlinedNb][isUnderlined ? 1 : 0])
        {
            isUnderlined = isUnderlined ? false : true;
            if (!isUnderlined)
                elementUnderlinedNb++;
            actualiseStyle = true;
            delimiterLengthLeft = std::max(delimiterLengthLeft, underlinedDelimiter.length());
        }
        if (elementReversedNb < rangesReversed.size() && charnb == rangesReversed[elementReversedNb][isReversed ? 1 : 0])
        {
            isReversed = isReversed ? false : true;
            if (!isReversed)
                elementReversedNb++;
            actualiseStyle = true;
            delimiterLengthLeft = std::max(delimiterLengthLeft, reversedDelimiter.length());
        }
        if (elementStrikedNb < rangesStriked.size() && charnb == rangesStriked[elementStrikedNb][isStriked ? 1 : 0])
        {
            isStriked = isStriked ? false : true;
            if (!isStriked)
                elementStrikedNb++;
            actualiseStyle = true;
            delimiterLengthLeft = std::max(delimiterLengthLeft, strikedDelimiter.length());
        }
        //std::cout << "DelimLength " << delimiterLengthLeft << " : " << str.at(charnb) << std::endl;
        if (actualiseStyle)
        {
            actualiseStyle = false;
            newstr += getColor("NORMAL");
            if (isBold)
                newstr += getColor("BOLD");
            if (isItalic)
                newstr += getColor("ITALIC");
            if (isUnderlined)
                newstr += getColor("UNDERLINED");
            if (isReversed)
                newstr += getColor("REVERSED");
            if (isStriked)
                newstr += getColor("STRIKED");
        }
        if (delimiterLengthLeft > 0)
        {
            delimiterLengthLeft--;
        }
        else
            newstr += str[charnb];
        charnb++;
    }

    //std::cout << "New string : " << newstr << std::endl;
    return newstr;
}