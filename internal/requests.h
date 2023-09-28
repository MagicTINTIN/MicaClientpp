#pragma once
#include <string>
#include "message.h"
#include "messagememory.h"

int getServerUpdate(std::string const &geturl, MessageMemory &mem, MessageMemory::memorySettings &memsettings);
int sendMessage(std::string const &posturl, std::string const &content, std::string const &sender, std::string const &token = "");