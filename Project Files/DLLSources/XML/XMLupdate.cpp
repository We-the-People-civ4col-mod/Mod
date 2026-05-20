#include "XMLUpdateEngine.h"

// handles "script" updating a single xml file

// StartXMLUpdate() is called whenever the exe attaches to the DLL.
// This means any changes done here will be written to the xml file before the normal xml code.
// In other words the game will use the changes during this startup.


void StartXMLUpdate()
{
	const bool bEnableXMLUpdate = false;

	if (!bEnableXMLUpdate)
	{
		return;
	}

	// tells where the xml file is relative to assets/xml/
	XMLUpdateEngine("Events/CIV4EventTriggerInfos.xml");

	// file is automatically saved
}

// called for each entry in the file
// note that tag order can be ignored as it's automatically adjusted to match the schema
void XMLUpdateEngine::updateEntry(const XML_tag rootTag)
{
	XML_tag plotType = rootTag.findChild("ePlotType");

	if (!plotType.isPresent())
	{
		return;
	}

	if (!plotType.isNone())
	{
		rootTag.findOrCreateChild("PlotRequirements").findOrCreateChild("PlotTypes").findOrCreateChild("ePlotType", plotType.getText());
	}
	plotType.deleteElement();
}
