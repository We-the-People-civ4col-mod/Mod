from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import CvEventManager

class xmlPopupElementDelete:

	def __init__(self, editor, element, iColumn):
		self.editor = editor
		self.element = element
		self.iColumn = iColumn

	def draw(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		
		nScreenWidth = 700
		nScreenHeight = 480
		
		left = (screen.getXResolution()/2) - (nScreenWidth/2)
		top  = (screen.getYResolution()/2) - (nScreenHeight/2)
		
		screen.addPanel( "xmlEditorSmallBG", u"", u"", True, False, left, top, nScreenWidth, nScreenHeight, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setLabel("xmlEditorSmallHeader", "xmlEditorSmallBG", u"<font=4b>" + "Delete element" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, left + nScreenWidth / 2, top + 18, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		

		screen.setTextAt("BA", "xmlEditorSmallBG", u"<font=4u>" + "WARNING:" + u"</font>"              , CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth/2,   220, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setTextAt("BB", "xmlEditorSmallBG", u"<font=4u>" + "This can't be undone" + u"</font>"              , CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth/2,   270, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		screen.setTextAt("cancel_button", "xmlEditorSmallBG", u"<font=4u>" + "CANCEL" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, 3*(nScreenWidth / 4),  nScreenHeight - 60, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_XML_CANCEL, -1, -1)
		screen.setTextAt("ok_button", "xmlEditorSmallBG", u"<font=4u>" + "CONFIRM" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth / 4,  nScreenHeight - 60, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_XML_OK, -1, -1)
	
	def delete(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )

		screen.deleteWidget("xmlEditorSmallBG")

	def input(self, input):
		if (input.isOK()):
			# delete the entry in the file
			self.element.deleteXMLEntry()
			# redraw the display without the deleted element
			self.editor.updateColumn(self.iColumn)
			# remove the popup window itself
			self.editor.closePopupWindow()
			return True
		return False

	def getHelp(self, eWidgetType, iData1, iData2, bOption):
		return None
