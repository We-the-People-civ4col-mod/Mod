
#include "CvGameCoreDLL.h"

#include "CyXMLEditor.h"
#include "CyXMLObject.h"

#include "tinyxml2.h"
#include <direct.h>

using namespace tinyxml2;

// list of directories to search when opening files
// when opening a file, directories will be searched in this order
// since this is all about XML files, provide the path into XML
// the code assumes each path ends with / when appending filenames
const TCHAR* CONST_VANILLA_PATH[] = { 
	NULL,          // replaced with path to mod at startup
	"Assets/XML/", // vanilla, path is relative to the exe file

	// warlords and original for BTS
	NULL,
	NULL,

	// append a NULL at the end as this can be used to assert on errors rather than crash
	NULL };

CvString szAutoPath;

CyXMLEditor* CyXMLEditor::m_pInstance = NULL; // singleton declaration

const TCHAR* CONST_ICON_PATHS[][2] = {
	{ "BoolOn"       ,  "Art/Interface/Buttons/Units/Medallion_CanMove.dds" },
	{ "BoolOff"      ,  "Art/Interface/Buttons/Units/Medallion_CantMove.dds" },
	{ "Clone"        ,  "Art/Interface/MainScreen/CityScreen/hurry_commerce.dds" },
	{ "DefaultButton",  "Art/Buttons/Tech_Categorys/NativeCat.dds" },
	{ "DeleteElement", ",Art/Interface/Buttons/Actions_Builds_LeaderHeads_Specialists_Atlas.dds,8,4" },
	{ "DirClosed"    ,  "Art/Interface/Screens/City_Management/ArrowLEFT.dds" },
	{ "DirOpen"      ,  "Art/Interface/Screens/City_Management/ArrowRIGHT.dds" },
	{ "IntUpArrow"   ,  "Art/Interface/Buttons/up_arrow.dds" },
	{ "IntDownArrow" ,  "Art/Interface/Buttons/down_arrow.dds" },
	};

const TCHAR* LANGUAGES[] = {
	"French",
    "German",
    "Italian",
    "Spanish",
	NULL};

static void saveFile(XMLDocument *pDoc, const TCHAR *filename)
{
	// dedicated function to have one place to fix encoding issues should we ever have any.
	// ideally we will only have ASCII in the files we need to save, so for the time being we will chance it and save with whatever format tinyxml2 chooses (utf-8?)
	pDoc->SaveFile(filename);
}

static const TCHAR* sortByChildTag(const XMLElement *pElement)
{
	const XMLElement *pChild = pElement->FirstChildElement("Tag");
	return pChild ? pChild->GetText() : NULL;
}

static const TCHAR* sortByValue(const XMLElement *pElement)
{
	return pElement->Value();
}

// insert a new child element alphabetically
static void insertElement(XMLNode *pParent, XMLElement *pChild, const TCHAR* (*func)(const XMLElement *pElement))
{
	const TCHAR* szTag = (*func)(pChild);
	XMLElement* pLoop = pParent->FirstChildElement();
	while (pLoop != NULL && (*func)(pLoop) != NULL && strcmp((*func)(pLoop), szTag) < 0)
	{
		pLoop = pLoop->NextSiblingElement();
	}

	if (pLoop == NULL)
	{
		// tag is after all existing tags. Attach it to the end of the list
		pParent->InsertEndChild(pChild);
	}
	else
	{
		// attach before pLoop
		// since elements are attached after an element, not before, we need the previous element
		pLoop = pLoop->PreviousSiblingElement();
		if (pLoop == NULL)
		{
			// insert as the first element
			pParent->InsertFirstChild(pChild);
		}
		else
		{
			pParent->InsertAfterChild(pLoop, pChild);
		}
	}
}

static float getFloatChild(XMLElement *pElement, const char *tag)
{
	XMLElement *pChild = pElement->FirstChildElement(tag);
	return pChild != NULL ? pChild->FloatText() : 0;
}

static const char* getTextChild(XMLElement *pElement, const char *tag)
{
	XMLElement *pChild = pElement->FirstChildElement(tag);
	return pChild != NULL ? pChild->GetText() : "";
}

static void clone(XMLElement *pOriginal, XMLElement *pClone)
{
	XMLElement *pOrig = pOriginal->FirstChildElement();
	if (pOrig == NULL)
	{
		const TCHAR *szText = pOriginal->GetText();
		if (szText != NULL)
		{
			pClone->SetText(szText);
		}
		return;
	}

	for (; pOrig != NULL; pOrig = pOrig->NextSiblingElement())
	{
		XMLElement *pNew = pClone->GetDocument()->NewElement(pOrig->Value());
		pClone->InsertEndChild(pNew);
		clone(pOrig, pNew);
	}
}

// change a string into a vector of strings seperated by semicolons
// This is useful for reading strings from text xml files
static std::vector<CvWString> split(CvWString szwText)
{
	std::vector<CvWString> vector;

	while (szwText.size() > 0)
	{
		std::basic_string<wchar>::size_type iIndex = szwText.find_first_of(':');
		if (iIndex == std::basic_string<wchar>::npos)
		{
			vector.push_back(szwText);
			break;
		}
		else
		{
			vector.push_back(szwText.substr(0, iIndex));
			szwText.erase(0, iIndex + 1);
		}
	}

	return vector;
}

FileStorage::FileStorage(const TCHAR* szName, XMLDocument *Document)
{
	m_szName = szName;
	m_Document = Document;
}

xmlFileContainer::xmlFileContainer()
	: m_Description(NULL)
	, m_bIsInMod(false)
{
}

xmlFileContainer::xmlFileContainer(tinyxml2::XMLElement *Description)
	: m_Description(NULL)
	, m_bIsInMod(false)
{
	m_Description = Description;

	FAssert(getDir() != NULL);
	FAssert(getTag() != NULL);

	if (isCombo())
	{
		comboConstructor();
		return;
	}

	FAssert(getRoot() != NULL);
	FAssert(getName() != NULL);

	XMLDocument *pDocument = new XMLDocument;
	XMLDocument *m_Schema  = new XMLDocument;

	CvString szFileName = getDir();
	szFileName.append("/");
	szFileName.append(getName());
	szFileName.append(".xml");

	m_vectorFiles.push_back(FileStorage(szFileName, pDocument));

	m_bIsInMod = openFile(pDocument, szFileName.c_str());

	// open root
	XMLElement* pRoot = pDocument->FirstChildElement(getRoot());
	const char* pSchemaName = pRoot->Attribute("xmlns");
	FAssertMsg(pSchemaName != NULL, CvString::format("Failed to read schema file for %s", szFileName.c_str()));
	pSchemaName += 9; // remove leading "x-schema:" as we do not need that part

	szFileName = getDir();
	szFileName.append("/");
	szFileName.append(pSchemaName);
	openFile(m_Schema, szFileName.c_str());

	m_szSchemaName = szFileName;

	setAllTags();
}

void xmlFileContainer::comboConstructor()
{
	FAssert(getRoot() == NULL);
	FAssert(getName() == NULL);

	XMLElement *pElement = m_Description->FirstChildElement("SubFiles");
	if (pElement == NULL)
	{
		return;
	}


	XMLElement *pLoop = pElement->FirstChildElement("Tag");
	while (pLoop != NULL)
	{
		const char *szName = pLoop->GetText();
		m_TagCache[szName] = m_szTags.size();
		m_szTags.push_back(szName);
		m_ElementCache[szName] = pElement;
		pLoop = pLoop->NextSiblingElement("Tag");
	}

	// sort the elements to make them appear alphabetically
	std::sort(m_szTags.begin(), m_szTags.end());
}

xmlFileContainer::~xmlFileContainer()
{
	while (!m_vectorFiles.empty())
	{
		SAFE_DELETE(m_vectorFiles.back().m_Document);
		m_vectorFiles.pop_back();
	}
}

const TCHAR* xmlFileContainer::getDir() const
{
	const TCHAR* pReturnVal = getDesc("Dir");
	return pReturnVal;
}

const TCHAR* xmlFileContainer::getTag() const
{
	return getDesc("Tag");
}

const TCHAR* xmlFileContainer::getType() const
{
	const TCHAR* type = getDesc("Type");
	if (type == NULL)
	{
		type = "Type";
	}
	return type;
}

void xmlFileContainer::getSchema(XMLDocument *pDocument)
{
	openFile(pDocument, m_szSchemaName.c_str());
}

const TCHAR* xmlFileContainer::getDesc(const TCHAR* szTag) const
{
	XMLElement *pElement = m_Description->FirstChildElement(szTag);
	if (pElement != NULL)
	{
		return pElement->GetText();
	}
	return NULL;
}

const TCHAR* xmlFileContainer::getTag(int iIndex, bool bSkipPrefix)
{
	FAssert(iIndex >= 0);
	FAssert(iIndex < getNumTags());

	return m_szTags[iIndex].c_str() + (bSkipPrefix ? getPrefixLength() : 0);
}

