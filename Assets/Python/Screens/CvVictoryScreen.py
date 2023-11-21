## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import time
import CvScreenEnums

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

VICTORY_CONDITION_SCREEN = 0
GAME_SETTINGS_SCREEN = 1

class CvVictoryScreen:
	"Keeps track of victory conditions"

	def __init__(self):
		self.SCREEN_NAME = "VictoryScreen"
		self.DEBUG_DROPDOWN_ID =  "VictoryScreenDropdownWidget"
		self.EXIT_AREA = "EXIT"
		self.EXIT_ID = "VictoryScreenExit"
		self.BACKGROUND_ID = "VictoryScreenBackground"
		self.HEADER_ID = "VictoryScreenHeader"
		self.WIDGET_ID = "VictoryScreenWidget"
		self.VC_TAB_ID = 0
		self.SETTINGS_TAB_ID = 1

		self.Z_BACKGROUND = -6.1
		self.Z_CONTROLS = self.Z_BACKGROUND - 0.2
		self.DZ = -0.2

		self.Y_TITLE = 12

		self.X_EXIT = 994
		self.Y_EXIT = 726

		self.W_AREA = 1010
		self.H_AREA = 550

		self.TABLE_WIDTH_0 = 350
		self.TABLE_WIDTH_1 = 80
		self.TABLE_WIDTH_2 = 180
		self.TABLE_WIDTH_3 = 100
		self.TABLE_WIDTH_4 = 180
		self.TABLE_WIDTH_5 = 100

		self.TABLE2_WIDTH_0 = 740
		self.TABLE2_WIDTH_1 = 265

		self.X_LINK = 100
		self.DX_LINK = 220
		self.Y_LINK = 726
		self.MARGIN = 20
		
		self.SETTINGS_PANEL_Y = 150
		
		self.SETTINGS_PANEL_WIDTH = 300
		self.SETTINGS_PANEL_HEIGHT = 500

		self.nWidgetCount = 0
		self.iActivePlayer = -1

		self.iScreen = VICTORY_CONDITION_SCREEN

	def getScreen(self):
		return CyGInterfaceScreen("VictoryScreen", CvScreenEnums.VICTORY_SCREEN)

	def hideScreen(self):
		screen = self.getScreen()
		screen.hideScreen()

	def interfaceScreen(self):

		# Create a new screen
		screen = self.getScreen()
		if screen.isActive():
			return
			
		screen = self.getScreen()
		self.W_SCREEN = screen.getXResolution()
		self.H_SCREEN = screen.getYResolution()

		self.X_AREA = (self.W_SCREEN / 2) - (self.W_AREA / 2)
		self.Y_AREA = (self.H_SCREEN / 2) - (self.H_AREA / 2)

		self.SETTINGS_PANEL_X1 = self.X_AREA
		self.SETTINGS_PANEL_X2 = self.X_AREA + 355
		self.SETTINGS_PANEL_X3 = self.X_AREA + 710

		screen.setRenderInterfaceOnly(True);
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		self.iActivePlayer = CyGame().getActivePlayer()
		if self.iScreen == -1:
			self.iScreen = VICTORY_CONDITION_SCREEN

		# Set the background widget and exit button
		screen.addDDSGFC(self.BACKGROUND_ID, ArtFileMgr.getInterfaceArtInfo("MAINMENU_SLIDESHOW_LOAD").getPath(), 0, 0, self.W_SCREEN, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDDSGFC("TopPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TITLE").getPath(), 0, 0, self.W_SCREEN, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.showWindowBackground( False )
		screen.setDimensions(0, 0, self.W_SCREEN, self.H_SCREEN)

		# Header...
		self.SCREEN_TITLE = u"<font=4b>" + localText.getText("TXT_KEY_VICTORY_SCREEN_TITLE", ()).upper() + u"</font>"
		self.SCREEN_TITLE = localText.changeTextColor(self.SCREEN_TITLE, gc.getInfoTypeForString("COLOR_FONT_CREAM"))

		screen.setLabel(self.HEADER_ID, "Background", self.SCREEN_TITLE, CvUtil.FONT_CENTER_JUSTIFY, self.W_SCREEN / 2, 4, self.Z_CONTROLS, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		if self.iScreen == VICTORY_CONDITION_SCREEN:
			self.showVictoryConditionScreen()
		elif self.iScreen == GAME_SETTINGS_SCREEN:
			self.showGameSettingsScreen()

	def drawTabs(self):

		screen = self.getScreen()

		Tabs = [localText.getObjectText("TXT_KEY_MAIN_MENU_VICTORIES", 0), localText.getObjectText("TXT_KEY_MAIN_MENU_SETTINGS", 0)]
		NumTabs = len(Tabs)
		TabWidth = self.W_SCREEN / (NumTabs + 1)

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

		screen.addPanel("ExitTab", "", "", False, False, TabWidth * NumTabs, self.H_SCREEN - BottomPanelHight, TabWidth, BottomPanelHight, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addDrawControl("ExitTab", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_OFF").getPath(), TabWidth * NumTabs, self.H_SCREEN - BottomPanelHight, TabWidth, BottomPanelHight, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setTextAt("ExitTabTitle", "ExitTab", u"<font=4>" + CyTranslator().getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + "</font>", CvUtil.FONT_RIGHT_JUSTIFY, TabWidth - 30 , BottomPanelHight - 18, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)

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

	def showGameSettingsScreen(self):

		self.deleteAllWidgets()
		screen = self.getScreen()

		activePlayer = gc.getPlayer(self.iActivePlayer)

		szSettingsPanel = self.getNextWidgetName()
		screen.addPanel(szSettingsPanel, localText.getText("TXT_KEY_MAIN_MENU_SETTINGS", ()).upper(), "", True, True, self.SETTINGS_PANEL_X1, self.SETTINGS_PANEL_Y - 10, self.SETTINGS_PANEL_WIDTH, self.SETTINGS_PANEL_HEIGHT, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1)
		szSettingsTable = self.getNextWidgetName()
		screen.addListBoxGFC(szSettingsTable, "", self.SETTINGS_PANEL_X1 + self.MARGIN, self.SETTINGS_PANEL_Y + self.MARGIN, self.SETTINGS_PANEL_WIDTH - 2*self.MARGIN, self.SETTINGS_PANEL_HEIGHT - 2*self.MARGIN, TableStyles.TABLE_STYLE_EMPTY)
		screen.enableSelect(szSettingsTable, False)

		screen.appendListBoxStringNoUpdate(szSettingsTable, localText.getText("TXT_KEY_LEADER_CIV_DESCRIPTION", (activePlayer.getNameKey(), activePlayer.getCivilizationShortDescriptionKey())), WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
		screen.appendListBoxStringNoUpdate(szSettingsTable, u"     (" + CyGameTextMgr().parseLeaderTraits(activePlayer.getLeaderType(), activePlayer.getCivilizationType(), True, False) + ")", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
		screen.appendListBoxStringNoUpdate(szSettingsTable, " ", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
		screen.appendListBoxStringNoUpdate(szSettingsTable, localText.getText("TXT_KEY_SETTINGS_DIFFICULTY", (gc.getHandicapInfo(activePlayer.getHandicapType()).getTextKey(), )), WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
		screen.appendListBoxStringNoUpdate(szSettingsTable, " ", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
		screen.appendListBoxStringNoUpdate(szSettingsTable, gc.getMap().getMapScriptName(), WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
		screen.appendListBoxStringNoUpdate(szSettingsTable, localText.getText("TXT_KEY_SETTINGS_MAP_SIZE", (gc.getWorldInfo(gc.getMap().getWorldSize()).getTextKey(), )), WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
		screen.appendListBoxStringNoUpdate(szSettingsTable, localText.getText("TXT_KEY_SETTINGS_CLIMATE", (gc.getClimateInfo(gc.getMap().getClimate()).getTextKey(), )), WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
		screen.appendListBoxStringNoUpdate(szSettingsTable, localText.getText("TXT_KEY_SETTINGS_SEA_LEVEL", (gc.getSeaLevelInfo(gc.getMap().getSeaLevel()).getTextKey(), )), WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
		screen.appendListBoxStringNoUpdate(szSettingsTable, " ", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
		screen.appendListBoxStringNoUpdate(szSettingsTable, localText.getText("TXT_KEY_SETTINGS_STARTING_ERA", (gc.getEraInfo(gc.getGame().getStartEra()).getTextKey(), )), WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
		screen.appendListBoxStringNoUpdate(szSettingsTable, localText.getText("TXT_KEY_SETTINGS_GAME_SPEED", (gc.getGameSpeedInfo(gc.getGame().getGameSpeedType()).getTextKey(), )), WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )

		screen.updateListBox(szSettingsTable)

		szOptionsPanel = self.getNextWidgetName()
		screen.addPanel(szOptionsPanel, localText.getText("TXT_KEY_MAIN_MENU_CUSTOM_SETUP_OPTIONS", ()).upper(), "", True, True, self.SETTINGS_PANEL_X2, self.SETTINGS_PANEL_Y - 10, self.SETTINGS_PANEL_WIDTH, self.SETTINGS_PANEL_HEIGHT, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1)
		szOptionsTable = self.getNextWidgetName()
		screen.addListBoxGFC(szOptionsTable, "", self.SETTINGS_PANEL_X2 + self.MARGIN, self.SETTINGS_PANEL_Y + self.MARGIN, self.SETTINGS_PANEL_WIDTH - 2*self.MARGIN, self.SETTINGS_PANEL_HEIGHT - 2*self.MARGIN, TableStyles.TABLE_STYLE_EMPTY)
		screen.enableSelect(szOptionsTable, False)

		for i in range(GameOptionTypes.NUM_GAMEOPTION_TYPES):
			if gc.getGame().isOption(i):
				screen.appendListBoxStringNoUpdate(szOptionsTable, gc.getGameOptionInfo(i).getDescription(), WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

		if (gc.getGame().isOption(GameOptionTypes.GAMEOPTION_ADVANCED_START)):
			szNumPoints = u"%s %d" % (localText.getText("TXT_KEY_ADVANCED_START_POINTS", ()), gc.getGame().getNumAdvancedStartPoints())
			screen.appendListBoxStringNoUpdate(szOptionsTable, szNumPoints, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

		if (gc.getGame().isGameMultiPlayer()):
			for i in range(gc.getNumMPOptionInfos()):
				if (gc.getGame().isMPOption(i)):
					szText = gc.getMPOptionInfo(i).getDescription()
					if (i == MultiplayerOptionTypes.MPOPTION_TURN_TIMER and gc.getGame().getTurnTimerType() != TurnTimerTypes.NO_TURNTIMER):
						szText += u" (%s)" % (gc.getTurnTimerInfo(gc.getGame().getTurnTimerType()).getDescription(),)
					screen.appendListBoxStringNoUpdate(szOptionsTable, szText, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

			if (gc.getGame().getMaxTurns() > 0):
				szMaxTurns = u"%s %d" % (localText.getText("TXT_KEY_TURN_LIMIT_TAG", ()), gc.getGame().getMaxTurns())
				screen.appendListBoxStringNoUpdate(szOptionsTable, szMaxTurns, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

			if (gc.getGame().getMaxCityElimination() > 0):
				szMaxCityElimination = u"%s %d" % (localText.getText("TXT_KEY_CITY_ELIM_TAG", ()), gc.getGame().getMaxCityElimination())
				screen.appendListBoxStringNoUpdate(szOptionsTable, szMaxCityElimination, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

		screen.updateListBox(szOptionsTable)
		szCivsPanel = self.getNextWidgetName()
		screen.addPanel(szCivsPanel, localText.getText("TXT_KEY_RIVALS_MET", ()).upper(), "", True, True, self.SETTINGS_PANEL_X3, self.SETTINGS_PANEL_Y - 10, self.SETTINGS_PANEL_WIDTH, self.SETTINGS_PANEL_HEIGHT, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1)

		szCivsTable = self.getNextWidgetName()
		screen.addListBoxGFC(szCivsTable, "", self.SETTINGS_PANEL_X3 + self.MARGIN, self.SETTINGS_PANEL_Y + self.MARGIN, self.SETTINGS_PANEL_WIDTH - 2*self.MARGIN, self.SETTINGS_PANEL_HEIGHT - 2*self.MARGIN, TableStyles.TABLE_STYLE_EMPTY)
		screen.enableSelect(szCivsTable, False)

		for iLoopPlayer in range(gc.getMAX_CIV_PLAYERS()):
			player = gc.getPlayer(iLoopPlayer)
			if (player.isEverAlive() and iLoopPlayer != self.iActivePlayer and not player.isEurope() and (gc.getTeam(player.getTeam()).isHasMet(activePlayer.getTeam()) or gc.getGame().isDebugMode())):
				screen.appendListBoxStringNoUpdate(szCivsTable, localText.getText("TXT_KEY_LEADER_CIV_DESCRIPTION", (player.getNameKey(), player.getCivilizationShortDescriptionKey())), WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				screen.appendListBoxStringNoUpdate(szCivsTable, u"     (" + CyGameTextMgr().parseLeaderTraits(player.getLeaderType(), player.getCivilizationType(), True, False) + ")", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				screen.appendListBoxStringNoUpdate(szCivsTable, " ", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )

		screen.updateListBox(szCivsTable)

		self.drawTabs()
		self.setTab(GAME_SETTINGS_SCREEN, True)


	def showVictoryConditionScreen(self):

		activePlayer = gc.getPlayer(self.iActivePlayer)
		iActiveTeam = activePlayer.getTeam()
		activeTeam = gc.getTeam(iActiveTeam)

		# Conquest
		nRivals = -1
		for i in range(gc.getMAX_CIV_TEAMS()):
			if (gc.getTeam(i).isAlive()):
				nRivals += 1

		# Population
		totalPop = gc.getGame().getTotalPopulation()
		ourPop = activeTeam.getTotalPopulation()
		if (totalPop > 0):
			popPercent = (ourPop * 100.0) / totalPop
		else:
			popPercent = 0.0

		iBestPopTeam = -1
		bestPop = 0
		for iLoopTeam in range(gc.getMAX_CIV_TEAMS()):
			if (gc.getTeam(iLoopTeam).isAlive()):
				if (iLoopTeam != iActiveTeam and (activeTeam.isHasMet(iLoopTeam) or gc.getGame().isDebugMode())):
					teamPop = gc.getTeam(iLoopTeam).getTotalPopulation()
					if (teamPop > bestPop):
						bestPop = teamPop
						iBestPopTeam = iLoopTeam

		# Score
		ourScore = gc.getGame().getTeamScore(iActiveTeam)

		iBestScoreTeam = -1
		bestScore = 0
		for iLoopTeam in range(gc.getMAX_CIV_TEAMS()):
			if (gc.getTeam(iLoopTeam).isAlive()):
				if (iLoopTeam != iActiveTeam and (activeTeam.isHasMet(iLoopTeam) or gc.getGame().isDebugMode())):
					teamScore = gc.getGame().getTeamScore(iLoopTeam)
					if (teamScore > bestScore):
						bestScore = teamScore
						iBestScoreTeam = iLoopTeam

		# Land Area
		totalLand = gc.getMap().getLandPlots()
		ourLand = activeTeam.getTotalLand()
		if (totalLand > 0):
			landPercent = (ourLand * 100.0) / totalLand
		else:
			landPercent = 0.0

		iBestLandTeam = -1
		bestLand = 0
		for iLoopTeam in range(gc.getMAX_CIV_TEAMS()):
			if (gc.getTeam(iLoopTeam).isAlive()):
				if (iLoopTeam != iActiveTeam and (activeTeam.isHasMet(iLoopTeam) or gc.getGame().isDebugMode())):
					teamLand = gc.getTeam(iLoopTeam).getTotalLand()
					if (teamLand > bestLand):
						bestLand = teamLand
						iBestLandTeam = iLoopTeam

		# Total Culture
		ourCulture = activeTeam.countTotalCulture()

		iBestCultureTeam = -1
		bestCulture = 0
		for iLoopTeam in range(gc.getMAX_CIV_TEAMS()):
			if (gc.getTeam(iLoopTeam).isAlive()):
				if (iLoopTeam != iActiveTeam and (activeTeam.isHasMet(iLoopTeam) or gc.getGame().isDebugMode())):
					teamCulture = gc.getTeam(iLoopTeam).countTotalCulture()
					if (teamCulture > bestCulture):
						bestCulture = teamCulture
						iBestCultureTeam = iLoopTeam
		
		# Rebel Sentiment
		ourRebel = activeTeam.getRebelPercent()

		iBestRebelTeam = -1
		bestRebel = 0
		for iLoopTeam in range(gc.getMAX_CIV_TEAMS()):
			if (gc.getTeam(iLoopTeam).isAlive()):
				if (iLoopTeam != iActiveTeam and (activeTeam.isHasMet(iLoopTeam) or gc.getGame().isDebugMode())):
					teamRebel = gc.getTeam(iLoopTeam).getRebelPercent()
					if (teamRebel > bestRebel):
						bestRebel = teamRebel
						iBestRebelTeam = iLoopTeam

		self.deleteAllWidgets()
		screen = self.getScreen()

		# Start filling in the table below
		screen.addPanel(self.getNextWidgetName(), "", "", False, False, self.X_AREA-10, self.Y_AREA-15, self.W_AREA+20, self.H_AREA+30, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1)
		szTable = self.getNextWidgetName()
		screen.addTableControlGFC(szTable, 6, self.X_AREA, self.Y_AREA, self.W_AREA, self.H_AREA, False, False, 32,32, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader(szTable, 0, "", self.TABLE_WIDTH_0)
		screen.setTableColumnHeader(szTable, 1, "", self.TABLE_WIDTH_1)
		screen.setTableColumnHeader(szTable, 2, "", self.TABLE_WIDTH_2)
		screen.setTableColumnHeader(szTable, 3, "", self.TABLE_WIDTH_3)
		screen.setTableColumnHeader(szTable, 4, "", self.TABLE_WIDTH_4)
		screen.setTableColumnHeader(szTable, 5, "", self.TABLE_WIDTH_5)
		screen.appendTableRow(szTable)

		for iLoopVC in range(gc.getNumVictoryInfos()):
			victory = gc.getVictoryInfo(iLoopVC)
			if gc.getGame().isVictoryValid(iLoopVC):

				iNumRows = screen.getTableNumRows(szTable)
				szVictoryType = u"<font=4b>" + victory.getDescription().upper() + u"</font>"
				if ((victory.isEndScore() or victory.isEndEurope()) and (gc.getGame().getMaxTurns() > gc.getGame().getElapsedGameTurns())):
					szVictoryType += "    (" + localText.getText("TXT_KEY_MISC_TURNS_LEFT", (gc.getGame().getMaxTurns() - gc.getGame().getElapsedGameTurns(), )) + ")"
					
				iVictoryTitleRow = iNumRows - 1
				screen.setTableText(szTable, 0, iVictoryTitleRow, szVictoryType, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

				bEntriesFound = False

				if (victory.isTargetScore() and gc.getGame().getTargetScore() != 0):

					iRow = screen.appendTableRow(szTable)
					screen.setTableText(szTable, 0, iRow, localText.getText("TXT_KEY_VICTORY_SCREEN_TARGET_SCORE", (gc.getGame().getTargetScore(), )), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					screen.setTableText(szTable, 2, iRow, activeTeam.getName() + ":", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					screen.setTableText(szTable, 3, iRow, (u"%d" % ourScore), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

					if (iBestScoreTeam != -1):
						screen.setTableText(szTable, 4, iRow, gc.getTeam(iBestScoreTeam).getName() + ":", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
						screen.setTableText(szTable, 5, iRow, (u"%d" % bestScore), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

					bEntriesFound = True

				if (victory.isEndScore()):

					szText1 = localText.getText("TXT_KEY_VICTORY_SCREEN_HIGHEST_SCORE", (CyGameTextMgr().getTimeStr(gc.getGame().getStartTurn() + gc.getGame().getMaxTurns(), False), ))

					iRow = screen.appendTableRow(szTable)
					screen.setTableText(szTable, 0, iRow, szText1, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					screen.setTableText(szTable, 2, iRow, activeTeam.getName() + ":", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					screen.setTableText(szTable, 3, iRow, (u"%d" % ourScore), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

					if (iBestScoreTeam != -1):
						screen.setTableText(szTable, 4, iRow, gc.getTeam(iBestScoreTeam).getName() + ":", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
						screen.setTableText(szTable, 5, iRow, (u"%d" % bestScore), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

					bEntriesFound = True
					
				if (victory.isEndEurope()):
				
					bEntriesFound = True

				if (victory.isConquest()):
					iRow = screen.appendTableRow(szTable)
					screen.setTableText(szTable, 0, iRow, localText.getText("TXT_KEY_VICTORY_SCREEN_ELIMINATE_ALL", ()), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					screen.setTableText(szTable, 2, iRow, localText.getText("TXT_KEY_VICTORY_SCREEN_RIVALS_LEFT", ()), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					screen.setTableText(szTable, 3, iRow, unicode(nRivals), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					bEntriesFound = True

				if (gc.getGame().getAdjustedPopulationPercent(iLoopVC) > 0):
					iRow = screen.appendTableRow(szTable)
					screen.setTableText(szTable, 0, iRow, localText.getText("TXT_KEY_VICTORY_SCREEN_PERCENT_POP", (gc.getGame().getAdjustedPopulationPercent(iLoopVC), )), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					screen.setTableText(szTable, 2, iRow, activeTeam.getName() + ":", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					screen.setTableText(szTable, 3, iRow, (u"%.2f%%" % popPercent), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					if (iBestPopTeam != -1):
						screen.setTableText(szTable, 4, iRow, gc.getTeam(iBestPopTeam).getName() + ":", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
						screen.setTableText(szTable, 5, iRow, (u"%.2f%%" % (bestPop * 100 / totalPop)), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					bEntriesFound = True


				if (gc.getGame().getAdjustedLandPercent(iLoopVC) > 0):
					iRow = screen.appendTableRow(szTable)
					screen.setTableText(szTable, 0, iRow, localText.getText("TXT_KEY_VICTORY_SCREEN_PERCENT_LAND", (gc.getGame().getAdjustedLandPercent(iLoopVC), )), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					screen.setTableText(szTable, 2, iRow, activeTeam.getName() + ":", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					screen.setTableText(szTable, 3, iRow, (u"%.2f%%" % landPercent), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					if (iBestLandTeam != -1):
						screen.setTableText(szTable, 4, iRow, gc.getTeam(iBestLandTeam).getName() + ":", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
						screen.setTableText(szTable, 5, iRow, (u"%.2f%%" % (bestLand * 100 / totalLand)), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					bEntriesFound = True

				if (victory.getTotalCultureRatio() > 0):
					iRow = screen.appendTableRow(szTable)
					screen.setTableText(szTable, 0, iRow, localText.getText("TXT_KEY_VICTORY_SCREEN_PERCENT_CULTURE", (int((100.0 * bestCulture) / victory.getTotalCultureRatio()), )), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					screen.setTableText(szTable, 2, iRow, activeTeam.getName() + ":", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					screen.setTableText(szTable, 3, iRow, unicode(ourCulture), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					if (iBestLandTeam != -1):
						screen.setTableText(szTable, 4, iRow, gc.getTeam(iBestCultureTeam).getName() + ":", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
						screen.setTableText(szTable, 5, iRow, unicode(bestCulture), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					bEntriesFound = True

				iBestBuildingTeam = -1
				bestBuilding = 0
				for iLoopTeam in range(gc.getMAX_CIV_TEAMS()):
					if (gc.getTeam(iLoopTeam).isAlive()):
						if (iLoopTeam != iActiveTeam and (activeTeam.isHasMet(iLoopTeam) or gc.getGame().isDebugMode())):
							teamBuilding = 0
							for i in range(gc.getNumBuildingClassInfos()):
								if (gc.getBuildingClassInfo(i).getVictoryThreshold(iLoopVC) > 0):
									teamBuilding += gc.getTeam(iLoopTeam).getBuildingClassCount(i)
							if (teamBuilding > bestBuilding):
								bestBuilding = teamBuilding
								iBestBuildingTeam = iLoopTeam

				for i in range(gc.getNumBuildingClassInfos()):
					if (gc.getBuildingClassInfo(i).getVictoryThreshold(iLoopVC) > 0):
						iRow = screen.appendTableRow(szTable)
						szNumber = unicode(gc.getBuildingClassInfo(i).getVictoryThreshold(iLoopVC))
						screen.setTableText(szTable, 0, iRow, localText.getText("TXT_KEY_VICTORY_SCREEN_BUILDING", (szNumber, gc.getBuildingClassInfo(i).getTextKey())), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
						screen.setTableText(szTable, 2, iRow, activeTeam.getName() + ":", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
						screen.setTableText(szTable, 3, iRow, activeTeam.getBuildingClassCount(i), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
						if (iBestBuildingTeam != -1):
							screen.setTableText(szTable, 4, iRow, gc.getTeam(iBestBuildingTeam).getName() + ":", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
							screen.setTableText(szTable, 5, iRow, gc.getTeam(iBestBuildingTeam).getBuildingClassCount(i), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
						bEntriesFound = True

				if (victory.getCityCulture() != CultureLevelTypes.NO_CULTURELEVEL and victory.getNumCultureCities() > 0):
					ourBestCities = self.getListCultureCities(self.iActivePlayer)[0:victory.getNumCultureCities()]

					iBestCulturePlayer = -1
					bestCityCulture = 0
					maxCityCulture = (gc.getCultureLevelInfo(victory.getCityCulture()).getThreshold() * gc.getGameSpeedInfo(gc.getGame().getGameSpeedType()).getCulturePercent()) / 100
					for iLoopPlayer in range(gc.getMAX_PLAYERS()):
						if (gc.getPlayer(iLoopPlayer).isAlive()):
							if (iLoopPlayer != self.iActivePlayer and (activeTeam.isHasMet(gc.getPlayer(iLoopPlayer).getTeam()) or gc.getGame().isDebugMode())):
								theirBestCities = self.getListCultureCities(iLoopPlayer)[0:victory.getNumCultureCities()]

								iTotalCulture = 0
								for loopCity in theirBestCities:
									if loopCity[0] >= maxCityCulture:
										iTotalCulture += maxCityCulture
									else:
										iTotalCulture += loopCity[0]

								if (iTotalCulture >= bestCityCulture):
									bestCityCulture = iTotalCulture
									iBestCulturePlayer = iLoopPlayer

					if (iBestCulturePlayer != -1):
						theirBestCities = self.getListCultureCities(iBestCulturePlayer)[0:(victory.getNumCultureCities())]
					else:
						theirBestCities = []

					iRow = screen.appendTableRow(szTable)
					screen.setTableText(szTable, 0, iRow, localText.getText("TXT_KEY_VICTORY_SCREEN_CITY_CULTURE", (victory.getNumCultureCities(), gc.getCultureLevelInfo(victory.getCityCulture()).getTextKey())), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

					for i in range(victory.getNumCultureCities()):
						if (len(ourBestCities) > i):
							screen.setTableText(szTable, 2, iRow, ourBestCities[i][1].getName() + ":", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
							screen.setTableText(szTable, 3, iRow, str(ourBestCities[i][0]), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
						if (len(theirBestCities) > i):
							screen.setTableText(szTable, 4, iRow, theirBestCities[i][1].getName() + ":", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
							screen.setTableText(szTable, 5, iRow, unicode(theirBestCities[i][0]), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
						if (i < victory.getNumCultureCities()-1):
							iRow = screen.appendTableRow(szTable)
					bEntriesFound = True

				if (victory.isRevolution()):
					if activePlayer.getParent() != -1:
						iRow = screen.appendTableRow(szTable)
						screen.setTableText(szTable, 0, iRow, localText.getText("TXT_KEY_VICTORY_SCREEN_DEFEAT_PLAYER", (gc.getPlayer(activePlayer.getParent()).getNameKey(), )), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

						screen.setTableText(szTable, 2, iRow, activeTeam.getName() + ":", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
						screen.setTableText(szTable, 3, iRow, unicode(ourRebel) + u"%", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
						if (iBestRebelTeam != -1):
							screen.setTableText(szTable, 4, iRow, gc.getTeam(iBestRebelTeam).getName() + ":", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
							screen.setTableText(szTable, 5, iRow, unicode(bestRebel) + u"%", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
						bEntriesFound = True

# PatchMod: Victorys START
				if (victory.getTotalProductionRate() > 0):
					totalProduction = activeTeam.getTotalProductionRate()
					targetProduction = victory.getTotalProductionRate()
					iRow = screen.appendTableRow(szTable)
					screen.setTableText(szTable, 0, iRow, localText.getText("TXT_KEY_VICTORY_PRODUCTION_RATE", ()), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					screen.setTableText(szTable, 2, iRow, activeTeam.getName() + ":", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					screen.setTableText(szTable, 3, iRow, unicode(totalProduction), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					screen.setTableText(szTable, 4, iRow, localText.getText("TXT_KEY_VICTORY_PRODUCTION_OF", ()), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					screen.setTableText(szTable, 5, iRow, unicode(targetProduction), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					for iLoopTeam in range(gc.getMAX_CIV_TEAMS()):
						if (gc.getTeam(iLoopTeam).isAlive()):
					# CBM 0.7.012 display Europeans only - start
							# if (iLoopTeam != iActiveTeam and (activeTeam.isHasMet(iLoopTeam) or gc.getGame().isDebugMode())):
							if (iLoopTeam != iActiveTeam and (activeTeam.isHasMet(iLoopTeam) or gc.getGame().isDebugMode()) and not (gc.getPlayer(iLoopTeam).isNative())):
					# CBM 0.7.012 display Europeans only - end
								totalProduction = gc.getTeam(iLoopTeam).getTotalProductionRate()
								if (totalProduction > 0):
									iRow = screen.appendTableRow(szTable)
									screen.setTableText(szTable, 2, iRow, gc.getTeam(iLoopTeam).getName() + ":", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
									screen.setTableText(szTable, 3, iRow, unicode(totalProduction), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					bEntriesFound = True

# PatchMod: Victorys END
						
				if (bEntriesFound):
					screen.appendTableRow(szTable)
					screen.appendTableRow(szTable)

		# civ picker dropdown
		if (CyGame().isDebugMode()):
			self.szDropdownName = self.DEBUG_DROPDOWN_ID
			screen.addDropDownBoxGFC(self.szDropdownName, 22, 12, 300, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			for j in range(gc.getMAX_PLAYERS()):
				if (gc.getPlayer(j).isAlive()):
					screen.addPullDownString(self.szDropdownName, gc.getPlayer(j).getName(), j, j, False )

		self.drawTabs()
		self.setTab(VICTORY_CONDITION_SCREEN, True)

	def getListCultureCities(self, iPlayer):
		if iPlayer >= 0:
			player = gc.getPlayer(iPlayer)
			if player.isAlive():

				cityList = []
				(loopCity, iter) = player.firstCity(False)
				while(loopCity):
					cityList.append(loopCity)
					(loopCity, iter) = player.nextCity(iter, False)

				listCultureCities = len(cityList) * [(0, 0)]
				i = 0
				for city in cityList:
					listCultureCities[i] = (city.getCulture(iPlayer), city)
					i += 1
				listCultureCities.sort()
				listCultureCities.reverse()
				return listCultureCities
		return []


	# returns a unique ID for a widget in this screen
	def getNextWidgetName(self):
		szName = self.WIDGET_ID + str(self.nWidgetCount)
		self.nWidgetCount += 1
		return szName

	def deleteAllWidgets(self):
		screen = self.getScreen()
		i = self.nWidgetCount - 1
		while (i >= 0):
			self.nWidgetCount = i
			screen.deleteWidget(self.getNextWidgetName())
			i -= 1

		self.nWidgetCount = 0


	# handle the input for this screen...
	def handleInput (self, inputClass):
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED):
			if (inputClass.getFunctionName() == self.DEBUG_DROPDOWN_ID):
				szName = self.DEBUG_DROPDOWN_ID
				iIndex = self.getScreen().getSelectedPullDownID(szName)
				self.iActivePlayer = self.getScreen().getPullDownData(szName, iIndex)
				self.iScreen = VICTORY_CONDITION_SCREEN
				self.showVictoryConditionScreen()
		elif (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if (inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL and inputClass.getData1() == 1111):
				self.iScreen = inputClass.getData2()
				if (self.iScreen == VICTORY_CONDITION_SCREEN):
					self.showVictoryConditionScreen()
				elif(self.iScreen == GAME_SETTINGS_SCREEN):
					self.showGameSettingsScreen()
		
				for x in range(2):
					if (x == self.iScreen):
						self.setTab(x, True)
					else:
						self.setTab(x, False)

	def update(self, fDelta):
		return
