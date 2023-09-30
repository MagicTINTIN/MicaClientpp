#pragma once

#ifndef ARGUMENTS_H_INCLUDED
#define ARGUMENTS_H_INCLUDED

int deleteArg(std::string &input, std::string const &serverurl, std::string &username, std::string &token, int &exitSendCode);

int replyArg(json config, MessageMemory &mem, std::string &input, std::string const &serverurl, Message::messageSettings &msgsettings, std::string &username, std::string &token, int &exitSendCode);

int uSendArg(std::string &input, std::string const &serverurl, std::string &username, std::string &token, int &exitSendCode);

int pSendArg(MessageMemory &mem, json &config, std::string &input, std::string const &serverurl, Message::messageSettings &msgsettings, std::string &username, std::string &token, int &exitSendCode);

int sendArg(Message::messageSettings &msgsettings, std::string &input, std::string const &serverurl, std::string &username, std::string &token, int &exitSendCode);

#endif // ARGUMENTS_H_INCLUDED