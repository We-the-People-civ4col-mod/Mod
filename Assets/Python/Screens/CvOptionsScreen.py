## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005

# For Input see CvOptionsScreenCallbackInterface in Python\EntryPoints\

import CvUtil
from CvPythonExtensions import *

# globals
gc = CyGlobalContext()
UserProfile = CyUserProfile()
localText = CyTranslator()

class CvOptionsScreen:
	"Options Screen"

	def __init__(self):

		self.iScreenHeight = 50

		self.iGameOptionsTabID		= 0
		self.iGraphicOptionsTabID	= 1
		self.iAudioOptionsTabID		= 2
		self.iOtherOptionsTabID	        = 3

		self.callbackIFace = "CvOptionsScreenCallbackInterface"

	def getTabControl(self):
		return self.pTabControl

	def getGameOptionsTabName(self):
		return self.szGameOptionsTabName
	def getGraphicOptionsTabName(self):
		return self.szGraphicsOptionsTabName
	def getAudioOptionsTabName(self):
		return self.szAudioOptionsTabName
	def getOtherOptionsTabName(self):
		return self.szOtherOptionsTabName

	# Used by Callback Interface to set path via checkbox
	def getMusicPath(self):
		return self.getTabControl().getText("CustomMusicEditBox")
	def getCustomMusicCheckboxName(self):
		return self.szCustomMusicCheckboxName

	# Used by Callback Interface to set Alarm time via checkbox
	def getAlarmHour(self):
		return self.getTabControl().getText("AlarmHourEditBox")
	def getAlarmMin(self):
		return self.getTabControl().getText("AlarmMinEditBox")

	# Used by Callback Interface to get user defined profile names from editbox
	def setProfileEditCtrlText(self, szProfileName):
		szWideProfName = CvUtil.convertToUnicode(szProfileName)
		self.getTabControl().setText("ProfileNameEditBox", szWideProfName)
	def getProfileEditCtrlText(self):
		return self.getTabControl().getText("ProfileNameEditBox")

	# Called from C++ after a custom music path is selected via FileDialogBox
	def updateMusicPath (self, szMusicPath):

		self.getTabControl().setText("CustomMusicEditBox", szMusicPath)
		self.getTabControl().setValue(self.getCustomMusicCheckboxName(), true)

#########################################################################################
################################## SCREEN CONSTRUCTION ##################################
#########################################################################################

	def initText(self):

		self.szTabControlName = localText.getText("TXT_KEY_OPTIONS_TITLE", ())

		self.szGameOptionsTabName = localText.getText("TXT_KEY_OPTIONS_GAME", ())
		self.szGraphicsOptionsTabName = localText.getText("TXT_KEY_OPTIONS_GRAPHICS", ())
		self.szAudioOptionsTabName = localText.getText("TXT_KEY_OPTIONS_AUDIO", ())
		self.szOtherOptionsTabName = localText.getText("TXT_KEY_OPTIONS_SCREEN_OTHER", ())

	def refreshScreen (self):

		#################### Game Options ####################

		szTab = self.getGameOptionsTabName()
		for iOptionLoop in range(PlayerOptionTypes.NUM_PLAYEROPTION_TYPES):
			szWidgetName = "GameOptionCheckBox_" + str(iOptionLoop)
			bOptionOn = UserProfile.getPlayerOption(iOptionLoop)
			self.pTabControl.setValue(szWidgetName, bOptionOn)

		# Languages Dropdown
		self.getTabControl().setValue("LanguagesDropdownBox", CyGame().getCurrentLanguage())

		#################### GRAPHICS ####################

		szTab = self.getGraphicOptionsTabName()

		# Graphics Dropdowns

		self.getTabControl().setValue(self.szResolutionComboBoxName, UserProfile.getResolution() )
		self.getTabControl().setValue("AntiAliasingDropdownBox", UserProfile.getAntiAliasing() )
		self.getTabControl().setValue("GraphicsLevelDropdownBox", UserProfile.getGraphicsLevel() )
		self.getTabControl().setValue("MainMenuDropdownBox", UserProfile.getMainMenu() )

		# Graphic Option Checkboxes
		for iOptionLoop in range(GraphicOptionTypes.NUM_GRAPHICOPTION_TYPES):
			szWidgetName = "GraphicOptionCheckbox_" + str(iOptionLoop)
			bOptionOn = UserProfile.getGraphicOption(iOptionLoop)
			self.pTabControl.setValue(szWidgetName, bOptionOn)

		#################### AUDIO ####################

		szTab = self.getAudioOptionsTabName()

		iMax = UserProfile.getVolumeStops()

		# Volume Sliders and No Sound Checkboxes
		for iWidgetNum in range(6):
			if (iWidgetNum == 0):		# Master Volume
				iInitialVal = iMax-UserProfile.getMasterVolume()-1
				bNoSoundTrue = UserProfile.isMasterNoSound()
			elif (iWidgetNum == 1):		# Music Volume
				iInitialVal = iMax-UserProfile.getMusicVolume()-1
				bNoSoundTrue = UserProfile.isMusicNoSound()
			elif (iWidgetNum == 2):		# Sound Effects Volume
				iInitialVal = iMax-UserProfile.getSoundEffectsVolume()-1
				bNoSoundTrue = UserProfile.isSoundEffectsNoSound()
			elif (iWidgetNum == 3):		# Speech Volume
				iInitialVal = iMax-UserProfile.getSpeechVolume()-1
				bNoSoundTrue = UserProfile.isSpeechNoSound()
			elif (iWidgetNum == 4):		# Ambience Volume
				iInitialVal = iMax-UserProfile.getAmbienceVolume()-1
				bNoSoundTrue = UserProfile.isAmbienceNoSound()
			elif (iWidgetNum == 5):		# Interface Volume
				iInitialVal = iMax-UserProfile.getInterfaceVolume()-1
				bNoSoundTrue = UserProfile.isInterfaceNoSound()

			# Volume Slider
			szWidgetName = "VolumeSlider_" + str(iWidgetNum)
   			self.getTabControl().setValue(szWidgetName, iInitialVal)

			# Volume Checkbox
			szWidgetName = "VolumeNoSoundCheckbox_" + str(iWidgetNum)
			self.pTabControl.setValue(szWidgetName, bNoSoundTrue)

		# Voice Capture Dropdown
		self.getTabControl().setValue("CaptureDeviceDropdownBox", UserProfile.getCaptureDeviceIndex() )
		# Voice Capture Slider
