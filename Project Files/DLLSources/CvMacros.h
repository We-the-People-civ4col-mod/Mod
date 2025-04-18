#ifndef CVMACRO_H
#define CVMACRO_H
#pragma once

// shortcut to looping enums
// Example: FOREACH(Yield) will loop all yields using the loop variable eLoopYield

#define FOREACH(TYPE) \
for (TYPE##Types eLoop##TYPE = (TYPE##Types)0; \
eLoop##TYPE < NUM_##TYPE##_TYPES; \
eLoop##TYPE=(TYPE##Types)(eLoop##TYPE + 1))

#define FOR_EACH_PLOT_IN_RANGE(iSearchRange, ACTION) \
    for (int iDX = -(iSearchRange); iDX <= (iSearchRange); iDX++) \
        for (int iDY = -(iSearchRange); iDY <= (iSearchRange); iDY++) \
        { \
            CvPlot* const pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY); \
            if (pLoopPlot != NULL) \
            { \
                ACTION; \
            } \
        }
#endif

#define FOREACH_PROFESSION(VAR, INFO)                                            \
for (ProfessionTypes VAR = FIRST_PROFESSION;                                     \
    VAR < NUM_PROFESSION_TYPES;                                                  \
    ++VAR)                                                                       \
    for (bool _once = true; _once; _once = false)                                \
        for (const CvProfessionInfo& INFO = GC.getProfessionInfo(VAR); _once; _once = false)

#define FOREACH_CITIZEN_PROFESSION(VAR,INFO)                                     \
for (ProfessionTypes VAR = FIRST_PROFESSION;                                     \
     VAR < NUM_PROFESSION_TYPES;                                                 \
     ++VAR)                                                                      \
    if (!GC.getProfessionInfo(VAR).isCitizen()) {} else                          \
        for (bool _once = true; _once; _once = false)                            \
            for (const CvProfessionInfo& INFO = GC.getProfessionInfo(VAR); _once; _once = false)
