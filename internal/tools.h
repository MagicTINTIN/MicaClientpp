/**
 * Clear the screen if system supports it
*/
bool arguments(std::vector<std::string> &args, std::string &cfgPath);
void clearScreen();

void createLine();

void showHelp();

bool isEncryptedMessage(const std::string &str);

std::string urlEncode(const std::string &input);

int getWidth();

void oldCreateLine(char c, int size);

std::string createLineString(char c, int size);

time_t getTimestamp(std::string t);
void ReplaceStringInPlace(std::string &s, const std::string &search, const std::string &replace);
void escapeBackslash(std::string &s);
char getCharacterFromEscapeSequence(const std::string &escapeSequence);
void replaceEscapeSequences(std::string &s);