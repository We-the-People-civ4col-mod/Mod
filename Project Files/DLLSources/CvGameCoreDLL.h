#pragma once

#ifndef CvGameCoreDLL_h
#define CvGameCoreDLL_h

//
// includes (pch) for gamecore dll files
// Author - Mustafa Thamer
//

//
// WINDOWS
//
#pragma warning( disable: 4530 )	// C++ exception handler used, but unwind semantics are not enabled

// enable extra warnings
#pragma warning( 3: 4701 ) // local variable used without being initialized

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <MMSystem.h>
#if defined _DEBUG && !defined USE_MEMMANAGER
//#define USE_MEMMANAGER
#include <crtdbg.h>
#endif
#include <vector>
#include <list>
#include <tchar.h>
#include <math.h>
#include <assert.h>
#include <map>
#include <hash_map>
#include <set>
#include <deque>

#define DllExport   __declspec( dllexport )

// The makefile use version 1310 to compile
// VC use never versions for IntelliSense 
// Used to avoid IntelliSense spamming bogus errors
#if _MSC_VER == 1310
# define MakefileCompilation
#endif


//
// GameBryo
//
class NiColor
{
public:
	float r, g, b;
};
class NiColorA
{
public:
	NiColorA(float fr, float fg, float fb, float fa) : r(fr), g(fg), b(fb), a(fa) {}
	NiColorA() {}
	float r, g, b, a;
};
class NiPoint2
{
public:
	NiPoint2() {}
	NiPoint2(float fx, float fy) : x(fx),y(fy) {}

	float x, y;
};
class NiPoint3
{
public:
	NiPoint3() {}
	NiPoint3(float fx, float fy, float fz) : x(fx),y(fy),z(fz) {}

	bool NiPoint3::operator== (const NiPoint3& pt) const
	{	return (x == pt.x && y == pt.y && z == pt.z);	}

	inline NiPoint3 NiPoint3::operator+ (const NiPoint3& pt) const
	{	return NiPoint3(x+pt.x,y+pt.y,z+pt.z);	}

	inline NiPoint3 NiPoint3::operator- (const NiPoint3& pt) const
	{	return NiPoint3(x-pt.x,y-pt.y,z-pt.z);	}

	inline float NiPoint3::operator* (const NiPoint3& pt) const
	{	return x*pt.x+y*pt.y+z*pt.z;	}

	inline NiPoint3 NiPoint3::operator* (float fScalar) const
	{	return NiPoint3(fScalar*x,fScalar*y,fScalar*z);	}

	inline NiPoint3 NiPoint3::operator/ (float fScalar) const
	{
		float fInvScalar = 1.0f/fScalar;
		return NiPoint3(fInvScalar*x,fInvScalar*y,fInvScalar*z);
	}

	inline NiPoint3 NiPoint3::operator- () const
	{	return NiPoint3(-x,-y,-z);	}

	inline float Length() const
	{ return sqrt(x * x + y * y + z * z); }

	inline float Unitize()
	{
		float length = Length();
		if(length != 0)
		{
			x /= length;
			y /= length;
			z /= length;
		}
		return length;
	}

//	inline NiPoint3 operator* (float fScalar, const NiPoint3& pt)
//	{	return NiPoint3(fScalar*pt.x,fScalar*pt.y,fScalar*pt.z);	}
	float x, y, z;
};

namespace NiAnimationKey
{
	enum KeyType
	{
		NOINTERP,
		LINKEY,
		BEZKEY,
		TCBKEY,
		EULERKEY,
		STEPKEY,
		NUMKEYTYPES
	};
};

typedef unsigned char    byte;
typedef unsigned short   word;
typedef unsigned int     uint;
typedef unsigned long    dword;
typedef unsigned __int64 qword;
typedef wchar_t          wchar;

#define MAX_CHAR                            (0x7f)
#define MIN_CHAR                            (0x80)
#define MAX_SHORT                           (0x7fff)
#define MIN_SHORT                           (0x8000)
#define MAX_INT                             (0x7fffffff)
#define MIN_INT                             (0x80000000)
#define MAX_UNSIGNED_CHAR                   (0xff)
#define MIN_UNSIGNED_CHAR                   (0x00)
#define MAX_UNSIGNED_SHORT                  (0xffff)
#define MIN_UNSIGNED_SHORT                  (0x0000)
#define MAX_UNSIGNED_INT                    (0xffffffff)
#define MIN_UNSIGNED_INT                    (0x00000000)

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#define SQR(x)      ( (x) * (x) )
#define DEGTORAD(x) ( (float)( (x) * (M_PI / 180) ))
#define LIMIT_RANGE(low, value, high) value = (value < low ? low : (value > high ? high : value));
#define M_PI       3.14159265358979323846
#define fM_PI		3.141592654f		//!< Pi (float)

__forceinline DWORD FtoDW( float f ) { return *(DWORD*)&f; }
__forceinline float DWtoF( dword n ) { return *(float*)&n; }
__forceinline float MaxFloat() { return DWtoF(0x7f7fffff); }

