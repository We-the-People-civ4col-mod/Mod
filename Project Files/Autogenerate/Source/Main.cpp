#include "EnumGen.h"
#include "EnumMap.h"
#include "FileAccess.h"
#include "EnumGen.h"
#include "EnumLengthSetupGen.h"


void readXMLFiles()
{
	FileAccessXML xml_files("Files.xml");

	Element file = xml_files.getRoot();
	for (file = file.FirstChild(); file.isValid(); file = file.NextSibling())
	{
		EnumGen test(file);
		test.generateFiles();
	}
}


int main()
{
	TextFile::setup();
	readXMLFiles();

	{
		EnumGen("AreaAI", 7);
		EnumGen("CityPlot", 25, "NUM_CITY_PLOTS");
		EnumGen("Contact", 12);
		EnumGen("Emotion", 5);
		EnumGen("Feat", 10);
		EnumGen("Player", 60);
		EnumGen("Strategy", 13);
		EnumGen("Team", 60);
		EnumGen("WarPlan", 8);
	}

	EnumMapGen EnumMaps;
	EnumLengthSetupGen LengthSetup;

	return 0;
}

void my_assert(const char* file, const char* function, int line)
{
	printf("%s(%i): Assertion failed in %s\n", file, line, function);
	exit(1);
}
