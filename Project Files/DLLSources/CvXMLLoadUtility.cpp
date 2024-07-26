//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvXMLLoadUtility.cpp
//
//  AUTHOR:  Eric MacDonald  --  8/2003
//			Mustafa Thamer
//
//  PURPOSE: Group of functions to load in the xml files for Civilization 4
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2003 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvDLLXMLIFaceBase.h"
#include "CvXMLLoadUtility.h"
#include "CvGlobals.h"
#include "FProfiler.h"
#include "CvDLLUtilityIFaceBase.h"

static const int kBufSize = 2048;

//
// for logging
//
void CvXMLLoadUtility::logMsg(char* format, ... )
{
	static char buf[kBufSize];
	_vsnprintf( buf, kBufSize-4, format, (char*)(&format+1) );
	gDLL->logMsg("xml.log", buf);
}

bool CvXMLLoadUtility::CreateFXml()
{
	PROFILE_FUNC();
	try
	{
		m_pFXml = gDLL->getXMLIFace()->CreateFXml(m_pSchemaCache);
	}
	catch(...)
	{
		char	szMessage[512];
		sprintf( szMessage, "Caught unhandled exception creating XML parser object \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox( szMessage, "Loading Error" );
		return false;
	}
	return true;
}

void CvXMLLoadUtility::DestroyFXml()
{
	PROFILE_FUNC();
	gDLL->getXMLIFace()->DestroyFXml(m_pFXml);
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvXMLLoadUtility()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvXMLLoadUtility::CvXMLLoadUtility() :
m_iCurProgressStep(0),
m_pCBFxn(NULL),
m_pFXml(NULL)
{
	m_pSchemaCache = gDLL->getXMLIFace()->CreateFXmlSchemaCache();
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvXMLLoadUtility()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvXMLLoadUtility::~CvXMLLoadUtility(void)
{
	gDLL->getXMLIFace()->DestroyFXmlSchemaCache(m_pSchemaCache);
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ResetLandscapeInfo()
//
//  PURPOSE :  Clean up items for in-game reloading
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::ResetLandscapeInfo()
{
	for (int i = 0; i < GC.getNumLandscapeInfos(); ++i)
	{
		SAFE_DELETE(GC.getLandscapeInfo()[i]);
	}

	GC.getLandscapeInfo().clear();

	SetupGlobalLandscapeInfo();
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ResetGlobalEffectInfo()
//
//  PURPOSE :  Clean up items for in-game reloading
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::ResetGlobalEffectInfo()
{
	for (int i = 0; i < GC.getNumEffectInfos(); ++i)
	{
		SAFE_DELETE(GC.getEffectInfo()[i]);
	}

	GC.getEffectInfo().clear();

	LoadGlobalClassInfo(GC.getEffectInfo(), "CIV4EffectInfos", "Misc", "Civ4EffectInfos/EffectInfos/EffectInfo", NULL);
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   MakeMaskFromString(unsigned int *puiMask, char* szMask)
//
//  PURPOSE :   takes a string of hex digits, 0-f and converts it into an unsigned integer
//				mask value
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::MakeMaskFromString(unsigned int *puiMask, char* szMask)
{
	const int iLength = (int)strlen(szMask);

	// loop through each character in the szMask parameter
	for (int i = 0; i<iLength; i++)
	{
		// if the current character in the string is a zero
		if (szMask[i] == '0')
		{
			// shift the current value of the mask to the left by 4 bits
			*puiMask <<= 4;
			// making the last 4 bits of the mask 0000
		}
		// if the current character in the string is a zero
		else if (szMask[i] == '1')
		{
			// shift the current value of the mask to the left by 4 bits
			*puiMask <<= 4;
			// add 1 to the adjusted value of the mask
			// making the last 4 bits of the mask 0001
			*puiMask += 1;
		}
		// if the current character in the string is a two
		else if (szMask[i] == '2')
		{
			// shift the current value of the mask to the left by 4 bits
			*puiMask <<= 4;
			// add 2 to the adjusted value of the mask
			// making the last 4 bits of the mask 0010
			*puiMask += 2;
		}
		// if the current character in the string is a three
		else if (szMask[i] == '3')
		{
			// shift the current value of the mask to the left by 4 bits
			*puiMask <<= 4;
			// add 3 to the adjusted value of the mask
			// making the last 4 bits of the mask 0011
			*puiMask += 3;
		}
		// if the current character in the string is a four
		else if (szMask[i] == '4')
		{
			// shift the current value of the mask to the left by 4 bits
			*puiMask <<= 4;
			// add 4 to the adjusted value of the mask
			// making the last 4 bits of the mask 0100
			*puiMask += 4;
		}
		// if the current character in the string is a five
		else if (szMask[i] == '5')
		{
			// shift the current value of the mask to the left by 4 bits
			*puiMask <<= 4;
			// add 5 to the adjusted value of the mask
			// making the last 4 bits of the mask 0101
			*puiMask += 5;
		}
		// if the current character in the string is a six
		else if (szMask[i] == '6')
		{
			// shift the current value of the mask to the left by 4 bits
			*puiMask <<= 4;
			// add 6 to the adjusted value of the mask
			// making the last 4 bits of the mask 0110
			*puiMask += 6;
		}
		// if the current character in the string is a seven
		else if (szMask[i] == '7')
		{
			// shift the current value of the mask to the left by 4 bits
			*puiMask <<= 4;
			// add 7 to the adjusted value of the mask
			// making the last 4 bits of the mask 0111
			*puiMask += 7;
		}
		// if the current character in the string is a eight
		else if (szMask[i] == '8')
		{
			// shift the current value of the mask to the left by 4 bits
			*puiMask <<= 4;
			// add 8 to the adjusted value of the mask
			// making the last 4 bits of the mask 1000
			*puiMask += 8;
		}
		// if the current character in the string is a nine
		else if (szMask[i] == '9')
		{
			// shift the current value of the mask to the left by 4 bits
			*puiMask <<= 4;
			// add 9 to the adjusted value of the mask
			// making the last 4 bits of the mask 1001
			*puiMask += 9;
		}
		// if the current character in the string is a A, 10
		else if ((szMask[i] == 'a') || (szMask[i] == 'A'))
		{
			// shift the current value of the mask to the left by 4 bits
			*puiMask <<= 4;
			// add 10 to the adjusted value of the mask
			// making the last 4 bits of the mask 1010
			*puiMask += 10;
		}
		// if the current character in the string is a B, 11
		else if ((szMask[i] == 'b') || (szMask[i] == 'B'))
		{
			// shift the current value of the mask to the left by 4 bits
			*puiMask <<= 4;
			// add 11 to the adjusted value of the mask
			// making the last 4 bits of the mask 1011
			*puiMask += 11;
		}
		// if the current character in the string is a C, 12
		else if ((szMask[i] == 'c') || (szMask[i] == 'C'))
		{
			// shift the current value of the mask to the left by 4 bits
			*puiMask <<= 4;
			// add 12 to the adjusted value of the mask
			// making the last 4 bits of the mask 1100
			*puiMask += 12;
		}
		// if the current character in the string is a D, 13
		else if ((szMask[i] == 'd') || (szMask[i] == 'D'))
		{
			// shift the current value of the mask to the left by 4 bits
			*puiMask <<= 4;
			// add 13 to the adjusted value of the mask
			// making the last 4 bits of the mask 1101
			*puiMask += 13;
		}
		// if the current character in the string is a E, 14
		else if ((szMask[i] == 'd') || (szMask[i] == 'E'))
		{
			// shift the current value of the mask to the left by 4 bits
			*puiMask <<= 4;
			// add 14 to the adjusted value of the mask
			// making the last 4 bits of the mask 1110
			*puiMask += 14;
		}
		// if the current character in the string is a F, 15
		else if ((szMask[i] == 'f') || (szMask[i] == 'F'))
		{
			// shift the current value of the mask to the left by 4 bits
			*puiMask <<= 4;
			// add 15 to the adjusted value of the mask
			// making the last 4 bits of the mask 1111
			*puiMask += 15;
		}
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SkipToNextVal()
//
//  PURPOSE :   Go through the xml until we either reach a non-comment node or the end of the xml
//		returns false if it can't find one
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::SkipToNextVal()
{
	// we will loop through and skip over any comment nodes
	while (gDLL->getXMLIFace()->IsLastLocatedNodeCommentNode(m_pFXml))
	{
		// if we cannot set the current xml node to it's next sibling then we will break out of the for loop
		// otherwise we will continue looping
		if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
		{
			return false;	// couldn't find any non-comment nodes
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   FindInInfoClass(char const* pszVal, CvInfoBase* pInfos, int iClassSize, int iListLen)
//
//  PURPOSE :   check through the pszList parameter for the pszVal and returns the location a match
//				is found if one is found.
//				returns -1 if no match is found
//
//------------------------------------------------------------------------------------------------------
int CvXMLLoadUtility::FindInInfoClass(char const* pszVal, bool hideAssert)
{
	// R&R, ray, save performance without unnecessaray logging - START
	if (strcmp(pszVal,"NONE")==0 || strcmp(pszVal,"")==0)
	{
		return -1;
	}
	// R&R, ray, save performance without unnecessaray logging - END

	int idx = GC.getInfoTypeForString(pszVal, hideAssert);

	// if we found a match in the list we will return the value of the loop counter
	// which will hold the location of the match in the list
	if (idx != -1)
	{
		return idx;
	}

	if(!hideAssert)
	{
		// R&R, ray, save performance without unnecessaray logging - START
		//if (strcmp(pszVal,"NONE")!=0 && strcmp(pszVal,"")!=0)
		//{
			char errorMsg[1024];
			sprintf(errorMsg, "Tag: %s in Info class was incorrect \n Current XML file is: %s", pszVal, GC.getCurrentXMLFile().GetCString());
			gDLL->MessageBox(errorMsg, "XML Error");
		//}
		// R&R, ray, save performance without unnecessaray logging - END
	}

	return idx;
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   LoadCivXml(FXml* pFXml, char const* szFilename)
//
//  PURPOSE :   Gets the full pathname for the xml file from the FileManager .
//				If it is succesful we return true
//				from the function and a valid FXml pointer to the pFXml parameter.
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::LoadCivXml(FXml* pFXml, char const* szFilename)
{
	char szLog[256];
	sprintf(szLog, "LoadCivXml (%s)", szFilename);
	PROFILE(szLog);
	OutputDebugString(szLog);
	OutputDebugString("\n");

	CvString szPath = szFilename;
	CvString fsFilename = szFilename;

	if (!gDLL->fileManagerEnabled())
	{
		szPath = "Assets//" + szPath;
	}

	logMsg("Loading XML file %s\n", szPath.c_str());

	if (!gDLL->getXMLIFace()->LoadXml(pFXml, szPath))
	{
		logMsg("Load XML file %s FAILED\n", szPath.c_str());
		return false;
	}

	logMsg("Load XML file %s SUCCEEDED\n", szPath.c_str());
	GC.setCurrentXMLFile(szFilename);
	return true;	// success
}

bool CvXMLLoadUtility::SetStringList(CvString** ppszStringArray, int* piSize)
{
	int i;
	CvString* pszStringArray;

	FAssertMsg(*ppszStringArray == NULL, "Possible memory leak");
	*piSize = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);
	*ppszStringArray = NULL;
	if (*piSize > 0)
	{
		*ppszStringArray = new CvString[*piSize];
		pszStringArray = *ppszStringArray;
		if (GetChildXmlVal(pszStringArray[0]))
		{
			for (i=1;i<*piSize;i++)
			{
				if (!GetNextXmlVal(&(pszStringArray[i])))
				{
					break;
				}
			}

			gDLL->getXMLIFace()->SetToParent(m_pFXml);
		}
	}

	return true;
}

//
// call the progress updater fxn if it exists
//
void CvXMLLoadUtility::UpdateProgressCB(const char* szMessage)
{
	if (m_iCurProgressStep>GetNumProgressSteps())
	{
		m_iCurProgressStep=1;	// wrap
	}

	if (m_pCBFxn)
	{
		m_pCBFxn(++m_iCurProgressStep, GetNumProgressSteps(), CvString::format("Reading XML %s",
			szMessage ? szMessage : "").c_str());
	}
}

//
// use for fast lookup of children by name
//
void CvXMLLoadUtility::MapChildren()
{
	gDLL->getXMLIFace()->MapChildren(m_pFXml);
}
