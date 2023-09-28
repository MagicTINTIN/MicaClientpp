#pragma once
#include <string>
#include "message.h"
#include "messagememory.h"

int getServerUpdate(std::string const &geturl, MessageMemory &mem, MessageMemory::memorySettings &memsettings);
