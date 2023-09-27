#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "../includes/nlohmann/json.hpp"
#include "message.h"

using json = nlohmann::json;

/* MESSAGE CLASS*/

Message::Message() : sender(""), content(""), dateTime(""), certifiedUser(0), rank(0), status(ONLINE)
{
}

Message::Message(std::string author, std::string message, std::string date, int certified, int rnk, messageStatus msgstatus = ONLINE) : sender(author), content(message), dateTime(date), certifiedUser(certified), rank(rnk), status(msgstatus)
{
}

void Message::print()
{
    std::cout << "[" << sender << "] " << content << std::endl;
}

void Message::updateStatus(messageStatus const &newStatus)
{
    this->status = newStatus;
}

Message::messageStatus Message::getStatus()
{
    return this->status;
}

bool Message::operator==(Message const &m)
{
    return this->sender == m.sender &&
           this->content == m.content &&
           this->dateTime == m.dateTime &&
           this->certifiedUser == m.certifiedUser &&
           this->rank == m.rank;
}

bool Message::operator!=(Message const &m)
{
    return this->sender != m.sender ||
           this->content != m.content ||
           this->dateTime != m.dateTime ||
           this->certifiedUser != m.certifiedUser ||
           this->rank != m.rank;
}

/* MESSAGE MEMORY CLASS */

MessageMemory::MessageMemory() : nbMessages(0), nbFirstServerMessage(0)
{
}

void MessageMemory::AddMessage(Message msg, int jsonMaxHistoryLength = -1)
{
    int msgmem(nbMessages - 1);
    int checkUntilMsgNb;
    if (jsonMaxHistoryLength == -1)
        checkUntilMsgNb = 0;
    else
        checkUntilMsgNb = nbMessages - jsonMaxHistoryLength - 1;

    while (msgmem >= checkUntilMsgNb && memory[msgmem] != msg && memory[msgmem].getStatus() != Message::OFFLINE)
    {
        /* code */
    }
}

void MessageMemory::setAllMessages(json const &messages)
{
    for (const auto &msg : messages.items())
    {
    }
}