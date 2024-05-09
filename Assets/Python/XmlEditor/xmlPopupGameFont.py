from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import CvEventManager

class xmlPopupGameFont:

	def __init__(self, editor):
		self.editor = editor

	def draw(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )

		nScreenWidth = 400
		nScreenHeight = screen.getYResolution() - 50

		left = (screen.getXResolution()/2) - (nScreenWidth/2)
		top  = (screen.getYResolution()/2) - (nScreenHeight/2)

		screen.addPanel( "xmlEditorSmallBG", u"", u"", True, False, left, top, nScreenWidth, nScreenHeight, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		nTableWidth = 3*56 + 20
		nTableHeight = nScreenHeight - 80
		nTableLeft = left + ((nScreenWidth-nTableWidth) / 2)
		nTableTop = top + 20

		screen.addTableControlGFC("GameFontTable", 3, nTableLeft, nTableTop, nTableWidth, nTableHeight, True, False, 20, 20, TableStyles.TABLE_STYLE_STANDARD )
		screen.setStyle("GameFontTable", "Table_StandardCiv_Style")

		screen.setTableColumnHeader( "GameFontTable", 0, "<font=2>ID</font>", 56)
		screen.setTableColumnHeader( "GameFontTable", 1, "<font=2>Small</font>", 56)
		screen.setTableColumnHeader( "GameFontTable", 2, "<font=2>Big</font>", 56)

		screen.setTextAt("close_button", "xmlEditorSmallBG", u"<font=4u>" + "Close" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth / 2,  nScreenHeight - 30, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_XML_CANCEL, -1, -1)

		# fill the table
		iMax = self.editor.XML.getMaxGameFontID() - 8483
		for iLine in range(iMax + 1):
			iID = iLine + 8483
			screen.appendTableRow("GameFontTable")
			screen.setTableInt("GameFontTable", 0, iLine , "<font=2>" + unicode(iID) + "<font/>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			screen.setTableInt("GameFontTable", 1, iLine , "<font=2>" + (u" %c" %  (iID)) + "<font/>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
			screen.setTableInt("GameFontTable", 2, iLine , "<font=4>" + (u" %c" %  (iID)) + "<font/>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )

	def delete(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )

		screen.deleteWidget("xmlEditorSmallBG")
		screen.deleteWidget("GameFontTable")

	def input(self, input):
		return False

	def getHelp(self, eWidgetType, iData1, iData2, bOption):
		return None
