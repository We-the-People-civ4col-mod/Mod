from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import CvEventManager

def getKeyToken(keyID):
	keys = [
		["KB_NONE", False],
		["KB_ESCAPE", False],
		["KB_0", True],
		["KB_1", True],
		["KB_2", True],
		["KB_3", True],
		["KB_4", True],
		["KB_5", True],
		["KB_6", True],
		["KB_7", True],
		["KB_8", True],
		["KB_9", True],
		["KB_MINUS", True],
		["KB_A", True],
		["KB_B", True],
		["KB_C", True],
		["KB_D", True],
		["KB_E", True],
		["KB_F", True],
		["KB_G", True],
		["KB_H", True],
		["KB_I", True],
		["KB_J", True],
		["KB_K", True],
		["KB_L", True],
		["KB_M", True],
		["KB_N", True],
		["KB_O", True],
		["KB_P", True],
		["KB_Q", True],
		["KB_R", True],
		["KB_S", True],
		["KB_T", True],
		["KB_U", True],
		["KB_V", True],
		["KB_W", True],
		["KB_X", True],
		["KB_Y", True],
		["KB_Z", True],
		["KB_EQUALS", True],
		["KB_BACKSPACE", True],
		["KB_TAB", True],
		["KB_LBRACKET", True],
		["KB_RBRACKET", True],
		["KB_RETURN", False],
		["KB_LCONTROL", False],
		["KB_SEMICOLON", True],
		["KB_APOSTROPHE", True],
		["KB_GRAVE", True],
		["KB_LSHIFT", False],
		["KB_BACKSLASH", True],
		["KB_COMMA", True],
		["KB_PERIOD", True],
		["KB_SLASH", True],
		["KB_RSHIFT", False],
		["KB_NUMPADSTAR", True],
		["KB_LALT", False],
		["KB_SPACE", True],
		["KB_CAPSLOCK", False],
		["KB_F1", True],
		["KB_F2", True],
		["KB_F3", True],
		["KB_F4", True],
		["KB_F5", True],
		["KB_F6", True],
		["KB_F7", True],
		["KB_F8", True],
		["KB_F9", True],
		["KB_F10", True],
		["KB_F11", True],
		["KB_F12", True],
		["KB_NUMLOCK", False],
		["KB_SCROLL", False],
		["KB_NUMPAD0", True],
		["KB_NUMPAD1", True],
		["KB_NUMPAD2", True],
		["KB_NUMPAD3", True],
		["KB_NUMPAD4", True],
		["KB_NUMPAD5", True],
		["KB_NUMPAD6", True],
		["KB_NUMPAD7", True],
		["KB_NUMPAD8", True],
		["KB_NUMPAD9", True],
		["KB_NUMPADMINUS", True],
		["KB_NUMPADPLUS", True],
		["KB_NUMPADPERIOD", True],
		["KB_NUMPADEQUALS", True],
		["KB_AT", True],
		["KB_UNDERLINE", True],
		["KB_COLON", True],
		["KB_NUMPADENTER", False],
		["KB_RCONTROL", False],
		["KB_VOLUMEDOWN", False],
		["KB_VOLUMEUP", False],
		["KB_NUMPADCOMMA", True],
		["KB_NUMPADSLASH", True],
		["KB_SYSRQ", False],
		["KB_RALT", False],
		["KB_PAUSE", False],
		["KB_HOME", False],
		["KB_UP", False],
		["KB_PGUP", False],
		["KB_LEFT", False],
		["KB_RIGHT", False],
		["KB_END", False],
		["KB_DOWN", False],
		["KB_PGDN", False],
		["KB_INSERT", False],
		["KB_DELETE", False]]

	if  0 <= keyID < len(keys):
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
			return not self.inputClass.getData() in \
				[InputTypes.KB_LCONTROL, InputTypes.KB_RCONTROL,
				 InputTypes.KB_LALT,     InputTypes.KB_RALT,
				 InputTypes.KB_LSHIFT,   InputTypes.KB_RSHIFT]

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

		if key == None:
			return string

		# some keywords have special meaning. Apply the meaning rather than the keywords
		# by default, we want the key at end of the string
		return {
			"delete" : string[:-1],
			"delete front" : string [1:],
			"space" : string + " "
		}.get(key, string + key)


	def getNotificationText(self):

		notifications = [
			"NOTIFY_CLICKED",
			"NOTIFY_DBL_CLICKED",
			"NOTIFY_FOCUS",
			"NOTIFY_UNFOCUS",
			"NOTIFY_CURSOR_MOVE_ON",
			"NOTIFY_CURSOR_MOVE_OFF",
			"NOTIFY_CHARACTER",
			"NOTIFY_SCROLL_UP",
			"NOTIFY_SCROLL_DOWN",
			"NOTIFY_NEW_HORIZONTAL_STOP",
			"NOTIFY_NEW_VERTICAL_STOP",
			"NOTIFY_LISTBOX_ITEM_SELECTED",
			"NOTIFY_MOUSEMOVE", # 12 is also NOTIFY_FLYOUT_ITEM_SELECTED
			"13",
			"NOTIFY_MOUSEWHEELUP",
			"NOTIFY_MOUSEWHEELDOWN",
			"16",
			"17",
			"NOTIFY_LINKEXECUTE",
			"NOTIFY_MOVIE_DONE",
			"NOTIFY_SLIDER_NEWSTOP",
			"NOTIFY_TABLE_HEADER_SELECTED"]

		return notifications[self.inputClass.getNotifyCode()]

	def getWidgetString(self):
		eWidget = self.getWidget()
		types_to_string = {\
			WidgetTypes.WIDGET_GENERAL: "WIDGET_GENERAL",
			WidgetTypes.WIDGET_XML_EDITOR: "WIDGET_XML_EDITOR",
			WidgetTypes.WIDGET_XML_EDITOR_BOX: "WIDGET_XML_EDITOR_BOX",
			WidgetTypes.WIDGET_XML_OK: "WIDGET_XML_OK",
			WidgetTypes.WIDGET_XML_CANCEL: "WIDGET_XML_CANCEL",
			WidgetTypes.WIDGET_XML_DELETE: "WIDGET_XML_DELETE",
			WidgetTypes.WIDGET_XML_CLONE: "WIDGET_XML_CLONE",
			WidgetTypes.WIDGET_XML_DOWN: "WIDGET_XML_DOWN",
			WidgetTypes.WIDGET_XML_ZERO: "WIDGET_XML_ZERO",
			WidgetTypes.WIDGET_XML_UP: "WIDGET_XML_UP"
		}
		return types_to_string.get(eWidget, str(eWidget))


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
