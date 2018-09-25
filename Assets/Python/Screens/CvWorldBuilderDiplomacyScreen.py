## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005

import CvUtil
import CvScreenEnums
from CvPythonExtensions import *

# globals
gc = CyGlobalContext()
UserProfile = CyUserProfile()
localText = CyTranslator()

class CvWorldBuilderDiplomacyScreen:
	"Options Screen"

	def __init__(self):

		self.iScreenColumnHeight = 50
		self.m_iScreenHeight = 200
		self.m_iScreenWidth = 200

		self.iDiplomacyTabID	= 0

		self.callbackIFace = "CvScreensInterface"

		self.m_bShown = False
		self.m_iActivePlayer = 1
		self.szDiplomacyTab = "WBDiplomacyTab"

	def getTabControl(self):
		return self.pTabControl

	def getDiplomacyTabName(self):
		return self.szDiplomacyTabName

#########################################################################################
################################## SCREEN CONSTRUCTION ##################################
#########################################################################################

	def initText(self):

		self.szTabControlName = localText.getText("TXT_KEY_WB_DIPLOMACY", ())
		self.szDiplomacyTabName = localText.getText("TXT_KEY_DIPLOMACY", ())
	def initVars(self):
		self.m_bShown = False
		return

	def interfaceScreen (self):
		"Initial creation of the screen"
		self.initText()
		self.initVars()
		screen = CyGInterfaceScreen( "WorldBuilderDiplomacyScreen", CvScreenEnums.WORLDBUILDER_DIPLOMACY_SCREEN )

		screen.setDimensions( 0, 0, screen.getXResolution(), screen.getYResolution() )

		iMaxScreenWidth = screen.getXResolution()
		iMaxScreenHeight = screen.getYResolution()

		self.m_iScreenHeight = 400
		self.m_iScreenWidth = 260*self.getNumPlayers()
		if (self.m_iScreenWidth > (iMaxScreenWidth-200)):
			self.m_iScreenWidth = iMaxScreenWidth-200

		self.refreshCtrl()
		self.m_bShown = True

	def killScreen(self):
		if (self.m_bShown):
			screen = CyGInterfaceScreen( "WorldBuilderDiplomacyScreen", CvScreenEnums.WORLDBUILDER_DIPLOMACY_SCREEN )
			screen.hideScreen()
			self.m_bShown = False
			saveDiplomacySettings()
			destroyWBDiplomacyCtrl()
		return

	def drawDiplomacyTab(self):

		tab = self.getTabControl()

		tab.attachVBox(self.szDiplomacyTab, "WBDiplomacyVBox")

		tab.attachScrollPanel("WBDiplomacyVBox", "WBDiplomacyPanel")
		tab.setLayoutFlag("WBDiplomacyPanel", "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag("WBDiplomacyPanel", "LAYOUT_SIZE_VEXPANDING")

		tab.attachHBox("WBDiplomacyPanel", "WBDiplomacyPanelHBox")
		tab.setLayoutFlag("WBDiplomacyPanelHBox", "LAYOUT_SIZE_HPREFERREDEXPANDING")
		tab.setLayoutFlag("WBDiplomacyPanelHBox", "LAYOUT_SIZE_VPREFERREDEXPANDING")


		####### Active Player

		tab.attachVBox("WBDiplomacyPanelHBox", "ActivePlayerVBox")
		tab.setLayoutFlag("ActivePlayerVBox", "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag("ActivePlayerVBox", "LAYOUT_SIZE_VEXPANDING")

		tab.attachPanel("ActivePlayerVBox", "ActivePlayerPanel")
		tab.setStyle("ActivePlayerPanel", "Panel_Tan15_Style")
		tab.setLayoutFlag("ActivePlayerPanel", "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag("ActivePlayerPanel", "LAYOUT_SIZE_VEXPANDING")

		hbox = "ActivePlayerPanelHBox"
		tab.attachHBox("ActivePlayerPanel", hbox)
		tab.setLayoutFlag(hbox, "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag(hbox, "LAYOUT_SIZE_VEXPANDING")


		vbox = "ActivePlayerPanelVBox"
		tab.attachVBox(hbox, vbox)
		tab.setLayoutFlag(vbox, "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag(vbox, "LAYOUT_SIZE_VEXPANDING")

		# Screen Image
		tab.attachPanel(vbox, "ActivePlayerScreenPanel")
		tab.setLayoutFlag("ActivePlayerScreenPanel", "LAYOUT_SIZE_HEXPANDING")
		tab.setStyle("ActivePlayerScreenPanel", "Panel_Black25_Style")

		iCount = -1
		for i in range( gc.getMAX_CIV_PLAYERS() ):
			if ( gc.getPlayer(i).isEverAlive() ):
				if (i != self.m_iActivePlayer):
					iCount = iCount + 1
					tab.attachVSeparator(hbox, "ActivePlayerSeparator")
					tab.setLayoutFlag("ActivePlayerSeparator", "LAYOUT_LEFT")
					#Other Player Panels
					self.addOtherPlayerPanel(iCount)
					#Other Players
					self.addOtherPlayerName(i, iCount)
					#Other At war?
					self.addOtherAtWar(i, iCount)
					#Other AI Attitude Weight
					self.addOtherAIAttitude(i, iCount)

		vbox1 = "ActivePlayerScreenPanel"

		# Active Player Dropdown
		szDropdownDesc = "ResolutionDropdownBox"
		aszDropdownElements = ()
		iInitialSelection = -1
		iPlayerCount = -1
		for i in range( gc.getMAX_CIV_PLAYERS() ):
			if ( gc.getPlayer(i).isEverAlive() ):
				iPlayerCount = iPlayerCount + 1
				aszDropdownElements = aszDropdownElements + (unicode(gc.getPlayer(i).getName()),)
				if (i == self.m_iActivePlayer):
					iInitialSelection = iPlayerCount
		szCallbackFunction = "handleWorldBuilderDiplomacyPlayerPullDownCB"
		szWidgetName = "WBDChangePlayerDropdownBox"
		tab.attachDropDown(vbox1, szWidgetName, szDropdownDesc, aszDropdownElements, self.callbackIFace, szCallbackFunction, szWidgetName, iInitialSelection)
		tab.setLayoutFlag(szWidgetName, "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag(szWidgetName, "LAYOUT_HCENTER")

		# At war?
		szLabel = localText.getText("TXT_KEY_TRADE_WAR_WITH", ())
		szLabel = szLabel + "?"
		tab.attachLabel(vbox1, "AtWar", szLabel)

		# AI Attitude Weight
		tab.attachLabel(vbox1, "AIAttitudeWeight", localText.getText("TXT_KEY_WB_AI_ATTITUDE", ()))

		########## EXIT

		tab.attachHSeparator("WBDiplomacyVBox", "GraphicsExitSeparator")

		tab.attachVBox("WBDiplomacyVBox", "LowerHBox")
		tab.setLayoutFlag("LowerHBox", "LAYOUT_HCENTER")

		szOptionDesc = localText.getText("TXT_KEY_OPTIONS_RESET", ())
		szCallbackFunction = "handleWorldBuilderDiplomacyAIWeightResetAllCB"
		szWidgetName = "WBDResetButton"
		tab.attachButton("LowerHBox", szWidgetName, szOptionDesc, self.callbackIFace, szCallbackFunction, szWidgetName)

		szOptionDesc = localText.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ())
		szCallbackFunction = "handleWorldBuilderDiplomacyExitCB"
		szWidgetName = "WBDExitButton"
		tab.attachButton("LowerHBox", szWidgetName, szOptionDesc, self.callbackIFace, szCallbackFunction, szWidgetName)

	def getSliderTextFront(self, iMin, iMax, iCurrent):
		szSliderText = str(iMin)
		return szSliderText

	def getSliderTextMiddle(self, iMin, iMax, iCurrent):
		szSliderText = str(iCurrent)
		return szSliderText

	def getSliderTextBack(self, iMin, iMax, iCurrent):
		szSliderText =  str(iMax)
		return szSliderText

	def getNumPlayers(self):
		iCount = 0
		for i in range( gc.getMAX_CIV_PLAYERS() ):
			if ( gc.getPlayer(i).isEverAlive() ):
				iCount = iCount + 1
		return iCount

	def getActualPlayer(self, iPlayer):
		iActualPlayer = -1
		iCount = -1
		iPanelCount = -1
		for i in range( gc.getMAX_CIV_PLAYERS() ):
			if ( gc.getPlayer(i).isEverAlive() ):
				iCount = iCount + 1
				if (iCount != self.m_iActivePlayer):
					iPanelCount = iPanelCount+1
					if (iPanelCount == iPlayer):
						iActualPlayer = i
		return iActualPlayer

	def addOtherPlayerPanel(self, iPanelNumber):
		tab = self.getTabControl()
		hbox = "ActivePlayerPanelHBox"
		vbox = "OtherPlayerPanelVBox" + str(iPanelNumber)
		tab.attachVBox(hbox, vbox)
		tab.setLayoutFlag(vbox, "LAYOUT_SIZE_HEXPANDING")

		# Screen Image
		szPanel = "OtherPlayerScreenPanel" + str(iPanelNumber)
		tab.attachPanel(vbox, szPanel)
		tab.setLayoutFlag(szPanel, "LAYOUT_SIZE_HEXPANDING")
		tab.setStyle(szPanel, "Panel_Black25_Style")
		return

	def addOtherPlayerName(self, iPlayer, iPanelNumber):
		tab = self.getTabControl()
		szWidget = "Player" + str(iPanelNumber)
		vbox = "OtherPlayerScreenPanel" + str(iPanelNumber)
		tab.attachLabel(vbox, szWidget, gc.getPlayer(iPlayer).getName())
		tab.setLayoutFlag(szWidget, "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag(szWidget, "LAYOUT_HCENTER")
		return

	def addOtherAtWar(self, iPlayer, iPanelNumber):
		tab = self.getTabControl()
		vbox = "OtherPlayerScreenPanel" + str(iPanelNumber)

		if (atWar(gc.getPlayer(self.m_iActivePlayer).getTeam(), gc.getPlayer(iPlayer).getTeam())):
			iInitialSelection = 0
		else:
			iInitialSelection = 1

		szWidgetName = "WBDAtWarDropdownBox_" + str(iPanelNumber)
		if (gc.getPlayer(iPlayer).getTeam() == gc.getPlayer(self.m_iActivePlayer).getTeam()):
			tab.attachLabel(vbox, szWidgetName, localText.getText("TXT_KEY_PITBOSS_TEAM", ()))
		else:
			szDropdownDesc = "AtWarDropdownBox" + str(iPanelNumber)
			aszDropdownElements = ()
			aszDropdownElements = aszDropdownElements + (unicode(localText.getText("TXT_KEY_TRADE_WAR_WITH", ())),)
			aszDropdownElements = aszDropdownElements + (unicode(localText.getText("TXT_KEY_TRADE_PEACE_WITH", ())),)
			szCallbackFunction = "handleWorldBuilderDiplomacyAtWarPullDownCB"
			tab.attachDropDown(vbox, szWidgetName, szDropdownDesc, aszDropdownElements, self.callbackIFace, szCallbackFunction, szWidgetName, iInitialSelection)

	def addOtherAIAttitude(self, iPlayer, iPanelNumber):
		tab = self.getTabControl()
		vbox = "OtherPlayerScreenPanel" + str(iPanelNumber)
		hbox = "OtherPlayerSliderHBox" + str(iPanelNumber)

		tab.attachHBox(vbox, hbox)
		tab.setLayoutFlag(hbox, "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag(hbox, "LAYOUT_SIZE_VEXPANDING")
		tab.setLayoutFlag(hbox, "LAYOUT_HCENTER")

		szSliderText = self.getSliderTextFront(-100, 100, gc.getPlayer(iPlayer).AI_getAttitudeExtra(self.m_iActivePlayer))
		szTextName = "WorldBuilderDiplomacySliderTextFront" + str(iPanelNumber)
		tab.attachLabel(hbox, szTextName, szSliderText)
		tab.setLayoutFlag(szTextName, "LAYOUT_LEFT")

		szWidgetName = "WBDAIAttitudeWeightSlider_" + str(iPanelNumber)
		iMin = -100
		iMax = 100
		iInitialVal = gc.getPlayer(iPlayer).AI_getAttitudeExtra(self.m_iActivePlayer)
		szCallbackFunction = "handleWorldBuilderDiplomacyAIWeightPullDownCB"
		tab.attachHSlider(hbox, szWidgetName, self.callbackIFace, szCallbackFunction, szWidgetName, iMin, iMax, iInitialVal)
		tab.setSliderWidth(szWidgetName, 100)

		szSliderText = self.getSliderTextBack(-100, 100, gc.getPlayer(iPlayer).AI_getAttitudeExtra(self.m_iActivePlayer))
		szTextName = "WorldBuilderDiplomacySliderTextBack" + str(iPanelNumber)
		tab.attachLabel(hbox, szTextName, szSliderText)
		tab.setLayoutFlag(szTextName, "LAYOUT_RIGHT")

		szSliderText = self.getSliderTextMiddle(-100, 100, gc.getPlayer(iPlayer).AI_getAttitudeExtra(self.m_iActivePlayer))
		szTextName = "WorldBuilderDiplomacySliderTextMiddle" + str(iPanelNumber)
		tab.attachLabel(vbox, szTextName, szSliderText)
		tab.setLayoutFlag(szTextName, "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag(szTextName, "LAYOUT_HCENTER")
		return

	##################################################################################################################################
	#######	Refresh Tab Ctrl
	##################################################################################################################################
	def refreshCtrl (self):
		if (self.m_bShown):
			saveDiplomacySettings()
			destroyWBDiplomacyCtrl()
			initWBDiplomacyCtrl(self.szTabControlName, True, False)
			setDiplomacySettings()
			self.pTabControl = getWBToolDiplomacyTabCtrl()
			szTab = self.getDiplomacyTabName()
			self.getTabControl().setSize(self.m_iScreenWidth,self.m_iScreenHeight)
		else:
			initWBDiplomacyCtrl(self.szTabControlName, True, False)
			self.pTabControl = getWBToolDiplomacyTabCtrl()
			self.getTabControl().setSize(self.m_iScreenWidth,self.m_iScreenHeight)

		self.getTabControl().setControlsExpanding(false)
		self.getTabControl().setColumnLength(self.iScreenColumnHeight)

		# Set Tabs
		self.getTabControl().attachTabItem(self.szDiplomacyTab, self.szDiplomacyTabName)

		self.drawDiplomacyTab()
		return

	##################################################################################################################################
	#######	Callbacks
	##################################################################################################################################

	def handlePlayerPullDownCB ( self, iIndex ) :
		iCount = -1
		for i in range( gc.getMAX_CIV_PLAYERS() ):
			if ( gc.getPlayer(i).isEverAlive() ):
				iCount = iCount + 1
				if (iCount == iIndex):
					self.m_iActivePlayer = i
					self.refreshCtrl()
					return
		return

	def handleAtWarPullDownCB ( self, argList ) :
		iSelection, szWidget = argList
		iPlayer = int(szWidget[szWidget.find("_")+1:])
		iActualPlayer = self.getActualPlayer(iPlayer)
		if (iSelection == 0):
			gc.getTeam(gc.getPlayer(self.m_iActivePlayer).getTeam()).declareWar(gc.getPlayer(iActualPlayer).getTeam(), False, WarPlanTypes.NO_WARPLAN)
		elif (iSelection == 1):
			gc.getTeam(gc.getPlayer(self.m_iActivePlayer).getTeam()).makePeace(gc.getPlayer(iActualPlayer).getTeam())

		#refresh to update teams
		self.refreshCtrl()

	def handleAIWeightResetAll(self):
		for i in range( gc.getMAX_CIV_PLAYERS() ):
			if ( gc.getPlayer(i).isEverAlive() ):
				if (i != self.m_iActivePlayer):
					gc.getPlayer(i).AI_setAttitudeExtra(self.m_iActivePlayer, 0)
					gc.getTeam(gc.getPlayer(self.m_iActivePlayer).getTeam()).makePeace(gc.getPlayer(i).getTeam())

		self.refreshCtrl()
		return

	def handleAIWeightPullDownCB(self, argList):
		iValue, szWidget = argList
		iPlayer = int(szWidget[szWidget.find("_")+1:])
		iActualPlayer = self.getActualPlayer(iPlayer)

		szTextName = "WorldBuilderDiplomacySliderTextMiddle" + str(iPlayer)

		gc.getPlayer(iActualPlayer).AI_setAttitudeExtra(self.m_iActivePlayer, iValue)

		tab = self.getTabControl()
		szSliderText = self.getSliderTextMiddle(-100, 100, gc.getPlayer(iActualPlayer).AI_getAttitudeExtra(self.m_iActivePlayer))
		tab.setText(szTextName, szSliderText)
		return

