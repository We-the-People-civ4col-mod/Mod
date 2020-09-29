## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums

# Helper class to create and fill tables for Domestic Advisor
# Most pages use mostly the same approach and this class is to simply and reuse code for setup.
# Hopefully this will make the code for the individual tables easier to read and less buggy.
# Nightinggale


# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class DomesticAdvisorTable:
	def __init__(self, parent, szName):
		self.parent = parent
		self.name = szName
		self.iWidth = parent.nTableWidth
		self.iWidthLeft = self.iWidth
		self.iRowHeight = self.parent.ROW_HIGHT
		self.columnWidth = []
		
	def show(self):
		self.getScreen().show( self.name )
	
	def hide(self):
		self.getScreen().hide( self.name )
		
	def isNeedInit(self):
		return len(self.columnWidth) == 0
		
	def enableSelect(self):
		self.getScreen().enableSelect( self.name, True )
		
	def enableSort(self):
		self.getScreen().enableSort( self.name )
		
	def tableInit(self):
		self.columnWidth = []
		self.getScreen().addTableControlGFC( self.name, 7, (self.parent.nScreenWidth - self.iWidth) / 2, 60, self.iWidth, self.parent.nTableHeight, True, False, self.parent.iCityButtonSize, self.parent.iCityButtonSize, TableStyles.TABLE_STYLE_STANDARD )
		self.getScreen().setStyle( self.name, "Table_StandardCiv_Style" )
		
	# note 0 means default height
	def setNumRows(self, iRows, iHeight = 0):
		if (iRows != self.getScreen().getTableNumRows(self.name)):
			self.getScreen().setTableNumRows ( self.name, iRows)
			self.setRowHeight(iHeight)
		
	def setRowHeight(self, iHeight = 0):
		if iHeight != 0:
			self.iRowHeight = iHeight
		for i in range(self.getScreen().getTableNumRows(self.name)):
			self.getScreen().setTableRowHeight(self.name, i, self.iRowHeight)
		
	def addHeaderDirect(self, iWidth, szName):
		self.getScreen().setTableColumnHeader( self.name, len(self.columnWidth), "<font=2>" + szName + "</font>", iWidth )
		self.columnWidth.append(iWidth)
		self.iWidthLeft -= iWidth
		
	def addHeaderTxt(self, szName, iWidth):
		self.addHeaderDirect(iWidth, localText.getText(szName, ()).upper())
		
	def addHeaderButton(self, szTitle = ""):
		self.addHeaderDirect(self.parent.ROW_HIGHT, szTitle)
		
	def addHeaderCityName(self, szName = "TXT_KEY_DOMESTIC_ADVISOR_NAME"):
		self.addHeaderTxt(szName, self.parent.CITY_NAME_COLUMN_WIDTH)
		
	def addHeaderChar(self, iChar, iWidth = 50):
		char = (u" %c" % iChar)
		self.addHeaderDirect(iWidth, char)

	def clearRows(self):
		# removes all rows and sets the counters to be ready to start on the first row
		self.curRow = -1
		self.curColumn = len(self.columnWidth)

	def getCellHeight(self):
		return self.iRowHeight
	
	def getCellWidth(self):
		return self.columnWidth[self.curColumn]

	def progressCell(self):
		self.curColumn += 1
		if self.curColumn >= len(self.columnWidth):
			self.curRow += 1
			self.curColumn = 0
			if self.getScreen().getTableNumRows(self.name) == self.curRow:
				self.getScreen().appendTableRow( self.name )
				self.getScreen().setTableRowHeight(self.name, self.curRow, self.parent.ROW_HIGHT)
	
	def addText(self, szText, iData1 = -1, iData2 = -1, widget = WidgetTypes.WIDGET_GENERAL, justified = CvUtil.FONT_LEFT_JUSTIFY):
		self.progressCell()
		self.getScreen().setTableText(self.name, self.curColumn, self.curRow, "<font=2>" + szText + "</font>", "", widget, iData1, iData2, justified )

	def addInt(self, iValue, iData1 = -1, iData2 = -1, widget = WidgetTypes.WIDGET_GENERAL, justified = CvUtil.FONT_RIGHT_JUSTIFY):
		self.progressCell()
		self.getScreen().setTableInt(self.name, self.curColumn, self.curRow, "<font=2>" + unicode(iValue) + "</font>", "", widget, iData1, iData2, justified )
		
	def drawChar(self, pInstance ):
		self.progressCell()
		eChar = u" %c" % pInstance.getChar()
		self.getScreen().setTableText(self.name, self.curColumn, self.curRow, "<font=2>" + eChar + "</font>", "", pInstance.getWikiWidget(), pInstance.getID(), -1, CvUtil.FONT_LEFT_JUSTIFY )
		
	def applyPanel(self):
		self.progressCell()
		name = self.name + "-" + str(self.curRow) + "-" + str(self.curColumn)
		self.getScreen().addPanel(name, u"", u"", True, False, 0, 0, self.getCellWidth(), self.getCellHeight(), PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)
		self.getScreen().attachControlToTableCell(name, self.name, self.curRow, self.curColumn )
		return name
		
	def addPanelButton(self, buttonArt, widget = WidgetTypes.WIDGET_GENERAL, iData1 = -1, iData2 = -1):
		name = self.applyPanel()
		szButtonName = self.name + "Button-" + str(self.curRow) + "-" + str(self.curColumn)
		self.getScreen().setImageButtonAt(szButtonName, name, buttonArt, 0, 0, self.getCellWidth(), self.getCellHeight(), widget, iData1, iData2)
		
	def skipCell(self):
		self.addText("")


	def getScreen(self):
		return CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )
