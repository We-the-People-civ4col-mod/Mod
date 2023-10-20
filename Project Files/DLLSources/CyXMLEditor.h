#pragma once

#ifndef CyxmlEditor_h
#define CyxmlEditor_h

#include "tinyxml2.h"

#if CIV4_PRODUCT_ID == 11476
#define COLONIZATION_EXE
#endif

#ifdef MEDIEVAL_CONQUEST
#define USING_EDITOR_IN_MOD
#endif

// File to handle the classes for storing the xml editor interface for xml
// The same classes are python exposed to provide full access from python
//
// The python interface has some limitations
// because of this, not all const functions are marked as const
// Also the python interface won't compile without default constructors (at least in some cases)
// To get around this issue, some default contructors have been added even if they should not be used

typedef stdext::hash_map<std::string /* type name */, tinyxml2::XMLElement* /* element containing this type */> ElementCacheType;

class CyXMLObject;
class CyXMLCommandItem;

class FileStorage
{
public:
	FileStorage(const TCHAR* szName, tinyxml2::XMLDocument *Document);

	const TCHAR* getName()               const {return m_szName.c_str();};
	tinyxml2::XMLDocument* getDocument() const {return m_Document;};

	CvString m_szName;
	tinyxml2::XMLDocument *m_Document;
};


class xmlFileContainer
{
public:
	xmlFileContainer();
	xmlFileContainer(tinyxml2::XMLElement *Description);
	~xmlFileContainer();

	void getSchema(tinyxml2::XMLDocument *pDocument);

	bool isInMod() const {return m_bIsInMod;}

	virtual const TCHAR* getDir() const;
	const TCHAR* getName() const {return getDesc("Name");}
	const TCHAR* getRoot() const {return getDesc("Root");}
	virtual const TCHAR* getTag() const;
	
	int getNumTags()                  {return m_szTags.size();};
	const TCHAR* getTag(int iIndex, bool bSkipPrefix = false);

	void setTags(tinyxml2::XMLElement *pElement);

	tinyxml2::XMLElement* getElement(const TCHAR *szTag) const;
	tinyxml2::XMLElement* getList(int iIndex = 0) const;

	bool isCombo() const;

	void writeFile(int iIndex);

	void writeAllFiles();

	// clear all type strings and set them up again
	void setAllTags();

	void renameType(const TCHAR* szFrom, const TCHAR* szTo);

	const TCHAR* getPrefix()       const {return m_szPrefix.c_str();};
	int          getPrefixLength() const {return m_szPrefix.size() ;};

protected:
	bool renameTypeRecursive(tinyxml2::XMLElement* pElement, const TCHAR* szFrom, const TCHAR* szTo);

	void comboConstructor();

	bool openFile(tinyxml2::XMLDocument *pFile, const TCHAR* szFileName);

	const TCHAR* getDesc(const TCHAR* szTag) const;
	void postSetTags();

	bool m_bIsInMod;

	std::vector<FileStorage> m_vectorFiles;
	tinyxml2::XMLElement  *m_Description;

	CvString m_szSchemaName;
	CvString m_szPrefix;

	std::vector<CvString> m_szTags;
	stdext::hash_map<std::string /* type name */, int /* info index */> m_TagCache;
	ElementCacheType m_ElementCache;
};

class GlobalTypeContainer: public xmlFileContainer
{
public:
	GlobalTypeContainer(tinyxml2::XMLElement *Description);

	const TCHAR* getDir() const;
	const TCHAR* getTag() const;
};

class CyXMLEditor
{
public:
	CyXMLEditor();
	~CyXMLEditor();

	inline const TCHAR* getModPath() const { return m_modPath; }
	inline const TCHAR* getDLLPath() const { return m_dllPath; }

	void setModPath();
	void setDLLpath();
	tinyxml2::XMLDocument* getModSettings();

	bool openFile(const TCHAR* szFileName, tinyxml2::XMLDocument *pDoc, bool bIgnoreFileNotFound = false) const;
	bool openFile(const TCHAR* szFileName);
	void openKeyboard();
	void writeFile(bool bInfo = false) const;

