## Sid Meier's Civilization 4
## Copyright Firaxis Games 2006
##
## CvEventManager
## This class is passed an argsList from CvAppInterface.onEvent
## The argsList can contain anything from mouse location to key info
## The EVENTLIST that are being notified can be found

from CvPythonExtensions import *
import CvUtil
import CvScreensInterface
import CvDebugTools
import CvWBPopups
import CvCameraControls
import sys
import CvWorldBuilderScreen
import CvAdvisorUtils

gc = CyGlobalContext()
localText = CyTranslator()

# globals
###################################################
class CvEventManager:
	def __init__(self):
		#################### ON EVENT MAP ######################
		self.bCtrl = False
		self.bShift = False
		self.bAlt = False
		self.bAllowCheats = False

		# OnEvent Enums
		self.EventLButtonDown=1
		self.EventLcButtonDblClick=2
		self.EventRButtonDown=3
		self.EventBack=4
		self.EventForward=5
		self.EventKeyDown=6
		self.EventKeyUp=7

		self.__LOG_MOVEMENT = 0
		self.__LOG_BUILDING = 0
		self.__LOG_COMBAT = 0
		self.__LOG_CONTACT = 0
		self.__LOG_IMPROVEMENT =0
		self.__LOG_CITYBUILT = 0	# TAC - koma13
		self.__LOG_CITYLOST = 0
		self.__LOG_CITYBUILDING = 0
		self.__LOG_UNITBUILD = 0
		self.__LOG_UNITKILLED = 1
		self.__LOG_UNITLOST = 0
		self.__LOG_UNITPROMOTED = 0
		self.__LOG_UNITSELECTED = 0
		self.__LOG_UNITPILLAGE = 0
		self.__LOG_GOODYRECEIVED = 0
		self.__LOG_WARPEACE = 0
		self.__LOG_PUSH_MISSION = 0

		## EVENTLIST
		self.EventHandlerMap = {
			'mouseEvent'			: self.onMouseEvent,
			'kbdEvent' 				: self.onKbdEvent,
			'ModNetMessage'			: self.onModNetMessage,
			'Init'					: self.onInit,
			'Update'				: self.onUpdate,
			'UnInit'				: self.onUnInit,
			'OnSave'				: self.onSaveGame,
			'OnPreSave'				: self.onPreSave,
			'OnLoad'				: self.onLoadGame,
			'GameStart'				: self.onGameStart,
			'GameEnd'				: self.onGameEnd,
			'plotRevealed' 			: self.onPlotRevealed,
			'plotFeatureRemoved' 	: self.onPlotFeatureRemoved,
			'plotPicked'			: self.onPlotPicked,
			'gotoPlotSet'			: self.onGotoPlotSet,
			'BeginGameTurn'			: self.onBeginGameTurn,
			'EndGameTurn'			: self.onEndGameTurn,
			'BeginPlayerTurn'		: self.onBeginPlayerTurn,
			'EndPlayerTurn'			: self.onEndPlayerTurn,
			'endTurnReady'			: self.onEndTurnReady,
			'combatResult' 			: self.onCombatResult,
			'combatLogCalc'	 		: self.onCombatLogCalc,
			'combatLogHit'			: self.onCombatLogHit,
			'improvementBuilt' 		: self.onImprovementBuilt,
			'improvementDestroyed' 	: self.onImprovementDestroyed,
			'routeBuilt' 			: self.onRouteBuilt,
			'firstContact' 			: self.onFirstContact,
			'cityBuilt' 			: self.onCityBuilt,
			'cityRazed'				: self.onCityRazed,
			'cityAcquired' 			: self.onCityAcquired,
			'cityAcquiredAndKept' 	: self.onCityAcquiredAndKept,
			'cityLost'				: self.onCityLost,
			'cultureExpansion' 		: self.onCultureExpansion,
			'cityGrowth' 			: self.onCityGrowth,
			'cityDoTurn' 			: self.onCityDoTurn,
			'cityBuildingUnit'		: self.onCityBuildingUnit,
			'cityBuildingBuilding'	: self.onCityBuildingBuilding,
			'cityRename'			: self.onCityRename,
			'createTradeRoute'		: self.onCreateTradeRoute,
			'editTradeRoute'		: self.onEditTradeRoute,
			'cityHurry'				: self.onCityHurry,
			'selectionGroupPushMission'		: self.onSelectionGroupPushMission,
			'unitMove' 				: self.onUnitMove,
			'unitSetXY' 			: self.onUnitSetXY,
			'unitCreated' 			: self.onUnitCreated,
			'unitBuilt' 			: self.onUnitBuilt,
			'unitKilled'			: self.onUnitKilled,
			'unitLost'				: self.onUnitLost,
			'unitPromoted'			: self.onUnitPromoted,
			'unitSelected'			: self.onUnitSelected,
			'missionaryConvertedUnit' : self.onMissionaryConvertedUnit,
			'UnitRename'			: self.onUnitRename,
			'unitPillage'			: self.onUnitPillage,
			'unitGifted'			: self.onUnitGifted,
			'unitBuildImprovement'	: self.onUnitBuildImprovement,
			'goodyReceived'        	: self.onGoodyReceived,
			'buildingBuilt' 		: self.onBuildingBuilt,
			'chat' 					: self.onChat,
			'victory'				: self.onVictory,
			'yieldSoldToEurope'		: self.onYieldSoldToEurope,
			'yieldBoughtFromEurope'	: self.onYieldBoughtFromEurope,
			'unitBoughtFromEurope'	: self.onUnitBoughtFromEurope,
			'unitTravelStateChanged'	: self.onUnitTravelStateChanged,
			'emmigrantAtDocks'		: self.onEmmigrantAtDocks,
			'populationJoined'		: self.onPopulationJoined,
			'populationUnjoined'	: self.onPopulationUnjoined,
			'unitLearned'			: self.onUnitLearned,
			'yieldProduced'			: self.onYieldProduced,
			'changeWar'				: self.onChangeWar,
			'setPlayerAlive'		: self.onSetPlayerAlive,
			'playerGoldTrade'		: self.onPlayerGoldTrade,
			'windowActivation'		: self.onWindowActivation,
			'cityScreenOpen'		: self.onCityScreenOpen,
			'gameUpdate'			: self.onGameUpdate,		# sample generic event
		}

		################## Events List ###############################
		#
		# Dictionary of Events, indexed by EventID (also used at popup context id)
		#   entries have name, beginFunction, applyFunction [, randomization weight...]
		#
		# Normal events first, random events after
		#
		################## Events List ###############################
		self.Events={
			CvUtil.EventEditCityName : ('EditCityName', self.__eventEditCityNameApply, self.__eventEditCityNameBegin),
			CvUtil.EventEditCity : ('EditCity', self.__eventEditCityApply, self.__eventEditCityBegin),
			CvUtil.EventPlaceObject : ('PlaceObject', self.__eventPlaceObjectApply, self.__eventPlaceObjectBegin),
			CvUtil.EventAwardGold: ('AwardGold', self.__EventAwardGoldApply, self.__EventAwardGoldBegin),
			CvUtil.EventEditUnitName : ('EditUnitName', self.__eventEditUnitNameApply, self.__eventEditUnitNameBegin),
			CvUtil.EventWBAllPlotsPopup : ('WBAllPlotsPopup', self.__eventWBAllPlotsPopupApply, self.__eventWBAllPlotsPopupBegin),
			CvUtil.EventWBLandmarkPopup : ('WBLandmarkPopup', self.__eventWBLandmarkPopupApply, self.__eventWBLandmarkPopupBegin),
			CvUtil.EventWBScriptPopup : ('WBScriptPopup', self.__eventWBScriptPopupApply, self.__eventWBScriptPopupBegin),
			CvUtil.EventWBStartYearPopup : ('WBStartYearPopup', self.__eventWBStartYearPopupApply, self.__eventWBStartYearPopupBegin),
			CvUtil.EventShowWonder: ('ShowWonder', self.__eventShowWonderApply, self.__eventShowWonderBegin),
			CvUtil.EventCreateTradeRoute: ('CreateTradeRoute', self.__eventCreateTradeRouteApply, self.__eventCreateTradeRouteBegin),
			CvUtil.EventEditTradeRoute: ('EditTradeRoute', self.__eventEditTradeRouteApply, self.__eventEditTradeRouteBegin),

# Dale - AoD: AoDCheatMenu START
			CvUtil.EventAoDCheatMenu: ('AoDCheatMenu', self.AoDCheatMenuApply, self.AoDCheatMenuBegin),
# Dale - AoD: AoDCheatMenu END

# EuropeScreen START
			CvUtil.EventDoEuropeScreen: ('DoEuropeScreen', self.doEuropeScreenApply, self.doEuropeScreenBegin),
# EuropeScreen END

		}
