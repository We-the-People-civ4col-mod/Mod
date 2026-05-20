#include "../CvGameCoreDLL.h"
#include "../tinyxml2.h"
#include "XMLUpdateEngine.h"

XMLUpdateEngine::XML_tag::XML_tag(tinyxml2::XMLElement* pElement)
	: m_pElement(pElement)
{
}


XMLUpdateEngine::XML_tag XMLUpdateEngine::XML_tag::findChild(const char* name, const char* text) const
{
	tinyxml2::XMLElement* element = (tinyxml2::XMLElement*)m_pElement->FirstChildElement(name);

	if (text == NULL)
	{
		return element;
	}

	for (; element != NULL; element = element->NextSiblingElement(name))
	{
		if (strcmp(element->GetText(), text) == 0)
		{
			return element;
		}
	}


	return XML_tag(NULL);
}

XMLUpdateEngine::XML_tag XMLUpdateEngine::XML_tag::findOrCreateChild(const char* name, const char* text) const
{ 
	XML_tag elementTag = findChild(name, text);
	if (elementTag.isPresent())
	{
		return elementTag;
	}

	tinyxml2::XMLElement* element = ((tinyxml2::XMLElement*)m_pElement)->GetDocument()->NewElement(name);
	((tinyxml2::XMLElement*)m_pElement)->InsertEndChild(element);
	
	if (text != NULL)
	{
		element->SetText(text);
	}

	return XML_tag(element);
}

void XMLUpdateEngine::XML_tag::rename(const char* newName)
{
	m_pElement->SetName(newName);
}

void XMLUpdateEngine::XML_tag::deleteElement()
{
	if (!isPresent())
	{
		return;
	}

	m_pElement->Parent()->DeleteChild(m_pElement);
}

void XMLUpdateEngine::XML_tag::moveInto(XMLUpdateEngine::XML_tag newParent)
{
	newParent.m_pElement->InsertEndChild(m_pElement);
}

void XMLUpdateEngine::XML_tag::moveOverwriteInto(XMLUpdateEngine::XML_tag newParent)
{
	tinyxml2::XMLElement* child = newParent.m_pElement->FirstChildElement(m_pElement->Name());
	if (child == NULL)
	{
		moveInto(newParent);
		return;
	}

	newParent.m_pElement->InsertAfterChild(child, m_pElement);
	newParent.m_pElement->DeleteChild(child);
}

bool XMLUpdateEngine::XML_tag::isPresent() const
{
	return m_pElement != NULL;
}

const char* XMLUpdateEngine::XML_tag::getText() const
{
	return m_pElement->GetText();
}

void XMLUpdateEngine::XML_tag::setText(const char* newText)
{
	m_pElement->SetText(newText);
}

bool XMLUpdateEngine::XML_tag::is(const char* text) const
{
	return strcmp(text, getText()) == 0;
}

bool XMLUpdateEngine::XML_tag::isNone() const
{
	return is("NONE");
}

bool XMLUpdateEngine::XML_tag::next()
{
	if (m_pElement != NULL)
	{
		m_pElement = m_pElement->NextSiblingElement(m_pElement->Name());
	}

	return m_pElement != NULL;
}

XMLUpdateEngine::XMLUpdateEngine(const char* file)
	: m_filePath(std::string(GetDLLPath()).append("/XML/").append(file))
	, m_file(new tinyxml2::XMLDocument())
	, m_schema(new tinyxml2::XMLDocument())
{
	m_file->LoadFile(m_filePath.c_str());
	std::string::size_type pos = std::string(m_filePath).find_last_of("\\/");
	std::string schemafile(std::string(m_filePath).substr(0, pos));

	const tinyxml2::XMLAttribute* rootAttribute = m_file->FirstChildElement()->FirstAttribute();
	FAssert(strcmp(rootAttribute->Name(), "xmlns") == 0);
	schemafile.append("/").append(std::string(rootAttribute->Value()).substr(9));
	m_schema->LoadFile(schemafile.c_str());

	tinyxml2::XMLElement* loopElement = m_file->FirstChildElement();
	for (; loopElement->NextSiblingElement(loopElement->Name()) == NULL; loopElement = loopElement->FirstChildElement())
	{
	}

	for (; loopElement != NULL; loopElement = loopElement->NextSiblingElement())
	{
		updateEntry(loopElement);
	}
}

XMLUpdateEngine::~XMLUpdateEngine()
{
	updateAccordingToSchema();
	m_file->SaveFile(m_filePath.c_str());
	SAFE_DELETE(m_file);
	SAFE_DELETE(m_schema);
}

void XMLUpdateEngine::updateAccordingToSchema()
{
	updateAccordingToSchema(m_file->FirstChildElement());
}

void XMLUpdateEngine::updateAccordingToSchema(tinyxml2::XMLElement* pElement)
{
	tinyxml2::XMLElement* schema = getSchema(pElement);

	FAssert(schema != NULL);

	if (schema->NoChildren())
	{
		// ignore elements, which doesn't have child elements
		return;
	}



	tinyxml2::XMLElement* schema_child = schema->FirstChildElement("element");

	const char* max = schema_child->Attribute("maxOccurs");

	if (max != NULL && strcmp(max, "*") == 0)
	{
		// we are dealing with a list
		FAssert(schema_child->NextSiblingElement() == NULL);

		const char* name = schema_child->Attribute("type");

		for (tinyxml2::XMLElement* loopElement = pElement->FirstChildElement(); loopElement != NULL; loopElement = loopElement->NextSiblingElement())
		{
			FAssertMsg(strcmp(loopElement->Name(), name) == 0, CvString::format("Expected tag %s at line %d", name, loopElement->GetLineNum()).c_str());
			updateAccordingToSchema(loopElement);
		}

		return;
	}

	tinyxml2::XMLElement* target_child = NULL;

	for (; schema_child != NULL; schema_child = schema_child->NextSiblingElement("element"))
	{
		const char* min = schema_child->Attribute("minOccurs");
		const bool bSkip = min != NULL && strcmp(min, "0") == 0;

		tinyxml2::XMLElement* target_temp = pElement->FirstChildElement(schema_child->Attribute("type"));

		if (target_temp != NULL)
		{
			if (target_child == NULL)
			{
				pElement->InsertFirstChild(target_temp);
			}
			else if (target_child->NextSiblingElement() != target_temp)
			{
				pElement->InsertAfterChild(target_child, target_temp);
			}
			target_child = target_temp;
			updateAccordingToSchema(target_child);
		}
		else if (!bSkip)
		{
			if (target_child == NULL)
			{
				FAssertMsg(false, CvString::format("Expected tag %s at start of %s at line %d", schema_child->Attribute("type"), pElement->Name(), pElement->GetLineNum()).c_str());
			}
			else
			{
				FAssertMsg(false, CvString::format("Expected tag %s at line %d", schema_child->Attribute("type"), target_child->GetLineNum()).c_str());
			}
		}
	}
}

tinyxml2::XMLElement* XMLUpdateEngine::getSchema(tinyxml2::XMLElement* element)
{
	return getSchema(element->Name());
}

tinyxml2::XMLElement* XMLUpdateEngine::getSchema(const char* name)
{
	tinyxml2::XMLElement* element = m_schema->FirstChildElement()->FirstChildElement("ElementType");

	for (; element != NULL; element = element->NextSiblingElement("ElementType"))
	{
		const char* loopName = element->Attribute("name");
		if (strcmp(loopName, name) == 0)
		{
			return element;
		}
	}
	FAssertMsg(false, "invalid schema file");
	return NULL;
}