#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "../includes/nlohmann/json.hpp"
#include "message.h"
#include "messagememory.h"
using json = nlohmann::json;

/* JSON TOOLS */
json to_json(const Message::jsonMessage &m)
{
    return json{
        {"sender", m.sender},
        {"content", m.content},
        {"dateTime", m.dateTime},
        {"certifiedUser", m.certifiedUser},
        {"rank", m.rank},
        {"timestamp", m.timestamp},
        {"decrypted", m.decrypted},
        {"status", m.status},
    };
}

/* MESSAGE MEMORY CLASS */

MessageMemory::memorySettings::memorySettings() : backup(true)
{
}
MessageMemory::memorySettings::memorySettings(bool back) : backup(back)
{
}

MessageMemory::MessageMemory() : nbMessages(0), firstOnline(0)
{
}

void MessageMemory::AddMessage(Message msg)
{
    memory.push_back(msg);
    nbMessages++;
}

void MessageMemory::importMemory(nlohmann::json const &backup)
{
    for (auto &m : backup.items())
    {
        Message msg(m.value()["sender"].get<std::string>(),
                    m.value()["content"].get<std::string>(),
                    m.value()["dateTime"].get<std::string>(),
                    m.value()["certifiedUser"].get<int>(),
                    m.value()["rank"].get<int>(),
                    m.value()["timestamp"].get<time_t>(),
                    m.value()["decrypted"].get<std::string>(),
                    m.value()["status"].get<Message::messageStatus>());
        AddMessage(msg);
    }
}

void MessageMemory::updateMemory(json const &messages, memorySettings memsettings)
{
    int nbJsonMsg(0), onlineMessagesFound(0);
    int memMsgNb = firstOnline;

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
        else if ((memory[memMsgNb].getMsgTimestamp() < nextJsonMsgTime || onlineMessagesFound == nbJsonMsg) && onlineMessagesFound > 0)
            memory[memMsgNb].setStatus(Message::DELETED);
        else
        {
            if (onlineMessagesFound == 0)
                firstOnline = memMsgNb;
            onlineMessagesFound++;
            if (onlineMessagesFound < nbJsonMsg)
                nextJsonMsgTime = getTimestamp(messages[onlineMessagesFound]["date_time"].get<std::string>());
        }
    contin:;
    }
    json tmpmsgjson;
    for (; onlineMessagesFound < nbJsonMsg; onlineMessagesFound++)
    {
        // std::cout << "Importing msg nb " << onlineMessagesFound << std::endl;
        try
        {
            tmpmsgjson = messages[onlineMessagesFound];
            AddMessage(Message(tmpmsgjson["sender"].get<std::string>(),
                               tmpmsgjson["content"].get<std::string>(),
                               tmpmsgjson["date_time"].get<std::string>(),
                               tmpmsgjson["id_certified_user"].get<std::string>(),
                               tmpmsgjson["rank"].get<std::string>()));
        }
        catch (std::out_of_range &e)
        {
            std::cout << "JSON MESSAGE READING ERROR : " << e.what() << '\n';
        }
    }
    if (memsettings.backup)
    {
        json j;
        for (Message &msg : memory)
        {
            Message::jsonMessage jsonMsg(msg);
            j.push_back(to_json(jsonMsg));
        }
        std::ofstream o("backup.json");
        o << std::setw(4) << j << std::endl;
    }
}

void MessageMemory::print(Message::messageSettings const &msettings)
{
    for (Message &msg : memory)
    {
        msg.print(msettings);
    }
}