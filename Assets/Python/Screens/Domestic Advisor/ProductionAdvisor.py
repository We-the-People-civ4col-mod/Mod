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

class ProductionAdvisor(BaseAdvisorWindow.BaseAdvisorWindow):
	def __init__(self, parent):
		BaseAdvisorWindow.BaseAdvisorWindow.__init__(self, parent, "ProductionStateClass")

	def drawColonyCell(self, iCity, pCity, iYield, info):
		iNetYield = pCity.calculateNetYield(iYield)
		szText = unicode(iNetYield)
		if iNetYield > 0:
			szText = localText.getText("TXT_KEY_COLOR_POSITIVE", ()) + u"+" + szText + localText.getText("TXT_KEY_COLOR_REVERT", ())
		elif iNetYield < 0:
			szText = localText.getText("TXT_KEY_COLOR_NEGATIVE", ()) + szText + localText.getText("TXT_KEY_COLOR_REVERT", ())
		elif iNetYield == 0:
			szText = ""
		self.tableManager.addTextRight(szText)

	def createTableHeader(self):
		# create table headers
		self.tableManager.addHeaderButton()
		self.tableManager.addHeaderCityName()
		self.tableManager.addHeaderArrayYields()
