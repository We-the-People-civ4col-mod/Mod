## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
import CvUtil
from CvPythonExtensions import *

# globals
gc = CyGlobalContext()

class CvGFCScreen:
	def __init__(self):
		self.screenName = "None"
		self.screenID = -1

	def setScreenValues(self, screenName, screenID):
		self.screenName = screenName
		self.screenID = screenID

	def getScreen(self):
		screen = CyGInterfaceScreen(self.screenName, self.screenID)
		return screen

	def setCloseButton(widgetname, text, x, y):
		#self.getScreen().setButtonGFC(widgetname, text, "", x, y, 50,50, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1, ButtonStyles.BUTTON_STYLE_STANDARD)
		return

	def addText(self, widgetName,message, iX,iY, iWidth, iHeight):
		screen = self.getScreen()
		screen.addMultilineText( widgetName, unicode(message),
								iX, iY, iWidth, iHeight,
								WidgetTypes.WIDGET_GENERAL, -1,-1, CvUtil.FONT_LEFT_JUSTIFY );

	def hideScreen(self):
		self.getScreen().hideScreen()

	# Screen construction function
	def showScreen(self, iCategory):
		return

	# returns a unique ID for a widget in this screen
	def getNextWidgetName(self):
		return

	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		return 0

	def update(self, fDelta):
		return
