
#include "CvGameCoreDLL.h"

#include "CyXMLEditor.h"
#include "CyXMLObject.h"

#include "tinyxml2.h"
#include <direct.h>

using namespace tinyxml2;

const int iNumTXTTags = 6;

const TCHAR* TXT_TAGS[iNumTXTTags] = {
	"Civilopedia",
	"Description",
	"Help",
	"Strategy",
	"ShortDescription",
	"Adjective",
};

const TCHAR* TXT_TAGS_POSTFIX[iNumTXTTags] = {
	"_PEDIA",
	"",
	"_HELP",
	"_STRATEGY",
	"_SHORT_DESC",
	"_ADJECTIVE",
};

static bool isText(const TCHAR* szTag)
{
	for (int i = 0; i < 4; ++i)
	{
		if (strcmp(szTag, TXT_TAGS[i]) == 0)
		{
			return true;
		}
	}
	return false;
}


CyXMLObject::CyXMLObject()
	: m_pXML(NULL)
	, m_pSchema(NULL)
	, m_pSchemaParent(NULL)
	, m_pXMLparent(NULL)
	, m_Attributes(NULL, NULL)
{
}


CyXMLObject::CyXMLObject(XMLElement *pXML, CyXMLObject *pXMLparent, XMLElement *pSchema, XMLElement *pSchemaParent, CyXMLEditor *pEditor)
	: m_pXML(pXML)
	, m_pSchema(pSchema)
	, m_pSchemaParent(pSchemaParent)
	, m_pXMLparent(pXMLparent)
	, m_pEditor(pEditor)
	, m_Attributes(pXML, pXMLparent, pSchema, pSchemaParent, pEditor, getName())
{
}

XMLElement* CyXMLObject::FirstChildElement(const TCHAR *szName) const
{
	return m_pXML == NULL ? NULL : m_pXML->FirstChildElement(szName);
}

// next object of the same name
// useful for cycling through a list
CyXMLObject* CyXMLObject::getNextSiblingSameName()
{
	FAssert(m_pXML != NULL);
	if (m_pXML != NULL)
	{
		XMLElement *pNext = m_pXML->NextSiblingElement(m_pXML->Name());
		if (pNext != NULL)
		{
			CyXMLObject *pNew = new CyXMLObject(pNext, m_pXMLparent, m_pSchema, m_pSchemaParent, m_pEditor);
			return pNew;
		}
		else if (isAllocated())
		{
			// always append an empty element to allow adding more
			CyXMLObject *pNew = new CyXMLObject(NULL, m_pXMLparent, m_pSchema, m_pSchemaParent, m_pEditor);
			return pNew;
		}
	}
	return NULL;
}

void CyXMLObject::allocate()
{
	FAssertMsg(!isAllocated(), "Allocating node, which is already allocated (memory leak)");
	m_pXML = m_pXMLparent->getDocument()->NewElement(getName());
	m_pXMLparent->insertChild(m_pXML, m_pSchemaParent);
}


void CyXMLObject::insertChild(XMLElement *pChildXML, XMLElement *pChildSchema)
{
	if (!isAllocated())
	{
		allocate();
	}

	// find the previous element
	// start with the element itself. This will make MaxOccurs=* append to the end
	XMLElement* pPrev = NULL;
	XMLElement* pSchema = pChildSchema;
	while (pPrev == NULL && pSchema != NULL)
	{
		pPrev = m_pXML->LastChildElement(pSchema->Attribute("type"));
		pSchema = pSchema->PreviousSiblingElement("element");
	}

	if (pPrev == NULL)
	{
		// no element found. Insert as first
		m_pXML->InsertFirstChild(pChildXML);
	}
	else
	{
		m_pXML->InsertAfterChild(pPrev, pChildXML);
	}
}



CyXMLObject* CyXMLObject::getFirstSchemaChild()
{
	FAssert(m_pSchema != NULL);
	if (m_pSchema != NULL)
	{
		XMLElement *pNewParent = m_pSchema->FirstChildElement("element");
		if (pNewParent != NULL)
		{
			const char *szType = pNewParent->Attribute("type");
			XMLElement *pNewSchema = m_pEditor->getSchemaElement(szType);
			XMLElement *pNewXML = NULL;
			if (m_pXML != NULL)
			{
				pNewXML = m_pXML->FirstChildElement(szType);
			}

			CyXMLObject *pNew = new CyXMLObject(pNewXML, this, pNewSchema, pNewParent, m_pEditor);
			return pNew;
		}
	}
	return NULL;
}

