#pragma once

#ifndef _CVDLLWIDGETDATA_H_
#define _CVDLLWIDGETDATA_H_

//#include "CvEnums.h"

class CvDLLWidgetData
{

public:

	DllExport static CvDLLWidgetData& getInstance();
	DllExport static void freeInstance();

	DllExport void parseHelp(CvWStringBuffer &szBuffer, CvWidgetDataStruct &widgetDataStruct);

	DllExport bool executeAction( CvWidgetDataStruct &widgetDataStruct );
	DllExport bool executeAltAction( CvWidgetDataStruct &widgetDataStruct );
	DllExport bool executeDropOn(const CvWidgetDataStruct& destinationWidgetData, const CvWidgetDataStruct& sourceWidgetData);
	DllExport bool executeDoubleClick(const CvWidgetDataStruct& destinationWidgetData);

	DllExport bool isLink(const CvWidgetDataStruct &widgetDataStruct) const;

protected:
	//	Actions to be executed
	void doPlotList(CvWidgetDataStruct &widgetDataStruct);
	void doLiberateCity();
	void doRenameCity();
	void doRenameUnit();
	void doCreateGroup();
	void doDeleteGroup();
	void doTrain(CvWidgetDataStruct &widgetDataStruct);
	void doConstruct(CvWidgetDataStruct &widgetDataStruct);
	void doConvince(CvWidgetDataStruct &widgetDataStruct);
	void doHurry(CvWidgetDataStruct &widgetDataStruct);
	void doPlayerHurry(CvWidgetDataStruct &widgetDataStruct);
	void doAction(CvWidgetDataStruct &widgetDataStruct);
	void doContactCiv(CvWidgetDataStruct &widgetDataStruct);
	void doAutomateCitizens(CvWidgetDataStruct &widgetDataStruct);
	void doAutomateProduction();
	void doEmphasize(CvWidgetDataStruct &widgetDataStruct);
	void doCancelCivics();
	void applyCityEdit();
	void doUnitModel();
	void doFlag();
	void doSelected(CvWidgetDataStruct &widgetDataStruct);
	void doPediaUnitJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaProfessionJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaBuildingJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaBack();
	void doPediaForward();
	void doPediaBonusJump(CvWidgetDataStruct &widgetDataStruct, bool bData2 = false);
	void doPediaTerrainJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaYieldJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaFeatureJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaFatherJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaMain(CvWidgetDataStruct &widgetDataStruct);
	void doPediaPromotionJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaImprovementJump(CvWidgetDataStruct &widgetDataStruct, bool bData2 = false);
	void doPediaCivicJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaCivilizationJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaLeaderJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaDescription(CvWidgetDataStruct &widgetDataStruct);
	void doGotoTurnEvent(CvWidgetDataStruct &widgetDataStruct);
	void doPediaConstructJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaTrainJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaBuildJump(CvWidgetDataStruct &widgetDataStruct);
	void doDealKill(CvWidgetDataStruct &widgetDataStruct);
	void doRefreshMilitaryAdvisor(CvWidgetDataStruct &widgetDataStruct);
	void doCityUnitAssignCitizen(const CvWidgetDataStruct& destinationWidgetData, const CvWidgetDataStruct& sourceWidgetData);
	void doMoveShipCargo(const CvWidgetDataStruct& destinationWidgetData, const CvWidgetDataStruct& sourceWidgetData);
	void doMoveDockUnit(const CvWidgetDataStruct& destinationWidgetData, const CvWidgetDataStruct& sourceWidgetData);
	void doMoveDockAfricaUnit(const CvWidgetDataStruct& destinationWidgetData, const CvWidgetDataStruct& sourceWidgetData); /*** TRIANGLETRADE 10/28/08 by DPII ***/
	void doMoveDockPortRoyalUnit(const CvWidgetDataStruct& destinationWidgetData, const CvWidgetDataStruct& sourceWidgetData); // R&R, ray, Port Royal
	void doUnitIntoCity(const CvWidgetDataStruct& destinationWidgetData, const CvWidgetDataStruct& sourceWidgetData);
	void doMoveCargoToCity(const CvWidgetDataStruct& destinationWidgetData, const CvWidgetDataStruct& sourceWidgetData);
	void doAssignTradeRoute(const CvWidgetDataStruct& widgetDataStruct);
	void doGoToCity(const CvWidgetDataStruct& widgetDataStruct);
	void doCityCenterPlotClicked(const CvWidgetDataStruct& widgetDataStruct);
	void doMoveCargoToTransport(const CvWidgetDataStruct& destinationWidgetData, const CvWidgetDataStruct& sourceWidgetData);
	void doCreateTradeRoute(const CvWidgetDataStruct& widgetDataStruct);
	void doEditTradeRoute(const CvWidgetDataStruct& widgetDataStruct);
	// R&R mod, vetiarvind, max yield import limit - start
	//void doYieldImportExport(const CvWidgetDataStruct& widgetDataStruct);
	void doYieldImportExport(const CvWidgetDataStruct& widgetDataStruct, bool condensed = false);
	// R&R mod, vetiarvind, max yield import limit - end

