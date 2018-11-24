#ifndef civ4iconv
#define civ4iconv

// Implementing a full iconv feature is overkill for our need.
// Instead this file will act as the gateway to the part of iconv, which we need.

// First task is to define whatever variables and constants needed to include unmodified tables.

typedef unsigned int conv_t;
typedef unsigned int ucs4_t;

enum
{
	RET_ILUNI,
	RET_ILSEQ,
};

// Include the actual conversion tables.

#include "cp1250.h"
#include "cp1251.h"
#include "cp1252.h"

#endif // civ4iconv