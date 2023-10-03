#include "colors.h"
#include <string>

std::string getColor(std::string color)
{
  if (color == "NORMAL")
        return NORMAL;

  if (color == "BOLD")
        return BOLD;

  if (color == "THIN")
        return THIN;

  if (color == "ITALIC")
        return ITALIC;

  if (color == "UNDERLINED")
        return UNDERLINED;

  if (color == "BLINK")
        return BLINK;

  if (color == "BLINK2")
        return BLINK2;

  if (color == "REVERSED")
        return REVERSED;

  if (color == "INVISIBLE")
        return INVISIBLE;

  if (color == "STRIKED")
        return STRIKED;


  if (color == "BLACK_NORMAL_COLOR")
        return BLACK_NORMAL_COLOR;

  if (color == "RED_NORMAL_COLOR")
        return RED_NORMAL_COLOR;

  if (color == "GREEN_NORMAL_COLOR")
        return GREEN_NORMAL_COLOR;

  if (color == "YELLOW_NORMAL_COLOR")
        return YELLOW_NORMAL_COLOR;

  if (color == "BLUE_NORMAL_COLOR")
        return BLUE_NORMAL_COLOR;

  if (color == "PURPLE_NORMAL_COLOR")
        return PURPLE_NORMAL_COLOR;

  if (color == "CYAN_NORMAL_COLOR")
        return CYAN_NORMAL_COLOR;

  if (color == "WHITE_NORMAL_COLOR")
        return WHITE_NORMAL_COLOR;

  if (color == "BLACK_NORMAL_BACKGROUND")
        return BLACK_NORMAL_BACKGROUND;

  if (color == "RED_NORMAL_BACKGROUND")
        return RED_NORMAL_BACKGROUND;

  if (color == "GREEN_NORMAL_BACKGROUND")
        return GREEN_NORMAL_BACKGROUND;

  if (color == "YELLOW_NORMAL_BACKGROUND")
        return YELLOW_NORMAL_BACKGROUND;

  if (color == "BLUE_NORMAL_BACKGROUND")
        return BLUE_NORMAL_BACKGROUND;

  if (color == "PURPLE_NORMAL_BACKGROUND")
        return PURPLE_NORMAL_BACKGROUND;

  if (color == "CYAN_NORMAL_BACKGROUND")
        return CYAN_NORMAL_BACKGROUND;

  if (color == "WHITE_NORMAL_BACKGROUND")
        return WHITE_NORMAL_BACKGROUND;

  if (color == "BLACK_DESAT_COLOR")
        return BLACK_DESAT_COLOR;

  if (color == "RED_DESAT_COLOR")
        return RED_DESAT_COLOR;

  if (color == "GREEN_DESAT_COLOR")
        return GREEN_DESAT_COLOR;

  if (color == "YELLOW_DESAT_COLOR")
        return YELLOW_DESAT_COLOR;

  if (color == "BLUE_DESAT_COLOR")
        return BLUE_DESAT_COLOR;

  if (color == "PURPLE_DESAT_COLOR")
        return PURPLE_DESAT_COLOR;

  if (color == "CYAN_DESAT_COLOR")
        return CYAN_DESAT_COLOR;

  if (color == "WHITE_DESAT_COLOR")
        return WHITE_DESAT_COLOR;

  if (color == "BLACK_DESAT_BACKGROUND")
        return BLACK_DESAT_BACKGROUND;

  if (color == "RED_DESAT_BACKGROUND")
        return RED_DESAT_BACKGROUND;

  if (color == "GREEN_DESAT_BACKGROUND")
        return GREEN_DESAT_BACKGROUND;

  if (color == "YELLOW_DESAT_BACKGROUND")
        return YELLOW_DESAT_BACKGROUND;

  if (color == "BLUE_DESAT_BACKGROUND")
        return BLUE_DESAT_BACKGROUND;

  if (color == "PURPLE_DESAT_BACKGROUND")
        return PURPLE_DESAT_BACKGROUND;

  if (color == "CYAN_DESAT_BACKGROUND")
        return CYAN_DESAT_BACKGROUND;

  if (color == "WHITE_DESAT_BACKGROUND")
        return WHITE_DESAT_BACKGROUND;

  return "";
}
