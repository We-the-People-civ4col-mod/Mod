## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
##
## Europe Screen 5.01 - TAC
## by koma13 
## converted to Port Royal Screen by ray

## (+ Multiple rows of Yields by Robert Surcouf) edit 17 dec. 2011
## (+ Price dependent tax increase by vetiarvind) edit 26 July 2014
## (+ Navigation sectors by vetiarvind) edit 27 July 2014
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import sdToolKit
import random # R&R, vetiarvind, Navigation sectors

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

# Multiplayer Events
SEND_TO_NEW_WORLD_CITY = 0
SEND_TO_EAST_OR_WEST = 1
RECALL_TO_EUROPE = 2
SELL_SHIP_IN_PORT_ROYAL = 3
LIFT_BOYCOTT_IN_PORT_ROYAL = 4
RECALL_TO_PORT_ROYAL = 8

# Sound Types
INIT_SOUND = 0
UNIT_SOUND_SELECT = 1
UNIT_SOUND_ORDER = 2
		
class CvPortRoyalScreen:

	def __init__(self):
		self.WIDGET_ID = "PortRoyalScreenWidget"
		self.nWidgetCount = 0

		self.UNIT_BUTTON_ID = 1
		self.UNIT_CARGO_BUTTON_ID = 2
		self.BUY_YIELD_BUTTON_ID = 3
		self.YIELD_CARGO_BUTTON_ID = 4
		self.BUY_UNIT_BUTTON_ID = 5
		self.DOCK_BUTTON_ID = 6
		self.SAIL_TO_NEW_WORLD = 7
		self.SELL_ALL = 8
		self.LOAD_ALL = 9
		self.HELP_CROSS_RATE = 10
		self.TREASURY_ID = 11
		self.TRAVEL_INFO = 12
		self.TRADE_LOG = 13
		self.SAIL_EXEC = 14
		self.SELL_SHIP = 15
		self.SELL_SHIP_EXEC = 16
		self.PREVIEW_MODE = 17
		self.RECALL = 18
		self.BOYCOTT = 19
		self.BOYCOTT_EXEC = 20
		self.CLOSE_DIALOG = 21
		self.SAIL_EAST = 22
		self.SAIL_WEST = 23
		# R&R, Robert Surcouf, No More Variables Hidden game option START
		self.HELP_TAX_RATE = 24
		# R&R, Robert Surcouf, No More Variables Hidden game option END
		# R&R, vetiarvind, Navigation Sectors - START
		self.SAIL_EAST_NFrigid = 25
		self.SAIL_EAST_NTemperate = 26
		self.SAIL_EAST_NTropic = 27
		self.SAIL_EAST_STropic = 28
		self.SAIL_EAST_STemperate = 29
		self.SAIL_EAST_SFrigid = 30
		self.SAIL_WEST_NFrigid = 31
		self.SAIL_WEST_NTemperate = 32
		self.SAIL_WEST_NTropic = 33
		self.SAIL_WEST_STropic = 34
		self.SAIL_WEST_STemperate = 35
		self.SAIL_WEST_SFrigid = 36
		self.SectorNames = dict([(0, 'NFrigid'), (1, 'NTemperate'), (2, 'NTropic'), (3, 'STropic'), (4, 'STemperate'), (5, 'SFrigid')])
		# R&R, vetiarvind, Navigation Sectors - END

		self.bBookIntro, self.bBookOutro = false, false
		
	
	def getScreen(self):
		return CyGInterfaceScreen("PortRoyalScreen", CvScreenEnums.PORT_ROYAL_SCREEN)

	
	def interfaceScreen(self):
		if ( CyGame().isPitbossHost() ):
			return

		if gc.getPlayer(gc.getGame().getActivePlayer()).getParent() == PlayerTypes.NO_PLAYER:
			return

		screen = self.getScreen()
		if screen.isActive():
			return

		## Port Royal Screen Configuration START
		# To disable a feature set value to 0

		self.iWinterScreen = 9			# min turns between 2 winter screens
		self.iSellShip = 6				# base factor for ship selling price (0 to 12)
		self.iBoycott = 6				# Boycott price base factor
		self.iVideo = 1					# show sea gulls every x turns
		self.iPlotDebug = 0				# show plot coordinates in transport tool tips
		
		## Port Royal Screen Configuration END
	
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		self.playerEurope = gc.getPlayer(player.getParent())
		self.iThisWinter = 0
		self.iSoundID = 0
		self.bBookIntro, self.bBookOutro = false, false
		
		if (not sdToolKit.sdEntityExists( 'komaScreens', player.getID())): 
			sdToolKit.sdEntityInit('komaScreens', player.getID(), { 'TradeBox': false, 'LastWinter': 0, 'PreviewMode': "INTERFACE_DOMESTIC_ADVISOR", 'DealFailed': -1 })

		bShowTradeBox = sdToolKit.sdGetVal('komaScreens', player.getID(), 'TradeBox')
		self.PortRoyalUnitsList = []
		self.OutboundUnitsList = []
		self.EuropePlotList = []
		self.PreviewPlotList = []
				
		# Winter
		if self.iWinterScreen > 0:
			iCurrentTurn = CyGame().getGameTurn()
			
			GameSpeedInfo = gc.getGameSpeedInfo(gc.getGame().getGameSpeedType())
			iTurnIndex = 0
			iTotalTurnsPerIncrement = 0
			
			for i in range(GameSpeedInfo.getNumTurnIncrements()):
				iTotalTurnsPerIncrement += GameSpeedInfo.getGameTurnInfo(i).iNumGameTurnsPerIncrement
				if iTotalTurnsPerIncrement >= iCurrentTurn:
					break
				iTurnIndex += 1
			
			January = localText.getText("TXT_KEY_MONTH_JANUARY", ())
			February = localText.getText("TXT_KEY_MONTH_FEBRUARY", ())
			December = localText.getText("TXT_KEY_MONTH_DECEMBER", ())
			
			szDate = CyGameTextMgr().getTimeStr(iCurrentTurn, true)

			January = localText.getText("TXT_KEY_MONTH_JANUARY", ())
			February = localText.getText("TXT_KEY_MONTH_FEBRUARY", ())
			December = localText.getText("TXT_KEY_MONTH_DECEMBER", ())

			if (GameSpeedInfo.getGameTurnInfo(iTurnIndex).iMonthIncrement != 12):
				if (January in szDate or February in szDate or December in szDate):
					iLastWinterDiff = iCurrentTurn - sdToolKit.sdGetVal('komaScreens', player.getID(), 'LastWinter')
					if (iLastWinterDiff >= self.iWinterScreen or iLastWinterDiff == 0) and (iCurrentTurn != 0):
						self.iThisWinter = 1
						sdToolKit.sdSetVal('komaScreens', player.getID(), 'LastWinter', iCurrentTurn)

			elif (iCurrentTurn % self.iWinterScreen == 0) and (iCurrentTurn != 0):
				self.iThisWinter = 1
			
		# Yield list
		self.YieldList = []
	
		for iYield in range(YieldTypes.NUM_YIELD_TYPES):
			kYield = gc.getYieldInfo(iYield)
			if kYield.isCargo():
				self.YieldList.append(iYield)
		
		screen.setRenderInterfaceOnly(False)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)
		
		self.XResolution = screen.getXResolution()
		self.YResolution = screen.getYResolution()
		
		self.STANDARD_MARGIN = 17
		self.SKY_H = self.YResolution * 23 / 100
		
		self.NUM_YIELDS = len(self.YieldList)
		## R&R, Robert Surcouf,  Multiple rows of Yields if necessary START
		#self.NUM_YIELDS_IN_A_ROW = 26 -> Two rows if num > 26
		self.NUM_YIELDS_IN_A_ROW = 33
		
		# Window reduction for small resolutions
		# Sometimes yield boxes are too large ...
		#self.Yield_Window_Reduction_Percent = 80 - (self.XResolution-1000) / 50
		self.Yield_Window_Reduction_Percent = 100
		# Yield Window Reduction Percent. Larger screens should have a lower percent ! 80% for 1024*768 seems fine !
		self.BOX_X = (self.XResolution - (self.XResolution / self.NUM_YIELDS_IN_A_ROW) * self.NUM_YIELDS_IN_A_ROW ) / 2 
		
		#self.BOX_W = self.XResolution / self.NUM_YIELDS
		self.BOX_W = (self.XResolution / self.NUM_YIELDS_IN_A_ROW) * self.Yield_Window_Reduction_Percent / 100
		##self.BOX_W = (self.XResolution / self.NUM_YIELDS_IN_A_ROW
		#self.BOX_H = self.BOX_W * 6 / 5  
		self.BOX_H = self.BOX_W * 6 / 5 - ((self.XResolution * 9) / (self.YResolution * 16) )* self.BOX_W / 10 
		##self.BOX_X = (self.XResolution - self.BOX_W * self.NUM_YIELDS_IN_A_ROW ) / 2  
		#self.BOX_X = (self.XResolution - self.BOX_W * self.NUM_YIELDS) / 2
		self.BOX_Y = self.YResolution -3* self.BOX_H  -self.STANDARD_MARGIN + 25
		#self.BOX_Y = self.YResolution - self.BOX_H - self.STANDARD_MARGIN
		## R&R, Robert Surcouf,  Multiple rows of Yields if necessary END 
		self.INBOUND_SHIP_W = self.YResolution / 20
		self.INBOUND_SHIP_H = self.INBOUND_SHIP_W * 7 / 4
		self.INBOUND_X = 0
		self.INBOUND_Y = self.SKY_H - self.INBOUND_SHIP_H * 2 / 3
		self.INBOUND_W = self.XResolution / 2
		self.INBOUND_H = self.INBOUND_SHIP_H
		
		self.OUTBOUND_SHIP_W = self.YResolution / 10
		self.OUTBOUND_SHIP_H = self.OUTBOUND_SHIP_W * 7 / 4
		self.OUTBOUND_X = 0
		self.OUTBOUND_Y = self.INBOUND_Y + self.INBOUND_H / 2
		self.OUTBOUND_W = self.XResolution  * 2 / 3
		self.OUTBOUND_H = self.OUTBOUND_SHIP_H * 7 / 4
		
		self.CARGO_ICON_SIZE = self.XResolution / 26
		self.CARGO_SPACING  = self.CARGO_ICON_SIZE + 2

		self.INPORT_SHIP_W = self.YResolution / 3
		self.INPORT_SHIP_H = self.INPORT_SHIP_W * 7 / 4
		self.INPORT_X = -5
		self.INPORT_Y = self.OUTBOUND_Y + self.OUTBOUND_H - self.INPORT_SHIP_H / 4
		self.INPORT_W = self.XResolution + 10
		self.INPORT_H = self.YResolution - self.INPORT_Y - 23
				
		self.DOCK_UNIT_W = self.YResolution / 16
		self.DOCK_UNIT_H = self.DOCK_UNIT_W * 2
		self.DOCK_X = self.XResolution * 30 / 48
		self.DOCK_Y = self.YResolution * 13 / 48
		self.DOCK_W = self.XResolution - self.DOCK_X
		self.DOCK_H = self.DOCK_UNIT_H
		
		self.RECRUIT_H = 100 + ((self.YResolution - 768) / 11)
		self.RECRUIT_W = self.RECRUIT_H / 2
		# RaR Change for 4 Immigrants waiting
		#self.RECRUIT_X = self.XResolution * 61 / 80
		self.RECRUIT_X = self.XResolution * 61 / 82
		self.RECRUIT_Y = self.BOX_Y - self.RECRUIT_H
		
		self.TRADE_X = 0
		self.TRADE_Y = self.OUTBOUND_Y + self.OUTBOUND_H * 3 / 4
		self.TRADE_H = self.BOX_Y - self.TRADE_Y - self.STANDARD_MARGIN * 2
		self.TRADE_W = 280
		
		self.IMMIGRANT_BAR_W = 200
		
		self.DIALOG_TABLE_W = 256
		self.MAP_SIZE = 360
		
		self.DIALOG_W = self.DIALOG_TABLE_W + self.MAP_SIZE + self.STANDARD_MARGIN * 3
		self.DIALOG_H = self.MAP_SIZE + self.STANDARD_MARGIN * 2
		self.DIALOG_X = self.XResolution / 2 - self.DIALOG_W / 2
		self.DIALOG_Y = self.INPORT_Y + self.INPORT_H / 2 - self.DIALOG_H
		
		self.DIALOG_TABLE_X = self.DIALOG_X + self.DIALOG_W - self.DIALOG_TABLE_W - self.STANDARD_MARGIN
		self.DIALOG_TABLE_Y = self.DIALOG_Y + self.STANDARD_MARGIN
		self.DIALOG_TABLE_H = self.DIALOG_H - self.STANDARD_MARGIN * 2
		
		self.MAP_X = self.STANDARD_MARGIN
		self.MAP_Y = self.STANDARD_MARGIN
			
		self.BORDER_SIZE = 3
		
		self.SELL_SHIP_W = self.MAP_SIZE + self.STANDARD_MARGIN * 2
		self.SELL_SHIP_H = self.SELL_SHIP_W + self.SELL_SHIP_W / 20
		self.SELL_SHIP_X = (self.XResolution - self.SELL_SHIP_W) / 2
		self.SELL_SHIP_Y = (self.BOX_Y - self.SELL_SHIP_H) / 2
				
		self.SELL_SHIP_IMAGE_W = self.MAP_SIZE
		self.SELL_SHIP_IMAGE_H = self.SELL_SHIP_IMAGE_W * 3 / 4
		self.SELL_SHIP_IMAGE_X = self.STANDARD_MARGIN
		self.SELL_SHIP_IMAGE_Y = self.STANDARD_MARGIN
		
		self.SELL_SHIP_BUTTON_W = self.SELL_SHIP_IMAGE_W
		self.SELL_SHIP_BUTTON_H = self.STANDARD_MARGIN * 2
		self.SELL_SHIP_BUTTON_X = self.SELL_SHIP_X + self.SELL_SHIP_W / 2 - self.SELL_SHIP_BUTTON_W / 2
		self.SELL_SHIP_BUTTON_Y = self.SELL_SHIP_Y + self.SELL_SHIP_H - self.STANDARD_MARGIN / 2 - self.SELL_SHIP_BUTTON_H
		
		self.SELL_SHIP_MESSAGE_Y = self.SELL_SHIP_Y + self.SELL_SHIP_IMAGE_Y + self.SELL_SHIP_IMAGE_H + self.STANDARD_MARGIN / 2
		self.SELL_SHIP_MESSAGE_H = self.SELL_SHIP_BUTTON_Y - self.SELL_SHIP_MESSAGE_Y - self.STANDARD_MARGIN / 2
		
		#
		
		screen.setDimensions(0, 0, self.XResolution, self.YResolution)
		screen.showWindowBackground(False)
	
		# Water Animation
		screen.addUnitGraphicGFC("WaterAnim_Widget", gc.getInfoTypeForString("UNIT_CARAVEL"), -1, 0, 0, self.XResolution, self.XResolution, WidgetTypes.WIDGET_GENERAL, -1, -1, 0, 0, 0, true)
	
		# show background
		if not self.iThisWinter:
			if self.iVideo > 0 and iCurrentTurn % self.iVideo == 0 and not iCurrentTurn == 0 and not CyUserProfile().getGraphicOption(gc.getInfoTypeForString("GRAPHICOPTION_NO_MOVIES")):
				screen.playMovie("Art/Interface/Screens/Europe/intro.bik", 0, 0, self.XResolution, self.YResolution * 23 / 100, 0)
				screen.addDDSGFC("PortRoyalScreenBackground", ArtFileMgr.getInterfaceArtInfo("INTERFACE_PORT_ROYAL_BACKGROUND").getPath(), 0, 0, self.XResolution, self.YResolution, WidgetTypes.WIDGET_GENERAL, -1, -1 )
			else:
				screen.addDDSGFC("PortRoyalScreenBackground", "Art/Interface/Screens/Port_Royal/FullBackground.dds", 0, 0, self.XResolution, self.YResolution, WidgetTypes.WIDGET_GENERAL, -1, -1 )
			
		else:
			screen.addDDSGFC("PortRoyalScreenBackground", "Art/Interface/Screens/Port_Royal/BackgroundWinter.dds", 0, 0, self.XResolution, self.YResolution, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		
		# InBound
		screen.addScrollPanel("InBoundList", u"", self.INBOUND_X, self.INBOUND_Y, self.INBOUND_W, self.INBOUND_H, PanelStyles.PANEL_STYLE_MAIN, false, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		# OutBound
		screen.addScrollPanel("OutBoundList", u"", self.OUTBOUND_X, self.OUTBOUND_Y, self.OUTBOUND_W, self.OUTBOUND_H, PanelStyles.PANEL_STYLE_MAIN, false, WidgetTypes.WIDGET_SAIL, UnitTravelStates.UNIT_TRAVEL_STATE_FROM_PORT_ROYAL, -1 )
	
		# In Port
		screen.addScrollPanel("LoadingList", u"", self.INPORT_X, self.INPORT_Y, self.INPORT_W, self.INPORT_H, PanelStyles.PANEL_STYLE_MAIN, True, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addScrollPanel("OutBoundListOverlay", u"", self.OUTBOUND_X, self.OUTBOUND_Y, self.OUTBOUND_W, self.OUTBOUND_H, PanelStyles.PANEL_STYLE_MAIN, false, WidgetTypes.WIDGET_SAIL, UnitTravelStates.UNIT_TRAVEL_STATE_FROM_PORT_ROYAL, -1 )
	
		# Dock
		screen.addScrollPanel("DockList", u"", self.DOCK_X, self.DOCK_Y, self.DOCK_W, self.DOCK_H, PanelStyles.PANEL_STYLE_MAIN, false, WidgetTypes.WIDGET_DOCK_PORT_ROYAL, -1, -1 )
	
		# Messages
		self.iCounter = self.TRADE_X
		if bShowTradeBox:
			self.iCounter = 240
			
		screen.setImageButton("TradeMessagePanel", "Art/Interface/Screens/Europe/Book.dds", self.iCounter - 240, self.TRADE_Y, self.TRADE_W, self.TRADE_H, WidgetTypes.WIDGET_GENERAL, self.TRADE_LOG, -1)
		self.szTradeTable = "TradeMessages"
		screen.addTableControlGFC(self.szTradeTable, 4, self.TRADE_X, self.TRADE_Y + self.STANDARD_MARGIN, self.TRADE_W - self.STANDARD_MARGIN * 2, self.TRADE_H - self.STANDARD_MARGIN * 2, false, false, 32, 32, TableStyles.TABLE_STYLE_ALTEMPTY)
		screen.setTableColumnHeader(self.szTradeTable, 0, u"", 58)
		screen.setTableColumnHeader(self.szTradeTable, 1, u"", 68)
		screen.setTableColumnHeader(self.szTradeTable, 2, u"", 48)
		screen.setTableColumnHeader(self.szTradeTable, 3, u"", 48)
		
		screen.appendTableRow(self.szTradeTable)
		screen.setTableText(self.szTradeTable, 0, 0, u"<color=102,61,41>" + localText.getText("TXT_KEY_EU_TRADE_LOG_1", ()) + u"</color>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_RIGHT_JUSTIFY)
		screen.setTableText(self.szTradeTable, 1, 0, u"<color=102,61,41>" + localText.getText("TXT_KEY_EU_TRADE_LOG_2", ()) + u"</color>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_RIGHT_JUSTIFY)
		screen.setTableText(self.szTradeTable, 2, 0, localText.getText("TXT_KEY_EU_TRADE_LOG_3", ()), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_RIGHT_JUSTIFY)
		screen.setTableText(self.szTradeTable, 3, 0, u"<color=102,61,41>" + localText.getText("TXT_KEY_EU_TRADE_LOG_4", ()) + u"</color>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_RIGHT_JUSTIFY)
		
		screen.setImageButtonAt(self.szTradeTable + "Close", self.szTradeTable, "", 0, 0, self.TRADE_W, self.XResolution, WidgetTypes.WIDGET_GENERAL, self.TRADE_LOG, -1)
		
		# Purchase - was moved to other place because of "Ships in Port Condition".
		#if (gc.getPlayer(gc.getGame().getActivePlayer()).canTradeWithPortRoyal()):
			# screen.addUnitGraphicGFC("PurchaseButton", gc.getInfoTypeForString("UNIT_GREAT_GENERAL"), -1, self.XResolution - self.RECRUIT_W - self.STANDARD_MARGIN, self.RECRUIT_Y, self.RECRUIT_W, self.RECRUIT_H, WidgetTypes.WIDGET_GENERAL, self.BUY_UNIT_BUTTON_ID, -1, 0, 0, 1.0, false)
			# screen.setImageButton("PurchaseButton",  "Art/Interface/Screens/Port_Royal/Great_General_Dollar.dds",  self.XResolution - self.RECRUIT_W - self.STANDARD_MARGIN,  self.RECRUIT_Y, self.RECRUIT_W,self.RECRUIT_H,  WidgetTypes.WIDGET_GENERAL, self.BUY_UNIT_BUTTON_ID, -1)
		
		# draw the contents
		self.drawContents()

	
	def drawContents(self):
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		screen = self.getScreen()

		self.deleteAllWidgets()
		self.setSound(INIT_SOUND)
	
		if self.iThisWinter:
			hudColor = "[COLOR_FONT_GOLD]"
		else:
			hudColor = "[COLOR_WHITE]"
	
		szTreasury = u"<font=4>" + localText.getText(hudColor, ()) + localText.getText("TXT_KEY_FINANCIAL_ADVISOR_TREASURY", (player.getGold(), )).upper() + u"</font>"
		screen.setText(self.getNextWidgetName(), "Background", szTreasury, CvUtil.FONT_LEFT_JUSTIFY, self.STANDARD_MARGIN, self.STANDARD_MARGIN, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, self.TREASURY_ID, -1 )
		szExit = u"<font=4>" + localText.getText(hudColor, ()) + localText.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + "</font>"
		screen.setText(self.getNextWidgetName(), "Background", szExit, CvUtil.FONT_RIGHT_JUSTIFY, self.XResolution - self.STANDARD_MARGIN, self.STANDARD_MARGIN, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )
		szTaxRate = u"<font=4>" + localText.getText(hudColor, ()) + localText.getText("TXT_KEY_MISC_BRIBE_RATE", (gc.getDefineINT("PORT_ROYAL_PORT_TAX"), player.NBMOD_GetMaxTaxRate())).upper() + u"</font>"
		# R&R, Robert Surcouf, No More Variables Hidden game option START
		#screen.setText(self.getNextWidgetName(), "Background", szTaxRate, CvUtil.FONT_RIGHT_JUSTIFY, self.XResolution - CyInterface().determineWidth(szExit) - self.STANDARD_MARGIN * 2, self.STANDARD_MARGIN, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setText(self.getNextWidgetName(), "Background", szTaxRate, CvUtil.FONT_RIGHT_JUSTIFY, self.XResolution - CyInterface().determineWidth(szExit) - self.STANDARD_MARGIN * 2, self.STANDARD_MARGIN, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, self.HELP_TAX_RATE, -1 )
		# R&R, Robert Surcouf, No More Variables Hidden game option END
		
		if (sdToolKit.sdGetVal('komaScreens', player.getID(), 'TradeBox') == false):
			screen.hide(self.szTradeTable)
			
		#Units
		InboundUnitsList = []
		iSeaUnitCount = 0
		
		# R&R, ray, Purchase only available if ship in Port Royal
		iShipsInPortRoyal = 0
	
		(unit, iter) = player.firstUnit()
		while (unit):
			if (not unit.isCargo() and not unit.isDelayedDeath()):
				if (unit.getUnitTravelState() == UnitTravelStates.UNIT_TRAVEL_STATE_IN_PORT_ROYAL):
					if not unit.getID() in self.PortRoyalUnitsList:
						self.PortRoyalUnitsList.append(unit.getID())
						iShipsInPortRoyal += 1 # R&R, ray, Purchase only available if ship in Port Royal
				elif (unit.getUnitTravelState() == UnitTravelStates.UNIT_TRAVEL_STATE_FROM_PORT_ROYAL):
					if not unit.getID() in self.OutboundUnitsList:
						self.OutboundUnitsList.append(unit.getID())
				elif (unit.getUnitTravelState() == UnitTravelStates.UNIT_TRAVEL_STATE_TO_PORT_ROYAL):
					InboundUnitsList.append(unit)
				
				if (gc.getUnitInfo(unit.getUnitType()).getDomainType() == DomainTypes.DOMAIN_SEA):
					iSeaUnitCount += 1
	
			(unit, iter) = player.nextUnit(iter)

		InboundUnitsList.sort(lambda y, x: cmp(player.getUnit(x.getID()).getUnitTravelTimer(), player.getUnit(y.getID()).getUnitTravelTimer()))
		
		#in port
		iX = (self.INPORT_W - self.INPORT_SHIP_W) / 2
				
		for iUnit in self.PortRoyalUnitsList:
			unit = player.getUnit(iUnit)
			plot = unit.plot()
			YieldOnBoard = False
			iCargoCount = 0
			iX_Space = iX + self.INPORT_SHIP_W * 17 / 32 - self.CARGO_SPACING * unit.cargoSpace() / 2
			iX_Cargo = iX_Space
			
			UnitInfo = gc.getUnitInfo(unit.getUnitType())
			iProfession = unit.getProfession()
			iUnitArtStyle = gc.getCivilizationInfo(player.getCivilizationType()).getUnitArtStyleType()
			screen.setImageButtonAt(self.getNextWidgetName(), "LoadingList", UnitInfo.getUnitArtStylesArtInfo(0, iProfession, iUnitArtStyle).getFullLengthIcon(), iX, 0, self.INPORT_SHIP_W, self.INPORT_SHIP_H, WidgetTypes.WIDGET_SHIP_CARGO_PORT_ROYAL, unit.getID(), -1)
			
			for i in range(unit.cargoSpace()):
				screen.addDDSGFCAt(self.getNextWidgetName(), "LoadingList", ArtFileMgr.getInterfaceArtInfo("INTERFACE_EUROPE_BOX_CARGO").getPath(), iX_Space , self.INPORT_SHIP_H * 7 / 12, self.CARGO_ICON_SIZE, self.CARGO_ICON_SIZE, WidgetTypes.WIDGET_SHIP_CARGO_PORT_ROYAL, unit.getID(), -1, False)
				iX_Space += self.CARGO_SPACING
				
			for i in range(plot.getNumUnits()):
				loopUnit = plot.getUnit(i)
				transportUnit = loopUnit.getTransportUnit()
				if (not transportUnit.isNone() and transportUnit.getID() == unit.getID() and transportUnit.getOwner() == unit.getOwner()):
					if loopUnit.isGoods():
						szText = u"<font=2>%s</font>" % loopUnit.getYieldStored()
						iWidgetId = self.YIELD_CARGO_BUTTON_ID
						if (gc.getUnitInfo(loopUnit.getUnitType()).isTreasure()):
							YieldOnBoard = True
						elif (loopUnit.getYield() != YieldTypes.NO_YIELD) and player.isYieldPortRoyalTradable(loopUnit.getYield()):
							YieldOnBoard = True
					else:
						szText = ""
						iWidgetId = self.UNIT_CARGO_BUTTON_ID

					screen.addDragableButtonAt("LoadingList", self.getNextWidgetName(), loopUnit.getButton(), "", iX_Cargo , self.INPORT_SHIP_H * 7 / 12, self.CARGO_ICON_SIZE, self.CARGO_ICON_SIZE, WidgetTypes.WIDGET_SHIP_CARGO_PORT_ROYAL, loopUnit.getID(), transportUnit.getID(), ButtonStyles.BUTTON_STYLE_LABEL)
					iX_Cargo += self.CARGO_SPACING
					iCargoCount += 1

			if (YieldOnBoard):
				screen.setImageButtonAt(self.getNextWidgetName(), "LoadingList", gc.getActionInfo(gc.getInfoTypeForString("COMMAND_YIELD_TRADE")).getButton(), iX + self.INPORT_SHIP_W * 15 / 16 - self.CARGO_ICON_SIZE * 5 / 4, self.INPORT_SHIP_H * 9 / 20, self.CARGO_ICON_SIZE * 5 / 4, self.CARGO_ICON_SIZE * 5 / 4, WidgetTypes.WIDGET_GENERAL, self.SELL_ALL, unit.getID())
			elif (iCargoCount == 0) and (gc.getUnitInfo(unit.getUnitType()).getEuropeCost() > 0) and (iSeaUnitCount > 1) and (self.iSellShip > 0):
				screen.setImageButtonAt(self.getNextWidgetName(), "LoadingList", ArtFileMgr.getInterfaceArtInfo("INTERFACE_EUROPE_PURCHASE_UNIT").getPath(), iX + self.INPORT_SHIP_W * 15 / 16 - self.CARGO_ICON_SIZE * 5 / 4, self.INPORT_SHIP_H * 9 / 20, self.CARGO_ICON_SIZE * 5 / 4, self.CARGO_ICON_SIZE * 5 / 4, WidgetTypes.WIDGET_GENERAL, self.SELL_SHIP, unit.getID())

			if (not unit.isFull() and player.getNumPortRoyalUnits() > 0 and unit.specialCargo() == SpecialUnitTypes.NO_SPECIALUNIT):
				screen.setImageButtonAt(self.getNextWidgetName(), "LoadingList", gc.getActionInfo(gc.getInfoTypeForString("COMMAND_LOAD")).getButton(), iX + self.INPORT_SHIP_W * 15 / 16 - self.CARGO_ICON_SIZE * 17 / 8, self.INPORT_SHIP_H * 9 / 20, self.CARGO_ICON_SIZE * 5 / 4, self.CARGO_ICON_SIZE * 5 / 4, WidgetTypes.WIDGET_GENERAL, self.LOAD_ALL, unit.getID())
			
			screen.setImageButtonAt(self.getNextWidgetName(), "LoadingList", ArtFileMgr.getInterfaceArtInfo("INTERFACE_EUROPE_SAIL").getPath(), iX + self.INPORT_SHIP_W / 8, self.INPORT_SHIP_H * 9 / 20, self.CARGO_ICON_SIZE * 5 / 4, self.CARGO_ICON_SIZE * 5 / 4, WidgetTypes.WIDGET_GENERAL, self.SAIL_TO_NEW_WORLD, unit.getID())

			iTotalCargoWidth = unit.cargoSpace() * self.CARGO_SPACING
			if iTotalCargoWidth > self.INPORT_SHIP_W :
				iX += iTotalCargoWidth + self.CARGO_ICON_SIZE / 2
			else:
				iX += self.INPORT_SHIP_W * 5 / 4
			
		#inbound
		iX = 0
		self.InboundCargoDictionary = {}

		for unit in InboundUnitsList:
			plot = unit.plot()

			for i in range(plot.getNumUnits()):
				loopUnit = plot.getUnit(i)
				transportUnit = loopUnit.getTransportUnit()

				if (loopUnit.isCargo()):
					if (not transportUnit.isNone() and transportUnit.getID() == unit.getID() and transportUnit.getOwner() == unit.getOwner()):
						iCargoStored = 0

						if not loopUnit.getTransportUnit().getID() in self.InboundCargoDictionary:
							self.InboundCargoDictionary[loopUnit.getTransportUnit().getID()] = {}
						if loopUnit.getUnitType() in self.InboundCargoDictionary[loopUnit.getTransportUnit().getID()]:
							iCargoStored = int(self.InboundCargoDictionary[loopUnit.getTransportUnit().getID()][loopUnit.getUnitType()])
						if loopUnit.isGoods():
							self.InboundCargoDictionary[loopUnit.getTransportUnit().getID()][loopUnit.getUnitType()] = loopUnit.getYieldStored() + iCargoStored
						else:
							self.InboundCargoDictionary[loopUnit.getTransportUnit().getID()][loopUnit.getUnitType()] = iCargoStored + 1

			screen.addDDSGFCAt(self.getNextWidgetName(), "InBoundList", self.getMirrorShipIcon(unit), iX, 0, self.INBOUND_SHIP_W, self.INBOUND_SHIP_H, WidgetTypes.WIDGET_GENERAL, self.TRAVEL_INFO, unit.getID(), False)
			iX += self.XResolution / len(InboundUnitsList) * 2 / 5
	
		#outbound
		iX, iY = 0, 0
		iW, iH = self.OUTBOUND_SHIP_W, self.OUTBOUND_SHIP_H
		iNumOutbound = len(self.OutboundUnitsList)
		if (iNumOutbound < 5): 
			iNumOutbound = 5
		
		self.OutboundCargoDictionary = {}
	
		for iUnit in self.OutboundUnitsList:
			unit = player.getUnit(iUnit)
			plot = unit.plot()

			for i in range(plot.getNumUnits()):
				loopUnit = plot.getUnit(i)
				transportUnit = loopUnit.getTransportUnit()
				if (loopUnit.isCargo()):
					if (not transportUnit.isNone() and transportUnit.getID() == unit.getID() and transportUnit.getOwner() == unit.getOwner()):
						iCargoStored = 0

						if not loopUnit.getTransportUnit().getID() in self.OutboundCargoDictionary:
							self.OutboundCargoDictionary[loopUnit.getTransportUnit().getID()] = {}
						if loopUnit.getUnitType() in self.OutboundCargoDictionary[loopUnit.getTransportUnit().getID()]:
							iCargoStored = int(self.OutboundCargoDictionary[loopUnit.getTransportUnit().getID()][loopUnit.getUnitType()])
						if loopUnit.isGoods():
							self.OutboundCargoDictionary[loopUnit.getTransportUnit().getID()][loopUnit.getUnitType()] = loopUnit.getYieldStored() + iCargoStored
						else:
							self.OutboundCargoDictionary[loopUnit.getTransportUnit().getID()][loopUnit.getUnitType()] = iCargoStored + 1

			szOutboundIcon = self.getNextWidgetName()
			UnitInfo = gc.getUnitInfo(unit.getUnitType())
			iProfession = unit.getProfession()
			iUnitArtStyle = gc.getCivilizationInfo(player.getCivilizationType()).getUnitArtStyleType()
			screen.addDDSGFCAt(szOutboundIcon, "OutBoundList", UnitInfo.getUnitArtStylesArtInfo(0, iProfession, iUnitArtStyle).getFullLengthIcon(), iX, iY, iW, iH, WidgetTypes.WIDGET_GENERAL, -1, -1, False)
			
			iTravelTimer = player.getUnit(unit.getID()).getUnitTravelTimer()
			iMaxTravelTimer = self.getMaxTravelTimer(unit.plot())
			if (iMaxTravelTimer - iTravelTimer > 0):
				screen.addDDSGFCAt(self.getNextWidgetName(), "OutBoundListOverlay", "", iX, iY, iW, iH * 7 / 12, WidgetTypes.WIDGET_GENERAL, self.TRAVEL_INFO, unit.getID(), False)
			else:
				screen.addDDSGFCAt(self.getNextWidgetName(), szOutboundIcon, ArtFileMgr.getInterfaceArtInfo("INTERFACE_EUROPE_SAIL").getPath(), iW / 2, iH / 3, self.CARGO_ICON_SIZE * 3 / 4, self.CARGO_ICON_SIZE * 3 / 4, WidgetTypes.WIDGET_GENERAL, -1, -1, False)
				screen.setImageButtonAt(self.getNextWidgetName(), "OutBoundListOverlay", "", iX, iY, iW, iH * 7 / 12, WidgetTypes.WIDGET_GENERAL, self.RECALL, unit.getID())
			
			if self.OutboundUnitsList.index(iUnit) % 2 == 0:
				iY += self.OUTBOUND_SHIP_H / 4
				iW += self.OUTBOUND_SHIP_W / 2
			else:
				iY -= self.OUTBOUND_SHIP_H / 4
				iW -= self.OUTBOUND_SHIP_W / 2
				
			iX += self.OUTBOUND_SHIP_W / iNumOutbound * 6
			iH = iW * 7 / 4
			
		# Units waiting on Docks
		iX = 0
		iNumDocks = player.getNumPortRoyalUnits()
		if (iNumDocks < 5):
			iNumDocks = 5
	
		for i in range(player.getNumPortRoyalUnits()):
			loopUnit = player.getPortRoyalUnit(i)
			iY, iW = 0, self.DOCK_UNIT_W
			UnitInfo = gc.getUnitInfo(loopUnit.getUnitType())
			iProfession = loopUnit.getProfession()
			iUnitArtStyle = gc.getCivilizationInfo(player.getCivilizationType()).getUnitArtStyleType()
			screen.addDragableButtonAt("DockList", self.getNextWidgetName(), UnitInfo.getUnitArtStylesArtInfo(0, iProfession, iUnitArtStyle).getFullLengthIcon(), "", iX, iY, iW, self.DOCK_UNIT_H, WidgetTypes.WIDGET_DOCK_PORT_ROYAL, loopUnit.getID(), -1, ButtonStyles.BUTTON_STYLE_LABEL )
			iX += self.DOCK_UNIT_W * 6 / iNumDocks
		
		screen.addDDSGFC("DockOverlay", "Art/Interface/Screens/Port_Royal/DockOverlay.dds", 0, 0, self.XResolution, self.YResolution, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setHitTest("DockOverlay", HitTestTypes.HITTEST_NOHIT)
		
		# R&R, ray, Purchase only available if ship in Port Royal
		if (gc.getPlayer(gc.getGame().getActivePlayer()).canTradeWithPortRoyal() and iShipsInPortRoyal > 0):
			screen.setImageButton("PurchaseButton",  "Art/Interface/Screens/Port_Royal/Great_General_Dollar.dds",  self.XResolution - self.RECRUIT_W - self.STANDARD_MARGIN,  self.RECRUIT_Y, self.RECRUIT_W,self.RECRUIT_H,  WidgetTypes.WIDGET_GENERAL, self.BUY_UNIT_BUTTON_ID, -1)
		
		# Units to Recruit - Not available in Port Royal
		#iX = self.RECRUIT_X -20
		#for i in range (gc.getDefineINT("DOCKS_NEXT_UNITS")):
		#	if player.getDocksNextUnit(i) != UnitTypes.NO_UNIT:
		#		UnitInfo = gc.getUnitInfo(player.getDocksNextUnit(i))
		#		## R&R, Robert Surcouf,  Multiple rows of Yields if necessary START
		#		#screen.addUnitGraphicGFC("Recruits" + str(i), player.getDocksNextUnit(i), UnitInfo.getDefaultProfession(), iX, self.RECRUIT_Y, self.RECRUIT_W, self.RECRUIT_H, WidgetTypes.WIDGET_PLAYER_HURRY, gc.getInfoTypeForString("HURRY_IMMIGRANT"), i, 0, 0, 1.0, false)
		#		# Sometimes immigrants appear on water !!!!!
		#		screen.addUnitGraphicGFC("Recruits" + str(i), player.getDocksNextUnit(i), UnitInfo.getDefaultProfession(), iX, self.RECRUIT_Y, self.RECRUIT_W, self.RECRUIT_H, WidgetTypes.WIDGET_PLAYER_HURRY, gc.getInfoTypeForString("HURRY_IMMIGRANT"), i, 0, 0, 1.0, false)
		#		## R&R, Robert Surcouf,  Multiple rows of Yields if necessary END
		#		iX += self.RECRUIT_W
				
		#Trade messages
		iLastFailed = -1
		
		tradeMessages = []
		for i in range(player.getNumTradeMessages()):
			iYield = player.getTradeMessageYield(i)
			iPrice = player.getTradeMessageAmount(i)
			szBuffer = u"-"
			szTax = u"-"
			#gc.getDefineINT("PORT_ROYAL_PORT_TAX")
			if player.getTradeMessageType(i) == TradeMessageTypes.TRADE_MESSAGE_EUROPE_YIELD_SOLD:
				iPrice *= self.playerEurope.getYieldPortRoyalBuyPrice(iYield)
				szBuffer = u"%d%c" % (player.getTradeMessageAmount(i), gc.getYieldInfo(iYield).getChar())
				szAction = localText.getText("TXT_KEY_EU_TRADE_LOG_SOLD", ())
				if gc.getDefineINT("PORT_ROYAL_PORT_TAX") > 0:
					szTax = str(iPrice * gc.getDefineINT("PORT_ROYAL_PORT_TAX") / 100)
			elif player.getTradeMessageType(i) == TradeMessageTypes.TRADE_MESSAGE_EUROPE_YIELD_BOUGHT:
				iPrice *= self.playerEurope.getYieldPortRoyalSellPrice(iYield)
				szBuffer = u"%d%c" % (player.getTradeMessageAmount(i), gc.getYieldInfo(iYield).getChar())
				szAction = localText.getText("TXT_KEY_EU_TRADE_LOG_BOUGHT", ())
			elif player.getTradeMessageType(i) == TradeMessageTypes.TRADE_MESSAGE_TREASURE:
				szAction = localText.getText("TXT_KEY_UNIT_TREASURE", ())
				if gc.getDefineINT("PORT_ROYAL_PORT_TAX") > 0:
					szTax = str(iPrice * gc.getDefineINT("PORT_ROYAL_PORT_TAX") / 100)
			
			if player.getTradeMessageType(i) == TradeMessageTypes.TRADE_MESSAGE_LACK_FUNDS:
				iLastFailed = i
			else:
				tradeMessages.append([szBuffer, szAction, str(iPrice), szTax])
				
		tradeMessages.reverse()
		
		i = 1
		for message in tradeMessages:
			if len(tradeMessages) >= screen.getTableNumRows(self.szTradeTable):
				screen.appendTableRow(self.szTradeTable)
			for j in range(len(message)):
				screen.setTableText(self.szTradeTable, j, i, u"<font=2><color=102,61,41>" + message[j] + u"</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_RIGHT_JUSTIFY)
			i += 1
			
		if iLastFailed > -1:
			if iLastFailed == sdToolKit.sdGetVal('komaScreens', player.getID(), 'DealFailed'):
				screen.hide("DealFailedText")
			else:
				screen.setLabel( "DealFailedText", "Background", localText.getText("%s1", (player.getTradeMessage(iLastFailed),())), CvUtil.FONT_CENTER_JUSTIFY, self.XResolution / 2, self.BOX_Y - self.STANDARD_MARGIN * 3 / 2, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
				sdToolKit.sdSetVal('komaScreens', player.getID(), 'DealFailed', iLastFailed)

		# Yield Rates
		iX = self.BOX_X
		## R&R, Robert Surcouf,  Multiple rows of Yields if necessary START
		iY = self.BOX_Y -10
		for iYield in self.YieldList:
			kYield = gc.getYieldInfo(iYield)
			iSellPrice = self.playerEurope.getYieldPortRoyalSellPrice(iYield)
			iBuyPrice = self.playerEurope.getYieldPortRoyalBuyPrice(iYield)
			#player.setYieldEuropeTradable(iYield, false)
			#screen.addDDSGFC(self.getNextWidgetName(), ArtFileMgr.getInterfaceArtInfo("INTERFACE_EUROPE_SHADOW_BOX").getPath(), iX, self.BOX_Y, self.BOX_W, self.BOX_H, WidgetTypes.WIDGET_MOVE_CARGO_TO_TRANSPORT, iYield, -1)
			#screen.addDDSGFC(self.getNextWidgetName(), ArtFileMgr.getInterfaceArtInfo("INTERFACE_EUROPE_BOX_PRICE").getPath(), iX, self.BOX_Y, self.BOX_W, self.BOX_H, WidgetTypes.WIDGET_MOVE_CARGO_TO_TRANSPORT, iYield, -1)
			screen.addDDSGFC(self.getNextWidgetName(), ArtFileMgr.getInterfaceArtInfo("INTERFACE_EUROPE_SHADOW_BOX").getPath(), iX,iY, self.BOX_W, self.BOX_H, WidgetTypes.WIDGET_MOVE_CARGO_TO_TRANSPORT_PORT_ROYAL, iYield, -1)
			screen.addDDSGFC(self.getNextWidgetName(), ArtFileMgr.getInterfaceArtInfo("INTERFACE_EUROPE_BOX_PRICE").getPath(), iX, iY, self.BOX_W, self.BOX_H, WidgetTypes.WIDGET_MOVE_CARGO_TO_TRANSPORT_PORT_ROYAL, iYield, -1)
			szPrices = u"<font=2>%d/%d</font>" % (iBuyPrice, iSellPrice)
			szIcons = self.getNextWidgetName()
			if not player.isYieldPortRoyalTradable(iYield) and self.iBoycott > 0:
				szPrices = u"<color=255,0,0>" + szPrices + u"</color>"
				screen.setImageButton(szIcons, gc.getYieldInfo(iYield).getIcon(), iX + self.BOX_W / 12, iY + self.BOX_H / 3, self.BOX_W * 5 / 6, self.BOX_W * 5 / 6, WidgetTypes.WIDGET_GENERAL, self.BOYCOTT, iYield)
				#screen.setImageButton(szIcons, gc.getYieldInfo(iYield).getIcon(), iX + self.BOX_W / 12, self.BOX_Y + self.BOX_H / 3, self.BOX_W * 5 / 6, self.BOX_W * 5 / 6, WidgetTypes.WIDGET_GENERAL, self.BOYCOTT, iYield)
			else:
				screen.addDragableButton(szIcons, gc.getYieldInfo(iYield).getIcon(), "", iX + self.BOX_W / 12, iY + self.BOX_H / 3, self.BOX_W * 5 / 6, self.BOX_W * 5 / 6, WidgetTypes.WIDGET_MOVE_CARGO_TO_TRANSPORT_PORT_ROYAL, iYield, -1, ButtonStyles.BUTTON_STYLE_IMAGE )
				#screen.addDragableButton(szIcons, gc.getYieldInfo(iYield).getIcon(), "", iX + self.BOX_W / 12, self.BOX_Y + self.BOX_H / 3, self.BOX_W * 5 / 6, self.BOX_W * 5 / 6, WidgetTypes.WIDGET_MOVE_CARGO_TO_TRANSPORT, iYield, -1, ButtonStyles.BUTTON_STYLE_IMAGE )
			#screen.setLabel("EuropePrices" + str(iYield), "Background", szPrices, CvUtil.FONT_CENTER_JUSTIFY, iX + self.BOX_W / 2, self.BOX_Y + self.BOX_H / 12, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_MOVE_CARGO_TO_TRANSPORT, iYield, -1)
			screen.setLabel("PortRoyalPrices" + str(iYield), "Background", szPrices, CvUtil.FONT_CENTER_JUSTIFY, iX + self.BOX_W / 2, iY + self.BOX_H / 12, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_MOVE_CARGO_TO_TRANSPORT_PORT_ROYAL, iYield, -1)
			screen.moveBackward("PortRoyalPrices" + str(iYield))
	
			iX += self.BOX_W
			#iY += (iX / (self.XResolution -5) ) * self.BOX_H
			iY += (iX / (self.BOX_X + self.NUM_YIELDS_IN_A_ROW * self.BOX_W -1) ) * self.BOX_H
			#iX =  max( iX  % (self.XResolution -5) , (self.XResolution - self.BOX_W * self.NUM_YIELDS_IN_A_ROW) / 2)
			iX =  max( iX  % (self.BOX_X + self.NUM_YIELDS_IN_A_ROW * self.BOX_W -1) , (self.XResolution - (self.XResolution / self.NUM_YIELDS_IN_A_ROW) * self.NUM_YIELDS_IN_A_ROW) / 2)
		## R&R, Robert Surcouf,  Multiple rows of Yields if necessary END
	
		#Immigration Bar
		szWidget = self.getNextWidgetName()
		iX_Begin = CyInterface().determineWidth(szTreasury) + self.STANDARD_MARGIN * 2
		iX_End = self.XResolution - CyInterface().determineWidth(szExit) - CyInterface().determineWidth(szTaxRate) - self.STANDARD_MARGIN * 3
		iW = self.IMMIGRANT_BAR_W
		iW_Max = iX_End - iX_Begin - self.STANDARD_MARGIN * 2
		if iW_Max < iW:
			iW = iW_Max
		iX = iX_Begin + (iX_End - iX_Begin) / 2 - iW / 2
		screen.addStackedBarGFC(szWidget, iX, self.STANDARD_MARGIN, iW, self.STANDARD_MARGIN * 2, InfoBarTypes.NUM_INFOBAR_TYPES, WidgetTypes.WIDGET_GENERAL, self.HELP_CROSS_RATE, -1)
		screen.setStackedBarColors(szWidget, InfoBarTypes.INFOBAR_STORED, gc.getInfoTypeForString("COLOR_WATER_TEXT"))
		screen.setStackedBarColors(szWidget, InfoBarTypes.INFOBAR_RATE, gc.getInfoTypeForString("COLOR_CITY_BLUE"))
		screen.setStackedBarColors(szWidget, InfoBarTypes.INFOBAR_RATE_EXTRA, gc.getInfoTypeForString("COLOR_EMPTY"))
		screen.setStackedBarColors(szWidget, InfoBarTypes.INFOBAR_EMPTY, gc.getInfoTypeForString("COLOR_EMPTY"))
		fStoredPercent = float(player.getCrossesStored()) / float(player.immigrationThreshold())
		screen.setBarPercentage(szWidget, InfoBarTypes.INFOBAR_STORED, fStoredPercent)
		if (fStoredPercent < 1.0):
			fRatePercent = float(player.getYieldRate(YieldTypes.YIELD_CROSSES)) / float(player.immigrationThreshold()) / (1 - fStoredPercent)
			screen.setBarPercentage(szWidget, InfoBarTypes.INFOBAR_RATE, fRatePercent)
		screen.setLabel(self.getNextWidgetName(), "", u"<font=3>" + localText.getText("TXT_KEY_IMMIGRATION_BAR", (player.getCrossesStored(), player.immigrationThreshold(), gc.getYieldInfo(YieldTypes.YIELD_CROSSES).getChar())) + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, iX + iW / 2, self.STANDARD_MARGIN * 3 / 2, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, self.HELP_CROSS_RATE, -1)

		return 0

	
	def getNextWidgetName(self):
		szName = self.WIDGET_ID + str(self.nWidgetCount)
		self.nWidgetCount += 1
		return szName

	
	def deleteAllWidgets(self):
		screen = self.getScreen()
		i = self.nWidgetCount - 1
	
		while (i >= 0):
			self.nWidgetCount = i
			screen.deleteWidget(self.getNextWidgetName())
			i -= 1

		self.nWidgetCount = 0
	
	
	def handleInput(self, inputClass):
		screen = self.getScreen()
		## R&R, vetiarvind, Navigation sectors - START
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		inputFnName = inputClass.getFunctionName()				
				
		constStrEast = "DialogMap" + "East"
		constStrWest = "DialogMap" + "West"		
		## R&R, vetiarvind, Navigation sectors - END
		
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if (inputClass.getButtonType() == WidgetTypes.WIDGET_PLAYER_HURRY):
				self.setSound(UNIT_SOUND_SELECT)

			if (inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL):
				if inputClass.getFunctionName() == self.szTradeTable:
					self.toggleTradeLog()
					
				elif (inputClass.getData1() == self.BUY_UNIT_BUTTON_ID) :
					popupInfo = CyPopupInfo()
					popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PURCHASE_PORT_ROYAL_UNIT)
					CyInterface().addPopup(popupInfo, gc.getGame().getActivePlayer(), true, false)

				elif (inputClass.getData1() == self.SAIL_TO_NEW_WORLD) :
					self.sailToNewWorld(inputClass.getData2())
					
				elif (inputClass.getData1() == self.SAIL_EAST) :
					self.sailToEastOrWest(inputClass.getData2(), self.pPlotEast)
					
				elif (inputClass.getData1() == self.SAIL_WEST) :
					self.sailToEastOrWest(inputClass.getData2(), self.pPlotWest)
					
				#R&R, vetiarvind, navigation sectors - START
				elif (inputClass.getData1() >= self.SAIL_EAST_NFrigid and inputClass.getData1() <= self.SAIL_EAST_SFrigid) :					
					self.sailToEastOrWest(inputClass.getData2(), random.choice(self.EuropePlotListSectorsEast[inputClass.getData1() - self.SAIL_EAST_NFrigid])) 
				
				elif (inputClass.getData1() >= self.SAIL_WEST_NFrigid and inputClass.getData1() <= self.SAIL_WEST_SFrigid) :
					self.sailToEastOrWest(inputClass.getData2(), random.choice(self.EuropePlotListSectorsWest[inputClass.getData1() - self.SAIL_WEST_NFrigid]))
				#R&R, vetiarvind, navigation sectors - END				
					
				elif (inputClass.getData1() == self.SAIL_EXEC) :
					transport = player.getUnit(inputClass.getData2())
					index = self.getCityByTableRow(inputClass.getID())
					pCity = player.getCity(self.getCityByTableRow(index))
					if (inputClass.getFunctionName() == "DialogTable") :
						if (not transport.isNone()) and transport.getUnitTravelState() != UnitTravelStates.UNIT_TRAVEL_STATE_FROM_PORT_ROYAL:
							pCity, pPlot = self.CityPlotList[index][0], self.CityPlotList[index][1]
							if pCity != None:
								if pPlot == None:
									pPlot = self.getBestCityPlot(pCity.plot()) 
									self.CityPlotList[index] = [pCity, pPlot]
								CyMessageControl().sendApplyEvent(CvUtil.EventDoEuropeScreen, EventContextTypes.EVENTCONTEXT_ALL, (SEND_TO_NEW_WORLD_CITY, inputClass.getData2(), pPlot.getX(), pPlot.getY(), pCity.plot().getX(), pCity.plot().getY(), -1, -1, -1))
								CyMessageControl().sendDoCommand(inputClass.getData2(), CommandTypes.COMMAND_SAIL_TO_PORT_ROYAL, UnitTravelStates.UNIT_TRAVEL_STATE_FROM_PORT_ROYAL, -1, false)
								self.PortRoyalUnitsList.remove(inputClass.getData2())
								self.setSound(UNIT_SOUND_ORDER)

					self.hideDialogBox(index)
				
				elif (inputClass.getData1() == self.CLOSE_DIALOG) :
					self.hideDialogBox(-1)
					
				elif (inputClass.getData1() == self.SELL_ALL) :
					#player = gc.getPlayer(gc.getGame().getActivePlayer())#R&R, vetiarvind, navigation sectors 
					transport = player.getUnit(inputClass.getData2())
					(unit, iter) = player.firstUnit()
					while (unit):
						if (unit.getUnitTravelState() == UnitTravelStates.UNIT_TRAVEL_STATE_IN_PORT_ROYAL and unit.isCargo() and unit.isGoods()):
							if (unit.getTransportUnit().getID() == transport.getID()):
								CyMessageControl().sendPlayerAction(player.getID(), PlayerActionTypes.PLAYER_ACTION_SELL_YIELD_UNIT_PORT_ROYAL, 0, unit.getYieldStored(), unit.getID())
						(unit, iter) = player.nextUnit(iter)
					screen.setSoundId(CyAudioGame().Play2DSound("AS2D_BUILD_BANK"))

				elif (inputClass.getData1() == self.LOAD_ALL) :
					#player = gc.getPlayer(gc.getGame().getActivePlayer())#R&R, vetiarvind, navigation sectors 
					transport = player.getUnit(inputClass.getData2())
					for i in range(player.getNumPortRoyalUnits()):
						loopUnit = player.getPortRoyalUnit(i)
						if (not transport.isNone() and transport.getUnitTravelState() == UnitTravelStates.UNIT_TRAVEL_STATE_IN_PORT_ROYAL and not transport.isFull()):
							CyMessageControl().sendPlayerAction(player.getID(), PlayerActionTypes.PLAYER_ACTION_LOAD_UNIT_FROM_PORT_ROYAL, loopUnit.getID(), inputClass.getData2(), -1)
					screen.setSoundId(CyAudioGame().Play2DSound("AS2D_REVOLTEND"))

				elif (inputClass.getData1() == self.RECALL) :
					transport = player.getUnit(inputClass.getData2())
					if (not transport.isNone()) and transport.getUnitTravelState() != UnitTravelStates.UNIT_TRAVEL_STATE_IN_PORT_ROYAL:
						CyMessageControl().sendApplyEvent(CvUtil.EventDoEuropeScreen, EventContextTypes.EVENTCONTEXT_ALL, (RECALL_TO_PORT_ROYAL, transport.getID(), -1, -1, -1, -1, -1, -1, -1))
						self.OutboundUnitsList.remove(transport.getID())
						screen.setSoundId(CyAudioGame().Play2DSound("AS2D_REVOLTSTART"))

				elif (inputClass.getData1() == self.TRADE_LOG) :
					self.toggleTradeLog()
					
				elif (inputClass.getData1() == self.SELL_SHIP) :
					self.sellShip(inputClass.getData2())
				
				elif (inputClass.getData1() == self.SELL_SHIP_EXEC) :
					transport = player.getUnit(inputClass.getData2())
					iSellPrice = self.getShipSellPrice(inputClass.getData2())
					if (not transport.isNone() and not iSellPrice == -1):
						CyMessageControl().sendApplyEvent(CvUtil.EventDoEuropeScreen, EventContextTypes.EVENTCONTEXT_ALL, (SELL_SHIP_IN_PORT_ROYAL, transport.getID(), -1, -1, -1, -1, iSellPrice, -1, -1))
						self.PortRoyalUnitsList.remove(transport.getID())
						screen.setSoundId(CyAudioGame().Play2DSound("AS2D_BUILD_BANK"))
					self.hideDialogBox(-1)

				elif (inputClass.getData1() == self.PREVIEW_MODE) :
					if (sdToolKit.sdGetVal('komaScreens', player.getID(), 'PreviewMode') == "INTERFACE_DOMESTIC_ADVISOR"):
						szPreviewMode = "INTERFACE_FOREIGN_ADVISOR"
					else:
						szPreviewMode = "INTERFACE_DOMESTIC_ADVISOR"
					sdToolKit.sdSetVal('komaScreens', player.getID(), 'PreviewMode', szPreviewMode)
					screen.setImageButtonAt("PreviewMode", "DialogMap", ArtFileMgr.getInterfaceArtInfo(szPreviewMode).getPath(), self.MAP_SIZE - 40, self.MAP_SIZE - 40, 40, 40, WidgetTypes.WIDGET_GENERAL, self.PREVIEW_MODE, -1)
				
				elif (inputClass.getData1() == self.BOYCOTT) :
					self.liftBoycott(inputClass.getData2())
					
				elif (inputClass.getData1() == self.BOYCOTT_EXEC) :	
					iYield = inputClass.getData2()
					if (not iYield == -1) :
						iBoycottPrice = self.getBoycottPrice(iYield)
						if (player.getGold() - iBoycottPrice) >= 0:
							CyMessageControl().sendApplyEvent(CvUtil.EventDoEuropeScreen, EventContextTypes.EVENTCONTEXT_ALL, (LIFT_BOYCOTT_IN_PORT_ROYAL, -1, -1, -1, -1, -1, -1, iYield, iBoycottPrice))
							screen.setSoundId(CyAudioGame().Play2DSound("AS2D_KISS_MY_RING"))
						else:
							screen.setSoundId(CyAudioGame().Play2DSound("AS2D_LOSS_EARLY"))
					
					self.hideDialogBox(-1)
	
		elif (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CURSOR_MOVE_ON) :
			if (inputClass.getFunctionName() == "DialogTable") :
				screen.changeImageButton("DialogTable" + str(inputClass.getID()), "Art/Interface/Screens/TradeRoutes/Pixel.dds")
				self.getCityInfo(inputClass.getID(), player.getUnit(inputClass.getData2()))
			elif (inputClass.getFunctionName() == "DialogMap" + "East"):
				screen.changeDDSGFC("DialogMap", "Art/Interface/Screens/Europe/DialogMapAtlantic.dds")
			elif (inputClass.getFunctionName() == "DialogMap" + "West"):
				screen.changeDDSGFC("DialogMap", "Art/Interface/Screens/Europe/DialogMapPacific.dds")
			## R&R, vetiarvind, Navigation sectors - START			
			elif (inputFnName.startswith(constStrEast)) :
				for i in xrange(6):
					if (inputFnName == constStrEast+self.SectorNames[i]):
						screen.changeDDSGFC("DialogMap", "Art/Interface/Screens/Europe/DialogMapAtlantic" + self.SectorNames[i] + ".dds")			
						break
			elif (inputFnName.startswith(constStrWest)) :
				for i in xrange(6):
					if (inputFnName == constStrWest+self.SectorNames[i]):
						screen.changeDDSGFC("DialogMap", "Art/Interface/Screens/Europe/DialogMapPacific" + self.SectorNames[i] + ".dds")			
						break			
			## R&R, vetiarvind, Navigation sectors - END	
		elif (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CURSOR_MOVE_OFF) :
			if (inputClass.getFunctionName() == "DialogTable") :
				screen.changeImageButton("DialogTable" + str(inputClass.getID()), "")
				index = self.getCityByTableRow(inputClass.getID())
				screen.hide("DialogImage" + str(index))
				screen.hide("DialogWarehouse")
				for iYield in self.YieldList :
					screen.hide("DialogStorage" + str(iYield))
					screen.show("PortRoyalPrices" + str(iYield))
			#R&R, vetiarvind, Navigation sectors - START
			#elif (inputClass.getFunctionName() == "DialogMap" + "East") :
			#	screen.changeDDSGFC("DialogMap", "Art/Interface/Screens/Europe/DialogMapAmerica.dds")
			#elif (inputClass.getFunctionName() == "DialogMap" + "West") :
			#	screen.changeDDSGFC("DialogMap", "Art/Interface/Screens/Europe/DialogMapAmerica.dds")
			elif (inputFnName.startswith(constStrEast)) :
				screen.changeDDSGFC("DialogMap", "Art/Interface/Screens/Europe/DialogMapAmerica.dds")
			elif (inputFnName.startswith(constStrWest)) :
				screen.changeDDSGFC("DialogMap", "Art/Interface/Screens/Europe/DialogMapAmerica.dds")				
			#R&R, vetiarvind, Navigation sectors - END
				
		return 0

	
	def update(self, fDelta):
		if (CyInterface().isDirty(InterfaceDirtyBits.PortRoyalScreen_DIRTY_BIT)):
			CyInterface().setDirty(InterfaceDirtyBits.PortRoyalScreen_DIRTY_BIT, False)
			self.drawContents()
	
		if self.bBookIntro:
			self.moveTradeLog(self.iCounter < 280, 8)
		elif self.bBookOutro:
			self.moveTradeLog(self.iCounter > 40, -8)
	
	
	def moveTradeLog(self, Condition, iStep):
		screen = self.getScreen()
		screen.setImageButton("TradeMessagePanel", "Art/Interface/Screens/Europe/Book.dds", self.iCounter - 280, self.TRADE_Y, self.TRADE_W, self.TRADE_H, WidgetTypes.WIDGET_GENERAL, self.TRADE_LOG, -1)
		if Condition:
			self.iCounter += iStep
		else:
			if self.bBookIntro:
				self.bBookIntro = false
				screen.show(self.szTradeTable)
				screen.moveToFront(self.szTradeTable)
			else:
				self.bBookOutro = false
			
	
	def toggleTradeLog(self):
		screen = self.getScreen()
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		self.bBookOutro = sdToolKit.sdGetVal('komaScreens', player.getID(), 'TradeBox')
		if self.bBookOutro:
			screen.hide(self.szTradeTable)
		self.bBookIntro = not self.bBookOutro
		sdToolKit.sdSetVal('komaScreens', player.getID(), 'TradeBox', self.bBookIntro)
	
	
	def getWidgetHelp(self, argsList):
		iScreen, eWidgetType, iData1, iData2, bOption = argsList
		player = gc.getPlayer(gc.getGame().getActivePlayer())
	
		if eWidgetType == WidgetTypes.WIDGET_GENERAL:
			if iData1 == self.SAIL_TO_NEW_WORLD:
				return localText.getText("TXT_KEY_SAIL", ())
			elif iData1 == self.SELL_ALL:
				return localText.getText("TXT_KEY_SELL_ALL", ())
			elif iData1 == self.LOAD_ALL:
				return localText.getText("TXT_KEY_LOAD_ALL_EUROPE", ())
			elif iData1 == self.BUY_UNIT_BUTTON_ID:
				return localText.getText("TXT_KEY_PURCHASE_EUROPE", ())
			elif iData1 == self.TREASURY_ID:
				return localText.getText("TXT_KEY_ECON_GOLD_RESERVE" , ())
			elif iData1 == self.HELP_CROSS_RATE:
				return localText.getText("TXT_KEY_YIELD_RATE", (player.getYieldRate(YieldTypes.YIELD_CROSSES), gc.getYieldInfo(YieldTypes.YIELD_CROSSES).getChar()))
			elif iData1 == self.TRAVEL_INFO or iData1 == self.RECALL:
				return self.cargoMessage(iData2)
			elif iData1 == self.TRADE_LOG:
				if (sdToolKit.sdGetVal('komaScreens', player.getID(), 'TradeBox') == 0):
					return localText.getText("TXT_KEY_EU_SHOW_TRADE_LOG", ())
				else:
					return localText.getText("TXT_KEY_EU_HIDE_TRADE_LOG", ())
			elif iData1 == self.SELL_SHIP:
				return localText.getText("TXT_KEY_EU_SELL_SHORT", (player.getUnit(iData2).getName(), self.getShipSellPrice(iData2)))
			elif iData1 == self.PREVIEW_MODE:
				return localText.getText("TXT_KEY_EU_PREVIEW_MODE", ())
			elif iData1 == self.BOYCOTT:
				return localText.getText("TXT_KEY_EU_BOYCOTT_MESSAGE", (self.getBoycottPrice(iData2), gc.getYieldInfo(iData2).getDescription()))
			# R&R, Robert Surcouf, No More Variables Hidden game option START
			elif iData1 == self.HELP_TAX_RATE:
				if gc.getGame().isOption(GameOptionTypes.GAMEOPTION_NO_MORE_VARIABLES_HIDDEN):
					# R&R, vetiarvind, Price dependent tax increase - START
					return localText.getText("TXT_KEY_TAX_BAR", (self.getTotalYieldsScore(), self.getTaxTreshold(), gc.getYieldInfo(YieldTypes.YIELD_TRADE_GOODS).getChar(),self.getChanceProb()/10,self.getChanceProb()%10))				
					#return localText.getText("TXT_KEY_TAX_BAR", (self.getTotalYieldsTraded(), self.getTaxTreshold(), gc.getYieldInfo(YieldTypes.YIELD_TRADE_GOODS).getChar(),self.getChanceProb()/10,self.getChanceProb()%10))				
					# R&R, vetiarvind, Price dependent tax increase - END
				else:
					return localText.getText("TXT_KEY_MISC_TAX_RATE", (player.getTaxRate(), player.NBMOD_GetMaxTaxRate())).upper() + u"</font>"
					#return localText.getText("TXT_KEY_TAX_BAR", (self.getTotalYieldsTraded(), self.getTaxTreshold(), gc.getYieldInfo(YieldTypes.YIELD_TRADE_GOODS).getChar()))
					#return localText.getText("TXT_KEY_TAX_BAR", (0, self.getTaxTreshold(), gc.getYieldInfo(YieldTypes.YIELD_TRADE_GOODS).getChar()))
			# R&R, Robert Surcouf, No More Variables Hidden game option END	
		return u""
	
	
	def sailToNewWorld(self, iUnit):
		if self.bBookIntro or self.bBookOutro:
			return 0
		
		screen = self.getScreen()
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		pTransport = player.getUnit(iUnit)
				
		# WTP, ray, this is a problem now if we have different rules for Ships sailing to the new world
		# the Plot List needs to be regenerated every time, not just if it is empty
		#if self.EuropePlotList == []:
		self.getPlotLists(pTransport)
				
		self.createBox(self.DIALOG_X, self.DIALOG_Y, self.DIALOG_W, self.DIALOG_H, true)
		
		screen.hide("DealFailedText")
		
		screen.addUnitGraphicGFC("DialogMap" + "Water", gc.getInfoTypeForString("UNIT_CARAVEL"), -1, self.DIALOG_X + self.MAP_X, self.DIALOG_Y + self.MAP_Y, self.MAP_SIZE, self.MAP_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1, 0, 0, 0, true)			
		screen.addDDSGFC("DialogMap", "Art/Interface/Screens/Europe/DialogMapAmerica.dds", self.DIALOG_X + self.MAP_X, self.DIALOG_Y + self.MAP_Y, self.MAP_SIZE, self.MAP_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setLabelAt(self.getNextWidgetName(), "DialogMap", u"<font=4>" + localText.getText("TXT_KEY_EU_SAIL", ()) + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.MAP_SIZE / 2, self.STANDARD_MARGIN * 3 / 2, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		self.createBorder(0, 0, self.MAP_SIZE, self.MAP_SIZE, self.BORDER_SIZE, "DialogMap", true)
		# R&R, vetiarvind, Navigation Sectors - START
		#The first param in setImageButtonAt's chars needs to have a name followed by a unique digit to be processed by handleInput(which removes the digit in it's method)
		#if len(self.EuropePlotListEast) > 0:		#east button
		#	screen.setImageButtonAt("DialogMap" + "East" + str(1), "DialogMap", "", self.MAP_SIZE / 2, 0, self.MAP_SIZE / 2, self.MAP_SIZE, WidgetTypes.WIDGET_GENERAL, self.SAIL_EAST, iUnit)
		#if len(self.EuropePlotListWest) > 0:		#west button
		#	screen.setImageButtonAt("DialogMap" + "West" + str(2), "DialogMap", "", 0, 0, self.MAP_SIZE / 2, self.MAP_SIZE, WidgetTypes.WIDGET_GENERAL, self.SAIL_WEST, iUnit)
		sectorWidthRatio = 6
		sectorWidthAbsolute = self.MAP_SIZE / sectorWidthRatio		
		eastSectorX = self.MAP_SIZE - sectorWidthAbsolute
		if len(self.EuropePlotListEast) > 0:		#east button
			screen.setImageButtonAt("DialogMap" + "East"+str(1), "DialogMap", "", self.MAP_SIZE / 2, 0, self.MAP_SIZE / 2, self.MAP_SIZE, WidgetTypes.WIDGET_GENERAL, self.SAIL_EAST, iUnit)
		
		if len(self.EuropePlotListWest) > 0:		#west button
			screen.setImageButtonAt("DialogMap" + "West"+str(2), "DialogMap", "", 0, 0, self.MAP_SIZE / 2, self.MAP_SIZE, WidgetTypes.WIDGET_GENERAL, self.SAIL_WEST, iUnit)
				
		for i in range(0, 6) : 
			if len(self.EuropePlotListSectorsEast[i]) > 0:		#east sector buttons  (frigids,temperates,tropics)			
				screen.setImageButtonAt("DialogMap" + "East" + self.SectorNames[i]+str(i+2+1), "DialogMap", "", eastSectorX , i * self.MAP_SIZE / 6, sectorWidthAbsolute, self.MAP_SIZE / 6, WidgetTypes.WIDGET_GENERAL, self.SAIL_EAST_NFrigid + i, iUnit)
				
		for i in range(0, 6) : 
			if len(self.EuropePlotListSectorsWest[i]) > 0:		#west sector buttons (frigids,temperates,tropics)
				screen.setImageButtonAt("DialogMap" + "West" + self.SectorNames[i] + str(i+6+2+1), "DialogMap", "", 0, i * self.MAP_SIZE / 6, sectorWidthAbsolute, self.MAP_SIZE / 6, WidgetTypes.WIDGET_GENERAL, self.SAIL_WEST_NFrigid + i, iUnit)			

		
		# R&R, vetiarvind, Navigation Sectors - END
		
		szPreviewMode = sdToolKit.sdGetVal('komaScreens', player.getID(), 'PreviewMode')
		screen.setImageButtonAt("PreviewMode", "DialogMap", ArtFileMgr.getInterfaceArtInfo(szPreviewMode).getPath(), self.MAP_SIZE - 40, self.MAP_SIZE - 40, 40, 40, WidgetTypes.WIDGET_GENERAL, self.PREVIEW_MODE, -1)
		
		screen.addTableControlGFC("DialogTable", 3, self.DIALOG_TABLE_X, self.DIALOG_TABLE_Y, self.DIALOG_TABLE_W, self.DIALOG_TABLE_H, true, true, 32, 32, TableStyles.TABLE_STYLE_STANDARD)
		screen.enableSort("DialogTable")
		screen.setTableColumnHeader("DialogTable", 0, u"id", 0)
		screen.setTableColumnHeader("DialogTable", 1, localText.getText("TXT_KEY_TRADE_ROUTES_EXPORT_TABLE_2", ()), self.DIALOG_TABLE_W - 64)
		screen.setTableColumnHeader("DialogTable", 2, u"    <img=%s size=16></img>" % "Art/Interface/Screens/TradeRoutes/ExportImport.dds", 64)
				
		iI = 0
		for item in self.CityPlotList:
			pCity = item[0]
			screen.appendTableRow("DialogTable")
			screen.setTableRowHeight("DialogTable", iI, 32)
			screen.setTableText("DialogTable", 0, iI, u"%d" % iI, "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)
			screen.setTableText("DialogTable", 1, iI, u"%s" % pCity.getName(), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_RIGHT_JUSTIFY)
			screen.setTableText("DialogTable", 2, iI, self.getTradeRouteIcon(pCity, pTransport), "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)
			screen.setImageButtonAt("DialogTable" + str(iI), "DialogTable", "", 0, iI * 32, self.DIALOG_TABLE_W, 32, WidgetTypes.WIDGET_GENERAL, self.SAIL_EXEC, iUnit)
			iI += 1
		
		
	def sailToEastOrWest(self, iUnit, pPlot):
		CyMessageControl().sendApplyEvent(CvUtil.EventDoEuropeScreen, EventContextTypes.EVENTCONTEXT_ALL, (SEND_TO_EAST_OR_WEST, iUnit, pPlot.getX(), pPlot.getY(), -1, -1, -1, -1, -1))
		CyMessageControl().sendDoCommand(iUnit, CommandTypes.COMMAND_SAIL_TO_PORT_ROYAL, UnitTravelStates.UNIT_TRAVEL_STATE_FROM_PORT_ROYAL, -1, false)
		self.PortRoyalUnitsList.remove(iUnit)
		self.setSound(UNIT_SOUND_ORDER)
		self.hideDialogBox(-1)
		
	
	def sellShip(self, iUnit):
		screen = self.getScreen()
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		pTransport = player.getUnit(iUnit)
		
		if player.getNumShips() > 1:
			self.createBox(self.SELL_SHIP_X, self.SELL_SHIP_Y, self.SELL_SHIP_W, self.SELL_SHIP_H, true) 
			screen.addDDSGFCAt("SellShipEurope", "DialogPanel", "Art/Interface/Screens/Europe/Background.dds", self.SELL_SHIP_IMAGE_X, self.SELL_SHIP_IMAGE_Y, self.SELL_SHIP_IMAGE_W, self.SELL_SHIP_IMAGE_H, WidgetTypes.WIDGET_GENERAL, -1, -1, false)
			screen.setLabelAt(self.getNextWidgetName(), "SellShipEurope", u"<font=4>" + localText.getText("TXT_KEY_EU_SELL_LABEL", ()) + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, self.STANDARD_MARGIN, self.STANDARD_MARGIN * 3 / 2, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			self.createBorder(0, 0, self.SELL_SHIP_IMAGE_W, self.SELL_SHIP_IMAGE_H, self.BORDER_SIZE, "SellShipEurope", true)
			screen.addDDSGFCAt(self.getNextWidgetName(), "SellShipEurope", self.getMirrorShipIcon(pTransport), self.SELL_SHIP_IMAGE_W / 4, self.SELL_SHIP_IMAGE_W / 16, self.SELL_SHIP_IMAGE_W / 2, self.SELL_SHIP_IMAGE_W * 7 / 8, WidgetTypes.WIDGET_SHIP_CARGO_PORT_ROYAL, pTransport.getID(), -1, false)
				
			szMessage = localText.getText("TXT_KEY_EU_SELL_MESSAGE", (pTransport.getName(), self.getShipSellPrice(iUnit)))
			screen.addMultilineText("DialogMessage", szMessage, self.SELL_SHIP_X + self.SELL_SHIP_IMAGE_X, self.SELL_SHIP_MESSAGE_Y, self.SELL_SHIP_IMAGE_W, self.SELL_SHIP_MESSAGE_H, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
			
			screen.setButtonGFC("DialogButton", u"<font=3>" + localText.getText("TXT_KEY_EU_DIALOG_ACCEPT", ()) + u"</font>", "", self.SELL_SHIP_BUTTON_X, self.SELL_SHIP_BUTTON_Y, self.SELL_SHIP_BUTTON_W, self.SELL_SHIP_BUTTON_H, WidgetTypes.WIDGET_GENERAL, self.SELL_SHIP_EXEC, iUnit, ButtonStyles.BUTTON_STYLE_STANDARD)


	def liftBoycott(self, iYield):
		screen = self.getScreen()
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		iKingAttitude = self.playerEurope.AI_getAttitude(player.getID())
		
		self.createBox(self.SELL_SHIP_X, self.SELL_SHIP_Y, self.SELL_SHIP_W, self.SELL_SHIP_H, true) 
		screen.addLeaderheadGFC ("DialogKing", self.playerEurope.getLeaderType(), iKingAttitude, self.SELL_SHIP_X + self.SELL_SHIP_IMAGE_X, self.SELL_SHIP_Y + self.SELL_SHIP_IMAGE_Y, self.SELL_SHIP_IMAGE_W, self.SELL_SHIP_IMAGE_H, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setLabelAt(self.getNextWidgetName(), "DialogKing", u"<font=4>" + localText.getText("TXT_KEY_EU_BOYCOTT_LABEL", ()) + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, self.STANDARD_MARGIN, self.STANDARD_MARGIN * 3 / 2, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		self.createBorder(0, 0, self.SELL_SHIP_IMAGE_W, self.SELL_SHIP_IMAGE_H, self.BORDER_SIZE, "DialogKing", true)
			
		szMessage = localText.getText("TXT_KEY_EU_BOYCOTT_MESSAGE", (self.getBoycottPrice(iYield), gc.getYieldInfo(iYield).getDescription()))
		screen.addMultilineText("DialogMessage", szMessage, self.SELL_SHIP_X + self.SELL_SHIP_IMAGE_X, self.SELL_SHIP_MESSAGE_Y, self.SELL_SHIP_IMAGE_W, self.SELL_SHIP_MESSAGE_H, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

		screen.setButtonGFC("DialogButton", u"<font=3>" + localText.getText("TXT_KEY_EU_DIALOG_ACCEPT", ()) + u"</font>", "", self.SELL_SHIP_BUTTON_X, self.SELL_SHIP_BUTTON_Y, self.SELL_SHIP_BUTTON_W, self.SELL_SHIP_BUTTON_H, WidgetTypes.WIDGET_GENERAL, self.BOYCOTT_EXEC, iYield, ButtonStyles.BUTTON_STYLE_STANDARD)
		
		
	def getTradeRouteIcon(self, pCity, pTransport):
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		szIcon = u""
		for iTradeRoute in range(player.getNumTradeRoutes()):
			pTradeRoute = player.getTradeRouteByIndex(iTradeRoute)
			if pTransport.getGroup().isAssignedTradeRoute(pTradeRoute.getID()):
				if pTradeRoute.getSourceCity().iID == pCity.getID() and pTradeRoute.getDestinationCity().iID == -1:
					szIcon = u"%c" % CyGame().getSymbolID(FontSymbols.EXPORT_CHAR)
						
		return szIcon

	
	def getMirrorShipIcon(self, pUnit):
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		UnitInfo = gc.getUnitInfo(pUnit.getUnitType())
		iProfession = pUnit.getProfession()
		iUnitArtStyle = gc.getCivilizationInfo(player.getCivilizationType()).getUnitArtStyleType()
		szFile = UnitInfo.getUnitArtStylesArtInfo(0, iProfession, iUnitArtStyle).getFullLengthIcon()
		
		return szFile[:-len(".dds")] + "_inbound.dds"
		
		
	def createBox(self, iX, iY, iW, iH, bShadow):
		screen = self.getScreen()
		screen.addPanel ("DialogBackground", u"", u"", True, False, 0, 0, self.XResolution, self.YResolution, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setImageButtonAt(self.getNextWidgetName(), "DialogBackground", "", 0, 50, self.XResolution, self.YResolution - 50, WidgetTypes.WIDGET_GENERAL, self.CLOSE_DIALOG, -1)
		screen.addPanel ("DialogPanel", u"", u"", True, False, iX, iY, iW, iH, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addDrawControlAt("DialogPanel", "DialogBackground", "Art/Interface/Screens/Europe/AchieveBG.dds", iX, iY, iW, iH, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		self.createBorder(0, 0, iW, iH, self.BORDER_SIZE, "DialogPanel", bShadow)
		
	
	def createBorder(self, iX, iY, iW, iH, iBorderSize, szAttachTo, bShadow):
		screen = self.getScreen()
		szBorderArt = "Art/Interface/Screens/TradeRoutes/Pixel.dds"
		screen.addDDSGFCAt(szAttachTo + "Border" + str(1), szAttachTo, szBorderArt, iX, iY, iW, iBorderSize, WidgetTypes.WIDGET_GENERAL, -1, -1, false)
		screen.addDDSGFCAt(szAttachTo + "Border" + str(2), szAttachTo, szBorderArt, iX + iW - iBorderSize, iY + iBorderSize, iBorderSize, iH - iBorderSize * 2, WidgetTypes.WIDGET_GENERAL, -1, -1, false)
		screen.addDDSGFCAt(szAttachTo + "Border" + str(3), szAttachTo, szBorderArt, iX, iY + iH - iBorderSize, iW, iBorderSize, WidgetTypes.WIDGET_GENERAL, -1, -1, false)
		screen.addDDSGFCAt(szAttachTo + "Border" + str(4), szAttachTo, szBorderArt, iX, iY + iBorderSize, iBorderSize, iH - iBorderSize * 2, WidgetTypes.WIDGET_GENERAL, -1, -1, false)
		if bShadow:
			screen.addDDSGFCAt(szAttachTo + "Shadow", szAttachTo, "Art/Interface/Screens/Europe/Shadow.dds", iX, iY, iW, iH, WidgetTypes.WIDGET_GENERAL, -1, -1, false)
	

	def hideDialogBox (self, index):
		screen = self.getScreen()
		screen.hide("DialogBackground")
		screen.hide("DialogTable")
		screen.hide("DialogMap")
		screen.hide("DialogMap" + "Water")
		screen.hide("DialogMap" + "East" + str(1))
		screen.hide("DialogMap" + "West" + str(2))
		for i in range(6) :
			screen.hide("DialogMap" + "East" + self.SectorNames[i] + str(i+2+1))
			screen.hide("DialogMap" + "West" + self.SectorNames[i] + str(i+2+6+1))

		screen.hide("DialogKing")
		screen.hide("DialogImage" + str(index))
		screen.hide("DialogMessage")
		screen.hide("DialogButton")
		for iYield in self.YieldList :
			screen.show("PortRoyalPrices" + str(iYield))
	
	
	def getCityByTableRow(self, iRow):
		return int(self.getScreen().getTableText("DialogTable", 0, iRow))
		
	
	def getCityInfo(self, iRow, pTransport) :
		screen = self.getScreen()
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		index = self.getCityByTableRow(iRow)
		pCity = self.CityPlotList[index][0]
		iX = self.BOX_X
		## R&R, Robert Surcouf,  Multiple rows of Yields if necessary START
		iY = self.BOX_Y
		## R&R, Robert Surcouf,  Multiple rows of Yields if necessary END
		
		#get yield stored in city warehouse
		for iYield in self.YieldList :
			screen.hide("PortRoyalPrices" + str(iYield))
			szNumStorage = u"-"

			if (pCity.getYieldStored(iYield) > 0):
				szNumStorage = u"%d" % (pCity.getYieldStored(iYield))
			## R&R, Robert Surcouf,  Multiple rows of Yields if necessary START
			
			#screen.setLabel("DialogStorage" + str(iYield), "Background", u"<font=3><color=255,255,255>" + szNumStorage + u"</color></font>", CvUtil.FONT_CENTER_JUSTIFY, iX + self.BOX_W / 2, self.BOX_Y + self.BOX_H / 12, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.setLabel("DialogStorage" + str(iYield), "Background", u"<font=2><color=255,255,255>" + szNumStorage + u"</color></font>", CvUtil.FONT_CENTER_JUSTIFY, iX + self.BOX_W / 2, iY + self.BOX_H / 12, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			iX += self.BOX_W

			#iY += (iX / (self.XResolution -5) ) * self.BOX_H
			iY += (iX / (self.BOX_X + self.NUM_YIELDS_IN_A_ROW * self.BOX_W -1) ) * self.BOX_H
			#iX =  max( iX  % (self.XResolution -5) , (self.XResolution - self.BOX_W * self.NUM_YIELDS_IN_A_ROW) / 2)
			iX =  max( iX  % (self.BOX_X + self.NUM_YIELDS_IN_A_ROW * self.BOX_W -1) , (self.XResolution - (self.XResolution / self.NUM_YIELDS_IN_A_ROW) * self.NUM_YIELDS_IN_A_ROW) / 2)
			## R&R, Robert Surcouf,  Multiple rows of Yields if necessary END
		screen.setLabelAt("DialogWarehouse", "DialogBackground", u"<font=4>" + localText.getText("TXT_KEY_EU_WAREHOUSE_INFO", (pCity.getName(), pCity.getMaxYieldCapacity())) + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, self.BOX_X, self.BOX_Y - self.STANDARD_MARGIN, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1) 
	
		#city preview	
		if sdToolKit.sdGetVal('komaScreens', player.getID(), 'PreviewMode') == "INTERFACE_DOMESTIC_ADVISOR":
			szName = "DialogImage" + str(index)
			if not szName in self.PreviewPlotList:
				screen.addPlotGraphicGFC(szName, self.DIALOG_X + self.MAP_X, self.DIALOG_Y + self.MAP_Y, self.MAP_SIZE, self.MAP_SIZE, pCity.plot(), 350, true, WidgetTypes.WIDGET_GENERAL, -1, -1)
				screen.setLabelAt(szName + "Label", szName, u"<font=4>" + localText.getText("TXT_KEY_EU_SAIL", ()) + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.MAP_SIZE / 2, self.STANDARD_MARGIN * 3 / 2, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
				self.createBorder(0, 0, self.MAP_SIZE, self.MAP_SIZE, self.BORDER_SIZE, szName, true)
				self.PreviewPlotList.append(szName)
			else:
				screen.show(szName)
				screen.moveToFront(szName)
		
	
	def getPlotLists (self, unit) :
		player = gc.getPlayer(gc.getGame().getActivePlayer())
	
		#Europe plot list
		self.EuropePlotListEast = []
		self.EuropePlotListWest = []
		## R&R, vetiarvind, navigation sectors - START
		self.EuropePlotListSectorsEast = [[] for _ in range(6)]
		self.EuropePlotListSectorsWest = [[] for _ in range(6)]
		europePlotListSectorsEast_All = [[] for _ in range(6)] 
		europePlotListSectorsWest_All = [[] for _ in range(6)]
		mapWidth = CyMap().getGridWidth()
		mapHeight = CyMap().getGridHeight()			
		## R&R, vetiarvind, navigation sectors - END

		for i in range(CyMap().numPlots()):
			pLoopPlot = CyMap().plotByIndex(i)
			curX = pLoopPlot.getX()
			curY = pLoopPlot.getY()
			if CyMap().isPlot(pLoopPlot.getX(), pLoopPlot.getY()):
				if pLoopPlot.isRevealed(player.getTeam(), false):
					if pLoopPlot.isEurope():						
						## R&R, vetiarvind, navigation sectors - START
						
						#if pLoopPlot.getX() >= CyMap().getGridWidth() / 2:
						#	self.EuropePlotListEast.append(pLoopPlot)																					
						#else:
						#	self.EuropePlotListWest.append(pLoopPlot)
							
						isEuropeEdgePlot = False
						if pLoopPlot.getX() >= mapWidth >> 1:
							self.EuropePlotListEast.append(pLoopPlot)														
							sectorList = self.EuropePlotListSectorsEast	
							allPlotsInSector = europePlotListSectorsEast_All
							if CyMap().isPlot(curX-1,curY):							
								isEuropeEdgePlot = not CyMap().plotByIndex(CyMap().plotNum(curX-1,curY)).isEurope()
						else:
							self.EuropePlotListWest.append(pLoopPlot)
							sectorList = self.EuropePlotListSectorsWest		
							allPlotsInSector = europePlotListSectorsWest_All
							if CyMap().isPlot(curX+1,curY):							
								isEuropeEdgePlot = not CyMap().plotByIndex(CyMap().plotNum(curX+1,curY)).isEurope()						
												
						if curY < (mapHeight / 6):
							sectorIndex = 5
						elif curY < (mapHeight / 3):
							sectorIndex = 4
						elif curY < (mapHeight / 2):
							sectorIndex = 3
						elif curY < (mapHeight << 1) / 3:
							sectorIndex = 2
						elif curY < (5 * mapHeight / 6):
							sectorIndex = 1
						else:
							sectorIndex = 0
							
						if not isEuropeEdgePlot:							
							allPlotsInSector[sectorIndex].append(pLoopPlot)
						else:						
							sectorList[sectorIndex].append(pLoopPlot)
		for i in range(6):
			if not self.EuropePlotListSectorsEast[i]:
				self.EuropePlotListSectorsEast[i].extend(europePlotListSectorsEast_All[i])
			if not self.EuropePlotListSectorsWest[i]:
				self.EuropePlotListSectorsWest[i].extend(europePlotListSectorsWest_All[i])
		## R&R, vetiarvind, navigation sectors - END

		pCenterPlot = self.getCenterPlot()
		
		self.pPlotEast = self.getNextOceanPlot(pCenterPlot, self.EuropePlotListEast)
		self.pPlotWest = self.getNextOceanPlot(pCenterPlot, self.EuropePlotListWest)
		
		self.EuropePlotList = self.EuropePlotListEast + self.EuropePlotListWest
		
		plotEast = self.pPlotEast
		plotWest = self.pPlotWest
		
		if plotEast == None:
			plotEast = self.getNextOceanPlot(pCenterPlot, self.EuropePlotList)
		if plotWest == None:
			plotWest = self.getNextOceanPlot(pCenterPlot, self.EuropePlotList)
		
		#City list
		self.CityPlotList = []
			
		(city, iter) = player.firstCity(false)

		while (city):
			if (city.isCoastal(gc.getMIN_WATER_SIZE_FOR_OCEAN()) and city.isAccessibleByShip(unit)):
				if unit.getGroup().generatePath(plotEast, city.plot(), 0, false, None):
					self.CityPlotList.append([city, None])
				elif unit.getGroup().generatePath(plotWest, city.plot(), 0, false, None):
					self.CityPlotList.append([city, None])
			(city, iter) = player.nextCity(iter, false)
	
	
	def getCenterPlot (self):
		player = gc.getPlayer(gc.getGame().getActivePlayer())
	
		pCenterPlot = player.getStartingPlot()
		iCenterX, iCenterY, iCityCount = 0, 0, 0
	
		(city, iter) = player.firstCity(false)
		while (city):
			iCenterX += city.plot().getX()
			iCenterY += city.plot().getY()
			iCityCount += 1
			(city, iter) = player.nextCity(iter, false)
	
		if iCityCount > 0:
			iCenterX /= iCityCount
			iCenterY /= iCityCount
			pCenterPlot = CyMap().plot(iCenterX, iCenterY)
	
		return pCenterPlot
	
	
	def getBestCityPlot (self, pCityPlot) :
		pBestPlot = self.getNextOceanPlot(pCityPlot, self.EuropePlotList)
		iPath = self.getPathDistance(pBestPlot, pCityPlot)
		
		if iPath <= CyMap().getGridWidth() / 2:
			return pBestPlot
	
		if len(self.EuropePlotListEast) > 0:
			iPathEast = self.getPathDistance(self.pPlotEast, pCityPlot)
			if iPathEast < iPath:
				iPath = iPathEast
				pBestPlot = self.pPlotEast
	
		if len(self.EuropePlotListWest) > 0:
			iPathWest = self.getPathDistance(self.pPlotWest, pCityPlot)
			if iPathWest < iPath:
				pBestPlot = self.pPlotWest
	
		return pBestPlot

	
	def getNextOceanPlot (self, pPlot, EuropePlotSide) :
		pBestPlot = None
		iBestDistance = 1000

		for pLoopPlot in EuropePlotSide:
			iDistance = plotDistance(pLoopPlot.getX(), pLoopPlot.getY(), pPlot.getX(), pPlot.getY())
			if iDistance < iBestDistance:
				iBestDistance = iDistance
				pBestPlot = pLoopPlot
	
		return pBestPlot
	
	
	def getPathDistance (self, pPlot, pCityPlot):
		iX = pCityPlot.getX()
		iY = pCityPlot.getY()
		bestPathDistance = 1000
	
		for iDirection in range(CardinalDirectionTypes.NUM_CARDINALDIRECTION_TYPES):
			pPathPlot = plotCardinalDirection(iX, iY, CardinalDirectionTypes(iDirection))
			if pPathPlot != None:
				if pPathPlot.isWater():
					iPathDistance = CyMap().calculatePathDistance(pPlot, pPathPlot)
					if not iPathDistance == -1:
						if iPathDistance < bestPathDistance:
							bestPathDistance = iPathDistance

		return bestPathDistance
	
	
	def cargoMessage(self, iUnit):
		#cargo info
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		transport = player.getUnit(iUnit)

		if (transport.getUnitTravelState() == UnitTravelStates.UNIT_TRAVEL_STATE_FROM_PORT_ROYAL):
			currentCargoDictionary = self.OutboundCargoDictionary
		else:
			currentCargoDictionary = self.InboundCargoDictionary

		szText = localText.getText("[COLOR_HIGHLIGHT_TEXT]%s1[COLOR_REVERT][NEWLINE]", (transport.getName(),""))
		szText += localText.getText("TXT_KEY_ARRIVALS_IN", ("",transport.getUnitTravelTimer()))
	
		for iCargoUnit in currentCargoDictionary:
			if iCargoUnit == transport.getID():
				for cargo in currentCargoDictionary[iCargoUnit]:
					szCargoName = gc.getUnitInfo(cargo).getDescription()
					if currentCargoDictionary[iCargoUnit][cargo] > 1:
						szCargoName = gc.getUnitInfo(cargo).getDescriptionForm(1)

					szText += localText.getText("[NEWLINE][COLOR_BUILDING_TEXT][ICON_BULLET]", ())
					szText += u"<font=2>%d %s</font>" % (currentCargoDictionary[iCargoUnit][cargo], szCargoName)
					szText += localText.getText("[COLOR_REVERT]", ())
		
		szText += self.travelMessage(iUnit)

		return szText
	
	
	def travelMessage(self, iUnit):
		#destination info
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		plot = player.getUnit(iUnit).plot()
		nextCityName = localText.getText("%s1", (CyMap().findCity (plot.getX(), plot.getY(), player.getID(), -1, true, true, -1, -1, plot.getPlotCity()).getName(), ()))
	
		if (plot.getX() >= CyMap().getGridWidth() / 2):
			direction = localText.getText("TXT_KEY_EU_SAIL_EAST", ())
		else:
			direction = localText.getText("TXT_KEY_EU_SAIL_WEST", ())
		if (nextCityName == ""):
			nextCity = u""
		elif player.getUnit(iUnit).getGroup().getLengthMissionQueue() >= 1 :
			nextCityName = localText.getText("%s1", (player.getUnit(iUnit).getGroup().lastMissionPlot().getPlotCity().getName(), ()))
			nextCity = localText.getText("TXT_KEY_EU_TO_CITY", (nextCityName, ()))
		else:
			nextCity = localText.getText("TXT_KEY_EU_NEAR_CITY", (nextCityName, ()))
			direction = localText.getText("[COLOR_YELLOW]", ()) + direction + localText.getText("[COLOR_REVERT]", ())
		if self.iPlotDebug:
			direction += u" (%d, %d)" % (plot.getX(), plot.getY())
	
		return localText.getText("TXT_KEY_EU_ROUTE", (direction, nextCity))
	
	
	def playSound(self, SoundName):
		self.iSoundID = CyAudioGame().Play2DSound(SoundName)
		return self.iSoundID
	
	
	def setSound(self, SoundType):
		screen = self.getScreen()
		iCivilization = gc.getPlayer(gc.getGame().getActivePlayer()).getCivilizationType()
		
		if SoundType == INIT_SOUND:
			if (self.iSoundID != 0):
				CyAudioGame().Destroy2DSound(self.iSoundID)

			if self.iThisWinter:
				unitsVolume = 1.0
				screen.setSoundId(self.playSound("AS2D_SS_JUNGLELOOP"))
			else:
				unitsVolume = 0.75
				screen.setSoundId(self.playSound("AS2D_SS_PORTROYALELOOP"))

			CyAudioGame().Set2DSoundVolume(self.iSoundID, unitsVolume)

		elif SoundType == UNIT_SOUND_SELECT:
			iSelectionSound = gc.getCivilizationInfo(iCivilization).getSelectionSoundScriptId()
			screen.setSoundId(CyAudioGame().Play3DSoundWithId(iSelectionSound, -1, -1, -1))
	
		elif SoundType == UNIT_SOUND_ORDER:
			iActionSound = gc.getCivilizationInfo(iCivilization).getActionSoundScriptId()
			screen.setSoundId(CyAudioGame().Play3DSoundWithId(iActionSound, -1, -1, -1))
	
	
	def getShipSellPrice(self, iUnit):
		if iUnit != -1:
			player = gc.getPlayer(gc.getGame().getActivePlayer())
			iTrainPercent = gc.getGameSpeedInfo(CyGame().getGameSpeedType()).getTrainPercent()
			if player.getUnit(iUnit).getUnitType() > -1:
				iSellPrice = self.iSellShip * gc.getUnitInfo(player.getUnit(iUnit).getUnitType()).getEuropeCost() * iTrainPercent / 1200
				iSellPrice -= iSellPrice * player.getTaxRate() / 100
				if iSellPrice > 0:
					return iSellPrice
		
		return 0
	
	
	def getMaxTravelTimer(self, transportPlot):
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		iMaxTravelTimer = (gc.getEuropeInfo(transportPlot.getEurope()).getTripLength() * gc.getGameSpeedInfo(CyGame().getGameSpeedType()).getGrowthPercent()) / 100
	
		for i in range(gc.getNumTraitInfos()):
			if player.hasTrait(i):
				if (gc.getTraitInfo(i).getEuropeTravelTimeModifier() != 0):
					iMaxTravelTimer *= 100 + gc.getTraitInfo(i).getEuropeTravelTimeModifier()
					iMaxTravelTimer /= 100
	
		return iMaxTravelTimer

	
	def getBoycottPrice(self, iYield):
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		iStorage = gc.getGameSpeedInfo(CyGame().getGameSpeedType()).getStoragePercent()
		iBoycottPrice = self.iBoycott * iStorage * player.getYieldSellPrice(iYield)
		iBoycottPrice += iBoycottPrice * player.getTaxRate() / 100
	
		return iBoycottPrice
	
	## R&R, vetiarvind, Price dependent tax increase - START	
	def getYieldScore(self, iYield):
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		# WTP, ray fixing that the value is read from wrong player
		playerEurope = gc.getPlayer(player.getParent())
		iScore = playerEurope.getYieldScoreTotalINT(iYield)
		return iScore
	
	def getTotalYieldsScore(self):
		iTotalScore = 0
		for iYield in range(YieldTypes.NUM_YIELD_TYPES):
			iTotalScore += self.getYieldScore(iYield)
		return iTotalScore
	## R&R, vetiarvind, Price dependent tax increase - END
	
	# R&R, Robert Surcouf, No More Variables Hidden game option START
	def getYieldTraded(self, iYield):
		iTraded = 0
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		#iTraded += player.getYieldTradedTotal(iYield)
		iTraded += player.getYieldTradedTotalINT(iYield)
		return iTraded
		
	def getTotalYieldsTraded(self):
		iTotalTraded = 0
		for iYield in range(YieldTypes.NUM_YIELD_TYPES):
			iTotalTraded += self.getYieldTraded(iYield)
		return iTotalTraded
	
	def getAIattitudeVal(self):	
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		playerEurope = gc.getPlayer(player.getParent())
		iKingAttitude = player.AI_getAttitudeVal(playerEurope.getID())
		return iKingAttitude
		
	def getTaxMultiplier(self):
		iMultiplier = 100
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		playerEurope = gc.getPlayer(player.getParent())
		for i in range(gc.getNumTraitInfos()):
			if player.hasTrait(i):
				iMultiplier += + gc.getTraitInfo(i).getTaxRateThresholdModifier();
		iMultiplier += player.getTaxRate()* gc.getDefineINT("TAX_TRADE_THRESHOLD_TAX_RATE_PERCENT") / 100
		#iMultiplier += self.getAIattitudeVal()  * gc.getDefineINT("TAX_TRADE_THRESHOLD_ATTITUDE_PERCENT")
		return iMultiplier
	
	def getTaxTreshold(self):
		iThreshold = (max(self.getTaxMultiplier(),gc.getDefineINT("MAX_TAX_TRADE_MULTIPLIER") )* gc.getDefineINT("TAX_TRADE_THRESHOLD")) * gc.getGameSpeedInfo(gc.getGame().getGameSpeedType()).getGrowthPercent() / 10000
		#iThreshold /= 100
		return iThreshold
	
	def getChanceModifierFromKingAttitude(self):
		iChanceModifier= self.getAIattitudeVal()  * gc.getDefineINT("TAX_TRADE_INCREASE_CHANCE_KING_ATTITUDE_BASE")
		if iChanceModifier > 50:
			iChanceModifier= 50
		elif iChanceModifier <- 50:
			iChanceModifier= -50
		return iChanceModifier
	
	def getChanceProb(self):
		iChance =(1000* gc.getDefineINT("TAX_INCREASE_CHANCE")) / (100+self.getChanceModifierFromKingAttitude())
		if iChance > 1000:
			iChance = 1000
		return iChance
	# R&R, Robert Surcouf, No More Variables Hidden game option END

