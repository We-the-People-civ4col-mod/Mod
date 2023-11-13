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

class CvPediaLeader:
	"Civilopedia Screen for Leaders"

	def __init__(self, main):
		self.iLeader = -1
		self.top = main

	# Screen construction function
	def interfaceScreen(self, iLeader, x, y, h, w):

		self.iLeader = iLeader

		self.X_LEADERHEAD_PANE = x
		self.Y_LEADERHEAD_PANE = y
		self.W_LEADERHEAD_PANE = (w * 45 / 100)
		self.H_LEADERHEAD_PANE = (h * 50 / 100)
		
		self.H_LEADERHEAD = (h * 45 / 100)
		if (self.H_LEADERHEAD < 384):
			self.H_LEADERHEAD = 384			
		self.W_LEADERHEAD = self.H_LEADERHEAD * 4 / 5
		
		if (self.W_LEADERHEAD < self.W_LEADERHEAD_PANE and self.H_LEADERHEAD > self.H_LEADERHEAD_PANE):
			self.H_LEADERHEAD_PANE = self.H_LEADERHEAD + (h * 5 / 100)
			
		self.X_LEADERHEAD = x + (self.W_LEADERHEAD_PANE / 2) - (self.W_LEADERHEAD / 2)
		self.Y_LEADERHEAD = y + (self.H_LEADERHEAD_PANE / 2) - (self.H_LEADERHEAD / 2)
		if (self.Y_LEADERHEAD < y):
			self.Y_LEADERHEAD = y

		self.X_TRAITS = x + self.W_LEADERHEAD_PANE + (w * 5 / 100)
		self.Y_TRAITS = y
		self.W_TRAITS = (w * 50 / 100)

		self.X_HISTORY = x
		if (self.H_LEADERHEAD > self.H_LEADERHEAD_PANE):
			self.Y_HISTORY = y + self.H_LEADERHEAD + (h * 5 / 100)
			self.H_HISTORY = h - self.H_LEADERHEAD - (h * 5 / 100)
			self.H_TRAITS = self.H_LEADERHEAD
		else:
			self.Y_HISTORY = y + self.H_LEADERHEAD_PANE + (h * 5 / 100)
			self.H_HISTORY = h - self.H_LEADERHEAD_PANE - (h * 5 / 100)
			self.H_TRAITS = self.H_LEADERHEAD_PANE
	
		self.W_HISTORY = w

		self.X_CIV = x + self.W_LEADERHEAD_PANE + (w * 5 / 100)
		self.Y_CIV = y
		self.W_CIV = (h * 10 / 100)
		self.H_CIV = (h * 10 / 100)

		self.X_CIVIC = self.X_HISTORY + self.W_CIV + 10
		self.Y_CIVIC = self.Y_LEADERHEAD_PANE - 5
		self.W_CIVIC = self.W_HISTORY - self.W_CIV - 10
		self.H_CIVIC = 80

		self.top.deleteAllWidgets()
		screen = self.top.getScreen()

		bNotActive = (not screen.isActive())
		if bNotActive:
			self.top.setPediaCommonWidgets()

		# Header...
		szHeader = u"<font=4b>" + gc.getLeaderHeadInfo(self.iLeader).getDescription().upper() + u"</font>"
		szHeaderId = self.top.getNextWidgetName()
		screen.setLabel(szHeaderId, "Background", szHeader, CvUtil.FONT_CENTER_JUSTIFY, self.top.X_SCREEN, self.top.Y_TITLE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# Top
		screen.setText(self.top.getNextWidgetName(), "Background", self.top.MENU_TEXT, CvUtil.FONT_LEFT_JUSTIFY, self.top.X_MENU, self.top.Y_MENU, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_PEDIA_MAIN, CivilopediaPageTypes.CIVILOPEDIA_PAGE_LEADER, -1)

		if self.top.iLastScreen	!= CvScreenEnums.PEDIA_LEADER or bNotActive:
			self.placeLinks(true)
			self.top.iLastScreen = CvScreenEnums.PEDIA_LEADER
		else:
			self.placeLinks(true)

		# Leaderhead
		leaderPanelWidget = self.top.getNextWidgetName()
		screen.addPanel( leaderPanelWidget, "", "", true, true, self.X_LEADERHEAD_PANE, self.Y_LEADERHEAD_PANE, self.W_LEADERHEAD_PANE, self.H_LEADERHEAD_PANE, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		self.leaderWidget = self.top.getNextWidgetName()
		screen.addLeaderheadGFC(self.leaderWidget, self.iLeader, AttitudeTypes.ATTITUDE_PLEASED, self.X_LEADERHEAD, self.Y_LEADERHEAD, self.W_LEADERHEAD, self.H_LEADERHEAD, WidgetTypes.WIDGET_GENERAL, -1, -1)

		print "Head Hight " + str(self.H_LEADERHEAD)
		print "Head Width " + str(self.W_LEADERHEAD)
		
		self.placeHistory()
		#self.placeCiv()
		self.placeTraits()

	def placeCiv(self):
		screen = self.top.getScreen()

		for iCiv in range(gc.getNumCivilizationInfos()):
			civ = gc.getCivilizationInfo(iCiv)
			if civ.isLeaders(self.iLeader):
				screen.setImageButton(self.top.getNextWidgetName(), civ.getButton(), self.X_CIV, self.Y_CIV, self.W_CIV, self.H_CIV, WidgetTypes.WIDGET_GENERAL, -1, -1)

	def placeTraits(self):
		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_PEDIA_TRAITS", ()), "", true, false, self.X_TRAITS, self.Y_TRAITS, self.W_TRAITS, self.H_TRAITS, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		listName = self.top.getNextWidgetName()

		iNumCivs = 0
		iLeaderCiv = -1
		for iCiv in range(gc.getNumCivilizationInfos()):
			civ = gc.getCivilizationInfo(iCiv)
			if civ.isLeaders(self.iLeader):
				iNumCivs += 1
				iLeaderCiv = iCiv

		if iNumCivs == 1:
			szSpecialText = CyGameTextMgr().parseLeaderTraits(self.iLeader, iLeaderCiv, False, True)
		else:
			szSpecialText = CyGameTextMgr().parseLeaderTraits(self.iLeader, -1, False, True)
		szSpecialText = szSpecialText[1:]
		screen.addMultilineText(listName, szSpecialText, self.X_TRAITS + 15, self.Y_TRAITS + 40, self.W_TRAITS - 30, self.H_TRAITS - 60, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

	def placeHistory(self):
		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_CIVILOPEDIA_HISTORY", ()), "", true, true, self.X_HISTORY, self.Y_HISTORY, self.W_HISTORY, self.H_HISTORY, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		historyTextName = self.top.getNextWidgetName()
		CivilopediaText = gc.getLeaderHeadInfo(self.iLeader).getCivilopedia()
		CivilopediaText = u"<font=2>" + CivilopediaText + u"</font>"
		screen.addMultilineText( historyTextName, CivilopediaText, self.X_HISTORY + 15, self.Y_HISTORY + 40, self.W_HISTORY- 30, self.H_HISTORY - 60, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )

	def placeLinks(self, bRedraw):
		screen = self.top.getScreen()

		if bRedraw:
			screen.clearListBoxGFC(self.top.LIST_ID)

		# sort leaders alphabetically
		rowListName=[(0,0)]*gc.getNumLeaderHeadInfos()
		for j in range(gc.getNumLeaderHeadInfos()):
			rowListName[j] = (gc.getLeaderHeadInfo(j).getDescription(), j)
		rowListName.sort()

		i = 0
		iSelected = 0
		for iI in range(gc.getNumLeaderHeadInfos()):
			if (not gc.getLeaderHeadInfo(rowListName[iI][1]).isGraphicalOnly()):
				if (not gc.getDefineINT("CIVILOPEDIA_SHOW_ACTIVE_CIVS_ONLY") or not gc.getGame().isFinalInitialized() or gc.getGame().isLeaderEverActive(rowListName[iI][1])):
					if bRedraw:
						screen.appendListBoxStringNoUpdate(self.top.LIST_ID, rowListName[iI][0], WidgetTypes.WIDGET_PEDIA_JUMP_TO_LEADER, rowListName[iI][1], -1, CvUtil.FONT_LEFT_JUSTIFY)
					if rowListName[iI][1] == self.iLeader:
						iSelected = i
					i += 1

		if bRedraw:
			screen.updateListBox(self.top.LIST_ID)

		screen.setSelectedListBoxStringGFC(self.top.LIST_ID, iSelected)


	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CHARACTER):
			if (inputClass.getData() == int(InputTypes.KB_0)):
				self.top.getScreen().performLeaderheadAction(self.leaderWidget, LeaderheadAction.LEADERANIM_GREETING)
			elif (inputClass.getData() == int(InputTypes.KB_6)):
				self.top.getScreen().performLeaderheadAction(self.leaderWidget, LeaderheadAction.LEADERANIM_DISAGREE)
			elif (inputClass.getData() == int(InputTypes.KB_7)):
				self.top.getScreen().performLeaderheadAction(self.leaderWidget, LeaderheadAction.LEADERANIM_AGREE)
			elif (inputClass.getData() == int(InputTypes.KB_8)):
				self.top.getScreen().performLeaderheadAction(self.leaderWidget, LeaderheadAction.LEADERANIM_OFFER_PINKY)
			elif (inputClass.getData() == int(InputTypes.KB_EQUALS)):
				self.top.getScreen().performLeaderheadAction(self.leaderWidget, LeaderheadAction.LEADERANIM_PINKY_ACCEPT)
				CyInterface().playGeneralSound("AS2D_KISS_MY_RING")
			elif (inputClass.getData() == int(InputTypes.KB_MINUS)):
				self.top.getScreen().performLeaderheadAction(self.leaderWidget, LeaderheadAction.LEADERANIM_PINKY_REJECT)
			elif (inputClass.getData() == int(InputTypes.KB_1)):
				self.top.getScreen().setLeaderheadMood(self.leaderWidget, AttitudeTypes.ATTITUDE_FRIENDLY)
				self.top.getScreen().performLeaderheadAction(self.leaderWidget, LeaderheadAction.NO_LEADERANIM)
			elif (inputClass.getData() == int(InputTypes.KB_2)):
				self.top.getScreen().setLeaderheadMood(self.leaderWidget, AttitudeTypes.ATTITUDE_PLEASED)
				self.top.getScreen().performLeaderheadAction(self.leaderWidget, LeaderheadAction.NO_LEADERANIM)
			elif (inputClass.getData() == int(InputTypes.KB_3)):
				self.top.getScreen().setLeaderheadMood(self.leaderWidget, AttitudeTypes.ATTITUDE_CAUTIOUS)
				self.top.getScreen().performLeaderheadAction(self.leaderWidget, LeaderheadAction.NO_LEADERANIM)
			elif (inputClass.getData() == int(InputTypes.KB_4)):
				self.top.getScreen().setLeaderheadMood(self.leaderWidget, AttitudeTypes.ATTITUDE_ANNOYED)
				self.top.getScreen().performLeaderheadAction(self.leaderWidget, LeaderheadAction.NO_LEADERANIM)
			elif (inputClass.getData() == int(InputTypes.KB_5)):
				self.top.getScreen().setLeaderheadMood(self.leaderWidget, AttitudeTypes.ATTITUDE_FURIOUS)
				self.top.getScreen().performLeaderheadAction(self.leaderWidget, LeaderheadAction.NO_LEADERANIM)
			else:
				self.top.getScreen().leaderheadKeyInput(self.leaderWidget, inputClass.getData())
		return 0


