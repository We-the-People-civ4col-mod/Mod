## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005

# Thanks to "Ulf 'ulfn' Norell" from Apolyton for his additions relating to the graph section of this screen

from CvPythonExtensions import *
import CvScreenEnums
import CvUtil
import ScreenInput

import string
import time

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

def iff(b, x, y):
	if b:
		return x
	else:
		return y

class CvInfoScreen:
	"Info Screen! Contains the Demographics, Top Cities and Statistics Screens"

	def __init__(self, screenId):

		self.screenId = screenId
		self.DEMO_SCREEN_NAME = "DemographicsScreen"
		self.WIDGET_ID = "DemoScreenWidget"
		self.LINE_ID   = "DemoLine"

	def setGlobals(self):
	
		self.Z_BACKGROUND = -6.1
		self.Z_CONTROLS = self.Z_BACKGROUND - 0.2
		self.DZ = -0.2
		self.Z_HELP_AREA = self.Z_CONTROLS - 1

		self.X_SCREEN = 0
		self.Y_SCREEN = 0
		self.X_TITLE = 512
		self.Y_TITLE = 8
		self.BORDER_WIDTH = 4
		self.W_HELP_AREA = 200

		self.X_EXIT = 994
		self.Y_EXIT = 730

		self.X_GRAPH_TAB	= 30
		self.X_DEMOGRAPHICS_TAB = 165
		self.X_TOP_CITIES_TAB	= 425
		self.X_STATS_TAB	= 663
		self.Y_TABS		= 730
		self.W_BUTTON		= 200
		self.H_BUTTON		= 30

		self.graphEnd		= CyGame().getGameTurn() - 1
		self.graphZoom		= self.graphEnd - CyGame().getStartTurn()
		self.nWidgetCount   = 0
		self.nLineCount		= 0

		self.iGraphID		=	0
		self.iDemographicsID	=	1
		self.iTopCitiesID	=	2
		self.iStatsID		=	3

		self.iActiveTab = self.iGraphID

		self.TOTAL_SCORE	= 0
		self.ECONOMY_SCORE	= 1
		self.INDUSTRY_SCORE	= 2
		self.AGRICULTURE_SCORE	= 3
		self.POWER_SCORE	= 4
		self.CULTURE_SCORE	= 5
		self.NUM_SCORES		= 6
		self.RANGE_SCORES	= range(self.NUM_SCORES)

		self.scoreCache	= []
		for t in self.RANGE_SCORES:
			self.scoreCache.append(None)

		self.GRAPH_H_LINE = "GraphHLine"
		self.GRAPH_V_LINE = "GraphVLine"

		self.xSelPt = 0
		self.ySelPt = 0

		self.graphLeftButtonID = ""
		self.graphRightButtonID = ""


################################################## GRAPH ###################################################

		self.X_MARGIN	= 15
		self.Y_MARGIN	= 70
		self.H_DROPDOWN	= 35

		self.X_DEMO_DROPDOWN	= self.X_MARGIN
		self.Y_DEMO_DROPDOWN	= self.Y_MARGIN
		self.W_DEMO_DROPDOWN	= 150

		self.X_ZOOM_DROPDOWN	= self.X_DEMO_DROPDOWN
		self.Y_ZOOM_DROPDOWN	= self.Y_DEMO_DROPDOWN + self.H_DROPDOWN
		self.W_ZOOM_DROPDOWN	= self.W_DEMO_DROPDOWN

		self.X_LEGEND		= self.X_DEMO_DROPDOWN
		self.Y_LEGEND		= self.Y_ZOOM_DROPDOWN + self.H_DROPDOWN + 3
		self.W_LEGEND		= self.W_DEMO_DROPDOWN + 20

		self.X_GRAPH = self.X_DEMO_DROPDOWN + self.W_DEMO_DROPDOWN + 30
		self.Y_GRAPH = self.Y_MARGIN
		self.W_GRAPH = self.W_SCREEN - self.X_GRAPH - self.X_MARGIN
		self.H_GRAPH = self.H_SCREEN - 170

		self.W_LEFT_BUTTON  = 20
		self.H_LEFT_BUTTON  = 20
		self.X_LEFT_BUTTON  = self.X_GRAPH
		self.Y_LEFT_BUTTON  = self.Y_GRAPH + self.H_GRAPH

		self.W_RIGHT_BUTTON  = self.W_LEFT_BUTTON
		self.H_RIGHT_BUTTON  = self.H_LEFT_BUTTON
		self.X_RIGHT_BUTTON  = self.X_GRAPH + self.W_GRAPH - self.W_RIGHT_BUTTON
		self.Y_RIGHT_BUTTON  = self.Y_LEFT_BUTTON

		self.X_LEFT_LABEL   = self.X_LEFT_BUTTON + self.W_LEFT_BUTTON + 10
		self.X_RIGHT_LABEL  = self.X_RIGHT_BUTTON - 10
		self.Y_LABEL		= self.Y_GRAPH + self.H_GRAPH + 3

		self.H_LEGEND = 0
		self.Y_LEGEND = 0
		self.X_LEGEND_MARGIN= 10
		self.Y_LEGEND_MARGIN= 5
		self.X_LEGEND_LINE = self.X_LEGEND_MARGIN
		self.Y_LEGEND_LINE = self.Y_LEGEND_MARGIN + 9  # to center it relative to the text
		self.W_LEGEND_LINE = 30
		self.X_LEGEND_TEXT = self.X_LEGEND_LINE + self.W_LEGEND_LINE - 3
		self.Y_LEGEND_TEXT = self.Y_LEGEND_MARGIN
		self.H_LEGEND_TEXT = 16
		self.LEGEND_PANEL_ID = ""
		self.LEGEND_CANVAS_ID = ""

		self.TOTAL_SCORE = 0
		self.ECONOMY_SCORE = 1
		self.INDUSTRY_SCORE = 2
		self.AGRICULTURE_SCORE = 3
		self.POWER_SCORE = 4
		self.CULTURE_SCORE = 5

############################################### DEMOGRAPHICS ###############################################

		self.W_CHART = 934
		self.H_CHART = 600
		self.X_CHART = 45
		self.Y_CHART = (self.H_SCREEN / 2) - (self.H_CHART / 2)

		self.BUTTON_SIZE = 20

		self.W_TEXT = 140
		self.H_TEXT = 15
		self.X_TEXT_BUFFER = 0
		self.Y_TEXT_BUFFER = 43

		self.X_COL_1 = 535
		self.X_COL_2 = self.X_COL_1 + self.W_TEXT + self.X_TEXT_BUFFER
		self.X_COL_3 = self.X_COL_2 + self.W_TEXT + self.X_TEXT_BUFFER
		self.X_COL_4 = self.X_COL_3 + self.W_TEXT + self.X_TEXT_BUFFER

		self.Y_ROW_1 = 100
		self.Y_ROW_2 = self.Y_ROW_1 + self.H_TEXT + self.Y_TEXT_BUFFER
		self.Y_ROW_3 = self.Y_ROW_2 + self.H_TEXT + self.Y_TEXT_BUFFER
		self.Y_ROW_4 = self.Y_ROW_3 + self.H_TEXT + self.Y_TEXT_BUFFER
		self.Y_ROW_5 = self.Y_ROW_4 + self.H_TEXT + self.Y_TEXT_BUFFER
		self.Y_ROW_6 = self.Y_ROW_5 + self.H_TEXT + self.Y_TEXT_BUFFER
		self.Y_ROW_7 = self.Y_ROW_6 + self.H_TEXT + self.Y_TEXT_BUFFER
		self.Y_ROW_8 = self.Y_ROW_7 + self.H_TEXT + self.Y_TEXT_BUFFER
		self.Y_ROW_9 = self.Y_ROW_8 + self.H_TEXT + self.Y_TEXT_BUFFER
		self.Y_ROW_10 = self.Y_ROW_9 + self.H_TEXT + self.Y_TEXT_BUFFER

		self.bAbleToShowAllPlayers = False
		self.iShowingPlayer = -1
		self.aiDropdownPlayerIDs = []

############################################### TOP CITIES ###############################################

		self.X_LEFT_PANE = 45
		self.Y_LEFT_PANE = 70
		self.W_LEFT_PANE = 470
		self.H_LEFT_PANE = self.H_SCREEN - 160

		# Text

		self.W_TC_TEXT = 280
		self.H_TC_TEXT = 15
		self.X_TC_TEXT_BUFFER = 0
		self.Y_TC_TEXT_BUFFER = 43

		# Animated City thingies

		self.X_CITY_ANIMATION = self.X_LEFT_PANE + 20
		self.Z_CITY_ANIMATION = self.Z_BACKGROUND - 0.5
		self.W_CITY_ANIMATION = 150
		self.H_CITY_ANIMATION = self.H_LEFT_PANE / 6
		self.Y_CITY_ANIMATION_BUFFER = self.H_CITY_ANIMATION / 2

		self.X_ROTATION_CITY_ANIMATION = -20
		self.Z_ROTATION_CITY_ANIMATION = 30
		self.SCALE_ANIMATION = 0.5

		# Placement of Cities

		self.X_COL_1_CITIES = self.X_LEFT_PANE + 20
		self.Y_CITIES_BUFFER = self.H_LEFT_PANE / 5 #118

		self.Y_ROWS_CITIES = []
		self.Y_ROWS_CITIES.append(self.Y_LEFT_PANE + 20)
		for i in range(1,5):
			self.Y_ROWS_CITIES.append(self.Y_ROWS_CITIES[i-1] + self.Y_CITIES_BUFFER)

		self.W_CITIES_DESC = 275
		self.H_CITIES_DESC = 60

		self.X_COL_1_CITIES_DESC = self.X_LEFT_PANE + self.W_CITY_ANIMATION + 30
		self.Y_CITIES_DESC_BUFFER = -4

		self.H_LEFT_PANE += (self.Y_CITIES_BUFFER - self.H_CITY_ANIMATION)