#   		self.getTabControl().setValue("CaptureVolumeSlider", UserProfile.getMaxCaptureVolume() - UserProfile.getCaptureVolume())
   		self.getTabControl().setValue("CaptureVolumeSlider", UserProfile.getCaptureVolume())

		# Voice Playback Dropdown
		self.getTabControl().setValue("PlaybackDeviceDropdownBox", UserProfile.getPlaybackDeviceIndex() )
		# Voice Playback Slider
#   		self.getTabControl().setValue("PlaybackVolumeSlider", UserProfile.getMaxPlaybackVolume() - UserProfile.getPlaybackVolume())
   		self.getTabControl().setValue("PlaybackVolumeSlider", UserProfile.getPlaybackVolume())

		# Voice Chatting Checkbox
		self.getTabControl().setValue("VoiceChatCheckbox", UserProfile.useVoice())

		# Speaker config
		iInitialSelection = 0
		for iSpeakerConfigLoop in range(16):
			szActiveConfig = UserProfile.getSpeakerConfigFromList(iSpeakerConfigLoop)
			if (UserProfile.getSpeakerConfig() == szActiveConfig):
				iInitialSelection = iSpeakerConfigLoop

		# Speaker Config Dropdown
		self.getTabControl().setValue("SpeakerConfigDropdownBox", iInitialSelection )

		# Custom Music Path Checkbox
		bUseCustomMusicPath = false
		if (UserProfile.getMusicPath() != ""):
			bUseCustomMusicPath = true
		self.getTabControl().setValue(self.getCustomMusicCheckboxName(), bUseCustomMusicPath)

		# Custom Music Path Editbox
		szEditBoxDesc = ""
		if (UserProfile.getMusicPath() != ""):
			szEditBoxDesc = CvUtil.convertToUnicode(UserProfile.getMusicPath())
		self.getTabControl().setText("CustomMusicEditBox", szEditBoxDesc)

		#################### CLOCK ####################

		szTab = self.getOtherOptionsTabName()

		# Clock On Checkbox
		self.getTabControl().setValue("ClockOnCheckbox", UserProfile.isClockOn())

		# 24 Hour Clock Checkbox
		self.getTabControl().setValue("24HourClockCheckbox", UserProfile.is24Hours())

		# Alarm On Checkbox
		self.getTabControl().setValue("AlarmOnCheckbox", isAlarmOn())

		# Alarm Hours
		self.getTabControl().setText("AlarmHourEditBox", str(getAlarmHourLeft()))
		self.getTabControl().setText("AlarmMinEditBox", str(getAlarmMinLeft()))

		#################### PROFILE ####################

		# Profile Name Editbox
		self.getTabControl().setText("ProfileNameEditBox", CvUtil.convertToUnicode(UserProfile.getProfileName()))

		aszDropdownElements = ()
		for iProfileLoop in range(UserProfile.getNumProfileFiles()):
			szProfileFileName = UserProfile.getProfileFileName(iProfileLoop)
			# Cut off file path and extension
			szProfile = szProfileFileName[szProfileFileName.find("PROFILES\\")+9:-4]
			aszDropdownElements = aszDropdownElements + (szProfile,)
			if (UserProfile.getProfileName() == szProfile):
				iInitialSelection = iProfileLoop

		self.getTabControl().changeDropdownContents("ProfilesDropdownBox", aszDropdownElements)

		# Profile List Dropdown
		self.getTabControl().setValue("ProfilesDropdownBox", iInitialSelection)

		#################### PROFILE ####################

		# Broadband Radio Button
   		self.getTabControl().setValue("BroadbandSelection", not gc.getGame().isModem())

		# Modem Checkbox
   		self.getTabControl().setValue("ModemSelection", gc.getGame().isModem())

	def interfaceScreen (self):
		"Initial creation of the screen"
		self.initText()

		self.pTabControl = CyGTabCtrl(self.szTabControlName, false, false)
		self.pTabControl.setModal(1)
		self.pTabControl.setSize(800,600)
		self.pTabControl.setControlsExpanding(false)
		self.pTabControl.setColumnLength(self.iScreenHeight)

		# Set Tabs
		self.pTabControl.attachTabItem("GameForm", self.szGameOptionsTabName)
		self.pTabControl.attachTabItem("GraphicsForm", self.szGraphicsOptionsTabName)
		self.pTabControl.attachTabItem("AudioForm", self.szAudioOptionsTabName)
		self.pTabControl.attachTabItem("OtherForm", self.szOtherOptionsTabName)

		self.drawGameOptionsTab()
		self.drawGraphicOptionsTab()
		self.drawAudioOptionsTab()
		self.drawOtherTab()


	def drawGameOptionsTab(self):

		tab = self.pTabControl

		tab.attachVBox("GameForm", "GameVBox")

		# Add Game Options

		tab.attachPanel("GameVBox", "GamePanelCenter")
		tab.setStyle("GamePanelCenter", "Panel_Tan15_Style")
		tab.setLayoutFlag("GamePanelCenter", "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag("GamePanelCenter", "LAYOUT_SIZE_VEXPANDING")

		tab.attachScrollPanel("GamePanelCenter", "GamePanel")
		tab.setLayoutFlag("GamePanel", "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag("GamePanel", "LAYOUT_SIZE_VEXPANDING")

		tab.attachHBox("GamePanel", "GameHBox")
		tab.setLayoutFlag("GameHBox", "LAYOUT_SIZE_HEXPANDING")

		tab.attachVBox("GameHBox", "GameVBox1")
		tab.setLayoutFlag("GameVBox1", "LAYOUT_SIZE_HEXPANDING")
		#tab.attachVSeparator("GameHBox", "GameHBoxSeparator")
		tab.attachVBox("GameHBox", "GameVBox2")
		tab.setLayoutFlag("GameVBox2", "LAYOUT_SIZE_HEXPANDING")

		i = 0
		for iOptionLoop in range(PlayerOptionTypes.NUM_PLAYEROPTION_TYPES):

			bContinue = true

			if iOptionLoop == PlayerOptionTypes.PLAYEROPTION_MODDER_1:
				if GlobalDefines.USE_MODDERS_PLAYEROPTION_1 == 0:
					bContinue = false
			elif iOptionLoop == PlayerOptionTypes.PLAYEROPTION_MODDER_2:
				if GlobalDefines.USE_MODDERS_PLAYEROPTION_2 == 0:
					bContinue = false
			elif iOptionLoop == PlayerOptionTypes.PLAYEROPTION_MODDER_3:
				if GlobalDefines.USE_MODDERS_PLAYEROPTION_3 == 0:
					bContinue = false

			if (bContinue):

				szOptionDesc = gc.getPlayerOptionsInfoByIndex(iOptionLoop).getDescription()
				szHelp = gc.getPlayerOptionsInfoByIndex(iOptionLoop).getHelp()
				szCallbackFunction = "handleGameOptionsClicked"
				szWidgetName = "GameOptionCheckBox_" + str(iOptionLoop)
				bOptionOn = UserProfile.getPlayerOption(iOptionLoop)#gc.getPlayer(gc.getGame().getActivePlayer()).isOption(iOptionLoop)
				if ((i+1) <= (PlayerOptionTypes.NUM_PLAYEROPTION_TYPES + 1)/2):
					vbox = "GameVBox1"
				else:
					vbox = "GameVBox2"
				tab.attachCheckBox(vbox, szWidgetName, szOptionDesc, self.callbackIFace, szCallbackFunction, szWidgetName, bOptionOn)
				tab.setToolTip(szWidgetName, szHelp)
				i += 1


		tab.attachSpacer("GamePanelCenter")

		tab.attachHBox("GamePanelCenter", "LangHBox")

		# Languages Dropdown
		tab.attachLabel("LangHBox", "LangLabel", localText.getText("TXT_KEY_OPTIONS_SCREEN_LANGUAGE", ()))	# Label
		szDropdownDesc = "LanguagesDropdownBox"

		tab.attachSpacer("LangHBox")

		# "TXT_KEY_LANGUAGE_0"
		# "TXT_KEY_LANGUAGE_1"
		# "TXT_KEY_LANGUAGE_2"
		# "TXT_KEY_LANGUAGE_3"
		# "TXT_KEY_LANGUAGE_4"

		aszDropdownElements = ()
		for i in range(CvGameText().getNumLanguages()):
			szKey = "TXT_KEY_LANGUAGE_%d" % i
			aszDropdownElements = aszDropdownElements + (localText.getText(szKey, ()),)

		szCallbackFunction = "handleLanguagesDropdownBoxInput"
		szWidgetName = "LanguagesDropdownBox"
		iInitialSelection = CyGame().getCurrentLanguage()
		tab.attachDropDown("LangHBox", szWidgetName, szDropdownDesc, aszDropdownElements, self.callbackIFace, szCallbackFunction, szWidgetName, iInitialSelection)
		tab.setLayoutFlag(szWidgetName, "LAYOUT_LEFT")

		########## Lower Panel

		tab.attachHSeparator("GameVBox", "GameExitSeparator")

		tab.attachHBox("GameVBox", "LowerHBox")
		tab.setLayoutFlag("LowerHBox", "LAYOUT_HCENTER")

		szOptionDesc = localText.getText("TXT_KEY_OPTIONS_RESET", ())
		szCallbackFunction = "handleGameReset"
		szWidgetName = "GameOptionsResetButton"
		tab.attachButton("LowerHBox", szWidgetName, szOptionDesc, self.callbackIFace, szCallbackFunction, szWidgetName)

		szOptionDesc = localText.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ())
		szCallbackFunction = "handleExitButtonInput"
		szWidgetName = "GameOptionsExitButton"
		tab.attachButton("LowerHBox", szWidgetName, szOptionDesc, self.callbackIFace, szCallbackFunction, szWidgetName)

	def drawGraphicOptionsTab(self):

		tab = self.pTabControl

		tab.attachVBox("GraphicsForm", "GraphicsVBox")

		tab.attachScrollPanel("GraphicsVBox", "GraphicsPanel")
		tab.setLayoutFlag("GraphicsPanel", "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag("GraphicsPanel", "LAYOUT_SIZE_VEXPANDING")

		tab.attachHBox("GraphicsPanel", "GraphicsPanelHBox")
		tab.setLayoutFlag("GraphicsPanelHBox", "LAYOUT_SIZE_HPREFERREDEXPANDING")
		tab.setLayoutFlag("GraphicsPanelHBox", "LAYOUT_SIZE_VPREFERREDEXPANDING")


		####### RESOLUTION

		tab.attachVBox("GraphicsPanelHBox", "ResVBox")
		tab.setLayoutFlag("ResVBox", "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag("ResVBox", "LAYOUT_SIZE_VEXPANDING")

		tab.attachPanel("ResVBox", "ResPanel")
		tab.setStyle("ResPanel", "Panel_Tan15_Style")
		tab.setLayoutFlag("ResPanel", "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag("ResPanel", "LAYOUT_SIZE_VEXPANDING")

		hbox = "ResPanelHBox"
		tab.attachHBox("ResPanel", hbox)
		tab.setLayoutFlag(hbox, "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag(hbox, "LAYOUT_SIZE_VEXPANDING")


		vbox = "ResPanelVBox"
		tab.attachVBox(hbox, vbox)
		tab.setLayoutFlag(vbox, "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag(vbox, "LAYOUT_SIZE_VEXPANDING")

		# Screen Image
		tab.attachPanel(vbox, "ResScreenPanel")
		tab.setLayoutFlag("ResScreenPanel", "LAYOUT_SIZE_HEXPANDING")
		tab.setStyle("ResScreenPanel", "Panel_Black25_Style")

		tab.attachHBox(vbox, "ResHBox")

		vbox1 = "ResVBox1"
		vbox2 = "ResVBox2"
		tab.attachVBox("ResHBox", vbox1)
		tab.attachVBox("ResHBox", vbox2)

		# Screen Resolution Dropdown
		tab.attachLabel(vbox1, "ResLabel", localText.getText("TXT_KEY_OPTIONS_SCREEN_RES", ()))	# Label
		tab.setControlFlag("ResLabel", "CF_LABEL_DEFAULTSIZE")
		szDropdownDesc = "ResolutionDropdownBox"
		aszDropdownElements = ()
		for iResLoop in range(UserProfile.getResolutionMaxModes()):
			aszDropdownElements = aszDropdownElements + (UserProfile.getResolutionString(iResLoop),)
		szCallbackFunction = "handleResolutionDropdownInput"
		szWidgetName = self.szResolutionComboBoxName = "ResolutionDropdownBox"
		iInitialSelection = UserProfile.getResolution()
		tab.attachDropDown(vbox2, szWidgetName, szDropdownDesc, aszDropdownElements, self.callbackIFace, szCallbackFunction, szWidgetName, iInitialSelection)

		# Anti-Aliasing Dropdown
		tab.attachLabel(vbox1, "AALabel", localText.getText("TXT_KEY_OPTIONS_ANTIALIAS", ()))
		tab.setControlFlag("AALabel", "CF_LABEL_DEFAULTSIZE")
		szDropdownDesc = "AntiAliasingDropdownBox"
		aszDropdownElements = ()
		for iAALoop in range(UserProfile.getAntiAliasingMaxMultiSamples()+1):
			if (iAALoop == 0):
				aszDropdownElements = aszDropdownElements + (u"0",)
			elif (iAALoop == 1):
				aszDropdownElements = aszDropdownElements + (u"2",)
			elif (iAALoop == 2):
				aszDropdownElements = aszDropdownElements + (u"4",)
			elif (iAALoop == 3):
				aszDropdownElements = aszDropdownElements + (u"8",)
			elif (iAALoop == 4):
				aszDropdownElements = aszDropdownElements + (u"16",)

		szCallbackFunction = "handleAntiAliasingDropdownInput"
		szWidgetName = "AntiAliasingDropdownBox"
		iInitialSelection = UserProfile.getAntiAliasing()
		tab.attachDropDown(vbox2, szWidgetName, szDropdownDesc, aszDropdownElements, self.callbackIFace, szCallbackFunction, szWidgetName, iInitialSelection)
		tab.setLayoutFlag(szWidgetName, "LAYOUT_LEFT")

		# Graphics Level
		tab.attachLabel(vbox1, "GraphicsLevelLabel", localText.getText("TXT_KEY_OPTIONS_SCREEN_GRAPHICS_LEVEL", ()))	# Label
		tab.setControlFlag("GraphicsLevelLabel", "CF_LABEL_DEFAULTSIZE")
		szDropdownDesc = "GraphicsLevelDropdownBox"
		aszDropdownElements = (localText.getText("TXT_KEY_SEALEVEL_HIGH", ()), localText.getText("TXT_KEY_SEALEVEL_MEDIUM", ()), localText.getText("TXT_KEY_SEALEVEL_LOW", ()))
		szCallbackFunction = "handleGraphicsLevelDropdownBoxInput"
		szWidgetName = szDropdownDesc
		iInitialSelection = UserProfile.getGraphicsLevel()
		tab.attachDropDown(vbox2, szWidgetName, szDropdownDesc, aszDropdownElements, self.callbackIFace, szCallbackFunction, szWidgetName, iInitialSelection)

		# Main menu
		tab.attachLabel(vbox1, "MainMenuLabel", localText.getText("TXT_KEY_OPENING_MENU", ()))	# Label
		tab.setControlFlag("MainMenuLabel", "CF_LABEL_DEFAULTSIZE")

		szDropdownDesc = "MainMenuDropdownBox"
		aszDropdownElements = ()
		for iMainMenuLoop in range(gc.getNumMainMenus()):
			aszDropdownElements = aszDropdownElements + (gc.getMainMenus(iMainMenuLoop).getDescription(),)
		szCallbackFunction = "handleMainMenuDropdownBoxInput"
		szWidgetName = self.szMainMenuDropdownBoxName = "DropdownBox"
		iInitialSelection = UserProfile.getMainMenu()
		tab.attachDropDown(vbox2, szWidgetName, szDropdownDesc, aszDropdownElements, self.callbackIFace, szCallbackFunction, szWidgetName, iInitialSelection)

		####### GAME GRAPHICS

		tab.attachVSeparator(hbox, "GfxSeparator")
		tab.setLayoutFlag("GfxSeparator", "LAYOUT_LEFT")

		vbox = "GfxPanelVBox"
		tab.attachVBox(hbox, vbox)
		tab.setLayoutFlag(vbox, "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag(vbox, "LAYOUT_SIZE_VEXPANDING")
		tab.setLayoutFlag(vbox, "LAYOUT_SPACING_NONE")

		# Checkboxes
		for iOptionLoop in range(GraphicOptionTypes.NUM_GRAPHICOPTION_TYPES):
			szOptionDesc = gc.getGraphicOptionsInfoByIndex(iOptionLoop).getDescription()
			szHelp = gc.getGraphicOptionsInfoByIndex(iOptionLoop).getHelp()
			szCallbackFunction = "handleGraphicOptionsClicked"
			szWidgetName = "GraphicOptionCheckbox_" + str(iOptionLoop)
			bOptionOn = UserProfile.getGraphicOption(iOptionLoop)
			tab.attachCheckBox(vbox, szWidgetName, szOptionDesc, self.callbackIFace, szCallbackFunction, szWidgetName, bOptionOn)
			tab.setToolTip(szWidgetName, szHelp)

		########## EXIT

		tab.attachHSeparator("GraphicsVBox", "GraphicsExitSeparator")

		tab.attachHBox("GraphicsVBox", "LowerHBox")
		tab.setLayoutFlag("LowerHBox", "LAYOUT_HCENTER")

		szOptionDesc = localText.getText("TXT_KEY_OPTIONS_RESET", ())
		szCallbackFunction = "handleGraphicsReset"
		szWidgetName = "GraphicOptionsResetButton"
		tab.attachButton("LowerHBox", szWidgetName, szOptionDesc, self.callbackIFace, szCallbackFunction, szWidgetName)

		szOptionDesc = localText.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ())
		szCallbackFunction = "handleExitButtonInput"
		szWidgetName = "GraphicOptionsExitButton"
		tab.attachButton("LowerHBox", szWidgetName, szOptionDesc, self.callbackIFace, szCallbackFunction, szWidgetName)

	def drawAudioOptionsTab(self):

		tab = self.pTabControl

		tab.attachVBox("AudioForm", "AudioVBox")

		tab.attachScrollPanel("AudioVBox", "AudioPanel")
		tab.setLayoutFlag("AudioPanel", "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag("AudioPanel", "LAYOUT_SIZE_VEXPANDING")

		tab.attachVBox("AudioPanel", "AudioPanelVBox")
		tab.setLayoutFlag("AudioPanelHBox", "LAYOUT_SPACING_FORM")
		tab.setLayoutFlag("AudioPanelHBox", "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag("AudioPanelHBox", "LAYOUT_SIZE_VEXPANDING")


		######################### Create the 6 volume slider/checkboxes #########################

		tab.attachVBox("AudioPanelVBox", "VolumeVBox")
		tab.setLayoutFlag("VolumeVBox", "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag("VolumeVBox", "LAYOUT_SIZE_VEXPANDING")

		#tab.attachLabel("VolumeVBox", "VolumeLabel", "VOLUME")

		tab.attachPanel("VolumeVBox", "VolumePanel")
		tab.setStyle("VolumePanel", "Panel_Tan15_Style")
		tab.setLayoutFlag("VolumePanel", "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag("VolumePanel", "LAYOUT_SIZE_VEXPANDING")

		tab.attachVBox("VolumePanel", "VolumePanelVBox")
		tab.setLayoutFlag("VolumePanelVBox", "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag("VolumePanelVBox", "LAYOUT_SIZE_VEXPANDING")

		tab.attachScrollPanel("VolumePanelVBox", "VolumeScrollPanel")
		tab.setLayoutFlag("VolumeScrollPanel", "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag("VolumeScrollPanel", "LAYOUT_SIZE_VEXPANDING")

		tab.attachHBox("VolumeScrollPanel", "VolumePanelHBox")
		tab.setLayoutFlag("VolumePanelHBox", "LAYOUT_HEVENSTRETCH")
		tab.setLayoutFlag("VolumePanelHBox", "LAYOUT_SIZE_VEXPANDING")

		for iWidgetNum in range(6):

			# SLIDER

			if (iWidgetNum == 0):		# Master Volume
				szWidgetDesc = localText.getText("TXT_KEY_OPTIONS_MASTERVOLUME", ())
				iInitialVal = 20-UserProfile.getMasterVolume()-1
				bNoSoundTrue = UserProfile.isMasterNoSound()
			elif (iWidgetNum == 1):		# Music Volume
				szWidgetDesc = localText.getText("TXT_KEY_OPTIONS_MUSICVOLUME", ())
				iInitialVal = 20-UserProfile.getMusicVolume()-1
				bNoSoundTrue = UserProfile.isMusicNoSound()
			elif (iWidgetNum == 2):		# Sound Effects Volume
				szWidgetDesc = localText.getText("TXT_KEY_OPTIONS_EFFECTSVOLUME", ())
				iInitialVal = 20-UserProfile.getSoundEffectsVolume()-1
				bNoSoundTrue = UserProfile.isSoundEffectsNoSound()
			elif (iWidgetNum == 3):		# Speech Volume
				szWidgetDesc = localText.getText("TXT_KEY_OPTIONS_SPEECHVOLUME", ())
				iInitialVal = 20-UserProfile.getSpeechVolume()-1
				bNoSoundTrue = UserProfile.isSpeechNoSound()
			elif (iWidgetNum == 4):		# Ambience Volume
				szWidgetDesc = localText.getText("TXT_KEY_OPTIONS_AMBIENCEVOLUME", ())
				iInitialVal = 20-UserProfile.getAmbienceVolume()-1
				bNoSoundTrue = UserProfile.isAmbienceNoSound()
			elif (iWidgetNum == 5):		# Interface Volume
				szWidgetDesc = localText.getText("TXT_KEY_OPTIONS_INTERFACEVOLUME", ())
				iInitialVal = 20-UserProfile.getInterfaceVolume()-1
				bNoSoundTrue = UserProfile.isInterfaceNoSound()

			islider = str(iWidgetNum)

			vbox = "VolumeSliderVBox"+islider
			tab.attachVBox("VolumePanelHBox", vbox)

			# Volume Slider
			szSliderDesc = szWidgetDesc
			szWidgetName = "VolumeSliderLabel"+islider
			tab.attachLabel(vbox, szWidgetName, szSliderDesc)
			tab.setLayoutFlag(szWidgetName, "LAYOUT_HCENTER")

			szCallbackFunction = "handleVolumeSlidersInput"
			szWidgetName = "VolumeSlider_" + str(iWidgetNum)
			iMin = 0
			iMax = UserProfile.getVolumeStops()
			# iInitialVal set above
			tab.attachVSlider(vbox, szWidgetName, self.callbackIFace, szCallbackFunction, szWidgetName, iMin, iMax, iInitialVal)
			tab.setLayoutFlag(szWidgetName, "LAYOUT_SIZE_VEXPANDING")
			tab.setControlFlag(szWidgetName, "CF_SLIDER_FILL_DOWN")

			# CHECKBOX

			szOptionDesc = localText.getText("TXT_KEY_OPTIONS_NO_SOUND", ())
			szCallbackFunction = "handleVolumeCheckboxesInput"
			szWidgetName = "VolumeNoSoundCheckbox_" + str(iWidgetNum)
			# bNoSoundTrue set above
			tab.attachCheckBox(vbox, szWidgetName, szOptionDesc, self.callbackIFace, szCallbackFunction, szWidgetName, bNoSoundTrue)
			tab.setLayoutFlag(szWidgetName, "LAYOUT_HCENTER")


		tab.attachHSeparator("VolumePanelVBox", "SoundSeparator")

		tab.attachHBox("VolumePanelVBox", "SoundPanelHBox")
		tab.setLayoutFlag("SoundPanelHBox", "LAYOUT_SIZE_HPREFERREDEXPANDING")
		tab.setLayoutFlag("SoundPanelHBox", "LAYOUT_SIZE_VPREFERRED")

		######################### Voice Config Section #########################

		tab.attachVBox("SoundPanelHBox", "VoiceVBox")

		# Checkbox
		szOptionDesc = localText.getText("TXT_KEY_OPTIONS_VOICE", ())
		szCallbackFunction = "handleVoiceCheckboxInput"
		self.szVoiceCheckboxName = "VoiceCheckbox"
		szWidgetName = "VoiceChatCheckbox"
		bUseVoice = UserProfile.useVoice()
		tab.attachCheckBox("VoiceVBox", szWidgetName, szOptionDesc, self.callbackIFace, szCallbackFunction, szWidgetName, bUseVoice)

		# Capture Device Dropdown
		tab.attachLabel("VoiceVBox", "VoiceCaptureLabel", localText.getText("TXT_KEY_OPTIONS_CAPTURE_DEVICE", ()))
		szDropdownDesc = "CaptureDeviceDropdownBox"
		aszDropdownElements = ()
		for iCaptureDevice in range(UserProfile.getNumCaptureDevices()):
			aszDropdownElements = aszDropdownElements + (UserProfile.getCaptureDeviceDesc(iCaptureDevice),)
		szCallbackFunction = "handleCaptureDeviceDropdownInput"
		szWidgetName = "CaptureDeviceDropdownBox"
		iInitialSelection = UserProfile.getCaptureDeviceIndex()
		tab.attachDropDown("VoiceVBox", szWidgetName, szDropdownDesc, aszDropdownElements, self.callbackIFace, szCallbackFunction, szWidgetName, iInitialSelection)

		# Capture Volume Slider
		szSliderDesc = localText.getText("TXT_KEY_OPTIONS_CAPTUREVOLUME", ())
		szCallbackFunction = "handleCaptureVolumeSliderInput"
		szWidgetName = "CaptureVolumeSlider"
		iMin = 0
		iMax = UserProfile.getMaxCaptureVolume()
#		iInitialVal = iMax - UserProfile.getCaptureVolume()
		iInitialVal = UserProfile.getCaptureVolume()
		tab.attachHSlider("VoiceVBox", szWidgetName, self.callbackIFace, szCallbackFunction, szWidgetName, iMin, iMax, iInitialVal)
		tab.setControlFlag(szWidgetName, "CF_SLIDER_FILL_UP")

		# Playback Device Dropdown
		tab.attachLabel("VoiceVBox", "VoicePlaybackLabel", localText.getText("TXT_KEY_OPTIONS_PLAYBACK_DEVICE", ()))	# Label
		szDropdownDesc = "PlaybackDeviceDropdownBox"
		aszDropdownElements = ()
		for iPlaybackDevice in range(UserProfile.getNumPlaybackDevices()):
			aszDropdownElements = aszDropdownElements + (UserProfile.getPlaybackDeviceDesc(iPlaybackDevice),)
		szCallbackFunction = "handlePlaybackDeviceDropdownInput"
		szWidgetName = "PlaybackDeviceDropdownBox"
		iInitialSelection = UserProfile.getPlaybackDeviceIndex()
		tab.attachDropDown("VoiceVBox", szWidgetName, szDropdownDesc, aszDropdownElements, self.callbackIFace, szCallbackFunction, szWidgetName, iInitialSelection)

		# Playback Volume Slider
		szSliderDesc = localText.getText("TXT_KEY_OPTIONS_PLAYBACKVOLUME", ())
		szCallbackFunction = "handlePlaybackVolumeSliderInput"
		szWidgetName = "PlaybackVolumeSlider"
		iMin = 0
		iMax = UserProfile.getMaxPlaybackVolume()
#		iInitialVal = iMax - UserProfile.getPlaybackVolume()
		iInitialVal = UserProfile.getPlaybackVolume()
		tab.attachHSlider("VoiceVBox", szWidgetName, self.callbackIFace, szCallbackFunction, szWidgetName, iMin, iMax, iInitialVal)
		tab.setControlFlag(szWidgetName, "CF_SLIDER_FILL_UP")

		######################### Speaker Config Dropdown #########################

		tab.attachVSeparator("SoundPanelHBox", "SoundVSeparator")

		tab.attachVBox("SoundPanelHBox", "SoundConfigVBox")

		tab.attachImage("SoundConfigVBox", "SoundBlasterLogo", CyArtFileMgr().getMiscArtInfo("SOUND_BLASTER_LOGO").getPath())

		tab.attachLabel("SoundConfigVBox", "SpeakerConfigLabel", localText.getText("TXT_KEY_OPTIONS_SPEAKERS", ()))	# Label
		szDropdownDesc = "SpeakerConfigDropdownBox"
		aszDropdownElements = ()
		iInitialSelection = 0
		for iSpeakerConfigLoop in range(15):
			szActiveConfigKey = UserProfile.getSpeakerConfigFromList(iSpeakerConfigLoop)
			szActiveConfig = localText.getText(szActiveConfigKey, ())
			aszDropdownElements = aszDropdownElements + (szActiveConfig,)
			if (UserProfile.getSpeakerConfig() == szActiveConfigKey):
				iInitialSelection = iSpeakerConfigLoop

		szCallbackFunction = "handleSpeakerConfigDropdownInput"
		szWidgetName = "SpeakerConfigDropdownBox"
		# iInitialSelection set above
		tab.attachDropDown("SoundConfigVBox", szWidgetName, szDropdownDesc, aszDropdownElements, self.callbackIFace, szCallbackFunction, szWidgetName, iInitialSelection)
		tab.setLayoutFlag(szWidgetName, "LAYOUT_SIZE_HFIXEDEXPANDING")
		tab.setLayoutFlag(szWidgetName, "LAYOUT_LEFT")

		######################### Custom Audio Path #########################

		tab.attachHSeparator("SoundConfigVBox", "SoundSeparator")

		tab.attachHBox("SoundConfigVBox", "CustomPanelHBox")
		tab.setLayoutFlag("CustomPanelHBox", "LAYOUT_SIZE_HPREFERREDEXPANDING")
		tab.setLayoutFlag("CustomPanelHBox", "LAYOUT_SIZE_VPREFERRED")

		# Checkbox
		szOptionDesc = localText.getText("TXT_KEY_OPTIONS_CUSTOM_MUSIC", ())
		szCallbackFunction = "handleCustomMusicPathCheckboxInput"
		self.szCustomMusicCheckboxName = "CustomMusicPathCheckbox"
		szWidgetName = CvUtil.convertToStr(self.szCustomMusicCheckboxName)
		bUseCustomMusicPath = false
		if (UserProfile.getMusicPath() != ""):
			bUseCustomMusicPath = true
		tab.attachCheckBox("CustomPanelHBox", szWidgetName, szOptionDesc, self.callbackIFace, szCallbackFunction, szWidgetName, bUseCustomMusicPath)

		tab.attachHBox("CustomPanelHBox", "AudioPathHBox")
		tab.setLayoutFlag("AudioPathHBox", "LAYOUT_SIZE_HFIXEDEXPANDING")

		# Browse Button
		szOptionDesc = localText.getText("TXT_KEY_OPTIONS_BROWSE", ())
		szCallbackFunction = "handleCustomMusicPathButtonInput"
		szWidgetName = "CustomMusicPathButton"
		tab.attachButton("AudioPathHBox", szWidgetName, szOptionDesc, self.callbackIFace, szCallbackFunction, szWidgetName)

		# Edit Box
		szEditBoxDesc = u""
		if (UserProfile.getMusicPath() != ""):
			szEditBoxDesc = CvUtil.convertToUnicode(UserProfile.getMusicPath())
		szWidgetName = "CustomMusicEditBox"
		szCallbackFunction = "DummyCallback"

		tab.attachEdit("AudioPathHBox", szWidgetName, szEditBoxDesc, self.callbackIFace, szCallbackFunction, szWidgetName, -1)

		########## EXIT

		tab.attachHSeparator("AudioVBox", "AudioExitSeparator")

		tab.attachHBox("AudioVBox", "LowerHBox")
		tab.setLayoutFlag("LowerHBox", "LAYOUT_HCENTER")

		szOptionDesc = localText.getText("TXT_KEY_OPTIONS_RESET", ())
		szCallbackFunction = "handleAudioReset"
		szWidgetName = "AudioOptionsResetButton"
		tab.attachButton("LowerHBox", szWidgetName, szOptionDesc, self.callbackIFace, szCallbackFunction, szWidgetName)

		szOptionDesc = localText.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ())
		szCallbackFunction = "handleExitButtonInput"
		szWidgetName = "AudioOptionsExitButton"
		tab.attachButton("LowerHBox", szWidgetName, szOptionDesc, self.callbackIFace, szCallbackFunction, szWidgetName)
		tab.setLayoutFlag(szWidgetName, "LAYOUT_HCENTER")


	def drawOtherTab(self):

		tab = self.pTabControl

		tab.attachVBox("OtherForm", "OtherVBox")

		tab.attachScrollPanel("OtherVBox", "OtherPanel")
		tab.setLayoutFlag("OtherPanel", "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag("OtherPanel", "LAYOUT_SIZE_VEXPANDING")

		tab.attachHBox("OtherPanel", "OtherPanelHBox")
		tab.setLayoutFlag("OtherPanelHBox", "LAYOUT_SPACING_INNERFORM")
		tab.setLayoutFlag("OtherPanelHBox", "LAYOUT_SIZE_HEXPANDING")


		########### CLOCK

		tab.attachVBox("OtherPanelHBox", "ClockVBox")
		tab.setLayoutFlag("ClockVBox", "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag("ClockVBox", "LAYOUT_SIZE_VEXPANDING")

		tab.attachLabel("ClockVBox", "ClockLabel", localText.getText("TXT_KEY_OPTIONS_CLOCK", ()).upper() )

		tab.attachPanel("ClockVBox", "ClockPanel")
		tab.setStyle("ClockPanel", "Panel_Tan15_Style")
		tab.setLayoutFlag("ClockPanel", "LAYOUT_SIZE_HPREFERREDEXPANDING")
		tab.setLayoutFlag("ClockPanel", "LAYOUT_SIZE_VPREFERREDEXPANDING")

		tab.attachVBox("ClockPanel", "ClockPanelVBox")
		tab.setLayoutFlag("ClockPanelVBox", "LAYOUT_SIZE_HPREFERREDEXPANDING")
		tab.setLayoutFlag("ClockPanelVBox", "LAYOUT_SIZE_VPREFERREDEXPANDING")

		# Clock On Checkbox
		szOptionDesc = localText.getText("TXT_KEY_OPTIONS_CLOCK_ON", ())
		szCallbackFunction = "handleClockOnCheckboxInput"
		szWidgetName = "ClockOnCheckbox"
		bClockOn = UserProfile.isClockOn()
		tab.attachCheckBox("ClockPanelVBox", szWidgetName, szOptionDesc, self.callbackIFace, szCallbackFunction, szWidgetName, bClockOn)

		# 24 Hour Clock Checkbox
		szOptionDesc = localText.getText("TXT_KEY_OPTIONS_24CLOCK", ())
		szCallbackFunction = "handle24HourClockCheckboxInput"
		szWidgetName = "24HourClockCheckbox"
		b24HourClock = UserProfile.is24Hours()
		tab.attachCheckBox("ClockPanelVBox", szWidgetName, szOptionDesc, self.callbackIFace, szCallbackFunction, szWidgetName, b24HourClock)

		# Edit Box Hours
		tab.attachLabel("ClockPanelVBox", "HoursLabel", localText.getText("TXT_KEY_OPTIONS_HOURS", ()))	# Label
		szEditBoxDesc = str(getAlarmHourLeft())
		szCallbackFunction = "DummyCallback"
		szWidgetName = "AlarmHourEditBox"
		tab.attachEdit("ClockPanelVBox", szWidgetName, szEditBoxDesc, self.callbackIFace, szCallbackFunction, szWidgetName, 2)

		# Edit Box Mins
		tab.attachLabel("ClockPanelVBox", "MinsLabel", localText.getText("TXT_KEY_OPTIONS_MINS", ()))	# Label
		szEditBoxDesc = str(getAlarmMinLeft())
		szCallbackFunction = "DummyCallback"
		szWidgetName = "AlarmMinEditBox"
		tab.attachEdit("ClockPanelVBox", szWidgetName, szEditBoxDesc, self.callbackIFace, szCallbackFunction, szWidgetName, 2)

		# Alarm On Checkbox
		szOptionDesc = localText.getText("TXT_KEY_OPTIONS_ALARMON", ())
		szCallbackFunction = "handleAlarmOnCheckboxInput"
		szWidgetName = "AlarmOnCheckbox"
		bAlarmOn = isAlarmOn()
		tab.attachCheckBox("ClockPanelVBox", szWidgetName, szOptionDesc, self.callbackIFace, szCallbackFunction, szWidgetName, bAlarmOn)


		########### PROFILE

		UserProfile.loadProfileFileNames()

		tab.attachVBox("OtherPanelHBox", "ProfileVBox")
		tab.setLayoutFlag("ProfileVBox", "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag("ProfileVBox", "LAYOUT_SIZE_VEXPANDING")

		tab.attachLabel("ProfileVBox", "ProfileLabel", localText.getText("TXT_KEY_OPTIONS_SCREEN_PROFILES", ()).upper() )

		tab.attachPanel("ProfileVBox", "ProfilePanel")
		tab.setStyle("ProfilePanel", "Panel_Tan15_Style")
		tab.setLayoutFlag("ProfilePanel", "LAYOUT_SIZE_HPREFERREDEXPANDING")
		tab.setLayoutFlag("ProfilePanel", "LAYOUT_SIZE_VPREFERREDEXPANDING")

		tab.attachVBox("ProfilePanel", "ProfilePanelVBox")
		tab.setLayoutFlag("ProfilePanelVBox", "LAYOUT_SIZE_HPREFERREDEXPANDING")
		tab.setLayoutFlag("ProfilePanelVBox", "LAYOUT_SIZE_VPREFERREDEXPANDING")


		# Profiles Dropdown

		tab.attachLabel("ProfilePanelVBox", "ProfileComboLabel", localText.getText("TXT_KEY_OPTIONS_SCREEN_PROFILES", ()))

		szDropdownDesc = "ProfilesDropdownBox"
		aszDropdownElements = ()
		iInitialSelection = 0
		for iProfileLoop in range(UserProfile.getNumProfileFiles()):
			szProfileFileName = UserProfile.getProfileFileName(iProfileLoop)

			# Cut off file path and extension
			szProfile = szProfileFileName[szProfileFileName.find("PROFILES\\")+9:-4]

			aszDropdownElements = aszDropdownElements + (szProfile,)

			if (UserProfile.getProfileName() == szProfile):
				iInitialSelection = iProfileLoop

		szCallbackFunction = "handleProfilesDropdownInput"
		szWidgetName = "ProfilesDropdownBox"
		# iInitialSelection set above
		tab.attachDropDown("ProfilePanelVBox",szWidgetName,szDropdownDesc, aszDropdownElements, self.callbackIFace, szCallbackFunction, szWidgetName, iInitialSelection)

		# Edit Box ProfileName
		tab.attachLabel("ProfilePanelVBox","ProfilesName",localText.getText("TXT_KEY_OPTIONS_SCREEN_PROFILE_NAME", ()))	# Label


		#szCallbackIFace = ""
		szEditBoxDesc = UserProfile.getProfileName()
		szCallbackFunction = "DummyCallback"
		szWidgetName = "ProfileNameEditBox"
		szWideEditBoxDesc = CvUtil.convertToUnicode(szEditBoxDesc)
		tab.attachEdit("ProfilePanelVBox", szWidgetName, szWideEditBoxDesc, self.callbackIFace, szCallbackFunction, szWidgetName, 24)

		# New Profile Button
		szOptionDesc = localText.getText("TXT_KEY_OPTIONS_NEW_PROFILE", ())
		szCallbackFunction = "handleNewProfileButtonInput"
		szWidgetName = "NewProfileButton"
		tab.attachButton("ProfilePanelVBox", szWidgetName, szOptionDesc, self.callbackIFace, szCallbackFunction, szWidgetName)

		# Delete Profile Button
		szOptionDesc = localText.getText("TXT_KEY_OPTIONS_DELETE_PROFILE", ())
		szCallbackFunction = "handleDeleteProfileButtonInput"
		szWidgetName = "DeleteProfileButton"
		tab.attachButton("ProfilePanelVBox", szWidgetName, szOptionDesc, self.callbackIFace, szCallbackFunction, szWidgetName)


		########## NETWORKING

		tab.attachVBox("OtherPanelHBox", "NetVBox")
		tab.setLayoutFlag("NetVBox", "LAYOUT_SIZE_HEXPANDING")
		tab.setLayoutFlag("NetVBox", "LAYOUT_SIZE_VEXPANDING")

		tab.attachLabel("NetVBox", "NetLabel", localText.getText("TXT_KEY_OPTIONS_NETWORK", ()).upper() )

		tab.attachPanel("NetVBox", "NetPanel")
		tab.setStyle("NetPanel", "Panel_Tan15_Style")
		tab.setLayoutFlag("NetPanel", "LAYOUT_SIZE_HPREFERREDEXPANDING")
		tab.setLayoutFlag("NetPanel", "LAYOUT_SIZE_VPREFERREDEXPANDING")

		tab.attachVBox("NetPanel", "NetPanelVBox")
		tab.setLayoutFlag("NetPanelVBox", "LAYOUT_SIZE_HPREFERREDEXPANDING")
		tab.setLayoutFlag("NetPanelVBox", "LAYOUT_SIZE_VPREFERREDEXPANDING")

		# Radio Buttons
		tab.attachLabel("NetPanelVBox", "NetBandwidthLabel", localText.getText("TXT_KEY_OPTIONS_BANDWIDTH_DESC", ()) )

		bIsModem = gc.getGame().isModem()
		szCallbackFunction = "handleBroadbandSelected"
		szWidgetName = "BroadbandSelection"
		szWidgetLbl = localText.getText("TXT_KEY_OPTIONS_BROADBAND_LBL", ())
		tab.attachRadioButton("NetPanelVBox", szWidgetName, szWidgetLbl, self.callbackIFace, szCallbackFunction, str(szWidgetName), (not bIsModem))

		szCallbackFunction = "handleModemSelected"
		szWidgetName = "ModemSelection"
		szWidgetLbl = localText.getText("TXT_KEY_OPTIONS_MODEM_LBL", ())
		tab.attachRadioButton("NetPanelVBox", szWidgetName, szWidgetLbl, self.callbackIFace, szCallbackFunction, str(szWidgetName), bIsModem)


		########## EXIT

		tab.attachHSeparator("OtherVBox", "OtherExitSeparator")

		tab.attachHBox("OtherVBox", "LowerHBox")
		tab.setLayoutFlag("LowerHBox", "LAYOUT_HCENTER")

		szOptionDesc = localText.getText("TXT_KEY_OPTIONS_RESET", ())
		szCallbackFunction = "handleOtherReset"
		szWidgetName = "OtherOptionsResetButton"
		tab.attachButton("LowerHBox", szWidgetName, szOptionDesc, self.callbackIFace, szCallbackFunction, szWidgetName)

		szOptionDesc = localText.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ())
		szCallbackFunction = "handleExitButtonInput"
		szWidgetName = "OtherOptionsExitButton"
		tab.attachButton("LowerHBox", szWidgetName, szOptionDesc, self.callbackIFace, szCallbackFunction, szWidgetName)
		tab.setLayoutFlag(szWidgetName, "LAYOUT_HCENTER")

