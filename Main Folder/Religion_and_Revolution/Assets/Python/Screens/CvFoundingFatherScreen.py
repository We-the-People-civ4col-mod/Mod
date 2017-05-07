## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import CvScreensInterface

PIXEL_INCREMENT = 7
BOX_INCREMENT_WIDTH = 45 # Used to be 33 #Should be a multiple of 3...
BOX_INCREMENT_HEIGHT = 15 #Should be a multiple of 3...
BOX_INCREMENT_Y_SPACING = 9 #Should be a multiple of 3...
BOX_INCREMENT_X_SPACING = 3 #Should be a multiple of 3...

TEXTURE_SIZE = 24
X_START = 6
X_INCREMENT = 27
Y_ROW = 32

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class CvFoundingFatherScreen:
	"Founding Father Screen"

	def __init__(self):
		self.SCREEN_NAME = "FatherAdvisor"

		self.iCivSelected = 0

		self.TypeSelection = 0

		self.TopPanelHeight = 55

	def getScreen(self):
		return CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.FATHER_SCREEN)

	# Screen construction function
	def interfaceScreen (self, iCategory):
		
		screen = self.getScreen()

		if ( CyGame().isPitbossHost() ):
			return

		if screen.isActive():
			return
			
		if (iCategory != -1):
			self.TypeSelection = iCategory

		self.XResolution = self.getScreen().getXResolution()
		self.YResolution = self.getScreen().getYResolution()

		screen.setRenderInterfaceOnly(True)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		#Build Body
		self.iCivSelected = gc.getGame().getActivePlayer()

		screen.setDimensions(0, 0, self.XResolution, self.YResolution)
		screen.addDrawControl("FatherBackGround", ArtFileMgr.getInterfaceArtInfo("FATHER_BG").getPath(), 0, 0, self.XResolution, self.YResolution, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDDSGFC("FatherTopPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TITLE").getPath(), 0, 0, self.XResolution, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		TitleText = localText.getText("TXT_KEY_FATHER_SCREEN_TITLE", ()).upper()
		TitleText = localText.changeTextColor(TitleText, gc.getInfoTypeForString("COLOR_FONT_CREAM"))

		screen.setText("Title", "Background", u"<font=4b>" + TitleText + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.XResolution / 2, 4, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setActivation("FatherScreenExit", ActivationTypes.ACTIVATE_MIMICPARENTFOCUS)

		#Debug PullDown
		if ( CyGame().isDebugMode() ):
			screen.addDropDownBoxGFC( "CivDropDown", 22, 12, 192, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.SMALL_FONT )
			screen.setActivation( "CivDropDown", ActivationTypes.ACTIVATE_MIMICPARENTFOCUS )

			for j in range(gc.getMAX_PLAYERS()):
				if (gc.getPlayer(j).isAlive()):
					screen.addPullDownString( "CivDropDown", gc.getPlayer(j).getName(), j, j, False )
		else:
			screen.hide( "CivDropDown" )

		# Make the scrollable area
		self.FATHERLIST_HEIGHT = self.YResolution - 72

		#TAC - 1920x1080 Fix - koma13 - START
		#self.FatherRecordHeight = (self.FATHERLIST_HEIGHT * 4 / 10) 
		self.FatherRecordHeight = (self.FATHERLIST_HEIGHT * 39 / 100) 
		#TAC - 1920x1080 Fix - koma13 - END
		
		self.FatherRecordWidth = self.XResolution / 5
		self.BarAreaHeight = self.FATHERLIST_HEIGHT / 5

		# Compile Father Lists
		self.Fathers = []
		for i in range(gc.getNumFatherCategoryInfos()):
			self.Fathers.append([])
			
		for i in range(gc.getNumFatherInfos()):
			iType = gc.getFatherInfo(i).getFatherCategory()
			self.Fathers[iType].append(i)
		
		self.FatherBars = []
		for iCategory in range(gc.getNumFatherCategoryInfos()):
			iTopBarPoints = -1
			iBottomBarPoints = -1
			for iPoint in range(gc.getNumFatherPointInfos()):
				if (len(self.Fathers[iCategory]) > 0 and gc.getFatherInfo(self.Fathers[iCategory][0]).getPointCost(iPoint) > 0):
					if iBottomBarPoints == -1:
						iBottomBarPoints = iPoint
					elif iTopBarPoints == -1:
						iTopBarPoints = iPoint
			self.FatherBars.append((iTopBarPoints, iBottomBarPoints))

		for iCategory in range(gc.getNumFatherCategoryInfos()):
			screen.addScrollPanel("FoundingFatherList" + str(iCategory), u"", 0, 0, self.XResolution, self.FATHERLIST_HEIGHT, PanelStyles.PANEL_STYLE_EXTERNAL, true, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.setActivation("FoundingFatherList" + str(iCategory), ActivationTypes.ACTIVATE_NORMAL)
			screen.hide("FoundingFatherList" + str(iCategory))

		self.placeBars()
		self.placeFathers()
		self.placeTabs()
		self.setTab(self.TypeSelection, True)
		screen.show("FoundingFatherList" + str(self.TypeSelection))
		screen.moveToFront("CivDropDown")

	def placeTabs (self):
		screen = self.getScreen()

		ExitTabWidth = 130
		TabWidth = (self.XResolution - (ExitTabWidth * 9 / 10)) / gc.getNumFatherCategoryInfos()

		Ydisplacment = 38
		XIncrement = self.XResolution / 6

		for iTab in range(gc.getNumFatherCategoryInfos()):
			OnTabName = "OnTab" + str(iTab)
			EdgeWidth = 25
			BottomPanelHeight = 55
			screen.addPanel(OnTabName + "Left", "", "", False, False, TabWidth * iTab, self.YResolution - BottomPanelHeight, EdgeWidth, BottomPanelHeight, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.addPanel(OnTabName + "Center", "", "", False, False, (TabWidth * iTab) + EdgeWidth, self.YResolution - BottomPanelHeight, TabWidth - (EdgeWidth * 2), BottomPanelHeight, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.addPanel(OnTabName + "Right", "", "", False, False, (TabWidth * iTab) + TabWidth - EdgeWidth, self.YResolution - BottomPanelHeight, EdgeWidth, BottomPanelHeight, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)

			screen.addDrawControl(OnTabName + "Left", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_START").getPath(), TabWidth * iTab, self.YResolution - BottomPanelHeight, EdgeWidth, BottomPanelHeight, WidgetTypes.WIDGET_GENERAL, -1, -1 )
			screen.addDrawControl(OnTabName + "Center", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_ON").getPath(), (TabWidth * iTab) + EdgeWidth, self.YResolution - BottomPanelHeight, TabWidth - (EdgeWidth * 2), BottomPanelHeight, WidgetTypes.WIDGET_GENERAL, -1, -1 )
			screen.addDrawControl(OnTabName + "Right", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_END").getPath(), (TabWidth * iTab) + TabWidth - EdgeWidth, self.YResolution - BottomPanelHeight, EdgeWidth, BottomPanelHeight, WidgetTypes.WIDGET_GENERAL, -1, -1 )

			TabText = gc.getFatherCategoryInfo(iTab).getDescription()
			TabText = localText.changeTextColor(TabText, gc.getInfoTypeForString("COLOR_FONT_CREAM"))

			screen.setLabelAt("OnTabTitle" + str(iTab), OnTabName + "Center", u"<font=4>" + TabText + "</font>", CvUtil.FONT_CENTER_JUSTIFY, 65 , 33, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.hide(OnTabName + "Left")
			screen.hide(OnTabName + "Center")
			screen.hide(OnTabName + "Right")

			OffTabName = "OffTab" + str(iTab)
			screen.addPanel(OffTabName, "", "", False, False, TabWidth * iTab, self.YResolution - BottomPanelHeight, TabWidth, BottomPanelHeight, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, 1111, iTab)
			screen.addDrawControl(OffTabName, ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_OFF").getPath(), TabWidth * iTab, self.YResolution - BottomPanelHeight, TabWidth, BottomPanelHeight, WidgetTypes.WIDGET_GENERAL, -1, -1 )
			screen.setTextAt("OffTabTitle" + str(iTab), OffTabName, u"<font=4>" + TabText + "</font>", CvUtil.FONT_CENTER_JUSTIFY, EdgeWidth + 65, 33, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1111, iTab)

		screen.addPanel("ExitTab", "", "", False, False, self.XResolution - ExitTabWidth, self.YResolution - BottomPanelHeight, ExitTabWidth, BottomPanelHeight, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addDrawControl("ExitTab", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_OFF").getPath(), self.XResolution - ExitTabWidth, self.YResolution - BottomPanelHeight, ExitTabWidth, BottomPanelHeight, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setTextAt("ExitTabTitle", "ExitTab", u"<font=4>" + CyTranslator().getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + "</font>", CvUtil.FONT_RIGHT_JUSTIFY, ExitTabWidth - 20, BottomPanelHeight - 18, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)

	def setTab(self, iTab, State):
		screen = self.getScreen()
		OnTabName = "OnTab" + str(iTab)

		if (State):
			screen.show(OnTabName + "Left")
			screen.show(OnTabName + "Center")
			screen.show(OnTabName + "Right")
			screen.hide("OffTab" + str(iTab))
		else:
			screen.hide(OnTabName + "Left")
			screen.hide(OnTabName + "Center")
			screen.hide(OnTabName + "Right")
			screen.show("OffTab" + str(iTab))

	def placeBars (self):
		screen = self.getScreen()
		activeTeam = gc.getTeam(gc.getPlayer(self.iCivSelected).getTeam())
	
		for iCategory in range(gc.getNumFatherCategoryInfos()):
			screen.addDDSGFCAt("PointBox" + str(iCategory), "FoundingFatherList" + str(iCategory), ArtFileMgr.getInterfaceArtInfo("INTERFACE_EUROPE_SHADOW_BOX").getPath(), 25, self.YResolution - 300, 120 , (24 * 6), WidgetTypes.WIDGET_GENERAL, -1, -1, False )
			for iFatherPoint in range(gc.getNumFatherPointInfos()):
				szTotal = u"%c  %i" % (gc.getFatherPointInfo(iFatherPoint).getChar(), activeTeam.getFatherPoints(iFatherPoint))
				screen.setTextAt("PointTotal" + str(iCategory + iFatherPoint * gc.getNumFatherCategoryInfos()), "FoundingFatherList" + str(iCategory), u"<font=3b>" + szTotal + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, 40, self.YResolution - 300 + (24 * (iFatherPoint + 1)), -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
				screen.moveToFront("PointTotal" + str(iCategory + iFatherPoint * gc.getNumFatherCategoryInfos()))
				
			iTopBarPoints = self.FatherBars[iCategory][0]
			iBottomBarPoints = self.FatherBars[iCategory][1]

			if (iTopBarPoints != -1 and iBottomBarPoints != -1):
			
				screen.addDDSGFCAt("TopAcumulationBar" + str(iCategory), "FoundingFatherList" + str(iCategory), ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_START").getPath(), 20, (self.FATHERLIST_HEIGHT / 2) - (self.BarAreaHeight / 2), self.XResolution / 5, self.BarAreaHeight / 2, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
				szBarTitle = gc.getFatherPointInfo(iTopBarPoints).getDescription()
				szBarTitle = localText.changeTextColor(szBarTitle, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
				screen.setLabelAt("TopBarTitle" + str(iCategory), "TopAcumulationBar" + str(iCategory), "<font=3>" + szBarTitle + "</font>", CvUtil.FONT_LEFT_JUSTIFY, 20, self.BarAreaHeight / 4, -0.1, FontTypes.SMALL_FONT,  WidgetTypes.WIDGET_GENERAL, -1, -1)

				screen.addDDSGFCAt("BottomAcumulationBar" + str(iCategory), "FoundingFatherList" + str(iCategory), ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_START").getPath(), 20, (self.FATHERLIST_HEIGHT / 2), self.XResolution / 5, self.BarAreaHeight / 2, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
				szBarTitle = gc.getFatherPointInfo(iBottomBarPoints).getDescription()
				szBarTitle = localText.changeTextColor(szBarTitle, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
				screen.setLabelAt("BottomBarTitle" + str(iCategory), "BottomAcumulationBar" + str(iCategory), "<font=3>" + szBarTitle + "</font>", CvUtil.FONT_LEFT_JUSTIFY, 20, self.BarAreaHeight / 4, -0.1, FontTypes.SMALL_FONT,  WidgetTypes.WIDGET_GENERAL, -1, -1)

			else:
				screen.addDDSGFCAt("TopAcumulationBar" + str(iCategory), "FoundingFatherList" + str(iCategory), ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_START").getPath(), 20, (self.FATHERLIST_HEIGHT / 2) - (self.BarAreaHeight / 4), self.XResolution / 5, self.BarAreaHeight / 2, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
				szBarTitle = gc.getFatherPointInfo(iBottomBarPoints).getDescription()
				szBarTitle = localText.changeTextColor(szBarTitle, gc.getInfoTypeForString("COLOR_FONT_GOLD"))
				screen.setLabelAt("TopBarTitle" + str(iCategory), "TopAcumulationBar" + str(iCategory), "<font=3>" + szBarTitle + "</font>", CvUtil.FONT_LEFT_JUSTIFY, 20, self.BarAreaHeight / 4, -0.1, FontTypes.SMALL_FONT,  WidgetTypes.WIDGET_GENERAL, -1, -1)
				
	def placeFathers (self):

		# Get the screen
		screen = self.getScreen()
		activeTeam = gc.getTeam(gc.getPlayer(self.iCivSelected).getTeam())

		for list in self.Fathers:
			list.sort(lambda x, y: activeTeam.getFatherPointCost(x, gc.getFatherInfo(x).getFatherCategory()) - activeTeam.getFatherPointCost(y, gc.getFatherInfo(y).getFatherCategory()))

		iTopBarPointType = self.FatherBars[self.TypeSelection][0]
		iBottomBarPointType = self.FatherBars[self.TypeSelection][1]
		PlacedTopMedalion = False
		PlacedBottomMedalion = False

		for index in range(len(self.Fathers[self.TypeSelection])):
			i = self.Fathers[self.TypeSelection][index]
			iType = gc.getFatherInfo(i).getFatherCategory()
			ScrollPanel = "FoundingFatherList" + str(iType)

			PreceedingFather = 0
			szFatherData = u"<font=3>" + CyGameTextMgr().getFatherHelp(i, True)[1:] + u"</font>"
			szFatherTitle = gc.getFatherInfo(i).getDescription()
			szFatherTitle = localText.changeTextColor(szFatherTitle, gc.getInfoTypeForString("COLOR_FONT_CREAM"))

			j = index + 1
			BoxName = "PortraitBox" + str(i)
			TextBoxName = "FatherTextBox" + str(i)
			if (index % 2 == 0):
				screen.addDDSGFCAt(BoxName, ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_LEFT_BOX").getPath(), (self.FatherRecordWidth * (j+ 1)) - (self.FatherRecordWidth / 2), self.BarAreaHeight + self.FatherRecordHeight + (self.FatherRecordHeight / 6) + 5 + (self.TopPanelHeight * 3 / 4), self.FatherRecordWidth + (self.FatherRecordWidth / 2), (self.FatherRecordHeight / 2), WidgetTypes.WIDGET_GENERAL, -1, -1, False )
				screen.addDDSGFCAt("Portrait" + str(i), ScrollPanel, gc.getFatherInfo(i).getPortrait(), (self.FatherRecordWidth * (j+ 1)) - (self.FatherRecordHeight * 6 / 7), self.BarAreaHeight + self.FatherRecordHeight - 30 + (self.FatherRecordWidth / 5), self.FatherRecordHeight * 9 / 10, self.FatherRecordHeight * 9 / 10, WidgetTypes.WIDGET_PEDIA_JUMP_TO_FATHER, i, -1, False)
				screen.setImageShape("Portrait" + str(i), ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
				screen.setHitMargins("Portrait" + str(i), self.FatherRecordHeight / 5, 0);
			
				screen.attachPanelAt(ScrollPanel, TextBoxName, u"", u"", True, True, PanelStyles.PANEL_STYLE_EMPTY, (self.FatherRecordWidth * (j+ 1)) - (self.FatherRecordWidth / 2), self.BarAreaHeight + self.FatherRecordHeight + (self.FatherRecordHeight / 6) + 5 + (self.TopPanelHeight * 3 / 4), self.FatherRecordWidth + (self.FatherRecordWidth / 2), (self.FatherRecordHeight / 2), WidgetTypes.WIDGET_GENERAL, -1, -1 )
				screen.moveToFront(TextBoxName)
				if (len(szFatherTitle) > 55 and self.XResolution < 1280):
					screen.setTextAt("FatherID" + str(i), TextBoxName, "<font=3>" + szFatherTitle + "</font>", CvUtil.FONT_RIGHT_JUSTIFY, self.FatherRecordWidth + (self.FatherRecordWidth / 2) - 20, self.FatherRecordHeight / 15, -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_PEDIA_JUMP_TO_FATHER, i, -1 )
				else:
					screen.setTextAt("FatherID" + str(i), TextBoxName, "<font=4>" + szFatherTitle + "</font>", CvUtil.FONT_RIGHT_JUSTIFY, self.FatherRecordWidth + (self.FatherRecordWidth / 2) - 20, self.FatherRecordHeight / 15, -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_PEDIA_JUMP_TO_FATHER, i, -1 )
				screen.attachMultilineTextAt(TextBoxName, "FatherInfo" + str(i), szFatherData, self.FatherRecordHeight / 4, 40, self.FatherRecordWidth, (self.FatherRecordHeight / 2) - 45, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

# TAC 0201 Start: Test if FF was rejected before by active player. Set flag in lower row if FF unavailable
				if activeTeam.isFatherIgnore(i):
					screen.addDDSGFCAt("Flag Icon" + str(i), ScrollPanel, ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL").getPath(), (self.FatherRecordWidth * (j+ 1)) - (self.FatherRecordHeight * 6 / 7) + (self.FatherRecordHeight * 9 / 10) - (self.FatherRecordHeight / 5), self.BarAreaHeight + self.FatherRecordHeight - 15 + (self.FatherRecordWidth / 5), (self.FatherRecordHeight / 5), (self.FatherRecordHeight / 5), WidgetTypes.WIDGET_PEDIA_DESCRIPTION, CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT, gc.getInfoTypeForString("CONCEPT_FATHERS"), False )	
					szTempBuffer = localText.getText("TXT_KEY_FATHER_REJECTED", ())
					szTempBuffer = localText.changeTextColor(szTempBuffer, gc.getInfoTypeForString("COLOR_FONT_RED"))
					screen.setTextAt("FatherOwner" + str(i), ScrollPanel, "<font=3>" + szTempBuffer + "</font>", CvUtil.FONT_LEFT_JUSTIFY, (self.FatherRecordWidth * (j + 1)) - (self.FatherRecordHeight * 6 / 7) + (self.FatherRecordHeight * 9 / 10), self.BarAreaHeight + self.FatherRecordHeight - 30 + (self.FatherRecordWidth / 5) + (self.FatherRecordWidth / 5), -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_PEDIA_DESCRIPTION, CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT, gc.getInfoTypeForString("CONCEPT_FATHERS") )
# TAC End				

				if (gc.getGame().getFatherTeam(i) != TeamTypes.NO_TEAM):
					for k in range(gc.getMAX_PLAYERS()):
						if (gc.getPlayer(k).getTeam() == gc.getGame().getFatherTeam(i)):
							if (gc.getTeam(gc.getPlayer(gc.getGame().getActivePlayer()).getTeam()).isHasMet(gc.getGame().getFatherTeam(i))):
								screen.addDDSGFCAt("Flag Icon" + str(i), ScrollPanel, ArtFileMgr.getCivilizationArtInfo(gc.getCivilizationInfo(gc.getPlayer(k).getCivilizationType()).getArtDefineTag()).getButton() , (self.FatherRecordWidth * (j+ 1)) - (self.FatherRecordHeight * 6 / 7) + (self.FatherRecordHeight * 9 / 10) - (self.FatherRecordHeight / 5), self.BarAreaHeight + self.FatherRecordHeight - 15 + (self.FatherRecordWidth / 5), (self.FatherRecordHeight / 5), (self.FatherRecordHeight / 5), WidgetTypes.WIDGET_GENERAL, -1, -1, False )
								for m in range(gc.getMAX_PLAYERS()):
									if (gc.getPlayer(m).getTeam() == gc.getGame().getFatherTeam(i)):
										szTempBuffer = u"  <color=%d,%d,%d,%d>%s</color>" %(gc.getPlayer(m).getPlayerTextColorR(), gc.getPlayer(m).getPlayerTextColorG(), gc.getPlayer(m).getPlayerTextColorB(), gc.getPlayer(m).getPlayerTextColorA(), gc.getPlayer(m).getName())
										if (len(szTempBuffer) > 50):
											screen.attachMultilineTextAt(ScrollPanel, "FatherOwner" + str(i), "<font=3>" + szTempBuffer + "</font>", (self.FatherRecordWidth * (j + 1)) + (self.FatherRecordHeight / 20), self.BarAreaHeight + self.FatherRecordHeight - 15 + (self.FatherRecordWidth / 5), self.FatherRecordWidth, 55, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
										else:
											screen.setTextAt("FatherOwner" + str(i), ScrollPanel, "<font=4>" + szTempBuffer + "</font>", CvUtil.FONT_LEFT_JUSTIFY, (self.FatherRecordWidth * (j + 1)) - (self.FatherRecordHeight * 6 / 7) + (self.FatherRecordHeight * 9 / 10), self.BarAreaHeight + self.FatherRecordHeight - 30 + (self.FatherRecordWidth / 5) + (self.FatherRecordWidth / 5), -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
										break
							else:
								screen.addDDSGFCAt("Flag Icon" + str(i), ScrollPanel, ArtFileMgr.getMiscArtInfo("LEADERHEAD_RANDOM").getPath(), (self.FatherRecordWidth * (j+ 1)) - (self.FatherRecordHeight * 6 / 7) + (self.FatherRecordHeight * 9 / 10) - (self.FatherRecordHeight / 5), self.BarAreaHeight + self.FatherRecordHeight - 15 + (self.FatherRecordWidth / 5), (self.FatherRecordHeight / 5), (self.FatherRecordHeight / 5), WidgetTypes.WIDGET_GENERAL, -1, -1, False )
							break
			else:				
				screen.addDDSGFCAt(BoxName, ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_RIGHT_BOX").getPath(), (self.FatherRecordWidth * (j - 1)), self.TopPanelHeight, self.FatherRecordWidth + (self.FatherRecordWidth / 2), (self.FatherRecordHeight / 2), WidgetTypes.WIDGET_GENERAL, -1, -1, False )
				screen.addDDSGFCAt("Portrait" + str(i), ScrollPanel, gc.getFatherInfo(i).getPortrait(), (self.FatherRecordWidth * (j - 1)) + self.FatherRecordWidth, self.TopPanelHeight / 4, self.FatherRecordHeight * 9 / 10, self.FatherRecordHeight * 9 / 10, WidgetTypes.WIDGET_PEDIA_JUMP_TO_FATHER, i, -1, False)
				screen.setImageShape("Portrait" + str(i), ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
				screen.setHitMargins("Portrait" + str(i), self.FatherRecordHeight / 5, 0);
				
				screen.attachPanelAt(ScrollPanel, TextBoxName, u"", u"", True, True, PanelStyles.PANEL_STYLE_EMPTY, (self.FatherRecordWidth * (j - 1)), self.TopPanelHeight, self.FatherRecordWidth + (self.FatherRecordWidth / 2), (self.FatherRecordHeight / 2), WidgetTypes.WIDGET_GENERAL, -1, -1 )
				screen.moveToFront(TextBoxName)
				if (len(szFatherTitle) > 55 and self.XResolution < 1280):
					screen.setTextAt("FatherID" + str(i), TextBoxName, "<font=3>" + szFatherTitle + "</font>", CvUtil.FONT_LEFT_JUSTIFY, 10, self.FatherRecordHeight / 15, -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_PEDIA_JUMP_TO_FATHER, i, -1 )
				else:
					screen.setTextAt("FatherID" + str(i), TextBoxName, "<font=4>" + szFatherTitle + "</font>", CvUtil.FONT_LEFT_JUSTIFY, 10, self.FatherRecordHeight / 15, -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_PEDIA_JUMP_TO_FATHER, i, -1 )
				screen.attachMultilineTextAt(TextBoxName, "FatherInfo" + str(i), szFatherData, 10, 40, 6 * self.FatherRecordWidth / 5, (self.FatherRecordHeight / 2) - 45, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

# TAC 0201 Start: Test if FF was rejected before by active player. Set flag in upper row if FF unavailable
				if activeTeam.isFatherIgnore(i):
					screen.addDDSGFCAt("Flag Icon" + str(i), ScrollPanel, ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL").getPath(), (self.FatherRecordWidth * (j - 1)) + self.FatherRecordWidth + (self.FatherRecordHeight / 20), self.TopPanelHeight + (self.FatherRecordHeight / 2) - (self.FatherRecordHeight / 12), (self.FatherRecordHeight / 5), (self.FatherRecordHeight / 5), WidgetTypes.WIDGET_PEDIA_DESCRIPTION, CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT, gc.getInfoTypeForString("CONCEPT_FATHERS"), False )	
					szTempBuffer = localText.getText("TXT_KEY_FATHER_REJECTED", ())
					szTempBuffer = localText.changeTextColor(szTempBuffer, gc.getInfoTypeForString("COLOR_FONT_RED"))
					screen.setTextAt("FatherOwner" + str(i), ScrollPanel, "<font=3>" + szTempBuffer + "</font>", CvUtil.FONT_RIGHT_JUSTIFY, (self.FatherRecordWidth * (j - 1)) + self.FatherRecordWidth + (self.FatherRecordHeight / 20), self.TopPanelHeight + (self.FatherRecordHeight / 2) - (self.FatherRecordHeight / 12) + (self.FatherRecordWidth / 5), -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_PEDIA_DESCRIPTION, CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT, gc.getInfoTypeForString("CONCEPT_FATHERS") )
# TAC End
				
				if (gc.getGame().getFatherTeam(i) != TeamTypes.NO_TEAM):
					for k in range(gc.getMAX_PLAYERS()):
						if (gc.getPlayer(k).getTeam() == gc.getGame().getFatherTeam(i)):
							if (gc.getTeam(gc.getPlayer(gc.getGame().getActivePlayer()).getTeam()).isHasMet(gc.getGame().getFatherTeam(i))):
								screen.addDDSGFCAt("Flag Icon" + str(i), ScrollPanel, ArtFileMgr.getCivilizationArtInfo(gc.getCivilizationInfo(gc.getPlayer(k).getCivilizationType()).getArtDefineTag()).getButton(), (self.FatherRecordWidth * (j - 1)) + self.FatherRecordWidth + (self.FatherRecordHeight / 20), self.TopPanelHeight + (self.FatherRecordHeight / 2) - (self.FatherRecordHeight / 12), (self.FatherRecordHeight / 5), (self.FatherRecordHeight / 5), WidgetTypes.WIDGET_GENERAL, -1, -1, False )
								for m in range(gc.getMAX_PLAYERS()):
									if (gc.getPlayer(m).getTeam() == gc.getGame().getFatherTeam(i)):
										szTempBuffer = u"<color=%d,%d,%d,%d>%s</color>  " %(gc.getPlayer(m).getPlayerTextColorR(), gc.getPlayer(m).getPlayerTextColorG(), gc.getPlayer(m).getPlayerTextColorB(), gc.getPlayer(m).getPlayerTextColorA(), gc.getPlayer(m).getName())
										if (len(szTempBuffer) > 50):
											screen.attachMultilineTextAt(ScrollPanel, "FatherOwner" + str(i), "<font=3>" + szTempBuffer + "</font>", (self.FatherRecordWidth * (j - 1)) + (self.FatherRecordHeight / 20), self.TopPanelHeight + (self.FatherRecordHeight / 2), self.FatherRecordWidth, 55, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
										else:
											screen.setTextAt("FatherOwner" + str(i), ScrollPanel, "<font=4>" + szTempBuffer + "</font>", CvUtil.FONT_RIGHT_JUSTIFY, (self.FatherRecordWidth * (j - 1)) + self.FatherRecordWidth + (self.FatherRecordHeight / 20), self.TopPanelHeight + (self.FatherRecordHeight / 2) - (self.FatherRecordHeight / 12) + (self.FatherRecordWidth / 5), -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
										break

							else:
								screen.addDDSGFCAt("Flag Icon" + str(i), ScrollPanel, ArtFileMgr.getMiscArtInfo("LEADERHEAD_RANDOM").getPath(), (self.FatherRecordWidth * (j - 1)) + self.FatherRecordWidth + (self.FatherRecordHeight / 20), self.TopPanelHeight + (self.FatherRecordHeight / 2) - (self.FatherRecordHeight / 12), (self.FatherRecordHeight / 5), (self.FatherRecordHeight / 5), WidgetTypes.WIDGET_GENERAL, -1, -1, False )
							break
				

			#reset so that it offsets from the father record's panel			
			for iFatherPointType in range(gc.getNumFatherPointInfos()):
				if activeTeam.getFatherPointCost(i, iFatherPointType) > 0:
					if iBottomBarPointType != -1:
						szPolCost = u"%i%c " % (activeTeam.getFatherPointCost(i, iBottomBarPointType), gc.getFatherPointInfo(iBottomBarPointType).getChar())
					if iTopBarPointType != -1:
						szTypeCost = u"%i%c " % (activeTeam.getFatherPointCost(i, iTopBarPointType), gc.getFatherPointInfo(iTopBarPointType).getChar())

			if iTopBarPointType != -1:
				screen.setTextAt("FatherCostTop" + str(i), ScrollPanel, u"<font=3b>" + szTypeCost + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, (self.FatherRecordWidth * j) + (self.FatherRecordWidth / 2), (self.FATHERLIST_HEIGHT / 2) - (self.BarAreaHeight / 2), -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_PEDIA_JUMP_TO_FATHER, i, -1 )
			if iBottomBarPointType != -1:
				screen.setTextAt("FatherCostBottom" + str(i), ScrollPanel, u"<font=3b>" + szPolCost + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, (self.FatherRecordWidth * j) + (self.FatherRecordWidth / 2), (self.FATHERLIST_HEIGHT / 2) + (self.BarAreaHeight / 2), -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_PEDIA_JUMP_TO_FATHER, i, -1 )

			# Place Medalions
			if (iBottomBarPointType != -1 and iTopBarPointType != -1):
			
				BottomBarPoints = activeTeam.getFatherPoints(iBottomBarPointType)
				if (activeTeam.getFatherPointCost(i, iBottomBarPointType) >= BottomBarPoints):
					if (not PlacedBottomMedalion):
						FollowingTypeCost = activeTeam.getFatherPointCost(self.Fathers[self.TypeSelection][index], iBottomBarPointType)
						StartX = self.FatherRecordWidth * (j - 1)
						FatherWidth = self.FatherRecordWidth;
						if index == 0:
							PreceedingTypeCost = 0
							StartX = self.FatherRecordWidth / 2
							FatherWidth /= 2
						else:
							PreceedingTypeCost = activeTeam.getFatherPointCost(self.Fathers[self.TypeSelection][index - 1], iBottomBarPointType)

						SplitPercent =  float(BottomBarPoints - PreceedingTypeCost) / float(FollowingTypeCost - PreceedingTypeCost)
						MedalionX = StartX + (self.FatherRecordWidth / 2) - (self.BarAreaHeight / 4) + (self.FatherRecordWidth / 16) + int(SplitPercent * FatherWidth) + 4

						screen.addDDSGFCAt("BottomFillBar", ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_FILL").getPath(), self.XResolution / 5, (self.FATHERLIST_HEIGHT / 2), MedalionX - self.FatherRecordWidth + + (self.BarAreaHeight / 4), self.BarAreaHeight / 2, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt("BottomEmptyBar", ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_EMPTY").getPath(), MedalionX + 20, (self.FATHERLIST_HEIGHT / 2), (self.FatherRecordWidth * len(self.Fathers[iType])) + (self.FatherRecordWidth / 2) - MedalionX - 5, self.BarAreaHeight / 2, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt("BottomBarMedalion", ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_MEDALLION").getPath(), MedalionX, (self.FATHERLIST_HEIGHT / 2) + (self.BarAreaHeight * 5 / 24) - (self.BarAreaHeight / 5), self.BarAreaHeight / 2, self.BarAreaHeight / 2, WidgetTypes.WIDGET_GENERAL, -1, -1, False )

						screen.setLabelAt("BottomMedalionLabel", "BottomBarMedalion", u"<font=3b>" + str(BottomBarPoints) + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.BarAreaHeight * 9 / 40, self.BarAreaHeight * 9 / 40, -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
						PlacedBottomMedalion = True

				TopBarPoints = activeTeam.getFatherPoints(iTopBarPointType)
				if (activeTeam.getFatherPointCost(i, iTopBarPointType) > TopBarPoints):
					if (not PlacedTopMedalion):
						FollowingPoliticalCost = activeTeam.getFatherPointCost(self.Fathers[self.TypeSelection][index], iTopBarPointType)
						StartX = self.FatherRecordWidth * (j - 1)
						FatherWidth = self.FatherRecordWidth;
						if index == 0:
							PreceedingPoliticalCost = 0
							StartX = self.FatherRecordWidth / 2
							FatherWidth /= 2
						else:
							PreceedingPoliticalCost = activeTeam.getFatherPointCost(self.Fathers[self.TypeSelection][index - 1], iTopBarPointType)

						SplitPercent =  float(TopBarPoints - PreceedingPoliticalCost) / float(FollowingPoliticalCost - PreceedingPoliticalCost)
						
					
						MedalionX = StartX + (self.FatherRecordWidth / 2) - (self.BarAreaHeight / 4) + (self.FatherRecordWidth / 16) + int(SplitPercent * FatherWidth) + 4

						screen.addDDSGFCAt("TopFillBar", ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_FILL").getPath(), self.XResolution / 5, (self.FATHERLIST_HEIGHT / 2) - (self.BarAreaHeight / 2), MedalionX - self.FatherRecordWidth + (self.BarAreaHeight / 4), self.BarAreaHeight / 2, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt("TopEmptyBar", ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_EMPTY").getPath(), MedalionX + 20, (self.FATHERLIST_HEIGHT / 2) - (self.BarAreaHeight / 2), (self.FatherRecordWidth * len(self.Fathers[iType])) + (self.FatherRecordWidth / 2) - MedalionX - 5, self.BarAreaHeight / 2, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt("TopBarMedalion", ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_MEDALLION").getPath(), MedalionX, (self.FATHERLIST_HEIGHT / 2) - (self.BarAreaHeight * 7 / 24) - (self.BarAreaHeight / 5), self.BarAreaHeight / 2, self.BarAreaHeight / 2, WidgetTypes.WIDGET_GENERAL, -1, -1, False )

						screen.setLabelAt("TopMedalionLabel", "TopBarMedalion", u"<font=3b>" + str(TopBarPoints) + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.BarAreaHeight * 9 / 40, self.BarAreaHeight * 9 / 40, -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
						PlacedTopMedalion = True
			
			else:
				BottomBarPoints = activeTeam.getFatherPoints(iBottomBarPointType)
				if (activeTeam.getFatherPointCost(i, iBottomBarPointType) >= BottomBarPoints):
					if (not PlacedBottomMedalion):
						FollowingTypeCost = activeTeam.getFatherPointCost(self.Fathers[self.TypeSelection][index], iBottomBarPointType)
						StartX = self.FatherRecordWidth * (j - 1)
						FatherWidth = self.FatherRecordWidth;
						if index == 0:
							PreceedingTypeCost = 0
							StartX = self.FatherRecordWidth / 2
							FatherWidth /= 2
						else:
							PreceedingTypeCost = activeTeam.getFatherPointCost(self.Fathers[self.TypeSelection][index - 1], iBottomBarPointType)

						SplitPercent =  float(BottomBarPoints - PreceedingTypeCost) / float(FollowingTypeCost - PreceedingTypeCost)
						MedalionX = StartX + (self.FatherRecordWidth / 2) - (self.BarAreaHeight / 4) + (self.FatherRecordWidth / 16) + int(SplitPercent * FatherWidth) + 4

						screen.addDDSGFCAt("BottomFillBar", ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_FILL").getPath(), self.XResolution / 5, (self.FATHERLIST_HEIGHT / 2) - (self.BarAreaHeight / 4), MedalionX - self.FatherRecordWidth + + (self.BarAreaHeight / 4), self.BarAreaHeight / 2, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt("BottomEmptyBar", ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_EMPTY").getPath(), MedalionX + 20, (self.FATHERLIST_HEIGHT / 2) - (self.BarAreaHeight / 4), (self.FatherRecordWidth * len(self.Fathers[iType])) + (self.FatherRecordWidth / 2) - MedalionX - 5, self.BarAreaHeight / 2, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt("BottomBarMedalion", ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_MEDALLION").getPath(), MedalionX, (self.FATHERLIST_HEIGHT / 2) - (self.BarAreaHeight / 4) + (self.BarAreaHeight * 5 / 24) - (self.BarAreaHeight / 5), self.BarAreaHeight / 2, self.BarAreaHeight / 2, WidgetTypes.WIDGET_GENERAL, -1, -1, False )

						screen.setLabelAt("BottomMedalionLabel", "BottomBarMedalion", u"<font=3b>" + str(BottomBarPoints) + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.BarAreaHeight * 9 / 40, self.BarAreaHeight * 9 / 40, -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
						PlacedBottomMedalion = True


		# Place maxed out bars
		if (iBottomBarPointType != -1 and iTopBarPointType != -1):
			if (not PlacedBottomMedalion):
				BottomBarPoints = activeTeam.getFatherPoints(iBottomBarPointType)
				index = len(self.Fathers[self.TypeSelection])
				if BottomBarPoints == activeTeam.getFatherPointCost(index - 1, iBottomBarPointType):
					SplitPercent = 0
				else:
					SplitPercent = 0.5
					
				MedalionX = (self.FatherRecordWidth * index) + (self.FatherRecordWidth / 2) - (self.BarAreaHeight / 4) + (self.FatherRecordWidth / 16) + int(SplitPercent * self.FatherRecordWidth) + 4
				screen.addDDSGFCAt("BottomFillBar", ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_FILL").getPath(), self.XResolution / 5, (self.FATHERLIST_HEIGHT / 2), MedalionX - self.FatherRecordWidth + (self.BarAreaHeight / 4), self.BarAreaHeight / 2, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
				screen.addDDSGFCAt("BottomBarMedalion", ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_MEDALLION").getPath(), MedalionX, (self.FATHERLIST_HEIGHT / 2) + (self.BarAreaHeight * 5 / 24) - (self.BarAreaHeight / 5), self.BarAreaHeight / 2, self.BarAreaHeight / 2, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
				screen.setLabelAt("BottomMedalionLabel", "BottomBarMedalion", u"<font=3b>" + str(BottomBarPoints) + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.BarAreaHeight * 9 / 40, self.BarAreaHeight * 9 / 40, -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

			if (not PlacedTopMedalion):
				TopBarPoints = activeTeam.getFatherPoints(iTopBarPointType)
				index = len(self.Fathers[self.TypeSelection])
				if TopBarPoints == activeTeam.getFatherPointCost(index - 1, iTopBarPointType):
					SplitPercent = 0
				else:
					SplitPercent = 0.5
				MedalionX = (self.FatherRecordWidth * index) + (self.FatherRecordWidth / 2) - (self.BarAreaHeight / 4) + (self.FatherRecordWidth / 16) + int(SplitPercent * self.FatherRecordWidth) + 4
				screen.addDDSGFCAt("TopFillBar", ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_FILL").getPath(), self.XResolution / 5, (self.FATHERLIST_HEIGHT / 2) - (self.BarAreaHeight / 2), MedalionX - self.FatherRecordWidth + (self.BarAreaHeight / 4), self.BarAreaHeight / 2, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
				screen.addDDSGFCAt("TopBarMedalion", ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_MEDALLION").getPath(), MedalionX, (self.FATHERLIST_HEIGHT / 2) - (self.BarAreaHeight * 7 / 24) - (self.BarAreaHeight / 5), self.BarAreaHeight / 2, self.BarAreaHeight / 2, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
				screen.setLabelAt("TopMedalionLabel", "TopBarMedalion", u"<font=3b>" + str(TopBarPoints) + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.BarAreaHeight * 9 / 40, self.BarAreaHeight * 9 / 40, -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
			
		elif (not PlacedBottomMedalion):	
			BottomBarPoints = activeTeam.getFatherPoints(iBottomBarPointType)
			index = len(self.Fathers[self.TypeSelection])
			if BottomBarPoints == activeTeam.getFatherPointCost(index - 1, iBottomBarPointType):
				SplitPercent = 0
			else:
				SplitPercent = 0.5
				
			MedalionX = (self.FatherRecordWidth * index) + (self.FatherRecordWidth / 2) - (self.BarAreaHeight / 4) + int(SplitPercent * self.FatherRecordWidth) + 4
			screen.addDDSGFCAt("BottomFillBar", ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_FILL").getPath(), self.XResolution / 5, (self.FATHERLIST_HEIGHT / 2) - (self.BarAreaHeight / 4), MedalionX - self.FatherRecordWidth + (self.BarAreaHeight / 4), self.BarAreaHeight / 2, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
			screen.addDDSGFCAt("BottomBarMedalion", ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_MEDALLION").getPath(), MedalionX, (self.FATHERLIST_HEIGHT / 2) - (self.BarAreaHeight / 4) + (self.BarAreaHeight * 5 / 24) - (self.BarAreaHeight / 5), self.BarAreaHeight / 2, self.BarAreaHeight / 2, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
			screen.setLabelAt("BottomMedalionLabel", "BottomBarMedalion", u"<font=3b>" + str(BottomBarPoints) + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.BarAreaHeight * 9 / 40, self.BarAreaHeight * 9 / 40, -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		# Place Indicators
		for index in range(len(self.Fathers[self.TypeSelection])):
			i = self.Fathers[self.TypeSelection][index]
			iType = gc.getFatherInfo(i).getFatherCategory()
			j = index + 1

			if (iBottomBarPointType != -1 and iTopBarPointType != -1):
				screen.addDDSGFCAt("IndicatorBar" + str(i), ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_INDICATOR").getPath(), (self.FatherRecordWidth * j) + (self.FatherRecordWidth / 2), (self.FATHERLIST_HEIGHT / 2) - (self.BarAreaHeight / 3), self.FatherRecordWidth / 8, self.BarAreaHeight * 7 / 10, WidgetTypes.WIDGET_GENERAL, -1, -1, False)
				screen.addDDSGFCAt("TopIndicatorDot" + str(i), ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_DOT").getPath(), (self.FatherRecordWidth * j) + (self.FatherRecordWidth / 2), (self.FATHERLIST_HEIGHT / 2) - (self.BarAreaHeight / 3) - (self.FatherRecordWidth / 16), self.FatherRecordWidth / 8, self.FatherRecordWidth / 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False)
				screen.addDDSGFCAt("BottomIndicatorDot" + str(i), ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_DOT").getPath(), (self.FatherRecordWidth * j) + (self.FatherRecordWidth / 2), (self.FATHERLIST_HEIGHT / 2) + (self.BarAreaHeight / 3) - (self.FatherRecordWidth / 16), self.FatherRecordWidth / 8, self.FatherRecordWidth / 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False)
			else:
				screen.addDDSGFCAt("IndicatorBar" + str(i), ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_INDICATOR").getPath(), (self.FatherRecordWidth * j) + (self.FatherRecordWidth / 2), (self.FATHERLIST_HEIGHT / 2) - (self.BarAreaHeight / 4), self.FatherRecordWidth / 8, self.BarAreaHeight * 5 / 10, WidgetTypes.WIDGET_GENERAL, -1, -1, False)
				screen.addDDSGFCAt("TopIndicatorDot" + str(i), ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_DOT").getPath(), (self.FatherRecordWidth * j) + (self.FatherRecordWidth / 2), (self.FATHERLIST_HEIGHT / 2) - (self.BarAreaHeight / 4) - (self.FatherRecordWidth / 16), self.FatherRecordWidth / 8, self.FatherRecordWidth / 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False)
				screen.addDDSGFCAt("BottomIndicatorDot" + str(i), ScrollPanel, ArtFileMgr.getInterfaceArtInfo("FATHER_PROGRESS_BAR_DOT").getPath(), (self.FatherRecordWidth * j) + (self.FatherRecordWidth / 2), (self.FATHERLIST_HEIGHT / 2) + (self.BarAreaHeight / 4) - (self.FatherRecordWidth / 16), self.FatherRecordWidth / 8, self.FatherRecordWidth / 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False)

		#Bring medallions to front
		screen.moveToFront("BottomBarMedalion")
		screen.moveToFront("TopBarMedalion")

	def updateFatherRecords (self, bForce):
		self.placeFathers()

	def CivDropDown( self, inputClass ):
		if ( inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED ):
			screen = self.getScreen()
			iIndex = screen.getSelectedPullDownID("CivDropDown")
			self.iCivSelected = screen.getPullDownData("CivDropDown", iIndex)
			self.updateFatherRecords(false)

	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		' Calls function mapped in FoundingFatherScreenInputMap'
		screen = self.getScreen()

		# only get from the map if it has the key
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED):
			self.CivDropDown(inputClass)
			return 1

		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if (inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL and inputClass.getData1() == 1111):
				self.TypeSelection = inputClass.getData2()
				self.updateFatherRecords(false)
				for x in range(gc.getNumFatherCategoryInfos()):
					if (x == self.TypeSelection):
						self.setTab(x, True)
						screen.show("FoundingFatherList" + str(x))
					else:
						screen.hide("FoundingFatherList" + str(x))
						self.setTab(x, False)
				return 1

		return 0

	def update(self, fDelta):
		return
