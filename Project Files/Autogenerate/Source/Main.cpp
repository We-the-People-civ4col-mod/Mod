#include <stdio.h>

#include "FileAccess.h"
#include "OutputHandler.h"

void FileStart(OutputHandler& text, Element file)
{
	text.printLineNoIndent      ("#pragma once\n");
	text.printLineRemoveLastChar("class ", file.name());
	text.addStartBracket();
	text.printLine              (  "friend class CvXMLLoadUtility;");
	text.printLineNoIndent      ("public:");
	text.printLine              (  "enum types");
	text.addStartBracket();
	text.printLine              (    "NONE = -1");
}

void FileEnd(OutputHandler& text, Element file)
{

	text.printLine("NUM,");
	text.addEndBracket();
	text.addEndBracket();
}

void readXMLFiles()
{
	FileAccessXML xml_files("Files.xml");

	Element file = xml_files.getRoot();
	for (file = file.FirstChild(); file.isValid(); file = file.NextSibling())
	{
		OutputHandler text;
		FileStart(text, file);

		Element current_file = file.FirstChild("Files");
		for (current_file = current_file.FirstChild("File"); current_file.isValid(); current_file = current_file.NextSibling("File"))
		{
			const char* filename = current_file.getText();
			FileAccessXML xml_file(filename);

			Element tag = xml_file.getRoot();

			while (tag.isValid() && !tag.FirstChild("Type").isValid())
			{
				tag = tag.FirstChild();
			}

			for (; tag.isValid(); tag = tag.NextSibling())
			{
				const char* type = tag.FirstChild("Type").getText();
				text.printLine(type, ",");
			}
		}
		FileEnd(text, file);
		text.saveFile(file.name(), ".h");
	}
}


int main()
{
	TextFile::setup();

	readXMLFiles();

	return 0;
}

void my_assert(const char* file, const char* function, int line)
{
	printf("%s(%i): Assertion failed in %s\n", file, line, function);
	exit(1);
}