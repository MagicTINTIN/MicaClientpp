#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include "message.h"

/* TOOLS */

time_t getTimestamp(std::string t)
{
    struct tm tm;
    if (strptime(t.c_str(), "%Y-%m-%d %H:%M:%S", &tm) != NULL)
        return mktime(&tm);
    else
        return -1;
}

/* MESSAGE CLASS*/

Message::Message() : sender(""), content(""), dateTime(""), certifiedUser(0), rank(0), status(ONLINE), timestamp(-1)
{
}

Message::Message(std::string author, std::string message, std::string date, int certified, int rnk, messageStatus msgstatus = ONLINE) : sender(author), content(message),
                                                                                                                                        dateTime(date), timestamp(getTimestamp(date)),
                                                                                                                                        certifiedUser(certified), rank(rnk), status(msgstatus)
{
}

Message::Message(std::string author, std::string message, std::string date, std::string certified, std::string rnk, messageStatus msgstatus = ONLINE) : sender(author), content(message),
                                                                                                                                        dateTime(date), timestamp(getTimestamp(date)),
                                                                                                                                        certifiedUser(std::stoi(certified)), rank(std::stoi(rnk)), status(msgstatus)
{
}

void Message::print()
{
    std::cout << "[" << sender << "] " << content << std::endl;
}

void Message::setStatus(messageStatus const &newStatus)
{
    this->status = newStatus;
}

Message::messageStatus Message::getStatus()
{
    return this->status;
}

time_t Message::getMsgTimestamp()
{
    if (this->timestamp == -1)
        this->timestamp = getTimestamp(this->dateTime);

    return this->timestamp;
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