#################### EVENT STARTERS ######################
	def handleEvent(self, argsList):
		'EventMgr entry point'
		# extract the last 6 args in the list, the first arg has already been consumed
		self.origArgsList = argsList	# point to original
		tag = argsList[0]				# event type string
		idx = len(argsList)-6
		bDummy = False
		self.bDbg, bDummy, self.bAlt, self.bCtrl, self.bShift, self.bAllowCheats = argsList[idx:]
		ret = 0
		if self.EventHandlerMap.has_key(tag):
			fxn = self.EventHandlerMap[tag]
			ret = fxn(argsList[1:idx])
		return ret

#################### EVENT APPLY ######################
	def beginEvent( self, context, argsList=-1 ):
		'Begin Event'
		entry = self.Events[context]
		return entry[2]( argsList )

	def applyEvent( self, argsList ):
		'Apply the effects of an event '
		context, playerID, netUserData, popupReturn = argsList

		if context == CvUtil.PopupTypeEffectViewer:
			return CvDebugTools.g_CvDebugTools.applyEffectViewer( playerID, netUserData, popupReturn )

		entry = self.Events[context]

		if ( context not in CvUtil.SilentEvents ):
			self.reportEvent(entry, context, (playerID, netUserData, popupReturn) )
		return entry[1]( playerID, netUserData, popupReturn )   # the apply function

	def reportEvent(self, entry, context, argsList):
		'Report an Event to Events.log '
		if (gc.getGame().getActivePlayer() != -1):
			message = "DEBUG Event: %s (%s)" %(entry[0], gc.getActivePlayer().getName())
			CyInterface().addImmediateMessage(message,"")
			CvUtil.pyPrint(message)
		return 0

#################### ON EVENTS ######################
	def onKbdEvent(self, argsList):
		'keypress handler - return 1 if the event was consumed'

		eventType,key,mx,my,px,py = argsList
		game = gc.getGame()

		if (self.bAllowCheats):
			# notify debug tools of input to allow it to override the control
			argsList = (eventType,key,self.bCtrl,self.bShift,self.bAlt,mx,my,px,py,gc.getGame().isNetworkMultiPlayer())
			if ( CvDebugTools.g_CvDebugTools.notifyInput(argsList) ):
				return 0

		if ( eventType == self.EventKeyDown ):
			theKey=int(key)

			#Custom Camera Controls

			if (theKey == int(InputTypes.KB_LEFT)):
				if self.bCtrl:
						CyCamera().SetBaseTurn(CyCamera().GetBaseTurn() - 45.0)
						return 1
				elif self.bShift:
						CyCamera().SetBaseTurn(CyCamera().GetBaseTurn() - 15.0)
						return 1
			
			elif (theKey == int(InputTypes.KB_RIGHT)):
					if self.bCtrl:
							CyCamera().SetBaseTurn(CyCamera().GetBaseTurn() + 45.0)
							return 1
					elif self.bShift:
							CyCamera().SetBaseTurn(CyCamera().GetBaseTurn() + 15.0)
							return 1

			elif (theKey == int(InputTypes.KB_UP)):
					if (self.bCtrl or self.bShift) and CyCamera().GetBasePitch() > -45:
						CyCamera().SetBasePitch(CyCamera().GetBasePitch() - 5.0)
						return 1

			elif (theKey == int(InputTypes.KB_DOWN)):
					if (self.bCtrl or self.bShift) and CyCamera().GetBasePitch() < 20:
						CyCamera().SetBasePitch(CyCamera().GetBasePitch() + 5.0)
						return 1

			elif (theKey == int(InputTypes.KB_HOME) and self.bCtrl):
						CyCamera().SetBaseTurn(0)
						CyCamera().SetBasePitch(0)
						return 1

			#End Custom Camera Controls

			CvCameraControls.g_CameraControls.handleInput( theKey )

# Dale - AoD: AoDCheatMenu START
			if (self.bAllowCheats):		# TAC - Multiplayer - koma13
				if( theKey == int(InputTypes.KB_Z) and self.bShift and self.bCtrl ) :
					self.beginEvent(CvUtil.EventAoDCheatMenu)
# Dale - AoD: AoDCheatMenu END

# Achievements START
			if( theKey == int(InputTypes.KB_F10) and not self.bShift and not self.bCtrl ) :
				CvScreensInterface.showAchieveAdvisorScreen()
# Achievements END

# TAC: EventTriggerMenu START
# Shift+Ctrl+E im Cheatmodus
			if( theKey == int(InputTypes.KB_E) and self.bShift and self.bCtrl and self.bAllowCheats) :
				ePlayer = gc.getGame().getActivePlayer()
				popupInfo = CyPopupInfo()
				popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
				popupInfo.setText(CyTranslator().getText("TXT_KEY_POPUP_SELECT_EVENT",()))
				popupInfo.setData1(ePlayer)
				popupInfo.setOnClickedPythonCallback("selectOneEvent")
				popupInfo.addPythonButton(CyTranslator().getText("TXT_KEY_POPUP_SELECT_NEVER_MIND", ()), "")
				for i in range(gc.getNumEventTriggerInfos()):
					trigger = gc.getEventTriggerInfo(i)
					name = trigger.getType().replace("EVENTTRIGGER_", "").replace("_", " ").title()
					popupInfo.addPythonButton(name, "")
					# popupInfo.addPythonButton(str(trigger.getType()), "")
				popupInfo.addPythonButton(CyTranslator().getText("TXT_KEY_POPUP_SELECT_NEVER_MIND", ()), "")
				
				popupInfo.addPopup(ePlayer)
