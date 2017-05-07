## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
##
## Alex Mantzaris / Jesse Smith 09-2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import random

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class CvPediaProfession:
	"Civilopedia Screen for Professions"

	def __init__(self, main):
		self.iProfession = -1
		self.top = main

	def getSortedList( self, numInfos, getInfo ):
		'''returned a list of infos sorted alphabetically'''

		# count the items we are going to display
		iNumNonGraphical = 0
		for i in range(gc.getNumProfessionInfos()):
			if (not getInfo(i).isGraphicalOnly()):
				iNumNonGraphical += 1

		infoList = [(0,0)] * iNumNonGraphical
		j = 0
		for i in range(gc.getNumProfessionInfos()):
			if (not gc.getProfessionInfo(i).isGraphicalOnly()):
				infoList[j] = (gc.getProfessionInfo(i).getDescription(), i)
				j += 1

		infoList.sort()
		return infoList

	# Screen construction function
	def interfaceScreen(self, iProfession, x, y, h, w):
		self.iProfession = iProfession

		self.X_PROFESSION_PANE = x
		self.Y_PROFESSION_PANE = y
		self.W_PROFESSION_PANE = (w * 30 / 100)
		self.H_PROFESSION_PANE = self.W_PROFESSION_PANE
		
		self.X_ROTATION_UNIT_ANIMATION = -20
		self.Z_ROTATION_UNIT_ANIMATION = 30
		self.SCALE_ANIMATION = 1.0

		self.W_ICON = self.W_PROFESSION_PANE * 2 / 3
		self.H_ICON = self.H_PROFESSION_PANE * 2 / 3
		self.X_ICON = x + (self.W_PROFESSION_PANE / 2) - (self.W_ICON / 2)
		
		self.ICON_SIZE = self.W_ICON / 2
		self.W_PROFESSION_PANE = (w * 60 / 100)

		self.X_UNIT_ANIMATION = x + self.W_PROFESSION_PANE + (w * 5 / 100)
		self.Y_UNIT_ANIMATION = y
		self.W_UNIT_ANIMATION = (w * 35 / 100)
		self.H_UNIT_ANIMATION = (h * 35 / 100)

		self.H_PROFESSION_PANE = self.H_UNIT_ANIMATION
		self.Y_ICON = y + (self.H_PROFESSION_PANE / 2) - (self.H_ICON / 2)

		self.X_STATS_PANE = self.X_ICON + self.W_ICON + (w * 2 / 100)
		self.Y_STATS_PANE = self.Y_ICON
		self.W_STATS_PANE = (w * 35 / 100)
		self.H_STATS_PANE = (h * 30 / 100)

		self.X_REQUIRES = x
		self.Y_REQUIRES = y + self.H_PROFESSION_PANE + (h * 5 / 100)
		self.W_REQUIRES = (w * 50 / 100)
		self.H_REQUIRES = (h * 90 / 100) - self.Y_REQUIRES

		self.X_UNITCOMBAT_PANE = x
		self.Y_UNITCOMBAT_PANE = y + (h * 85 / 100)
		self.W_UNITCOMBAT_PANE = (w * 50 / 100)
		self.H_UNITCOMBAT_PANE = (h * 15 / 100)

		self.X_HISTORY_PANE = x + (w * 55 / 100)
		self.Y_HISTORY_PANE = y + self.H_UNIT_ANIMATION + (h * 5 / 100)
		self.W_HISTORY_PANE = (w * 45 / 100)
		self.H_HISTORY_PANE = (h * 60 / 100)

		self.X_BUILDINGS = x
		self.Y_BUILDINGS = self.Y_REQUIRES + self.H_REQUIRES + (h * 5 / 100)
		self.W_BUILDINGS = (w * 50 / 100)
		self.H_BUILDINGS = y + (h * 100 / 100) - self.Y_BUILDINGS

		self.top.deleteAllWidgets()
		screen = self.top.getScreen()

		bNotActive = (not screen.isActive())
		if bNotActive:
			self.top.setPediaCommonWidgets()

		# Header...
		szHeader = u"<font=4b>" + gc.getProfessionInfo(self.iProfession).getDescription().upper() + u"</font>"
		szHeaderId = self.top.getNextWidgetName()
		screen.setLabel(szHeaderId, "Background", szHeader, CvUtil.FONT_CENTER_JUSTIFY, self.top.X_SCREEN, self.top.Y_TITLE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, CivilopediaPageTypes.CIVILOPEDIA_PAGE_PROFESSION, iProfession)

		# Top
		screen.setText(self.top.getNextWidgetName(), "Background", self.top.MENU_TEXT, CvUtil.FONT_LEFT_JUSTIFY, self.top.X_MENU, self.top.Y_MENU, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_PEDIA_MAIN, CivilopediaPageTypes.CIVILOPEDIA_PAGE_PROFESSION, -1)

		if self.top.iLastScreen	!= CvScreenEnums.PEDIA_PROFESSION or bNotActive:
			self.placeLinks(true)
			self.top.iLastScreen = CvScreenEnums.PEDIA_PROFESSION
		else:
			self.placeLinks(true)

		# Icon
		screen.addPanel( self.top.getNextWidgetName(), "", "", False, False, self.X_PROFESSION_PANE, self.Y_PROFESSION_PANE, self.W_PROFESSION_PANE, self.H_PROFESSION_PANE, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addPanel(self.top.getNextWidgetName(), "", "", false, false, self.X_ICON, self.Y_ICON, self.W_ICON, self.H_ICON, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addDDSGFC(self.top.getNextWidgetName(), gc.getProfessionInfo(self.iProfession).getButton(), self.X_ICON + self.W_ICON / 2 - self.ICON_SIZE / 2, self.Y_ICON + self.H_ICON / 2 - self.ICON_SIZE / 2, self.ICON_SIZE, self.ICON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		#Find the Best Unit fit for this Profession
		iExpertUnit = -1
		Profession = gc.getProfessionInfo(iProfession)
		for iUnit in range(gc.getNumUnitInfos()):
			if (gc.getUnitInfo(iUnit).getDefaultProfession() == iProfession):
				iExpertUnit = iUnit
				break
##MultipleYieldsProduced Start
		if (iExpertUnit == -1 and Profession.getYieldsProduced(0) != YieldTypes.NO_YIELD):
			HighestBonus = 0
			for iUnit in range(gc.getNumUnitInfos()):
				if ((gc.getUnitInfo(iUnit).isLandYieldChanges() and not Profession.isWater()) or (gc.getUnitInfo(iUnit).isWaterYieldChanges() and Profession.isWater())):
					if(gc.getUnitInfo(iUnit).getYieldModifier(Profession.getYieldsProduced(0)) > 25 * HighestBonus):
						HighestBonus = gc.getUnitInfo(iUnit).getYieldModifier(Profession.getYieldsProduced(0))
						iExpertUnit = iUnit
					if(gc.getUnitInfo(iUnit).getYieldChange(Profession.getYieldsProduced(0)) > HighestBonus):
						HighestBonus = gc.getUnitInfo(iUnit).getYieldChange(Profession.getYieldsProduced(0))
						iExpertUnit = iUnit
##MultipleYieldsProduced End

		# TAC - Profession unit graphic fix - koma13 - START
		if (iExpertUnit == -1 and Profession.getCombatChange() > 0):
			for iCiv in range(gc.getNumCivilizationInfos()):
				Civilization = gc.getCivilizationInfo(iCiv)
				if Civilization.isValidProfession(iProfession):
					break
				
			iDefaultPopulationUnit = Civilization.getCivilizationUnits(gc.getDefineINT("DEFAULT_POPULATION_UNIT"))
			if gc.getUnitInfo(iDefaultPopulationUnit).getDefaultProfession() != ProfessionTypes.NO_PROFESSION:
				iExpertUnit = iDefaultPopulationUnit
		# TAC - Profession unit graphic fix - koma13 - END
		
		if (iExpertUnit == -1 and Profession.getCombatChange() > 0):
			for iCiv in range(gc.getNumCivilizationInfos()):
				Civilization = gc.getCivilizationInfo(iCiv)
				if Civilization.isValidProfession(iProfession):
					break

			MostPromotions = 0
			for iUnitClass in range(gc.getNumUnitClassInfos()):
				iUnitType = gc.getCivilizationInfo(iCiv).getCivilizationUnits(iUnitClass)
				if iUnitType != UnitTypes.NO_UNIT:
					UnitInfo = gc.getUnitInfo(iUnitType)
					if UnitInfo.getDefaultProfession() != ProfessionTypes.NO_PROFESSION:
						PromotionCount = 0
						for iPromotion in range(gc.getNumPromotionInfos()):
							if UnitInfo.getFreePromotions(iPromotion):
								PromotionCount += 1
						if PromotionCount >= MostPromotions:
							MostPromotions = PromotionCount
							iExpertUnit = iUnitType
#MultipleYieldsProduced Start
		if (iExpertUnit == -1 and Profession.getYieldsProduced(0) == YieldTypes.YIELD_EDUCATION):
			HighestBonus = 0
			for iUnit in range(gc.getNumUnitInfos()):
				if(gc.getUnitInfo(iUnit).getStudentWeight() > HighestBonus):
					HighestBonus = gc.getUnitInfo(iUnit).getStudentWeight()
					iExpertUnit = iUnit
#MultipleYieldsProduced End

		# Unit animation
		if(iExpertUnit != -1):
			screen.addUnitGraphicGFC(self.top.getNextWidgetName(), iExpertUnit, iProfession, self.X_UNIT_ANIMATION, self.Y_UNIT_ANIMATION, self.W_UNIT_ANIMATION, self.H_UNIT_ANIMATION, WidgetTypes.WIDGET_GENERAL, -1, -1, self.X_ROTATION_UNIT_ANIMATION, self.Z_ROTATION_UNIT_ANIMATION, self.SCALE_ANIMATION, True)

		self.placeStats()
		self.placeBuildings()
		self.placeRequires()
		self.placeHistory()

	def placeStats(self):
		'''Yields consumed and produced by profession'''
		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addListBoxGFC(panelName, "", self.X_STATS_PANE, self.Y_STATS_PANE, self.W_STATS_PANE, self.H_STATS_PANE, TableStyles.TABLE_STYLE_EMPTY)
		screen.enableSelect(panelName, False)

		#Yields Consumed and Produced
#MultipleYieldsConsumed Start
		for iYieldConsumed in range(gc.getProfessionInfo(self.iProfession).getNumYieldsConsumed()):
			if (gc.getProfessionInfo(self.iProfession).getYieldsConsumed(iYieldConsumed) != -1):
				screen.appendListBoxStringNoUpdate(panelName, u"<font=4>" + localText.getText("TXT_KEY_YIELD_CONSUMED", ()).upper() + (u"%c" % gc.getYieldInfo(YieldTypes (gc.getProfessionInfo(self.iProfession).getYieldsConsumed(iYieldConsumed))).getChar()) + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
#MultipleYieldsConsumed End
#MultipleYieldsProduced Start
		for iYieldProduced in range(gc.getProfessionInfo(self.iProfession).getNumYieldsProduced()):
			if (gc.getProfessionInfo(self.iProfession).getYieldsProduced(iYieldProduced) != -1):
				screen.appendListBoxStringNoUpdate(panelName, u"<font=4>" + localText.getText("TXT_KEY_YIELD_PRODUCED", ()).upper() + (u"%c" % gc.getYieldInfo(YieldTypes (gc.getProfessionInfo(self.iProfession).getYieldsProduced(iYieldProduced))).getChar()) + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
#MultipleYieldsProduced End
		#Combat/Movement/Work changes
		iCombatPowerChange = gc.getProfessionInfo(self.iProfession).getCombatChange()
		if(iCombatPowerChange != 0):
			screen.appendListBoxStringNoUpdate(panelName, u"<font=4>" + localText.getText("INTERFACE_PANE_STRENGTH", ()).upper() + (u" +%i" % iCombatPowerChange) + u" %c" % CyGame().getSymbolID(FontSymbols.STRENGTH_CHAR) + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
		iMovesChange = gc.getProfessionInfo(self.iProfession).getMovesChange()
		if(iMovesChange != 0):
			screen.appendListBoxStringNoUpdate(panelName, u"<font=4>" + localText.getText("INTERFACE_PANE_MOVEMENT", ()).upper() + (u" +%i" % iMovesChange) +u" %c" % CyGame().getSymbolID(FontSymbols.MOVES_CHAR) + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)

		iCombat = gc.getProfessionInfo(self.iProfession).getUnitCombatType()
		if (iCombat != -1):
			screen.appendListBoxStringNoUpdate(panelName, u" ", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
			screen.appendListBoxStringNoUpdate(panelName, u"<font=4>" + gc.getUnitCombatInfo(iCombat).getDescription() + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
		
		screen.updateListBox(panelName)

	def placeBuildings(self):
		'''Buildings required to use this Profession'''
		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_PEDIA_WORKS_IN_BUILDING", ()), "", false, true, self.X_BUILDINGS, self.Y_BUILDINGS, self.W_BUILDINGS, self.H_BUILDINGS, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		screen.attachLabel(panelName, "", "  ")
		iSpecialBuilding = gc.getProfessionInfo(self.iProfession).getSpecialBuilding()

		if (iSpecialBuilding != -1):
			for iBuilding in range(gc.getNumBuildingInfos()):
				if (gc.getBuildingInfo(iBuilding).getSpecialBuildingType() == iSpecialBuilding):
					screen.attachImageButton(panelName, "", gc.getBuildingInfo(iBuilding).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING, iBuilding, 1, False )

	def placeRequires(self):
		'''equipment for military and certain professions'''
		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_PEDIA_SPECIAL_ABILITIES", ()), "", false, true, self.X_REQUIRES, self.Y_REQUIRES, self.W_REQUIRES, self.H_REQUIRES, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		listName = self.top.getNextWidgetName()
		szSpecialText = CyGameTextMgr().getProfessionHelp(self.iProfession, True)[1:]
		# TAC/Ronnar Start
		# Check learning by doing
		if gc.getProfessionInfo(self.iProfession).LbD_isUsed():
			iLearnLevel = gc.getProfessionInfo(self.iProfession).LbD_getLearnLevel()
			if iLearnLevel == 1: 	szSpecialText += "\n" + localText.getText("TXT_KEY_PROF_LBD1", ())
			if iLearnLevel == 2: 	szSpecialText += "\n" + localText.getText("TXT_KEY_PROF_LBD2", ())
			if iLearnLevel >= 3: 	szSpecialText += "\n" + localText.getText("TXT_KEY_PROF_LBD3", ())
		
		# TAC/Ronnar End
		screen.addMultilineText(listName, szSpecialText, self.X_REQUIRES + 15, self.Y_REQUIRES + 40, self.W_REQUIRES - 30, self.H_REQUIRES - 50, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

	def placeHistory(self):
		'''Pedia History about this Profession'''
		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_CIVILOPEDIA_HISTORY", ()), "", True, True, self.X_HISTORY_PANE, self.Y_HISTORY_PANE, self.W_HISTORY_PANE, self.H_HISTORY_PANE, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		textName = self.top.getNextWidgetName()
		szText = u""
		if len(gc.getProfessionInfo(self.iProfession).getStrategy()) > 0:
			szText += localText.getText("TXT_KEY_CIVILOPEDIA_STRATEGY", ())
			szText += gc.getProfessionInfo(self.iProfession).getStrategy()
			szText += u"\n\n"
		szText += localText.getText("TXT_KEY_CIVILOPEDIA_BACKGROUND", ())
		szText += gc.getProfessionInfo(self.iProfession).getCivilopedia()
		screen.addMultilineText( textName, szText, self.X_HISTORY_PANE + 15, self.Y_HISTORY_PANE + 40, self.W_HISTORY_PANE - 30, self.H_HISTORY_PANE - 60, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

	def placeLinks(self, bRedraw):
		'''Reset the MasterList with Professions'''
		screen = self.top.getScreen()

		if bRedraw:
			screen.clearListBoxGFC(self.top.LIST_ID)

		# sort Units alphabetically
		
		# TAC - Hide Whaling Profession - koma13 - START
		# RaR - Fishing Boat Profession - ray - START
		ProfessionList=[(0,0)]*(gc.getNumProfessionInfos() - 2)
		for j in range(gc.getNumProfessionInfos()):
			if not self.isWhalingProfession(j):
				ProfessionList[j] = (gc.getProfessionInfo(j).getDescription(), j)
		# TAC - Hide Whaling Profession - koma13 - END
		
		ProfessionList.sort()

		iSelected = 0
		i = 0
		for iI in range(gc.getNumProfessionInfos()):
			# TAC - Hide Whaling Profession - koma13 - START
			if not self.isWhalingProfession(iI):
				if bRedraw:
					screen.appendListBoxStringNoUpdate(self.top.LIST_ID, ProfessionList[iI][0], WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROFESSION, ProfessionList[iI][1], 0, CvUtil.FONT_LEFT_JUSTIFY)
				if ProfessionList[iI][1] == self.iProfession:
					iSelected = iI
					i += 1
			# TAC - Hide Whaling Profession - koma13 - END
			
		if bRedraw:
			screen.updateListBox(self.top.LIST_ID)

		screen.setSelectedListBoxStringGFC(self.top.LIST_ID, iSelected)

	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		return 0


	# TAC - Hide Whaling Profession - koma13 - START
	def isWhalingProfession(self, iProfession):
		return (iProfession == gc.getInfoTypeForString("PROFESSION_WHALING_BOAT_WORKING") or iProfession == gc.getInfoTypeForString("PROFESSION_FISHING_BOAT_WORKING"))
		#return (iProfession == gc.getInfoTypeForString("PROFESSION_WHALING_BOAT_WORKING"))
	# TAC - Hide Whaling Profession - koma13 - END
	
	
	
	