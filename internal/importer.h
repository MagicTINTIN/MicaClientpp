#pragma once
#ifndef IMPORTER_H_INCLUDED
#define IMPORTER_H_INCLUDED

int themeImporter(int const &THEME_VERSION, json &themeData, json &data);
int languageImporter(int const &LANGUAGE_VERSION, json &languageData, json &data);
int backupImporter(MessageMemory::memorySettings &memsettings, MessageMemory &mem, std::string &cfgPath);
int configImporter(int const &MCPP_VERSION, int const &CONFIG_VERSION, int const &MSG_MAX_SIZE, std::string &cfgFilePath, json &data, bool &encryptenabled, bool &moderatormode, std::string &genkey, std::string &serverurl, std::string &username, std::string &token, std::string &cfgPath, Message::messageSettings &msgsettings, MessageMemory::memorySettings &memsettings);

#endif //IMPORTER_H_INCLUDED