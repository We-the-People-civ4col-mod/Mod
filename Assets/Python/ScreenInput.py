## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
import CvUtil


# Class to decipher and make screen input easy to read...
class ScreenInput:

	# Init call...
	def __init__ (self, argsList):
		self.eNotifyCode = argsList[0]
		self.iData = argsList[1]
		self.uiFlags = argsList[2]
		self.iItemID = argsList[3]
		self.ePythonFileEnum = argsList[4]
		self.szFunctionName = argsList[5]
		self.bShift = argsList[6]
		self.bCtrl = argsList[7]
		self.bAlt = argsList[8]
		self.iMouseX = argsList[9]
		self.iMouseY = argsList[10]
		self.iButtonType = argsList[11]
		self.iData1 = argsList[12]
		self.iData2 = argsList[13]
		self.bOption = argsList[14]

	# NotifyCode
	def getNotifyCode (self):
		return self.eNotifyCode

	# Data
	def getData (self):
		return self.iData

	# Flags
	def getFlags (self):
		return self.uiFlags

	# Item ID
	def getID (self):
		return self.iItemID

	# Python File
	def getPythonFile (self):
		return self.ePythonFileEnum

	# Function Name...
	def getFunctionName (self):
		return self.szFunctionName

	# Shift Key Down
	def isShiftKeyDown (self):
		return self.bShift

	# Ctrl Key Down
	def isCtrlKeyDown (self):
		return self.bCtrl

	# Alt Key Down
	def isAltKeyDown (self):
		return self.bAlt

	# X location of the mouse cursor
	def getMouseX (self):
		return self.iMouseX

	# Y location of the mouse cursor
	def getMouseY (self):
		return self.iMouseY

	# WidgetType
	def getButtonType (self):
		return self.iButtonType

	# Widget Data 1
	def getData1 (self):
		return self.iData1

	# Widget Data 2
	def getData2 (self):
		return self.iData2

	# Widget Option
	def getOption (self):
		return self.bOption

