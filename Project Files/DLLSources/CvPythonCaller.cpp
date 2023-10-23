#include "CvGameCoreDll.h"
#include "CvPythonCaller.h"
#include "CyArgsList.h"

Python gPython;

Python::Python()
 : m_Python(NULL)
{
}

CvDLLPythonIFaceBase& Python::IF()
{
	if (m_Python == NULL)
	{
		m_Python = gDLL->getPythonIFace();
	}
	return *m_Python;
}

Python& Python::call()
{
	return gPython;
}

void Python::openXMLEditor()
{
	IF().callFunction(PYScreensModule, "showXmlEditor");
}

