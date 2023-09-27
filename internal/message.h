#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include <string>
#include <time.h>

time_t getTimestamp(std::string t);
void cleanMessageList(std::string &s);

class Message
{
public:
    enum messageStatus{ UNKNOWN=-1, ONLINE=0, OFFLINE=1, DELETED=2 };

    Message();
    Message(std::string author, std::string message, std::string date, int certified, int rnk, Message::messageStatus msgstatus = Message::messageStatus::ONLINE);
    Message(std::string author, std::string message, std::string date, std::string certified, std::string rnk, Message::messageStatus msgstatus = Message::messageStatus::ONLINE);

    /**
     * To print a message in terminal
    */
    void print();
    /**
     * Set a message status
    */
    void setStatus(messageStatus const &newStatus);
    /**
     * Get a message status
    */
    messageStatus getStatus();
    /**
     * Returns timestamp of the message
    */
    time_t getMsgTimestamp();
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
    time_t timestamp;
    messageStatus status;
};


#endif // MESSAGE_H_INCLUDED