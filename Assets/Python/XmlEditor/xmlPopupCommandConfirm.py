from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import CvEventManager

class xmlPopupCommandConfirm:

	def __init__(self, editor, commandElement, commandID):
		self.editor = editor
		self.element = commandElement
		self.commandID = commandID

	def draw(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		
		nScreenWidth = 400
		nScreenHeight = 300
		
		left = (screen.getXResolution()/2) - (nScreenWidth/2)
		top  = (screen.getYResolution()/2) - (nScreenHeight/2)
		
		panelLeft   = left + 10
		panelTop    = top + 50
		panelWidth  = nScreenWidth - 20
		panelHeight = nScreenHeight - 150
		
		
		screen.addPanel( "xmlEditorSmallBG", u"", u"", True, False, left, top, nScreenWidth, nScreenHeight, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setLabel("xmlEditorSmallHeader", "xmlEditorSmallBG", u"<font=4b>" + self.element.getName() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, left + nScreenWidth / 2, top + 18, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		screen.addPanel( "HelpTextField", u"", u"", True, False, panelLeft, panelTop, panelWidth, panelHeight, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		

		text = CyTranslator().getText(self.element.getText(), ())
		screen.attachMultilineText("HelpTextField", "MultiLineText", text, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		
		screen.setTextAt("cancel_button", "xmlEditorSmallBG", u"<font=4u>" + "CANCEL" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, 3*(nScreenWidth / 4),  nScreenHeight - 60, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_XML_CANCEL, -1, -1)
		screen.setTextAt("ok_button", "xmlEditorSmallBG", u"<font=4u>" + "OK" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth / 4,  nScreenHeight - 60, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_XML_OK, self.commandID, -1)
		

	def delete(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )

		screen.deleteWidget("xmlEditorSmallBG")
		screen.deleteWidget("HelpTextField")

	def input(self, input):
		if (input.isOK()):
			iCommandID = input.getData1()
			if (iCommandID >= 0 and iCommandID < len(self.editor.commandList)):
				command = self.editor.commandList[iCommandID]
				self.editor.doCommandExecute(command)
				return True

		return False

	def getHelp(self, eWidgetType, iData1, iData2, bOption):
		return None
