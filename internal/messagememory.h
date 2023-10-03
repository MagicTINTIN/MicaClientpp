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
        std::string cfgpath;
        memorySettings();
        memorySettings(bool back, std::string path);
    };

    MessageMemory();

    /**
     * Add message to the local memory
     */
    void AddMessage(Message msg);
    /**
     * get message from local memory by ID
     */
    Message getMessageByID(int id);
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
    void print(json config, Message::messageSettings const &msettings, bool const &showids = false);
};

#endif // MESSAGEMEMORY_H_INCLUDED