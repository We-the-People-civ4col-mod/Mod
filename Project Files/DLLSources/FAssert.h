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
struct AssertCallerData
{
	const char* file;
	const unsigned int line;
	const char* function;

	AssertCallerData(const char* fileAug, int lineAug, const char* functionAug)
		: file(fileAug)
		, line(lineAug)
		, function(functionAug)
	{}
};

#define CREATE_ASSERT_DATA AssertCallerData(__FILE__, __LINE__, __FUNCTION__)

#else
struct AssertCallerData {};
#define CREATE_ASSERT_DATA AssertCallerData()
#endif

#ifdef FASSERT_ENABLE

#ifdef WIN32

bool FAssertDlg(const char*, const char*, const char*, unsigned int,
	/*  advc.006f (from C2C): const char* param added. And changed the
	two locations below so that __FUNCTION__ is passed. */
	const char*,
	// add arguments to forward caller location
	const char*, unsigned int, const char*,
	bool&);

#define FAssert( expr )	\
{ \
	static bool bIgnoreAlways = false; \
	if( !bIgnoreAlways && !(expr) ) \
{ \
	if( FAssertDlg( #expr, 0, __FILE__, __LINE__, __FUNCTION__, NULL, 0, NULL, bIgnoreAlways ) ) \
{ _asm int 3 } \
} \
}

#define FAssertMsg( expr, msg ) \
{ \
	static bool bIgnoreAlways = false; \
	if( !bIgnoreAlways && !(expr) ) \
{ \
	if( FAssertDlg( #expr, msg, __FILE__, __LINE__, __FUNCTION__, NULL, 0, NULL, bIgnoreAlways ) ) \
{ _asm int 3 } \
} \
}

#define FAssertWithCaller( data, expr )	\
{ \
	static bool bIgnoreAlways = false; \
	if( !bIgnoreAlways && !(expr) ) \
{ \
	if( FAssertDlg( #expr, 0, __FILE__, __LINE__, __FUNCTION__, data.file, data.line, data.function, bIgnoreAlways ) ) \
{ _asm int 3 } \
} \
}

#define FAssertMsgWithCaller( data, expr, msg ) \
{ \
	static bool bIgnoreAlways = false; \
	if( !bIgnoreAlways && !(expr) ) \
{ \
	if( FAssertDlg( #expr, msg, __FILE__, __LINE__, __FUNCTION__, data.file, data.line, data.function, bIgnoreAlways ) ) \
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
// <advc.006>
#define FAssertBOOL(i) \
	FAssertBounds(0, 2, i); // </advc.006>
#else // FASSERT_ENABLE not defined - advc.006c: void(0) added.
// FASSERT_ENABLE not defined
#define FAssert(expr) (void)0
#define FAssertMsg(expr, msg) (void)0
// K-Mod:
#define FAssertBounds(lower,upper,index) (void)0
#define FAssertBOOL(i) void(0) // advc.006
#define FAssertWithCaller( data, expr ) (void)0
#define FAssertMsgWithCaller( data, expr, msg ) (void)0
#endif

#define FErrorMsg(msg) FAssertMsg(false, msg) // advc.006i (from C2C)

#endif // FASSERT_H