	// sets the types in the active file
	void setTypes(const TCHAR* szOld, const TCHAR* szNew);

	int getFileIndex(const TCHAR* szTagName) const;

	tinyxml2::XMLElement* getRoot(const TCHAR* szRootName, tinyxml2::XMLDocument *pDoc = NULL) const;

	tinyxml2::XMLElement* getSchemaElement(const char *szType);

	tinyxml2::XMLDocument* getDocument() const {return m_Document;}

	xmlFileContainer* getFileContainer(const TCHAR* szTag) const;
	xmlFileContainer* getCurrentFileContainer() const;

	// info interface
	// returns NULL unless the requested tag/setting is found
	const TCHAR* getInfo(const TCHAR* szTag, const TCHAR* szSetting) const;

	// write settings for a tag
	// except for szTag, use NULL whenever a setting is unused
	void setInfo(bool bFileSpecific, const TCHAR* szTag, const TCHAR* szType, const TCHAR* szHelp, const TCHAR* szClass, bool bAllowTypeNone, bool bRemoteCreate, bool bRemoteCreatePrefix, const TCHAR* szButtonChild);

	// get the first GameFont of group in question. Returns 0 if not found
	int getGameFontGroupStart(int iGameFontGroup) const;

	// get art string for the button icon
	// Note: NULL return possible
	const TCHAR* getButtonArt(const tinyxml2::XMLElement* pElement) const;

	// get which combo subfile a string belongs to
	int getComboFileIndex(int iFile, const TCHAR* szString) const;

	// singleton instance
	static CyXMLEditor* getInstance() { return m_pInstance; }
	
	//
	// python exposed
	//

	int getNumFiles() const;
	const TCHAR* getFileName(int iIndex) const;
	int getActiveFile() const;
	void setActiveFile(int iIndex);
	int getNumTypes(int iFile) const;
	int getComboFile(int iFile, int iIndex) const;

	// tells if a specific file is a combo of multiple files
	bool isCombo(int iFile) const;

	const TCHAR* getType(int iFile, int iIndex) const;
	const TCHAR* getTypeNoPrefix(int iFile, int iIndex) const;
	const TCHAR* getFilePrefix(int iFile) const;
	const TCHAR* getFileTag(int iFile) const;
	const TCHAR* getFileDir(int iFile) const;
	const TCHAR* getXMLDir(int iDir) const;
	int getXMLNumDir() const;
	CyXMLObject* getList();

	int getMaxGameFontID() const {return m_iMaxGameFontID;}

	const TCHAR* getKey(int iKey, bool bShift, bool bControl, bool bAlt) const;
	std::wstring getWKey(int iKey, bool bShift, bool bControl, bool bAlt) const;

	// returns the GameFont ID with group info from a GameFont ID without group info
	int getGameFontGroupIndex(int iGameFontID) const;

	// tells if active file is GameFontInto
	bool isGameFontFile() const;

	// get the lowest group start ID, which is higher than iHigherThanThis
	int getNextGameFontGroupStart(int iHigherThanThis) const;

	// convert a ColorInfo string to a color
	NiColorA getColor(const char *ColorType) const;

	// convert a PlayerColorInfo to a color. iIndex is 0=primary, 1=secondary, 2=text
	NiColorA getPlayerColor(const char *PlayerColorType, int iIndex) const;

	// write all XML files
	void writeAllFiles();

	const TCHAR* getIcon(const TCHAR* szIcon) const;

	// used to tell the kind of exe file the game use
	bool isColonizationExe() const;

	// tell if the editor should mod files in the same mod or a remote mod
	bool isEditorInMod() const;

	// get the keyboard layout string
	const TCHAR* getKeyboard() const;

	void resetKeyboard();
	void setKeyboardKey(std::wstring szKey, int iIndex, bool bShift, bool bControl, bool bAlt);

	// get the unicode version of cp1252 keys
	std::wstring toUnicode(int iKey) const;