CyXMLObject* CyXMLObject::next()
{
	FAssert(m_pSchemaParent != NULL);
	if (m_pSchemaParent != NULL)
	{
		if (m_pXML != NULL && isListElement())
		{
			CyXMLObject *pTemp = getNextSiblingSameName();
			if (pTemp != NULL)
			{
				return pTemp;
			}
		}
		XMLElement *pNewParent = m_pSchemaParent->NextSiblingElement("element");
		if (pNewParent != NULL)
		{
			const char *szType = pNewParent->Attribute("type");
			XMLElement *pNewSchema = m_pEditor->getSchemaElement(szType);
			XMLElement *pNewXML = m_pXMLparent->FirstChildElement(szType);

			CyXMLObject *pNew = new CyXMLObject(pNewXML, m_pXMLparent, pNewSchema, pNewParent, m_pEditor);
			return pNew;
		}
	}
	return NULL;
}

// schema info

int CyXMLObject::getID() const
{
	// use the memory address to identify if two instances points to the same element
	return (int)m_pXML;
}

bool CyXMLObject::isAllocated() const
{
	return m_pXML != NULL;
}

bool CyXMLObject::isBool() const
{
	FAssertMsg(m_pSchema != NULL, "isBool() without schema");
	const char *szContents = m_pSchema->Attribute("dt:type");
	return (szContents != NULL && strcmp(szContents, "boolean") == 0);
}

bool CyXMLObject::isInt() const
{
	FAssertMsg(m_pSchema != NULL, "isBool() without schema");
	const char *szContents = m_pSchema->Attribute("dt:type");
	return (szContents != NULL && strcmp(szContents, "int") == 0);
}

bool CyXMLObject::isListElement() const
{
	FAssertMsg(m_pSchemaParent != NULL, "isListElement() without schema parent");
	const char *szMax = m_pSchemaParent->Attribute("maxOccurs");
	if (szMax == NULL || strcmp(szMax, "1") == 0)
	{
		return false;
	}
	FAssert(strcmp(szMax, "*") == 0);
	return true;
}

bool CyXMLObject::isCombo() const
{
	return m_pEditor->isCombo(getInfoType());
}

bool CyXMLObject::isDir() const
{
	FAssertMsg(m_pSchema != NULL, "isDir() without schema");
	const char *szContents = m_pSchema->Attribute("content");
	if (szContents != NULL && strcmp(szContents, "eltOnly") == 0)
	{
		return true;
	}
	return false;
}

// there is no schema flag for strings
// they are detected by elimination, meaning it's a string if it's nothing else
bool CyXMLObject::isString() const
{
	return !isBool() && !isDir() && !isInt();
}

bool CyXMLObject::isMandatory() const
{
	if (isAllocated())
	{
		return true;
	}
	else if (isOptional())
	{
		return false;
	}

	if (isListElement() && m_pXMLparent != NULL)
	{
		XMLElement *pPrev = m_pXMLparent->FirstChildElement(getName());
		if (pPrev != NULL)
		{
			// not the only element in the list
			return false;
		}
	}

	return m_pXMLparent->isMandatory();
}

bool CyXMLObject::isOptional() const
{
	FAssertMsg(m_pSchemaParent != NULL, "isOptional() without schema parent");
	const char *szMin = m_pSchemaParent->Attribute("minOccurs");
	if (szMin != NULL && strcmp(szMin, "0") == 0)
	{
		return true;
	}
	return false;
}

int CyXMLObject::getTextType() const
{
	const TCHAR *szName = getName();
	for (int i = 0; i < iNumTXTTags; ++i)
	{
		if (strcmp(TXT_TAGS[i], szName) == 0)
		{
			return i;
		}
	}
	return -1;
}

bool CyXMLObject::isText() const
{
	return getTextType() != -1;
}

