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

class HappinessAdvisor(BaseAdvisorWindow.BaseAdvisorWindow):
	def __init__(self, parent):
		BaseAdvisorWindow.BaseAdvisorWindow.__init__(self, parent, "HappinessStateClass")
		
	def drawColonyCell(self, iCity, pCity, iColumn, noInfo):
		#  0 Total Happiness
		if (iColumn == 0):
			if (pCity.getCityHappiness() > pCity.getCityUnHappiness() ):
				self.tableManager.addText("<color=0,255,0>" + unicode(pCity.getCityHappiness() - pCity.getCityUnHappiness()) + "</font>")
			elif (pCity.getCityHappiness() < pCity.getCityUnHappiness() ):
				self.tableManager.addText("<color=255,0,0>" + unicode(pCity.getCityHappiness() - pCity.getCityUnHappiness()) + "</font>")
			elif (pCity.getCityHappiness() == pCity.getCityUnHappiness() ):
				self.tableManager.addText(unicode(pCity.getCityHappiness() - pCity.getCityUnHappiness()))
		#  1 Happiness from Crosses
		elif (iColumn == 1):
			self.tableManager.addIntLeft(pCity.getHappinessFromCrosses())
		#  2 Happiness from Bells
		elif (iColumn == 2):
			self.tableManager.addIntLeft(pCity.getHappinessFromBells())
		#  3 Happiness from Health
		elif (iColumn == 3):
			self.tableManager.addIntLeft(pCity.getHappinessFromHealth())
		#  4 Happiness from Culture
		elif (iColumn == 4):
			self.tableManager.addIntLeft(pCity.getHappinessFromCulture())
		#  5 Happiness from Law
		elif (iColumn == 5):
			self.tableManager.addIntLeft(pCity.getHappinessFromLaw())
		#  6 Happiness from Education
		elif (iColumn == 6):
			self.tableManager.addIntLeft(pCity.getHappinessFromEducation())
		#  7 Happiness from Domestic Market
		elif (iColumn == 7):
			self.tableManager.addIntLeft(pCity.getHappinessFromDomesticDemandsFulfilled())
		#  8 Happiness from Treates
		elif (iColumn == 8):
			self.tableManager.addIntLeft(pCity.getHappinessFromTreaties())
		#  9 Unhappiness from Population
		elif (iColumn == 9):
			self.tableManager.addIntLeft(pCity.getUnhappinessFromPopulation())
		# 10 Unhappiness from Crime
		elif (iColumn == 10):
			self.tableManager.addIntLeft(pCity.getUnhappinessFromCrime())
		# 11 Unhappiness from Slavery
		elif (iColumn == 11):
			self.tableManager.addIntLeft(pCity.getUnhappinessFromSlavery())
		# 12 Unhappiness from Wars
		elif (iColumn == 12):
			self.tableManager.addIntLeft(pCity.getUnhappinessFromWars())
		# 13 Unhappiness from Missing Defense
		elif (iColumn == 13):
			self.tableManager.addIntLeft(pCity.getUnhappinessFromMissingDefense())
		# 14 Unhappiness from Tax Rate
		elif (iColumn == 14):
			self.tableManager.addIntLeft(pCity.getUnhappinessFromTaxRate())

		else:
			# shouldn't be needed, but in case of errors, empty cells are better than columns and column headers going out of sync
			self.tableManager.addText("")
		
		
	def setDirty(self):
		self.dirty = True
		
	def __addTableHeader(self, char):
		self.tableManager.addHeaderName((u" %c" % gc.getYieldInfo(YieldTypes.YIELD_HAPPINESS).getChar()) + (u" %c" % char), 65)
		
	def __addTableHeaderSymbol(self, iSymbol):
		self.__addTableHeader(CyGame().getSymbolID(iSymbol))
		
	def __addTableHeaderYield(self, iYield):
		self.__addTableHeader(gc.getYieldInfo(iYield).getChar())
		
	def createTableHeader(self):
		# create table headers
		self.tableManager.addHeaderButton()
		self.tableManager.addHeaderCityName()
		
		# the list of columns
		# note that drawColonyCell will have iColumn set according to the list here
		
		#  0 Total Happiness
		self.tableManager.addHeaderName((u" %c" % gc.getYieldInfo(YieldTypes.YIELD_HAPPINESS).getChar()) +"(TOTAL)")
		#  1 Happiness from Crosses
		self.__addTableHeaderYield   (YieldTypes.YIELD_CROSSES)
		#  2 Happiness from Bells
		self.__addTableHeaderYield   (YieldTypes.YIELD_BELLS)
		#  3 Happiness from Health
		self.__addTableHeaderSymbol  (FontSymbols.HEALTHY_CHAR)
		#  4 Happiness from Culture
		self.__addTableHeaderYield   (YieldTypes.YIELD_CULTURE)
		#  5 Happiness from Law
		self.__addTableHeaderYield   (YieldTypes.YIELD_LAW)
		#  6 Happiness from Education
		self.__addTableHeaderYield   (YieldTypes.YIELD_EDUCATION)
		#  7 Happiness from Domestic Market
		self.__addTableHeaderYield   (YieldTypes.YIELD_HOUSEHOLD_GOODS)
		#  8 Happiness from Treates
		self.__addTableHeaderSymbol  (FontSymbols.TRADE_CHAR)
		#  9 Unhappiness from Population
		self.__addTableHeaderSymbol  (FontSymbols.ANGRY_POP_CHAR)
		# 10 Unhappiness from Crime
		self.__addTableHeaderYield   (YieldTypes.YIELD_CRIME)
		# 11 Unhappiness from Slavery
		self.__addTableHeaderSymbol  (FontSymbols.OCCUPATION_CHAR)
		# 12 Unhappiness from Wars
		self.__addTableHeaderYield   (YieldTypes.YIELD_BLADES)
		# 13 Unhappiness from Missing Defense
		self.__addTableHeaderSymbol  (FontSymbols.DEFENSE_CHAR)
		# 14 Unhappiness from Tax Rate
		self.__addTableHeaderSymbol  (FontSymbols.GOLD_CHAR)
		
		
		self.tableManager.buildHeaderArray()
		self.tableManager.expandColumnsToFillTableWidth()
