#pragma once

#ifndef _CVDLLWIDGETDATA_H_
#define _CVDLLWIDGETDATA_H_

//#include "CvEnums.h"

class CvDLLWidgetData
{

public:

	DllExport static CvDLLWidgetData& getInstance();
	static void freeInstance();

	void parseHelp(CvWStringBuffer &szBuffer, const CvWidgetDataStruct &widgetDataStruct);

	bool executeAction(const CvWidgetDataStruct &widgetDataStruct);
	bool executeAltAction(const CvWidgetDataStruct &widgetDataStruct);
	bool executeDropOn(const CvWidgetDataStruct& destinationWidgetData, const CvWidgetDataStruct& sourceWidgetData);
	bool executeDoubleClick(const CvWidgetDataStruct& destinationWidgetData);

	// removing isLink might cause issues even if it appears to be unused in the exe
	// let's just keep exporting as this one doesn't really make a difference
	DllExport bool isLink(const CvWidgetDataStruct &widgetDataStruct) const;

protected:
	//	Actions to be executed
	void doPlotList(const CvWidgetDataStruct &widgetDataStruct);
	void doLiberateCity();
	void doRenameCity();
	void doRenameUnit();
	void doCreateGroup();
	void doDeleteGroup();
	void doTrain(const CvWidgetDataStruct &widgetDataStruct);
	void doConstruct(const CvWidgetDataStruct &widgetDataStruct);
	void doConvince(const CvWidgetDataStruct &widgetDataStruct);
	void doHurry(const CvWidgetDataStruct &widgetDataStruct);
	void doPlayerHurry(const CvWidgetDataStruct &widgetDataStruct);
	void doAction(const CvWidgetDataStruct &widgetDataStruct);
	void doContactCiv(const CvWidgetDataStruct &widgetDataStruct);
	void doAutomateCitizens(const CvWidgetDataStruct &widgetDataStruct);
	void doAutomateProduction();
	void doEmphasize(const CvWidgetDataStruct &widgetDataStruct);
	void doCancelCivics();
	void applyCityEdit();
	void doUnitModel();
	void doFlag();
	void doSelected(const CvWidgetDataStruct &widgetDataStruct);
	void doPediaUnitJump(const CvWidgetDataStruct &widgetDataStruct);
	void doPediaProfessionJump(const CvWidgetDataStruct &widgetDataStruct);
	void doPediaBuildingJump(const CvWidgetDataStruct &widgetDataStruct);
	void doPediaBack();
	void doPediaForward();
	void doPediaBonusJump(const CvWidgetDataStruct &widgetDataStruct, bool bData2 = false);
	void doPediaTerrainJump(const CvWidgetDataStruct &widgetDataStruct);
	void doPediaYieldJump(const CvWidgetDataStruct &widgetDataStruct);
	void doPediaFeatureJump(const CvWidgetDataStruct &widgetDataStruct);
	void doPediaFatherJump(const CvWidgetDataStruct &widgetDataStruct);
	void doPediaMain(const CvWidgetDataStruct &widgetDataStruct);
	void doPediaPromotionJump(const CvWidgetDataStruct &widgetDataStruct);
	void doPediaImprovementJump(const CvWidgetDataStruct &widgetDataStruct, bool bData2 = false);
	void doPediaCivicJump(const CvWidgetDataStruct &widgetDataStruct);
	void doPediaCivilizationJump(const CvWidgetDataStruct &widgetDataStruct);
	void doPediaLeaderJump(const CvWidgetDataStruct &widgetDataStruct);
	void doPediaDescription(const CvWidgetDataStruct &widgetDataStruct);
	void doGotoTurnEvent(const CvWidgetDataStruct &widgetDataStruct);
	void doPediaConstructJump(const CvWidgetDataStruct &widgetDataStruct);
	void doPediaTrainJump(const CvWidgetDataStruct &widgetDataStruct);
	void doPediaBuildJump(const CvWidgetDataStruct &widgetDataStruct);
	void doDealKill(const CvWidgetDataStruct &widgetDataStruct);
	void doRefreshMilitaryAdvisor(const CvWidgetDataStruct &widgetDataStruct);
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
	void parsePlotListHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseLiberateCityHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCityNameHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseTrainHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseConstructHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseConvinceHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseHurryHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parsePlayerHurryHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseActionHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCitizenHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCityPlotHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer); // city plot mouse over help - inaiwae
	void parseContactCivHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseScoreHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseImportExportHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCondensedImportExportHelp(const CvWidgetDataStruct& widgetDataStruct, CvWStringBuffer& szBuffer);// R&R mod, vetiarvind, max yield import limit
	void parseEjectCitizenHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer, bool bUseData2);
	void parseConvertHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseAutomateCitizensHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseAutomateProductionHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseEmphasizeHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseTradeItem(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseUnitModelHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCityDefenseHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCityHarbourSystemHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer); // WTP, ray, Widgets for Harbour System and Barracks System - START
	void parseCityBarracksSystemHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer); // WTP, ray, Widgets for Harbour System and Barracks System - START
	void parseShowOrHideYieldsHelp(const CvWidgetDataStruct& widgetDataStruct, CvWStringBuffer& szBuffer);
	void parseFlagHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parsePopulationHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseRebelHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseGreatGeneralHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseSelectedHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseBuildingHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseSpecialBuildingHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseTerrainHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseYieldHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseFeatureHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseFatherHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseShipCargoUnitHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseEuropeUnitHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseAfricaUnitHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer); /*** TRIANGLETRADE 10/28/08 by DPII ***/
	void parsePortRoyalUnitHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer); // R&R, ray, Port Royal
	void parseProfessionHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parsePediaBack(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parsePediaForward(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseBonusHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parsePromotionHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseUnitPromotionHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	// Teacher List - start - Nightinggale
	void parseCityUnitHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	// Teacher List - end - Nightinggale
	void parseCityYieldHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	//Androrc Multiple Professions per Building
	void parseTwoCityYieldsHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	//Androrc End
	void parseAssignTradeRoute(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseReceiveMoveCargoToTransportHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseEventHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseImprovementHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCivicHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCivilizationHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseLeaderHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseDescriptionHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer, bool bMinimal);
	void parseCloseScreenHelp(CvWStringBuffer &szBuffer);
	void parseKillDealHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseProductionModHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseLeaderheadHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseLeaderLineHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);

	void parseHelp(const CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);

	EmphasizeTypes getNextEmphasis(CvCity* pCity, YieldTypes eYield);
	void doAssignCitizenToPlot(CvCity* pCity, int iPlotIndex, int iUnitId);

	static CvDLLWidgetData* m_pInst;

};

#endif//_CVDLLWIDGETDATA_H_