	// Teacher List - start - Nightinggale
	void doTeacherList(const CvWidgetDataStruct& widgetDataStruct);
	// Teacher List - end - Nightinggale
	// R&R, Robert Surcouf, Custom House Popup-Screen START
	void doCustomHouse(const CvWidgetDataStruct& widgetDataStruct);
	void doDomesticMarket(const CvWidgetDataStruct& widgetDataStruct);
	// R&R, Robert Surcouf, Custom House Popup-Screen END
	void doMenu();

	void doDoubleClickCitizen(const CvWidgetDataStruct& widgetDataStruct);
	void doDoubleClickGarrison(const CvWidgetDataStruct& widgetDataStruct);
	void doDoubleClickDock(const CvWidgetDataStruct& widgetDataStruct);

	//	Help parsing
	void parsePlotListHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseLiberateCityHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCityNameHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseTrainHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseConstructHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseConvinceHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseHurryHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parsePlayerHurryHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseActionHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCitizenHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCityPlotHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer); // city plot mouse over help - inaiwae
	void parseContactCivHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseScoreHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseImportExportHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCondensedImportExportHelp(CvWidgetDataStruct& widgetDataStruct, CvWStringBuffer& szBuffer);// R&R mod, vetiarvind, max yield import limit
	void parseEjectCitizenHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseConvertHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseAutomateCitizensHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseAutomateProductionHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseEmphasizeHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseTradeItem(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseUnitModelHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCityDefenseHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCityHarbourSystemHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer); // WTP, ray, Widgets for Harbour System and Barracks System - START
	void parseCityBarracksSystemHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer); // WTP, ray, Widgets for Harbour System and Barracks System - START
	void parseShowOrHideYieldsHelp(CvWidgetDataStruct& widgetDataStruct, CvWStringBuffer& szBuffer);
	void parseFlagHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parsePopulationHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseRebelHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseGreatGeneralHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseSelectedHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseBuildingHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseSpecialBuildingHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseTerrainHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseYieldHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseFeatureHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseFatherHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseShipCargoUnitHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseEuropeUnitHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseAfricaUnitHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer); /*** TRIANGLETRADE 10/28/08 by DPII ***/
	void parsePortRoyalUnitHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer); // R&R, ray, Port Royal
	void parseProfessionHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parsePediaBack(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parsePediaForward(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseBonusHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parsePromotionHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseUnitPromotionHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	// Teacher List - start - Nightinggale
	void parseCityUnitHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	// Teacher List - end - Nightinggale
	void parseCityYieldHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	//Androrc Multiple Professions per Building
	void parseTwoCityYieldsHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	//Androrc End
	void parseAssignTradeRoute(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseReceiveMoveCargoToTransportHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseEventHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseImprovementHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCivicHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCivilizationHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseLeaderHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseDescriptionHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer, bool bMinimal);
	void parseCloseScreenHelp(CvWStringBuffer &szBuffer);
	void parseKillDealHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseProductionModHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseLeaderheadHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseLeaderLineHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);

	void parseHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);

	EmphasizeTypes getNextEmphasis(CvCity* pCity, YieldTypes eYield);
	void doAssignCitizenToPlot(CvCity* pCity, int iPlotIndex, int iUnitId);

	static CvDLLWidgetData* m_pInst;

};

#endif//_CVDLLWIDGETDATA_H_
