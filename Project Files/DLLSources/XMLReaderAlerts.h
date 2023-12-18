#pragma once

#include "AlertWindow.h"

namespace XMLAlert
{
	AlertWindow::returnTypes handleMissingEnumType(const char* file, const char* entry, const char* tag);
	AlertWindow::returnTypes handleWrongEnumType(const char* file, const char* entry, const char* tag, const char* element, const char* readString, const char* enumType);
	AlertWindow::returnTypes handleNoneEnumType(const char* file, const char* entry, const char* tag, const char* element);
	
	void MissingEnumType(const char* file, const char* entry, const char* tag);
	void WrongEnumType(const char* file, const char* entry, const char* tag, const char* element, const char* readString, const char* enumType);
	void NoneEnumType(const char* file, const char* entry, const char* tag, const char* element);
}

