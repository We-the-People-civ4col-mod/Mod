from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import CvEventManager

class xmlPopupElementFloat:

	def __init__(self, editor, element, iColumn):
		self.editor = editor
		self.element = element
		self.iColumn = iColumn
		
		self.nScreenWidth  = 600
		self.nScreenHeight = 300

	def draw(self):
		string = None
		if (self.element.isAllocated()):
			string = self.element.getValue()
		if string == None and self.element.getName() == "Type":
			string = self.editor.XML.getFilePrefix(self.FILE)

		self.nScreenWidth = 600
		self.redraw(self.nScreenWidth, string)

	def redraw(self, nScreenWidth, string):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		nScreenHeight = 300

		left = (screen.getXResolution()/2) - (nScreenWidth/2)
		top  = (screen.getYResolution()/2) - (nScreenHeight/2)

		screen.addPanel( "xmlEditorSmallBG", u"", u"", True, False, left, top, nScreenWidth, nScreenHeight, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setLabel("xmlEditorSmallHeader", "xmlEditorSmallBG", u"<font=4b>" + self.element.getName() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, left + nScreenWidth / 2, top + 18, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addEditBoxGFC("stringInputBox", left + 10, top + 60, nScreenWidth - 20, 40, WidgetTypes.WIDGET_GENERAL, 0, 0, FontTypes.TITLE_FONT)

		if (string != None):
			screen.setEditBoxString ("stringInputBox", string)
		screen.setEditBoxMaxCharCount("stringInputBox", 10000, 10)

		helpText = self.element.getHelp()
		if (helpText != None):
			if (len(helpText) > 0):
				helpText = str(CyTranslator().getText(helpText, ()))
			screen.addMultilineText("helpText", helpText, left + 10, top + 110, nScreenWidth - 20, nScreenHeight - 180, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

		screen.setTextAt("cancel_button", "xmlEditorSmallBG", u"<font=4u>" + "CANCEL" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, 3*(nScreenWidth / 4),  nScreenHeight - 60, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_XML_CANCEL, -1, -1)
		screen.setTextAt("ok_button", "xmlEditorSmallBG", u"<font=4u>" + "CONFIRM" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth / 4,  nScreenHeight - 60, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_XML_OK, -1, -1)

	def delete(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )

		screen.deleteWidget("xmlEditorSmallBG")
		screen.deleteWidget("stringInputBox")
		screen.deleteWidget("helpText")

	def input(self, input):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )

		if (input.isOK()):
			newtext = screen.getEditBoxString("stringInputBox")
			if ((not self.element.isAllocated() and len(newtext) > 0) or newtext != self.element.getValue()):
				self.element.setValue(str(newtext))
				self.editor.columnContainers[self.iColumn].drawContents(self.element)
			self.editor.closePopupWindow()
			return True
		elif input.inputClass.getNotifyCode() == NotifyCode.NOTIFY_CHARACTER and InputTypes.KB_0 <= input.inputClass.getData() <= InputTypes.KB_9:
			string = screen.getEditBoxString("stringInputBox")
			string = input.readKeyboard(string)
			self.redraw(self.nScreenWidth, string)
			return True
		elif input.isKey(InputTypes.KB_PERIOD):
			string = screen.getEditBoxString("stringInputBox")
			if not "." in string:
				if string == "":
					string = "0."
				else:
					string += "."
				self.redraw(self.nScreenWidth, string)
			return True
		elif input.getKeyFromKeyboard() == "delete":
			string = screen.getEditBoxString("stringInputBox")
			self.redraw(self.nScreenWidth, string[:-1])
			return True

		return True

	def getHelp(self, eWidgetType, iData1, iData2, bOption):
		return None