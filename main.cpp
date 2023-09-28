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

using json = nlohmann::json;

int main(int argc, char const *argv[]) {
    int exitUpdateCode(0);

    std::ifstream f("config.json");
    json data = json::parse(f);
    // std::cout << data << std::endl;
    std::string geturl;
    try
    {
        geturl = data["server"].get<std::string>() + "msg.php?getmsg=json";
    }
    catch (std::out_of_range& e)
    {
        std::cout << "JSON ERROR : " << e.what() << '\n';
        return 1;
    }
    
    MessageMemory mem;
    exitUpdateCode = getServerUpdate(geturl, mem);
    if (exitUpdateCode == 0)
        mem.print();
    else {
        std::cout << "UPDATE ERROR " << exitUpdateCode << std::endl;
        return exitUpdateCode; 
    }
    return 0;
 }