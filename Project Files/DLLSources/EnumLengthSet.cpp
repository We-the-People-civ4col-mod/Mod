#include "FAssert.h"

#include <string>
#include "EnumLengthSet.h"
#include "lib/tinyxml2/tinyxml2.h"

EnumLengthSet::EnumLengthSet(const std::string path)
	: m_basePath(path)
{
	m_path = path.c_str();
	std::string fileXML = path;
	fileXML.append("/XML//Files.xml");
	m_doc = new tinyxml2::XMLDocument;
	m_doc->LoadFile(fileXML.c_str());
	m_files = m_doc->FirstChildElement();
	
	setup();
}

EnumLengthSet::~EnumLengthSet()
{
	free(m_doc);
}

int EnumLengthSet::getTypeLength(const char* type) const
{
	unsigned int iCount = 0;

	const tinyxml2::XMLElement* file = m_files->FirstChildElement(type);
	if (file != NULL)
	{
		file = file->FirstChildElement("Files");
		for (file = file->FirstChildElement("File"); file != NULL; file = file->NextSiblingElement("File"))
		{
			iCount += getTypeLength(file);
		}
	}

	return iCount;
}

int EnumLengthSet::getTypeLength(const tinyxml2::XMLElement* filename) const
{
	std::string path = filename->GetText();

	tinyxml2::XMLDocument doc;
	openFile(doc, path.c_str());

	tinyxml2::XMLElement* root = doc.FirstChildElement();
	
	int iOffset = path.find_last_of("/");
	if (iOffset == -1)
	{
		path = "";
	}
	else
	{
		path = path.substr(0, iOffset + 1);
	}
	std::string schemaPath = root->Attribute("xmlns");
	schemaPath = schemaPath.substr(schemaPath.find(":") + 1);
	path.append(schemaPath.c_str());
	tinyxml2::XMLDocument schema;
	openFile(schema, path.c_str());
	
	unsigned int iCount = 0;

	tinyxml2::XMLElement* firstSchema = schema.FirstChildElement()->FirstChildElement();
	tinyxml2::XMLElement* loopElement = firstListElement(root, firstSchema);

	for (; loopElement != NULL; loopElement = loopElement->NextSiblingElement())
	{
		++iCount;
	}

	return iCount;
}

tinyxml2::XMLElement* EnumLengthSet::firstListElement(tinyxml2::XMLElement* currentElement, tinyxml2::XMLElement* schemaElement) const
{
	if (currentElement == NULL || schemaElement == NULL) return NULL;

	const char* tagName = currentElement->Name();
	currentElement = currentElement->FirstChildElement();

	tinyxml2::XMLElement* loopElement = schemaElement;
	//loopElement->Attribute("name");
	for (; loopElement != NULL && strcmp(tagName, loopElement->Attribute("name")) != 0; loopElement = loopElement->NextSiblingElement())
	{
	}

	if (loopElement == NULL) return NULL;

	tinyxml2::XMLElement* schemaChild = loopElement->FirstChildElement("element");


	const char* maxOccurs = schemaChild->Attribute("maxOccurs");
	if (maxOccurs != NULL && strcmp("*", maxOccurs) == 0)
	{
		return currentElement;
	}
	return firstListElement(currentElement, schemaElement);
}

void EnumLengthSet::openFile(tinyxml2::XMLDocument& doc, const char* file) const
{
	std::string fileXML = m_path;
	fileXML.append("/XML/");
	fileXML.append(file);
	doc.LoadFile(fileXML.c_str());

	if (doc.Error() != tinyxml2::XML_SUCCESS)
	{
		// file failed to load. Try vanilla instead
		fileXML = "Assets";
		fileXML.append("/XML/");
		fileXML.append(file);
		doc.LoadFile(fileXML.c_str());
	}
}

void EnumLengthSet::verifyTypeLength(const char* type, int iLength) const
{
	FAssert(iLength == getTypeLength(type));
}
