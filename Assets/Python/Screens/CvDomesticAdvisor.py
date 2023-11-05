## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums

import BuildingAdvisor
import DomesticDemandAdvisor
import HappinessAdvisor
import ImportExportAdvisor
import ProductionAdvisor
import NativeAdvisor
import WarehouseAdvisor
import TeacherAdvisor
import DebugTerrainAdvisor



## I rewrote most of the page to remove all hardcoded values
## In this process, a lot of the old code was moved and changed to look a lot differently
## Sadly this meant I had to give up preserving comments about who did what, which is a shame
##
##  R&R, Robert Surcouf did a lot of work expanding the Domestic Advisor screen
##   I used his code as a starting point for the new code since the outcome should be the same.
##   As a result the lines doing the actual drawing is his work even though loops and variable name changes makes it hard to see

## Guide to add pages - by Nightinggale
##
## Buttons are added at # Button generation
## They appear in the order they are entered
## 
## Subpages (left/right arrow accessible) are added with createSubpage(iState, iPage)
##   It adds pages to iState until iPage is the highest INDEX for a page (iPage = 2 gives pages 0,1,2)
##   Nothing happens if the page already exist meaning a loop can easily call it every time it writes to a page
##   Together with floor divide (//) and modulus (%) this allows a loop to make columns without knowing how many pages it fills before it starts
##
## Pages are accessed with self.StatePages[iState][iPage]
## Accessing the page (arrows and button pressing) is set up automatically


# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class CvDomesticAdvisor:
	"Domestic Advisor Screen"
	def __init__(self):
		self.listSelectedCities = []
		self.selectedSelectionGroupHeadUnitID = -1
		
	# Screen construction function
	def interfaceScreen(self):
		player = gc.getPlayer(gc.getGame().getActivePlayer())

		# Create a new screen, called DomesticAdvisur, using the file CvDomesticAdvisor.py for input
		screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )

		self.nScreenWidth = screen.getXResolution()
		self.nScreenHeight = (screen.getYResolution() - (screen.getYResolution() * 31 / 100))
		
		# widget setup of widget types, which only applies to this file
		self.WIDGET_MISSION = WidgetTypes(WidgetTypes.NUM_WIDGET_TYPES)
		
#VET NewCapacity - begin 1/4
		# WTP ray, careful this does not exist anymore, we set it to true
		# self.bNewCapacity = (gc.getDefineINT("NEW_CAPACITY") > 0)
		self.bNewCapacity = True
#VET NewCapacity - end 1/4
		## R&R, Robert Surcouf,  Domestic Advisor Screen - Start
		#self.nTableWidth = self.nScreenWidth * 19 / 20
		self.nTableWidth = self.nScreenWidth * 99 / 100
		## R&R, Robert Surcouf,  Domestic Advisor Screen - END
		self.nTableHeight = self.nScreenHeight - 150
		self.nNormalizedTableWidth = self.nTableWidth
		
		self.iButtonSpacing = 80
		self.iButtonSize = 60
		self.iCityButtonSize = 48
		## R&R, Robert Surcouf,  Domestic Advisor Screen - Start
		#self.Y_LOWER_ROW = self.nScreenHeight - 70
		self.Y_LOWER_ROW = self.nScreenHeight - 75
		#self.CITY_NAME_COLUMN_WIDTH = 250
		self.CITY_NAME_COLUMN_WIDTH = 190
		#self.PRODUCTION_COLUMN_SIZE = (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / len(range(YieldTypes.YIELD_FOOD, YieldTypes.YIELD_LUXURY_GOODS + 1))
#VET NewCapacity - begin 2/4
		#self.WAREHOUSE_COLUMN_SIZE = (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH - 60) / len(range(YieldTypes.YIELD_FOOD, YieldTypes.YIELD_LUXURY_GOODS+ 1))
		if self.bNewCapacity:
			##VET 
			##self.iWareHouseW = 120
			self.iWareHouseW = 100
		else:
			## VET self.iWareHouseW = 60
			self.iWareHouseW = 40
		#self.WAREHOUSE_COLUMN_SIZE = (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH - self.iWareHouseW) / len(range(YieldTypes.YIELD_FOOD, YieldTypes.YIELD_LUXURY_GOODS + 1))
