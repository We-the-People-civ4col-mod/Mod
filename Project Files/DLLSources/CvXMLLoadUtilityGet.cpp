//
// XML Get functions
//

#include "CvGameCoreDLL.h"
#include "CvDLLXMLIFaceBase.h"
#include "CvXMLLoadUtility.h"
#include "CvGlobals.h"
#include "CvArtFileMgr.h"
#include "FInputDevice.h"
#include "FProfiler.h"

//
// STATIC
// for progress bar display
// returns the number of steps we use
//
int CvXMLLoadUtility::GetNumProgressSteps()
{
	return 20;	// the function UpdateProgressCB() is called 20 times by CvXMLLoadUtilitySet
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetXmlVal(char* pszVal, char* pszDefault = NULL)
//
//  PURPOSE :   Get the string value of the current xml node or the next non-comment xml node if the
//				current node is a comment node
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetXmlVal(char* pszVal, char* pszDefault)
{
	if (pszDefault)
	{
		strcpy(pszVal, pszDefault);
	}
	else
	{
		strcpy(pszVal, "");
	}

	// skip to the next non-comment node
	if (SkipToNextVal())
	{
		// get the string value of the current xml node
		gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,pszVal);
		return true;
	}
	// otherwise we can't find a non-comment node on this level so we will FAssert and return false
	else
	{
		FAssertMsg(false , "Error in GetXmlVal function, unable to find the next non-comment node");
		return false;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetXmlVal(wchar* pszVal, wchar* pszDefault = NULL)
//
//  PURPOSE :   Get the string value of the current xml node or the next non-comment xml node if the
//				current node is a comment node
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetXmlVal(wchar* pszVal, wchar* pszDefault)
{
	if (pszDefault)
	{
		wcscpy(pszVal, pszDefault);
	}
	else
	{
		wcscpy(pszVal, L"");
	}

	// skip to the next non-comment node
	if (SkipToNextVal())
	{
		// get the string value of the current xml node
		gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,pszVal);
		return true;
	}
	// otherwise we can't find a non-comment node on this level so we will FAssert and return false
	else
	{
		FAssertMsg(false , "Error in GetXmlVal function, unable to find the next non-comment node");
		return false;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetXmlVal(std::string& pszVal, char* pszDefault = NULL)
//
//  PURPOSE :   Get the string value of the current xml node or the next non-comment xml node if the
//				current node is a comment node
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetXmlVal(std::string& pszVal, char* pszDefault)
{
	if (pszDefault)
	{
		pszVal = pszDefault;
	}
	else
	{
		pszVal.clear();
	}

	// skip to the next non-comment node
	if (SkipToNextVal())
	{
		// get the string value of the current xml node
		gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,pszVal);
		return true;
	}
	// otherwise we can't find a non-comment node on this level so we will FAssert and return false
	else
	{
		FAssertMsg(false , "Error in GetXmlVal function, unable to find the next non-comment node");
		return false;
	}
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetXmlVal(std::wstring& pszVal, wchar* pszDefault = NULL)
//
//  PURPOSE :   Get the string value of the current xml node or the next non-comment xml node if the
//				current node is a comment node
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetXmlVal(std::wstring& pszVal, wchar* pszDefault)
{
	if (pszDefault)
	{
		pszVal = pszDefault;
	}
	else
	{
		pszVal.clear();
	}

	// skip to the next non-comment node
	if (SkipToNextVal())
	{
		// get the string value of the current xml node
		gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,pszVal);
		return true;
	}
	// otherwise we can't find a non-comment node on this level so we will FAssert and return false
	else
	{
		FAssertMsg(false , "Error in GetXmlVal function, unable to find the next non-comment node");
		return false;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetXmlVal(int* piVal, int iDefault = 0)
//
//  PURPOSE :   Get the int value of the current xml node or the next non-comment xml node if the
//				current node is a comment node
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetXmlVal(int* piVal, int iDefault)
{
	// set the value to the default
	*piVal = iDefault;

	// skip to the next non-comment node
	if (SkipToNextVal())
	{
		// get the string value of the current xml node
		gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,piVal);
		return true;
	}
	// otherwise we can't find a non-comment node on this level so we will FAssert and return false
	else
	{
		FAssertMsg(false , "Error in GetXmlVal function, unable to find the next non-comment node");
		return false;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetXmlVal(float* pfVal, float fDefault = 0.0f)
//
//  PURPOSE :   Get the float value of the current xml node or the next non-comment xml node if the
//				current node is a comment node
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetXmlVal(float* pfVal, float fDefault)
{
	// set the value to the default
	*pfVal = fDefault;

	// skip to the next non-comment node
	if (SkipToNextVal())
	{
		// get the string value of the current xml node
		gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,pfVal);
		return true;
	}
	// otherwise we can't find a non-comment node on this level so we will FAssert and return false
	else
	{
		FAssertMsg(false , "Error in GetXmlVal function, unable to find the next non-comment node");
		return false;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetXmlVal(bool* pbVal, bool bDefault = false)
//
//  PURPOSE :   Get the boolean value of the current xml node or the next non-comment xml node if the
//				current node is a comment node
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetXmlVal(bool* pbVal, bool bDefault)
{
	// set the boolean value to it's default value
	*pbVal = bDefault;

	// skip to the next non-comment node
	if (SkipToNextVal())
	{
		// get the string value of the current xml node
		gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,pbVal);
		return true;
	}
	// otherwise we can't find a non-comment node on this level so we will FAssert and return false
	else
	{
		FAssertMsg(false , "Error in GetXmlVal function, unable to find the next non-comment node");
		return false;
	}
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetNextXmlVal(std::string& pszVal, char* pszDefault = NULL)
//
//  PURPOSE :   Get the string value of the next sibling of the current xml node or the next
//				non-comment xml node if the current node is a comment node
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetNextXmlVal(std::string* pszVal, char* pszDefault)
{
	if (pszDefault)
	{
		*pszVal = pszDefault;
	}
	else
	{
		(*pszVal).clear();
	}

	// if we can set the current xml node to it's next sibling
	if (gDLL->getXMLIFace()->NextSibling(m_pFXml))
	{
		// skip to the next non-comment node
		if (SkipToNextVal())
		{
			// get the string value of the current xml node
			gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml, *pszVal);
			return true;
		}
		// otherwise we can't find a non-comment node on this level so we will FAssert and return false
		else
		{
			FAssertMsg(false , "Error in GetNextXmlVal function, unable to find the next non-comment node");
			return false;
		}
	}
	// otherwise there are no more sibling nodes but we were expecting them so FAssert and return false
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetNextXmlVal(std::wstring& pszVal, wchar* pszDefault = NULL)
//
//  PURPOSE :   Get the string value of the next sibling of the current xml node or the next
//				non-comment xml node if the current node is a comment node
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetNextXmlVal(std::wstring* pszVal, wchar* pszDefault)
{
	if (pszDefault)
	{
		*pszVal = pszDefault;
	}
	else
	{
		(*pszVal).clear();
	}

	// if we can set the current xml node to it's next sibling
	if (gDLL->getXMLIFace()->NextSibling(m_pFXml))
	{
		// skip to the next non-comment node
		if (SkipToNextVal())
		{
			// get the string value of the current xml node
			gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml, *pszVal);
			return true;
		}
		// otherwise we can't find a non-comment node on this level so we will FAssert and return false
		else
		{
			FAssertMsg(false , "Error in GetNextXmlVal function, unable to find the next non-comment node");
			return false;
		}
	}
	// otherwise there are no more sibling nodes but we were expecting them so FAssert and return false
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetNextXmlVal(char* pszVal, char* pszDefault = NULL)
//
//  PURPOSE :   Get the string value of the next sibling of the current xml node or the next
//				non-comment xml node if the current node is a comment node
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetNextXmlVal(char* pszVal, char* pszDefault)
{
	if (pszDefault)
	{
		strcpy(pszVal, pszDefault);
	}
	else
	{
		strcpy(pszVal, "");
	}

	// if we can set the current xml node to it's next sibling
	if (gDLL->getXMLIFace()->NextSibling(m_pFXml))
	{
		// skip to the next non-comment node
		if (SkipToNextVal())
		{
			// get the string value of the current xml node
			gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,pszVal);
			return true;
		}
		// otherwise we can't find a non-comment node on this level so we will FAssert and return false
		else
		{
			FAssertMsg(false , "Error in GetNextXmlVal function, unable to find the next non-comment node");
			return false;
		}
	}
	// otherwise there are no more sibling nodes but we were expecting them so FAssert and return false
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetNextXmlVal(wchar* pszVal, wchar* pszDefault = NULL)
//
//  PURPOSE :   Get the string value of the next sibling of the current xml node or the next
//				non-comment xml node if the current node is a comment node
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetNextXmlVal(wchar* pszVal, wchar* pszDefault)
{
	if (pszDefault)
	{
		wcscpy(pszVal, pszDefault);
	}
	else
	{
		wcscpy(pszVal, L"");
	}

	// if we can set the current xml node to it's next sibling
	if (gDLL->getXMLIFace()->NextSibling(m_pFXml))
	{
		// skip to the next non-comment node
		if (SkipToNextVal())
		{
			// get the string value of the current xml node
			gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,pszVal);
			return true;
		}
		// otherwise we can't find a non-comment node on this level so we will FAssert and return false
		else
		{
			FAssertMsg(false , "Error in GetNextXmlVal function, unable to find the next non-comment node");
			return false;
		}
	}
	// otherwise there are no more sibling nodes but we were expecting them so FAssert and return false
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetNextXmlVal(int* piVal, int iDefault = 0)
//
//  PURPOSE :   Get the int value of the next sibling of the current xml node or the next
//				non-comment xml node if the current node is a comment node
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetNextXmlVal(int* piVal, int iDefault)
{
	// set the value to the default
	*piVal = iDefault;

	// if we can set the current xml node to it's next sibling
	if (gDLL->getXMLIFace()->NextSibling(m_pFXml))
	{
		// skip to the next non-comment node
		if (SkipToNextVal())
		{
			// get the string value of the current xml node
			gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,piVal);
			return true;
		}
		// otherwise we can't find a non-comment node on this level so we will FAssert and return false
		else
		{
			FAssertMsg(false , "Error in GetNextXmlVal function, unable to find the next non-comment node");
			return false;
		}
	}
	// otherwise there are no more sibling nodes but we were expecting them so FAssert and return false
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetNextXmlVal(float* pfVal, float fDefault = 0.0f)
//
//  PURPOSE :   Get the float value of the next sibling of the current xml node or the next
//				non-comment xml node if the current node is a comment node
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetNextXmlVal(float* pfVal, float fDefault)
{
	// set the value to the default
	*pfVal = fDefault;

	// if we can set the current xml node to it's next sibling
	if (gDLL->getXMLIFace()->NextSibling(m_pFXml))
	{
		// skip to the next non-comment node
		if (SkipToNextVal())
		{
			// get the string value of the current xml node
			gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,pfVal);
			return true;
		}
		// otherwise we can't find a non-comment node on this level so we will FAssert and return false
		else
		{
			FAssertMsg(false , "Error in GetNextXmlVal function, unable to find the next non-comment node");
			return false;
		}
	}
	// otherwise there are no more sibling nodes but we were expecting them so FAssert and return false
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetNextXmlVal(bool* pbVal, bool bDefault = false)
//
//  PURPOSE :   Get the boolean value of the next sibling of the current xml node or the next
//				non-comment xml node if the current node is a comment node
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetNextXmlVal(bool* pbVal, bool bDefault)
{
	// set the boolean value to it's default value
	*pbVal = bDefault;

	// if we can set the current xml node to it's next sibling
	if (gDLL->getXMLIFace()->NextSibling(m_pFXml))
	{
		// skip to the next non-comment node
		if (SkipToNextVal())
		{
			// get the string value of the current xml node
			gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,pbVal);
			return true;
		}
		// otherwise we can't find a non-comment node on this level so we will FAssert and return false
		else
		{
			FAssertMsg(false , "Error in GetNextXmlVal function, unable to find the next non-comment node");
			return false;
		}
	}
	// otherwise there are no more sibling nodes but we were expecting them so FAssert and return false
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetChildXmlVal(std::string& pszVal, char* pszDefault = NULL)
//
//  PURPOSE :   overloaded function that sets the current xml node to it's first non-comment child node
//				and then that node's string value
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetChildXmlVal(std::string& pszVal, char* pszDefault)
{
	if (pszDefault)
	{
		pszVal = pszDefault;
	}
	else
	{
		pszVal.clear();
	}

	// if we successfully set the current xml node to it's first child node
	if (gDLL->getXMLIFace()->SetToChild(m_pFXml))
	{
		// skip to the next non-comment node
		if (SkipToNextVal())
		{
			// get the string value of the current xml node
			gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,pszVal);
			return true;
		}
		// otherwise we can't find a non-comment node on this level so we will FAssert and return false
		else
		{
			FAssertMsg(false , "Error in GetChildXmlVal function, unable to find the next non-comment node");
			return false;
		}
	}
	// otherwise there are no child nodes but we were expecting them so FAssert and return false
	else
	{
		FAssertMsg(false , "Error in GetChildXmlVal function, unable to find a child node");
		return false;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetChildXmlVal(std::wstring& pszVal, wchar* pszDefault = NULL)
//
//  PURPOSE :   overloaded function that sets the current xml node to it's first non-comment child node
//				and then that node's string value
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetChildXmlVal(std::wstring& pszVal, wchar* pszDefault)
{
	if (pszDefault)
	{
		pszVal = pszDefault;
	}
	else
	{
		pszVal.clear();
	}

	// if we successfully set the current xml node to it's first child node
	if (gDLL->getXMLIFace()->SetToChild(m_pFXml))
	{
		// skip to the next non-comment node
		if (SkipToNextVal())
		{
			// get the string value of the current xml node
			gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,pszVal);
			return true;
		}
		// otherwise we can't find a non-comment node on this level so we will FAssert and return false
		else
		{
			FAssertMsg(false , "Error in GetChildXmlVal function, unable to find the next non-comment node");
			return false;
		}
	}
	// otherwise there are no child nodes but we were expecting them so FAssert and return false
	else
	{
		FAssertMsg(false , "Error in GetChildXmlVal function, unable to find a child node");
		return false;
	}
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetChildXmlVal(char* pszVal, char* pszDefault = NULL)
//
//  PURPOSE :   overloaded function that sets the current xml node to it's first non-comment child node
//				and then that node's string value
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetChildXmlVal(char* pszVal, char* pszDefault)
{
	if (pszDefault)
	{
		strcpy(pszVal, pszDefault);
	}
	else
	{
		strcpy(pszVal, "");
	}

	// if we successfully set the current xml node to it's first child node
	if (gDLL->getXMLIFace()->SetToChild(m_pFXml))
	{
		// skip to the next non-comment node
		if (SkipToNextVal())
		{
			// get the string value of the current xml node
			gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,pszVal);
			return true;
		}
		// otherwise we can't find a non-comment node on this level so we will FAssert and return false
		else
		{
			FAssertMsg(false , "Error in GetChildXmlVal function, unable to find the next non-comment node");
			return false;
		}
	}
	// otherwise there are no child nodes but we were expecting them so FAssert and return false
	else
	{
		FAssertMsg(false , "Error in GetChildXmlVal function, unable to find a child node");
		return false;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetChildXmlVal(wchar* pszVal, wchar* pszDefault = NULL)
//
//  PURPOSE :   overloaded function that sets the current xml node to it's first non-comment child node
//				and then that node's string value
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetChildXmlVal(wchar* pszVal, wchar* pszDefault)
{
	if (pszDefault)
	{
		wcscpy(pszVal, pszDefault);
	}
	else
	{
		wcscpy(pszVal, L"");
	}

	// if we successfully set the current xml node to it's first child node
	if (gDLL->getXMLIFace()->SetToChild(m_pFXml))
	{
		// skip to the next non-comment node
		if (SkipToNextVal())
		{
			// get the string value of the current xml node
			gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,pszVal);
			return true;
		}
		// otherwise we can't find a non-comment node on this level so we will FAssert and return false
		else
		{
			FAssertMsg(false , "Error in GetChildXmlVal function, unable to find the next non-comment node");
			return false;
		}
	}
	// otherwise there are no child nodes but we were expecting them so FAssert and return false
	else
	{
		FAssertMsg(false , "Error in GetChildXmlVal function, unable to find a child node");
		return false;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetChildXmlVal(int* piVal, int iDefault = 0)
//
//  PURPOSE :   overloaded function that sets the current xml node to it's first non-comment child node
//				and then that node's integer value
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetChildXmlVal(int* piVal, int iDefault)
{
	// set the value to the default
	*piVal = iDefault;

	// if we successfully set the current xml node to it's first child node
	if (gDLL->getXMLIFace()->SetToChild(m_pFXml))
	{
		// skip to the next non-comment node
		if (SkipToNextVal())
		{
			// get the string value of the current xml node
			gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,piVal);
			return true;
		}
		// otherwise we can't find a non-comment node on this level so we will FAssert and return false
		else
		{
			FAssertMsg(false , "Error in GetChildXmlVal function, unable to find the next non-comment node");
			return false;
		}
	}
	// otherwise there are no child nodes but we were expecting them so FAssert and return false
	else
	{
		FAssertMsg(false , "Error in GetChildXmlVal function, unable to find a child node");
		return false;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetChildXmlVal(float* pfVal, float fDefault = 0.0f)
//
//  PURPOSE :   overloaded function that sets the current xml node to it's first non-comment child node
//				and then that node's float value
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetChildXmlVal(float* pfVal, float fDefault)
{
	// set the value to the default
	*pfVal = fDefault;

	// if we successfully set the current xml node to it's first child node
	if (gDLL->getXMLIFace()->SetToChild(m_pFXml))
	{
		// skip to the next non-comment node
		if (SkipToNextVal())
		{
			// get the string value of the current xml node
			gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,pfVal);
			return true;
		}
		// otherwise we can't find a non-comment node on this level so we will FAssert and return false
		else
		{
			FAssertMsg(false, "Error in GetChildXmlVal function, unable to find the next non-comment node");
			return false;
		}
	}
	// otherwise there are no child nodes but we were expecting them so FAssert and return false
	else
	{
		FAssertMsg(false, "Error in GetChildXmlVal function, unable to find a child node");
		return false;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetChildXmlVal(bool* pbVal, bool bDefault = false)
//
//  PURPOSE :   overloaded function that sets the current xml node to it's first non-comment child node
//				and then that node's boolean value
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetChildXmlVal(bool* pbVal, bool bDefault)
{
	// set the boolean value to it's default value
	*pbVal = bDefault;

	// if we successfully set the current xml node to it's first child node
	if (gDLL->getXMLIFace()->SetToChild(m_pFXml))
	{
		// skip to the next non-comment node
		if (SkipToNextVal())
		{
			// get the string value of the current xml node
			gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,pbVal);
			return true;
		}
		// otherwise we can't find a non-comment node on this level so we will FAssert and return false
		else
		{
			FAssertMsg(false, "Error in GetChildXmlVal function, unable to find the next non-comment node");
			return false;
		}
	}
	// otherwise there are no child nodes but we were expecting them so FAssert and return false
	else
	{
		FAssertMsg(false, "Error in GetChildXmlVal function, unable to find a child node");
		return false;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetChildXmlValByName(wchar* pszVal, char const* szName, wchar* pszDefault = NULL)
//
//  PURPOSE :   Overloaded function that gets the child value of the tag with szName if there is only one child
// 				value of that name

//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetChildXmlValByName(wchar* pszVal, char const* szName, wchar* pszDefault)
{
	int iNumChildrenByTagName=1;

	if (pszDefault)
	{
		wcscpy(pszVal, pszDefault);
	}
	else
	{
		wcscpy(pszVal, L"");
	}

#if 0
	iNumChildrenByTagName = gDLL->getXMLIFace()->NumOfChildrenByTagName(m_pFXml,szName);
	FAssertMsg((iNumChildrenByTagName < 2),"More children with tag name than expected, should only be 1.");
#endif
	// we only continue if there are one and only one children with this tag name
	if (iNumChildrenByTagName == 1)
	{
		if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,szName))
		{
			// skip to the next non-comment node
			if (SkipToNextVal())
			{
				// get the string value of the current xml node
				gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,pszVal);
				gDLL->getXMLIFace()->SetToParent(m_pFXml);
				return true;
			}
			// otherwise we can't find a non-comment node on this level so we will FAssert and return false
			else
			{
				FAssertMsg(false, "Error in GetChildXmlValByName function, unable to find the next non-comment node");
				gDLL->getXMLIFace()->SetToParent(m_pFXml);
				return false;
			}
		}
		// otherwise there are no child nodes but we were expecting them so FAssert and return false
		else
		{
//			FAssertMsg(false, "Error in GetChildXmlValByName function, unable to find a specified node");
			return false;
		}
	}
	else
	{
		// FAssertMsg(false, "Error in GetChildXmlValByName function, unable to find a specified node");
		return false;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetChildXmlValByName(char* pszVal, char const* szName, char* pszDefault = NULL)
//
//  PURPOSE :   Overloaded function that gets the child value of the tag with szName if there is only one child
// 				value of that name

//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetChildXmlValByName(char* pszVal, char const* szName, char* pszDefault)
{
	int iNumChildrenByTagName=1;

	if (pszDefault)
	{
		strcpy(pszVal, pszDefault);
	}
	else
	{
		strcpy(pszVal, "");
	}

#if 0
	iNumChildrenByTagName = gDLL->getXMLIFace()->NumOfChildrenByTagName(m_pFXml,szName);
	FAssertMsg((iNumChildrenByTagName < 2),"More children with tag name than expected, should only be 1.");
#endif
	// we only continue if there are one and only one children with this tag name
	if (iNumChildrenByTagName == 1)
	{
		if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,szName))
		{
			// skip to the next non-comment node
			if (SkipToNextVal())
			{
				// get the string value of the current xml node
				gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,pszVal);
				gDLL->getXMLIFace()->SetToParent(m_pFXml);
				return true;
			}
			// otherwise we can't find a non-comment node on this level so we will FAssert and return false
			else
			{
				FAssertMsg(false, "Error in GetChildXmlValByName function, unable to find the next non-comment node");
				gDLL->getXMLIFace()->SetToParent(m_pFXml);
				return false;
			}
		}
		// otherwise there are no child nodes but we were expecting them so FAssert and return false
		else
		{
//			FAssertMsg(false, "Error in GetChildXmlValByName function, unable to find a specified node");
			return false;
		}
	}
	else
	{
		// FAssertMsg(false, "Error in GetChildXmlValByName function, unable to find a specified node");
		return false;
	}
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetChildXmlValByName(std::string& pszVal, char const* szName, char* pszDefault = NULL)
//
//  PURPOSE :   Overloaded function that gets the child value of the tag with szName if there is only one child
// 				value of that name

//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetChildXmlValByName(std::string& pszVal, char const* szName, char* pszDefault)
{
	int iNumChildrenByTagName=1;

	if (pszDefault)
	{
		pszVal=pszDefault;
	}
	else
	{
		pszVal.clear();
	}

#if 0
	iNumChildrenByTagName = gDLL->getXMLIFace()->NumOfChildrenByTagName(m_pFXml,szName);
	FAssertMsg((iNumChildrenByTagName < 2),"More children with tag name than expected, should only be 1.");
#endif
	// we only continue if there are one and only one children with this tag name
	if (iNumChildrenByTagName == 1)
	{
		if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,szName))
		{
			// skip to the next non-comment node
			if (SkipToNextVal())
			{
				// get the string value of the current xml node
				gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,pszVal);
				gDLL->getXMLIFace()->SetToParent(m_pFXml);
				return true;
			}
			// otherwise we can't find a non-comment node on this level so we will FAssert and return false
			else
			{
				FAssertMsg(false, "Error in GetChildXmlValByName function, unable to find the next non-comment node");
				gDLL->getXMLIFace()->SetToParent(m_pFXml);
				return false;
			}
		}
		// otherwise there are no child nodes but we were expecting them so FAssert and return false
		else
		{
//			FAssertMsg(false, "Error in GetChildXmlValByName function, unable to find a specified node");
			return false;
		}
	}
	else
	{
		//FAssertMsg(false, "Error in GetChildXmlValByName function, unable to find a specified node");
		return false;
	}
}

//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetChildXmlValByName(std::wstring& pszVal, char const* szName, wchar* pszDefault)
{
	int iNumChildrenByTagName=1;

	if (pszDefault)
	{
		pszVal=pszDefault;
	}
	else
	{
		pszVal.clear();
	}

#if 0
	iNumChildrenByTagName = gDLL->getXMLIFace()->NumOfChildrenByTagName(m_pFXml,szName);
	FAssertMsg((iNumChildrenByTagName < 2),"More children with tag name than expected, should only be 1.");
#endif
	// we only continue if there are one and only one children with this tag name
	if (iNumChildrenByTagName == 1)
	{
		if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,szName))
		{
			// skip to the next non-comment node
			if (SkipToNextVal())
			{
				// get the string value of the current xml node
				gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,pszVal);
				gDLL->getXMLIFace()->SetToParent(m_pFXml);
				return true;
			}
			// otherwise we can't find a non-comment node on this level so we will FAssert and return false
			else
			{
				FAssertMsg(false, "Error in GetChildXmlValByName function, unable to find the next non-comment node");
				gDLL->getXMLIFace()->SetToParent(m_pFXml);
				return false;
			}
		}
		// otherwise there are no child nodes but we were expecting them so FAssert and return false
		else
		{
//			FAssertMsg(false, "Error in GetChildXmlValByName function, unable to find a specified node");
			return false;
		}
	}
	else
	{
		//FAssertMsg(false, "Error in GetChildXmlValByName function, unable to find a specified node");
		return false;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetChildXmlValByName(int* piVal, char const* szName, int iDefault = 0)
//
//  PURPOSE :   Overloaded function that gets the child value of the tag with szName if there is only one child
// 				value of that name
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetChildXmlValByName(int* piVal, char const* szName, int iDefault)
{
	int iNumChildrenByTagName=1;

	// set the value to the default
	*piVal = iDefault;

#if 0	// def _DEBUG
	iNumChildrenByTagName = gDLL->getXMLIFace()->NumOfChildrenByTagName(m_pFXml,szName);
	FAssertMsg((iNumChildrenByTagName < 2),"More children with tag name than expected, should only be 1.");
	// we only continue if there are one and only one children with this tag name
#endif
	if (iNumChildrenByTagName == 1)
	{
		if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,szName))
		{
			// skip to the next non-comment node
			if (SkipToNextVal())
			{
				// get the string value of the current xml node
				gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,piVal);
				gDLL->getXMLIFace()->SetToParent(m_pFXml);
				return true;
			}
			// otherwise we can't find a non-comment node on this level so we will FAssert and return false
			else
			{
				FAssertMsg(false, "Error in GetChildXmlValByName function, unable to find the next non-comment node");
				gDLL->getXMLIFace()->SetToParent(m_pFXml);
				return false;
			}
		}
		// otherwise there are no child nodes but we were expecting them so FAssert and return false
		else
		{
//			FAssertMsg(false, "Error in GetChildXmlValByName function, unable to find a specified node");
			return false;
		}
	}
	else
	{
		//FAssertMsg(false, "Error in GetChildXmlValByName function, unable to find a specified node");
		return false;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetChildXmlValByName(float* pfVal, char const* szName, float fDefault = 0.0f)
//
//  PURPOSE :   Overloaded function that gets the child value of the tag with szName if there is only one child
// 				value of that name

//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetChildXmlValByName(float* pfVal, char const* szName, float fDefault)
{
	int iNumChildrenByTagName=1;

	// set the value to the default
	*pfVal = fDefault;
#if 0
	iNumChildrenByTagName = gDLL->getXMLIFace()->NumOfChildrenByTagName(m_pFXml,szName);
	FAssertMsg((iNumChildrenByTagName < 2),"More children with tag5 name than expected, should only be 1.");
#endif
	// we only continue if there are one and only one children with this tag name
	if (iNumChildrenByTagName == 1)
	{
		if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,szName))
		{
			// skip to the next non-comment node
			if (SkipToNextVal())
			{
				// get the string value of the current xml node
				gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,pfVal);
				gDLL->getXMLIFace()->SetToParent(m_pFXml);
				return true;
			}
			// otherwise we can't find a non-comment node on this level so we will FAssert and return false
			else
			{
				FAssertMsg(false, "Error in GetChildXmlValByName function, unable to find the next non-comment node");
				gDLL->getXMLIFace()->SetToParent(m_pFXml);
				return false;
			}
		}
		// otherwise there are no child nodes but we were expecting them so FAssert and return false
		else
		{
//			FAssertMsg(false, "Error in GetChildXmlValByName function, unable to find a specified node");
			return false;
		}
	}
	else
	{
		//FAssertMsg(false, "Error in GetChildXmlValByName function, unable to find a specified node");
		return false;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   GetChildXmlValByName(bool* pbVal, char const* szName, bool bDefault = false)
//
//  PURPOSE :   Overloaded function that gets the child value of the tag with szName if there is only one child
// 				value of that name

//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::GetChildXmlValByName(bool* pbVal, char const* szName, bool bDefault)
{
	int iNumChildrenByTagName=1;

	// set the boolean value to it's default value
	*pbVal = bDefault;

#if 0
	iNumChildrenByTagName = gDLL->getXMLIFace()->NumOfChildrenByTagName(m_pFXml,szName);
	FAssertMsg((iNumChildrenByTagName < 2),"More children with tag name than expected, should only be 1.");
#endif
	// we only continue if there are one and only one children with this tag name
	if (iNumChildrenByTagName == 1)
	{
		if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,szName))
		{
			// skip to the next non-comment node
			if (SkipToNextVal())
			{
				// get the string value of the current xml node
				gDLL->getXMLIFace()->GetLastNodeValue(m_pFXml,pbVal);
				gDLL->getXMLIFace()->SetToParent(m_pFXml);
				return true;
			}
			// otherwise we can't find a non-comment node on this level so we will FAssert and return false
			else
			{
				FAssertMsg(false, "Error in GetChildXmlValByName function, unable to find the next non-comment node");
				gDLL->getXMLIFace()->SetToParent(m_pFXml);
				return false;
			}
		}
		// otherwise there are no child nodes but we were expecting them so FAssert and return false
		else
		{
//			FAssertMsg(false, "Error in GetChildXmlValByName function, unable to find a specified node");
			return false;
		}
	}
	else
	{
		//FAssertMsg(false, "Error in GetChildXmlValByName function, unable to find a specified node");
		return false;
	}
}

bool CvXMLLoadUtility::GetIntOrType(const char* szType, int& iVar, const char* szTagName, bool bMandatory)
{
	// set the default value
	iVar = 0;

	CvString szValue;

	bool bFound = GetChildXmlValByName(szValue, szTagName);
	FAssertMsg(bFound || !bMandatory, CvString::format("%s: mandatory tag %s not found", szType, szTagName).c_str());

	if (!bFound)
	{
		return false;
	}
	
	if (szValue == "NONE")
	{
		FAssertMsg(false, CvString::format("%s: tag %s contains NONE, but NONE is not allowed", szType, szTagName).c_str());
		iVar = -1;
		return !bMandatory;
	}
	else if (szValue == "0")
	{
		iVar = 0;
	}
	else
	{
		iVar = std::atoi(szValue.c_str());

		// atoi returns 0 if the string isn't an int
		if (iVar == 0)
		{
			// string can't be converted to int
			// try to look it up as a type instead
			iVar = GC.getInfoTypeForString(szValue.c_str(), true);

			if (iVar == -1)
			{
				FAssertMsg(false, CvString::format("%s: Tag %s contains string %s, but it can't be read as either int or type", szType, szTagName, szValue.c_str()).c_str());
				return false;
			}
		}
	}
	return true;
}
