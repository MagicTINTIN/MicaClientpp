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
     * Import messages from backup in memory
     */
    void importMemory(nlohmann::json const &backup);
    /**
     * Update messages from server in memory
     */
    void updateMemory(nlohmann::json const &messages, memorySettings memsettings);
    /**
     * Print messages from memory
     */
    void print(Message::messageSettings const &msettings, bool const &showids = false);
};

#endif // MESSAGEMEMORY_H_INCLUDED