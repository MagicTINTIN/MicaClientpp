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
themeVariables::themeVariables(bool isr, bool iig, bool isg, bool imm,
                               std::string u, std::string ra, std::string r,
                               std::string ig, std::string tg, std::string idr)
    : isSendReply(isr), isInGroup(iig), isSendGroup(isg), isYourMessage(false), isModeratorMode(imm),
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
                               bool mr, bool ar, bool igm, bool im, bool iym, bool imm)
    : isSendReply(false), isInGroup(false), isSendGroup(false), isYourMessage(iym), isModeratorMode(imm),
      isDeleted(idel), isOffline(ioff), isUnkonwnStatus(ius), isReply(ir),
      askingReply(askr), isEncrypted(iencr), isVerified(iv), certifiedRank(cr),
      botRank(br), moderatorRank(mr), adminRank(ar), isGroupMessage(igm), isMention(im),

      username(u), mention("@" + u), rAuthor(ra), reply(r), rID(idr), inGroup(gm), toGroup(gm), msgID(mid),
      IDMAuthor(idma), datetime(dt), mAuthor(ma), groupMsg(gm), messageContent(mc)
{
}

std::string themeProcessStringVar(json &lang, std::string s, themeVariables &tv, json &mentionstyle, json &postmentionstyle, json &themesettings, json &theme)
{
    std::string mention = printStyle(mentionstyle) + tv.mention + NORMAL + printStyle(postmentionstyle);

    replacePrefixes(lang["fromTheme"], "$lang:", s);
    replacePrefixes(themesettings, "$settings:", s);

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

    std::string displayContent = tv.messageContent;
    if (theme["formatText"].get<bool>())
    {
        if (false) {
        displayContent = replaceDelimiters(displayContent, "**", "**", getColor("BOLD"), getColor("NORMAL"));
        displayContent = replaceDelimiters(displayContent, "__", "__", getColor("UNDERLINED"), getColor("NORMAL"));
        displayContent = replaceDelimiters(displayContent, "*", "*", getColor("ITALIC"), getColor("NORMAL"));
        displayContent = replaceDelimiters(displayContent, "_", "_", getColor("ITALIC"), getColor("NORMAL"));
        displayContent = replaceDelimiters(displayContent, "~~", "~~", getColor("STRIKED"), getColor("NORMAL"));
        displayContent = replaceDelimiters(displayContent, "```", "```", getColor("REVERSED"), getColor("NORMAL"));
        displayContent = replaceDelimiters(displayContent, "`", "`", getColor("REVERSED"), getColor("NORMAL"));
        }
        else {
            displayContent = textFormatter(displayContent);
        }

        replaceStringInPlace(displayContent, "\\\\", "\\");
        replaceStringInPlace(displayContent, "\\_", "_");
        replaceStringInPlace(displayContent, "\\*", "*");
        replaceStringInPlace(displayContent, "\\~", "~");
        replaceStringInPlace(displayContent, "\\`", "`");
        
    }
    replaceStringInPlace(s, "$MESSAGECONTENT", displayContent);

    replaceRegexWishBoundaries(s, tv.mention, mention);

    // maybe add local variable that users can enter
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
        if (s == "isModeratorMode")
            return tv.isModeratorMode;
    }
    else if (s.rfind("settings:", 0) == 0)
    {
        replaceStringInPlace(s, "settings:", "");
        if (themesettings.contains(s))
            return themesettings[s];
    }

    return false;
}

int themeProcessSequence(json &lang, json &themeseq, themeVariables &tv, json &themesettings, json &mentionstyle, std::string &str, json &theme)
{
    int rtn(0);
    for (auto &ofs : themeseq.items())
    {
        if (ofs.value()["type"].get<std::string>() == "if")
        {
            if (themeProcessBoolVar(ofs.value()["condition"].get<std::string>(), tv, themesettings))
                rtn = themeProcessSequence(lang, ofs.value()["true"], tv, themesettings, mentionstyle, str, theme);
            else
                rtn = themeProcessSequence(lang, ofs.value()["false"], tv, themesettings, mentionstyle, str, theme);
        }
        else if (ofs.value()["type"].get<std::string>() == "print")
            str += printStyle(ofs.value()["style"]) + themeProcessStringVar(lang, ofs.value()["print"], tv, mentionstyle, ofs.value()["style"], themesettings, theme) + NORMAL;
        else if (ofs.value()["type"].get<std::string>() == "NEWLINE")
            str += '\n';
        else if (ofs.value()["type"].get<std::string>() == "BREAKDISPLAY")
            return 1;
        if (rtn != 0)
            return rtn;
    }
    return 0;
}

int themeProcessLocation(json &lang, json &theme, std::string const &location, themeVariables &tv, std::string &str)
{
    str = "";
    int rtn(0);
    if (location == "prompt" || location == "message")
        rtn = themeProcessSequence(lang, theme[location], tv, theme["settings"], theme["mention"]["style"], str, theme);

    return rtn;
}

void themeProcessPrint(json &lang, json &theme, std::string const &location, themeVariables &tv)
{
    std::string str("");
    themeProcessLocation(lang, theme, location, tv, str);

    std::cout << str;
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