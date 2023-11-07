#pragma once

#ifndef CyxmlObject_h
#define CyxmlObject_h

#include "CyXMLAttributes.h"

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

	tinyxml2::XMLElement* getInfoElement() const;

	const CyXMLObject* getParent() { return m_pXMLparent; }

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

	//
	int getID() const;

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
	CyXMLAttributes       m_Attributes;
};

#endif