void xmlFileContainer::setTags(XMLElement *pElement)
{
	const TCHAR* szSubType = getDesc("SubType");
	while (pElement != NULL)
	{
		XMLElement *pType = pElement->FirstChildElement(getType());
		if (pType != NULL)
		{
			const char *szName = pType->GetText();
			m_TagCache[szName] = m_szTags.size();
			m_szTags.push_back(szName);
			m_ElementCache[szName] = pElement;
		}
		// check if there is a subtype present
		if (szSubType != NULL)
		{
			XMLElement *pSubType = pElement->FirstChildElement(szSubType);
			if (pSubType != NULL)
			{
				// call the subtype recursively
				// this allows looping through all the subtypes
				// it also means subtypes are check for containing subtypes, but that should never be the case
				setTags(pSubType->FirstChildElement());
			}
		}
		pElement = pElement->NextSiblingElement(getTag());
	}
}

void xmlFileContainer::setAllTags()
{
	m_szTags.clear();
	m_TagCache.clear();
	m_ElementCache.clear();
	for (unsigned int i = 0; i < m_vectorFiles.size(); ++i)
	{
		XMLElement* pElement = m_vectorFiles[i].getDocument()->FirstChildElement(getRoot());
		while (pElement != NULL && strcmp(pElement->Value(), getTag()) != 0)
		{
			pElement = pElement->FirstChildElement();
		}
		setTags(pElement);
	}
	postSetTags();
}

void xmlFileContainer::postSetTags()
{
	if (m_szTags.size() > 1)
	{
		// sort the elements to make them appear alphabetically
		std::sort(m_szTags.begin(), m_szTags.end());

		// get the prefix
		// the idea is that the prefix is the longest possible string where
		// the first and the last tag are identical

		m_szPrefix = m_szTags[0];
		const TCHAR* a = m_szTags[m_szTags.size() - 1].c_str();
		const TCHAR* b = m_szPrefix.c_str();

		// get the full length of the shortest string
		unsigned int iLength = std::min(strlen(a), strlen(b));

		// shorten the comparison by one character as long as the strings aren't identical
		while (iLength > 0 && strncmp(a, b, iLength) != 0)
		{
			--iLength;
		}

		// shorten the prefix to the length just calculated
		if (iLength >= 0)
		{
			m_szPrefix.resize(iLength);
		}
	}
	else
	{
		// list has less than two elements. Too short to determine prefix
		m_szPrefix.clear();
	}
}

void xmlFileContainer::renameType(const TCHAR* szFrom, const TCHAR* szTo)
{
	for (unsigned int i = 0; i < m_vectorFiles.size(); ++i)
	{
		XMLElement* pElement = m_vectorFiles[i].getDocument()->FirstChildElement(getRoot());
		while (pElement != NULL && strcmp(pElement->Value(), getTag()) != 0)
		{
			pElement = pElement->FirstChildElement();
		}
		pElement = pElement->Parent()->ToElement();
		bool bAltered = renameTypeRecursive(pElement, szFrom, szTo);
		
		// write the file if anything changed
		if (bAltered)
		{
			writeFile(i);
		}
	}
}

bool xmlFileContainer::renameTypeRecursive(XMLElement* pElement, const TCHAR* szFrom, const TCHAR* szTo)
{
	XMLElement* pLoopElement = pElement->FirstChildElement();
	if (pLoopElement == NULL)
	{
		const TCHAR* szCurrent = pElement->GetText();
		if (szCurrent != NULL && strcmp(szCurrent, szFrom) == 0)
		{
			// the element has the value from before the type was changed. Overwrite with the new value
			pElement->SetText(szTo);
			// use the return value to tell the file was altered
			return true;
		}
	}

	bool bAltered = false;

	while (pLoopElement != NULL)
	{
		// set bAltered to the return value
		// if the return value is false, set the value to bAltered
		// this way bAltered will be true if at least one loop child returns true
		bAltered = renameTypeRecursive(pLoopElement, szFrom, szTo) || bAltered;
		pLoopElement = pLoopElement->NextSiblingElement();
	}

	return bAltered;
}

bool xmlFileContainer::openFile(XMLDocument *pFile, const TCHAR* szFileName)
{
	CvString szPath;

	bool bIsInMod = true;

	for (int i = 0; CONST_VANILLA_PATH[i] != NULL; ++i)
	{
		szPath = CONST_VANILLA_PATH[i];
		szPath.append(szFileName);
		XMLError eError = pFile->LoadFile(szPath.c_str());

		if (eError == XML_SUCCESS)
		{
			return bIsInMod;
		}
		bIsInMod = false;
	}
	FAssertMsg(false, CvString::format("Failed to locate file %s", szFileName));
	return false;
}

XMLElement* xmlFileContainer::getElement(const TCHAR *szTag) const
{
	if (szTag == NULL)
	{
		// NULL tag name should be treated as any other tag not present in the file 
		return NULL;
	}

	ElementCacheType::const_iterator it = m_ElementCache.find(szTag);
	if (it!=m_ElementCache.end())
	{
		return it->second;
	}
	return NULL;
}

XMLElement* xmlFileContainer::getList(int iIndex) const
{
	XMLElement* pElement = m_vectorFiles[iIndex].getDocument()->FirstChildElement(getRoot());
	while (pElement != NULL && strcmp(pElement->Value(), getTag()) != 0)
	{
		pElement = pElement->FirstChildElement();
	}
	return pElement->Parent()->ToElement();
}

bool xmlFileContainer::isCombo() const
{
	return m_Description->FirstChildElement("SubFiles") != NULL;
}

void xmlFileContainer::writeFile(int iIndex)
{
	FAssert(iIndex >= 0 && iIndex < (int)m_vectorFiles.size());
	CvString szSavePath = CONST_VANILLA_PATH[0];
	szSavePath.append(m_vectorFiles[iIndex].getName());
	XMLError eResult = m_vectorFiles[iIndex].getDocument()->SaveFile(szSavePath.c_str());
	if (eResult != XML_SUCCESS)
	{
		switch (eResult)
		{
			case XML_ERROR_FILE_NOT_FOUND:
				FAssertMsg(false, CvString::format("XML file not found: %s", szSavePath));
				break;
			case XML_ERROR_FILE_COULD_NOT_BE_OPENED:
				FAssertMsg(false, CvString::format("XML file can't be opened: %s\n(opened by other applications?)", szSavePath));
				break;
			default:
				FAssertMsg(false, CvString::format("XML file read error %d: %s", eResult, szSavePath));
		}
	}
}

void xmlFileContainer::writeAllFiles()
{
	int iMax = m_vectorFiles.size();
	// write all files
	// skip first file if m_bIsInMod is not set as this means it's a vanilla file
	for (int i = 0; i < iMax; ++i)
	{
		if (m_bIsInMod || i > 0)
		{
			writeFile(i);
		}
	}
}

GlobalTypeContainer::GlobalTypeContainer(XMLElement *Description)
{
	m_Description = Description->FirstChildElement();

	XMLElement *pElement = m_Description;

	if (strcmp(getTag(), "FootstepAudioTag") == 0)
	{
		// no need to set up this one
		return;
	}


	while (pElement != NULL)
	{
		CvString szTag = pElement->GetText();
		m_szTags.push_back(szTag);
		pElement = pElement->NextSiblingElement();
	}

	postSetTags();
}

const TCHAR* GlobalTypeContainer::getDir() const
{
	return "GlobalTypes";
}

const TCHAR* GlobalTypeContainer::getTag() const
{
	return m_Description->Value();
}


// container sort function. Used by sort
bool sortContainers(const xmlFileContainer* a, const xmlFileContainer* b)
{
	return strcmp(a->getTag(), b->getTag()) < 0;
}

///
///
///    class CyXMLEditor
///
///

