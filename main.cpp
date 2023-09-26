#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <fstream>
#include "includes/nlohmann/json.hpp"

using json = nlohmann::json;

static size_t getHtmlCallback(void *contents, size_t size, size_t nmemb, void *ptr)
{
    // Cast ptr to std::string pointer and append contents to that string
    ((std::string*)ptr)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main(int argc, char const *argv[]) {
    CURL *curl;
    CURLcode res;
    std::string htmlBuffer;

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
    }
   

    curl = curl_easy_init();
    if(curl) {
        std::cout << "getURL page : " << geturl << std::endl;
        curl_easy_setopt(curl, CURLOPT_URL, geturl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, getHtmlCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &htmlBuffer);
        res = curl_easy_perform(curl);
        std::cout << "RES : " << res << std::endl;
        if(CURLE_OK == res)
        {
            std::cout << htmlBuffer << std::endl;
        }
        else {
            std::cout << "An error has occured, may be check the server in config.json" << std::endl;
        }
        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    return 0;
 }