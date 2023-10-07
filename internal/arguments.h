#pragma once

#ifndef ARGUMENTS_H_INCLUDED
#define ARGUMENTS_H_INCLUDED
#include <string>
#include <iomanip>
#include <iostream>
#include "message.h"
#include "messagememory.h"
#include "colors.h"
#include "tools.h"
#include "display.h"
#include "requests.h"
#include "aes.h"
#include "arguments.h"

using json = nlohmann::json;

int deleteArg(json &theme, std::string &input, std::string const &serverurl, std::string &username, std::string &token, int &exitSendCode);

int replyArg(json &lang, json &theme, json config, MessageMemory &mem, std::string &input, std::string const &serverurl, Message::messageSettings &msgsettings, std::string &username, std::string &token, int &exitSendCode);

int uSendArg(std::string &input, std::string const &serverurl, std::string &username, std::string &token, int &exitSendCode);

int pSendArg(json &theme, MessageMemory &mem, json &config, std::string &input, std::string const &serverurl, Message::messageSettings &msgsettings, std::string &username, std::string &token, int &exitSendCode);

int sendArg(Message::messageSettings &msgsettings, std::string &input, std::string const &serverurl, std::string &username, std::string &token, int &exitSendCode);

int groupArg(json &theme, std::string &input, Message::messageSettings &msettings, json &config);

int pChannelSendArg(json &theme, MessageMemory &mem, json &config, std::string &input, std::string const &serverurl, Message::messageSettings &msgsettings, std::string &username, std::string &token, int &exitSendCode);

#endif // ARGUMENTS_H_INCLUDED