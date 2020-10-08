#ifndef CVMACRO_H
#define CVMACRO_H
#pragma once

// shortcut to looping enums
// Example: FOREACH(Yield) will loop all yields using the loop variable eLoopYield

#define FOREACH(TYPE) \
for (TYPE##Types eLoop##TYPE = (TYPE##Types)0; \
eLoop##TYPE < NUM_##TYPE##_TYPES; \
eLoop##TYPE=(TYPE##Types)(eLoop##TYPE + 1))

#endif
