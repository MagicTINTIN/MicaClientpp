#pragma once
#ifndef THEMES_H_INCLUDED
#define THEMES_H_INCLUDED

#include <string>
#include "../includes/nlohmann/json.hpp"
using json = nlohmann::json;

struct themeVariables
{
    bool isSendReply;
    bool isInGroup;
    bool isSendGroup;
    bool isDeleted;
    bool isOffline;
    bool isUnkonwnStatus;
    bool isReply;
    bool askingReply;
    bool isEncrypted;
    bool isVerified;
    bool certifiedRank;
    bool botRank;
    bool moderatorRank;
    bool adminRank;
    bool isGroupMessage;
    bool isMention;

    std::string username;
    std::string mention;
    std::string rAuthor;
    std::string reply;
    std::string inGroup;
    std::string toGroup;
    std::string msgID;
    std::string datetime;
    std::string mAuthor;
    std::string groupMsg;
    std::string messageContent;
};

std::string themeProcessStringVar(json &lang, std::string s, themeVariables &tv, json &mentionstyle, json &postmentionstyle);

bool themeProcessBoolVar(std::string s, themeVariables &tv, json &themesettings);

void themeProcessSequence(json &lang, json &themeseq, themeVariables &tv, json &themesettings, json &mentionstyle);

void themeProcessLocation(json &lang, json &theme, std::string &location, themeVariables &tv);

std::string printStyle(json &style);

void userError(json &theme, std::string err);

#endif // THEMES_H_INCLUDED