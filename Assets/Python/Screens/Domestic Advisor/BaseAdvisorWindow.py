## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums

import DomesticAdvisorTable


# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class BaseAdvisorWindow:

	# init shouldn't be overwritten, but it should be called from child init
	def __init__(self, parent, name):
		self.parent = parent
		self.drawn = False
		self.name = name
		self.screenName = self.name + "ListBackground"
		self.dirty = True
		self.tableManager = DomesticAdvisorTable.DomesticAdvisorTable(self)
		self.bAlwaysForceRedraw = True

	# fill the cells of the table
	def drawTableContent(self):
		self.drawEachColonyLine()

	def drawEachColonyLine(self):
		self.tableManager.clearRows()
		self.tableManager.setNumRows(len(self.parent.Cities))
		for iCity in range(len(self.parent.Cities)):
			pLoopCity = self.parent.Cities[iCity]
			self.tableManager.addPanelButton(ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_CITYSELECTION").getPath(), WidgetTypes.WIDGET_ZOOM_CITY, pLoopCity.getOwner(), pLoopCity.getID())
			self.tableManager.addText(pLoopCity.getName())
			self.drawColonyRow(iCity, pLoopCity)
			
	def drawColonyRow(self, iCity, pCity):
		self.tableManager.autofillRow(iCity, pCity)
		
	def drawColonyCell(self, iCity, pCity, iType, info):
		self.tableManager.skipCell()

	# set custom help
	def getWidgetHelp(self, argsList):
		iScreen, eWidgetType, iData1, iData2, bOption = argsList
		
		return None
	
	# add custom actions to input
	# note that standard action like sort or widget based (like click to jump to colony) will not require custom input code
	def handleInput (self, inputClass):
	
		return -1
	
	# hide the window. Overwrite if you add custom widgets outside of tableManager as they will have to be hidden here
	def hide(self):
		self.tableManager.hide()

	def createTableHeader(self):
		# create table headers
		self.tableManager.addHeaderButton()
		
		# some rows here

	# fairly self explanatory. Overwrite and remove unwanted lines.
	# Tip: if you don't want either, write return because python doesn't allow empty methods.
	def setSortSelect(self):
		self.tableManager.enableSelect()
		self.tableManager.enableSort()

	# left/right arrow sub pages
	# in most cases the table manager can handle this automatic, but it is possible to override here if needed
	def nextPage(self):
		self.tableManager.nextPage()
		
	def prevPage(self):
		self.tableManager.prevPage()
		
	def isFirstPage(self):
		return self.tableManager.isFirstPage()
		
	def isLastPage(self):
		return self.tableManager.isLastPage()

	###
	### the rest of the methods are unlikely to need being overwritten
	###

	# redraw if dirty
	def draw(self):
		# assume dirty every time if window needs to be redrawn every time
		if not self.dirty and not self.bAlwaysForceRedraw:
			self.tableManager.show()
			return
		self.dirty = False
		
		if self.tableManager.isNeedInit():
			self.createTableHeader()
		
		self.tableManager.clearRows()
		self.tableManager.show()
		
		self.tableManager.setHeader()
		
		self.setSortSelect()
		
		self.drawTableContent()

	# can't think of a reason to overwrite this. Might be called while the window is not even showing
	def setDirty(self):
		self.dirty = True
