#include "CvGameCoreDll.h"
#include "XMLReaderAlerts.h"
#include "AlertWindow.h"


namespace XMLAlert
{
	AlertWindow::returnTypes handleNoneEnumType(const char* file, const char* entry, const char* tag)
	{
		AlertWindow window;

		window.header = "TXT_KEY_ALERT_XML_READ_ERROR_HEADER";
		window.message = "TXT_KEY_ALERT_XML_ENUM_MISSING_TYPE";
		window.addArgument(file);
		window.addArgument(entry);
		window.addArgument(tag);

		window.setIcon(AlertWindow::iconTypes::IconError);
		return window.openWindow();
	}

	AlertWindow::returnTypes handleWrongEnumType(const char* file, const char* entry, const char* tag, const char* element, const char* readString, const char* enumType)
	{
		AlertWindow window;

		const bool hasChild = element != NULL && strcmp(entry, element);

		window.header = "TXT_KEY_ALERT_XML_READ_ERROR_HEADER";
		window.message = hasChild ? "TXT_KEY_ALERT_XML_ENUM_WRONG_TYPE_CHILD" : "TXT_KEY_ALERT_XML_ENUM_WRONG_TYPE";
		window.addArgument(file);
		window.addArgument(entry);
		window.addArgument(tag);
		if (hasChild)
		{
			window.addArgument(element);
		}
		window.addArgument(readString);
		window.addArgument(enumType);

		window.setIcon(AlertWindow::iconTypes::IconError);
		return window.openWindow();
	}

	AlertWindow::returnTypes handleNoneEnumType(const char* file, const char* entry, const char* tag, const char* element)
	{
		AlertWindow window;

		const bool hasChild = element != NULL && strcmp(entry, element);

		window.header = "TXT_KEY_ALERT_XML_READ_ERROR_HEADER";
		window.message = hasChild ? "TXT_KEY_ALERT_XML_ENUM_NONE_TYPE_CHILD" : "TXT_KEY_ALERT_XML_ENUM_NONE_TYPE";
		window.addArgument(file);
		window.addArgument(entry);
		window.addArgument(tag);
		if (hasChild)
		{
			window.addArgument(element);
		}

		window.setIcon(AlertWindow::iconTypes::IconError);
		return window.openWindow();
	}

	void MissingEnumType(const char* file, const char* entry, const char* tag)
	{
		handleNoneEnumType(file, entry, tag);
	}

	void WrongEnumType(const char* file, const char* entry, const char* tag, const char* element, const char* readString, const char* enumType)
	{
		handleWrongEnumType(file, entry, tag, element, readString, enumType);
	}

	void NoneEnumType(const char* file, const char* entry, const char* tag, const char* element)
	{
		handleNoneEnumType(file, entry, tag, element);
	}
}
