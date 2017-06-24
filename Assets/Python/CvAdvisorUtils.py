## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
##
## CvAdvisorUtils


from CvPythonExtensions import *

gc = CyGlobalContext()
localText = CyTranslator()

g_iAdvisorNags = 0
g_listUnitNags = []

def endTurnNags(iPlayer):
	if iPlayer == gc.getGame().getActivePlayer():
		global g_iAdvisorNags
		g_iAdvisorNags = 0
		global g_listUnitNags
		g_listUnitNags = []

def unitBuiltFeats(pCity, pUnit):
	return

def unitMoveFeats(pUnit, pPlot, pOldPlot):
	if not pPlot.isNone():
		player = gc.getPlayer(pUnit.getOwner())
		if (player.shouldDisplayFeatPopup(FeatTypes.FEAT_GOTO_EUROPE)):
			pGroup = pUnit.getGroup()
			if (not pGroup.isNone() and pGroup.getLengthMissionQueue() == 1):
				if (pGroup.getMissionType(0) == MissionTypes.MISSION_MOVE_TO and pGroup.at(pGroup.getMissionData1(0), pGroup.getMissionData2(0))):
					if (not pUnit.isAutomated() and pUnit.canDoCommand(CommandTypes.COMMAND_SAIL_TO_EUROPE, UnitTravelStates.UNIT_TRAVEL_STATE_TO_EUROPE, -1, false)):
						popupInfo = CyPopupInfo()
						popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_FEAT)
						popupInfo.setData1(FeatTypes.FEAT_GOTO_EUROPE)
						popupInfo.setData2(pUnit.getID())
						popupInfo.setText(localText.getText("TXT_KEY_GOTO_EUROPE_POPUP", ()))
						popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
						popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
						popupInfo.addPythonButton(localText.getText("TXT_KEY_GOTO_EUROPE_POPUP_YES", ()), "")
						popupInfo.addPythonButton(localText.getText("TXT_KEY_GOTO_EUROPE_POPUP_NO", ()), "")
						popupInfo.addPythonButton(localText.getText("TXT_KEY_GOTO_EUROPE_POPUP_NEVER_AGAIN", ()), "")
						popupInfo.addPopup(pUnit.getOwner())

def cityScreenFeats(iPlayer, iCityId):
	player = gc.getPlayer(iPlayer)
	if player.isOption(PlayerOptionTypes.PLAYEROPTION_TUTORIAL) and player.shouldDisplayFeatPopup(FeatTypes.FEAT_CITY_SCREEN):
		CyMessageControl().sendPlayerAction(iPlayer, PlayerActionTypes.PLAYER_ACTION_FEAT, FeatTypes.FEAT_CITY_SCREEN, 1, -1);
		popupInfo = CyPopupInfo()
		popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_FEAT)
		popupInfo.setData1(FeatTypes.FEAT_CITY_SCREEN)
		popupInfo.setText(localText.getText("TXT_KEY_EVENT_TUTORIAL_CITY_SCREEN", ()))
		popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
		popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
		popupInfo.addPythonButton(localText.getText("TXT_KEY_MAIN_MENU_OK", ()), "")
		popupInfo.addPythonButton(localText.getText("TXT_KEY_EVENT_TUTORIAL_END_TUTORIAL_2", ()), "")
		CyInterface().addPopup(popupInfo, iPlayer, true, false)
	
def buildingBuiltFeats(pCity, iBuildingType):
	return
	
def addUnitToNagList(pUnit):
	#this assumes endTurnNags has already occured this turn, otherwise the new information would be removed
	global g_listUnitNags
	if (pUnit.getOwner() == gc.getGame().getActivePlayer()):
		g_listUnitNags.append(pUnit.getID())

def unitSelectedFeats(pUnit):
	global g_listUnitNags
	player = gc.getPlayer(pUnit.getOwner())
	if (player.shouldDisplayFeatPopup(FeatTypes.FEAT_TREASURE)):
		if (pUnit.getUnitType() == gc.getInfoTypeForString("UNIT_TREASURE")):
			if pUnit.getID() not in g_listUnitNags:
				CyMessageControl().sendPlayerAction(pUnit.getOwner(), PlayerActionTypes.PLAYER_ACTION_FEAT, FeatTypes.FEAT_TREASURE, 1, -1);
				popupInfo = CyPopupInfo()
				popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_FEAT)
				popupInfo.setData1(FeatTypes.FEAT_TREASURE)
				popupInfo.setData2(pUnit.getID())
				popupInfo.setText(localText.getText("TXT_KEY_NEW_TREASURE", (pUnit.getNameKey(), )))
				popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
				popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
				popupInfo.addPythonButton(localText.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
				popupInfo.addPopup(pUnit.getOwner())
				g_listUnitNags.append(pUnit.getID())

	if (player.shouldDisplayFeatPopup(FeatTypes.FEAT_TREASURE_IN_PORT)):
		if (pUnit.canKingTransport()):
			if pUnit.getID() not in g_listUnitNags:
				CyMessageControl().sendPlayerAction(pUnit.getOwner(), PlayerActionTypes.PLAYER_ACTION_FEAT, FeatTypes.FEAT_TREASURE_IN_PORT, 1, -1);
				CyMessageControl().sendDoCommand(pUnit.getID(), CommandTypes.COMMAND_KING_TRANSPORT, -1, -1, false)
				g_listUnitNags.append(pUnit.getID())

	if (player.shouldDisplayFeatPopup(FeatTypes.FEAT_TALK_NATIVES)):
		if (pUnit.canSpeakWithChief(pUnit.plot()) or pUnit.canLearn() or pUnit.canTradeYield(pUnit.plot()) or pUnit.canEstablishMission()):
			if pUnit.getID() not in g_listUnitNags:
				popupInfo = CyPopupInfo()
				popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_TALK_NATIVES)
				popupInfo.setData1(pUnit.getID())
				popupInfo.addPopup(pUnit.getOwner())
				g_listUnitNags.append(pUnit.getID())

