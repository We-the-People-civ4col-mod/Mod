## Sid Meier's Civilization 4
## Copyright Firaxis Games 2008

from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums


#R&R mod, vetiarvind, trade groups - start
import CvPopupInterface
import CvEventInterface 
#R&R mod, vetiarvind, trade groups - end

#import time

##
## New Trade Routes Advisor
## Version 1.02
## by koma13
##

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class CvTradeRoutesAdvisor:
	"Trade Routes Advisor"
	
	def __init__(self):
		
		self.WIDGET_ID = "TradeRoutesWidget"
		self.nWidgetCount = 0
		
		self.SCREEN_NAME = "TradeRoutesScreen"
		self.BACKGROUND_ID = "TradeRoutesBackground"

		self.XResolution = 0
		self.YResolution = 0

		# Table enums
		self.NO_TABLE = -1
		self.EXISTING_ROUTES = 0
		self.LIMITED_ROUTES = 1
		self.BUILDER_TABLE = 2
		self.YIELD_TABLE = 3
		self.EXPORT_TABLE = 4
		self.IMPORT_TABLE = 5
		#R&R mod, vetiarvind, trade groups - start
		self.LOAD_GROUP_TABLE = 6 		
		self.DELETE_GROUP_TABLE = 7
		#R&R mod, vetiarvind, trade groups - end
		
		# Selection enums
		self.NO_YIELD = -1
		self.EUROPE_CITY = -1
		self.NO_CITY = -2
		
		# Button ids
		self.YIELD_TABLE_ID = 0
		self.EXPORT_TABLE_ID = 1
		self.IMPORT_TABLE_ID = 2
		
		self.YIELD_LIST_ID = 3
		
		self.CLEAR_YIELD_ID = 4
		self.CLEAR_EXPORT_ID = 5
		self.CLEAR_IMPORT_ID = 6
		
		self.SELECT_ID = 7
		self.DANGER_ID = 8
		self.ROUTES_ID = 9
		self.RETURN_ID = 10
		
		#R&R mod, vetiarvind, trade groups - START
		self.LOAD_GROUP_ID = 11
		self.SAVE_GROUP_ID = 12		
		self.DELETE_GROUP_ID = 13
		#R&R mod, vetiarvind, trade groups - END
		
		
	def interfaceScreen (self):
		screen = self.getScreen()
		if screen.isActive():
			return
		
		self.player = gc.getPlayer(gc.getGame().getActivePlayer())
		
		# Transport Unit
		self.pTransport = CyInterface().getHeadSelectedUnit()
		if self.pTransport == None:
			return
		
		# City list
		self.CityList = []
		(city, iter) = self.player.firstCity(False)
		while (city):
			if self.pTransport.generatePath(city.plot(), 0, False, None):
				self.CityList.append(city)
			(city, iter) = self.player.nextCity(iter, False)
		
		if self.CityList == []:
			return
		
		# Cargo list
		self.YieldList = []
		for iYield in range(YieldTypes.NUM_YIELD_TYPES):
			kYield = gc.getYieldInfo(iYield)
			if kYield.isCargo():
				self.YieldList.append(iYield)
		
		self.iNumYields = len(self.YieldList)
		if self.iNumYields == 0:
			return
		
		# Trade routes
		self.ExistingRoutes, self.AssignedRoutes = [], {}
		for iRoute in range(self.player.getNumTradeRoutes()):
			pRoute = self.player.getTradeRouteByIndex(iRoute)
			if self.pTransport.canAssignTradeRoute(pRoute.getID(), True):
				self.ExistingRoutes.append(pRoute)
				self.AssignedRoutes[pRoute.getID()] = self.pTransport.getGroup().isAssignedTradeRoute(pRoute.getID())
		
		#if len(self.ExistingRoutes) == 0:
		#	return
		
		self.iYields = self.NO_YIELD
		self.iExport, self.iImport = self.NO_CITY, self.NO_CITY
		self.iExportPreview, self.iImportPreview = self.NO_CITY, self.NO_CITY
		
		self.bAssigned, self.bSelected = False, False
		self.bDanger = self.pTransport.isIgnoreDanger()
		
		# Table names		
		self.TableNames = []
		self.TableNames.append("ExistingRoutes")
		self.TableNames.append("LimitedRoutes")
		self.TableNames.append("BuilderTable")
		self.TableNames.append("YieldTable")
		self.TableNames.append("ExportTable")
		self.TableNames.append("ImportTable")
		#R&R mod, vetiarvind, trade groups - START
		self.TableNames.append("LoadGroupTable")
		self.TableNames.append("DeleteGroupTable")
		#R&R mod, vetiarvind, trade groups - END
		
		self.TableLabel = []		
		szTitle = localText.getText("TXT_KEY_TRADE_ROUTES_MAIN_TABLE", ())
		self.TableLabel.append(szTitle)		
		szTitle = localText.getText("TXT_KEY_TRADE_ROUTES_MAIN_TABLE", ())
		self.TableLabel.append(szTitle)
		szTitle = localText.getText("TXT_KEY_TRADE_ROUTES_NEW_ROUTE", ())
		self.TableLabel.append(szTitle)
		szTitle = localText.getText("TXT_KEY_TRADE_ROUTES_YIELD_TABLE", ())
		self.TableLabel.append(szTitle)
		szTitle = localText.getText("TXT_KEY_TRADE_ROUTES_EXPORT_TABLE", ())
		self.TableLabel.append(szTitle)
		szTitle = localText.getText("TXT_KEY_TRADE_ROUTES_IMPORT_TABLE", ())
		self.TableLabel.append(szTitle)
		#R&R mod, vetiarvind, trade groups - START				
		szTitle = localText.getText("TXT_KEY_TRADE_ROUTES_LOAD_GROUP_TABLE", ()) 
		self.TableLabel.append(szTitle) 
		szTitle = localText.getText("TXT_KEY_TRADE_ROUTES_DELETE_GROUP_TABLE", ()) 		
		self.TableLabel.append(szTitle) 
		#R&R mod, vetiarvind, trade groups - END		
		
		self.CURRENT_TABLE = self.NO_TABLE
		
		# Widget positions
		self.XResolution = self.getScreen().getXResolution()
		self.YResolution = self.getScreen().getYResolution()

		self.STANDARD_MARGIN = 16
		
		self.PANEL_WIDTH = 420
		self.PANEL_X = self.XResolution - self.PANEL_WIDTH - self.STANDARD_MARGIN
		self.PANEL_Y = 55
		
		self.PANEL_HEIGHT = self.YResolution - self.PANEL_Y - self.STANDARD_MARGIN - self.YResolution * 31 * 28 / 10000
		self.PANEL_BORDER_SIZE = 3
		
		self.PREVIEW_X = self.STANDARD_MARGIN
		self.PREVIEW_WIDTH = (self.PANEL_WIDTH - self.STANDARD_MARGIN * 3) / 3
		self.PREVIEW_HEIGHT = self.PREVIEW_WIDTH * 3 / 4
		self.PREVIEW_Y = self.PANEL_HEIGHT - self.PREVIEW_HEIGHT - self.STANDARD_MARGIN - 36
		
		self.TABLE_X = self.STANDARD_MARGIN
		self.TABLE_Y = self.STANDARD_MARGIN * 2
		self.TABLE_WIDTH = self.PANEL_WIDTH - self.STANDARD_MARGIN * 2
		#self.TABLE_HEIGHT = self.PREVIEW_Y - self.TABLE_Y - self.STANDARD_MARGIN / 2 #R&R mod, vetiarvind, trade groups
		self.TABLE_HEIGHT = self.PREVIEW_Y - self.TABLE_Y - self.STANDARD_MARGIN / 2 - 40 #R&R mod, vetiarvind, trade groups
		self.ROW_HIGHT = 32		
		
		self.EXIT_Y = self.PANEL_HEIGHT - 45
		
		#R&R mod, vetiarvind, trade groups - start
		self.GROUPBUTTON_Y = self.EXIT_Y - 140
		self.GROUPBUTTON_WIDTH = self.TABLE_WIDTH/3 - 10
		#R&R mod, vetiarvind, trade groups - end
		
		self.BUTTON_SIZE = 24
		self.BUTTON_X = self.PANEL_WIDTH - self.STANDARD_MARGIN - self.BUTTON_SIZE * 3 - 4
		self.BUTTON_Y = self.STANDARD_MARGIN / 4
		self.BUTTON_WIDTH = self.BUTTON_SIZE * 3 + 8
		self.BUTTON_HEIGHT = self.BUTTON_SIZE + 2
		
		# Set the background and exit button, and show the screen
		screen.setRenderInterfaceOnly(False)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)
		screen.setDimensions(self.PANEL_X, self.PANEL_Y, self.PANEL_WIDTH, self.PANEL_HEIGHT)
		screen.showWindowBackground(False)
		
		screen.addDDSGFC(self.BACKGROUND_ID, "Art/Interface/Screens/TradeRoutes/Background.dds", 0, 0, self.PANEL_WIDTH, self.PANEL_HEIGHT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		self.szTitle = "PanelTitle"
		screen.setLabelAt( self.szTitle, self.BACKGROUND_ID, u"<font=3b>-</font>", CvUtil.FONT_LEFT_JUSTIFY, self.STANDARD_MARGIN, self.STANDARD_MARGIN + 2, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		#R&R mod, vetiarvind, trade groups - start		
		
		screen.setButtonGFC("BtnLoad", u"<font=3b>" + localText.getText("TXT_KEY_PEDIA_SCREEN_LOAD_GROUP", ()).upper() + u"</font>", "", self.TABLE_X, self.GROUPBUTTON_Y, self.GROUPBUTTON_WIDTH, 30, WidgetTypes.WIDGET_GENERAL, self.LOAD_GROUP_ID, -1, ButtonStyles.BUTTON_STYLE_STANDARD)
		screen.setButtonGFC("BtnSave", u"<font=3b>" + localText.getText("TXT_KEY_PITBOSS_SAVE", ()).upper() + u"</font>", "", self.TABLE_X + self.TABLE_WIDTH/3, self.GROUPBUTTON_Y, self.GROUPBUTTON_WIDTH, 30, WidgetTypes.WIDGET_GENERAL, self.SAVE_GROUP_ID, -1, ButtonStyles.BUTTON_STYLE_STANDARD)	
		screen.setButtonGFC("BtnDelete", u"<font=3b>" + localText.getText("TXT_KEY_GLOBELAYER_STRATEGY_DELETE", ()).upper() + u"</font>", "", self.TABLE_X + self.TABLE_WIDTH*2/3, self.GROUPBUTTON_Y, self.GROUPBUTTON_WIDTH, 30, WidgetTypes.WIDGET_GENERAL, self.DELETE_GROUP_ID, -1, ButtonStyles.BUTTON_STYLE_STANDARD)	
		
		#R&R mod, vetiarvind, trade groups - end
		
		screen.setButtonGFC("ScreenExit", u"<font=3b>" + localText.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + u"</font>", "", self.TABLE_X, self.EXIT_Y, self.TABLE_WIDTH, 30, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1, ButtonStyles.BUTTON_STYLE_STANDARD)
		screen.setActivation( "ScreenExit", ActivationTypes.ACTIVATE_MIMICPARENTFOCUS )
		
		# Panel border
		szBorderArt = "Art/Interface/Screens/TradeRoutes/Pixel.dds"
		
		screen.addDDSGFC(self.getNextWidgetName(), szBorderArt, 0, 0, self.PANEL_WIDTH, self.PANEL_BORDER_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addDDSGFC(self.getNextWidgetName(), szBorderArt, self.PANEL_WIDTH - self.PANEL_BORDER_SIZE, self.PANEL_BORDER_SIZE, self.PANEL_BORDER_SIZE, self.PANEL_HEIGHT - self.PANEL_BORDER_SIZE * 2, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addDDSGFC(self.getNextWidgetName(), szBorderArt, 0, self.PANEL_HEIGHT - self.PANEL_BORDER_SIZE, self.PANEL_WIDTH, self.PANEL_BORDER_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addDDSGFC(self.getNextWidgetName(), szBorderArt, 0, self.PANEL_BORDER_SIZE, self.PANEL_BORDER_SIZE, self.PANEL_HEIGHT - self.PANEL_BORDER_SIZE * 2, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		# Upper buttons
		self.szButtonPanel = "ButtonPanel"
		
		screen.addMultiListControlGFC(self.szButtonPanel, u"", self.BUTTON_X, self.BUTTON_Y, self.BUTTON_WIDTH, self.BUTTON_HEIGHT, 0, self.BUTTON_SIZE, self.BUTTON_SIZE, TableStyles.TABLE_STYLE_STANDARD )
		
		# Preview bar
		self.szPreviewYields = "PreviewYields"
		self.szPreviewExport = "PreviewExport"
		self.szPreviewImport = "PreviewImport"
		
		CURRENT_X = self.PREVIEW_X
		TABLE_ID, CLEAR_ID = self.YIELD_TABLE_ID, self.CLEAR_YIELD_ID
		
		for szPreview in [self.szPreviewYields, self.szPreviewExport, self.szPreviewImport]:
			screen.addDDSGFC(szPreview + "Highlight", "Art/Interface/Screens/TradeRoutes/PreviewHighlight.dds", CURRENT_X, self.PREVIEW_Y, self.PREVIEW_WIDTH, self.PREVIEW_HEIGHT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.addDDSGFC(szPreview + "Background", "Art/Interface/Screens/TradeRoutes/PreviewBackground.dds", CURRENT_X, self.PREVIEW_Y, self.PREVIEW_WIDTH, self.PREVIEW_HEIGHT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.setImageButton(szPreview + "Border" + str(0), "Art/Interface/Screens/TradeRoutes/PreviewBorder.dds", CURRENT_X, self.PREVIEW_Y, self.PREVIEW_WIDTH, self.PREVIEW_HEIGHT, WidgetTypes.WIDGET_GENERAL, TABLE_ID, -1)
			screen.addDDSGFCAt(szPreview + "Banner", szPreview + "Border" + str(0), "Art/Interface/Screens/TradeRoutes/BlackPixel.dds", 10, self.PREVIEW_HEIGHT - 26 , self.PREVIEW_WIDTH - 20, 17, WidgetTypes.WIDGET_GENERAL, -1, -1, False)
			screen.setImageButtonAt(szPreview + "Cancel", szPreview + "Border" + str(0), "Art/Interface/Screens/TradeRoutes/Cancel.dds", self.PREVIEW_WIDTH - 28, 4, 24, 24, WidgetTypes.WIDGET_GENERAL, CLEAR_ID, -1)
			screen.hide(szPreview + "Highlight")
			screen.hide(szPreview + "Cancel")
			screen.hide(szPreview + "Banner")
			
			CURRENT_X += self.PREVIEW_WIDTH + self.STANDARD_MARGIN / 2
			TABLE_ID += 1; CLEAR_ID += 1
			
		screen.setLabelAt(self.szPreviewYields + "Title", self.szPreviewYields + "Background", u"<font=3b>" + localText.getText("TXT_KEY_TRADE_ROUTES_YIELD_PREVIEW", ()).upper() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.PREVIEW_WIDTH / 2, self.PREVIEW_HEIGHT / 2, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setLabelAt(self.szPreviewExport + "Title", self.szPreviewExport + "Background", u"<font=3b>" + localText.getText("TXT_KEY_TRADE_ROUTES_EXPORT_PREVIEW", ()).upper() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.PREVIEW_WIDTH / 2, self.PREVIEW_HEIGHT / 2, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setLabelAt(self.szPreviewImport + "Title", self.szPreviewImport + "Background", u"<font=3b>" + localText.getText("TXT_KEY_TRADE_ROUTES_IMPORT_PREVIEW", ()).upper() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.PREVIEW_WIDTH / 2, self.PREVIEW_HEIGHT / 2, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			
		CyCamera().LookAtUnit(self.pTransport)
		
		self.drawContents()
		
		return 0

	
	def drawContents(self):
		self.routesTable(True)
				
	
	def routesTable(self, bRebuild):
		# Routes tables
		screen = self.getScreen()
		screen.hide(self.TableNames[self.CURRENT_TABLE])
		
		self.updatePreview()
		
		if self.isSelectionEmpty():
			self.CURRENT_TABLE = self.EXISTING_ROUTES
			self.CurrentList = self.ExistingRoutes
			if self.CurrentList > [] and not bRebuild:
				for iI in xrange(len(self.ExistingRoutes)):
					screen.setTableText(self.TableNames[self.CURRENT_TABLE], 1, iI, self.getAssignment(self.getRouteByTableRow(iI).getID()), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)
				screen.show(self.TableNames[self.CURRENT_TABLE])
				screen.modifyLabel(self.szTitle, u"<font=3b>" + self.TableLabel[self.CURRENT_TABLE] + u"</font>", CvUtil.FONT_LEFT_JUSTIFY)
				self.updateButtons()
				return
		else:
			self.CURRENT_TABLE = self.LIMITED_ROUTES
			self.CurrentList = []
			for pRoute in self.ExistingRoutes:
				if self.iYields == self.NO_YIELD or self.iYields == pRoute.getYield():
					if self.iExport == self.NO_CITY or self.iExport == pRoute.getSourceCity().iID:
						if self.iImport == self.NO_CITY or self.iImport == pRoute.getDestinationCity().iID:
							self.CurrentList.append(pRoute)
		
		if self.CurrentList == []:
			self.CURRENT_TABLE = self.BUILDER_TABLE
			
		def sortBySourceCities(pRoute):
			return pRoute.getSourceCityName()
		def sortByYields(pRoute):
			return pRoute.getYield()
		def sortByDestinationCities(pRoute):
			return pRoute.getDestinationCityName()
	
		self.CurrentList = sorted(self.CurrentList, key = sortByDestinationCities)
		self.CurrentList = sorted(self.CurrentList, key = sortByYields)
		self.CurrentList = sorted(self.CurrentList, key = sortBySourceCities)
		
		szTable = self.TableNames[self.CURRENT_TABLE]
		
		screen.modifyLabel(self.szTitle, u"<font=3b>" + self.TableLabel[self.CURRENT_TABLE] + u"</font>", CvUtil.FONT_LEFT_JUSTIFY)
		screen.addTableControlGFC(szTable, 5, self.TABLE_X, self.TABLE_Y, self.TABLE_WIDTH, self.TABLE_HEIGHT, True, False, 32, 32, TableStyles.TABLE_STYLE_STANDARD)
		screen.setStyle(szTable, "Table_StandardCiv_Style")
		screen.enableSort(szTable)
		screen.enableSelect(szTable, False)
		screen.setTableColumnHeader(szTable, 0, u"id", 0)
		screen.setTableColumnHeader(szTable, 1, u"", 32)
		screen.setTableColumnHeader(szTable, 2, u"", 32)
		screen.setTableColumnHeader(szTable, 3, localText.getText("TXT_KEY_TRADE_ROUTES_MAIN_TABLE_3", ()), self.TABLE_WIDTH / 2 - 41)
		screen.setTableColumnHeader(szTable, 4, localText.getText("TXT_KEY_TRADE_ROUTES_MAIN_TABLE_4", ()), self.TABLE_WIDTH / 2 - 41)
		
		self.updateButtons()
		
		if self.CURRENT_TABLE == self.BUILDER_TABLE:
			self.appendBuilderRow()
			return
				
		iI = 0
		for pRoute in self.CurrentList:
			screen.appendTableRow(szTable)
			screen.setTableRowHeight(szTable, iI, self.ROW_HIGHT)
			screen.setTableText(szTable, 0, iI, u"%d" % pRoute.getID(), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)
			screen.setTableText(szTable, 1, iI, self.getAssignment(pRoute.getID()), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)
			screen.setTableText(szTable, 2, iI, u"<font=3>%c</font>" % gc.getYieldInfo(pRoute.getYield()).getChar(), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)
			screen.setTableText(szTable, 3, iI, self.getColor(pRoute) + pRoute.getSourceCityName() + u"</color>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
			screen.setTableText(szTable, 4, iI, self.getColor(pRoute) + pRoute.getDestinationCityName() + u"</color>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
			
			iI += 1

				
	def getColor(self, pRoute):
		szColor = u"<color=255,255,255>"
		if pRoute.getDestinationCity().iID == self.EUROPE_CITY:
			szColor = u"<color=170,170,170>"
		
		return szColor

	
	def updateRoutes(self):
		self.ExistingRoutes = []
		for iRoute in range(self.player.getNumTradeRoutes()):
			pRoute = self.player.getTradeRouteByIndex(iRoute)
			if self.pTransport.canAssignTradeRoute(pRoute.getID(), True):
				self.ExistingRoutes.append(pRoute)
				if not pRoute.getID() in self.AssignedRoutes:
					self.AssignedRoutes[pRoute.getID()] = False
					if self.iYields == pRoute.getYield():
						if self.iExport == pRoute.getSourceCity().iID:
							if self.iImport == pRoute.getDestinationCity().iID:
								self.AssignedRoutes[pRoute.getID()] = self.bAssigned
					
		self.bAssigned = False
		
		self.iYields = self.NO_YIELD
		self.iExport, self.iImport = self.NO_CITY, self.NO_CITY
		self.iExportPreview, self.iImportPreview = self.NO_CITY, self.NO_CITY
		
		self.drawContents()
			
	
	def appendBuilderRow(self):
		screen = self.getScreen()
		
		szAssigned = u""
		szYields = localText.getText("TXT_KEY_TRADE_ROUTES_MISSING_YIELD", ())
		if self.iYields != self.NO_YIELD:
			szYields = u"<font=3>%c</font>" % gc.getYieldInfo(self.iYields).getChar()
		szExport = localText.getText("TXT_KEY_TRADE_ROUTES_MISSING_CITY", ())
		if self.iExport != self.NO_CITY:
			szExport = u"%s" % self.player.getCity(self.iExport).getName()
		szImport = localText.getText("TXT_KEY_TRADE_ROUTES_MISSING_CITY", ())
		if self.iImport > self.EUROPE_CITY:
			szImport = u"%s" % self.player.getCity(self.iImport).getName()
		elif self.iImport == self.EUROPE_CITY:
			szImport = localText.getText("TXT_KEY_CONCEPT_EUROPE", ())
							
		szTable = self.TableNames[self.CURRENT_TABLE]
		szColor = u"<color=255,200,50>"
		
		screen.appendTableRow(szTable)
		screen.setTableRowHeight(szTable, 0, self.ROW_HIGHT)
		screen.setTableText(szTable, 0, 0, u"-1", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)
		screen.setTableText(szTable, 1, 0, szColor + szAssigned + u"</color>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)
		screen.setTableText(szTable, 2, 0, szColor + szYields + u"</color>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)
		screen.setTableText(szTable, 3, 0, szColor + szExport + u"</color>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		screen.setTableText(szTable, 4, 0, szColor + szImport + u"</color>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		
		
	def getAssignment(self, iRoute):
		szLabel = u"%c" % CyGame().getSymbolID(FontSymbols.CHECKBOX_CHAR)
		if self.AssignedRoutes[iRoute]:
			szLabel = u"%c" % CyGame().getSymbolID(FontSymbols.CHECKBOX_SELECTED_CHAR)
		
		return szLabel
	

	def toggleAssignment(self, iRow):
		iRoute = self.getRouteByTableRow(iRow).getID()
			
		if self.CURRENT_TABLE == self.BUILDER_TABLE:
			if self.bAssigned:
				self.bAssigned = False
				szLabel = u"%c" % CyGame().getSymbolID(FontSymbols.CHECKBOX_CHAR)
			else:
				self.bAssigned = True
				szLabel = u"%c" % CyGame().getSymbolID(FontSymbols.CHECKBOX_SELECTED_CHAR)
				
		elif self.AssignedRoutes[iRoute]:
			self.AssignedRoutes[iRoute] = False
			szLabel = u"%c" % CyGame().getSymbolID(FontSymbols.CHECKBOX_CHAR)
		else:
			self.AssignedRoutes[iRoute] = True
			szLabel = u"%c" % CyGame().getSymbolID(FontSymbols.CHECKBOX_SELECTED_CHAR)
		
		self.getScreen().setTableText(self.TableNames[self.CURRENT_TABLE], 1, iRow, szLabel, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)
		
		
	def getRouteByTableRow(self, iRow):
		return self.player.getTradeRoute(int(self.getScreen().getTableText(self.TableNames[self.CURRENT_TABLE], 0, iRow)))
		
	
	def yieldTable(self):
		# Yield table
		screen = self.getScreen()
		screen.hide(self.TableNames[self.CURRENT_TABLE])
		
		self.CURRENT_TABLE = self.YIELD_TABLE
		szTable = self.TableNames[self.CURRENT_TABLE]
		
		iRows = int(self.iNumYields**0.5 + 1)
		YIELD_X, YIELD_Y = 0, 0
		BOX_SIZE = self.TABLE_WIDTH / iRows
		
		screen.modifyLabel(self.szTitle, u"<font=3b>" + self.TableLabel[self.CURRENT_TABLE] + u"</font>", CvUtil.FONT_LEFT_JUSTIFY)
		screen.addPanel(szTable, u"", u"", True, True, self.TABLE_X, self.TABLE_Y, self.TABLE_WIDTH, self.TABLE_HEIGHT, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		for iYield in self.YieldList:
			screen.addDDSGFCAt(szTable + "Highlight" + str(iYield), szTable, "Art/Interface/Screens/TradeRoutes/BoxSelected.dds", YIELD_X, YIELD_Y, BOX_SIZE, BOX_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1, False)
			screen.hide(szTable + "Highlight" + str(iYield))
			screen.addDDSGFCAt(szTable + "Box" + str(iYield), szTable, ArtFileMgr.getInterfaceArtInfo("INTERFACE_EUROPE_BOX_PRICE").getPath(), YIELD_X, YIELD_Y, BOX_SIZE, BOX_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1, False)
			screen.setImageButtonAt(szTable + "Selector" + str(iYield), szTable + "Box" + str(iYield), gc.getYieldInfo(iYield).getIcon(), BOX_SIZE / 6, BOX_SIZE / 6, BOX_SIZE * 2 / 3, BOX_SIZE * 2 / 3, WidgetTypes.WIDGET_GENERAL, self.YIELD_LIST_ID, iYield)
		
			YIELD_X += BOX_SIZE
			if (iYield % iRows) + 1 == iRows:
				YIELD_X = 0
				YIELD_Y += BOX_SIZE
		
		self.updateButtons()
		
	
	def cityTable(self, bImport):
		# Export, import table
		screen = self.getScreen()
		
		screen.hide(self.TableNames[self.CURRENT_TABLE])
		self.CURRENT_TABLE = self.EXPORT_TABLE
		if bImport:
			self.CURRENT_TABLE = self.IMPORT_TABLE
			
		szTable = self.TableNames[self.CURRENT_TABLE]
		szYieldHeader = u""
		if self.iYields != self.NO_YIELD:
			szYieldHeader = u"<font=3>%c</font>" % gc.getYieldInfo(self.iYields).getChar()
			
		screen.modifyLabel(self.szTitle, u"<font=3b>" + self.TableLabel[self.CURRENT_TABLE] + u"</font>", CvUtil.FONT_LEFT_JUSTIFY)
		screen.addTableControlGFC(szTable, 5, self.TABLE_X, self.TABLE_Y, self.TABLE_WIDTH, self.TABLE_HEIGHT, True, False, 32, 32, TableStyles.TABLE_STYLE_STANDARD)
		screen.setStyle(szTable, "Table_StandardCiv_Style")
		screen.enableSort(szTable)
		screen.enableSelect(szTable, False)
		screen.setTableColumnHeader(szTable, 0, u"id", 0)
		screen.setTableColumnHeader(szTable, 1, u"<img=%s size=16></img>" % "Art/Interface/Screens/TradeRoutes/Anchor.dds", 32)
		screen.setTableColumnHeader(szTable, 2, localText.getText("TXT_KEY_TRADE_ROUTES_EXPORT_TABLE_2", ()), self.TABLE_WIDTH * 2 / 3 - 80)
		screen.setTableColumnHeader(szTable, 3, u"  %s" % szYieldHeader, 48)
		screen.setTableColumnHeader(szTable, 4, u"  <img=%s size=16></img>" % "Art/Interface/Screens/TradeRoutes/ExportImport.dds", 48)
		
		iI = 0
		if bImport and self.pTransport.getDomainType() == DomainTypes.DOMAIN_SEA:
			#Europe
			screen.appendTableRow(szTable)
			screen.setTableRowHeight(szTable, iI, self.ROW_HIGHT)
			screen.setTableText(szTable, 0, iI, u"-1", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)
			screen.setTableText(szTable, 1, iI, u"%c" % CyGame().getSymbolID(FontSymbols.ANCHOR_EUROPE_CHAR), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)
			screen.setTableText(szTable, 2, iI, localText.getText("TXT_KEY_CONCEPT_EUROPE", ()), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
			screen.setTableText(szTable, 3, iI, u"-", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)
			screen.setTableText(szTable, 4, iI, u"%c" % CyGame().getSymbolID(FontSymbols.IMPORT_CHAR), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)
			
			iI += 1
		
		for city in self.CityList:
			screen.appendTableRow(szTable)
			screen.setTableRowHeight(szTable, iI, self.ROW_HIGHT)
			screen.setTableText(szTable, 0, iI, u"%d" % city.getID(), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)
			screen.setTableText(szTable, 1, iI, self.getHabor(city), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)
			screen.setTableText(szTable, 2, iI, u"%s" % city.getName(), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
			screen.setTableText(szTable, 3, iI, u"%s" % self.getRate(city), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)
			screen.setTableText(szTable, 4, iI, u"%s" % self.getExport(city), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)
			
			iI += 1
			
		self.updateButtons()
	
	#R&R mod, vetiarvind, trade groups - start
	
	
	def populateGroupTable(self, newTable, headerLabel):
		#self.getScreen().modifyLabel(self.szTitle, u"<font=3b>" + self.TableLabel[self.CURRENT_TABLE] + "blaa" + u"</font>", CvUtil.FONT_LEFT_JUSTIFY)		
		
		screen = self.getScreen()		
		screen.hide(self.TableNames[self.CURRENT_TABLE])
		self.CURRENT_TABLE = newTable
		screen.modifyLabel(self.szTitle, u"<font=3b>" + self.TableLabel[self.CURRENT_TABLE] + headerLabel + u"</font>", CvUtil.FONT_LEFT_JUSTIFY)	
		
		
		szTable = self.TableNames[self.CURRENT_TABLE]		
		screen.addTableControlGFC(szTable, 4, self.TABLE_X, self.TABLE_Y, self.TABLE_WIDTH, self.TABLE_HEIGHT, True, False, 32, 32, TableStyles.TABLE_STYLE_STANDARD)
		screen.setStyle(szTable, "Table_StandardCiv_Style")
		screen.enableSort(szTable)
		screen.enableSelect(szTable, False)
		screen.setTableColumnHeader(szTable, 0, u"id", 0)		
		screen.setTableColumnHeader(szTable, 1, u"Routes", 0)		
		screen.setTableColumnHeader(szTable, 2, localText.getText("TXT_KEY_NAME_COL_TRADE_GROUP", ()), self.TABLE_WIDTH / 4)				
		screen.setTableColumnHeader(szTable, 3, localText.getText("TXT_KEY_DESC_COL_TRADE_GROUP", ()), self.TABLE_WIDTH * 3 / 4)		
		iI = 0
		numTg = self.player.getNumTradeGroups()		
		
		for itg in range(numTg):
			tg = self.player.getTradeGroup(itg)
			
			iRoutes = []			
			iRouteDetails = {}
			iRC = tg.getRouteCount()
			
			for iRt in range(iRC):
				rt = tg.getRouteByIndex(iRt)				
				srcCityName = rt.getSourceCityName()
				destCityName = rt.getDestinationCityName()
				if len(srcCityName)==0 or len(destCityName) == 0:
					continue
				iRoutes.append(str(rt.getSourceCity().iID) + " " + str(rt.getDestinationCity().iID) + " " + str(rt.getYield()))	
				descKeyStr = srcCityName + "-" + destCityName				
				if descKeyStr in iRouteDetails:
					iRouteDetails[descKeyStr].append(u"<font=3>%c</font>" % gc.getYieldInfo(rt.getYield()).getChar())
				else:
					iRouteDetails[descKeyStr] = [u"<font=3>%c</font>" % gc.getYieldInfo(rt.getYield()).getChar()]
				
			routeIdsStr = ','.join(str(x) for x in iRoutes)			
			routeDetailsStr = ','.join("%s%s" % (''.join(val), key) for (key, val) in iRouteDetails.iteritems() )
			screen.appendTableRow(szTable)
			screen.setTableRowHeight(szTable, iI, self.ROW_HIGHT)
			screen.setTableText(szTable, 0, iI, u"%d" % tg.getID(), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)
			screen.setTableText(szTable, 1, iI, u"%s" % routeIdsStr, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)
			screen.setTableText(szTable, 2, iI, u"%s" % tg.getName(), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)			
			screen.setTableText(szTable, 3, iI, u"%s" % routeDetailsStr, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
			
			iI += 1		
		self.updateButtons()
		return
	
	# displays the load group table
	def loadGroupTable(self):			
		self.populateGroupTable(self.LOAD_GROUP_TABLE, localText.getText("TXT_KEY_HELP_LOAD_TRADE_GROUP", ()))		
		return		
		
	#concat route id's in form (s1 d1 y1,s2 d2 y2...) and pass into popup for save
	def saveGroupPopup(self):								
		idlist=[]
		
		for pRoute in self.CurrentList:
			if self.AssignedRoutes[pRoute.getID()]:
				idlist.append(str(pRoute.getSourceCity().iID) + " " + str(pRoute.getDestinationCity().iID) + " " + str(pRoute.getYield()))

		idstr =','.join(str(x) for x in idlist)		
		popupInfo = CyPopupInfo()		
		popupInfo.setText(idstr) 
		
		popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_SAVE_TRADEGROUP)
		CyInterface().addPopup(popupInfo, gc.getGame().getActivePlayer(), True, False)
		return	
		
	def deleteGroupTable(self):		
		self.populateGroupTable(self.DELETE_GROUP_TABLE, localText.getText("TXT_KEY_HELP_DELETE_TRADE_GROUP", ()))
		return	
		
	#Loads selected routes in trade group into the main trade screen
	def loadSelectedGroup(self, iRow):		
		for pRoute in self.CurrentList:			
			self.AssignedRoutes[pRoute.getID()] = False
			
		tableCellValue = str(self.getScreen().getTableText(self.TableNames[self.CURRENT_TABLE], 1, iRow))
		
		routeTokens = tableCellValue.split(",")
				
		for token in routeTokens:
			spl = token.split(" ")			
			for pRoute in self.CurrentList:				
				if str(pRoute.getSourceCity().iID) == spl[0] and str(pRoute.getDestinationCity().iID) == spl[1] and str(pRoute.getYield()) == spl[2]:								
					self.AssignedRoutes[pRoute.getID()] = True
		
		self.routesTable(False)
		self.updateRoutes()
		
		
		return
		
	#"deletes selected row"		
	def deleteSelectedGroup(self, iRow):		
		
			
		tableCellValue = str(self.getScreen().getTableText(self.TableNames[self.CURRENT_TABLE], 0, iRow))
		
		self.player.removeTradeRouteGroup(int(tableCellValue))
		
		self.routesTable(False)
		
		return
		
	#R&R mod, vetiarvind, trade groups - end
	
	def getHabor(self, city):
		szLabel = u"%c" % CyGame().getSymbolID(FontSymbols.NO_ANCHOR_CHAR)
		if city.isCoastal(gc.getMIN_WATER_SIZE_FOR_OCEAN()):
			szLabel = u"%c" % CyGame().getSymbolID(FontSymbols.ANCHOR_CHAR)
		
		return szLabel
	
	
	def getRate(self, city):
		szLabel = u"-"
	
		if self.iYields != self.NO_YIELD:
			iRate = city.getYieldRate(self.iYields)
			if iRate > 0:
				szLabel = u"<color=63,255,63>+" + str(iRate) + u"</color>"
			elif iRate < 0:
				szLabel = u"<color=255,63,63>" + str(iRate) + u"</color>"
		
		return szLabel
	
	
	def getExport(self, city):
		szLabel = u"-"
		
		if self.iYields != self.NO_YIELD:
			bExport = city.isExport(self.iYields)
			bImport = city.isImport(self.iYields)
			
			if bExport and bImport:
				szLabel = u"%c" % CyGame().getSymbolID(FontSymbols.EXPORT_IMPORT_CHAR)
			elif bExport:
				szLabel = u"%c" % CyGame().getSymbolID(FontSymbols.EXPORT_CHAR)
			elif bImport:
				szLabel = u"%c" % CyGame().getSymbolID(FontSymbols.IMPORT_CHAR)

		return szLabel
		

	def getCityByTableRow(self, iRow):
		return int(self.getScreen().getTableText(self.TableNames[self.CURRENT_TABLE], 0, iRow))
		
		
	def selectExport(self, iRow):
		self.iExport = self.getCityByTableRow(iRow)
		if self.iImport != self.NO_CITY:
			if self.iExport == self.iImport:
				self.iImport = self.NO_CITY
				self.iImportPreview = self.NO_CITY
		
		self.routesTable(False)
	
	
	def selectImport(self, iRow):
		self.iImport = self.getCityByTableRow(iRow)
		if self.iExport != self.NO_CITY:
			if self.iImport == self.iExport:
				self.iExport = self.NO_CITY
				self.iExportPreview = self.NO_CITY
		
		self.routesTable(False)
				
		
	def updatePreview(self):
		# Update preview pictures
		screen = self.getScreen()
		
		if self.iYields != self.NO_YIELD:
			screen.hide(self.szPreviewYields + "Title")
			screen.show(self.szPreviewYields + "Cancel")
			screen.addDDSGFC(self.szPreviewYields, gc.getYieldInfo(self.iYields).getIcon(), self.PREVIEW_X + self.PREVIEW_WIDTH / 4, self.PREVIEW_Y + self.PREVIEW_HEIGHT / 4, self.PREVIEW_WIDTH / 2, self.PREVIEW_WIDTH / 2, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.moveToFront(self.szPreviewYields + "Border" + str(0))
		else:
			screen.hide(self.szPreviewYields)
			screen.hide(self.szPreviewYields + "Cancel")
			screen.show(self.szPreviewYields + "Title")
			
		CURRENT_X = self.PREVIEW_X + self.PREVIEW_WIDTH + self.STANDARD_MARGIN / 2 
		
		if self.iExport != self.NO_CITY:
			if self.iExport != self.iExportPreview:
				self.iExportPreview = self.iExport
				screen.show(self.szPreviewExport + "Banner")
				screen.show(self.szPreviewExport + "Cancel")
				szLabel = u"<font=2>" + u"%s" % self.player.getCity(self.iExport).getName() + u"</font>"
				if CyInterface().determineWidth(szLabel) > self.PREVIEW_WIDTH - 22:
					screen.setLabelAt(self.szPreviewExport + "Label", self.szPreviewExport + "Banner", szLabel, CvUtil.FONT_LEFT_JUSTIFY, 3, 10, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
				else:
					screen.setLabelAt(self.szPreviewExport + "Label", self.szPreviewExport + "Banner", szLabel, CvUtil.FONT_CENTER_JUSTIFY, (self.PREVIEW_WIDTH - 20) / 2, 10, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
				screen.addPlotGraphicGFC(self.szPreviewExport, CURRENT_X + 6, self.PREVIEW_Y + 6, self.PREVIEW_WIDTH - 12, self.PREVIEW_HEIGHT - 12, self.player.getCity(self.iExport).plot(), 350, True, WidgetTypes.WIDGET_GENERAL, -1, -1)
				screen.moveToFront(self.szPreviewExport + "Border" + str(0))
		else:
			screen.hide(self.szPreviewExport)
			screen.hide(self.szPreviewExport + "Banner")
			screen.hide(self.szPreviewExport + "Cancel")
					
		CURRENT_X += self.PREVIEW_WIDTH + self.STANDARD_MARGIN / 2
		
		if self.iImport > self.EUROPE_CITY:
			if self.iImport != self.iImportPreview:
				self.iImportPreview = self.iImport
				screen.show(self.szPreviewImport + "Banner")
				screen.show(self.szPreviewImport + "Cancel")
				szLabel = u"<font=2>" + u"%s" % self.player.getCity(self.iImport).getName() + u"</font>"
				if CyInterface().determineWidth(szLabel) > self.PREVIEW_WIDTH - 22:
					screen.setLabelAt(self.szPreviewImport + "Label", self.szPreviewImport + "Banner", u"<font=2>" + u"%s" % self.player.getCity(self.iImport).getName() + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, 3, 10, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
				else:
					screen.setLabelAt(self.szPreviewImport + "Label", self.szPreviewImport + "Banner", u"<font=2>" + u"%s" % self.player.getCity(self.iImport).getName() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, (self.PREVIEW_WIDTH - 20) / 2, 10, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
				screen.addPlotGraphicGFC(self.szPreviewImport, CURRENT_X + 6, self.PREVIEW_Y + 6, self.PREVIEW_WIDTH - 12, self.PREVIEW_HEIGHT - 12, self.player.getCity(self.iImport).plot(), 350, True, WidgetTypes.WIDGET_GENERAL, -1, -1)
				screen.moveToFront(self.szPreviewImport + "Border" + str(0))
		elif self.iImport == self.EUROPE_CITY:
			screen.show(self.szPreviewImport + "Banner")
			screen.show(self.szPreviewImport + "Cancel")
			screen.setLabelAt(self.szPreviewImport + "Label", self.szPreviewImport + "Banner", u"<font=2>" + localText.getText("TXT_KEY_CONCEPT_EUROPE", ()) + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, (self.PREVIEW_WIDTH - 10) / 2, 10, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.addDDSGFC(self.szPreviewImport, "Art/Interface/Screens/TradeRoutes/EuropePreview.dds", CURRENT_X + 6, self.PREVIEW_Y + 6, self.PREVIEW_WIDTH - 12, self.PREVIEW_HEIGHT - 12, WidgetTypes.WIDGET_GENERAL, -1, -1 )
			screen.moveToFront(self.szPreviewImport + "Border" + str(0))
		else:
			screen.hide(self.szPreviewImport)
			screen.hide(self.szPreviewImport + "Banner")
			screen.hide(self.szPreviewImport + "Cancel")
		
	
	def updateButtons(self):
		# Updates button panel
		screen = self.getScreen()
		
		szSelectButton = "Art/Interface/Screens/TradeRoutes/Select.dds"
		if self.bSelected:
			szSelectButton = "Art/Interface/Screens/TradeRoutes/Deselect.dds"
		szDangerButton = "Art/Interface/Screens/TradeRoutes/IgnoreDangerOff.dds"
		if self.bDanger:
			szDangerButton = "Art/Interface/Screens/TradeRoutes/IgnoreDanger.dds"
		szRoutesButton = "Art/Interface/Screens/TradeRoutes/AddRoute.dds"
		szReturnButton = "Art/Interface/Screens/TradeRoutes/Return.dds"
		szHelpButton = "Art/Interface/Screens/TradeRoutes/Help.dds"
		
		screen.clearMultiList(self.szButtonPanel)
		
		if self.CURRENT_TABLE in [self.EXISTING_ROUTES, self.LIMITED_ROUTES]:
			screen.appendMultiListButton(self.szButtonPanel, szSelectButton, 0, WidgetTypes.WIDGET_GENERAL, self.SELECT_ID, -1, False)
		elif self.CURRENT_TABLE == self.BUILDER_TABLE:
			screen.appendMultiListButton(self.szButtonPanel, szRoutesButton, 0, WidgetTypes.WIDGET_GENERAL, self.ROUTES_ID, -1, False)
			if self.isSelectionReady():
				screen.enableMultiListPulse(self.szButtonPanel, True, 0, 0)
			else:
				screen.disableMultiListButton(self.szButtonPanel, 0, 0, szRoutesButton)
		else:
			screen.appendMultiListButton(self.szButtonPanel, szReturnButton, 0, WidgetTypes.WIDGET_GENERAL, self.RETURN_ID, -1, False)
		
		screen.appendMultiListButton(self.szButtonPanel, szDangerButton, 0, WidgetTypes.WIDGET_GENERAL, self.DANGER_ID, -1, False)
		screen.appendMultiListButton(self.szButtonPanel, szHelpButton, 0, WidgetTypes.WIDGET_PEDIA_DESCRIPTION, CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT, gc.getInfoTypeForString("CONCEPT_TRADE_ROUTE"), False)
	
	def toggleSelect(self):
		self.bSelected = not self.bSelected
		for pRoute in self.CurrentList:
			self.AssignedRoutes[pRoute.getID()] = self.bSelected
		self.routesTable(False)
		
		
	def toggleDanger(self):
		self.bDanger = not self.bDanger
		CyMessageControl().sendDoCommand(self.pTransport.getID(), CommandTypes.COMMAND_IGNORE_DANGER, self.bDanger, -1, False)
		self.updateButtons()

		
	def addSelection(self):
		screen = self.getScreen()
		
		if not self.isSelectionReady():
			return
			
		CyMessageControl().sendDoTask(self.iExport, TaskTypes.TASK_YIELD_EXPORT, self.iYields, True, False, False, False, False)
		if self.iImport > self.EUROPE_CITY:
			CyMessageControl().sendDoTask(self.iImport, TaskTypes.TASK_YIELD_IMPORT, self.iYields, True, False, False, False, False)
				
	
	def isSelectionEmpty(self):
		return (self.iYields == self.NO_YIELD and self.iExport == self.NO_CITY and self.iImport == self.NO_CITY)
		
	
	def isSelectionReady(self):
		return (self.iYields != self.NO_YIELD and self.iExport != self.NO_CITY and self.iImport != self.NO_CITY)
		
		
	def handleInput(self, inputClass):
		if inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED:
			if inputClass.getFunctionName() in [self.TableNames[self.EXISTING_ROUTES], self.TableNames[self.LIMITED_ROUTES]]:
				self.toggleAssignment(inputClass.getMouseY())
			elif inputClass.getFunctionName() == self.TableNames[self.EXPORT_TABLE]:
				self.selectExport(inputClass.getMouseY())
			elif inputClass.getFunctionName() == self.TableNames[self.IMPORT_TABLE]:
				self.selectImport(inputClass.getMouseY())
			elif inputClass.getFunctionName() == self.szButtonPanel:
				if inputClass.getData1() == self.SELECT_ID:
					self.toggleSelect()
				elif inputClass.getData1() == self.DANGER_ID:
					self.toggleDanger()
				elif inputClass.getData1() == self.ROUTES_ID:
					self.addSelection()
				elif inputClass.getData1() == self.RETURN_ID:
					self.routesTable(False)
			#R&R mod, vetiarvind, trade groups - START
			elif inputClass.getFunctionName() == self.TableNames[self.LOAD_GROUP_TABLE]:							
				self.loadSelectedGroup(inputClass.getMouseY())									
			elif inputClass.getFunctionName() == self.TableNames[self.DELETE_GROUP_TABLE]:							
				self.deleteSelectedGroup(inputClass.getMouseY())
			#R&R mod, vetiarvind, trade groups - END

		elif inputClass.getNotifyCode() == NotifyCode.NOTIFY_CURSOR_MOVE_ON:
			if inputClass.getFunctionName() == self.TableNames[self.YIELD_TABLE] + "Selector":
				self.getScreen().show(self.TableNames[self.YIELD_TABLE] + "Highlight" + str(inputClass.getID()))
			elif inputClass.getFunctionName() == self.szPreviewYields + "Border":
				self.getScreen().show(self.szPreviewYields + "Highlight")
			elif inputClass.getFunctionName() == self.szPreviewExport + "Border":
				self.getScreen().show(self.szPreviewExport + "Highlight")
			elif inputClass.getFunctionName() == self.szPreviewImport + "Border":
				self.getScreen().show(self.szPreviewImport + "Highlight")
				
		elif inputClass.getNotifyCode() == NotifyCode.NOTIFY_CURSOR_MOVE_OFF:
			if inputClass.getFunctionName() == self.TableNames[self.YIELD_TABLE] + "Selector":
				self.getScreen().hide(self.TableNames[self.YIELD_TABLE] + "Highlight" + str(inputClass.getID()))
			elif inputClass.getFunctionName() == self.szPreviewYields + "Border":
				self.getScreen().hide(self.szPreviewYields + "Highlight")
			elif inputClass.getFunctionName() == self.szPreviewExport + "Border":
				self.getScreen().hide(self.szPreviewExport + "Highlight")
			elif inputClass.getFunctionName() == self.szPreviewImport + "Border":
				self.getScreen().hide(self.szPreviewImport + "Highlight")
				
		elif inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED:
			if inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL:
				if inputClass.getData1() == self.YIELD_TABLE_ID:
					self.yieldTable()
				elif inputClass.getData1() == self.EXPORT_TABLE_ID:
					self.cityTable(False)
				elif inputClass.getData1() == self.IMPORT_TABLE_ID:
					self.cityTable(True)
					
				elif inputClass.getData1() == self.YIELD_LIST_ID:
					self.iYields = inputClass.getData2()
					self.routesTable(False)
				
				elif inputClass.getData1() == self.CLEAR_YIELD_ID:
					self.iYields = self.NO_YIELD
					self.routesTable(False)
				elif inputClass.getData1() == self.CLEAR_EXPORT_ID:
					self.iExport = self.NO_CITY
					self.iExportPreview = self.NO_CITY
					self.routesTable(False)
				elif inputClass.getData1() == self.CLEAR_IMPORT_ID:
					self.iImport = self.NO_CITY
					self.iImportPreview = self.NO_CITY
					self.routesTable(False)
				#R&R mod, vetiarvind, trade groups - START
				elif inputClass.getData1() == self.LOAD_GROUP_ID:
					self.loadGroupTable()
				elif inputClass.getData1() == self.SAVE_GROUP_ID:					
					self.saveGroupPopup()				
				elif inputClass.getData1() == self.DELETE_GROUP_ID:					
					self.deleteGroupTable()				
				#R&R mod, vetiarvind, trade groups - END
					
		return 0
		

	def getWidgetHelp(self, argsList):
		iScreen, eWidgetType, iData1, iData2, bOption = argsList
		player = gc.getPlayer(gc.getGame().getActivePlayer())
	
		if eWidgetType == WidgetTypes.WIDGET_GENERAL:
			if iData1 == self.CLEAR_YIELD_ID:
				return localText.getText("TXT_KEY_TRADE_ROUTES_CLEAR_SELECTION_HELP", ())
			elif iData1 == self.CLEAR_EXPORT_ID:
				return localText.getText("TXT_KEY_TRADE_ROUTES_CLEAR_SELECTION_HELP", ())
			elif iData1 == self.CLEAR_IMPORT_ID:
				return localText.getText("TXT_KEY_TRADE_ROUTES_CLEAR_SELECTION_HELP", ())
			elif iData1 == self.SELECT_ID:
				if self.bSelected:
					return localText.getText("TXT_KEY_TRADE_ROUTES_DESELECT_ROUTES_HELP", ())
				else:
					return localText.getText("TXT_KEY_TRADE_ROUTES_SELECT_ROUTES_HELP", ())
			elif iData1 == self.DANGER_ID:
				if self.bDanger:
					return localText.getText("TXT_KEY_TRADE_ROUTES_CANCEL_IGNORE_DANGER_HELP", ())
				else:
					return localText.getText("TXT_KEY_TRADE_ROUTES_IGNORE_DANGER_HELP", ())
			elif iData1 == self.ROUTES_ID:
				return localText.getText("TXT_KEY_TRADE_ROUTES_ADD_SELECTION_HELP", ())
			elif iData1 == self.RETURN_ID:
				return localText.getText("TXT_KEY_TRADE_ROUTES_RETURN_HELP", ())
			elif iData1 == self.YIELD_TABLE_ID:
				return localText.getText("TXT_KEY_TRADE_ROUTES_YIELD_HELP", ())
			elif iData1 == self.EXPORT_TABLE_ID:
				return localText.getText("TXT_KEY_TRADE_ROUTES_EXPORT_HELP", ())
			elif iData1 == self.IMPORT_TABLE_ID:
				return localText.getText("TXT_KEY_TRADE_ROUTES_IMPORT_HELP", ())
				
		if eWidgetType == WidgetTypes.WIDGET_PEDIA_DESCRIPTION:
			if iData1 == CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT:
				return localText.getText("TXT_KEY_TRADE_ROUTES_HELP_HELP", ())
		
		return u""
	
	
	def update(self, fDelta):
		if (CyInterface().isDirty(InterfaceDirtyBits.TradeRoutesAdvisor_DIRTY_BIT)):
			CyInterface().setDirty(InterfaceDirtyBits.TradeRoutesAdvisor_DIRTY_BIT, False)
			self.updateRoutes()
				
		return 0
	
	
	def getScreen(self):
		return CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.TRADE_ROUTES_ADVISOR)
	
	
	def getNextWidgetName(self):
		szName = self.WIDGET_ID + str(self.nWidgetCount)
		self.nWidgetCount += 1
		return szName

		
	def onClose(self) :
		for iRoute, bAssigned in self.AssignedRoutes.iteritems():
			CyMessageControl().sendDoCommand(self.pTransport.getID(), CommandTypes.COMMAND_ASSIGN_TRADE_ROUTE, iRoute, bAssigned, False)
		
		return 0
	
