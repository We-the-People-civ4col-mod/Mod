## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class CvPediaCivilization:
	"Civilopedia Screen for Civilizations"

	def __init__(self, main):
		self.iCivilization = -1
		self.top = main

	# Screen construction function
	def interfaceScreen(self, iCivilization, x, y, h, w):

		self.iCivilization = iCivilization

		self.X_ICON_PANE = x
		self.Y_ICON_PANE = y
		self.W_ICON_PANE = (w * 30 / 100)
		self.H_ICON_PANE = self.W_ICON_PANE

		self.W_ICON = self.W_ICON_PANE * 2 / 3
		self.H_ICON = self.H_ICON_PANE * 2 / 3

		self.ICON_SIZE = self.W_ICON / 2
		
		self.X_BUILDING = x + (w * 70 / 100)
		self.Y_BUILDING = y + (h * 22 / 100)
		self.W_BUILDING = (w * 30 / 100)
		self.H_BUILDING = (h * 17 / 100)

		self.X_UNIT = x + (w * 35 / 100)
		self.Y_UNIT = y + (h * 22 / 100)
		self.W_UNIT = (w * 30 / 100)
		self.H_UNIT = (h * 17 / 100)

		self.X_LEADER = x + (w * 35 / 100)
		self.Y_LEADER = y
# TAC 0201 Start		
#		self.W_LEADER = (w * 65 / 100)
		self.W_LEADER = (w * 30 / 100)
# TAC End		
		self.H_LEADER = (h * 17 / 100)

# TAC 0201 Start	
		self.X_STARTUNIT = x + (w * 70 / 100)
		self.Y_STARTUNIT = y
		self.W_STARTUNIT = (w * 30 / 100)
		self.H_STARTUNIT = (h * 17 / 100)		