CyXMLEditor::CyXMLEditor()
	: m_Document(NULL)
	, m_GlobalTypes(NULL)
	, m_modPath(NULL)
	, m_dllPath(NULL)
	, m_ModSettingsDoc(NULL)
	, m_Schema(NULL)
	, m_Info(NULL)
	, m_Keyboard(NULL)
	, m_iMaxGameFontID(0)
	, m_iActiveModFile(0)
	, m_szKeyboard(NULL)
	, m_pFileSpecificInfo(NULL)
	, m_DocCommandFile(new XMLDocument)
	, m_EditorInMod(false)
{
	m_Document = new XMLDocument;
	m_GlobalTypes = new XMLDocument;
	m_Schema   = new XMLDocument;
	m_Info     = new XMLDocument;
	m_Keyboard = new XMLDocument;

	m_pInstance = this;

	// get the paths to both DLL and mod
	setDLLpath();
	setModPath();

	// update path to mod
	CONST_VANILLA_PATH[0] = getModPath();
	szAutoPath = getModPath();
	szAutoPath.append("TEXT/XML_AUTO_");

	// set the keyboard string
	m_szKeyboard = new TCHAR[KL_NAMELENGTH];
	GetKeyboardLayoutName(m_szKeyboard);

	{
		std::string name = getDLLPath();
		name.append("XML/Editor/CommandText.xml");
		XMLError eError = m_DocCommandFile->LoadFile(name.c_str());
		FAssertMsg(eError == XML_SUCCESS, "Assets/XML/Editor/CommandText.xml not found in Editor mod");
	}

	bool bHasEditorFiles = openFile("Editor/EditorFiles.xml");
	if (!bHasEditorFiles)
	{
		try
		{
			createEditorFiles();
		}
		catch (int e)
		{
			FAssertMsg(false, "Creating editor files failed");
			e = 0;
		}
	}
	XMLElement * pRoot = getRoot("Files");
	FAssertMsg(pRoot != NULL, "Failed to read files in XML/Editor/EditorFiles.xml");
	XMLElement * pElement = pRoot != NULL ? pRoot->FirstChildElement("File") : NULL;
	while (pElement != NULL)
	{
		// create file container
		xmlFileContainer *pContainer = new xmlFileContainer(pElement);
		// store dirname
		m_vectorDirs.push_back(pContainer->getDir());
		// store pointer
		m_szFiles.push_back(pContainer);
		// move on to next file
		pElement = pElement->NextSiblingElement("File");
	}

	openFile("GlobalTypes.xml", m_GlobalTypes);
	pRoot = getRoot("Civ4Types", m_GlobalTypes);
	pElement = pRoot->FirstChildElement();
	while (pElement != NULL)
	{
		// create file container
		GlobalTypeContainer *pContainer = new GlobalTypeContainer(pElement);
		// store dirname
		m_vectorDirs.push_back("GlobalTypes");
		// store pointer
		m_szFiles.push_back(pContainer);
		// move on to next file
		pElement = pElement->NextSiblingElement();
	}

	// sort tagnames (filenames)
	sort(m_szFiles.begin(), m_szFiles.end(), sortContainers);

	// cache index
	for (unsigned int i = 0; i < m_szFiles.size(); ++i)
	{
		m_FileTypeCache[m_szFiles[i]->getTag()] = i;
		m_ContainerCache[m_szFiles[i]->getTag()] = m_szFiles[i];
	}

	// remove dublicated dirs
	// first sort alphabetically
	sort( m_vectorDirs.begin(), m_vectorDirs.end() );
	// next delete any string, which is identical to the last one
	m_vectorDirs.erase( unique( m_vectorDirs.begin(), m_vectorDirs.end() ), m_vectorDirs.end() );

	pRoot = getRoot("MaxGameFontID");
	m_iMaxGameFontID = pRoot != NULL ? pRoot->IntText() : 0;
	FAssertMsg(m_iMaxGameFontID != 0, "Failed to read MaxGameFontID from XML/Editor/EditorFiles.xml");

	readActiveFile();

	// read keys
	openKeyboard();
	pRoot = getRoot("keys", m_Keyboard);
	FAssertMsg(pRoot != NULL, "Corrupted keyboard xml");
	if (pRoot != NULL)
	{
		pElement = pRoot != NULL ? pRoot->FirstChildElement("key") : NULL;
		FAssertMsg(pElement != NULL, "Corrupted keyboard xml");
		while (pElement != NULL)
		{
			m_vectorKeys.push_back(pElement);
			pElement = pElement->NextSiblingElement("key");
		}
	}

	// opening the info file (help text, user assigned tag types etc)
	m_szInfoFileName = "Editor/EditorInfos.xml";
	openFile(m_szInfoFileName, m_Info, true);
	if (m_Info->ErrorID() != XML_SUCCESS)
	{
		XMLNode * pRoot = m_Info->NewElement("Tags");
		m_Info->InsertFirstChild(pRoot);
		writeFile(true);
	}
}

CyXMLEditor::~CyXMLEditor()
{
	while (!m_szFiles.empty())
	{
		SAFE_DELETE(m_szFiles.back());
		m_szFiles.pop_back();
	}

	SAFE_DELETE(m_Document);
	SAFE_DELETE(m_GlobalTypes);
	SAFE_DELETE(m_Info);
#ifndef USING_EDITOR_IN_MOD
	SAFE_DELETE(m_modPath);
#endif
	SAFE_DELETE(m_dllPath);
	SAFE_DELETE(m_ModSettingsDoc);
	SAFE_DELETE(m_Schema);
	SAFE_DELETE(m_Keyboard);
	SAFE_DELETE(m_szKeyboard);
	SAFE_DELETE(m_DocCommandFile);

	if (m_pInstance == this)
	{
		// if python makes a new editor instance without clearing the old one, then the instance pointer will already point to the new one
		m_pInstance = NULL;
	}
}

void CyXMLEditor::createEditorFiles() const
{
	tinyxml2::XMLDeclaration* Header = m_Document->NewDeclaration();
	m_Document->InsertFirstChild(Header);

	// add the icons
	tinyxml2::XMLElement* Icons = m_Document->NewElement("Icons");
	m_Document->InsertEndChild(Icons);
	for (int i = 0; i < (int)(sizeof CONST_ICON_PATHS/sizeof CONST_ICON_PATHS[0]) != NULL; ++i)
	{
		tinyxml2::XMLElement* temp = m_Document->NewElement(CONST_ICON_PATHS[i][0]);
		temp->SetText(CONST_ICON_PATHS[i][1]);
		Icons->InsertEndChild(temp);
	}
	// gamefont end
	//TODO set the end automatically to match the xml
	tinyxml2::XMLElement* GameFont = m_Document->NewElement("MaxGameFontID");
	GameFont->SetText("8950");
	m_Document->InsertEndChild(GameFont);

	// set the languages needed
	{
		XMLElement *Languages = m_Document->NewElement("Languages");
		m_Document->InsertEndChild(Languages);

		Languages->InsertEndChild(m_Document->NewComment(" DefaultString is whatever string should be added for non-English strings. "));

		XMLElement *Default = m_Document->NewElement("DefaultString");
		Default->SetText("FIXME");
		Languages->InsertEndChild(Default);

		Languages->InsertEndChild(m_Document->NewComment(" List of languages to add to xml OTHER than English.                       "));
		Languages->InsertEndChild(m_Document->NewComment(" Order matters and English is assumed to always be first.                  "));

		XMLElement *LangList = m_Document->NewElement("List");
		Languages->InsertEndChild(LangList);

		for (int i = 0; LANGUAGES[i] != NULL; ++i)
		{
			XMLElement *lang = m_Document->NewElement(LANGUAGES[i]);
			lang->SetText(LANGUAGES[i]);
			LangList->InsertEndChild(lang);
		}
	}


	// generate a list of all xml files
	std::vector<std::string> files;
	for (unsigned int i = 0; CONST_VANILLA_PATH[i] != NULL; ++i)
	{
		std::vector<std::string> temp_files = getFiles(CONST_VANILLA_PATH[i]);
		files.insert(files.end(), temp_files.begin(), temp_files.end());
		std::sort(files.begin(), files.end());
		files.erase(std::unique(files.begin(), files.end()), files.end());
	}

	// insert the now sorted list into xml
	tinyxml2::XMLElement* FileElement = m_Document->NewElement("Files");
	m_Document->InsertEndChild(FileElement);

	for (unsigned int i = 0; i < files.size(); ++i)
	{
		tinyxml2::XMLElement* temp = generateFileElementForEditorFiles(files[i].c_str());
		if (temp != NULL)
		{
			FileElement->InsertEndChild(temp);
		}
	}

	// generate the file path
	CvString szPath = getModPath();
	szPath.append("Editor");
	// create the editor directory. Let it silently fail if it's already present. No need to check explicitly
	mkdir(szPath.c_str());
	szPath.append("/EditorFiles.xml");
	tinyxml2::XMLError eResult = m_Document->SaveFile(szPath);

	FAssertMsg(eResult == XML_SUCCESS, CvString::format("Error creating new EditorFiles.xml: %s", XMLDocument::ErrorIDToName(eResult)).c_str());
}

std::vector<std::string> CyXMLEditor::getFiles(const TCHAR* path, const TCHAR* prefix) const
{
	std::vector<std::string> files;
    std::string search_path = path;
	search_path.append("*.*");
    WIN32_FIND_DATA fd; 
    HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd); 
    if(hFind != INVALID_HANDLE_VALUE) { 
        do { 
            if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (fd.cFileName[0] == '.'
					|| strcmp(fd.cFileName, "Text") == 0
					|| strcmp(fd.cFileName, "text") == 0
					|| strcmp(fd.cFileName, "Editor") == 0
					|| strcmp(fd.cFileName, "Audio") == 0
					|| strcmp(fd.cFileName, "audio") == 0
					|| strcmp(fd.cFileName, "Scenarios") == 0
					|| strcmp(fd.cFileName, "scenarios") == 0
					)
				{
					continue;
				}
                std::string newPrefix = prefix;
				newPrefix.append(fd.cFileName);
				newPrefix.append("/");
				std::string newPath = path;
				newPath.append(fd.cFileName);
				newPath.append("/");
				std::vector<std::string> newFiles = getFiles(newPath.c_str(), newPrefix.c_str());
				files.insert(files.end(), newFiles.begin(), newFiles.end());
            }
			else
			{
				if (strcmp(prefix, "") == 0)
				{
					continue;
				}
				std::string temp = prefix;
				temp.append(fd.cFileName);
				files.push_back(temp);
			}
        }while(::FindNextFile(hFind, &fd)); 
        ::FindClose(hFind); 
    } 
	return files;
}