############################################### STATISTICS ###############################################

		# STATISTICS TAB

		# Top Panel

		self.X_STATS_TOP_PANEL = 45
		self.Y_STATS_TOP_PANEL = 75
		self.W_STATS_TOP_PANEL = 935
		self.H_STATS_TOP_PANEL = 180

		# Leader

		self.X_LEADER_ICON = self.W_SCREEN / 5
		self.Y_LEADER_ICON = 95
		self.H_LEADER_ICON = 140
		self.W_LEADER_ICON = 110

		# Top Chart

		self.X_STATS_TOP_CHART = 400
		self.Y_STATS_TOP_CHART = 130
		self.W_STATS_TOP_CHART = 380
		self.H_STATS_TOP_CHART = 102

		self.STATS_TOP_CHART_W_COL_0 = 304
		self.STATS_TOP_CHART_W_COL_1 = 76

		self.iNumTopChartCols = 2
		self.iNumTopChartRows = 4

		self.X_LEADER_NAME = self.X_STATS_TOP_CHART
		self.Y_LEADER_NAME = self.Y_STATS_TOP_CHART - 40

		# Bottom Chart

		self.Y_STATS_BOTTOM_CHART = 270
		self.W_STATS_BOTTOM_CHART_UNITS = 545
		self.W_STATS_BOTTOM_CHART_BUILDINGS = 390
		self.H_STATS_BOTTOM_CHART = self.H_SCREEN - 370

		self.reset()

	def initText(self):

		###### TEXT ######
		self.SCREEN_TITLE = u"<font=4b>" + localText.getText("TXT_KEY_INFO_SCREEN", ()).upper() + u"</font>"
		self.SCREEN_GRAPH_TITLE = u"<font=4b>" + localText.getText("TXT_KEY_INFO_GRAPH", ()).upper() + u"</font>"
		self.SCREEN_DEMOGRAPHICS_TITLE = u"<font=4b>" + localText.getText("TXT_KEY_DEMO_SCREEN_TITLE", ()).upper() + u"</font>"
		self.SCREEN_TOP_CITIES_TITLE = u"<font=4b>" + localText.getText("TXT_KEY_WONDERS_SCREEN_TOP_CITIES_TEXT", ()).upper() + u"</font>"
		self.SCREEN_STATS_TITLE = u"<font=4b>" + localText.getText("TXT_KEY_INFO_SCREEN_STATISTICS_TITLE", ()).upper() + u"</font>"

		self.EXIT_TEXT = u"<font=4>" + localText.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + u"</font>"

		self.TEXT_GRAPH = localText.getText("TXT_KEY_INFO_GRAPH", ()).lower()
		self.TEXT_GRAPH = self.TEXT_GRAPH[0].upper() + self.TEXT_GRAPH[1:]
		self.TEXT_DEMOGRAPHICS = localText.getText("TXT_KEY_DEMO_SCREEN_TITLE", ())
		self.TEXT_DEMOGRAPHICS_SMALL = localText.getText("TXT_KEY_DEMO_SCREEN_TITLE", ())
		self.TEXT_TOP_CITIES = localText.getText("TXT_KEY_WONDERS_SCREEN_TOP_CITIES_TEXT", ())
		self.TEXT_STATS = localText.getText("TXT_KEY_INFO_SCREEN_STATISTICS_TITLE", ())

		self.TEXT_SHOW_ALL_PLAYERS =  localText.getText("TXT_KEY_SHOW_ALL_PLAYERS", ())
		self.TEXT_SHOW_ALL_PLAYERS_GRAY = localText.getColorText("TXT_KEY_SHOW_ALL_PLAYERS", (), gc.getInfoTypeForString("COLOR_PLAYER_GRAY")).upper()

		self.TEXT_ENTIRE_HISTORY = localText.getText("TXT_KEY_INFO_ENTIRE_HISTORY", ())

		self.TEXT_SCORE = localText.getText("TXT_KEY_GAME_SCORE", ())
		self.TEXT_POWER = localText.getText("TXT_KEY_POWER", ())
		self.TEXT_CULTURE = gc.getYieldInfo(gc.getDefineINT("CULTURE_YIELD")).getDescription()

		self.TEXT_VALUE = localText.getText("TXT_KEY_DEMO_SCREEN_VALUE_TEXT_COLOURED", ())
		self.TEXT_RANK = localText.getText("TXT_KEY_DEMO_SCREEN_RANK_TEXT", ())
		self.TEXT_AVERAGE = localText.getText("TXT_KEY_DEMOGRAPHICS_SCREEN_RIVAL_AVERAGE", ())
		self.TEXT_BEST = localText.getText("TXT_KEY_INFO_RIVAL_BEST", ())
		self.TEXT_WORST = localText.getText("TXT_KEY_INFO_RIVAL_WORST", ())

		self.TEXT_ECONOMY = localText.getText("TXT_KEY_DEMO_SCREEN_ECONOMY_TEXT", ())
		self.TEXT_INDUSTRY = localText.getText("TXT_KEY_DEMO_SCREEN_INDUSTRY_TEXT", ())
		self.TEXT_AGRICULTURE = localText.getText("TXT_KEY_DEMO_SCREEN_AGRICULTURE_TEXT", ())
		self.TEXT_MILITARY = localText.getText("TXT_KEY_DEMO_SCREEN_MILITARY_TEXT", ())
		self.TEXT_LAND_AREA = localText.getText("TXT_KEY_DEMO_SCREEN_LAND_AREA_TEXT", ())
		self.TEXT_POPULATION = localText.getText("TXT_KEY_DEMO_SCREEN_POPULATION_TEXT", ())

		self.TEXT_ECONOMY_MEASURE = (u"  %c" % CyGame().getSymbolID(FontSymbols.BULLET_CHAR)) + localText.getText("TXT_KEY_DEMO_SCREEN_ECONOMY_MEASURE", ())
		self.TEXT_INDUSTRY_MEASURE = (u"  %c" % CyGame().getSymbolID(FontSymbols.BULLET_CHAR)) + localText.getText("TXT_KEY_DEMO_SCREEN_INDUSTRY_MEASURE", ())
		self.TEXT_AGRICULTURE_MEASURE = (u"  %c" % CyGame().getSymbolID(FontSymbols.BULLET_CHAR)) + localText.getText("TXT_KEY_DEMO_SCREEN_AGRICULTURE_MEASURE", ())
		self.TEXT_MILITARY_MEASURE = ""
		self.TEXT_LAND_AREA_MEASURE = (u"  %c" % CyGame().getSymbolID(FontSymbols.BULLET_CHAR)) + localText.getText("TXT_KEY_DEMO_SCREEN_LAND_AREA_MEASURE", ())
		self.TEXT_POPULATION_MEASURE = ""

		self.TEXT_TIME_PLAYED = localText.getText("TXT_KEY_INFO_SCREEN_TIME_PLAYED", ())
		self.TEXT_CITIES_BUILT = localText.getText("TXT_KEY_INFO_SCREEN_CITIES_BUILT", ())
		self.TEXT_CITIES_RAZED = localText.getText("TXT_KEY_INFO_SCREEN_CITIES_RAZED", ())

		self.TEXT_CURRENT = localText.getText("TXT_KEY_CURRENT", ())
		self.TEXT_UNITS = localText.getText("TXT_KEY_UNITS", ())
		self.TEXT_BUILDINGS = localText.getText("TXT_KEY_BUILDINGS", ())
		self.TEXT_KILLED = localText.getText("TXT_KEY_INFO_SCREEN_KILLED", ())
		self.TEXT_LOST = localText.getText("TXT_KEY_INFO_SCREEN_LOST", ())
		self.TEXT_BUILT = localText.getText("TXT_KEY_INFO_SCREEN_BUILT", ())

	def reset(self):

		# City Members

		self.szCityNames = ["",
					"",
					"",
					"",
					""]

		self.iCitySizes = [-1,
					-1,
					-1,
					-1,
					-1]

		self.szCityDescs = ["",
					"",
					"",
					"",
					""]

		self.aaCitiesXY = [[-1, -1],
					[-1, -1],
					[-1, -1],
					[-1, -1],
					[-1, -1]]

		self.iCityValues =   [  0,
					0,
					0,
					0,
					0]

		self.pCityPointers = [  0,
					0,
					0,
					0,
					0]

