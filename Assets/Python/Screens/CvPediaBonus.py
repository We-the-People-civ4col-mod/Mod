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

class CvPediaBonus:
	"Civilopedia Screen for Bonus Resources"

	def __init__(self, main):
		self.iBonus = -1
		self.top = main

	# Screen construction function
	def interfaceScreen(self, iBonus, x, y, h, w):

		self.iBonus = iBonus
		
		self.X_ROTATION_BONUS_ANIMATION = -20
		self.Z_ROTATION_BONUS_ANIMATION = 30
		self.SCALE_ANIMATION = 0.6

		self.X_ICON_PANE = x
		self.Y_ICON_PANE = y
		self.W_ICON_PANE = (w * 30 / 100)
		self.H_ICON_PANE = self.W_ICON_PANE

		self.W_ICON = self.W_ICON_PANE * 2 / 3
		self.H_ICON = self.H_ICON_PANE * 2 / 3
		self.X_ICON = x + (self.W_ICON_PANE / 2) - (self.W_ICON / 2)
		self.Y_ICON = y + (self.H_ICON_PANE / 2) - (self.H_ICON / 2)
		
		self.ICON_SIZE = self.W_ICON / 2
		self.W_ICON_PANE = (w * 55 / 100)

		self.X_BONUS_ANIMATION = x + (w * 60 / 100)
		self.Y_BONUS_ANIMATION = y
		self.W_BONUS_ANIMATION = (w * 40 / 100)
		self.H_BONUS_ANIMATION = self.H_ICON_PANE

		# WTP, ray, lowering for scroll bar to look better
		self.X_STATS_PANE = self.X_ICON + self.W_ICON + (w * 2 / 100)
		self.Y_STATS_PANE = self.Y_ICON
		#self.W_STATS_PANE = (w * 55 / 100)
		self.W_STATS_PANE = (w * 25 / 100)
		self.H_STATS_PANE = (h * 30 / 100)

		self.X_HISTORY_PANE = x
		self.Y_HISTORY_PANE = y + self.H_ICON_PANE + (h * 5 / 100)
		self.W_HISTORY_PANE = w
		self.H_HISTORY_PANE = y + h - self.Y_HISTORY_PANE

		self.top.deleteAllWidgets()

		screen = self.top.getScreen()

		bNotActive = (not screen.isActive())
		if bNotActive:
			self.top.setPediaCommonWidgets()

		# Header...
		szHeader = u"<font=4b>" + gc.getBonusInfo(self.iBonus).getDescription().upper() + u"</font>"
		szHeaderId = self.top.getNextWidgetName()
		screen.setLabel(szHeaderId, "Background", szHeader, CvUtil.FONT_CENTER_JUSTIFY, self.top.X_SCREEN, self.top.Y_TITLE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, CivilopediaPageTypes.CIVILOPEDIA_PAGE_BONUS, iBonus)

		# Top
		screen.setText(self.top.getNextWidgetName(), "Background", self.top.MENU_TEXT, CvUtil.FONT_LEFT_JUSTIFY, self.top.X_MENU, self.top.Y_MENU, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_PEDIA_MAIN, CivilopediaPageTypes.CIVILOPEDIA_PAGE_BONUS, -1)
		if self.top.iLastScreen	!= CvScreenEnums.PEDIA_BONUS or bNotActive:
			self.placeLinks(True)
			self.top.iLastScreen = CvScreenEnums.PEDIA_BONUS
		else:
			self.placeLinks(True)

		# Icon
		screen.addPanel( self.top.getNextWidgetName(), "", "", False, False, self.X_ICON_PANE, self.Y_ICON_PANE, self.W_ICON_PANE, self.H_ICON_PANE, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addPanel(self.top.getNextWidgetName(), "", "", False, False, self.X_ICON, self.Y_ICON, self.W_ICON, self.H_ICON, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addDDSGFC(self.top.getNextWidgetName(), gc.getBonusInfo(self.iBonus).getButton(), self.X_ICON + self.W_ICON / 2 - self.ICON_SIZE / 2, self.Y_ICON + self.H_ICON / 2 - self.ICON_SIZE / 2, self.ICON_SIZE, self.ICON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		# Bonus animation
		screen.addBonusGraphicGFC(self.top.getNextWidgetName(), self.iBonus, self.X_BONUS_ANIMATION, self.Y_BONUS_ANIMATION, self.W_BONUS_ANIMATION, self.H_BONUS_ANIMATION, WidgetTypes.WIDGET_GENERAL, -1, -1, self.X_ROTATION_BONUS_ANIMATION, self.Z_ROTATION_BONUS_ANIMATION, self.SCALE_ANIMATION, True)

		self.placeStats()
		self.placeHistory()

		return

	def placeStats(self):

		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addListBoxGFC(panelName, "", self.X_STATS_PANE, self.Y_STATS_PANE, self.W_STATS_PANE, self.H_STATS_PANE, TableStyles.TABLE_STYLE_EMPTY)
		screen.enableSelect(panelName, False)

		for iYield in range(YieldTypes.NUM_YIELD_TYPES):
			iYieldChange = gc.getBonusInfo(self.iBonus).getYieldChange(iYield)
			if (iYieldChange != 0):
				if (iYieldChange > 0):
					sign = "+"
				else:
					sign = ""

				szYield = (u"%s: %s%i " % (gc.getYieldInfo(iYield).getDescription(), sign, iYieldChange))
				## R&R, Robert Surcouf,  Pedia - Start
				#screen.appendListBoxString(panelName, u"<font=4>" + szYield.upper() + (u"%c" % gc.getYieldInfo(k).getChar()) + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
				screen.appendListBoxString(panelName, u"<font=3>" + szYield + (u"%c" % gc.getYieldInfo(iYield).getChar()) + u"</font>", WidgetTypes.WIDGET_PEDIA_JUMP_TO_YIELDS, iYield, 1, CvUtil.FONT_LEFT_JUSTIFY)
				## R&R, Robert Surcouf,  Pedia - End

		## WTP, ray, Health from specific Bonus Ressources if worked
		iHealthChangeinCityRadius = gc.getBonusInfo(self.iBonus).getHealthEffectFromRessource()
		if (iHealthChangeinCityRadius > 0):
			szHealthText = localText.getText("TXT_KEY_HEALTH_FROM_RESSOURCE", (iHealthChangeinCityRadius, gc.getYieldInfo(YieldTypes.YIELD_HEALTH).getChar()))
			screen.appendListBoxString(panelName, u"<font=3>" + szHealthText + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
			
		elif (iHealthChangeinCityRadius < 0):
			szHealthText = localText.getText("TXT_KEY_HEALTH_FROM_RESSOURCE_NEGATIVE", (iHealthChangeinCityRadius, gc.getYieldInfo(YieldTypes.YIELD_HEALTH).getChar()))
			screen.appendListBoxString(panelName, u"<font=3>" + szHealthText + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
		
		## WTP, ray, we have pedia list the valid Terrains and Features automatically - no need for pointless Strategy texts anymore.

		## WTP, here are the flags for Water, Hills, Flatlands and Peaks
		bValidWater = False
		bValidFlatland = gc.getBonusInfo(self.iBonus).isFlatlands()
		bValidHills = gc.getBonusInfo(self.iBonus).isHills()
		bValidPeaks = gc.getBonusInfo(self.iBonus).isPeaks()

		## WTP, here are the flags for Water, Hills, Flatlands and Peaks
		## we need to do a more complicated check for water
		for iTerrain in range(TerrainTypes.NUM_TERRAIN_TYPES):
			if (gc.getBonusInfo(self.iBonus).isTerrain(iTerrain) and gc.getTerrainInfo(iTerrain).isWater()):
				bValidWater = True

		if (bValidWater):
			sWaterText = localText.getText("TXT_BONUS_RESOURCE_IS_WATER", ())
			screen.appendListBoxString(panelName, u"<font=3>" + sWaterText + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
			#sNewline = localText.getText("[NEWLINE]", ())
			#screen.appendListBoxString(panelName, u"<font=3>" + sNewline + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
		else:
			if (bValidFlatland):
				sFlatlandText = localText.getText("TXT_BONUS_RESOURCE_IS_FLATLAND", ())
				screen.appendListBoxString(panelName, u"<font=3>" + sFlatlandText + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
			if (bValidHills):
				sHillsText = localText.getText("TXT_BONUS_RESOURCE_IS_HILLS", ())
				screen.appendListBoxString(panelName, u"<font=3>" + sHillsText + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
			if (bValidPeaks):
				sPeakText = localText.getText("TXT_BONUS_RESOURCE_IS_PEAKS", ())
				screen.appendListBoxString(panelName, u"<font=3>" + sPeakText + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
			#sNewline = localText.getText("[NEWLINE]", ())
			#screen.appendListBoxString(panelName, u"<font=3>" + sNewline + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)

		szIcon = localText.getText("[ICON_BULLET] ", ())
		
		# First the List of Terrains
		szValidTerrainText = localText.getText("TXT_KEY_PEDIA_VALID_TERRAINS", ())
		screen.appendListBoxString(panelName, szValidTerrainText, WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
		for iTerrain in range(TerrainTypes.NUM_TERRAIN_TYPES):
			if (gc.getBonusInfo(self.iBonus).isTerrain(iTerrain)):
				TerrainDescription = gc.getTerrainInfo(iTerrain).getDescription()
				screen.appendListBoxString(panelName, u"<font=3>" + szIcon + TerrainDescription + u"</font>", WidgetTypes.WIDGET_PEDIA_JUMP_TO_TERRAIN, iTerrain, 1, CvUtil.FONT_LEFT_JUSTIFY)
		
		# Then the List of Features
		szValidFeatureText = localText.getText("TXT_KEY_PEDIA_VALID_FEATURES", ())
		screen.appendListBoxString(panelName, szValidFeatureText, WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
		for iFeature in range(FeatureTypes.NUM_FEATURE_TYPES):
			if (gc.getBonusInfo(self.iBonus).isFeature(iFeature)):
				FeatureDescription = gc.getFeatureInfo(iFeature).getDescription()
				screen.appendListBoxString(panelName, u"<font=3>"  + szIcon +  FeatureDescription + u"</font>", WidgetTypes.WIDGET_PEDIA_JUMP_TO_FEATURE, iFeature, 1, CvUtil.FONT_LEFT_JUSTIFY)
		
		
	def placeHistory(self):

		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_CIVILOPEDIA_HISTORY", ()), "", True, True,self.X_HISTORY_PANE, self.Y_HISTORY_PANE, self.W_HISTORY_PANE, self.H_HISTORY_PANE, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1)

		screen.attachLabel(panelName, "", "  ")

		textName = self.top.getNextWidgetName()
		screen.addMultilineText( textName, gc.getBonusInfo(self.iBonus).getCivilopedia(), self.X_HISTORY_PANE + 15, self.Y_HISTORY_PANE + 40,
		    self.W_HISTORY_PANE - (15 * 2), self.H_HISTORY_PANE - (15 * 2) - 25, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

	def placeLinks(self, bRedraw):

		screen = self.top.getScreen()

		if bRedraw:
			screen.clearListBoxGFC(self.top.LIST_ID)

		# sort resources alphabetically
		rowListName=[(0,0)]*gc.getNumBonusInfos()
		for j in range(gc.getNumBonusInfos()):
			rowListName[j] = (gc.getBonusInfo(j).getDescription(), j)
		rowListName.sort(key = CvUtil.sortkey)

		iSelected = 0
		i = 0
		for iI in range(gc.getNumBonusInfos()):
			if (not gc.getBonusInfo(rowListName[iI][1]).isGraphicalOnly()):
				if bRedraw:
					screen.appendListBoxString(self.top.LIST_ID, rowListName[iI][0], WidgetTypes.WIDGET_PEDIA_JUMP_TO_BONUS, rowListName[iI][1], 0, CvUtil.FONT_LEFT_JUSTIFY )
				if rowListName[iI][1] == self.iBonus:
					iSelected = i
				i += 1

		screen.setSelectedListBoxStringGFC(self.top.LIST_ID, iSelected)

	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		return 0


