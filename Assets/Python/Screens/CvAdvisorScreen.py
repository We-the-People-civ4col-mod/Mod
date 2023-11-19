## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
import CvScreenEnums
import CvGFCScreen
import CvUtil
from CvPythonExtensions import *

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()

class CvAdvisorScreen(CvGFCScreen.CvGFCScreen):
	bShowDebugText = False

	advisorImageAreas={
		'MainBackground' : ("Art/Interface/Screens/Advisor/TutorialAdvisorPopBG.dds", 1024,256),
		}
	advisorButtons={
		'Ok':("Continue","Continue","",350,160,120,30,WidgetTypes.WIDGET_GENERAL,-1,-1,ButtonStyles.BUTTON_STYLE_STANDARD),
		}

	# leaderhead position/size details
	bVisibleLeaderhead = False
	DEFAULT_LEADER = 0
	LEADER_WIDGET = "AdvisorHeadWidget"
	X_LEADERHEAD = 0
	Y_LEADERHEAD = 10
	W_LEADERHEAD = 192
	H_LEADERHEAD = 224
	LEADER_MOOD = AttitudeTypes.ATTITUDE_FRIENDLY

	advisorTextPanelWidget = "AdvisorScreenTextArea"
	advisorTextWidget = None
	advisorText = ""
	X_ADVISOR_TEXT = 258
	Y_ADVISOR_TEXT = 62
	W_ADVISOR_TEXT = 758
	H_ADVISOR_TEXT = 95

	last_AdvisorTextWidget = None

	debugTextName = None
	X_DEBUG_TEXT = 265
	Y_DEBUG_TEXT = 61
	W_DEBUG_TEXT = 150
	H_DEBUG_TEXT = 800

	bPassInput = False

	# position of screen
	X_SCREEN = 120
	Y_SCREEN = 48
	W_SCREEN = 1024
	H_SCREEN = 768

	def __init__(self):
		self.setScreenValues("AdvisorScreen", CvScreenEnums.ADVISOR_SCREEN)

	def interfaceScreen(self):
		screen = self.getScreen()

		xResolution = self.getScreen().getXResolution()
		yResolution = self.getScreen().getYResolution()

		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, self.bPassInput)
		screen.setAlwaysShown( True )
		screen.setDimensions(0, 0, xResolution, yResolution)
		screen.addPanel( "AdvisorScreenLeaderheadBack", u"", u"", False, False, self.X_SCREEN + self.X_LEADERHEAD - 15, self.Y_SCREEN + self.Y_LEADERHEAD - 15, self.W_LEADERHEAD + 30, self.H_LEADERHEAD + 30, PanelStyles.PANEL_STYLE_BLUELARGE, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		#screen.addPanel( "AdvisorScreenLeaderheadBack", u"", u"", False, False, self.X_SCREEN, self.Y_SCREEN, self.W_LEADERHEAD, self.H_LEADERHEAD, PanelStyles.PANEL_STYLE_EXTERNAL, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addPanel( self.advisorTextPanelWidget, u"", u"", False, False, self.X_SCREEN + self.W_LEADERHEAD, self.Y_SCREEN + 32, self.W_SCREEN - ( self.W_LEADERHEAD + 200 ), self.H_LEADERHEAD - 86, PanelStyles.PANEL_STYLE_BLUELARGE, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addPanel( "AdvisorButtonPanel", u"", u"", False, False, self.X_SCREEN + self.W_LEADERHEAD, self.Y_SCREEN + 152, self.W_SCREEN - ( self.W_LEADERHEAD + 200 ), 53, PanelStyles.PANEL_STYLE_BLUELARGE, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.showWindowBackground( False )

		#self.showImageArea('MainBackground')
		self.showAllAdvisorButtons()
		self.addLeaderhead(self.DEFAULT_LEADER, self.X_LEADERHEAD, self.Y_LEADERHEAD)

	def setBackgroundPosition(self, iX, iY):
		self.X_SCREEN = iX
		self.Y_SCREEN = iY
		self.clearScreen()
		self.interfaceScreen()
		self.setAdvisorText(self.advisorText)

	def hideMainBackground(self):
		#self.hideImageArea('MainBackground')
		self.clearAdvisorButtons()

	def setAdvisorText(self, message):
		screen = self.getScreen()
		self.advisorText = message
		TextName = self.getNextWidgetName()
		if ( self.advisorTextWidget ):
			screen.hide(self.advisorTextWidget)
		#screen.deleteWidget(self.advisorTextWidget)

		screen.addMultilineText( TextName, u"%s"%unicode(message), self.X_SCREEN + self.W_LEADERHEAD + 10, self.Y_SCREEN + 42, self.W_SCREEN - ( self.W_LEADERHEAD + 220 ), self.H_LEADERHEAD - 116, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
		#screen.attachMultilineText( self.advisorTextPanelWidget, TextName, u"%s"%unicode(message), WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
		self.advisorTextWidget=TextName
		return

	def setDebugText(self, message):
		return
		if self.bShowDebugText:
			self.debugTextName = self.getNextWidgetName()
			self.getScreen().setText(self.debugTextName,"Background", unicode(message),
						CvUtil.FONT_LEFT_JUSTIFY,
						self.X_SCREEN + self.X_DEBUG_TEXT, self.Y_SCREEN + self.Y_DEBUG_TEXT,-2.1,
						FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL,-1,-1)
#			self.addText(self.WIDGET_DEBUGTEXT, message,
#						self.X_SCREEN + self.X_DEBUG_TEXT, self.Y_SCREEN + self.Y_DEBUG_TEXT,
#						self.W_DEBUG_TEXT,self.W_DEBUG_TEXT)
		return

	def clearText(self):
		screen = self.getScreen()
		if self.debugTextName:
			screen.hide(self.debugTextName)
		if self.advisorTextWidget:
			screen.hide(self.advisorTextWidget)

	def showAllAdvisorButtons(self):
		for key in self.advisorButtons.keys():
			self.showAdvisorButtons(key)

	def showAdvisorButtons(self, advisorButtonName):
		if self.advisorButtons.has_key(advisorButtonName):
			screen = self.getScreen()
			button = self.advisorButtons[advisorButtonName]
			screen.setButtonGFC(button[0],button[1],button[2],
				self.X_SCREEN + button[3], self.Y_SCREEN + button[4],
				button[5],button[6],button[7],
				button[8],button[9],button[10])

	def showAllImageAreas(self):
		for key in self.advisorImageAreas.keys():
			self.showImageArea(key)

	def showImageArea(self, imageArea, filePath=None):
		if self.advisorImageAreas.has_key(imageArea):
			screen = self.getScreen()
			imageAreaInfo = self.advisorImageAreas[imageArea]
			imagePath = ""
			if filePath:
				imagePath = filePath
			else:
				imagePath = imageAreaInfo[0]
			screen.addDDSGFC( str(imageArea), imagePath,
				self.X_SCREEN, self.Y_SCREEN,
				imageAreaInfo[1], imageAreaInfo[2],
				WidgetTypes.WIDGET_GENERAL,-1,-1 )

	def hideImageArea(self, imageArea):
		screen = self.getScreen()
		if ( imageArea ):
			screen.hide(imageArea)

	def setHelpImage(self, filePath):
		self.showImageArea('HelpImage', filePath)

	def playInterfaceEffect(self):
		screen = self.getScreen()
		screen.playMovie("Art/Tutorial/Marca256-Y.bik", 0,100, 100,100, -2.5)

	def addLeaderhead(self, iLeader, iX, iY):
		screen = self.getScreen()
		self.X_LEADERHEAD = iX
		self.Y_LEADERHEAD = iY
		screen.addLeaderheadGFC(self.LEADER_WIDGET,
			iLeader, self.LEADER_MOOD,
			self.X_SCREEN + self.X_LEADERHEAD, self.Y_SCREEN + self.Y_LEADERHEAD,
			self.W_LEADERHEAD, self.H_LEADERHEAD,
			WidgetTypes.WIDGET_GENERAL,-1,-1)
	def doLeaderheadAction(self, leaderHeadAction):
		self.getScreen().performLeaderheadAction(self.LEADER_WIDGET, leaderHeadAction)
	def setLeaderheadMood(self, attitude):
		self.getScreen().setLeaderheadMood(self.LEADER_WIDGET, attitude)
	def doLeaderheadApproval(self):
		if int(self.LEADER_MOOD) < int(AttitudeTypes.ATTITUDE_FRIENDLY):
			self.LEADER_MOOD += 1
		self.doLeaderheadAction(LeaderheadAction.LEADERANIM_GREETING)
		self.setLeaderheadMood(self.LEADER_MOOD)
	def doLeaderheadNotListening(self):
		if int(self.LEADER_MOOD) > int(AttitudeTypes.ATTITUDE_FURIOUS):
			self.LEADER_MOOD -= 1
		self.doLeaderheadAction(LeaderheadAction.LEADERANIM_DISAGREE)
		self.setLeaderheadMood(self.LEADER_MOOD)

	def focusCamera(self, object, niPosition):
		CyCamera().SimpleLookAt(niPosition,object.getPoint())

	def playAdvisorSound(self, soundfile):
		if (soundfile != 'No Text'):
			CyInterface().stopAdvisorSound()
			CyInterface().playAdvisorSound(soundfile)
		return

	def clearAdvisorButtons(self):
		screen = self.getScreen()
		for key in self.advisorButtons.keys():
			button = self.advisorButtons[key]
			if ( button[0] ):
				screen.hide(button[0])
	def clearImageAreas(self):
		screen = self.getScreen()
		for imageArea in self.advisorImageAreas.keys():
			if ( imageArea ):
				screen.hide(imageArea)
	def clearLeaderhead(self):
		if self.bVisibleLeaderhead:
			self.clear(self.LEADER_WIDGET)
	def clearHelpImage(self):
		screen = self.getScreen()
		screen.hide('HelpImage')
	def clearScreen(self):
		self.clearLeaderhead()
		self.clearAdvisorButtons()
		self.clearImageAreas()
		self.clearText()
		self.hideScreen()

	def highlightArea( self, iX, iY, iWidth, iHeight ):
		screen = self.getScreen()
		screen.addDDSGFC( "TutorialHighlight", ArtFileMgr.getInterfaceArtInfo("INTERFACE_TUTORIAL_HIGHLIGHT_LARGE").getPath(), iX, iY, iWidth, iHeight, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.moveToFront( "TutorialHighlight" )
		screen.moveBackward( "TutorialHighlight" )
		screen.show( "TutorialHighlight" )

	def hideItem( self, szName ):
		screen = self.getScreen()
		if ( szName ):
			screen.hide( szName )

	def update(self, fDelta):
		screen = self.getScreen()
		screen.moveToFront( "Background" )

	def handleInput(self, inputClass):
		return