	// removes tags, which have been removed in the schema file
	void cleanActiveFile();

	// copy graphics files from mod to editor
	void copyFiles();

	// is key held down
	bool altKey()     const;
	bool shiftKey()   const;
	bool ctrlKey()    const;
	bool scrollLock() const;
	bool capsLock()   const;
	bool numLock()    const;

	// quit the game
	void quit() const;

	const CyXMLCommandItem* getCommandItem(const char* name) const;

private:
	void cleanActiveFileRecursive(tinyxml2::XMLElement *pParent);

	const TCHAR* getKeyInternal(int iKey, bool bShift, bool bControl, bool bAlt, bool bWide) const;
	tinyxml2::XMLElement* gotoList(int iFile, tinyxml2::XMLDocument *pDoc = NULL);

	void createEditorFiles() const;
	std::vector<std::string> getFiles(const TCHAR* path, const TCHAR* prefix = "") const;
	tinyxml2::XMLElement* generateFileElementForEditorFiles(std::string path) const;

	void copyDir(const char *dir, const char* extension) const;

	void readActiveFile();
	void writeActiveFile() const;

	static CyXMLEditor* m_pInstance;

	int m_iActiveFile;
	int m_iActiveModFile;
	tinyxml2::XMLElement  *m_pFileSpecificInfo;
	tinyxml2::XMLDocument *m_Document;
	tinyxml2::XMLDocument *m_GlobalTypes;
	tinyxml2::XMLDocument *m_Schema;
	CvString               m_szSchemaFileName;
	tinyxml2::XMLDocument *m_Info;
	CvString               m_szInfoFileName;
	TCHAR *m_dllPath;
	TCHAR *m_modPath;
	tinyxml2::XMLDocument *m_ModSettingsDoc;
	std::vector<xmlFileContainer*> m_szFiles;

	tinyxml2::XMLDocument *m_DocCommandFile;

	int m_iMaxGameFontID;

	TCHAR *m_szKeyboard;

	tinyxml2::XMLDocument *m_Keyboard;
	std::vector<tinyxml2::XMLElement*> m_vectorKeys;
	std::vector<CvString> m_vectorDirs;


	typedef stdext::hash_map<std::string /* file type name */, xmlFileContainer*> ContainerCacheType;
	ContainerCacheType m_ContainerCache;

	stdext::hash_map<std::string /* element name */, tinyxml2::XMLElement* /* info index */> m_SchemaCache;
	stdext::hash_map<std::string /* element name */, tinyxml2::XMLElement* /* info index */> m_InfoCache;

	// cache for linking a file type like UnitInfo to a file index
	typedef stdext::hash_map<std::string /* file type name */, int /* file index */> FileTypeCacheType;
	FileTypeCacheType m_FileTypeCache;
};

class CyXMLObject
{
public:
	CyXMLObject();
	CyXMLObject(tinyxml2::XMLElement *pXML, CyXMLObject *pXMLparent, tinyxml2::XMLElement *pSchema, tinyxml2::XMLElement *pSchemaParent, CyXMLEditor *pEditor);

	tinyxml2::XMLElement* FirstChildElement(const TCHAR *szName) const;

	// allocate an xml object and inserting it into the parent
	void allocate();

	// inserting an object in the location appointed by the schema
	void insertChild(tinyxml2::XMLElement *pXML, tinyxml2::XMLElement *pChildSchema);

	// note: allocate() calls insertChild()
	// insertChild() called allocate() if needed
	// this means it will walk back through the tree and allocate everything until it finds an allocated tag 

	// get the previous element of the same name
	// note: this returns NULL if it's the first element in the list or not a list element
	tinyxml2::XMLElement* PreviousElement() const;

	// get the document to add elements to
	tinyxml2::XMLDocument* getDocument() const;

	//
	// python exposed
	//

	CyXMLObject* getNextSiblingSameName();
	CyXMLObject* getFirstSchemaChild();
	CyXMLObject* next();

	///
	/// get info from schema
	/// (always available)
	///

	// tells if the tag is present in the XML file
	bool isAllocated() const;

