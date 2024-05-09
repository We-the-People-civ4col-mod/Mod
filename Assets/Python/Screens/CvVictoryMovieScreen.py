## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()

class CvVictoryMovieScreen:

	def interfaceScreen (self, movieArtDef):

		self.X_SCREEN = 0
		self.Y_SCREEN = 0
		self.W_SCREEN = 1024
		self.H_SCREEN = 768
		self.Y_TITLE = 12
		self.BORDER_HEIGHT = 100

		self.X_EXIT = 410
		self.Y_EXIT = 326

		game = CyGame()
		if ( game.isNetworkMultiPlayer() or game.isPitbossHost()):
			return

		if (len(movieArtDef) == 0):
			return
		self.createMovieScreen(movieArtDef)

	def createMovieScreen(self, movieArtDef):
		# Create a new screen, called VictoryMovieScreen, using the file CvVictoryMovieScreen.py for input
		screen = CyGInterfaceScreen( "VictoryMovieScreen", CvScreenEnums.VICTORY_MOVIE_SCREEN )
		screen.setDimensions(screen.centerX(0), screen.centerY(0), -1, -1)
		screen.setRenderInterfaceOnly(True)
		screen.enableWorldSounds( false )
		screen.addDDSGFC("VictoryMovieScreenBackground", ArtFileMgr.getInterfaceArtInfo("SCREEN_BG_OPAQUE").getPath(), 0, 0, -1, -1, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addPanel( "VictoryMovieTopPanel", u"", u"", False, False, self.X_SCREEN, self.Y_SCREEN, self.W_SCREEN, self.BORDER_HEIGHT, PanelStyles.PANEL_STYLE_TOPBAR, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addPanel( "VictoryMovieBottomPanel", u"", u"", False, False, self.X_SCREEN, self.H_SCREEN-(self.BORDER_HEIGHT+3), self.W_SCREEN, self.BORDER_HEIGHT+3, PanelStyles.PANEL_STYLE_BOTTOMBAR, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.showWindowBackground( False )

		# Show the screen
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		screen.setButtonGFC("Exit", u"Your movies are not installed correctly.", "", #self.EXIT_TEXT,
			self.X_EXIT, self.Y_EXIT, 400, 100, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1, ButtonStyles.BUTTON_STYLE_STANDARD)

		# Play the movie
		movieFilePath = CyArtFileMgr().getMovieArtInfo(movieArtDef).getPath()
		screen.playMovie( movieFilePath, -1, -1, -1, -1, 0)

	def closeScreen(self):
		screen = CyGInterfaceScreen( "VictoryMovieScreen", CvScreenEnums.VICTORY_MOVIE_SCREEN )
		screen.hideScreen()

	def hideScreen(self):
		screen = CyGInterfaceScreen( "VictoryMovieScreen", CvScreenEnums.VICTORY_MOVIE_SCREEN )
		screen.hideScreen()

	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		screen = CyGInterfaceScreen( "VictoryMovieScreen", CvScreenEnums.VICTORY_MOVIE_SCREEN )
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_MOVIE_DONE or inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			return self.hideScreen()
		return 0

	def update(self, fDelta):
		return