#TAC End

		self.W_ICON_PANE = (w * 30 / 100)
		self.H_ICON_PANE = (h * 39 / 100)

		self.X_ICON = x + (self.W_ICON_PANE / 2) - (self.W_ICON / 2)
		self.Y_ICON = y + (self.H_ICON_PANE / 2) - (self.H_ICON / 2)

		self.X_TEXT = x
		self.Y_TEXT = y + self.H_ICON_PANE + (h * 5 / 100)
		self.W_TEXT = w
		self.H_TEXT = (h * 56 / 100)

		self.top.deleteAllWidgets()
		screen = self.top.getScreen()

		bNotActive = (not screen.isActive())
		if bNotActive:
			self.top.setPediaCommonWidgets()

		# Header...
		szHeader = u"<font=4b>" + gc.getCivilizationInfo(self.iCivilization).getDescription().upper() + u"</font>"
		szHeaderId = self.top.getNextWidgetName()
		screen.setLabel(szHeaderId, "Background", szHeader, CvUtil.FONT_CENTER_JUSTIFY, self.top.X_SCREEN, self.top.Y_TITLE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# Top
		screen.setText(self.top.getNextWidgetName(), "Background", self.top.MENU_TEXT, CvUtil.FONT_LEFT_JUSTIFY, self.top.X_MENU, self.top.Y_MENU, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_PEDIA_MAIN, CivilopediaPageTypes.CIVILOPEDIA_PAGE_CIV, -1)
		if self.top.iLastScreen	!= CvScreenEnums.PEDIA_CIVILIZATION or bNotActive:
			self.placeLinks(True)
			self.top.iLastScreen = CvScreenEnums.PEDIA_CIVILIZATION
		else:
			self.placeLinks(True)

		# Icon
		screen.addPanel( self.top.getNextWidgetName(), "", "", False, False, self.X_ICON_PANE, self.Y_ICON_PANE, self.W_ICON_PANE, self.H_ICON_PANE, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addPanel(self.top.getNextWidgetName(), "", "", false, false, self.X_ICON, self.Y_ICON, self.W_ICON, self.H_ICON, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addDDSGFC(self.top.getNextWidgetName(), ArtFileMgr.getCivilizationArtInfo(gc.getCivilizationInfo(self.iCivilization).getArtDefineTag()).getButton(), self.X_ICON + self.W_ICON / 2 - self.ICON_SIZE / 2, self.Y_ICON + self.H_ICON / 2 - self.ICON_SIZE / 2, self.ICON_SIZE, self.ICON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		
		self.placeBuilding()
		self.placeUnit()
		self.placeLeader()
# TAC 0201 Start
		if gc.getCivilizationInfo(self.iCivilization).isPlayable():
			self.placeStartunit()
# TAC End		
		self.placeText()

		return
	def placeBuilding(self):

		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_UNIQUE_BUILDINGS", ()), "", false, True, self.X_BUILDING, self.Y_BUILDING, self.W_BUILDING, self.H_BUILDING, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.attachLabel(panelName, "", "  ")

		for iBuilding in range(gc.getNumBuildingClassInfos()):
			iUniqueBuilding = gc.getCivilizationInfo(self.iCivilization).getCivilizationBuildings(iBuilding);
			iDefaultBuilding = gc.getBuildingClassInfo(iBuilding).getDefaultBuildingIndex();
			if (iDefaultBuilding > -1 and iUniqueBuilding > -1 and iDefaultBuilding != iUniqueBuilding):
				screen.attachImageButton( panelName, "", gc.getBuildingInfo(iUniqueBuilding).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING, iUniqueBuilding, 1, False )

	def placeUnit(self):

		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_FREE_UNITS", ()), "", false, True, self.X_UNIT, self.Y_UNIT, self.W_UNIT, self.H_UNIT, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.attachLabel(panelName, "", "  ")

		for iUnit in range(gc.getNumUnitClassInfos()):
			iUniqueUnit = gc.getCivilizationInfo(self.iCivilization).getCivilizationUnits(iUnit);
			iDefaultUnit = gc.getUnitClassInfo(iUnit).getDefaultUnitIndex();
			if (iDefaultUnit > -1 and iUniqueUnit > -1 and iDefaultUnit != iUniqueUnit):
				szButton = gc.getUnitInfo(iUniqueUnit).getButton()
				if self.top.iActivePlayer != -1:
					szButton = gc.getPlayer(self.top.iActivePlayer).getUnitButton(iUniqueUnit)
				screen.attachImageButton( panelName, "", szButton, GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_UNIT, iUniqueUnit, -1, False )

	def placeLeader(self):

		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_LEADERS", ()), "", false, True, self.X_LEADER, self.Y_LEADER, self.W_LEADER, self.H_LEADER, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.attachLabel(panelName, "", "  ")

		for iLeader in range(gc.getNumLeaderHeadInfos()):
			civ = gc.getCivilizationInfo(self.iCivilization)
			if civ.isLeaders(iLeader):
				screen.attachImageButton( panelName, "", gc.getLeaderHeadInfo(iLeader).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_LEADER, iLeader, self.iCivilization, False )

# TAC 0201 Start
	def placeStartunit(self):

		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_DAWN_OF_MAN_SCREEN_STARTING_UNITS", ()), "", false, True, self.X_STARTUNIT, self.Y_STARTUNIT, self.W_STARTUNIT, self.H_STARTUNIT, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.attachLabel(panelName, "", "  ")

		for iUnit in range(gc.getCivilizationInfo(self.iCivilization).getNumCivilizationFreeUnits()):
			iFreeUnitClass = gc.getCivilizationInfo(self.iCivilization).getCivilizationFreeUnitsClass(iUnit);
			iFreeUnit = gc.getUnitClassInfo(iFreeUnitClass).getDefaultUnitIndex()
			szButton = gc.getUnitInfo(iFreeUnit).getButton()
			screen.attachImageButton( panelName, "", szButton, GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_UNIT, iFreeUnit, -1, False )

# TAC End				
				
	def placeText(self):

		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_CIVILOPEDIA_HISTORY", ()), "", True, True, self.X_TEXT, self.Y_TEXT, self.W_TEXT, self.H_TEXT, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		szText = gc.getCivilizationInfo(self.iCivilization).getCivilopedia()
		listName = self.top.getNextWidgetName()
		screen.addMultilineText(listName, szText, self.X_TEXT + 15, self.Y_TEXT + 40, self.W_TEXT - 30, self.H_TEXT - 60, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

	def placeLinks(self, bRedraw):

		screen = self.top.getScreen()

		if bRedraw:
			screen.clearListBoxGFC(self.top.LIST_ID)

		# sort Improvements alphabetically
		listSorted=[(0,0)]*gc.getNumCivilizationInfos()
		for j in range(gc.getNumCivilizationInfos()):
			listSorted[j] = (gc.getCivilizationInfo(j).getDescription(), j)
		listSorted.sort(key = CvUtil.sortkey)

		iSelected = 0
		i = 0
		for iI in range(gc.getNumCivilizationInfos()):
			# < JAnimals Mod Start 1/1 > AND # R&R, ray, Church
			# if (not gc.getCivilizationInfo(listSorted[iI][1]).isEurope() and not listSorted[iI][1] == gc.getInfoTypeForString("CIVILIZATION_BARBARIAN") and not gc.getCivilizationInfo(listSorted[iI][1]).isGraphicalOnly()):
			if (not gc.getCivilizationInfo(listSorted[iI][1]).isEurope() and not listSorted[iI][1] == gc.getInfoTypeForString("CIVILIZATION_BARBARIAN") and not listSorted[iI][1] == gc.getInfoTypeForString("CIVILIZATION_CHURCH") and not gc.getCivilizationInfo(listSorted[iI][1]).isGraphicalOnly()):
				if (not gc.getDefineINT("CIVILOPEDIA_SHOW_ACTIVE_CIVS_ONLY") or not gc.getGame().isFinalInitialized() or gc.getGame().isCivEverActive(listSorted[iI][1])):
					if bRedraw:
						screen.appendListBoxStringNoUpdate(self.top.LIST_ID, listSorted[iI][0], WidgetTypes.WIDGET_PEDIA_JUMP_TO_CIV, listSorted[iI][1], 0, CvUtil.FONT_LEFT_JUSTIFY )
					if listSorted[iI][1] == self.iCivilization:
						iSelected = i
					i += 1

		if bRedraw:
			screen.updateListBox(self.top.LIST_ID)

		screen.setSelectedListBoxStringGFC(self.top.LIST_ID, iSelected)

	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		return 0


