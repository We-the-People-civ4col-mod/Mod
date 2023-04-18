#include "CvGameCoreDLL.h"
#include "UserSettings.h"

// file to declare and control "variable enum values" and other global constants
// the idea is to globally declare extern const variable, which will allow read access anywhere in the code
// those references are then set to refer to variables in this file. This will allow this file and only this file to alter the value

// Reasons for doing this can be to set xml data in dynamic DLLs, which will be hardcoded in hardcoded DLLs, hence making write once variables.
// It can also be to grant access to commonly used variables, which are rarely changed, such as city radius

CityPlotTypes local_NUM_CITY_PLOTS = FIRST_CITY_PLOT;
CityPlotTypes local_LAST_CITY_PLOT = FIRST_CITY_PLOT;
CityPlotTypes local_CITY_PLOTS_DIAMETER = FIRST_CITY_PLOT;
CityPlotTypes local_CITY_PLOTS_RADIUS = FIRST_CITY_PLOT;

const CityPlotTypes& NUM_CITY_PLOTS = local_NUM_CITY_PLOTS;
const CityPlotTypes& LAST_CITY_PLOT = local_LAST_CITY_PLOT;
const CityPlotTypes& CITY_PLOTS_DIAMETER = local_CITY_PLOTS_DIAMETER;
const CityPlotTypes& CITY_PLOTS_RADIUS = local_CITY_PLOTS_RADIUS;

const CityPlotTypes& VARINFO<CityPlotTypes>::END = local_NUM_CITY_PLOTS;
const CityPlotTypes& VARINFO<CityPlotTypes>::LAST = local_LAST_CITY_PLOT;
const CityPlotTypes& VARINFO<CityPlotTypes>::NUM_ELEMENTS = local_NUM_CITY_PLOTS;

#ifndef CHECK_GLOBAL_CONSTANTS
ArtStyleTypes    NUM_ARTSTYLE_TYPES    = static_cast<ArtStyleTypes   >(0);
//ColorTypes       NUM_COLOR_TYPES       = static_cast<ColorTypes      >(0);
//PlayerColorTypes NUM_PLAYERCOLOR_TYPES = static_cast<PlayerColorTypes>(0);

bool GAME_IS_STARTING_UP = true;
#endif


void CvGlobals::setCityCatchmentRadius(int iRadius)
{
	// Ideally this assert should trigger if altered after players are added, but it doesn't look like there are any way to check that.
	// What really should be checked here is that the total count of cities in the game should be 0.
//	FAssert(!GC.getGameINLINE().isFinalInitialized());
	local_CITY_PLOTS_RADIUS = static_cast<CityPlotTypes>(iRadius);
	if (iRadius == 1)
	{
		m_aaiXYCityPlot = m_aaiXYCityPlot_1_plot;
		local_NUM_CITY_PLOTS = NUM_CITY_PLOTS_1_PLOT;
		local_CITY_PLOTS_DIAMETER = static_cast<CityPlotTypes>(3);
		m_iMIN_CITY_RANGE = getDefineINT("MIN_CITY_RANGE");
		GC.setDefineFLOAT("CAMERA_CITY_ZOOM_IN_DISTANCE", GC.getDefineFLOAT("CAMERA_CITY_ZOOM_IN_DISTANCE_ONE_PLOT"));
	}
	else if (iRadius == 2)
	{
		m_aaiXYCityPlot = m_aaiXYCityPlot_2_plot;
		local_NUM_CITY_PLOTS = NUM_CITY_PLOTS_2_PLOTS;
		local_CITY_PLOTS_DIAMETER = static_cast<CityPlotTypes>(5);
		m_iMIN_CITY_RANGE = getDefineINT("MIN_CITY_RANGE_TWO_PLOT");
		GC.setDefineFLOAT("CAMERA_CITY_ZOOM_IN_DISTANCE", GC.getDefineFLOAT("CAMERA_CITY_ZOOM_IN_DISTANCE_TWO_PLOT"));
	}
	else
	{
		// invalid setting (likely 0). Use UserSetting value.
		// Odds are that a scenario is read and the radius isn't specified.
		UserSettings settings;
		setCityCatchmentRadius(settings.getColonyRadius());
	}
	local_LAST_CITY_PLOT = NUM_CITY_PLOTS - static_cast<CityPlotTypes>(1);
}
