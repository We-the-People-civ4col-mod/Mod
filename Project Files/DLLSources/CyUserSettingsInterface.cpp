#include "CvGameCoreDLL.h"
#include "CyUserSettings.h"
//
// published python interface for CyUserSettings
//

// WARNING: riks of bugs if used incorrectly
// Do not add settings, which are stored in CvGame, CvMap or similar
// If it's a value set at game start or by host in multiplayer, then using UserSettings instead of game data can cause issues.
// For instance changing default city radius will not affect running games.

// list of intentionally obmitted functions
// getColonyRadius: use CyMap::getCityCatchmentRadius()

void CyUserSettingsPythonInterface()
{
	OutputDebugString("Python Extension Module - CyUserSettingsPythonInterface\n");
	python::class_<CyUserSettings>("CyUserSettings")
		.def("getDebugMaxGameFont", &CyUserSettings::getDebugMaxGameFont, "int ()")
		;
}
