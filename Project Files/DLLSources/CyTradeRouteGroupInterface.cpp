//R&R mod,vetiarvind, trade groups - start
#include "CvGameCoreDLL.h"
#include "CyTradeRouteGroup.h"

#include <boost/python/class.hpp>
#include <boost/python/manage_new_object.hpp>
namespace python = boost::python;

//
// published python interface for CyTradeRouteGroup
//

void CyTradeRouteGroupPythonInterface()
{
	OutputDebugString("Python Extension Module - CyTradeGroupPythonInterface\n");

	python::class_<CyTradeRouteGroup>("CyTradeRouteGroup")
		.def("getID", &CyTradeRouteGroup::getID, "int ()")
		.def("getName", &CyTradeRouteGroup::getName, "wstring ()")
		.def("setName", &CyTradeRouteGroup::setName, "void (wstring)")		
		.def("removeRoute", &CyTradeRouteGroup::removeRoute, "void (int)")
		.def("getRouteCount", &CyTradeRouteGroup::getRouteCount, "int ()")
		.def("getRouteByIndex", &CyTradeRouteGroup::getRouteByIndex, python::return_value_policy<python::manage_new_object>(),  "CyTradeGroup* (int)")		
	;
}

//R&R mod,vetiarvind, trade groups - end
