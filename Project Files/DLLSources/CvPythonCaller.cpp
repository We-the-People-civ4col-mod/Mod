#include "CvGameCoreDll.h"
#include "CvPythonCaller.h"
#include "CyArgsList.h"



static CvDLLPythonIFaceBase& IF()
{
	CvDLLPythonIFaceBase& ref = *gDLL->getPythonIFace();
	return ref;
}


void Python::XML::editorScreenDragOn(int source1, int source2, int dest1, int dest2)
{
	CyArgsList argsList;
	argsList.add(source1);
	argsList.add(source2);
	argsList.add(dest1);
	argsList.add(dest2);
	IF().callFunction(PYScreensModule, "editorScreenDragOn", argsList.makeFunctionArgs());
}

void Python::XML::openXMLEditor()
{
	IF().callFunction(PYScreensModule, "showXmlEditor");
}

