#pragma once

#if defined(__APPLE__) || defined(__linux__)
//the following are UBUNTU/LINUX, and MacOS ONLY terminal color codes.
#define NORMAL "\e[0m"
#define BOLD "\e[1m"
#define THIN "\e[2m"
#define ITALIC "\e[3m"
#define UNDERLINED "\e[4m"
#define BLINK "\e[5m"
#define BLINK2 "\e[6m"
#define REVERSED "\e[7m"
#define INVISIBLE "\e[8m"
#define STRIKED "\e[9m"

#define BLACK_NORMAL_COLOR "\e[30m"
#define RED_NORMAL_COLOR "\e[31m"
#define GREEN_NORMAL_COLOR "\e[32m"
#define YELLOW_NORMAL_COLOR "\e[33m"
#define BLUE_NORMAL_COLOR "\e[34m"
#define PURPLE_NORMAL_COLOR "\e[35m"
#define CYAN_NORMAL_COLOR "\e[36m"
#define WHITE_NORMAL_COLOR "\e[37m"
#define BLACK_NORMAL_BACKGROUND "\e[40m"
#define RED_NORMAL_BACKGROUND "\e[41m"
#define GREEN_NORMAL_BACKGROUND "\e[42m"
#define YELLOW_NORMAL_BACKGROUND "\e[43m"
#define BLUE_NORMAL_BACKGROUND "\e[44m"
#define PURPLE_NORMAL_BACKGROUND "\e[45m"
#define CYAN_NORMAL_BACKGROUND "\e[46m"
#define WHITE_NORMAL_BACKGROUND "\e[47m"
#define BLACK_DESAT_COLOR "\e[90m"
#define RED_DESAT_COLOR "\e[91m"
#define GREEN_DESAT_COLOR "\e[92m"
#define YELLOW_DESAT_COLOR "\e[93m"
#define BLUE_DESAT_COLOR "\e[94m"
#define PURPLE_DESAT_COLOR "\e[95m"
#define CYAN_DESAT_COLOR "\e[96m"
#define WHITE_DESAT_COLOR "\e[97m"
#define BLACK_DESAT_BACKGROUND "\e[100m"
#define RED_DESAT_BACKGROUND "\e[101m"
#define GREEN_DESAT_BACKGROUND "\e[102m"
#define YELLOW_DESAT_BACKGROUND "\e[103m"
#define BLUE_DESAT_BACKGROUND "\e[104m"
#define PURPLE_DESAT_BACKGROUND "\e[105m"
#define CYAN_DESAT_BACKGROUND "\e[106m"
#define WHITE_DESAT_BACKGROUND "\e[107m"


// other os
#else

#define NORMAL_DESAT_BACKGROUND ""
#define BOLD_DESAT_BACKGROUND ""
#define THIN_DESAT_BACKGROUND ""
#define ITALIC_DESAT_BACKGROUND ""
#define UNDERLINED_DESAT_BACKGROUND ""
#define BLINK_DESAT_BACKGROUND ""
#define BLINK2_DESAT_BACKGROUND ""
#define REVERSED_DESAT_BACKGROUND ""
#define INVISIBLE_DESAT_BACKGROUND ""
#define STRIKED_DESAT_BACKGROUND ""

#define BLACK_NORMAL_COLOR ""
#define RED_NORMAL_COLOR ""
#define GREEN_NORMAL_COLOR ""
#define YELLOW_NORMAL_COLOR ""
#define BLUE_NORMAL_COLOR ""
#define PURPLE_NORMAL_COLOR ""
#define CYAN_NORMAL_COLOR ""
#define WHITE_NORMAL_COLOR ""
#define BLACK_NORMAL_BACKGROUND ""
#define RED_NORMAL_BACKGROUND ""
#define GREEN_NORMAL_BACKGROUND ""
#define YELLOW_NORMAL_BACKGROUND ""
#define BLUE_NORMAL_BACKGROUND ""
#define PURPLE_NORMAL_BACKGROUND ""
#define CYAN_NORMAL_BACKGROUND ""
#define WHITE_NORMAL_BACKGROUND ""
#define BLACK_DESAT_COLOR ""
#define RED_DESAT_COLOR ""
#define GREEN_DESAT_COLOR ""
#define YELLOW_DESAT_COLOR ""
#define BLUE_DESAT_COLOR ""
#define PURPLE_DESAT_COLOR ""
#define CYAN_DESAT_COLOR ""
#define WHITE_DESAT_COLOR ""
#define BLACK_DESAT_BACKGROUND ""
#define RED_DESAT_BACKGROUND ""
#define GREEN_DESAT_BACKGROUND ""
#define YELLOW_DESAT_BACKGROUND ""
#define BLUE_DESAT_BACKGROUND ""
#define PURPLE_DESAT_BACKGROUND ""
#define CYAN_DESAT_BACKGROUND ""
#define WHITE_DESAT_BACKGROUND ""



#endif