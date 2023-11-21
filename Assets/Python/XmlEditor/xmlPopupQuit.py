from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import CvEventManager

localText = CyTranslator()

class xmlPopupQuit:

	def __init__(self, editor):
		self.editor = editor

	def draw(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		
		nScreenWidth = 400
		nScreenHeight = 300
		
		left = (screen.getXResolution()/2) - (nScreenWidth/2)
		top  = (screen.getYResolution()/2) - (nScreenHeight/2)
		
		screen.addPanel( "xmlEditorSmallBG", u"", u"", True, False, left, top, nScreenWidth, nScreenHeight, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setLabel("xmlEditorSmallHeader", "xmlEditorSmallBG", u"<font=4b>" + localText.getText("TXT_KEY_XML_EDITOR_CONFIRM_QUIT", ()) + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, left + nScreenWidth / 2, top + 18, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		screen.addMultilineText("quit_text", u"<font=4u>" + localText.getText("TXT_KEY_XML_EDITOR_CONFIRM_QUIT_MESSAGE", ()) + u"</font>", left + 10, top + 100, nScreenWidth - 20, 100, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)

		screen.setTextAt("cancel_button", "xmlEditorSmallBG", u"<font=4u>" + localText.getText("TXT_KEY_SCREEN_CANCEL", ()).upper() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, 3*(nScreenWidth / 4),  nScreenHeight - 60, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_XML_CANCEL, -1, -1)
		screen.setTextAt("ok_button", "xmlEditorSmallBG", u"<font=4u>" + localText.getText("TXT_KEY_MAIN_MENU_OK", ()) + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth / 4,  nScreenHeight - 60, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)
		
	def delete(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )

		screen.deleteWidget("xmlEditorSmallBG")
		screen.deleteWidget("quit_text")

	def input(self, input):
		if (input.isOK()):
			self.editor.XML.quit()
		return False

	def getHelp(self, eWidgetType, iData1, iData2, bOption):
		return None
