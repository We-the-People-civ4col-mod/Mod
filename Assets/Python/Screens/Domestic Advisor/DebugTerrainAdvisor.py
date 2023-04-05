from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums

import DomesticAdvisorTable
import BaseAdvisorWindow

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class DebugTerrainAdvisor(BaseAdvisorWindow.BaseAdvisorWindow):
	def __init__(self, parent):
		BaseAdvisorWindow.BaseAdvisorWindow.__init__(self, parent, "TerrainState")
	
	def drawTableContent(self):
		# first get an InfoArray of the terrain counts
		# beware that any terrain with a count of 0 won't be in the InfoArray, hence needs to be set as default
		count = [0] * gc.getNumTerrainInfos()
		info = gc.getMap().getTerrainCount()
		for i in range(info.getLength()):
			count[info.get(i)] = info.get1(i)
		
		self.tableManager.clearRows()
		self.tableManager.setNumRows(gc.getNumTerrainInfos())
		
		for i in range(gc.getNumTerrainInfos()):
			self.tableManager.getScreen().setTableText(self.tableManager.currentPageName(), 0, i, "<font=2>" + gc.getTerrainInfo(i).getDescription() + "</font>", "", WidgetTypes.WIDGET_PEDIA_JUMP_TO_TERRAIN, i, 1, CvUtil.FONT_LEFT_JUSTIFY )
			self.tableManager.getScreen().setTableText(self.tableManager.currentPageName(), 1, i, "<font=2>" + unicode(count[i]) + "</font>", "", WidgetTypes.WIDGET_PEDIA_JUMP_TO_TERRAIN, i, 1, CvUtil.FONT_RIGHT_JUSTIFY )


	def createTableHeader(self):
		self.tableManager.addHeaderTxt("Terrain", 200)
		self.tableManager.addHeaderTxt("Count", 75)
		self.tableManager.iRowHeight = 22
