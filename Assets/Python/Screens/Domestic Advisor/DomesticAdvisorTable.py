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
	def __init__(self, parent):
		self.iFillWidthWhenOverPercentage = 90
		self.bFillSinglePage = True
	
		self.parent = parent
		self.MainAdvisor = parent.parent
		self.iWidth = self.MainAdvisor.nTableWidth
		self.defaultColumnWidth = self.MainAdvisor.DEFAULT_COLUMN_WIDTH
		self.iRowHeight = self.MainAdvisor.ROW_HIGHT
		self.InfoArray = None
		self.columnWidth = [[]]
		self.columnName = [[]]
		self.columnsOnPage = []
		self.curPage = 0
		self.pagesNotSet = []
		
		self.pagesNotSet.append(True)
		
	def show(self):
		self.getScreen().show( self.currentPageName() )
	
	def hide(self):
		self.getScreen().hide( self.currentPageName() )
		
	def isNeedInit(self):
		return len(self.columnWidth[0]) == 0
		
	def enableSelect(self):
		self.getScreen().enableSelect( self.currentPageName(), True )
		
	def enableSort(self):
		self.getScreen().enableSort( self.currentPageName() )
	
	def setHeader(self):
		if (self.pagesNotSet[self.currentPage()]):
			self.pagesNotSet[self.currentPage()] = False
			self.tableHeaderComplete()
	
	def tableHeaderComplete(self):
		self.getScreen().addTableControlGFC( self.currentPageName(), self.columnsOnCurrentPage(), (self.MainAdvisor.nScreenWidth - self.iWidth) / 2, 60, self.iWidth, self.MainAdvisor.nTableHeight, True, False, self.MainAdvisor.iCityButtonSize, self.MainAdvisor.iCityButtonSize, TableStyles.TABLE_STYLE_STANDARD )
		self.getScreen().setStyle( self.currentPageName(), "Table_StandardCiv_Style" )
		
		for i in range(len(self.columnWidth[self.currentPage()])):
			self.getScreen().setTableColumnHeader( self.currentPageName(), i, "<font=2>" + self.columnName[self.currentPage()][i] + "</font>", self.columnWidth[self.currentPage()][i] )
		
	# note 0 means default height
	def setNumRows(self, iRows, iHeight = 0):
		if (iRows != self.getScreen().getTableNumRows(self.currentPageName())):
			self.getScreen().setTableNumRows ( self.currentPageName(), iRows)
			self.setRowHeight(iHeight)
		elif iRows == 1:
			# The table inits to 1 line, not 0
			# Assume the height not to be set if there is just one line
			self.setRowHeight(iHeight)
		
		
	def setRowHeight(self, iHeight = 0):
		if iHeight != 0:
			self.iRowHeight = iHeight
		for i in range(self.getScreen().getTableNumRows(self.currentPageName())):
			self.getScreen().setTableRowHeight(self.currentPageName(), i, self.iRowHeight)
		
	def addHeaderDirect(self, iWidth, szName):
		self.columnWidth[self.currentPage()].append(iWidth)
		self.columnName[self.currentPage()].append(szName)
		
	def addHeaderTxt(self, szName, iWidth):
		self.addHeaderDirect(iWidth, localText.getText(szName, ()).upper())
		
	def addHeaderButton(self, szTitle = ""):
		self.addHeaderDirect(self.MainAdvisor.ROW_HIGHT, szTitle)
		
	def addHeaderCityName(self, szName = "TXT_KEY_DOMESTIC_ADVISOR_NAME"):
		self.addHeaderTxt(szName, self.MainAdvisor.CITY_NAME_COLUMN_WIDTH)
		
	def addHeaderChar(self, iChar, iWidth = 50):
		char = (u" %c" % iChar)
		self.addHeaderDirect(iWidth, char)

	def addHeaderArrayBuildings(self):
		self.InfoArray = gc.getPlayer(CyGame().getActivePlayer()).getSpecialBuildingTypes()
		iTableWidth = self.iWidth - self.columnWidth[0][0] - self.columnWidth[0][1]
		self.__addHeaderArray(iTableWidth)
		self.curPage = JITarrayTypes.JIT_ARRAY_BUILDING_SPECIAL

	def addHeaderArrayYields(self):
		self.InfoArray = gc.getPlayer(CyGame().getActivePlayer()).getStoredYieldTypes()
		iTableWidth = self.iWidth - self.columnWidth[0][0] - self.columnWidth[0][1]
		self.__addHeaderArray(iTableWidth)
		self.curPage = JITarrayTypes.JIT_ARRAY_YIELD

	def __addHeaderArray(self, iTableWidth):
		iMaxColumnsOnPage = iTableWidth // self.defaultColumnWidth
		iNumColumns = self.InfoArray.getLength()
		
		# add sub pages to allow enough room for all of the array content
		while (iNumColumns > iMaxColumnsOnPage):
			iNumColumns -= iMaxColumnsOnPage
			self.columnsOnPage.append(iMaxColumnsOnPage)
			self.pagesNotSet.append(True)
			self.columnWidth.append(self.columnWidth[0][:])
			self.columnName.append(self.columnName[0][:])
		self.columnsOnPage.append(iNumColumns)

		# fill each sub page with columns
		for i in range(len(self.columnsOnPage)):
			self.curPage = i
			iColumnOffset = len(self.columnWidth)
		
			iOffset = self.__getCurrentOffset()
			iIndex = self.currentPage()
			iColumn = len(self.columnWidth)
			
			iNumColumnsOnPage = self.columnsOnPage[iIndex]
			
			iColumnWidth = self.defaultColumnWidth
			iNumExtraPixels = 0
			iColumnLength = iColumnWidth * iNumColumnsOnPage
			
			iFillPercentage = ((self.iWidth - self.widthLeft() + iColumnLength) * 100) / self.iWidth
			
			bFill = self.bFillSinglePage and len(self.columnsOnPage) == 1
			
			if (bFill or iFillPercentage > self.iFillWidthWhenOverPercentage):
				iColumnWidth = int(self.widthLeft() // iNumColumnsOnPage)
				iNumExtraPixels = self.widthLeft() - (iColumnWidth * iNumColumnsOnPage)
			
			for iNum in range(iNumColumnsOnPage):
				iArrayIndex = iOffset + iNum
				
				iWidth = iColumnWidth
				if (iNum < iNumExtraPixels):
					iWidth += 1
				
				self.addHeaderDirect(iWidth, self.getColumnHeader(iArrayIndex))
		self.curPage = 0

	def clearRows(self):
		# removes all rows and sets the counters to be ready to start on the first row
		self.curRow = -1
		self.curColumn = self.columnsOnCurrentPage()

	def getCellHeight(self):
		return self.iRowHeight
	
	def getCellWidth(self):
		return self.columnWidth[self.currentPage()][self.curColumn]

	def progressCell(self):
		self.curColumn += 1
		if self.curColumn >= self.columnsOnCurrentPage():
			self.curRow += 1
			self.curColumn = 0
			if self.getScreen().getTableNumRows(self.currentPageName()) == self.curRow:
				self.getScreen().appendTableRow( self.currentPageName() )
				self.getScreen().setTableRowHeight(self.currentPageName(), self.curRow, self.MainAdvisor.ROW_HIGHT)
	
	def addText(self, szText, iData1 = -1, iData2 = -1, widget = WidgetTypes.WIDGET_GENERAL, justified = CvUtil.FONT_LEFT_JUSTIFY):
		self.progressCell()
		self.getScreen().setTableText(self.currentPageName(), self.curColumn, self.curRow, "<font=2>" + szText + "</font>", "", widget, iData1, iData2, justified )

	def addTextRight(self, szText, iData1 = -1, iData2 = -1, widget = WidgetTypes.WIDGET_GENERAL):
		self.addText(szText, iData1, iData2, widget, CvUtil.FONT_RIGHT_JUSTIFY)

	def addInt(self, iValue, iData1 = -1, iData2 = -1, widget = WidgetTypes.WIDGET_GENERAL, justified = CvUtil.FONT_RIGHT_JUSTIFY):
		self.progressCell()
		self.getScreen().setTableInt(self.currentPageName(), self.curColumn, self.curRow, "<font=2>" + unicode(iValue) + "</font>", "", widget, iData1, iData2, justified )
		
	def drawChar(self, pInstance ):
		self.progressCell()
		eChar = u" %c" % pInstance.getChar()
		self.getScreen().setTableText(self.currentPageName(), self.curColumn, self.curRow, "<font=2>" + eChar + "</font>", "", pInstance.getWikiWidget(), pInstance.getID(), -1, CvUtil.FONT_LEFT_JUSTIFY )
		
	def applyPanel(self):
		self.progressCell()
		name = self.currentPageName() + "-" + str(self.curRow) + "-" + str(self.curColumn)
		self.getScreen().addPanel(name, u"", u"", True, False, 0, 0, self.getCellWidth(), self.getCellHeight(), PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)
		self.getScreen().attachControlToTableCell(name, self.currentPageName(), self.curRow, self.curColumn )
		return name
		
	def addPanelButton(self, buttonArt, widget = WidgetTypes.WIDGET_GENERAL, iData1 = -1, iData2 = -1):
		name = self.applyPanel()
		szButtonName = self.currentPageName() + "Button-" + str(self.curRow) + "-" + str(self.curColumn)
		self.getScreen().setImageButtonAt(szButtonName, name, buttonArt, 0, 0, self.getCellWidth(), self.getCellHeight(), widget, iData1, iData2)
		
	def skipCell(self):
		self.addText("")

	def autofillRow(self, iCity, pCity):
		iOffset = self.__getCurrentOffset()
		iNumColumnsOnPage = self.columnsOnPage[self.currentPage()]
		for i in range(iNumColumnsOnPage):
			iType = self.InfoArray.get(i + iOffset)
			self.parent.drawColonyCell(iCity, pCity, iType, self.getInfoForType(iType))
		
	def __getCurrentOffset(self):
		iOffset = 0
		iPage = self.currentPage()
		if (iPage > 0):
			for i in range(iPage):
				iOffset += self.columnsOnPage[i]
		return iOffset
	
	def columnsOnCurrentPage(self):
		return len(self.columnWidth[self.currentPage()])
		
	def widthLeft(self):
		iWidth = self.iWidth
		for i in (self.columnWidth[self.currentPage()]):
			iWidth -= i
		return iWidth
	
	def getColumnHeader(self, iColumn):
		info = self.getInfoForColumn(iColumn)
		if (info != None):
			return (u" %c" % info.getChar())
			
		return ""
	
	def getInfoForType(self, iType):
		if (self.InfoArray != None):
			if (self.InfoArray.getType(0) == JITarrayTypes.JIT_ARRAY_BUILDING_SPECIAL):
				return gc.getSpecialBuildingInfo(iType)
			if (self.InfoArray.getType(0) == JITarrayTypes.JIT_ARRAY_YIELD):
				return gc.getYieldInfo(iType)
			
		return None
	
	def getInfoForColumn(self, iColumn):
		if (self.InfoArray != None):
			return self.getInfoForType(self.InfoArray.get(iColumn))
			
		return None

	def nextPage(self):
		self.setPage(self.currentPage() + 1)
		
	def prevPage(self):
		self.setPage(self.currentPage() - 1)
		
	def isFirstPage(self):
		return self.currentPage() == 0
		
	def isLastPage(self):
		return (self.currentPage() + 1) == self.numPages()

	def currentPage(self):
		if (self.curPage ==JITarrayTypes.JIT_ARRAY_BUILDING_SPECIAL):
			return self.MainAdvisor.iCurrentBuildingSubPage
		if (self.curPage == JITarrayTypes.JIT_ARRAY_YIELD):
			return self.MainAdvisor.iCurrentYieldSubPage
		return self.curPage

	def currentPageName(self):
		return self.parent.screenName + str(self.currentPage())

	def numPages(self):
		return len(self.columnWidth)

	def setPage(self, iPage):
		if (iPage >= 0 and iPage < self.numPages() and iPage != self.currentPage()):
			self.parent.hide()
			if (self.curPage == JITarrayTypes.JIT_ARRAY_BUILDING_SPECIAL):
				self.MainAdvisor.iCurrentBuildingSubPage = iPage
			elif (self.curPage == JITarrayTypes.JIT_ARRAY_YIELD):
				self.MainAdvisor.iCurrentYieldSubPage = iPage
			else:
				self.curPage = iPage
			self.parent.draw()

	def getScreen(self):
		return CyGInterfaceScreen( "DomesticAdvisor", CvScreenEnums.DOMESTIC_ADVISOR )
