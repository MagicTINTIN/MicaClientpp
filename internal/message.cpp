#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include "../includes/nlohmann/json.hpp"
#include "message.h"

using json = nlohmann::json;

Message::Message() : sender(""), content(""), dateTime(""), certifiedUser(0), rank(0)
{}

Message::Message(std::string author, std::string message, std::string date, int certified, int rnk) : sender(author), content(message), dateTime(date), certifiedUser(certified), rank(rnk)
{}

void Message::print()
{
    std::cout << "[" << sender << "] " << content << std::endl;
}

void setAllMessages(json messages) {
    for (const auto &msg : messages.items())
	{
		
	}
}