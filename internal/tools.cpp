#include <stdio.h>
#include <stdlib.h>
#include "tools.h"

void clearScreen() {
    #ifdef _WIN32
        // Windows
        system("cls");
    #elif __linux__
        // Linux
        system("clear");
    #else
        // Other or unknown OS
        printf("____________________________________\n");
        printf("Unsupported operating system.\n");
    #endif
}