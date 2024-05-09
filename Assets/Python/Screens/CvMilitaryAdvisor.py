## Sid Meier's Civilization 4: Colonization
## Copyright Firaxis Games 2008
## Special thanks to SupremeOverlord & TheLopez of Civilization Fanatics

from CvPythonExtensions import *
import CvUtil
import ScreenInput
import time
import re

gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class CvMilitaryAdvisor:
	"Military Advisor"

	groupSelectionName = "MIL_groupSelection"
	unitCheckBox = "MIL_unitCheckBox"

	PROFESSION_GROUP_ID = 0
	OWNER_GROUP_ID = 1
	STACK_GROUP_ID = 2
	SKILL_GROUP_ID = 3
	LOCATION_GROUP_ID = 4

	numGroups = 0
	INITED = False

	def __init__(self, screenId):
		self.screenId = screenId
		self.MILITARY_SCREEN_NAME = "MilitaryAdvisor"
		self.BACKGROUND_ID = "MilitaryAdvisorBackground"
		self.EXIT_ID = "MilitaryAdvisorExitWidget"

		self.WIDGET_ID = "MilitaryAdvisorWidget"
		self.REFRESH_WIDGET_ID = "MilitaryAdvisorRefreshWidget"
		self.ATTACH_WIDGET_ID = "MilitaryAdvisorAttachWidget"
		self.SELECTION_WIDGET_ID = "MilitaryAdvisorSelectionWidget"
		self.ATTACHED_WIDGET_ID = "MilitaryAdvisorAttachedWidget"
		self.LEADER_BUTTON_ID = "MilitaryAdvisorLeaderButton"
		self.UNIT_PANEL_ID = "MilitaryAdvisorUnitPanel"
		self.UNIT_BUTTON_ID = "MilitaryAdvisorUnitButton"
		self.UNIT_BUTTON_LABEL_ID = "MilitaryAdvisorUnitButtonLabel"
		self.LEADER_PANEL_ID = "MilitaryAdvisorLeaderPanel"
		self.UNIT_LIST_ID = "MilitaryAdvisorUnitList"

		self.Z_BACKGROUND = -2.1
		self.Z_CONTROLS = self.Z_BACKGROUND - 0.2
		self.DZ = -0.2

		self.Y_SCREEN = 396
		
		#Engine is not initialized until interfaceScreen() is called - Jason
		#screen = self.getScreen()
		#self.W_SCREEN = screen.getXResolution()
		#self.H_SCREEN = screen.getYResolution()

		self.Y_TITLE = 4
		self.BORDER_WIDTH = 4
		self.W_HELP_AREA = 200

		self.nWidgetCount = 0
		self.nRefreshWidgetCount = 0
		self.nAttachedWidgetCount = 0
		self.iActivePlayer = -1
		self.selectedPlayerList = []
		self.selectedGroupList = []
		self.selectedUnitList = []
		self.unitList = []
		self.groups = {}

		self.X_LEADERS = 20
		self.Y_LEADERS = 80
		self.W_LEADERS = 985
		self.H_LEADERS = 70
		self.LEADER_BUTTON_SIZE = 64
		self.LEADER_MARGIN = 12

		self.GROUPING_SELECTION = self.PROFESSION_GROUP_ID

		self.GROUP_TOGGLE = 100
		self.UNIT_TOGGLE = 200

		self.bUnitDetails = False
		self.iShiftKeyDown = 0

	def setScreen(self):
		self.screen = CyGInterfaceScreen(self.MILITARY_SCREEN_NAME, self.screenId)

		global INITED
		INITED = True

	def getScreen(self):
		if self.INITED:
			return self.screen
		else:
			self.setScreen()
			return self.screen

	# Screen construction function
	def interfaceScreen(self):
		screen = self.getScreen()
		if screen.isActive():
			return

 		screen.setRenderInterfaceOnly(True);
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		self.W_SCREEN = screen.getXResolution()
		self.H_SCREEN = screen.getYResolution()

		self.nWidgetCount = 0
		self.iActivePlayer = gc.getGame().getActivePlayer()

		self.Y_EXIT = self.H_SCREEN - 42
		self.Y_BOTTOM_PANEL = self.H_SCREEN - 55

		screen.setDimensions((screen.getXResolution() - self.W_SCREEN) / 2, (screen.getYResolution() - self.H_SCREEN) / 2, self.W_SCREEN, self.H_SCREEN)
		screen.addDDSGFC(self.BACKGROUND_ID, ArtFileMgr.getInterfaceArtInfo("MAINMENU_SLIDESHOW_LOAD").getPath(), 0, 0, self.W_SCREEN, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		self.viewMargin = 20
		self.viewX = 455 - self.viewMargin
		self.viewY = 60
		self.viewWidth = self.W_SCREEN - self.viewX
		self.viewHeight = self.H_SCREEN - 220
		#self.LEADER_COLUMNS = int((self.viewWidth - 20) / (self.LEADER_BUTTON_SIZE + self.LEADER_MARGIN))

		self.unitBackgroundName = self.getNextWidgetName()
		
		screen.addDDSGFC("TopPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TITLE").getPath(), 0, 0, self.W_SCREEN, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDDSGFC("BottomPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_OFF").getPath(), 0, self.H_SCREEN - 55, self.W_SCREEN, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.showWindowBackground(False)
		screen.setText(self.EXIT_ID, "", u"<font=4>" + localText.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + "</font>", CvUtil.FONT_RIGHT_JUSTIFY, self.W_SCREEN - 30, self.H_SCREEN - 36, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )

		screen.addPanel( "", u"", "", False, False, self.viewX + 10, self.viewY, self.viewWidth - 20, self.viewHeight, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		self.initMinimap()

		screen.addDropDownBoxGFC(self.groupSelectionName, 15, 55, self.viewX - 30, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString(self.groupSelectionName, localText.getText("TXT_KEY_UNIT_ADVISOR_PROFESSION_SORT", ()), self.PROFESSION_GROUP_ID, self.PROFESSION_GROUP_ID, False)
		screen.addPullDownString(self.groupSelectionName, localText.getText("TXT_KEY_UNIT_ADVISOR_OWNER_SORT", ()), self.OWNER_GROUP_ID, self.OWNER_GROUP_ID, False)
		screen.addPullDownString(self.groupSelectionName, localText.getText("TXT_KEY_UNIT_ADVISOR_STACK_SORT", ()), self.STACK_GROUP_ID, self.STACK_GROUP_ID, False)
		screen.addPullDownString(self.groupSelectionName, localText.getText("TXT_KEY_UNIT_ADVISOR_SKILL_SORT", ()), self.SKILL_GROUP_ID, self.SKILL_GROUP_ID, False)
		screen.addPullDownString(self.groupSelectionName, localText.getText("TXT_KEY_UNIT_ADVISOR_TERRITORY_SORT", ()), self.LOCATION_GROUP_ID, self.LOCATION_GROUP_ID, False)

		self.szHeader = self.getNextWidgetName()
		self.SCREEN_TITLE = u"<font=4b>" + localText.getText("TXT_KEY_MILITARY_ADVISOR_TITLE", ()).upper() + u"</font>"
		self.SCREEN_TITLE = localText.changeTextColor(self.SCREEN_TITLE, ColorTypes.COLOR_FONT_CREAM)
		screen.setText(self.szHeader, "Background", self.SCREEN_TITLE, CvUtil.FONT_CENTER_JUSTIFY, self.W_SCREEN / 2, self.Y_TITLE, self.Z_CONTROLS, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		self.unitPanelName = self.getNextWidgetName()
		self.unitGroupName = self.getNextWidgetName()

		self.pActivePlayer = gc.getPlayer(self.iActivePlayer)

		# GREAT GENERAL
		szWidget = "GreatGeneralBar"
		screen.addStackedBarGFC(szWidget, self.W_SCREEN * 2 / 8, self.Y_EXIT + 5, self.W_SCREEN / 4, 30, InfoBarTypes.NUM_INFOBAR_TYPES, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setStackedBarColors(szWidget, InfoBarTypes.INFOBAR_STORED, ColorTypes.COLOR_GREAT_PEOPLE_STORED)
		screen.setStackedBarColors(szWidget, InfoBarTypes.INFOBAR_RATE, ColorTypes.COLOR_GREAT_PEOPLE_RATE)
		screen.setStackedBarColors(szWidget, InfoBarTypes.INFOBAR_RATE_EXTRA, ColorTypes.COLOR_EMPTY)
		screen.setStackedBarColors(szWidget, InfoBarTypes.INFOBAR_EMPTY, ColorTypes.COLOR_EMPTY)
		screen.setBarPercentage(szWidget, InfoBarTypes.INFOBAR_STORED, float(self.pActivePlayer.getCombatExperience()) / float(self.pActivePlayer.greatGeneralThreshold()))
		screen.setLabel(self.getNextWidgetName(), "", u"<font=3>" + localText.getText("TXT_KEY_MISC_GREAT_GENERAL", (self.pActivePlayer.getCombatExperience(), self.pActivePlayer.greatGeneralThreshold())) + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.W_SCREEN / 2 - self.W_SCREEN * 1/ 8, self.Y_EXIT + 8, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# GREAT ADMIRALS
		szWidget = "GreatAdmiralBar"
		screen.addStackedBarGFC(szWidget, self.W_SCREEN * 4 / 8, self.Y_EXIT + 5, self.W_SCREEN / 4, 30, InfoBarTypes.NUM_INFOBAR_TYPES, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setStackedBarColors(szWidget, InfoBarTypes.INFOBAR_STORED, ColorTypes.COLOR_GREAT_PEOPLE_STORED)
		screen.setStackedBarColors(szWidget, InfoBarTypes.INFOBAR_RATE, ColorTypes.COLOR_GREAT_PEOPLE_RATE)
		screen.setStackedBarColors(szWidget, InfoBarTypes.INFOBAR_RATE_EXTRA, ColorTypes.COLOR_EMPTY)
		screen.setStackedBarColors(szWidget, InfoBarTypes.INFOBAR_EMPTY, ColorTypes.COLOR_EMPTY)
		screen.setBarPercentage(szWidget, InfoBarTypes.INFOBAR_STORED, float(self.pActivePlayer.getSeaCombatExperience()) / float(self.pActivePlayer.greatAdmiralThreshold()))
		screen.setLabel(self.getNextWidgetName(), "", u"<font=3>" + localText.getText("TXT_KEY_MISC_GREAT_ADMIRAL", (self.pActivePlayer.getSeaCombatExperience(), self.pActivePlayer.greatAdmiralThreshold())) + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.W_SCREEN / 2 + self.W_SCREEN * 1/ 8, self.Y_EXIT + 8, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		
		#LEADERS
		screen.addScrollPanel(self.LEADER_PANEL_ID, "", self.viewX + 10, self.viewY + self.viewHeight + (self.viewMargin / 2) , self.viewWidth - 20, self.H_LEADERS, PanelStyles.PANEL_STYLE_MAIN, False, WidgetTypes.WIDGET_GENERAL, -1, -1)

		self.KnownLeaders = []
		for iLoopPlayer in range(gc.getMAX_PLAYERS()):
			player = gc.getPlayer(iLoopPlayer)
			if ( player.isAlive() and gc.getTeam(player.getTeam()).isHasMet(gc.getPlayer(self.iActivePlayer).getTeam()) ):
				self.KnownLeaders.append(iLoopPlayer)

		iNumLeaders = len(self.KnownLeaders)
		iButtonSize = self.LEADER_BUTTON_SIZE

		for iIndex in range(iNumLeaders):
			iLoopPlayer = self.KnownLeaders[iIndex]
			player = gc.getPlayer(iLoopPlayer)

			szButton = gc.getLeaderHeadInfo(gc.getPlayer(iLoopPlayer).getLeaderType()).getButton()
			screen.addCheckBoxGFCAt( self.LEADER_PANEL_ID, self.LEADER_BUTTON_ID + str(iLoopPlayer), szButton, ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), iIndex * int(iButtonSize * 1.1), 0, iButtonSize, iButtonSize, WidgetTypes.WIDGET_MINIMAP_HIGHLIGHT, 2, iLoopPlayer, ButtonStyles.BUTTON_STYLE_LABEL )
			screen.setState(self.LEADER_BUTTON_ID + str(iLoopPlayer), (iLoopPlayer in self.selectedPlayerList))

		self.refreshSelectedLeader(self.iActivePlayer)

	def ListActivePlayerUnits(self):
		AllUnits = []
		(city, iter) = self.pActivePlayer.firstCity(True)
		while(city):
			for iCitizen in range(city.getPopulation()):
				AllUnits.append(city.getPopulationUnitByIndex(iCitizen))
			(city, iter) = self.pActivePlayer.nextCity(iter, True)

	 	(unit, iter) = self.pActivePlayer.firstUnit()
		while(unit):
			AllUnits.append(unit)
			(unit, iter) = self.pActivePlayer.nextUnit(iter)

		return AllUnits

	def ListPlayerVisibleUnits(self, iPlayer):
		VisibleUnits = []
		if (iPlayer in self.KnownLeaders):
			(unit, iter) = gc.getPlayer(iPlayer).firstUnit()
			while(unit):
				unitType = unit.getUnitType()
				plot = unit.plot()
				if (not plot.isNone()):
					if (plot.isVisible(self.pActivePlayer.getTeam(), False) and not unit.isInvisible(self.pActivePlayer.getTeam(), False)):
						VisibleUnits.append(unit)
				(unit, iter) = gc.getPlayer(iPlayer).nextUnit(iter)

		return VisibleUnits

	def initMinimap(self):
		screen = self.getScreen()
		mapWidth = self.viewWidth - 2 * self.viewMargin
		mapHeight = self.viewHeight - 2 * self.viewMargin

		mapHeightPref = (mapWidth * CyMap().getGridHeight()) / CyMap().getGridWidth()
		if (mapHeightPref > mapHeight):
			mapWidth = (mapHeight * CyMap().getGridWidth()) / CyMap().getGridHeight()
			horMapMargin = (self.viewWidth - mapWidth) / 2
			verMapMargin = self.viewMargin
		else:
			mapHeight = mapHeightPref
			horMapMargin = self.viewMargin
			verMapMargin = (self.viewHeight - mapHeight) / 2

		screen.initMinimap( self.viewX + horMapMargin, self.viewX + self.viewWidth - horMapMargin, self.viewY + verMapMargin, self.viewY + self.viewHeight - verMapMargin, self.Z_CONTROLS, False)
		screen.updateMinimapSection(False)

		screen.setMinimapMode(MinimapModeTypes.MINIMAPMODE_MILITARY)

		iOldMode = CyInterface().getShowInterface()
		CyInterface().setShowInterface(InterfaceVisibility.INTERFACE_MINIMAP_ONLY)
		screen.updateMinimapVisibility()
		CyInterface().setShowInterface(iOldMode)

	def initPlotView(self):
		self.plotview = self.getNextWidgetName()

	def addGroupPanel(self, label, groupIndex, verticalIndex, groupID):
		screen = self.getScreen()
		NewGroupPanel = self.unitGroupName + str(verticalIndex)
		screen.attachPanelAt(self.unitBackgroundName, NewGroupPanel, "", "", True, False, PanelStyles.PANEL_STYLE_DAWNTOP, 0, 50 * verticalIndex, 415, 50, WidgetTypes.WIDGET_GENERAL, self.GROUP_TOGGLE, groupIndex)
		screen.setTextAt(NewGroupPanel + "Label" + str(groupID), self.unitBackgroundName, u"<font=4>" + label + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, 10, 25 + (50 * verticalIndex), -1.3, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, self.GROUP_TOGGLE, groupIndex)

		if (groupIndex == -1):
			count = len(self.unitList)
		else:
			groupID = self.groupIDs[groupIndex]
			units = self.groups[groupID].units
			count = len(units)

		screen.setTextAt(NewGroupPanel + "Count" + str(groupID), self.unitBackgroundName, u"<font=4>" + str(count) + u"</font>", CvUtil.FONT_RIGHT_JUSTIFY, 405, 25 + (50 * verticalIndex), -1.3, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, self.GROUP_TOGGLE, groupIndex)

	def addUnitToList(self, unit, idx, verticalIndex):
		screen = self.getScreen()
		screen.addCheckBoxGFCAt(self.unitBackgroundName, self.unitCheckBox + str(idx), gc.getUnitInfo(unit.getUnitType()).getButton(), ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), 1, 2 + verticalIndex * 50, 46, 46, WidgetTypes.WIDGET_GENERAL, self.UNIT_TOGGLE, idx, ButtonStyles.BUTTON_STYLE_LABEL)
		szDescription = CyGameTextMgr().getSpecificUnitHelp(unit, True, False)
		screen.attachMultilineTextAt(self.unitBackgroundName, "description" + str(idx), u"<font=3>" + szDescription + u"</font>", 50, (verticalIndex * 50) - 4, 360, 50, WidgetTypes.WIDGET_GENERAL, self.UNIT_TOGGLE, idx, CvUtil.FONT_LEFT_JUSTIFY)

	def addUnitsToList(self, units):
		verticalIndex = 0
		for idx in range(len(units)):
			self.addUnitToList(units[idx], idx, verticalIndex)
			verticalIndex += 1

	def setUnitCheckBoxStates(self):
		screen = self.getScreen()
		for idx in range(len(self.unitList)):
			screen.setState(self.unitCheckBox + str(idx), False)
		for idx in self.selectedUnitList:
			screen.setState(self.unitCheckBox + str(idx), True)

	def handleInput (self, inputClass):
		screen = self.getScreen()
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED and inputClass.getFlags() & MouseFlags.MOUSE_LBUTTONUP):
			if(inputClass.getData1() == self.GROUP_TOGGLE):
				if inputClass.getData2() == -1:
					self.toggleAllCheckBoxes(-1)
				else:
					self.toggleGroupCheckBoxes(inputClass.getData2())
			if (inputClass.getData1() == self.UNIT_TOGGLE):
				self.toggleUnitCheckBox(inputClass.getData2())
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED):
			if(inputClass.getFunctionName() == self.groupSelectionName):
				iIndex = screen.getSelectedPullDownID(self.groupSelectionName)
				self.GROUPING_SELECTION = iIndex
				self.groupSelectionChanged(iIndex)
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CHARACTER):
			if (inputClass.getData() == int(InputTypes.KB_LSHIFT) or inputClass.getData() == int(InputTypes.KB_RSHIFT)):
				self.iShiftKeyDown = inputClass.getID()

		return 0

	def toggleUnitCheckBox(self, unitIndex):
		screen = self.getScreen()
		if unitIndex in self.selectedUnitList:
			self.selectedUnitList.remove(unitIndex)
			screen.setState(self.unitCheckBox + str(unitIndex), False)
		else:
			self.selectedUnitList.append(unitIndex)
			screen.setState(self.unitCheckBox + str(unitIndex), True)
		self.refreshMinimap()

	def toggleAllCheckBoxes(self, groupIndex):
		screen = self.getScreen()

		if self.allState:
			self.allState = False
			self.selectedUnitList = []
			for unitIndex in range(len(self.unitList)):
				screen.setState(self.unitCheckBox + str(unitIndex), False)
		else:
			self.allState = True
			self.selectedUnitList = []
			for unitIndex in range(len(self.unitList)):
				screen.setState(self.unitCheckBox + str(unitIndex), True)
				self.selectedUnitList.append(unitIndex)

		self.refreshMinimap()

	def toggleGroupCheckBoxes(self, groupIndex):
		screen = self.getScreen()
		groupID = self.groupIDs[groupIndex]
		units = self.groups[groupID].units

		if (self.groupState[groupIndex]):
			self.groupState[groupIndex] = False
			for unitIndex in units:
				if unitIndex in self.selectedUnitList:
					self.selectedUnitList.remove(unitIndex)
					screen.setState(self.unitCheckBox + str(unitIndex), False)
		else:
			self.groupState[groupIndex] = True
			for unitIndex in units:
				if unitIndex not in self.selectedUnitList:
					self.selectedUnitList.append(unitIndex)
					screen.setState(self.unitCheckBox + str(unitIndex), True)

		self.refreshMinimap()

	def groupSelectionChanged(self, groupId):
		screen = self.getScreen()

		screen.deleteWidget(self.unitBackgroundName)
		screen.addScrollPanel(self.unitBackgroundName, "", 0, 85, self.viewX + 13, self.H_SCREEN - 164, PanelStyles.PANEL_STYLE_CITY_LEFT, False, WidgetTypes.WIDGET_GENERAL, -1, -1)

		if (groupId == self.OWNER_GROUP_ID):
			self.createGroups(OwnerGroupHelper())
		elif (groupId == self.PROFESSION_GROUP_ID):
			self.createGroups(ProfessionGroupHelper())
		elif (groupId == self.STACK_GROUP_ID):
			self.createGroups(StackGroupHelper())
		elif (groupId == self.SKILL_GROUP_ID):
			self.createGroups(TypeGroupHelper())
		elif (groupId == self.LOCATION_GROUP_ID):
			self.createGroups(LocationGroupHelper())
		else:
			pass

		self.setUnitCheckBoxStates()
		self.refreshMinimap()

	def createGroups(self, groupHelper):
		self.groups = {}
		for idx in range(len(self.unitList)):
			groupId = groupHelper.getGroupId(self.unitList[idx])
			if (self.groups.has_key(groupId)):
				self.groups[groupId].addUnit(idx)
			else:
				self.groups[groupId] = UnitGroup()
				self.groups[groupId].addUnit(idx)

		self.groupState = []
		self.groupIDs = []
		keys = self.groups.keys()
		keys.sort(groupHelper.compareGroups)

		self.addGroupPanel(localText.getText("TXT_KEY_PEDIA_ALL_UNITS", ()).upper(), -1, 0, 0)
		self.allState = False

		groupIndex = 0
		verticalIndex = 1
		for groupId in keys:
			self.groupIDs.append(groupId)
			self.groupState.append(False)
			self.addGroupPanel(groupHelper.getGroupLabel(groupId), groupIndex, verticalIndex, groupId)

			units = self.groups[groupId].units
			groupIndex += 1
			verticalIndex += 1
			for idx in units:
				self.addUnitToList(self.unitList[idx], idx, verticalIndex)
				verticalIndex += 1

		self.numGroups = groupIndex

	def refreshMinimap(self):
		screen = self.getScreen()
		screen.minimapClearAllFlashingTiles()
		screen.updateMinimapVisibility()
		screen.minimapClearLayer(0)
		for iPlayer in self.selectedPlayerList:
			screen.updateMinimapColorFromMap(MinimapModeTypes.MINIMAPMODE_TERRITORY, iPlayer, 0.4)

		for unit in self.unitList:
			iColor = gc.getPlayerColorInfo(gc.getPlayer(unit.getOwner()).getPlayerColor()).getColorTypePrimary()
			screen.setMinimapColor(MinimapModeTypes.MINIMAPMODE_MILITARY, unit.plot().getX(), unit.plot().getY(), iColor, 0.7)

		for unitIndex in self.selectedUnitList:
			unit = self.unitList[unitIndex]
			iColor = gc.getPlayerColorInfo(gc.getPlayer(unit.getOwner()).getPlayerColor()).getColorTypePrimary()
			if (unit.getOwner() == self.iActivePlayer or not unit.isInvisible(gc.getGame().getActiveTeam(), False)):
				screen.minimapFlashPlot(unit.plot().getX(), unit.plot().getY(), iColor, -1)


	def compareUnits(self, unit1, unit2):
		if (unit1.getUnitType() > unit2.getUnitType()):
			return -1
		elif (unit1.getUnitType() < unit2.getUnitType()):
			return 1
		elif (unit1.getLevel() > unit2.getLevel()):
			return -1
		elif (unit1.getLevel() < unit2.getLevel()):
			return 1
		elif (unit1.getExperience() > unit2.getExperience()):
			return -1
		elif (unit1.getExperience() < unit2.getExperience()):
			return 1
		else:
			return 0

	def getNextWidgetName(self):
		szName = self.WIDGET_ID + str(self.nWidgetCount)
		self.nWidgetCount += 1
		return szName

	def update(self, fDelta):
		screen = self.getScreen()
		screen.updateMinimap(fDelta)

	def minimapClicked(self):
		self.hideScreen()

	def hideScreen(self):
		screen = self.getScreen()
		for iIndex in range(len(self.KnownLeaders)):
			screen.setState(self.LEADER_BUTTON_ID + str(self.KnownLeaders[iIndex]), False)

		self.selectedPlayerList = []
		self.selectedGroupList = []
		self.selectedUnitList = []
		self.unitList = []
		self.groups = {}
		screen.hideScreen()

	def onClose(self):
		self.refreshSelectedLeader(self.iActivePlayer)
		return 0
		
	def refreshSelectedLeader(self, iPlayer):
		bDirty = False
		if iPlayer == self.iActivePlayer:
			Units = self.ListActivePlayerUnits()
		else:
			Units = self.ListPlayerVisibleUnits(iPlayer)

		if (iPlayer in self.selectedPlayerList):
			self.selectedPlayerList.remove(iPlayer)
			if len(Units) > 0:
				bDirty = True
			for iUnit in self.selectedUnitList:
				if self.unitList[iUnit].getOwner() == iPlayer:
					self.selectedUnitList.remove(iUnit)
		else:
			self.selectedPlayerList.append(iPlayer)
			if len(Units) > 0:
				bDirty = True

		if bDirty:
			NewSelectedUnitList = []
			for iunit in self.selectedUnitList:
				NewSelectedUnitList.append(self.unitList[iunit])

			screen = self.getScreen()
			for iIndex in range(len(self.KnownLeaders)):
				screen.setState(self.LEADER_BUTTON_ID + str(self.KnownLeaders[iIndex]), (self.KnownLeaders[iIndex] in self.selectedPlayerList))

			self.unitList = []
			for iPlayer in self.selectedPlayerList:
				if iPlayer == self.iActivePlayer:
					Units = self.ListActivePlayerUnits()
				else:
					Units = self.ListPlayerVisibleUnits(iPlayer)
				for Unit in Units:
					self.unitList.append(Unit)

			self.selectedUnitList = []
			for iunit in range(len(self.unitList)):
				for punit in NewSelectedUnitList:
					if self.unitList[iunit].getID() == punit.getID():
						self.selectedUnitList.append(iunit)

			self.groupSelectionChanged(self.GROUPING_SELECTION)
		else:
			self.refreshMinimap()

	def getWidgetHelp(self, argsList):
		iScreen, eWidgetType, iData1, iData2, bOption = argsList

		if eWidgetType == WidgetTypes.WIDGET_MINIMAP_HIGHLIGHT:
			iLeader =  gc.getPlayer(iData2).getLeaderType()
			iCiv = gc.getPlayer(iData2).getCivilizationType()
			szHelp = CyGameTextMgr().parseLeaderTraits(iLeader, iCiv, False, True)
			szName = gc.getLeaderHeadInfo(gc.getPlayer(iData2).getLeaderType()).getDescription()
			szColoredName = localText.changeTextColor(szName, ColorTypes.COLOR_HIGHLIGHT_TEXT)
			return szColoredName + szHelp

		return u""

class UnitGroup:
	def __init__(self):
		self.label = ""
		self.units = []

	def setLabel(self, label):
		self.label = label

	def addUnit(self, idx):
		self.units.append(idx)

class TypeGroupHelper:
	def getGroupId(self, unit):
		return unit.getUnitType()

	def getGroupLabel(self, groupId):
		if (groupId != -1):
			return gc.getUnitInfo(groupId).getDescription()
		else:
			return localText.getText("TXT_KEY_UNIT_ADVISOR_DOMESTIC_UNITS", ())

	def compareGroups(self, groupId1, groupId2):
		return cmp(groupId1, groupId2)

class ProfessionGroupHelper:
	def getGroupId(self, unit):
		return unit.getProfession()

	def getGroupLabel(self, groupId):
		if (groupId != -1):
			return gc.getProfessionInfo(groupId).getDescription()
		else:
			return localText.getText("TXT_KEY_UNIT_ADVISOR_NO_PROFESSION", ())

	def compareGroups(self, groupId1, groupId2):
		return cmp(groupId1, groupId2)

class OwnerGroupHelper:
	def getGroupId(self, unit):
		return unit.getOwner()

	def getGroupLabel(self, groupId):
		if (groupId != -1):
			return gc.getPlayer(groupId).getName()
		else:
			return localText.getText("TXT_KEY_UNIT_ADVISOR_NO_OWNER", ())

	def compareGroups(self, groupId1, groupId2):
		return cmp(groupId1, groupId2)

class StackGroupHelper:
	SINGLE_STACK = -1

	def __init__(self):
		self.stackList = [self.SINGLE_STACK]

	def getGroupId(self, unit):
		#Europe =
		plot = unit.plot()
		if (not plot.isCity()):
			numUnits = 0
			for unitIndex in range(plot.getNumUnits()):
				if (plot.getUnit(unitIndex).getOwner() == unit.getOwner()):
					numUnits += 1
			if (numUnits == 1):
				return self.SINGLE_STACK

		plotIndex = CyMap().plotNum(plot.getX(), plot.getY())

		if (not plotIndex in self.stackList):
			self.stackList.append(plotIndex)

		return plotIndex

	def getGroupLabel(self, groupId):
		if (groupId == self.SINGLE_STACK):
			return localText.getText( "TXT_KEY_UNIT_ADVISOR_SINGLE_STACK", () )
		plot = CyMap().plotByIndex(groupId)
		if (plot.isCity()):
			return plot.getPlotCity().getName()
		else:
			return localText.getText("TXT_KEY_UNIT_ADVISOR_STACK", ()) + " (" + str(plot.getX()) + ", " + str(plot.getY()) + ")"

	def compareGroups(self, groupId1, groupId2):
		if (groupId1 == self.SINGLE_STACK):
			return 1
		elif (groupId2 == self.SINGLE_STACK):
			return -1
		elif (CyMap().plotByIndex(groupId1).isCity() and not CyMap().plotByIndex(groupId2).isCity()):
			return -1
		elif (CyMap().plotByIndex(groupId2).isCity() and not CyMap().plotByIndex(groupId1).isCity()):
			return 1
		else:
			return cmp(self.getGroupLabel(groupId1), self.getGroupLabel(groupId2))

class LocationGroupHelper:
	INVALID = -1
	CITY = 0
	HOME = 1
	FRIENDLY = 2
	NEUTRAL = 3
	ENEMY = 4
	TO_EUROPE = 5
	IN_EUROPE = 6
	FROM_EUROPE = 7
	WORKER = 8

	def getGroupId(self, unit):
		iEurope = unit.getUnitTravelState()

		if iEurope != UnitTravelStates.NO_UNIT_TRAVEL_STATE:
			if iEurope == UnitTravelStates.UNIT_TRAVEL_STATE_TO_EUROPE:
				return self.TO_EUROPE
			elif iEurope == UnitTravelStates.UNIT_TRAVEL_STATE_IN_EUROPE:
				return self.IN_EUROPE
			elif iEurope == UnitTravelStates.UNIT_TRAVEL_STATE_FROM_EUROPE:
				return self.FROM_EUROPE
			else:
				return self.INVALID
		else:
			plot = unit.plot()
			if (not plot.isOwned()):
				print "Nutral Unit"
				return self.NEUTRAL
			if (plot.getOwner() == unit.getOwner()):
				if (plot.isCity()):
					pCity = plot.getPlotCity()
					if (pCity.getPopulationUnitById(unit.getID()).isNone()):
						return self.CITY
					else:
						return self.WORKER
				else:
					return self.HOME
			else:
				unitTeam = gc.getTeam(gc.getPlayer(unit.getOwner()).getTeam())
				plotTeam = gc.getPlayer(plot.getOwner()).getTeam()

				if (unitTeam.isOpenBorders(plotTeam) or (gc.getPlayer(plot.getOwner()).isNative() or gc.getPlayer(unit.getOwner()).isNative() and not unitTeam.isAtWar(plotTeam))):
					return self.FRIENDLY
				if (unitTeam.isAtWar(plotTeam)):
					return self.ENEMY

		return self.INVALID

	def getGroupLabel(self,groupId):
		if (groupId == self.CITY):
			return localText.getText("TXT_KEY_UNIT_ADVISOR_CITY_GARRISON", ())
		elif (groupId == self.HOME):
			return localText.getText("TXT_KEY_UNIT_ADVISOR_HOME_TERRITORY", ())
		elif (groupId == self.FRIENDLY):
			return localText.getText("TXT_KEY_UNIT_ADVISOR_FRIENDLY_TERRITORY", ())
		elif (groupId == self.NEUTRAL):
			return localText.getText("TXT_KEY_UNIT_ADVISOR_NEUTRAL_TERRITORY", ())
		elif (groupId == self.ENEMY):
			return localText.getText("TXT_KEY_UNIT_ADVISOR_ENEMY_TERRITORY", ())
		elif (groupId == self.TO_EUROPE):
			return localText.getText("TXT_KEY_UNIT_ADVISOR_TO_EUROPE", ())
		elif (groupId == self.IN_EUROPE):
			return localText.getText("TXT_KEY_UNIT_ADVISOR_IN_EUROPE", ())
		elif (groupId == self.FROM_EUROPE):
			return localText.getText("TXT_KEY_UNIT_ADVISOR_FROM_EUROPE", ())
		elif (groupId == self.WORKER):
			return localText.getText("TXT_KEY_DOMESTIC_ADVISOR_STATE_CITIZEN", ())
		else:
			return "ERROR, INVALID GROUP ID (LocationGroup)"

	def compareGroups(self, groupId1, groupId2):
		return cmp(groupId1, groupId2)

