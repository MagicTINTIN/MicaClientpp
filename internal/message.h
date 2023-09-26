#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include <string>
#include <vector>
#include "../includes/nlohmann/json.hpp"

class Message
{
private:
    std::string sender;
    std::string content;
    std::string dateTime;
    int certifiedUser;
    int rank;
    bool deleted;
public:
    Message();
    Message(std::string author, std::string message, std::string date, int certified, int rnk, bool del=false);

    /**
     * To print a message in terminal
    */
    void print();
    /**
     * Set a message as deleted from the server
    */
    void deletedFromServer();
    /**
     * Compare messages
    */
    bool operator==(Message const &m);
};

class MessageMemory
{
private:
    int nbMessages;
    int nbLastMessageFound;
    std::vector<Message> memory;
public:
    MessageMemory();
    
    /**
     * Add message to the local memory
    */
    void AddMessage(Message msg);

    /**
     * Set all messages from server in memory
    */
    void setAllMessages(json messages);
};



#endif // MESSAGE_H_INCLUDED