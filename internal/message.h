#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include <string>
#include <time.h>
#include "tools.h"

time_t getTimestamp(std::string t);
void ReplaceStringInPlace(std::string &s, const std::string &search, const std::string &replace);
void escapeBackslash(std::string &s);
char getCharacterFromEscapeSequence(const std::string &escapeSequence);
void replaceEscapeSequences(std::string &s);
void cleanMessageList(std::string &s);
class Message
{
public:
    enum messageStatus
    {
        UNKNOWN = -1,
        ONLINE = 0,
        OFFLINE = 1,
        DELETED = 2
    };
    struct messageSettings
    {
        bool deletedmsg;
        bool offlinemsg;
        std::string generalkey;
        messageSettings();
        messageSettings(bool ddel, bool doff, std::string gkey);
    };

    struct jsonMessage
    {
        std::string sender;
        std::string content;
        std::string dateTime;
        int certifiedUser;
        int rank;
        time_t timestamp;
        std::string decrypted;
        messageStatus status;
        jsonMessage();
        jsonMessage(Message message);
        jsonMessage(std::string a, std::string c, std::string d, int cu, int r, time_t t, std::string dc, messageStatus s);
    };

    Message();
    Message(std::string author, std::string message, std::string date, int certified, int rnk, Message::messageStatus msgstatus = Message::messageStatus::ONLINE);
    Message(std::string author, std::string message, std::string date, int certified, int rnk, time_t ts, std::string dc, Message::messageStatus msgstatus = Message::messageStatus::ONLINE);
    Message(std::string author, std::string message, std::string date, std::string certified, std::string rnk, Message::messageStatus msgstatus = Message::messageStatus::ONLINE);

    /**
     * To print a message in terminal
     */
    void print(messageSettings const &msettings);
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
    std::string decrypted;
    std::string dateTime;
    int certifiedUser;
    int rank;
    time_t timestamp;
    messageStatus status;
};

#endif // MESSAGE_H_INCLUDED