#		self.bShowAllPlayers = False
		self.graphEnd = CyGame().getGameTurn() - 1
		self.graphZoom = self.graphEnd - CyGame().getStartTurn()
		self.iShowingPlayer = -1

		for t in self.RANGE_SCORES:
			self.scoreCache[t] = None

	def getScreen(self):
		return CyGInterfaceScreen(self.DEMO_SCREEN_NAME, self.screenId)

	def hideScreen(self):
		screen = self.getScreen()
		screen.hideScreen()

	def getLastTurn(self):
		return (gc.getGame().getReplayMessageTurn(gc.getGame().getNumReplayMessages()-1))

	# Screen construction function
	def showScreen(self, iTurn, iTabID, iEndGame):


		self.iStartTurn = 0
		for iI in range(gc.getGameSpeedInfo(gc.getGame().getGameSpeedType()).getNumTurnIncrements()):
			self.iStartTurn += gc.getGameSpeedInfo(gc.getGame().getGameSpeedType()).getGameTurnInfo(iI).iNumGameTurnsPerIncrement
		self.iStartTurn *= gc.getEraInfo(gc.getGame().getStartEra()).getStartPercent()
		self.iStartTurn /= 100

		self.iTurn = 0

		if (iTurn > self.getLastTurn()):
			return

		# Create a new screen
		screen = self.getScreen()
		if screen.isActive():
			return
		self.W_SCREEN = screen.getXResolution()
		self.H_SCREEN = screen.getYResolution()

		self.setGlobals()
		self.initText()
		
		screen.setRenderInterfaceOnly(True)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		self.reset()

		self.deleteAllWidgets()

		# Set the background widget and exit button
		screen.addDDSGFC("DemographicsScreenBackground", ArtFileMgr.getInterfaceArtInfo("MAINMENU_SLIDESHOW_LOAD").getPath(), 0, 0, self.W_SCREEN, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDDSGFC("TopPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TITLE").getPath(), 0, 0, self.W_SCREEN, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.showWindowBackground( False )
		screen.setDimensions(0, 0, self.W_SCREEN, self.H_SCREEN)

		# Header...
		self.szHeaderWidget = self.getNextWidgetName()
		self.SCREEN_TITLE = localText.changeTextColor(self.SCREEN_TITLE, gc.getInfoTypeForString("COLOR_FONT_CREAM"))
		screen.setText(self.szHeaderWidget, "Background", self.SCREEN_TITLE, CvUtil.FONT_CENTER_JUSTIFY, self.W_SCREEN / 2, 4, self.Z_CONTROLS, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# Help area for tooltips
		screen.setHelpTextArea(self.W_HELP_AREA, FontTypes.SMALL_FONT, self.X_SCREEN, self.Y_SCREEN, self.Z_HELP_AREA, 1, ArtFileMgr.getInterfaceArtInfo("POPUPS_BACKGROUND_TRANSPARENT").getPath(), True, True, CvUtil.FONT_LEFT_JUSTIFY, 0 )

		self.DEBUG_DROPDOWN_ID = ""

		if (CyGame().isDebugMode()):
			self.DEBUG_DROPDOWN_ID = "InfoScreenDropdownWidget"
			self.szDropdownName = self.DEBUG_DROPDOWN_ID
			screen.addDropDownBoxGFC(self.szDropdownName, 22, 12, 300, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			for j in range(gc.getMAX_PLAYERS()):
				if (gc.getPlayer(j).isAlive()):
					screen.addPullDownString(self.szDropdownName, gc.getPlayer(j).getName(), j, j, False )

		self.iActivePlayer = CyGame().getActivePlayer()
		self.pActivePlayer = gc.getPlayer(self.iActivePlayer)
		self.iActiveTeam = self.pActivePlayer.getTeam()
		self.pActiveTeam = gc.getTeam(self.iActiveTeam)

		# Determine who this active player knows
		self.aiPlayersMet = []
		self.aiPlayerToggle = []
		self.iNumPlayersMet = 0
		for iLoopPlayer in range(gc.getMAX_CIV_PLAYERS()):
			pLoopPlayer = gc.getPlayer(iLoopPlayer)
			iLoopPlayerTeam = pLoopPlayer.getTeam()
			self.aiPlayerToggle.append(True)
			if (gc.getTeam(iLoopPlayerTeam).isEverAlive()):
				# Erik: Exclude all other players other than Europeans from the graph
				if (self.pActiveTeam.isHasMet(iLoopPlayerTeam) and not gc.getPlayer(iLoopPlayer).isEurope() and not gc.getPlayer(iLoopPlayer).isNative() and not gc.getGame().isChurchPlayer(iLoopPlayer) and not gc.getGame().isBarbarianPlayer(iLoopPlayer)):
					self.aiPlayersMet.append(iLoopPlayer)
					self.iNumPlayersMet += 1

		# "Save" current widgets so they won't be deleted later when changing tabs
		self.iNumPermanentWidgets = self.nWidgetCount

		# Reset variables
		self.graphEnd = CyGame().getGameTurn() - 1
		self.graphZoom = self.graphEnd - CyGame().getStartTurn()

		self.iActiveTab = iTabID
		if (self.iNumPlayersMet > 1):
			self.iShowingPlayer = 666
		else:
			self.iShowingPlayer = self.iActivePlayer

		self.placeTabs()
		self.redrawContents()

		return

	def placeTabs (self):
		screen = self.getScreen()

		Tabs = [self.TEXT_GRAPH, self.TEXT_DEMOGRAPHICS, self.TEXT_TOP_CITIES, self.TEXT_STATS]
		ExitTabWidth = 120
		NumTabs = len(Tabs)
		TabWidth = (self.W_SCREEN - ExitTabWidth) / NumTabs

		Ydisplacment = 38
		XIncrement = self.W_SCREEN / 6

		for iTab in range(NumTabs):
			OnTabName = "OnTab" + str(iTab)
			EdgeWidth = 25
			BottomPanelHight = 55
			screen.addPanel(OnTabName + "Left", "", "", False, False, TabWidth * iTab, self.H_SCREEN - BottomPanelHight, EdgeWidth, BottomPanelHight, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.addPanel(OnTabName + "Center", "", "", False, False, (TabWidth * iTab) + EdgeWidth, self.H_SCREEN - BottomPanelHight, TabWidth - (EdgeWidth * 2), BottomPanelHight, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.addPanel(OnTabName + "Right", "", "", False, False, (TabWidth * iTab) + TabWidth - EdgeWidth, self.H_SCREEN - BottomPanelHight, EdgeWidth, BottomPanelHight, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)

			screen.addDrawControl(OnTabName + "Left", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_START").getPath(), TabWidth * iTab, self.H_SCREEN - BottomPanelHight, EdgeWidth, BottomPanelHight, WidgetTypes.WIDGET_GENERAL, -1, -1 )
			screen.addDrawControl(OnTabName + "Center", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_ON").getPath(), (TabWidth * iTab) + EdgeWidth, self.H_SCREEN - BottomPanelHight, TabWidth - (EdgeWidth * 2), BottomPanelHight, WidgetTypes.WIDGET_GENERAL, -1, -1 )
			screen.addDrawControl(OnTabName + "Right", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_END").getPath(), (TabWidth * iTab) + TabWidth - EdgeWidth, self.H_SCREEN - BottomPanelHight, EdgeWidth, BottomPanelHight, WidgetTypes.WIDGET_GENERAL, -1, -1 )

			TabText = Tabs[iTab]
			TabText = localText.changeTextColor(TabText, gc.getInfoTypeForString("COLOR_FONT_CREAM"))

			screen.setTextAt("OnTabTitle" + str(iTab), OnTabName + "Center", u"<font=4>" + TabText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, 0 , 33, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.hide(OnTabName + "Left")
			screen.hide(OnTabName + "Center")
			screen.hide(OnTabName + "Right")

			OffTabName = "OffTab" + str(iTab)
			screen.addPanel(OffTabName, "", "", False, False, TabWidth * iTab, self.H_SCREEN - BottomPanelHight, TabWidth, BottomPanelHight, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, 1111, iTab)
			screen.addDrawControl(OffTabName, ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_OFF").getPath(), TabWidth * iTab, self.H_SCREEN - BottomPanelHight, TabWidth, BottomPanelHight, WidgetTypes.WIDGET_GENERAL, -1, -1 )
			screen.setTextAt("OffTabTitle" + str(iTab), OffTabName, u"<font=4>" + TabText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, EdgeWidth , 33, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1111, iTab)

		screen.addPanel("ExitTab", "", "", False, False, self.W_SCREEN - ExitTabWidth, self.H_SCREEN - BottomPanelHight, ExitTabWidth, BottomPanelHight, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addDrawControl("ExitTab", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_OFF").getPath(), self.W_SCREEN - ExitTabWidth, self.H_SCREEN - BottomPanelHight, ExitTabWidth, BottomPanelHight, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setTextAt("ExitTabTitle", "ExitTab", u"<font=4>" + CyTranslator().getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + "</font>", CvUtil.FONT_RIGHT_JUSTIFY, ExitTabWidth - 10 , BottomPanelHight - 18, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)

	def setTab(self, iTab, State):
		screen = self.getScreen()
		OnTabName = "OnTab" + str(iTab)

		if (State):
			screen.show(OnTabName + "Left")
			screen.show(OnTabName + "Center")
			screen.show(OnTabName + "Right")
			screen.hide("OffTab" + str(iTab))
		else:
			screen.hide(OnTabName + "Left")
			screen.hide(OnTabName + "Center")
			screen.hide(OnTabName + "Right")
			screen.show("OffTab" + str(iTab))
		
	def redrawContents(self):

		screen = self.getScreen()
		self.deleteAllWidgets(self.iNumPermanentWidgets)

		self.szGraphTabWidget = self.getNextWidgetName()
		self.szDemographicsTabWidget = self.getNextWidgetName()
		self.szTopCitiesTabWidget = self.getNextWidgetName()
		self.szStatsTabWidget = self.getNextWidgetName()

		# Draw Tab buttons and tabs
		if (self.iActiveTab == self.iGraphID):
			self.drawGraphTab()

		elif (self.iActiveTab == self.iDemographicsID):
			self.drawDemographicsTab()

		elif(self.iActiveTab == self.iTopCitiesID):
			self.drawTopCitiesTab()

		elif(self.iActiveTab == self.iStatsID):
			self.drawStatsTab()
			
		for i in range(4):
			if (i != self.iActiveTab):
				self.setTab(i, False)
			else:
				self.setTab(i, True)

#################################################### GRAPH ##################################################

	def drawGraphTab(self):

		self.iGraphTabID = self.TOTAL_SCORE
		self.drawPermanentGraphWidgets()
		self.drawGraph()

	def drawPermanentGraphWidgets(self):

		screen = self.getScreen()

		self.H_LEGEND = 2 * self.Y_LEGEND_MARGIN + self.iNumPlayersMet * self.H_LEGEND_TEXT + 3
		self.Y_LEGEND = self.Y_GRAPH + self.H_GRAPH - self.H_LEGEND
		#self.X_LEGEND -= 25
		#self.W_LEGEND += 30

		self.LEGEND_PANEL_ID = self.getNextWidgetName()
		screen.addPanel(self.LEGEND_PANEL_ID, "", "", True, True, self.X_LEGEND, self.Y_LEGEND, self.W_LEGEND, self.H_LEGEND, PanelStyles.PANEL_STYLE_IN, WidgetTypes.WIDGET_GENERAL, -1, -1)
		#screen.addDrawControl("LegendBackGroundCanvas", None, self.X_LEGEND, self.Y_LEGEND, self.W_LEGEND, self.H_LEGEND, WidgetTypes.WIDGET_GENERAL, -1, -1)
		self.LEGEND_CANVAS_ID = self.getNextWidgetName()

		self.drawLegend()

		self.graphLeftButtonID = self.getNextWidgetName()
		screen.setButtonGFC( self.graphLeftButtonID, u"", "", self.X_LEFT_BUTTON, self.Y_LEFT_BUTTON, self.W_LEFT_BUTTON, self.H_LEFT_BUTTON, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_ARROW_LEFT )
		self.graphRightButtonID = self.getNextWidgetName()
		screen.setButtonGFC( self.graphRightButtonID, u"", "", self.X_RIGHT_BUTTON, self.Y_RIGHT_BUTTON, self.W_RIGHT_BUTTON, self.H_RIGHT_BUTTON, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_ARROW_RIGHT )
		screen.enable(self.graphLeftButtonID, False)
		screen.enable(self.graphRightButtonID, False)

		# Dropdown Box
		self.szGraphDropdownWidget = self.getNextWidgetName()
		screen.addDropDownBoxGFC(self.szGraphDropdownWidget, self.X_DEMO_DROPDOWN, self.Y_DEMO_DROPDOWN, self.W_DEMO_DROPDOWN, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString(self.szGraphDropdownWidget, self.TEXT_SCORE, 0, 0, False )
		screen.addPullDownString(self.szGraphDropdownWidget, self.TEXT_ECONOMY, 1, 1, False )
		screen.addPullDownString(self.szGraphDropdownWidget, self.TEXT_INDUSTRY, 2, 2, False )
		screen.addPullDownString(self.szGraphDropdownWidget, self.TEXT_AGRICULTURE, 3, 3, False )
		screen.addPullDownString(self.szGraphDropdownWidget, self.TEXT_POWER, 4, 4, False )
		screen.addPullDownString(self.szGraphDropdownWidget, self.TEXT_CULTURE, 5, 5, False )

		self.dropDownTurns = []
		self.szTurnsDropdownWidget = self.getNextWidgetName()
		screen.addDropDownBoxGFC(self.szTurnsDropdownWidget, self.X_ZOOM_DROPDOWN, self.Y_ZOOM_DROPDOWN, self.W_ZOOM_DROPDOWN, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		start = CyGame().getStartTurn()
		now   = CyGame().getGameTurn()
		nTurns = now - start - 1
		screen.addPullDownString(self.szTurnsDropdownWidget, self.TEXT_ENTIRE_HISTORY, 0, 0, False)
		self.dropDownTurns.append(nTurns)
		iCounter = 1
		last = 50
		while (last < nTurns):
			screen.addPullDownString(self.szTurnsDropdownWidget, localText.getText("TXT_KEY_INFO_NUM_TURNS", (last,)), iCounter, iCounter, False)
			self.dropDownTurns.append(last)
			iCounter += 1
			last += 50

		self.iNumPreDemoChartWidgets = self.nWidgetCount

	def updateGraphButtons(self):
		screen = self.getScreen()
		screen.enable(self.graphLeftButtonID, self.graphEnd - self.graphZoom > CyGame().getStartTurn())
		screen.enable(self.graphRightButtonID, self.graphEnd < CyGame().getGameTurn() - 1)

	def checkGraphBounds(self):
		start = CyGame().getStartTurn()
		end   = CyGame().getGameTurn() - 1
		if (self.graphEnd - self.graphZoom < start):
			self.graphEnd = start + self.graphZoom
		if (self.graphEnd > end):
			self.graphEnd = end

	def zoomGraph(self, zoom):
		self.graphZoom = zoom
		self.checkGraphBounds()
		self.updateGraphButtons()

	def slideGraph(self, right):
		self.graphEnd += right
		self.checkGraphBounds()
		self.updateGraphButtons()

	def buildScoreCache(self, scoreType):

		# Check if the scores have already been computed
		if (self.scoreCache[scoreType]):
			return

		print("Rebuilding score cache")

		# Get the player with the highest ID
		maxPlayer = 0
		for p in self.aiPlayersMet:
			if (maxPlayer < p):
				maxPlayer = p

		# Compute the scores
		self.scoreCache[scoreType] = []
		for p in range(maxPlayer + 1):
			if (p not in self.aiPlayersMet):
				# Don't compute score for people we haven't met
				self.scoreCache[scoreType].append(None)
			else:
				self.scoreCache[scoreType].append([])
				firstTurn = CyGame().getStartTurn()
				thisTurn = CyGame().getGameTurn()
				turn = firstTurn
				while (turn <= thisTurn):
					self.scoreCache[scoreType][p].append(self.computeHistory(scoreType, p, turn))
					turn += 1

		return

	def computeHistory(self, scoreType, iPlayer, iTurn):

		iScore = gc.getPlayer(iPlayer).getScoreHistory(iTurn)

		if (iScore == 0):	# for some reason only the score is 0 when you're dead..?
			return 0

		if (scoreType == self.TOTAL_SCORE):
			return iScore
		elif (scoreType == self.ECONOMY_SCORE):
			return gc.getPlayer(iPlayer).getEconomyHistory(iTurn)
		elif (scoreType == self.INDUSTRY_SCORE):
			return gc.getPlayer(iPlayer).getIndustryHistory(iTurn)
		elif (scoreType == self.AGRICULTURE_SCORE):
			return gc.getPlayer(iPlayer).getAgricultureHistory(iTurn)
		elif (scoreType == self.POWER_SCORE):
			return gc.getPlayer(iPlayer).getPowerHistory(iTurn)
		elif (scoreType == self.CULTURE_SCORE):
			return gc.getPlayer(iPlayer).getCultureHistory(iTurn)

	# Requires the cache to be built
	def getHistory(self, scoreType, iPlayer, iRelTurn):
		return self.scoreCache[scoreType][iPlayer][iRelTurn]

	def drawGraphLines(self):
		screen = self.getScreen()

		if (self.xSelPt != 0 or self.ySelPt != 0):
			screen.addLineGFC(self.GRAPH_CANVAS_ID, self.GRAPH_H_LINE, 0, self.ySelPt, self.W_GRAPH, self.ySelPt, gc.getInfoTypeForString("COLOR_GREY"))
			screen.addLineGFC(self.GRAPH_CANVAS_ID, self.GRAPH_V_LINE, self.xSelPt, 0, self.xSelPt, self.H_GRAPH, gc.getInfoTypeForString("COLOR_GREY"))
		else:
			screen.addLineGFC(self.GRAPH_CANVAS_ID, self.GRAPH_H_LINE, -1, -1, -1, -1, gc.getInfoTypeForString("COLOR_GREY"))
			screen.addLineGFC(self.GRAPH_CANVAS_ID, self.GRAPH_V_LINE, -1, -1, -1, -1, gc.getInfoTypeForString("COLOR_GREY"))


	def drawXLabel(self, screen, turn, x, just = CvUtil.FONT_CENTER_JUSTIFY):
		screen.setLabel( self.getNextWidgetName(), "", u"<font=2>" + self.getTurnDate(turn) + u"</font>", just , x , self.Y_LABEL, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

	def drawGraph(self):

		screen = self.getScreen()
		self.deleteAllLines()
		self.deleteAllWidgets(self.iNumPreDemoChartWidgets)

		# Draw the graph widget
		self.GRAPH_CANVAS_ID = self.getNextWidgetName()
		screen.addDrawControl(self.GRAPH_CANVAS_ID, ArtFileMgr.getInterfaceArtInfo("SCREEN_BG_STATISTICS").getPath(), self.X_GRAPH, self.Y_GRAPH, self.W_GRAPH, self.H_GRAPH, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# Compute the scores
		self.buildScoreCache(self.iGraphTabID)

		# Compute max score
		max = 0
		thisTurn = CyGame().getGameTurn()
		startTurn = CyGame().getStartTurn()

		if (self.graphZoom == 0 or self.graphEnd == 0):
			firstTurn = startTurn
		else:
			firstTurn = self.graphEnd - self.graphZoom

		if (self.graphEnd == 0):
			lastTurn  = thisTurn - 1 # all civs haven't neccessarily got a score for the current turn
		else:
			lastTurn  = self.graphEnd

		self.drawGraphLines()

		# Draw x-labels
		self.drawXLabel( screen, firstTurn, self.X_LEFT_LABEL,  CvUtil.FONT_LEFT_JUSTIFY  )
		self.drawXLabel( screen, lastTurn,  self.X_RIGHT_LABEL, CvUtil.FONT_RIGHT_JUSTIFY )

		# Don't draw anything the first turn
		if (firstTurn >= lastTurn):
			return

		# Compute max and min
		max = 1
		min = 0
		for p in self.aiPlayersMet:
			for turn in range(firstTurn,lastTurn + 1):
				score = self.getHistory(self.iGraphTabID, p, turn - startTurn)
				if (max < score):
					max = score
				if (min > score):
					min = score

		yFactor = (1.0 * self.H_GRAPH / (1.0 * (max - min)))
		xFactor = (1.0 * self.W_GRAPH / (1.0 * (lastTurn - firstTurn)))

		if (lastTurn - firstTurn > 10):
			turn = (firstTurn + lastTurn) / 2
			self.drawXLabel ( screen, turn, self.X_GRAPH + int(xFactor * (turn - firstTurn)) )
		if (lastTurn - firstTurn > 20):
			turn = firstTurn + (lastTurn - firstTurn) / 4
			self.drawXLabel ( screen, turn, self.X_GRAPH + int(xFactor * (turn - firstTurn)) )
			turn = firstTurn + 3 * (lastTurn - firstTurn) / 4
			self.drawXLabel ( screen, turn, self.X_GRAPH + int(xFactor * (turn - firstTurn)) )

		# Draw the lines
		for p in self.aiPlayersMet:

			if (self.aiPlayerToggle[p]):
				color = gc.getPlayerColorInfo(gc.getPlayer(p).getPlayerColor()).getColorTypePrimary()
				oldX = -1
				oldY = self.H_GRAPH
				turn = lastTurn
				while (turn >= firstTurn):

					score = self.getHistory(self.iGraphTabID, p, turn - startTurn)
					y = self.H_GRAPH - int(yFactor * (score - min))
					x = int(xFactor * (turn - firstTurn))

					if (x < oldX):
						if (y != self.H_GRAPH or oldY != self.H_GRAPH): # don't draw if score is constant zero
							self.drawLine(screen, self.GRAPH_CANVAS_ID, oldX, oldY, x, y, color)
						oldX = x
						oldY = y
					elif (oldX == -1):
						oldX = x
						oldY = y

					turn -= 1

		return

	def drawLegend(self):
		screen = self.getScreen()

		yLine = self.Y_LEGEND_LINE
		yText = self.Y_LEGEND + self.Y_LEGEND_TEXT

		self.H_LEGEND = 2 * self.Y_LEGEND_MARGIN + self.iNumPlayersMet * self.H_LEGEND_TEXT + 3
		self.Y_LEGEND = self.Y_GRAPH + self.H_GRAPH - self.H_LEGEND

		screen.addDrawControl(self.LEGEND_CANVAS_ID, None, self.X_LEGEND, self.Y_LEGEND, self.W_LEGEND_LINE, self.H_LEGEND, WidgetTypes.WIDGET_GENERAL, -1, -1)

		for iPlayer in range(len(self.aiPlayersMet)):
			p = self.aiPlayersMet[iPlayer]
			lineColor = gc.getPlayerColorInfo(gc.getPlayer(p).getPlayerColor()).getColorTypePrimary()
			textColorR = gc.getPlayer(p).getPlayerTextColorR()
			textColorG = gc.getPlayer(p).getPlayerTextColorG()
			textColorB = gc.getPlayer(p).getPlayerTextColorB()
			textColorA = gc.getPlayer(p).getPlayerTextColorA()
			name = gc.getPlayer(p).getName()

			if (self.aiPlayerToggle[p]):
				self.drawLine(screen, self.LEGEND_CANVAS_ID, self.X_LEGEND_LINE, yLine, self.X_LEGEND_LINE + self.W_LEGEND_LINE, yLine, lineColor)
			
			str = u"<color=%d,%d,%d,%d>%s</color>" %(textColorR, textColorG, textColorB, textColorA, name[:22])
			szTitle = self.getNextWidgetName()
			screen.setText(szTitle, "", u"<font=2>" + str + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, self.X_LEGEND + self.X_LEGEND_TEXT, yText, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 2222, p)

			yLine += self.H_LEGEND_TEXT
			yText += self.H_LEGEND_TEXT

################################################# DEMOGRAPHICS ##############################################

	def drawDemographicsTab(self):

		self.drawTextChart()

	def drawTextChart(self):

		######## DATA ########

		iNumActivePlayers = 0

		pPlayer = gc.getPlayer(self.iActivePlayer)

		iEconomy = pPlayer.getGold()
		iIndustry = pPlayer.getYieldRate(YieldTypes.YIELD_HAMMERS)
		iAgriculture = pPlayer.calculateTotalYield(YieldTypes.YIELD_FOOD)
		fMilitary = pPlayer.getPower()
		iLandArea = pPlayer.getTotalLand() * 1000
		iPopulation = pPlayer.getRealPopulation()

		iEconomyRank = 0
		iIndustryRank = 0
		iAgricultureRank = 0
		iMilitaryRank = 0
		iLandAreaRank = 0
		iPopulationRank = 0

		fEconomyGameAverage = 0
		fIndustryGameAverage = 0
		fAgricultureGameAverage = 0
		fMilitaryGameAverage = 0
		fLandAreaGameAverage = 0
		fPopulationGameAverage = 0

		# Lists of Player values - will be used to determine rank, strength and average per city

		aiGroupEconomy = []
		aiGroupIndustry = []
		aiGroupAgriculture = []
		aiGroupMilitary = []
		aiGroupLandArea = []
		aiGroupPopulation = []

		# Loop through all players to determine Rank and relative Strength
		for iPlayerLoop in range(gc.getMAX_PLAYERS()):

			if (gc.getPlayer(iPlayerLoop).isAlive()):

				# Erik: We only include the Europeans when computing the demographics
				if (not gc.getPlayer(iPlayerLoop).isNative() and not gc.getPlayer(iPlayerLoop).isEurope() and not gc.getGame().isChurchPlayer(iPlayerLoop) and not gc.getGame().isBarbarianPlayer(iPlayerLoop)):

					iNumActivePlayers += 1

					pCurrPlayer = gc.getPlayer(iPlayerLoop)
					aiGroupEconomy.append(pCurrPlayer.getGold())
					aiGroupIndustry.append(pCurrPlayer.getYieldRate(YieldTypes.YIELD_HAMMERS))
					aiGroupAgriculture.append(pCurrPlayer.calculateTotalYield(YieldTypes.YIELD_FOOD))
					aiGroupMilitary.append(pCurrPlayer.getPower())
					aiGroupLandArea.append(pCurrPlayer.getTotalLand() * 1000)
					aiGroupPopulation.append(pCurrPlayer.getRealPopulation())

		aiGroupEconomy.sort()
		aiGroupIndustry.sort()
		aiGroupAgriculture.sort()
		aiGroupMilitary.sort()
		aiGroupLandArea.sort()
		aiGroupPopulation.sort()

		aiGroupEconomy.reverse()
		aiGroupIndustry.reverse()
		aiGroupAgriculture.reverse()
		aiGroupMilitary.reverse()
		aiGroupLandArea.reverse()
		aiGroupPopulation.reverse()

		# Lists of player values are ordered from highest first to lowest, so determine Rank, Strength and World Average

		bEconomyFound = False
		bIndustryFound = False
		bAgricultureFound = False
		bMilitaryFound = False
		bLandAreaFound = False
		bPopulationFound = False

		for i in range(len(aiGroupEconomy)):

			if (iEconomy == aiGroupEconomy[i] and not bEconomyFound):
				iEconomyRank = i + 1
				bEconomyFound = True
			else:
				fEconomyGameAverage += aiGroupEconomy[i]

			if (iIndustry == aiGroupIndustry[i] and not bIndustryFound):
				iIndustryRank = i + 1
				bIndustryFound = True
			else:
				fIndustryGameAverage += aiGroupIndustry[i]

			if (iAgriculture == aiGroupAgriculture[i] and not bAgricultureFound):
				iAgricultureRank = i + 1
				bAgricultureFound = True
			else:
				fAgricultureGameAverage += aiGroupAgriculture[i]

			if (fMilitary == aiGroupMilitary[i] and not bMilitaryFound):
				iMilitaryRank = i + 1
				bMilitaryFound = True
			else:
				fMilitaryGameAverage += aiGroupMilitary[i]

			if (iLandArea == aiGroupLandArea[i] and not bLandAreaFound):
				iLandAreaRank = i + 1
				bLandAreaFound = True
			else:
				fLandAreaGameAverage += aiGroupLandArea[i]

			if (iPopulation == aiGroupPopulation[i] and not bPopulationFound):
				iPopulationRank = i + 1
				bPopulationFound = True
			else:
				fPopulationGameAverage += aiGroupPopulation[i]

		iEconomyGameBest = 0
		iIndustryGameBest = 0
		iAgricultureGameBest = 0
		iMilitaryGameBest = 0
		iLandAreaGameBest = 0
		iPopulationGameBest = 0

		iEconomyGameWorst = 0
		iIndustryGameWorst = 0
		iAgricultureGameWorst = 0
		iMilitaryGameWorst = 0
		iLandAreaGameWorst = 0
		iPopulationGameWorst = 0

		if (iNumActivePlayers > 1):

			fEconomyGameAverage = (1.0 * fEconomyGameAverage) / (iNumActivePlayers - 1)
			fIndustryGameAverage = (1.0 * fIndustryGameAverage) / (iNumActivePlayers - 1)
			fAgricultureGameAverage = (1.0 * fAgricultureGameAverage) / (iNumActivePlayers - 1)
			fMilitaryGameAverage = int((1.0 * fMilitaryGameAverage) / (iNumActivePlayers - 1))
			fLandAreaGameAverage = (1.0 * fLandAreaGameAverage) / (iNumActivePlayers - 1)
			fPopulationGameAverage = int((1.0 * fPopulationGameAverage) / (iNumActivePlayers - 1))

			ix = lambda x: iff(x == 1, 1, 0)

			iEconomyGameBest	= aiGroupEconomy[ix(iEconomyRank)]
			iIndustryGameBest	= aiGroupIndustry[ix(iIndustryRank)]
			iAgricultureGameBest	= aiGroupAgriculture[ix(iAgricultureRank)]
			iMilitaryGameBest	= aiGroupMilitary[ix(iMilitaryRank)]
			iLandAreaGameBest	= aiGroupLandArea[ix(iLandAreaRank)]
			iPopulationGameBest	= aiGroupPopulation[ix(iPopulationRank)]

			ix = lambda x: iff(x == iNumActivePlayers, iNumActivePlayers - 2, iNumActivePlayers - 1)

			iEconomyGameWorst	= aiGroupEconomy[ix(iEconomyRank)]
			iIndustryGameWorst	= aiGroupIndustry[ix(iIndustryRank)]
			iAgricultureGameWorst	= aiGroupAgriculture[ix(iAgricultureRank)]
			iMilitaryGameWorst	= aiGroupMilitary[ix(iMilitaryRank)]
			iLandAreaGameWorst	= aiGroupLandArea[ix(iLandAreaRank)]
			iPopulationGameWorst	= aiGroupPopulation[ix(iPopulationRank)]


		######## TEXT ########

		screen = self.getScreen()

		# Create Table
		szTable = self.getNextWidgetName()
		screen.addTableControlGFC(szTable, 6, (self.W_SCREEN / 2) - (self.W_CHART / 2), self.Y_CHART, self.W_CHART, self.H_CHART, True, True, 32,32, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader(szTable, 0, self.TEXT_DEMOGRAPHICS_SMALL, 224) # Total graph width is 430
		screen.setTableColumnHeader(szTable, 1, self.TEXT_VALUE, 155)
		screen.setTableColumnHeader(szTable, 2, self.TEXT_BEST, 155)
		screen.setTableColumnHeader(szTable, 3, self.TEXT_AVERAGE, 155)
		screen.setTableColumnHeader(szTable, 4, self.TEXT_WORST, 155)
		screen.setTableColumnHeader(szTable, 5, self.TEXT_RANK, 90)

		for i in range(18):
			screen.appendTableRow(szTable)
		iNumRows = screen.getTableNumRows(szTable)
		iRow = iNumRows - 1
		iCol = 0
		screen.setTableText(szTable, iCol, 0, self.TEXT_ECONOMY, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 1, self.TEXT_ECONOMY_MEASURE, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 3, self.TEXT_INDUSTRY, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 4, self.TEXT_INDUSTRY_MEASURE, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 6, self.TEXT_AGRICULTURE, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 7, self.TEXT_AGRICULTURE_MEASURE, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 9, self.TEXT_MILITARY, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 11, self.TEXT_LAND_AREA, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 12, self.TEXT_LAND_AREA_MEASURE, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 14, self.TEXT_POPULATION, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

		iCol = 1
		screen.setTableText(szTable, iCol, 0, str(iEconomy), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 3, str(iIndustry), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 6, str(iAgriculture), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 9, str(int(fMilitary)), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 11, str(iLandArea), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 14, str(iPopulation), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

		iCol = 2
		screen.setTableText(szTable, iCol, 0, str(iEconomyGameBest), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 3, str(iIndustryGameBest), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 6, str(iAgricultureGameBest), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 9, str(iMilitaryGameBest), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 11, str(iLandAreaGameBest), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 14, str(iPopulationGameBest), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

		iCol = 3
		screen.setTableText(szTable, iCol, 0, str(int(fEconomyGameAverage)), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 3, str(int(fIndustryGameAverage)), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 6, str(int(fAgricultureGameAverage)), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 9, str(int(fMilitaryGameAverage)), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 11, str(int(fLandAreaGameAverage)), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 14, str(int(fPopulationGameAverage)), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

		iCol = 4
		screen.setTableText(szTable, iCol, 0, str(iEconomyGameWorst), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 3, str(iIndustryGameWorst), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 6, str(iAgricultureGameWorst), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 9, str(iMilitaryGameWorst), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 11, str(iLandAreaGameWorst), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 14, str(iPopulationGameWorst), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

		iCol = 5
		screen.setTableText(szTable, iCol, 0, str(iEconomyRank), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 3, str(iIndustryRank), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 6, str(iAgricultureRank), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 9, str(iMilitaryRank), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 11, str(iLandAreaRank), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, iCol, 14, str(iPopulationRank), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

		return

################################################## TOP CITIES ###############################################

	def drawTopCitiesTab(self):

		screen = self.getScreen()

		# Background Panes
		self.szLeftPaneWidget = self.getNextWidgetName()
		screen.addPanel( self.szLeftPaneWidget, "", "", True, True, self.X_LEFT_PANE, self.Y_LEFT_PANE, self.W_LEFT_PANE, self.H_LEFT_PANE, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )#PanelStyles.PANEL_STYLE_DAWNTOP )

		self.szFatherPaneWidget = self.getNextWidgetName()
		screen.addPanel( self.szFatherPaneWidget, "", "", True, True, self.W_SCREEN - self.W_LEFT_PANE - self.X_LEFT_PANE, self.Y_LEFT_PANE, self.W_LEFT_PANE, self.H_LEFT_PANE, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )#PanelStyles.PANEL_STYLE_DAWNTOP )

		self.drawTopCities()

	def drawTopCities(self):

		self.calculateTopCities()
		self.determineCityData()

		screen = self.getScreen()

		self.szCityNameWidgets = []
		self.szCityDescWidgets = []
		self.szCityAnimWidgets = []

		for iWidgetLoop in range(self.iNumCities):
			
			szTextPanel = self.getNextWidgetName()
			screen.addPanel( szTextPanel, "", "", False, True, self.X_COL_1_CITIES_DESC, self.Y_ROWS_CITIES[iWidgetLoop] + self.Y_CITIES_DESC_BUFFER, self.W_CITIES_DESC, self.H_CITIES_DESC, PanelStyles.PANEL_STYLE_DAWNTOP, WidgetTypes.WIDGET_GENERAL, -1, -1 )
			self.szCityNameWidgets.append(self.getNextWidgetName())
			szCityDesc = u"<font=4b>" + str(self.iCitySizes[iWidgetLoop]) + u"</font>" + " - " + u"<font=3b>" + self.szCityNames[iWidgetLoop] + u"</font>" + "\n"
			szCityDesc += self.szCityDescs[iWidgetLoop]
			screen.addMultilineText(self.szCityNameWidgets[iWidgetLoop], szCityDesc, self.X_COL_1_CITIES_DESC + 6, self.Y_ROWS_CITIES[iWidgetLoop] + self.Y_CITIES_DESC_BUFFER + 3, self.W_CITIES_DESC - 6, self.H_CITIES_DESC - 6, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
			#screen.attachMultilineText( szTextPanel, self.szCityNameWidgets[iWidgetLoop], str(self.iCitySizes[iWidgetLoop]) + " - " + self.szCityNames[iWidgetLoop] + "\n" + self.szCityDescs[iWidgetLoop], WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

			iCityX = self.aaCitiesXY[iWidgetLoop][0]
			iCityY = self.aaCitiesXY[iWidgetLoop][1]
			pPlot = CyMap().plot(iCityX, iCityY)
			pCity = pPlot.getPlotCity()

			iDistance = 200 + (pCity.getPopulation() * 5)
			if (iDistance > 350):
				iDistance = 350

			#print "City # " + str(iWidgetLoop) + "  " + pCity.getName()

			self.szCityAnimWidgets.append(self.getNextWidgetName())
			if (pCity.isRevealed(gc.getGame().getActiveTeam(), False)):
				screen.addPlotGraphicGFC(self.szCityAnimWidgets[iWidgetLoop], self.X_CITY_ANIMATION, self.Y_ROWS_CITIES[iWidgetLoop], self.W_CITY_ANIMATION, self.H_CITY_ANIMATION, pPlot, iDistance, False, WidgetTypes.WIDGET_GENERAL, -1, -1)
				
		
		szFatherTable = self.getNextWidgetName()
		screen.addScrollPanel(szFatherTable, u"", self.W_SCREEN - self.W_LEFT_PANE - self.X_LEFT_PANE + 20, self.Y_LEFT_PANE + 30, self.W_LEFT_PANE - 40, self.H_LEFT_PANE- 60, PanelStyles.PANEL_STYLE_EMPTY, True, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		screen.setTextAt("FatherHeader", szFatherTable, "<font=4>" + localText.getText("TXT_KEY_FATHER_SCREEN_TITLE", ()).upper() + "</font>", CvUtil.FONT_LEFT_JUSTIFY, 50, 10, -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		iYPosition = 60
		for iFather in range(gc.getNumFatherInfos()):
			if (gc.getGame().getFatherTeam(iFather) != TeamTypes.NO_TEAM):
				for k in range(gc.getMAX_PLAYERS()):
					# RaR, fix for wrong display of FF Team name
					# if (gc.getPlayer(k).getTeam() == gc.getGame().getFatherTeam(iFather)):
					if (gc.getPlayer(k).getTeam() == gc.getGame().getFatherTeam(iFather) and not gc.getPlayer(k).isNative()):
						if (gc.getTeam(gc.getPlayer(gc.getGame().getActivePlayer()).getTeam()).isHasMet(gc.getGame().getFatherTeam(iFather))):
						
							szText = gc.getFatherInfo(iFather).getDescription() + " - " + localText.getText("TXT_KEY_LEADER_CIV_DESCRIPTION", (gc.getPlayer(k).getNameKey(), gc.getPlayer(k).getCivilizationShortDescriptionKey()))
							szTempBuffer = u"<color=%d,%d,%d,%d>%s</color>" %(gc.getPlayer(k).getPlayerTextColorR(), gc.getPlayer(k).getPlayerTextColorG(), gc.getPlayer(k).getPlayerTextColorB(), gc.getPlayer(k).getPlayerTextColorA(), szText)

							screen.addDDSGFCAt("Portrait" + str(iFather), szFatherTable, gc.getFatherInfo(iFather).getPortrait(), 0, iYPosition, 50, 50, WidgetTypes.WIDGET_GENERAL, -1, -1, False)
							screen.setTextAt("FatherID" + str(iFather), szFatherTable, "<font=2>" + szTempBuffer + "</font>", CvUtil.FONT_LEFT_JUSTIFY, 50, iYPosition + 20, -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_PEDIA_JUMP_TO_FATHER, iFather, -1 )
							iYPosition += 50
						else:
							screen.addDDSGFCAt("Portrait" + str(iFather), szFatherTable, gc.getFatherInfo(iFather).getPortrait(), 0, iYPosition, 50, 50, WidgetTypes.WIDGET_GENERAL, -1, -1, False)
							screen.setTextAt("FatherID" + str(iFather), szFatherTable, "<font=2>" + gc.getFatherInfo(iFather).getDescription() + " - " + localText.getText("TXT_KEY_UNKNOWN", ()) + "</font>", CvUtil.FONT_LEFT_JUSTIFY, 50, iYPosition + 20, -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_PEDIA_JUMP_TO_FATHER, iFather, -1 )
							iYPosition += 50

		return

	def calculateTopCities(self):

		# Calculate the top 5 cities
		for iPlayerLoop in range(gc.getMAX_PLAYERS()):

			player = gc.getPlayer(iPlayerLoop)

			(pCity, iter) = player.firstCity(False)

			while(pCity):

				iTotalCityValue = ((pCity.getCulture(iPlayerLoop) / 5) + (pCity.getYieldRate(YieldTypes.YIELD_FOOD) + pCity.getYieldRate(YieldTypes.YIELD_HAMMERS))) * pCity.getPopulation()

				for iRankLoop in range(5):
					if (iTotalCityValue > self.iCityValues[iRankLoop]):
						self.addCityToList(iRankLoop, pCity, iTotalCityValue)
						break

				(pCity, iter) = player.nextCity(iter, False)

	# Recursive
	def addCityToList(self, iRank, pCity, iTotalCityValue):

		if (iRank > 4):
			return

		else:
			pTempCity = self.pCityPointers[iRank]

			# Verify a city actually exists at this rank
			if (pTempCity):
				iTempCityValue = self.iCityValues[iRank]
				self.addCityToList(iRank+1, pTempCity, iTempCityValue)
				self.pCityPointers[iRank] = pCity
				self.iCityValues[iRank] = iTotalCityValue
			else:
				self.pCityPointers[iRank] = pCity
				self.iCityValues[iRank] = iTotalCityValue

				return

	def determineCityData(self):

		self.iNumCities = 0

		for iRankLoop in range(5):

			pCity = self.pCityPointers[iRankLoop]

			# If this city exists and has data we can use
			if (pCity):
				pPlayer = gc.getPlayer(pCity.getOwner())
				iTurnYear = CyGame().getTurnYear(pCity.getGameTurnFounded())

				if (iTurnYear < 0):
					szTurnFounded = localText.getText("TXT_KEY_TIME_BC", (-iTurnYear,))#"%d %s" %(-iTurnYear, self.TEXT_BC)
				else:
					szTurnFounded = localText.getText("TXT_KEY_TIME_AD", (iTurnYear,))#"%d %s" %(iTurnYear, self.TEXT_AD)

				if (pCity.isRevealed(gc.getGame().getActiveTeam(), False) or gc.getTeam(pPlayer.getTeam()).isHasMet(gc.getGame().getActiveTeam())):
					self.szCityNames[iRankLoop] = pCity.getName().upper()
					if (not pPlayer.isNative()):
						self.szCityDescs[iRankLoop] = ("%s, %s" %(pPlayer.getCivilizationAdjective(0), localText.getText("TXT_KEY_MISC_FOUNDED_IN", (szTurnFounded,))))
					else:
						self.szCityDescs[iRankLoop] = ("%s" %(pPlayer.getCivilizationAdjective(0)))
				else:
					self.szCityNames[iRankLoop] = localText.getText("TXT_KEY_UNKNOWN", ()).upper()
					if (not pPlayer.isNative()):
						self.szCityDescs[iRankLoop] = ("%s" %(localText.getText("TXT_KEY_MISC_FOUNDED_IN", (szTurnFounded,)), ))
				self.iCitySizes[iRankLoop] = pCity.getPopulation()
				self.aaCitiesXY[iRankLoop] = [pCity.getX(), pCity.getY()]

				self.iNumCities += 1
			else:

				self.szCityNames[iRankLoop] = ""
				self.iCitySizes[iRankLoop] = -1
				self.szCityDescs[iRankLoop] = ""
				self.aaCitiesXY[iRankLoop] = [-1, -1]

		return

################################################## STATISTICS ###############################################

	def drawStatsTab(self):

		screen = self.getScreen()
		ShowUnits = []
		for i in range(gc.getNumUnitInfos()):
			if gc.getUnitInfo(i).getMoves()!= 0:
				ShowUnits.append(i)
		
		iNumBuildings = 0
		for iBuilding in range(gc.getNumBuildingInfos()):
			if (not gc.getBuildingInfo(iBuilding).isGraphicalOnly()):
				iNumBuildings += 1

		self.iNumUnitStatsChartCols = 5
		self.iNumBuildingStatsChartCols = 2
		self.iNumUnitStatsChartRows = len(ShowUnits)
		self.iNumBuildingStatsChartRows = iNumBuildings

################################################### CALCULATE STATS ###################################################

		iMinutesPlayed = CyGame().getMinutesPlayed()
		iHoursPlayed = iMinutesPlayed / 60
		iMinutesPlayed = iMinutesPlayed - (iHoursPlayed * 60)

		szMinutesString = str(iMinutesPlayed)
		if (iMinutesPlayed < 10):
			szMinutesString = "0" + szMinutesString
		szHoursString = str(iHoursPlayed)
		if (iHoursPlayed < 10):
			szHoursString = "0" + szHoursString

		szTimeString = szHoursString + ":" + szMinutesString

		iNumCitiesBuilt = CyStatistics().getPlayerNumCitiesBuilt(self.iActivePlayer)

		iNumCitiesRazed = CyStatistics().getPlayerNumCitiesRazed(self.iActivePlayer)

		aiUnitsBuilt = []
		for i in range(gc.getNumUnitInfos()):
			aiUnitsBuilt.append(CyStatistics().getPlayerNumUnitsBuilt(self.iActivePlayer, i))

		aiUnitsKilled = []
		for i in range(gc.getNumUnitInfos()):
			aiUnitsKilled.append(CyStatistics().getPlayerNumUnitsKilled(self.iActivePlayer, i))

		aiUnitsLost = []
		for i in range(gc.getNumUnitInfos()):
			aiUnitsLost.append(CyStatistics().getPlayerNumUnitsLost(self.iActivePlayer, i))

		aiBuildingsBuilt = []
		for iBuildingLoop in range(iNumBuildings):
			aiBuildingsBuilt.append(CyStatistics().getPlayerNumBuildingsBuilt(self.iActivePlayer, iBuildingLoop))

		aiUnitsCurrent = []
		for i in range(gc.getNumUnitInfos()):
			aiUnitsCurrent.append(0)

		player = gc.getPlayer(self.iActivePlayer)
		(loopUnit, iter) = player.firstUnit()
		while(loopUnit):
			iType = loopUnit.getUnitType()
			aiUnitsCurrent[iType] += 1
			(loopUnit, iter) = player.nextUnit(iter)

		for i in range(player.getNumEuropeUnits()):
			loopUnit = player.getEuropeUnit(i)
			iType = loopUnit.getUnitType()
			aiUnitsCurrent[iType] += 1
			
		(city, iter) = player.firstCity(True)
		while(city):
			for iCitizen in range(city.getPopulation()):
				Unit = city.getPopulationUnitByIndex(iCitizen)
				iType = Unit.getUnitType()
				aiUnitsCurrent[iType] += 1
			(city, iter) = player.nextCity(iter, True)

################################################### TOP PANEL ###################################################

		# Add Panel
		szTopPanelWidget = self.getNextWidgetName()
		screen.addPanel( szTopPanelWidget, u"", u"", True, False, (self.W_SCREEN / 2) - (self.W_STATS_TOP_PANEL / 2), self.Y_STATS_TOP_PANEL, self.W_STATS_TOP_PANEL, self.H_STATS_TOP_PANEL, PanelStyles.PANEL_STYLE_DAWNTOP, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		# Leaderhead graphic
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		szLeaderWidget = self.getNextWidgetName()
		screen.addLeaderheadGFC(szLeaderWidget, player.getLeaderType(), AttitudeTypes.ATTITUDE_PLEASED,
		self.X_LEADER_ICON, self.Y_LEADER_ICON, self.W_LEADER_ICON, self.H_LEADER_ICON, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# Leader Name
		self.szLeaderNameWidget = self.getNextWidgetName()
		szText = u"<font=4b>" + gc.getPlayer(self.iActivePlayer).getName() + u"</font>"
		screen.setText(self.szLeaderNameWidget, "", szText, CvUtil.FONT_LEFT_JUSTIFY, (self.W_SCREEN / 2) - (self.W_STATS_TOP_CHART / 2), self.Y_LEADER_NAME, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# Create Table
		szTopChart = self.getNextWidgetName()
		screen.addTableControlGFC(szTopChart, self.iNumTopChartCols, (self.W_SCREEN / 2) - (self.W_STATS_TOP_CHART / 2), self.Y_STATS_TOP_CHART, self.W_STATS_TOP_CHART, self.H_STATS_TOP_CHART, False, True, 32,32, TableStyles.TABLE_STYLE_STANDARD)

		# Add Columns
		screen.setTableColumnHeader(szTopChart, 0, "", self.STATS_TOP_CHART_W_COL_0)
		screen.setTableColumnHeader(szTopChart, 1, "", self.STATS_TOP_CHART_W_COL_1)

		# Add Rows
		for i in range(self.iNumTopChartRows - 1):
			screen.appendTableRow(szTopChart)
		iNumRows = screen.getTableNumRows(szTopChart)

		# Graph itself
		iRow = 0
		iCol = 0
		screen.setTableText(szTopChart, iCol, iRow, self.TEXT_TIME_PLAYED, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		iCol = 1
		screen.setTableText(szTopChart, iCol, iRow, szTimeString, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

		iRow = 1
		iCol = 0
		screen.setTableText(szTopChart, iCol, iRow, self.TEXT_CITIES_BUILT, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		iCol = 1
		screen.setTableText(szTopChart, iCol, iRow, str(iNumCitiesBuilt), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

		iRow = 2
		iCol = 0
		screen.setTableText(szTopChart, iCol, iRow, self.TEXT_CITIES_RAZED, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		iCol = 1
		screen.setTableText(szTopChart, iCol, iRow, str(iNumCitiesRazed), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)


################################################### BOTTOM PANEL ###################################################

		# Create Tables
		szUnitsTable = self.getNextWidgetName()
		screen.addTableControlGFC(szUnitsTable, self.iNumUnitStatsChartCols, (self.W_SCREEN / 2) - ((self.W_STATS_BOTTOM_CHART_UNITS + self.W_STATS_BOTTOM_CHART_BUILDINGS) / 2), self.Y_STATS_BOTTOM_CHART, self.W_STATS_BOTTOM_CHART_UNITS, self.H_STATS_BOTTOM_CHART, True, True, 32,32, TableStyles.TABLE_STYLE_STANDARD)
		screen.enableSort(szUnitsTable)

		szBuildingsTable = self.getNextWidgetName()
		screen.addTableControlGFC(szBuildingsTable, self.iNumBuildingStatsChartCols, (self.W_SCREEN / 2) - ((self.W_STATS_BOTTOM_CHART_UNITS + self.W_STATS_BOTTOM_CHART_BUILDINGS) /2) + self.W_STATS_BOTTOM_CHART_UNITS, self.Y_STATS_BOTTOM_CHART, self.W_STATS_BOTTOM_CHART_BUILDINGS, self.H_STATS_BOTTOM_CHART, True, True, 32,32, TableStyles.TABLE_STYLE_STANDARD)
		screen.enableSort(szBuildingsTable)


		# Reducing the width a bit to leave room for the vertical scrollbar, preventing a horizontal scrollbar from also being created
		iChartWidth = self.W_STATS_BOTTOM_CHART_UNITS + self.W_STATS_BOTTOM_CHART_BUILDINGS - 24

		# Add Columns
		iColWidth = int((iChartWidth / 12 * 3))
		screen.setTableColumnHeader(szUnitsTable, 0, self.TEXT_UNITS, iColWidth)
		iColWidth = int((iChartWidth / 12 * 1))
		screen.setTableColumnHeader(szUnitsTable, 1, self.TEXT_CURRENT, iColWidth)
		iColWidth = int((iChartWidth / 12 * 1))
		screen.setTableColumnHeader(szUnitsTable, 2, self.TEXT_BUILT, iColWidth)
		iColWidth = int((iChartWidth / 12 * 1))
		screen.setTableColumnHeader(szUnitsTable, 3, self.TEXT_KILLED, iColWidth)
		iColWidth = int((iChartWidth / 12 * 1))
		screen.setTableColumnHeader(szUnitsTable, 4, self.TEXT_LOST, iColWidth)
		iColWidth = int((iChartWidth / 12 * 4))
		screen.setTableColumnHeader(szBuildingsTable, 0, self.TEXT_BUILDINGS, iColWidth)
		iColWidth = int((iChartWidth / 12 * 1))
		screen.setTableColumnHeader(szBuildingsTable, 1, self.TEXT_BUILT, iColWidth)

		# Add Rows
		for i in range(self.iNumUnitStatsChartRows):
			screen.appendTableRow(szUnitsTable)
		iNumUnitRows = screen.getTableNumRows(szUnitsTable)

		for i in range(self.iNumBuildingStatsChartRows):
			screen.appendTableRow(szBuildingsTable)
		iNumBuildingRows = screen.getTableNumRows(szBuildingsTable)

		# Add Units to table
		iCounter = 0
		for iUnitLoop in ShowUnits:
			iRow = iCounter
			
			iCol = 0
			szUnitName = gc.getUnitInfo(iUnitLoop).getDescription()
			screen.setTableText(szUnitsTable, iCol, iRow, szUnitName, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

			iCol = 1
			iNumUnitsCurrent = aiUnitsCurrent[iUnitLoop]
			screen.setTableInt(szUnitsTable, iCol, iRow, str(iNumUnitsCurrent), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

			iCol = 2
			iNumUnitsBuilt = aiUnitsBuilt[iUnitLoop]
			screen.setTableInt(szUnitsTable, iCol, iRow, str(iNumUnitsBuilt), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

			iCol = 3
			iNumUnitsKilled = aiUnitsKilled[iUnitLoop]
			screen.setTableInt(szUnitsTable, iCol, iRow, str(iNumUnitsKilled), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

			iCol = 4
			iNumUnitsLost = aiUnitsLost[iUnitLoop]
			screen.setTableInt(szUnitsTable, iCol, iRow, str(iNumUnitsLost), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
			
			iCounter += 1
			
		# Add Buildings to table
		for iBuildingLoop in range(iNumBuildings):
			iRow = iBuildingLoop

			if (not gc.getBuildingInfo(iBuildingLoop).isGraphicalOnly()):
				iCol = 0
				szBuildingName = gc.getBuildingInfo(iBuildingLoop).getDescription()
				screen.setTableText(szBuildingsTable, iCol, iRow, szBuildingName, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
				iCol = 1
				iNumBuildingsBuilt = aiBuildingsBuilt[iBuildingLoop]
				screen.setTableInt(szBuildingsTable, iCol, iRow, str(iNumBuildingsBuilt), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

##################################################### OTHER #################################################

	def drawLine (self, screen, canvas, x0, y0, x1, y1, color):
		screen.addLineGFC(canvas, self.getNextLineName(), x0, y0 + 1, x1, y1 + 1, color)
		screen.addLineGFC(canvas, self.getNextLineName(), x0 + 1, y0, x1 + 1, y1, color)
		screen.addLineGFC(canvas, self.getNextLineName(), x0, y0, x1, y1, color)

	def getTurnDate(self,turn):

		year = CyGame().getTurnYear(turn)

		if (year < 0):
			return localText.getText("TXT_KEY_TIME_BC", (-year,))
		else:
			return localText.getText("TXT_KEY_TIME_AD", (year,))

	def lineName(self,i):
		return self.LINE_ID + str(i)

	def getNextLineName(self):
		name = self.lineName(self.nLineCount)
		self.nLineCount += 1
		return name

	# returns a unique ID for a widget in this screen
	def getNextWidgetName(self):
		szName = self.WIDGET_ID + str(self.nWidgetCount)
		self.nWidgetCount += 1
		return szName

	def deleteAllLines(self):
		screen = self.getScreen()
		i = 0
		while (i < self.nLineCount):
			screen.deleteWidget(self.lineName(i))
			i += 1
		self.nLineCount = 0

	def deleteAllWidgets(self, iNumPermanentWidgets = 0):
		self.deleteAllLines()
		screen = self.getScreen()
		i = self.nWidgetCount - 1
		while (i >= iNumPermanentWidgets):
			self.nWidgetCount = i
			screen.deleteWidget(self.getNextWidgetName())
			i -= 1

		self.nWidgetCount = iNumPermanentWidgets
		self.yMessage = 5

	# handle the input for this screen...
	def handleInput (self, inputClass):

		screen = self.getScreen()
		szWidgetName = inputClass.getFunctionName() + str(inputClass.getID())
		code = inputClass.getNotifyCode()

		# Slide graph
		if (szWidgetName == self.graphLeftButtonID and code == NotifyCode.NOTIFY_CLICKED):
			self.slideGraph(- 2 * self.graphZoom / 5)
			self.drawGraph()

		elif (szWidgetName == self.graphRightButtonID and code == NotifyCode.NOTIFY_CLICKED):
			self.slideGraph(2 * self.graphZoom / 5)
			self.drawGraph()

		# Dropdown Box/ ListBox
		if (code == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED):

			# Debug dropdown
			if (inputClass.getFunctionName() == self.DEBUG_DROPDOWN_ID):
				iIndex = screen.getSelectedPullDownID(self.DEBUG_DROPDOWN_ID)
				self.iActivePlayer = screen.getPullDownData(self.DEBUG_DROPDOWN_ID, iIndex)

				self.pActivePlayer = gc.getPlayer(self.iActivePlayer)
				self.iActiveTeam = self.pActivePlayer.getTeam()
				self.pActiveTeam = gc.getTeam(self.iActiveTeam)

				# Determine who this active player knows
				self.aiPlayersMet = []

				self.iNumPlayersMet = 0
				for iLoopPlayer in range(gc.getMAX_CIV_PLAYERS()):
					pLoopPlayer = gc.getPlayer(iLoopPlayer)
					iLoopPlayerTeam = pLoopPlayer.getTeam()
					if (self.pActiveTeam.isHasMet(iLoopPlayerTeam)):
						self.aiPlayersMet.append(iLoopPlayer)
						self.iNumPlayersMet += 1
				self.redrawContents()

			iSelected = inputClass.getData()

################################## GRAPH TAB ###################################

			if (self.iActiveTab == self.iGraphID):

				# Graph dropdown to select what values are being graphed
				if (szWidgetName == self.szGraphDropdownWidget):

					if (iSelected == 0):
						self.iGraphTabID = self.TOTAL_SCORE

					elif (iSelected == 1):
						self.iGraphTabID = self.ECONOMY_SCORE

					elif (iSelected == 2):
						self.iGraphTabID = self.INDUSTRY_SCORE

					elif (iSelected == 3):
						self.iGraphTabID = self.AGRICULTURE_SCORE

					elif (iSelected == 4):
						self.iGraphTabID = self.POWER_SCORE

					elif (iSelected == 5):
						self.iGraphTabID = self.CULTURE_SCORE

					self.drawGraph()

				elif (szWidgetName == self.szTurnsDropdownWidget):

					self.zoomGraph(self.dropDownTurns[iSelected])
					self.drawGraph()

		# Something Clicked
		elif (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if (inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL and inputClass.getData1() == 1111):
				self.iActiveTab = inputClass.getData2()
				self.reset()
				self.redrawContents()

			elif (inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL and inputClass.getData1() == 2222):
				playerIndex = inputClass.getData2()
				self.aiPlayerToggle[playerIndex] = not self.aiPlayerToggle[playerIndex]
				self.drawLegend()
				self.drawGraph()

		return 0

	def update(self, fDelta):

		return
