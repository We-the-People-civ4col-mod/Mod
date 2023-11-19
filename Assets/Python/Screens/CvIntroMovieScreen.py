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

class CvIntroMovieScreen:
	"Intro Movie Screen"
	bMovieState = 0
	def interfaceScreen (self):

		self.currentMovie = "ART_DEF_MOVIE_2K_INTRO"
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

		self.createMovieScreen(self.currentMovie)

	def createMovieScreen(self, movieArtDef):
		#return
		if CyUserProfile().getGraphicOption(GraphicOptionTypes.GRAPHICOPTION_NO_MOVIES):
			return

		# Create a new screen, called IntroMovieScreen, using the file CvIntroMovieScreen.py for input
		screen = CyGInterfaceScreen( "IntroMovieScreen", CvScreenEnums.INTRO_MOVIE_SCREEN )
		screen.setDimensions(screen.centerX(0), screen.centerY(0), -1, -1)
		screen.setRenderInterfaceOnly(True)
		screen.showWindowBackground( False )
		screen.setShowFor( 0 )

		# Show the screen
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		screen.setButtonGFC("Exit", u"Your movies are not installed correctly.", "", #self.EXIT_TEXT,
			self.X_EXIT, self.Y_EXIT, 400, 100, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1, ButtonStyles.BUTTON_STYLE_STANDARD)

		# Play the movie
		movieFilePath = CyArtFileMgr().getMovieArtInfo(movieArtDef).getPath()
		screen.playMovie( movieFilePath, -1, -1, -1, -1, 0)

	def createLogoScreen(self):
		screen = CyGInterfaceScreen( "IntroMovieScreen", CvScreenEnums.INTRO_MOVIE_SCREEN )
		screen.setRenderInterfaceOnly(True)
		screen.showWindowBackground( False )

		# Show the screen
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		# Background First
		screen.setImageButton( "LogoBackground", ArtFileMgr.getInterfaceArtInfo("MAINMENU_LOAD").getPath(), screen.centerX(0)-2, screen.centerY(0)-2, 1028, 776, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		# Logos
		screen.setImageButton( "Logos", ArtFileMgr.getInterfaceArtInfo("INTRO_LOGOS").getPath(), screen.centerX(0)+233, screen.centerY(0)+331, 549, 294, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		screen.addMultilineText("LegalText1", "<font=1>" + localText.getText("TXT_LEGAL_LINE_1", ()) + "</font>", screen.centerX(0)+110, screen.centerY(0)+590, 804, 200, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setLabel( "LegalText2", "Background", "<font=2>" + localText.getText("TXT_LEGAL_LINE_2", ()) + "</font>", CvUtil.FONT_CENTER_JUSTIFY, screen.centerX(0)+512, screen.centerY(0)+740, -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setShowFor( 4000 )

	def closeScreen(self):
		screen = CyGInterfaceScreen( "IntroMovieScreen", CvScreenEnums.INTRO_MOVIE_SCREEN )
		screen.hideScreen()

	def hideScreen(self):
		screen = CyGInterfaceScreen( "IntroMovieScreen", CvScreenEnums.INTRO_MOVIE_SCREEN )
		screen.hideScreen()

	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		screen = CyGInterfaceScreen( "IntroMovieScreen", CvScreenEnums.INTRO_MOVIE_SCREEN )
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_MOVIE_DONE or inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED or inputClass.getNotifyCode() == NotifyCode.NOTIFY_CHARACTER):
			if (inputClass.getNotifyCode() != NotifyCode.NOTIFY_MOVIE_DONE):
				setNoIntroMovie(true)
			if self.bMovieState == 3:
				return self.hideScreen()
			elif self.bMovieState == 2:
				self.bMovieState = 3
				self.currentMovie = "ART_DEF_MOVIE_INTRO"
				self.closeScreen()
				self.createMovieScreen( "ART_DEF_MOVIE_INTRO" )
			elif self.bMovieState == 1:
				self.bMovieState = 2
				self.closeScreen()
				self.createLogoScreen()
			else:
				self.bMovieState = 1
				self.currentMovie = "ART_DEF_MOVIE_NVIDIA_INTRO"
				self.closeScreen()
				self.createMovieScreen( "ART_DEF_MOVIE_NVIDIA_INTRO" )
		return 0

	def update(self, fDelta):
		return

