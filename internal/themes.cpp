#include <string>
#include <iomanip>
#include <iostream>
#include "colors.h"
#include "tools.h"
#include "../includes/nlohmann/json.hpp"
#include "themes.h"
using json = nlohmann::json;

std::string themeProcessStringVar(json &lang, std::string s, themeVariables &tv, json &mentionstyle, json &postmentionstyle)
{
    std::string mention = printStyle(mentionstyle) + tv.mention + printStyle(postmentionstyle);
    replaceStringInPlace(s, "$USERNAME", tv.username);
    replaceStringInPlace(s, "$MENTION", mention);
    replaceStringInPlace(s, "$RAUTHOR", tv.rAuthor);
    replaceStringInPlace(s, "$REPLY", tv.reply);
    replaceStringInPlace(s, "$INGROUP", tv.inGroup);
    replaceStringInPlace(s, "$TOGROUP", tv.toGroup);
    replaceStringInPlace(s, "$MSGID", tv.msgID);
    replaceStringInPlace(s, "$DATETIME", tv.datetime);
    replaceStringInPlace(s, "$MAUTHOR", tv.mAuthor);
    replaceStringInPlace(s, "$GROUPMESSAGE", tv.groupMsg);
    replaceStringInPlace(s, "$MESSAGECONTENT", tv.messageContent);

    replaceRegexWishBoundaries(s, tv.mention, mention);

    replacePrefixes(lang["fromTheme"], "$lang:", s);
    return s;
}

bool themeProcessBoolVar(std::string s, themeVariables &tv, json &themesettings)
{
    if (s.rfind("?", 0) == 0)
    {
        replaceStringInPlace(s, "?", "");
        if (s == "isSendReply")
            return tv.isSendReply;
        if (s == "isInGroup")
            return tv.isInGroup;
        if (s == "isSendGroup")
            return tv.isSendGroup;
        if (s == "isDeleted")
            return tv.isDeleted;
        if (s == "isOffline")
            return tv.isOffline;
        if (s == "isUnkonwnStatus")
            return tv.isUnkonwnStatus;
        if (s == "isReply")
            return tv.isReply;
        if (s == "askingReply")
            return tv.askingReply;
        if (s == "isEncrypted")
            return tv.isEncrypted;
        if (s == "isVerified")
            return tv.isVerified;
        if (s == "certifiedRank")
            return tv.certifiedRank;
        if (s == "botRank")
            return tv.botRank;
        if (s == "moderatorRank")
            return tv.moderatorRank;
        if (s == "adminRank")
            return tv.adminRank;
        if (s == "isGroupMessage")
            return tv.isGroupMessage;
        if (s == "isMention")
            return tv.isMention;
    }
    else if (s.rfind("settings:", 0) == 0)
    {
        replaceStringInPlace(s, "settings:", "");
        if (themesettings.contains(s))
            return themesettings[s];
    }

    return false;
}

void themeProcessSequence(json &lang, json &themeseq, themeVariables &tv, json &themesettings, json &mentionstyle)
{
    for (auto &ofs : themeseq.items())
    {
        if (ofs.value()["type"].get<std::string>() == "if")
        {
            if (themeProcessBoolVar(ofs.value()["condition"].get<std::string>(), tv, themesettings))
                themeProcessSequence(lang, ofs.value()["true"], tv, themesettings, mentionstyle);
            else
                themeProcessSequence(lang, ofs.value()["false"], tv, themesettings, mentionstyle);
        }
        else if (ofs.value()["type"].get<std::string>() == "print")
            std::cout << printStyle(ofs.value()["style"]) << themeProcessStringVar(lang, ofs.value()["print"], tv, mentionstyle, ofs.value()["style"]) << NORMAL;
        else if (ofs.value()["type"].get<std::string>() == "NEWLINE")
            std::cout << std::endl;
    }
}

void themeProcessLocation(json &lang, json &theme, std::string &location, themeVariables &tv)
{
    if (location == "prompt" || location == "message")
        themeProcessSequence(lang, theme[location], tv, theme["settings"], theme["mention"]);
}

std::string printStyle(json &style)
{
    std::string finalstyle("");
    for (auto &s : style.items())
    {
        finalstyle += getColor(s.value().get<std::string>());
    }
    return finalstyle;
}

void userError(json &theme, std::string err)
{
    std::cout << printStyle(theme["error"]["style"]) << err << NORMAL << std::endl;
}