# TAC: EventTriggerMenu END

			if( theKey == int(InputTypes.KB_N) and self.bShift and self.bCtrl ) :
				gc.openNetworkOOSMenu();

			if (self.bAllowCheats):
				# Shift - T (Debug - No MP)
				if (theKey == int(InputTypes.KB_T)):
					if ( self.bShift ):
						self.beginEvent(CvUtil.EventAwardGold)
						#self.beginEvent(CvUtil.EventCameraControlPopup)
						return 1

# TAC: Wonder Movie Cheat disabled
				#elif (theKey == int(InputTypes.KB_W)):
				#	if ( self.bShift and self.bCtrl):
				#		self.beginEvent(CvUtil.EventShowWonder)
				#		return 1

				# Shift - ] (Debug - currently mouse-overd unit, health += 10
				elif (theKey == int(InputTypes.KB_LBRACKET) and self.bShift ):
					unit = CyMap().plot(px, py).getUnit(0)
					if ( not unit.isNone() ):
						d = min( unit.maxHitPoints()-1, unit.getDamage() + 10 )
						unit.setDamage( d )

				# Shift - [ (Debug - currently mouse-overd unit, health -= 10
				elif (theKey == int(InputTypes.KB_RBRACKET) and self.bShift ):
					unit = CyMap().plot(px, py).getUnit(0)
					if ( not unit.isNone() ):
						d = max( 0, unit.getDamage() - 10 )
						unit.setDamage( d )

				elif (theKey == int(InputTypes.KB_F1)):
					if ( self.bShift ):
						CvScreensInterface.replayScreen.showScreen(False)
						return 1
						
				elif (theKey == int(InputTypes.KB_F2)):
					if ( self.bShift ):
						city = CyMap().plot(px, py).getPlotCity()
						if (city != None):
							CyInterface().selectCity(city, True)
							return 1
					# don't return 1 unless you want the input consumed


		return 0

	def onModNetMessage(self, argsList):
		'Called whenever CyMessageControl().sendModNetMessage() is called - this is all for you modders!'

		iData1, iData2, iData3, iData4, iData5 = argsList

		print("Modder's net message!")

		CvUtil.pyPrint( 'onModNetMessage' )

	def onInit(self, argsList):
		'Called when Civ starts up'
		CvUtil.pyPrint( 'OnInit' )

	def onUpdate(self, argsList):
		'Called every frame'
		fDeltaTime = argsList[0]

		# allow camera to be updated
		CvCameraControls.g_CameraControls.onUpdate( fDeltaTime )

	def onWindowActivation(self, argsList):
		'Called when the game window activates or deactivates'
		bActive = argsList[0]

	def onCityScreenOpen(self, argsList):
		'Called when the game window activates or deactivates'
		iPlayer = argsList[0]
		iCityId = argsList[1]
		CvAdvisorUtils.cityScreenFeats(iPlayer, iCityId)

	def onUnInit(self, argsList):
		'Called when Civ shuts down'
		CvUtil.pyPrint('OnUnInit')

	def onPreSave(self, argsList):
		"called before a game is actually saved"
		CvUtil.pyPrint('OnPreSave')

	def onSaveGame(self, argsList):
		"return the string to be saved - Must be a string"
		return ""

	def onLoadGame(self, argsList):
		return 0

	def onGameStart(self, argsList):
		'Called at the start of the game'
		if (gc.getGame().getGameTurnYear() == GlobalDefines.START_YEAR and not gc.getGame().isOption(GameOptionTypes.GAMEOPTION_ADVANCED_START)):
			for iPlayer in range(gc.getMAX_PLAYERS()):
				player = gc.getPlayer(iPlayer)
				if (player.isAlive() and player.isHuman()):
					popupInfo = CyPopupInfo()
					popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
					popupInfo.setText(u"showDawnOfMan")
					popupInfo.addPopup(iPlayer)
		else:
			CyInterface().setSoundSelectionReady(True)

		if gc.getGame().isPbem():
			for iPlayer in range(gc.getMAX_PLAYERS()):
				player = gc.getPlayer(iPlayer)
				if (player.isAlive() and player.isHuman()):
					popupInfo = CyPopupInfo()
					popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_DETAILS)
					popupInfo.setOption1(True)
					popupInfo.addPopup(iPlayer)
		
		
		CyMap().calculateCanalAndChokePoints() # Super Forts

	def onGameEnd(self, argsList):
		'Called at the End of the game'
		print("Game is ending")
		return

	def onBeginGameTurn(self, argsList):
		'Called at the beginning of the end of each turn'
		iGameTurn = argsList[0]

	def onEndGameTurn(self, argsList):
		'Called at the end of the end of each turn'
		iGameTurn = argsList[0]

	def onBeginPlayerTurn(self, argsList):
		'Called at the beginning of a players turn'
		iGameTurn, iPlayer = argsList

	def onEndPlayerTurn(self, argsList):
		'Called at the end of a players turn'
		iGameTurn, iPlayer = argsList

		CvAdvisorUtils.endTurnNags(iPlayer)
		CvAdvisorUtils.endTurnFeats(iPlayer)

	def onEndTurnReady(self, argsList):
		iGameTurn = argsList[0]

	def onFirstContact(self, argsList):
		'Contact'
		iTeamX,iHasMetTeamY = argsList
		if (not self.__LOG_CONTACT):
			return
		CvUtil.pyPrint('Team %d has met Team %d' %(iTeamX, iHasMetTeamY))

	def onCombatResult(self, argsList):
		'Combat Result'
		pWinner,pLoser = argsList
		playerX = gc.getPlayer(pWinner.getOwner())
		unitX = gc.getUnitInfo(pWinner.getUnitType())
		playerY = gc.getPlayer(pLoser.getOwner())
		unitY = gc.getUnitInfo(pLoser.getUnitType())
		if (not self.__LOG_COMBAT):
			return
		if playerX and playerX and unitX and playerY:
			CvUtil.pyPrint('Player %d Civilization %s Unit %s has defeated Player %d Civilization %s Unit %s'
				%(playerX.getID(), playerX.getCivilizationDescription(0), unitX.getDescription(),
				playerY.getID(), playerY.getCivilizationDescription(0), unitY.getDescription()))

	def onCombatLogCalc(self, argsList):
		'Combat Result'
		genericArgs = argsList[0][0]
		cdAttacker = genericArgs[0]
		cdDefender = genericArgs[1]
		iCombatOdds = genericArgs[2]
		CvUtil.combatMessageBuilder(cdAttacker, cdDefender, iCombatOdds)

	def onCombatLogHit(self, argsList):
		'Combat Message'
		global gCombatMessages, gCombatLog
		genericArgs = argsList[0][0]
		cdAttacker = genericArgs[0]
		cdDefender = genericArgs[1]
		iIsAttacker = genericArgs[2]
		iDamage = genericArgs[3]

		if cdDefender.eOwner == cdDefender.eVisualOwner:
			szDefenderName = gc.getPlayer(cdDefender.eOwner).getNameKey()
		else:
			szDefenderName = localText.getText("TXT_KEY_TRAIT_PLAYER_UNKNOWN", ())
		if cdAttacker.eOwner == cdAttacker.eVisualOwner:
			szAttackerName = gc.getPlayer(cdAttacker.eOwner).getNameKey()
		else:
			szAttackerName = localText.getText("TXT_KEY_TRAIT_PLAYER_UNKNOWN", ())

		if (iIsAttacker == 0):
			combatMessage = localText.getText("TXT_KEY_COMBAT_MESSAGE_HIT", (szDefenderName, cdDefender.sUnitName, iDamage, cdDefender.iCurrHitPoints, cdDefender.iMaxHitPoints))
			CyInterface().addCombatMessage(cdAttacker.eOwner,combatMessage)
			CyInterface().addCombatMessage(cdDefender.eOwner,combatMessage)
			if (cdDefender.iCurrHitPoints <= 0):
				combatMessage = localText.getText("TXT_KEY_COMBAT_MESSAGE_DEFEATED", (szAttackerName, cdAttacker.sUnitName, szDefenderName, cdDefender.sUnitName))
				CyInterface().addCombatMessage(cdAttacker.eOwner,combatMessage)
				CyInterface().addCombatMessage(cdDefender.eOwner,combatMessage)
		elif (iIsAttacker == 1):
			combatMessage = localText.getText("TXT_KEY_COMBAT_MESSAGE_HIT", (szAttackerName, cdAttacker.sUnitName, iDamage, cdAttacker.iCurrHitPoints, cdAttacker.iMaxHitPoints))
			CyInterface().addCombatMessage(cdAttacker.eOwner,combatMessage)
			CyInterface().addCombatMessage(cdDefender.eOwner,combatMessage)
			if (cdAttacker.iCurrHitPoints <= 0):
				combatMessage = localText.getText("TXT_KEY_COMBAT_MESSAGE_DEFEATED", (szDefenderName, cdDefender.sUnitName, szAttackerName, cdAttacker.sUnitName))
				CyInterface().addCombatMessage(cdAttacker.eOwner,combatMessage)
				CyInterface().addCombatMessage(cdDefender.eOwner,combatMessage)

	def onImprovementBuilt(self, argsList):
		'Improvement Built'
		iImprovement, iX, iY = argsList
		if (not self.__LOG_IMPROVEMENT):
			return
		CvUtil.pyPrint('Improvement %s was built at %d, %d'
			%(gc.getImprovementInfo(iImprovement).getDescription(), iX, iY))


	def onImprovementDestroyed(self, argsList):
		'Improvement Destroyed'
		iImprovement, iOwner, iX, iY = argsList
		if (not self.__LOG_IMPROVEMENT):
			return
		CvUtil.pyPrint('Improvement %s was Destroyed at %d, %d'
			%(gc.getImprovementInfo(iImprovement).getDescription(), iX, iY))


	def onRouteBuilt(self, argsList):
		'Route Built'
		iRoute, iX, iY = argsList
		if (not self.__LOG_IMPROVEMENT):
			return
		CvUtil.pyPrint('Route %s was built at %d, %d'
			%(gc.getRouteInfo(iRoute).getDescription(), iX, iY))

	def onPlotRevealed(self, argsList):
		'Plot Revealed'
		pPlot = argsList[0]
		iTeam = argsList[1]

	def onPlotFeatureRemoved(self, argsList):
		'Plot Revealed'
		pPlot = argsList[0]
		iFeatureType = argsList[1]
		pCity = argsList[2] # This can be null

	def onPlotPicked(self, argsList):
		'Plot Picked'
		pPlot = argsList[0]
		CvUtil.pyPrint('Plot was picked at %d, %d'
			%(pPlot.getX(), pPlot.getY()))

	def onGotoPlotSet(self, argsList):
		'Goto Plot'
		pPlot, iPlayer = argsList

	def onBuildingBuilt(self, argsList):
		'Building Completed'
		pCity, iBuildingType = argsList

		CvAdvisorUtils.buildingBuiltFeats(pCity, iBuildingType)

		if (not self.__LOG_BUILDING):
			return
		CvUtil.pyPrint('%s was finished by Player %d Civilization %s'
			%(gc.getBuildingInfo(iBuildingType).getDescription(), pCity.getOwner(), gc.getPlayer(pCity.getOwner()).getCivilizationDescription(0)))


	def onSelectionGroupPushMission(self, argsList):
		'selection group mission'
		eOwner = argsList[0]
		eMission = argsList[1]
		iNumUnits = argsList[2]
		listUnitIds = argsList[3]

		if (not self.__LOG_PUSH_MISSION):
			return
		if pHeadUnit:
			CvUtil.pyPrint("Selection Group pushed mission %d" %(eMission))

	def onUnitMove(self, argsList):
		'unit move'
		pPlot,pUnit,pOldPlot = argsList
		player = gc.getPlayer(pUnit.getOwner())
		unitInfo = gc.getUnitInfo(pUnit.getUnitType())
		CvAdvisorUtils.unitMoveFeats(pUnit, pPlot, pOldPlot)
		if (not self.__LOG_MOVEMENT):
			return
		if player and unitInfo:
			CvUtil.pyPrint('Player %d Civilization %s unit %s is moving to %d, %d'
				%(player.getID(), player.getCivilizationDescription(0), unitInfo.getDescription(),
				pUnit.getX(), pUnit.getY()))

	def onUnitSetXY(self, argsList):
		'units xy coords set manually'
		pPlot,pUnit = argsList
		if (not self.__LOG_MOVEMENT):
			return
			
	def onUnitCreated(self, argsList):
		'Unit Completed'
		unit = argsList[0]
		if (not self.__LOG_UNITBUILD):
			return

	def onUnitBuilt(self, argsList):
		'Unit Completed'
		city = argsList[0]
		unit = argsList[1]
		player = gc.getPlayer(city.getOwner())

		CvAdvisorUtils.unitBuiltFeats(city, unit)

		if (not self.__LOG_UNITBUILD):
			return
		CvUtil.pyPrint('%s was finished by Player %d Civilization %s'
			%(gc.getUnitInfo(unit.getUnitType()).getDescription(), player.getID(), player.getCivilizationDescription(0)))

	def onUnitKilled(self, argsList):
		'Unit Killed'
		unit, iAttacker = argsList
		player = gc.getPlayer(unit.getOwner())
		attacker = gc.getPlayer(iAttacker)
		if (not self.__LOG_UNITKILLED):
			return
		CvUtil.pyPrint('Player %d Civilization %s Unit %s was killed by Player %d'
			%(player.getID(), player.getCivilizationDescription(0), gc.getUnitInfo(unit.getUnitType()).getDescription(), attacker.getID()))

	def onUnitLost(self, argsList):
		'Unit Lost'
		unit = argsList[0]
		player = gc.getPlayer(unit.getOwner())
		if (not self.__LOG_UNITLOST):
			return
		CvUtil.pyPrint('%s was lost by Player %d Civilization %s'
			%(gc.getUnitInfo(unit.getUnitType()).getDescription(), player.getID(), player.getCivilizationDescription(0)))

	def onUnitPromoted(self, argsList):
		'Unit Promoted'
		pUnit, iPromotion = argsList
		player = gc.getPlayer(pUnit.getOwner())
		if (not self.__LOG_UNITPROMOTED):
			return
		CvUtil.pyPrint('Unit Promotion Event: %s - %s' %(player.getCivilizationDescription(0), pUnit.getName(),))

	def onUnitRename(self, argsList):
		'Unit is renamed'
		pUnit = argsList[0]
		if (pUnit.getOwner() == gc.getGame().getActivePlayer()):
			self.__eventEditUnitNameBegin(pUnit)

	def onUnitPillage(self, argsList):
		'Unit pillages a plot'
		pUnit, iImprovement, iRoute, iOwner = argsList
		iPlotX = pUnit.getX()
		iPlotY = pUnit.getY()
		pPlot = CyMap().plot(iPlotX, iPlotY)

		if (not self.__LOG_UNITPILLAGE):
			return
		CvUtil.pyPrint("Player %d's %s pillaged improvement %d and route %d at plot at (%d, %d)"
			%(iOwner, gc.getUnitInfo(pUnit.getUnitType()).getDescription(), iImprovement, iRoute, iPlotX, iPlotY))

	def onUnitGifted(self, argsList):
		'Unit is gifted from one player to another'
		pUnit, iGiftingPlayer, pPlotLocation = argsList

	def onUnitBuildImprovement(self, argsList):
		'Unit begins enacting a Build (building an Improvement or Route)'
		pUnit, iBuild, bFinished = argsList

	def onUnitSelected(self, argsList):
		pUnit = argsList[0]
		CvAdvisorUtils.unitSelectedFeats(pUnit)
		
	def onMissionaryConvertedUnit(self, argsList):
		pUnit = argsList[0]
		CvAdvisorUtils.addUnitToNagList(pUnit)

	def onGoodyReceived(self, argsList):
		'Goody received'
		iPlayer, pPlot, pUnit, iGoodyType = argsList
		if (not self.__LOG_GOODYRECEIVED):
			return
		CvUtil.pyPrint('%s received a goody' %(gc.getPlayer(iPlayer).getCivilizationDescription(0)),)

	def onChangeWar(self, argsList):
		'War Status Changes'
		bIsWar = argsList[0]
		iTeam = argsList[1]
		iRivalTeam = argsList[2]
		if not (bIsWar):
			# TAC Baby Boom Event Start
			pPlayer = gc.getPlayer(gc.getTeam(iTeam).getLeaderID())
			pRivalPlayer = gc.getPlayer(gc.getTeam(iRivalTeam).getLeaderID())
			if gc.getNumEventTriggerInfos() > 0: # prevents mods that don't have events from getting an error
				iEvent = CvUtil.findInfoTypeNum('EVENTTRIGGER_BABY_BOOM')
				if iEvent != -1 and gc.getGame().isEventActive(iEvent):
					pPlayer.trigger(iEvent)
					pRivalPlayer.trigger(iEvent)
			# TAC Baby Boom Event Ende
		if (not self.__LOG_WARPEACE):
			return
		if (bIsWar):
			strStatus = "declared war"
		else:
			strStatus = "declared peace"
		CvUtil.pyPrint('Team %d has %s on Team %d'
			%(iTeam, strStatus, iRivalTeam))

	def onChat(self, argsList):
		'Chat Message Event'
		chatMessage = "%s" %(argsList[0],)

	def onSetPlayerAlive(self, argsList):
		'Set Player Alive Event'
		iPlayerID = argsList[0]
		bNewValue = argsList[1]
		CvUtil.pyPrint("Player %d's alive status set to: %d" %(iPlayerID, int(bNewValue)))

	def onPlayerGoldTrade(self, argsList):
		'Player Trades gold to another player'
		iFromPlayer, iToPlayer, iGoldAmount = argsList

	def onCityBuilt(self, argsList):
		'City Built'
		city = argsList[0]

