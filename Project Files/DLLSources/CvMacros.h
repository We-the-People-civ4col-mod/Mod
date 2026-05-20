#ifndef CVMACRO_H
#define CVMACRO_H
#pragma once

// Helper for creating unique iteration variable
#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define UNIQUE_VAR(base) TOKENPASTE2(base, __LINE__)

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

#define FOR_EACH_GROUPAI_VAR(pGroup, kOwner) \
    for (int UNIQUE_VAR(iLoop) = 0, _dummy_flag_ = 1; _dummy_flag_ && ((_dummy_flag_ = 0) || true);) \
    for (CvSelectionGroupAI* pGroup = static_cast<CvSelectionGroupAI*>((kOwner).firstSelectionGroup(&UNIQUE_VAR(iLoop))); \
         pGroup != NULL; pGroup = static_cast<CvSelectionGroupAI*>((kOwner).nextSelectionGroup(&UNIQUE_VAR(iLoop))))
