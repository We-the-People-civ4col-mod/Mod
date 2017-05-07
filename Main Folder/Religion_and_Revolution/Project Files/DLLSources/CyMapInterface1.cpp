#include "CvGameCoreDLL.h"
#include "CyMap.h"
#include "CyArea.h"
#include "CyCity.h"
#include "CySelectionGroup.h"
#include "CyUnit.h"
#include "CyPlot.h"
//#include "CvStructs.h"
//# include <boost/python/manage_new_object.hpp>
//# include <boost/python/return_value_policy.hpp>

//
// published python interface for CyMap
//

void CyMapPythonInterface1(python::class_<CyMap>& x)
{
	OutputDebugString("Python Extension Module - CyMapPythonInterface1\n");

	x
		.def("isNone", &CyMap::isNone, "bool () - valid CyMap() interface")

		.def("erasePlots", &CyMap::erasePlots, "() - erases the plots")
		.def("setRevealedPlots", &CyMap::setRevealedPlots, "void (int /*TeamTypes*/ eTeam, bool bNewValue, bool bTerrainOnly) - reveals the plots to eTeam")
		.def("setAllPlotTypes", &CyMap::setAllPlotTypes, "void (int /*PlotTypes*/ ePlotType) - sets all plots to ePlotType")

		.def("updateVisibility", &CyMap::updateVisibility, "() - updates the plots visibility")
		.def("syncRandPlot", &CyMap::syncRandPlot, python::return_value_policy<python::manage_new_object>(), "CyPlot* (iFlags,iArea,iMinUnitDistance,iTimeout) - random plot based on conditions")
		.def("findCity", &CyMap::findCity, python::return_value_policy<python::manage_new_object>(), "CyCity* (int iX, int iY, int (PlayerTypes) eOwner = NO_PLAYER, int (TeamTypes) eTeam = NO_TEAM, bool bSameArea = true, bool bCoastalOnly = false, int (TeamTypes) eTeamAtWarWith = NO_TEAM, int (DirectionTypes) eDirection = NO_DIRECTION, CvCity* pSkipCity = NULL) - finds city")
		.def("findSelectionGroup", &CyMap::findSelectionGroup, python::return_value_policy<python::manage_new_object>(), "CvSelectionGroup* (int iX, int iY, int /*PlayerTypes*/ eOwner, bool bReadyToSelect)")

		.def("findBiggestArea", &CyMap::findBiggestArea, python::return_value_policy<python::manage_new_object>(), "CyArea* ()")

		.def("getMapFractalFlags", &CyMap::getMapFractalFlags, "int ()")
		.def("findWater", &CyMap::findWater, "bool (CyPlot* pPlot, int iRange, bool bFreshWater)")
		.def("isPlot", &CyMap::isPlot, "bool (iX,iY) - is (iX, iY) a valid plot?")
		.def("numPlots", &CyMap::numPlots, "int () - total plots in the map")
		.def("plotNum", &CyMap::plotNum, "int (iX,iY) - the index for a given plot")
		.def("plotX", &CyMap::plotX, "int (iIndex) - given the index of a plot, returns its X coordinate")
		.def("plotY", &CyMap::plotY, "int (iIndex) - given the index of a plot, returns its Y coordinate")
		.def("getGridWidth", &CyMap::getGridWidth, "int () - the width of the map, in plots")
		.def("getGridHeight", &CyMap::getGridHeight, "int () - the height of the map, in plots")

		.def("getLandPlots", &CyMap::getLandPlots, "int () - total land plots")
		.def("getOwnedPlots", &CyMap::getOwnedPlots, "int () - total owned plots")

		.def("getTopLatitude", &CyMap::getTopLatitude, "int () - top latitude (usually 90)")
		.def("getBottomLatitude", &CyMap::getBottomLatitude, "int () - bottom latitude (usually -90)")

		.def("getNextRiverID", &CyMap::getNextRiverID, "int ()")
		.def("incrementNextRiverID", &CyMap::incrementNextRiverID, "void ()")
		;
}