tinyxml2::XMLElement* CyXMLEditor::generateFileElementForEditorFiles(std::string path) const
{
	tinyxml2::XMLDocument doc;
	openFile(path.c_str(), &doc);

	// ignore any xml file without a 3 level structure (excludes files like schema files)
	tinyxml2::XMLElement* first = doc.FirstChildElement();
	if (first == NULL) return NULL;
	tinyxml2::XMLElement* second = first->FirstChildElement();
	if (second == NULL) return NULL;
	tinyxml2::XMLElement* third = second->FirstChildElement();
	if (third == NULL) return NULL;

	// generate all needed strings
	unsigned index = path.find_last_of("/");

	std::string Dir  = path.substr(0, index);
	std::string Name = path.substr(index+1);
	            Name = Name.substr(0, Name.size()-4);
	std::string Tag  = third->Value();
	std::string Root = first->Value();

	// build the element

	tinyxml2::XMLElement* elementFile  = m_Document->NewElement("File");
	tinyxml2::XMLElement* elementDir   = m_Document->NewElement("Dir");
	elementDir->SetText(Dir.c_str());
	elementFile->InsertEndChild(elementDir);
	tinyxml2::XMLElement* elementName  = m_Document->NewElement("Name");
	elementName->SetText(Name.c_str());
	elementFile->InsertEndChild(elementName);
	tinyxml2::XMLElement* elementTag   = m_Document->NewElement("Tag");
	elementTag->SetText(Tag.c_str());
	elementFile->InsertEndChild(elementTag);
	tinyxml2::XMLElement* elementRoot  = m_Document->NewElement("Root");
	elementRoot->SetText(Root.c_str());
	elementFile->InsertEndChild(elementRoot);


	return elementFile;
}

/**
 * Copy an entire directory tree from one place to another and copy one type of files
 * The target directories are created if needed
 * source: the directory to copy from
 * target: the directory to copy to
 * extension: the file extension for files to copy, example "/*.exe"
 */
void copyDirRecursively(const char *source, const char *target, const char* extension)
{
	// start by making the directory. Let it silently fail if it already exist.
	mkdir(target);

	// directory search of source. List everything and skip any result without the dir attribute set.
    std::string search_path = source;
	search_path.append("/*.*");
    WIN32_FIND_DATA fd; 
    HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd); 
    if(hFind != INVALID_HANDLE_VALUE) { 
        do { 
            if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (fd.cFileName[0] == '.')
				{
					continue;
				}
				// it's a directory. Append the dir name and call recursively.
				std::string newSource = source;
				newSource.append("/");
				newSource.append(fd.cFileName);
				std::string newTarget = target;
				newTarget.append("/");
				newTarget.append(fd.cFileName);
				copyDirRecursively(newSource.c_str(), newTarget.c_str(), extension);
            }
        }while(::FindNextFile(hFind, &fd)); 
        ::FindClose(hFind); 
    }

	// new directory search. This time list only the files with the chosen file extension.
	search_path = source;
	search_path.append(extension); 
    hFind = ::FindFirstFile(search_path.c_str(), &fd);
	if(hFind != INVALID_HANDLE_VALUE) { 
        do { 
            if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				// Found a file. Set up the paths to copy it.
				std::string newSource = source;
				newSource.append("/");
				newSource.append(fd.cFileName);
				std::string newTarget = target;
				newTarget.append("/");
				newTarget.append(fd.cFileName);
				// Copy and let it silently fail if it already exist.
				CopyFile(newSource.c_str(), newTarget.c_str(), false);

            }
        }while(::FindNextFile(hFind, &fd)); 
        ::FindClose(hFind); 
    }
}

void deleteFilesRecursively(const char *path)
{
	std::string search_path = path;
	search_path.append("/*.*");
	WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{ 
        do
		{
			if (fd.cFileName[0] == '.')
			{
				continue;
			}
			std::string newPath = path;
			newPath.append("/");
			newPath.append(fd.cFileName);

            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				// it's a directory. Call it recursively to empty it and then delete.
				deleteFilesRecursively(newPath.c_str());
				RemoveDirectory(newPath.c_str());
            }
			else
			{
				// It's a file. Just delete the file.
				remove(newPath.c_str());
			}
        }while(::FindNextFile(hFind, &fd)); 
        ::FindClose(hFind); 
    }
	// delete the file/directory itself
	RemoveDirectory(path);
	remove(path);
}

void CyXMLEditor::copyDir(const char *dir, const char* extension) const
{
	// generate the path to the editor (where the files should be copied to)
	std::string target = getDLLPath();
	target.append(dir);

	// when copied, any existing files will be skipped.
	// To make sure we get the updated list, delete all files in the editor before starting.
	deleteFilesRecursively(target.c_str());

	// loop all possible sources for file copying
	for (int i = 0;; ++i)
	{
		const TCHAR *path =	CONST_VANILLA_PATH[i];
		if (path == NULL || strcmp(path, "Assets/XML/") == 0)
		{
			// end of list of sources
			return;
		}
		std::string source = path;
		source.append("../");
		source.append(dir);
	
		// start recursive copy of the located directory.
		copyDirRecursively(source.c_str(), target.c_str(), extension);
	}
}

void CyXMLEditor::copyFiles()
{
	// list graphical locations to copy
	copyDir("Art", "/*.dds");
	copyDir("Res", "/*.tga");

	// quit the editor to force reload of graphics.
	quit();
}

//  write a new EditorSettings.xml
void CyXMLEditor::setupEditorSettings()
{
	tinyxml2::XMLDocument doc;

	tinyxml2::XMLElement* root = doc.NewElement("Editor");

	doc.InsertFirstChild(root);

	root->InsertEndChild(doc.NewComment(" Path to the mod. Path should end with Assets "));
	root->InsertEndChild(doc.NewElement("ModPath"));


	root->InsertEndChild(doc.NewComment(" Delete tags below if remote mod is a colonization mod "));
	root->InsertEndChild(doc.NewComment(" Path to civ4 vanilla folders. Should end with Assets "));
	root->InsertEndChild(doc.NewElement("Original"));
	root->InsertEndChild(doc.NewElement("Warlords"));
	root->InsertEndChild(doc.NewElement("BTS"));


	CvString path = getDLLPath();
	path.append("EditorSettings.xml");
	doc.SaveFile(path.c_str());
}

bool CyXMLEditor::altKey() const
{
	return gDLL->altKey();
}

bool CyXMLEditor::shiftKey() const
{
	return gDLL->shiftKey();
}

bool CyXMLEditor::ctrlKey() const
{
	return gDLL->ctrlKey();
}

bool CyXMLEditor::scrollLock() const
{
	return gDLL->scrollLock();
}

bool CyXMLEditor::capsLock() const
{
	return gDLL->capsLock();
}

bool CyXMLEditor::numLock() const
{
	return gDLL->numLock();
}

void CyXMLEditor::quit() const
{
	gDLL->SetDone(true);
}

const CyXMLCommandItem* CyXMLEditor::getCommandItem(const char* name) const
{
	CvWString szName;
	CvWString szHelp;
	CvWString szText;

	CvWString tagPrefix = L"TXT_KEY_XML_EDITOR_";
	tagPrefix.append(CvWString(name));

	CvWString tag(tagPrefix);
	tag.append(L"_Name");
	szName = gDLL->getText(tag);
	if (tag == szName)
	{
		szName.clear();
	}

	tag = tagPrefix;
	tag.append(L"_Help");
	szHelp = gDLL->getText(tag);
	if (tag == szHelp || szHelp == L"-")
	{
		szHelp.clear();
	}

	tag = tagPrefix;
	tag.append(L"_Text");
	szText = gDLL->getText(tag);
	if (tag == szText || szText == L"-")
	{
		szText.clear();
	}

	return new CyXMLCommandItem(gDLL->getText(szName), gDLL->getText(szHelp), gDLL->getText(szText));
}

bool CyXMLEditor::openFile(const TCHAR* szFileName)
{
	return openFile(szFileName, m_Document, true);
}

bool CyXMLEditor::openFile(const TCHAR* szFileName, XMLDocument *pDoc, bool bIgnoreFileNotFound) const
{
	bool bFileInMod = true;
	XMLError eResult = XML_ERROR_FILE_NOT_FOUND; // needs an init value

	for (int i = 0; CONST_VANILLA_PATH[i] != NULL; ++i)
	{
		CvString szPath = CONST_VANILLA_PATH[i];
		szPath.append(szFileName);
		XMLError eError = pDoc->LoadFile(szPath.c_str());

		if (eError == XML_SUCCESS)
		{
			return bFileInMod;
		}
		bFileInMod = false; // not found in the first try means it's not in vanilla
	}

	FAssertMsg(eResult == XML_SUCCESS || (eResult == XML_ERROR_FILE_NOT_FOUND && bIgnoreFileNotFound), CvString::format("%s read error: %s", szFileName, XMLDocument::ErrorIDToName(eResult)).c_str());
	
	return bFileInMod;
}