# Dale - AoD: AI Autoplay START
		if (city.getOwner() == gc.getGame().getActivePlayer() and gc.getGame().getAIAutoPlay() == 0 and gc.getPlayer(city.getOwner()).isHuman()):
#		if (city.getOwner() == gc.getGame().getActivePlayer()):
# Dale - AoD: AI Autoplay END
			self.__eventEditCityNameBegin(city, False)
# TAC - koma13 - START
		if (not self.__LOG_CITYBUILT):
			return
# TAC - koma13 - END
		CvUtil.pyPrint('City Built Event: %s' %(city.getName()))

	def onCityRazed(self, argsList):
		'City Razed'
		city, iPlayer = argsList
		iOwner = city.findHighestCulture()

		CvUtil.pyPrint("City Razed Event: %s" %(city.getName(),))

	def onCityAcquired(self, argsList):
		'City Acquired'
		iPreviousOwner,iNewOwner,pCity,bConquest,bTrade = argsList
		CvUtil.pyPrint('City Acquired Event: %s' %(pCity.getName()))

	def onCityAcquiredAndKept(self, argsList):
		'City Acquired and Kept'
		iOwner,pCity = argsList
		CvUtil.pyPrint('City Acquired and Kept Event: %s' %(pCity.getName()))

	def onCityLost(self, argsList):
		'City Lost'
		city = argsList[0]
		player = gc.getPlayer(city.getOwner())
		if (not self.__LOG_CITYLOST):
			return
		CvUtil.pyPrint('City %s was lost by Player %d Civilization %s'
			%(city.getName(), player.getID(), player.getCivilizationDescription(0)))

	def onCultureExpansion(self, argsList):
		'City Culture Expansion'
		pCity = argsList[0]
		iPlayer = argsList[1]
		CvUtil.pyPrint("City %s's culture has expanded" %(pCity.getName(),))

	def onCityGrowth(self, argsList):
		'City Population Growth'
		pCity = argsList[0]
		iPlayer = argsList[1]
		CvUtil.pyPrint("%s has grown" %(pCity.getName(),))

	def onCityDoTurn(self, argsList):
		'City Production'
		pCity = argsList[0]
		iPlayer = argsList[1]

		CvAdvisorUtils.cityAdvise(pCity, iPlayer)

	def onCityBuildingUnit(self, argsList):
		'City begins building a unit'
		pCity = argsList[0]
		iUnitType = argsList[1]
		if (not self.__LOG_CITYBUILDING):
			return
		CvUtil.pyPrint("%s has begun building a %s" %(pCity.getName(),gc.getUnitInfo(iUnitType).getDescription()))

	def onCityBuildingBuilding(self, argsList):
		'City begins building a Building'
		pCity = argsList[0]
		iBuildingType = argsList[1]
		if (not self.__LOG_CITYBUILDING):
			return
		CvUtil.pyPrint("%s has begun building a %s" %(pCity.getName(),gc.getBuildingInfo(iBuildingType).getDescription()))

	def onCityRename(self, argsList):
		'City is renamed'
		pCity = argsList[0]
		if (pCity.getOwner() == gc.getGame().getActivePlayer()):
			self.__eventEditCityNameBegin(pCity, True)

	def onCreateTradeRoute(self, argsList):
		'Trade Route is Created'
		PlayerID = argsList[0]
		self.__eventCreateTradeRouteBegin(PlayerID)

	def onEditTradeRoute(self, argsList):
		'Trade Route is Modified'
		PlayerID = argsList[0]
		iRouteID = argsList[1]
		self.__eventEditTradeRouteBegin(PlayerID, iRouteID)

	def onCityHurry(self, argsList):
		'City is renamed'
		pCity = argsList[0]
		iHurryType = argsList[1]

	def onVictory(self, argsList):
		'Victory'
		iTeam, iVictory = argsList
		if (iVictory >= 0 and iVictory < gc.getNumVictoryInfos()):
			victoryInfo = gc.getVictoryInfo(int(iVictory))
			CvUtil.pyPrint("Victory!  Team %d achieves a %s victory"
				%(iTeam, victoryInfo.getDescription()))

	def onYieldSoldToEurope(self, argsList):
		'Yield Sold To Europe'
		iPlayer, iYield, iAmount = argsList

	def onYieldBoughtFromEurope(self, argsList):
		'Yield Bought From Europe'
		iPlayer, iYield, iAmount = argsList

	def onUnitBoughtFromEurope(self, argsList):
		'Unit Bought From Europe'
		iPlayer, iUnitId = argsList

	def onUnitTravelStateChanged(self, argsList):
		'Ship Arrived in Europe or America'
		iPlayer, iUnitTravelState, iUnitId = argsList

	def onEmmigrantAtDocks(self, argsList):
		'Emmigrant At Docks'
		iPlayer, iUnitId = argsList

	def onPopulationJoined(self, argsList):
		'Population Joined'
		iPlayer, iCityId, iUnitId = argsList

	def onPopulationUnjoined(self, argsList):
		'Population Unjoined'
		iPlayer, iCityId, iUnitId = argsList

	def onUnitLearned(self, argsList):
		'Unit Learned'
		iPlayer, iUnitId = argsList

	def onYieldProduced(self, argsList):
		'Yield Produced'
		iPlayer, iCityId, iYield = argsList

	def onGameUpdate(self, argsList):
		'sample generic event, called on each game turn slice'
		genericArgs = argsList[0][0]	# tuple of tuple of my args
		turnSlice = genericArgs[0]

	def onMouseEvent(self, argsList):
		'mouse handler - returns 1 if the event was consumed'
		eventType,mx,my,px,py,interfaceConsumed,screens = argsList
		if ( px!=-1 and py!=-1 ):
			if ( eventType == self.EventLButtonDown ):
				if (self.bAllowCheats and self.bCtrl and self.bAlt and CyMap().plot(px,py).isCity() and not interfaceConsumed):
					# Launch Edit City Event
					self.beginEvent( CvUtil.EventEditCity, (px,py) )
					return 1

				elif (self.bAllowCheats and self.bCtrl and self.bShift and not interfaceConsumed):
					# Launch Place Object Event
					self.beginEvent( CvUtil.EventPlaceObject, (px, py) )
					return 1

		if ( eventType == self.EventBack ):
			return CvScreensInterface.handleBack(screens)
		elif ( eventType == self.EventForward ):
			return CvScreensInterface.handleForward(screens)

		return 0


