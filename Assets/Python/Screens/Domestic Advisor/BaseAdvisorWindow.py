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
		return

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
		self.tableManager.tableInit()
		self.tableManager.addHeaderButton()
		
		# some rows here
		
		self.tableManager.tableHeaderComplete()

	# fairly self explanatory. Overwrite and remove unwanted lines.
	# Tip: if you don't want either, write return because python doesn't allow empty methods.
	def setSortSelect(self):
		self.tableManager.enableSelect()
		self.tableManager.enableSort()

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
