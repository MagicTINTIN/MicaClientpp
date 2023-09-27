#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include <string>
#include <vector>
#include "../includes/nlohmann/json.hpp"
class Message
{
public:
    enum messageStatus{ UNKNOWN=-1, ONLINE=2, DELETED=3, OFFLINE=4 };

    Message();
    Message(std::string author, std::string message, std::string date, int certified, int rnk, messageStatus msgstatus=ONLINE);

    /**
     * To print a message in terminal
    */
    void print();
    /**
     * Set a message status
    */
    void updateStatus(messageStatus const &newStatus);
    /**
     * Get a message status
    */
    messageStatus getStatus();
    /**
     * Returns if messages are equal
    */
    bool operator==(Message const &m);
    /**
     * Returns if messages not are equal
    */
    bool operator!=(Message const &m);
private:
    std::string sender;
    std::string content;
    std::string dateTime;
    int certifiedUser;
    int rank;
    messageStatus status;
};

class MessageMemory
{
private:
    int nbMessages;
    int nbFirstServerMessage;
    std::vector<Message> memory;
public:
    MessageMemory();
    
    /**
     * Add message to the local memory
    */
    void AddMessage(Message msg, int jsonMaxHistoryLength = -1);

    /**
     * Set all messages from server in memory
    */
    void setAllMessages(json const &messages);
};



#endif // MESSAGE_H_INCLUDED