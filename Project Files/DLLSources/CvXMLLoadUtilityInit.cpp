//
// init/cleanup XML functions
//
#include "CvGameCoreDLL.h"
#include "CvXMLLoadUtility.h"
#include "CvGlobals.h"
#include "CvArtFileMgr.h"
#include "CvGameTextMgr.h"
#include "CvInfoWater.h"
#include "FProfiler.h"

// ignore type check for template functions
// no need to be strict in this file
DEFINE_ENUM_INT_COMPARISON(YieldTypes)

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   InitStringList(CvString **ppszList, int iListLen, CvString szString = "")
//
//  PURPOSE :   allocate and initialize a CvString pointer list of iListLen size to "", or
//				to szString if it is supplied
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::InitStringList(CvString **ppszList, int iListLen, CvString szString)
{
	PROFILE_FUNC();
	int i;	// loop counter
	CvString* pszList;

	FAssertMsg(*ppszList == NULL,"memory leak?");
	FAssertMsg((0 < iListLen),"list size to allocate is less than 1");
	// allocate memory for the boolean pointer based on the list length parameter
	*ppszList = new CvString[iListLen];
	// set the local pointer to the memory we just allocated
	pszList = *ppszList;

	// loop through all the booleans
	for (i=0;i<iListLen;i++)
	{
		// set the current boolean to false
		pszList[i] = szString;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   void Init2DIntList(int*** pppiList, int iSizeX, int iSizeY)
//
//  PURPOSE :   allocate and initialize a 2d array of int data
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::Init2DIntList(std::vector<int*>& aaiList, int iSizeX, int iSizeY)
{
	PROFILE_FUNC();
	FAssertMsg(((0 < iSizeX) && (0 < iSizeY)), "list size to allocate is less than 1");

	// allocate the memory for the array of pointers to arrays of ints
	aaiList.clear();
	aaiList.reserve(iSizeX);

	// loop through each of the pointers
	for (int i=0;i<iSizeX;i++)
	{
		// allocate a list of ints for the current pointer
		aaiList.push_back(new int[iSizeY]);
		// loop through all of the current pointer's ints
		for (int j=0;j<iSizeY;j++)
		{
			// set the current int to zero
			aaiList[i][j] = 0;
		}
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   InitImprovementBonusList(CvImprovementBonusInfo** ppImprovementBonus, int iListLen)
//
//  PURPOSE :   allocate a improvement bonus struct list of iListLen size and initialize it's members
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::InitImprovementBonusList(CvImprovementBonusInfo** ppImprovementBonus, int iListLen)
{
	// SPEEDUP
	PROFILE_FUNC();
	int i;	// loop counter
	CvImprovementBonusInfo* paImprovementBonus;

	FAssertMsg(*ppImprovementBonus == NULL,"memory leak?");
	FAssertMsg((0 < iListLen),"list size to allocate is less than 1");
	// allocate memory for the bonus type pointer based on the list length parameter
	*ppImprovementBonus = new CvImprovementBonusInfo[iListLen];
	// set the local pointer to the memory we just allocated
	paImprovementBonus = *ppImprovementBonus;

	// loop through all the bonus structs
	for (i=0;i<iListLen;i++)
	{
		paImprovementBonus[i].m_bBonusMakesValid = false;

		FAssertMsg(paImprovementBonus[i].m_aiYieldChange==NULL, "mem leak");
		paImprovementBonus[i].m_aiYieldChange = new int[NUM_YIELD_TYPES];
		for (int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			paImprovementBonus[i].m_aiYieldChange[j] = 0;
		}

		paImprovementBonus[i].m_iDiscoverRand = 0;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   InitBuildingDefaults(int **ppiDefaults)
//
//  PURPOSE :   allocate and initialize the civilization's default buildings
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::InitBuildingDefaults(int **ppiDefaults)
{
	// SPEEDUP
	PROFILE_FUNC();

	int i;
	int* piDefaults;

	FAssertMsg(*ppiDefaults == NULL,"memory leak?");
	// allocate memory based on the number of building classes
	*ppiDefaults = new int[GC.getNumBuildingClassInfos()];
	// set the local pointer to the new memory
	piDefaults = *ppiDefaults;

	// loop through all the pointers and set their default values
	for (i=0;i<GC.getNumBuildingClassInfos();i++)
	{
		piDefaults[i] = GC.getBuildingClassInfo((BuildingClassTypes) i).getDefaultBuildingIndex();
	}

}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   InitUnitDefaults(int **ppiDefaults)
//
//  PURPOSE :   allocate and initialize the civilization's default Units
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::InitUnitDefaults(int **ppiDefaults)
{
	// SPEEDUP
	PROFILE_FUNC();

	int i;
	int* piDefaults;

	FAssertMsg(*ppiDefaults == NULL,"memory leak?");
	// allocate memory based on the number of uniting classes
	*ppiDefaults = new int[GC.getNumUnitClassInfos()];
	// set the local pointer to the new memory
	piDefaults = *ppiDefaults;

	// loop through all the pointers and set their default values
	for (i=0;i<GC.getNumUnitClassInfos();i++)
	{
		piDefaults[i] = GC.getUnitClassInfo((UnitClassTypes) i).getDefaultUnitIndex();
	}
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CleanUpGlobals()
//
//  PURPOSE :   free the variables that are in globals.cpp/h
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::CleanUpGlobalVariables()
{
	GC.deleteInfoArrays();
}
