#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED

void showHelp(bool moderator = false);
int showReplying(MessageMemory &mem, int id, Message::messageSettings &msgs);
void clearScreen();

#endif // DISPLAY_H_INCLUDED