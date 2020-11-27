#pragma once
#ifndef CyUserSettings_h
#define CyUserSettings_h
//
// Python wrapper class for CyUserSettings
//

#include "UserSettings.h"

class CyUserSettings
{
public:
	CyUserSettings();
	~CyUserSettings();

	int getDebugMaxGameFont() const;

protected:
	UserSettings* m_pSettings;
};
#endif	// #ifndef CyUserSettings
