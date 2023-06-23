#include "CvGameCoreDll.h"
#include "AlertWindow.h"
#include "StartupErrorChecking.h"
#include "OpenURL.h"
#include "SavegameAlert.h"

#include <iostream>
#include <sstream>
#include <fstream>

enum WindowType
{
	WINDOW_DLL_LOCATION,
	WINDOW_NO_INI_FILE,
	WINDOW_PUBLIC_MAPS_DISABLED,
	WINDOW_LARGE_ADDRESS_AWARE,
	WINDOW_WRONG_STEAM_VERION,
	WINDOW_XML_HARDCODE_MISMATCH,
};

template <int T>
static AlertWindow::returnTypes showWindow()
{
	// only specialized templates should be used
	BOOST_STATIC_ASSERT(0);
}

template <int T>
static AlertWindow::returnTypes showWindow(const char*)
{
	// only specialized templates should be used
	BOOST_STATIC_ASSERT(0);
}

template <int T>
static AlertWindow::returnTypes showWindow(const char*, int, int)
{
	// only specialized templates should be used
	BOOST_STATIC_ASSERT(0);
}


template <>
static AlertWindow::returnTypes showWindow<WINDOW_DLL_LOCATION>()
{
	AlertWindow window;

	window.header = "TXT_KEY_ALERT_MOD_IN_DOCUMENTS_HEADER";
	window.message = "TXT_KEY_ALERT_MOD_IN_DOCUMENTS";
	window.setIcon(AlertWindow::iconTypes::IconError);
	window.setButtonLayout(AlertWindow::Buttons::BtnAboutRetryIgnore);
	return window.openWindow();
}

template <>
static AlertWindow::returnTypes showWindow<WINDOW_NO_INI_FILE>(const char* iniFile)
{
	AlertWindow window;

	window.header = "TXT_KEY_ALERT_CONFIGURATION_ERROR_HEADER";
	window.message = "TXT_KEY_ALERT_CONFIGURATION_ERROR";
	window.setIcon(AlertWindow::iconTypes::IconError);
	window.setButtonLayout(AlertWindow::Buttons::BtnAboutRetryIgnore);
	window.addArgument(iniFile);
	return window.openWindow();
}

template <>
static AlertWindow::returnTypes showWindow<WINDOW_PUBLIC_MAPS_DISABLED>()
{
	AlertWindow window;

	window.header = "TXT_KEY_ALERT_PUBLIC_MAPS_DISABLED_HEADER";
	window.message = "TXT_KEY_ALERT_PUBLIC_MAPS_DISABLED";
	window.setButtonLayout(AlertWindow::Buttons::BtnYesNo);
	return window.openWindow();
}

template <>
static AlertWindow::returnTypes showWindow<WINDOW_LARGE_ADDRESS_AWARE>()
{
	AlertWindow window;

	window.header = "TXT_KEY_ALERT_LARGE_ADDRESS_AWARE_HEADER";
	window.message = "TXT_KEY_ALERT_LARGE_ADDRESS_AWARE";
	window.setIcon(AlertWindow::iconTypes::IconWarning);
	window.setButtonLayout(AlertWindow::Buttons::BtnYesNo);
	return window.openWindow();
}

template <>
static AlertWindow::returnTypes showWindow<WINDOW_WRONG_STEAM_VERION>()
{
	AlertWindow window;

	window.header = "TXT_KEY_ALERT_WRONG_STEAM_VERION_HEADER";
	window.message = "TXT_KEY_ALERT_WRONG_STEAM_VERION";
	window.setIcon(AlertWindow::iconTypes::IconError);
	window.setButtonLayout(AlertWindow::Buttons::BtnYesNo);
	return window.openWindow();
}

template <>
static AlertWindow::returnTypes showWindow<WINDOW_XML_HARDCODE_MISMATCH>(const char* type, int iSizeHardcoded, int iSizeXML)
{
	AlertWindow window;

	window.header = "TXT_KEY_ALERT_XML_HARDCODE_MISMATCH_HEADER";
	window.message = "TXT_KEY_ALERT_XML_HARDCODE_MISMATCH";
	window.setIcon(AlertWindow::iconTypes::IconError);
	window.addArgument(type);
	window.setMessageArguments(iSizeHardcoded, iSizeXML);
	window.setButtonLayout(AlertWindow::Buttons::BtnYesNo);
	return window.openWindow();
}

static void TestDLLLocation()
{
	std::string name_exe = GetDLLPath(false);
	std::string name_dll = GetDLLPath(true);
	name_dll.resize(name_exe.size());

	if (name_exe != name_dll)
	{
		AlertWindow::returnTypes returnValue = showWindow<WINDOW_DLL_LOCATION>();
		if (returnValue == returnValue.clickedAbout)
		{
			exit(1);
		}
	}
}

