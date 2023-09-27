#ifndef MESSAGEMEMORY_H_INCLUDED
#define MESSAGEMEMORY_H_INCLUDED

#include <string>
#include <vector>
#include <time.h>
#include "../includes/nlohmann/json.hpp"

class MessageMemory
{
private:
    int nbMessages;
    int lastOffline;
    std::vector<Message> memory;
public:
    MessageMemory();
    
    /**
     * Add message to the local memory
    */
    void AddMessage(Message msg);

    /**
     * Update messages from server in memory
    */
    void updateMemory(json const &messages);
};


#endif // MESSAGEMEMORY_H_INCLUDED