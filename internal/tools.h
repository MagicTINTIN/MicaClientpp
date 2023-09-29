/**
 * Clear the screen if system supports it
*/
void clearScreen();

void createLine(char c, int size);

void showHelp();

bool isEncryptedMessage(const std::string &str);

std::string urlEncode(const std::string &input);

int getWidth();

time_t getTimestamp(std::string t);
void ReplaceStringInPlace(std::string &s, const std::string &search, const std::string &replace);
void escapeBackslash(std::string &s);
char getCharacterFromEscapeSequence(const std::string &escapeSequence);
void replaceEscapeSequences(std::string &s);