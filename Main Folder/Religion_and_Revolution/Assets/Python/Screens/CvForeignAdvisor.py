## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import math

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

FOREIGN_RELATIONS_SCREEN = 0
FOREIGN_ACTIVE_TRADE_SCREEN = 1
NUM_FOREIGN_SCREENS = 2

class CvForeignAdvisor:
	"Foreign Advisor Screen"

	def __init__(self):
		self.iScreen = -1
		self.nWidgetCount = 0
		self.nLineCount = 0
		self.WIDGET_ID = "ForeignAdvisorWidget"
		self.LINE_ID = "ForeignAdvisorLine"
		self.SCREEN_NAME = "ForeignAdvisor"
		self.DEBUG_DROPDOWN_ID =  "ForeignAdvisorDropdownWidget"
		self.BACKGROUND_ID = "ForeignAdvisorBackground"
		self.Y_TITLE = 4
		self.X_LEADER = 80
		self.Y_LEADER = 115
		self.H_LEADER = 64
		self.W_LEADER = 64

		self.X_LEGEND = 20
		self.Y_LEGEND = 530
		self.H_LEGEND = 180
		self.W_LEGEND = 160
		self.MARGIN_LEGEND = 10

		self.RADIUS_LEADER_ARC = 480
		self.LINE_WIDTH = 6
		self.BUTTON_SIZE = 64

		self.iSelectedLeader = -1
		self.iActiveLeader = -1
		self.listSelectedLeaders = []
		self.iShiftKeyDown = 0

		self.iDefaultScreen = FOREIGN_RELATIONS_SCREEN

	def killScreen(self):
		if (self.iScreen >= 0):
			screen = self.getScreen()
			screen.hideScreen()
			self.iScreen = -1
		return

	def getScreen(self):
		return CyGInterfaceScreen(self.SCREEN_NAME + str(self.iScreen), CvScreenEnums.FOREIGN_ADVISOR)

	def interfaceScreen (self, iScreen):

		self.XResolution = self.getScreen().getXResolution()
		self.YResolution = self.getScreen().getYResolution()

		if (iScreen < 0):
			if (self.iScreen < 0):
				iScreen = self.iDefaultScreen
			else:
				iScreen = self.iScreen

		self.X_LEADER_CIRCLE_TOP = (self.XResolution / 2) + 10
		self.Y_LEADER_CIRCLE_TOP = 107

		self.EXIT_TEXT = u"<font=4>" + localText.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + u"</font>"
		self.SCREEN_TITLE = u"<font=4b>" + localText.getText("TXT_KEY_FOREIGN_ADVISOR_TITLE", ()).upper() + u"</font>"
		self.SCREEN_TITLE = localText.changeTextColor(self.SCREEN_TITLE, gc.getInfoTypeForString("COLOR_FONT_CREAM"))

		if (self.iScreen != iScreen):
			self.killScreen()
			self.iScreen = iScreen

		screen = self.getScreen()
		if screen.isActive():
			return
		screen.setRenderInterfaceOnly(True);
		screen.showScreen( PopupStates.POPUPSTATE_IMMEDIATE, False)

		self.iActiveLeader = CyGame().getActivePlayer()
		self.iSelectedLeader = self.iActiveLeader
		self.listSelectedLeaders = []

		# Set the background and exit button, and show the screen
		screen.setDimensions(0, 0, self.XResolution, self.YResolution)
		screen.addDrawControl(self.BACKGROUND_ID, ArtFileMgr.getInterfaceArtInfo("SCREEN_BG_OPAQUE").getPath(), 0, 0, self.XResolution, self.YResolution, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDDSGFC("TopPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TITLE").getPath(), 0, 0, self.XResolution, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.showWindowBackground(False)		

		self.nWidgetCount = 0
		self.nLineCount = 0

		if (CyGame().isDebugMode()):
			self.szDropdownName = self.getWidgetName(self.DEBUG_DROPDOWN_ID)
			screen.addDropDownBoxGFC(self.szDropdownName, 22, 12, 300, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			for j in range(gc.getMAX_PLAYERS()):
				if (gc.getPlayer(j).isAlive()):
					screen.addPullDownString(self.szDropdownName, gc.getPlayer(j).getName(), j, j, False )

		CyInterface().setDirty(InterfaceDirtyBits.Foreign_Screen_DIRTY_BIT, False)

		# Draw leader heads
		self.drawContents(True)

	# Drawing Leaderheads
	def drawContents(self, bInitial):
		if (self.iScreen < 0):
			return

		self.deleteAllWidgets()
		screen = self.getScreen()

		# Header...
		screen.setLabel(self.getNextWidgetName(), "", self.SCREEN_TITLE, CvUtil.FONT_CENTER_JUSTIFY, self.XResolution / 2, self.Y_TITLE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		if (self.iScreen == FOREIGN_RELATIONS_SCREEN):
			self.drawRelations(bInitial)
		elif (self.iScreen == FOREIGN_ACTIVE_TRADE_SCREEN):
			self.drawActive()
		self.drawTabs()

	def drawActive(self):
		screen = self.getScreen()

		# Get the Players
		playerActive = gc.getPlayer(self.iActiveLeader)

		# Put everything inside a main panel, so we get vertical scrolling
		mainPanelName = self.getNextWidgetName()
		#screen.addPanel(mainPanelName, "", "", True, True, 50, 100, self.XResolution - 100, self.YResolution - 200, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addScrollPanel(mainPanelName, u"", 50, 100, self.XResolution - 100, self.YResolution - 200, PanelStyles.PANEL_STYLE_EMPTY, true, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# loop through all players and sort them by number of active deals
		listPlayers = [(0,0)] * gc.getMAX_PLAYERS()
		nNumPLayers = 0
		for iLoopPlayer in range(gc.getMAX_PLAYERS()):
			if (gc.getPlayer(iLoopPlayer).isAlive() and iLoopPlayer != self.iActiveLeader):
				# < JAnimals Mod Start 1/4 >
				if (gc.getGame().isBarbarianPlayer(iLoopPlayer)):
					continue
				# < JAnimals Mod End 1/4 >
				if (gc.getTeam(gc.getPlayer(iLoopPlayer).getTeam()).isHasMet(gc.getPlayer(self.iActiveLeader).getTeam()) or gc.getGame().isDebugMode()):
					nDeals = 0
					for i in range(gc.getGame().getIndexAfterLastDeal()):
						deal = gc.getGame().getDeal(i)
						if ((deal.getFirstPlayer() == iLoopPlayer and deal.getSecondPlayer() == self.iActiveLeader) or (deal.getSecondPlayer() == iLoopPlayer and deal.getFirstPlayer() == self.iActiveLeader)):
							nDeals += 1
					listPlayers[nNumPLayers] = (nDeals, iLoopPlayer)
					nNumPLayers += 1
		listPlayers.sort()
		listPlayers.reverse()

		# loop through all players and display leaderheads
		for j in range (nNumPLayers):
			iLoopPlayer = listPlayers[j][1]
			# < JAnimals Mod Start 2/4 >
			if (gc.getGame().isBarbarianPlayer(iLoopPlayer)):
				continue
			# < JAnimals Mod End 2/4 >

			# Player panel
			playerPanelName = self.getNextWidgetName()

			screen.attachPanelAt(mainPanelName, playerPanelName, "", "", True, True, PanelStyles.PANEL_STYLE_MAIN, 0, 20 + (j * 140), self.XResolution - 135, 120, WidgetTypes.WIDGET_GENERAL, -1, -1 )
			screen.addDDSGFCAt("Portrait" + str(j), mainPanelName, gc.getLeaderHeadInfo(gc.getPlayer(iLoopPlayer).getLeaderType()).getButton(), 20, 40 + (j * 140), 80, 80, WidgetTypes.WIDGET_GENERAL, -1, -1, False)
			# TAC/Ronnar Start: Add civilization name to leader
			szTempBuffer = u"<color=%d,%d,%d,%d>%s <font=3>(%s)</font></color>  " %(gc.getPlayer(iLoopPlayer).getPlayerTextColorR(), gc.getPlayer(iLoopPlayer).getPlayerTextColorG(), gc.getPlayer(iLoopPlayer).getPlayerTextColorB(), gc.getPlayer(iLoopPlayer).getPlayerTextColorA(), gc.getPlayer(iLoopPlayer).getName(), gc.getPlayer(iLoopPlayer).getCivilizationDescription(iLoopPlayer))
			# TAC/Ronnar End
			screen.setTextAt("NameString" + str(j), mainPanelName, "<font=4>" + szTempBuffer + "</font>", CvUtil.FONT_LEFT_JUSTIFY, 30, 10 + (j * 140), -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

			iRow = 0
			for i in range(gc.getGame().getIndexAfterLastDeal()):
				deal = gc.getGame().getDeal(i)

				if (deal.getFirstPlayer() == iLoopPlayer and deal.getSecondPlayer() == self.iActiveLeader and not deal.isNone()) or (deal.getSecondPlayer() == iLoopPlayer and deal.getFirstPlayer() == self.iActiveLeader):
					screen.setTextAt("DealString" + str(i), playerPanelName, "<font=3>" + CyGameTextMgr().getDealString(deal, iLoopPlayer) + "</font>", CvUtil.FONT_LEFT_JUSTIFY, 110, 20 + (iRow * 20), -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_DEAL_KILL, deal.getID(), -1 )
					iRow += 1

	def drawRelations(self, bInitial):

		if self.iShiftKeyDown == 1:
			if (self.iSelectedLeader in self.listSelectedLeaders):
				self.listSelectedLeaders.remove(self.iSelectedLeader)
			else:
				self.listSelectedLeaders.append(self.iSelectedLeader)
		else:
			self.listSelectedLeaders = []
			if (not bInitial):
				self.listSelectedLeaders.append(self.iSelectedLeader)

		bNoLeadersSelected = (len(self.listSelectedLeaders) == 0)
		bSingleLeaderSelected = (len(self.listSelectedLeaders) == 1)
		if bSingleLeaderSelected:
			self.iSelectedLeader = self.listSelectedLeaders[0]

		# Get the Players
		playerActive = gc.getPlayer(self.iActiveLeader)

		# count the leaders
		iCount = 0
		leaderMap = { }
		# Count all other leaders
		for iPlayer in range(gc.getMAX_PLAYERS()):
			# < JAnimals Mod Start 3/4 >
			if (gc.getGame().isBarbarianPlayer(iPlayer)):
				continue
			# < JAnimals Mod End 3/4 >
			player = gc.getPlayer(iPlayer)
			if (player.isAlive() and iPlayer != self.iActiveLeader and gc.getTeam(player.getTeam()).isHasMet(gc.getPlayer(self.iActiveLeader).getTeam())):
				leaderMap[iPlayer] = iCount
				iCount = iCount + 1
		fLeaderTop = self.Y_LEADER_CIRCLE_TOP
		xRadius = (self.XResolution / 2) - self.H_LEADER - self.H_LEADER
		yRadius = (self.YResolution * 3 / 5)
		if (xRadius > yRadius):
			fRadius = yRadius
		else:
			fRadius = xRadius
		fLeaderArcTop = fLeaderTop + self.H_LEADER + 10

		if iCount < 8:
			iLeaderHeight = int((3 * self.H_LEADER) / 2)
			iLeaderWidth = int((3 * self.W_LEADER) / 2)
		else:
			iLeaderHeight = self.H_LEADER
			iLeaderWidth = self.W_LEADER

		screen = self.getScreen()

		# legend
		screen.addPanel(self.getNextWidgetName(), u"", u"", True, False, self.X_LEGEND, self.YResolution - self.H_LEGEND - 75, self.W_LEGEND, self.H_LEGEND, PanelStyles.PANEL_STYLE_IN, WidgetTypes.WIDGET_GENERAL, -1, -1)
		x = self.X_LEGEND + self.MARGIN_LEGEND
		y = self.YResolution - self.H_LEGEND + self.MARGIN_LEGEND - 75
		screen.setLabel(self.getNextWidgetName(), "", u"<font=2>" + localText.getText("TXT_KEY_FOREIGN_ADVISOR_CONTACT", ()) + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, x, y-10, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		y += self.MARGIN_LEGEND
		screen.addLineGFC(self.BACKGROUND_ID, self.getNextLineName(), x, y, x + self.W_LEGEND - 2*self.MARGIN_LEGEND, y, gc.getInfoTypeForString("COLOR_WHITE"))
		y += 2 * self.MARGIN_LEGEND
		screen.setLabel(self.getNextWidgetName(), "", u"<font=2>" + localText.getText("TXT_KEY_WAR", ()) + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, x, y-10, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		y += self.MARGIN_LEGEND
		screen.addLineGFC(self.BACKGROUND_ID, self.getNextLineName(), x, y, x + self.W_LEGEND - 2*self.MARGIN_LEGEND, y, gc.getInfoTypeForString("COLOR_RED"))
		y += 2 * self.MARGIN_LEGEND
		screen.setLabel(self.getNextWidgetName(), "", u"<font=2>" + localText.getText("TXT_KEY_TRADE_DEFENSIVE_PACT_STRING", ()) + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, x, y-10, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		y += self.MARGIN_LEGEND
		screen.addLineGFC(self.BACKGROUND_ID, self.getNextLineName(), x, y, x + self.W_LEGEND - 2*self.MARGIN_LEGEND, y, gc.getInfoTypeForString("COLOR_BLUE"))
		y += 2 * self.MARGIN_LEGEND
		screen.setLabel(self.getNextWidgetName(), "", u"<font=2>" + localText.getText("TXT_KEY_TRADE_OPEN_BORDERS_STRING", ()) + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, x, y-10, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		y += self.MARGIN_LEGEND
		screen.addLineGFC(self.BACKGROUND_ID, self.getNextLineName(), x, y, x + self.W_LEGEND - 2*self.MARGIN_LEGEND, y, gc.getInfoTypeForString("COLOR_CITY_GREEN"))
		y += 2 * self.MARGIN_LEGEND
		screen.setLabel(self.getNextWidgetName(), "", u"<font=2>" + localText.getText("TXT_KEY_PITBOSS_TEAM", ()) + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, x, y-10, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		y += self.MARGIN_LEGEND
		screen.addLineGFC(self.BACKGROUND_ID, self.getNextLineName(), x, y, x + self.W_LEGEND - 2*self.MARGIN_LEGEND, y, gc.getInfoTypeForString("COLOR_YELLOW"))

		# Our leader head
		szLeaderHead = self.getNextWidgetName()
		screen.addCheckBoxGFC(szLeaderHead, gc.getLeaderHeadInfo(gc.getPlayer(self.iActiveLeader).getLeaderType()).getButton(), ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), self.X_LEADER_CIRCLE_TOP - iLeaderWidth/2, int(fLeaderTop), iLeaderWidth, iLeaderHeight, WidgetTypes.WIDGET_LEADERHEAD, self.iActiveLeader, -1, ButtonStyles.BUTTON_STYLE_LABEL)
		if (self.iActiveLeader in self.listSelectedLeaders):
			screen.setState(szLeaderHead, True)
		else:
			screen.setState(szLeaderHead, False)
		szName = self.getNextWidgetName()
		szLeaderName = u"<font=3>" + playerActive.getName() + u"</font>"
		screen.setLabel(szName, "", szLeaderName, CvUtil.FONT_CENTER_JUSTIFY, self.X_LEADER_CIRCLE_TOP, fLeaderTop + iLeaderHeight + 5, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# angle increment in radians (180 degree range)
		if (iCount < 2):
			deltaTheta = 0
		else:
			deltaTheta = 3.1415927 / (iCount - 1)

		# draw other leaderheads
		for iPlayer in leaderMap.keys():
			# < JAnimals Mod Start 4/4 >
			if (gc.getGame().isBarbarianPlayer(iPlayer)):
				continue
			# < JAnimals Mod End 4/4 >
			player = gc.getPlayer(iPlayer)

			if bSingleLeaderSelected:
				# attitudes shown are towards single selected leader
				iBaseLeader = self.iSelectedLeader
			else:
				# attitudes shown are towards active leader
				iBaseLeader = self.iActiveLeader
			playerBase = gc.getPlayer(iBaseLeader)
			fX = int(self.X_LEADER_CIRCLE_TOP - fRadius * math.cos(deltaTheta * leaderMap[iPlayer]) - iLeaderWidth/2)
			fY = int(fLeaderArcTop + fRadius * math.sin(deltaTheta * leaderMap[iPlayer]) - iLeaderHeight/2)

			szLeaderHead = self.getNextWidgetName()
			screen.addCheckBoxGFC(szLeaderHead, gc.getLeaderHeadInfo(player.getLeaderType()).getButton(), ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), int(fX), int(fY), iLeaderWidth, iLeaderHeight, WidgetTypes.WIDGET_LEADERHEAD, iPlayer, iBaseLeader, ButtonStyles.BUTTON_STYLE_LABEL)
			if (iPlayer in self.listSelectedLeaders):
				screen.setState(szLeaderHead, True)
			else:
				screen.setState(szLeaderHead, False)

			szName = self.getNextWidgetName()

			textColorR = player.getPlayerTextColorR()
			textColorG = player.getPlayerTextColorG()
			textColorB = player.getPlayerTextColorB()
			textColorA = player.getPlayerTextColorA()

			szText = u"<font=3>" + u"<color=%d,%d,%d,%d>" %(textColorR, textColorG, textColorB, textColorA) + player.getName() + u"</font>"
			screen.setLabel(szName, "", szText, CvUtil.FONT_CENTER_JUSTIFY, fX + iLeaderWidth/2, fY + iLeaderHeight + 5, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
			
			#TAC/Ronnar: Add Civilization to leader name
			szName = self.getNextWidgetName()
			szText = u"<color=%d,%d,%d,%d>(" %(textColorR, textColorG, textColorB, textColorA) + player.getCivilizationDescription(iPlayer) + u")"
			screen.setLabel(szName, "", szText, CvUtil.FONT_CENTER_JUSTIFY, fX + iLeaderWidth/2, fY + iLeaderHeight + 25, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )			
			# TAc End

			# Leader attitude towards active player
			szName = self.getNextWidgetName()
			if (gc.getTeam(player.getTeam()).isHasMet(playerBase.getTeam()) and iBaseLeader != iPlayer):
				szText = " (" + gc.getAttitudeInfo(gc.getPlayer(iPlayer).AI_getAttitude(iBaseLeader)).getDescription() + ")"
			else:
				szText = u""
			screen.setLabel(szName, "", szText, CvUtil.FONT_CENTER_JUSTIFY, fX + iLeaderWidth/2, fY + iLeaderHeight + 40, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
			

		# draw lines
		for iSelectedLeader in range(gc.getMAX_PLAYERS()):
			bDisplayed = (gc.getPlayer(iSelectedLeader).isAlive() and (gc.getGame().isDebugMode() or gc.getTeam(playerActive.getTeam()).isHasMet(gc.getPlayer(iSelectedLeader).getTeam())))

			if iSelectedLeader in self.listSelectedLeaders or (bNoLeadersSelected and bDisplayed):
				# get selected player and location
				if (iSelectedLeader in leaderMap):
					thetaSelected = deltaTheta * leaderMap[iSelectedLeader]
					fXSelected = self.X_LEADER_CIRCLE_TOP - fRadius * math.cos(thetaSelected)
					fYSelected = fLeaderArcTop + fRadius * math.sin(thetaSelected)
				else:
					fXSelected = self.X_LEADER_CIRCLE_TOP
					fYSelected = fLeaderTop + iLeaderHeight/2

				for iPlayer in leaderMap.keys():
					player = gc.getPlayer(iPlayer)

					fX = self.X_LEADER_CIRCLE_TOP - fRadius * math.cos(deltaTheta * leaderMap[iPlayer])
					fY = fLeaderArcTop + fRadius * math.sin(deltaTheta * leaderMap[iPlayer])

					# draw lines
					if (iSelectedLeader != iPlayer):
						if (player.getTeam() == gc.getPlayer(iSelectedLeader).getTeam()):
							szName = self.getNextLineName()
							screen.addLineGFC(self.BACKGROUND_ID, szName, int(fXSelected), int(fYSelected), int(fX), int(fY), gc.getInfoTypeForString("COLOR_YELLOW") )
						elif (gc.getTeam(player.getTeam()).isHasMet(gc.getPlayer(iSelectedLeader).getTeam())):
							if (gc.getTeam(player.getTeam()).isAtWar(gc.getPlayer(iSelectedLeader).getTeam())):
								szName = self.getNextLineName()
								screen.addLineGFC(self.BACKGROUND_ID, szName, int(fXSelected), int(fYSelected), int(fX), int(fY), gc.getInfoTypeForString("COLOR_RED") )
							else:
								bJustPeace = True
								if (gc.getTeam(player.getTeam()).isOpenBorders(gc.getPlayer(iSelectedLeader).getTeam())):
									fDy = fYSelected - fY
									fDx = fXSelected - fX
									fTheta = math.atan2(fDy, fDx)
									if (fTheta > 0.5 * math.pi):
										fTheta -= math.pi
									elif (fTheta < -0.5 * math.pi):
										fTheta += math.pi
									fSecondLineOffsetY = self.LINE_WIDTH * math.cos(fTheta)
									fSecondLineOffsetX = -self.LINE_WIDTH * math.sin(fTheta)
									szName = self.getNextLineName()
									screen.addLineGFC(self.BACKGROUND_ID, szName, int(fXSelected + fSecondLineOffsetX), int(fYSelected + fSecondLineOffsetY), int(fX + fSecondLineOffsetX), int(fY + fSecondLineOffsetY), gc.getInfoTypeForString("COLOR_CITY_GREEN") )
									bJustPeace = False
								if (gc.getTeam(player.getTeam()).isDefensivePact(gc.getPlayer(iSelectedLeader).getTeam())):
									szName = self.getNextLineName()
									screen.addLineGFC(self.BACKGROUND_ID, szName, int(fXSelected), int(fYSelected), int(fX), int(fY), gc.getInfoTypeForString("COLOR_BLUE") )
									bJustPeace = False
								if (bJustPeace):
									szName = self.getNextLineName()
									screen.addLineGFC(self.BACKGROUND_ID, szName, int(fXSelected), int(fYSelected), int(fX), int(fY), gc.getInfoTypeForString("COLOR_WHITE") )

				player = gc.getPlayer(self.iActiveLeader)
				if (player.getTeam() == gc.getPlayer(iSelectedLeader).getTeam()):
					szName = self.getNextLineName()
					screen.addLineGFC(self.BACKGROUND_ID, szName, int(fXSelected), int(fYSelected), self.X_LEADER_CIRCLE_TOP, fLeaderTop + iLeaderHeight/2, gc.getInfoTypeForString("COLOR_YELLOW") )
				elif (gc.getTeam(player.getTeam()).isHasMet(gc.getPlayer(iSelectedLeader).getTeam())):
					if (gc.getTeam(player.getTeam()).isAtWar(gc.getPlayer(iSelectedLeader).getTeam())):
						szName = self.getNextLineName()
						screen.addLineGFC(self.BACKGROUND_ID, szName, int(fXSelected), int(fYSelected), self.X_LEADER_CIRCLE_TOP, fLeaderTop + iLeaderHeight/2, gc.getInfoTypeForString("COLOR_RED") )
					else:
						bJustPeace = True
						if (gc.getTeam(player.getTeam()).isOpenBorders(gc.getPlayer(iSelectedLeader).getTeam())):
							fDy = fLeaderTop + iLeaderHeight/2 - fYSelected
							fDx = self.X_LEADER_CIRCLE_TOP - fXSelected
							fTheta = math.atan2(fDy, fDx)
							if (fTheta > 0.5 * math.pi):
								fTheta -= math.pi
							elif (fTheta < -0.5 * math.pi):
								fTheta += math.pi
							fSecondLineOffsetY = self.LINE_WIDTH * math.cos(fTheta)
							fSecondLineOffsetX = -self.LINE_WIDTH * math.sin(fTheta)
							szName = self.getNextLineName()
							screen.addLineGFC(self.BACKGROUND_ID, szName, int(fXSelected + fSecondLineOffsetX), int(fYSelected + fSecondLineOffsetY), int(self.X_LEADER_CIRCLE_TOP + fSecondLineOffsetX), int(fLeaderTop + iLeaderHeight/2 + fSecondLineOffsetY), gc.getInfoTypeForString("COLOR_CITY_GREEN") )
							bJustPeace = False
						if (gc.getTeam(player.getTeam()).isDefensivePact(gc.getPlayer(iSelectedLeader).getTeam())):
							szName = self.getNextLineName()
							screen.addLineGFC(self.BACKGROUND_ID, szName, int(fXSelected), int(fYSelected), int(self.X_LEADER_CIRCLE_TOP), int(fLeaderTop + iLeaderHeight/2), gc.getInfoTypeForString("COLOR_BLUE") )
							bJustPeace = False
						if (bJustPeace):
							szName = self.getNextLineName()
							screen.addLineGFC(self.BACKGROUND_ID, szName, int(fXSelected), int(fYSelected), int(self.X_LEADER_CIRCLE_TOP), int(fLeaderTop + iLeaderHeight/2), gc.getInfoTypeForString("COLOR_WHITE") )


	def drawTabs(self):

		screen = self.getScreen()

		Tabs = [localText.getText("TXT_KEY_FOREIGN_ADVISOR_RELATIONS", ()), localText.getText("TXT_KEY_FOREIGN_ADVISOR_ACTIVE", ())]
		NumTabs = len(Tabs)
		TabWidth = self.XResolution / (NumTabs + 1)

		for iTab in range(NumTabs):
			OnTabName = "OnTab" + str(iTab)
			EdgeWidth = 25
			BottomPanelHight = 55
			screen.addPanel(OnTabName + "Left", "", "", False, False, TabWidth * iTab, self.YResolution - BottomPanelHight, EdgeWidth, BottomPanelHight, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.addPanel(OnTabName + "Center", "", "", False, False, (TabWidth * iTab) + EdgeWidth, self.YResolution - BottomPanelHight, TabWidth - (EdgeWidth * 2), BottomPanelHight, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.addPanel(OnTabName + "Right", "", "", False, False, (TabWidth * iTab) + TabWidth - EdgeWidth, self.YResolution - BottomPanelHight, EdgeWidth, BottomPanelHight, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)

			screen.addDrawControl(OnTabName + "Left", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_START").getPath(), TabWidth * iTab, self.YResolution - BottomPanelHight, EdgeWidth, BottomPanelHight, WidgetTypes.WIDGET_GENERAL, -1, -1 )
			screen.addDrawControl(OnTabName + "Center", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_ON").getPath(), (TabWidth * iTab) + EdgeWidth, self.YResolution - BottomPanelHight, TabWidth - (EdgeWidth * 2), BottomPanelHight, WidgetTypes.WIDGET_GENERAL, -1, -1 )
			screen.addDrawControl(OnTabName + "Right", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_END").getPath(), (TabWidth * iTab) + TabWidth - EdgeWidth, self.YResolution - BottomPanelHight, EdgeWidth, BottomPanelHight, WidgetTypes.WIDGET_GENERAL, -1, -1 )

			TabText = Tabs[iTab]
			TabText = localText.changeTextColor(TabText, gc.getInfoTypeForString("COLOR_FONT_CREAM"))

			screen.setTextAt("OnTabTitle" + str(iTab), OnTabName + "Center", u"<font=4>" + TabText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, 0 , 33, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.hide(OnTabName + "Left")
			screen.hide(OnTabName + "Center")
			screen.hide(OnTabName + "Right")

			OffTabName = "OffTab" + str(iTab)
			screen.addPanel(OffTabName, "", "", False, False, TabWidth * iTab, self.YResolution - BottomPanelHight, TabWidth, BottomPanelHight, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, 1111, iTab)
			screen.addDrawControl(OffTabName, ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_OFF").getPath(), TabWidth * iTab, self.YResolution - BottomPanelHight, TabWidth, BottomPanelHight, WidgetTypes.WIDGET_GENERAL, -1, -1 )
			screen.setTextAt("OffTabTitle" + str(iTab), OffTabName, u"<font=4>" + TabText + "</font>", CvUtil.FONT_LEFT_JUSTIFY, EdgeWidth , 33, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1111, iTab)

		screen.addPanel("ExitTab", "", "", False, False, TabWidth * NumTabs, self.YResolution - BottomPanelHight, TabWidth, BottomPanelHight, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addDrawControl("ExitTab", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_OFF").getPath(), TabWidth * NumTabs, self.YResolution - BottomPanelHight, TabWidth, BottomPanelHight, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setTextAt("ExitTabTitle", "ExitTab", u"<font=4>" + CyTranslator().getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + "</font>", CvUtil.FONT_RIGHT_JUSTIFY, TabWidth - 30 , BottomPanelHight - 18, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)

		for iTab in range(NumTabs):
			self.setTab(iTab, iTab == self.iScreen)


	def setTab(self, iTab, State):
		screen = self.getScreen()
		OnTabName = "OnTab" + str(iTab)

		if (State):
			screen.show(OnTabName + "Left")
			screen.show(OnTabName + "Center")
			screen.show(OnTabName + "Right")
			screen.hide("OffTab" + str(iTab))
		else:
			screen.hide(OnTabName + "Left")
			screen.hide(OnTabName + "Center")
			screen.hide(OnTabName + "Right")
			screen.show("OffTab" + str(iTab))

	# returns a unique ID for a widget in this screen
	def getNextWidgetName(self):
		szName = self.WIDGET_ID + str(self.nWidgetCount * NUM_FOREIGN_SCREENS + self.iScreen)
		self.nWidgetCount += 1
		return szName

	def getNextLineName(self):
		szName = self.LINE_ID + str(self.nLineCount * NUM_FOREIGN_SCREENS + self.iScreen)
		self.nLineCount += 1
		return szName

	def getWidgetName(self, szBaseName):
		szName = szBaseName + str(self.iScreen)
		return szName

	def clearAllLines(self):
		screen = self.getScreen()
		nLines = self.nLineCount
		self.nLineCount = 0
		for i in range(nLines):
			screen.removeLineGFC(self.BACKGROUND_ID, self.getNextLineName())
		self.nLineCount = 0


	def deleteAllWidgets(self):
		screen = self.getScreen()
		i = self.nWidgetCount - 1
		while (i >= 0):
			self.nWidgetCount = i
			screen.deleteWidget(self.getNextWidgetName())
			i -= 1

		self.nWidgetCount = 0
		self.clearAllLines()

	# Handles the input for this screen...
	def handleInput (self, inputClass):
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if (inputClass.getButtonType() == WidgetTypes.WIDGET_LEADERHEAD):
				if (inputClass.getFlags() & MouseFlags.MOUSE_LBUTTONUP):
					self.iSelectedLeader = inputClass.getData1()
					self.drawContents(False)
				elif (inputClass.getFlags() & MouseFlags.MOUSE_RBUTTONUP) and not inputClass.isShiftKeyDown() and not inputClass.isCtrlKeyDown() and not inputClass.isAltKeyDown():
					if (self.iActiveLeader != inputClass.getData1()):
						self.getScreen().hideScreen()
			elif (inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL and inputClass.getData1() == 1111):
				self.interfaceScreen(inputClass.getData2())
						
		elif (inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED):
			if (inputClass.getFunctionName() + str(inputClass.getID()) == self.getWidgetName(self.DEBUG_DROPDOWN_ID)):
				szName = self.getWidgetName(self.DEBUG_DROPDOWN_ID)
				iIndex = self.getScreen().getSelectedPullDownID(szName)
				self.iActiveLeader = self.getScreen().getPullDownData(szName, iIndex)
				self.drawContents(False)
		elif (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CHARACTER):
			if (inputClass.getData() == int(InputTypes.KB_LSHIFT) or inputClass.getData() == int(InputTypes.KB_RSHIFT)):
				self.iShiftKeyDown = inputClass.getID()


	def update(self, fDelta):
		if (CyInterface().isDirty(InterfaceDirtyBits.Foreign_Screen_DIRTY_BIT)):
			CyInterface().setDirty(InterfaceDirtyBits.Foreign_Screen_DIRTY_BIT, False)
			self.drawContents(False)
		return
