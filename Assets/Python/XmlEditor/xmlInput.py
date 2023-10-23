from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import CvEventManager

def getKeyToken(keyID):
	keys = []
	keys.append(["KB_NONE", False])
	keys.append(["KB_ESCAPE", False])
	keys.append(["KB_0", True])
	keys.append(["KB_1", True])
	keys.append(["KB_2", True])
	keys.append(["KB_3", True])
	keys.append(["KB_4", True])
	keys.append(["KB_5", True])
	keys.append(["KB_6", True])
	keys.append(["KB_7", True])
	keys.append(["KB_8", True])
	keys.append(["KB_9", True])
	keys.append(["KB_MINUS", True])
	keys.append(["KB_A", True])
	keys.append(["KB_B", True])
	keys.append(["KB_C", True])
	keys.append(["KB_D", True])
	keys.append(["KB_E", True])
	keys.append(["KB_F", True])
	keys.append(["KB_G", True])
	keys.append(["KB_H", True])
	keys.append(["KB_I", True])
	keys.append(["KB_J", True])
	keys.append(["KB_K", True])
	keys.append(["KB_L", True])
	keys.append(["KB_M", True])
	keys.append(["KB_N", True])
	keys.append(["KB_O", True])
	keys.append(["KB_P", True])
	keys.append(["KB_Q", True])
	keys.append(["KB_R", True])
	keys.append(["KB_S", True])
	keys.append(["KB_T", True])
	keys.append(["KB_U", True])
	keys.append(["KB_V", True])
	keys.append(["KB_W", True])
	keys.append(["KB_X", True])
	keys.append(["KB_Y", True])
	keys.append(["KB_Z", True])
	keys.append(["KB_EQUALS", True])
	keys.append(["KB_BACKSPACE", True])
	keys.append(["KB_TAB", True])
	keys.append(["KB_LBRACKET", True])
	keys.append(["KB_RBRACKET", True])
	keys.append(["KB_RETURN", False])
	keys.append(["KB_LCONTROL", False])
	keys.append(["KB_SEMICOLON", True])
	keys.append(["KB_APOSTROPHE", True])
	keys.append(["KB_GRAVE", True])
	keys.append(["KB_LSHIFT", False])
	keys.append(["KB_BACKSLASH", True])
	keys.append(["KB_COMMA", True])
	keys.append(["KB_PERIOD", True])
	keys.append(["KB_SLASH", True])
	keys.append(["KB_RSHIFT", False])
	keys.append(["KB_NUMPADSTAR", True])
	keys.append(["KB_LALT", False])
	keys.append(["KB_SPACE", True])
	keys.append(["KB_CAPSLOCK", False])
	keys.append(["KB_F1", True])
	keys.append(["KB_F2", True])
	keys.append(["KB_F3", True])
	keys.append(["KB_F4", True])
	keys.append(["KB_F5", True])
	keys.append(["KB_F6", True])
	keys.append(["KB_F7", True])
	keys.append(["KB_F8", True])
	keys.append(["KB_F9", True])
	keys.append(["KB_F10", True])
	keys.append(["KB_F11", True])
	keys.append(["KB_F12", True])
	keys.append(["KB_NUMLOCK", False])
	keys.append(["KB_SCROLL", False])
	keys.append(["KB_NUMPAD0", True])
	keys.append(["KB_NUMPAD1", True])
	keys.append(["KB_NUMPAD2", True])
	keys.append(["KB_NUMPAD3", True])
	keys.append(["KB_NUMPAD4", True])
	keys.append(["KB_NUMPAD5", True])
	keys.append(["KB_NUMPAD6", True])
	keys.append(["KB_NUMPAD7", True])
	keys.append(["KB_NUMPAD8", True])
	keys.append(["KB_NUMPAD9", True])
	keys.append(["KB_NUMPADMINUS", True])
	keys.append(["KB_NUMPADPLUS", True])
	keys.append(["KB_NUMPADPERIOD", True])
	keys.append(["KB_NUMPADEQUALS", True])
	keys.append(["KB_AT", True])
	keys.append(["KB_UNDERLINE", True])
	keys.append(["KB_COLON", True])
	keys.append(["KB_NUMPADENTER", False])
	keys.append(["KB_RCONTROL", False])
	keys.append(["KB_VOLUMEDOWN", False])
	keys.append(["KB_VOLUMEUP", False])
	keys.append(["KB_NUMPADCOMMA", True])
	keys.append(["KB_NUMPADSLASH", True])
	keys.append(["KB_SYSRQ", False])
	keys.append(["KB_RALT", False])
	keys.append(["KB_PAUSE", False])
	keys.append(["KB_HOME", False])
	keys.append(["KB_UP", False])
	keys.append(["KB_PGUP", False])
	keys.append(["KB_LEFT", False])
	keys.append(["KB_RIGHT", False])
	keys.append(["KB_END", False])
	keys.append(["KB_DOWN", False])
	keys.append(["KB_PGDN", False])
	keys.append(["KB_INSERT", False])
	keys.append(["KB_DELETE", False])

	if (keyID >= 0 and keyID < len(keys)):
		return keys[keyID]
	return None

