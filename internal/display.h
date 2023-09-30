#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED

void showHelp(bool moderator = false);
int showReplying(json config, MessageMemory &mem, int id, Message::messageSettings &msgs, Message::isgroupmessage &igm);
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
int getArguments(MessageMemory &mem, Message::messageSettings &msgsettings, std::string const &serverurl, json &config, std::string &username, std::string &token, std::string &input, bool const &moderatormode, int &exitUpdateCode, int &exitSendCode);
void clearScreen();

#endif // DISPLAY_H_INCLUDED