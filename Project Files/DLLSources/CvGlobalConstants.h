#pragma once

#ifndef CIV4_GLOBAL_CONSTANTS_H
#define CIV4_GLOBAL_CONSTANTS_H

// This file is intended to set "defines", but instead of hardcoding, they are ints.
// This will allow setting up the data once and then use them like they were constants.
// The debugger will no longer have to follow GC for each call, which makes debugging more smooth and less annoying.
// A nice bonus is that we can add a hardcoding flag, allowing xml data to be hardcoded at compile time.

extern const bool& SAVEGAME_IS_LOADING;

#ifndef CHECK_GLOBAL_CONSTANTS
extern ArtStyleTypes NUM_ARTSTYLE_TYPES;

extern bool GAME_IS_STARTING_UP;

#else

enum
{
	GAME_IS_STARTING_UP,
};

#endif

#endif
