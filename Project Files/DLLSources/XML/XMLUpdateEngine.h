#pragma once

#include <string>

namespace tinyxml2
{
	class XMLDocument;
	class XMLElement;
}

class XMLUpdateEngine
{
public:
	class XML_tag
	{
		friend XML_tag;
	public:
		XML_tag(tinyxml2::XMLElement*);


		XML_tag findChild(const char* name, const char* text = NULL) const;
		XML_tag findOrCreateChild(const char* name, const char* text = NULL) const;
		void rename(const char* newName);
		void deleteElement();
		void moveInto(XML_tag newParent);
		void moveOverwriteInto(XML_tag newParent);
		bool isPresent() const; // findChild can fail and delete will also remove it
		const char* getText() const;
		void setText(const char* newText);
		bool is(const char* text) const;
		bool isNone() const;
		bool next(); // for lists



	private:
		tinyxml2::XMLElement* m_pElement;
	};

	XMLUpdateEngine(const char* file);
	~XMLUpdateEngine();

	void updateEntry(const XML_tag);


private:
	void updateAccordingToSchema();
	void updateAccordingToSchema(tinyxml2::XMLElement*);

	tinyxml2::XMLElement* getSchema(tinyxml2::XMLElement*);
	tinyxml2::XMLElement* getSchema(const char* name);

	const std::string m_filePath;
	tinyxml2::XMLDocument* m_file;
	tinyxml2::XMLDocument* m_schema;
};
