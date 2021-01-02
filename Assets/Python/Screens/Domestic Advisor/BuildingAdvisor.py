## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums

import DomesticAdvisorTable
import BaseAdvisorWindow

## Based on work from R&R, Robert Surcouf,  Domestic Advisor Screen

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class BuildingAdvisor(BaseAdvisorWindow.BaseAdvisorWindow):
	def __init__(self, parent):
		BaseAdvisorWindow.BaseAdvisorWindow.__init__(self, parent, "BuildingStateClass")

	def drawColonyCell(self, iCity, pCity, iSpecial, pSpecialInfo):
		iIconBuilding = -1
		for iBuilding in range(gc.getNumBuildingInfos()):
			if gc.getBuildingInfo(iBuilding).getSpecialBuildingType() == iSpecial:
				if pCity.isHasBuilding(iBuilding):
					iIconBuilding = iBuilding
					break
		if (iIconBuilding != -1):
			self.tableManager.addPanelButton(gc.getBuildingInfo(iBuilding).getButton(), WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING, iBuilding)
		else:
			self.tableManager.skipCell()

	def createTableHeader(self):
		# create table headers
		self.tableManager.addHeaderButton()
		self.tableManager.addHeaderCityName()
		self.tableManager.addHeaderArrayBuildings()
