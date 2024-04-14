#ifndef CVMACRO_H
#define CVMACRO_H
#pragma once

//#include "CvEnums.h"

// shortcut to looping enums
// Example: FOREACH(Yield) will loop all yields using the loop variable eLoopYield

#define FOREACH(TYPE) \
for (TYPE##Types eLoop##TYPE = (TYPE##Types)0; \
eLoop##TYPE < NUM_##TYPE##_TYPES; \
eLoop##TYPE=(TYPE##Types)(eLoop##TYPE + 1))

//TODO: Add static assert to provide a meaningful error msg for non-YieldTypes
#define FOREACH_CARGO_YIELD(TYPE) \
    for (TYPE##Types eLoop##TYPE = (TYPE##Types)FIRST_YIELD; \
    eLoop##TYPE < NUM_CARGO_YIELD_TYPES; \
    eLoop##TYPE=(TYPE##Types)(eLoop##TYPE + 1))

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define UNIQUE_VAR(base) TOKENPASTE2(base, __LINE__)

// Note: The first loop is needed to get the expansion right when using a unique variable. It does not result in nested iteration!
#define FOREACH_CITY(pLoopCity) \
    for (int UNIQUE_VAR(iLoop) = 0, _dummy_flag_ = 1; _dummy_flag_ && ((_dummy_flag_ = 0) || true);) \
    for (CvCity* pLoopCity = firstCity(&UNIQUE_VAR(iLoop)); pLoopCity != NULL; pLoopCity = nextCity(&UNIQUE_VAR(iLoop)))

#define FOREACH_CITY_OF_OWNER(pLoopCity, kOwner) \
    for (int UNIQUE_VAR(iLoop) = 0, _dummy_flag_ = 1; _dummy_flag_ && ((_dummy_flag_ = 0) || true);) \
    for (CvCity* pLoopCity = (kOwner).firstCity(&UNIQUE_VAR(iLoop)); pLoopCity != NULL; pLoopCity = (kOwner).nextCity(&UNIQUE_VAR(iLoop)))

#define FOREACH_UNIT(pLoopUnit) \
    for (int UNIQUE_VAR(iLoop) = 0, _dummy_flag_ = 1; _dummy_flag_ && ((_dummy_flag_ = 0) || true);) \
    for (CvUnit* pLoopUnit = firstUnit(&UNIQUE_VAR(iLoop)); pLoopUnit != NULL; pLoopUnit = nextUnit(&UNIQUE_VAR(iLoop)))

#define FOREACH_UNIT_OF_OWNER(pLoopUnit,kOwner) \
    for (int UNIQUE_VAR(iLoop) = 0, _dummy_flag_ = 1; _dummy_flag_ && ((_dummy_flag_ = 0) || true);) \
    for (CvUnit* pLoopUnit = (kOwner).firstUnit(&UNIQUE_VAR(iLoop)); pLoopUnit != NULL; pLoopUnit = (kOwner).nextUnit(&UNIQUE_VAR(iLoop)))

#endif