bool CyXMLObject::canDelete() const
{
	if (!isAllocated())
	{
		return false;
	}
	if (isOptional())
	{
		return true;
	}

	// the last possible way to allow deletion is if it's a list with multiple elements
	if (isListElement())
	{
		if (m_pXML->NextSiblingElement(getName()) != NULL || m_pXML->PreviousSiblingElement(getName()) != NULL)
		{
			return true;
		}
	}

	return false;
}

const TCHAR* CyXMLObject::getName() const
{
	FAssertMsg(m_pSchema != NULL, "getName() without schema");
	if (m_pSchema != NULL)
	{
		const char *szName = m_pSchema->Attribute("name");
		return szName;
	}
	return NULL;
}

int CyXMLObject::getGameFontDisplayID() const
{
	int iChar = 0;
	if (isAllocated())
	{
		iChar = m_pXML->IntText(0);
	}

	if (iChar >= 0)
	{
		return iChar;
	}

	iChar = -iChar;

	int iGroup = iChar / 10000;
	int iIndex = iChar % 10000;

	int iReturnVal = m_pEditor->getGameFontGroupStart(iGroup);
	iReturnVal += iIndex;

	return iReturnVal;
}

const TCHAR* CyXMLObject::getInfoTypeString() const
{
	if (isDir() || isBool() || isInt())
	{
		// tag can't have a type
		return NULL;
	}

	return m_Attributes.getType();
}

int CyXMLObject::getInfoType() const
{
	if (isDir() || isBool() || isInt())
	{
		// tag can't have a type
		return -2;
	}

	const TCHAR* pString = getInfoTypeString();
	return m_pEditor->getFileIndex(pString);
}

bool CyXMLObject::allowsTypeNone() const
{
	return m_Attributes.allowsTypeNone();
}

bool CyXMLObject::isRemoteCreate() const
{
	return false;
	return m_pEditor->getInfo(getName(), "bRemoteCreate") != NULL;
}

bool CyXMLObject::isRemoteCreatePrefix() const
{
	return false;
	return m_pEditor->getInfo(getName(), "bRemoteCreatePrefix") != NULL;
}

const TCHAR* CyXMLObject::getHelp() const
{
	return m_Attributes.getHelp();
}

const TCHAR* CyXMLObject::getInfoClass() const
{
	const char* szClass = m_Attributes.getInfoClass();

	if (szClass  == NULL) return getDefaultClassForType();
	return szClass;
}

const TCHAR* CyXMLObject::getSchemaChild(int iIndex) const
{
	XMLElement* pElement = m_pSchema->FirstChildElement("element");
	int iCounter = 0;
	while (pElement != NULL && iCounter < iIndex)
	{
		++iCounter;
		pElement = pElement->NextSiblingElement("element");
	}
	if (pElement != NULL)
	{
		return pElement->Attribute("type");
	}
	return NULL;
}


int CyXMLObject::getNumSchemaChildren() const
{
	XMLElement* pElement = m_pSchema->FirstChildElement("element");
	int iCounter = 0;
	while (pElement != NULL)
	{
		++iCounter;
		pElement = pElement->NextSiblingElement("element");
	}
	return iCounter;
}

const TCHAR* CyXMLObject::getParentType() const
{
	return m_pXMLparent->getType();
}


void CyXMLObject::setInfo(bool bFileSpecific, int iNewFileForType, const TCHAR* szHelp, const TCHAR* szClass, bool bAllowTypeNone, bool bRemoteCreate, bool bRemoteCreatePrefix, const TCHAR* szButtonChild)
{
	const char *szType = NULL;
	if (iNewFileForType >= 0)
	{
		szType = m_pEditor->getFileTag(iNewFileForType);
	}
	if (szClass == NULL)
	{
		szClass = getDefaultClassForType();
	}
	m_Attributes.setInfo(szType, szHelp, szClass, bAllowTypeNone, szButtonChild);
}

// XML info

const char* CyXMLObject::getDefaultClassForType() const
{
	return isDir() ? "Dir" :
		isBool() ? "Bool" :
		isInt() ? "Int" :
		NULL;
}

