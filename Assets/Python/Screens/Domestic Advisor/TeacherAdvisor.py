from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums

import DomesticAdvisorTable
import BaseAdvisorWindow
import MultiPageDomesticAdvisor

## Display which units can be trained in various colonies
## Idea and written by Raubwürger
## Converted to new format and hardcoding removed by Nightinggale

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class TeacherAdvisor(MultiPageDomesticAdvisor.MultiPageDomesticAdvisor):
	def __init__(self, parent):
		MultiPageDomesticAdvisor.MultiPageDomesticAdvisor.__init__(self, parent, "TeacherAdvisor")
		
	def setupPages(self, parent, name):
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		
		for i in range(player.getMaxTeachLevel()):
			self.pages.append(TeacherAdvisorSingleLevel(parent, i+1))

class TeacherAdvisorSingleLevel(BaseAdvisorWindow.BaseAdvisorWindow):
	def __init__(self, parent, iTeachLevel):
		BaseAdvisorWindow.BaseAdvisorWindow.__init__(self, parent, "TeacherState" + str(iTeachLevel))
		self.iTeachLevel = iTeachLevel
		
	def drawColonyRowCustom(self, iCity, pCity):
		iBuilding = pCity.getDominantBuilding(SpecialBuildingTypes.SPECIALBUILDING_EDUCATION)
		if iBuilding != -1:
			self.tableManager.addPanelButton(gc.getBuildingInfo(iBuilding).getButton(), WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING, iBuilding, -1)
		else:
			self.tableManager.skipCell()
		self.tableManager.addTextRight(str(pCity.calculateNetYield(YieldTypes.YIELD_EDUCATION)))
		
	def drawColonyCell(self, iCity, pCity, iType, info):
		if pCity.canTeach(iType):
			self.tableManager.addPanelButton(info.getButton(), WidgetTypes.WIDGET_PEDIA_JUMP_TO_UNIT, iType, -1)
		else:
			self.tableManager.skipCell()
	
		
	def createTableHeader(self):
		# create table headers
		self.tableManager.defaultColumnWidth = self.tableManager.iRowHeight
		self.tableManager.bFillSinglePage = False
		
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		self.tableManager.addHeaderButton()
		self.tableManager.addHeaderCityName()
		self.tableManager.addHeaderTxt("", self.tableManager.defaultColumnWidth)
		self.tableManager.addHeaderTxt("", self.tableManager.defaultColumnWidth)
		self.tableManager.addHeaderArray(player.getTeachUnitTypes(self.iTeachLevel))
		
