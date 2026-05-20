#include "../../CvGameCoreDLL.h"
#include "../../DesyncMonitor.h"

#include <boost/python/class.hpp>
namespace python = boost::python;

//
// published python interface for CyTradeRoute
//

void CySmallClassesPythonInterface()
{
	OutputDebugString("Python Extension Module - CySmallClassesInterface\n");

	python::class_<CxDesyncMonitor>("DesyncMonitor");
}
