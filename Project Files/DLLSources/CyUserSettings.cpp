//
// Python wrapper class for CyUserSettings
//
#include "CvGameCoreDLL.h"
#include "CyUserSettings.h"

CyUserSettings::CyUserSettings()
{
	m_pSettings = new UserSettings();
}

CyUserSettings::~CyUserSettings()
{
	SAFE_DELETE(m_pSettings);
}

int CyUserSettings::getDebugMaxGameFont() const
{
	return m_pSettings ? m_pSettings->getDebugMaxGameFont() : 0;
}
