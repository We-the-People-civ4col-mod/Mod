from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums

import DomesticAdvisorTable
import BaseAdvisorWindow


# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class TradeCountAdvisor(BaseAdvisorWindow.BaseAdvisorWindow):
	def __init__(self, parent):
		BaseAdvisorWindow.BaseAdvisorWindow.__init__(self, parent, "TradeCountStateClass")

	def getString(self, iRow, iYield, bCompact):
		player = gc.getPlayer(gc.getGame().getActivePlayer())

		iBought = 0
		iSold = 0

		if iRow == 0:
			iBought = player.getYieldBoughtTotal(TradeLocationTypes.TRADE_LOCATION_EUROPE, iYield)
			iSold = player.getYieldSoldTotal(TradeLocationTypes.TRADE_LOCATION_EUROPE, iYield)
		elif iRow == 1:
			iBought = player.getYieldBoughtTotal(TradeLocationTypes.TRADE_LOCATION_AFRICA, iYield)
			iSold = player.getYieldSoldTotal(TradeLocationTypes.TRADE_LOCATION_AFRICA, iYield)
		elif iRow == 2:
			iBought = player.getYieldBoughtTotal(TradeLocationTypes.TRADE_LOCATION_PORT_ROYAL, iYield)
			iSold = player.getYieldSoldTotal(TradeLocationTypes.TRADE_LOCATION_PORT_ROYAL, iYield)

		if bCompact:
			return unicode(iSold) + "/" + unicode(iBought)
		else:
			return localText.getText("TXT_KEY_DOMESTIC_ADVISOR_TRADE_STATE_HELP", (iSold, iBought))

	def drawColonyCell(self, iRow, pCity, iYield, pYieldInfo):
		output = self.getString(iRow, iYield, True)

		self.tableManager.addText(output, iRow, iYield, WidgetTypes.WIDGET_HELP_DOMESTIC_TRADED_COUNT)

	def getWidgetHelp(self, argsList):
		iScreen, eWidgetType, iData1, iData2, bOption = argsList

		if eWidgetType == WidgetTypes.WIDGET_HELP_DOMESTIC_TRADED_COUNT:
			return self.getString(iData1, iData2, False)

		return None

	def getRowName(self, iRow):
		if iRow == 0:
			return localText.getText("TXT_KEY_CONCEPT_EUROPE", ())
		elif iRow == 1:
			return localText.getText("TXT_KEY_CONCEPT_AFRICA", ())
		elif iRow == 2:
			return localText.getText("TXT_KEY_CONCEPT_PORT_ROYAL", ())

	def drawEachColonyLine(self):
		self.tableManager.clearRows()
		iNumRows = 3

		self.tableManager.setNumRows(iNumRows)
		for iRow in range(iNumRows):
			self.tableManager.skipCell()
			self.tableManager.addText(self.getRowName(iRow))
			self.drawColonyRow(iRow, None)

	def createTableHeader(self):
		# create table headers
		self.tableManager.addHeaderTxt("", self.tableManager.MainAdvisor.ROW_HIGHT)
		self.tableManager.addHeaderTxt("", self.tableManager.MainAdvisor.CITY_NAME_COLUMN_WIDTH)
		self.tableManager.addHeaderArrayYields()