#################### TRIGGERED EVENTS ##################

	def __eventEditCityNameBegin(self, city, bRename):
		popup = CyPopup(CvUtil.EventEditCityName, EventContextTypes.EVENTCONTEXT_ALL, True)
		popup.setUserData((city.getID(), bRename))
		popup.setHeaderString(localText.getText("TXT_KEY_NAME_CITY", ()), CvUtil.FONT_CENTER_JUSTIFY)
		popup.setBodyString(localText.getText("TXT_KEY_SETTLE_NEW_CITY_NAME", ()), CvUtil.FONT_CENTER_JUSTIFY)
		popup.createEditBox(city.getName(), 0)
		popup.setEditBoxMaxCharCount( 15, 32, 0 )
		popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)

	def __eventEditCityNameApply(self, playerID, userData, popupReturn):

		'Edit City Name Event'
		iCityID = userData[0]
		bRename = userData[1]
		player = gc.getPlayer(playerID)
		city = player.getCity(iCityID)
		cityName = popupReturn.getEditBoxString(0)
		if (len(cityName) > 30):
			cityName = cityName[:30]
		city.setName(cityName, not bRename)

	def __eventCreateTradeRouteBegin(self, PlayerID):
		popup = CyPopup(CvUtil.EventCreateTradeRoute, EventContextTypes.EVENTCONTEXT_ALL, 1)
		popup.setHeaderString(localText.getText("TXT_KEY_CREATE_TRADE_ROUTE", ()), CvUtil.FONT_LEFT_JUSTIFY)

		popup.setBodyString(localText.getText("TXT_KEY_SOURCE", ()), CvUtil.FONT_LEFT_JUSTIFY)
		popup.createPullDown(0)
		popup.addPullDownString(localText.getText("TXT_KEY_NO_SOURCE", ()), -1, 0)
		player = gc.getPlayer(PlayerID)
		for iPlayer in range(gc.getMAX_PLAYERS()):
			loopPlayer = gc.getPlayer(iPlayer)
			if (loopPlayer.isAlive() and player.canLoadYield(iPlayer)):
				(pCity, iter) = loopPlayer.firstCity(False)
				while (pCity):
					iId = gc.getMAX_PLAYERS() * pCity.getID() + pCity.getOwner()
					popup.addPullDownString(pCity.getName(), iId, 0)
					(pCity, iter) = loopPlayer.nextCity(iter, False)

		popup.setBodyString(localText.getText("TXT_KEY_DESTINATION", ()), CvUtil.FONT_LEFT_JUSTIFY)
		popup.createPullDown(1)
		popup.addPullDownString(localText.getText("TXT_KEY_NO_DESTINATION", ()), -1, 1)
		for iPlayer in range(gc.getMAX_PLAYERS()):
			loopPlayer = gc.getPlayer(iPlayer)
			if (loopPlayer.isAlive() and player.canUnloadYield(iPlayer)):
				(pCity, iter) = loopPlayer.firstCity(False)
				while (pCity):
					iId = gc.getMAX_PLAYERS() * pCity.getID() + pCity.getOwner()
					popup.addPullDownString(pCity.getName(), iId, 1)
					(pCity, iter) = loopPlayer.nextCity(iter, False)

		popup.setBodyString(localText.getText("TXT_KEY_YIELD", ()), CvUtil.FONT_LEFT_JUSTIFY)
		popup.createPullDown(2)
		popup.addPullDownString(localText.getText("TXT_KEY_NO_YIELD", ()), -1, 2)
		for i in range( YieldTypes.NUM_YIELD_TYPES ):
			if (gc.getYieldInfo(i).isCargo()):
				popup.addPullDownString(gc.getYieldInfo(i).getDescription(), i, 2)

		popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)

	def __eventCreateTradeRouteApply(self, playerID, userData, popupReturn):
		'Create Trade Route Event'
		if (popupReturn.getSelectedPullDownValue(0) != -1 and popupReturn.getSelectedPullDownValue(1) != -1 and popupReturn.getSelectedPullDownValue(2) != -1):
			iSourceCityID = popupReturn.getSelectedPullDownValue( 0 ) / gc.getMAX_PLAYERS()
			iSourceCityPlayer = popupReturn.getSelectedPullDownValue( 0 ) % gc.getMAX_PLAYERS()
			iDestinationCityID = popupReturn.getSelectedPullDownValue( 1 ) / gc.getMAX_PLAYERS()
			iDestinationCityPlayer = popupReturn.getSelectedPullDownValue( 1 ) % gc.getMAX_PLAYERS()
			iYieldType = popupReturn.getSelectedPullDownValue( 2 )

			player = gc.getPlayer(playerID)
			gc.getPlayer(playerID).addTradeRoute(iSourceCityPlayer, iSourceCityID, iDestinationCityPlayer, iDestinationCityID, iYieldType)

	def __eventEditTradeRouteBegin(self, playerID, iRouteID):
		popup = CyPopup(CvUtil.EventEditTradeRoute, EventContextTypes.EVENTCONTEXT_ALL, 1)
		popup.setHeaderString(localText.getText("TXT_KEY_EDIT_TRADE_ROUTE", ()), CvUtil.FONT_LEFT_JUSTIFY)

		player = gc.getPlayer(playerID)
		pRoute = player.getTradeRoute(iRouteID)
		popup.setUserData((iRouteID,))

		popup.setBodyString(localText.getText("TXT_KEY_SOURCE", ()), CvUtil.FONT_LEFT_JUSTIFY)
		popup.createPullDown(0)
		for iPlayer in range(gc.getMAX_PLAYERS()):
			loopPlayer = gc.getPlayer(iPlayer)
			if (loopPlayer.isAlive() and player.canLoadYield(iPlayer)):
				(pCity, iter) = loopPlayer.firstCity(False)
				while (pCity):
					iId = gc.getMAX_PLAYERS() * pCity.getID() + pCity.getOwner()
					popup.addPullDownString(pCity.getName(), iId, 0)
					if (pRoute.getSourceCity().iID == pCity.getID() and pRoute.getSourceCity().eOwner == pCity.getOwner()):
						popup.setSelectedPulldownID(iId, 0);
					(pCity, iter) = loopPlayer.nextCity(iter, False)

		popup.setBodyString(localText.getText("TXT_KEY_DESTINATION", ()), CvUtil.FONT_LEFT_JUSTIFY)
		popup.createPullDown(1)
		for iPlayer in range(gc.getMAX_PLAYERS()):
			loopPlayer = gc.getPlayer(iPlayer)
			if (loopPlayer.isAlive() and player.canUnloadYield(iPlayer)):
				(pCity, iter) = loopPlayer.firstCity(False)
				while (pCity):
					iId = gc.getMAX_PLAYERS() * pCity.getID() + pCity.getOwner()
					popup.addPullDownString(pCity.getName(), iId, 1)
					if (pRoute.getDestinationCity().iID == pCity.getID() and pRoute.getDestinationCity().eOwner == pCity.getOwner()):
						popup.setSelectedPulldownID(iId, 1);
					(pCity, iter) = loopPlayer.nextCity(iter, False)

				if player.canTradeWithEurope():
					popup.addPullDownString(localText.getText("TXT_KEY_CONCEPT_EUROPE", ()), -1, 1)

				if (pRoute.getDestinationCity().iID == -1 and pRoute.getDestinationCity().eOwner == playerID):
					popup.setSelectedPulldownID(-1, 1);


		popup.setBodyString(localText.getText("TXT_KEY_YIELD", ()), CvUtil.FONT_LEFT_JUSTIFY)
		popup.createPullDown(2)
		for i in range( YieldTypes.NUM_YIELD_TYPES ):
			if (gc.getYieldInfo(i).isCargo()):
				popup.addPullDownString(gc.getYieldInfo(i).getDescription(), i, 2)
		popup.setSelectedPulldownID(pRoute.getYield(), 2);

		popup.createCheckBoxes( 1, 3 )
		popup.setCheckBoxText( 0, localText.getText("TXT_KEY_DELETE_TRADE_ROUTE", ()), 3 )

		popup.createCheckBoxes( 1, 4 )
		popup.setCheckBoxText( 0, localText.getText("TXT_KEY_CREATE_TRADE_ROUTE", ()), 4 )

		popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)

	def __eventEditTradeRouteApply(self, PlayerID, userData, popupReturn):
		'Edit Trade Route Event'
		iSourceCityID = popupReturn.getSelectedPullDownValue( 0 ) / gc.getMAX_PLAYERS()
		iSourceCityPlayer = popupReturn.getSelectedPullDownValue( 0 ) % gc.getMAX_PLAYERS()
		iDestinationCityID = popupReturn.getSelectedPullDownValue( 1 ) / gc.getMAX_PLAYERS()
		iDestinationCityPlayer = popupReturn.getSelectedPullDownValue( 1 ) % gc.getMAX_PLAYERS()
		iYieldType = popupReturn.getSelectedPullDownValue( 2 )

		iRouteID = userData[0]
		player = gc.getPlayer(PlayerID)

		if (popupReturn.getCheckboxBitfield(3)):
			player.removeTradeRoute(iRouteID)
		elif (popupReturn.getCheckboxBitfield(4)):
			player.addTradeRoute(iSourceCityPlayer, iSourceCityID, iDestinationCityPlayer, iDestinationCityID, iYieldType)
		else:
			player.editTradeRoute(iRouteID, iSourceCityPlayer, iSourceCityID, iDestinationCityPlayer, iDestinationCityID, iYieldType)

	def __eventEditCityBegin(self, argsList):
		'Edit City Event'
		px,py = argsList
		CvWBPopups.CvWBPopups().initEditCity(argsList)

	def __eventEditCityApply(self, playerID, userData, popupReturn):
		'Edit City Event Apply'
		if (getChtLvl() > 0):
			CvWBPopups.CvWBPopups().applyEditCity( (popupReturn, userData) )

	def __eventPlaceObjectBegin(self, argsList):
		'Place Object Event'
		CvDebugTools.CvDebugTools().initUnitPicker(argsList)

	def __eventPlaceObjectApply(self, playerID, userData, popupReturn):
		'Place Object Event Apply'
		if (getChtLvl() > 0):
			CvDebugTools.CvDebugTools().applyUnitPicker( (popupReturn, userData) )
	def __EventAwardGoldBegin(self, argsList):
		'Award Gold Event'
		CvDebugTools.CvDebugTools().cheatGold()
	def __EventAwardGoldApply(self, playerID, netUserData, popupReturn):
		'Award Gold Event Apply'

		if (getChtLvl() > 0):
			CvDebugTools.CvDebugTools().applyGoldCheat( (popupReturn) )

	def __eventShowWonderBegin(self, argsList):
		'Show Wonder Event'
		CvDebugTools.CvDebugTools().wonderMovie()

	def __eventShowWonderApply(self, playerID, netUserData, popupReturn):
		'Wonder Movie Apply'
		if (getChtLvl() > 0):
			CvDebugTools.CvDebugTools().applyWonderMovie( (popupReturn) )

	def __eventEditUnitNameBegin(self, argsList):
		pUnit = argsList
		popup = CyPopup(CvUtil.EventEditUnitName, EventContextTypes.EVENTCONTEXT_ALL, True)
		popup.setUserData((pUnit.getID(),))
		popup.setBodyString(localText.getText("TXT_KEY_RENAME_UNIT", ()), CvUtil.FONT_CENTER_JUSTIFY)
		popup.createEditBox(pUnit.getNameNoDesc(), 0)
		popup.setEditBoxMaxCharCount(20, 20, 0)
		popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)

	def __eventEditUnitNameApply(self, playerID, userData, popupReturn):

		'Edit Unit Name Event'
		iUnitID = userData[0]
		unit = gc.getPlayer(playerID).getUnit(iUnitID)
		newName = popupReturn.getEditBoxString(0)
		if (len(newName) > 25):
			newName = newName[:25]
		unit.setName(newName)

	def __eventWBAllPlotsPopupBegin(self, argsList):
		CvScreensInterface.getWorldBuilderScreen().allPlotsCB()
		return
	def __eventWBAllPlotsPopupApply(self, playerID, userData, popupReturn):
		if (popupReturn.getButtonClicked() >= 0):
			CvScreensInterface.getWorldBuilderScreen().handleAllPlotsCB(popupReturn)
		return

	def __eventWBLandmarkPopupBegin(self, argsList):
		CvScreensInterface.getWorldBuilderScreen().setLandmarkCB("")
		return

	def __eventWBLandmarkPopupApply(self, playerID, userData, popupReturn):
		if (popupReturn.getEditBoxString(0)):
			szLandmark = popupReturn.getEditBoxString(0)
			if (len(szLandmark)):
				CvScreensInterface.getWorldBuilderScreen().setLandmarkCB(szLandmark)
		return

	def __eventWBScriptPopupBegin(self, argsList):
		popup = CyPopup(CvUtil.EventWBScriptPopup, EventContextTypes.EVENTCONTEXT_ALL, True)
		popup.setHeaderString(localText.getText("TXT_KEY_WB_SCRIPT", ()), CvUtil.FONT_CENTER_JUSTIFY)
		popup.createEditBox(CvScreensInterface.getWorldBuilderScreen().getCurrentScript(), 0)
		popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)

	def __eventWBScriptPopupApply(self, playerID, userData, popupReturn):
		if (popupReturn.getEditBoxString(0)):
			szScriptName = popupReturn.getEditBoxString(0)
			CvScreensInterface.getWorldBuilderScreen().setScriptCB(szScriptName)

	def __eventWBStartYearPopupBegin(self, argsList):
		popup = CyPopup(CvUtil.EventWBStartYearPopup, EventContextTypes.EVENTCONTEXT_ALL, True)
		popup.createSpinBox(0, "", gc.getGame().getStartYear(), 1, 5000, -5000)
		popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)

	def __eventWBStartYearPopupApply(self, playerID, userData, popupReturn):
		iStartYear = popupReturn.getSpinnerWidgetValue(int(0))
		CvScreensInterface.getWorldBuilderScreen().setStartYearCB(iStartYear)
		
