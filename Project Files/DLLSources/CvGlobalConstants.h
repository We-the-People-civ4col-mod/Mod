#pragma once

#ifndef CIV4_GLOBAL_CONSTANTS_H
#define CIV4_GLOBAL_CONSTANTS_H

// This file is intended to set "defines", but instead of hardcoding, they are ints.
// This will allow setting up the data once and then use them like they were constants.
// The debugger will no longer have to follow GC for each call, which makes debugging more smooth and less annoying.
// A nice bonus is that we can add a hardcoding flag, allowing xml data to be hardcoded at compile time.


#ifndef CHECK_GLOBAL_CONSTANTS

extern int NUM_CITY_PLOTS;
extern int CITY_PLOTS_DIAMETER;
extern int CITY_PLOTS_RADIUS;

#else

// Add all values to the enum. If the game can compile with CHECK_GLOBAL_CONSTANTS set, then the variables won't be altered somewhere by mistake.
enum
{
	NUM_CITY_PLOTS,
	CITY_PLOTS_DIAMETER,
	CITY_PLOTS_RADIUS,
};

#endif

#endif
