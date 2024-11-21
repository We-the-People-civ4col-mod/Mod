#pragma once
#include "LineString.h"
#include <fstream>

#include "tinyxml2.h"

namespace FileLocation
{
	enum locations
	{
		FILE_LOCATION_ROOT,
		FILE_LOCATION_SOURCE,
		FILE_LOCATION_AUTO,
		FILE_LOCATION_XML,

	};
};

class Element
{
	friend class FileAccessXML;
public:
	bool isValid() const;
	const char* name() const;

	int numChildren() const;
	bool hasMultipleChildren() const; // faster than full count

	Element FirstChild(const char* name = NULL);
	Element NextSibling(const char* name = NULL);

	bool getBool() const;
	int getInt() const;
	const char* getText() const;

private:
	Element(const tinyxml2::XMLElement *pElement);

	const tinyxml2::XMLElement* m_pElement;
};

class FileAccessXML
{
public:
	FileAccessXML(const char* FileName);

	Element getRoot() const;

private:
	const char * const m_Filename;

	tinyxml2::XMLDocument m_Doc;
	tinyxml2::XMLElement* m_pElement;
};

class TextFile 
{
public:
	TextFile(const char* filename, FileLocation::locations location);
	~TextFile();

	LineString readLine();
	unsigned int getLineNumber() const;
	bool hasLinesLeft() const;

	static void setup();

private:
	std::ifstream file;
	unsigned int line_number;
};
