#ifndef TOOLS_H_INCLUDED
#define TOOLS_H_INCLUDED

#include <vector>
#include <string>
#include "../includes/nlohmann/json.hpp"
using json = nlohmann::json;

bool replaceRegexWishBoundaries(std::string &text, const std::string &word, const std::string &replacement);
bool regexWishBoundaries(std::string &text, const std::string &word);
int arguments(std::vector<std::string> &args, std::string &cfgPath, bool &moderator, std::string const &v);
bool isEncryptedMessage(const std::string &str);
std::string urlEncode(const std::string &input);
int getWidth();
void oldCreateLine(char c, int size);
std::string createLineString(char c, int size = getWidth());
time_t getTimestamp(std::string t);
void replaceStringInPlace(std::string &s, const std::string &search, const std::string &replace);
std::string getValFromJSONSource(json &valuesource, std::string k);
void replacePrefixes(json &valuesource, std::string const prefix, std::string &s);
void escapeBackslash(std::string &s);
std::string jsonBackslash(std::string s);
char getCharacterFromEscapeSequence(const std::string &escapeSequence);
void replaceEscapeSequences(std::string &s);
std::vector<std::string> split(std::string s, std::string delimiter);

std::string const KEYNOTFOUND("-------------------------");
struct privategroup {
    bool found;
    std::string name;
    std::string key;
    bool isin;
    privategroup();
    privategroup(bool f, std::string n, std::string k, std::vector<std::string> v, bool is);
};

privategroup findPrivateGroup(json config, std::string name, bool isUserIn = false, std::string pseudo = "");

std::string escapeBourrinJson(const std::string &input);

std::string escapeJson(const std::string &input);

std::string stringCleaner(std::string toclean);

void charsCleaner(unsigned char *data, int const &sizeofdata, unsigned char *modifiedData);

std::string charsToStringCleaner(unsigned char *data, int const &sizeofdata);

std::string replaceDelimiters(std::string str, std::string const &lowerDelimiter, std::string const &upperDelimiter, std::string const &lowerReplacement, std::string const &upperReplacement) noexcept;

void delimitersOuterRanges(std::string &str, std::string const &lowerDelimiter, std::string const &upperDelimiter, std::vector<size_t[2]> ranges) noexcept;

void delimitersRanges(std::string &str, std::string const &lowerDelimiter, std::string const &upperDelimiter, std::vector<std::vector<size_t>> &ranges) noexcept;

std::string textFormatter(std::string str) noexcept;

#endif //TOOLS_H_INCLUDED