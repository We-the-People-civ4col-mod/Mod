from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import CvEventManager

class xmlPopupMenu:

	def __init__(self, editor):
		self.editor = editor

	def draw(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )

		iBorderSide   = 15
		iBorderTop    = 50
		iBorderBottom = 50
		iRowHeight    = 30

		nScreenWidth = 400
		nScreenHeight = (self.editor.menuItemCount * (iRowHeight + 1)) + iBorderTop + iBorderBottom

		if (screen.getYResolution() < nScreenHeight):
			nScreenHeight = screen.getYResolution()

		left = (screen.getXResolution()/2) - (nScreenWidth/2)
		top  = (screen.getYResolution()/2) - (nScreenHeight/2)

		screen.addPanel( "xmlEditorSmallBG", u"", u"", True, False, left, top, nScreenWidth, nScreenHeight, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setLabel("xmlEditorSmallHeader", "xmlEditorSmallBG", u"<font=4b>" + "MENU" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, left + nScreenWidth / 2, top + 18, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		scrollHeight = nScreenHeight - iBorderTop - iBorderBottom
		scrollWidth  = nScreenWidth - (2 * iBorderSide)

		panelTop  = top  + iBorderTop
		panelLeft = left + iBorderSide

		screen.addPanel( "MenuBaseTable", u"", u"", True, False, panelLeft, panelTop, scrollWidth, scrollHeight, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		screen.attachTableControlGFC("MenuBaseTable", "MenuTable", 0, False, True, scrollWidth, scrollHeight, TableStyles.TABLE_STYLE_ALTEMPTY)
		screen.setTableColumnHeader("MenuTable", 0, "", scrollWidth)

		for i in range(self.editor.menuItemCount):
			element = self.editor.XML.getCommandItem(self.editor.commandList[i])
			title = u"<font=3>" + element.getName() + u"</font>"
			screen.appendTableRow("MenuTable")
			screen.setTableRowHeight("MenuTable", i, iRowHeight)
			screen.setTableText("MenuTable", 0, i, title, "", WidgetTypes.WIDGET_XML_MENU_ITEM, i, -1, CvUtil.FONT_CENTER_JUSTIFY)

		screen.setTextAt("cancel_button", "xmlEditorSmallBG", u"<font=4u>" + "CANCEL" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, 2*(nScreenWidth / 4),  nScreenHeight - 30, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_XML_CANCEL, -1, -1)

	def delete(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )

		screen.deleteWidget("xmlEditorSmallBG")
		screen.deleteWidget("MenuBaseTable")

	def input(self, input):
		if (input.getFunctionName() == "MenuTable"):
			iIndex = input.getMenuItem()
			if (iIndex >= 0 and iIndex < len(self.editor.commandList)):
				self.editor.doCommand(self.editor.commandList[iIndex])
				return True
		return False

	def getHelp(self, eWidgetType, iData1, iData2, bOption):
		if eWidgetType == WidgetTypes.WIDGET_XML_MENU_ITEM:
			if iData1 >= 0 and iData1 < len(self.editor.commandList):
				return self.editor.XML.getCommandItem(self.editor.commandList[iData1]).getHelp()
		return None
