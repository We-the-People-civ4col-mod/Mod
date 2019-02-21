## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

MOVIE_SCREEN_WONDER = 0

class CvWonderMovieScreen:
	"Wonder Movie Screen"

	def __init__(self):
		self.fDelay = -1.0
		self.fTime = 0.0
		self.bDone = false

	def interfaceScreen (self, iMovieItem, iCityId):

		if CyUserProfile().getGraphicOption(GraphicOptionTypes.GRAPHICOPTION_NO_MOVIES):
			return

		self.Z_CONTROLS = -2.2

		self.X_SCREEN = 0
		self.Y_SCREEN = 0
		self.W_SCREEN = 1024
		self.H_SCREEN = 768

		self.X_WINDOW = 250
		self.Y_WINDOW = 40
		self.W_WINDOW = 760
		self.H_WINDOW = 590
		self.Y_TITLE = self.Y_WINDOW + 20
		self.iWonderId = iMovieItem

		self.X_EXIT = self.X_WINDOW + self.W_WINDOW/2 - 50
		self.Y_EXIT = self.Y_WINDOW + self.H_WINDOW - 50
		self.W_EXIT = 120
		self.H_EXIT = 30

		self.X_MOVIE = 20
		self.Y_MOVIE = 50
		self.W_MOVIE = 720
		self.H_MOVIE = 480

		self.fTime = 0.0
		self.fDelay = 1.5
		self.bDone = false

		self.szMovieFile = gc.getBuildingInfo(iMovieItem).getMovie()
		if (self.szMovieFile == None or len(self.szMovieFile) == 0):
			return

		# move the camera and mark the interface camera as dirty so that it gets reset - JW
		CyInterface().lookAtCityBuilding(iCityId, iMovieItem)
		CyInterface().setDirty(InterfaceDirtyBits.SelectionCamera_DIRTY_BIT, True)

		screen = CyGInterfaceScreen( "WonderMovieScreen" + str(iMovieItem), CvScreenEnums.WONDER_MOVIE_SCREEN )
		screen.addPanel("WonderMoviePanel", "", "", true, true,
			self.X_WINDOW, self.Y_WINDOW, self.W_WINDOW, self.H_WINDOW, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1)

		screen.showWindowBackground( True )
		screen.setDimensions(screen.centerX(self.X_SCREEN), screen.centerY(self.Y_SCREEN), self.W_SCREEN, self.H_SCREEN)
		screen.setRenderInterfaceOnly(False)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)
		screen.enableWorldSounds( false )

		# Header...
		szHeaderId = "WonderTitleHeader" + str(iMovieItem)
		szHeader = gc.getBuildingInfo(iMovieItem).getDescription()

		screen.setLabel(szHeaderId, "Background", u"<font=4b>" + szHeader + "</font>", CvUtil.FONT_CENTER_JUSTIFY,
				self.X_WINDOW + self.W_WINDOW / 2, self.Y_TITLE, self.Z_CONTROLS, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		screen.hide("Background")

		screen.playMovie("", 0, 0, 0, 0, 0) # dummy call to hide screen if no movies are supposed to be shown

	def playMovie(self):

		screen = CyGInterfaceScreen( "WonderMovieScreen" + str(self.iWonderId), CvScreenEnums.WONDER_MOVIE_SCREEN )
		screen.setRenderInterfaceOnly(True)
		screen.show("Background")

		# Play the movie
		screen.playMovie(self.szMovieFile, self.X_WINDOW + self.X_MOVIE, self.Y_WINDOW + self.Y_MOVIE, self.W_MOVIE, self.H_MOVIE, -2.3 )

		screen.setButtonGFC("WonderExit" + str(self.iWonderId), localText.getText("TXT_KEY_MAIN_MENU_OK", ()), "", self.X_EXIT, self.Y_EXIT, self.W_EXIT, self.H_EXIT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1, ButtonStyles.BUTTON_STYLE_STANDARD )

	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_MOVIE_DONE):
			if (not self.bDone):
				screen = CyGInterfaceScreen( "WonderMovieScreen" + str(self.iWonderId), CvScreenEnums.WONDER_MOVIE_SCREEN )
				szHelp = CyGameTextMgr().getBuildingHelp(self.iWonderId, False, False, None)

				if len(szHelp) > 0:
					screen.addPanel("MonkeyPanel", "", "", true, true, self.X_WINDOW + self.X_MOVIE + self.W_MOVIE / 8 - 10, self.Y_WINDOW + self.Y_MOVIE + 90, 3 * self.W_MOVIE / 4 + 20, self.H_MOVIE - 180, PanelStyles.PANEL_STYLE_MAIN_BLACK50, WidgetTypes.WIDGET_GENERAL, -1, -1)
					screen.addMultilineText("MonkeyText", szHelp, self.X_WINDOW + self.X_MOVIE + self.W_MOVIE / 8, self.Y_WINDOW + self.Y_MOVIE + 100, 3 * self.W_MOVIE / 4, self.H_MOVIE - 200, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
				self.bDone = true

		return 0

	def update(self, fDelta):

		if self.fDelay > 0:
			self.fTime += fDelta
			if self.fTime > self.fDelay:
				self.fDelay = -1
				self.playMovie()
		return
