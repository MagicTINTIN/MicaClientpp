#include <string>
#include <iomanip>
#include <iostream>
#include "colors.h"
#include "tools.h"
#include "../includes/nlohmann/json.hpp"
#include "themes.h"
using json = nlohmann::json;

themeVariables::themeVariables()
    : isSendReply(false), isInGroup(false), isSendGroup(false), isYourMessage(false),
      isDeleted(false), isOffline(false), isUnkonwnStatus(false), isReply(false),
      askingReply(false), isEncrypted(false), isVerified(false), certifiedRank(false),
      botRank(false), moderatorRank(false), adminRank(false), isGroupMessage(false), isMention(false),

      username(""), mention(""), rAuthor(""), reply(""), rID("X"), inGroup(""), toGroup(""), msgID("X"),
      IDMAuthor("X"), datetime("YYYY-MM-DD HH:MM:SS"), mAuthor(""), groupMsg(""), messageContent("")
{
}
themeVariables::themeVariables(bool isr, bool iig, bool isg,
                               std::string u, std::string ra, std::string r,
                               std::string ig, std::string tg, std::string idr)
    : isSendReply(isr), isInGroup(iig), isSendGroup(isg), isYourMessage(false),
      isDeleted(false), isOffline(false), isUnkonwnStatus(false), isReply(false),
      askingReply(false), isEncrypted(false), isVerified(false), certifiedRank(false),
      botRank(false), moderatorRank(false), adminRank(false), isGroupMessage(false), isMention(false),

      username(u), mention("@" + u), rAuthor(ra), reply(r), rID(idr), inGroup(ig), toGroup(tg), msgID("X"),
      IDMAuthor("X"), datetime("YYYY-MM-DD HH:MM:SS"), mAuthor(""), groupMsg(""), messageContent("")

{
}
themeVariables::themeVariables(std::string u, std::string ra, std::string r,
                               std::string idr, std::string mid, std::string idma,
                               std::string dt, std::string ma, std::string gm, std::string mc,
                   bool idel, bool ioff, bool ius, bool ir,
                   bool askr, bool iencr, bool iv, bool cr, bool br,
                   bool mr, bool ar, bool igm, bool im, bool iym)
    : isSendReply(false), isInGroup(false), isSendGroup(false), isYourMessage(iym),
      isDeleted(idel), isOffline(ioff), isUnkonwnStatus(ius), isReply(ir),
      askingReply(askr), isEncrypted(iencr), isVerified(iv), certifiedRank(cr),
      botRank(br), moderatorRank(mr), adminRank(ar), isGroupMessage(igm), isMention(im),

      username(u), mention("@" + u), rAuthor(ra), reply(r), rID(idr), inGroup(gm), toGroup(gm), msgID(mid),
      IDMAuthor(idma), datetime(dt), mAuthor(ma), groupMsg(gm), messageContent(mc)
{
}

std::string themeProcessStringVar(json &lang, std::string s, themeVariables &tv, json &mentionstyle, json &postmentionstyle)
{
    std::string mention = printStyle(mentionstyle) + tv.mention + printStyle(postmentionstyle);
    replaceStringInPlace(s, "$USERNAME", tv.username);
    replaceStringInPlace(s, "$MENTION", mention);
    replaceStringInPlace(s, "$RAUTHOR", tv.rAuthor);
    replaceStringInPlace(s, "$REPLY", tv.reply);
    replaceStringInPlace(s, "$RID", tv.rID);
    replaceStringInPlace(s, "$INGROUP", tv.inGroup);
    replaceStringInPlace(s, "$TOGROUP", tv.toGroup);
    replaceStringInPlace(s, "$MSGID", tv.msgID);
    replaceStringInPlace(s, "$IDMAUTHOR", tv.IDMAuthor);
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
        if (s == "isYourMessage")
            return tv.isYourMessage;
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

void themeProcessLocation(json &lang, json &theme, std::string const &location, themeVariables &tv)
{
    if (location == "prompt" || location == "message")
        themeProcessSequence(lang, theme[location], tv, theme["settings"], theme["mention"]["style"]);
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