void CyXMLEditor::openKeyboard()
{
	CvString szPath = getDLLPath();
	szPath.append("xml/Editor/Keyboard_");
	szPath.append(getKeyboard());
	szPath.append(".xml");

	XMLError eResult = m_Keyboard->LoadFile(szPath.c_str());
	if (eResult == XML_SUCCESS)
	{
		return;
	}
	szPath = getDLLPath();
	szPath.append("xml/Editor/Keyboard.xml");
	eResult = m_Keyboard->LoadFile(szPath.c_str());
	if (eResult == XML_SUCCESS)
	{
		return;
	}
	FAssertMsg(false, "Failed to open keyboard xml file");
}

// write the file currently open
// bInfo makes it save the info file instead
void CyXMLEditor::writeFile(bool bInfo) const
{
	if (!bInfo)
	{
		m_szFiles[m_iActiveFile]->writeFile(m_iActiveModFile);
		return;
	}

	CvString szSavePath = getModPath();
	CvString szFileName;

	XMLDocument *pDoc = NULL;

	if (bInfo)
	{
		szFileName = m_szInfoFileName;
		pDoc = m_Info;
	}
	else
	{
		szFileName = m_szFiles[m_iActiveFile]->getName();
		pDoc = m_Document;
	}

	szSavePath.append(szFileName);

	XMLError eResult = pDoc->SaveFile(szSavePath.c_str());
	if (eResult != XML_SUCCESS)
	{
		switch (eResult)
		{
			case XML_ERROR_FILE_NOT_FOUND:
				FAssertMsg(false, CvString::format("XML file not found: %s", szFileName));
				break;
			case XML_ERROR_FILE_COULD_NOT_BE_OPENED:
				FAssertMsg(false, CvString::format("XML file can't be opened: %s\n(opened by other applications?)", szFileName));
				break;
			default:
				FAssertMsg(false, CvString::format("XML file read error %d: %s", eResult, szFileName));
		}
	}
}

void CyXMLEditor::setTypes(const TCHAR* szOld, const TCHAR* szNew)
{
	// update type cache in the active file
	m_szFiles[m_iActiveFile]->setAllTags();

	// replace szOld with szNew in all files
	if (szOld != NULL && szNew != NULL)
	{
		int iMax = m_szFiles.size();
		for (int i = 0; i < iMax; ++i)
		{
			m_szFiles[i]->renameType(szOld, szNew);
		}
	}
}

int CyXMLEditor::getFileIndex(const TCHAR* szTagName) const
{
	if (szTagName == NULL || strlen(szTagName) == 0)
	{
		return -1;
	}

	FileTypeCacheType::const_iterator it = m_FileTypeCache.find(szTagName);
	if (it!=m_FileTypeCache.end())
	{
		return it->second;
	}
	return -1;
}

XMLElement* CyXMLEditor::getRoot(const TCHAR* szRootName, XMLDocument *pDoc) const
{
	XMLNode *pRoot = pDoc ? pDoc->FirstChild() : m_Document->FirstChild();

	while (pRoot != NULL)
	{
		if (strcmp(pRoot->Value(), szRootName) == 0)
		{
			return pRoot->ToElement();
		}
		pRoot = pRoot->NextSiblingElement(szRootName);
	}
	FAssertMsg(false, CvString::format("Failed to find xml root %s", szRootName));
	return NULL;
}

XMLElement* CyXMLEditor::getSchemaElement(const char *szType)
{
	return m_SchemaCache[szType];
}

void CyXMLEditor::setModPath()
{
#ifdef USING_EDITOR_IN_MOD
	m_modPath = m_dllPath;
#else
	XMLElement *pEditor = getModSettings()->FirstChildElement("Editor");

	const char* pathBuffer = pEditor ? pEditor->FirstChildElement("ModPath")->GetText() : ".";

	if (pathBuffer == NULL)
	{
		// 
		pathBuffer = ".";
	}
	
	std::string XMLpath = "";
	if (pathBuffer[0] == '.' && pathBuffer[1] == 0)
	{
		m_EditorInMod = true;
		XMLpath = getDLLPath();
	}
	XMLpath.append(pathBuffer);
	XMLpath.append("/XML/");

	m_modPath = new TCHAR[XMLpath.size() + 1];
	memcpy(m_modPath, XMLpath.c_str(), XMLpath.size()*sizeof(TCHAR));
	m_modPath[XMLpath.size()] = 0;

	CONST_VANILLA_PATH[0] = m_modPath;

	if (pEditor == NULL || m_EditorInMod)
	{
		return;
	}

	// assign BTS paths
	std::vector<const char*> vanillaPaths;
#ifdef COLONIZATION_EXE
	// Colonization also needs to be told where BTS is while BTS use the running exe location
	vanillaPaths.push_back("BTS");
#endif
	vanillaPaths.push_back("Warlords");
	vanillaPaths.push_back("Original");

	unsigned int iVanilla = 4 - vanillaPaths.size();

	for (unsigned int i = 0; i < 3; ++i)
	{
		XMLElement *eTemp = pEditor->FirstChildElement(vanillaPaths[i]);
		if (eTemp != NULL)
		{
			std::string path = eTemp->GetText();
			if (path.empty())
			{
				continue;
			}
			path.append("/xml/");
			char *str = new char[path.size() + 1];
			memcpy(str, path.c_str(), path.size()*sizeof(CHAR));
			str[path.size()] = 0;
			CONST_VANILLA_PATH[iVanilla] = str;
			++iVanilla;
		}
	}
#endif
}

void CyXMLEditor::setDLLpath()
{
	HMODULE hModule = GetModuleHandle("CvGameCoreDLL.dll");
	m_dllPath = new TCHAR[MAX_PATH];
	GetModuleFileName(hModule, m_dllPath, MAX_PATH);
	std::string strModPath = m_dllPath;
	unsigned int found = strModPath.find_last_of("\\");
	strModPath = strModPath.substr(0,found + 1);
	memcpy(m_dllPath, strModPath.c_str(), strModPath.size()*sizeof(TCHAR));
	m_dllPath[strModPath.size()] = 0;
}

tinyxml2::XMLDocument* CyXMLEditor::getModSettings()
{
	if (m_ModSettingsDoc != NULL)
	{
		return m_ModSettingsDoc;
	}
	m_ModSettingsDoc = new tinyxml2::XMLDocument;

	std::string szPath = getDLLPath();
	szPath.append("EditorSettings.xml");

	XMLError eResult = m_ModSettingsDoc->LoadFile(szPath.c_str());
	//FAssertMsg(eResult == XML_SUCCESS, CvString::format("EditorSettings.xml read error: %s", XMLDocument::ErrorIDToName(eResult)).c_str());
	return m_ModSettingsDoc;
}

int CyXMLEditor::getNumFiles() const
{
	return m_szFiles.size();
}

const TCHAR* CyXMLEditor::getFileName(int iIndex) const
{
	if (iIndex >= 0 && iIndex < getNumFiles())
	{
		return m_szFiles[iIndex]->getName();
	}


	FAssert(iIndex >= 0);
	FAssert(iIndex < getNumFiles());
	return "";
}

int CyXMLEditor::getActiveFile() const
{
	return m_iActiveFile;
}

void CyXMLEditor::setActiveFile(int iIndex)
{
	FAssert(iIndex >= 0);
	FAssert(iIndex < getNumFiles());

	if (iIndex >= 0 && iIndex < getNumFiles())
	{
		CvString szName = m_szFiles[iIndex]->getName();
		m_iActiveFile = iIndex;

		m_szFiles[iIndex]->getSchema(m_Schema);

		// rebuild ElementType hash
		m_SchemaCache.clear();
		XMLElement *pElement = getRoot("Schema", m_Schema);
		pElement = pElement->FirstChildElement("ElementType");
		while (pElement != NULL)
		{
			m_SchemaCache[pElement->Attribute("name")] = pElement;
			pElement = pElement->NextSiblingElement("ElementType");
		}

		// update the file specific info pointer
		m_pFileSpecificInfo = m_Info->FirstChildElement("FileSpecific");
		if (m_pFileSpecificInfo != NULL)
		{
			m_pFileSpecificInfo = m_pFileSpecificInfo->FirstChildElement(m_szFiles[iIndex]->getTag());	
		}

		m_pFileInfoRoot = m_Info->FirstChildElement(m_szFiles[iIndex]->getTag());
		if (m_pFileInfoRoot == NULL)
		{
			// file not already mentioned in xml
			// add a new root for the file and insert it to keep files alphabetically
			// having files stored alphabetically reduces the risk of git conflicts

			const TCHAR* name = m_szFiles[iIndex]->getTag();
			m_pFileInfoRoot = m_Info->NewElement(name);

			tinyxml2::XMLElement* loopElement = m_Info->FirstChildElement("Tags");
			if (loopElement != NULL)
			{
				loopElement = loopElement->NextSiblingElement();
			}
			else
			{
				loopElement = m_Info->FirstChildElement();
			}

			while (true)
			{
				if (loopElement == NULL)
				{
					m_Info->InsertEndChild(m_pFileInfoRoot);
					break;
				}
				if (strcmp(name, loopElement->Name()) < 0)
				{
					tinyxml2::XMLElement* prevElement = loopElement->PreviousSiblingElement();
					if (prevElement != NULL)
					{
						m_Info->InsertAfterChild(prevElement, m_pFileInfoRoot);
					}
					else
					{
						m_Info->InsertFirstChild(m_pFileInfoRoot);
					}
					break;
				}
				loopElement = loopElement->NextSiblingElement();
			}

			writeFile(true);
		}

		// update the cache file to tell which file to start loading on next startup
		writeActiveFile();
	}
}

