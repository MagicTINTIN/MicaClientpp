#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <fstream>
#include "requests.h"
#include "message.h"
#include "messagememory.h"
#include "../includes/nlohmann/json.hpp"

using json = nlohmann::json;

static size_t getHtmlCallback(void *contents, size_t size, size_t nmemb, void *ptr)
{
    // Cast ptr to std::string pointer and append contents to that string
    ((std::string*)ptr)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int getServerUpdate(std::string const &serverurl, MessageMemory &mem, MessageMemory::memorySettings &memsettings)
{
    int exitcode(0);
    CURL *curl;
    CURLcode res;
    std::string htmlBuffer;
    std::string geturl = serverurl + "msg.php?getmsg=json";

    curl = curl_easy_init();
    if (curl)
    {
        //std::cout << "getURL page : " << serverurl << std::endl;
        curl_easy_setopt(curl, CURLOPT_URL, geturl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, getHtmlCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &htmlBuffer);
        res = curl_easy_perform(curl);
        //std::cout << "RES : " << res << std::endl;
        if (CURLE_OK == res)
        {
            cleanMessageList(htmlBuffer);
            //std::cout << htmlBuffer << std::endl;
            json jsonMsgList = json::parse(htmlBuffer);
            mem.updateMemory(jsonMsgList, memsettings);
        }
        else
        {
            std::cout << "An error has occured, maybe check the server in config.json" << std::endl;
            exitcode = 2;
        }
    }
    else
    {
        std::cout << "ERROR: Curl is not working..." << std::endl;
        exitcode = 9;
    }

    /* always cleanup */
    curl_easy_cleanup(curl);
    return exitcode;
}

int sendMessage(std::string const &serverurl, std::string const &content, std::string const &sender, std::string const &token)
{
    int exitcode(0);
    CURL *curl;
    CURLcode res;

    std::string url = serverurl + "msg.php?message=" + urlEncode(content) + "&sender=" + urlEncode(sender) + "&token=" + urlEncode(token);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        res = curl_easy_perform(curl);
        if (CURLE_OK != res)
        {
            std::cout << "An error has occured, maybe check the server in config.json" << std::endl;
            exitcode = 3;
        }
    }
    else
    {
        std::cout << "ERROR: Curl is not working..." << std::endl;
        exitcode = 9;
    }

    /* always cleanup */
    curl_easy_cleanup(curl);
    return exitcode;
}

int delMessage(std::string const &serverurl, std::string const &msgid, std::string const &sender, std::string const &token)
{
    int exitcode(0);
    CURL *curl;
    CURLcode res;

    // delmsg.php?del=$msg\&adminpseudo=$user\&admintoken=$token
    std::string url = serverurl + "delmsg.php?del=" + urlEncode(msgid) + "&adminpseudo=" + urlEncode(sender) + "&admintoken=" + urlEncode(token);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        res = curl_easy_perform(curl);
        if (CURLE_OK != res)
        {
            std::cout << "An error has occured, maybe check the server in config.json" << std::endl;
            exitcode = 4;
        }
    }
    else
    {
        std::cout << "ERROR: Curl is not working..." << std::endl;
        exitcode = 9;
    }

    /* always cleanup */
    curl_easy_cleanup(curl);
    return exitcode;
}