const TCHAR* CyXMLObject::getType() const
{
	if (m_pXML != NULL)
	{
		XMLElement *pType = m_pXML->FirstChildElement("Type");
		if (pType == NULL)
		{
			pType = m_pXML->PreviousSiblingElement("Type");
		}
		if (pType == NULL)
		{
			pType = m_pXML->NextSiblingElement("Type");
		}
		if (pType != NULL)
		{
			return pType->GetText();
		}
	}
	return NULL;
}

const TCHAR* CyXMLObject::getChildType() const
{
	if (m_pXML != NULL)
	{
		XMLElement *pType = m_pXML->FirstChildElement("Type");
		if (pType != NULL)
		{
			return pType->GetText();
		}
	}
	return NULL;
}

const TCHAR* CyXMLObject::getValue() const
{
	if (m_pXML != NULL)
	{
		const char *szValue = m_pXML->GetText();
		return szValue;
	}
	return NULL;
}

bool CyXMLObject::getBoolValue() const
{
	if (!isAllocated())
	{
		return false;
	}
	const TCHAR *szValue = getValue();
	return *szValue == '1' && szValue[1] == 0;
}

const TCHAR* CyXMLObject::getChildString(const TCHAR* szChildName) const
{
	if (!isAllocated())
	{
		return NULL;
	}
	XMLElement *pChild = m_pXML->FirstChildElement(szChildName);
	if (pChild == NULL)
	{
		return NULL;
	}
	return pChild->GetText();
}

const TCHAR* CyXMLObject::getButtonArt() const
{
	return m_Attributes.getButtonArt();
}

const TCHAR* CyXMLObject::getButtonArtChild() const
{
	return m_Attributes.getButtonArtChild();
}

int CyXMLObject::getActiveComboFile() const
{
	if (!isAllocated() || !isCombo())
	{
		return -1;
	}
	return m_pEditor->getComboFileIndex(getInfoType(), getValue());
}

std::wstring CyXMLObject::getText() const
{
	FAssert(isText());
	if (!isAllocated())
	{
		return L"";
	}

	const TCHAR* pValue = getValue();
	if (pValue == NULL)
	{
		return L"";
	}
	CvWString tag(pValue);

	return gDLL->getText(tag.c_str());
}

// write to XML

void CyXMLObject::setValue(const TCHAR* szNewValue)
{
	if (!isAllocated())
	{
		allocate();
	}

	FAssertMsg(m_pXML != NULL, "setValue() without XML object");
	if (m_pXML != NULL)
	{
		const TCHAR* szOldValue = getValue();
		
		m_pXML->SetText(szNewValue);
		m_pEditor->writeFile();

		if (strcmp(getName(), "Type") == 0)
		{
			// recalculate the type cache
			m_pEditor->setTypes(szOldValue, getValue());

		}
	}
}

void CyXMLObject::createRemote()
{
	// first ignore calls from entries without a type
	const TCHAR* szTypeOfFirstElement = getParentType();
	if (szTypeOfFirstElement == NULL)
	{
		return;
	}

	const TCHAR* pInfoType = m_pEditor->getInfo(getName(), "Type");

	// access the file containers
	xmlFileContainer* pCurrentFileContainer = m_pEditor->getCurrentFileContainer();
	xmlFileContainer* pFileContainer = m_pEditor->getFileContainer(pInfoType);

	// generate the new value
	// prefix + type
	// type has to add the length of prefix unless the prefix is included as well
	CvString szNewValue = pFileContainer->getPrefix();
	szNewValue.append(szTypeOfFirstElement + (isRemoteCreatePrefix() ? 0 : pCurrentFileContainer->getPrefixLength()));

	// check if the remote entry exists
	if (pFileContainer->getElement(szNewValue.c_str()) == NULL)
	{
		// no such remote Type. Create it
		XMLElement* pParent = pFileContainer->getList();
		XMLElement* pClone = pParent->GetDocument()->NewElement(pInfoType);
		pParent->InsertEndChild(pClone);
		cloneRecursive(pParent->FirstChildElement(pInfoType), pClone, szNewValue.c_str());

		// save the changed file
		pFileContainer->writeFile(0);

		// update cache
		pFileContainer->setAllTags();
	}

	// write the newly created Type to this element
	setValue(szNewValue.c_str());
}

