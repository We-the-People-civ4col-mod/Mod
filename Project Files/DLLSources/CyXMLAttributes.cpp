
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

const TCHAR* CyXMLAttributes::getButtonArt() const
{
	// first report not having found anything unless both pointers have been set
	// this will both provide the correct return value and ensure the rest of the function can use either pointer without checking for NULL
	if (m_pInfo == NULL || m_pXML == NULL)
	{
		return NULL;
	}

	if (isDir())
	{
		const char* childType = m_pInfo->Attribute("child");
		if (childType)
		{
			CyXMLAttributes child(m_pXML->FirstChildElement(childType), m_pInfo->FirstChildElement(childType));
			return child.getButtonArt();
		}
	}
	else if (isButton())
	{
		return m_pXML->GetText();
	}
	else if (isType())
	{
		// switch to the "new" file
		// returning NULL means no art found. Just return NULL if anything doesn't go perfect for stability reasons

		// the Type to look up in the other file
		const TCHAR* pNewTag = m_pXML->GetText();
		if (pNewTag == NULL) return NULL;

		// the enum type representing the "new" file
		const char* type = getType();
		if (type == NULL) return NULL;

		tinyxml2::XMLElement* pNewInfo = m_pInfo->GetDocument()->FirstChildElement(type);
		if (pNewInfo == NULL) return NULL;


		xmlFileContainer* container = CyXMLEditor::getInstance()->getFileContainer(type);

		if (container == NULL) return NULL;

		CyXMLAttributes child(container->getElement(pNewTag), pNewInfo);
		return child.getButtonArt();
	}


	return NULL;
}

const TCHAR* CyXMLAttributes::getButtonArtChild() const
{
	return m_pInfo->Attribute("child");
}

bool CyXMLAttributes::isButton() const
{
	const char* type = m_pInfo->Attribute("class");
	return type != NULL && strcmp(type, "Button") == 0;
}

bool CyXMLAttributes::isDir() const
{
	const char* type = m_pInfo->Attribute("class");
	return type != NULL && strcmp(type, "Dir") == 0;
}

bool CyXMLAttributes::isType() const
{
	const char* type = m_pInfo->Attribute("class");
	return type != NULL && strcmp(type, "Type") == 0;
}

bool CyXMLAttributes::allowsTypeNone() const
{
	return m_pInfo->BoolAttribute("none");
}
const TCHAR* CyXMLAttributes::getHelp() const
{
	return m_pInfo->Attribute("help");
}
const TCHAR* CyXMLAttributes::getInfoClass() const
{
	return m_pInfo->Attribute("class");
}
const TCHAR* CyXMLAttributes::getType() const
{
	return m_pInfo->Attribute("type");
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
