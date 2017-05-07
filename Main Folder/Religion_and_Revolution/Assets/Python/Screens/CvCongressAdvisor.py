## Sid Meier's Civilization 4
## Copyright Firaxis Games 2008

from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums

# Modified by Solver for Col v1

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

# this class is shared by both the resource and technology foreign advisors
class CvCongressAdvisor:
	"Congress Advisor Screen"

	def __init__(self):
		self.SCREEN_NAME = "CongressAdvisor"
		self.BACKGROUND_ID = "CongressAdvisorBackground"

		self.XResolution = 0
		self.YResolution = 0

		self.PICTURE_FF = 3
		self.BIG_PICTURE_FF = 4
		self.BIGFF = 0
		
		
	def interfaceScreen (self):

		self.player = gc.getPlayer(gc.getGame().getActivePlayer())
		self.team = gc.getTeam(self.player.getTeam())

		screen = self.getScreen()
		if screen.isActive():
			return

		self.MyFathers = []
		self.XResolution = self.getScreen().getXResolution()
		self.YResolution = self.getScreen().getYResolution()
		
		screen.setRenderInterfaceOnly(True);
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		# Set the background and exit button, and show the screen
		screen.setDimensions(0, 0, self.XResolution, self.YResolution)
		screen.addDDSGFC(self.BACKGROUND_ID, ArtFileMgr.getInterfaceArtInfo("INTERFACE_CONGRESS_BG").getPath(), 0, 0, self.XResolution, self.YResolution, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		#Fankman/TAC - Title-Bar
		screen.addDDSGFC("TopPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TITLE").getPath(), 0, 0, self.XResolution, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		#screen.addDDSGFC("BottomPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_OFF").getPath(), 0, self.YResolution - 55, self.XResolution, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDDSGFCAt("BottomBGPanel" + str(1), "Background", ArtFileMgr.getInterfaceArtInfo("INTERFACE_CITY_BG_TOP").getPath(), 0, self.YResolution - 45, self.XResolution, 45, WidgetTypes.WIDGET_GENERAL, -1, -1, False)
		screen.showWindowBackground(False)

		TitleText = localText.getText("TXT_KEY_CONGRESS_ADVISOR_TITLE", ()).upper()
		TitleText = localText.changeTextColor(TitleText, gc.getInfoTypeForString("COLOR_FONT_CREAM"))

		screen.setLabel( "Title", "Background", u"<font=4b>" + TitleText + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.XResolution / 2, 4, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setText( "CongressScreenExit", "Background", u"<font=4>" + CyTranslator().getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + "</font>", CvUtil.FONT_RIGHT_JUSTIFY, self.XResolution - 30, self.YResolution - 35, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )
		screen.setActivation( "CongressScreenExit", ActivationTypes.ACTIVATE_MIMICPARENTFOCUS )

		self.FatherColumn = 0
		self.ButtonClick = 0
		self.lastIndex = 999
		
		self.drawContents()
		return 0

	def getScreen(self):
		return CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.CONGRESS_ADVISOR)
		
	def getActualColumnCount(self):
		return ((len(self.MyFathers) + 1) / 2)

	def drawContents(self):
		screen = self.getScreen()
				
		self.MaxColumnCount = 50
		iRecordHight = ((self.YResolution - 110) / 2) - 5
		iRecordWidth = ((self.XResolution - 15) / 5) * 19 / 20
		PortraitSize = int(iRecordWidth * 1)
		PortraitBorder = PortraitSize / 20

		self.MyFathers = []
		self.MySortedFathers = []
		
		for iFather in range(gc.getNumFatherInfos()):
			Father = gc.getFatherInfo(iFather)
			if (gc.getGame().getFatherTeam(iFather) == self.team.getID()):
				self.MyFathers.append(iFather)

		FatherCountText = localText.getText("TXT_KEY_MEMBERS", (CyGameTextMgr().getInterfaceTimeStr(gc.getGame().getActivePlayer()), len(self.MyFathers)))
		FatherCountText = localText.changeTextColor(FatherCountText, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
		screen.setLabel("FatherCount", "Background", "<font=4>" + FatherCountText + "</font>", CvUtil.FONT_CENTER_JUSTIFY, self.XResolution / 2, self.YResolution - 35, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
	
		#Fankman/TAC - Sort the Fathers by the year
		self.MyFathers.sort(lambda x, y: cmp(CyGame().getFatherGameTurn(x), CyGame().getFatherGameTurn(y)))
		
		iIndex = 0
		PosX = 0
		PosY = 0
		category = 0					
			
		#Fankman/TAC - Place the Fathers in row and columns
		for i in range(0,len(self.MyFathers)):
			screen.addDDSGFCAt("Portrait Black" + str(iIndex), "Background", ArtFileMgr.getInterfaceArtInfo("INTERFACE_CONGRESS_BLACK").getPath(),   (self.XResolution / 12 * PosX) + (self.XResolution / 24), (self.YResolution / 6 * PosY) + (self.YResolution / 12 * 1) - (PortraitSize * 1 / 20), PortraitSize * 4 / 10, PortraitSize * 4 / 10, WidgetTypes.WIDGET_GENERAL, -1, -1, False)
			#screen.hide("Portrait Black" + str(len(self.MyFathers)))	
			screen.addDDSGFCAt("Portrait" + str(iIndex), "Background", gc.getFatherInfo(self.MyFathers[iIndex]).getPortrait(),                       (self.XResolution / 12 * PosX) + (self.XResolution / 24), (self.YResolution / 6 * PosY) + (self.YResolution / 12 * 1), PortraitSize * 4 / 10, PortraitSize * 4 / 10, WidgetTypes.WIDGET_GENERAL, -1, -1, False)
			screen.setImageButtonAt("Portrait Box" + str(iIndex), "Background", ArtFileMgr.getInterfaceArtInfo("INTERFACE_CONGRESS_BOX").getPath(),  (self.XResolution / 12 * PosX) + (self.XResolution / 24), (self.YResolution / 6 * PosY) + (self.YResolution / 12 * 1) - (PortraitSize * 1 / 20), PortraitSize * 4 / 10, PortraitSize * 4 / 10, WidgetTypes.WIDGET_GENERAL, self.PICTURE_FF, iIndex)				
			#Fankman/TAC - Place the Icons
			category = (gc.getFatherInfo(self.MyFathers[iIndex]).getFatherCategory()) + 1
			screen.addDDSGFCAt("Button" + str(iIndex), "Background", "Art/Interface/Screens/ContinentalCongress/FF_Symbol" + str(category) + ".dds", (self.XResolution / 12 * PosX) + (self.XResolution / 24) + (PortraitSize * 3 / 11), (self.YResolution / 6 * PosY) + (self.YResolution / 12 * 1) + (PortraitSize * 3 / 11), self.XResolution / 150 * 4, self.XResolution / 150 * 4, WidgetTypes.WIDGET_GENERAL, -1, -1, False)
			iIndex += 1
			PosX += 1
			if PosX == 11:
				PosX = 0
				PosY += 1
			
		iButtonClick = 0
					
	def handleInput(self, inputClass):

		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):

			if (inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL):
				
				if(inputClass.getData1() == self.PICTURE_FF):
						
					self.BIGFF = (inputClass.getData2())
					iFather = self.MyFathers[self.BIGFF + (self.FatherColumn * 2)]
					iFatherTurn = CyGame().getFatherGameTurn(iFather)
					iFatherYear = CyGame().getTurnYear(iFatherTurn)
					screen = self.getScreen()
					iRecordWidth = ((self.XResolution - 15) / 5) * 29 / 45
					PortraitSize = int(iRecordWidth * 1.0)
					screen.addDDSGFCAt("Big Portrait Black" + str(1), "Background", ArtFileMgr.getInterfaceArtInfo("INTERFACE_CONGRESS_BLACK").getPath(), self.XResolution / 2 - ((PortraitSize * 41 / 16) / 2) , self.YResolution / 40 * 3, PortraitSize * 5 / 2 , PortraitSize * 5 / 2, WidgetTypes.WIDGET_GENERAL, -1, -1, False)
					screen.addDDSGFCAt("Big Portrait" + str(1), "Background", gc.getFatherInfo(self.MyFathers[self.BIGFF + (self.FatherColumn * 2)]).getPortrait(), self.XResolution / 2 - ((PortraitSize * 41 / 16) / 2) , self.YResolution / 35 * 5, PortraitSize * 5 / 2, PortraitSize * 5 / 2, WidgetTypes.WIDGET_GENERAL, -1, -1, False)
					screen.setImageButtonAt("Big Portrait Box" + str(1), "Background", ArtFileMgr.getInterfaceArtInfo("INTERFACE_CONGRESS_BOX").getPath(), self.XResolution / 2 - ((PortraitSize * 41 / 16) / 2) , self.YResolution / 40 * 3, PortraitSize * 5 / 2, PortraitSize * 5 / 2, WidgetTypes.WIDGET_GENERAL, self.BIG_PICTURE_FF, -1)
						
					screen.addDDSGFC("BigFatherShadow" + str(1), ArtFileMgr.getInterfaceArtInfo("INTERFACE_CONGRESS_PAPER").getPath(),self.XResolution / 2 -((self.XResolution * 4 / 12) / 2), self.YResolution - (self.YResolution / 30 * 10) , self.XResolution * 4 / 12, self.YResolution / 4, WidgetTypes.WIDGET_GENERAL, -1, -1 )
					szFatherTitle = u"<font=3b>" + gc.getFatherInfo(iFather).getDescription() + u" (%d)" %iFatherYear
					szFatherTitle = localText.changeTextColor(szFatherTitle, gc.getInfoTypeForString("COLOR_FONT_CREAM"))
						
					#screen.attachMultilineTextAt("BigFatherShadow" + "1", "BigFatherName" + str(1), szFatherTitle, 70, 40, self.XResolution * 2 / 9, (iRecordWidth * 18 / 100), WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)
					screen.attachMultilineTextAt("BigFatherShadow" + "1", "BigFatherName" + str(1), szFatherTitle, 70, 30, self.XResolution * 2 / 9, (iRecordWidth * 21 / 100), WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY) # Robert Surcouf Alignment problem
					szFatherData = u"<font=3>" + CyGameTextMgr().getFatherHelp(iFather, True)[1:] + u"</font>"						
					screen.attachMultilineTextAt("BigFatherShadow" + "1", "BigFatherData" + str(1), szFatherData, 70, 60, self.XResolution * 2 / 9, self.YResolution / 7, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)	 # Robert Surcouf Alignment problem					
					#screen.attachMultilineTextAt("BigFatherShadow" + "1", "BigFatherData" + str(1), szFatherData, 70, 70, self.XResolution * 2 / 9, self.YResolution / 5, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)	
					
				elif(inputClass.getData1() == self.BIG_PICTURE_FF):
						
					self.getScreen().hide("Big Portrait Box" + "1")
					self.getScreen().hide("Big Portrait" + "1")
					self.getScreen().hide("Big Portrait Black" + "1")
					self.getScreen().hide("BigFatherName" + "1")
					self.getScreen().hide("BigFatherData" + "1")
					self.getScreen().hide("BigFatherShadow" + "1")
							
					self.drawContents()
				
		return 0

	def update(self, fDelta):
		#if (CyInterface().isDirty(InterfaceDirtyBits.EuropeScreen_DIRTY_BIT)):
		#	CyInterface().setDirty(InterfaceDirtyBits.EuropeScreen_DIRTY_BIT, False)
			
		#	self.drawContents()

		return 0
	
	