void CyXMLObject::setGameFontChar(int iNewValue)
{
	if (!isAllocated())
	{
		if (iNewValue == 0)
		{
			return;
		}
		allocate();
	}

	if (!m_pEditor->isGameFontFile())
	{
		// use the groups unless it's the file, which sets the groups
		iNewValue = m_pEditor->getGameFontGroupIndex(iNewValue);
	}

	m_pXML->SetText(CvString::format("%d", iNewValue).c_str());
	m_pEditor->writeFile();
}

void CyXMLObject::deleteXMLEntry()
{
	if (!isAllocated())
	{
		FAssertMsg(isAllocated(), "Trying to delete non-existing XML data");
		return;
	}

	XMLNode *pParent = m_pXML->Parent();
	pParent->DeleteChild(m_pXML);
	m_pXML = NULL;
	m_pEditor->writeFile();

	if (strcmp(getName(), "Type") == 0)
	{
		// recalculate the type cache
		m_pEditor->setTypes(NULL, NULL);
	}
}

void CyXMLObject::dragTo(CyXMLObject *pDest)
{
	if (strcmp(getName(), pDest->getName()))
	{
		// make sure the elements are of the same type, hence one list
		return;
	}

	XMLNode *pParent = m_pXML->Parent();
	if (!pDest->isAllocated())
	{
		// pDest is the last object in the list
		// this means m_pXML should be moved to the end
		pParent->InsertEndChild(m_pXML);
	}
	else
	{
		// here is a minor problem. This->m_pXML should be inserted before pDest
		// However tinyxml2 only supports inserting after an element
		// the solution is to get the element before pDest and insert after that one
		XMLElement *pPrev = pDest->PreviousElement();
		if (pPrev != NULL)
		{
			pParent->InsertAfterChild(pPrev, m_pXML);
		}
		else
		{
			// turns out pDest is the first element
			// insert in the front of the list
			pParent->InsertFirstChild(m_pXML);
		}
	}
	m_pEditor->writeFile();
}

void CyXMLObject::clone()
{
	// avoid working on elements, which can't be cloned
	// the GUI shouldn't allow this, but just to be safe
	if (!isAllocated() || !isListElement())
	{
		return;
	}

	// create a new "root" element with the same name as this
	XMLNode *pParent = m_pXML->Parent();
	tinyxml2::XMLElement* pClone = getDocument()->NewElement(getName());

	// insert after this
	pParent->InsertAfterChild(m_pXML, pClone);

	// clone children
	cloneRecursive(m_pXML, pClone, NULL);

	// write the result
	m_pEditor->writeFile();
}

void CyXMLObject::cloneRecursive(tinyxml2::XMLElement* pOriginal, tinyxml2::XMLElement* pClone, const TCHAR* szNewType)
{
	// make sure pClone has the same text/children contents as pOriginal

	tinyxml2::XMLElement* pLoop = pOriginal->FirstChildElement();
	if (pLoop == NULL)
	{
		// no children. Try to clone the text
		const TCHAR* szText = pOriginal->GetText();
		if (strcmp(pOriginal->Value(), "Type") == 0)
		{
			szText = szNewType;
		}
		if (szText != NULL)
		{
			pClone->SetText(szText);
		}
	}
	// loop the children
	for(;pLoop != NULL; pLoop = pLoop->NextSiblingElement())
	{
		if (strcmp(pLoop->Value(), "Type") == 0 && szNewType == NULL)
		{
			// do not clone types
			continue;
		}

		// make a new child with the same value (name)
		tinyxml2::XMLElement* pNew = pClone->GetDocument()->NewElement(pLoop->Value());

		// add it to the clone
		pClone->InsertEndChild(pNew);

		// call recursively to clone children/text contents to the new element
		cloneRecursive(pLoop, pNew, szNewType);
	}
}

XMLElement* CyXMLObject::PreviousElement() const
{
	return m_pXML->PreviousSiblingElement(getName());
}

XMLDocument* CyXMLObject::getDocument() const
{
	if (isAllocated())
	{
		return m_pXML->GetDocument();
	}
	return m_pXMLparent->getDocument();
}

tinyxml2::XMLElement* CyXMLObject::getInfoElement() const
{
	return m_Attributes.getInfo();
}
