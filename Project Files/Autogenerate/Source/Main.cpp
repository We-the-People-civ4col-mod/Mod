//#include <stdio.h>

#include "EnumGen.h"
#include "EnumMap.h"
#include "FileAccess.h"


void readXMLFiles()
{
	FileAccessXML xml_files("Files.xml");

	Element file = xml_files.getRoot();
	for (file = file.FirstChild(); file.isValid(); file = file.NextSibling())
	{
		EnumGen test(file);
		test.writeFile();
	}
}


int main()
{
	TextFile::setup();
	EnumMapGen EnumMaps;

	readXMLFiles();

	return 0;
}

void my_assert(const char* file, const char* function, int line)
{
	printf("%s(%i): Assertion failed in %s\n", file, line, function);
	exit(1);
}