static void checkPublicMapSetting()
{
	const std::string modPath = gDLL->getModName();
	const std::string modName = modPath.substr(5, modPath.length() - 6);
	const std::string iniFile = modPath + modName + ".ini";

	std::vector<std::string> file_content;

	try
	{
		std::ifstream input(iniFile.c_str());
		std::stringstream buffer;
		buffer << input.rdbuf();
		input.close();

		for (std::string line; std::getline(buffer, line); )
		{
			file_content.push_back(line);
			if (line.substr(0, 15) == "AllowPublicMaps")
			{
				if (line == "AllowPublicMaps = 0")
				{
					return;
				}
			}
		}
	}
	catch (const std::exception&)
	{
		// make the game silently ignore file read crashes
		return;
	}

	try
	{
		std::ofstream output(iniFile.c_str());
		for (unsigned i = 0; i < file_content.size(); i++)
		{
			const std::string& line = file_content[i];
			if (line.substr(0, 15) == "AllowPublicMaps")
			{
				output << "AllowPublicMaps = 0" << std::endl;
			}
			else
			{
				output << line << std::endl;
			}
		}
		output.close();
	}
	catch (const std::exception&)
	{
		AlertWindow::returnTypes returnVal = showWindow<WINDOW_NO_INI_FILE>(iniFile.c_str());
		switch (returnVal)
		{
		case AlertWindow::returnTypes::clickedAbout: exit(0);
		case AlertWindow::returnTypes::clickedRetry: 
			checkPublicMapSetting();
			return;
		case AlertWindow::returnTypes::clickedIgnore:
			break;
		default:
			FAssertMsg(0, "Unmatched case");
		}

		return;
	}

	{
		AlertWindow::returnTypes returnVal = showWindow<WINDOW_PUBLIC_MAPS_DISABLED>();
		if (returnVal == returnVal.clickedYes)
		{
			exit(0);
		}
	}
}

static void checkLargeAddressAwareness()
{
	// a bit of code to tell precisely how much memory can be allowcated.
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	unsigned __int64 size = statex.ullTotalVirtual;
	bool largeAddressAware = size > 0xF0000000;

	if (!largeAddressAware)
	{
		AlertWindow::returnTypes returnVal = showWindow<WINDOW_LARGE_ADDRESS_AWARE>();
		if (returnVal == returnVal.clickedYes)
		{
			OpenURL::openReadme("ramProblem");
			exit(0);
		}
	}
}

static void checkSteam()
{
	std::ifstream exe("Colonization.exe", std::ios::in | std::ios::binary);
	exe.seekg(0x48);

	std::vector<byte> wrong_steam_version;
	wrong_steam_version.push_back(0x10);
	wrong_steam_version.push_back(0x9C);
	wrong_steam_version.push_back(0x00);
	wrong_steam_version.push_back(0xC0);
	wrong_steam_version.push_back(0xAF);
	wrong_steam_version.push_back(0x86);
	wrong_steam_version.push_back(0x53);

	for (unsigned int i = 0; i < wrong_steam_version.size(); ++i)
	{
		char buffer;
		exe.read(&buffer, 1);
		if (buffer != wrong_steam_version[i])
		{
			return;
		}
	}
	AlertWindow::returnTypes returnVal = showWindow<WINDOW_WRONG_STEAM_VERION>();
	if (returnVal == returnVal.clickedYes)
	{
		OpenURL::openReadme("wrongSteam");
		exit(0);
	}
}

void testHardcodedXML();

namespace StartupCheck
{
	void CheckXmlLength(const char* type, int iSizeHardcoded, int iSizeXML)
	{
		if (iSizeHardcoded != iSizeXML)
		{
			AlertWindow::returnTypes returnVal = showWindow<WINDOW_XML_HARDCODE_MISMATCH>(type, iSizeHardcoded, iSizeXML);
			if (returnVal == returnVal.clickedYes)
			{
				exit(0);
			}
		}
	}

	void testAllWindows()
	{
		AlertWindow window;

		window.header = "Alert window testing";
		window.message = "Displaying all the possible startup window alerts";
		window.openWindow();

		showWindow<WINDOW_DLL_LOCATION>();
		showWindow<WINDOW_NO_INI_FILE>("test file");
		showWindow<WINDOW_PUBLIC_MAPS_DISABLED>();
		showWindow<WINDOW_LARGE_ADDRESS_AWARE>();
		showWindow<WINDOW_WRONG_STEAM_VERION>();
		showWindow<WINDOW_XML_HARDCODE_MISMATCH>("TestTypes", 1, 2);

		SavegameAlert::SavegameTooOld();
		SavegameAlert::SavegameTooNew();

		window.message = "Test complete";
		window.openWindow();
	}

	void GlobalInitXMLCheck()
	{
		FAssert(gDLL != NULL);
		if (GC.shiftKey())
		{
			testAllWindows();
		}

		TestDLLLocation();
		checkPublicMapSetting();
		checkLargeAddressAwareness();
		checkSteam();
		testHardcodedXML();
	}
}
