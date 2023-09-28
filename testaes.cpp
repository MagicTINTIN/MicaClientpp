#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <fstream>
#include "includes/nlohmann/json.hpp"
#include "internal/message.h"
#include "internal/messagememory.h"
#include "internal/aes.h"

int main(int argc, char const *argv[])
{
    unsigned char text[256] = "This is a text !";
    unsigned char key[17] = "thisIsTheKey";
    unsigned char output[512 + 30];

    std::string action = "both";

    if (argc < 4 || argc > 4)
    {
        std::cout << "Invalid number of arguments" << std::endl
                  << "Please give 1 text and 1 key only + encrypt/decrypt/both" << std::endl
                  << std::endl
                  << "Using default testing settings to encrypt and decrypt" << std::endl
                  << "text=\"This is a text !\", key=\"thisIsTheKey\"" << std::endl;
    }
    else
    {
        strcpy( (char*) text, argv[1] );
        strcpy( (char*) key, argv[2] );
        if (std::strcmp(argv[3], "encrypt") == 0 || std::strcmp(argv[3], "decrypt") == 0 || std::strcmp(argv[3], "both") == 0)
            action = argv[3];
        else {
            action = "both";
        }
    }
    if (action == "encrypt" || action == "both")
    {
        std::cout << "Encrypting : \"" << text << "\"" << std::endl;
        AES(text, key, output);
        std::cout << "Encrypted : \"" << output << "\"" << std::endl;
    }
    if (action == "decrypt")
    {
        std::cout << "Decrypting : \"" << text << "\"" << std::endl;
        inv_AES(text, key, output);
        std::cout << "Decrypted : \"" << output << "\"" << std::endl;
    }
    if (action == "both")
    {
        std::cout << "Decrypting : \"" << output << "\"" << std::endl;
        inv_AES(output, key, text);
        std::cout << "Decrypted : \"" << text << "\"" << std::endl;
    }
    return 0;
}
