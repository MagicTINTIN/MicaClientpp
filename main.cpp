#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <fstream>
#include "includes/nlohmann/json.hpp"
#include "internal/message.h"
#include "internal/messagememory.h"
#include "internal/aes.h"
#include "internal/requests.h"
#include "internal/tools.h"

using json = nlohmann::json;

int main(int argc, char const *argv[])
{
    int exitUpdateCode(0);

    std::ifstream f("config.json");
    json data = json::parse(f);
    // std::cout << data << std::endl;
    std::string geturl, username;
    try
    {
        geturl = data["server"].get<std::string>() + "msg.php?getmsg=json";
        username = data["username"].get<std::string>();
    }
    catch (std::out_of_range &e)
    {
        std::cout << "JSON ERROR : " << e.what() << '\n';
        return 1;
    }

    MessageMemory mem;

    std::string input;

    while (true)
    {
        exitUpdateCode = getServerUpdate(geturl, mem);
        if (exitUpdateCode == 0)
        {
            //clearScreen();
            mem.print();
        }
        else
        {
            std::cout << "UPDATE ERROR " << exitUpdateCode << std::endl;
            return exitUpdateCode;
        }
        std::cout << username << " > ";
        std::getline(std::cin, input);

        if (input == "/exit")
        {
            break; // Exit program if /exit
        }
        else
        {
            // send message to server
        }
    }

    std::cout << "Exiting MicaClient++..." << std::endl;

    return 0;
}