void CyXMLEditor::readActiveFile()
{
	std::string szPath = getDLLPath();
	szPath.append("EditorCache.xml");

	m_iActiveFile = -1;

	XMLDocument doc;
	XMLError eResult = doc.LoadFile(szPath.c_str());
	if (eResult == XML_SUCCESS)
	{
		XMLElement *element = doc.FirstChildElement("ActiveFile");
		if (element != NULL)
		{
			m_iActiveFile = getFileIndex(element->GetText());
		}
	}

	// check that a valid file was loaded
	if (m_iActiveFile == -1)
	{
		// use units if no valid setup can be loaded (like first run)
		m_iActiveFile = getFileIndex("UnitInfo");
		if (m_iActiveFile == -1)
		{
			// This shouldn't happen, but weird xml files might allow it and it's best to avoid a crash
			m_iActiveFile = 0;
		}
	}
}

void CyXMLEditor::writeActiveFile() const
{
	std::string szPath = getDLLPath();
	szPath.append("EditorCache.xml");

	XMLDocument doc;
	XMLDeclaration* Header = doc.NewDeclaration();
	doc.InsertFirstChild(Header);
	XMLElement *element = doc.NewElement("ActiveFile");
	element->SetText(m_szFiles[m_iActiveFile]->getTag());
	doc.InsertEndChild(element);
	doc.SaveFile(szPath.c_str());
}

xmlFileContainer* CyXMLEditor::getFileContainer(const TCHAR* szTag) const
{
	ContainerCacheType::const_iterator it = m_ContainerCache.find(szTag);
	if (it!=m_ContainerCache.end())
	{
		return it->second;
	}
	// File not found
	return NULL;
}

xmlFileContainer* CyXMLEditor::getCurrentFileContainer() const
{
	return m_szFiles[m_iActiveFile];
}

const TCHAR* CyXMLEditor::getInfo(const TCHAR* szTag, const TCHAR* szSetting) const
{
	XMLElement* pTag = NULL;
	if (m_pFileSpecificInfo != NULL)
	{
		// try to use the file specific setting
		pTag = m_pFileSpecificInfo->FirstChildElement(szTag);
	}
	
	if (pTag == NULL)
	{
		// no file specific settings found. Try global settings
		pTag = m_Info->FirstChildElement("Tags");
		if (pTag == NULL)
		{
			FAssertMsg(pTag != NULL, "editor file corrupted");
			return NULL;
		}

		// the the tag in question
		pTag = pTag->FirstChildElement(szTag);
		if (pTag == NULL)
		{
			return NULL;
		}
	}

	// get the child, which contains the requested setting
	pTag = pTag->FirstChildElement(szSetting);
	if (pTag == NULL)
	{
		return NULL;
	}

	return pTag->GetText();
}

void CyXMLEditor::setInfo(bool bFileSpecific, const TCHAR* szTag, const TCHAR* szType, const TCHAR* szHelp, const TCHAR* szClass, bool bAllowTypeNone, bool bRemoteCreate, bool bRemoteCreatePrefix, const TCHAR* szButtonChild)
{
	XMLElement* pRoot = m_Info->FirstChildElement("Tags");
	if (pRoot == NULL)
	{
		FAssertMsg(pRoot != NULL, "editor file corrupted");
		return;
	}

	if (bFileSpecific)
	{
		pRoot = m_pFileSpecificInfo; // use cached pointer to file specific settings
		if (pRoot == NULL)
		{
			// file specific entry doesn't exist. Create it
			XMLElement* pNewRoot = m_Info->FirstChildElement("FileSpecific");
			if (pNewRoot == NULL)
			{
				pNewRoot = m_Info->NewElement("FileSpecific");
				insertElement(m_Info, pNewRoot, sortByValue);
			}
			const TCHAR* szFile = m_szFiles[m_iActiveFile]->getTag();
			pRoot = pNewRoot->FirstChildElement(szFile);
			if (pRoot == NULL)
			{
				pRoot = m_Info->NewElement(szFile);
				insertElement(pNewRoot, pRoot, sortByValue);
				// update cache
				m_pFileSpecificInfo = pRoot;
			}
		}
	}

	XMLElement* pTag = pRoot->FirstChildElement(szTag);
	if (pTag != NULL)
	{
		// remove existing children to start over
		pTag->DeleteChildren();
	}
	else
	{
		// tag is not present in the file.
		pTag = m_Info->NewElement(szTag);
		insertElement(pRoot, pTag, sortByValue);
	}


	// insert all tags while ignoring NULL or empty settings

	if (szType != NULL && strlen(szType) > 0)
	{
		XMLElement* pElement = m_Info->NewElement("Type");
		pElement->SetText(szType);
		pTag->InsertEndChild(pElement);
	}
	if (szHelp != NULL && strlen(szHelp) > 0)
	{
		XMLElement* pElement = m_Info->NewElement("Help");
		pElement->SetText(szHelp);
		pTag->InsertEndChild(pElement);
	}
	if (szClass != NULL && strlen(szClass) > 0)
	{
		XMLElement* pElement = m_Info->NewElement("Class");
		pElement->SetText(szClass);
		pTag->InsertEndChild(pElement);
	}
	if (bAllowTypeNone)
	{
		XMLElement* pElement = m_Info->NewElement("bAllowTypeNone");
		pElement->SetText("1");
		pTag->InsertEndChild(pElement);
	}
	if (bRemoteCreate)
	{
		XMLElement* pElement = m_Info->NewElement("bRemoteCreate");
		pElement->SetText("1");
		pTag->InsertEndChild(pElement);
	}
	if (bRemoteCreatePrefix)
	{
		XMLElement* pElement = m_Info->NewElement("bRemoteCreatePrefix");
		pElement->SetText("1");
		pTag->InsertEndChild(pElement);
	}
	if (szButtonChild != NULL && strlen(szButtonChild) > 0)
	{
		XMLElement* pElement = m_Info->NewElement("ButtonChild");
		pElement->SetText(szButtonChild);
		pTag->InsertEndChild(pElement);
	}
	writeFile(true);
}

int CyXMLEditor::getNumTypes(int iFile) const
{
	FAssert(iFile >= 0);
	FAssert(iFile < getNumFiles());
	return m_szFiles[iFile]->getNumTags();
}

int CyXMLEditor::getComboFile(int iFile, int iIndex) const
{
	FAssert(isCombo(iFile));
	const TCHAR* pType = getType(iFile, iIndex);
	return getFileIndex(pType);
}

const TCHAR* CyXMLEditor::getType(int iFile, int iIndex) const
{
	// index -1 is always NONE
	if (iIndex == -1)
	{
		return "NONE";
	}
	FAssert(iFile >= 0);
	FAssert(iFile < getNumFiles());
	return m_szFiles[iFile]->getTag(iIndex);
}

const TCHAR* CyXMLEditor::getTypeNoPrefix(int iFile, int iIndex) const
{
	// index -1 is always NONE
	if (iIndex == -1)
	{
		return "NONE";
	}
	FAssert(iFile >= 0);
	FAssert(iFile < getNumFiles());
	return m_szFiles[iFile]->getTag(iIndex, true);
}

const TCHAR* CyXMLEditor::getFilePrefix(int iFile) const
{
	FAssert(iFile >= 0);
	FAssert(iFile < getNumFiles());
	return m_szFiles[iFile]->getPrefix();
}

const TCHAR* CyXMLEditor::getFileTag(int iFile) const
{
	FAssert(iFile >= 0);
	FAssert(iFile < getNumFiles());
	return m_szFiles[iFile]->getTag();
}

const TCHAR* CyXMLEditor::getFileDir(int iFile) const
{
	FAssert(iFile >= 0);
	FAssert(iFile < getNumFiles());
	return m_szFiles[iFile]->getDir();
}

const TCHAR* CyXMLEditor::getXMLDir(int iDir) const
{
	FAssert(iDir >= 0);
	FAssert(iDir < getXMLNumDir());
	return m_vectorDirs[iDir];
}

int CyXMLEditor::getXMLNumDir() const
{
	return m_vectorDirs.size();
}

