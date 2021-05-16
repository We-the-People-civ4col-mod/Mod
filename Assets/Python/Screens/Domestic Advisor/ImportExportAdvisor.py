## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums

import DomesticAdvisorTable
import BaseAdvisorWindow

## Based on work from R&R, Robert Surcouf,  Domestic Advisor Screen

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class ImportExportAdvisor(BaseAdvisorWindow.BaseAdvisorWindow):
	def __init__(self, parent):
		BaseAdvisorWindow.BaseAdvisorWindow.__init__(self, parent, "ImportExportStateClass")

	def drawColonyCell(self, iCity, pCity, iYield, pYieldInfo):
		bExportYield = pCity.isExport(iYield)
		bImportYield = pCity.isImport(iYield)
		## R&R, Robert Surcouf,  Domestic Advisor Screen - End
		if (bExportYield and bImportYield):
			self.tableManager.addText("<color=255,255,0>" + localText.getText("TXT_KEY_IN_AND_OUT", ()) + u"</color>")
		elif (not bExportYield and bImportYield):
			self.tableManager.addText("<color=0,255,0>" + localText.getText("TXT_KEY_IN", ()) + u"</color>")
		elif (bExportYield and not bImportYield):
			self.tableManager.addText("<color=255,0,0>" + localText.getText("TXT_KEY_OUT", ()) + u"</color>")
		else:
			self.tableManager.skipCell()

	def createTableHeader(self):
		# create table headers
		self.tableManager.addHeaderButton()
		self.tableManager.addHeaderCityName()
		self.tableManager.addHeaderArrayYields()