# Dale - AoD: AoDCheatMenu START
	def AoDCheatMenuBegin(self, argsList):
		popup = CyPopup(CvUtil.EventAoDCheatMenu, EventContextTypes.EVENTCONTEXT_ALL, True)
		popup.setHeaderString(localText.getText("TXT_KEY_CHEATMENU_TITLE", ()), CvUtil.FONT_CENTER_JUSTIFY)
		popup.setBodyString(localText.getText("TXT_KEY_CHEATMENU_TEXT", ()), CvUtil.FONT_CENTER_JUSTIFY)
		popup.addButton(localText.getText("TXT_KEY_CHEATMENU_CANCEL", ()))
		popup.addSeparator()
		popup.setBodyString(localText.getText("TXT_KEY_AIAUTOPLAY", ()), CvUtil.FONT_CENTER_JUSTIFY)
		popup.createSpinBox(0, "", 0, 5, 300, 0)
		popup.addButton(localText.getText("TXT_KEY_AIAUTOPLAY10", ()))
		popup.addButton(localText.getText("TXT_KEY_AIAUTOPLAY50", ()))
		popup.addSeparator()
		popup.setBodyString(localText.getText("TXT_KEY_MONEYTREE", ()), CvUtil.FONT_CENTER_JUSTIFY)
		popup.createSpinBox(1, "", 0, 100, 10000, 0)
		popup.addButton(localText.getText("TXT_KEY_MONEYTREE1000", ()))
		popup.addButton(localText.getText("TXT_KEY_MONEYTREE5000", ()))
		popup.addSeparator()
		popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)
		return

	def AoDCheatMenuApply(self, playerID, userData, popupReturn):
		autoIdx = popupReturn.getButtonClicked()
		iPlayer = gc.getPlayer(playerID)
		iAutoplay = 0
		iAutoplay = popupReturn.getSpinnerWidgetValue(int(0))
		if (iAutoplay > 0):
			CyGame().setAIAutoPlay(iAutoplay)
		if (autoIdx == 0):
			return
		if (autoIdx == 1):
			CyGame().setAIAutoPlay(10)
		if (autoIdx == 2):
			CyGame().setAIAutoPlay(50)
		iMoneyTree = 0
		iMoneyTree = popupReturn.getSpinnerWidgetValue(int(1))
		if (iMoneyTree > 0):
			iPlayer.changeGold(iMoneyTree)
		if (autoIdx == 3):
			iPlayer.changeGold(1000)
		if (autoIdx == 4):
			iPlayer.changeGold(5000)
		return