CyXMLObject* CyXMLEditor::getList()
{
	// get schema element of the tag in question
	XMLElement *pElement = m_szFiles[m_iActiveFile]->getList();
	XMLElement *pSchema = m_SchemaCache[pElement->Value()];

	// get schema element of the parent
	CvString szTagParent = m_szFiles[m_iActiveFile]->getRoot();
	XMLElement *pSchemaParent = m_SchemaCache[szTagParent.c_str()];

	// move to the first element from the parent
	// use it to loop through all elements
	pSchemaParent = pSchemaParent->FirstChildElement("element");
	while (pSchemaParent != NULL)
	{
		// stop when an element has the type, which matches the szTag from the start of the function
		const char *Type = pSchemaParent->Attribute("type");
		if (Type != NULL && strcmp(Type, pElement->Value()) == 0)
		{
			break;
		}
		pSchemaParent = pSchemaParent->NextSiblingElement("element");
	}

	CyXMLObject *pObject = new CyXMLObject(m_szFiles[m_iActiveFile]->getList(), NULL, pSchema, pSchemaParent, this);
	return pObject;
}

const TCHAR* CyXMLEditor::getKeyInternal(int iKey, bool bShift, bool bControl, bool bAlt, bool bWide) const
{
	if (iKey < 0 || iKey >= (int)m_vectorKeys.size())
	{
		return NULL;
	}

	XMLElement *pElement = m_vectorKeys[iKey];

	// loop though all possible characters
	pElement = pElement->FirstChildElement("char");
	while (pElement != NULL)
	{
		bool bValid = true;
		const TCHAR* attribute = pElement->Attribute("shift");
		if (!bShift && attribute != NULL)
		{
			bValid = false;
		}
		attribute = pElement->Attribute("control");
		if (!bControl && attribute != NULL)
		{
			bValid = false;
		}
		attribute = pElement->Attribute("alt");
		if (!bAlt && attribute != NULL)
		{
			bValid = false;
		}

		if (!bWide && bValid)
		{
			const TCHAR* pTemp = pElement->GetText();
			while (bValid && pTemp[0] != 0)
			{
				if (pTemp[0] & 0x80)
				{
					bValid = false;
				}
				++pTemp;
			}
		}

		// return the key if modifier keys are valid
		if (bValid)
		{
			return pElement->GetText();
		}
		pElement = pElement->NextSiblingElement("char");
	}

	// no key found
	return NULL;
}

const TCHAR* CyXMLEditor::getKey(int iKey, bool bShift, bool bControl, bool bAlt) const
{
	return getKeyInternal(iKey, bShift, bControl, bAlt, false);
}

std::wstring CyXMLEditor::getWKey(int iKey, bool bShift, bool bControl, bool bAlt) const
{
	const TCHAR* szReturnValue = getKeyInternal(iKey, bShift, bControl, bAlt, true);
	if (szReturnValue == NULL)
	{
		return L"";
	}
	// TODO: figure out how to return proper values and if this is still needed
	//return iconv::fromUTF8(szReturnValue);
	return L"";
}

int CyXMLEditor::getGameFontGroupStart(int iGameFontGroup) const
{
#ifdef USING_EDITOR_IN_MOD
	XMLElement *pElement = m_szFiles[getFileIndex("GameFontInfo")]->getList()->FirstChildElement();

	switch (iGameFontGroup)
	{
		case GAMEFONT_YIELD:       return pElement->FirstChildElement("iYield"   )->IntText();
		case GAMEFONT_BUILDING:    return pElement->FirstChildElement("iBuilding")->IntText();
		case GAMEFONT_BONUS:       return pElement->FirstChildElement("iBonus"   )->IntText();
		case GAMEFONT_FATHERS:     return pElement->FirstChildElement("iFather"  )->IntText();
		case GAMEFONT_MISSIONS:    return pElement->FirstChildElement("iMission" )->IntText();
		case GAMEFONT_FONTSYMBOLS: return gc.getSymbolID(0); // exe hardcoded
		case GAMEFONT_UNITS:       return pElement->FirstChildElement("iUnit"    )->IntText();
		case GAMEFONT_RUSSIAN:     return pElement->FirstChildElement("iRussian" )->IntText();
		
	}
	FAssert(false);
#endif
	return 0;
}

int CyXMLEditor::getGameFontGroupIndex(int iGameFontID) const
{
#ifdef USING_EDITOR_IN_MOD
	int iGroup = -1;
	int iGroupStart = 0;

	for (int i = 1; i < NUM_GAMEFONT_TYPES; ++i)
	{
		int iStart = getGameFontGroupStart(i);
		if (iStart > iGroupStart && iStart <= iGameFontID)
		{
			iGroupStart = iStart;
			iGroup = i;
		}
	}

	FAssertMsg(iGroup != -1, "GameFont group not found");

	int iReturnVal = -10000 * iGroup;
	iReturnVal += iGroupStart - iGameFontID;

	return iReturnVal;
#else
	return 0;
#endif
}

bool CyXMLEditor::isGameFontFile() const
{
	return m_iActiveFile == getFileIndex("GameFontInfo");
}

int CyXMLEditor::getNextGameFontGroupStart(int iHigherThanThis) const
{
#ifdef USING_EDITOR_IN_MOD
	int iReturnVal = MAX_INT;

	for (int i = 1; i < NUM_GAMEFONT_TYPES; ++i)
	{
		int iStart = getGameFontGroupStart(i);
		if (iStart > iHigherThanThis && iStart < iReturnVal)
		{
			iReturnVal = iStart;
		}
	}
	return iReturnVal;
#else
	return 0;
#endif
}

NiColorA CyXMLEditor::getColor(const char *ColorType) const
{
	NiColorA color;
	xmlFileContainer *container = getFileContainer("ColorVal");
	XMLElement *pColor = container != NULL ? container->getElement(ColorType) : NULL;

	if (pColor != NULL)
	{
		color.r = getFloatChild(pColor, "fRed"  );
		color.g = getFloatChild(pColor, "fGreen");
		color.b = getFloatChild(pColor, "fBlue" );
		color.a = getFloatChild(pColor, "fAlpha");
	}

	return color;
}

NiColorA CyXMLEditor::getPlayerColor(const char *PlayerColorType, int iIndex) const
{
	xmlFileContainer *container = getFileContainer("PlayerColorInfo");
	XMLElement *pColor = container->getElement(PlayerColorType);

	switch (iIndex)
	{
	case 0:  return getColor(getTextChild(pColor, "ColorTypePrimary"  ));
	case 1:  return getColor(getTextChild(pColor, "ColorTypeSecondary"));
	case 2:  return getColor(getTextChild(pColor, "TextColorType"     ));
	default: return NiColorA(0,0,0,0);
	}
}

void CyXMLEditor::writeAllFiles()
{
	unsigned int iMax = m_szFiles.size();
	for (unsigned int i = 0; i < iMax; ++i)
	{
		m_szFiles[i]->writeAllFiles();
	}
}

const TCHAR* CyXMLEditor::getIcon(const TCHAR* szIcon) const
{
	XMLElement *pRoot = getRoot("Icons");
	if (pRoot != NULL)
	{
		XMLElement *pChild = pRoot->FirstChildElement(szIcon);
		if (pChild != NULL)
		{
			const TCHAR *pReturnVal = pChild->GetText();
			return pReturnVal;
		}
	}
	return NULL;
}

const TCHAR* CyXMLEditor::getButtonArt(const XMLElement* pElement) const
{
	if (pElement == NULL)
	{
		// no element means no set icon
		return NULL;
	}

	const TCHAR* pType = getInfo(pElement->Name(), "Type");
	if (pType != NULL)
	{
		// The element is a string of type Type
		// Look up the string in the file set in settings.
		ContainerCacheType::const_iterator it = m_ContainerCache.find(pType);
		if (it!=m_ContainerCache.end())
		{
			return getButtonArt(it->second->getElement(pElement->GetText()));
		}
		// File not found
		return NULL;
	}

	// get the name of the child tag, which is used to set icon
	const TCHAR* pChild = getInfo(pElement->Name(), "ButtonChild");
	if (pChild == NULL)
	{
		// nothing set. Use the default
		pChild = "Button";
	}

	const XMLElement* pChildElement = pElement->FirstChildElement(pChild);
	if (pChildElement != NULL)
	{
		// child found (telling that pElement is a dir). Call recursively on the child
		return getButtonArt(pChildElement);
	}

	// check if pElement is of type Button
	const TCHAR* pClass = getInfo(pElement->Name(), "Class");
	if (pClass != NULL && strcmp(pClass, "Button") == 0)
	{
		// it is, which means it should return the contents
		return pElement->GetText();
	}

	// the code ended with something (possibly invalid), which failed to provide button art
	return NULL;
}

bool CyXMLEditor::isColonizationExe() const
{
#ifdef COLONIZATION_EXE
	return true;
#else
	return false;
#endif
}

bool CyXMLEditor::isEditorInMod() const
{
	return m_EditorInMod;
}

const TCHAR* CyXMLEditor::getKeyboard() const
{
	return m_szKeyboard;
}

void CyXMLEditor::resetKeyboard()
{
	XMLElement *pElement = m_Keyboard->FirstChildElement("keys");
	pElement = pElement->FirstChildElement("key");

	while (pElement != NULL)
	{
		while (pElement->FirstChildElement("char") != NULL)
		{
			pElement->DeleteChild(pElement->FirstChildElement("char"));
		}

		pElement = pElement->NextSiblingElement("key");
	}
}

