#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include <string>
#include <time.h>

//the following are UBUNTU/LINUX, and MacOS ONLY terminal color codes.
#define RESET       "\033[0m"
#define BLACK       "\033[30m"      /* Black */
#define RED         "\033[31m"      /* Red */
#define GREEN       "\033[32m"      /* Green */
#define YELLOW      "\033[33m"      /* Yellow */
#define BLUE        "\033[34m"      /* Blue */
#define MAGENTA     "\033[35m"      /* Magenta */
#define CYAN        "\033[36m"      /* Cyan */
#define WHITE       "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

time_t getTimestamp(std::string t);
void ReplaceStringInPlace(std::string &s, const std::string &search, const std::string &replace);
void escapeBackslash(std::string &s);
char getCharacterFromEscapeSequence(const std::string& escapeSequence);
void replaceEscapeSequences(std::string &s);
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