#include "CvGameCoreDLL.h"
#include "CyData.h"
//#include "CvStructs.h"
//# include <boost/python/manage_new_object.hpp>
//# include <boost/python/return_value_policy.hpp>

//
// published python interface for CyMapGenerator
//

void CyDataPythonInterface()
{
	OutputDebugString("Python Extension Module - CyDataPythonInterface\n");

	python::class_<CyInfoArray>("CyInfoArray")
		.def("get", &CyInfoArray::get, "int(int)")
		.def("get1", &CyInfoArray::get1, "int(int)")
		.def("get2", &CyInfoArray::get2, "int(int)")
		.def("get3", &CyInfoArray::get3, "int(int)")
		.def("getSub", &CyInfoArray::getSub, "int(int, int)")
		.def("getLength", &CyInfoArray::getLength, "int()")
		.def("getDimentions", &CyInfoArray::getDimentions, "int()")
		.def("getType", &CyInfoArray::getType, "enum(int)")
		;
}
