from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import CvEventManager

class xmlPopupElementType:

	def __init__(self, editor, element, iColumn):
		self.editor = editor
		self.element = element
		self.iColumn = iColumn
		
		self.nScreenWidth  = 400
		self.nScreenHeight = 300

	def draw(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		
		nScreenWidth  = self.nScreenWidth
		nScreenHeight = self.nScreenHeight
		
		left = (screen.getXResolution()/2) - (nScreenWidth/2)
		top  = (screen.getYResolution()/2) - (nScreenHeight/2)
		
		screen.addPanel( "xmlEditorSmallBG", u"", u"", True, False, left, top, nScreenWidth, nScreenHeight, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setLabel("xmlEditorSmallHeader", "xmlEditorSmallBG", u"<font=4b>" + self.element.getName() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, left + nScreenWidth / 2, top + 18, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		if self.element.isCombo():
			screen.addDropDownBoxGFC("xmlEditorTypeDirSelector", left + 20, top + 50, 360, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.TITLE_FONT)
			typeID = self.element.getInfoType()
			iActiveComboFile = self.element.getActiveComboFile()
			for i in range(self.editor.XML.getNumTypes(typeID)):
				loopType = self.editor.XML.getType(typeID, i)
				screen.addPullDownString("xmlEditorTypeDirSelector", loopType, i, i, i == iActiveComboFile)
		
		self.popupTypeDrawPulldown()
		
		helpText = self.element.getHelp()
		if (helpText != None):
			screen.addMultilineText("helpText", helpText, left + 10, top + 110, nScreenWidth - 20, nScreenHeight - 60 - 110, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		
		if self.element.isRemoteCreate():
			screen.setTextAt("create_button", "xmlEditorSmallBG", u"<font=4u>" + "Create remote entry" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth / 2,  nScreenHeight - 100, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_XML_OK, -10, -10)
		
		screen.setTextAt("cancel_button", "xmlEditorSmallBG", u"<font=4u>" + "CANCEL" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, 3*(nScreenWidth / 4),  nScreenHeight - 60, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_XML_CANCEL, -1, -1)
		screen.setTextAt("ok_button", "xmlEditorSmallBG", u"<font=4u>" + "OK" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth / 4,  nScreenHeight - 60, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_XML_OK, -1, -1)
		
		self.drawColorPreview()
		
	def delete(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )

		screen.deleteWidget("xmlEditorSmallBG")
		screen.deleteWidget("xmlEditorTypeDirSelector")
		screen.deleteWidget("xmlEditorTypeSelector")
		screen.deleteWidget("helpText")

	def input(self, input):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		
		if input.getFunctionName() == "xmlEditorTypeDirSelector":
			self.popupTypeDrawPulldown()
		elif input.getFunctionName() == "xmlEditorTypeSelector":
			self.drawColorPreview()
		elif input.isOK():
			if input.getData1() == -10:
				self.element.createRemote()
			else:
				# ok button was pressed
				typeID = self.element.getInfoType()
				if self.element.isCombo():
					iIndex = screen.getSelectedPullDownID("xmlEditorTypeDirSelector")
					typeID = self.editor.XML.getComboFile(typeID, iIndex)
				iIndex = screen.getPullDownData("xmlEditorTypeSelector", screen.getSelectedPullDownID("xmlEditorTypeSelector"))
			
				newString = self.editor.XML.getType(typeID, iIndex)
				self.element.setValue(newString)
			self.editor.columnContainers[self.iColumn].drawContents(True, self.element)
			self.editor.closePopupWindow()
		else:
			return False
		return True

	def getHelp(self, eWidgetType, iData1, iData2, bOption):
		return None
		
	def popupTypeDrawPulldown(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )

		nScreenWidth  = self.nScreenWidth
		nScreenHeight = self.nScreenHeight

		left = (screen.getXResolution()/2) - (nScreenWidth/2)
		top  = (screen.getYResolution()/2) - (nScreenHeight/2)

		typeID = self.element.getInfoType()
		if self.element.isCombo():
			iIndex = screen.getSelectedPullDownID("xmlEditorTypeDirSelector")
			typeID = self.editor.XML.getComboFile(typeID, iIndex)

		selectedType = ""
		if (self.element.isAllocated()):
			selectedType = self.element.getValue()

		# dropdown menu
		prefix = self.editor.XML.getFilePrefix(typeID)
		iPrefixOffset = 0
		if prefix != None:
			iPrefixOffset = 100
			screen.setTextAt("prefix", "xmlEditorSmallBG", u"<font=3>" + prefix + u"</font>", CvUtil.FONT_RIGHT_JUSTIFY, 120,  95, -0.1, FontTypes.MENU_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		screen.addDropDownBoxGFC("xmlEditorTypeSelector", left + 20 + iPrefixOffset, top + 80, 360 - iPrefixOffset, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.TITLE_FONT)
		if self.element.allowsTypeNone():
			screen.addPullDownString("xmlEditorTypeSelector", "NONE", -1, -1, "NONE" == selectedType)

		for i in range(self.editor.XML.getNumTypes(typeID)):
			loopType = self.editor.XML.getType(typeID, i)
			loopTypeNoPrefix = self.editor.XML.getTypeNoPrefix(typeID, i)
			screen.addPullDownString("xmlEditorTypeSelector", loopTypeNoPrefix, i, i, loopType == selectedType)
	

	def drawColorPreview(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		nScreenWidth  = self.nScreenWidth
		nScreenHeight = self.nScreenHeight
		
		if self.element.getInfoTypeString() == "ColorVal":
		# color
			typeID = self.element.getInfoType()
			iIndex = screen.getPullDownData("xmlEditorTypeSelector", screen.getSelectedPullDownID("xmlEditorTypeSelector"))
			ColorString = self.editor.XML.getType(typeID, iIndex)
			color = self.editor.XML.getColor(ColorString)
			printString = u"<font=4u><color="
			printString += unicode(int(color.r*255)) + "," + unicode(int(color.g*255)) + "," + unicode(int(color.b*255)) + "," + unicode(int(color.a*255))
			printString += ">"
			printString += "COLOR PREVIEW"
			printString += u"</color></font>"
			screen.setTextAt("color secondary", "xmlEditorSmallBG", printString, CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth/2, 115, -0.1, FontTypes.MENU_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		elif self.element.getInfoTypeString() == "PlayerColorInfo":
		# player color
			typeID = self.element.getInfoType()
			iIndex = screen.getPullDownData("xmlEditorTypeSelector", screen.getSelectedPullDownID("xmlEditorTypeSelector"))
			PlayerColorString = self.editor.XML.getType(typeID, iIndex)
			
			screen.setTextAt("color primary"  , "xmlEditorSmallBG", self.getColorString(PlayerColorString, 0, "Primary"  ), CvUtil.FONT_LEFT_JUSTIFY  ,                10, 115, -0.1, FontTypes.MENU_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.setTextAt("color secondary", "xmlEditorSmallBG", self.getColorString(PlayerColorString, 1, "Secondary"), CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth/2   , 115, -0.1, FontTypes.MENU_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.setTextAt("color text"     , "xmlEditorSmallBG", self.getColorString(PlayerColorString, 2, "Text"     ), CvUtil.FONT_RIGHT_JUSTIFY , nScreenWidth - 10, 115, -0.1, FontTypes.MENU_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
	def getColorString(self, PlayerColor, iIndex, string):
		color = self.editor.XML.getPlayerColor(PlayerColor, iIndex)
				
		output = u"<font=4u><color="
		output += unicode(int(color.r*255)) + "," + unicode(int(color.g*255)) + "," + unicode(int(color.b*255)) + "," + unicode(int(color.a*255))
		output += ">"
		output += string
		output += u"</color></font>"

		return output