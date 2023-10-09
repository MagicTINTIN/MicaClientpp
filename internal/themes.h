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
    bool isYourMessage;
    bool isModeratorMode;

    std::string username;
    std::string mention;
    std::string rAuthor;
    std::string reply;
    std::string rID;
    std::string inGroup;
    std::string toGroup;
    std::string msgID;
    std::string IDMAuthor;
    std::string datetime;
    std::string mAuthor;
    std::string groupMsg;
    std::string messageContent;

    themeVariables();
    themeVariables(bool isr, bool iig, bool isg, bool imm,
                   std::string u, std::string ra, std::string r,
                   std::string ig, std::string tg, std::string idr);
    themeVariables(std::string u, std::string ra, std::string r,
                   std::string idr, std::string mid, std::string idma, std::string dt,
                   std::string ma, std::string gm, std::string mc,
                   bool idel, bool ioff, bool ius, bool ir,
                   bool askr, bool iencr, bool iv, bool cr, bool br,
                   bool mr, bool ar, bool igm, bool im, bool iym, bool imm
                   );
};

std::string themeProcessStringVar(json &lang, std::string s, themeVariables &tv, json &mentionstyle, json &postmentionstyle, json &themesettings);

bool themeProcessBoolVar(std::string s, themeVariables &tv, json &themesettings);

int themeProcessSequence(json &lang, json &themeseq, themeVariables &tv, json &themesettings, json &mentionstyle, std::string &str);

int themeProcessLocation(json &lang, json &theme, std::string const &location, themeVariables &tv, std::string &str);

void themeProcessPrint(json &lang, json &theme, std::string const &location, themeVariables &tv);

std::string printStyle(json &style);

void userError(json &theme, std::string err);

#endif // THEMES_H_INCLUDED