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

class DomesticDemandAdvisor(BaseAdvisorWindow.BaseAdvisorWindow):
	def __init__(self, parent):
		BaseAdvisorWindow.BaseAdvisorWindow.__init__(self, parent, "DomesticDemandAdvisor")

	def drawColonyCell(self, iCity, pCity, iYield, pYieldInfo):
		iStored = pCity.getYieldStored(iYield)
		iDemand = pCity.getYieldDemand(iYield)
		
		colorA = "255,0,0"
		colorB = "255,0,0"
		
		if (iStored < iDemand):
			if (iStored == 0):
				colorA = "255,0,0"
			else:
				colorA = "0,255,255"
		elif (iStored > iDemand):
			colorA = "0,255,0"
			colorB = colorA
		else:
			colorA = "0,255,255"
			colorB = colorA
			
		self.tableManager.addText("<color=" + colorA + ">" + unicode(iStored) + " / " + "</color>" + "<color=" + colorB + ">" + unicode(iDemand) + " / " + "</color>" + unicode(pCity.getYieldBuyPrice(iYield)))


	def createTableHeader(self):
		# create table headers
		
		self.tableManager.defaultColumnWidth = 85
		
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		self.tableManager.addHeaderButton()
		self.tableManager.addHeaderCityName()
		self.tableManager.addHeaderArray(player.getDomesticDemandYieldTypes())
