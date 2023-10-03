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

/* MESSAGE MEMORY CLASS */

MessageMemory::memorySettings::memorySettings() : backup(true), cfgpath("./")
{
}
MessageMemory::memorySettings::memorySettings(bool back, std::string path) : backup(back), cfgpath(path)
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
        Message msg(m.value()["id"].get<int>(),
                    m.value()["sender"].get<std::string>(),
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

    int nextOnlineID(stoi(messages[onlineMessagesFound]["id"].get<std::string>()));
    while (memMsgNb > 0 && memory[memMsgNb].getID() > nextOnlineID)
    {
        memMsgNb--;
        memory[memMsgNb].setStatus(Message::ONLINE);
    }
    for (; memMsgNb < nbMessages; memMsgNb++)
    {
        if (memory[memMsgNb].getStatus() != Message::ONLINE)
            continue;
        if (memory[memMsgNb].getID() < nextOnlineID && onlineMessagesFound == 0)
            memory[memMsgNb].setStatus(Message::OFFLINE);
        else if ((memory[memMsgNb].getID() < nextOnlineID || onlineMessagesFound == nbJsonMsg) && onlineMessagesFound > 0)
            memory[memMsgNb].setStatus(Message::DELETED);
        else
        {
            if (onlineMessagesFound == 0)
                firstOnline = memMsgNb;
            onlineMessagesFound++;
            if (onlineMessagesFound < nbJsonMsg)
            {
                nextOnlineID = stoi(messages[onlineMessagesFound]["id"].get<std::string>());
            }
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
            AddMessage(Message(tmpmsgjson["id"].get<std::string>(),
                               tmpmsgjson["sender"].get<std::string>(),
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
            j.push_back(jsonMsg.toJson());
        }
        std::ofstream o(memsettings.cfgpath + "backup.json");
        o << std::setw(4) << j << std::endl;
    }
}

void MessageMemory::print(json config, Message::messageSettings const &msettings, bool const &showids)
{
    for (Message &msg : memory)
    {
        Message::isgroupmessage igm = msg.isGroupContent(config);
        Message::isreplymessage irm;
        if (igm.isgroup)
            irm = msg.isRelpyContent(true, igm.messagecontent);
        else
            irm = msg.isRelpyContent();

        if (irm.isreply)
            msg.print(msettings, showids, igm, irm.idreply, getMessageByID(irm.idreply), true, irm.messagecontent);
        else
            msg.print(msettings, showids, igm);
    }
}

Message MessageMemory::getMessageByID(int id)
{
    int i(nbMessages - 1);
    Message msg(memory[i]);
    bool found(msg.getID() == id);
    while (i > 0 && !found)
    {
        i--;
        msg = memory[i];
        if (msg.getID() == id)
            found = true;
    }
    if (found)
        return msg;
    else
        return Message();
}