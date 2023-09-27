#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "../includes/nlohmann/json.hpp"
#include "message.h"
#include "messagememory.h"

using json = nlohmann::json;

/* MESSAGE MEMORY CLASS */

MessageMemory::MessageMemory() : nbMessages(0), lastOffline(-1)
{
}

void MessageMemory::AddMessage(Message msg)
{
    memory.push_back(msg);
}

void MessageMemory::updateMemory(json const &messages)
{
    int msgnb = lastOffline;
    for (; msgnb < nbMessages; msgnb++)
    {
        /* code */
    }

    for (const auto &msg : messages.items())
    {
    }
}
