#include "CvGameCoreDLL.h"
#include "CyMap.h"
#include "CyPlayer.h"
#include "CyPlot.h"
#include "CyGame.h"
#include "CyUnit.h"
#include "CyGlobalContext.h"
#include "CyCity.h"

#include <boost/python/class.hpp>

void CyCityPythonInterface1(python::class_<CyCity>& x);
void CyCityPythonInterface2(python::class_<CyCity>& x);
void CyCityPythonInterface3(python::class_<CyCity>& x);
void CyCityPythonInterface4(python::class_<CyCity>& x);
void CyCityPythonInterface5(python::class_<CyCity>& x);
void CyDealPythonInterface();
void CyPlotPythonInterface1(python::class_<CyPlot>& x);
void CyPlotPythonInterface2(python::class_<CyPlot>& x);
void CyPlayerPythonInterface1(python::class_<CyPlayer>& x);
void CyPlayerPythonInterface2(python::class_<CyPlayer>& x);
void CyPlayerPythonInterface3(python::class_<CyPlayer>& x);
void CyPlayerPythonInterface4(python::class_<CyPlayer>& x);
void CyUnitPythonInterface1(python::class_<CyUnit>& x);
void CyUnitPythonInterface2(python::class_<CyUnit>& x);
void CyGlobalContextPythonInterface1(python::class_<CyGlobalContext>& x);
void CyGlobalContextPythonInterface2(python::class_<CyGlobalContext>& x);
void CyGlobalContextPythonInterface3(python::class_<CyGlobalContext>& x);
void CyGlobalContextPythonInterface4(python::class_<CyGlobalContext>& x);
void CyGlobalContextPythonInterface5(python::class_<CyGlobalContext>& x);
void CyGlobalContextPythonInterface6(python::class_<CyGlobalContext>& x);
void CyGlobalContextPythonInterface7(python::class_<CyGlobalContext>& x);
void CyGamePythonInterface1(python::class_<CyGame>& x);
void CyGamePythonInterface2(python::class_<CyGame>& x);
void CyGamePythonInterface3(python::class_<CyGame>& x);
void CyRandomPythonInterface();
void CyEnumsPythonInterface();
void CyTeamPythonInterface();
void CyAreaPythonInterface();
void CyStructsPythonInterface1();
void CyMapPythonInterface1(python::class_<CyMap>& x);
void CyMapPythonInterface2(python::class_<CyMap>& x);
void CyMapGeneratorPythonInterface();
void CyInfoPythonInterface1();
void CyInfoPythonInterface2();
void CyInfoPythonInterface3();
void CySelectionGroupInterface();
void CyArtFileMgrPythonInterface();
void CyGameTextMgrInterface();
void CyHallOfFameInterface();
void CyGameCoreUtilsPythonInterface();
void CyTradeRoutePythonInterface();
void CyTradeRouteGroupPythonInterface();//R&R mod,vetiarvind, trade groups
void CyUserSettingsPythonInterface(); // WTP
void CyDataPythonInterface(); // WTP

//
//
//
DllExport void DLLPublishToPython()
{
	CyDealPythonInterface();
	CyEnumsPythonInterface();
	CyRandomPythonInterface();
	CyTeamPythonInterface();
	CyAreaPythonInterface();
	CyStructsPythonInterface1();
	CyMapGeneratorPythonInterface();
	CySelectionGroupInterface();
	CyArtFileMgrPythonInterface();
	CyGameTextMgrInterface();
	CyInfoPythonInterface1();
	CyInfoPythonInterface2();
	CyInfoPythonInterface3();
	CyHallOfFameInterface();
	CyGameCoreUtilsPythonInterface();
	CyTradeRoutePythonInterface();
	CyTradeRouteGroupPythonInterface();//R&R mod,vetiarvind, trade groups
	CyUserSettingsPythonInterface(); // WTP
	CyDataPythonInterface(); // WTP

	//
	// large interfaces which can be split across files if need be
	//
	
	python::class_<CyGame> game ("CyGame");		// define game class
	CyGamePythonInterface1(game);
	CyGamePythonInterface2(game);
	CyGamePythonInterface3(game);

	python::class_<CyMap> map ("CyMap");		// define map class
	CyMapPythonInterface1(map);
	CyMapPythonInterface2(map);

	python::class_<CyCity> city ("CyCity");		// define city class
	CyCityPythonInterface1(city);				// publish it's methods
	CyCityPythonInterface2(city);				// publish it's methods
	CyCityPythonInterface3(city);				// publish it's methods
	CyCityPythonInterface4(city);
	CyCityPythonInterface5(city);

	python::class_<CyPlayer> player ("CyPlayer");	// define player class
	CyPlayerPythonInterface1(player);				// publish it's methods
	CyPlayerPythonInterface2(player);				// publish it's methods
	CyPlayerPythonInterface3(player);
	CyPlayerPythonInterface4(player);

	python::class_<CyUnit> unit ("CyUnit");		// define unit class
	CyUnitPythonInterface1(unit);				// publish it's methods
	CyUnitPythonInterface2(unit);

	python::class_<CyPlot> plot ("CyPlot");		// define plot class
	CyPlotPythonInterface1(plot);				// publish it's methods
	CyPlotPythonInterface2(plot);

	python::class_<CyGlobalContext> gc ("CyGlobalContext");	// define globals class
	CyGlobalContextPythonInterface1(gc);					// publish it's methods
	CyGlobalContextPythonInterface2(gc);					// publish it's methods
	CyGlobalContextPythonInterface3(gc);					// publish it's methods
	CyGlobalContextPythonInterface4(gc);					// publish it's methods
	CyGlobalContextPythonInterface5(gc);
	CyGlobalContextPythonInterface6(gc);
	CyGlobalContextPythonInterface7(gc);
}
