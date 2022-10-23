#pragma once

#ifndef FASSERT_H
#define FASSERT_H

// Only compile in FAssert's if FASSERT_ENABLE is defined.  By default, however, let's key off of
// _DEBUG.  Sometimes, however, it's useful to enable asserts in release builds, and you can do that
// simply by changing the following lines to define FASSERT_ENABLE or using project settings to override
#ifdef _DEBUG
#define FASSERT_ENABLE
#endif

#ifdef FASSERT_ENABLE

#ifdef WIN32

bool FAssertDlg(const char*, const char*, const char*, unsigned int,
	/*  advc.006f (from C2C): const char* param added. And changed the
	two locations below so that __FUNCTION__ is passed. */
	const char*, bool&);

#define FAssert( expr )	\
{ \
	static bool bIgnoreAlways = false; \
	if( !bIgnoreAlways && !(expr) ) \
{ \
	if( FAssertDlg( #expr, 0, __FILE__, __LINE__, __FUNCTION__, bIgnoreAlways ) ) \
{ _asm int 3 } \
} \
}

#define FAssertMsg( expr, msg ) \
{ \
	static bool bIgnoreAlways = false; \
	if( !bIgnoreAlways && !(expr) ) \
{ \
	if( FAssertDlg( #expr, msg, __FILE__, __LINE__, __FUNCTION__, bIgnoreAlways ) ) \
{ _asm int 3 } \
} \
}

#else
// Non Win32 platforms--just use built-in FAssert
#define FAssert( expr )	FAssert( expr )
#define FAssertMsg( expr, msg )	FAssert( expr )

#endif

// K-mod. moved the following macro from CvInitCore.h to here (and modified it)
// advc.006: Renamed from "FASSERT_BOUNDS", casts added
// advc.006f: fnString (function name) param removed
#define FAssertBounds(lower, upper, index) \
	if (static_cast<int>(index) < static_cast<int>(lower)) \
	{ \
		char acOut[256]; \
		snprintf(acOut, 256, "Index expected to be >= %d. (value: %d)", \
				static_cast<int>(lower), static_cast<int>(index)); \
		FAssertMsg(static_cast<int>(index) >= static_cast<int>(lower), acOut); \
	} \
	else if (static_cast<int>(index) >= static_cast<int>(upper)) \
	{ \
		char acOut[256]; \
		snprintf(acOut, 256, "Index expected to be < %d. (value: %d)", \
				static_cast<int>(upper), static_cast<int>(index)); \
		FAssertMsg(static_cast<int>(index) < static_cast<int>(upper), acOut); \
	}
// K-Mod end
#else
// FASSERT_ENABLE not defined
#define FAssert( expr )
#define FAssertMsg( expr, msg )
// K-Mod:
#define FAssertBounds(lower,upper,index) (void)0

#endif

#endif // FASSERT_H
