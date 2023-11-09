from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import CvEventManager

class columnContainer:
	# arguments:
	#   columnID: 0 indexed count from left
	#   parent:   parent object, or in case of lists, the first object
	#   editor:   the editor object
	#     left:   the panel X offset to where to start drawing
	#   offset:   the panel Y offset to where to start drawing

	def __init__(self, columnID, parent, editor, left, offset, bDraw = True):
		self.ID = columnID
		self.parent = parent
		self.editor = editor
		self.left   = left
		self.offset = offset

		self.widgets = []
		self.elements = []

		self.selectedLine = -1
		self.width = 400
		self.height = 50

		self.bContainsBroken = False

		self.drawBackground(bDraw)

	def drawBackground(self, bDraw):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )
		height = self.editor.scrollHeight
		self.panel = "columnBG" + str(self.ID)
		self.tabel = "table" + str(self.ID)
		self.panel = self.editor.scrollPanel

		self.drawContents(bDraw)

	def drawContents(self, bDraw = True, pUpdatingElement = None):
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )

		if pUpdatingElement != None:
			updatingID = pUpdatingElement.getID()
			child = None
			for x in self.elements:
				if x.getID() == updatingID:
					child = x
					break

			counter = child.counter
			y_offset = child.y_offset
		else:
			updatingID = None
			self.clear()
			self.bContainsBroken = False

			self.elements = []

			child = self.parent.getFirstSchemaChild()
			y_offset = self.offset
			counter = 0

		while (child != None):
			panel = "column_" + str(self.ID) + "_" + str(counter) + "_"
			self.widgets.append(panel)

			iBackGroundOffset = 6

			# set background to tell if the tag is allocated or not
			panelBG = PanelStyles.PANEL_STYLE_CITY_TOP
			if (child.isAllocated()):
				panelBG = PanelStyles.PANEL_STYLE_IN
				iBackGroundOffset = 0
			elif (child.isMandatory()):
				panelBG = PanelStyles.PANEL_STYLE_DAWNBOTTOM
				self.bContainsBroken = True

			# if not drawing, build the list and continue loop
			if not bDraw:
				self.elements.append(child)
				child = child.next()
				continue

			if self.editor.hideUnallocated and panelBG == PanelStyles.PANEL_STYLE_CITY_TOP:
				child = child.next()
				continue
				
			strClass = child.getInfoClass()

			if strClass == None:
				panelBG = PanelStyles.PANEL_STYLE_STONE

			screen.attachPanelAt(self.panel, panel, "", "", False, False, panelBG, self.left, y_offset, self.width, self.height, WidgetTypes.WIDGET_GENERAL, self.ID, counter)

			size = 20

			if (child.isDir()):
				title = None
				ArtPath = None
				if (child.isAllocated()):
					title = child.getChildType()
					ArtPath = child.getButtonArt()
				if (title == None):
					title = child.getName()

				# set the default button art
				if (ArtPath == None and child.isListElement()):
					if (child.isAllocated()):
						ArtPath = self.editor.iconDefaultButton

				# how far from the left the text starts
				text_offset = 7 + iBackGroundOffset

				if (ArtPath != None):
					# draw a button and move text to the right

					if not child.isListElement() or not self.editor.XML.isColonizationExe():
						screen.setImageButtonAt(panel + "ButtonArt", panel, ArtPath, text_offset, (self.height/2) -25, 40, 40,  self.editor.widgetDrag, self.ID, counter )	
					else:
						screen.addDragableButtonAt(panel, panel + "ButtonArt", ArtPath, "", text_offset, (self.height/2) -25, 40, 40,  self.editor.widgetDrag, self.ID, counter, ButtonStyles.BUTTON_STYLE_IMAGE )	
					text_offset += 43

				title = "<font=3>" + title + "</font>"
				screen.setTextAt(panel + "name", panel, title, CvUtil.FONT_LEFT_JUSTIFY, text_offset, (self.height/2) - 5, -0.1, FontTypes.SMALL_FONT, self.editor.widgetDrag, self.ID, counter)

				iconString = self.editor.iconDirClosed
				if (counter == self.selectedLine):
					iconString = self.editor.iconDirOpen
					size = 40
				screen.setImageButtonAt(panel + "arrow", panel, iconString, self.width - 35 + iBackGroundOffset - (size/2), (self.height/2) - 5 - (size/2), size, size,  self.editor.widgetDrag, self.ID, counter )

			elif (child.isBool()):
				title = "<font=3>" + child.getName() + "</font>"
				screen.setTextAt(panel + "name", panel, title, CvUtil.FONT_LEFT_JUSTIFY, 7 + iBackGroundOffset, (self.height/2) - 5, -0.1, FontTypes.SMALL_FONT, self.editor.widgetDrag, self.ID, counter)
				iconString = self.editor.iconBoolOff
				if (child.getBoolValue()):
					iconString = self.editor.iconBoolOn

				size = 30
				screen.setImageButtonAt(panel + "icon", panel, iconString, self.width - 35 + iBackGroundOffset - (size/2), (self.height/2) - 5 - (size/2), size, size, self.editor.widgetDrag, self.ID, counter)

			elif (child.isInt()):
				title = "<font=3>" + child.getName() + "</font>"
				screen.setTextAt(panel + "name", panel, title, CvUtil.FONT_LEFT_JUSTIFY, 7 + iBackGroundOffset,
								 (self.height / 2) - 5, -0.1, FontTypes.SMALL_FONT, self.editor.widgetDrag, self.ID,
								 counter)

				strValue = "0"
				if (child.isAllocated() and child.getValue() != None):
					strValue = child.getValue()

				size = 40

				screen.setImageButtonAt(panel + "UpArrow", panel, self.editor.iconIntUpArrow,
										self.width - 15 + iBackGroundOffset - (size / 2),
										(self.height / 2) - 5 - (size / 2), size / 2, size / 2, self.editor.widgetUp,
										self.ID, counter)
				screen.setImageButtonAt(panel + "DownArrow", panel, self.editor.iconIntDownArrow,
										self.width - 15 + iBackGroundOffset - (size / 2), (self.height / 2) - 5,
										size / 2, size / 2, self.editor.widgetDown, self.ID, counter)

				screen.setTextAt(panel + "value", panel, strValue, CvUtil.FONT_RIGHT_JUSTIFY,
								 self.width - 18 + iBackGroundOffset - (size / 2), (self.height / 2) - 5, -0.1,
								 FontTypes.SMALL_FONT, self.editor.widgetDrag, self.ID, counter)

				# set a high number to reserve room for the int in case more icons are added
				size = 80

			elif (strClass != None and strClass != "TxtKey"):
				if (strClass == "Button"):
					size = 40
					title = "<font=3>" + child.getName() + "</font>"
					screen.setTextAt(panel + "name", panel, title, CvUtil.FONT_LEFT_JUSTIFY, 50 + iBackGroundOffset, 10, -0.1, FontTypes.SMALL_FONT, self.editor.widgetDrag, self.ID, counter)

					ArtPath = child.getButtonArt()

					#if (child.isAllocated() and child.getValue() != None):
					if ArtPath != None:
						title = "<font=3>" + ArtPath + "</font>"
						screen.setTextAt(panel + "value", panel, title, CvUtil.FONT_LEFT_JUSTIFY, 50 + iBackGroundOffset, 30, -0.1, FontTypes.SMALL_FONT, self.editor.widgetDrag, self.ID, counter)

					screen.setImageButtonAt(panel + "ButtonArt", panel, ArtPath, 7 + iBackGroundOffset, (self.height/2) - 5 - (size/2), size, size,  self.editor.widgetDrag, self.ID, counter )
					size = 0
				elif (strClass == "GameFont"):
					title = "<font=3>" + child.getName() + "</font>"
					screen.setTextAt(panel + "name", panel, title, CvUtil.FONT_LEFT_JUSTIFY, 40 + iBackGroundOffset, 10, -0.1, FontTypes.SMALL_FONT, self.editor.widgetDrag, self.ID, counter)

					if (child.isAllocated() and child.getValue() != None):
						title = "<font=3>" + child.getValue() + "</font>"
						screen.setTextAt(panel + "value", panel, title, CvUtil.FONT_LEFT_JUSTIFY, 40 + iBackGroundOffset, 30, -0.1, FontTypes.SMALL_FONT, self.editor.widgetDrag, self.ID, counter)

						iChar = child.getGameFontDisplayID()

						if (iChar != 0):
							screen.setTextAt(panel + "iCharBig", panel, u"<font=4u>" + (u" %c" %  (iChar)) + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, 18 + iBackGroundOffset,  10, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, self.editor.widgetDrag, self.ID, counter)
							screen.setTextAt(panel + "iCharSmall", panel, u"<font=2>" + (u" %c" %  (iChar)) + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, 18 + iBackGroundOffset,  30, -0.1, FontTypes.MENU_HIGHLIGHT_FONT, self.editor.widgetDrag, self.ID, counter)
					size = 0
				elif (strClass == "Type"):
					iXOffset = 7 + iBackGroundOffset
					if (child.isAllocated()):
						size = 40
						ArtPath = child.getButtonArt()
						if ArtPath != None:
							screen.setImageButtonAt(panel + "ButtonArt", panel, ArtPath, iXOffset, (self.height/2) - 5 - (size/2), size, size,  self.editor.widgetDrag, self.ID, counter )
							iXOffset += 50

					title = "<font=3>" + child.getName() + "</font>"
					screen.setTextAt(panel + "name", panel, title, CvUtil.FONT_LEFT_JUSTIFY, iXOffset, 10, -0.1, FontTypes.SMALL_FONT, self.editor.widgetDrag, self.ID, counter)

					if (child.isAllocated() and child.getValue() != None):
						title = "<font=3>" + child.getValue() + "</font>"
						screen.setTextAt(panel + "value", panel, title, CvUtil.FONT_LEFT_JUSTIFY, iXOffset, 30, -0.1, FontTypes.SMALL_FONT, self.editor.widgetDrag, self.ID, counter)
					size = 0

			elif child.isText():
				title = "<font=3>" + child.getName() + "</font>"
				screen.setTextAt(panel + "name", panel, title, CvUtil.FONT_LEFT_JUSTIFY, 7 + iBackGroundOffset, 10, -0.1, FontTypes.SMALL_FONT, self.editor.widgetDrag, self.ID, counter)
				title = "<font=3>" + child.getText() + "</font>"
				screen.setTextAt(panel + "value", panel, title, CvUtil.FONT_LEFT_JUSTIFY, 7 + iBackGroundOffset, 30, -0.1, FontTypes.SMALL_FONT, self.editor.widgetDrag, self.ID, counter)

			else:
				title = "<font=3>" + child.getName() + "</font>"
				screen.setTextAt(panel + "name", panel, title, CvUtil.FONT_LEFT_JUSTIFY, 7 + iBackGroundOffset, 10, -0.1, FontTypes.SMALL_FONT, self.editor.widgetDrag, self.ID, counter)

				if (child.isAllocated() and child.getValue() != None):
					title = "<font=3>" + child.getValue() + "</font>"
					screen.setTextAt(panel + "value", panel, title, CvUtil.FONT_LEFT_JUSTIFY, 7 + iBackGroundOffset, 30, -0.1, FontTypes.SMALL_FONT, self.editor.widgetDrag, self.ID, counter)

					if (strClass != None and strClass == "Button"):
						size = 40
						screen.setImageButtonAt(panel + "ButtonArt", panel, child.getValue(), self.width - 35 + iBackGroundOffset - (size/2), (self.height/2) - 5 - (size/2), size, size,  self.editor.widgetDrag, self.ID, counter )

			if (child.canDelete()):
				screen.setImageButtonAt(panel + "delete", panel, self.editor.iconDeleteElement, self.width - 40 - size - 20 + iBackGroundOffset, (self.height/2) - 25, 40, 40,  self.editor.widgetDelete, self.ID, counter )
				size += 45

			if (child.isAllocated() and child.isListElement()):
				screen.setImageButtonAt(panel + "clone", panel, self.editor.iconClone, self.width - 40 - size - 20 + iBackGroundOffset, (self.height/2) - 25, 40, 40,  self.editor.widgetClone, self.ID, counter )
				size += 45	

			if updatingID != None:
				return

			child.counter = counter
			child.y_offset = y_offset

			self.elements.append(child)

			counter += 1
			y_offset += self.height

			child = child.next()

	def clickElement(self, elementIndex, bUpdate, bDraw = True):
		if (bUpdate):
			self.selectedLine = elementIndex
			if bDraw:
				self.drawContents()
		return self.elements[elementIndex]

	def getNumElements(self):
		return len(self.elements)

	def containsBroken(self):
		return self.bContainsBroken

	def getRight(self):
		return self.width + self.left

	def getOffset(self, iIndex):
		return (iIndex * self.height) + self.offset

	def clear(self):
		# should be __exit__, but python is one version too old to understand this keyword
		screen = CyGInterfaceScreen( "xmlEditor", CvScreenEnums.XML_EDITOR )

		while (len(self.widgets) > 0):
			widget = self.widgets.pop()
			screen.deleteWidget(widget)
