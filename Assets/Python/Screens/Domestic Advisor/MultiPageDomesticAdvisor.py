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

class MultiPageDomesticAdvisor:

	# init shouldn't be overwritten, but it should be called from child init
	def __init__(self, parent, name):
		self.parent = parent
		self.name = name
		self.screenName = self.name + "ListBackground"
		self.pages = []
		
	def setupPages(self, parent, name):
		# the only function to be overwritten. It should contain lines like:
		# self.pages.append(MultiPageDomesticAdvisor.MultiPageDomesticAdvisor(parent, "name"))
		# note that parent argument should be parent here as that's the same as self in CvDoesticAdvisor.
		# this way a page file can be freely moved between buttons or get a button of its own without modifying the page file.
		return
		
	def draw(self):
		if len(self.pages) == 0:
			self.setupPages(self.parent, self.name)		
			self.currentPage = 0
			self.lastPage = len(self.pages) - 1
		self.pages[self.currentPage].draw()
		
	def hide(self):
		self.pages[self.currentPage].hide()
		
	def setDirty(self):
		for i in range(len(self.pages)):
			self.pages[i].setDirty()
		
	def getWidgetHelp(self, argsList):
		self.pages[self.currentPage].getWidgetHelp(argsList)
		
	def handleInput(self, inputClass):
		return self.pages[self.currentPage].handleInput(inputClass)
		
	def nextPage(self):
		if not self.pages[self.currentPage].isLastPage():
			self.pages[self.currentPage].nextPage()
		else:
			if self.currentPage != self.lastPage:
				self.pages[self.currentPage].hide()
				self.currentPage += 1
				self.pages[self.currentPage].draw()
		
	def prevPage(self):
		if not self.pages[self.currentPage].isFirstPage():
			self.pages[self.currentPage].prevPage()
		else:
			if self.currentPage != 0:
				self.pages[self.currentPage].hide()
				self.currentPage -= 1
				self.pages[self.currentPage].draw()
		
	def isFirstPage(self):
		if self.currentPage == 0:
			return self.pages[0].isFirstPage()
		return false
		
	def isLastPage(self):
		if self.currentPage == self.lastPage:
			return self.pages[self.currentPage].isLastPage()
		return false

