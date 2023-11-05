#pragma once

// handles attributes to each tag
// this is the configuration from the right click windows (storing help text, type configuration etc)
// also designed to fetch art from a child tag

class CyXMLAttributes
{
public:
	CyXMLAttributes(tinyxml2::XMLElement *pXML, tinyxml2::XMLElement *pInfo);
	CyXMLAttributes(tinyxml2::XMLElement *pXML, CyXMLObject *pXMLparent, tinyxml2::XMLElement *pSchema, tinyxml2::XMLElement *pSchemaParent, CyXMLEditor *pEditor, const TCHAR* szName);

	tinyxml2::XMLElement* getInfo() const;

	void setInfo(const char* szType, const TCHAR* szHelp, const TCHAR* szClass, bool bAllowTypeNone, const TCHAR* szButtonChild);

protected:
	void setInfo(const TCHAR* tag, const TCHAR* value);
	void setInfo(const TCHAR* tag, bool value, const TCHAR* type);


	tinyxml2::XMLElement *m_pXML;
	tinyxml2::XMLElement *m_pInfo;
};
