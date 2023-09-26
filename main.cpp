#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <fstream>
#include "includes/nlohmann/json.hpp"

using json = nlohmann::json;

int main(int argc, char const *argv[]) {
   CURL *curl;
   CURLcode res;
   std::ifstream f("./config.json");
   json data = json::parse(f);
   const std::string geturl = data["url"] + "msg.php?getmsg=json";

   curl = curl_easy_init();
   if(curl) {
     curl_easy_setopt(curl, CURLOPT_URL, geturl);
     res = curl_easy_perform(curl);
      /* always cleanup */
    curl_easy_cleanup(curl);
   }
   return 0;
 }