void CyXMLEditor::setKeyboardKey(std::wstring szKey, int iIndex, bool bShift, bool bControl, bool bAlt)
{
	XMLElement *pElement = m_Keyboard->FirstChildElement("keys");
	pElement = pElement->FirstChildElement("key");

	for (int i = 0; pElement != NULL && i < iIndex; ++i)
	{
		pElement = pElement->NextSiblingElement("key");
	}

	if (pElement != NULL)
	{
		int iNewID = 0;
		iNewID += bShift   ? 4 : 0;
		iNewID += bControl ? 2 : 0;
		iNewID += bAlt     ? 1 : 0;
		

		XMLElement *pKey = pElement->FirstChildElement("char");

		XMLElement *pPrev = pElement->FirstChildElement("enum");

		bool bOverwrite = false;

		while (pKey != NULL)
		{
			int iKeyID = 0;
			iKeyID += pKey->Attribute("shift")   != NULL ? 4 : 0;
			iKeyID += pKey->Attribute("control") != NULL ? 2 : 0;
			iKeyID += pKey->Attribute("alt")     != NULL ? 1 : 0;
			if (iNewID < iKeyID)
			{
				pPrev = pKey;
			}
			else if (iNewID == iKeyID)
			{
				// key combo exist. Overwrite the existing text
				bOverwrite = true;
				//pKey->SetText(iconv::toUTF8(szKey.c_str()));
				break;
			}

			pKey = pKey->NextSiblingElement("char");
		}
		if (!bOverwrite)
		{
			// a new key combo. Add a new element
			XMLElement *pNew = m_Keyboard->NewElement("char");
			//pNew->SetText(iconv::toUTF8(szKey.c_str()));
			if (bShift)
			{
				pNew->SetAttribute("shift", "1");
			}
			if (bControl)
			{
				pNew->SetAttribute("control", "1");
			}
			if (bAlt)
			{
				pNew->SetAttribute("alt", "1");
			}
			pElement->InsertAfterChild(pPrev, pNew);
		}
		CvString szSavePath = getDLLPath();
		szSavePath.append("xml/Editor/Keyboard_");
		szSavePath.append(getKeyboard());
		szSavePath.append(".xml");
		m_Keyboard->SaveFile(szSavePath.c_str());
	}
}

std::wstring CyXMLEditor::toUnicode(int iKey) const
{
	char buffer[2];
	buffer[0] = iKey;
	buffer[1] = 0;
	//const wchar* szReturnVal = iconv::fromCP1252(buffer);
	//return szReturnVal;
	return L"";
}

void CyXMLEditor::cleanActiveFile()
{
	cleanActiveFileRecursive(m_szFiles[m_iActiveFile]->getList());
	writeFile();
}

// function to get an XMLElement attribute
// will default to "", meaning it avoids the (crashing) issue of NULL pointers 
const char* getAttribute(const XMLElement *pElement, const char *szAttribute)
{
	if (pElement && szAttribute)
	{
		const char *szReturnVal = pElement->Attribute(szAttribute);
		if (szReturnVal)
		{
			return szReturnVal;
		}
	}
	return "";
}

void CyXMLEditor::cleanActiveFileRecursive(XMLElement *pParent)
{
	if (pParent == NULL)
	{
		// this should never happen, but in case it does, make sure it will not crash
		FAssert(false);
		return;
	}

	XMLElement *pElement = pParent->FirstChildElement();
	if (pElement == NULL)
	{
		// xml file empty, hence nothing to reorder or delete
		return;
	}

	XMLElement *pSchema = m_SchemaCache[pParent->Value()];
	XMLElement *pSchemaChild = pSchema->FirstChildElement("element");

	// first ensure that at least one tag is present in both xml file and schema
	if (pSchemaChild == NULL)
	{
		// no tags in schema, delete everything in xml file
		pParent->DeleteChildren();
		return;
	}

	// check if the first tag is correct
	if (strcmp(getAttribute(pSchemaChild, "type"), pElement->Name()) == 0)
	{
		// first tag is correct
		bool bList = strcmp(getAttribute(pSchemaChild, "maxOccurs"), "*") == 0;
		// progress to next in schema
		pSchemaChild = pSchemaChild->NextSiblingElement("element");

		if (bList)
		{
			FAssertMsg(pSchemaChild == NULL, "Having non-list elements after a list is not supported");
			if (pSchemaChild == NULL)
			{
				// remove any non-list elements
				const char *pTag = pElement->Name();
				while (pElement != NULL)
				{
					if (strcmp(pElement->Name(), pTag))
					{
						// delete the element as it's not part of the list
						XMLElement *pBroken = pElement;
						pElement = pElement->PreviousSiblingElement();
						pBroken->GetDocument()->DeleteNode(pBroken);
					}
					else
					{
						cleanActiveFileRecursive(pElement);
						pElement = pElement->NextSiblingElement();
					}
				}
			}
			return;
		}
	}
	else
	{
		// first element is incorrect, or possibly MinOccur = 0
		pElement = NULL;
	}

	// sort the tags to match the schema order
	for (;pSchemaChild != NULL; pSchemaChild = pSchemaChild->NextSiblingElement("element"))
	{
		XMLElement *pLoopElement = pParent->FirstChildElement(getAttribute(pSchemaChild, "type"));
		if (pLoopElement == NULL)
		{
			// tag is not present
			continue;
		}
		
		if (pElement == NULL)
		{
			// this has to be the first tag
			if (pLoopElement != pParent->FirstChildElement())
			{
				// it's not the first tag. Move it to the front
				pParent->InsertFirstChild(pLoopElement);
			}
		}
		else
		{
			// check if the location is ok. It should be right after pElement
			if (pElement->NextSiblingElement() != pLoopElement)
			{
				// it's not. Move it
				pParent->InsertAfterChild(pElement, pLoopElement);
			}
		}

		pElement = pLoopElement;
		cleanActiveFileRecursive(pElement);
	}

	// loop is done. If there are leftover tags, then they aren't used in the schema file and should be deleted
	if (pElement == NULL)
	{
		// the parent have children, but none of them are present in the schema 
		pParent->DeleteChildren();
	}	
	else if (pElement->NextSiblingElement() != NULL)
	{
		pSchemaChild = pSchema->LastChildElement("element");
		bool bList = strcmp(getAttribute(pSchemaChild, "maxOccurs"), "*") == 0;
		
		XMLElement *pLoopElement = pElement->NextSiblingElement();
		while (pLoopElement != NULL)
		{
			if (bList && pLoopElement->Name(), getAttribute(pSchemaChild, "type") == 0)
			{
				// item is a list element, which is ok. Move the pointer forward
				pElement = pLoopElement;

				// call recursively since the list element might have children
				cleanActiveFileRecursive(pElement);
			}
			else
			{
				// the tag should not be here
				pParent->DeleteChild(pLoopElement);
			}
			// prepare for next iteration
			pLoopElement = pElement->NextSiblingElement();
		}
	}
}

bool CyXMLEditor::isCombo(int iFile) const
{
	FAssert(iFile >= 0);
	FAssert(iFile < getNumFiles());
	return m_szFiles[iFile]->isCombo();
}

int CyXMLEditor::getComboFileIndex(int iFile, const TCHAR* szString) const
{
	// get file info
	xmlFileContainer *pContainer = m_szFiles[iFile];

	// loop subfiles
	for (int i = 0; i < pContainer->getNumTags(); ++i)
	{
		const TCHAR* szSubName = pContainer->getTag(i);
		xmlFileContainer *pSubContainer = getFileContainer(szSubName);
		// get pointer to tag with the type szString
		// returns NULL if not found
		if (pSubContainer->getElement(szString) != NULL)
		{
			// the tag has been found. Return the index to this file
			return i;
		}

	}
	// string not found in any of the files
	return -1;
}


//
// private
//

XMLElement* CyXMLEditor::gotoList(int iFile, XMLDocument *pDoc)
{
	if (pDoc == NULL)
	{
		pDoc = m_Document;
	}
	FAssert(pDoc != NULL);

	XMLElement* pRoot = getRoot(m_szFiles[iFile]->getRoot(), pDoc);
	FAssert(pRoot != NULL);
	CvString szString = m_szFiles[iFile]->getTag();
	szString.append("s");
	XMLElement* pElement = pRoot->FirstChildElement(szString);
	pElement = pElement->FirstChildElement(m_szFiles[iFile]->getTag());

	return pElement;
}

CyXMLCommandItem::CyXMLCommandItem()
{
}

CyXMLCommandItem::CyXMLCommandItem(CvWString szName, CvWString szPopupHelp, CvWString szFullText)
	: m_szName(CvString(szName))
	, m_szHelp(CvString(szPopupHelp))
	, m_szText(CvString(szFullText))
{
}

const char* CyXMLCommandItem::getName() const
{
	return m_szName.c_str();
}
const char* CyXMLCommandItem::getHelp() const
{
	return m_szHelp.c_str();
}
const char* CyXMLCommandItem::getText() const
{
	// returning NULL skips some text drawing code entirely
	if (m_szText.IsEmpty()) return NULL;
	return m_szText.c_str();
}
