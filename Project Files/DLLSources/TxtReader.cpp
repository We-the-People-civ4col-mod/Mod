#include "CvGameCoreDLL.h"
#include "tinyxml2.h"
#include "TxtReader.h"


void TxtReader::readFiles()
{
	CvString directory = gDLL->getModName();
	directory.append("\\Assets\\XML\\Text\\");

	HANDLE dir;
	WIN32_FIND_DATA file_data;

	if ((dir = FindFirstFile((directory + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
		return; /* No files found */

	do {
		const std::string file_name = file_data.cFileName;
		const std::string full_file_name = directory + "/" + file_name;
		const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

		if (file_name[0] == '.')
			continue;

		if (is_directory)
			continue;

		const std::string ext = file_name.substr(file_name.size() - 4);

		if (strcmp(ext.c_str(), ".xml") != 0)
			continue;

		CvString name = directory;
		name.append("\\");
		name.append(file_name);

		readFile(name);

	} while (FindNextFile(dir, &file_data));

	FindClose(dir);
}

void TxtReader::readFile(const char* pFile)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(pFile);
	tinyxml2::XMLElement* root = doc.RootElement();
	if (strcmp(root->Name(), "Civ4GameText") != 0)
		root = root->NextSiblingElement("Civ4GameText");
	root = root->FirstChildElement("TEXT");

	const int languageID = gDLL->getCurrentLanguage();
	const char* language = CvGameText::getLanguageName(languageID);

	for (; root != NULL; root = root->NextSiblingElement("TEXT"))
	{
		tinyxml2::XMLElement* tag = root->FirstChildElement("Tag");
		tinyxml2::XMLElement* text = root->FirstChildElement(language);
		if (text == NULL)
		{
			text = root->FirstChildElement("English");
		}
		if (text == NULL || tag == NULL)
		{
			// todo error message for malformated xml file
			continue;
		}

		tinyxml2::XMLElement* gender = text->FirstChildElement("Gender");
		tinyxml2::XMLElement* plural = NULL;
		if (gender != NULL)
		{
			plural = text->FirstChildElement("Plural");
			text = text->FirstChildElement("Text");
		}

		CvWString convertedText = CvGameText::convertFromUTF8(text->GetText(), false, pFile, tag->GetText());
		CvWString convertedGender = gender ? CvWString(gender->GetText()) : L"N";
		CvWString convertedPlural = gender ? CvWString(gender->GetText()) : L"false";

		gDLL->addText(tag->GetText(), convertedText.c_str(), convertedGender, convertedPlural);
	}
}
