#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <algorithm>
#include "message.h"
#include "tools.h"
#include "aes.h"
#include <map>

/* ESCAPE SEQUENCES */

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

/* TOOLS */

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

void cleanMessageList(std::string &s)
{
    ReplaceStringInPlace(s, "§", " ");
    replaceEscapeSequences(s);
    escapeBackslash(s);
    ReplaceStringInPlace(s, "\\\\n", "\\n");
}

/* MESSAGE CLASS*/
Message::messageSettings::messageSettings() : deletedmsg(true), offlinemsg(true), generalkey("----------------")
{
}

Message::messageSettings::messageSettings(bool ddel, bool doff, std::string gkey) : deletedmsg(ddel), offlinemsg(doff), generalkey(gkey)
{
}

Message::jsonMessage::jsonMessage() : sender(""), content(""), dateTime(""), timestamp(-1), certifiedUser(0), rank(0), decrypted(""), status(UNKNOWN)
{
}

Message::jsonMessage::jsonMessage(Message message) : sender(message.sender), content(message.content), dateTime(message.dateTime), decrypted(message.decrypted), timestamp(message.timestamp), certifiedUser(message.certifiedUser), rank(message.rank), status(message.status)
{
}

Message::jsonMessage::jsonMessage(std::string a, std::string c, std::string d, int cu, int r, time_t t, Message::messageStatus s) : sender(a), content(c), dateTime(d), timestamp(t), certifiedUser(cu), rank(r), status(s)
{
}

Message::Message() : sender(""), content(""), dateTime(""), timestamp(-1), certifiedUser(0), rank(0), decrypted(""), status(UNKNOWN)
{
}

Message::Message(std::string author, std::string message, std::string date, int certified, int rnk, Message::messageStatus msgstatus) : sender(author), content(message),
                                                                                                                                        dateTime(date), timestamp(getTimestamp(date)), decrypted(""),
                                                                                                                                        certifiedUser(certified), rank(rnk), status(msgstatus)
{
}

Message::Message(std::string author, std::string message, std::string date, int certified, int rnk, time_t ts, std::string dc, Message::messageStatus msgstatus) : sender(author), content(message),
                                                                                                                                                                   dateTime(date), timestamp(ts), decrypted(dc),
                                                                                                                                                                   certifiedUser(certified), rank(rnk), status(msgstatus)
{
}

Message::Message(std::string author, std::string message, std::string date, std::string certified, std::string rnk, Message::messageStatus msgstatus) : sender(author), content(message),
                                                                                                                                                        dateTime(date), timestamp(getTimestamp(date)), decrypted(""),
                                                                                                                                                        certifiedUser(std::stoi(certified)), rank(std::stoi(rnk)), status(msgstatus)
{
}

void Message::print(messageSettings const &msettings)
{
    std::string text;
    if (content.rfind("护", 0) == 0)
    {
        if (decrypted == "")
        {
            std::string copyContent = content;
            ReplaceStringInPlace(copyContent, "护", "");
            if (isEncryptedMessage(copyContent))
            {
                std::cout << ON_GREEN << BIWHITE << "S" << RESET;
                unsigned char tdecryptedText[980] = "";
                unsigned char tkey[40] = "";
                unsigned char tencryptedText[980] = "";
                std::copy(copyContent.cbegin(), copyContent.cend(), tencryptedText);
                std::copy(msettings.generalkey.cbegin(), msettings.generalkey.cend(), tkey);
                inv_AES(tencryptedText, tkey, tdecryptedText);
                std::string decryptedContent(reinterpret_cast<char *>(tdecryptedText));
                decrypted = decryptedContent;
                cleanMessageList(decryptedContent);
                text = decrypted;
            }
            else
                text = content;
        }
        else
        {
            std::cout << ON_GREEN << BIWHITE << "S" << RESET;
            text = decrypted;
        }
    }
    else
        text = content;
    if (status == ONLINE)
        std::cout << "[" << BIRED << sender << RESET << "] " << text << std::endl
                  << std::endl;
    else if (status == OFFLINE && msettings.offlinemsg)
        std::cout << "[" << BIRED << sender << RESET << "] " << BIBLACK << "(offline) " << IBLACK << text << RESET << std::endl
                  << std::endl;
    else if (status == DELETED && msettings.deletedmsg)
        std::cout << "[" << BIRED << sender << RESET << "] " << RED << "(deleted) " << RED << text << RESET << std::endl
                  << std::endl;
    else if (status != OFFLINE && status != DELETED)
        std::cout << "[" << BIRED << sender << RESET << "] " << RED << "(unknown status) " << RED << text << RESET << std::endl
                  << std::endl;
}

void Message::setStatus(messageStatus const &newStatus)
{
    status = newStatus;
}

Message::messageStatus Message::getStatus()
{
    return status;
}

time_t Message::getMsgTimestamp()
{
    if (timestamp == -1)
        timestamp = getTimestamp(dateTime);

    return timestamp;
}

bool Message::operator==(Message const &m)
{
    return sender == m.sender &&
           content == m.content &&
           dateTime == m.dateTime &&
           certifiedUser == m.certifiedUser &&
           rank == m.rank;
}

bool Message::operator!=(Message const &m)
{
    return sender != m.sender ||
           content != m.content ||
           dateTime != m.dateTime ||
           certifiedUser != m.certifiedUser ||
           rank != m.rank;
}