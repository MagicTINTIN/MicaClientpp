#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include <string>
#include <time.h>
#include "tools.h"
#include "../includes/nlohmann/json.hpp"
using json = nlohmann::json;

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

    struct isreplymessage
    {
        bool isreply;
        std::string messagecontent;
        int idreply;
        isreplymessage();
        isreplymessage(bool is, std::string c, int id);
    };

    struct isgroupmessage
    {
        bool isgroup;
        std::string messagecontent;
        std::string groupname;
        std::string key;
        bool visible;
        isgroupmessage();
        isgroupmessage(bool is, std::string c, std::string n, bool v, std::string k);
    };
    struct messageSettings
    {
        bool deletedmsg;
        bool offlinemsg;
        bool datetimemsg;
        bool modmsg;
        bool securemsg;
        bool blockUnverified;
        std::vector<std::string> blockedUsers;
        std::string generalkey;
        std::string pseudo;
        std::string channel;
        int msgmaxsize;
        int encryptedmaxsize;
        messageSettings();
        messageSettings(bool ddel, bool doff, std::string gkey, bool dt, std::string psd, bool mod, bool sec, bool buu, json blu, std::string ch, int maxsmsg);
    };

    struct jsonMessage
    {
        int id;
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
        jsonMessage(int i, std::string a, std::string c, std::string d, int cu, int r, time_t t, std::string dc, messageStatus s);
        /**
         * To convert a jsonMessage struct to a json object
         */
        json toJson();
    };

    Message();
    Message(int i, std::string author, std::string message, std::string date, int certified, int rnk, Message::messageStatus msgstatus = Message::messageStatus::ONLINE);
    // used when parsing backup json
    Message(int i, std::string author, std::string message, std::string date, int certified, int rnk, time_t ts, std::string dc, Message::messageStatus msgstatus = Message::messageStatus::ONLINE);
    // used when parsing server json
    Message(std::string i, std::string author, std::string message, std::string date, std::string certified, std::string rnk, Message::messageStatus msgstatus = Message::messageStatus::ONLINE);

    /**
     * To print a message in terminal
     */
    void print(json &lang, json &theme, messageSettings const &msettings, bool const &showids, isgroupmessage const &igm = isgroupmessage(), int idReplied = -1, Message repliedTo = Message(), bool const &isReply = false, std::string replyContent = "");
    /**
     * Replied message
     */
    std::string getReplyContent(json &theme, messageSettings const &msettings, isgroupmessage const &igm = isgroupmessage());
    /**
     * Set a message author
     */
    std::string getAuthor();
    /**
     * Set a message content
     */
    std::string getContent();
    /**
     * Set a message status
     */
    void setStatus(messageStatus const &newStatus);
    /**
     * Get a message status
     */
    messageStatus getStatus();
    /**
     * Get a message ID
     */
    int getID();
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
    /**
     * Returns data to know if it is a reply
    */
    isreplymessage isRelpyContent(bool fromGroup = false, std::string groupContent = "");
    /**
     * Returns data to know if it is a group message
    */
    isgroupmessage isGroupContent(json config);

private:
    int id;
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