#VET NewCapacity - end 2/4
		#self.BUILDING_COLUMN_SIZE = (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / (gc.getNumSpecialBuildingInfos() -1)
		## R&R, Robert Surcouf,  Domestic Advisor Screen - End
		self.ROW_HIGHT = 48

		self.Y_EXIT = self.nScreenHeight - 36
		self.X_EXIT = self.nScreenWidth - 30
		
		self.DEFAULT_COLUMN_WIDTH = 50
		
		self.iCurrentBuildingSubPage = 0
		self.iCurrentYieldSubPage = 0

		screen.setRenderInterfaceOnly(True)
		screen.setRenderFrozenWorld(True)
		screen.setDimensions((screen.getXResolution() - self.nScreenWidth) / 2, 0, self.nScreenWidth, self.nScreenHeight)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		# Here we set the background widget and exit button, and we show the screen
		screen.addPanel( "DomesticAdvisorBG", u"", u"", True, False, 0, 0, self.nScreenWidth, self.nScreenHeight, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDrawControl("DomesticAdvisorBG", ArtFileMgr.getInterfaceArtInfo("MAINMENU_SLIDESHOW_LOAD").getPath(), 0, 0, self.nScreenWidth, self.nScreenHeight, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDDSGFC("TopPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TITLE").getPath(), 0, 0, self.nScreenWidth, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDDSGFC("BottomPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_OFF").getPath(), 0, self.nScreenHeight - 55, self.nScreenWidth, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setText("DomesticExit", "Background", u"<font=4>" + localText.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + "</font>", CvUtil.FONT_RIGHT_JUSTIFY, self.X_EXIT, self.Y_EXIT, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )

		screen.setLabel("DomesticScreenWidgetHeader", "Background", u"<font=4b>" + localText.getText("TXT_KEY_DOMESTIC_ADVISOR_TITLE", ()).upper() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.nScreenWidth / 2, 4, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		self.StateButtons = []
		self.StatePages = []
		self.StateWindow = []
		
		# Button generation
		
		self.GAME_FONT_STATE = -1
		self.TERRAIN_STATE = -1
		
		self.GENERAL_STATE            = self.addButton("GeneralState",           "INTERFACE_CITY_MAP_BUTTON")
		self.DOMESTIC_DEMAND_STATE    = self.addButton("DomesticDemandState",    "INTERFACE_DOMESTIC_DEMAND_BUTTON"  , DomesticDemandAdvisor.DomesticDemandAdvisor(self))
		self.HAPPINESS_STATE          = self.addButton("HappinessState",         "INTERFACE_HAPPINESS_BUTTON"        , HappinessAdvisor.HappinessAdvisor(self))
		self.PRODUCTION_STATE         = self.addButton("ProductionState",        "INTERFACE_NET_YIELD_BUTTON"        , ProductionAdvisor.ProductionAdvisor(self))
		self.WAREHOUSE_STATE          = self.addButton("WareHouseState",         "INTERFACE_STORES_BUTTON"           , WarehouseAdvisor.WarehouseAdvisor(self))
		self.BUILDING_STATE           = self.addButton("BuildingState",          "INTERFACE_CITY_BUILD_BUTTON"       , BuildingAdvisor.BuildingAdvisor(self))
		self.IMPORTEXPORT_STATE       = self.addButton("ImportExportState",      "INTERFACE_CITY_GOVENOR_BUTTON"     , ImportExportAdvisor.ImportExportAdvisor(self))
		self.CITIZEN_STATE            = self.addButton("CitizenState",           "INTERFACE_CITY_CITIZEN_BUTTON")
		self.TOTAL_PRODUCTION_STATE   = self.addButton("TotalProductionState",   "INTERFACE_TOTAL_PRODUCTION_BUTTON")  # total production page - Nightinggale
		self.TRADEROUTE_STATE         = self.addButton("TradeRouteState",        "INTERFACE_IMPORT_EXPORT_BUTTON")
		self.TEACHER_STATE            = self.addButton("TeacherState",           "INTERFACE_TEACHER_LIST"            , TeacherAdvisor.TeacherAdvisor(self))
		self.NATIVE_STATE             = self.addButton("NativeState",            "INTERFACE_NATIVE_BUTTON"           , NativeAdvisor.NativeAdvisor(self))
		
		# WTP, ray, just putting the 3 new button references in here so nobdy has to search.
		# Domestic Demand Advisor Button: INTERFACE_DOMESTIC_DEMAND_BUTTON
		# Happiness Advisor Button: INTERFACE_HAPPINESS_BUTTON
		# Crime and Law Advisor Button: INTERFACE_LAW_BUTTON
		
		if (gc.getUserSettings().getDebugMaxGameFont() > 0):
			self.GAME_FONT_STATE      = self.addButton("GameFontState",          "INTERFACE_CITY_MAP_BUTTON")
			self.TERRAIN_STATE        = self.addButton("TerrainState",           "INTERFACE_NET_YIELD_BUTTON"        , DebugTerrainAdvisor.DebugTerrainAdvisor(self))
			self.GameFontSet = False
		
		self.YieldPages = set([self.WAREHOUSE_STATE])
		
		
		spaceForButtons = self.nScreenWidth - (self.X_EXIT - self.nScreenWidth)
		spaceForButtons -= 150 # making room for the exit text as it's right aligned
		spaceForEachButton = spaceForButtons // len(self.StateButtons)
		if spaceForEachButton < self.iButtonSpacing:
			self.iButtonSpacing = spaceForEachButton
		
		## R&R, Robert Surcouf,  Domestic Advisor Screen START
		
		# Exra left/right arrow buttons (big ones)... (2)
		self.StateButtons.append("INTERFACE_CITY_LEFT_ARROW")
		self.StateButtons.append("INTERFACE_CITY_RIGHT_ARROW")
		# Next Page / Previous Page
		self.MAX_YIELDS_IN_A_PAGE = 20
		#self.MAX_YIELDS_IN_A_PAGE = 19
		#self.MAX_BUILDINGS_IN_A_PAGE = 26
		self.MAX_BUILDINGS_IN_A_PAGE = 18
		
		self.WAREHOUSE_COLUMN_SIZE = (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH - self.iWareHouseW) / self.MAX_YIELDS_IN_A_PAGE
		self.PRODUCTION_COLUMN_SIZE = (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / self.MAX_YIELDS_IN_A_PAGE
		self.BUILDING_COLUMN_SIZE = (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / self.MAX_BUILDINGS_IN_A_PAGE
		## R&R, Robert Surcouf,  Domestic Advisor Screen END

		#Default State on Screen opening
		self.CurrentState = gc.getDomesticAdvisorState()
		if self.CurrentState >= len(self.StatePages):
			# a button has vanished since last time
			# most likely the user turned off debug mode
			# set a different page to recover
			self.CurrentState = 0
		self.CurrentPage = 0
		
		self.RebuildArrays()

		#Initialize the Lists
		for iState in range(len(self.StatePages)):
			if iState != self.TRADEROUTE_STATE and iState != self.NATIVE_STATE and iState != self.GAME_FONT_STATE:
				self.initPage(iState, 0)

		self.createSubpage(self.GENERAL_STATE, 1)
		
		#GeneralState Headers
		szListName = self.StatePages[self.GENERAL_STATE][0] + "ListBackground"
		# Population Column
		screen.setTableColumnHeader( szListName, 2, "<font=2>" + localText.getText("TXT_KEY_POPULATION", ()) + "</font>", (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / 13 )
		# Liberty Column
		screen.setTableColumnHeader( szListName, 3, "<font=2>" + (u" %c" %(CyGame().getSymbolID(FontSymbols.POWER_CHAR))) + "</font>", (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / 13 )
		# Food Column
		screen.setTableColumnHeader( szListName, 5, "<font=2>" + (u" %c" % gc.getYieldInfo(YieldTypes.YIELD_FOOD).getChar()) + "</font>", (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / 13 )
		# Hammers Column
		screen.setTableColumnHeader( szListName, 6, "<font=2>" + (u" %c" % gc.getYieldInfo(YieldTypes.YIELD_HAMMERS).getChar()) + "</font>", (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / 13 )
		# Bells Column
		screen.setTableColumnHeader( szListName, 8, "<font=2>" + (u" %c" % gc.getYieldInfo(YieldTypes.YIELD_BELLS).getChar()) + "</font>", (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / 13 )
		# Crosses Column
		screen.setTableColumnHeader( szListName, 10, "<font=2>" + (u" %c" % gc.getYieldInfo(YieldTypes.YIELD_CROSSES).getChar()) + "</font>", (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / 13 )
		# Education Column
		screen.setTableColumnHeader( szListName, 11, "<font=2>" + (u" %c" % gc.getYieldInfo(YieldTypes.YIELD_EDUCATION).getChar()) + "</font>", (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / 13 )
		# Garrison Column
		screen.setTableColumnHeader( szListName, 13, "<font=2>" + (u" %c" % CyGame().getSymbolID(FontSymbols.STRENGTH_CHAR)) + "</font>", (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / 13 )
		# Defense Column
		screen.setTableColumnHeader( szListName, 14, "<font=2>" + (u" %c" % CyGame().getSymbolID(FontSymbols.DEFENSE_CHAR)) + "</font>", (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / 13 )
		# Happiness Column
		screen.setTableColumnHeader( szListName, 15, "<font=2>" + (u" %c" % gc.getYieldInfo(YieldTypes.YIELD_HAPPINESS).getChar()) + "</font>", (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / 13 )
		# Health Column
		screen.setTableColumnHeader( szListName, 16, "<font=2>" + (u" %c" % gc.getYieldInfo(YieldTypes.YIELD_HEALTH).getChar()) + "</font>", (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / 13 )
		# Production Column
		screen.setTableColumnHeader( szListName, 17, "<font=2>" + localText.getText("TXT_KEY_DOMESTIC_ADVISOR_PRODUCING", ()).upper() + "</font>", (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / 4 )

		## R&R, Robert Surcouf,  Domestic Advisor Screen START
		szListName = self.StatePages[self.GENERAL_STATE][1] + "ListBackground"
		# Culture Column
		screen.setTableColumnHeader(szListName, 2, "<font=2>" + (u" %c" % gc.getYieldInfo(YieldTypes.YIELD_BELLS).getChar()) + "</font>", (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / 12 )
		# Culture Column
		screen.setTableColumnHeader( szListName, 3, "<font=2>" + localText.getText("TXT_KEY_ADVISOR_CULTURE", ()).upper() + "</font>", (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / 4 )
		## R&R, Robert Surcouf,  Domestic Advisor Screen END

		#WareHouseState Headers
#VET NewCapacity - begin 3/4
		#screen.setTableColumnHeader( "WareHouseStateListBackground", 2, "<font=3>" + "MAX" + "</font>", 60 )
		## R&R, Robert Surcouf,  Domestic Advisor Screen START
		#### old VET 
		####screen.setTableColumnHeader( "WareHouseStateListBackground", 2, "<font=3>" + "MAX" + "</font>", self.iWareHouseW )
		## R&R, Robert Surcouf,  Domestic Advisor Screen END
#VET NewCapacity - begin 3/4
		for iYield in range(YieldTypes.YIELD_FOOD, YieldTypes.YIELD_LUXURY_GOODS + 1):
			iYieldOnPage = iYield % self.MAX_YIELDS_IN_A_PAGE
			iPage = iYield // self.MAX_YIELDS_IN_A_PAGE
			self.createSubpage(self.WAREHOUSE_STATE, iPage)
			screen.setTableColumnHeader( self.StatePages[self.WAREHOUSE_STATE][iPage] + "ListBackground", iYieldOnPage + 3, "<font=2> " + (u" %c" % gc.getYieldInfo(iYield).getChar()) + "</font>", (self.WAREHOUSE_COLUMN_SIZE * self.nTableWidth) / self.nNormalizedTableWidth )
		
		for PageName in self.StatePages[self.WAREHOUSE_STATE]:
			screen.setTableColumnHeader( PageName + "ListBackground", 2, "<font=2>" + "MAX" + "</font>", self.iWareHouseW)
		
		# Headers for pages showing yields
		for iState in [self.PRODUCTION_STATE, self.IMPORTEXPORT_STATE, self.TOTAL_PRODUCTION_STATE]: # total production page - Nightinggale
			self.YieldPages.add(iState)
			for iYield in range(YieldTypes.YIELD_FOOD, YieldTypes.YIELD_LUXURY_GOODS + 1):
				iYieldOnPage = iYield % self.MAX_YIELDS_IN_A_PAGE
				iPage = iYield // self.MAX_YIELDS_IN_A_PAGE
				self.createSubpage(iState, iPage)
				screen.setTableColumnHeader( self.StatePages[iState][iPage] + "ListBackground", iYieldOnPage + 2, "<font=2> " + (u" %c" % gc.getYieldInfo(iYield).getChar()) + "</font>", (self.PRODUCTION_COLUMN_SIZE * self.nTableWidth) / self.nNormalizedTableWidth )

		# Building Headers
		for iSpecial in range(gc.getNumSpecialBuildingInfos()):
			if (iSpecial != gc.getInfoTypeForString("SPECIALBUILDING_BELLS")):
				iBuildingOnPage = (iSpecial-1) % self.MAX_BUILDINGS_IN_A_PAGE
				iPage = (iSpecial-1) // self.MAX_BUILDINGS_IN_A_PAGE
				self.createSubpage(self.BUILDING_STATE, iPage)
				
				if (iSpecial == gc.getInfoTypeForString("SPECIALBUILDING_WHALE_OIL")):
					screen.setTableColumnHeader( self.StatePages[self.BUILDING_STATE][iPage] + "ListBackground", iBuildingOnPage + 2, "<font=2> " + (u" %c" %  gc.getYieldInfo(YieldTypes.YIELD_WHALE_OIL).getChar()) + "</font>", (self.BUILDING_COLUMN_SIZE * self.nTableWidth) / self.nNormalizedTableWidth )				
				else:
					screen.setTableColumnHeader( self.StatePages[self.BUILDING_STATE][iPage] + "ListBackground", iBuildingOnPage + 2, "<font=2> " + (u" %c" %  gc.getSpecialBuildingInfo(iSpecial).getChar())         + "</font>", (self.BUILDING_COLUMN_SIZE * self.nTableWidth) / self.nNormalizedTableWidth )
	
		# Citizen Headers
		screen.setTableColumnHeader( self.StatePages[self.CITIZEN_STATE][0] + "ListBackground", 2, "<font=2>" +  localText.getText("TXT_KEY_DOMESTIC_ADVISOR_STATE_CITIZEN", ()).upper() + "</font>", self.nTableWidth * 3 / 4 )
			
		
		# Draw the city list...
		self.drawContents()
		
	def drawButtons(self):
		screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )

		#for iState in range(len(self.StateTypes)):
		for iState in range(len(self.StatePages)):
			for PageName in self.StatePages[iState]:
				szStateName = PageName + "ListBackground"
				## R&R, Robert Surcouf,  Domestic Advisor Screen START
				#screen.setImageButton(szStateName + "Button", ArtFileMgr.getInterfaceArtInfo(self.StateButtons[iState]).getPath(), (self.iButtonSpacing * iState) + (self.iButtonSpacing / 2), self.Y_LOWER_ROW, self.iButtonSize, self.iButtonSize, WidgetTypes.WIDGET_GENERAL, iState, -1 )
				#if (iState < len(self.StatePages)):
				screen.setImageButton(szStateName + "Button", ArtFileMgr.getInterfaceArtInfo(self.StateButtons[iState]).getPath(), (self.iButtonSpacing * iState) + (self.iButtonSpacing / 2), self.Y_LOWER_ROW, self.iButtonSize, self.iButtonSize, WidgetTypes.WIDGET_GENERAL, iState, -1 )
				## R&R, Robert Surcouf,  Domestic Advisor Screen END
				if (int(self.CurrentState) == iState):
					RelativeButtonSize = 130
					## R&R, Robert Surcouf,  Domestic Advisor Screen START
					#if (iState < len(self.StatePages)):
					screen.setImageButton("HighlightButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_HIGHLIGHTED_BUTTON").getPath(), (self.iButtonSpacing * iState) + (self.iButtonSpacing / 2) - ((self.iButtonSize * RelativeButtonSize / 100) / 2) + (self.iButtonSize / 2), self.Y_LOWER_ROW - ((self.iButtonSize * RelativeButtonSize / 100) / 2) + (self.iButtonSize / 2), self.iButtonSize * RelativeButtonSize / 100, self.iButtonSize * RelativeButtonSize / 100, WidgetTypes.WIDGET_GENERAL, iState, -1 )
				
				
		# auto-generated list creation - Nightinggale
		# Added hardcoded button values 100 and 102
		screen.setImageButton("MainLeftButton", ArtFileMgr.getInterfaceArtInfo(self.StateButtons[len(self.StateButtons)-2]).getPath(), 15, 20, 2*self.iButtonSize/3, 2*self.iButtonSize/3, WidgetTypes.WIDGET_GENERAL, 100, -1 )
		screen.setImageButton("MainRightButton", ArtFileMgr.getInterfaceArtInfo(self.StateButtons[len(self.StateButtons)-1]).getPath(), self.nScreenWidth -50, 20, 2*self.iButtonSize/3, 2*self.iButtonSize/3, WidgetTypes.WIDGET_GENERAL, 102, -1 )
			## R&R, Robert Surcouf,  Domestic Advisor Screen END
	# Function to draw the contents of the cityList passed in
	def drawContents (self):

		# Get the screen and the player
		screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )
		player = gc.getPlayer(CyGame().getActivePlayer())
		screen.moveToFront( "Background" )
		
		if self.StateWindow[self.CurrentState] != None:
			self.StateWindow[self.CurrentState].draw()

		# total production page - start - Nightinggale
		if self.CurrentState == self.TOTAL_PRODUCTION_STATE:
			self.updateTotalProduction()
		# total production page - end - Nightinggale
		#Loop through the cities and update the table
		## R&R, Robert Surcouf,  Domestic Advisor Screen START
		elif (self.CurrentState != self.TRADEROUTE_STATE and self.CurrentState != self.NATIVE_STATE and self.CurrentState != self.GAME_FONT_STATE):
		## R&R, Robert Surcouf,  Domestic Advisor Screen END
			for iCity in range(len(self.Cities)):
				if (self.Cities[iCity].getName() in self.listSelectedCities):
					screen.selectRow( self.StatePages[self.CurrentState][self.CurrentPage] + "ListBackground", iCity, True )
				self.updateCityTable(self.Cities[iCity], iCity)

			if (self.CurrentState == self.CITIZEN_STATE):
				for iCity in range(len(self.Cities)):
					self.updateCitizenTable(self.Cities[iCity], iCity)
		## R&R, Robert Surcouf,  Domestic Advisor Screen START
		#else:
		elif self.CurrentState == self.TRADEROUTE_STATE:
		## R&R, Robert Surcouf,  Domestic Advisor Screen END
			self.RebuildRouteTable()
			BonusRow = 0
			for iRoute in range(player.getNumTradeRoutes()):
				self.updateRouteTable(self.Routes[iRoute], player.getTradeRouteByIndex(iRoute).getID(), iRoute)
				BonusRow += 1
			self.BuildAllTransportsRow(BonusRow)
		elif self.CurrentState == self.GAME_FONT_STATE:
			self.drawGameFont()
		
		self.drawButtons()
		if self.StateWindow[self.CurrentState] == None:
			screen.show(self.StatePages[self.CurrentState][self.CurrentPage] + "ListBackground")
		self.updateAppropriateCitySelection()

	def updateCityTable(self, pLoopCity, i):
		screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )

		szState = self.StatePages[self.CurrentState][self.CurrentPage]
		screen.setTableText(szState + "ListBackground", 0, i, "", ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_CITYSELECTION").getPath(), WidgetTypes.WIDGET_ZOOM_CITY, pLoopCity.getOwner(), pLoopCity.getID(), CvUtil.FONT_LEFT_JUSTIFY);
		## R&R, Robert Surcouf,  Domestic Advisor Screen - Start
		# City Name (text font size reduction)
		#screen.setTableText(szState + "ListBackground", 1, i, "<font=4>" + pLoopCity.getName() + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
		screen.setTableText(szState + "ListBackground", 1, i, "<font=2>" + pLoopCity.getName() + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
		## R&R, Robert Surcouf,  Domestic Advisor Screen - End
		
		if(self.CurrentState == self.GENERAL_STATE and self.CurrentPage == 0):
			# Population
			screen.setTableInt(szState + "ListBackground", 2, i, "<font=2>" + unicode(pLoopCity.getPopulation()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Liberty
			screen.setTableInt(szState + "ListBackground", 3, i, "<font=2>" + unicode(pLoopCity.getRebelPercent()) + "%" + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Food rate
			iNetFood = pLoopCity.foodDifference()
			szText = unicode(iNetFood)
			if iNetFood > 0:
				szText = localText.getText("TXT_KEY_COLOR_POSITIVE", ()) + szText + localText.getText("TXT_KEY_COLOR_REVERT", ())
			elif iNetFood < 0:
				szText = localText.getText("TXT_KEY_COLOR_NEGATIVE", ()) + szText + localText.getText("TXT_KEY_COLOR_REVERT", ())
			screen.setTableInt(szState + "ListBackground", 5, i, "<font=2>" + szText + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Hammers rate
			screen.setTableInt(szState + "ListBackground", 6, i, "<font=2>" + unicode(pLoopCity.calculateNetYield(YieldTypes.YIELD_HAMMERS)) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			#Bell rate
			screen.setTableInt(szState + "ListBackground", 8, i, "<font=2>" + unicode(pLoopCity.calculateNetYield(YieldTypes.YIELD_BELLS)) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Crosses rate
			screen.setTableInt(szState + "ListBackground", 10, i, "<font=2>" + unicode(pLoopCity.calculateNetYield(YieldTypes.YIELD_CROSSES)) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Education rate
			screen.setTableInt(szState + "ListBackground", 11, i, "<font=2>" + unicode(pLoopCity.calculateNetYield(YieldTypes.YIELD_EDUCATION)) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Garrison
			screen.setTableInt(szState + "ListBackground", 13, i, "<font=2>" + unicode(pLoopCity.plot().getNumDefenders(pLoopCity.getOwner())) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Defense
			szBuffer = u"<font=2>%s%%</font>" % (str(pLoopCity.getDefenseModifier()))
			screen.setTableInt(szState + "ListBackground", 14, i, "<font=2>" + szBuffer + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Happiness
			if (pLoopCity.getCityHappiness() > pLoopCity.getCityUnHappiness() ):
				screen.setTableInt(szState + "ListBackground", 15, i, "<font=2>" + "<color=0,255,0>" +unicode(pLoopCity.getCityHappiness() - pLoopCity.getCityUnHappiness()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			elif (pLoopCity.getCityHappiness() == pLoopCity.getCityUnHappiness() ):
				screen.setTableInt(szState + "ListBackground", 15, i, "<font=2>" +unicode(pLoopCity.getCityHappiness() - pLoopCity.getCityUnHappiness()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			elif (pLoopCity.getCityHappiness() < pLoopCity.getCityUnHappiness() ):
				screen.setTableInt(szState + "ListBackground", 15, i, "<font=2>" + "<color=255,0,0>" +unicode(pLoopCity.getCityHappiness() - pLoopCity.getCityUnHappiness()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			#Health
			if (pLoopCity.getCityHealth() > 0 ):
				screen.setTableInt(szState + "ListBackground", 16, i, "<font=2>" + "<color=0,255,0>" +unicode(pLoopCity.getCityHealth()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			elif (pLoopCity.getCityHealth() == 0):
				screen.setTableInt(szState + "ListBackground", 16, i, "<font=2>" +unicode(pLoopCity.getCityHealth()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			elif (pLoopCity.getCityHealth() < 0):
				screen.setTableInt(szState + "ListBackground", 16, i, "<font=2>" + "<color=255,0,0>" +unicode(pLoopCity.getCityHealth()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Producing
			screen.setTableText(szState + "ListBackground", 17, i, "<font=2>" + pLoopCity.getProductionName() + " (" + str(pLoopCity.getGeneralProductionTurnsLeft()) + ")" + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )

		elif(self.CurrentState == self.PRODUCTION_STATE):
			start = self.YieldStart()
			for iYield in range(start, self.YieldEnd()):
				iNetYield = pLoopCity.calculateNetYield(iYield)
				szText = unicode(iNetYield)
				if iNetYield > 0:
					szText = localText.getText("TXT_KEY_COLOR_POSITIVE", ()) + u"+" + szText + localText.getText("TXT_KEY_COLOR_REVERT", ())
				elif iNetYield < 0:
					szText = localText.getText("TXT_KEY_COLOR_NEGATIVE", ()) + szText + localText.getText("TXT_KEY_COLOR_REVERT", ())
				elif iNetYield == 0:
					szText = ""
				screen.setTableInt(szState + "ListBackground", iYield - start + 2, i, "<font=1>" + szText + "<font/>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				
		elif(self.CurrentState == self.WAREHOUSE_STATE):
#VET NewCapacity - begin 4/4
			#screen.setTableInt("WareHouseStateListBackground", 2, i, u"<font=3><color=255,255,255>" + str(pLoopCity.getMaxYieldCapacity()) + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )

			#for iYield in range(YieldTypes.YIELD_FOOD, YieldTypes.YIELD_LUXURY_GOODS + 1):
			#	iNetYield = pLoopCity.getYieldStored(iYield)
			#	szText = unicode(iNetYield)
			#	if iNetYield == 0:
			#		szText = ""
			#	if (pLoopCity.calculateNetYield(iYield) * 5 + pLoopCity.getYieldStored(iYield) <= pLoopCity.getMaxYieldCapacity() or iYield == YieldTypes.YIELD_FOOD):
			#		screen.setTableInt("WareHouseStateListBackground", iYield + 3, i, u"<font=3><color=0,255,255>" + szText + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			#	elif (pLoopCity.getYieldStored(iYield) <= pLoopCity.getMaxYieldCapacity()):
			#		screen.setTableInt("WareHouseStateListBackground", iYield + 3, i, u"<font=3><color=255,255,0>" + szText + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			#	else:
			#		screen.setTableInt("WareHouseStateListBackground", iYield + 3, i, u"<font=3><color=255,0,0>" + szText + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			iMaxYield = pLoopCity.getMaxYieldCapacity()
			if self.bNewCapacity:
				aiProducedYields = [[]] * YieldTypes.NUM_YIELD_TYPES
				for iYield in range(YieldTypes.NUM_YIELD_TYPES):
					aiProducedYields[iYield] = 0
				iNetYield = 0
				for iYield in range(YieldTypes.NUM_YIELD_TYPES):
					iUnproducedYield = pLoopCity.calculateActualYieldProduced(iYield) - pLoopCity.getBaseRawYieldProduced(iYield)
					if iUnproducedYield < 0:
						for iProfession in range(gc.getNumProfessionInfos()):
							if gc.getProfessionInfo(iProfession).getYieldsProduced(0) == iYield: #MultipleYieldsProduced Start
								iNeedYield = gc.getProfessionInfo(iProfession).getYieldsConsumed(0) #MultipleYieldsProduced Start
								aiProducedYields[iNeedYield] += iUnproducedYield
					iProducedYield = pLoopCity.calculateNetYield(iYield)
					aiProducedYields[iYield] += iProducedYield

					# ray, fixed hardcoded check for Food, Stone and Lumber
					if gc.getYieldInfo(iYield).isIgnoredForStorageCapacity() or not gc.getYieldInfo(iYield).isCargo(): # R&R, ray, small fix for Display
						continue
					iNetYield += pLoopCity.getYieldStored(iYield)

				iProdusedYield = 0
				for iYield in range(YieldTypes.NUM_YIELD_TYPES):
					# ray, fixed hardcoded check for Food, Stone and Lumber
					if gc.getYieldInfo(iYield).isCargo() and not gc.getYieldInfo(iYield).isIgnoredForStorageCapacity(): # R&R, ray, small fix for Display
						iProdusedYield += aiProducedYields[iYield]

				#szText = u"<font=3><color=" 
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
				screen.setTableInt(self.StatePages[self.WAREHOUSE_STATE][self.CurrentPage] + "ListBackground", 2, i, szText, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				
				for iYield in range(self.YieldStart(), self.YieldEnd()):
					iNetYield = pLoopCity.getYieldStored(iYield)
					szText = unicode(iNetYield)
					if iNetYield == 0:
						szText = ""
					else:
						screen.setTableInt(szState + "ListBackground", (iYield % self.MAX_YIELDS_IN_A_PAGE) + 3, i, u"<font=2><color=0,255,0>" + szText + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			else:
				screen.setTableInt(self.StatePages[self.WAREHOUSE_STATE][0] + "ListBackground", 2, i, u"<font=2><color=255,255,255>" + str(pLoopCity.getMaxYieldCapacity()) + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				for iYield in range(YieldTypes.NUM_YIELD_TYPES):
					iNetYield = pLoopCity.getYieldStored(iYield)
					szText = unicode(iNetYield)
					# ray, making special storage capacity rules for Yields XML configurable
					bIgnoredForStorageCapacity = gc.getYieldInfo(iYield).isIgnoredForStorageCapacity()
					if iNetYield == 0:
						szText = ""
					## R&R, Robert Surcouf,  Domestic Advisor Screen - Start
					if (iYield < self.MAX_YIELDS_IN_A_PAGE ):
						#if (pLoopCity.calculateNetYield(iYield) * 5 + pLoopCity.getYieldStored(iYield) <= pLoopCity.getMaxYieldCapacity() or iYield == YieldTypes.YIELD_FOOD or iYield == YieldTypes.YIELD_LUMBER or iYield == YieldTypes.YIELD_STONE):  # R&R, ray, small fix for Display
						if ((pLoopCity.calculateNetYield(iYield) * 5 + pLoopCity.getYieldStored(iYield) <= pLoopCity.getMaxYieldCapacity()) or bIgnoredForStorageCapacity):
							screen.setTableInt(self.StatePages[self.WAREHOUSE_STATE][0] + "ListBackground", iYield + 3, i, u"<font=1><color=0,255,255>" + szText + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
						elif (pLoopCity.getYieldStored(iYield) <= pLoopCity.getMaxYieldCapacity()):			
							screen.setTableInt(self.StatePages[self.WAREHOUSE_STATE][0] + "ListBackground", iYield + 3, i, u"<font=1><color=255,255,0>" + szText + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
						else:
							screen.setTableInt(self.StatePages[self.WAREHOUSE_STATE][0] + "ListBackground", iYield + 3, i, u"<font=1><color=255,0,0>" + szText + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
					else:
						#if (pLoopCity.calculateNetYield(iYield) * 5 + pLoopCity.getYieldStored(iYield) <= pLoopCity.getMaxYieldCapacity() or iYield == YieldTypes.YIELD_FOOD or iYield == YieldTypes.YIELD_LUMBER or iYield == YieldTypes.YIELD_STONE):  # R&R, ray, small fix for Display
						if ((pLoopCity.calculateNetYield(iYield) * 5 + pLoopCity.getYieldStored(iYield) <= pLoopCity.getMaxYieldCapacity()) or bIgnoredForStorageCapacity):
							screen.setTableInt(self.StatePages[self.WAREHOUSE_STATE][1] + "ListBackground", iYield - self.MAX_YIELDS_IN_A_PAGE + 3, i, u"<font=1><color=0,255,255>" + szText + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
						elif (pLoopCity.getYieldStored(iYield) <= pLoopCity.getMaxYieldCapacity()):			
							screen.setTableInt(self.StatePages[self.WAREHOUSE_STATE][1] + "ListBackground", iYield - self.MAX_YIELDS_IN_A_PAGE + 3, i, u"<font=1><color=255,255,0>" + szText + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
						else:
							screen.setTableInt(self.StatePages[self.WAREHOUSE_STATE][1] + "ListBackground", iYield - self.MAX_YIELDS_IN_A_PAGE + 3, i, u"<font=1><color=255,0,0>" + szText + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
							#screen.setTableText("WareHouseStatePage2ListBackground", iYield - self.MAX_YIELDS_IN_A_PAGE + 3, i, u"<font=1><color=255,0,0>" + szText + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
					## R&R, Robert Surcouf,  Domestic Advisor Screen - End
#VET NewCapacity - end 4/4
				
		elif(self.CurrentState == self.BUILDING_STATE):
			start = (self.MAX_BUILDINGS_IN_A_PAGE * self.CurrentPage) + 1
			end = min((self.MAX_BUILDINGS_IN_A_PAGE * (self.CurrentPage + 1)) + 1, gc.getNumSpecialBuildingInfos()-1)
		
			for iSpecial in range(start, end):
				if (iSpecial != gc.getInfoTypeForString("SPECIALBUILDING_BELLS")):
					iIconBuilding = -1
					for iBuilding in range(gc.getNumBuildingInfos()):
						if gc.getBuildingInfo(iBuilding).getSpecialBuildingType() == iSpecial:
							if pLoopCity.isHasBuilding(iBuilding):
								iIconBuilding = iBuilding
								break
					if iIconBuilding != -1:
						screen.setTableInt(szState + "ListBackground", iSpecial - start  + 2, i, "", gc.getBuildingInfo(iBuilding).getButton(), WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING, iBuilding, -1, CvUtil.FONT_LEFT_JUSTIFY )
						
		elif(self.CurrentState == self.IMPORTEXPORT_STATE):
			start = self.YieldStart()
			for iYield in range(start, self.YieldEnd()):
				bExportYield = pLoopCity.isExport(iYield)
				bImportYield = pLoopCity.isImport(iYield)
				## R&R, Robert Surcouf,  Domestic Advisor Screen - End
				if (bExportYield and bImportYield):
					screen.setTableInt(szState + "ListBackground", iYield - start + 2, i, u"<font=2><color=255,255,0>" + localText.getText("TXT_KEY_IN_AND_OUT", ()) + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				elif (not bExportYield and bImportYield):
					screen.setTableInt(szState + "ListBackground", iYield - start + 2, i, u"<font=2><color=0,255,0>" + localText.getText("TXT_KEY_IN", ()) + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				elif (bExportYield and not bImportYield):
					screen.setTableInt(szState + "ListBackground", iYield - start + 2, i, u"<font=2><color=255,0,0>" + localText.getText("TXT_KEY_OUT", ()) + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )

			screen.setTableText(szState + "ListBackground", 1, i, "<font=2>" + pLoopCity.getName() + "</font>", "", WidgetTypes.WIDGET_YIELD_IMPORT_EXPORT, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# RR - Domestic Advisor Screen - END
			
		## R&R, Robert Surcouf,  Domestic Advisor Screen START
		elif(self.CurrentState == self.GENERAL_STATE and self.CurrentPage == 1): 
			#Culture rate
			screen.setTableInt(szState + "ListBackground", 2, i, "<font=3>" + unicode(pLoopCity.calculateNetYield(YieldTypes.YIELD_BELLS)) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Culture Level
			
			#screen.setTableText(szState + "ListBackground", 3, i, "<font=3>" + localText.getText("TXT_KEY_IMMIGRATION_BAR", (pLoopCity.getCultureLevel(),pLoopCity.getCultureThreshold(), gc.getYieldInfo(YieldTypes.YIELD_BELLS).getChar())) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			#screen.setTableText(szState + "ListBackground", 3, i, "<font=3>" + gc.getCultureLevelInfo(pLoopCity.getCultureLevel()).getTextKey()+"</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			###screen.setTableText(szState + "ListBackground", 3, i, "<font=3>" + gc.getCultureLevelInfo(pLoopCity.getCultureLevel()).getText()+"</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			screen.setTableText(szState + "ListBackground", 3, i, "<font=2>" + localText.getText("TXT_KEY_CITY_BAR_CULTURE", (pLoopCity.getCulture(pLoopCity.getOwner()),pLoopCity.getCultureThreshold(), gc.getCultureLevelInfo(pLoopCity.getCultureLevel()).getText()))+"</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			
		## R&R, Robert Surcouf,  Domestic Advisor Screen END
		
		## R&R, Robert Surcouf, Domestic Market display START
		elif(self.CurrentState == self.GENERAL_STATE and self.CurrentPage == 2): 
			iStartYield=gc.getDefineINT("DOMESTIC_MARKET_SCREEN_START_YIELD_ID")
			for iYield in range(iStartYield, YieldTypes.YIELD_LUXURY_GOODS + 1):
				#screen.setTableInt("GeneralStatePage3ListBackground", iYield-iStartYield + 2, i, "<font=2>" + unicode(pLoopCity.getYieldBuyPrice(iYield)) + "/"+ "<color=0,255,0>" +  unicode(pLoopCity.getYieldDemand(iYield)) + "</color>" "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				# CBM 0.8.020 display of quantity available in city - start 
				if (pLoopCity.getYieldStored(iYield)<pLoopCity.getYieldDemand(iYield)):
					# CBM - screen.setTableInt(szState + "ListBackground", iYield-iStartYield + 2, i, "<font=2>" + unicode(pLoopCity.getYieldBuyPrice(iYield)) + "/"+ "<color=255,0,0>" +  unicode(pLoopCity.getYieldDemand(iYield)) + "</color>" "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
					# CBM added differentiation between no stock and not enough stock - start
					if (pLoopCity.getYieldStored(iYield) == 0):
						screen.setTableInt(szState + "ListBackground", iYield-iStartYield + 2, i, "<font=2>" + "<color=255,0,0>" + unicode(pLoopCity.getYieldStored(iYield)) + " / " + "</color>" + "<color=255,0,0>" + unicode(pLoopCity.getYieldDemand(iYield)) + " / " + "</color>" + unicode(pLoopCity.getYieldBuyPrice(iYield)) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
					else:
						screen.setTableInt(szState + "ListBackground", iYield-iStartYield + 2, i, "<font=2>" + "<color=0,255,255>" + unicode(pLoopCity.getYieldStored(iYield)) + " / " + "</color>" + "<color=255,0,0>" + unicode(pLoopCity.getYieldDemand(iYield)) + " / " + "</color>" + unicode(pLoopCity.getYieldBuyPrice(iYield)) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
					# CBM added differentiation between no stock and not enough stock - end
				elif (pLoopCity.getYieldStored(iYield)>pLoopCity.getYieldDemand(iYield)):
					# CBM - screen.setTableInt(szState + "ListBackground", iYield-iStartYield + 2, i, "<font=2>" + unicode(pLoopCity.getYieldBuyPrice(iYield)) + "/"+ "<color=0,255,0>" +  unicode(pLoopCity.getYieldDemand(iYield)) + "</color>" "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
					screen.setTableInt(szState + "ListBackground", iYield-iStartYield + 2, i, "<font=2>" + "<color=0,255,0>" + unicode(pLoopCity.getYieldStored(iYield)) + " / " +  unicode(pLoopCity.getYieldDemand(iYield)) + " / " + "</color>" + unicode(pLoopCity.getYieldBuyPrice(iYield)) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				else:
					# CBM - screen.setTableInt(szState + "ListBackground", iYield-iStartYield + 2, i, "<font=2>" + unicode(pLoopCity.getYieldBuyPrice(iYield)) + "/"+ "<color=0,255,255>" +  unicode(pLoopCity.getYieldDemand(iYield)) + "</color>" "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
					screen.setTableInt(szState + "ListBackground", iYield-iStartYield + 2, i, "<font=2>" + "<color=0,255,255>" + unicode(pLoopCity.getYieldStored(iYield)) + " / " + unicode(pLoopCity.getYieldDemand(iYield)) + " / " + "</color>" + unicode(pLoopCity.getYieldBuyPrice(iYield)) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				# CBM 0.8.020 display of quantity available in city - end
		## R&R, Robert Surcouf, Domestic Market display End

		elif(self.CurrentState == self.GENERAL_STATE and self.CurrentPage == 3): 
			# Total Happiness
			if (pLoopCity.getCityHappiness() > pLoopCity.getCityUnHappiness() ):
				screen.setTableInt(szState + "ListBackground", 2, i, "<font=2>" + "<color=0,255,0>" +unicode(pLoopCity.getCityHappiness() - pLoopCity.getCityUnHappiness()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			elif (pLoopCity.getCityHappiness() < pLoopCity.getCityUnHappiness() ):
				screen.setTableInt(szState + "ListBackground", 2, i, "<font=2>" + "<color=255,0,0>" +unicode(pLoopCity.getCityHappiness() - pLoopCity.getCityUnHappiness()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			elif (pLoopCity.getCityHappiness() == pLoopCity.getCityUnHappiness() ):
				screen.setTableInt(szState + "ListBackground", 2, i, "<font=2>" +unicode(pLoopCity.getCityHappiness() - pLoopCity.getCityUnHappiness()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Happiness from Crosses
			screen.setTableInt(szState + "ListBackground", 5, i, "<font=2>" + unicode(pLoopCity.getHappinessFromCrosses()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Happiness from Bells
			screen.setTableInt(szState + "ListBackground", 6, i, "<font=2>" + unicode(pLoopCity.getHappinessFromBells()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Happiness from Health
			screen.setTableInt(szState + "ListBackground", 7, i, "<font=2>" + unicode(pLoopCity.getHappinessFromHealth()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Happiness from Culture
			screen.setTableInt(szState + "ListBackground", 8, i, "<font=2>" + unicode(pLoopCity.getHappinessFromCulture()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Happiness from Law
			screen.setTableInt(szState + "ListBackground", 9, i, "<font=2>" + unicode(pLoopCity.getHappinessFromLaw()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Happiness from Education
			screen.setTableInt(szState + "ListBackground", 10, i, "<font=2>" + unicode(pLoopCity.getHappinessFromEducation()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Happiness from Domestic Market
			screen.setTableInt(szState + "ListBackground", 11, i, "<font=2>" + unicode(pLoopCity.getHappinessFromDomesticDemandsFulfilled()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Happiness from Treates
			screen.setTableInt(szState + "ListBackground", 12, i, "<font=2>" + unicode(pLoopCity.getHappinessFromTreaties()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )

			# Unhappiness from Population
			screen.setTableInt(szState + "ListBackground", 13, i, "<font=2>" + unicode(pLoopCity.getUnhappinessFromPopulation()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Unhappiness from Crime
			screen.setTableInt(szState + "ListBackground", 14, i, "<font=2>" + unicode(pLoopCity.getUnhappinessFromCrime()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Unhappiness from Slavery
			screen.setTableInt(szState + "ListBackground", 15, i, "<font=2>" + unicode(pLoopCity.getUnhappinessFromSlavery()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Unhappiness from Wars
			screen.setTableInt(szState + "ListBackground", 16, i, "<font=2>" + unicode(pLoopCity.getUnhappinessFromWars()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Unhappiness from Missing Defense
			screen.setTableInt(szState + "ListBackground", 17, i, "<font=2>" + unicode(pLoopCity.getUnhappinessFromMissingDefense()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			# Unhappiness from Tax Rate
			screen.setTableInt(szState + "ListBackground", 18, i, "<font=2>" + unicode(pLoopCity.getUnhappinessFromTaxRate()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
		
	def updateCitizenTable(self, pCity, iRow):
		screen = CyGInterfaceScreen("DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR)

		if(self.CurrentState == self.CITIZEN_STATE):
			szState = self.StatePages[self.CurrentState][self.CurrentPage]
			screen.addPanel("CitizenPanel" + str(iRow), u"", u"", True, False, 0, 0, self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH, 30, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.attachControlToTableCell("CitizenPanel" + str(iRow), szState + "ListBackground", iRow, 2 )

			iSpace = (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH - (self.iCityButtonSize / 4)) / pCity.getPopulation()
			if iSpace > self.iCityButtonSize:
				iSpace = self.iCityButtonSize
			ButtonSizePercentage = 40
			for iCitizen in range(pCity.getPopulation() - 1, -1, -1):
				pCitizen = pCity.getPopulationUnitByIndex(iCitizen)
				iType = pCitizen.getUnitType()
				szButtonName = "CitizenGroupButton" + str(iRow) + "-" + str(iCitizen)
				# R&R, Robert Surcouf, Rebellion Fix START
				#screen.setImageButtonAt(szButtonName, "CitizenPanel" + str(iRow), gc.getUnitInfo(iType).getButton(), iCitizen * iSpace, self.iCityButtonSize / 5, self.iCityButtonSize * 4 / 5, self.iCityButtonSize * 4 / 5, WidgetTypes.WIDGET_CITIZEN, pCitizen.getID(), pCity.getID())
				#screen.setImageButtonAt("CitizenProfession" + str(iRow) + "-" + str(iCitizen), "CitizenPanel" + str(iRow), gc.getProfessionInfo(pCitizen.getProfession()).getButton(), (iCitizen * iSpace) + (self.iCityButtonSize * 2 / 4), 0, self.iCityButtonSize * ButtonSizePercentage / 100 , self.iCityButtonSize * ButtonSizePercentage / 100, WidgetTypes.WIDGET_CITIZEN, pCitizen.getID(), pCity.getID())
				if pCitizen.getProfession() != ProfessionTypes.NO_PROFESSION:
					screen.setImageButtonAt(szButtonName, "CitizenPanel" + str(iRow), gc.getUnitInfo(iType).getButton(), iCitizen * iSpace, self.iCityButtonSize / 5, self.iCityButtonSize * 4 / 5, self.iCityButtonSize * 4 / 5, WidgetTypes.WIDGET_CITIZEN, pCitizen.getID(), pCity.getID())
					screen.setImageButtonAt("CitizenProfession" + str(iRow) + "-" + str(iCitizen), "CitizenPanel" + str(iRow), gc.getProfessionInfo(pCitizen.getProfession()).getButton(), (iCitizen * iSpace) + (self.iCityButtonSize * 2 / 4), 0, self.iCityButtonSize * ButtonSizePercentage / 100 , self.iCityButtonSize * ButtonSizePercentage / 100, WidgetTypes.WIDGET_CITIZEN, pCitizen.getID(), pCity.getID())
				else:
					#screen.setTextAt( szButtonName, "CitizenPanel" + str(iRow),  u"<font=2>" + u"%c" %(CyGame().getSymbolID(FontSymbols.ANGRY_POP_CHAR))+ "</font>", CvUtil.FONT_RIGHT_JUSTIFY, iCitizen * iSpace, self.iCityButtonSize / 5, -0.3, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
					screen.setImageButtonAt(szButtonName, "CitizenPanel" + str(iRow), gc.getUnitInfo(iType).getButton(), iCitizen * iSpace, self.iCityButtonSize / 5, self.iCityButtonSize * 4 / 5, self.iCityButtonSize * 4 / 5, WidgetTypes.WIDGET_GENERAL, -1, -1)
					screen.setTextAt("CitizenProfession" + str(iRow) + "-" + str(iCitizen), "CitizenPanel" + str(iRow), u"<font=2>" + u"%c" %(CyGame().getSymbolID(FontSymbols.ANGRY_POP_CHAR))+ "</font>", CvUtil.FONT_RIGHT_JUSTIFY, iCitizen * iSpace +  (self.iCityButtonSize * 2 / 4) + self.iCityButtonSize/2, self.iCityButtonSize / 5, -0.3, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )#(iCitizen * iSpace) + (self.iCityButtonSize * 2 / 4), 0, WidgetTypes.WIDGET_GENERAL, -1, -1 )
				
				# R&R, Robert Surcouf, Rebellion Fix END
	def updateRouteTable(self, pRoute, iRoute, iRow):
		screen = CyGInterfaceScreen("DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR)

		if(self.CurrentState == self.TRADEROUTE_STATE):
			szState = self.StatePages[self.CurrentState][self.CurrentPage]
			player = gc.getPlayer(gc.getGame().getActivePlayer())

			screen.setTableText(szState + "ListBackground", 0, iRow, "<font=4>" + pRoute.getSourceCityName() + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
			screen.setTableText(szState + "ListBackground", 1, iRow, "<font=4>" + pRoute.getDestinationCityName() + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			screen.setTableText(szState + "ListBackground", 2, iRow, u"<font=2>%c</font>" % gc.getYieldInfo(pRoute.getYield()).getChar(), "", WidgetTypes.WIDGET_PEDIA_JUMP_TO_YIELDS, pRoute.getYield(), 1, CvUtil.FONT_LEFT_JUSTIFY )

			screen.addPanel("RoutePanel" + str(iRow), u"", u"", True, False, 0, 0, (390 * self.nTableWidth) / self.nNormalizedTableWidth, 30, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, iRoute, -1)
			screen.attachControlToTableCell("RoutePanel" + str(iRow), szState + "ListBackground", iRow, 4 )

			SelectionState = False
			iPosition = 10
			SelectedTransportID = -1
			ToggleButtonSet = False

			if (len(self.Transports)): ## R&R, Robert Surcouf, avoid division by zero !
				iSpace = (self.nTableWidth - 390) / len(self.Transports)
				if iSpace > self.iCityButtonSize:
					iSpace = self.iCityButtonSize
						
			for iSelectionGroupIndex in range(len(self.Transports) -1, -1, -1):
				SelectionGroup = self.Transports[iSelectionGroupIndex]

				if (not SelectionGroup.isNone()):
					GroupHeadID = SelectionGroup.getHeadUnit().getID()
					szButtonName = "UnitGroupButton" + str(SelectionGroup.getID()) + "Route" + str(iRow)
					if(SelectionGroup.isAssignedTradeRoute(pRoute.getID())):
						screen.addCheckBoxGFCAt("RoutePanel" + str(iRow), szButtonName, SelectionGroup.getHeadUnit().getButton(), ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), iSelectionGroupIndex * iSpace, 0, self.iCityButtonSize, self.iCityButtonSize, WidgetTypes.WIDGET_GENERAL, 10001, SelectionGroup.getHeadUnit().getID(), ButtonStyles.BUTTON_STYLE_LABEL )
					elif (not self.RouteValidity[iSelectionGroupIndex][iRow]):
						screen.addDDSGFCAt( szButtonName, "RoutePanel" + str(iRow), ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL").getPath(), iSelectionGroupIndex * iSpace, 0, self.iCityButtonSize, self.iCityButtonSize, WidgetTypes.WIDGET_GENERAL, -1, -1, False )

					iPosition += 20
					SelectedTransportID = self.selectedSelectionGroupHeadUnitID
					if (SelectedTransportID != -1):
						if (SelectedTransportID == SelectionGroup.getHeadUnit().getID()):
						
							if (self.RouteValidity[iSelectionGroupIndex][iRow]):
								if (SelectionGroup.isAssignedTradeRoute(pRoute.getID())):
									screen.setState(szButtonName, True)
									screen.setButtonGFC("RouteToggle" + str(iRow), "-", "", 0, 0, 60, 30, WidgetTypes.WIDGET_ASSIGN_TRADE_ROUTE, self.selectedSelectionGroupHeadUnitID, iRoute, ButtonStyles.BUTTON_STYLE_STANDARD )
								else:
									screen.setButtonGFC("RouteToggle" + str(iRow), "+", "", 0, 0, 60, 30, WidgetTypes.WIDGET_ASSIGN_TRADE_ROUTE, self.selectedSelectionGroupHeadUnitID, iRoute, ButtonStyles.BUTTON_STYLE_STANDARD )
								screen.attachControlToTableCell("RouteToggle" + str(iRow), szState + "ListBackground", iRow, 3 )
	
	# total production page - start - Nightinggale
	def updateTotalProduction(self):
		screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )

		if self.CurrentState != self.TOTAL_PRODUCTION_STATE:
			return
		
		szState = self.StatePages[self.CurrentState][self.CurrentPage]
		start = self.YieldStart()
		
		for i in range(0,2):
			sign = ""
			line_name = "Warehouse"
			if i == 0:
				sign = u"+"
				line_name = "Production"
			screen.setTableText(szState + "ListBackground", 0, i, "<font=2>" +""         + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			screen.setTableText(szState + "ListBackground", 1, i, "<font=2>" + line_name + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			for iYield in range(start, self.YieldEnd()):
				iNetYield = 0
				if i == 0:
					for iCity in range(len(self.Cities)):
						iNetYield += self.Cities[iCity].calculateNetYield(iYield)
				else:
					for iCity in range(len(self.Cities)):
						iNetYield += self.Cities[iCity].getYieldStored(iYield)
				szText = unicode(iNetYield)
				if iNetYield > 0:
					szText = localText.getText("TXT_KEY_COLOR_POSITIVE", ()) + sign + szText + localText.getText("TXT_KEY_COLOR_REVERT", ())
				elif iNetYield < 0:
					szText = localText.getText("TXT_KEY_COLOR_NEGATIVE", ()) + szText + localText.getText("TXT_KEY_COLOR_REVERT", ())
				elif iNetYield == 0:
					szText = ""
				
				screen.setTableInt(szState + "ListBackground", iYield - start + 2, i, "<font=1>" + szText + "<font/>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
	# total production page - end - Nightinggale

	def BuildAllTransportsRow(self, iRow):
		screen = CyGInterfaceScreen("DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR)

		if(self.CurrentState == self.TRADEROUTE_STATE):
			szState = self.StatePages[self.CurrentState][self.CurrentPage]
			player = gc.getPlayer(gc.getGame().getActivePlayer())

			screen.addPanel("AllTransportPanel", u"", u"", True, False, 0, 0, (390 * self.nTableWidth) / self.nNormalizedTableWidth, 20, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, iRow, -1)
			screen.attachControlToTableCell("AllTransportPanel", szState + "ListBackground", iRow, 4 )

			iSpace = self.iCityButtonSize
			if (len(self.Transports)):
				iSpace = (self.nTableWidth - 390) / len(self.Transports)
				if iSpace > self.iCityButtonSize:
					iSpace = self.iCityButtonSize

			SelectionState = False
			iPosition = 10
			SelectedTransportID = -1
			for iSelectionGroupIndex in range(len(self.Transports) -1, -1, -1):
				SelectionGroup = self.Transports[iSelectionGroupIndex]

				if (not SelectionGroup.isNone()):
					GroupHeadID = SelectionGroup.getHeadUnit().getID()
					szButtonName = "AllTransport" + str(SelectionGroup.getID())
					screen.addCheckBoxGFCAt("AllTransportPanel", szButtonName, SelectionGroup.getHeadUnit().getButton(), ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), iSelectionGroupIndex * iSpace, 0, self.iCityButtonSize, self.iCityButtonSize, WidgetTypes.WIDGET_GENERAL, 10001, SelectionGroup.getHeadUnit().getID(), ButtonStyles.BUTTON_STYLE_LABEL )
					screen.setLabelAt("SelectionGroupCount" + str(SelectionGroup.getID()), szButtonName, "<font=4>" + str(SelectionGroup.getNumUnits()) + "</font>", CvUtil.FONT_LEFT_JUSTIFY, 35, 35, -1.3, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

					iPosition += 20
					SelectedTransportID = self.selectedSelectionGroupHeadUnitID
					if (SelectedTransportID != -1):
						if (SelectedTransportID == SelectionGroup.getHeadUnit().getID()):
							screen.setState(szButtonName, True)
							SelectionState = True
							
	def RebuildArrays (self):
		self.NativeCities = []
		
		if 'stateWindow' in dir(self):
			for i in len(self.StateWindow):
				if self.StateWindow[i] != None:
					self.StateWindow[i].setDirty()
		
		#Get a list of the Players Cities
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		self.Cities = []
		(pLoopCity, iter) = player.firstCity(False)
		while(pLoopCity):
			self.Cities.append(pLoopCity)
			(pLoopCity, iter) = player.nextCity(iter, False)

		self.Routes = []
		for iRoute in range(player.getNumTradeRoutes()):
			self.Routes.append(player.getTradeRouteByIndex(iRoute))

		self.Transports = []
		SelectionGroup, Iterator = player.firstSelectionGroup(False)
		while (SelectionGroup != None):
			if (SelectionGroup.canAssignTradeRoute(-1, False)):
				self.Transports.append(SelectionGroup)
			SelectionGroup, Iterator = player.nextSelectionGroup(Iterator, False)

		self.RouteValidity = []
		for iTransport in range(len(self.Transports)):
			Transport = self.Transports[iTransport]
			RouteValidArray = []
			bReusePath = False
			for Route in self.Routes:
				RouteValidArray.append(Transport.canAssignTradeRoute(Route.getID(), bReusePath))
				bReusePath = True
			self.RouteValidity.append(RouteValidArray)
		
		
	def GetNativeCities(self):
		# R&R, Robert Surcouf,  Domestic Advisor Screen - Start
		if len(self.NativeCities) > 0:
			return self.NativeCities
		for iLoopPlayer in range(gc.getMAX_CIV_PLAYERS()):
			ePlayer = gc.getPlayer(iLoopPlayer)
			#if (player.isAlive() and player.isNative() and (gc.getTeam(player.getTeam()).isHasMet(activePlayer.getTeam()))):
			if (ePlayer.isAlive() and ePlayer.isNative()):
				(pLoopCity, iter) = ePlayer.firstCity(False)
				while(pLoopCity):
					self.NativeCities.append(pLoopCity)
					(pLoopCity, iter) = ePlayer.nextCity(iter, False)
		## R&R, Robert Surcouf,  Domestic Advisor Screen - End
		return self.NativeCities
		
	def RebuildTransportTable (self):
		if self.CurrentState != self.TRADEROUTE_STATE:
			return
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )

		szStateName = self.StatePages[self.TRADEROUTE_STATE][0]
		screen.setImageButton( szStateName + "Button", ArtFileMgr.getInterfaceArtInfo(self.StateButtons[self.TRADEROUTE_STATE]).getPath(), (self.iButtonSpacing * 5) + (self.iButtonSpacing / 2), self.Y_LOWER_ROW, self.iButtonSize, self.iButtonSize, WidgetTypes.WIDGET_GENERAL, 5, -1 )
		
		screen.addTableControlGFC( szStateName + "ListBackground", 19, (self.nScreenWidth - self.nTableWidth) / 2, 60, self.nTableWidth, self.nTableHeight, True, False, self.iCityButtonSize, self.iCityButtonSize, TableStyles.TABLE_STYLE_STANDARD )
		screen.enableSelect( szStateName + "ListBackground", True )
		screen.enableSort( szStateName + "ListBackground" )
		screen.setStyle( szStateName + "ListBackground", "Table_StandardCiv_Style")
		screen.hide( szStateName + "ListBackground")
		screen.setTableColumnHeader( szStateName + "ListBackground", 0, "", (30 * self.nTableWidth) / self.nNormalizedTableWidth )
		screen.setTableColumnHeader( szStateName + "ListBackground", 1, "<font=2>" + localText.getText("TXT_KEY_DOMESTIC_ADVISOR_NAME", ()) + "</font>", (221 * self.nTableWidth) / self.nNormalizedTableWidth )
		
		
	def RebuildRouteTable (self):
		if self.CurrentState != self.TRADEROUTE_STATE:
			return
	
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )

		szTableName = self.StatePages[self.TRADEROUTE_STATE][0] + "ListBackground"
		screen.setImageButton( szTableName + "Button", ArtFileMgr.getInterfaceArtInfo(self.StateButtons[self.TRADEROUTE_STATE]).getPath(), (self.iButtonSpacing * 5) + (self.iButtonSpacing / 2), self.Y_LOWER_ROW, self.iButtonSize, self.iButtonSize, WidgetTypes.WIDGET_GENERAL, 5, -1 )

		screen.addTableControlGFC( szTableName, 19, (self.nScreenWidth - self.nTableWidth) / 2, 60, self.nTableWidth, self.nTableHeight, True, False, self.iCityButtonSize, self.iCityButtonSize, TableStyles.TABLE_STYLE_STANDARD )
		screen.enableSelect( szTableName, True )
		screen.enableSort( szTableName )
		screen.setStyle( szTableName, "Table_StandardCiv_Style" )
		screen.hide( szTableName )

		screen.setTableColumnHeader( szTableName, 0, localText.getText("TXT_KEY_SOURCE", ()), (150 * self.nTableWidth) / self.nNormalizedTableWidth)
		screen.setTableColumnHeader( szTableName, 1, localText.getText("TXT_KEY_DESTINATION", ()), (150 * self.nTableWidth) / self.nNormalizedTableWidth)
		screen.setTableColumnHeader( szTableName, 2, localText.getText("TXT_KEY_YIELD", ()), (65 * self.nTableWidth) / self.nNormalizedTableWidth)
		screen.setTableColumnHeader( szTableName, 3, " +/-", (45 * self.nTableWidth) / self.nNormalizedTableWidth)
		screen.setTableColumnHeader( szTableName, 4, localText.getText("TXT_KEY_TRANSPORT_PANEL", ()), self.nTableWidth - 390)

		for iRoute in range(len(self.Routes)):
			screen.appendTableRow( szTableName )
			screen.setTableRowHeight(szTableName, iRoute, self.ROW_HIGHT)

		screen.appendTableRow( szTableName )
		screen.setTableRowHeight(szTableName, len(self.Routes), self.ROW_HIGHT)
	
	
	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		' Calls function mapped in DomesticAdvisorInputMap'
		
		if self.StateWindow[self.CurrentState] != None:
			returnVal = self.StateWindow[self.CurrentState].handleInput(inputClass)
			if returnVal != -1:
				return returnVal
		
		screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )
		if ( inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED ):
			## R&R, Robert Surcouf,  Domestic Advisor Screen START
			if self.CurrentState == self.GAME_FONT_STATE:
				gc.setGameFontDebug(inputClass.getData()+8483)
			elif self.CurrentState != self.TRADEROUTE_STATE and self.CurrentState != self.NATIVE_STATE:
			## R&R, Robert Surcouf,  Domestic Advisor Screen END
				if (inputClass.getMouseX() == 0):
					screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )
					screen.hideScreen()

					CyInterface().selectCity(gc.getPlayer(inputClass.getData1()).getCity(inputClass.getData2()), True);

					popupInfo = CyPopupInfo()
					popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
					popupInfo.setText(u"showDomesticAdvisor")
					popupInfo.addPopup(inputClass.getData1())
				else:
					self.updateAppropriateCitySelection()

		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if (inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL):
				iData = inputClass.getData1()
				if (iData >= 0 and iData < len(self.StatePages)):
					if(self.CurrentState != iData):
						if self.StateWindow[self.CurrentState] != None:
							self.StateWindow[self.CurrentState].hide()
						screen.hide(self.StatePages[self.CurrentState][self.CurrentPage] + "ListBackground")
						if self.CurrentState not in self.YieldPages or iData not in self.YieldPages:
							self.CurrentPage = 0
						self.CurrentState = iData
						gc.setDomesticAdvisorState(iData)
						if self.CurrentState != self.GAME_FONT_STATE:
							gc.setGameFontDebug(-1)
						self.drawContents()
				# auto-generated list creation - start - Nightinggale
				elif (iData == 100 or iData == 102):
					# iData == 1 was already taken. (100, 102) - 101 gives the -1/+1 needed
					if self.StateWindow[self.CurrentState] != None:
						if (iData == 100):
							self.StateWindow[self.CurrentState].prevPage()
						else:
							self.StateWindow[self.CurrentState].nextPage()
					else:
						new_page = self.CurrentPage + iData - 101
						
						if (new_page >= 0 and new_page < len(self.StatePages[self.CurrentState])):
							screen.hide(self.StatePages[self.CurrentState][self.CurrentPage] + "ListBackground")
							self.CurrentPage = new_page
							self.drawContents()
				# auto-generated list creation - end - Nightinggale
				elif (iData == 10001):
					if (self.selectedSelectionGroupHeadUnitID == inputClass.getData2()):
						self.selectedSelectionGroupHeadUnitID = -1
						self.drawContents()
					else:
						self.selectedSelectionGroupHeadUnitID = inputClass.getData2()
						self.drawContents()
		return 0

	def updateAppropriateCitySelection(self):
		nCities = gc.getPlayer(gc.getGame().getActivePlayer()).getNumCities()
		screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )
		screen.updateAppropriateCitySelection( self.StatePages[self.CurrentState][self.CurrentPage] + "ListBackground", nCities, 1 )
		self.listSelectedCities = []
		for iCity in range(nCities):
			if screen.isRowSelected(self.StatePages[self.CurrentState][self.CurrentPage] + "ListBackground", iCity):
				self.listSelectedCities.append(screen.getTableText(self.StatePages[self.CurrentState][self.CurrentPage] + "ListBackground", 2, iCity))

	def update(self, fDelta):
		if (CyInterface().isDirty(InterfaceDirtyBits.Domestic_Advisor_DIRTY_BIT)):
			CyInterface().setDirty(InterfaceDirtyBits.Domestic_Advisor_DIRTY_BIT, False)
			player = gc.getPlayer(gc.getGame().getActivePlayer())

			self.RebuildArrays()
			
			for iCity in range(len(self.Cities)):
				self.updateCityTable(self.Cities[iCity], iCity)

			self.updateTotalProduction()
			self.RebuildRouteTable()
			self.RebuildTransportTable()

			self.drawContents()
			
	def getWidgetHelp(self, argsList):
		if self.StateWindow[self.CurrentState] != None:
			returnVal = self.StateWindow[self.CurrentState].getWidgetHelp(argsList)
			if returnVal != None:
				return returnVal
	
		iScreen, eWidgetType, iData1, iData2, bOption = argsList

		if eWidgetType == WidgetTypes.WIDGET_GENERAL:
			if iData1 == self.GENERAL_STATE:
				return localText.getText("TXT_KEY_DOMESTIC_ADVISOR_STATE_GENERAL", ())
			elif iData1 == self.DOMESTIC_DEMAND_STATE:
				return localText.getText("TXT_KEY_DOMESTIC_ADVISOR_STATE_DOMESTIC_MARKET", ())
			elif iData1 == self.HAPPINESS_STATE:
				return localText.getText("TXT_KEY_DOMESTIC_ADVISOR_STATE_HAPPINESS", ())
			elif iData1 == self.PRODUCTION_STATE:
				return localText.getText("TXT_KEY_CONCEPT_PRODUCTION", ())
			elif iData1 == self.BUILDING_STATE:
				return localText.getText("TXT_KEY_BUILDINGS", ())
			elif iData1 == self.WAREHOUSE_STATE:
				# R&R, Robert Surcouf, French game text fix START
				#return localText.getText("TXT_KEY_BUILDING_WAREHOUSE", ())
				return localText.getText("TXT_KEY_DOMESTIC_ADVISOR_WAREHOUSE", ())
				# R&R, Robert Surcouf, French game text fix END
			elif iData1 == self.CITIZEN_STATE:
				return localText.getText("TXT_KEY_DOMESTIC_ADVISOR_STATE_CITIZEN", ())
			elif iData1 == self.IMPORTEXPORT_STATE:
				return localText.getText("TXT_KEY_CONCEPT_TRADE_ROUTE", ())
			# total production page - start - Nightinggale
			elif iData1 == self.TOTAL_PRODUCTION_STATE:
				return localText.getText("TXT_KEY_CONCEPT_TOTAL_PRODUCTION", ())
			# total production page - end - Nightinggale
			elif iData1 == self.TRADEROUTE_STATE:
				return localText.getText("TXT_KEY_DOMESTIC_ADVISOR_STATE_TRADEROUTE", ())
			elif iData1 == self.TEACHER_STATE:
				return localText.getText("TXT_KEY_DOMESTIC_ADVISOR_STATE_TEACHER", ())
			elif iData1 == self.NATIVE_STATE:
				return localText.getText("TXT_KEY_DOMESTIC_ADVISOR_STATE_NATIVE", ())
			elif iData1 == 10001:
				unit = gc.getActivePlayer().getUnit(iData2)
				if not unit.isNone():
					return CyGameTextMgr().getSpecificUnitHelp(unit, True, False)
			elif iData1 == self.GAME_FONT_STATE and iData1 != -1:
				return "DEBUG: GameFont"
			elif iData1 == self.TERRAIN_STATE and iData1 != -1:
				return "DEBUG: terrain count"
		if eWidgetType == WidgetTypes.WIDGET_MISSION_CHAR:
			return gc.getPlayer(iData1).getCivilizationAdjective(0)
			
	## R&R, Robert Surcouf,  Domestic Advisor Screen - Start
	def getGeneralStateColumnSize(self, iNum):
		return (self.nTableWidth - self.CITY_NAME_COLUMN_WIDTH) / iNum 
	## R&R, Robert Surcouf,  Domestic Advisor Screen - End
	
	# two small functions to get the yields on the current page - Nightinggale
	def YieldStart(self):
		return self.MAX_YIELDS_IN_A_PAGE * self.CurrentPage
		
	def YieldEnd(self):
		#return min((self.MAX_YIELDS_IN_A_PAGE * (self.CurrentPage + 1)) - 1, YieldTypes.YIELD_LUXURY_GOODS + 1)
		return min((self.MAX_YIELDS_IN_A_PAGE * (self.CurrentPage + 1)), YieldTypes.YIELD_LUXURY_GOODS + 1)
	
	# auto-generated list creation - start - Nightinggale
	def addButton(self, state_type, state_button, state_class = None):
		index = len(self.StatePages)
		self.StateButtons.append(state_button)
		self.StatePages.append([state_type])
		self.StateWindow.append(state_class)
		return index
		
	def createSubpage(self, iState, iPage):
		length = len(self.StatePages[iState])
		if (length <= iPage):
			self.StatePages[iState].append(self.StatePages[iState][0] + "Page" + str(length))
			self.initPage(iState, length)
			self.createSubpage(iState, iPage)
			
	def initPage(self, iState, iPage):
		if iState != self.TRADEROUTE_STATE and iState != self.NATIVE_STATE:
			screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )
			szStateName = self.StatePages[iState][iPage] + "ListBackground"
			## R&R, Robert Surcouf,  Domestic Advisor Screen START
			#screen.addTableControlGFC(szStateName, 22, (self.nScreenWidth - self.nTableWidth) / 2, 60, self.nTableWidth, self.nTableHeight, True, False, self.iCityButtonSize, self.iCityButtonSize, TableStyles.TABLE_STYLE_STANDARD )
			screen.addTableControlGFC(szStateName, self.MAX_YIELDS_IN_A_PAGE + 4, (self.nScreenWidth - self.nTableWidth) / 2, 60, self.nTableWidth, self.nTableHeight, True, False, self.iCityButtonSize, self.iCityButtonSize, TableStyles.TABLE_STYLE_STANDARD )
			screen.setStyle(szStateName, "Table_StandardCiv_Style")
			screen.hide(szStateName)
			screen.setTableColumnHeader(szStateName, 0, "", 45 )
			#screen.setTableColumnHeader(szStateName, 0, "", 56 )
			## R&R, Robert Surcouf,  Domestic Advisor Screen END
			screen.setTableColumnHeader(szStateName, 1, "<font=2>" + localText.getText("TXT_KEY_DOMESTIC_ADVISOR_NAME", ()).upper() + "</font>", self.CITY_NAME_COLUMN_WIDTH - 56 )

			# total production page - start - Nightinggale
			num_cities = 2
			
			if iState != self.TOTAL_PRODUCTION_STATE:
				num_cities = len(self.Cities)
				screen.enableSelect(szStateName, True)
				screen.enableSort(szStateName)
			
			for iCity in range(num_cities):
			# for iCity in range(len(self.Cities)):
			# total production page - end - Nightinggale
				screen.appendTableRow(szStateName)
				screen.setTableRowHeight(szStateName, iCity, self.ROW_HIGHT)
	# auto-generated list creation - end - Nightinggale 
	
	def drawGameFont(self):
		szStateName = self.StatePages[self.GAME_FONT_STATE][0] + "ListBackground"
		
		if self.GameFontSet == False:
			screen = CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )
			screen.addTableControlGFC(szStateName, 5, (self.nScreenWidth - self.nTableWidth) / 2, 60, self.nTableWidth, self.nTableHeight, True, False, self.iCityButtonSize, self.iCityButtonSize, TableStyles.TABLE_STYLE_STANDARD )
			screen.setTableColumnHeader( szStateName, 0, "<font=2>ID</font>", 56)
			screen.setTableColumnHeader( szStateName, 1, "<font=2>Small</font>", 56)
			screen.setTableColumnHeader( szStateName, 2, "<font=2>Big</font>", 56)
			screen.setTableColumnHeader( szStateName, 3, "<font=2>Button</font>", 56)
			screen.setTableColumnHeader( szStateName, 4, "<font=2>Type</font>", 500)
			
			# load the max value from UserSettings
			iMax = gc.getUserSettings().getDebugMaxGameFont() - 8483 + 1
			if iMax <= 0:
				# UserSettings didn't request a specific max. Use the lenght assumed by the game
				iMax = self.maxNumGameFontID() - 8483 + 5
			for iLine in range(iMax):
				iID = iLine + 8483
				screen.appendTableRow(szStateName)
				screen.setTableInt(szStateName, 0, iLine , "<font=2>" + unicode(iID) + "<font/>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				screen.setTableInt(szStateName, 1, iLine , "<font=2>" + (u" %c" %  (iID)) + "<font/>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				screen.setTableInt(szStateName, 2, iLine , "<font=4>" + (u" %c" %  (iID)) + "<font/>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )

				infoPointer = self.getInfoPointerFromGameFont(iID)
				
				if infoPointer != None:
					if isinstance(infoPointer, str):
						screen.setTableInt(szStateName, 4, iLine , "<font=2>" + infoPointer + "<font/>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
					else:
						szButton = infoPointer.getButton()
						if szButton != "" and szButton != "Art/Interface/Buttons/Buildings/BombShelters.dds":
							screen.setTableRowHeight(szStateName, iLine, self.ROW_HIGHT)
							screen.setTableText(szStateName, 3, iLine, "", szButton, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY);
						screen.setTableInt(szStateName, 4, iLine , "<font=2>" + infoPointer.getType() + "<font/>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
					
				
	def getInfoPointerFromGameFont(self, iIndex):
		for iYieldIndex in range(gc.getNumYieldInfos()):
			infoPointer = gc.getYieldInfo(iYieldIndex)
			if infoPointer.getChar() == iIndex:
				return infoPointer
		
		for iBuildingIndex in range(gc.getNumSpecialBuildingInfos()):
			infoPointer = gc.getSpecialBuildingInfo(iBuildingIndex)
			if infoPointer.getChar() == iIndex:
				return infoPointer
		
		for iBonusIndex in range(gc.getNumBonusInfos()):
			infoPointer = gc.getBonusInfo(iBonusIndex)
			if infoPointer.getChar() == iIndex:
				return infoPointer
			
		for iFatherIndex in range(gc.getNumFatherPointInfos()):
			infoPointer = gc.getFatherPointInfo(iFatherIndex)
			if infoPointer.getChar() == iIndex:
				return infoPointer

		for iCiv in range(gc.getNumCivilizationInfos()):
			infoPointer = gc.getCivilizationInfo(iCiv)
			if infoPointer.getMissionaryChar() == iIndex:
				return infoPointer
			
		for iSymbol in range(FontSymbols.MAX_NUM_SYMBOLS):
			if iIndex == gc.getSymbolID(iSymbol):
				return gc.getSymbolName(iSymbol)
		
		return None
	
	def maxNumGameFontID(self):
		iMax = FontSymbols.MAX_NUM_SYMBOLS + CyGame().getSymbolID(FontSymbols.HAPPY_CHAR) + 5
		
		for iYieldIndex in range(gc.getNumYieldInfos()):
			infoPointer = gc.getYieldInfo(iYieldIndex)
			if infoPointer.getChar() > iMax:
				iMax = infoPointer.getChar()
		
		for iBuildingIndex in range(gc.getNumSpecialBuildingInfos()):
			infoPointer = gc.getSpecialBuildingInfo(iBuildingIndex)
			if infoPointer.getChar() > iMax:
				iMax = infoPointer.getChar()
		
		for iBonusIndex in range(gc.getNumBonusInfos()):
			infoPointer = gc.getBonusInfo(iBonusIndex)
			if infoPointer.getChar() > iMax:
				iMax = infoPointer.getChar()
			
		for iFatherIndex in range(gc.getNumFatherPointInfos()):
			infoPointer = gc.getFatherPointInfo(iFatherIndex)
			if infoPointer.getChar() > iMax:
				iMax = infoPointer.getChar()

		for iCiv in range(gc.getNumCivilizationInfos()):
			infoPointer = gc.getCivilizationInfo(iCiv)
			if infoPointer.getMissionaryChar() > iMax:
				iMax = infoPointer.getMissionaryChar()
		return iMax
		
	
	def createTable(self, szName):
		self.getScreen().addTableControlGFC( szName, 7, (self.nScreenWidth - self.nTableWidth) / 2, 60, self.nTableWidth, self.nTableHeight, True, False, self.iCityButtonSize, self.iCityButtonSize, TableStyles.TABLE_STYLE_STANDARD )
		self.getScreen().setStyle( szName, "Table_StandardCiv_Style" )

	def getPlayer(self):
		return gc.getPlayer(gc.getGame().getActivePlayer())
	
	def getScreen(self):
		return CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )
