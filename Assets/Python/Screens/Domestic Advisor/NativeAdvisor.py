## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums

import DomesticAdvisorTable

## Based on work from R&R, Robert Surcouf,  Domestic Advisor Screen

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class NativeAdvisor:
	def __init__(self, parent):
		self.parent = parent
		self.drawn = False
		self.name = "NativeStateClass"
		self.screenName = self.name + "ListBackground"
		self.settlementIcon = gc.getBuildingInfo(gc.getInfoTypeForString("BUILDING_CHICKEE")).getButton()
		self.tradeStationChar = gc.getYieldInfo(YieldTypes.YIELD_TRADE_GOODS).getChar()
		self.tradeStationText = u" %c" % self.tradeStationChar
		self.dirty = True
		
		self.tableManager = DomesticAdvisorTable.DomesticAdvisorTable(parent, self.screenName)
		
	def getKnownSettlementList(self):
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		knownList = []
		citylist = self.parent.GetNativeCities()
		for iNativeCity in range(len(citylist)):
			pLoopCity = citylist[iNativeCity]
			
			ePlayer = gc.getPlayer(pLoopCity.getOwner())
			
			# figure out if the city should be displayed
			bIsVisited = pLoopCity.isScoutVisited(gc.getGame().getActiveTeam())
			
			# visited cities are always known. No need to test visited cities
			bIsKnown = bIsVisited
			
			if not bIsKnown:
				if (ePlayer.isAlive() and (gc.getTeam(ePlayer.getTeam()).isHasMet(player.getTeam()))):
					bIsKnown = pLoopCity.isRevealed(player.getTeam(), False)
			
			# add a line for every single known native city
			if bIsKnown:
				knownList.append(pLoopCity)
		return knownList
		
	def draw(self):
		if not self.dirty and False:
			self.tableManager.show()
			return
			
		self.dirty = False
		citylist = self.getKnownSettlementList()		
		if self.tableManager.isNeedInit():
			self.createTableHeader()
			
		self.tableManager.setNumRows(len(citylist))
		
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		self.tableManager.show()
		self.tableManager.clearRows()
		
		for iNativeCity in range(len(citylist)):
			pLoopCity = citylist[iNativeCity]
			
			ePlayer = gc.getPlayer(pLoopCity.getOwner())
			
			# figure out if the city should be displayed
			bIsVisited = pLoopCity.isScoutVisited(gc.getGame().getActiveTeam())
			
			# visited cities are always known. No need to test visited cities
			bIsKnown = bIsVisited
			
			if not bIsKnown:
				if (ePlayer.isAlive() and (gc.getTeam(ePlayer.getTeam()).isHasMet(player.getTeam()))):
					bIsKnown = pLoopCity.isRevealed(player.getTeam(), False)
			
			# add a line for every single known native city
			if bIsKnown:
				self.tableManager.addPanelButton(self.settlementIcon, WidgetTypes.WIDGET_JUMP_TO_SETTLEMENT, pLoopCity.plot().getIndex())
				self.tableManager.addText(pLoopCity.getName())
				self.tableManager.addText(gc.getPlayer(pLoopCity.getOwner()).getCivilizationShortDescription(0))
				self.tableManager.addInt(pLoopCity.getPopulation())
				
				if bIsVisited:
					self.tableManager.drawChar(gc.getYieldInfo(pLoopCity.AI_getDesiredYield()))
				else:
					self.tableManager.skipCell()
				
				if pLoopCity.getTradePostPlayer() != -1:
					self.tableManager.addText(self.tradeStationText, pLoopCity.getTradePostPlayer(), -1, player.getWikiWidget())
				else:
					self.tableManager.skipCell()
				
				# mission icon
				if pLoopCity.getMissionaryRate() > 0 and pLoopCity.getMissionaryPlayer() != PlayerTypes.NO_PLAYER:
					pMissionPlayer = gc.getPlayer(pLoopCity.getMissionaryPlayer())
					if gc.getGame().isOption(GameOptionTypes.GAMEOPTION_NO_MORE_VARIABLES_HIDDEN) and pLoopCity.getMissionaryPlayer() == player.getID():
						iModifier = 100 + player.getMissionaryRateModifier()+ ePlayer.getMissionaryRateModifier()
						szText = localText.getText("TXT_KEY_GROWTH", (ePlayer.getMissionaryPoints(pMissionPlayer.getID()),ePlayer.missionaryThreshold(pMissionPlayer.getID()), pLoopCity.getMissionaryRate() * iModifier /100))
						self.tableManager.addText(szText, pMissionPlayer.getID(), -1, pMissionPlayer.getWikiWidget())
					else:
						self.tableManager.drawChar(pMissionPlayer)
				else:
					self.tableManager.skipCell()

				# teach unit
				if bIsVisited:
					self.tableManager.addText(gc.getUnitInfo(pLoopCity.getTeachUnit()).getDescription(), pLoopCity.getTeachUnit(), -1, WidgetTypes.WIDGET_PEDIA_JUMP_TO_UNIT)
				else:
					self.tableManager.skipCell()
	
	def handleInput (self, inputClass):
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if (inputClass.getButtonType() == WidgetTypes.WIDGET_JUMP_TO_SETTLEMENT):
				self.parent.getScreen().hideScreen()
				CyCamera().JustLookAtPlot(gc.getMap().plotByIndex(inputClass.iData1))
				return 0
		
		return -1
	
	def hide(self):
		self.tableManager.hide()
		
	def setDirty(self):
		self.dirty = True
		
	def createTableHeader(self):
		# create table headers
		
		self.tableManager.tableInit()
		self.tableManager.addHeaderButton()
		self.tableManager.addHeaderCityName()
		self.tableManager.addHeaderCityName("CIV")
		self.tableManager.addHeaderTxt("TXT_KEY_POPULATION", 50)
		self.tableManager.addHeaderChar(CyGame().getSymbolID(FontSymbols.TRADE_CHAR))
		self.tableManager.addHeaderChar(self.tradeStationChar)
		
		# not hiding variables requires a wider column for missions
		iCrossWidth = 50
		if gc.getGame().isOption(GameOptionTypes.GAMEOPTION_NO_MORE_VARIABLES_HIDDEN):
			iCrossWidth = 130
		
		self.tableManager.addHeaderChar(gc.getYieldInfo(YieldTypes.YIELD_CROSSES).getChar(), iCrossWidth)
		self.tableManager.addHeaderTxt("TXT_KEY_PEDIA_SPECIAL_ABILITIES", 200)
		
		self.tableManager.tableHeaderComplete()
		

	