def endTurnFeats(iPlayer):
	player = gc.getPlayer(iPlayer)
	if (player.shouldDisplayFeatPopup(FeatTypes.FEAT_EUROPE_SHIPS)):
		if (player.countNumTravelUnits(UnitTravelStates.UNIT_TRAVEL_STATE_IN_EUROPE, DomainTypes.DOMAIN_SEA) > 0 and player.countNumTravelUnits(UnitTravelStates.UNIT_TRAVEL_STATE_TO_EUROPE, DomainTypes.DOMAIN_SEA) == 0):
			popupInfo = CyPopupInfo()
			popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_FEAT)
			popupInfo.setData1(FeatTypes.FEAT_EUROPE_SHIPS)
			popupInfo.setText(localText.getText("TXT_KEY_EUROPE_UNITS_POPUP", ()))
			popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
			popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
			popupInfo.addPythonButton(localText.getText("TXT_KEY_EUROPE_UNITS_POPUP_OK", ()), "")
			popupInfo.addPythonButton(localText.getText("TXT_KEY_EUROPE_UNITS_POPUP_SHOW", ()), "")
			popupInfo.addPythonButton(localText.getText("TXT_KEY_EUROPE_UNITS_POPUP_NO_MORE", ()), "")
			popupInfo.addPopup(iPlayer)
		
	return

