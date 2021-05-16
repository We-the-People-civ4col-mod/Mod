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

class WarehouseAdvisor(BaseAdvisorWindow.BaseAdvisorWindow):
	def __init__(self, parent):
		BaseAdvisorWindow.BaseAdvisorWindow.__init__(self, parent, "WarehouseStateClass")
	
	def drawColonyRow(self, iCity, pCity):
		# a whole lot of code, but all it does is calculating the warehouse column
		iMaxYield = pCity.getMaxYieldCapacity()
		aiProducedYields = [[]] * YieldTypes.NUM_YIELD_TYPES
		for iYield in range(YieldTypes.NUM_YIELD_TYPES):
			aiProducedYields[iYield] = 0
		iNetYield = 0
		for iYield in range(YieldTypes.NUM_YIELD_TYPES):
			iUnproducedYield = pCity.calculateActualYieldProduced(iYield) - pCity.getBaseRawYieldProduced(iYield)
			if iUnproducedYield < 0:
				for iProfession in range(gc.getNumProfessionInfos()):
					if gc.getProfessionInfo(iProfession).getYieldsProduced(0) == iYield: #MultipleYieldsProduced Start
						iNeedYield = gc.getProfessionInfo(iProfession).getYieldsConsumed(0) #MultipleYieldsProduced Start
						aiProducedYields[iNeedYield] += iUnproducedYield
			iProducedYield = pCity.calculateNetYield(iYield)
			aiProducedYields[iYield] += iProducedYield

			if iYield == YieldTypes.YIELD_FOOD or iYield == YieldTypes.YIELD_LUMBER or iYield == YieldTypes.YIELD_STONE or not gc.getYieldInfo(iYield).isCargo(): # R&R, ray, small fix for Display
				continue
			iNetYield += pCity.getYieldStored(iYield)

		iProdusedYield = 0
		for iYield in range(YieldTypes.NUM_YIELD_TYPES):
			if iYield != YieldTypes.YIELD_FOOD and iYield != YieldTypes.YIELD_LUMBER and iYield != YieldTypes.YIELD_STONE and gc.getYieldInfo(iYield).isCargo(): # R&R, ray, small fix for Display
				iProdusedYield += aiProducedYields[iYield]

		szText = u"<font=2><color=" ## R&R, Robert Surcouf,  Domestic Advisor Screen
		if iNetYield > iMaxYield:
			szText += u"255,0,0"
		elif iNetYield + iProdusedYield > iMaxYield:
			szText += u"255,255,0"
		else:
			szText += u"0,255,0"
		szText += u">" + str(iNetYield)
		if iProdusedYield > 0:
			szText += u"+" + str(iProdusedYield)
		elif iProdusedYield < 0:
			szText += str(iProdusedYield)
		szText += u"/" + str(iMaxYield) + u"</color></font>"
		
		# print the text
		self.tableManager.addTextRight(szText)
		
		# fill all the yield columns
		self.tableManager.autofillRow(iCity, pCity)

	def drawColonyCell(self, iCity, pCity, iYield, info):
		iNetYield = pCity.getYieldStored(iYield)
		if iNetYield == 0:
			self.tableManager.skipCell()
		else:
			self.tableManager.addTextRight("<color=0,255,0>" + unicode(iNetYield) + u"</color>")
		
	def createTableHeader(self):
		# create table headers
		self.tableManager.addHeaderButton()
		self.tableManager.addHeaderCityName()
		self.tableManager.addHeaderTxt("MAX", self.parent.iWareHouseW)
		self.tableManager.addHeaderArrayYields()
