#ifndef TOOLS_H_INCLUDED
#define TOOLS_H_INCLUDED

#include <vector>
#include <string>
#include "../includes/nlohmann/json.hpp"
using json = nlohmann::json;

bool regexWishBoundaries(std::string &text, const std::string &word, const std::string &replacement);
bool arguments(std::vector<std::string> &args, std::string &cfgPath, bool &moderator);
bool isEncryptedMessage(const std::string &str);
std::string urlEncode(const std::string &input);
int getWidth();
void oldCreateLine(char c, int size);
std::string createLineString(char c, int size = getWidth());
time_t getTimestamp(std::string t);
void ReplaceStringInPlace(std::string &s, const std::string &search, const std::string &replace);
void escapeBackslash(std::string &s);
char getCharacterFromEscapeSequence(const std::string &escapeSequence);
void replaceEscapeSequences(std::string &s);
std::vector<std::string> split(std::string s, std::string delimiter);


struct privategroup {
    bool found;
    std::string name;
    std::string key;
    privategroup();
    privategroup(bool f, std::string n, std::string k, std::vector<std::string> v);
};

privategroup findPrivateGroup(json config, std::string name);

#endif //TOOLS_H_INCLUDED