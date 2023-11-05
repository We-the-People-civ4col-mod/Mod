
#include "CvGameCoreDLL.h"

#include "CyXMLEditor.h"
#include "CyXMLAttributes.h"
#include "CyXMLObject.h"

#include "tinyxml2.h"


CyXMLAttributes::CyXMLAttributes(tinyxml2::XMLElement *pXML, tinyxml2::XMLElement *pInfo)
	: m_pXML(pXML)
	, m_pInfo(pInfo)
{}

CyXMLAttributes::CyXMLAttributes(tinyxml2::XMLElement *pXML, CyXMLObject *pXMLparent, tinyxml2::XMLElement *pSchema, tinyxml2::XMLElement *pSchemaParent, CyXMLEditor *pEditor, const TCHAR* szName)
	: m_pXML(pXML)
{
	if (pXMLparent == NULL)
	{
		m_pInfo = pEditor->getInfoRoot();
		return;
	}
	if (pXMLparent->getParent() == NULL && strcmp(szName, pEditor->getInfoRoot()->Name()) == 0)
	{
		m_pInfo = pEditor->getInfoRoot();
		return;
	}

	tinyxml2::XMLElement* parentElement = pXMLparent != NULL ?
		parentElement = pXMLparent->getInfoElement()
		: pEditor->getInfoRoot();

	m_pInfo = parentElement->FirstChildElement(szName);

	if (m_pInfo != NULL)
	{
		return;
	}
	m_pInfo = pEditor->getInfoRoot()->GetDocument()->NewElement(szName);

	parentElement->InsertEndChild(m_pInfo);
	pEditor->writeFile(true);
}

tinyxml2::XMLElement* CyXMLAttributes::getInfo() const
{
	return m_pInfo;
}

void CyXMLAttributes::setInfo(const char* szType, const TCHAR* szHelp, const TCHAR* szClass, bool bAllowTypeNone, const TCHAR* szButtonChild)
{
	setInfo("class", szClass);
	setInfo("type", szType);
	setInfo("help", szHelp);
	setInfo("none", bAllowTypeNone, szType);
	setInfo("child", szButtonChild);
	CyXMLEditor::getInstance()->writeFile(true);
}

void CyXMLAttributes::setInfo(const TCHAR* tag, const TCHAR* value)
{
	if (value == NULL || strlen(value) == 0)
	{
		m_pInfo->DeleteAttribute(tag);
	}
	else
	{
		m_pInfo->SetAttribute(tag, value);
	}
}
void CyXMLAttributes::setInfo(const TCHAR* tag, bool value, const TCHAR* type)
{
	if (type == NULL || strlen(type) == 0)
	{
		m_pInfo->DeleteAttribute(tag);
		return;
	}
	m_pInfo->SetAttribute(tag, value);
}
