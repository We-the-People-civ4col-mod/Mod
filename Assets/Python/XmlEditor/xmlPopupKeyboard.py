from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import CvEventManager

class xmlPopupKeyboard:

	def __init__(self, editor):
		self.editor = editor
		self.generateListItems()
		
		self.selectedItem = -1

	def draw(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )

		iBorderSide   = 15
		iBorderTop    = 50
		iBorderBottom = 50
		iRowHeight    = 30

		nScreenWidth = 400
		nScreenHeight = (len(self.list) * (iRowHeight + 1)) + iBorderTop + iBorderBottom

		if (screen.getYResolution() < nScreenHeight):
			nScreenHeight = screen.getYResolution()

		left = (screen.getXResolution()/2) - (nScreenWidth/2)
		top  = (screen.getYResolution()/2) - (nScreenHeight/2)

		screen.addPanel( "xmlEditorSmallBG", u"", u"", True, False, left, top, nScreenWidth, nScreenHeight, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setLabel("xmlEditorSmallHeader", "xmlEditorSmallBG", u"<font=4b>" + "Keyboard calibration" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, left + nScreenWidth / 2, top + 18, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		scrollHeight = nScreenHeight - iBorderTop - iBorderBottom
		scrollWidth  = nScreenWidth - (2 * iBorderSide)

		panelTop  = top  + iBorderTop
		panelLeft = left + iBorderSide

		screen.addPanel( "KeyboardBaseTable", u"", u"", True, False, panelLeft, panelTop, scrollWidth, scrollHeight, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		screen.attachTableControlGFC("KeyboardBaseTable", "KeyboardTable", 0, False, True, scrollWidth, scrollHeight, TableStyles.TABLE_STYLE_ALTEMPTY)
		screen.setTableColumnHeader("KeyboardTable", 0, "", scrollWidth)

		for i in range(len(self.list)):
			screen.appendTableRow("KeyboardTable")
			screen.setTableRowHeight("KeyboardTable", i, iRowHeight)
			screen.setTableText("KeyboardTable", 0, i, u"<font=4>" + self.list[i][0] + u"</font>", "", WidgetTypes.WIDGET_XML_MENU_ITEM, i, -1, CvUtil.FONT_CENTER_JUSTIFY)

		screen.setTextAt("cancel_button", "xmlEditorSmallBG", u"<font=4u>" + "CLOSE" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, 2*(nScreenWidth / 4),  nScreenHeight - 30, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_XML_CANCEL, -1, -1)

	def delete(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )

		screen.deleteWidget("xmlEditorSmallBG")
		screen.deleteWidget("KeyboardBaseTable")
		self.deleteInputScreen()

	def deleteInputScreen(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		screen.deleteWidget("xmlKeyboardKeyInput")
		screen.deleteWidget("xmlKeyboardInput")
		screen.deleteWidget("xmlKeyboardInputHelp1")
		screen.deleteWidget("xmlKeyboardInputHelp2")
		screen.deleteWidget("xmlKeyboardInputHelp3")
		screen.deleteWidget("xmlKeyboardInputHelp4")

	def input(self, input):
		if (input.getFunctionName() == "KeyboardTable"):
			iIndex = input.getMenuItem()
			if (iIndex >= 0 and iIndex < len(self.list)):
				self.selectedItem = iIndex
				self.drawMinor()
				return True
		elif (self.selectedItem >= 0 and self.selectedItem < len(self.list) and input.isKeyNoneModifier()):
			self.editor.XML.setKeyboardKey(self.list[self.selectedItem][1], input.getData(), input.isShiftKeyDown(), input.isCtrlKeyDown(), input.isAltKeyDown())
			self.deleteInputScreen()
			self.selectedItem = -1
		
		return False

	def getHelp(self, eWidgetType, iData1, iData2, bOption):
		return None

	def drawMinor(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		nScreenWidth  = 220
		nScreenHeight = 126
		
		if len(self.list[self.selectedItem][0]) > 7:
			nScreenWidth  = 360
		
		left = (screen.getXResolution()/2) - (nScreenWidth/2)
		top  = (screen.getYResolution()/2) - (nScreenHeight/2)
	
		screen.addPanel( "xmlKeyboardKeyInput", u"", u"", True, False, left, top, nScreenWidth, nScreenHeight, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		
		screen.setLabel("xmlKeyboardInput", "xmlKeyboardKeyInput", u"<font=4b>" + "Type key: " + self.list[self.selectedItem][0] + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, left + 12, top + 18, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setLabel("xmlKeyboardInputHelp1", "xmlKeyboardKeyInput", u"<font=3>" + "Allowed modifier keys: " + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, left + 12, top + 48, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setLabel("xmlKeyboardInputHelp2", "xmlKeyboardKeyInput", u"<font=3>" + "Shift"                   + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, left + 12, top + 64, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setLabel("xmlKeyboardInputHelp3", "xmlKeyboardKeyInput", u"<font=3>" + "Control"                 + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, left + 12, top + 80, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setLabel("xmlKeyboardInputHelp4", "xmlKeyboardKeyInput", u"<font=3>" + "Alt"                     + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, left + 12, top + 96, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)


	def generateListItems(self):
		self.list = []
		
		self.list.append(["Space", "space"])
		self.list.append(["Newline", "[NEWLINE]"])
		self.list.append(["Delete first character", "delete front"])
		self.list.append(["Delete last character", "delete"])
		
		for i in range(256):
			if i <= 32 or i == 38 or i == 60 or i == 62 or i == 127:
				continue
			printedChar = (u"%c" %  (i))
			self.list.append([printedChar, printedChar])
