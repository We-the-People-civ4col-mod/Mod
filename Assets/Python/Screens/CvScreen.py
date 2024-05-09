# Sid Meier's Civilization 4
# Copyright Firaxis Games 2005
#
# CvScreen - Base class for all of the Screens

from CvPythonExtensions import *
import CvUtil

class CvScreen:
	"Base Screen"

	def __init__(self):
		return

	def getScreen(self):
		return -1

	def hideScreen(self):
		return

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


