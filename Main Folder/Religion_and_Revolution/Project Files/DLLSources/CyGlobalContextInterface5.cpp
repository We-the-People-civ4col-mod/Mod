//
// published python interface for CyGlobalContext
// Author - Mustafa Thamer
//

#include "CvGameCoreDLL.h"
#include "CyMap.h"
#include "CyPlayer.h"
#include "CyGame.h"
#include "CyGlobalContext.h"
#include "CvRandom.h"
//#include "CvStructs.h"
#include "CvInfos.h"
#include "CyTeam.h"


void CyGlobalContextPythonInterface5(python::class_<CyGlobalContext>& x)
{
	OutputDebugString("Python Extension Module - CyGlobalContextPythonInterface5\n");

	x
		// infos
		.def("getNumEffectInfos", &CyGlobalContext::getNumEffectInfos, "int () - Number of effect infos")
		.def("getEffectInfo", &CyGlobalContext::getEffectInfo, python::return_value_policy<python::reference_existing_object>(), "(int (EffectTypes) eEffectID) - CvInfo for EffectID")

		.def("getNumTerrainInfos", &CyGlobalContext::getNumTerrainInfos, "() - Total Terrain Infos XML\\Terrain\\CIV4TerrainInfos.xml")
		.def("getTerrainInfo", &CyGlobalContext::getTerrainInfo, python::return_value_policy<python::reference_existing_object>(), "(int (TerrainTypes) eTerrainID) - CvInfo for TerrainID")

		.def("getNumYieldInfos", &CyGlobalContext::getNumYieldInfos, "() - Total Yield Infos XML\\Terrain\\CIV4YieldInfos.xml")

		.def("getNumBonusInfos", &CyGlobalContext::getNumBonusInfos, "() - Total Bonus Infos XML\\Terrain\\CIV4BonusInfos.xml")
		.def("getBonusInfo", &CyGlobalContext::getBonusInfo, python::return_value_policy<python::reference_existing_object>(), "(BonusID) - CvInfo for BonusID")

		.def("getNumFeatureInfos", &CyGlobalContext::getNumFeatureInfos, "() - Total Feature Infos XML\\Terrain\\CIV4FeatureInfos.xml")
		.def("getFeatureInfo", &CyGlobalContext::getFeatureInfo, python::return_value_policy<python::reference_existing_object>(), "(FeatureID) - CvInfo for FeatureID")

		.def("getNumCultureLevelInfos", &CyGlobalContext::getNumCultureLevelInfos, "int () - Number of culture level infos")
		.def("getCultureLevelInfo", &CyGlobalContext::getCultureLevelInfo, python::return_value_policy<python::reference_existing_object>(), "(CultureLevelID) - CvInfo for CultureLevelID")

		.def("getNumEraInfos", &CyGlobalContext::getNumEraInfos, "int () - Number of era infos")
		.def("getEraInfo", &CyGlobalContext::getEraInfo, python::return_value_policy<python::reference_existing_object>())

		.def("getNumWorldInfos", &CyGlobalContext::getNumWorldInfos, "int () - Number of world infos")
		.def("getWorldInfo", &CyGlobalContext::getWorldInfo, python::return_value_policy<python::reference_existing_object>(), "CvWorldInfo - (WorldTypeID)")

		.def("getNumClimateInfos", &CyGlobalContext::getNumClimateInfos, "int () - Number of climate infos")
		.def("getClimateInfo", &CyGlobalContext::getClimateInfo, python::return_value_policy<python::reference_existing_object>(), "CvClimateInfo - (ClimateTypeID)")

		.def("getNumSeaLevelInfos", &CyGlobalContext::getNumSeaLevelInfos, "int () - Number of seal level infos")
		.def("getSeaLevelInfo", &CyGlobalContext::getSeaLevelInfo, python::return_value_policy<python::reference_existing_object>(), "CvSeaLevelInfo - (SeaLevelTypeID)")

		.def("getNumEuropeInfos", &CyGlobalContext::getNumEuropeInfos, "int () - Number of seal level infos")
		.def("getEuropeInfo", &CyGlobalContext::getEuropeInfo, python::return_value_policy<python::reference_existing_object>(), "CvEuropeInfo - (EuropeTypeID)")

		.def("getNumCivilizationInfos", &CyGlobalContext::getNumCivilizatonInfos, "() - Total Civilization Infos XML\\Civilizations\\CIV4CivilizationInfos.xml")
		.def("getCivilizationInfo", &CyGlobalContext::getCivilizationInfo, python::return_value_policy<python::reference_existing_object>(), "(CivilizationID) - CvInfo for CivilizationID")

		.def("getNumLeaderHeadInfos", &CyGlobalContext::getNumLeaderHeadInfos, "() - Total LeaderHead Infos XML\\Civilizations\\CIV4LeaderHeadInfos.xml")
		.def("getLeaderHeadInfo", &CyGlobalContext::getLeaderHeadInfo, python::return_value_policy<python::reference_existing_object>(), "(LeaderHeadID) - CvInfo for LeaderHeadID")

		.def("getNumTraitInfos", &CyGlobalContext::getNumTraitInfos, "() - Total Civilization Infos XML\\Civilizations\\CIV4TraitInfos.xml")
		.def("getTraitInfo", &CyGlobalContext::getTraitInfo, python::return_value_policy<python::reference_existing_object>(), "(TraitID) - CvInfo for TraitID")

		.def("getNumUnitInfos", &CyGlobalContext::getNumUnitInfos, "() - Total Unit Infos XML\\Units\\CIV4UnitInfos.xml")
		.def("getUnitInfo", &CyGlobalContext::getUnitInfo, python::return_value_policy<python::reference_existing_object>(), "(UnitID) - CvInfo for UnitID")
		.def("getNumSpecialUnitInfos", &CyGlobalContext::getNumSpecialUnitInfos, "() - Total SpecialUnit Infos XML\\Units\\CIV4SpecialUnitInfos.xml")
		.def("getSpecialUnitInfo", &CyGlobalContext::getSpecialUnitInfo, python::return_value_policy<python::reference_existing_object>(), "(UnitID) - CvInfo for UnitID")

	;
}