# Dale - AoD: AoDCheatMenu END

# EuropeScreen START	
	def doEuropeScreenBegin(self, argslist):
		return 0
	
	def doEuropeScreenApply(self, playerID, userData, popupReturn):
		iMode, iUnit, iX, iY, iCityX, iCityY, iSellPrice, iYield, iBoycottPrice = userData
		
		SEND_TO_NEW_WORLD_CITY = 0
		SEND_TO_EAST_OR_WEST = 1
		RECALL_TO_EUROPE = 2
		SELL_SHIP_IN_EUROPE = 3
		LIFT_BOYCOTT_IN_EUROPE = 4
		RECALL_TO_AFRICA = 5
		RECALL_TO_PORT_ROYAL = 8
				
		player = gc.getPlayer(playerID)
		transport = player.getUnit(iUnit)
		
		if iMode == SEND_TO_NEW_WORLD_CITY:
			transport.getGroup().clearMissionQueue ()
			transport.getGroup().pushMoveToMission(iCityX, iCityY)
			if not iX == transport.getX() or not iY == transport.getY():
				transport.setXY(iX, iY, True, False, False)
		elif iMode == SEND_TO_EAST_OR_WEST:
			if not iX == transport.getX() or not iY == transport.getY():
				transport.setXY(iX, iY, True, False, False)
		elif iMode == RECALL_TO_EUROPE:
			transport.setUnitTravelState(1,False)
			transport.getGroup().clearMissionQueue()
		elif iMode == RECALL_TO_AFRICA:
			transport.setUnitTravelState(5,False)
			transport.getGroup().clearMissionQueue()
		elif iMode == RECALL_TO_PORT_ROYAL:
			transport.setUnitTravelState(8,False)
			transport.getGroup().clearMissionQueue()
		elif iMode == SELL_SHIP_IN_EUROPE:
			if (not transport.isNone() and not iSellPrice == -1):
				transport.kill(False)
				player.changeGold(iSellPrice)
				CyInterface().setDirty(InterfaceDirtyBits.EuropeScreen_DIRTY_BIT, True)
				CyInterface().setDirty(InterfaceDirtyBits.AfricaScreen_DIRTY_BIT, True)
				CyInterface().setDirty(InterfaceDirtyBits.PortRoyalScreen_DIRTY_BIT, True)
		elif iMode == LIFT_BOYCOTT_IN_EUROPE:
			if (not iYield == -1 and (player.getGold() - iBoycottPrice) >= 0):
				player.setYieldEuropeTradable(iYield, True)
				player.changeGold(-iBoycottPrice)
				CyInterface().setDirty(InterfaceDirtyBits.EuropeScreen_DIRTY_BIT, True)
				CyInterface().setDirty(InterfaceDirtyBits.AfricaScreen_DIRTY_BIT, True)
				
		return 0
		
# EuropeScreen END

