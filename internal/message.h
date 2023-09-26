#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include <string>

class Message
{
private:
    std::string sender;
    std::string content;
    std::string dateTime;
    int certifiedUser;
    int rank;
public:
    Message();
    Message(std::string author, std::string message, std::string date, int certified, int rnk);

    void print();
};

#endif // MESSAGE_H_INCLUDED