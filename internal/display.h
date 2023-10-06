#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED

void showHelp(json &theme, bool moderator = false);
int showReplying(json &theme, json config, MessageMemory &mem, int id, Message::messageSettings &msgs, Message::isgroupmessage &igm);
// 0 nothing, -1 exit, 1 nothing but help, 2 impossible to find msg/group
/**
 * Execute commands from the main chat
 * @returns 0 if nothing has been done ||
 *          1 if user is invited to see /help ||
 *          2 if user asked for a message or a group that can't be found ||
 *          3 message/request sent
 *          -1 if user wanted to exit MC++ ||
 *          < -1 fatal errors from internals functions ||
 *          > 3 errors messages from internals functions
*/
int getArguments(json &lang, json &theme, MessageMemory &mem, Message::messageSettings &msgsettings, std::string const &serverurl, json &config, std::string &username, std::string &token, std::string &input, bool const &moderatormode, int &exitUpdateCode, int &exitSendCode);
std::string printStyle(json style);
void title(std::string t);
void clearScreen();

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

    std::string username; //
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


#endif // DISPLAY_H_INCLUDED