#include "CvGameCoreDLL.h"
#include "CvRandom.h"
#include "CySmallClasses.h"
# include <boost/python/overloads.hpp>
using namespace boost::python;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CvRandom_get_overloads, CvRandom::get, 2, 2)

//
// published python interface for CvRandom
//
void CySmallClassesPythonInterface()
{
	OutputDebugString("Python Extension Module - CyRandomPythonInterface\n");

	python::class_<CvRandom>("CyRandom")
		.def("get", &CvRandom::get, CvRandom_get_overloads( args("usNum", "pszLog"), "returns a random number"))
		.def("init", &CvRandom::init, "void (unsigned long int ulSeed)")
		;

	python::class_<CyTradeMessage>("CyTradeMessage")
		.def("getScreenType", &CyTradeMessage::getScreenType, "enum ()")
		.def("getMessageType", &CyTradeMessage::getMessageType, "enum ()")
		.def("getYield", &CyTradeMessage::getYield, "int ()")
		.def("getAmount", &CyTradeMessage::getAmount, "int ()")
		.def("getPrice", &CyTradeMessage::getPrice, "int ()")
		.def("getCommission", &CyTradeMessage::getCommission, "int ()")
		.def("getMessage", &CyTradeMessage::getMessage, "string ()")
		;
}
