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

class CvPediaYields:
	"Civilopedia Screen for Yields"

	def __init__(self, main):
		self.iYields = -1
		self.top = main
		
	# Screen construction function
	def interfaceScreen(self, iYields, x, y, h, w):	
		
		self.iYields = iYields
	
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
		#self.W_STATS_PANE = (w * 55 / 100)
		#self.H_STATS_PANE = (h * 30 / 100)
		self.W_STATS_PANE = (w * 60 / 100)
		self.H_STATS_PANE = (h * 20 / 100)
		## R&R, Robert Surcouf,  Pedia - End
		
		self.X_HISTORY_PANE = x
		self.Y_HISTORY_PANE = y + self.H_ICON_PANE + (h * 5 / 100)
		self.W_HISTORY_PANE = w
		self.H_HISTORY_PANE = h - self.H_ICON_PANE - (h * 5 / 100)

		self.top.deleteAllWidgets()						
		screen = self.top.getScreen()
		
		bNotActive = (not screen.isActive())
		if bNotActive:
			self.top.setPediaCommonWidgets()

		# Header...
		szHeader = u"<font=4b>" + gc.getYieldInfo(self.iYields).getDescription().upper() + u"</font>"
		szHeaderId = self.top.getNextWidgetName()
		screen.setLabel(szHeaderId, "Background", szHeader, CvUtil.FONT_CENTER_JUSTIFY, self.top.X_SCREEN, self.top.Y_TITLE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		# Top
		screen.setText(self.top.getNextWidgetName(), "Background", self.top.MENU_TEXT, CvUtil.FONT_LEFT_JUSTIFY, self.top.X_MENU, self.top.Y_MENU, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_PEDIA_MAIN, CivilopediaPageTypes.CIVILOPEDIA_PAGE_YIELDS, -1)

		if self.top.iLastScreen	!= CvScreenEnums.PEDIA_YIELDS or bNotActive:		
			self.placeLinks(true)
			self.top.iLastScreen = CvScreenEnums.PEDIA_YIELDS
		else:
			self.placeLinks(true)
		
		# Icon
		screen.addPanel(self.top.getNextWidgetName(), "", "", false, false, self.X_ICON_PANE, self.Y_ICON_PANE, self.W_ICON_PANE, self.H_ICON_PANE, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addPanel(self.top.getNextWidgetName(), "", "", false, false, self.X_ICON, self.Y_ICON, self.W_ICON, self.H_ICON, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addDDSGFC(self.top.getNextWidgetName(), gc.getYieldInfo(self.iYields).getButton(), self.X_ICON + self.W_ICON/2 - self.ICON_SIZE/2, self.Y_ICON + self.H_ICON/2 - self.ICON_SIZE/2, self.ICON_SIZE, self.ICON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		self.placeStats()
		self.placeHistory()

	def placeStats(self):
	
		screen = self.top.getScreen()

		yieldInfo = gc.getYieldInfo(self.iYields)

		panelName = self.top.getNextWidgetName()

		screen.addListBoxGFC(panelName, "", self.X_STATS_PANE, self.Y_STATS_PANE, self.W_STATS_PANE, self.H_STATS_PANE, TableStyles.TABLE_STYLE_EMPTY)
		screen.enableSelect(panelName, False)

		for i in range(gc.getNumBuildingInfos()):
			buildingInfo = gc.getBuildingInfo(i)
#MultipleYieldsProduced Start
			if (buildingInfo.getProfessionOutput() > 0):
				for iProfession in range(gc.getNumProfessionInfos()):
					if (gc.getProfessionInfo(iProfession).getYieldsProduced(0) == self.iYields):
						if(gc.getProfessionInfo(iProfession).getSpecialBuilding() == buildingInfo.getSpecialBuildingType()):
							szText = localText.getText("TXT_KEY_BUILDING_PROFESSION_OUTPUT", (buildingInfo.getProfessionOutput(), gc.getYieldInfo(gc.getProfessionInfo(iProfession).getYieldsProduced(0)).getChar()))
							## R&R, Robert Surcouf,  Pedia - Start
							#screen.appendListBoxStringNoUpdate(panelName, u"<font=4>" + u"%s: " %buildingInfo.getDescription() + szText + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
							screen.appendListBoxStringNoUpdate(panelName, u"<font=3>" + u"%s: " %buildingInfo.getDescription() + szText + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
							## R&R, Robert Surcouf,  Pedia - End
#MultipleYieldsProduced End	
			if(buildingInfo.getYieldModifier(self.iYields) > 0):
				## R&R, Robert Surcouf,  Pedia - Start
				#screen.appendListBoxStringNoUpdate(panelName, u"<font=4>" + u"%s: " %(buildingInfo.getDescription()) + u"+%d%% %c" % (buildingInfo.getYieldModifier(self.iYields), gc.getYieldInfo(self.iYields).getChar()) + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
				screen.appendListBoxStringNoUpdate(panelName, u"<font=3>" + u"%s: " %(buildingInfo.getDescription()) + u"+%d%% %c" % (buildingInfo.getYieldModifier(self.iYields), gc.getYieldInfo(self.iYields).getChar()) + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
				## R&R, Robert Surcouf,  Pedia - End
				
			if (buildingInfo.getYieldChange(self.iYields) != 0):
				if (buildingInfo.getYieldChange(self.iYields) > 0):
						szSign = "+"
				else:
					szSign = ""

				szYield = gc.getYieldInfo(self.iYields).getDescription() + ": "

				## R&R, ray,  Pedia - Start
				# szText1 = szYield.upper() + szSign + str(buildingInfo.getYieldChange(self.iYields))
				szText1 = (buildingInfo.getDescription()) +  ": " + szSign + str(buildingInfo.getYieldChange(self.iYields))
				szText2 = szText1 + (u"%c" % (gc.getYieldInfo(self.iYields).getChar()))
				## R&R, ray,  Pedia - End
				## R&R, Robert Surcouf,  Pedia - Start
				#screen.appendListBoxStringNoUpdate(panelName, u"<font=4>" + szText2 + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
				screen.appendListBoxStringNoUpdate(panelName, u"<font=3>" + szText2 + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
				## R&R, Robert Surcouf,  Pedia - End
				
## Show-yield-producing-units Start - Nightinggale
		# sort Units to get the biggest bonus first
		unitsList=[(0,0)]*gc.getNumUnitInfos()
		for iUnit in range(gc.getNumUnitInfos()):
			value = max(gc.getUnitInfo(iUnit).getYieldChange(self.iYields) + gc.getUnitInfo(iUnit).getBonusYieldChange(self.iYields), gc.getUnitInfo(iUnit).getYieldModifier(self.iYields))
			if value <= 0:
				value = min(gc.getUnitInfo(iUnit).getYieldChange(self.iYields) + gc.getUnitInfo(iUnit).getBonusYieldChange(self.iYields), gc.getUnitInfo(iUnit).getYieldModifier(self.iYields))
			
			unitsList[iUnit] = (value, iUnit)
		unitsList.sort(reverse=True)
		
		# draw the units found 
		for it in range(gc.getNumUnitInfos()):
			if unitsList[it][0] != 0:
				iUnit = unitsList[it][1]
				
				# short names for readability
				change   = gc.getUnitInfo(iUnit).getYieldChange(self.iYields)
				bonus    = gc.getUnitInfo(iUnit).getBonusYieldChange(self.iYields)
				modifier = gc.getUnitInfo(iUnit).getYieldModifier(self.iYields)
				
				# TODO: draw unit icon left of the text
				szName = gc.getUnitInfo(iUnit).getDescription()
				szBonus = ": "
				if (change != 0):
					if (change > 0):
							szBonus +=  "+"
					szBonus +=  str(change)
					if (bonus != 0):
						szBonus += "("
						if (bonus > 0):
							szBonus +=  "+"
						szBonus += str(bonus) + ")"
					szBonus += u"%c" % (gc.getYieldInfo(self.iYields).getChar())
				if (modifier != 0):
					if (modifier > 0):
						szBonus +=  "+"
					szBonus +=  str(modifier) + u"%%%c" % (gc.getYieldInfo(self.iYields).getChar())
					
				screen.appendListBoxStringNoUpdate(panelName, u"<font=3>" + szName + szBonus + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
## Show-yield-producing-units End - Nightinggale

		screen.updateListBox(panelName)

	def placeHistory(self):

		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_CIVILOPEDIA_HISTORY", ()), "", True, True, self.X_HISTORY_PANE, self.Y_HISTORY_PANE, self.W_HISTORY_PANE, self.H_HISTORY_PANE, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1)

		screen.attachLabel(panelName, "", "  ")

		textName = self.top.getNextWidgetName()
		screen.addMultilineText( textName, gc.getYieldInfo(self.iYields).getCivilopedia(), self.X_HISTORY_PANE + 15, self.Y_HISTORY_PANE + 40, self.W_HISTORY_PANE - (15 * 2), self.H_HISTORY_PANE - (15 * 2) - 25, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		
	def placeLinks(self, bRedraw):

		screen = self.top.getScreen()

		if bRedraw:
			screen.clearListBoxGFC(self.top.LIST_ID)
		
		# sort resources alphabetically
		listSorted=[(0,0)]*gc.getNumYieldInfos()
		for j in range(gc.getNumYieldInfos()):
			listSorted[j] = (gc.getYieldInfo(j).getDescription(), j)
		listSorted.sort()	
			
		iSelected = 0
		i = 0
		for iI in range(gc.getNumYieldInfos()):
			if (not gc.getYieldInfo(listSorted[iI][1]).isGraphicalOnly()):
				if bRedraw:
					screen.appendListBoxString(self.top.LIST_ID, listSorted[iI][0], WidgetTypes.WIDGET_PEDIA_JUMP_TO_YIELDS, listSorted[iI][1], 0, CvUtil.FONT_LEFT_JUSTIFY )
				if listSorted[iI][1] == self.iYields:
					iSelected = i
				i += 1

		screen.setSelectedListBoxStringGFC(self.top.LIST_ID, iSelected)
				

	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		return 0


