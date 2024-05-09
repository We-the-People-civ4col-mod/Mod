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

class CvPediaFeature:
	"Civilopedia Screen for Terrain Features"

	def __init__(self, main):
		self.iFeature = -1
		self.top = main

	# Screen construction function
	def interfaceScreen(self, iFeature, x, y, h, w):

		self.iFeature = iFeature

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
		self.W_STATS_PANE = (w * 25 / 100)
		self.H_STATS_PANE = (h * 35 / 100)
		
		# WTP, ray, playing with these values
		# Also displaying the valid Terrain
		
		self.X_STATS_PANE_2 = self.X_STATS_PANE * 2
		self.Y_STATS_PANE_2 = self.Y_ICON
		self.W_STATS_PANE_2 = (w * 25 / 100)
		self.H_STATS_PANE_2 = (h * 35 / 100)

		self.H_ICON_PANE = (h * 45 / 100)
		
		self.X_SPECIAL_PANE = x
		self.Y_SPECIAL_PANE = self.Y_STATS_PANE + self.H_STATS_PANE + 30#350#y + (h * 50 / 100)
		self.W_SPECIAL_PANE = w
		self.H_SPECIAL_PANE = 100#(h * 50 / 100)

		self.X_HISTORY_PANE = x
		self.Y_HISTORY_PANE = self.Y_SPECIAL_PANE + self.H_SPECIAL_PANE + 5#450
		self.W_HISTORY_PANE = w
		self.H_HISTORY_PANE = 210

		self.top.deleteAllWidgets()

		screen = self.top.getScreen()

		bNotActive = (not screen.isActive())
		if bNotActive:
			self.top.setPediaCommonWidgets()

		# Header...
		szHeader = u"<font=4b>" + gc.getFeatureInfo(self.iFeature).getDescription().upper() + u"</font>"
		szHeaderId = self.top.getNextWidgetName()
		screen.setLabel(szHeaderId, "Background", szHeader, CvUtil.FONT_CENTER_JUSTIFY, self.top.X_SCREEN, self.top.Y_TITLE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# Top
		screen.setText(self.top.getNextWidgetName(), "Background", self.top.MENU_TEXT, CvUtil.FONT_LEFT_JUSTIFY, self.top.X_MENU, self.top.Y_MENU, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_PEDIA_MAIN, CivilopediaPageTypes.CIVILOPEDIA_PAGE_FEATURE, -1)

		if self.top.iLastScreen	!= CvScreenEnums.PEDIA_FEATURE or bNotActive:
			self.placeLinks(True)
			self.top.iLastScreen = CvScreenEnums.PEDIA_FEATURE
		else:
			self.placeLinks(True)

		# Icon
		screen.addPanel( self.top.getNextWidgetName(), "", "", False, False, self.X_ICON_PANE, self.Y_ICON_PANE, self.W_ICON_PANE, self.H_ICON_PANE, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addPanel(self.top.getNextWidgetName(), "", "", False, False, self.X_ICON, self.Y_ICON, self.W_ICON, self.H_ICON, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addDDSGFC(self.top.getNextWidgetName(), gc.getFeatureInfo(self.iFeature).getButton(), self.X_ICON + self.W_ICON / 2 - self.ICON_SIZE / 2, self.Y_ICON + self.H_ICON / 2 - self.ICON_SIZE / 2, self.ICON_SIZE, self.ICON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		self.placeStats()
		self.placeSpecial()
		self.placeHistory()

		return

	def placeStats(self):

		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addListBoxGFC(panelName, "", self.X_STATS_PANE, self.Y_STATS_PANE, self.W_STATS_PANE, self.H_STATS_PANE, TableStyles.TABLE_STYLE_EMPTY)
		screen.enableSelect(panelName, False)

		szYieldIncreaseText = localText.getText("TXT_KEY_PEDIA_YIELD_INCREASES", ())
		szYieldDecreaseText = localText.getText("TXT_KEY_PEDIA_YIELD_DECREASES", ())
		
		# first we list Yield Increases
		screen.appendListBoxString(panelName, szYieldIncreaseText, WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
		for iYield in range(YieldTypes.NUM_YIELD_TYPES):
			iYieldChange = gc.getFeatureInfo(self.iFeature).getYieldChange(iYield)
			if (iYieldChange > 0):
				if (iYieldChange > 0):
					sign = "+"
				else:
					sign = ""
				## R&R, Robert Surcouf,  Pedia - Start
				#szYield = (u"%s: %s%i " % (gc.getYieldInfo(k).getDescription().upper(), sign, iYieldChange))
				szYield = (u"%s: %s%i " % (gc.getYieldInfo(iYield).getDescription(), sign, iYieldChange))
				
				#screen.appendListBoxString(panelName, u"<font=4>" + szYield + (u"%c" % gc.getYieldInfo(k).getChar()) + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
				screen.appendListBoxString(panelName, u"<font=3>" + szYield + (u"%c" % gc.getYieldInfo(iYield).getChar()) + u"</font>", WidgetTypes.WIDGET_PEDIA_JUMP_TO_YIELDS, iYield, 1, CvUtil.FONT_LEFT_JUSTIFY)
				## R&R, Robert Surcouf,  Pedia - End
				
		# then we list Yield Decreases
		screen.appendListBoxString(panelName, szYieldDecreaseText, WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
		for iYield in range(YieldTypes.NUM_YIELD_TYPES):
			iYieldChange = gc.getFeatureInfo(self.iFeature).getYieldChange(iYield)
			if (iYieldChange < 0):
				if (iYieldChange > 0):
					sign = "+"
				else:
					sign = ""
				## R&R, Robert Surcouf,  Pedia - Start
				#szYield = (u"%s: %s%i " % (gc.getYieldInfo(k).getDescription().upper(), sign, iYieldChange))
				szYield = (u"%s: %s%i " % (gc.getYieldInfo(iYield).getDescription(), sign, iYieldChange))
				
				#screen.appendListBoxString(panelName, u"<font=4>" + szYield + (u"%c" % gc.getYieldInfo(k).getChar()) + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
				screen.appendListBoxString(panelName, u"<font=3>" + szYield + (u"%c" % gc.getYieldInfo(iYield).getChar()) + u"</font>", WidgetTypes.WIDGET_PEDIA_JUMP_TO_YIELDS, iYield, 1, CvUtil.FONT_LEFT_JUSTIFY)
				## R&R, Robert Surcouf,  Pedia - End
				
		## WTP, ray, we have pedia list the valid Terrains of Features automatically - no need for pointless Strategy texts anymore.
		panelName = self.top.getNextWidgetName()
		screen.addListBoxGFC(panelName, "", self.X_STATS_PANE_2, self.Y_STATS_PANE_2, self.W_STATS_PANE_2, self.H_STATS_PANE_2, TableStyles.TABLE_STYLE_EMPTY)
		screen.enableSelect(panelName, False)
		
		szIcon = localText.getText("[ICON_BULLET] ", ())
		
		# First the List of Terrains
		szValidTerrainText = localText.getText("TXT_KEY_PEDIA_VALID_TERRAINS_2", ())
		screen.appendListBoxString(panelName, szValidTerrainText, WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
		for iTerrain in range(TerrainTypes.NUM_TERRAIN_TYPES):
			if (gc.getFeatureInfo(self.iFeature).isTerrain(iTerrain)):
				TerrainDescription = gc.getTerrainInfo(iTerrain).getDescription()
				screen.appendListBoxString(panelName, u"<font=3>" + szIcon + TerrainDescription + u"</font>", WidgetTypes.WIDGET_PEDIA_JUMP_TO_TERRAIN, iTerrain, 1, CvUtil.FONT_LEFT_JUSTIFY)
		

	def placeSpecial(self):

		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_PEDIA_SPECIAL_ABILITIES", ()), "", True, False, self.X_SPECIAL_PANE, self.Y_SPECIAL_PANE, self.W_SPECIAL_PANE, self.H_SPECIAL_PANE, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		listName = self.top.getNextWidgetName()
		screen.attachListBoxGFC( panelName, listName, "", TableStyles.TABLE_STYLE_EMPTY )
		screen.enableSelect(listName, False)

		szSpecialText = CyGameTextMgr().getFeatureHelp(self.iFeature, True)
		splitText = string.split( szSpecialText, "\n" )
		for special in splitText:
			if len( special ) != 0:
				screen.appendListBoxString( listName, special, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )

	def placeHistory(self):
		
		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_CIVILOPEDIA_HISTORY", ()), "", True, True, self.X_HISTORY_PANE, self.Y_HISTORY_PANE, self.W_HISTORY_PANE, self.H_HISTORY_PANE, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		listName = self.top.getNextWidgetName()
		screen.attachListBoxGFC( panelName, listName, "", TableStyles.TABLE_STYLE_EMPTY )
		screen.enableSelect(listName, False)
		
		textName = self.top.getNextWidgetName()
		screen.addMultilineText( textName, gc.getFeatureInfo(self.iFeature).getCivilopedia(), self.X_HISTORY_PANE + 15, self.Y_HISTORY_PANE + 40,
		    self.W_HISTORY_PANE - (15 * 2), self.H_HISTORY_PANE - (15 * 2) - 25, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

	def placeLinks(self, bRedraw):

		screen = self.top.getScreen()

		if bRedraw:
			screen.clearListBoxGFC(self.top.LIST_ID)

		# sort resources alphabetically
		listSorted=[(0,0)]*gc.getNumFeatureInfos()
		for j in range(gc.getNumFeatureInfos()):
			listSorted[j] = (gc.getFeatureInfo(j).getDescription(), j)
		listSorted.sort(key = CvUtil.sortkey)

		iSelected = 0
		i = 0
		for iI in range(gc.getNumFeatureInfos()):
			if (not gc.getFeatureInfo(listSorted[iI][1]).isGraphicalOnly()):
				if bRedraw:
					screen.appendListBoxString( self.top.LIST_ID, listSorted[iI][0], WidgetTypes.WIDGET_PEDIA_JUMP_TO_FEATURE, listSorted[iI][1], 0, CvUtil.FONT_LEFT_JUSTIFY )
				if listSorted[iI][1] == self.iFeature:
					iSelected = i
				i += 1

		screen.setSelectedListBoxStringGFC(self.top.LIST_ID, iSelected)

	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		return 0


