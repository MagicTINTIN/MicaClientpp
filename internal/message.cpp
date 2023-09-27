#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "../includes/nlohmann/json.hpp"
#include "message.h"

using json = nlohmann::json;

/* MESSAGE CLASS*/

Message::Message() : sender(""), content(""), dateTime(""), certifiedUser(0), rank(0), deleted(false)
{}

Message::Message(std::string author, std::string message, std::string date, int certified, int rnk, bool del=false) : sender(author), content(message), dateTime(date), certifiedUser(certified), rank(rnk), deleted(del)
{}

void Message::print()
{
    std::cout << "[" << sender << "] " << content << std::endl;
}

bool Message::operator==(Message const &m) {
    return this->sender == m.sender 
        && this->content == m.content
        && this->dateTime == m.dateTime
        && this->certifiedUser == m.certifiedUser
        && this->rank == m.rank;
}

bool Message::operator!=(Message const &m) {
    return this->sender != m.sender 
        || this->content != m.content
        || this->dateTime != m.dateTime
        || this->certifiedUser != m.certifiedUser
        || this->rank != m.rank;
}

/* MESSAGE MEMORY CLASS */

MessageMemory::MessageMemory() : nbMessages(0), nbFirstServerMessage(0)
{
}

void MessageMemory::AddMessage(Message msg, int jsonMaxHistoryLength = -1) {
    int msgmem(nbMessages - 1);
    int checkUntilMsgNb;
    if (jsonMaxHistoryLength == -1)
        checkUntilMsgNb = 0;
    else
        checkUntilMsgNb = nbMessages - jsonMaxHistoryLength - 1;
    
    while (msgmem >= checkUntilMsgNb && memory[msgmem] != msg)
    {
        /* code */
    }
    
}

void MessageMemory::setAllMessages(json messages) {
    for (const auto &msg : messages.items())
	{
		
	}
}