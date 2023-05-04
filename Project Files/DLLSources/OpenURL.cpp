#include "CvGameCoreDLL.h"
#include "OpenURL.h"
#include <algorithm>

#include <shellapi.h>

static std::string FixURL(std::string URL)
{
	const size_t pos = URL.find(" ");
	if (pos == std::string::npos)
	{
		return URL;
	}
	URL = URL.replace(pos, 1, "%20");
	return FixURL(URL);
}



void OpenURL::openURL(const char* URL)
{
	std::string app = getBrowser();
	if (app.length() < 1)
	{
		return;
	}
	std::string URL_buffer = FixURL(URL);

	::ShellExecute(0, NULL, app.c_str(), _T(URL_buffer.c_str()), NULL, SW_SHOWNORMAL);
}

void OpenURL::openReadme(const std::string section)
{
	std::string readme = "file://";
	readme
		.append(GetDLLPath(true))
		.append("\\..\\readme.html")
		;

	const int language = gDLL->getCurrentLanguage();

	readme.append("#").append(section);
	switch (language)
	{
	case 2:
		readme.append("German");
		break;
	case 8:
		readme.append("Russian");
		break;
	default:
		readme.append("English");
		break;
	}

	openURL(readme.c_str());
}

std::string OpenURL::getBrowser()
{
	std::string readme = GetDLLPath(true);
	readme.append("\\..\\readme.html");
	TCHAR app[MAX_PATH] = { 0 };

	int result = (int)::FindExecutable(_T(readme.c_str()), NULL, app);
	if (result > 32)
	{
		return app;
	}
	return "";
}
