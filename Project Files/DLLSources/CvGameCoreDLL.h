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
#include <Windows.h>
#include <MMSystem.h>
#if defined _DEBUG && !defined USE_MEMMANAGER
//#define USE_MEMMANAGER
//#include <crtdbg.h>
#endif
#include <vector>
#include <list>
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

// Unused and disabled for now to avoid cppcheck complaining
/*
__forceinline DWORD FtoDW( float f ) { return *(DWORD*)&f; }
__forceinline float DWtoF( dword n ) { return *(float*)&n; }
__forceinline float MaxFloat() { return DWtoF(0x7f7fffff); }
*/

//
// Boost Python
//
#ifdef MakefileCompilation
#include <boost/graph/detail/is_same.hpp>
#include <boost/noncopyable.hpp>
#include <boost/static_assert.hpp>
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

// setting this to false will provide the path to the exe dir instead
// this is usually not needed as it's the working directory
std::string GetDLLPath(bool bLoadDLLPath = true);


#include "NiColorA.h"
#include "NiPoint2.h"
#include "NiPoint3.h"
#include "NiAnimationKey.h"

#include "BitFunctions.h"

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
#include "AlertWindow.h"

#include "CvGlobalConstants.h"
#include "CvEnumsFunctions.h"
#include "EnumMapSpecialization.h"

#include "JustInTimeArray.h"
#include "InfoArray.h"
#include "EnumMap.h"

#include "CvGlobals.h"

#include "CvPlayerAI.h"
#include "CvGameCoreUtils.h"
#include "CvMap.h"
#include "CvGameAI.h"
#include "CvPlot.h"
#include "CvUnit.h"
#include "CvCity.h"
#include "FProfiler.h"
#include "CvInfos.h"
#include "CvTeamAI.h"
#include "CvDLLPythonIFaceBase.h"
#include "CvRandom.h"
#include "CvArea.h"
#include "CvDLLEntity.h"
#include "CvDeal.h"
#include "CvDLLEntityIFaceBase.h"
#include "CvGame.h"
#include "CvArtFileMgr.h"
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
