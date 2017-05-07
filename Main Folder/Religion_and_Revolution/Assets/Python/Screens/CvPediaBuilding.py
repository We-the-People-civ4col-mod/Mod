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

class CvPediaBuilding:
	"Civilopedia Screen for Buildings"

	def __init__(self, main):
		self.iBuilding = -1
		self.top = main

	# Screen construction function
	def interfaceScreen(self, iBuilding, x, y, h, w):
		
		# TAC/Ronnar
		self.XResolution = self.top.getScreen().getXResolution()
		# TAC
		self.iBuilding = iBuilding

		self.X_BUILDING_PANE = x
		self.Y_BUILDING_PANE = y
		self.W_BUILDING_PANE = (w * 55 / 100)
		self.H_BUILDING_PANE = (h * 35 / 100)

		self.X_BUILDING_ANIMATION = x + self.W_BUILDING_PANE + (w * 5 / 100)
		self.Y_BUILDING_ANIMATION = y
		self.W_BUILDING_ANIMATION = (w * 40 / 100)
		self.H_BUILDING_ANIMATION = (h * 35 / 100)
		
		self.X_ROTATION_BUILDING_ANIMATION = -20
		self.Z_ROTATION_BUILDING_ANIMATION = 30
		self.SCALE_ANIMATION = 1.0

		self.X_ICON_PANE = x
		self.Y_ICON_PANE = y
		self.W_ICON_PANE = (w * 30 / 100)
		self.H_ICON_PANE = self.W_ICON_PANE

		self.W_ICON = self.W_ICON_PANE * 2 / 3
		self.H_ICON = self.H_ICON_PANE * 2 / 3
		self.X_ICON = x + (self.W_ICON_PANE / 2) - (self.W_ICON / 2)
		self.Y_ICON = y + (self.H_BUILDING_ANIMATION / 2) - (self.H_ICON / 2)
	
		self.X_STATS_PANE = self.X_ICON + self.W_ICON + (w * 2 / 100)
		self.Y_STATS_PANE = self.Y_ICON
		## R&R, Robert Surcouf,  Pedia - Start
		#self.W_STATS_PANE = (w * 35 / 100)
		#self.H_STATS_PANE = (h * 30 / 100)
		self.W_STATS_PANE = (w * 30 / 100)
		self.H_STATS_PANE = (h * 25 / 100)
		## R&R, Robert Surcouf,  Pedia - End
		
		self.ICON_SIZE = self.W_ICON / 2
		self.W_ICON_PANE = w

		self.X_PREREQ_PANE = x
		self.Y_PREREQ_PANE = self.Y_BUILDING_PANE + (h * 65 / 100)
		self.W_PREREQ_PANE = (w * 45 / 100)
		self.H_PREREQ_PANE = (h * 65 / 100)

		self.X_SPECIAL_PANE = x
		self.Y_SPECIAL_PANE = y + self.H_BUILDING_PANE + (h * 5 / 100)
		self.W_SPECIAL_PANE = (w * 40 / 100)
		self.H_SPECIAL_PANE = (h * 60 / 100)

		self.X_HISTORY_PANE = x + (w * 45 / 100)
		self.Y_HISTORY_PANE = y + self.H_BUILDING_PANE + (h * 5 / 100)
		self.W_HISTORY_PANE = (w * 55 / 100)
		self.H_HISTORY_PANE = (h * 60 / 100)


		self.top.deleteAllWidgets()
		screen = self.top.getScreen()

		bNotActive = (not screen.isActive())
		if bNotActive:
			self.top.setPediaCommonWidgets()

		# Header...
		szHeader = u"<font=4b>" + gc.getBuildingInfo(self.iBuilding).getDescription().upper() + u"</font>"
		szHeaderId = self.top.getNextWidgetName()
		screen.setLabel(szHeaderId, "Background", szHeader, CvUtil.FONT_CENTER_JUSTIFY, self.top.X_SCREEN, self.top.Y_TITLE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, CivilopediaPageTypes.CIVILOPEDIA_PAGE_BUILDING, iBuilding)

		# Top
		link = CivilopediaPageTypes.CIVILOPEDIA_PAGE_BUILDING
		screen.setText(self.top.getNextWidgetName(), "Background", self.top.MENU_TEXT, CvUtil.FONT_LEFT_JUSTIFY, self.top.X_MENU, self.top.Y_MENU, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_PEDIA_MAIN, link, -1)
		self.placeLinks(true)

		screen.addPanel( self.top.getNextWidgetName(), "", "", False, False, self.X_BUILDING_PANE, self.Y_BUILDING_PANE, self.W_BUILDING_PANE, self.H_BUILDING_PANE, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# Icon
		screen.addPanel(self.top.getNextWidgetName(), "", "", false, false, self.X_ICON, self.Y_ICON, self.W_ICON, self.H_ICON, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addDDSGFC(self.top.getNextWidgetName(), gc.getBuildingInfo(self.iBuilding).getButton(), self.X_ICON + self.W_ICON / 2 - self.ICON_SIZE / 2, self.Y_ICON + self.H_ICON / 2 - self.ICON_SIZE / 2, self.ICON_SIZE, self.ICON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		# Unit animation
		screen.addBuildingGraphicGFC(self.top.getNextWidgetName(), self.iBuilding, self.X_BUILDING_ANIMATION, self.Y_BUILDING_ANIMATION, self.W_BUILDING_ANIMATION, self.H_BUILDING_ANIMATION, WidgetTypes.WIDGET_GENERAL, -1, -1, self.X_ROTATION_BUILDING_ANIMATION, self.Z_ROTATION_BUILDING_ANIMATION, self.SCALE_ANIMATION, True)

		self.placeStats()
		self.placeSpecial()
		self.placeHistory()


	# Place great people modifiers
	def placeStats(self):

		screen = self.top.getScreen()

		buildingInfo = gc.getBuildingInfo(self.iBuilding)

		panelName = self.top.getNextWidgetName()

		screen.addListBoxGFC(panelName, "", self.X_STATS_PANE, self.Y_STATS_PANE, self.W_STATS_PANE, self.H_STATS_PANE, TableStyles.TABLE_STYLE_EMPTY)
		screen.enableSelect(panelName, False)

		for iYield in range(YieldTypes.NUM_YIELD_TYPES):
			if (buildingInfo.getYieldCost(iYield) > 0):
				if self.top.iActivePlayer == -1:
					szCost = localText.getText("TXT_KEY_PEDIA_COST", ((buildingInfo.getYieldCost(iYield) * gc.getDefineINT("BUILDING_PRODUCTION_PERCENT"))/100, ))
				else:
					szCost = localText.getText("TXT_KEY_PEDIA_COST", (gc.getPlayer(self.top.iActivePlayer).getBuildingYieldProductionNeeded(self.iBuilding, iYield), ))
				## R&R, Robert Surcouf,  Pedia - Start
				screen.appendListBoxStringNoUpdate(panelName, u"<font=3>" + szCost + u"%c" % gc.getYieldInfo(iYield).getChar() + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
				#screen.appendListBoxStringNoUpdate(panelName, u"<font=4>" + szCost.upper() + u"%c" % gc.getYieldInfo(iYield).getChar() + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
				## R&R, Robert Surcouf,  Pedia - End
				
#MultipleYieldsProduced Start

		if (buildingInfo.getProfessionOutput() > 0):
			for iProfession in range(gc.getNumProfessionInfos()):
				if(gc.getProfessionInfo(iProfession).getSpecialBuilding() == buildingInfo.getSpecialBuildingType()):
					szText = localText.getText("TXT_KEY_BUILDING_PROFESSION_OUTPUT", (buildingInfo.getProfessionOutput(), gc.getYieldInfo(gc.getProfessionInfo(iProfession).getYieldsProduced(0)).getChar()))
					## R&R, Robert Surcouf,  Pedia - Start
					#screen.appendListBoxStringNoUpdate(panelName, u"<font=4>" + szText.upper() + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
					screen.appendListBoxStringNoUpdate(panelName, u"<font=3>" + szText + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
					## R&R, Robert Surcouf,  Pedia - End
					
#MultipleYieldsProduced End
		for iYield in range(YieldTypes.NUM_YIELD_TYPES):
			if(buildingInfo.getYieldModifier(iYield) > 0):
				## R&R, Robert Surcouf,  Pedia - Start
				#screen.appendListBoxStringNoUpdate(panelName, u"<font=4>" + u"+%d%% %c" % (buildingInfo.getYieldModifier(iYield), gc.getYieldInfo(iYield).getChar()) + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
				screen.appendListBoxStringNoUpdate(panelName, u"<font=3>" + u"+%d%% %c" % (buildingInfo.getYieldModifier(iYield), gc.getYieldInfo(iYield).getChar()) + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
				## R&R, Robert Surcouf,  Pedia - End
				
		for k in range(YieldTypes.NUM_YIELD_TYPES):
			if (buildingInfo.getYieldChange(k) != 0):
				if (buildingInfo.getYieldChange(k) > 0):
					szSign = "+"
				else:
					szSign = ""

				szYield = gc.getYieldInfo(k).getDescription() + ": "

				szText1 = szYield.upper() + szSign + str(buildingInfo.getYieldChange(k))
				szText2 = szText1 + (u"%c" % (gc.getYieldInfo(k).getChar()))
				## R&R, Robert Surcouf,  Pedia - Start
				#screen.appendListBoxStringNoUpdate(panelName, u"<font=4>" + szText2 + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
				screen.appendListBoxStringNoUpdate(panelName, u"<font=3>" + szText2 + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
				## R&R, Robert Surcouf,  Pedia - End
				
		# TAC/Ronnar: Add number of max workers per building
		if (self.XResolution >= 1280):
			iMaxWorkers = gc.getBuildingInfo(self.iBuilding).getMaxWorkers()
			szSpecialText = localText.getText("TXT_KEY_BUILDING_MAX_WORKERS2", (iMaxWorkers, ))
			if (iMaxWorkers > 0) :
				## R&R, Robert Surcouf,  Pedia - Start
				#screen.appendListBoxStringNoUpdate(panelName, u"<font=4>" + szSpecialText.upper() + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
				screen.appendListBoxStringNoUpdate(panelName, u"<font=3>" + szSpecialText.upper() + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
				## R&R, Robert Surcouf,  Pedia - End
				
		# TAC/Ronnar End				
		screen.updateListBox(panelName)

	# Place Special abilities
	def placeSpecial(self):

		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_PEDIA_SPECIAL_ABILITIES", ()), "", true, false, self.X_SPECIAL_PANE, self.Y_SPECIAL_PANE, self.W_SPECIAL_PANE, self.H_SPECIAL_PANE, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		listName = self.top.getNextWidgetName()
		szSpecialText = CyGameTextMgr().getBuildingHelp(self.iBuilding, True, False, None)[1:]
		# TAC/Ronnar Start: Add number of max workers per building
		if (self.XResolution < 1280):
			iMaxWorkers = gc.getBuildingInfo(self.iBuilding).getMaxWorkers()
			if (iMaxWorkers > 0) :
				szSpecialText = localText.getText("TXT_KEY_BUILDING_MAX_WORKERS", (iMaxWorkers, )) + "\n" + szSpecialText 
		# TAC/Ronnar End
		screen.addMultilineText(listName, szSpecialText, self.X_SPECIAL_PANE+5, self.Y_SPECIAL_PANE+30, self.W_SPECIAL_PANE-10, self.H_SPECIAL_PANE-35, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

	def placeHistory(self):

		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_CIVILOPEDIA_HISTORY", ()), "", True, True, self.X_HISTORY_PANE, self.Y_HISTORY_PANE, self.W_HISTORY_PANE, self.H_HISTORY_PANE, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		textName = self.top.getNextWidgetName()
		szText = u""
		if len(gc.getBuildingInfo(self.iBuilding).getStrategy()) > 0:
			szText += localText.getText("TXT_KEY_CIVILOPEDIA_STRATEGY", ())
			szText += gc.getBuildingInfo(self.iBuilding).getStrategy()
			szText += u"\n\n"
		szText += localText.getText("TXT_KEY_CIVILOPEDIA_BACKGROUND", ())
		szText += gc.getBuildingInfo(self.iBuilding).getCivilopedia()
		screen.addMultilineText( textName, szText, self.X_HISTORY_PANE + 15, self.Y_HISTORY_PANE + 40, self.W_HISTORY_PANE - (15 * 2), self.H_HISTORY_PANE - (15 * 2) - 25, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

	def placeLinks(self, bRedraw):

		screen = self.top.getScreen()

		if bRedraw:
			screen.clearListBoxGFC(self.top.LIST_ID)

		listSorted = self.getBuildingSortedList()

		iSelected = 0
		i = 0
		for iI in range(len(listSorted)):
			if (not gc.getBuildingInfo(listSorted[iI][1]).isGraphicalOnly()):
				if (not gc.getDefineINT("CIVILOPEDIA_SHOW_ACTIVE_CIVS_ONLY") or not gc.getGame().isFinalInitialized() or gc.getGame().isBuildingEverActive(listSorted[iI][1])):
					if bRedraw:
						screen.appendListBoxStringNoUpdate(self.top.LIST_ID, listSorted[iI][0], WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING, listSorted[iI][1], 0, CvUtil.FONT_LEFT_JUSTIFY)
					if listSorted[iI][1] == self.iBuilding:
						iSelected = i
					i += 1

		if bRedraw:
			screen.updateListBox(self.top.LIST_ID)

		screen.setSelectedListBoxStringGFC(self.top.LIST_ID, iSelected)

	def getBuildingSortedList(self):
		listBuildings = []
		iCount = 0
		for iBuilding in range(gc.getNumBuildingInfos()):
			if (not gc.getBuildingInfo(iBuilding).isGraphicalOnly()):
				listBuildings.append(iBuilding)
				iCount += 1

		listSorted = [(0,0)] * iCount
		iI = 0
		for iBuilding in listBuildings:
			listSorted[iI] = (gc.getBuildingInfo(iBuilding).getDescription(), iBuilding)
			iI += 1
		listSorted.sort()
		return listSorted


	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		return 0