def getKeyString(keyID):
	key = getKeyToken(keyID)
	if key != None:
		return key[0]
	return ""

def isKeyPrintable(keyID):
	key = getKeyToken(keyID)
	if key != None:
		return key[1]
	return False


# The vanilla inputClass contains what's needed, but it can be confusing to read.
# For instance if flags are 16384 and notify code is 0, it's just a click on the left mouse button.
# This module/class is intended to provide human readable tests on the input.
# Sometimes it's converting numbers to names, sometimes it's testing combos in variables, sometimes both.
# Splitting this decoding away from the other code makes both this and the other code more readable.

class xmlInput:

	def __init__(self, inputClass, editor):
		self.inputClass = inputClass
		self.editor = editor

	def isLeftCLick(self):
		if (self.inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if (self.inputClass.getFlags() == 0x4000):
				return True
		return False

	def isRightCLick(self):
		if (self.inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if (self.inputClass.getFlags() == 0x20000):
				return True
		return False

	def getFunctionName(self):
		return self.inputClass.getFunctionName()

	def isShiftKeyDown(self):
		return self.inputClass.isShiftKeyDown()

	def isCtrlKeyDown(self):
		return self.inputClass.isCtrlKeyDown()

	def isAltKeyDown(self):
		return self.inputClass.isAltKeyDown()

	def getWidget(self):
		return WidgetTypes(self.inputClass.getButtonType())

	def getData(self):
		return self.inputClass.getData()

	def getData1(self):
		return self.inputClass.getData1()

	def getData2(self):
		return self.inputClass.getData2()

	def isOK(self):
		if (self.inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if (self.getWidget() == WidgetTypes.WIDGET_XML_OK):
				return True
		return False

	def isCancel(self):
		if (self.inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if (self.getWidget() == WidgetTypes.WIDGET_XML_CANCEL):
				return True
		return False

	def getMenuItem(self):
		if (self.inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED):
			return self.inputClass.getData()
		return -1

	def isKey(self, key):
		if self.inputClass.getNotifyCode() == NotifyCode.NOTIFY_CHARACTER:
			return self.inputClass.getData() == key
		return False

	def isKeyNoneModifier(self):
		if self.inputClass.getNotifyCode() == NotifyCode.NOTIFY_CHARACTER:
			if self.inputClass.getData() != InputTypes.KB_LCONTROL:
				if self.inputClass.getData() != InputTypes.KB_RCONTROL:
					if self.inputClass.getData() != InputTypes.KB_LALT:
						if self.inputClass.getData() != InputTypes.KB_RALT:
							if self.inputClass.getData() != InputTypes.KB_LSHIFT:
								if self.inputClass.getData() != InputTypes.KB_RSHIFT:
									return True
		return False

	# directs keyboard input to a specific EditBox
	# returns true if something changed
	def updateEditBox(self, editBoxName, uppercase = False):
		if (self.inputClass.getNotifyCode() == NotifyCode.NOTIFY_CHARACTER):
			if isKeyPrintable(self.inputClass.getData()):
				screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
				string = screen.getEditBoxString(editBoxName)
				string = self.readKeyboard(string)
				if uppercase:
					string = string.upper()
				screen.setEditBoxString(editBoxName, string)
				return True
		return False
		
	# read keycode and append/remove from a string
	def readKeyboard(self, string):
		# read the string associated with the key in question 
		key = self.editor.XML.getWKey(self.inputClass.getData(), self.inputClass.isShiftKeyDown(), self.inputClass.isCtrlKeyDown(), self.inputClass.isAltKeyDown())

		if (key != None):
			# some keywords have special meaning. Apply the meaning rather than the keywords
			if (key == "delete"):
				return string[:-1]
			if (key == "delete front"):
				return string[1:]
			if (key == "space"):
				return string + " "

			return string + key
		return string

	def getNotificationText(self):
		notifications = []
		notifications.append("NOTIFY_CLICKED")
		notifications.append("NOTIFY_DBL_CLICKED")
		notifications.append("NOTIFY_FOCUS")
		notifications.append("NOTIFY_UNFOCUS")
		notifications.append("NOTIFY_CURSOR_MOVE_ON")
		notifications.append("NOTIFY_CURSOR_MOVE_OFF")
		notifications.append("NOTIFY_CHARACTER")
		notifications.append("NOTIFY_SCROLL_UP")
		notifications.append("NOTIFY_SCROLL_DOWN")
		notifications.append("NOTIFY_NEW_HORIZONTAL_STOP")
		notifications.append("NOTIFY_NEW_VERTICAL_STOP")
		notifications.append("NOTIFY_LISTBOX_ITEM_SELECTED")
		notifications.append("NOTIFY_MOUSEMOVE") # 12 is also NOTIFY_FLYOUT_ITEM_SELECTED
		notifications.append("13")
		notifications.append("NOTIFY_MOUSEWHEELUP")
		notifications.append("NOTIFY_MOUSEWHEELDOWN")
		notifications.append("16")
		notifications.append("17")
		notifications.append("NOTIFY_LINKEXECUTE")
		notifications.append("NOTIFY_MOVIE_DONE")
		notifications.append("NOTIFY_SLIDER_NEWSTOP")
		notifications.append("NOTIFY_TABLE_HEADER_SELECTED")

		return notifications[self.inputClass.getNotifyCode()]

	def getWidgetString(self):
		eWidget = self.getWidget()
		if eWidget == WidgetTypes.WIDGET_GENERAL:
			return "WIDGET_GENERAL"
		if eWidget == WidgetTypes.WIDGET_XML_EDITOR:
			return "WIDGET_XML_EDITOR"
		if eWidget == WidgetTypes.WIDGET_XML_EDITOR_BOX:
			return "WIDGET_XML_EDITOR_BOX"
		if eWidget == WidgetTypes.WIDGET_XML_OK:
			return "WIDGET_XML_OK"
		if eWidget == WidgetTypes.WIDGET_XML_CANCEL:
			return "WIDGET_XML_CANCEL"
		if eWidget == WidgetTypes.WIDGET_XML_DELETE:
			return "WIDGET_XML_DELETE"
		if eWidget == WidgetTypes.WIDGET_XML_CLONE:
			return "WIDGET_XML_CLONE"
		if eWidget == WidgetTypes.WIDGET_XML_DOWN:
			return "WIDGET_XML_DOWN"
		if eWidget == WidgetTypes.WIDGET_XML_ZERO:
			return "WIDGET_XML_ZERO"
		if eWidget == WidgetTypes.WIDGET_XML_UP:
			return "WIDGET_XML_UP"

		# fallback because it's impossible to always support everything in the expanding widget enum
		return str(eWidget)

	def printInput(self):
		print "-----------new input-----------"
		print "Notify code:  " + self.getNotificationText()
		print "Data:         " + str(self.inputClass.getData())
		if self.inputClass.getNotifyCode() == NotifyCode.NOTIFY_CHARACTER:
			print "Key (Data):   " + getKeyString(self.inputClass.getData())
		print "Flags:        " + str(self.inputClass.getFlags())
		print "ID:           " + str(self.inputClass.getID())
		print "Func Name:    " + str(self.inputClass.getFunctionName())
		print "Shift:        " + str(self.inputClass.isShiftKeyDown())
		print "Control:      " + str(self.inputClass.isCtrlKeyDown())
		print "Alt:          " + str(self.inputClass.isAltKeyDown())
		print "MouseX:       " + str(self.inputClass.getMouseX())
		print "MouseY:       " + str(self.inputClass.getMouseY())
		print "ButtonType:   " + self.getWidgetString()
		print "Data1:        " + str(self.inputClass.getData1())
		print "Data2:        " + str(self.inputClass.getData2())
		print "Option:       " + str(self.inputClass.getOption())
