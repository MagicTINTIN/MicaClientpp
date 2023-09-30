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

std::vector<int> split(const std::string &s, char delim)
{
    std::vector<int> elems;
    std::stringstream ss(s);
    std::string number;
    while (std::getline(ss, number, delim))
    {
        elems.push_back(std::stoi(number));
    }
    return elems;
}

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
Message::messageSettings::messageSettings() : deletedmsg(true), offlinemsg(true), generalkey("----------------"), datetimemsg(true), pseudo("NOBODY"), modmsg(false)
{
}

Message::messageSettings::messageSettings(bool ddel, bool doff, std::string gkey, bool dt, std::string psd, bool mod) : deletedmsg(ddel), offlinemsg(doff), generalkey(gkey), datetimemsg(dt), pseudo(psd), modmsg(mod)
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
        {"id", id},
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

void Message::print(messageSettings const &msettings, bool const &showids, int idReplied, Message repliedTo, bool const &isReply, std::string replyContent)
{
    std::string text;
    std::string copyContent = content;
    // IS REPLY ?

    if (isReply)
    {
        if (repliedTo.getID() == -1)
            std::cout << "> " << NORMAL BLACK_NORMAL_BACKGROUND BLACK_DESAT_COLOR << "Uknown message n°" << idReplied << NORMAL << std::endl;
        else
            repliedTo.printReply(msettings);
        copyContent = replyContent;
    }

    // SHOW IDS ?

    if (msettings.modmsg && !showids)
        std::cout << THIN << "[" << id << "] " << NORMAL;
    if (showids)
        std::cout << BOLD BLINK YELLOW_NORMAL_COLOR << "[" << id << "] -> " << NORMAL;
    else if (msettings.datetimemsg)
        std::cout << dateTime << " ";

    // DECRYPTION MESSAGE

    if (copyContent.rfind("护", 0) == 0)
    {
        if (decrypted == "")
        {
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
                text = copyContent;
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
    std::string mention("@" + msettings.pseudo);
    std::string coloredMention(CYAN_DESAT_BACKGROUND BOLD BLACK_NORMAL_COLOR "@" + msettings.pseudo + NORMAL BLACK_NORMAL_COLOR YELLOW_DESAT_BACKGROUND);
    if (regexWishBoundaries(text, mention, coloredMention) || (repliedTo.getID() != -1 && repliedTo.getAuthor() == msettings.pseudo))
    {
        std::cout << BLACK_NORMAL_COLOR YELLOW_DESAT_BACKGROUND;
    }
    if (status == ONLINE)
        std::cout << text << NORMAL << std::endl
                  << std::endl;
    else if (status == OFFLINE && msettings.offlinemsg)
        std::cout << BOLD BLACK_DESAT_COLOR << "(offline) " << NORMAL BLACK_DESAT_COLOR THIN << text << NORMAL << std::endl
                  << std::endl;
    else if (status == DELETED && msettings.deletedmsg)
        std::cout << BOLD RED_DESAT_COLOR << "(deleted) " << NORMAL RED_DESAT_COLOR THIN << text << NORMAL << std::endl
                  << std::endl;
    else if (status != OFFLINE && status != DELETED)
        std::cout << BLACK_DESAT_COLOR << "(unknown status) " << THIN << text << NORMAL << std::endl
                  << std::endl;
}

void Message::printReply(messageSettings const &msettings)
{
    std::string text;
    std::string copyContent = content;
    // DECRYPTION MESSAGE
    isreplymessage irm = isRelpyContent();
    if (irm.isreply)
    {
        copyContent = irm.messagecontent;
    }
    
    if (copyContent.rfind("护", 0) == 0)
    {
        if (decrypted == "")
        {
            ReplaceStringInPlace(copyContent, "护", "");
            if (isEncryptedMessage(copyContent))
            {
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
                text = copyContent;
        }
        else
            text = decrypted;
    }
    else
        text = content;

    std::cout << "> " << BLACK_NORMAL_BACKGROUND BLACK_DESAT_COLOR << "[" << BOLD << sender << NORMAL BLACK_NORMAL_BACKGROUND BLACK_DESAT_COLOR << "]" << text << NORMAL << std::endl;
}

void Message::setStatus(messageStatus const &newStatus)
{
    status = newStatus;
}

Message::messageStatus Message::getStatus()
{
    return status;
}

std::string Message::getAuthor()
{
    return sender;
}

std::string Message::getContent()
{
    return content;
}

time_t Message::getMsgTimestamp()
{
    if (timestamp == -1)
        timestamp = getTimestamp(dateTime);

    return timestamp;
}

int Message::getID()
{
    return id;
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

/* REPLY CLASS */
Message::isreplymessage::isreplymessage() : isreply(false), messagecontent(""), idreply(-1)
{
}
Message::isreplymessage::isreplymessage(bool is, std::string c, int id) : isreply(is), messagecontent(c), idreply(id)
{
}

Message::isreplymessage Message::isRelpyContent()
{
    std::string copyContent = content;
    if (content.rfind("答", 0) == 0)
    {
        ReplaceStringInPlace(copyContent, "答", "");
        if (copyContent.find("护") != std::string::npos)
        {
            std::vector<std::string> msgParts = split(copyContent, "护");
            return isreplymessage(true, "护" + msgParts[1], stoi(msgParts[0]));
        }
        else
            return isreplymessage(false, content, -1);
    }
    else
        return isreplymessage(false, content, -1);
}