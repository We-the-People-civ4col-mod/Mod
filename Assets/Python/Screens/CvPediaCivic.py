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

class CvPediaCivic:
	"Civilopedia Screen for Civics"

	def __init__(self, main):
		self.iCivic = -1
		self.top = main

	# Screen construction function
	def interfaceScreen(self, iCivic, x, y, h, w):

		self.iCivic = iCivic

		self.X_ICON_PANE = x
		self.Y_ICON_PANE = y
		self.W_ICON_PANE = (w * 30 / 100)
		self.H_ICON_PANE = self.W_ICON_PANE

		self.W_ICON = self.W_ICON_PANE * 2 / 3
		self.H_ICON = self.H_ICON_PANE * 2 / 3
		self.X_ICON = x + (self.W_ICON_PANE / 2) - (self.W_ICON / 2)
		self.Y_ICON = y + (self.H_ICON_PANE / 2) - (self.H_ICON / 2)
		
		self.ICON_SIZE = self.W_ICON / 2
		self.W_ICON_PANE = w

		self.X_STATS_PANE = self.X_ICON + self.W_ICON + (w * 2 / 100)
		self.Y_STATS_PANE = self.Y_ICON
		self.W_STATS_PANE = (w * 55 / 100)
		self.H_STATS_PANE = (h * 30 / 100)

		self.X_SPECIAL = x
		self.Y_SPECIAL = y + self.H_ICON_PANE + (h * 5 / 100)
		self.W_SPECIAL = w
		self.H_SPECIAL = (h * 20 / 100)

		self.X_TEXT = x
		self.Y_TEXT = y + self.H_ICON_PANE + self.H_SPECIAL + (h * 10 / 100)
		self.W_TEXT = w
		self.H_TEXT = y + h - self.Y_TEXT

		self.top.deleteAllWidgets()
		screen = self.top.getScreen()

		bNotActive = (not screen.isActive())
		if bNotActive:
			self.top.setPediaCommonWidgets()

		# Header...
		szHeader = u"<font=4b>" + gc.getCivicInfo(self.iCivic).getDescription().upper() + u"</font>"
		szHeaderId = self.top.getNextWidgetName()
		screen.setLabel(szHeaderId, "Background", szHeader, CvUtil.FONT_CENTER_JUSTIFY, self.top.X_SCREEN, self.top.Y_TITLE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# Top
		screen.setText(self.top.getNextWidgetName(), "Background", self.top.MENU_TEXT, CvUtil.FONT_LEFT_JUSTIFY, self.top.X_MENU, self.top.Y_MENU, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_PEDIA_MAIN, CivilopediaPageTypes.CIVILOPEDIA_PAGE_CIVIC, -1)
		if self.top.iLastScreen	!= CvScreenEnums.PEDIA_CIVIC or bNotActive:
			self.placeLinks(true)
			self.top.iLastScreen = CvScreenEnums.PEDIA_CIVIC
		else:
			self.placeLinks(true)

		# Icon
		screen.addPanel( self.top.getNextWidgetName(), "", "", False, False, self.X_ICON_PANE, self.Y_ICON_PANE, self.W_ICON_PANE, self.H_ICON_PANE, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addPanel(self.top.getNextWidgetName(), "", "", false, false, self.X_ICON, self.Y_ICON, self.W_ICON, self.H_ICON, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addDDSGFC(self.top.getNextWidgetName(), gc.getCivicInfo(self.iCivic).getButton(), self.X_ICON + self.W_ICON / 2 - self.ICON_SIZE / 2, self.Y_ICON + self.H_ICON / 2 - self.ICON_SIZE / 2, self.ICON_SIZE, self.ICON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		self.placeStats()
		self.placeSpecial()
		self.placeText()

	def placeStats(self):
		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addListBoxGFC(panelName, "", self.X_STATS_PANE, self.Y_STATS_PANE, self.W_STATS_PANE, self.H_STATS_PANE, TableStyles.TABLE_STYLE_EMPTY)
		screen.enableSelect(panelName, False)

		iCivicOptionType = gc.getCivicInfo(self.iCivic).getCivicOptionType()
		if (iCivicOptionType != -1):
			screen.appendListBoxString(panelName, u"<font=4>" + gc.getCivicOptionInfo(iCivicOptionType).getDescription().upper() + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)

	def placeSpecial(self):
		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_PEDIA_EFFECTS", ()), "", true, false, self.X_SPECIAL, self.Y_SPECIAL, self.W_SPECIAL, self.H_SPECIAL, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		listName = self.top.getNextWidgetName()
		screen.attachListBoxGFC( panelName, listName, "", TableStyles.TABLE_STYLE_EMPTY )
		screen.enableSelect(listName, False)
		
		szSpecialText = CyGameTextMgr().parseCivicInfo(self.iCivic, True, False, True)
		screen.addMultilineText(listName, szSpecialText[1:], self.X_SPECIAL+5, self.Y_SPECIAL+30, self.W_SPECIAL-10, self.H_SPECIAL-30, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

	def placeText(self):
		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_CIVILOPEDIA_HISTORY", ()), "", true, true, self.X_TEXT, self.Y_TEXT, self.W_TEXT, self.H_TEXT, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		
		szText = gc.getCivicInfo(self.iCivic).getCivilopedia()
		screen.attachMultilineText( panelName, "Text", szText, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

	def placeLinks(self, bRedraw):

		screen = self.top.getScreen()

		if bRedraw:
			screen.clearListBoxGFC(self.top.LIST_ID)

		# sort Improvements alphabetically
		listSorted=[(0,0)]*gc.getNumCivicInfos()
		for j in range(gc.getNumCivicInfos()):
			listSorted[j] = (gc.getCivicInfo(j).getDescription(), j)
		listSorted.sort()

		iSelected = 0
		i = 0
		for iI in range(gc.getNumCivicInfos()):
			if (not gc.getCivicInfo(listSorted[iI][1]).isGraphicalOnly()):
				if bRedraw:
					screen.appendListBoxString(self.top.LIST_ID, listSorted[iI][0], WidgetTypes.WIDGET_PEDIA_JUMP_TO_CIVIC, listSorted[iI][1], 0, CvUtil.FONT_LEFT_JUSTIFY )
				if listSorted[iI][1] == self.iCivic:
					iSelected = i
				i += 1

		screen.setSelectedListBoxStringGFC(self.top.LIST_ID, iSelected)

	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		return 0


