#include "CvGameCoreDLL.h"
#include "CyTradeRoute.h"
//
// published python interface for CyTradeRoute
//

void CyTradeRoutePythonInterface()
{
	OutputDebugString("Python Extension Module - CyTradeRoutePythonInterface\n");

	python::class_<CyTradeRoute>("CyTradeRoute")
		.def("getID", &CyTradeRoute::getID, "int ()")
		.def("getSourceCity", &CyTradeRoute::getSourceCity, "IDInfo ()")
		.def("getDestinationCity", &CyTradeRoute::getDestinationCity, "IDInfo ()")
		.def("getSourceCityNameKey", &CyTradeRoute::getSourceCityNameKey, "wstring ()")
		.def("getSourceCityName", &CyTradeRoute::getSourceCityName, "wstring ()")
		.def("getDestinationCityNameKey", &CyTradeRoute::getDestinationCityNameKey, "wstring ()")
		.def("getDestinationCityName", &CyTradeRoute::getDestinationCityName, "wstring ()")
		.def("getYield", &CyTradeRoute::getYield, "YieldTypes ()")
	;
}
