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

void CyGlobalContextPythonInterface2(python::class_<CyGlobalContext>& x)
{
	OutputDebugString("Python Extension Module - CyGlobalContextPythonInterface2\n");
	x
		// global defines.xml
		.def("getDefineINT", &CyGlobalContext::getDefineINT, "int ( string szName )" )
		.def("getDefineFLOAT", &CyGlobalContext::getDefineFLOAT, "float ( string szName )" )
		.def("getDefineSTRING", &CyGlobalContext::getDefineSTRING, "string getDefineSTRING( string szName )" )
		.def("setDefineINT", &CyGlobalContext::setDefineINT, "void ( string szName, int iValue )" )
		.def("setDefineFLOAT", &CyGlobalContext::setDefineFLOAT, "void setDefineFLOAT( string szName, float fValue )" )
		.def("setDefineSTRING", &CyGlobalContext::setDefineSTRING, "void ( string szName, string szValue )" )
		.def("getMOVE_DENOMINATOR", &CyGlobalContext::getMOVE_DENOMINATOR, "int ()")
		.def("getFOOD_CONSUMPTION_PER_POPULATION", &CyGlobalContext::getFOOD_CONSUMPTION_PER_POPULATION, "int ()")
		.def("getMAX_HIT_POINTS", &CyGlobalContext::getMAX_HIT_POINTS, "int ()")
		.def("getHILLS_EXTRA_DEFENSE", &CyGlobalContext::getHILLS_EXTRA_DEFENSE, "int ()")
		.def("getRIVER_ATTACK_MODIFIER", &CyGlobalContext::getRIVER_ATTACK_MODIFIER, "int ()")
		.def("getAMPHIB_ATTACK_MODIFIER", &CyGlobalContext::getAMPHIB_ATTACK_MODIFIER, "int ()")
		.def("getHILLS_EXTRA_MOVEMENT", &CyGlobalContext::getHILLS_EXTRA_MOVEMENT, "int ()")
		.def("getPEAK_EXTRA_MOVEMENT", &CyGlobalContext::getHILLS_EXTRA_MOVEMENT, "int ()")
		.def("getMAX_PLOT_LIST_ROWS", &CyGlobalContext::getMAX_PLOT_LIST_ROWS, "int ()")
		.def("getUNIT_MULTISELECT_MAX", &CyGlobalContext::getUNIT_MULTISELECT_MAX, "int ()")
		.def("getEVENT_MESSAGE_TIME", &CyGlobalContext::getEVENT_MESSAGE_TIME, "int ()")
		.def("getROUTE_FEATURE_GROWTH_MODIFIER", &CyGlobalContext::getROUTE_FEATURE_GROWTH_MODIFIER, "int ()")
		.def("getFEATURE_GROWTH_MODIFIER", &CyGlobalContext::getFEATURE_GROWTH_MODIFIER, "int ()")
		.def("getMIN_CITY_RANGE", &CyGlobalContext::getMIN_CITY_RANGE, "int ()")
		.def("getCITY_MAX_NUM_BUILDINGS", &CyGlobalContext::getCITY_MAX_NUM_BUILDINGS, "int ()")
		.def("getLAKE_MAX_AREA_SIZE", &CyGlobalContext::getLAKE_MAX_AREA_SIZE, "int ()")
		.def("getMIN_WATER_SIZE_FOR_OCEAN", &CyGlobalContext::getMIN_WATER_SIZE_FOR_OCEAN, "int ()")
		.def("getFORTIFY_MODIFIER_PER_TURN", &CyGlobalContext::getFORTIFY_MODIFIER_PER_TURN, "int ()")
		.def("getMAX_CITY_DEFENSE_DAMAGE", &CyGlobalContext::getMAX_CITY_DEFENSE_DAMAGE, "int ()")
		.def("getPEAK_SEE_THROUGH_CHANGE", &CyGlobalContext::getPEAK_SEE_THROUGH_CHANGE, "int ()")
		.def("getHILLS_SEE_THROUGH_CHANGE", &CyGlobalContext::getHILLS_SEE_THROUGH_CHANGE, "int ()")
		.def("getSEAWATER_SEE_FROM_CHANGE", &CyGlobalContext::getSEAWATER_SEE_FROM_CHANGE, "int ()")
		.def("getPEAK_SEE_FROM_CHANGE", &CyGlobalContext::getPEAK_SEE_FROM_CHANGE, "int ()")
		.def("getHILLS_SEE_FROM_CHANGE", &CyGlobalContext::getHILLS_SEE_FROM_CHANGE, "int ()")
		.def("getMAX_REBEL_YIELD_MODIFIER", &CyGlobalContext::getMAX_REBEL_YIELD_MODIFIER, "int ()")
		
		// TAC - AI Improved Naval AI - koma13 - START
		.def("getAI_TRANSPORT_DANGER_RANGE", &CyGlobalContext::getAI_TRANSPORT_DANGER_RANGE, "int()")
		.def("getAI_LOST_TRANSPORT_MEMORY_COUNT", &CyGlobalContext::getAI_LOST_TRANSPORT_MEMORY_COUNT, "int()")
		// TAC - AI Improved Naval AI - koma13 - END

		.def("getCAMERA_MIN_YAW", &CyGlobalContext::getCAMERA_MIN_YAW, "float ()")
		.def("getCAMERA_MAX_YAW", &CyGlobalContext::getCAMERA_MAX_YAW, "float ()")
		.def("getCAMERA_FAR_CLIP_Z_HEIGHT", &CyGlobalContext::getCAMERA_FAR_CLIP_Z_HEIGHT, "float ()")
		.def("getCAMERA_MAX_TRAVEL_DISTANCE", &CyGlobalContext::getCAMERA_MAX_TRAVEL_DISTANCE, "float ()")
		.def("getCAMERA_START_DISTANCE", &CyGlobalContext::getCAMERA_START_DISTANCE, "float ()")
		.def("getPLOT_SIZE", &CyGlobalContext::getPLOT_SIZE, "float ()")
		.def("getCAMERA_SPECIAL_PITCH", &CyGlobalContext::getCAMERA_SPECIAL_PITCH, "float ()")
		.def("getCAMERA_MAX_TURN_OFFSET", &CyGlobalContext::getCAMERA_MAX_TURN_OFFSET, "float ()")
		.def("getCAMERA_MIN_DISTANCE", &CyGlobalContext::getCAMERA_MIN_DISTANCE, "float ()")
		.def("getCAMERA_UPPER_PITCH", &CyGlobalContext::getCAMERA_UPPER_PITCH, "float ()")
		.def("getCAMERA_LOWER_PITCH", &CyGlobalContext::getCAMERA_LOWER_PITCH, "float ()")
		.def("getFIELD_OF_VIEW", &CyGlobalContext::getFIELD_OF_VIEW, "float ()")
		.def("getUNIT_MULTISELECT_DISTANCE", &CyGlobalContext::getUNIT_MULTISELECT_DISTANCE, "float ()")
		.def("getMAX_CIV_PLAYERS", &CyGlobalContext::getMAX_CIV_PLAYERS, "int ()")
		.def("getMAX_PLAYERS", &CyGlobalContext::getMAX_PLAYERS, "int ()")
		.def("getMAX_CIV_TEAMS", &CyGlobalContext::getMAX_CIV_TEAMS, "int ()")
		.def("getMAX_TEAMS", &CyGlobalContext::getMAX_TEAMS, "int ()")
		.def("getINVALID_PLOT_COORD", &CyGlobalContext::getINVALID_PLOT_COORD, "int ()")
		.def("getNUM_CITY_PLOTS", &CyGlobalContext::getNUM_CITY_PLOTS, "int ()")
		.def("getCITY_HOME_PLOT", &CyGlobalContext::getCITY_HOME_PLOT, "int ()")
		;
}
