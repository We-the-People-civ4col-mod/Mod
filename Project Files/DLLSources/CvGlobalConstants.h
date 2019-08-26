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

// enum xml types, which can be switched to ints, hence allowing dll files with less xml hardcoding
// if hardcoding is being used, then the values will be set in AutoXmlEnum.h
// As a result only the runtime version should be present here

#ifndef HARDCODE_XML_VALUES
extern YieldTypes YIELD_FOOD;
extern YieldTypes YIELD_LUMBER;
extern YieldTypes YIELD_STONE;
extern YieldTypes YIELD_ORE;
extern YieldTypes YIELD_HORSES;
extern YieldTypes YIELD_MUSKETS;
extern YieldTypes YIELD_TOOLS;
extern YieldTypes YIELD_BLADES;
extern YieldTypes YIELD_CANNONS;
extern YieldTypes YIELD_FUR;

extern YieldTypes YIELD_HAMMERS;
extern YieldTypes YIELD_BELLS;
extern YieldTypes YIELD_CROSSES;
extern YieldTypes YIELD_CULTURE;
extern YieldTypes YIELD_HEALTH;
extern YieldTypes YIELD_EDUCATION;

extern YieldTypes NUM_CARGO_YIELD_TYPES;
#endif

#endif
