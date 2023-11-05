## Sid Meier's Civilization 4
## Copyright Firaxis Games 2008

from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
#import re

# Modified by Solver for Col v1

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

# this class is shared by both the resource and technology foreign advisors
class CvRevolutionAdvisor:
	"Revolution Advisor Screen"

	def __init__(self):
		self.WIDGET_ID = "RevolutionAdvisorWidget"
		self.LINE_ID = "RevolutionAdvisorLine"
		self.SCREEN_NAME = "RevolutionAdvisor"
		self.DEBUG_DROPDOWN_ID =  "RevolutionAdvisorDropdownWidget"
		self.BACKGROUND_ID = "RevolutionAdvisorBackground"

		self.XResolution = 0
		self.YResolution = 0
		
	def interfaceScreen (self):

		self.player = gc.getPlayer(gc.getGame().getActivePlayer())
		if self.player.getParent() == PlayerTypes.NO_PLAYER:
			return
		self.parent = gc.getPlayer(self.player.getParent())
		if self.parent == PlayerTypes.NO_PLAYER:
			return
	
		screen = self.getScreen()
		if screen.isActive():
			return
	
		self.XResolution = self.getScreen().getXResolution()
		self.YResolution = self.getScreen().getYResolution()

		screen.setRenderInterfaceOnly(True);
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)
	
		# Set the background and exit button, and show the screen
		screen.setDimensions(0, 0, self.XResolution, self.YResolution)
		screen.addDDSGFC(self.BACKGROUND_ID, ArtFileMgr.getInterfaceArtInfo("INTERFACE_REVOLUTION_BG").getPath(), 0, 0, self.XResolution, self.YResolution, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDDSGFC("TopPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TITLE").getPath(), 0, 0, self.XResolution, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDDSGFC("BottomPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_OFF").getPath(), 0, self.YResolution - 55, self.XResolution, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.showWindowBackground(False)

		self.SCREEN_TITLE = u"<font=4b>" + localText.getText("TXT_KEY_REVOLUTION_ADVISOR_TITLE", ()).upper() + u"</font>"
		self.SCREEN_TITLE = localText.changeTextColor(self.SCREEN_TITLE, gc.getInfoTypeForString("COLOR_FONT_CREAM"))

		screen.setText( "Title", "Background", self.SCREEN_TITLE, CvUtil.FONT_CENTER_JUSTIFY, self.XResolution / 2, 4, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setText( "RevolutionScreenExit", "Background", u"<font=4>" + CyTranslator().getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + "</font>", CvUtil.FONT_RIGHT_JUSTIFY, self.XResolution - 30, self.YResolution - 36, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )
		screen.setActivation( "RevolutionScreenExit", ActivationTypes.ACTIVATE_MIMICPARENTFOCUS )

		self.Y_REVOLUTION = self.YResolution / 17
		self.W_REVOLUTION = self.XResolution * 2 / 5
		
		self.Y_REBEL_BAR = self.YResolution / 7
		
		self.Y_UNITS_LISTS = self.YResolution * 35 / 100
		self.X_ROYAL_UNITS = self.XResolution * 58 / 100
		self.X_COLONIAL_UNITS = self.XResolution  * 3 / 20
		
		self.BAR_SIDE_MARGIN = self.YResolution / 15
		self.BAR_END_ICON_SIZE = self.YResolution / 14
		self.ICON_BUTTON_SIZE = self.YResolution / 21
		
		self.REVOLUTION_BUTTON = 1776
		self.AMENDMENT_BUTTON = 1778

		#Debug PullDown
		if ( CyGame().isDebugMode() ):
			screen.addDropDownBoxGFC( "CivDropDown", 22, 12, 192, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.SMALL_FONT )
			screen.setActivation( "CivDropDown", ActivationTypes.ACTIVATE_MIMICPARENTFOCUS )

			for j in range(gc.getMAX_PLAYERS()):
				if (gc.getPlayer(j).isAlive()):
					if gc.getPlayer(j).getParent() != -1:	#TAC
						screen.addPullDownString( "CivDropDown", gc.getPlayer(j).getName(), j, j, False )
		else:
			screen.hide( "CivDropDown" )

		# Draw Contents
		self.drawBar()
		self.drawRoyalTroops()
		self.drawColonialTroops()
		self.drawWarMaterials()
		self.drawCivics()
		
		return 0

	def getScreen(self):
		return CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.REVOLUTION_ADVISOR)

	def drawBar(self):
		screen = self.getScreen()	

		if not self.player.isInRevolution() and gc.getGame().getGameState() == GameStateTypes.GAMESTATE_ON:
			screen.setImageButton("RevolutionButton", ArtFileMgr.getInterfaceArtInfo("SCREEN_DATE_BOX").getPath(), (self.XResolution - self.W_REVOLUTION) / 2, self.Y_REVOLUTION, self.W_REVOLUTION, self.BAR_END_ICON_SIZE * 3 / 2, WidgetTypes.WIDGET_GENERAL, self.REVOLUTION_BUTTON, -1)
			screen.setTextAt( "RevolutionButtonText", "RevolutionButton", "<font=4>" + localText.getText("INTERFACE_REVOLUTION", ()) + "</font>", CvUtil.FONT_CENTER_JUSTIFY, self.W_REVOLUTION / 2, self.BAR_END_ICON_SIZE * 3 / 4, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, self.REVOLUTION_BUTTON, -1 )
		
			screen.enable("RevolutionButton", gc.getTeam(self.player.getTeam()).canDoRevolution())
			screen.enable("RevolutionButtonText", gc.getTeam(self.player.getTeam()).canDoRevolution())
			
		self.BarHight = self.BAR_END_ICON_SIZE / 2
		
		screen.addDDSGFC( "RebelIcon", ArtFileMgr.getCivilizationArtInfo(gc.getCivilizationInfo(self.player.getCivilizationType()).getArtDefineTag()).getButton(), self.BAR_SIDE_MARGIN, self.Y_REBEL_BAR, self.BAR_END_ICON_SIZE, self.BAR_END_ICON_SIZE, WidgetTypes.WIDGET_HELP_REBEL, 0, -1)
		screen.addDDSGFC( "LoyalIcon", gc.getYieldInfo(YieldTypes.YIELD_BELLS).getButton(), self.XResolution - (self.BAR_SIDE_MARGIN + self.BAR_END_ICON_SIZE), self.Y_REBEL_BAR, self.BAR_END_ICON_SIZE, self.BAR_END_ICON_SIZE, WidgetTypes.WIDGET_HELP_REBEL,  0, -1)

		BarWidth = self.XResolution - ((self.BAR_SIDE_MARGIN  + self.BAR_END_ICON_SIZE) * 2)
		screen.addStackedBarGFC("RebelBar", self.BAR_SIDE_MARGIN + self.BAR_END_ICON_SIZE, self.Y_REBEL_BAR + ((self.BAR_END_ICON_SIZE - self.BarHight) / 2), BarWidth, self.BarHight, InfoBarTypes.NUM_INFOBAR_TYPES, WidgetTypes.WIDGET_HELP_REBEL, 0, -1 )
		screen.setStackedBarColors("RebelBar", InfoBarTypes.INFOBAR_STORED, gc.getInfoTypeForString("COLOR_RED") )
		screen.setStackedBarColors("RebelBar", InfoBarTypes.INFOBAR_RATE, gc.getInfoTypeForString("COLOR_RED") )
		screen.setStackedBarColors("RebelBar", InfoBarTypes.INFOBAR_RATE_EXTRA, gc.getInfoTypeForString("COLOR_EMPTY") )
		screen.setStackedBarColors("RebelBar", InfoBarTypes.INFOBAR_EMPTY, gc.getInfoTypeForString("COLOR_EMPTY") )
		RebelPercentage = (gc.getTeam(self.player.getTeam()).getRebelPercent()) / 100.0
		screen.setBarPercentage("RebelBar", InfoBarTypes.INFOBAR_STORED, RebelPercentage)

		screen.addDDSGFC( "GoldButton",ArtFileMgr.getInterfaceArtInfo("INTERFACE_REVOLUTION_GOLD_BUTTON").getPath(), (self.XResolution / 2) - (self.BAR_END_ICON_SIZE / 2), self.Y_REBEL_BAR, self.BAR_END_ICON_SIZE, self.BAR_END_ICON_SIZE, WidgetTypes.WIDGET_HELP_REBEL, 0, -1)
		screen.addDDSGFC( "Medalion",ArtFileMgr.getInterfaceArtInfo("INTERFACE_REVOLUTION_BROWN_BUTTON").getPath(), self.BAR_SIDE_MARGIN + self.BAR_END_ICON_SIZE + int(RebelPercentage * BarWidth) - (self.BAR_END_ICON_SIZE / 2), self.Y_REBEL_BAR, self.BAR_END_ICON_SIZE, self.BAR_END_ICON_SIZE, WidgetTypes.WIDGET_HELP_REBEL, 0, -1)

		screen.setLabelAt( "RebelBarText", "Medalion", "<font=3>" + str(gc.getTeam(self.player.getTeam()).getRebelPercent()) + " %</font>", CvUtil.FONT_CENTER_JUSTIFY, self.BAR_END_ICON_SIZE / 2, self.BAR_END_ICON_SIZE / 2, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		
		return 0

	def drawWarMaterials(self):
		screen = self.getScreen()

		## R&R, Robert Surcouf,  Revolution Advisor Screen, Cannons, Start
		#CityCount, iHorses, iMuskets = 0, 0, 0
		CityCount, iHorses, iBlades, iMuskets, iCannons, iBlackpowder = 0, 0, 0, 0, 0, 0
		(city, iter) = self.player.firstCity(True)
		while(city):
			iHorses += city.getYieldStored(gc.getInfoTypeForString("YIELD_HORSES"))
			iBlades += city.getYieldStored(gc.getInfoTypeForString("YIELD_BLADES")) # R&R, ray, Blades
			iMuskets += city.getYieldStored(gc.getInfoTypeForString("YIELD_MUSKETS"))
			iCannons += city.getYieldStored(gc.getInfoTypeForString("YIELD_CANNONS")) ## R&R, Robert Surcouf
			iBlackpowder += city.getYieldStored(gc.getInfoTypeForString("YIELD_BLACK_POWDER"))
			(city, iter) = self.player.nextCity(iter, True)

		screen.addDDSGFC( "HorseIcon", gc.getYieldInfo(gc.getInfoTypeForString("YIELD_HORSES")).getIcon(), (self.XResolution * 40 / 100) - self.ICON_BUTTON_SIZE / 2, (self.YResolution * 5 / 12), self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1)
		HorseText = str(iHorses)
		HorseText = localText.changeTextColor(HorseText, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
		screen.setLabel( "Horse Count", "Background", u"<font=4>" + HorseText + "</font>", CvUtil.FONT_CENTER_JUSTIFY, (self.XResolution * 40 / 100), (self.YResolution * 5 / 12) + self.ICON_BUTTON_SIZE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
	
		screen.addDDSGFC( "MusketIcon", gc.getYieldInfo(gc.getInfoTypeForString("YIELD_MUSKETS")).getIcon(), (self.XResolution * 40 / 100) - self.ICON_BUTTON_SIZE / 2, (self.YResolution * 9 / 12), self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1)
		GunText = str(iMuskets) 
		GunText = localText.changeTextColor(GunText, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
		screen.setLabel( "Musket Count", "Background", u"<font=4>" + GunText + "</font>", CvUtil.FONT_CENTER_JUSTIFY, (self.XResolution * 40 / 100), (self.YResolution * 9 / 12) + self.ICON_BUTTON_SIZE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		screen.addDDSGFC( "CannonIcon", gc.getYieldInfo(gc.getInfoTypeForString("YIELD_CANNONS")).getIcon(), (self.XResolution * 40 / 100) - self.ICON_BUTTON_SIZE / 2, (self.YResolution * 7 / 12), self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1)
		CannonText = str(iCannons) 
		CannonText = localText.changeTextColor(CannonText, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
		screen.setLabel( "Cannon Count", "Background", u"<font=4>" + CannonText + "</font>", CvUtil.FONT_CENTER_JUSTIFY, (self.XResolution * 40 / 100), (self.YResolution * 7 / 12) + self.ICON_BUTTON_SIZE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		
		screen.addDDSGFC( "BlackPowderIcon", gc.getYieldInfo(gc.getInfoTypeForString("YIELD_BLACK_POWDER")).getIcon(), (self.XResolution * 40 / 100) - self.ICON_BUTTON_SIZE / 2, (self.YResolution * 8 / 12), self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1)
		BlackPowderText = str(iBlackpowder) 
		BlackPowderText = localText.changeTextColor(BlackPowderText, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
		screen.setLabel( "Cannon Count", "Background", u"<font=4>" + BlackPowderText + "</font>", CvUtil.FONT_CENTER_JUSTIFY, (self.XResolution * 40 / 100), (self.YResolution * 8 / 12) + self.ICON_BUTTON_SIZE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		
		screen.addDDSGFC( "BladesIcon", gc.getYieldInfo(gc.getInfoTypeForString("YIELD_BLADES")).getIcon(), (self.XResolution * 40 / 100) - self.ICON_BUTTON_SIZE / 2, (self.YResolution * 6 / 12), self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1)
		BladesText = str(iBlades) 
		BladesText = localText.changeTextColor(BladesText, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
		screen.setLabel( "Blades Count", "Background", u"<font=4>" + BladesText + "</font>", CvUtil.FONT_CENTER_JUSTIFY, (self.XResolution * 40 / 100), (self.YResolution * 6 / 12) + self.ICON_BUTTON_SIZE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		## R&R, Robert Surcouf,  Revolution Advisor Screen, Cannons, End

	def drawCivics(self):
		screen = self.getScreen()
	
		for iType in range(gc.getNumCivicOptionInfos()):
			for iCivic in range(gc.getNumCivicInfos()):
				if (gc.getCivicInfo(iCivic).getCivicOptionType() == iType):
					if self.player.isCivic(iCivic):
						screen.addDDSGFC("ArcticleBox" + str(iType), gc.getCivicInfo(iCivic).getButton(), (self.XResolution / 2) - (self.ICON_BUTTON_SIZE * gc.getNumCivicOptionInfos() ) + (self.ICON_BUTTON_SIZE * iType * 2) + (self.ICON_BUTTON_SIZE / 2), self.Y_REVOLUTION, self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, self.AMENDMENT_BUTTON, iCivic)
						screen.hide("GoldButton")
						break
						
	def drawRoyalTroops(self):
		screen = self.getScreen()
#TAC -->
		RoyalText = localText.getText("INTERFACE_ROYAL_EXPEDITIONARY_FORCES", ())
		RoyalText = localText.changeTextColor(RoyalText, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
		screen.setLabel( "Royal Troops", "Background", u"<font=4>" + RoyalText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, self.X_ROYAL_UNITS, self.Y_UNITS_LISTS, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
	
		UnitList = []
		iUnitIndex = 0
				
		iParent = self.player.getParent()
		if iParent != -1:
			parent = gc.getPlayer(iParent)
			(unit, iter) = parent.firstUnit()
			while(unit):
				if (not unit.isNone()) and (unit.canAttack()) and (unit.getOwner() == iParent):			
					iProfession = unit.getProfession()
					iUnitType = unit.getUnitType()
					UnitList = self.calculateTroops(UnitList, iUnitIndex, iUnitType, iProfession)
						
				(unit, iter) = parent.nextUnit(iter)
									
			for i in range(parent.getNumEuropeUnits()):
				unit = parent.getEuropeUnit(i)
				if (not unit.isNone()) and (unit.canAttack()) and (unit.getOwner() == iParent):			
					iProfession = unit.getProfession()
					iUnitType = unit.getUnitType()
					UnitList = self.calculateTroops(UnitList, iUnitIndex, iUnitType, iProfession)
				
			for iUnit in range(self.player.getNumRevolutionEuropeUnits()):
				iProfession = self.player.getRevolutionEuropeProfession(iUnit)
				iUnitType = self.player.getRevolutionEuropeUnit(iUnit)			
				UnitList = self.calculateTroops(UnitList, iUnitIndex, iUnitType, iProfession)
				
		screen.addScrollPanel("royalTroopsList", u"", self.X_ROYAL_UNITS, self.Y_UNITS_LISTS + self.YResolution / 20, self.XResolution * 35 / 112, self.YResolution  * 2 / 5, PanelStyles.PANEL_STYLE_MAIN, True, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		self.displayTroops(UnitList, "royalTroops")
		

		
	def drawColonialTroops(self):
		screen = self.getScreen()
		RebelText = localText.getText("INTERFACE_COLONIAL_FORCES", ())
		RebelText = localText.changeTextColor(RebelText, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
		screen.setLabel( "Colonial Troops", "Background", u"<font=4>" + RebelText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, self.X_COLONIAL_UNITS - self.XResolution / 60, self.Y_UNITS_LISTS, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
	
		UnitList = []
		iUnitIndex = 0
		
		# Then Sea Units
		for i in range(self.player.getNumEuropeUnits()):
			unit = self.player.getEuropeUnit(i)
			if (not unit.isNone()) and (unit.canAttack()) and (unit.getDomainType() == DomainTypes.DOMAIN_SEA) and (unit.getOwner() == self.player.getID()):			
				iUnitType = self.player.getEuropeUnit(i).getUnitType()
				iProfession = self.player.getEuropeUnit(i).getProfession()
				UnitList = self.calculateTroops(UnitList, iUnitIndex, iUnitType, iProfession)
			
		(unit, iter) = self.player.firstUnit()
		while(unit):
			if (not unit.isNone()) and (unit.canAttack()) and (unit.getDomainType() == DomainTypes.DOMAIN_SEA) and (unit.getOwner() == self.player.getID()):			
				iUnitType = unit.getUnitType()
				iProfession = unit.getProfession()
				UnitList = self.calculateTroops(UnitList, iUnitIndex, iUnitType, iProfession)
					
			(unit, iter) = self.player.nextUnit(iter)
		
		# First Land Units
		for i in range(self.player.getNumEuropeUnits()):
			unit = self.player.getEuropeUnit(i)
			if (not unit.isNone()) and (unit.canAttack()) and (unit.getDomainType() == DomainTypes.DOMAIN_LAND) and (unit.getOwner() == self.player.getID()):			
				iUnitType = self.player.getEuropeUnit(i).getUnitType()
				iProfession = self.player.getEuropeUnit(i).getProfession()
				UnitList = self.calculateTroops(UnitList, iUnitIndex, iUnitType, iProfession)
			
		(unit, iter) = self.player.firstUnit()
		while(unit):
			if (not unit.isNone()) and (unit.canAttack()) and (unit.getDomainType() == DomainTypes.DOMAIN_LAND) and (unit.getOwner() == self.player.getID()):			
				iUnitType = unit.getUnitType()
				iProfession = unit.getProfession()
				UnitList = self.calculateTroops(UnitList, iUnitIndex, iUnitType, iProfession)
					
			(unit, iter) = self.player.nextUnit(iter)
		
		screen.addScrollPanel("rebelTroopsList", u"", self.X_COLONIAL_UNITS - self.XResolution / 60, self.Y_UNITS_LISTS + self.YResolution / 20, self.XResolution * 35 / 112, self.YResolution * 2 / 5,  PanelStyles.PANEL_STYLE_MAIN, True, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		self.displayTroops(UnitList, "rebelTroops")
		
# <-- TAC

	def handleInput(self, inputClass):
	
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED):
			self.CivDropDown(inputClass)
			return 1

		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if (inputClass.getData1() == self.REVOLUTION_BUTTON):
				CyMessageControl().sendChangeWar(gc.getPlayer(self.player.getParent()).getTeam(), True)
				self.getScreen().hideScreen()
			
		return 0

	def update(self, fDelta):
		return 0
		
	def getWidgetHelp(self, argsList):
		iScreen, eWidgetType, iData1, iData2, bOption = argsList

		if eWidgetType == WidgetTypes.WIDGET_GENERAL and iData1 == self.AMENDMENT_BUTTON:
			szHelp = CyGameTextMgr().parseCivicInfo(iData2, True, False, True)
			szName = gc.getCivicInfo(iData2).getDescription()
			szColoredName = localText.changeTextColor(szName, gc.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"))
			return szColoredName + szHelp

		return u""
	
		szSpecialText = CyGameTextMgr().parseCivicInfo(self.iCivic, True, False, True)
		
	def CivDropDown( self, inputClass ):
		if ( inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED ):
			screen = self.getScreen()
			iIndex = screen.getSelectedPullDownID("CivDropDown")
			self.player = gc.getPlayer(screen.getPullDownData("CivDropDown", iIndex))
			
			self.drawBar()
			self.drawRoyalTroops()
			self.drawColonialTroops()
			self.drawWarMaterials()
			self.drawCivics()


# TAC --->			
	def calculateTroops(self, UnitList, iUnitIndex, iUnitType, iProfession):

		if (not iProfession == -1):
			szUnitName = gc.getProfessionInfo(iProfession).getDescription()
		else:
			szUnitName = gc.getUnitInfo(iUnitType).getDescription()
								
		if (not szUnitName in UnitList):
			UnitList.insert(iUnitIndex, szUnitName)
			iUnitIndex += 1
			UnitList.insert(iUnitIndex, [iUnitType, iProfession, 1])
			iUnitIndex += 1
		else:
			iIndex = UnitList.index(szUnitName) + 1
			iNumUnit = UnitList[iIndex][2] + 1
			del UnitList[iIndex]
			UnitList.insert(iIndex, [iUnitType, iProfession, iNumUnit])
			iUnitIndex += 1
											
		return UnitList
	
	def displayTroops(self, UnitList, szAttachPanel):
		screen = self.getScreen()
		YMultiplier = int(self.ICON_BUTTON_SIZE * 1.3)
				
		for iIndex in range(len(UnitList)):
			if (iIndex%2 == 0):
				iUnitType = UnitList[iIndex + 1][0]
				iProfession = UnitList[iIndex + 1][1]
				iNumUnit = UnitList[iIndex + 1][2]
				
				i = 0
				if iNumUnit >> 1:
					i = 1
											
				if (iProfession == -1):
					szUnitName = gc.getUnitInfo(iUnitType).getDescriptionForm(i)
					szUnitIcon = gc.getUnitInfo(iUnitType).getButton()
				else:
					szUnitName = gc.getProfessionInfo(iProfession).getDescriptionForm(i)
					szUnitIcon = gc.getProfessionInfo(iProfession).getButton()
					
				screen.addDDSGFCAt(szAttachPanel + "Icon" + str(iIndex / 2), szAttachPanel + "List", szUnitIcon, 0, (self.ICON_BUTTON_SIZE + 20) * (iIndex / 2), self.ICON_BUTTON_SIZE, self.ICON_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1, False)
				szUnitText = localText.getText("[COLOR_BROWN_TEXT] %d1 %s2", (iNumUnit, szUnitName))
				screen.setLabelAt(szAttachPanel + "Count" + str(iIndex / 2), szAttachPanel + "List", u"<font=3>" + szUnitText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, (self.ICON_BUTTON_SIZE * 5 / 4), (self.ICON_BUTTON_SIZE + 20) * (iIndex / 2) + self.ICON_BUTTON_SIZE / 2 , 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		
		return 0
		
# <-- TAC
def isSoldier(eProfession):
	if eProfession == ProfessionTypes.NO_PROFESSION:
		return false
	if (gc.getProfessionInfo(eProfession).getCombatChange() == 0):
		return false
	if (gc.getProfessionInfo(eProfession).isUnarmed()):
		return false
	return (getProfessionYieldsRequired(eProfession) == 1)
	
def isDragoon(eProfession):
	if eProfession == ProfessionTypes.NO_PROFESSION:
		return false
	if (gc.getProfessionInfo(eProfession).getCombatChange() == 0):
		return false
	if (gc.getProfessionInfo(eProfession).isUnarmed()):
		return false
	return (getProfessionYieldsRequired(eProfession) > 1)

def isCannon(eUnit):
	unit = gc.getUnitInfo(eUnit)
	if unit.getDomainType() != DomainTypes.DOMAIN_LAND:
		return false
	if (unit.getCombat() == 0):
		return false
	return True

def isWarship(eUnit):
	unit = gc.getUnitInfo(eUnit)
	if unit.getDomainType() != DomainTypes.DOMAIN_SEA:
		return false
	if (unit.getCombat() == 0):
		return false
	if (unit.isOnlyDefensive()):
		return false
	return True

def getProfessionYieldsRequired(eProfession):
	iNumYieldsRequired = 0
	for iYield in range(YieldTypes.NUM_YIELD_TYPES):
		if (gc.getProfessionInfo(eProfession).getYieldEquipmentAmount(iYield) > 0):
			iNumYieldsRequired += 1
	return iNumYieldsRequired

def isVeteran(eUnit):
	unit = gc.getUnitInfo(eUnit)
	for iPromotion in range(gc.getNumPromotionInfos()):
		if unit.getFreePromotions(iPromotion):
			return True
	return false