def cityAdvise(pCity, iPlayer):

	global g_iAdvisorNags

	if (g_iAdvisorNags >= 2):
		return

	if (gc.getPlayer(iPlayer).isOption(PlayerOptionTypes.PLAYEROPTION_ADVISOR_POPUPS) and gc.getPlayer(iPlayer).isHuman() and not gc.getGame().isNetworkMultiPlayer()):

		eLiberationPlayer = pCity.getLiberationPlayer(false)
		if (eLiberationPlayer != -1):

			if (gc.getGame().getGameTurn()) % 40 == pCity.getGameTurnFounded() % 40:

				popupInfo = CyPopupInfo()
				popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
				popupInfo.setData1(pCity.getID())
				popupInfo.setText(localText.getText("TXT_KEY_POPUP_LIBERATION_DEMAND", (pCity.getNameKey(), gc.getPlayer(eLiberationPlayer).getCivilizationDescriptionKey(), gc.getPlayer(eLiberationPlayer).getNameKey())))
				popupInfo.setOnClickedPythonCallback("liberateOnClickedCallback")
				popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
				popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
				popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
				popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
				popupInfo.addPopup(iPlayer)
				g_iAdvisorNags += 1
				
	if (gc.getPlayer(iPlayer).shouldDisplayFeatPopup(FeatTypes.FEAT_CITY_MISSING_YIELD)):
		if (gc.getGame().getGameTurn()) % 20 == pCity.getGameTurnFounded() % 20:
			for iYield in range(YieldTypes.NUM_YIELD_TYPES):
				if gc.getYieldInfo(iYield).isCargo():
					order = pCity.isOrderWaitingForYield(iYield)
					if len(order) > 0:
						popupInfo = CyPopupInfo()
						popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_FEAT)
						popupInfo.setData1(FeatTypes.FEAT_CITY_MISSING_YIELD)
						if order[0] == OrderTypes.ORDER_TRAIN:
							orderKey = gc.getUnitInfo(order[1]).getTextKey()
						elif order[0] == OrderTypes.ORDER_CONSTRUCT:
							orderKey = gc.getBuildingInfo(order[1]).getTextKey()
						else:
							orderKey = "TXT_KEY_UNKNOWN"
						popupInfo.setText(localText.getText("TXT_KEY_CITY_MISSING_YIELD", (orderKey, pCity.getNameKey(), gc.getYieldInfo(iYield).getTextKey())))
						popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
						popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
						popupInfo.addPythonButton(localText.getText("TXT_KEY_EUROPE_UNITS_POPUP_OK", ()), "")
						popupInfo.addPythonButton(localText.getText("TXT_KEY_EUROPE_UNITS_POPUP_NO_MORE", ()), "")
						popupInfo.addPopup(pCity.getOwner())

		if (pCity.isProduction()):

			if (not pCity.isProductionBuilding() and (pCity.getOrderQueueLength() <= 1)):

				if ((gc.getGame().getGameTurn < 100) and (gc.getTeam(gc.getPlayer(iPlayer).getTeam()).getHasMetCivCount() > 0) and (pCity.getBuildingDefense() == 0)):

					if (gc.getGame().getGameTurn() + 4) % 40 == pCity.getGameTurnFounded() % 40:

						iBestValue = 0
						eBestBuilding = BuildingTypes.NO_BUILDING

						for iI in range(gc.getNumBuildingClassInfos()):

							eLoopBuilding = gc.getCivilizationInfo(gc.getPlayer(iPlayer).getCivilizationType()).getCivilizationBuildings(iI)

							if (eLoopBuilding != BuildingTypes.NO_BUILDING):

								if (gc.getBuildingInfo(eLoopBuilding).getDefenseModifier() > 0):

									if pCity.canConstruct(eLoopBuilding, False, False, False):

										iValue = gc.getBuildingInfo(eLoopBuilding).getDefenseModifier()

										if (iValue > iBestValue):

											iBestValue = iValue
											eBestBuilding = eLoopBuilding

						if (eBestBuilding != BuildingTypes.NO_BUILDING):
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(pCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_CONSTRUCT)
							popupInfo.setData3(eBestBuilding)
							popupInfo.setText(localText.getText("TXT_KEY_POPUP_BUILDING_DEFENSE_DEMAND", (pCity.getNameKey(), gc.getBuildingInfo(eBestBuilding).getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1

				iCultureYield = gc.getDefineINT("CULTURE_YIELD")
				if (pCity.getCultureRate() == 0 and not pCity.isOccupation() and iCultureYield != -1):

					if (gc.getGame().getGameTurn() + 7) % 40 == pCity.getGameTurnFounded() % 40:

						iBestValue = 0
						eBestBuilding = BuildingTypes.NO_BUILDING

						for iI in range(gc.getNumBuildingClassInfos()):

							eLoopBuilding = gc.getCivilizationInfo(gc.getPlayer(iPlayer).getCivilizationType()).getCivilizationBuildings(iI)
#MultipleYieldsProduced Start
							if (eLoopBuilding != BuildingTypes.NO_BUILDING):
								iValue = gc.getBuildingInfo(eLoopBuilding).getYieldChange(iCultureYield)
								for iJ in range(gc.getNumProfessionInfos()):
									if gc.getCivilizationInfo(gc.getPlayer(iPlayer).getCivilizationType()).isValidProfession(iJ):
										if (gc.getProfessionInfo(iJ).getYieldsProduced(0) == iCultureYield and gc.getProfessionInfo(iJ).getSpecialBuilding() == gc.getBuildingInfo(eLoopBuilding).getSpecialBuildingType()):
											iValue += gc.getBuildingInfo(eLoopBuilding).getProfessionOutput() * gc.getBuildingInfo(eLoopBuilding).getMaxWorkers()
#MultipleYieldsProduced End
								if (iValue > iBestValue):
									if pCity.canConstruct(eLoopBuilding, False, False, False):
											iBestValue = iValue
											eBestBuilding = eLoopBuilding

						if (eBestBuilding != BuildingTypes.NO_BUILDING):
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(pCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_CONSTRUCT)
							popupInfo.setData3(eBestBuilding)
							popupInfo.setText(localText.getText("TXT_KEY_POPUP_CULTURE_DEMAND", (pCity.getNameKey(), gc.getBuildingInfo(eBestBuilding).getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1


				if (pCity.countNumWaterPlots() > 10):

					if (gc.getGame().getGameTurn() + 11) % 40 == pCity.getGameTurnFounded() % 40:

						iBestValue = 0
						eBestBuilding = BuildingTypes.NO_BUILDING

						for iI in range(gc.getNumBuildingClassInfos()):

							eLoopBuilding = gc.getCivilizationInfo(gc.getPlayer(iPlayer).getCivilizationType()).getCivilizationBuildings(iI)

							if (eLoopBuilding != BuildingTypes.NO_BUILDING):

								if (gc.getBuildingInfo(eLoopBuilding).getSeaPlotYieldChange(YieldTypes.YIELD_FOOD) > 0):

									if pCity.canConstruct(eLoopBuilding, False, False, False):

										iValue = gc.getBuildingInfo(eLoopBuilding).getSeaPlotYieldChange(YieldTypes.YIELD_FOOD)

										if (iValue > iBestValue):

											iBestValue = iValue
											eBestBuilding = eLoopBuilding

						if (eBestBuilding != BuildingTypes.NO_BUILDING):
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(pCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_CONSTRUCT)
							popupInfo.setData3(eBestBuilding)
							popupInfo.setText(localText.getText("TXT_KEY_POPUP_WATER_FOOD_DEMAND", (pCity.getNameKey(), gc.getBuildingInfo(eBestBuilding).getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1
