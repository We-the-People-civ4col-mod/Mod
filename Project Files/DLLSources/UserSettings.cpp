
#include "CvGameCoreDLL.h"
#include "UserSettings.h"

static const int iDefaultCityCatchmentRadius = 2;
static const int iDefaultDebugMaxGameFont = 0;

UserSettings::UserSettings()
	: m_iColonyRadius(iDefaultCityCatchmentRadius)
	, m_iDebugMaxGameFont(iDefaultDebugMaxGameFont)
{
	read();
}

int UserSettings::getColonyRadius() const
{
	return m_iColonyRadius;
}

int UserSettings::getDebugMaxGameFont() const
{
	return m_iDebugMaxGameFont;
}

// open the settings file
// dedicated function for filename consistency
static FILE* openFile(bool bWrite)
{
	CvString filename = gDLL->getModName();
	filename.append("UserSettings.txt");

	return fopen(filename.GetCString(), bWrite ? "w" : "r");
}

// Read specific setting
// Checks all lines for starting with a pattern matching szName
// Read result in value
// Returns true if value NOT read and false if it is found
template<class T>
static bool scanFile(FILE *f, char* szName, T& value)
{
	rewind(f);

	while (true)
	{
		if (fscanf(f, szName, &value))
		{
			return false;
		}

		// string not found on this line. Move to the next
		fscanf(f, "%*s%*[\n]");

		if (feof(f))
		{
			// end fo file reached
			// string not matching any lines in this file
			return true;
		}
	}
}

void UserSettings::read()
{
	bool bFail = true;

	FILE *f = openFile(false);
	if (f)
	{
		bFail = false;

		// try to read all the settings
		// order doesn't matter as all settings will check all lines. Not the fastest approach, but it's the least likely to fail.
		// bFail goes true if at least one failed
		bFail |= scanFile(f, "Default Colony Catchment Radius=%d", m_iColonyRadius);
		bFail |= scanFile(f, "Debug Max Gamefont=%d", m_iDebugMaxGameFont);

		fclose(f);

		// fix invalid settings
		if (m_iColonyRadius != 1 && m_iColonyRadius != 2)
		{
			m_iColonyRadius = iDefaultCityCatchmentRadius;
			bFail = true;
		}
	}

	if (bFail)
	{
		// file or setting missing. Write to make sure the user has a file with all the settings present
		// if some settings were read, then those will be written again
		write();
	}
}

void UserSettings::write()
{
	FILE *f = openFile(true);

	if (f)
	{
		// Writing is more simple than reading. Just write all the settings one by one
		fprintf(f, "User configurable settings\n\n");

		fprintf(f, "\n# Default max catchment radius for colonies");
		fprintf(f, "\n# Can be changed at new game startup, but engine limitations prevents a GUI for this setting in scenarios");
		fprintf(f, "\n# Note: scenarios can ignore this setting and specify radius");
		fprintf(f, "\nDefault Colony Catchment Radius=%d\n", m_iColonyRadius);

		fprintf(f, "\n-----------------------------------------------------------------------------------");
		fprintf(f, "\nSettings below this are intended for modders only");
		fprintf(f, "\nThey serve no gameplay value and are only used to verify that mods work as intended");
		fprintf(f, "\n-----------------------------------------------------------------------------------\n");

		fprintf(f, "\nEnables the debug screen for GameFont");
		fprintf(f, "\n0 = off, 1 = on (autodetection of max value)");
		fprintf(f, "\nValue higher than 8483 will set the max directly");
		fprintf(f, "\nDebug Max Gamefont=%d\n", m_iDebugMaxGameFont);

		fclose(f);
	}
}
