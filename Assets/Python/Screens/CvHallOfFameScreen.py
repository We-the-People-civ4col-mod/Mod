## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
from CvScreenEnums import *
import CvReplayScreen
import CvScreensInterface

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

SORT_BY_NORMALIZED_SCORE = 0
SORT_BY_FINISH_DATE = 1
SORT_BY_GAME_SCORE = 2

class CvHallOfFameScreen:
	"Top scores and more"

	def __init__(self, screenId):

		self.screenId = screenId
		self.SCREEN_NAME = "HallOfFameScreen"

		self.WIDGET_ID = "HallOfFameWidget"
		self.HEADER_ID = "HallOfFameHeaderWidget"
		self.EXIT_ID = "HallOfFameScreenExitWidget"
		self.REPLAY_ID = "HallOfFameScreenReplayWidget"
		self.BACKGROUND_ID = "HallOfFameScreenBackground"
		self.TABLE_ID = "HallOfFameScreenTable"
		self.LEADER_DROPDOWN_ID = "HallOfFameScreenLeaderDropdown"
		self.DIFFICULTY_DROPDOWN_ID = "HallOfFameScreenDifficultyDropdown"
		self.MAPSIZE_DROPDOWN_ID = "HallOfFameScreenMapsizeDropdown"
		self.CLIMATE_DROPDOWN_ID = "HallOfFameScreenClimateDropdown"
		self.SEALEVEL_DROPDOWN_ID = "HallOfFameScreenSealevelDropdown"
		self.ERA_DROPDOWN_ID = "HallOfFameScreenEraDropdown"
		self.SPEED_DROPDOWN_ID = "HallOfFameScreenSpeedDropdown"
		self.VICTORY_DROPDOWN_ID = "HallOfFameScreenVictoryDropdown"
		self.MULTIPLAYER_DROPDOWN_ID = "HallOfFameScreenMultiplayerDropdown"
		self.SORT_DROPDOWN_ID = "HallOfFameScreenSortDropdown"
		self.REPLAY_BUTTON_ID = "HallOfFameReplayButton"

	def getScreen(self):
		return CyGInterfaceScreen(self.SCREEN_NAME, self.screenId)

	def hideScreen(self):
		screen = self.getScreen()
		screen.hideScreen()

	# Screen construction function
	def interfaceScreen(self, bAllowReplay):


		screen = self.getScreen()
		
		self.W_SCREEN = screen.getXResolution()
		self.H_SCREEN = screen.getYResolution()
		self.Y_TITLE = 4

		self.Y_EXIT = self.H_SCREEN - 36
		self.X_EXIT = self.W_SCREEN - 30

		self.DROPDOWN_WIDTH = 200
		self.DROPDOWN_Y = 70
		self.DROPDOWN_SPACING_X = 45
		self.DROPDOWN_SPACING_Y = 50
		
		self.DROPDOWN_X = (self.W_SCREEN / 2) - (((self.DROPDOWN_SPACING_X * 3) + (self.DROPDOWN_WIDTH * 4)) / 2)

		self.nWidgetCount = 0

		self.bAllowReplay = False

		# Create a new screen
		screen = self.getScreen()
		if screen.isActive():
			return
		screen.setRenderInterfaceOnly(True);
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)
		screen.setAlwaysShown(True)

		self.bAllowReplay = bAllowReplay
		self.iLeaderFilter = -1
		self.iHandicapFilter = -1
		self.iWorldFilter = -1
		self.iClimateFilter = -1
		self.iSeaLevelFilter = -1
		self.iEraFilter = -1
		self.iSpeedFilter = -1
		self.iVictoryFilter = -1
		if gc.getGame().isGameMultiPlayer():
			self.iMultiplayerFilter = 1
		else:
			self.iMultiplayerFilter = 0
		self.iSortBy = SORT_BY_NORMALIZED_SCORE

		self.EXIT_TEXT = u"<font=4>" + localText.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + u"</font>"

		self.hallOfFame = CyHallOfFameInfo()
		self.hallOfFame.loadReplays()

		# Set the background widget and exit button
		screen.setDimensions(0, 0, self.W_SCREEN, self.H_SCREEN)
		screen.addDDSGFC(self.BACKGROUND_ID, ArtFileMgr.getInterfaceArtInfo("SCREEN_BG_OPAQUE").getPath(), 0, 0, self.W_SCREEN, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDDSGFC("TopPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TITLE").getPath(), 0, 0, self.W_SCREEN, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDDSGFC("BottomPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_OFF").getPath(), 0, self.H_SCREEN - 55, self.W_SCREEN, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.showWindowBackground(False)
		screen.setText(self.EXIT_ID, "", self.EXIT_TEXT, CvUtil.FONT_RIGHT_JUSTIFY, self.X_EXIT, self.Y_EXIT, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		# Header...
		screen.setLabel(self.HEADER_ID, "Background", u"<font=4b>" + localText.getText("TXT_KEY_HALL_OF_FAME_SCREEN_TITLE", ()).upper() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.W_SCREEN / 2, self.Y_TITLE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		xDropDown = self.DROPDOWN_X
		yDropDown = self.DROPDOWN_Y
		iNumDropDowns = 0

		# Leader dropdown initialization
		screen.addDropDownBoxGFC(self.LEADER_DROPDOWN_ID, xDropDown, yDropDown, self.DROPDOWN_WIDTH, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString(self.LEADER_DROPDOWN_ID, localText.getText("TXT_KEY_ALL_LEADERS", ()), -1, -1, True)
		for iCiv in range(gc.getNumCivilizationInfos()):
			civ = gc.getCivilizationInfo(iCiv)
			if civ.isPlayable():
				for iLeader in range(gc.getNumLeaderHeadInfos()):
					if civ.isLeaders(iLeader):
						screen.addPullDownString(self.LEADER_DROPDOWN_ID, gc.getLeaderHeadInfo(iLeader).getDescription(), iCiv, iLeader, False)
		iNumDropDowns += 1

		yDropDown = self.DROPDOWN_SPACING_Y * (iNumDropDowns % 2) + self.DROPDOWN_Y

		# Victory dropdown initialization
		screen.addDropDownBoxGFC(self.VICTORY_DROPDOWN_ID, xDropDown, yDropDown, self.DROPDOWN_WIDTH, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString(self.VICTORY_DROPDOWN_ID, localText.getText("TXT_KEY_ALL_VICTORIES", ()), -1, -1, True)
		for i in range(gc.getNumVictoryInfos()):
			if not gc.getVictoryInfo(i).isPermanent():
				screen.addPullDownString(self.VICTORY_DROPDOWN_ID, gc.getVictoryInfo(i).getDescription(), i, i, False)
		iNumDropDowns += 1

		yDropDown = self.DROPDOWN_SPACING_Y * (iNumDropDowns % 2) + self.DROPDOWN_Y
		xDropDown += self.DROPDOWN_WIDTH + self.DROPDOWN_SPACING_X

		# Difficulty level dropdown initialization
		screen.addDropDownBoxGFC(self.DIFFICULTY_DROPDOWN_ID, xDropDown, yDropDown, self.DROPDOWN_WIDTH, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString(self.DIFFICULTY_DROPDOWN_ID, localText.getText("TXT_KEY_ALL_DIFFICULTIES", ()), -1, -1, True)
		for iHandicap in range(gc.getNumHandicapInfos()):
			screen.addPullDownString(self.DIFFICULTY_DROPDOWN_ID, gc.getHandicapInfo(iHandicap).getDescription(), iHandicap, iHandicap, False)
		iNumDropDowns += 1

		yDropDown = self.DROPDOWN_SPACING_Y * (iNumDropDowns % 2) + self.DROPDOWN_Y

		# World Size dropdown initialization
		screen.addDropDownBoxGFC(self.MAPSIZE_DROPDOWN_ID, xDropDown, yDropDown, self.DROPDOWN_WIDTH, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString(self.MAPSIZE_DROPDOWN_ID, localText.getText("TXT_KEY_ALL_WORLD_SIZES", ()), -1, -1, True)
		for i in range(gc.getNumWorldInfos()):
			screen.addPullDownString(self.MAPSIZE_DROPDOWN_ID, gc.getWorldInfo(i).getDescription(), i, i, False)
		iNumDropDowns += 1

		yDropDown = self.DROPDOWN_SPACING_Y * (iNumDropDowns % 2) + self.DROPDOWN_Y
		xDropDown += self.DROPDOWN_WIDTH + self.DROPDOWN_SPACING_X

		# Era dropdown initialization
		screen.addDropDownBoxGFC(self.ERA_DROPDOWN_ID, xDropDown, yDropDown, self.DROPDOWN_WIDTH, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString(self.ERA_DROPDOWN_ID, localText.getText("TXT_KEY_ALL_ERAS", ()), -1, -1, True)
		for i in range(gc.getNumEraInfos()):
			screen.addPullDownString(self.ERA_DROPDOWN_ID, gc.getEraInfo(i).getDescription(), i, i, False)
		iNumDropDowns += 1

		yDropDown = self.DROPDOWN_SPACING_Y * (iNumDropDowns % 2) + self.DROPDOWN_Y

		# Game Speed dropdown initialization
		screen.addDropDownBoxGFC(self.SPEED_DROPDOWN_ID, xDropDown, yDropDown, self.DROPDOWN_WIDTH, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString(self.SPEED_DROPDOWN_ID, localText.getText("TXT_KEY_ALL_GAME_SPEEDS", ()), -1, -1, True)
		for i in range(gc.getNumGameSpeedInfos()):
			screen.addPullDownString(self.SPEED_DROPDOWN_ID, gc.getGameSpeedInfo(i).getDescription(), i, i, False)
		iNumDropDowns += 1

		yDropDown = self.DROPDOWN_SPACING_Y * (iNumDropDowns % 2) + self.DROPDOWN_Y
		xDropDown += self.DROPDOWN_WIDTH + self.DROPDOWN_SPACING_X

		# Multiplayer dropdown initialization
		screen.addDropDownBoxGFC(self.MULTIPLAYER_DROPDOWN_ID, xDropDown, yDropDown, self.DROPDOWN_WIDTH, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		if self.iMultiplayerFilter == 1:
			screen.addPullDownString(self.MULTIPLAYER_DROPDOWN_ID, localText.getText("TXT_KEY_MAIN_MENU_MULTIPLAYER", ()), 1, 1, True)
			screen.addPullDownString(self.MULTIPLAYER_DROPDOWN_ID, localText.getText("TXT_KEY_MAIN_MENU_SINGLE_PLAYER", ()), 0, 0, False)
		else:
			screen.addPullDownString(self.MULTIPLAYER_DROPDOWN_ID, localText.getText("TXT_KEY_MAIN_MENU_SINGLE_PLAYER", ()), 0, 0, True)
			screen.addPullDownString(self.MULTIPLAYER_DROPDOWN_ID, localText.getText("TXT_KEY_MAIN_MENU_MULTIPLAYER", ()), 1, 1, False)
		iNumDropDowns += 1

		yDropDown = self.DROPDOWN_SPACING_Y * (iNumDropDowns % 2) + self.DROPDOWN_Y

		# Score dropdown initialization
		screen.addDropDownBoxGFC(self.SORT_DROPDOWN_ID, xDropDown, yDropDown, self.DROPDOWN_WIDTH, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString(self.SORT_DROPDOWN_ID, localText.getText("TXT_KEY_HALL_OF_FAME_SORT_BY_NORMALIZED_SCORE", ()), SORT_BY_NORMALIZED_SCORE, SORT_BY_NORMALIZED_SCORE, True)
		screen.addPullDownString(self.SORT_DROPDOWN_ID, localText.getText("TXT_KEY_HALL_OF_FAME_SORT_BY_DATE", ()), SORT_BY_FINISH_DATE, SORT_BY_FINISH_DATE, False)
		screen.addPullDownString(self.SORT_DROPDOWN_ID, localText.getText("TXT_KEY_HALL_OF_FAME_SORT_BY_GAME_SCORE", ()), SORT_BY_GAME_SCORE, SORT_BY_GAME_SCORE, False)
		iNumDropDowns += 1

		self.drawContents()

	def isDisplayed(self, replayInfo):
		return ((self.iLeaderFilter == -1 or self.iLeaderFilter == replayInfo.getLeader(replayInfo.getActivePlayer()))
			and (self.iHandicapFilter == -1 or self.iHandicapFilter == replayInfo.getDifficulty())
			and (self.iWorldFilter == -1 or self.iWorldFilter == replayInfo.getWorldSize())
			and (self.iClimateFilter == -1 or self.iClimateFilter == replayInfo.getClimate())
			and (self.iSeaLevelFilter == -1 or self.iSeaLevelFilter == replayInfo.getSeaLevel())
			and (self.iEraFilter == -1 or self.iEraFilter == replayInfo.getEra())
			and (self.iSpeedFilter == -1 or self.iSpeedFilter == replayInfo.getGameSpeed())
			and (self.iVictoryFilter == -1 or self.iVictoryFilter == replayInfo.getVictoryType())
			and ((self.iMultiplayerFilter == 1) == replayInfo.isMultiplayer()))


	def drawContents(self):

		screen = self.getScreen()

		screen.addTableControlGFC(self.TABLE_ID, 10, (self.W_SCREEN / 2) - (1018 / 2), 2 * self.DROPDOWN_SPACING_Y + self.DROPDOWN_Y, 1018, self.H_SCREEN - 245, True, True, 16, 16, TableStyles.TABLE_STYLE_STANDARD);
		screen.enableSelect(self.TABLE_ID, False)
		screen.setTableColumnHeader(self.TABLE_ID, 0, "", 20)
		screen.setTableColumnHeader(self.TABLE_ID, 1, localText.getText("TXT_KEY_PITBOSS_LEADER", ()), 202)
		screen.setTableColumnHeader(self.TABLE_ID, 2, localText.getText("TXT_KEY_NORMALIZED_SCORE", ()), 100)
		screen.setTableColumnHeader(self.TABLE_ID, 3, localText.getText("TXT_KEY_HALL_OF_FAME_SORT_BY_DATE", ()), 88)
		screen.setTableColumnHeader(self.TABLE_ID, 4, localText.getText("TXT_KEY_GAME_SCORE", ()), 100)
		screen.setTableColumnHeader(self.TABLE_ID, 5, localText.getText("TXT_KEY_VICTORY", ()), 100)
		screen.setTableColumnHeader(self.TABLE_ID, 6, localText.getText("TXT_KEY_PITBOSS_DIFFICULTY", ()), 100)
		screen.setTableColumnHeader(self.TABLE_ID, 7, localText.getText("TXT_KEY_HOF_SCREEN_SIZE", ()), 100)
		screen.setTableColumnHeader(self.TABLE_ID, 8, localText.getText("TXT_KEY_HOF_SCREEN_STARTING_ERA", ()), 100)
		screen.setTableColumnHeader(self.TABLE_ID, 9, localText.getText("TXT_KEY_HOF_SCREEN_GAME_SPEED", ()), 105)

		# count the filtered replays
		iNumGames = 0
		for i in range(self.hallOfFame.getNumGames()):
			replayInfo = self.hallOfFame.getReplayInfo(i)
			if self.isDisplayed(replayInfo):
				iNumGames += 1


		self.infoList = [(-1,"",-1,"",-1,"","","","","",0)] * iNumGames
		iItem = 0
		for i in range(self.hallOfFame.getNumGames()):
			replayInfo = self.hallOfFame.getReplayInfo(i)
			if self.isDisplayed(replayInfo):
				szVictory = u""
				if replayInfo.getVictoryType() <= 0:
					szVictory = localText.getText("TXT_KEY_NONE", ())
				else:
					szVictory = gc.getVictoryInfo(replayInfo.getVictoryType()).getDescription()

				if self.iSortBy == SORT_BY_NORMALIZED_SCORE:
					iValue = -replayInfo.getNormalizedScore()
				elif self.iSortBy == SORT_BY_FINISH_DATE:
					iValue = replayInfo.getFinalTurn()
				elif self.iSortBy == SORT_BY_GAME_SCORE:
					iValue = -replayInfo.getFinalScore()

				self.infoList[iItem] = (iValue,
						localText.getText("TXT_KEY_LEADER_CIV_DESCRIPTION", (replayInfo.getLeaderName(), replayInfo.getShortCivDescription())),
						replayInfo.getNormalizedScore(),
						replayInfo.getFinalDate(),
						replayInfo.getFinalScore(),
						szVictory,
						gc.getHandicapInfo(replayInfo.getDifficulty()).getDescription(),
						gc.getWorldInfo(replayInfo.getWorldSize()).getDescription(),
						gc.getEraInfo(replayInfo.getEra()).getDescription(),
						gc.getGameSpeedInfo(replayInfo.getGameSpeed()).getDescription(),
						i)
				iItem += 1
		self.infoList.sort()

		for i in range(len(self.infoList)):

			szButtonName = self.REPLAY_BUTTON_ID + str(i)
			screen.setButtonGFC(szButtonName, self.infoList[i][1], "",
				0, 0, 10, 10, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_STANDARD)

			screen.appendTableRow(self.TABLE_ID)
			screen.setTableText(self.TABLE_ID, 1, i, self.infoList[i][1], "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
			if self.infoList[i][2] >= 0:
				screen.setTableInt(self.TABLE_ID, 2, i, u"%d" % self.infoList[i][2], "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_RIGHT_JUSTIFY)
			screen.setTableDate(self.TABLE_ID, 3, i, self.infoList[i][3], "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
			if self.infoList[i][4] >= 0:
				screen.setTableInt(self.TABLE_ID, 4, i, u"%d" % self.infoList[i][4], "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_RIGHT_JUSTIFY)
			screen.setTableText(self.TABLE_ID, 5, i, self.infoList[i][5], "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
			screen.setTableText(self.TABLE_ID, 6, i, self.infoList[i][6], "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
			screen.setTableText(self.TABLE_ID, 7, i, self.infoList[i][7], "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
			screen.setTableText(self.TABLE_ID, 8, i, self.infoList[i][8], "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
			screen.setTableText(self.TABLE_ID, 9, i, self.infoList[i][9], "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
			screen.attachControlToTableCell(szButtonName, self.TABLE_ID, i, 0)

		return


	# handle the input for this screen...
	def handleInput (self, inputClass):
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED):
			if (inputClass.getFunctionName() == self.LEADER_DROPDOWN_ID):
				screen = self.getScreen()
				iIndex = screen.getSelectedPullDownID(self.LEADER_DROPDOWN_ID)
				self.iLeaderFilter = screen.getPullDownData(self.LEADER_DROPDOWN_ID, iIndex)
				self.drawContents()
			elif (inputClass.getFunctionName() == self.DIFFICULTY_DROPDOWN_ID):
				screen = self.getScreen()
				iIndex = screen.getSelectedPullDownID(self.DIFFICULTY_DROPDOWN_ID)
				self.iHandicapFilter = screen.getPullDownData(self.DIFFICULTY_DROPDOWN_ID, iIndex)
				self.drawContents()
			elif (inputClass.getFunctionName() == self.MAPSIZE_DROPDOWN_ID):
				screen = self.getScreen()
				iIndex = screen.getSelectedPullDownID(self.MAPSIZE_DROPDOWN_ID)
				self.iWorldFilter = screen.getPullDownData(self.MAPSIZE_DROPDOWN_ID, iIndex)
				self.drawContents()
			elif (inputClass.getFunctionName() == self.CLIMATE_DROPDOWN_ID):
				screen = self.getScreen()
				iIndex = screen.getSelectedPullDownID(self.CLIMATE_DROPDOWN_ID)
				self.iClimateFilter = screen.getPullDownData(self.CLIMATE_DROPDOWN_ID, iIndex)
				self.drawContents()
			elif (inputClass.getFunctionName() == self.SEALEVEL_DROPDOWN_ID):
				screen = self.getScreen()
				iIndex = screen.getSelectedPullDownID(self.SEALEVEL_DROPDOWN_ID)
				self.iSeaLevelFilter = screen.getPullDownData(self.SEALEVEL_DROPDOWN_ID, iIndex)
				self.drawContents()
			elif (inputClass.getFunctionName() == self.ERA_DROPDOWN_ID):
				screen = self.getScreen()
				iIndex = screen.getSelectedPullDownID(self.ERA_DROPDOWN_ID)
				self.iEraFilter = screen.getPullDownData(self.ERA_DROPDOWN_ID, iIndex)
				self.drawContents()
			elif (inputClass.getFunctionName() == self.SPEED_DROPDOWN_ID):
				screen = self.getScreen()
				iIndex = screen.getSelectedPullDownID(self.SPEED_DROPDOWN_ID)
				self.iSpeedFilter = screen.getPullDownData(self.SPEED_DROPDOWN_ID, iIndex)
				self.drawContents()
			elif (inputClass.getFunctionName() == self.VICTORY_DROPDOWN_ID):
				screen = self.getScreen()
				iIndex = screen.getSelectedPullDownID(self.VICTORY_DROPDOWN_ID)
				self.iVictoryFilter = screen.getPullDownData(self.VICTORY_DROPDOWN_ID, iIndex)
				self.drawContents()
			elif (inputClass.getFunctionName() == self.MULTIPLAYER_DROPDOWN_ID):
				screen = self.getScreen()
				iIndex = screen.getSelectedPullDownID(self.MULTIPLAYER_DROPDOWN_ID)
				self.iMultiplayerFilter = screen.getPullDownData(self.MULTIPLAYER_DROPDOWN_ID, iIndex)
				self.drawContents()
			elif (inputClass.getFunctionName() == self.SORT_DROPDOWN_ID):
				screen = self.getScreen()
				iIndex = screen.getSelectedPullDownID(self.SORT_DROPDOWN_ID)
				self.iSortBy = screen.getPullDownData(self.SORT_DROPDOWN_ID, iIndex)
				self.drawContents()
		elif (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if (inputClass.getFunctionName() == self.EXIT_ID):
				screen = self.getScreen()
				screen.hideScreen()
			elif (inputClass.getFunctionName() == self.REPLAY_BUTTON_ID and self.bAllowReplay):
				iRow = inputClass.getID()
				if iRow < len(self.infoList):
					CvScreensInterface.replayScreen.replayInfo = self.hallOfFame.getReplayInfo(self.infoList[iRow][10])
					CvScreensInterface.replayScreen.showScreen(True)

		return 0

	def update(self, fDelta):
		return

