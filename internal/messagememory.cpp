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
    int nbJsonMsg(0), onlineMessagesFound(0);
    int memMsgNb = lastOffline;

    for (const auto &msg : messages.items())
    {
        nbJsonMsg++;
    }

    // if there is no message in json, it means that every message has been deleted
    if (nbJsonMsg == 0)
    {
        for (int memMsgNb = 0; memMsgNb < nbMessages; memMsgNb++)
        {
            memory[memMsgNb].setStatus(Message::DELETED);
        }
        return;
    }

    time_t nextJsonMsgTime = getTimestamp(messages[0]["date_time"].get<std::string>());
    for (; memMsgNb < nbMessages; memMsgNb++)
    {
        // if message in memory is older than the oldest on the server
        if (memory[memMsgNb].getStatus() != Message::ONLINE)
            continue;
        if (memory[memMsgNb].getMsgTimestamp() < nextJsonMsgTime && onlineMessagesFound == 0)
            memory[memMsgNb].setStatus(Message::OFFLINE);
        else if (memory[memMsgNb].getMsgTimestamp() < nextJsonMsgTime && onlineMessagesFound > 0)
            memory[memMsgNb].setStatus(Message::DELETED);
        else
        {
            if (onlineMessagesFound == 0)
                lastOffline = memMsgNb - 1;
            onlineMessagesFound++;
        }
    contin:;
    }
    json tmpmsgjson;
    for (; onlineMessagesFound <= nbJsonMsg; onlineMessagesFound++)
    {
        try
        {
            tmpmsgjson = messages[onlineMessagesFound - 1];
            AddMessage(Message(tmpmsgjson["sender"].get<std::string>(),
                               tmpmsgjson["content"].get<std::string>(),
                               tmpmsgjson["date_time"].get<std::string>(),
                               tmpmsgjson["certified"].get<std::string>(),
                               tmpmsgjson["rank"].get<std::string>()));
        }
        catch (std::out_of_range &e)
        {
            std::cout << "JSON MESSAGE READING ERROR : " << e.what() << '\n';
        }
    }
}
