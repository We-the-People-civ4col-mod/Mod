from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import CvEventManager
import xmlColumnContainer
import xmlInput
import xmlPopupElementDelete
import xmlPopupElementString
import xmlPopupElementType

import xmlPopupCommandConfirm
import xmlPopupGameFont
import xmlPopupKeyboard
import xmlPopupMenu
import xmlPopupQuit

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class xmlEditorScreen:
	"XML_EDITOR"
	def __init__(self):
		self.FILE = 0
		self.FILE_DIR = "Units"
		
		self.columnContainers = []
		
		self.ICON_BUTTON_SIZE = 20
		
		self.selectedElement = None
		self.selectedColumn = -1
		
		# inputClass.getFlags() values
		self.inputLeftClick  =  16384
		self.inputRightClick = 131072
		
		
		self.popupNone = 0
		self.popupGameFont = 3
		self.popupTxtKey = 4
		self.popupEditor = 10
		self.popupHelp = 11
		self.popupReload = 12
		
		self.currentScreen = self.popupNone
		
		
		self.specialColumn = 100
		self.menuColumn = 101
		self.cancel_button = 0
		self.OK_button = 1
		self.help_button = 2
		self.reload_button = 3
		self.remote_create_button = 4
		self.string_type_button = 5
		self.string_type_none_button = 6
		self.string_type_remote_button = 7
		self.string_type_prefix_button = 8
		self.string_button_button = 9
		self.int_gamefont_button = 10
		self.dir_art_button = 11
		self.file_specific_button = 50
		self.txt_key_select = 100 # also use the next 3 numbers
		self.menu_button = -100
		self.error_button = -101
		
		# widgets
		self.widgetDelete = WidgetTypes.WIDGET_XML_DELETE
		self.widgetClone  = WidgetTypes.WIDGET_XML_CLONE
		self.widgetUp     = WidgetTypes.WIDGET_XML_UP
		self.widgetZero   = WidgetTypes.WIDGET_XML_ZERO
		self.widgetDown   = WidgetTypes.WIDGET_XML_DOWN
		
		self.widgetOkButton = WidgetTypes.WIDGET_XML_OK
		
		# set defaults. Overwritten later if the exe supports drag-n-drop
		self.widgetDrag   = WidgetTypes.WIDGET_GENERAL
		
	def onClose(self):
		# free memory allocated inside the DLL
		# free elements
		self.setNumColumns(0)
		
		# clear popup window class instance
		self.currentPopup = None
		
		# free XML interface
		self.XML = None
		
	# Screen construction function
	def interfaceScreen(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		self.nScreenWidth = screen.getXResolution()
		self.nScreenHeight = screen.getYResolution()
		
		# set up the editor early since the data from it can be used to set layout
		self.XML = gc.getxmlEditor()
		
		self.generateCommandList()
		
		self.TITLE_HEIGHT = 55
		
		self.dirty = False
		self.setColumnValue = -1
		self.updateCount = 0
		
		self.currentPopup = None
		
		self.hideUnallocated = False
		
		screen.setRenderInterfaceOnly(True)
		screen.setRenderFrozenWorld(True)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)
		screen.addPanel( "xmlEditorBG", u"", u"", True, False, 0, 0, self.nScreenWidth, self.nScreenHeight, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDDSGFC("TopPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TITLE").getPath(), 0, 0, self.nScreenWidth, self.TITLE_HEIGHT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setLabel("DomesticScreenWidgetHeader", "Background", u"<font=4b>" + u"XML editor" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.nScreenWidth / 2, 4, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		self.scrollPanel  = "ScrollPanel"
		self.scrollHeight = self.nScreenHeight - self.TITLE_HEIGHT - 30
		self.scrollWidth  = self.nScreenWidth
		
		screen.addScrollPanel(self.scrollPanel, u"", 0, self.TITLE_HEIGHT, self.scrollWidth, self.scrollHeight, PanelStyles.PANEL_STYLE_EXTERNAL, true, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		screen.setTextAt("top_help_button", "TopPanel", u"<font=4u>" + "HELP" + u"</font>", CvUtil.FONT_RIGHT_JUSTIFY, self.nScreenWidth - 20,  20, 0, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.help_button)
		if self.XML.isEditorInMod():
			screen.setTextAt("top_reload_button", "TopPanel", u"<font=4u>" + "Reload XML" + u"</font>", CvUtil.FONT_RIGHT_JUSTIFY, self.nScreenWidth - 120,  20, 0, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.reload_button)
		screen.setTextAt("top_menu_button", "TopPanel", u"<font=4u>" + "MENU" + u"</font>", CvUtil.FONT_RIGHT_JUSTIFY, self.nScreenWidth - 260,  20, 0, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.menu_button)
		screen.setTextAt("top_error_button", "TopPanel", u"<font=4u>" + "Find Error" + u"</font>", CvUtil.FONT_RIGHT_JUSTIFY, self.nScreenWidth - 340,  20, 0, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.error_button)
		
		# disable the feature where escape and enter automatically kills the screen
		# code further down will make a confirmed quit on escape
		screen.setCloseOnEscape(False)
				
		
		# load the icons from XML and cache them in the editor
		self.iconBoolOn          = self.XML.getIcon("BoolOn")
		self.iconBoolOff         = self.XML.getIcon("BoolOff")
		self.iconClone           = self.XML.getIcon("Clone")
		self.iconDefaultButton   = self.XML.getIcon("DefaultButton")
		self.iconDeleteElement   = self.XML.getIcon("DeleteElement")
		self.iconDirClosed       = self.XML.getIcon("DirClosed")
		self.iconDirOpen         = self.XML.getIcon("DirOpen")
		self.iconIntUpArrow      = self.XML.getIcon("IntUpArrow")
		self.iconIntDownArrow    = self.XML.getIcon("IntDownArrow")
		
		# add clone button
		if self.XML.isColonizationExe():
			self.widgetDrag   = WidgetTypes.WIDGET_XML_EDITOR_BOX
			screen.setImageButtonAt("CloneButton", "TopPanel", self.iconClone, (self.scrollWidth/2) - 120, 0, 35, 35,  WidgetTypes.WIDGET_XML_EDITOR_BOX, -10, -10 )	
					
		
		# set the active file to whatever it was the last time the editor was open
		self.fileStack = []
		self.fileStackForward = []
		self.fileStackType = []
		self.fileStackTypeForward = []
		self.FILE = -1
		file = self.XML.getActiveFile()
		self.FILE_DIR = "startup"
		self.changeFile(self.XML.getActiveFile())
	
	def drawDirSelector(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		screen.addDropDownBoxGFC("SelectedDir", 20, 5, 150, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.TITLE_FONT)
		for i in range(self.XML.getNumXMLDir()):
			dir = self.XML.getXMLDir(i)
			if (dir != "GlobalTypes"):
				screen.addPullDownString("SelectedDir", dir, i, i, dir == self.FILE_DIR)
		
		# if the currently selected file is not in the current dir, redraw the file selector
		# next set the currently selected file to the first in the list of the new file selector
		# this triggers whenever the dir selector picks a new dir and the file is in the old dir
		if (self.XML.getFileDir(self.FILE) != self.FILE_DIR):
			self.drawFileSelector()
			self.changeFile(screen.getPullDownData("SelectedFile", screen.getSelectedPullDownID("SelectedFile")))
	
	def drawFileSelector(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		screen.addDropDownBoxGFC("SelectedFile", 170, 5, 250, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.TITLE_FONT)
		for i in range(self.XML.getNumFiles()):
			if (self.XML.getFileDir(i) == self.FILE_DIR):
				if (not self.XML.isCombo(i)):
					screen.addPullDownString("SelectedFile", self.XML.getFileTag(i), i, i, i == self.FILE)
	
	# the only place self.FILE can be changed. This is useful in case more code needs to be added to changing file
	def changeFileWithoutStack(self, fileID):
		if (fileID == self.FILE):
			return
		# update the file
		self.FILE = fileID
		self.XML.setActiveFile(fileID)
		# update the dir if needed
		currentDir = self.XML.getFileDir(fileID)
		if (self.FILE_DIR != currentDir):
			self.FILE_DIR = currentDir
			self.drawDirSelector()
		#redraw the file selector and types
		self.drawFileSelector()
		self.drawTypes()
		self.closeAnyPopup()
	
	def changeFile(self, fileID):
		if (fileID == self.FILE):
			return
		self.changeFileWithoutStack(fileID)
		self.fileStack.insert(0, fileID)
		self.fileStackForward = []
		self.fileStackType.insert(0, None)
		self.fileStackTypeForward = []
		
	
	# jump to a file and then open Type
	def jumpToType(self, fileID, Type):
		self.changeFile(fileID)
		self.fileStackType[0] = Type
		self.clickElement(Type)
		
	def jumpToTypeWithoutStack(self, fileID, Type):
		self.changeFileWithoutStack(fileID)
		self.clickElement(Type)

	def clickElement(self, Type):
		if (Type == None):
			return
		for iRow in range(len(self.columnContainers[0].elements)):
			element = self.columnContainers[0].elements[iRow]
			if (element.getChildType() == Type):
				self.setNumColumns(1)
				self.columnContainers.append(xmlColumnContainer.columnContainer(1, element, self, self.columnContainers[0].getRight(), self.columnContainers[0].getOffset(iRow)))
				self.columnContainers[0].clickElement(iRow, True)
				return

	def jumpPrevFile(self):
		if (len(self.fileStack) > 1):
			fileID = self.fileStack.pop(0)
			self.fileStackForward.insert(0, fileID)
			fileType = self.fileStackType.pop(0)
			self.fileStackTypeForward.insert(0, fileType)
			self.jumpToTypeWithoutStack(self.fileStack[0], self.fileStackType[0])
	
	def jumpNextFile(self):
		if (len(self.fileStackForward) > 0):
			fileID = self.fileStackForward.pop(0)
			self.fileStack.insert(0, fileID)
			fileType = self.fileStackTypeForward.pop(0)
			self.fileStackType.insert(0, fileType)
			self.jumpToTypeWithoutStack(self.fileStack[0], self.fileStackType[0])
	
	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		
		# don't bother with mouse movements. They are frequent and will only clutter up debug output
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CURSOR_MOVE_OFF or inputClass.getNotifyCode() == NotifyCode.NOTIFY_CURSOR_MOVE_ON):
			return
		
		# first only react to key down events, not key up events
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CHARACTER and inputClass.getID() == 0):
			return
		
		input = xmlInput.xmlInput(inputClass, self)
		
		if self.currentPopup != None:
			bInputConsumed = self.currentPopup.input(input)
			if bInputConsumed:
				return 1
			if input.isCancel():
				self.closePopupWindow()
				return 1
			if inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED:
				# for the time being, disallow clicking on anything other than the popup windows
				# clicking on something else could result in undefined (buggy) behavior
				return 1
		
		rightClick = inputClass.getFlags() == self.inputRightClick
	
		if inputClass.getFunctionName() == "SelectedDir":
			id = screen.getPullDownData("SelectedDir", screen.getSelectedPullDownID("SelectedDir"))
			self.FILE_DIR = self.XML.getXMLDir(id)
			# redraw the dir selector as this forces the file selector to be redrawn to the new dir
			self.drawDirSelector()
		elif inputClass.getFunctionName() == "SelectedFile":
			self.changeFile(screen.getPullDownData("SelectedFile", screen.getSelectedPullDownID("SelectedFile")))
		elif (self.currentScreen == self.popupGameFont):
			self.popupGameFontInput(inputClass)
		elif (self.currentScreen == self.popupTxtKey):
			self.popupTxtKeyInput(inputClass)
		elif (self.currentScreen == self.popupEditor):
			self.popupEditorInput(inputClass)
		elif (self.currentScreen == self.popupHelp):
			self.popupHelpInput(inputClass)
		elif (self.currentScreen == self.popupReload):
			self.popupReloadInput(inputClass)
		elif (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if (inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL or inputClass.getButtonType() == self.widgetDrag):
				iColumn = inputClass.getData1()
				iRow = inputClass.getData2()
				if (iColumn == self.specialColumn):
					if (iRow == self.help_button):
						self.popupHelpCreate()
					elif (iRow == self.reload_button):
						self.popupReloadCreate()
					elif (iRow == self.menu_button):
						self.doCommand("Menu")
					elif (iRow == self.error_button):
						self.doCommand("FindError")
				elif (iColumn >= 0 and iRow >= 0 and iColumn < len(self.columnContainers)):
					element = self.columnContainers[iColumn].clickElement(iRow, not rightClick)
					self.selectedColumn = iColumn
					if (rightClick):
						self.popupEditorCreate(element)
					elif (element.isDir()):
						self.fileStackType[0] = element.getType()
						self.setNumColumns(iColumn+1)
						self.columnContainers.append(xmlColumnContainer.columnContainer(iColumn+1, element, self, self.columnContainers[iColumn].getRight(), self.columnContainers[iColumn].getOffset(iRow)))
					else:
						self.handleTagClicked(element, inputClass)
			elif (inputClass.getButtonType() == self.widgetDelete or inputClass.getButtonType() == self.widgetClone):
				iColumn = inputClass.getData1()
				iRow = inputClass.getData2()
				self.selectedColumn = iColumn
				element = self.columnContainers[iColumn].clickElement(iRow, False)
				if inputClass.getButtonType() == self.widgetDelete:
					self.openPopupWindow("ElementDelete", element, iColumn)
				elif inputClass.getButtonType() == self.widgetClone:
					element.clone()
					self.redrawColumns()
			elif (inputClass.getButtonType() == self.widgetUp or inputClass.getButtonType() == self.widgetDown):
				self.handleArrowClicked(inputClass)
		elif (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CHARACTER):
			self.handleKeyPressed(inputClass)
				
		
		return 1

	def handleKeyPressed(self, inputClass):
		
		#escape
		if (inputClass.getData() == InputTypes.KB_ESCAPE):
			self.doCommand("Quit")
		#left arrow
		elif (inputClass.getData() == InputTypes.KB_LEFT):
			self.doCommand("FilePrev")
		#right arrow
		elif (inputClass.getData() == InputTypes.KB_RIGHT):
			self.doCommand("FileNext")
		#F1
		elif (inputClass.getData() == InputTypes.KB_F1):
			self.doCommand("HideUnallocated")
		#F5
		elif (inputClass.getData() == InputTypes.KB_F5):
			self.doCommand("FindError")
		
	def handleTagClicked(self, element, inputClass):
		iColumn = inputClass.getData1()
		iRow    = inputClass.getData2()
	
		if (element.isBool()):
			if (element.getBoolValue()):
				element.setValue("0")
			else:
				element.setValue("1")
			self.redrawColumns()
		else:
			strClass = element.getInfoClass()
			if strClass == "GameFont":
				self.popupGameFontCreate(element)
			elif strClass == "Type":
				# jump to file if control is down
				if (self.XML.ctrlKey()):
					fileID = element.getInfoType()
					if (fileID >= 0):
						if (element.isAllocated()):
							self.jumpToType(fileID, element.getValue())
						else:
							self.changeFile(fileID)
						return
				self.openPopupWindow("ElementType", element)
			elif strClass == "TxtKey":
				self.popupTxtKeyCreate(element)
			else:
				self.openPopupWindow("ElementString", element)
		
	def handleArrowClicked(self, inputClass):
		iColumn = inputClass.getData1()
		iRow = inputClass.getData2()
		element = self.columnContainers[iColumn].clickElement(iRow, False)
		
		iValue = inputClass.getButtonType() - self.widgetZero
		if (element.isAllocated()):
			iValue += int(element.getValue())
		element.setValue(str(iValue))
		self.redrawColumns()
		
	def closeAnyPopup(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		# close all possible popup widgets
		screen.deleteWidget("xmlEditorSmallBG")
		screen.deleteWidget("xmlEditorSmallHeader")
		screen.deleteWidget("stringInputBox")
		screen.deleteWidget("helpText")
		screen.deleteWidget("xmlEditorTypeDirSelector")
		screen.deleteWidget("xmlEditorTypeSelector")
		screen.deleteWidget("xmlEditorTypeCheckBox")
		screen.deleteWidget("xmlEditorTypeCheckBox3")
		screen.deleteWidget("xmlEditorTypeCheckBox4")
		screen.deleteWidget("xmlEditorTypeCheckBoxSub1")
		screen.deleteWidget("xmlEditorTypeCheckBoxSub11")
		screen.deleteWidget("xmlEditorTypeCheckBoxSub12")
		screen.deleteWidget("xmlEditorFileSpecificCheckBox")
		screen.deleteWidget("table")
		screen.deleteWidget("stringSelectBox0")
		screen.deleteWidget("stringSelectBox1")
		screen.deleteWidget("stringSelectBox2")
		screen.deleteWidget("stringSelectBox3")
		screen.deleteWidget("stringInputBox0")
		screen.deleteWidget("stringInputBox1")
		screen.deleteWidget("stringInputBox2")
		screen.deleteWidget("stringInputBox3")
		self.selectedElement = None
		self.selectedColumn = -1
		self.currentScreen = self.popupNone
		if self.currentPopup != None:
			self.currentPopup.delete()
			self.currentPopup = None

	def handleDrag(self, source1, source2, dest1, dest2):

		if (source1 == dest1):
			if (source2 != dest2 and source2 != (dest2-1)):
				ObjectSource = self.columnContainers[source1].clickElement(source2, False)
				ObjectDest = self.columnContainers[dest1].clickElement(dest2, False)
				
				# move the object
				ObjectSource.dragTo(ObjectDest)
				
				# set an update flag
				# for some reason the game crashes if the update is done in this function
				# set the flag to update screen in update
				self.setColumnValue = source1
		elif dest1 == -10 and dest2 == -10:
			ObjectSource = self.columnContainers[source1].clickElement(source2, False)
			ObjectSource.clone()
			self.setColumnValue = source1
		
		
	def drawTypes(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		
		# goal: make a list of types for the file (not subtypes)
		# approach:
		# ask DLL for xml pointer to the first item
		# add item's type to the list
		# add pointer to array
		# ask DLL to get next sibling
		# menu list type is equal to array index
		# in handleInput, getting the pointer object is just a matter of
		#  reading the array element at index "selected menu type"
		
		self.setNumColumns(0)
		item = self.XML.getList()
		self.columnContainers.append(xmlColumnContainer.columnContainer(0, item, self, 0, 0))
		
		
	def update(self, fDelta):
		if (self.setColumnValue != -1):
			self.setNumColumns(self.setColumnValue+1)
			self.columnContainers[self.setColumnValue].drawContents()
			self.setColumnValue = -1
			
	def getWidgetHelp(self, argsList):
		iScreen, eWidgetType, iData1, iData2, bOption = argsList
		
		# display help text
		# this is called at widget creation. It seems to also be called on mouse over
		# mouseover calls appears to be disabled if creation call returns nothing

		# ask the popup for a help text
		if self.currentPopup != None:
			helpText = self.currentPopup.getHelp(eWidgetType, iData1, iData2, bOption)
			if helpText != None:
				return localText.getText(helpText, ())

		if eWidgetType == WidgetTypes.WIDGET_GENERAL or eWidgetType == self.widgetDrag:
			if (iData1 >= 0 and iData1 < len(self.columnContainers)):
				element = self.columnContainers[iData1].clickElement(iData2, False)
				helpText = element.getHelp()
				if (helpText != None):
					return localText.getText(helpText, ())
			if iData1 == self.specialColumn:
				helpText = None
				
				if iData2 == self.string_type_button:
					helpText = "String is a Type from an XML file or from GlobalTypes"
				elif iData2 == self.string_type_none_button:
					helpText = "The Type NONE is a valid setting for this tag"
				elif iData2 == self.string_type_remote_button:
					helpText = "Add a button to create an entry in the file this tag points to.[NEWLINE]Useful when big entries needs small entries in other files, like say create art and unitclass for a unit.[NEWLINE][NEWLINE]Not compatible with GlobalTypes"
				elif iData2 == self.string_type_prefix_button:
					helpText = "Use full string when creating the remote Type.[NEWLINE][NEWLINE]Example: say we have UNIT_WARRIOR, normally it will remote the prefix UNIT_ and create something like UNITCLASS_WARRIOR. Using this checkbox will use the full string and create UNITCLASS_UNIT_WARRIOR.[NEWLINE][NEWLINE]Rarely needed."
				elif iData2 == self.string_button_button:
					helpText = "The editor will display the string as button art when this option is used"
				elif iData2 == self.int_gamefont_button:
					helpText = "Use the GameFont char selector"
				elif iData2 == self.dir_art_button:
					helpText = "Selects which child tag to use for button art.[NEWLINE]If more than one entry exist of the tag name in question, the first is used.[NEWLINE]Selecting another dir tag will copy the art that one reads from a child, meaning chains are possible.[NEWLINE]Type selector strings will copy the button art from the entry it points to.[NEWLINE]Button strings provides the contents of the button in question.[NEWLINE]Complex chasins across multiple files are possible, but be careful of loops."
				elif iData2 == self.remote_create_button:
					helpText = "Create a new entry to match the current entry"
				elif iData2 == self.file_specific_button:
					helpText = "Settings apply only to the specific file rather than all tags of this name in all files.[NEWLINE]Used to deal with nameclashes where settings aren't the same in all files. An example would be the generic name \"Class\""
				elif iData2 >= self.txt_key_select and iData2 < (self.txt_key_select + 4):
					helpText = "The keyboard will only write in the selected string"
				elif iData2 == self.error_button:
					helpText = "Locates missing tags, which are mandatory in the schema file[NEWLINE][NEWLINE]No output is produced. Instead the tag with the error is displayed in the regular screen. It might take a bit of scrolling to find it, but it's usually in the bottom right.[NEWLINE]If no errors are found, all columns are closed with the exception of the far left."

				if helpText != None:
					return localText.getText(helpText, ())
			elif iData1 == -10 and iData2 == -10:
				helpText = "Drag list entries to this button to create a clone of it."
				return localText.getText(helpText, ())
		elif (eWidgetType == self.widgetDelete):
			return u"Delete"
		elif (eWidgetType == self.widgetClone):
			return u"Clone"
			
	def popupGameFontCreate(self, element):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		
		self.currentScreen = self.popupGameFont
		
		self.selectedElement = element

		iStart = 8483 # this number is hardcoded as start in the exe
		iEnd   = self.XML.getMaxGameFontID()
		iDiff = iEnd - iStart + 1
		
		spacing_horizontal = 22
		spacing_vertical = 25
		num_icons_on_line = 32
		
		
		nScreenWidth = (num_icons_on_line + 1)*spacing_horizontal + 7
		nScreenHeight = 200 + (4+spacing_vertical)*int(iDiff//num_icons_on_line)
		
		left = (screen.getXResolution()/2) - (nScreenWidth/2)
		top  = (screen.getYResolution()/2) - (nScreenHeight/2)
		
		screen.addPanel( "xmlEditorSmallBG", u"", u"", True, False, left, top, nScreenWidth, nScreenHeight, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setLabel("xmlEditorSmallHeader", "xmlEditorSmallBG", u"<font=4b>" + element.getName() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, left + nScreenWidth / 2, top + 18, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		iID = iStart
		iTopOffset = 40
		iNextBreak = self.XML.getNextGameFontGroupStart(iStart)
		while (iID <= iEnd):
			iTopOffset += spacing_vertical
			for i in range(num_icons_on_line):
				iButtonID = iID
				screen.setTextAt("gamefont" + str(iID), "xmlEditorSmallBG", u"<font=4u>" + (u" %c" %  (iID)) + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, 7 + (spacing_horizontal*i),  iTopOffset, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, self.specialColumn, iButtonID)
				iID += 1
				if (iID == iNextBreak):
					iNextBreak = self.XML.getNextGameFontGroupStart(iNextBreak)
					break
		
		screen.setTextAt("cancel_button", "xmlEditorSmallBG", u"<font=4u>" + "CANCEL" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, 3*(nScreenWidth / 4),  nScreenHeight - 60, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, self.specialColumn, -1)
		screen.setTextAt("ok_button", "xmlEditorSmallBG", u"<font=4u>" + "USE AUTOASSIGNED" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth / 4,  nScreenHeight - 60, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, self.specialColumn, 0)
		
	def popupGameFontInput(self, inputClass):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )

		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if (inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL):
				iColumn = inputClass.getData1()
				iButton = inputClass.getData2()
				if (iColumn == self.specialColumn):
					if (iButton != -1):
						self.selectedElement.setGameFontChar(iButton)
						self.columnContainers[self.selectedColumn].drawContents()
					self.closeAnyPopup()
	
	def popupTxtKeyCreate(self, element):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		
		self.currentScreen = self.popupTxtKey
		
		if (self.selectedElement != None):
			self.popupStringDelete(self.cancel_button)
		
		self.selectedElement = element
		
		textString = element.getTextString()
		
		nScreenWidth = 460
		nScreenHeight = 500
		
		left = (screen.getXResolution()/2) - (nScreenWidth/2)
		top  = (screen.getYResolution()/2) - (nScreenHeight/2)
		
		screen.addPanel( "xmlEditorSmallBG", u"", u"", True, False, left, top, nScreenWidth, nScreenHeight, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setLabel("xmlEditorSmallHeader", "xmlEditorSmallBG", u"<font=4b>" + element.getName() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, left + nScreenWidth / 2, top + 18, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		for i in range(4):
			iTopOffset = i * 60
			
			# use an empty string if the xml element doesn't contain any text
			strName = ""
			if textString != None:
				strName = textString.getNameFromIndex(i)
				
			screen.setTextAt("string_identifier_" + str(i), "xmlEditorSmallBG", strName, CvUtil.FONT_LEFT_JUSTIFY, 20,  iTopOffset + 60, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.cancel_button)
			screen.addCheckBoxGFC("stringSelectBox" + str(i), "", "Art/Interface/Buttons/Button_SELECT.dds", left + 25, top + 75 + iTopOffset, 30, 30, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.txt_key_select + i, ButtonStyles.BUTTON_STYLE_SQUARE)
			
			textBoxString = "stringInputBox" + str(i)
			
			screen.addEditBoxGFC(textBoxString, left + 60, top +  70 + iTopOffset, nScreenWidth - 70, 40, WidgetTypes.WIDGET_GENERAL, 0, 0, FontTypes.TITLE_FONT)
			screen.setEditBoxString (textBoxString, strName)
			screen.setEditBoxMaxCharCount(textBoxString, 10000, 10)
		
		screen.setState("stringSelectBox0", True)
		
		helpText = element.getHelp()
		if (helpText != None):
			if (len(helpText) > 0):
				helpText = str(localText.getText(helpText, ()))
			screen.addMultilineText("helpText", helpText, left + 10, top + 300, nScreenWidth - 20, nScreenHeight - 360, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		
		screen.setTextAt("cancel_button", "xmlEditorSmallBG", u"<font=4u>" + "CANCEL" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, 3*(nScreenWidth / 4),  nScreenHeight - 60, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.cancel_button)
		screen.setTextAt("ok_button", "xmlEditorSmallBG", u"<font=4u>" + "OK" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth / 4,  nScreenHeight - 60, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.OK_button)
		
	def popupTxtKeyDelete(self, bUpdate):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		
		if (bUpdate):
			strings = []
			for i in range(4):
				strings.append(str(screen.getEditBoxString("stringInputBox" + str(i))))
				
			self.selectedElement.setTxtKey(strings[0], strings[1], strings[2], strings[3])
			for i in range(len(self.columnContainers)):
				self.columnContainers[i].drawContents()
				
		self.closeAnyPopup()
		
	def popupTxtKeyInput(self, inputClass):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )

		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if (inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL):
				iColumn = inputClass.getData1()
				iButton = inputClass.getData2()
				if (iColumn == self.specialColumn):
					if (iButton == self.cancel_button or iButton == self.OK_button):
						self.popupTxtKeyDelete(iButton)
					elif (iButton >= self.txt_key_select and iButton < (self.txt_key_select + 4)):
						iButton = iButton - self.txt_key_select
						for i in range(4):
							screen.setState("stringSelectBox" + str(i), i == iButton)
				
				
		elif (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CHARACTER and inputClass.getData() == 44):
			# can't prevent the editor from dying, but it can save the string anyway when pressing enter
			self.popupTxtKeyDelete(True)			
		elif (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CHARACTER):
			iRow = 0
			for i in range(4):
				if screen.getCheckBoxState("stringSelectBox" + str(i)):
					iRow = i
			
			textBoxString = "stringInputBox" + str(iRow)
		
			string = screen.getEditBoxString(textBoxString)
			new_string = self.readKeyboard(inputClass, string, False)
			
			if inputClass.getData() == InputTypes.KB_UP or inputClass.getData() == InputTypes.KB_DOWN:
				if inputClass.getData() == InputTypes.KB_DOWN:
					iRow += 1
					if iRow == 4:
						iRow = 0
				else: # up
					iRow -= 1
					if iRow == -1:
						iRow = 3
				for i in range(4):
					screen.setState("stringSelectBox" + str(i), i == iRow)
			
			else:
				# no special (arrow) key pressed
				screen.setEditBoxString(textBoxString, new_string)
			
			return
	
				
	def popupEditorCreate(self, element):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		
		self.selectedElement = element
		self.currentScreen = self.popupEditor
		
		nScreenWidth = 500
		nScreenHeight = 500
		
		left = (screen.getXResolution()/2) - (nScreenWidth/2)
		top  = (screen.getYResolution()/2) - (nScreenHeight/2)
		
		screen.addPanel( "xmlEditorSmallBG", u"", u"", True, False, left, top, nScreenWidth, nScreenHeight, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setLabel("xmlEditorSmallHeader", "xmlEditorSmallBG", u"<font=4b>" + element.getName() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, left + nScreenWidth / 2, top + 18, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		top_offset = 50
		
		strClass = element.getInfoClass()
		
		if (element.isString()):
			# schema type is a string
		
			# checkbox
			screen.addCheckBoxGFC("xmlEditorTypeCheckBox", "", "Art/Interface/Buttons/Button_SELECT.dds", left + 25, top + top_offset, 30, 30, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.string_type_button, ButtonStyles.BUTTON_STYLE_SQUARE)
			
			typeID = element.getInfoType()
			selected_dir = self.FILE_DIR
			if (typeID >= 0):
				screen.setState("xmlEditorTypeCheckBox", True)
				selected_dir = self.XML.getFileDir(typeID)
			
			# dropdown menu
			screen.addDropDownBoxGFC("xmlEditorTypeDirSelector", left + 70, top + top_offset, 150, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.TITLE_FONT)
			for i in range(self.XML.getNumXMLDir()):
				name = self.XML.getXMLDir(i)
				screen.addPullDownString("xmlEditorTypeDirSelector", name, i, i, name == selected_dir)
				
			
			self.fileSelectorX = left + 220
			self.fileSelectorY = top + top_offset

			self.popupDrawFileSelector(typeID)
			
			screen.addCheckBoxGFC("xmlEditorTypeCheckBoxSub1", "", "Art/Interface/Buttons/Button_SELECT.dds", left + 70, top + top_offset + 35, 30, 30, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.string_type_none_button, ButtonStyles.BUTTON_STYLE_SQUARE)
			if (element.allowsTypeNone()):
				screen.setState("xmlEditorTypeCheckBoxSub1", True)
			screen.setTextAt("allow_type_none_text", "xmlEditorSmallBG", u"<font=3>" + "Allow Type NONE" + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, 100, top_offset + 45, -0.1, FontTypes.MENU_FONT, WidgetTypes.WIDGET_GENERAL, self.specialColumn, -1)
			
			#if element.getParentType() != None:
			if False:
				screen.addCheckBoxGFC("xmlEditorTypeCheckBoxSub11", "", "Art/Interface/Buttons/Button_SELECT.dds", left + 240, top + top_offset + 35, 30, 30, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.string_type_remote_button, ButtonStyles.BUTTON_STYLE_SQUARE)
				screen.setTextAt("remote_create", "xmlEditorSmallBG", u"<font=3>" + "Allow remove create" + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, 270, top_offset + 45, -0.1, FontTypes.MENU_FONT, WidgetTypes.WIDGET_GENERAL, self.specialColumn, -1)
				if element.isRemoteCreate():
					screen.setState("xmlEditorTypeCheckBoxSub11", True)
			
				top_offset += 35
				screen.addCheckBoxGFC("xmlEditorTypeCheckBoxSub12", "", "Art/Interface/Buttons/Button_SELECT.dds", left + 240, top + top_offset + 35, 30, 30, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.string_type_prefix_button, ButtonStyles.BUTTON_STYLE_SQUARE)
				screen.setTextAt("remote_create_prefix", "xmlEditorSmallBG", u"<font=3>" + "Use prefix" + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, 270, top_offset + 45, -0.1, FontTypes.MENU_FONT, WidgetTypes.WIDGET_GENERAL, self.specialColumn, -1)
				if element.isRemoteCreatePrefix():
					screen.setState("xmlEditorTypeCheckBoxSub12", True)
			
			top_offset += 70
			
			screen.addCheckBoxGFC("xmlEditorTypeCheckBox3", "", "Art/Interface/Buttons/Button_SELECT.dds", left + 25, top + top_offset, 30, 30, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.string_button_button, ButtonStyles.BUTTON_STYLE_SQUARE)
			screen.setTextAt("button_text", "xmlEditorSmallBG", u"<font=4u>" + "Button" + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, 70, top_offset + 15, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, self.specialColumn, -1)
			if (strClass != None and strClass == "Button"):
				screen.setState("xmlEditorTypeCheckBox3", True)
			
			top_offset += 40
			
		if (element.isInt()):
			screen.addCheckBoxGFC("xmlEditorTypeCheckBox", "", "Art/Interface/Buttons/Button_SELECT.dds", left + 25, top + top_offset, 30, 30, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.int_gamefont_button, ButtonStyles.BUTTON_STYLE_SQUARE)
			screen.setTextAt("gamefont_text", "xmlEditorSmallBG", u"<font=4u>" + "GameFont" + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, 70, top_offset + 15, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, self.specialColumn, -1)
			if (strClass != None and strClass == "GameFont"):
				screen.setState("xmlEditorTypeCheckBox", True)
			
			top_offset += 40
		
		if (element.isDir()):
			
			selected_child = element.getButtonArtChild()
			screen.addCheckBoxGFC("xmlEditorTypeCheckBox", "", "Art/Interface/Buttons/Button_SELECT.dds", left + 25, top + top_offset, 30, 30, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.dir_art_button, ButtonStyles.BUTTON_STYLE_SQUARE)
			if (selected_child != None):
				screen.setState("xmlEditorTypeCheckBox", True)
			else:
				selected_child = "Button"
			
			screen.addDropDownBoxGFC("xmlEditorTypeDirSelector", left + 70, top + top_offset, nScreenWidth - 70 - 20, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.dir_art_button, FontTypes.TITLE_FONT)
			for i in range(element.getNumSchemaChildren()):
				name = element.getSchemaChild(i)
				screen.addPullDownString("xmlEditorTypeDirSelector", name, i, i, name == selected_child)
			top_offset += 40
			
		screen.addEditBoxGFC("stringInputBox", left + 10, top + nScreenHeight - 120, nScreenWidth - 20, 40, WidgetTypes.WIDGET_GENERAL, 0, 0, FontTypes.TITLE_FONT)
		screen.setEditBoxMaxCharCount("stringInputBox", 10000, 10)
		helpText = element.getHelp()
		if (helpText != None):
			screen.setEditBoxString ("stringInputBox", helpText)
		else:
			helpText = ""
			
		self.helptTextLeft = left + 10
		self.helptTextTop = top + top_offset
		self.helptTextWidth = nScreenWidth - 20
		self.helptTextHeight = nScreenHeight - 120 - top_offset
		
		self.popupEditorSetHelpText(helpText)
		
		#screen.addCheckBoxGFC("xmlEditorFileSpecificCheckBox", "", "Art/Interface/Buttons/Button_SELECT.dds", left + 25, nScreenHeight + top - 160, 30, 30, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.file_specific_button, ButtonStyles.BUTTON_STYLE_SQUARE)
		#screen.setTextAt("FileSpecificCheckBoxText", "xmlEditorSmallBG", u"<font=4u>" + "File Specific" + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, 58,  nScreenHeight - 150, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.file_specific_button)
		
		screen.setTextAt("cancel_button", "xmlEditorSmallBG", u"<font=4u>" + "CANCEL" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, 3*(nScreenWidth / 4),  nScreenHeight - 60, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.cancel_button)
		screen.setTextAt("ok_button", "xmlEditorSmallBG", u"<font=4u>" + "OK" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth / 4,  nScreenHeight - 60, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.OK_button)
	
	def popupDrawFileSelector(self, typeID):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		
		if self.selectedElement.isDir():
			screen.setState("xmlEditorTypeCheckBox", True)
			return
		
		dir = screen.getPullDownData("xmlEditorTypeDirSelector", screen.getSelectedPullDownID("xmlEditorTypeDirSelector"))
		dir = self.XML.getXMLDir(dir)
		
		screen.addDropDownBoxGFC("xmlEditorTypeSelector", self.fileSelectorX, self.fileSelectorY, 250, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.TITLE_FONT)
		for i in range(self.XML.getNumFiles()):
			if (self.XML.getNumTypes(i) > 0): # skip files without Type as they will never be used
				if (self.XML.getFileDir(i) == dir):
					screen.addPullDownString("xmlEditorTypeSelector", self.XML.getFileTag(i), i, i, i == typeID)
			
	
	def popupEditorSetHelpText(self, helpText):
		# function to (re)draw the help text
		# using a function ensures it can be called from multiple places and use the same screen location each time
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		if (len(helpText) > 0):
			helpText = str(localText.getText(helpText, ()))
		screen.addMultilineText("helpText", helpText, self.helptTextLeft, self.helptTextTop, self.helptTextWidth, self.helptTextHeight, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		
	def popupEditorDelete(self, bUpdate):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		
		if (bUpdate):
			element = self.selectedElement
			
			newID = -1
			helpText = str(screen.getEditBoxString("stringInputBox"))
			newClass = None
			bAllowTypeNone = False
			ButtonChild = None
			bRemoteCreate = False
			bRemoteCreatePrefix = False
			bFileSpecific = screen.getCheckBoxState("xmlEditorFileSpecificCheckBox")
			
			if (element.isString()):
				state = screen.getCheckBoxState("xmlEditorTypeCheckBox")
				if (state):
					newClass = "Type"
					newID = screen.getPullDownData("xmlEditorTypeSelector", screen.getSelectedPullDownID("xmlEditorTypeSelector"))
					bAllowTypeNone = screen.getCheckBoxState("xmlEditorTypeCheckBoxSub1")
					bRemoteCreate = screen.getCheckBoxState("xmlEditorTypeCheckBoxSub11")
					bRemoteCreatePrefix = screen.getCheckBoxState("xmlEditorTypeCheckBoxSub12")	
				state = screen.getCheckBoxState("xmlEditorTypeCheckBox3")
				if (state):
					newClass = "Button"
			
			if (element.isInt()):
				state = screen.getCheckBoxState("xmlEditorTypeCheckBox")
				if (state):
					newClass = "GameFont"
			if (element.isDir()):
				state = screen.getCheckBoxState("xmlEditorTypeCheckBox")
				if (state):
					iSelected = screen.getPullDownData("xmlEditorTypeDirSelector", screen.getSelectedPullDownID("xmlEditorTypeDirSelector"))
					ButtonChild = element.getSchemaChild(iSelected)
				#	if ButtonChild == "Button":
						# default value. Avoid setting this
				#		ButtonChild = None
					
			# write the new data to file
			element.setInfo(bFileSpecific, newID, helpText, newClass, bAllowTypeNone, bRemoteCreate, bRemoteCreatePrefix, ButtonChild)
			
			# update display to make the change take effect right away
			self.redrawColumns()
			
		self.closeAnyPopup()
	
	def popupEditorInput(self, inputClass):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		
		if inputClass.getFunctionName() == "xmlEditorTypeSelector":
			self.popupEditorLineSelect(self.string_type_button)
		elif inputClass.getFunctionName() == "xmlEditorTypeDirSelector":
			self.popupDrawFileSelector(-1)
			self.popupEditorLineSelect(self.string_type_button)
		elif (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CHARACTER):
			string = screen.getEditBoxString("stringInputBox")
			string = self.readKeyboard(inputClass, string)
			screen.setEditBoxString("stringInputBox", string)
			self.popupEditorSetHelpText(str(string))
		elif (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if (inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL):
				iColumn = inputClass.getData1()
				iButton = inputClass.getData2()
				if (iColumn == self.specialColumn):
					if (iButton == self.cancel_button or iButton == self.OK_button):
						self.popupEditorDelete(iButton)
					else:
						self.popupEditorLineSelect(iButton)
	
	def popupEditorLineSelect(self, line):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		
		if (line == self.file_specific_button):
			# touching the file specific button should not affect any other buttons
			return
		
		if line == self.string_type_none_button or line == self.string_type_remote_button or line == self.string_type_prefix_button:
			line = self.string_type_button
			screen.setState("xmlEditorTypeCheckBox", True)
		
		if (line != self.string_type_button and line != self.int_gamefont_button and line != self.dir_art_button):
			screen.setState("xmlEditorTypeCheckBox", False)
		if (line != self.string_button_button):
			screen.setState("xmlEditorTypeCheckBox3", False)
		if (line != 4):
			screen.setState("xmlEditorTypeCheckBox4", False)
				
	def popupHelpCreate(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		
		self.currentScreen = self.popupHelp
		
		nScreenWidth = 700
		nScreenHeight = 480
		
		left = (screen.getXResolution()/2) - (nScreenWidth/2)
		top  = (screen.getYResolution()/2) - (nScreenHeight/2)
		
		screen.addPanel( "xmlEditorSmallBG", u"", u"", True, False, left, top, nScreenWidth, nScreenHeight, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setLabel("xmlEditorSmallHeader", "xmlEditorSmallBG", u"<font=4b>" + "HELP" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, left + nScreenWidth / 2, top + 18, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		screen.addPanel("A", u"", u"", False, False, left + 20,  top +  50, 400, 50, PanelStyles.PANEL_STYLE_IN        , WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addPanel("B", u"", u"", False, False, left + 20,  top + 100, 400, 50, PanelStyles.PANEL_STYLE_CITY_TOP  , WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addPanel("C", u"", u"", False, False, left + 20,  top + 150, 400, 50, PanelStyles.PANEL_STYLE_DAWNBOTTOM, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		screen.setTextAt("AA", "xmlEditorSmallBG", u"<font=3>" + "Present in XML" + u"</font>"              , CvUtil.FONT_LEFT_JUSTIFY, 450,   70, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setTextAt("AB", "xmlEditorSmallBG", u"<font=3>" + "Not present in XML" + u"</font>"          , CvUtil.FONT_LEFT_JUSTIFY, 450,  120, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setTextAt("AC", "xmlEditorSmallBG", u"<font=3>" + "Mandatory tag missing in XML" + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, 450,  170, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		screen.setTextAt("BA", "xmlEditorSmallBG", u"<font=4u>" + "Pressing escape or enter kills the editor" + u"</font>"              , CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth/2,   220, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setTextAt("BB", "xmlEditorSmallBG", u"<font=4u>" + "Left click on a box to edit" + u"</font>"              , CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth/2,   270, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setTextAt("BC", "xmlEditorSmallBG", u"<font=4u>" + "Right click on a box to assign the file from which Types are fetched" + u"</font>"              , CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth/2,   320, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		
		
		
		
		screen.setTextAt("ok_button", "xmlEditorSmallBG", u"<font=4u>" + "OK" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth / 2,  nScreenHeight - 60, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.OK_button)
		
	
	def popupHelpInput(self, inputClass):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if (inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL):
				iColumn = inputClass.getData1()
				iButton = inputClass.getData2()
				if (iColumn == self.specialColumn):
					if (iButton == self.OK_button):
						screen.deleteWidget("A")
						screen.deleteWidget("B")
						screen.deleteWidget("C")
						self.closeAnyPopup()
						
	def popupReloadCreate(self):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		
		self.currentScreen = self.popupReload
		
		nScreenWidth = 400
		nScreenHeight = 300
		
		left = (screen.getXResolution()/2) - (nScreenWidth/2)
		top  = (screen.getYResolution()/2) - (nScreenHeight/2)
		
		screen.addPanel( "xmlEditorSmallBG", u"", u"", True, False, left, top, nScreenWidth, nScreenHeight, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setLabel("xmlEditorSmallHeader", "xmlEditorSmallBG", u"<font=4b>" + "Reload XML" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, left + nScreenWidth / 2, top + 18, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			
		screen.setTextAt("AA", "xmlEditorSmallBG", u"<font=3>" + "Reloading without restarting the game" + u"</font>"              , CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth/2,   70, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setTextAt("AB", "xmlEditorSmallBG", u"<font=3>" + "This tend to corrupt game data and" + u"</font>"          , CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth/2,  120, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setTextAt("AC", "xmlEditorSmallBG", u"<font=3>" + "should only be used to view pedia" + u"</font>"          , CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth/2,  145, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setTextAt("AD", "xmlEditorSmallBG", u"<font=3>" + "Note: the editor freezes for seconds while reloading" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth/2,  180, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)	
		
		
		screen.setTextAt("cancel_button", "xmlEditorSmallBG", u"<font=4u>" + "CANCEL" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, 3*(nScreenWidth / 4),  nScreenHeight - 60, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.cancel_button)
		screen.setTextAt("ok_button", "xmlEditorSmallBG", u"<font=4u>" + "OK" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, nScreenWidth / 4,  nScreenHeight - 60, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, WidgetTypes.WIDGET_GENERAL, self.specialColumn, self.OK_button)
		
	
	def popupReloadInput(self, inputClass):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if (inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL):
				iColumn = inputClass.getData1()
				iButton = inputClass.getData2()
				if (iColumn == self.specialColumn):
					self.closeAnyPopup()
					if (iButton == self.OK_button):
						gc.reloadXML()
		
	def startFindingErrors(self):
		self.setNumColumns(1)
		self.findErrorLoop(0)
		
	def findErrorLoop(self, iColumn):
		# strategy for finding errors:
		# for each column, open all dirs
		# do this recursively
		# do not draw anything while looping as that will be way too slow
		# on error, stop all loops. Instead draw each column
		# the error is detected with containsBroken(), which is set when a columnContainer
		#   encounters at least one box, which should be drawn red, even if not drawing
		
		pColumnContainer = self.columnContainers[iColumn]
		if pColumnContainer.containsBroken():
			pColumnContainer.drawContents()
			return True
		
		for iElement in range(pColumnContainer.getNumElements()):
			element = pColumnContainer.clickElement(iElement, False)
			if element.isDir():
				pColumnContainer.clickElement(iElement, True, False)
				self.setNumColumns(iColumn+1)
				self.columnContainers.append(xmlColumnContainer.columnContainer(iColumn+1, element, self, self.columnContainers[iColumn].getRight(), self.columnContainers[iColumn].getOffset(iElement), False))
				bError = self.findErrorLoop(iColumn+1)
				if bError:
					pColumnContainer.drawContents(True)
					return True
		return False
		
	def updateColumn(self, iColumn):
		self.setNumColumns(iColumn+1)
		self.columnContainers[iColumn].drawContents()
	
	def setNumColumns(self, iNum):
		while (len(self.columnContainers) > iNum):
			columnObject = self.columnContainers.pop()
			columnObject.clear()
			
	def redrawColumns(self):
		for iColumn in range(len(self.columnContainers)):
			self.columnContainers[iColumn].drawContents()
	
	# read keycode and append/remove from a string
	def readKeyboard(self, inputClass, string, bReturnStringOnDublication=True):
		# read the string associated with the key in question 
		key = self.XML.getKey(inputClass.getData(), inputClass.isShiftKeyDown(), inputClass.isCtrlKeyDown(), inputClass.isAltKeyDown())
		
		# remove invalid pressed keys such as shift
		if key == None:
			return string
		
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
	
	def doCommand(self, command):
		element = self.XML.getCommandItem(command)
		
		if element.getText() == None:
			# no help means no confirm
			self.doCommandExecute(command)
		else:
			# open the confirm window
			self.openPopupWindow("CommandConfirm", element, self.getCommandID(command))

	def getCommandID(self, command):
		for i in range(len(self.commandList)):
			if command == self.commandList[i]:
				return i
		return -1
	
	def doCommandExecute(self, command):
		if   command == "WriteAllFiles":
			self.XML.writeAllFiles()
		elif command == "GenerateTextXML":
			self.XML.generateTextFiles()
		elif command == "FindError":
			self.startFindingErrors()
		elif command == "FileNext":
			self.jumpNextFile()
			return
		elif command == "FilePrev":
			self.jumpPrevFile()
			return
		elif command == "HideUnallocated":
			self.hideUnallocated = not self.hideUnallocated
			self.redrawColumns()
			return
		elif command == "CleanCurrentFile":
			self.XML.cleanActiveFile()
		elif command == "LoadArtAndGameFont":
			self.XML.copyFiles()

		# doubles as close window
		self.openPopupWindow(command)



	def closePopupWindow(self):
		self.closeAnyPopup()
		if self.currentPopup != None:
			self.currentPopup.delete()
			self.currentPopup = None
		
	# all popup windows should be opened by this function
	# the reason is that it allows deleting the old window (if any),
	# declare and init a new window class and then draw
	# If drawn from init, self.currentPopup will not be set and certain data will not be set
	# more specifically help text will not work
	
	# if called with an invalid window, all it will do is closing the current window
	def openPopupWindow(self, window, arg1 = None, arg2 = None, arg3 = None):
		self.closePopupWindow()

		if window == "CommandConfirm":
			self.currentPopup = xmlPopupCommandConfirm.xmlPopupCommandConfirm(self, arg1, arg2)
		elif window == "GameFont":
			self.currentPopup = xmlPopupGameFont.xmlPopupGameFont(self)
		elif window == "ElementDelete":
			self.currentPopup = xmlPopupElementDelete.xmlPopupElementDelete(self, arg1, arg2)
		elif window == "ElementString":
			self.currentPopup = xmlPopupElementString.xmlPopupElementString(self, arg1)
		elif window == "ElementType":
			self.currentPopup = xmlPopupElementType.xmlPopupElementType(self, arg1)
		elif window == "Keyboard":
			self.currentPopup = xmlPopupKeyboard.xmlPopupKeyboard(self)
		elif window == "Menu":
			self.currentPopup = xmlPopupMenu.xmlPopupMenu(self)
		elif window == "Quit":
			self.currentPopup = xmlPopupQuit.xmlPopupQuit(self)

		if self.currentPopup != None:
			self.currentPopup.draw()

	def generateCommandList(self):
		self.commandList = []
		self.commandList.append("WriteAllFiles")
		self.commandList.append("GameFont")
		self.commandList.append("Keyboard")
		self.commandList.append("GenerateTextXML")
		self.commandList.append("CleanCurrentFile")
		if (not self.XML.isEditorInMod()):
			self.commandList.append("LoadArtAndGameFont")
		# get the number of items intended for the menu popup
		self.menuItemCount = len(self.commandList)

		# add any command, which should not be in the menu below this line
		self.commandList.append("FindError")
		self.commandList.append("FileNext")
		self.commandList.append("FilePrev")
		self.commandList.append("Menu")
		self.commandList.append("Quit")