/// bitmap - start - Nightinggale
// variableless versions assuming the argument to be 0
// useful for enums
#define SETBIT( x ) (1 << x)
#define SETBITS( numBits, firstBit ) (((1 << numBits) - 1 ) << firstBit)

#define GETBIT ( x, y ) ((x >> y) & 1)
#define GETBITS( x, y, z ) ((x >> y) & ((1 << z) - 1 ))

template <typename T>
static inline bool HasBit(const T x, const int y)
{
	return (x & ((T)1U << y)) != 0;
}

template <typename T>
static inline T SetBit(T &x, const int y)
{
	return x = (T)(x | ((T)1U << y));
}

template <typename T>
static inline T ClrBit(T &x, const int y)
{
	return x = (T)(x & ~((T)1U << y));
}

// use one of the previous functions instead of hardcoding bValue
// this function is only for cases where a bit can be turned both on and off
template <typename T>
static inline T SetBit(T &x, const int y, const bool bValue)
{
	if (bValue)
	{
		return x = (T)(x | ((T)1U << y));
	}
	else {
		return x = (T)(x & ~((T)1U << y));
	}
}

// use both has and get prefix as both fits and remembering both appears to be a pain
template <typename T>
static inline T GetBits(T &x, const int iIndex, const int iNumBits)
{
	return (x >> iIndex) & (((T)1u << iNumBits) - 1);
}

template <typename T>
static inline T HasBits(T &x, const int iIndex, const int iNumBits)
{
	return GetBits(x, iIndex, iNumBits);
}

template <typename T>
static inline T SetBits(T &x, const int iIndex, const int iNumBits, const T iValue)
{
	x &= ~(((1 << iNumBits) - 1) << iIndex);
	x |= (iValue & ((1 << iNumBits) - 1)) << iIndex;
	return x;
}

/// bitmap - end - Nightinggale

//
// Boost Python
//
#ifdef MakefileCompilation
# include <boost/python/list.hpp>
# include <boost/python/tuple.hpp>
# include <boost/python/class.hpp>
# include <boost/python/manage_new_object.hpp>
# include <boost/python/return_value_policy.hpp>
# include <boost/python/object.hpp>
# include <boost/python/def.hpp>

namespace python = boost::python;

#else

// write some garbage code to kill IntelliSense errors
// the makefile compiler will never see this

#define BOOST_STATIC_ASSERT(x)
namespace python
{
	class tuple;
}
class PyObject;

namespace boost
{
	class noncopyable {};

	namespace detail
	{
		template< typename T1 >
		struct is_same_part_1
		{
			template<typename T2>  struct part_2 { enum { value = false }; };
			template<>             struct part_2<T1> { enum { value = true }; };
		};
	}

	template< typename T1, typename T2 >
	struct is_same
	{
		enum { value = detail::is_same_part_1<T1>::template part_2<T2>::value };
	};
}

#endif // IntelliSense workaround

#include "CvMacros.h"
#include "FAssert.h"
#include "CvGameCoreDLLDefNew.h"
#include "FDataStreamBase.h"
#include "FFreeListArrayBase.h"
#include "FFreeListTrashArray.h"
#include "FFreeListArray.h"
#include "CvIdVector.h"
//#include "FVariableSystem.h"
#include "CvString.h"
#include "CvEnums.h"
#include "CvStructs.h"
#include "NetworkDataPacking.h"
#include "CvDLLUtilityIFaceBase.h"

#include "CvGlobalConstants.h"
#include "CvEnumsFunctions.h"

#include "JustInTimeArray.h"
#include "BoolArray.h"
#include "PlayerArray.h"
#include "PlayerBoolArray.h"
#include "InfoArray.h"
#include "InfoCacheArray.h"




#include "EnumMap.h"
#include "EnumMap2D.h"

#include "CvGlobals.h"

#include "CvPlayerAI.h"
#include "CvGameCoreUtils.h"
#include "CvMap.h"
#include "CvGameAI.h"
#include "CvPlot.h"
#include "CvUnit.h"
#include "CvCity.h"
#include "FProfiler.h"
#include "CyCity.h"
#include "CvInfos.h"
#include "CvTeamAI.h"
#include "CvDLLPythonIFaceBase.h"
#include "CvRandom.h"
#include "CvArea.h"
#include "CvDllEntity.h"
#include "CvDeal.h"
#include "CvDLLEntityIFaceBase.h"
#include "CvGame.h"
#include "CyGlobalContext.h"
#include "CvSelectionGroup.h"
#include "CvTalkingHeadMessage.h"
#include "CvCityAI.h"
#include "CvSelectionGroupAI.h"
#include "CvUnitAI.h"

#ifdef FINAL_RELEASE
// Undefine OutputDebugString in final release builds
#undef OutputDebugString
#define OutputDebugString(x)
#endif //FINAL_RELEASE

#endif	// CvGameCoreDLL_h
