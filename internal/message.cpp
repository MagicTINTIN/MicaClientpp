#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <algorithm>
#include "message.h"
#include "tools.h"
#include "colors.h"
#include "aes.h"
#include "../includes/nlohmann/json.hpp"
using json = nlohmann::json;

/* TOOLS */


void cleanMessageList(std::string &s)
{
    ReplaceStringInPlace(s, "§", " ");
    replaceEscapeSequences(s);
    escapeBackslash(s);
    ReplaceStringInPlace(s, "\\\\n", "\\n");
    ReplaceStringInPlace(s, "\\\\\"", "\\\"");
}

/* MESSAGE SETTINGS */
Message::messageSettings::messageSettings() : deletedmsg(true), offlinemsg(true), generalkey("----------------"), datetimemsg(true), pseudo("NOBODY")
{
}

Message::messageSettings::messageSettings(bool ddel, bool doff, std::string gkey, bool dt, std::string psd) : deletedmsg(ddel), offlinemsg(doff), generalkey(gkey), datetimemsg(dt), pseudo(psd)
{
}

/* JSON MESSAGE */
Message::jsonMessage::jsonMessage() : id(-1), sender(""), content(""), dateTime(""), timestamp(-1), certifiedUser(0), rank(0), decrypted(""), status(UNKNOWN)
{
}

Message::jsonMessage::jsonMessage(Message message) : id(message.id), sender(message.sender), content(message.content), dateTime(message.dateTime), decrypted(message.decrypted), timestamp(message.timestamp), certifiedUser(message.certifiedUser), rank(message.rank), status(message.status)
{
}

Message::jsonMessage::jsonMessage(int i, std::string a, std::string c, std::string d, int cu, int r, time_t t, std::string dc, Message::messageStatus s) : id(i), sender(a), content(c), dateTime(d), timestamp(t), certifiedUser(cu), rank(r), decrypted(dc), status(s)
{
}

json Message::jsonMessage::toJson()
{
    return json{
        {"sender", sender},
        {"content", content},
        {"dateTime", dateTime},
        {"certifiedUser", certifiedUser},
        {"rank", rank},
        {"timestamp", timestamp},
        {"decrypted", decrypted},
        {"status", status},
    };
}

/* MESSAGE CLASS */

Message::Message() : id(-1), sender(""), content(""), dateTime(""), timestamp(-1), certifiedUser(0), rank(0), decrypted(""), status(UNKNOWN)
{
}

Message::Message(int i, std::string author, std::string message, std::string date, int certified, int rnk, Message::messageStatus msgstatus) : id(i), sender(author), content(message),
                                                                                                                                               dateTime(date), timestamp(getTimestamp(date)), decrypted(""),
                                                                                                                                               certifiedUser(certified), rank(rnk), status(msgstatus)
{
}

Message::Message(int i, std::string author, std::string message, std::string date, int certified, int rnk, time_t ts, std::string dc, Message::messageStatus msgstatus) : id(i), sender(author), content(message),
                                                                                                                                                                          dateTime(date), timestamp(ts), decrypted(dc),
                                                                                                                                                                          certifiedUser(certified), rank(rnk), status(msgstatus)
{
}

Message::Message(std::string i, std::string author, std::string message, std::string date, std::string certified, std::string rnk, Message::messageStatus msgstatus) : id(std::stoi(i)), sender(author), content(message),
                                                                                                                                                                       dateTime(date), timestamp(getTimestamp(date)), decrypted(""),
                                                                                                                                                                       certifiedUser(std::stoi(certified)), rank(std::stoi(rnk)), status(msgstatus)
{
}

void Message::print(messageSettings const &msettings, bool const &showids)
{
    std::string text;
    // SHOW IDS ?

    if (showids)
        std::cout << BOLD << "[" << id << "] -> " << NORMAL;
    else if (msettings.datetimemsg)
        std::cout << dateTime << " ";

    // DECRYPTION MESSAGE

    if (content.rfind("护", 0) == 0)
    {
        if (decrypted == "")
        {
            std::string copyContent = content;
            ReplaceStringInPlace(copyContent, "护", "");
            if (isEncryptedMessage(copyContent))
            {
                std::cout << GREEN_NORMAL_BACKGROUND << BOLD << WHITE_NORMAL_COLOR << "S" << NORMAL;
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
            {
                std::cout << " " << NORMAL;
                text = content;
            }
        }
        else
        {
            std::cout << GREEN_NORMAL_BACKGROUND << BOLD << WHITE_NORMAL_COLOR << "S" << NORMAL;
            text = decrypted;
        }
    }
    else
    {
        text = content;
        std::cout << " " << NORMAL;
    }

    // USER RANK AND NAME

    if (rank >= 15)
        std::cout << WHITE_NORMAL_COLOR << RED_NORMAL_BACKGROUND << "A" << NORMAL << "[" << BOLD << RED_NORMAL_COLOR << sender << NORMAL << "] ";
    else if (rank >= 12)
        std::cout << WHITE_NORMAL_COLOR << BLUE_NORMAL_BACKGROUND << "M" << NORMAL << "[" << BOLD << BLUE_NORMAL_COLOR << sender << NORMAL << "] ";
    else if (rank >= 11)
        std::cout << WHITE_NORMAL_COLOR << CYAN_NORMAL_BACKGROUND << "B" << NORMAL << "[" << BOLD << CYAN_NORMAL_COLOR << sender << NORMAL << "] ";
    else if (rank >= 1)
        std::cout << WHITE_NORMAL_COLOR << GREEN_NORMAL_BACKGROUND << "V" << NORMAL << "[" << BOLD << GREEN_NORMAL_COLOR << sender << NORMAL << "] ";
    else
        std::cout << " " << NORMAL << "[" << BOLD << WHITE_NORMAL_COLOR << sender << NORMAL << "] ";

    // MESSAGE CONTENT
    std::string mention("@" + msettings.pseudo + " ");
    std::string coloredMention(CYAN_DESAT_BACKGROUND BOLD BLACK_NORMAL_COLOR "@" + msettings.pseudo + NORMAL BLACK_NORMAL_COLOR YELLOW_DESAT_BACKGROUND " ");
    if (text.find(mention) != std::string::npos)
    {
        ReplaceStringInPlace(text, mention, coloredMention);
        std::cout << BLACK_NORMAL_COLOR YELLOW_DESAT_BACKGROUND;
    }
    if (status == ONLINE)
        std::cout << text << NORMAL << std::endl
                  << std::endl;
    else if (status == OFFLINE && msettings.offlinemsg)
        std::cout << BOLD BLACK_DESAT_COLOR << "(offline) " << THIN << text << NORMAL << std::endl
                  << std::endl;
    else if (status == DELETED && msettings.deletedmsg)
        std::cout << BOLD RED_DESAT_COLOR << "(deleted) " << THIN << text << NORMAL << std::endl
                  << std::endl;
    else if (status != OFFLINE && status != DELETED)
        std::cout << BLACK_DESAT_COLOR << "(unknown status) " << THIN << text << NORMAL << std::endl
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
    return id == m.id &&
           sender == m.sender &&
           content == m.content &&
           dateTime == m.dateTime &&
           certifiedUser == m.certifiedUser &&
           rank == m.rank;
}

bool Message::operator!=(Message const &m)
{
    return id != m.id ||
           sender != m.sender ||
           content != m.content ||
           dateTime != m.dateTime ||
           certifiedUser != m.certifiedUser ||
           rank != m.rank;
}