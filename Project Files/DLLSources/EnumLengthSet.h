#pragma once

#include <string>

namespace tinyxml2
{
	class XMLDocument;
	class XMLElement;
}

class EnumLengthSet
{
public:
	EnumLengthSet(const std::string path);
	~EnumLengthSet();

	int getTypeLength(const char* type) const;
	void verifyTypeLength(const char* type, int iLength) const;

private:
	void setup();

	int getTypeLength(const tinyxml2::XMLElement* ) const;
	tinyxml2::XMLElement* firstListElement(tinyxml2::XMLElement* currentElement, tinyxml2::XMLElement* schemaElement) const;

	void openFile(tinyxml2::XMLDocument& doc, const char* file) const;

	const std::string m_basePath;
	tinyxml2::XMLDocument* m_doc;
	const tinyxml2::XMLElement* m_files;
	const char* m_path;
};
