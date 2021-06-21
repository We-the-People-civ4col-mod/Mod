#include "CvGameCoreDLL.h"

#ifndef CHECK_GLOBAL_CONSTANTS

CityPlotTypes NUM_CITY_PLOTS      = FIRST_CITY_PLOT;
CityPlotTypes CITY_PLOTS_DIAMETER = FIRST_CITY_PLOT;
CityPlotTypes CITY_PLOTS_RADIUS   = FIRST_CITY_PLOT;

ArtStyleTypes    NUM_ARTSTYLE_TYPES    = static_cast<ArtStyleTypes   >(0);
ColorTypes       NUM_COLOR_TYPES       = static_cast<ColorTypes      >(0);
PlayerColorTypes NUM_PLAYERCOLOR_TYPES = static_cast<PlayerColorTypes>(0);

bool GAME_IS_STARTING_UP = true;
#endif
