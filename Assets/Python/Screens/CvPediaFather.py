## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
# Sid Meier's Civilization 4
# Copyright Firaxis Games 2005
#
# CvScreen - Base class for all of the Screens

from CvPythonExtensions import *
import CvUtil
import CvScreen
import CvScreenEnums
import ScreenInput

gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class CvPediaFather( CvScreen.CvScreen ):
	"Civilopedia Base Screen"

	def __init__(self, main):
		self.iFather = -1
		self.top = main
		self.XResolution = 0
		self.YResolution = 0
		self.PICTURE_FF = 3
		self.BIG_PICTURE_FF = 4
		self.BIGFF = 0

	# Screen construction function
	def interfaceScreen(self, iFather, x, y, h, w):
		self.iFather = iFather

		self.X_ICON_PANE = x + ((w * 50 / 100) / 2)  - ((h * 40 / 100) / 2)
		self.Y_ICON_PANE = y
		self.W_ICON_PANE = (h * 40 / 100)
		self.H_ICON_PANE = (h * 40 / 100)

		self.W_ICON = (h * 40 / 100)
		self.H_ICON = (h * 40 / 100)
		self.X_ICON = x + ((w * 50 / 100) / 2)  - ((h * 40 / 100) / 2)
		self.Y_ICON = y + ((self.H_ICON_PANE - self.H_ICON) / 2) + (h * 5 / 100)

		self.X_INFOS = x
		self.Y_INFOS = y + (h * 75 / 100)
		self.W_INFOS = (w * 50 / 100)
		self.H_INFOS = (h * 25 / 100)

		self.X_PRE_REQS = x
		self.Y_PRE_REQS = y + (h * 45 / 100)
		self.W_PRE_REQS = (w * 50 / 100)
		self.H_PRE_REQS = (h * 25 / 100)

		self.X_HISTORY = x + (w * 55 / 100)
		self.Y_HISTORY = y + (h * 20 / 100)
		self.W_HISTORY = (w * 45 / 100)
		self.H_HISTORY = (h * 80 / 100)

		self.X_CATAGORY = x + (w * 55 / 100)
		self.Y_CATAGORY = y
		self.W_CATAGORY = (w * 45 / 100)
		self.H_CATAGORY = (h * 15 / 100)

		self.top.deleteAllWidgets()
		screen = self.top.getScreen()

		bNotActive = (not screen.isActive())
		if bNotActive:
			self.top.setPediaCommonWidgets()

		# Header...
		szHeader = u"<font=4b>" + gc.getFatherInfo(self.iFather).getDescription().upper() + u"</font>"
		szHeaderId = self.top.getNextWidgetName()
		screen.setLabel(szHeaderId, "Background", szHeader, CvUtil.FONT_CENTER_JUSTIFY, self.top.X_SCREEN, self.top.Y_TITLE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# Top
		screen.setText(self.top.getNextWidgetName(), "Background", self.top.MENU_TEXT, CvUtil.FONT_LEFT_JUSTIFY, self.top.X_MENU, self.top.Y_MENU, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_PEDIA_MAIN, CivilopediaPageTypes.CIVILOPEDIA_PAGE_FATHER, -1)

		if self.top.iLastScreen	!= CvScreenEnums.PEDIA_FATHER or bNotActive:
			self.placeLinks(true)
			self.top.iLastScreen = CvScreenEnums.PEDIA_FATHER
		else:
			self.placeLinks(true)

		self.placeHistory()
		self.placeInfo()
		self.placeCategory()
		self.placeButton()
		self.placePreReq()

	def placeButton(self):

		screen = self.top.getScreen()
		#screen.addPanel(self.top.getNextWidgetName(), "", "", False, False, self.X_ICON_PANE, self.Y_ICON_PANE, self.W_ICON_PANE, self.H_ICON_PANE, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1)		
		
		#TAC - Add Picture-Frame / Fankman
		screen.addDDSGFC(self.top.getNextWidgetName(), ArtFileMgr.getInterfaceArtInfo("INTERFACE_CONGRESS_BLACK").getPath(), self.X_ICON_PANE, self.Y_ICON_PANE, self.H_ICON_PANE, self.W_ICON_PANE, WidgetTypes.WIDGET_GENERAL, self.BIG_PICTURE_FF, -1)
		screen.addDDSGFC(self.top.getNextWidgetName(), gc.getFatherInfo(self.iFather).getPortrait(), self.X_ICON, self.Y_ICON, self.H_ICON, self.W_ICON, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDDSGFC(self.top.getNextWidgetName(), ArtFileMgr.getInterfaceArtInfo("INTERFACE_CONGRESS_BOX").getPath(), self.X_ICON_PANE, self.Y_ICON_PANE, self.H_ICON_PANE, self.W_ICON_PANE, WidgetTypes.WIDGET_GENERAL, self.BIG_PICTURE_FF, -1)
		
	def placePreReq(self):

		screen = self.top.getScreen()
		panelName = self.top.getNextWidgetName()
		screen.addPanel(panelName, CyTranslator().getText("TXT_KEY_PEDIA_REQUIRES", ()), "", true, false, self.X_PRE_REQS, self.Y_PRE_REQS, self.W_PRE_REQS, self.H_PRE_REQS, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		panelName = self.top.getNextWidgetName()
		screen.addListBoxGFC(panelName, "", self.X_PRE_REQS + 15, self.Y_PRE_REQS + 40, self.W_PRE_REQS - 30, self.H_PRE_REQS - 60, TableStyles.TABLE_STYLE_EMPTY)
		screen.enableSelect(panelName, False)

		for iFatherPointType in range(gc.getNumFatherPointInfos()):
			szName = gc.getFatherPointInfo(iFatherPointType).getDescription()

			if self.top.iActivePlayer == -1:
				iCost = gc.getFatherInfo(self.iFather).getPointCost(iFatherPointType)
			else:
				iCost = gc.getTeam(gc.getPlayer(self.top.iActivePlayer).getTeam()).getFatherPointCost(self.iFather, iFatherPointType)

			if (iCost > 0):
				szText = u"<font=4b>" + u"%i %c %s" % (iCost, gc.getFatherPointInfo(iFatherPointType).getChar(), szName) + u"</font>"
				screen.appendListBoxStringNoUpdate(panelName, szText, WidgetTypes.WIDGET_GENERAL, 0, 0, CvUtil.FONT_LEFT_JUSTIFY)
		screen.updateListBox(panelName)

	def placeLinks(self, bRedraw):
		screen = self.top.getScreen()
		if bRedraw:
			screen.clearListBoxGFC(self.top.LIST_ID)

		rowListName =[(0,0)] * gc.getNumFatherInfos()
		for j in range(gc.getNumFatherInfos()):
			rowListName[j] = (gc.getFatherInfo(j).getDescription(), j)
		rowListName.sort()

		i = 0
		iSelected = 0
		for iI in range(gc.getNumFatherInfos()):
			if (not gc.getFatherInfo(rowListName[iI][1]).isGraphicalOnly()):
				if bRedraw:
					screen.appendListBoxStringNoUpdate(self.top.LIST_ID, rowListName[iI][0], WidgetTypes.WIDGET_PEDIA_JUMP_TO_FATHER, rowListName[iI][1], 0, CvUtil.FONT_LEFT_JUSTIFY)
				if rowListName[iI][1] == self.iFather:
					iSelected = i
				i += 1

		if bRedraw:
			screen.updateListBox(self.top.LIST_ID)

		screen.setSelectedListBoxStringGFC(self.top.LIST_ID, iSelected)

	def placeInfo(self):
		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel(panelName, CyTranslator().getText("TXT_KEY_PEDIA_SPECIAL_ABILITIES", ()), "", true, false, self.X_INFOS, self.Y_INFOS, self.W_INFOS, self.H_INFOS, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		listName = self.top.getNextWidgetName()

		szSpecialText = CyGameTextMgr().getFatherHelp(self.iFather, True)
		szSpecialText = szSpecialText[1:]
		screen.addMultilineText(listName, szSpecialText, self.X_INFOS + 15, self.Y_INFOS + 40, self.W_INFOS - 30, self.H_INFOS - 60, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

	def placeCategory(self):
		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel(panelName, CyTranslator().getText("TXT_KEY_PEDIA_FATHER_CATEGORY", ()), "", true, true, self.X_CATAGORY, self.Y_CATAGORY, self.W_CATAGORY, self.H_CATAGORY, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		iCategory = gc.getFatherInfo(self.iFather).getFatherCategory()

		if (iCategory != -1):
			szCatagoryText = u"<font=4>" + gc.getFatherCategoryInfo(iCategory).getDescription() + u"</font>"
			listName = self.top.getNextWidgetName()
			screen.addMultilineText(listName, szCatagoryText.upper(), self.X_CATAGORY + 15, self.Y_CATAGORY + 40, self.W_CATAGORY - 30, self.H_CATAGORY - 60, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

	def placeHistory(self):
		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel(panelName, localText.getText("TXT_KEY_CIVILOPEDIA_HISTORY", ()), "", true, true, self.X_HISTORY, self.Y_HISTORY, self.W_HISTORY, self.H_HISTORY, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1)

		historyTextName = self.top.getNextWidgetName()
		CivilopediaText = gc.getFatherInfo(self.iFather).getCivilopedia()
		CivilopediaText = u"<font=2>" + CivilopediaText + u"</font>"
		screen.addMultilineText(historyTextName, CivilopediaText, self.X_HISTORY + 15, self.Y_HISTORY + 40, self.W_HISTORY - 30, self.H_HISTORY - 60, WidgetTypes.WIDGET_GENERAL,-1,-1, CvUtil.FONT_LEFT_JUSTIFY)

	def getSortedList(self):
		' returned a list of infos sorted alphabetically '

		# count the items we are going to display
		iNumNonGraphical = 0
		for i in range(gc.getNumFatherInfos()):
			if (not gc.getFatherInfo(i).isGraphicalOnly()):
				iNumNonGraphical += 1

		infoList = [(0,0)] * iNumNonGraphical
		j = 0
		for i in range(gc.getNumFatherInfos()):
			if (not gc.getFatherInfo(i).isGraphicalOnly()):
				infoList[j] = (gc.getFatherInfo(i).getDescription(), i)
				j += 1

		return infoList.sort()

	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		return 0
