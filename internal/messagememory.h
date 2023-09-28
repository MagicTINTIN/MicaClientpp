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
    int firstOnline;
    std::vector<Message> memory;
public:
    struct memorySettings
    {
        bool backup;
        memorySettings();
        memorySettings(bool back);
    };

    MessageMemory();

    /**
     * Add message to the local memory
     */
    void AddMessage(Message msg);

    /**
     * Update messages from server in memory
     */
    void updateMemory(nlohmann::json const &messages, memorySettings memsettings);
    /**
     * Print messages from memory
     */
    void print(Message::messageSettings const &msettings);
};

#endif // MESSAGEMEMORY_H_INCLUDED