	// check if it's a bool
	bool isBool() const;

	// check if it's an int
	bool isInt() const;

	// tells if the element has MaxOccur=*
	bool isListElement() const;

	// tells if the Type selector is a file combo
	bool isCombo() const;

	// check is the tag has the ability to have children rahter than containing a single value
	bool isDir() const;

	// tells if a element is a string
	bool isString() const;

	// returns true if the tag is NOT optional
	// returns false if any parent is optional and not allocated
	bool isMandatory() const;

	// tells if minOccurs=0
	bool isOptional() const;

	// tells which type of TXT_KEY it is
	int getTextType() const;

	// tells if it is a TXT_KEY
	bool isText() const;

	// tells if the schema allows the element to be deleted
	// returns false if the element isn't allocated
	bool canDelete() const;

	// the name of the tag
	const TCHAR* getName() const;

	// the GameFont ID. Returns 0 if not allocated or otherwise unavailable
	int getGameFontDisplayID() const;

	// index of file, which the tag points to for types
	const TCHAR* getInfoTypeString() const;

	// index of file, which the tag points to for types
	int getInfoType() const;

	// tells if a Type selector should include type NONE
	bool allowsTypeNone() const;

	bool isRemoteCreate() const;
	bool isRemoteCreatePrefix() const;

	// get help text
	const TCHAR* getHelp() const;

	// get info type text
	const TCHAR* getInfoClass() const;

	// get tag name of schema child at index
	const TCHAR* getSchemaChild(int iIndex) const;

	// get the number of child tags in schema
	int getNumSchemaChildren() const;

	// get the contents of the type tag
	const TCHAR* getParentType() const;

	// setting info for the tag type
	void setInfo(bool bFileSpecific, int iNewFileForType, const TCHAR* szHelp, const TCHAR* szClass, bool bAllowTypeNone, bool bRemoteCreate, bool bRemoteCreatePrefix, const TCHAR* szButtonChild);

	///
	/// get info from XML file
	/// only present when tag is present in the XML file
	///

	// Type is the value of the first child or sibling named Type
	const TCHAR* getType() const;

	// Type is the value of the first child named Type
	const TCHAR* getChildType() const;

	// the string content of the tag
	const TCHAR* getValue() const;

	// bool value (unallocated returns false)
	bool getBoolValue() const;

	// get the string from the contents of a child
	// returns NULL if child doesn't exist
	const TCHAR* getChildString(const TCHAR* szChildName) const;

	// get art string for the button icon
	// Note: NULL return possible
	const TCHAR* getButtonArt() const;

	// returns the tag name of the child used to set button art
	// returns NULL if none is set
	const TCHAR* getButtonArtChild() const;

	// the index of the combo file
	// returns -1 if unallocated
	int getActiveComboFile() const;

	std::wstring getText() const;

	///
	/// write data to the tag
	///

	// set a new string
	void setValue(const TCHAR* szNewValue);

	// create a new remote entry and then assign the type value to this element
	void createRemote();

	// write a GameFont char
	void setGameFontChar(int iNewValue);

	// delete the element from XML
	void deleteXMLEntry();

	// drag element onto another object
	void dragTo(CyXMLObject *pDest);

	// clone the list element
	void clone();

private:
	void cloneRecursive(tinyxml2::XMLElement* pOriginal, tinyxml2::XMLElement* pClone, const TCHAR* szNewType);

	tinyxml2::XMLElement *m_pXML;
	CyXMLObject          *m_pXMLparent;
	tinyxml2::XMLElement *m_pSchema;
	tinyxml2::XMLElement *m_pSchemaParent;
	CyXMLEditor          *m_pEditor;
};

class CyXMLCommandItem
{
public:
	CyXMLCommandItem();
	CyXMLCommandItem(const char *szName, const char *szPopupHelp, const char *szFullText);

	const char* getName() const;
	const char* getHelp() const;
	const char* getText() const;

private:
	const char *m_szName;
	const char *m_szHelp;
	const char *m_szText;
};

#endif
