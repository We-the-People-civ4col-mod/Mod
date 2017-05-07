## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import string

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class CvPediaTerrain:
	"Civilopedia Screen for Terrain"

	def __init__(self, main):
		self.iTerrain = -1
		self.top = main

	# Screen construction function
	def interfaceScreen(self, iTerrain, x, y, h, w):

		self.iTerrain = iTerrain

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
		## R&R, Robert Surcouf,  Pedia - Start
		#self.W_STATS_PANE = (w * 35 / 100)
		#self.H_STATS_PANE = (h * 35 / 100)
		self.W_STATS_PANE = (w * 50 / 100)
		self.H_STATS_PANE = (h * 20 / 100)
		## R&R, Robert Surcouf,  Pedia - End

		self.X_SPECIAL_PANE = x
		self.Y_SPECIAL_PANE = y + self.H_ICON_PANE + (h * 1 / 100)#360#y + self.H_ICON_PANE + (h * 5 / 100)
		self.W_SPECIAL_PANE = w
		self.H_SPECIAL_PANE = 90#h - self.H_ICON_PANE - (h * 5 / 100)

		self.X_HISTORY_PANE = x
		self.Y_HISTORY_PANE = self.Y_SPECIAL_PANE + self.H_SPECIAL_PANE + 5#460
		self.W_HISTORY_PANE = w
		self.H_HISTORY_PANE = 200

		self.top.deleteAllWidgets()

		screen = self.top.getScreen()

		bNotActive = (not screen.isActive())
		if bNotActive:
			self.top.setPediaCommonWidgets()

		# Header...
		szHeader = u"<font=4b>" + gc.getTerrainInfo(self.iTerrain).getDescription().upper() + u"</font>"
		szHeaderId = self.top.getNextWidgetName()
		screen.setLabel(szHeaderId, "Background", szHeader, CvUtil.FONT_CENTER_JUSTIFY, self.top.X_SCREEN, self.top.Y_TITLE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# Top
		screen.setText(self.top.getNextWidgetName(), "Background", self.top.MENU_TEXT, CvUtil.FONT_LEFT_JUSTIFY, self.top.X_MENU, self.top.Y_MENU, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_PEDIA_MAIN, CivilopediaPageTypes.CIVILOPEDIA_PAGE_TERRAIN, -1)

		if self.top.iLastScreen	!= CvScreenEnums.PEDIA_TERRAIN or bNotActive:
			self.placeLinks(true)
			self.top.iLastScreen = CvScreenEnums.PEDIA_TERRAIN
		else:
			self.placeLinks(true)

		# Icon
		screen.addPanel( self.top.getNextWidgetName(), "", "", False, False, self.X_ICON_PANE, self.Y_ICON_PANE, self.W_ICON_PANE, self.H_ICON_PANE, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addPanel(self.top.getNextWidgetName(), "", "", false, false, self.X_ICON, self.Y_ICON, self.W_ICON, self.H_ICON, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addDDSGFC(self.top.getNextWidgetName(), gc.getTerrainInfo(self.iTerrain).getButton(), self.X_ICON + self.W_ICON / 2 - self.ICON_SIZE / 2, self.Y_ICON + self.H_ICON / 2 - self.ICON_SIZE / 2, self.ICON_SIZE, self.ICON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		self.placeStats()
		self.placeSpecial()
		self.placeHistory()

	def placeStats(self):

		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addListBoxGFC(panelName, "", self.X_STATS_PANE, self.Y_STATS_PANE, self.W_STATS_PANE, self.H_STATS_PANE, TableStyles.TABLE_STYLE_EMPTY)
#		screen.addPanel( panelName, "", "", true, true, self.X_STATS_PANE, self.Y_STATS_PANE, self.W_STATS_PANE, self.H_STATS_PANE, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.enableSelect(panelName, False)

		for k in range(YieldTypes.NUM_YIELD_TYPES):
			iYield = gc.getTerrainInfo(self.iTerrain).getYield(k)
			if (iYield != 0):
				#szYield = (u"%s: %i" % (gc.getYieldInfo(k).getDescription().upper(), iYield))
				szYield = (u"%s: %i" % (gc.getYieldInfo(k).getDescription(), iYield))
				## R&R, Robert Surcouf,  Pedia - Start
				#screen.appendListBoxString(panelName, u"<font=4>" + szYield + (u"%c" % gc.getYieldInfo(k).getChar()) + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
				screen.appendListBoxString(panelName, u"<font=3>" + szYield + (u"%c" % gc.getYieldInfo(k).getChar()) + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)				
				## R&R, Robert Surcouf,  Pedia - End
#				screen.attachTextGFC(panelName, "", szYield + (u"%c" % gc.getYieldInfo(k).getChar()), FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

	def placeSpecial(self):

		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_PEDIA_SPECIAL_ABILITIES", ()), "", true, false, self.X_SPECIAL_PANE, self.Y_SPECIAL_PANE, self.W_SPECIAL_PANE, self.H_SPECIAL_PANE, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		listName = self.top.getNextWidgetName()
		screen.attachListBoxGFC( panelName, listName, "", TableStyles.TABLE_STYLE_EMPTY )
		screen.enableSelect(listName, False)

		szSpecialText = CyGameTextMgr().getTerrainHelp(self.iTerrain, True)
		splitText = string.split( szSpecialText, "\n" )
		for special in splitText:
			if len( special ) != 0:
				screen.appendListBoxString( listName, special, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )

	def placeHistory(self):
		
		screen = self.top.getScreen()
		
		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("History", ()), "", true, true, self.X_HISTORY_PANE, self.Y_HISTORY_PANE, self.W_HISTORY_PANE, self.H_HISTORY_PANE, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		listName = self.top.getNextWidgetName()
		screen.attachListBoxGFC( panelName, listName, "", TableStyles.TABLE_STYLE_EMPTY )
		screen.enableSelect(listName, False)

		textName = self.top.getNextWidgetName()
		screen.addMultilineText( textName, gc.getTerrainInfo(self.iTerrain).getCivilopedia(), self.X_HISTORY_PANE + 15, self.Y_HISTORY_PANE + 40,
		    self.W_HISTORY_PANE - (15 * 2), self.H_HISTORY_PANE - (15 * 2) - 25, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

	def placeLinks(self, bRedraw):

		screen = self.top.getScreen()

		if bRedraw:
			screen.clearListBoxGFC(self.top.LIST_ID)

		# sort resources alphabetically
		listSorted=[(0,0)]*gc.getNumTerrainInfos()
		for j in range(gc.getNumTerrainInfos()):
			listSorted[j] = (gc.getTerrainInfo(j).getDescription(), j)
		listSorted.sort()

		iSelected = 0
		i = 0
		for iI in range(gc.getNumTerrainInfos()):
			if (not gc.getTerrainInfo(listSorted[iI][1]).isGraphicalOnly()):
				if bRedraw:
					screen.appendListBoxString(self.top.LIST_ID, listSorted[iI][0], WidgetTypes.WIDGET_PEDIA_JUMP_TO_TERRAIN, listSorted[iI][1], 0, CvUtil.FONT_LEFT_JUSTIFY )
				if listSorted[iI][1] == self.iTerrain:
					iSelected = i
				i += 1

		screen.setSelectedListBoxStringGFC(self.top.LIST_ID, iSelected)

	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		return 0


