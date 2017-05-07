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


void CyGlobalContextPythonInterface1(python::class_<CyGlobalContext>& x)
{
	OutputDebugString("Python Extension Module - CyGlobalContextPythonInterface1\n");

	x
		.def("isDebugBuild", &CyGlobalContext::isDebugBuild, "() - returns true if running a debug build")
		.def("getGame", &CyGlobalContext::getCyGame, python::return_value_policy<python::reference_existing_object>(), "() - CyGame()")
		.def("getMap", &CyGlobalContext::getCyMap, python::return_value_policy<python::reference_existing_object>(), "() - CyMap()")
		.def("getPlayer", &CyGlobalContext::getCyPlayer, python::return_value_policy<python::reference_existing_object>(), "(iPlayer) - iPlayer instance")
		.def("getActivePlayer", &CyGlobalContext::getCyActivePlayer, python::return_value_policy<python::reference_existing_object>(), "() - active player instance")
		.def("getASyncRand", &CyGlobalContext::getCyASyncRand, python::return_value_policy<python::reference_existing_object>(), "Non-Synch'd random #")
		.def("getTeam", &CyGlobalContext::getCyTeam, python::return_value_policy<python::reference_existing_object>(), "(iTeam) - iTeam instance")

		
	;
}
