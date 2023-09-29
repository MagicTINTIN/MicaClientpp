#pragma once
#include <string>
#include "message.h"
#include "messagememory.h"

int getServerUpdate(std::string const &serverurl, MessageMemory &mem, MessageMemory::memorySettings &memsettings);
int sendMessage(std::string const &serverurl, std::string const &content, std::string const &sender, std::string const &token = "");
int delMessage(std::string const &serverurl, std::string const &msgid, std::string const &sender, std::string const &token);
