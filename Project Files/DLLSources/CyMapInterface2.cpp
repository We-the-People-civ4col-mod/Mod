#include "CvGameCoreDLL.h"
#include "CyMap.h"
#include "CyArea.h"
#include "CyCity.h"
#include "CySelectionGroup.h"
#include "CyUnit.h"
#include "CyPlot.h"
#include "CyData.h"

#include <boost/python/manage_new_object.hpp>
#include <boost/python/class.hpp>

//
// published python interface for CyMap
//

void CyMapPythonInterface2(python::class_<CyMap>& x)
{
	OutputDebugString("Python Extension Module - CyMapPythonInterface2\n");

	x
		.def("isWrapX", &CyMap::isWrapX, "bool () - whether the map wraps in the X axis")
		.def("isWrapY", &CyMap::isWrapY, "bool () - whether the map wraps in the Y axis")
		.def("getMapScriptName", &CyMap::getMapScriptName, "wstring () - name of the map script")
		.def("getWorldSize", &CyMap::getWorldSize, "WorldSizeTypes () - size of the world")
		.def("getClimate", &CyMap::getClimate, "ClimateTypes () - climate of the world")
		.def("getSeaLevel", &CyMap::getSeaLevel, "SeaLevelTypes () - sealevel of the world")

		.def("getNumCustomMapOptions", &CyMap::getNumCustomMapOptions, "int () - number of custom map settings")
		.def("getCustomMapOption", &CyMap::getCustomMapOption, "CustomMapOptionTypes () - user defined map setting at this option id")

		.def("getNumBonuses", &CyMap::getNumBonuses, "int () - total bonuses")
		.def("getNumBonusesOnLand", &CyMap::getNumBonusesOnLand, "int () - total bonuses on land plots")

		.def("plotByIndex", &CyMap::plotByIndex, python::return_value_policy<python::manage_new_object>(), "CyPlot (iIndex) - get a plot by its Index")
		.def("sPlotByIndex", &CyMap::sPlotByIndex, python::return_value_policy<python::reference_existing_object>(), "CyPlot (iIndex) - static - get plot by iIndex")
		.def("plot", &CyMap::plot, python::return_value_policy<python::manage_new_object>(), "CyPlot (iX,iY) - get CyPlot at (iX,iY)")
		.def("sPlot", &CyMap::sPlot, python::return_value_policy<python::reference_existing_object>(), "CyPlot (iX,iY) - static - get CyPlot at (iX,iY)")
		.def("pointToPlot", &CyMap::pointToPlot, python::return_value_policy<python::manage_new_object>())
		.def("getIndexAfterLastArea", &CyMap::getIndexAfterLastArea, "int () - index for handling NULL areas")
		.def("getNumAreas", &CyMap::getNumAreas, "int () - total areas")
		.def("getNumLandAreas", &CyMap::getNumLandAreas, "int () - total land areas")
		.def("getArea", &CyMap::getArea, python::return_value_policy<python::manage_new_object>(), "CyArea (iID) - get CyArea at iID")
		.def("recalculateAreas", &CyMap::recalculateAreas, "void () - Recalculates the areaID for each plot. Should be preceded by CyMap.setPlotTypes(...)")
		.def("resetPathDistance", &CyMap::resetPathDistance, "void ()")

		.def("calculatePathDistance", &CyMap::calculatePathDistance, "finds the shortest passable path between two CyPlots and returns its length, or returns -1 if no such path exists. Note: the path must be all-land or all-water")
		.def("rebuild", &CyMap::rebuild, "used to initialize the map during WorldBuilder load")
		.def("regenerateGameElements", &CyMap::regenerateGameElements, "used to regenerate everything but the terrain and height maps")
		.def("updateFog", &CyMap::updateFog, "void ()")
		.def("updateMinimapColor", &CyMap::updateMinimapColor, "void ()")
		.def("updateMinOriginalStartDist", &CyMap::updateMinOriginalStartDist, "void (CyArea* pArea)")		
		.def("calculateCanalAndChokePoints", &CyMap::calculateCanalAndChokePoints, "void ()")// Super Forts  *canal* *choke*

		.def("updateWaterPlotTerrainTypes", &CyMap::updateWaterPlotTerrainTypes, "void ()") // autodetect lakes
		
		.def("getCityCatchmentRadius", &CyMap::getCityCatchmentRadius, "int ()")
		.def("setCityCatchmentRadiusMapMaker", &CyMap::setCityCatchmentRadiusMapMaker, "void (int)")
		.def("setCityCatchmentRadiusNoMapMaker", &CyMap::setCityCatchmentRadiusNoMapMaker, "void (int)")

		.def("getTerrainCount", &CyMap::getTerrainCount, python::return_value_policy<python::manage_new_object>(), "CyInfoArray(TerrainTypes, int)")
		;
}
