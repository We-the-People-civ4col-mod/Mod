#include "CvGameCoreDll.h"
#include "AlertWindow.h"
#include "tinyxml2.h"

AlertWindow::AlertWindow()
	: m_bKeysLookedup(false)
	, m_iDefaultButton(1)
{
}

UINT getFlag(AlertWindow::Buttons var)
{
	switch (var)
	{
	case AlertWindow::Buttons::BtnAboutRetryIgnore: return MB_ABORTRETRYIGNORE;
	case AlertWindow::Buttons::BtnCancelTryAgainContinue: return MB_CANCELTRYCONTINUE;
	case AlertWindow::Buttons::BtnHelp: return MB_HELP;
	case AlertWindow::Buttons::BtnOK: return MB_OK;
	case AlertWindow::Buttons::BtnCancel: return MB_OKCANCEL;
	case AlertWindow::Buttons::BtnRetryCancel: return MB_RETRYCANCEL;
	case AlertWindow::Buttons::BtnYesNo: return MB_YESNO;
	case AlertWindow::Buttons::BtnYesNoCancel: return MB_YESNOCANCEL;
	default:
		FAssert(0);
		return MB_OK;
	}
}

UINT getFlag(AlertWindow::iconTypes var)
{
	switch (var)
	{
	case AlertWindow::iconTypes::IconNone: return 0;
	case AlertWindow::iconTypes::IconError: return MB_ICONERROR;
	case AlertWindow::iconTypes::IconWarning: return MB_ICONWARNING;
	case AlertWindow::iconTypes::IconQuestion: return MB_ICONQUESTION;
	case AlertWindow::iconTypes::IconInformation: return MB_ICONINFORMATION;
	default:
		FAssert(0);
		return 0;
	}
}

UINT getFlag(int var)
{
	switch (var)
	{
	case 1: return MB_DEFBUTTON1;
	case 2: return MB_DEFBUTTON2;
	case 3: return MB_DEFBUTTON3;
	case 4: return MB_DEFBUTTON4;
	default:
		FAssert(0);
		return MB_DEFBUTTON1;
	}
}


AlertWindow::returnTypes AlertWindow::openWindow()
{
	UINT flag = 0;
	flag |= getFlag(m_Buttons);
	flag |= getFlag(m_Icon);
	flag |= getFlag(m_iDefaultButton);

	if (!m_bKeysLookedup)
	{
		lookupKeys();
	}

	// make sure the window is on top even in full screen mode
	// without this the game will freeze with the window behind and people can have problems getting out of the now frozen game
	flag |= MB_SETFOREGROUND | MB_TOPMOST;

	return (returnTypes)MessageBoxA(NULL, message.c_str(), header.c_str(), flag);
}

void AlertWindow::setDefaultButton(int iButton)
{
	FAssert(iButton >= 1 && iButton <= 4);
	m_iDefaultButton = iButton;
}

void AlertWindow::setButtonLayout(Buttons eButtons)
{
	m_Buttons = eButtons;
}

void AlertWindow::setIcon(iconTypes eIcon)
{
	m_Icon = eIcon;
}

void testStringCompare(tinyxml2::XMLElement* tagElement, CvString& str, const TCHAR* languageName)
{
	if (str == tagElement->GetText())
	{
		bool bFallback = false;
		tinyxml2::XMLElement* translationElement = tagElement->NextSiblingElement(languageName);
		if (translationElement == NULL)
		{
			bFallback = true;
			translationElement = tagElement->NextSiblingElement("English");
		}
		if (translationElement == NULL)
		{
			return;
		}
		str = translationElement->GetText();
		str = CvString(CvGameText::convertFromUTF8(str, bFallback, "Alert_Window_utf8.xml", tagElement->GetText()));
	}
}


void AlertWindow::addArgument(const char* argument)
{
	lookupKeys();
	const size_t pos = message.find("%s");
	if (pos == std::string::npos)
	{
		// should be an alert window?
		FAssertMsg(false, "AlertWindow::addArgument called on string without %s");
		return;
	}

	message = message.replace(pos, 2, argument);
}

void AlertWindow::setMessageArguments(int i1, int i2, int i3, int i4, int i5)
{
	lookupKeys();
	message = CvString::format(message, i1, i2, i3, i4, i5);
}

 void AlertWindow::lookupKeys()
{
	 FAssertMsg(header.length() > 0 && message.length() > 0, "lookupKeys called prior to setting the keys");
	 if (m_bKeysLookedup)
	 {
		 // no need to look up the keys multiple times
		 return;
	 }

	m_bKeysLookedup = true;

	std::string filePath = GetDLLPath();
	filePath.append("/XML/Text/Alert_Window_utf8.xml");

	tinyxml2::XMLDocument doc;
	doc.LoadFile(filePath.c_str());
	if (doc.ErrorID() != tinyxml2::XML_SUCCESS)
	{
		// this shouldn't happen, but just to be 100% safe from crashes
		FAssertMsg(false, "Alert_Window_utf8.xml failed to open");
		return;
	}
	FAssert(gDLL != NULL);
	// use English if somebody adds an alert window prior to gDLL being set (hence no current language available)
	const TCHAR* languageName = gDLL != NULL ? CvGameText::getLanguageName(gDLL->getCurrentLanguage()) : "English";

	tinyxml2::XMLElement* loopElement = doc.FirstChildElement("Civ4GameText");

	FAssert(loopElement);
	if (loopElement == NULL)
	{
		return;
	}

	for (loopElement = loopElement->FirstChildElement("TEXT"); loopElement != NULL; loopElement = loopElement->NextSiblingElement("TEXT"))
	{
		tinyxml2::XMLElement* tagElement = loopElement->FirstChildElement("Tag");
		if (tagElement != NULL)
		{
			testStringCompare(tagElement, header , languageName);
			testStringCompare(tagElement, message, languageName);
		}
	}
}
