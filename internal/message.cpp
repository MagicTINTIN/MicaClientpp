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
#include "themes.h"

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
    replaceStringInPlace(s, "§", " ");
    replaceEscapeSequences(s);
    escapeBackslash(s);
    replaceStringInPlace(s, "\\\\n", "\\n");
    replaceStringInPlace(s, "\\\\\"", "\\\"");
}

std::string messageDisplayImprove(std::string s, bool r = false)
{
    if (r)
    {
        replaceStringInPlace(s, "\\n", " ");
        replaceStringInPlace(s, "\\t", " ");
    }
    else
    {
        replaceStringInPlace(s, "\\n", "\n");
        replaceStringInPlace(s, "\\t", "\t");
    }
    return s;
}

/* MESSAGE SETTINGS */
Message::messageSettings::messageSettings() : generalkey(KEYNOTFOUND), pseudo("NOBODY"), modmsg(false), securemsg(true), blockUnverified(false), channel(""), msgmaxsize(0), encryptedmaxsize(0)
{
}

Message::messageSettings::messageSettings(std::string gkey, std::string psd, bool mod, bool sec, bool buu, json blu, std::string ch, int maxsmsg) : generalkey(gkey), pseudo(psd), modmsg(mod), securemsg(sec), blockUnverified(buu), channel(ch), msgmaxsize(maxsmsg), encryptedmaxsize(maxsmsg * 2)
{
    for (auto &u : blu.items())
    {
        blockedUsers.push_back(u.value().get<std::string>());
    }
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
        {"decrypted", stringCleaner(decrypted)},
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

void Message::print(json &lang, json &theme, messageSettings const &msettings, bool const &showids, isgroupmessage const &igm, int idReplied, Message repliedTo, bool const &isReply, std::string newMessageContent)
{

    if ((igm.isgroup && !igm.visible) ||
        (msettings.blockUnverified && rank < 1) ||
        (std::find(msettings.blockedUsers.begin(), msettings.blockedUsers.end(), sender) != msettings.blockedUsers.end()) ||
        (msettings.channel != "" && (!igm.visible || igm.groupname != msettings.channel)))
        return;
    std::string text;
    std::string copyContent = content;

    if (igm.isgroup)
        copyContent = igm.messagecontent;

    // IS REPLY ?
    std::string replyContent, rauthor, rid;
    if (isReply)
    {
        if (repliedTo.getID() == -1)
        {
            replyContent = lang["errors"]["unknownmsg"].get<std::string>() + std::to_string(idReplied);
            rauthor = lang["errors"]["unknownauthor"].get<std::string>();
            rid = "X";
        }
        else
        {
            replyContent = repliedTo.getReplyContent(msettings, igm);
            rauthor = repliedTo.getAuthor();
            rid = repliedTo.getID();
        }
        copyContent = newMessageContent;
    }
    else
    {
        replyContent = "";
        rauthor = "";
        rid = "X";
    }

    // DECRYPTION MESSAGE
    bool isSecured = false;
    if (copyContent.rfind("护", 0) == 0 && msettings.securemsg)
    {
        if (decrypted == "")
        {
            replaceStringInPlace(copyContent, "护", "");
            if (isEncryptedMessage(copyContent))
            {
                unsigned char tdecryptedText[msettings.encryptedmaxsize] = "";
                unsigned char tkey[40] = "";
                unsigned char tencryptedText[msettings.encryptedmaxsize] = "";
                std::copy(copyContent.cbegin(), copyContent.cend(), tencryptedText);
                if (igm.isgroup)
                    std::copy(igm.key.cbegin(), igm.key.cend(), tkey);
                else
                    std::copy(msettings.generalkey.cbegin(), msettings.generalkey.cend(), tkey);
                std::string decryptedContent;
                if (inv_AES(tencryptedText, tkey, tdecryptedText))
                {
                    isSecured = false;
                    decryptedContent = copyContent;
                }
                else
                {
                    decryptedContent = charsToStringCleaner(tdecryptedText, msettings.encryptedmaxsize);
                    isSecured = true;
                }
                decrypted = stringCleaner(decryptedContent);
                cleanMessageList(decrypted);
                text = decrypted;
            }
            else
            {
                isSecured = false;
                text = stringCleaner(copyContent);
            }
        }
        else
        {
            isSecured = true;
            text = decrypted;
        }
    }
    else
    {
        isSecured = false;
        text = stringCleaner(content);
    }

    // USER RANK AND NAME
    bool isVerified = false;
    bool adminRank = false;
    bool modRank = false;
    bool botRank = false;
    bool certifiedRank = false;
    if (rank >= 15) {
        isVerified = true;
        adminRank = true;
    }
    else if (rank >= 12)
    {
        isVerified = true;
        modRank = true;
    }
    else if (rank >= 11)
    {
        isVerified = true;
        botRank = true;
    }
    else if (rank >= 1)
    {
        isVerified = true;
        certifiedRank = true;
    }

    // MESSAGE CONTENT
    std::string mention("@" + msettings.pseudo);

    bool isMention = (regexWishBoundaries(text, mention) || (repliedTo.getID() != -1 && repliedTo.getAuthor() == msettings.pseudo));

    std::string msgcontent = messageDisplayImprove(text);
    
    themeVariables tv = themeVariables(msettings.pseudo, rauthor, replyContent, rid, std::to_string(id), std::to_string(certifiedUser), dateTime, sender, igm.groupname, msgcontent,
    status == DELETED, status == OFFLINE, (status != OFFLINE && status != ONLINE && status != DELETED), isReply, showids, isSecured, isVerified, certifiedRank, botRank, modRank, adminRank, igm.isgroup, isMention, sender == msettings.pseudo, msettings.modmsg);
    themeProcessLocation(lang, theme, "message", tv);
}

std::string Message::getReplyContent(messageSettings const &msettings, isgroupmessage const &igm)
{
    std::string text;
    std::string copyContent = content;
    // DECRYPTION MESSAGE

    if (igm.isgroup)
        copyContent = igm.messagecontent;
    isreplymessage irm = isRelpyContent(igm.isgroup, copyContent);
    if (irm.isreply)
    {
        copyContent = irm.messagecontent;
    }

    if (copyContent.rfind("护", 0) == 0)
    {
        if (decrypted == "")
        {
            replaceStringInPlace(copyContent, "护", "");
            if (isEncryptedMessage(copyContent))
            {
                unsigned char tdecryptedText[msettings.encryptedmaxsize] = "";
                unsigned char tkey[40] = "";
                unsigned char tencryptedText[msettings.encryptedmaxsize] = "";
                std::copy(copyContent.cbegin(), copyContent.cend(), tencryptedText);
                if (igm.isgroup)
                    std::copy(igm.key.cbegin(), igm.key.cend(), tkey);
                else
                    std::copy(msettings.generalkey.cbegin(), msettings.generalkey.cend(), tkey);
                std::string decryptedContent;
                if (inv_AES(tencryptedText, tkey, tdecryptedText))
                    decryptedContent = copyContent;
                else
                    decryptedContent = charsToStringCleaner(tdecryptedText, msettings.encryptedmaxsize);
                decrypted = stringCleaner(decryptedContent);
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

    return messageDisplayImprove(text, true);
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

/* GROUP CLASS */

Message::isgroupmessage::isgroupmessage() : isgroup(false), messagecontent(""), groupname(""), visible(false), key(KEYNOTFOUND)
{
}
Message::isgroupmessage::isgroupmessage(bool is, std::string c, std::string n, bool v, std::string k) : isgroup(is), messagecontent(c), groupname(n), visible(v), key(k)
{
}

Message::isgroupmessage Message::isGroupContent(json config)
{
    std::string copyContent = content;

    if (copyContent.rfind("团", 0) == 0)
    {
        replaceStringInPlace(copyContent, "团", "");
        if (copyContent.find("答") != std::string::npos)
        {
            std::vector<std::string> msgParts = split(copyContent, "答");
            privategroup pg = findPrivateGroup(config, msgParts[0], true, getAuthor());
            return isgroupmessage(true, "答" + msgParts[1], msgParts[0], pg.isin, pg.key);
        }
        else if (copyContent.find("护") != std::string::npos)
        {
            std::vector<std::string> msgParts = split(copyContent, "护");
            privategroup pg = findPrivateGroup(config, msgParts[0], true, getAuthor());
            return isgroupmessage(true, "护" + msgParts[1], msgParts[0], pg.isin, pg.key);
        }
        else
            return isgroupmessage(false, content, "", false, KEYNOTFOUND);
    }
    else
        return isgroupmessage(false, content, "", false, KEYNOTFOUND);
}
/* REPLY CLASS */

Message::isreplymessage::isreplymessage() : isreply(false), messagecontent(""), idreply(-1)
{
}
Message::isreplymessage::isreplymessage(bool is, std::string c, int id) : isreply(is), messagecontent(c), idreply(id)
{
}

Message::isreplymessage Message::isRelpyContent(bool fromGroup, std::string groupContent)
{
    std::string copyContent;
    if (fromGroup)
        copyContent = groupContent;
    else
        copyContent = content;
    if (copyContent.rfind("答", 0) == 0)
    {
        replaceStringInPlace(copyContent, "答", "");
        if (copyContent.find("护") != std::string::npos)
        {
            std::vector<std::string> msgParts = split(copyContent, "护");
            return isreplymessage(true, "护" + msgParts[1], stoi(msgParts[0]));
        }
        else
            return isreplymessage(false, copyContent, -1);
    }
    else
        return isreplymessage(false, copyContent, -1);
}