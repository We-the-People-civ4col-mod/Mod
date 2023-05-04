#pragma once


struct OpenURL
{
	static void openURL(const char* URL);
	// section is the part after the # excluding the language string
	// automatically selects language based on what WTP is set to use
	// if no section is given, the file is opened where the language in question starts
	static void openReadme(const std::string section = "Instruction");
protected:
	static std::string getBrowser();
};
