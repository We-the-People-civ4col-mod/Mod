# Sid Meier's Civilization 4
# Copyright Firaxis Games 2005
#
# CvRandomEventInterface.py
#
# These functions are App Entry Points from C++
# WARNING: These function names should not be changed
# WARNING: These functions can not be placed into a class
#
# No other modules should import this
#
import sys
import CvUtil
import CvScreensInterface
from CvPythonExtensions import *

gc = CyGlobalContext()
localText = CyTranslator()

# ============================================================
# CORE HELPERS (DO NOT TOUCH)
# ============================================================

def _scaleTurnsByGameSpeed(iBaseTurns):
	iGameSpeedType = CyGame().getGameSpeedType()
	iGrowthPercent = gc.getGameSpeedInfo(iGameSpeedType).getGrowthPercent()
	return max(1, int((iBaseTurns * iGrowthPercent) / 100))

def get_simple_help(text_key):
	""" This function constructs another function that returns the fixed localized text  """
	def get_help(argsList):
		szHelp = localText.getText(text_key, ())
		return szHelp

	return get_help

def doEventEndTutorial(argsList):
	eEvent = argsList[1]
	pTriggeredData = argsList[0]
	player = gc.getPlayer(pTriggeredData.ePlayer)
	CyMessageControl().sendPlayerOption(PlayerOptionTypes.PLAYEROPTION_TUTORIAL, False)

def isExpiredFoundColony(argsList):
	eEvent = argsList[1]
	pTriggeredData = argsList[0]
	player = gc.getPlayer(pTriggeredData.ePlayer)
	if player.getNumCities() > 0:
		return True
	return False

def doEventCivilopediaSettlement(argsList):
	eEvent = argsList[1]
	pTriggeredData = argsList[0]
	CvScreensInterface.pediaShowHistorical((CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT, gc.getInfoTypeForString("CONCEPT_SETTLEMENTS")))

def canDoTriggerImmigrant(argsList):
	pTriggeredData = argsList[0]
	player = gc.getPlayer(pTriggeredData.ePlayer)
	if player.getNumEuropeUnits() == 0:
		return False
	return True

def canDoTriggerImmigrantDone(argsList):
	pTriggeredData = argsList[0]
	player = gc.getPlayer(pTriggeredData.ePlayer)
	if player.getNumEuropeUnits() > 0:
		return False
	return True

def doEventCivilopediaEurope(argsList):
	eEvent = argsList[1]
	pTriggeredData = argsList[0]
	CvScreensInterface.pediaShowHistorical((CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT, gc.getInfoTypeForString("CONCEPT_EUROPE")))

def doEventCivilopediaImmigration(argsList):
	eEvent = argsList[1]
	pTriggeredData = argsList[0]
	CvScreensInterface.pediaShowHistorical((CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT, gc.getInfoTypeForString("CONCEPT_IMMIGRATION")))


def canDoTriggerMotherland(argsList):
	pTriggeredData = argsList[0]
	player = gc.getPlayer(pTriggeredData.ePlayer)
	(unit, iter) = player.firstUnit()
	while (unit):
		if unit.getDomainType() == DomainTypes.DOMAIN_SEA and unit.getUnitTravelState() == UnitTravelStates.UNIT_TRAVEL_STATE_TO_EUROPE and unit.getUnitTravelTimer() == 1:
			return True
		(unit, iter) = player.nextUnit(iter)
	return False

def doEventCivilopediaProfessions(argsList):
	eEvent = argsList[1]
	pTriggeredData = argsList[0]
	CvScreensInterface.pediaShowHistorical((CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT, gc.getInfoTypeForString("CONCEPT_PROFESSIONS")))

def canDoTriggerPioneer(argsList):
	pTriggeredData = argsList[0]
	player = gc.getPlayer(pTriggeredData.ePlayer)

	improvementList = [gc.getInfoTypeForString("IMPROVEMENT_FARM"), gc.getInfoTypeForString("IMPROVEMENT_MINE"), gc.getInfoTypeForString("IMPROVEMENT_LODGE")]
	for iImprovement in improvementList:
		if player.getImprovementCount(iImprovement) > 0:
			return False

	ePioneer = gc.getInfoTypeForString("PROFESSION_PIONEER")
	(unit, iter) = player.firstUnit()
	while (unit):
		if unit.getProfession() == ePioneer:
			return False
		(unit, iter) = player.nextUnit(iter)

	return True

def canDoTriggerImproveLand(argsList):
	pTriggeredData = argsList[0]
	player = gc.getPlayer(pTriggeredData.ePlayer)

	improvementList = [gc.getInfoTypeForString("IMPROVEMENT_FARM"), gc.getInfoTypeForString("IMPROVEMENT_MINE"), gc.getInfoTypeForString("IMPROVEMENT_LODGE")]
	for iImprovement in improvementList:
		if player.getImprovementCount(iImprovement) > 0:
			return False

	ePioneer = gc.getInfoTypeForString("PROFESSION_PIONEER")
	(unit, iter) = player.firstUnit()
	while (unit):
		if unit.getProfession() == ePioneer:
			return True
		(unit, iter) = player.nextUnit(iter)

	return False

def doEventCivilopediaImproveLand(argsList):
	eEvent = argsList[1]
	pTriggeredData = argsList[0]
	CvScreensInterface.pediaShowHistorical((CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT, gc.getInfoTypeForString("CONCEPT_IMPROVEMENTS")))

def canDoTriggerFoundingFather(argsList):
	pTriggeredData = argsList[0]
	player = gc.getPlayer(pTriggeredData.ePlayer)
	team = gc.getTeam(player.getTeam())

	for iFather in range(gc.getNumFatherInfos()):
		if (team.canConvinceFather(iFather)):
			return True
	return False

def doEventCivilopediaFoundingFather(argsList):
	eEvent = argsList[1]
	pTriggeredData = argsList[0]
	CvScreensInterface.pediaShowHistorical((CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT, gc.getInfoTypeForString("CONCEPT_FATHERS")))

def doEventCivilopediaNativeVillages(argsList):
	eEvent = argsList[1]
	pTriggeredData = argsList[0]
	CvScreensInterface.pediaShowHistorical((CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT, gc.getInfoTypeForString("CONCEPT_NATIVES")))

def canDoTriggerRevolution(argsList):
	pTriggeredData = argsList[0]
	player = gc.getPlayer( pTriggeredData.ePlayer )

	if gc.getTeam(player.getTeam()).canDoRevolution():
		return True

	return False

def doEventCivilopediaRevolution(argsList):
	eEvent = argsList[1]
	pTriggeredData = argsList[0]
	CvScreensInterface.pediaShowHistorical((CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT, gc.getInfoTypeForString("CONCEPT_LIBERTY")))

def canDoCityTriggerTools(argsList):
	eTrigger = argsList[0]
	ePlayer = argsList[1]
	iCityId = argsList[2]
	player = gc.getPlayer(ePlayer)
	city = player.getCity(iCityId)

	if (not city.isNone() and city.getYieldRate(gc.getInfoTypeForString("YIELD_TOOLS")) > 0):
		return True

	return False

def doEventCivilopediaTools(argsList):
	eEvent = argsList[1]
	pTriggeredData = argsList[0]
	CvScreensInterface.pediaShowHistorical((CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT, gc.getInfoTypeForString("CONCEPT_PROFESSIONS")))

def canDoCityTriggerBuildingRequiresTools(argsList):
	eTrigger = argsList[0]
	ePlayer = argsList[1]
	iCityId = argsList[2]
	player = gc.getPlayer(ePlayer)
	city = player.getCity(iCityId)
	building = city.getProductionBuilding()

	if building != BuildingTypes.NO_BUILDING:
		if (gc.getBuildingInfo(building).getYieldCost(gc.getInfoTypeForString("YIELD_TOOLS")) > 0):
			return True

	return False

def doEventCivilopediaAutomatedTools(argsList):
	eEvent = argsList[1]
	pTriggeredData = argsList[0]
	CvScreensInterface.pediaShowHistorical((CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT, gc.getInfoTypeForString("CONCEPT_TRADE")))

def canDoSpeakToChief(argsList):
	pTriggeredData = argsList[0]
	player = gc.getPlayer(pTriggeredData.ePlayer)

	bFoundNative = False
	for iPlayer in range(gc.getMAX_PLAYERS()):
		loopPlayer = gc.getPlayer(iPlayer)
		if loopPlayer.isAlive() and loopPlayer.isNative():
			bFoundNative = True
			(city, iter) = loopPlayer.firstCity(True)
			while(city):
				if city.isScoutVisited(player.getTeam()):
					return False
				(city, iter) = loopPlayer.nextCity(iter, True)

	return bFoundNative

def canDoSpeakToChiefCompleted(argsList):
	pTriggeredData = argsList[0]
	player = gc.getPlayer(pTriggeredData.ePlayer)

	for iPlayer in range(gc.getMAX_PLAYERS()):
		loopPlayer = gc.getPlayer(iPlayer)
		if loopPlayer.isAlive() and loopPlayer.isNative():
			(city, iter) = loopPlayer.firstCity(True)
			while(city):
				if city.isScoutVisited(player.getTeam()):
					return True
				(city, iter) = loopPlayer.nextCity(iter, True)

	return False

def doEventCivilopediaWar(argsList):
	eEvent = argsList[1]
	pTriggeredData = argsList[0]
	CvScreensInterface.pediaShowHistorical((CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT, gc.getInfoTypeForString("CONCEPT_WAR")))

def canCityTriggerDoOverstock(argsList):
	eTrigger = argsList[0]
	ePlayer = argsList[1]
	iCityId = argsList[2]
	player = gc.getPlayer(ePlayer)
	city = player.getCity(iCityId)

	for i in range(YieldTypes.NUM_YIELD_TYPES):
		if (not city.isNone() and city.getYieldStored(i) > city.getMaxYieldCapacity() and i != gc.getInfoTypeForString("YIELD_FOOD")):
			return True

	return False

def canDoTaxes(argsList):
	pTriggeredData = argsList[0]
	player = gc.getPlayer(pTriggeredData.ePlayer)

	if player.getTaxRate() > 0:
		return True

	return False

####### TAC Events ########

######## SECOND CITY ###########

def canTriggerSecondCity(argsList):
	ePlayer = argsList[1]
	iCity = argsList[2]

	player = gc.getPlayer(ePlayer)
	city = player.getCity(iCity)

	if city.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.getNumCities() >= 2:
		return True

	return False

def applySecondCity1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	if not player.isHuman():
		city = player.firstCity(True)[0]
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	iYield1 = gc.getInfoTypeForString("YIELD_SAILCLOTH")
	city.changeYieldStored(iYield1, event.getGenericParameter(1)*Speed.getTrainPercent()/100)
	iYield2 = gc.getInfoTypeForString("YIELD_TOOLS")
	city.changeYieldStored(iYield2, event.getGenericParameter(2)*Speed.getTrainPercent()/100)
	iYield3 = gc.getInfoTypeForString("YIELD_HORSES")
	city.changeYieldStored(iYield3, event.getGenericParameter(3)*Speed.getTrainPercent()/100)

def getHelpSecondCity1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	city = player.getCity(kTriggeredData.iCityId)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	iYield1 = gc.getInfoTypeForString("YIELD_SAILCLOTH")
	iYield2 = gc.getInfoTypeForString("YIELD_TOOLS")
	iYield3 = gc.getInfoTypeForString("YIELD_HORSES")
	szHelp = localText.getText("TXT_KEY_EVENT_SECONDCOLONY_1_HELP", (king.getCivilizationAdjectiveKey(), ))
	if event.getGenericParameter(1) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_GAIN", (event.getGenericParameter(1)*Speed.getTrainPercent()/100,  gc.getYieldInfo(iYield1).getChar(), city.getNameKey()))
	if event.getGenericParameter(2) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_GAIN", (event.getGenericParameter(2)*Speed.getTrainPercent()/100,  gc.getYieldInfo(iYield2).getChar(), city.getNameKey()))
	if event.getGenericParameter(3) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_GAIN", (event.getGenericParameter(3)*Speed.getTrainPercent()/100,  gc.getYieldInfo(iYield3).getChar(), city.getNameKey()))
	if event.getGenericParameter(1) <> 0 :
		overflow = event.getGenericParameter(1)*Speed.getTrainPercent()/100 + city.getYieldStored(iYield1) - city.getMaxYieldCapacity()
		if overflow > 0:
			szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_OVERFLOW", (overflow,  gc.getYieldInfo(iYield1).getChar(), city.getNameKey()))
	if event.getGenericParameter(2) <> 0 :
		overflow = event.getGenericParameter(2)*Speed.getTrainPercent()/100 + city.getYieldStored(iYield2) - city.getMaxYieldCapacity()
		if overflow > 0:
			szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_OVERFLOW", (overflow,  gc.getYieldInfo(iYield2).getChar(), city.getNameKey()))
	if event.getGenericParameter(3) <> 0 :
		overflow = event.getGenericParameter(3)*Speed.getTrainPercent()/100 + city.getYieldStored(iYield3) - city.getMaxYieldCapacity()
		if overflow > 0:
			szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_OVERFLOW", (overflow,  gc.getYieldInfo(iYield3).getChar(), city.getNameKey()))
	return szHelp

def applySecondCity2(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	if not player.isHuman():
		city = player.firstCity(True)[0]
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	iYield1 = gc.getInfoTypeForString("YIELD_BLADES")
	city.changeYieldStored(iYield1, event.getGenericParameter(1)*Speed.getTrainPercent()/100)
	iYield2 = gc.getInfoTypeForString("YIELD_BAKERY_GOODS")
	city.changeYieldStored(iYield2, event.getGenericParameter(2)*Speed.getTrainPercent()/100)

def getHelpSecondCity2(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	city = player.getCity(kTriggeredData.iCityId)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	iYield1 = gc.getInfoTypeForString("YIELD_BLADES")
	iYield2 = gc.getInfoTypeForString("YIELD_BAKERY_GOODS")
	szHelp = localText.getText("TXT_KEY_EVENT_SECONDCOLONY_2_HELP", (king.getCivilizationAdjectiveKey(), ))
	if event.getGenericParameter(1) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_GAIN", (event.getGenericParameter(1)*Speed.getTrainPercent()/100,  gc.getYieldInfo(iYield1).getChar(), city.getNameKey()))
	if event.getGenericParameter(2) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_GAIN", (event.getGenericParameter(2)*Speed.getTrainPercent()/100,  gc.getYieldInfo(iYield2).getChar(), city.getNameKey()))
	if event.getGenericParameter(1) <> 0 :
		overflow = event.getGenericParameter(1)*Speed.getTrainPercent()/100 + city.getYieldStored(iYield1) - city.getMaxYieldCapacity()
		if overflow > 0:
			szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_OVERFLOW", (overflow,  gc.getYieldInfo(iYield1).getChar(), city.getNameKey()))
	if event.getGenericParameter(2) <> 0 :
		overflow = event.getGenericParameter(2)*Speed.getTrainPercent()/100 + city.getYieldStored(iYield2) - city.getMaxYieldCapacity()
		if overflow > 0:
			szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_OVERFLOW", (overflow,  gc.getYieldInfo(iYield2).getChar(), city.getNameKey()))
	return szHelp

######## THIRD CITY ###########

def canTriggerThirdCity(argsList):
	ePlayer = argsList[1]
	iCity = argsList[2]

	player = gc.getPlayer(ePlayer)
	city = player.getCity(iCity)

	if city.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.getNumCities() >= 3:
		return True

	return False

######## FESTIVITY ###########

def canTriggerFestivity(argsList):
	ePlayer = argsList[1]
	iCity = argsList[2]
	player = gc.getPlayer(ePlayer)
	city = player.getCity(iCity)

	if not player.isPlayable():
		return False
	if city.isNone():
		return False

	king = gc.getPlayer(player.getParent())
	if not king.isEurope():
		return False
	if player.isInRevolution():
		return False

	# Read Parameter 1 from the two events and check if enough yield is stored in city
	eEvent1 = gc.getInfoTypeForString("EVENT_FESTIVITY_2")
	event1 = gc.getEventInfo(eEvent1)
	eEvent2 = gc.getInfoTypeForString("EVENT_FESTIVITY_3")
	event2 = gc.getEventInfo(eEvent2)
	iYield1 = gc.getInfoTypeForString("YIELD_CIGARS")
	iYield2 = gc.getInfoTypeForString("YIELD_RUM")
	quantity = event1.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	quantity2 = event2.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity2 = quantity2 * Speed.getStoragePercent()/100

	if city.getYieldStored(iYield1) < -quantity and city.getYieldStored(iYield2) < -quantity2:
		return False
	# If player has reached the maximum for max tax rate, do not start event
	if player.NBMOD_GetMaxTaxRate() == GlobalDefines.MAX_TAX_RATE:
		return False
	return True

def applyFestivity1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	player.AI_changeAttitudeExtra(eking, event.getGenericParameter(2))
	king.AI_changeAttitudeExtra(kTriggeredData.ePlayer, event.getGenericParameter(2))
	if player.getTaxRate() + event.getGenericParameter(1) <= player.NBMOD_GetMaxTaxRate():
		player.changeTaxRate(event.getGenericParameter(1))
	else:
		player.NBMOD_IncreaseMaxTaxRate()

def getHelpFestivity1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	szHelp = localText.getText("TXT_KEY_EVENT_FESTIVITY_1_HELP", ())
	if (player.getTaxRate() + event.getGenericParameter(1) <= player.NBMOD_GetMaxTaxRate()) and event.getGenericParameter(1) <>0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_TAX_INCREASE", (event.getGenericParameter(1), player.getTaxRate() + event.getGenericParameter(1)))
	if (player.getTaxRate() + event.getGenericParameter(1) > player.NBMOD_GetMaxTaxRate()) and event.getGenericParameter(1) <>0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_MAXTAX_INCREASE", (GlobalDefines.INCREASE_MAX_TAX_RATE, player.NBMOD_GetMaxTaxRate()+GlobalDefines.INCREASE_MAX_TAX_RATE))
	if event.getGenericParameter(2) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_DECREASE", (event.getGenericParameter(2), king.getCivilizationAdjectiveKey()))
	return szHelp

def CanDoFestivity2(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	iYield = gc.getInfoTypeForString("YIELD_CIGARS")
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.isNone():
		return False
	if city.getYieldStored(iYield) < -quantity :
		return False
	return True

def applyFestivity2(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	city = player.getCity(kTriggeredData.iCityId)
	player.AI_changeAttitudeExtra(eking, event.getGenericParameter(3))
	king.AI_changeAttitudeExtra(kTriggeredData.ePlayer, event.getGenericParameter(3))
	iYield = gc.getInfoTypeForString("YIELD_CIGARS")
	iPrice = king.getYieldBuyPrice(iYield)
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	king.setYieldBuyPrice(iYield, iPrice+event.getGenericParameter(2), 1)
	city.changeYieldStored(iYield, quantity)
	if event.getGenericParameter(4) == 1 :
		player.NBMOD_DecreaseMaxTaxRate()

def getHelpFestivity2(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_CIGARS")
	szHelp = localText.getText("TXT_KEY_EVENT_FESTIVITY_2_HELP", ())
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if quantity <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity,  gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
	if event.getGenericParameter(2) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_PRICE_INCREASE", (event.getGenericParameter(2), gc.getYieldInfo(iYield).getChar(), king.getCivilizationShortDescriptionKey()))
	if event.getGenericParameter(4) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_MAXTAXDECREASE", (-GlobalDefines.DECREASE_MAX_TAX_RATE, player.NBMOD_GetMaxTaxRate()-GlobalDefines.DECREASE_MAX_TAX_RATE))
	if event.getGenericParameter(4) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_MAXTAXINCREASE", (-GlobalDefines.INCREASE_MAX_TAX_RATE, player.NBMOD_GetMaxTaxRate()-GlobalDefines.INCREASE_MAX_TAX_RATE))
	if event.getGenericParameter(3) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_INCREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	if event.getGenericParameter(3) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_DECREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	return szHelp

def CanDoFestivity3(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	iYield = gc.getInfoTypeForString("YIELD_RUM")
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity :
		return False
	return True

def applyFestivity3(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	city = player.getCity(kTriggeredData.iCityId)
	player.AI_changeAttitudeExtra(eking, event.getGenericParameter(3))
	king.AI_changeAttitudeExtra(kTriggeredData.ePlayer, event.getGenericParameter(3))
	iYield = gc.getInfoTypeForString("YIELD_RUM")
	iPrice = king.getYieldBuyPrice(iYield)
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	king.setYieldBuyPrice(iYield, iPrice+event.getGenericParameter(2), 1)
	city.changeYieldStored(iYield, quantity)
	if event.getGenericParameter(4) == 1 :
		player.NBMOD_DecreaseMaxTaxRate()

def getHelpFestivity3(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_RUM")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	szHelp = localText.getText("TXT_KEY_EVENT_FESTIVITY_2_HELP", ())
	if event.getGenericParameter(1) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity,  gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
	if event.getGenericParameter(2) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_PRICE_INCREASE", (event.getGenericParameter(2), gc.getYieldInfo(iYield).getChar(), king.getCivilizationShortDescriptionKey()))
	if event.getGenericParameter(4) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_MAXTAXDECREASE", (-GlobalDefines.DECREASE_MAX_TAX_RATE, player.NBMOD_GetMaxTaxRate()-GlobalDefines.DECREASE_MAX_TAX_RATE))
	if event.getGenericParameter(4) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_MAXTAXINCREASE", (-GlobalDefines.INCREASE_MAX_TAX_RATE, player.NBMOD_GetMaxTaxRate()-GlobalDefines.INCREASE_MAX_TAX_RATE))
	if event.getGenericParameter(3) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_INCREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	if event.getGenericParameter(3) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_DECREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	return szHelp

def CanDoFestivity4(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	iYield1 = gc.getInfoTypeForString("YIELD_CIGARS")
	iYield2 = gc.getInfoTypeForString("YIELD_RUM")
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if (city.getYieldStored(iYield1) < -quantity) or (city.getYieldStored(iYield2) < -quantity) :
		return False
	return True

def applyFestivity4(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	city = player.getCity(kTriggeredData.iCityId)
	player.AI_changeAttitudeExtra(eking, event.getGenericParameter(3))
	king.AI_changeAttitudeExtra(kTriggeredData.ePlayer, event.getGenericParameter(3))
	iYield = gc.getInfoTypeForString("YIELD_CIGARS")
	iPrice = king.getYieldBuyPrice(iYield)
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	king.setYieldBuyPrice(iYield, iPrice+event.getGenericParameter(2), 1)
	city.changeYieldStored(iYield, quantity)
	iYield = gc.getInfoTypeForString("YIELD_RUM")
	iPrice = king.getYieldBuyPrice(iYield)
	king.setYieldBuyPrice(iYield, iPrice+event.getGenericParameter(2), 1)
	city.changeYieldStored(iYield, quantity)
	if event.getGenericParameter(4) == 1 :
		player.NBMOD_DecreaseMaxTaxRate()

def getHelpFestivity4(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	city = player.getCity(kTriggeredData.iCityId)
	iYield1 = gc.getInfoTypeForString("YIELD_CIGARS")
	iYield2 = gc.getInfoTypeForString("YIELD_RUM")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	szHelp = localText.getText("TXT_KEY_EVENT_FESTIVITY_4_HELP", ())
	if event.getGenericParameter(1) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity,  gc.getYieldInfo(iYield1).getChar(), city.getNameKey()))
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity,  gc.getYieldInfo(iYield2).getChar(), city.getNameKey()))
	if event.getGenericParameter(2) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_PRICE_INCREASE", (event.getGenericParameter(2), gc.getYieldInfo(iYield1).getChar(), king.getCivilizationShortDescriptionKey()))
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_PRICE_INCREASE", (event.getGenericParameter(2), gc.getYieldInfo(iYield2).getChar(), king.getCivilizationShortDescriptionKey()))
	if event.getGenericParameter(4) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_MAXTAXDECREASE", (-GlobalDefines.DECREASE_MAX_TAX_RATE, player.NBMOD_GetMaxTaxRate()-GlobalDefines.DECREASE_MAX_TAX_RATE))
	if event.getGenericParameter(4) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_MAXTAXINCREASE", (-GlobalDefines.INCREASE_MAX_TAX_RATE, player.NBMOD_GetMaxTaxRate()-GlobalDefines.INCREASE_MAX_TAX_RATE))
	if event.getGenericParameter(3) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_INCREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	if event.getGenericParameter(3) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_DECREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	return szHelp

def getHelpCounterblaste1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_CIGARS")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	szHelp = localText.getText("TXT_KEY_EVENT_COUNTERBLASTE_1_HELP", ())
	if event.getGenericParameter(1) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity,  gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
	if event.getGenericParameter(2) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_PRICE_INCREASE", (event.getGenericParameter(2), gc.getYieldInfo(iYield).getChar(), king.getCivilizationShortDescriptionKey()))
	if event.getGenericParameter(4) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_MAXTAXDECREASE", (-GlobalDefines.DECREASE_MAX_TAX_RATE, player.NBMOD_GetMaxTaxRate()-GlobalDefines.DECREASE_MAX_TAX_RATE))
	if event.getGenericParameter(4) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_MAXTAXINCREASE", (-GlobalDefines.INCREASE_MAX_TAX_RATE, player.NBMOD_GetMaxTaxRate()-GlobalDefines.INCREASE_MAX_TAX_RATE))
	if event.getGenericParameter(3) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_INCREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	if event.getGenericParameter(3) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_DECREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	return szHelp

def getHelpCounterblaste2(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	city = player.getCity(kTriggeredData.iCityId)
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	iYield = gc.getInfoTypeForString("YIELD_CIGARS")
	szHelp = localText.getText("TXT_KEY_EVENT_COUNTERBLASTE_2_HELP", ())
	if event.getGenericParameter(1) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity,  gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
	if event.getGenericParameter(2) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_PRICE_INCREASE", (event.getGenericParameter(2), gc.getYieldInfo(iYield).getChar(), king.getCivilizationShortDescriptionKey()))
	if event.getGenericParameter(4) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_MAXTAXDECREASE", (-GlobalDefines.DECREASE_MAX_TAX_RATE, player.NBMOD_GetMaxTaxRate()-GlobalDefines.DECREASE_MAX_TAX_RATE))
	if event.getGenericParameter(4) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_MAXTAXINCREASE", (-GlobalDefines.INCREASE_MAX_TAX_RATE, player.NBMOD_GetMaxTaxRate()-GlobalDefines.INCREASE_MAX_TAX_RATE))
	if event.getGenericParameter(3) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_INCREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	if event.getGenericParameter(3) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_DECREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	return szHelp

def CanDoWhaling1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	iYield = gc.getInfoTypeForString("YIELD_WHALE_OIL")
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.isNone():
		return False
	if city.getYieldStored(iYield) < -quantity :
		return False
	return True

def applyWhaling1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_WHALE_OIL")
	iPrice = king.getYieldBuyPrice(iYield)
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	king.setYieldBuyPrice(iYield, iPrice+event.getGenericParameter(2), 1)
	city.changeYieldStored(iYield, quantity)
	if event.getGenericParameter(4) == 1 :
		player.NBMOD_DecreaseMaxTaxRate()

def getHelpWhaling1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_WHALE_OIL")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	szHelp = localText.getText("TXT_KEY_EVENT_WHALING_1_HELP", ())
	if event.getGenericParameter(1) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity,  gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
	if event.getGenericParameter(2) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_PRICE_INCREASE", (event.getGenericParameter(2), gc.getYieldInfo(iYield).getChar(), king.getCivilizationShortDescriptionKey()))
	if event.getGenericParameter(4) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_MAXTAXDECREASE", (-GlobalDefines.DECREASE_MAX_TAX_RATE, player.NBMOD_GetMaxTaxRate()-GlobalDefines.DECREASE_MAX_TAX_RATE))
	if event.getGenericParameter(4) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_MAXTAXINCREASE", (-GlobalDefines.INCREASE_MAX_TAX_RATE, player.NBMOD_GetMaxTaxRate()-GlobalDefines.INCREASE_MAX_TAX_RATE))
	if event.getGenericParameter(3) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_INCREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	if event.getGenericParameter(3) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_DECREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	return szHelp

######## WINTER ###########

def _isWinterSeasonNow():
	game = CyGame()
	gameSpeed = gc.getGameSpeedInfo(game.getGameSpeedType())
	iMonthIncrement = gameSpeed.getGameTurnInfo(0).iMonthIncrement
	iCurrentTurn = game.getGameTurn()
	szDate = CyGameTextMgr().getTimeStr(iCurrentTurn + 1, True)

	January = localText.getText("TXT_KEY_MONTH_JANUARY", ())
	February = localText.getText("TXT_KEY_MONTH_FEBRUARY", ())
	December = localText.getText("TXT_KEY_MONTH_DECEMBER", ())
	November = localText.getText("TXT_KEY_MONTH_NOVEMBER", ())
	October = localText.getText("TXT_KEY_MONTH_OCTOBER", ())

	# 1) Monatssystem
	if iMonthIncrement < 6:
		if (January in szDate or February in szDate or December in szDate or November in szDate or October in szDate):
			return True
		return False

	# 2) Halbjahre
	if iMonthIncrement == 6:
		if (October in szDate or November in szDate or December in szDate or January in szDate or February in szDate):
			return True

		# Fallback falls Monatsnamen nicht zuverlässig sind
		if (iCurrentTurn % 2) == 0:
			return True

		return False

	# 3) Jahres-System (Fallback)
	if (iCurrentTurn % 4) == 0:
		return True

	return False


def canTriggerWinter(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False
	if not player.isPlayable():
		return False
	if player.isNative():
		return False

	king = gc.getPlayer(player.getParent())
	if king.isNone():
		return False
	if not king.isEurope():
		return False

	return _isWinterSeasonNow()


def applyWinter(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	iYield1 = gc.getInfoTypeForString("YIELD_COATS")
	iYield2 = gc.getInfoTypeForString("YIELD_FUR")

	player = gc.getPlayer(kTriggeredData.ePlayer)
	king = gc.getPlayer(player.getParent())

	iPrice1 = king.getYieldBuyPrice(iYield1)
	king.setYieldBuyPrice(iYield1, iPrice1 + event.getGenericParameter(1), 1)

	iPrice2 = king.getYieldBuyPrice(iYield2)
	king.setYieldBuyPrice(iYield2, iPrice2 + event.getGenericParameter(2), 1)


def getHelpWinter(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	king = gc.getPlayer(player.getParent())

	iYield1 = gc.getInfoTypeForString("YIELD_COATS")
	iYield2 = gc.getInfoTypeForString("YIELD_FUR")

	szHelp = localText.getText("TXT_KEY_EVENT_WINTER_HELP", (king.getCivilizationShortDescriptionKey(),))

	if event.getGenericParameter(1) <> 0:
		szHelp += "\n" + localText.getText(
			"TXT_KEY_EVENT_PRICE_INCREASE",
			(event.getGenericParameter(1), gc.getYieldInfo(iYield1).getChar(), king.getCivilizationShortDescriptionKey())
		)

	if event.getGenericParameter(2) <> 0:
		szHelp += "\n" + localText.getText(
			"TXT_KEY_EVENT_PRICE_INCREASE",
			(event.getGenericParameter(2), gc.getYieldInfo(iYield2).getChar(), king.getCivilizationShortDescriptionKey())
		)

	return szHelp


def canEndWinter(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False
	if not player.isPlayable():
		return False
	if player.isNative():
		return False

	king = gc.getPlayer(player.getParent())
	if king.isNone():
		return False
	if not king.isEurope():
		return False

	# Winter darf erst enden, wenn Mindestdauer erreicht ist
	eWinterEvent = gc.getInfoTypeForString("EVENT_WINTER_1")
	kEventData = player.getEventOccured(eWinterEvent)

	if kEventData is None:
		return True  # failsafe

	iWinterTurn = kEventData.iTurn
	iCurrentTurn = CyGame().getGameTurn()

	iMinDuration = _scaleTurnsByGameSpeed(30)

	if iCurrentTurn < iWinterTurn + iMinDuration:
		return False

	# danach zusätzlich prüfen: sind wir noch im Winter?
	return not _isWinterSeasonNow()


def applyEndWinter(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	iYield1 = gc.getInfoTypeForString("YIELD_COATS")
	iYield2 = gc.getInfoTypeForString("YIELD_FUR")

	player = gc.getPlayer(kTriggeredData.ePlayer)
	king = gc.getPlayer(player.getParent())

	iPrice1 = king.getYieldBuyPrice(iYield1)
	king.setYieldBuyPrice(iYield1, iPrice1 + event.getGenericParameter(1), 1)

	iPrice2 = king.getYieldBuyPrice(iYield2)
	king.setYieldBuyPrice(iYield2, iPrice2 + event.getGenericParameter(2), 1)


def getHelpEndWinter(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	king = gc.getPlayer(player.getParent())

	iYield1 = gc.getInfoTypeForString("YIELD_COATS")
	iYield2 = gc.getInfoTypeForString("YIELD_FUR")

	szHelp = localText.getText("TXT_KEY_EVENT_END_WINTER_HELP", (king.getCivilizationShortDescriptionKey(),))

	if event.getGenericParameter(1) <> 0:
		szHelp += "\n" + localText.getText(
			"TXT_KEY_EVENT_PRICE_DECREASE",
			(event.getGenericParameter(1), gc.getYieldInfo(iYield1).getChar(), king.getCivilizationShortDescriptionKey())
		)

	if event.getGenericParameter(2) <> 0:
		szHelp += "\n" + localText.getText(
			"TXT_KEY_EVENT_PRICE_DECREASE",
			(event.getGenericParameter(2), gc.getYieldInfo(iYield2).getChar(), king.getCivilizationShortDescriptionKey())
		)

	return szHelp

######## Peasant War Preparations ###########

def canTriggerPeasantWarPrep(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	player2 = gc.getPlayer(kTriggeredData.eOtherPlayer)
	#iStartYear = 1495
	if not player.isPlayable() or not player2.isPlayable() :
		return False
	if not player.isHuman():
		return False
	king = gc.getPlayer(player.getParent())
	if not king.isEurope():
		return False
	#iCurrentYear = CyGame().getGameTurnYear()
	#if iCurrentYear < iStartYear :
	#	return False
	#iChance = gc.getGame().getSorenRandNum(100, "(c) TAC 2010 Events")
	#iChance = iChance + 10 * (iCurrentYear-iStartYear)+5
	#if iChance > 100 :
	#	return true
	#return False
	return True

def applyPeasantWarPrep(argsList):
	kTriggeredData = argsList[0]
	iPriceChange = 2
	iYield1 = gc.getInfoTypeForString("YIELD_MUSKETS")
	iYield2 = gc.getInfoTypeForString("YIELD_FOOD")
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	iPrice1 = king.getYieldBuyPrice(iYield1)
	king.setYieldBuyPrice(iYield1, iPrice1+iPriceChange, 1)
	iPrice2 = king.getYieldBuyPrice(iYield2)
	king.setYieldBuyPrice(iYield2, iPrice2+iPriceChange, 1)

def getHelpPeasantWarPrep(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	iYield1 = gc.getInfoTypeForString("YIELD_MUSKETS")
	iYield2 = gc.getInfoTypeForString("YIELD_FOOD")
	iPriceChange = 2
	szHelp = localText.getText("TXT_KEY_EVENT_PEASANT_WARPREP_HELP", (iPriceChange, gc.getYieldInfo(iYield1).getChar(), king.getCivilizationDescriptionKey(), iPriceChange, gc.getYieldInfo(iYield2).getChar(), king.getCivilizationDescriptionKey()))
	return szHelp

def getHelpPeasantWarPrepWarend(argsList):
	return localText.getText(
		"TXT_KEY_EVENT_PEASANT_WARPREP_WAREND_HELP",
		()
	)

######## DISCOVERY LEGENDARY SCOUT EVENT ###########

DISCOVERY_LEGENDARY_SCOUT_LEFT_TURN_PREFIX = "[[WTP_DISCOVERY_LEGENDARY_SCOUT_LEFT_TURN="
DISCOVERY_LEGENDARY_SCOUT_LEFT_TURN_SUFFIX = "]]"

def _getDiscoveryLegendaryScoutLeftTurn(unit):
	if unit is None or unit.isNone():
		return -1

	szData = unit.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(DISCOVERY_LEGENDARY_SCOUT_LEFT_TURN_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(DISCOVERY_LEGENDARY_SCOUT_LEFT_TURN_PREFIX)
	iEnd = szData.find(DISCOVERY_LEGENDARY_SCOUT_LEFT_TURN_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1

def _setDiscoveryLegendaryScoutLeftTurn(unit, iTurn):
	if unit is None or unit.isNone():
		return

	szData = unit.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(DISCOVERY_LEGENDARY_SCOUT_LEFT_TURN_PREFIX)
	if iStart != -1:
		iEnd = szData.find(DISCOVERY_LEGENDARY_SCOUT_LEFT_TURN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(DISCOVERY_LEGENDARY_SCOUT_LEFT_TURN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szMarker = "%s%d%s" % (
		DISCOVERY_LEGENDARY_SCOUT_LEFT_TURN_PREFIX,
		iTurn,
		DISCOVERY_LEGENDARY_SCOUT_LEFT_TURN_SUFFIX
	)

	szData += szMarker
	unit.setScriptData(szData)

def _getDiscoveryLegendaryScoutRequiredTurns():
	iBaseTurns = 75
	iGameSpeed = CyGame().getGameSpeedType()
	gameSpeedInfo = gc.getGameSpeedInfo(iGameSpeed)
	iPercent = gameSpeedInfo.getGrowthPercent()

	iScaledTurns = iBaseTurns * iPercent / 100
	if iScaledTurns < 1:
		iScaledTurns = 1

	return int(iScaledTurns)

def _isValidDiscoveryLegendaryScoutUnit(player, unit, kTriggeredData):
	if player.isNone():
		return False
	if not player.isAlive():
		return False
	if not player.isPlayable():
		return False
	if player.isNative():
		return False

	if unit is None or unit.isNone():
		return False

	eScoutProfession = gc.getInfoTypeForString("PROFESSION_SCOUT")
	if unit.getProfession() != eScoutProfession:
		return False

	ePromo = gc.getInfoTypeForString("PROMOTION_LEGENDARY_SCOUT")
	if unit.isHasPromotion(ePromo):
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return False

	if plot.isWater():
		return False

	if plot.isCity():
		return False

	if plot.getOwner() == player.getID():
		return False

	return True

def canTriggerDiscoveryLegendaryScout(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if not _isValidDiscoveryLegendaryScoutUnit(player, unit, kTriggeredData):
		return False

	iCurrentTurn = CyGame().getGameTurn()
	iLeftTurn = _getDiscoveryLegendaryScoutLeftTurn(unit)

	# First valid turn in the wilderness
	if iLeftTurn == -1:
		_setDiscoveryLegendaryScoutLeftTurn(unit, iCurrentTurn)
		return False

	iTurnsAway = iCurrentTurn - iLeftTurn
	iRequiredTurns = _getDiscoveryLegendaryScoutRequiredTurns()

	if iTurnsAway < iRequiredTurns:
		return False

	return True

def applyDiscoveryLegendaryScoutXP(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	unit = player.getUnit(kTriggeredData.iUnitId)
	if not _isValidDiscoveryLegendaryScoutUnit(player, unit, kTriggeredData):
		return

	iXP = 15 + CyGame().getSorenRandNum(6, "Legendary Scout XP")

	unit.changeExperience(iXP, -1, False, False, False)

	ePromo = gc.getInfoTypeForString("PROMOTION_LEGENDARY_SCOUT")
	unit.setHasRealPromotion(ePromo, True)

	_setDiscoveryLegendaryScoutLeftTurn(unit, -1)

	CyInterface().addMessage(
		kTriggeredData.ePlayer,
		True,
		10,
		localText.getText("TXT_KEY_EVENT_DISCOVERY_LEGENDARY_SCOUT_XP_RESULT", (iXP,)),
		"",
		0,
		"",
		ColorTypes(8),
		unit.getX(),
		unit.getY(),
		True,
		True
	)

def getHelpDiscoveryLegendaryScoutXP(argsList):
	iTurns = _getDiscoveryLegendaryScoutRequiredTurns()

	return localText.getText(
		"TXT_KEY_EVENT_DISCOVERY_LEGENDARY_SCOUT_XP_HELP",
		(iTurns,)
	)

######## Seasoned Native City Events (Scout + Trader) ###########

SEASONED_SCOUT_NATIVE_CITY_SOFT_COOLDOWN_PREFIX = "[[WTP_SEASONED_SCOUT_NATIVE_CITY_SOFT_READY_TURN="
SEASONED_TRADER_NATIVE_CITY_SOFT_COOLDOWN_PREFIX = "[[WTP_SEASONED_TRADER_NATIVE_CITY_SOFT_READY_TURN="
SEASONED_NATIVE_CITY_SOFT_COOLDOWN_SUFFIX = "]]"


def _getSeasonedNativeCitySoftCooldownPrefix(kTriggeredData):
	eTrigger = kTriggeredData.eTrigger
	if eTrigger == -1:
		return ""

	triggerInfo = gc.getEventTriggerInfo(eTrigger)
	if triggerInfo is None:
		return ""

	szTriggerType = triggerInfo.getType()

	if szTriggerType == "EVENTTRIGGER_SEASONED_SCOUT_NATIVE_CITY":
		return SEASONED_SCOUT_NATIVE_CITY_SOFT_COOLDOWN_PREFIX

	if szTriggerType == "EVENTTRIGGER_SEASONED_TRADER_NATIVE_CITY":
		return SEASONED_TRADER_NATIVE_CITY_SOFT_COOLDOWN_PREFIX

	return ""


def _getSeasonedNativeCitySoftCooldownReadyTurn(player, kTriggeredData):
	if player.isNone():
		return -1

	szPrefix = _getSeasonedNativeCitySoftCooldownPrefix(kTriggeredData)
	if szPrefix == "":
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(szPrefix)
	if iStart == -1:
		return -1

	iStart += len(szPrefix)
	iEnd = szData.find(SEASONED_NATIVE_CITY_SOFT_COOLDOWN_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1


def _setSeasonedNativeCitySoftCooldownReadyTurn(player, kTriggeredData, iReadyTurn):
	if player.isNone():
		return

	szPrefix = _getSeasonedNativeCitySoftCooldownPrefix(kTriggeredData)
	if szPrefix == "":
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(szPrefix)
	if iStart != -1:
		iEnd = szData.find(SEASONED_NATIVE_CITY_SOFT_COOLDOWN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(SEASONED_NATIVE_CITY_SOFT_COOLDOWN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szMarker = "%s%d%s" % (
		szPrefix,
		iReadyTurn,
		SEASONED_NATIVE_CITY_SOFT_COOLDOWN_SUFFIX
	)

	szData += szMarker
	player.setScriptData(szData)


def _startSeasonedNativeCitySoftCooldown(player, kTriggeredData):
	if player.isNone():
		return

	_setSeasonedNativeCitySoftCooldownReadyTurn(
		player,
		kTriggeredData,
		CyGame().getGameTurn() + 60
	)


def _isSeasonedNativeCitySoftCooldownActive(player, kTriggeredData):
	if player.isNone():
		return False

	iReadyTurn = _getSeasonedNativeCitySoftCooldownReadyTurn(player, kTriggeredData)
	return iReadyTurn > CyGame().getGameTurn()


def _getSeasonedNativeCityTrackedUnit(player, kTriggeredData):
	if player.isNone():
		return None

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit is None or unit.isNone():
		return None

	return unit


def _getSeasonedNativeCityRequiredProfession(kTriggeredData):
	eTrigger = kTriggeredData.eTrigger
	if eTrigger == -1:
		return -1

	triggerInfo = gc.getEventTriggerInfo(eTrigger)
	if triggerInfo is None:
		return -1

	szTriggerType = triggerInfo.getType()

	if szTriggerType == "EVENTTRIGGER_SEASONED_SCOUT_NATIVE_CITY":
		return gc.getInfoTypeForString("PROFESSION_SCOUT")

	if szTriggerType == "EVENTTRIGGER_SEASONED_TRADER_NATIVE_CITY":
		return gc.getInfoTypeForString("PROFESSION_NATIVE_TRADER")

	return -1


def _getSeasonedNativeCityPlotCityFromUnit(unit):
	if unit is None or unit.isNone():
		return None

	plot = unit.plot()
	if plot is None or plot.isNone():
		return None

	if not plot.isCity():
		return None

	return plot.getPlotCity()


def _getSeasonedNativeCityNativePlayerFromUnit(unit):
	city = _getSeasonedNativeCityPlotCityFromUnit(unit)
	if city is None:
		return None

	iOwner = city.getOwner()
	if iOwner < 0:
		return None

	nativePlayer = gc.getPlayer(iOwner)
	if nativePlayer is None or nativePlayer.isNone():
		return None

	if not nativePlayer.isNative():
		return None

	return nativePlayer


def _isValidSeasonedNativeCityContext(player, unit, plot, kTriggeredData):
	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	if unit is None or unit.isNone():
		return False

	if plot is None or plot.isNone():
		return False

	if not plot.isCity():
		return False

	nativePlayer = _getSeasonedNativeCityNativePlayerFromUnit(unit)
	if nativePlayer is None:
		return False

	eRequiredProfession = _getSeasonedNativeCityRequiredProfession(kTriggeredData)
	if eRequiredProfession == -1:
		return False

	if unit.getProfession() != eRequiredProfession:
		return False

	return True


def _spawnSeasonedNativeCityHostileAdjacent(plot, iHostileUnitClass):
	if plot is None or plot.isNone():
		return None

	if iHostileUnitClass == -1:
		return None

	iBarbarian = gc.getGame().getBarbarianPlayer()
	barbPlayer = gc.getPlayer(iBarbarian)
	if barbPlayer.isNone():
		return None

	barbCiv = gc.getCivilizationInfo(barbPlayer.getCivilizationType())
	iUnitType = barbCiv.getCivilizationUnits(iHostileUnitClass)
	if iUnitType == UnitTypes.NO_UNIT:
		return None

	for iDX in range(-1, 2):
		for iDY in range(-1, 2):
			if iDX == 0 and iDY == 0:
				continue

			pLoop = plotXY(plot.getX(), plot.getY(), iDX, iDY)
			if pLoop is None or pLoop.isNone():
				continue
			if pLoop.isWater():
				continue
			if pLoop.isImpassable():
				continue
			if pLoop.isPeak():
				continue
			if pLoop.isCity():
				continue
			if pLoop.isUnit():
				continue

			return barbPlayer.initUnit(
				iUnitType,
				ProfessionTypes.NO_PROFESSION,
				pLoop.getX(),
				pLoop.getY(),
				UnitAITypes.NO_UNITAI,
				DirectionTypes.DIRECTION_SOUTH,
				0
			)

	return None


def _getSeasonedNativeCityTriggerChance(kTriggeredData):
	eTrigger = kTriggeredData.eTrigger
	if eTrigger == -1:
		return 100

	triggerInfo = gc.getEventTriggerInfo(eTrigger)
	if triggerInfo is None:
		return 100

	szTriggerType = triggerInfo.getType()
	iCurrentTurn = CyGame().getGameTurn()

	iEarlyTurns = _scaleTurnsByGameSpeed(100)
	iMidTurns = _scaleTurnsByGameSpeed(200)

	if szTriggerType == "EVENTTRIGGER_SEASONED_SCOUT_NATIVE_CITY":
		if iCurrentTurn <= iEarlyTurns:
			return 70
		if iCurrentTurn <= iMidTurns:
			return 50
		return 30

	if szTriggerType == "EVENTTRIGGER_SEASONED_TRADER_NATIVE_CITY":
		if iCurrentTurn <= iEarlyTurns:
			return 40
		if iCurrentTurn <= iMidTurns:
			return 50
		return 70

	return 100


def canTriggerSeasonedNativeCity(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	if _isSeasonedNativeCitySoftCooldownActive(player, kTriggeredData):
		return False

	unit = _getSeasonedNativeCityTrackedUnit(player, kTriggeredData)
	if unit is None:
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	if not _isValidSeasonedNativeCityContext(player, unit, plot, kTriggeredData):
		return False

	iChance = _getSeasonedNativeCityTriggerChance(kTriggeredData)
	if CyGame().getSorenRandNum(100, "Seasoned Native City trigger") >= iChance:
		return False

	return True


def applySeasonedNativeCityStartCooldown(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	unit = _getSeasonedNativeCityTrackedUnit(player, kTriggeredData)
	if unit is None:
		return

	plot = unit.plot()
	if plot is None or plot.isNone():
		return

	if not _isValidSeasonedNativeCityContext(player, unit, plot, kTriggeredData):
		return

	_startSeasonedNativeCitySoftCooldown(player, kTriggeredData)


def applySeasonedNativeCity1(argsList):
	applySeasonedNativeCityStartCooldown(argsList)


def applySeasonedNativeCity2(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	unit = _getSeasonedNativeCityTrackedUnit(player, kTriggeredData)
	if unit is None:
		return

	plot = unit.plot()
	if plot is None or plot.isNone():
		return

	if not _isValidSeasonedNativeCityContext(player, unit, plot, kTriggeredData):
		return

	iHostileUnitClass = event.getGenericParameter(1)

	hostileUnit = _spawnSeasonedNativeCityHostileAdjacent(plot, iHostileUnitClass)

	if hostileUnit is not None and not hostileUnit.isNone():
		if hostileUnit.canMoveInto(plot, True, False, False):
			hostileUnit.attack(plot, False)

	applySeasonedNativeCityStartCooldown(argsList)


def applySeasonedNativeCity3(argsList):
	applySeasonedNativeCityStartCooldown(argsList)


def _getSeasonedNativeCityNativePlayerName(unit):
	nativePlayer = _getSeasonedNativeCityNativePlayerFromUnit(unit)
	if nativePlayer is None:
		return u""

	return nativePlayer.getCivilizationDescription(0)


def _getSeasonedNativeCityHelp1(unit, event):
	return u""


def _getSeasonedNativeCityHelp2(unit, event):
	szNativeName = _getSeasonedNativeCityNativePlayerName(unit)
	if szNativeName == u"":
		return u""

	return localText.getText(
		"TXT_KEY_EVENT_SEASONED_NATIVE_CITY_HELP_ANGRY_ATTACK",
		(szNativeName,)
	)


def _getSeasonedNativeCityHelp3(unit, event):
	szNativeName = _getSeasonedNativeCityNativePlayerName(unit)
	if szNativeName == u"":
		return u""

	return localText.getText(
		"TXT_KEY_EVENT_SEASONED_NATIVE_CITY_HELP_UNPLEASED",
		(szNativeName,)
	)


def _getHelpSeasonedNativeCity(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return u""

	unit = _getSeasonedNativeCityTrackedUnit(player, kTriggeredData)
	if unit is None:
		return u""

	szEventType = event.getType()

	if szEventType.endswith("_1"):
		return _getSeasonedNativeCityHelp1(unit, event)

	if szEventType.endswith("_2"):
		return _getSeasonedNativeCityHelp2(unit, event)

	if szEventType.endswith("_3"):
		return _getSeasonedNativeCityHelp3(unit, event)

	return u""


def getHelpSeasonedScoutNativeCity1(argsList):
	return _getHelpSeasonedNativeCity(argsList)


def getHelpSeasonedScoutNativeCity2(argsList):
	return _getHelpSeasonedNativeCity(argsList)


def getHelpSeasonedScoutNativeCity3(argsList):
	return _getHelpSeasonedNativeCity(argsList)


def getHelpSeasonedTraderNativeCity1(argsList):
	return _getHelpSeasonedNativeCity(argsList)


def getHelpSeasonedTraderNativeCity2(argsList):
	return _getHelpSeasonedNativeCity(argsList)


def getHelpSeasonedTraderNativeCity3(argsList):
	return _getHelpSeasonedNativeCity(argsList)

######## Discovery Attacked Event ###########

DISCOVERY_ATTACKED_SOFT_COOLDOWN_PREFIX = "[[WTP_DISCOVERY_ATTACKED_SOFT_READY_TURN="
DISCOVERY_ATTACKED_SOFT_COOLDOWN_SUFFIX = "]]"

def _getDiscoveryAttackedSoftCooldownReadyTurn(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(DISCOVERY_ATTACKED_SOFT_COOLDOWN_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(DISCOVERY_ATTACKED_SOFT_COOLDOWN_PREFIX)
	iEnd = szData.find(DISCOVERY_ATTACKED_SOFT_COOLDOWN_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1

def _setDiscoveryAttackedSoftCooldownReadyTurn(player, iReadyTurn):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(DISCOVERY_ATTACKED_SOFT_COOLDOWN_PREFIX)
	if iStart != -1:
		iEnd = szData.find(DISCOVERY_ATTACKED_SOFT_COOLDOWN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(DISCOVERY_ATTACKED_SOFT_COOLDOWN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szMarker = "%s%d%s" % (
		DISCOVERY_ATTACKED_SOFT_COOLDOWN_PREFIX,
		iReadyTurn,
		DISCOVERY_ATTACKED_SOFT_COOLDOWN_SUFFIX
	)

	szData += szMarker
	player.setScriptData(szData)

def _startDiscoveryAttackedSoftCooldown(player):
	if player.isNone():
		return

	iCooldownTurns = _scaleTurnsByGameSpeed(15)

	_setDiscoveryAttackedSoftCooldownReadyTurn(
		player,
		CyGame().getGameTurn() + iCooldownTurns
	)

def _isDiscoveryAttackedSoftCooldownActive(player):
	if player.isNone():
		return False

	iReadyTurn = _getDiscoveryAttackedSoftCooldownReadyTurn(player)
	return iReadyTurn > CyGame().getGameTurn()

def _spawnDiscoveryAttackedHostileAdjacent(plot):
	iHostileUnitClass = gc.getInfoTypeForString("UNITCLASS_HOSTILE_NATIVE")
	if iHostileUnitClass == -1:
		return None

	if plot is None or plot.isNone():
		return None

	iBarbarian = gc.getGame().getBarbarianPlayer()
	barbPlayer = gc.getPlayer(iBarbarian)
	if barbPlayer.isNone():
		return None

	barbCiv = gc.getCivilizationInfo(barbPlayer.getCivilizationType())
	iUnitType = barbCiv.getCivilizationUnits(iHostileUnitClass)
	if iUnitType == UnitTypes.NO_UNIT:
		return None

	for iDX in range(-1, 2):
		for iDY in range(-1, 2):
			if iDX == 0 and iDY == 0:
				continue

			pLoop = plotXY(plot.getX(), plot.getY(), iDX, iDY)
			if pLoop is None or pLoop.isNone():
				continue
			if pLoop.isWater():
				continue
			if pLoop.isImpassable():
				continue
			if pLoop.isPeak():
				continue
			if pLoop.isCity():
				continue
			if pLoop.isUnit():
				continue

			pUnit = barbPlayer.initUnit(
				iUnitType,
				ProfessionTypes.NO_PROFESSION,
				pLoop.getX(),
				pLoop.getY(),
				UnitAITypes.NO_UNITAI,
				DirectionTypes.DIRECTION_SOUTH,
				0
			)

			if pUnit is not None and not pUnit.isNone():
				return pUnit

	return None

def _getDistanceToNearestOwnCity(player, plot):
	if player.isNone() or plot.isNone():
		return 99

	iBest = 99
	(city, iter) = player.firstCity(True)
	while city:
		iDist = plotDistance(plot.getX(), plot.getY(), city.getX(), city.getY())
		if iDist < iBest:
			iBest = iDist
		(city, iter) = player.nextCity(iter, True)

	return iBest

def _getDiscoveryAttackedChance(player, plot):
	iDistance = _getDistanceToNearestOwnCity(player, plot)

	# Scale for large maps (Gigantic tuned)
	iScaledDistance = min(iDistance, 120) / 6

	if _isDiscoveryAttackedSoftCooldownActive(player):
		iChance = (1 + iScaledDistance / 2) * 2
		return min(3, int(iChance))

	# Normal chance
	iChance = (2 + iScaledDistance) * 2
	return min(40, int(iChance))

def canTriggerDiscoveryAttacked(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	# Only playable European players
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	# Exact unit / plot binding to prevent ghost triggers
	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return False

	# Must be land plot
	if plot.isWater():
		return False

	# No cities
	if plot.isCity():
		return False

	# Must be outside own borders
	if plot.getOwner() == player.getID():
		return False

	# Must be a valid ambush feature
	aValidFeatures = [
		gc.getInfoTypeForString("FEATURE_FOREST"),
		gc.getInfoTypeForString("FEATURE_JUNGLE"),
		gc.getInfoTypeForString("FEATURE_MANGROVE"),
		gc.getInfoTypeForString("FEATURE_TROPICAL_GROVES"),
		gc.getInfoTypeForString("FEATURE_FOREST_EVERGREEN"),
		gc.getInfoTypeForString("FEATURE_DENSE_FOREST"),
		gc.getInfoTypeForString("FEATURE_FOREST_TUNDRA"),
		gc.getInfoTypeForString("FEATURE_ANCIENT_FOREST"),
		gc.getInfoTypeForString("FEATURE_LIGHT_FOREST"),
	]

	if plot.getFeatureType() not in aValidFeatures:
		return False

	iChance = _getDiscoveryAttackedChance(player, plot)

	if CyGame().getSorenRandNum(100, "Discovery Attacked trigger") >= iChance:
		return False

	return True

def applyDiscoveryAttacked(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return

	plot = unit.plot()
	if plot is None or plot.isNone():
		return

	# Exact unit / plot binding to prevent invalid execution
	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return

	if plot.isWater():
		return

	if plot.isCity():
		return

	# Safety: must still be outside own borders
	if plot.getOwner() == player.getID():
		return

	# Spawn exactly 1 hostile native and attack immediately via DLL combat
	hostileUnit = _spawnDiscoveryAttackedHostileAdjacent(plot)
	if hostileUnit is not None and not hostileUnit.isNone():
		if hostileUnit.canMoveInto(plot, True, False, False):
			hostileUnit.attack(plot, False)

	# Start soft cooldown
	_startDiscoveryAttackedSoftCooldown(player)
  
######## Discovery Events ###########

def canTriggerDiscoveryStart(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False
	if not player.isPlayable():
		return False
	if player.isNative():
		return False

	# Only before founding the first city
	if player.getNumCities() > 0:
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	# The tracked unit must stand exactly on the triggered plot
	if unit.getX() != kTriggeredData.iPlotX or unit.getY() != kTriggeredData.iPlotY:
		return False

	# Only on land
	if plot.isWater():
		return False

	eSettler = gc.getInfoTypeForString("PROFESSION_SETTLER")
	if unit.getProfession() != eSettler:
		return False

	return True

def getHelpDiscoveryStart1(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)

	szHelp = getHelpDiscoveryConquistador(argsList)
	szHelp += u"\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_DECREASE", (-2, king.getCivilizationAdjectiveKey()))
	return szHelp

def getHelpDiscoveryStart2(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)

	szHelp = getHelpDiscoveryMissionary(argsList)

	iFatherPointType = event.getGenericParameter(3)
	iFatherPoints = event.getGenericParameter(4)

	if iFatherPointType != -1 and iFatherPoints > 0:
		szHelp += u"\n" + localText.getText(
			"TXT_KEY_EVENT_FATHER_POINTS",
			(
				iFatherPoints,
				gc.getFatherPointInfo(iFatherPointType).getChar(),
				gc.getFatherPointInfo(iFatherPointType).getDescription()
			)
		)

	return szHelp

def getHelpDiscoveryStart3(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)

	szHelp = getHelpDiscoveryTrader(argsList)
	szHelp += u"\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_INCREASE", (1, king.getCivilizationAdjectiveKey()))
	return szHelp

def getHelpDiscoveryStart4(argsList):
	return getHelpDiscoveryOxcart(argsList)

def getHelpDiscoveryStart5(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)

	return localText.getText("TXT_KEY_EVENT_RELATION_KING_INCREASE", (2, king.getCivilizationAdjectiveKey()))

def applyDiscoveryStart1(argsList):
	kTriggeredData = argsList[0]
	spawnOwnPlayerUnitInEurope(argsList)
	_changeKingRelation(argsList, -2)

def applyDiscoveryStart2(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	spawnOwnPlayerUnitInEurope(argsList)

	iFatherPointType = event.getGenericParameter(3)
	iFatherPoints = event.getGenericParameter(4)

	if iFatherPointType != -1 and iFatherPoints > 0:
		team = gc.getTeam(player.getTeam())
		team.changeFatherPoints(iFatherPointType, iFatherPoints)

def applyDiscoveryStart3(argsList):
	kTriggeredData = argsList[0]
	spawnOwnPlayerUnitInEurope(argsList)
	_changeKingRelation(argsList, 1)

def applyDiscoveryStart4(argsList):
	kTriggeredData = argsList[0]
	spawnOwnPlayerUnitInEurope(argsList)

def applyDiscoveryStart5(argsList):
	kTriggeredData = argsList[0]
	_changeKingRelation(argsList, 2)

def spawnOwnPlayerUnitInEurope(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	iUnitClass = event.getGenericParameter(1)
	iNumUnits = event.getGenericParameter(2)

	iUnitType = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationUnits(iUnitClass)
	if iUnitType == -1:
		return

	# profession mapping
	professionMap = {
		gc.getInfoTypeForString("UNITCLASS_CHRISTIAN_MISSIONARY"): gc.getInfoTypeForString("PROFESSION_MISSIONARY"),
		gc.getInfoTypeForString("UNITCLASS_SEASONED_TRADER"): gc.getInfoTypeForString("PROFESSION_NATIVE_TRADER"),
	}

	for i in range(iNumUnits):
		unit = player.initEuropeUnit(
			iUnitType,
			UnitAITypes.NO_UNITAI,
			DirectionTypes.NO_DIRECTION
		)

		if unit is None or unit.isNone():
			continue

		if iUnitClass in professionMap:
			unit.setProfession(professionMap[iUnitClass])

def _changeKingRelation(argsList, iChange):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	eKing = player.getParent()
	if eKing == -1:
		return

	king = gc.getPlayer(eKing)
	if king.isNone():
		return

	player.AI_changeAttitudeExtra(eKing, iChange)
	king.AI_changeAttitudeExtra(kTriggeredData.ePlayer, iChange)

	if player.isHuman():
		if iChange > 0:
			CyInterface().addMessage(
				kTriggeredData.ePlayer,
				False,
				10,
				localText.getText("TXT_KEY_EVENT_RELATION_KING_INCREASE", (iChange, king.getCivilizationAdjectiveKey())),
				"",
				0,
				"",
				ColorTypes(8),
				-1,
				-1,
				True,
				True
			)
		elif iChange < 0:
			CyInterface().addMessage(
				kTriggeredData.ePlayer,
				False,
				10,
				localText.getText("TXT_KEY_EVENT_RELATION_KING_DECREASE", (iChange, king.getCivilizationAdjectiveKey())),
				"",
				0,
				"",
				ColorTypes(7),
				-1,
				-1,
				True,
				True
			)

def canTriggerDiscoveryFever(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	# Only playable European players
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False

	eFeverImmune = gc.getInfoTypeForString("PROMOTION_FEVER_IMMUNE")

	# Unit already had fever and is now immune
	if unit.isHasPromotion(eFeverImmune):
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return False

	# Must be land plot
	if plot.isWater():
		return False

	# No cities
	if plot.isCity():
		return False

	# Only specific "fever" terrain features
	eFeature = plot.getFeatureType()
	if eFeature not in (
		gc.getInfoTypeForString("FEATURE_JUNGLE"),
		gc.getInfoTypeForString("FEATURE_MANGROVE"),
		gc.getInfoTypeForString("FEATURE_TROPICAL_GROVES"),
		gc.getInfoTypeForString("FEATURE_SWAMP"),
	):
		return False

	# Fixed trigger chance on valid plots
	if CyGame().getSorenRandNum(100, "Discovery Fever trigger") >= 20:
		return False

	return True

def applyDiscoveryFever(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	unit = player.getUnit(kTriggeredData.iUnitId)

	# Safety check: unit must exist
	if unit.isNone():
		return

	plot = unit.plot()

	# Safety check: unit must still stand on the originally triggered plot
	if plot is None or plot.isNone():
		return

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return
	if plot.isWater():
		return
	if plot.isCity():
		return

	eFeature = plot.getFeatureType()
	if eFeature not in (
		gc.getInfoTypeForString("FEATURE_JUNGLE"),
		gc.getInfoTypeForString("FEATURE_MANGROVE"),
		gc.getInfoTypeForString("FEATURE_TROPICAL_GROVES"),
		gc.getInfoTypeForString("FEATURE_SWAMP"),
	):
		return

	eFeverImmune = gc.getInfoTypeForString("PROMOTION_FEVER_IMMUNE")

	# Extra safety: do nothing if unit already became immune somehow
	if unit.isHasPromotion(eFeverImmune):
		return

	# Apply immobilization (2–5 turns)
	iImmobileTurns = 2 + CyGame().getSorenRandNum(4, "Discovery Fever duration")
	unit.setImmobileTimer(iImmobileTurns)

	# Apply damage (10–20%), but do not kill the unit
	iDamage = 10 + CyGame().getSorenRandNum(11, "Discovery Fever damage")
	iNewDamage = min(99, unit.getDamage() + iDamage)
	unit.setDamage(iNewDamage)

	# Mark unit as immune after surviving fever
	unit.setHasRealPromotion(eFeverImmune, True)

	# Player feedback
	if player.isHuman():
		CyInterface().addMessage(
			kTriggeredData.ePlayer,
			True,
			10,
			localText.getText(
				"TXT_KEY_EVENT_DISCOVERY_FEVER_RESULT",
				(unit.getName(), iDamage, iImmobileTurns)
			),
			"",
			0,
			"",
			ColorTypes(7),
			kTriggeredData.iPlotX,
			kTriggeredData.iPlotY,
			True,
			True
		)

def getHelpDiscoveryFever(argsList):
	return localText.getText(
		"TXT_KEY_EVENT_DISCOVERY_FEVER_HELP",
		(2, 5, 10, 20)
	)
######## Discovery Desert ###########

DISCOVERY_DESERT_SOFT_COOLDOWN_PREFIX = "[[WTP_DISCOVERY_DESERT_SOFT_READY_TURN="
DISCOVERY_DESERT_SOFT_COOLDOWN_SUFFIX = "]]"

def _getDiscoveryDesertSoftCooldownReadyTurn(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(DISCOVERY_DESERT_SOFT_COOLDOWN_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(DISCOVERY_DESERT_SOFT_COOLDOWN_PREFIX)
	iEnd = szData.find(DISCOVERY_DESERT_SOFT_COOLDOWN_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1

def _setDiscoveryDesertSoftCooldownReadyTurn(player, iReadyTurn):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(DISCOVERY_DESERT_SOFT_COOLDOWN_PREFIX)
	if iStart != -1:
		iEnd = szData.find(DISCOVERY_DESERT_SOFT_COOLDOWN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(DISCOVERY_DESERT_SOFT_COOLDOWN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szMarker = "%s%d%s" % (
		DISCOVERY_DESERT_SOFT_COOLDOWN_PREFIX,
		iReadyTurn,
		DISCOVERY_DESERT_SOFT_COOLDOWN_SUFFIX
	)

	szData += szMarker
	player.setScriptData(szData)

def _startDiscoveryDesertSoftCooldown(player):
	if player.isNone():
		return

	_setDiscoveryDesertSoftCooldownReadyTurn(player, CyGame().getGameTurn() + 30)

def _isDiscoveryDesertSoftCooldownActive(player):
	if player.isNone():
		return False

	iReadyTurn = _getDiscoveryDesertSoftCooldownReadyTurn(player)
	return iReadyTurn > CyGame().getGameTurn()

def _getDiscoveryDesertScaledTurns(iBaseTurns):
	gameSpeedType = CyGame().getGameSpeedType()
	iPercent = gc.getGameSpeedInfo(gameSpeedType).getGrowthPercent()
	return max(1, int((iBaseTurns * iPercent) / 100))

def _getDiscoveryDesertChance(player):
	# 30% normally, 2% during soft cooldown
	if _isDiscoveryDesertSoftCooldownActive(player):
		return 2

	return 30

def canTriggerDiscoveryDesert(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	# Only playable European players
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False

	# Prevent re-trigger while already affected
	if unit.getImmobileTimer() > 0:
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	# Exact unit / plot binding (ANTI-GHOST)
	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return False

	# Must be land
	if plot.isWater():
		return False

	# No cities
	if plot.isCity():
		return False

	# Must be desert
	eDesert = gc.getInfoTypeForString("TERRAIN_DESERT")
	if plot.getTerrainType() != eDesert:
		return False

	iChance = _getDiscoveryDesertChance(player)

	if CyGame().getSorenRandNum(100, "Discovery Desert trigger") >= iChance:
		return False

	return True

def applyDiscoveryDesert(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return

	plot = unit.plot()
	if plot is None or plot.isNone():
		return

	# Exact plot check (ANTI-GHOST SAFETY)
	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return

	if plot.isWater():
		return

	if plot.isCity():
		return

	eDesert = gc.getInfoTypeForString("TERRAIN_DESERT")
	if plot.getTerrainType() != eDesert:
		return

	# Immobilization: 2–4 turns (scaled)
	iBaseImmobileTurns = 2 + CyGame().getSorenRandNum(3, "Discovery Desert duration")
	iImmobileTurns = _getDiscoveryDesertScaledTurns(iBaseImmobileTurns)
	unit.setImmobileTimer(iImmobileTurns)

	# Damage: 2–5% (non-lethal)
	iDamage = 2 + CyGame().getSorenRandNum(4, "Discovery Desert damage")
	iNewDamage = min(99, unit.getDamage() + iDamage)
	unit.setDamage(iNewDamage)

	# Start soft cooldown
	_startDiscoveryDesertSoftCooldown(player)

	# Player feedback
	if player.isHuman():
		CyInterface().addMessage(
			kTriggeredData.ePlayer,
			True,
			10,
			localText.getText(
				"TXT_KEY_EVENT_DISCOVERY_DESERT_RESULT",
				(unit.getName(), iDamage, iImmobileTurns)
			),
			"",
			0,
			"",
			ColorTypes(7),
			kTriggeredData.iPlotX,
			kTriggeredData.iPlotY,
			True,
			True
		)

def getHelpDiscoveryDesert(argsList):
	return localText.getText(
		"TXT_KEY_EVENT_DISCOVERY_DESERT_HELP",
		(2, 4, 2, 5)
	)

######## Discovery Mutiny ###########

DISCOVERY_MUTINY_SOFT_COOLDOWN_PREFIX = "[[WTP_DISCOVERY_MUTINY_SOFT_READY_TURN="
DISCOVERY_MUTINY_SOFT_COOLDOWN_SUFFIX = "]]"

def _getDiscoveryMutinySoftCooldownReadyTurn(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(DISCOVERY_MUTINY_SOFT_COOLDOWN_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(DISCOVERY_MUTINY_SOFT_COOLDOWN_PREFIX)
	iEnd = szData.find(DISCOVERY_MUTINY_SOFT_COOLDOWN_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1

def _setDiscoveryMutinySoftCooldownReadyTurn(player, iReadyTurn):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(DISCOVERY_MUTINY_SOFT_COOLDOWN_PREFIX)
	if iStart != -1:
		iEnd = szData.find(DISCOVERY_MUTINY_SOFT_COOLDOWN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(DISCOVERY_MUTINY_SOFT_COOLDOWN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szMarker = "%s%d%s" % (
		DISCOVERY_MUTINY_SOFT_COOLDOWN_PREFIX,
		iReadyTurn,
		DISCOVERY_MUTINY_SOFT_COOLDOWN_SUFFIX
	)

	szData += szMarker
	player.setScriptData(szData)

def _startDiscoveryMutinySoftCooldown(player):
	if player.isNone():
		return

	iCooldownTurns = _scaleTurnsByGameSpeed(15)

	_setDiscoveryMutinySoftCooldownReadyTurn(
		player,
		CyGame().getGameTurn() + iCooldownTurns
	)

def _isDiscoveryMutinySoftCooldownActive(player):
	if player.isNone():
		return False

	iReadyTurn = _getDiscoveryMutinySoftCooldownReadyTurn(player)
	return iReadyTurn > CyGame().getGameTurn()

def _getDistanceToNearestOwnCity(player, plot):
	if player.isNone() or plot.isNone():
		return 99

	iBest = 99
	(city, iter) = player.firstCity(True)
	while city:
		iDist = plotDistance(plot.getX(), plot.getY(), city.getX(), city.getY())
		if iDist < iBest:
			iBest = iDist
		(city, iter) = player.nextCity(iter, True)

	return iBest

def _getDiscoveryMutinyChance(player, plot):
	iDistance = _getDistanceToNearestOwnCity(player, plot)

	# Scale for large maps (Gigantic tuned)
	iScaledDistance = min(iDistance, 120) / 6

	if _isDiscoveryMutinySoftCooldownActive(player):
		iChance = (1 + iScaledDistance / 4) * 2
		return min(2, int(iChance))

	# Normal chance
	iChance = (1 + iScaledDistance / 2) * 2
	return min(20, int(iChance))

def _spawnDiscoveryMutinyHostileAdjacent(plot, iHostileUnitClass):
	if plot is None or plot.isNone():
		return None

	if iHostileUnitClass == -1:
		return None

	iBarbarian = gc.getGame().getBarbarianPlayer()
	barbPlayer = gc.getPlayer(iBarbarian)
	if barbPlayer.isNone():
		return None

	barbCiv = gc.getCivilizationInfo(barbPlayer.getCivilizationType())
	iUnitType = barbCiv.getCivilizationUnits(iHostileUnitClass)
	if iUnitType == UnitTypes.NO_UNIT:
		return None

	for iDX in range(-1, 2):
		for iDY in range(-1, 2):
			if iDX == 0 and iDY == 0:
				continue

			pLoop = plotXY(plot.getX(), plot.getY(), iDX, iDY)
			if pLoop is None or pLoop.isNone():
				continue
			if pLoop.isWater():
				continue
			if pLoop.isImpassable():
				continue
			if pLoop.isPeak():
				continue
			if pLoop.isCity():
				continue
			if pLoop.isUnit():
				continue

			pUnit = barbPlayer.initUnit(
				iUnitType,
				ProfessionTypes.NO_PROFESSION,
				pLoop.getX(),
				pLoop.getY(),
				UnitAITypes.NO_UNITAI,
				DirectionTypes.DIRECTION_SOUTH,
				0
			)

			if pUnit is not None and not pUnit.isNone():
				return pUnit

	return None

def canTriggerDiscoveryMutiny(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	# Exact unit / plot binding to prevent ghost triggers
	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return False

	if plot.isWater():
		return False

	if plot.isCity():
		return False

	# Must be outside own borders
	if plot.getOwner() == player.getID():
		return False

	iChance = _getDiscoveryMutinyChance(player, plot)

	if CyGame().getSorenRandNum(100, "Discovery Mutiny Trigger") >= iChance:
		return False

	return True

def applyDiscoveryMutinyPay(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return

	plot = unit.plot()
	if plot is None or plot.isNone():
		return

	# Exact unit / plot binding to prevent invalid execution
	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return

	if plot.isWater():
		return

	if plot.isCity():
		return

	# Safety: must still be outside own borders
	if plot.getOwner() == player.getID():
		return

	# Movement bonus for this turn
	try:
		unit.changeMoves(-60 * 2)
	except:
		pass

	_startDiscoveryMutinySoftCooldown(player)

def applyDiscoveryMutinyFight(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return

	plot = unit.plot()
	if plot is None or plot.isNone():
		return

	# Exact unit / plot binding to prevent invalid execution
	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return

	if plot.isWater():
		return

	if plot.isCity():
		return

	# Safety: must still be outside own borders
	if plot.getOwner() == player.getID():
		return

	iHostileUnitClass = event.getGenericParameter(1)
	iNumHostiles = event.getGenericParameter(2)

	if iHostileUnitClass == -1:
		return

	if iNumHostiles < 1:
		iNumHostiles = 1
	if iNumHostiles > 1:
		iNumHostiles = 1

	hostileUnit = _spawnDiscoveryMutinyHostileAdjacent(plot, iHostileUnitClass)
	if hostileUnit is not None and not hostileUnit.isNone():
		if hostileUnit.canMoveInto(plot, True, False, False):
			hostileUnit.attack(plot, False)

	_startDiscoveryMutinySoftCooldown(player)

def getHelpDiscoveryMutiny(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return u""

	unit = player.getUnit(kTriggeredData.iUnitId)
	szUnitName = u""
	if unit is not None and not unit.isNone():
		szUnitName = unit.getName()

	if event.getType() == "EVENT_DISCOVERY_EVENTS_MUTINY_1":
		return localText.getText(
			"TXT_KEY_EVENT_DISCOVERY_EVENTS_MUTINY_HELP_FIGHT",
			()
		)

	return localText.getText(
		"TXT_KEY_EVENT_DISCOVERY_EVENTS_MUTINY_HELP_PAY",
		(szUnitName,)
	)

######## The DISCOVERY EVENTS BRAVE FELLOWS EVENT ###########

DISCOVERY_BRAVE_FELLOWS_SOFT_COOLDOWN_PREFIX = "[[WTP_DISCOVERY_BRAVE_FELLOWS_SOFT_READY_TURN="
DISCOVERY_BRAVE_FELLOWS_SOFT_COOLDOWN_SUFFIX = "]]"

def _getDiscoveryBraveFellowsSoftCooldownReadyTurn(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(DISCOVERY_BRAVE_FELLOWS_SOFT_COOLDOWN_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(DISCOVERY_BRAVE_FELLOWS_SOFT_COOLDOWN_PREFIX)
	iEnd = szData.find(DISCOVERY_BRAVE_FELLOWS_SOFT_COOLDOWN_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1

def _setDiscoveryBraveFellowsSoftCooldownReadyTurn(player, iReadyTurn):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(DISCOVERY_BRAVE_FELLOWS_SOFT_COOLDOWN_PREFIX)
	if iStart != -1:
		iEnd = szData.find(DISCOVERY_BRAVE_FELLOWS_SOFT_COOLDOWN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(DISCOVERY_BRAVE_FELLOWS_SOFT_COOLDOWN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szMarker = "%s%d%s" % (
		DISCOVERY_BRAVE_FELLOWS_SOFT_COOLDOWN_PREFIX,
		iReadyTurn,
		DISCOVERY_BRAVE_FELLOWS_SOFT_COOLDOWN_SUFFIX
	)

	szData += szMarker
	player.setScriptData(szData)

def _startDiscoveryBraveFellowsSoftCooldown(player):
	if player.isNone():
		return

	_setDiscoveryBraveFellowsSoftCooldownReadyTurn(player, CyGame().getGameTurn() + 30)

def _isDiscoveryBraveFellowsSoftCooldownActive(player):
	if player.isNone():
		return False

	iReadyTurn = _getDiscoveryBraveFellowsSoftCooldownReadyTurn(player)
	return iReadyTurn > CyGame().getGameTurn()

def _getDiscoveryBraveFellowsChance(player):
	if _isDiscoveryBraveFellowsSoftCooldownActive(player):
		return 2

	return 35

def _getDiscoveryBraveFellowsVillageNativePlayer(player, plot):
	if player.isNone() or plot is None or plot.isNone():
		return None

	# Prefer city owner (village)
	if plot.isCity():
		city = plot.getPlotCity()
		if city is not None and not city.isNone():
			iOwner = city.getOwner()
			if iOwner >= 0:
				nativePlayer = gc.getPlayer(iOwner)
				if not nativePlayer.isNone() and nativePlayer.isNative():
					return nativePlayer

	# Fallback: plot owner
	iOwner = plot.getOwner()
	if iOwner < 0:
		return None

	nativePlayer = gc.getPlayer(iOwner)
	if nativePlayer.isNone() or not nativePlayer.isNative():
		return None

	return nativePlayer

def _isDiscoveryBraveFellowsFriendlyVillage(player, plot):
	nativePlayer = _getDiscoveryBraveFellowsVillageNativePlayer(player, plot)
	if nativePlayer is None:
		return False

	if gc.getTeam(player.getTeam()).isAtWar(nativePlayer.getTeam()):
		return False

	# Testwise lowered by one step: pleased or better
	return nativePlayer.AI_getAttitude(player.getID()) >= AttitudeTypes.ATTITUDE_PLEASED

def _applyDiscoveryBraveFellowsVillageRelationBonus(player, plot, iBonus):
	if player.isNone() or iBonus == 0:
		return

	nativePlayer = _getDiscoveryBraveFellowsVillageNativePlayer(player, plot)
	if nativePlayer is None:
		return

	player.AI_changeAttitudeExtra(nativePlayer.getID(), iBonus)
	nativePlayer.AI_changeAttitudeExtra(player.getID(), iBonus)

def canTriggerDiscoveryBraveFellows(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return False

	if not isNativeVillage(argsList):
		return False

	if unit.getProfession() != gc.getInfoTypeForString("PROFESSION_SCOUT"):
		return False

	nativePlayer = _getDiscoveryBraveFellowsVillageNativePlayer(player, plot)
	if nativePlayer is None:
		return False

	if gc.getTeam(player.getTeam()).isAtWar(nativePlayer.getTeam()):
		return False

	if nativePlayer.AI_getAttitude(player.getID()) < AttitudeTypes.ATTITUDE_CAUTIOUS:
		return False

	iChance = _getDiscoveryBraveFellowsChance(player)
	if CyGame().getSorenRandNum(100, "Discovery Brave Fellows Trigger") >= iChance:
		return False

	return True

def applyDiscoveryBraveFellowsMap(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unit = player.getUnit(kTriggeredData.iUnitId)

	if player.isNone() or unit.isNone():
		return

	plot = unit.plot()
	if plot is None or plot.isNone():
		return

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return

	if not isNativeVillage(argsList):
		return

	iRadius = 4
	if _isDiscoveryBraveFellowsFriendlyVillage(player, plot):
		iRadius = 5

	for iX in range(-iRadius, iRadius + 1):
		for iY in range(-iRadius, iRadius + 1):
			if (iX * iX) + (iY * iY) > (iRadius * iRadius):
				continue

			pLoop = plotXY(plot.getX(), plot.getY(), iX, iY)
			if pLoop is None or pLoop.isNone():
				continue

			pLoop.setRevealed(player.getTeam(), True, False, -1)

	_applyDiscoveryBraveFellowsVillageRelationBonus(player, plot, 1)
	_startDiscoveryBraveFellowsSoftCooldown(player)

def applyDiscoveryBraveFellowsAdvice(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unit = player.getUnit(kTriggeredData.iUnitId)

	if player.isNone() or unit.isNone():
		return

	plot = unit.plot()
	if plot is None or plot.isNone():
		return

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return

	if not isNativeVillage(argsList):
		return

	if _isDiscoveryBraveFellowsFriendlyVillage(player, plot):
		unit.changeExperience(1, -1, False, False, False)

	_applyDiscoveryBraveFellowsVillageRelationBonus(player, plot, 1)
	_startDiscoveryBraveFellowsSoftCooldown(player)

def applyDiscoveryBraveFellowsWarriors(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unit = player.getUnit(kTriggeredData.iUnitId)

	if player.isNone() or unit.isNone():
		return

	plot = unit.plot()
	if plot is None or plot.isNone():
		return

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return

	if not isNativeVillage(argsList):
		return

	spawnOwnPlayerUnitOnSamePlotAsPlot(argsList)

	_applyDiscoveryBraveFellowsVillageRelationBonus(player, plot, 1)
	_startDiscoveryBraveFellowsSoftCooldown(player)

def getHelpDiscoveryBraveFellows1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	iUnitClass = event.getGenericParameter(1)

	if iUnitClass == -1:
		return u""

	UnitClass = gc.getUnitClassInfo(iUnitClass)
	return localText.getText(
		"TXT_KEY_EVENT_DISCOVERY_EVENTS_BRAVE_FELLOWS_HELP_1",
		(300, UnitClass.getTextKey(), 1)
	)

def getHelpDiscoveryBraveFellows2(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return localText.getText(
			"TXT_KEY_EVENT_DISCOVERY_EVENTS_BRAVE_FELLOWS_HELP_2",
			(200, 9, 9, 1)
		)

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return localText.getText(
			"TXT_KEY_EVENT_DISCOVERY_EVENTS_BRAVE_FELLOWS_HELP_2",
			(200, 9, 9, 1)
		)

	plot = unit.plot()
	if plot is None or plot.isNone():
		return localText.getText(
			"TXT_KEY_EVENT_DISCOVERY_EVENTS_BRAVE_FELLOWS_HELP_2",
			(200, 9, 9, 1)
		)

	if _isDiscoveryBraveFellowsFriendlyVillage(player, plot):
		return localText.getText(
			"TXT_KEY_EVENT_DISCOVERY_EVENTS_BRAVE_FELLOWS_HELP_2_FRIENDLY",
			(200, 11, 11, 1)
		)

	return localText.getText(
		"TXT_KEY_EVENT_DISCOVERY_EVENTS_BRAVE_FELLOWS_HELP_2",
		(200, 9, 9, 1)
	)

def getHelpDiscoveryBraveFellows3(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return localText.getText(
			"TXT_KEY_EVENT_DISCOVERY_EVENTS_BRAVE_FELLOWS_HELP_3",
			(1,)
		)

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return localText.getText(
			"TXT_KEY_EVENT_DISCOVERY_EVENTS_BRAVE_FELLOWS_HELP_3",
			(1,)
		)

	plot = unit.plot()
	if plot is None or plot.isNone():
		return localText.getText(
			"TXT_KEY_EVENT_DISCOVERY_EVENTS_BRAVE_FELLOWS_HELP_3",
			(1,)
		)

	if _isDiscoveryBraveFellowsFriendlyVillage(player, plot):
		return localText.getText(
			"TXT_KEY_EVENT_DISCOVERY_EVENTS_BRAVE_FELLOWS_HELP_3_FRIENDLY",
			(1,)
		)

	return localText.getText(
		"TXT_KEY_EVENT_DISCOVERY_EVENTS_BRAVE_FELLOWS_HELP_3",
		(1,)
	)

######## The Lost Tribe ###########

def canTriggerLostTribe(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False

	if unit.getUnitClassType() != gc.getInfoTypeForString("UNITCLASS_SCOUT"):
		return False

	if unit.getProfession() != gc.getInfoTypeForString("PROFESSION_SCOUT"):
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	# Hard check: exact trigger plot
	if plot.getX() != kTriggeredData.iPlotX:
		return False
	if plot.getY() != kTriggeredData.iPlotY:
		return False

	# Hard check: unit is really still on that plot stack
	if unit.at(kTriggeredData.iPlotX, kTriggeredData.iPlotY) == False:
		return False

	if plot.getFeatureType() not in (
		gc.getInfoTypeForString("FEATURE_JUNGLE"),
		gc.getInfoTypeForString("FEATURE_MANGROVE"),
		gc.getInfoTypeForString("FEATURE_TROPICAL_GROVES"),
	):
		return False

	return TriggerChance(argsList)


######## Pacific Quest ###########

def canTriggerPacificDone(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	iAchieve = gc.getInfoTypeForString("ACHIEVE_PACIFIC")
	#CyInterface().addImmediateMessage("iAchieve "+str(iAchieve), "")
	if player.isAchieveGained(iAchieve):
		return True
	return False


######## VOLCANO ###########

def canApplyVolcano1(argsList):
	iEvent = argsList[1]
	kTriggeredData = argsList[0]

	iNumImprovements = 0
	for iDX in range(-1, 2):
		for iDY in range(-1, 2):
			loopPlot = plotXY(kTriggeredData.iPlotX, kTriggeredData.iPlotY, iDX, iDY)
			if not loopPlot.isNone():
				if (iDX != 0 or iDY != 0):
					if loopPlot.getImprovementType() != -1:
						iNumImprovements += 1

	return (iNumImprovements > 0)

def applyVolcano1(argsList):
	iEvent = argsList[1]
	kTriggeredData = argsList[0]

	plot = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	plot.setPlotType(PlotTypes.PLOT_LAND, True, True)
	plot.setFeatureType(gc.getInfoTypeForString('FEATURE_VOLCANO'), 0)

	listPlots = []
	for iDX in range(-1, 2):
		for iDY in range(-1, 2):
			loopPlot = plotXY(kTriggeredData.iPlotX, kTriggeredData.iPlotY, iDX, iDY)
			if not loopPlot.isNone():
				if (iDX != 0 or iDY != 0):
					if loopPlot.getImprovementType() != -1:
						listPlots.append(loopPlot)

	listRuins = []
	listRuins.append(CvUtil.findInfoTypeNum('IMPROVEMENT_FARM'))
	listRuins.append(CvUtil.findInfoTypeNum('IMPROVEMENT_PLANTATION'))


	iRuins = CvUtil.findInfoTypeNum('IMPROVEMENT_CITY_RUINS')

	for i in range(3):
		if len(listPlots) > 0:
			plot = listPlots[gc.getGame().getSorenRandNum(len(listPlots), "Volcano event improvement destroyed")]
			iImprovement = plot.getImprovementType()
			szBuffer = localText.getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", (gc.getImprovementInfo(iImprovement).getTextKey(), ))
			CyInterface().addMessage(kTriggeredData.ePlayer, False, gc.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, gc.getImprovementInfo(iImprovement).getButton(), gc.getInfoTypeForString("COLOR_RED"), plot.getX(), plot.getY(), True, True)
			if iImprovement in listRuins:
				plot.setImprovementType(iRuins)
			else:
				plot.setImprovementType(-1)
			listPlots.remove(plot)

			if i == 1 and gc.getGame().getSorenRandNum(100, "Volcano event num improvements destroyed") < 50:
				break


######## VOLCANO DORMANT ###########

def canTriggerVolcanoDormant1(argsList):
	kTriggeredData = argsList[0]
	if kTriggeredData.getRandomNumberForIndex(0) < 250:
		return True
	return False

def applyVolcanoDormant1(argsList):
	iEvent = argsList[1]
	kTriggeredData = argsList[0]

	plot = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	plot.setPlotType(PlotTypes.PLOT_PEAK, True, True)

######## BABY BOOM ###########

def canTriggerBabyBoom(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if not player.isPlayable():
		return False
	team = gc.getTeam(player.getTeam())
	if team.getAtWarCount() > 0:
		return False
	if not TriggerChance(argsList):
		return False
	#for iLoopTeam in range(gc.getMAX_CIV_TEAMS()):
	#	if iLoopTeam != player.getTeam():
	#		if team.AI_getAtPeaceCounter(iLoopTeam) == 1:
	#			CyInterface().addImmediateMessage("True!", "")
	#			return true
	#CyInterface().addImmediateMessage("anderes", "")
	return True

def ApplyBabyBoom(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	count = 0
	# Check all Cities for growth
	(loopCity, iter) = player.firstCity(False)
	while(loopCity):
		if gc.getGame().getSorenRandNum(100, "(c) TAC 2010 Events") < event.getGenericParameter(1):
			if not loopCity.isNone():
				loopCity.setFood(loopCity.growthThreshold())
				count += 1
				# Break if the max numbe of Cities is reached
		if count > event.getGenericParameter(2):
			break
		(loopCity, iter) = player.nextCity(iter, False)
	# Wenn keine Stadt Wachstum hat, eine festlegen
	if count < 1:
		city.setFood(city.growthThreshold())

def getHelpBabyBoom(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	if event.getGenericParameter(2) <> 0 :
		szHelp = localText.getText("TXT_KEY_EVENT_BABY_BOOM_HELP", (event.getGenericParameter(2),))
	return szHelp

######## Flaute ###########

CALM_COOLDOWN_TURNS = 30
CALM_COOLDOWN_PREFIX = "[[WTP_CALM_READY_TURN="
CALM_COOLDOWN_SUFFIX = "]]"


def _getCalmCooldown(player):
	if player.isNone():
		return 0

	szData = player.getScriptData()
	if szData is None:
		return 0

	iStart = szData.find(CALM_COOLDOWN_PREFIX)
	if iStart == -1:
		return 0

	iStart += len(CALM_COOLDOWN_PREFIX)
	iEnd = szData.find(CALM_COOLDOWN_SUFFIX, iStart)

	if iEnd == -1:
		return 0

	try:
		return int(szData[iStart:iEnd])
	except:
		return 0

def _setCalmCooldown(player, iTurn):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(CALM_COOLDOWN_PREFIX)

	if iStart != -1:
		iEnd = szData.find(CALM_COOLDOWN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(CALM_COOLDOWN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szData += "%s%d%s" % (CALM_COOLDOWN_PREFIX, iTurn, CALM_COOLDOWN_SUFFIX)

	player.setScriptData(szData)

def canTriggerCalm(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	if CyGame().getGameTurn() < _getCalmCooldown(player):
		return False

	return True

def isValidUnitTravelStateForTravel(unit):
	ts = unit.getUnitTravelState()
	return (ts == UnitTravelStates.UNIT_TRAVEL_STATE_FROM_EUROPE or
			ts == UnitTravelStates.UNIT_TRAVEL_STATE_TO_EUROPE or
			ts == UnitTravelStates.UNIT_TRAVEL_STATE_FROM_AFRICA or
			ts == UnitTravelStates.UNIT_TRAVEL_STATE_TO_AFRICA or
			ts == UnitTravelStates.UNIT_TRAVEL_STATE_FROM_PORT_ROYAL or
			ts == UnitTravelStates.UNIT_TRAVEL_STATE_TO_PORT_ROYAL)

def isValidUnitTravelStateForPort(unit):
	ts = unit.getUnitTravelState()
	return (ts == UnitTravelStates.UNIT_TRAVEL_STATE_IN_EUROPE or
			ts == UnitTravelStates.UNIT_TRAVEL_STATE_IN_AFRICA or
			ts == UnitTravelStates.UNIT_TRAVEL_STATE_IN_PORT_ROYAL)

def canApplyCalm(argsList):
	eEvent = argsList[1]
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False
	if (isValidUnitTravelStateForPort(unit)):
		return False
	return True

def applyCalm(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unit = player.getUnit(kTriggeredData.iUnitId)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	turn = Speed.getStoragePercent()/100

	if unit.isNone():
		return

	if isValidUnitTravelStateForTravel(unit):
		unit.setUnitTravelTimer(unit.getUnitTravelTimer() + turn)

	iCooldown = max(1, CALM_COOLDOWN_TURNS * Speed.getGrowthPercent() / 100)
	_setCalmCooldown(player, CyGame().getGameTurn() + iCooldown)

def getHelpCalm(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unit = player.getUnit(kTriggeredData.iUnitId)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	turn = Speed.getStoragePercent()/100
	szHelp = ""
	if not unit.isNone() and isValidUnitTravelStateForTravel(unit):
		szHelp = localText.getText("TXT_KEY_EVENT_CALM_HELP", (turn, unit.getName()))
	return szHelp

######## Tailwind ###########

TAILWIND_COOLDOWN_TURNS = 30
TAILWIND_COOLDOWN_PREFIX = "[[WTP_TAILWIND_READY_TURN="
TAILWIND_COOLDOWN_SUFFIX = "]]"


def _getTailwindCooldown(player):
	if player.isNone():
		return 0

	szData = player.getScriptData()
	if szData is None:
		return 0

	iStart = szData.find(TAILWIND_COOLDOWN_PREFIX)
	if iStart == -1:
		return 0

	iStart += len(TAILWIND_COOLDOWN_PREFIX)
	iEnd = szData.find(TAILWIND_COOLDOWN_SUFFIX, iStart)

	if iEnd == -1:
		return 0

	try:
		return int(szData[iStart:iEnd])
	except:
		return 0


def _setTailwindCooldown(player, iTurn):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(TAILWIND_COOLDOWN_PREFIX)
	if iStart != -1:
		iEnd = szData.find(TAILWIND_COOLDOWN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(TAILWIND_COOLDOWN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szData += "%s%d%s" % (TAILWIND_COOLDOWN_PREFIX, iTurn, TAILWIND_COOLDOWN_SUFFIX)
	player.setScriptData(szData)


def _startTailwindCooldown(player):
	if player.isNone():
		return

	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	iCooldown = max(1, TAILWIND_COOLDOWN_TURNS * Speed.getGrowthPercent() / 100)

	_setTailwindCooldown(player, CyGame().getGameTurn() + iCooldown)


def canTriggerTailwind(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	if CyGame().getGameTurn() < _getTailwindCooldown(player):
		return False

	return True


def applyTailwind(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return

	if event.getGenericParameter(1) <= 0:
		return

	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	turn = Speed.getStoragePercent() / 100

	if isValidUnitTravelStateForTravel(unit):
		if unit.getUnitTravelTimer() > turn:
			unit.setUnitTravelTimer(unit.getUnitTravelTimer() - turn)
		else:
			unit.setUnitTravelTimer(1)
	else:
		unit.changeMoves(-60 * event.getGenericParameter(1))

	_startTailwindCooldown(player)


def canApplyTailwind(argsList):
	eEvent = argsList[1]
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	unit = player.getUnit(kTriggeredData.iUnitId)

	if unit.isNone():
		return False

	if isValidUnitTravelStateForPort(unit):
		return False

	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	turn = Speed.getStoragePercent() / 100

	if isValidUnitTravelStateForTravel(unit):
		if unit.getUnitTravelTimer() <= 1:
			return False

	return True


def getHelpTailwind(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	unit = player.getUnit(kTriggeredData.iUnitId)

	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	turn = Speed.getStoragePercent() / 100

	szHelp = ""

	if not unit.isNone():
		if event.getGenericParameter(1) > 0:
			if isValidUnitTravelStateForTravel(unit):
				szHelp = localText.getText(
					"TXT_KEY_EVENT_TAILWIND_HELP_2",
					(turn, unit.getName())
				)
			else:
				szHelp = localText.getText(
					"TXT_KEY_EVENT_TAILWIND_HELP_1",
					(event.getGenericParameter(1), unit.getName())
				)

	return szHelp

######## RUNAWAY - Runaway horses ###########

def _runAwayNativeCityBordersPlayerTerritory(city, nativecity):
	if city.isNone() or nativecity.isNone():
		return False

	iPlayer = city.getOwner()

	for iDX in range(-2, 3):
		for iDY in range(-2, 3):
			loopPlot = plotXY(nativecity.getX(), nativecity.getY(), iDX, iDY)
			if loopPlot is None or loopPlot.isNone():
				continue

			if loopPlot.getOwner() != iPlayer:
				continue

			workingCity = loopPlot.getWorkingCity()
			if workingCity is None or workingCity.isNone():
				continue

			if workingCity.getOwner() == city.getOwner() and workingCity.getID() == city.getID():
				return True

	return False
 
def canTriggerRunAway(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return False

	player2 = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if player2.isNone():
		return False

	if not player2.isNative():
		return False

	nativecity = player2.getCity(kTriggeredData.iOtherPlayerCityId)
	if nativecity.isNone():
		return False

	if not _runAwayNativeCityBordersPlayerTerritory(city, nativecity):
		return False

	# Read Parameter 1 from the first event and check if enough yield is stored in city
	eEvent1 = gc.getInfoTypeForString("EVENT_RUNAWAY_1")
	event1 = gc.getEventInfo(eEvent1)
	iYield = gc.getInfoTypeForString("YIELD_HORSES")
	quantity = event1.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	if city.getYieldStored(iYield) < -quantity*2:
		return False

	return True
 
def applyRunAway1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	player2 = gc.getPlayer(kTriggeredData.eOtherPlayer)
	nativecity = player2.getCity(kTriggeredData.iOtherPlayerCityId)
	iYield = gc.getInfoTypeForString("YIELD_HORSES")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	# Re-check the event pre-condition (the game state may have changed inbetween canTriggerRunAway
	#   and applyRunAway1)
	# Note: This check should help prevent the city from ending up with
	#   negative horses
	if city.getYieldStored(iYield) < -quantity*2 :
		return

	city.changeYieldStored(iYield, quantity)
	nativecity.changeYieldStored(iYield, -quantity)

def getHelpRunAway1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	player2 = gc.getPlayer(kTriggeredData.eOtherPlayer)
	nativecity = player2.getCity(kTriggeredData.iOtherPlayerCityId)
	iYield = gc.getInfoTypeForString("YIELD_HORSES")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	szHelp = ""
	if event.getGenericParameter(1) <> 0 :
		szHelp = localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity,  gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_GAIN", (-quantity,  gc.getYieldInfo(iYield).getChar(), nativecity.getNameKey()))
	return szHelp


def canDoRunAway3(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)

	player2 = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if player2.isNone():
		return False

	if not player2.isNative():
		return False

	iCompensation = event.getGenericParameter(2)

	# Native tribe must be able to afford compensation
	if player2.getGold() < iCompensation:
		return False

	return True

def applyRunAway3(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	player2 = gc.getPlayer(kTriggeredData.eOtherPlayer)

	if player.isNone() or player2.isNone():
		return

	city = player.getCity(kTriggeredData.iCityId)
	nativecity = player2.getCity(kTriggeredData.iOtherPlayerCityId)

	if city.isNone() or nativecity.isNone():
		return

	iYield = gc.getInfoTypeForString("YIELD_HORSES")
	iAmount = event.getGenericParameter(1)
	iCompensation = event.getGenericParameter(2)

	if city.getYieldStored(iYield) < iAmount:
		return

	if player2.getGold() < iCompensation:
		return

	city.changeYieldStored(iYield, -iAmount)
	nativecity.changeYieldStored(iYield, iAmount)

	player2.changeGold(-iCompensation)
	player.changeGold(iCompensation)

def getHelpRunAway3(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)

	player2 = gc.getPlayer(kTriggeredData.eOtherPlayer)
	nativecity = player2.getCity(kTriggeredData.iOtherPlayerCityId)

	iYield = gc.getInfoTypeForString("YIELD_HORSES")
	iCompensation = event.getGenericParameter(2)

	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	szHelp = ""

	if event.getGenericParameter(1) <> 0:
		szHelp = localText.getText(
			"TXT_KEY_EVENT_YIELD_LOOSE",
			(quantity, gc.getYieldInfo(iYield).getChar(), city.getNameKey())
		)

		szHelp += "\n" + localText.getText(
			"TXT_KEY_EVENT_YIELD_GAIN",
			(quantity, gc.getYieldInfo(iYield).getChar(), nativecity.getNameKey())
		)

	szHelp += "\n" + localText.getText(
		"TXT_KEY_EVENT_RUNAWAY_3_HELP",
		(iCompensation,)
	)

	return szHelp

######## Terra X Quest ###########

def canTriggerTerraX(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if not player.isPlayable():
		return False
	(city, iter) = player.firstCity(True)
	while(city):
		if not city.isCoastal(gc.getMIN_WATER_SIZE_FOR_OCEAN()):
			return True
		(city, iter) = player.nextCity(iter, True)
	return False

def getHelpTerraX(argsList):
	worldsize = gc.getWorldInfo(CyMap().getWorldSize())
	szHelp = localText.getText("TXT_KEY_EVENT_TERRAX_HELP", (str(3+(3*worldsize.getBuildingClassPrereqModifier()/100)),))
	return szHelp

def canTriggerTerraXDone(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if not player.isPlayable():
		return False
	inlandcity = 0
	(city, iter) = player.firstCity(True)
	while(city):
		if not city.isCoastal(gc.getMIN_WATER_SIZE_FOR_OCEAN()):
			inlandcity += 1
		(city, iter) = player.nextCity(iter, True)
	worldsize = gc.getWorldInfo(CyMap().getWorldSize())
	if inlandcity >= 3+(3*worldsize.getBuildingClassPrereqModifier()/100):
		return True
	return False

def isExpiredTerraX(argsList):
	eEvent = argsList[1]
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	worldsize = gc.getWorldInfo(CyMap().getWorldSize())
	for j in range(gc.getMAX_PLAYERS()):
		if j != kTriggeredData.ePlayer:
			otherplayer = gc.getPlayer(j)
			if (otherplayer.isAlive() and otherplayer.isPlayable()):
				inlandcity = 0
				(city, iter) = otherplayer.firstCity(True)
				while(city):
					if not city.isCoastal(gc.getMIN_WATER_SIZE_FOR_OCEAN()):
						inlandcity += 1
					(city, iter) = otherplayer.nextCity(iter, True)
				if inlandcity >= 3+(3*worldsize.getBuildingClassPrereqModifier()/100):
					return True
	return False

######## Forrest Fire ###########

def applyForestFire(argsList):
	eEvent = argsList[1]
	kTriggeredData = argsList[0]
	pPlot = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	CyEngine().triggerEffect(gc.getInfoTypeForString("EFFECT_SETTLERSMOKE"), pPlot.getPoint())

def applyForestFire4(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	pPlot = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	if pPlot is None:
		return

	# Visual effect
	CyEngine().triggerEffect(gc.getInfoTypeForString("EFFECT_CITY_BIG_BURNING_SMOKE"), pPlot.getPoint())

	# Only human players
	if not player.isHuman():
		return

	city = None
	if kTriggeredData.iCityId != -1:
		city = player.getCity(kTriggeredData.iCityId)
		if city.isNone():
			city = None

	popupInfo = CyPopupInfo()
	popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_TEXT)

	if city is not None:
		popupInfo.setText(localText.getText("TXT_KEY_EVENT_FOREST_FIRE_4", (city.getNameKey(),)))
	else:
		popupInfo.setText(localText.getText("TXT_KEY_EVENT_FOREST_FIRE_4", ()))

	popupInfo.addPopup(kTriggeredData.ePlayer)

######## Cargospace ###########

def canTriggerCargoSpace(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone() or unit.getOwner() != player.getID():
		return False

	if city.getX() == unit.getX() and city.getY() == unit.getY():
		return True

	return False

def applyCargoSpace(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unit = player.getUnit(kTriggeredData.iUnitId)
	# apply is executed in sync, so random will not cause OOS
	if gc.getGame().getSorenRandNum(100, "(c) TAC 2010 Events") < event.getGenericParameter(3):
		if event.getGenericParameter(1) >= 0:
			unit.changeFreePromotionCount(event.getGenericParameter(1), 1)
			CyInterface().addImmediateMessage(localText.getText("TXT_KEY_EVENT_CARGOSPACE_SUCCESS", (unit.getName(),)), "")
	else:
		CyInterface().addImmediateMessage(localText.getText("TXT_KEY_EVENT_CARGOSPACE_FAILED", ()), "")
	if event.getGenericParameter(2) > 0:
		unit.setImmobileTimer(event.getGenericParameter(2))

def helpCargoSpace(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unit = player.getUnit(kTriggeredData.iUnitId)
	szHelp = ""
	if event.getGenericParameter(1) >= 0:
		iCargoSlots = gc.getPromotionInfo(event.getGenericParameter(1)).getCargoChange()
		szHelp = localText.getText("TXT_KEY_EVENT_CARGOSPACE_HELP", (iCargoSlots, unit.getName(), event.getGenericParameter(3)))
	if event.getGenericParameter(2) > 0:
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_IMMOBILE_UNIT", (event.getGenericParameter(2), unit.getName()))
	return szHelp

######## Anti Pirate ###########

def canTriggerAntiPirate(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if not player.isPlayable():
		return False
	iKilledTradeships = 0
	i=0
	eEvent = gc.getEventTriggerInfo(kTriggeredData.eTrigger).getEvent(0)
	event = gc.getEventInfo(eEvent)
	if player.isInRevolution():
		return False
	for i in (UnitTypes.UNIT_CARAVEL, UnitTypes.UNIT_FLUYT, UnitTypes.UNIT_MERCHANTMAN, UnitTypes.UNIT_WHALING_BOAT, UnitTypes.UNIT_CARRACK, UnitTypes.UNIT_CARAVELA_REDONDA, UnitTypes.UNIT_WEST_INDIAMAN, UnitTypes.UNIT_BRIGANTINE):
		iKilledTradeships += CyStatistics().getPlayerNumUnitsLost(kTriggeredData.ePlayer, i)
	if iKilledTradeships >= event.getGenericParameter(1):
		(loopUnit, iter) = player.firstUnit()
		while(loopUnit):
			if loopUnit.getUnitType() in (UnitTypes.UNIT_FRIGATE, UnitTypes.UNIT_SHIP_OF_THE_LINE, UnitTypes.UNIT_COLONIAL_MAN_O_WAR):
				return False
			(loopUnit, iter) = player.nextUnit(iter)
		return True
	return False

######## RUM BLOSSOM ###########

def _rumBlossomNativeCityBordersPlayerCity(city, nativecity):
	if city.isNone() or nativecity.isNone():
		return False

	iPlayer = city.getOwner()

	for iDX in range(-2, 3):
		for iDY in range(-2, 3):
			loopPlot = plotXY(nativecity.getX(), nativecity.getY(), iDX, iDY)
			if loopPlot is None or loopPlot.isNone():
				continue

			if loopPlot.getOwner() != iPlayer:
				continue

			workingCity = loopPlot.getWorkingCity()
			if workingCity is None or workingCity.isNone():
				continue

			if workingCity.getOwner() == city.getOwner() and workingCity.getID() == city.getID():
				return True

	return False


def _getRumBlossomData(kTriggeredData):
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return (None, None, None, None)

	if not player.isPlayable():
		return (None, None, None, None)

	if player.isNative():
		return (None, None, None, None)

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return (None, None, None, None)

	player2 = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if player2.isNone():
		return (None, None, None, None)

	if not player2.isNative():
		return (None, None, None, None)

	if gc.getTeam(player.getTeam()).isAtWar(player2.getTeam()):
		return (None, None, None, None)

	nativecity = player2.getCity(kTriggeredData.iOtherPlayerCityId)
	if nativecity.isNone():
		return (None, None, None, None)

	if not _rumBlossomNativeCityBordersPlayerCity(city, nativecity):
		return (None, None, None, None)

	return (player, city, player2, nativecity)


def canTriggerRumBlossom(argsList):
	kTriggeredData = argsList[0]

	player, city, player2, nativecity = _getRumBlossomData(kTriggeredData)
	if player is None:
		return False

	eEvent1 = gc.getInfoTypeForString("EVENT_RUM_BLOSSOM_1")
	event1 = gc.getEventInfo(eEvent1)

	iYield = gc.getInfoTypeForString("YIELD_RUM")
	quantity = event1.getGenericParameter(1)

	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	if city.getYieldStored(iYield) < -quantity:
		return False

	return True


def applyRumBlossom1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player, city, player2, nativecity = _getRumBlossomData(kTriggeredData)
	if player is None:
		return

	iYield = gc.getInfoTypeForString("YIELD_RUM")
	quantity = event.getGenericParameter(1)

	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	if city.getYieldStored(iYield) < -quantity:
		return

	city.changeYieldStored(iYield, quantity)
	nativecity.changeYieldStored(iYield, -quantity)


def getHelpRumBlossom1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player, city, player2, nativecity = _getRumBlossomData(kTriggeredData)
	if player is None:
		return u""

	iYield = gc.getInfoTypeForString("YIELD_RUM")
	quantity = event.getGenericParameter(1)

	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	szHelp = ""

	if event.getGenericParameter(1) <> 0:
		szHelp = localText.getText(
			"TXT_KEY_EVENT_YIELD_LOOSE",
			(quantity, gc.getYieldInfo(iYield).getChar(), city.getNameKey())
		)
		szHelp += "\n" + localText.getText(
			"TXT_KEY_EVENT_YIELD_GAIN",
			(-quantity, gc.getYieldInfo(iYield).getChar(), nativecity.getNameKey())
		)

	return szHelp


def canApplyRumBlossom3(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player, city, player2, nativecity = _getRumBlossomData(kTriggeredData)
	if player is None:
		return False

	iYield = gc.getInfoTypeForString("YIELD_RUM")
	quantity = event.getGenericParameter(1)

	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	if city.getYieldStored(iYield) < -quantity:
		return False

	return True

######## Ruins Quest ###########

def isExpiredRuins(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	plot = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	improvementtype = gc.getInfoTypeForString("IMPROVEMENT_CITY_RUINS")
	if (plot.getOwner() != kTriggeredData.ePlayer):
		return True
	if plot.getImprovementType() != improvementtype:
		return True
	if gc.getGame().getGameTurn() >= kTriggeredData.iTurn + event.getGenericParameter(1):
		return True
	return False

def getHelpRuins(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	UnitClass = gc.getUnitClassInfo(CvUtil.findInfoTypeNum('UNITCLASS_SCOUT'))
	szHelp = localText.getText("TXT_KEY_EVENT_RUINS_HELP", (UnitClass.getTextKey(), city.getNameKey(), event.getGenericParameter(1)))
	return szHelp

def applyRuins5(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	iUnitClassType = CvUtil.findInfoTypeNum('UNITCLASS_CARRIER')
	iUnitType = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationUnits(iUnitClassType)
	if iUnitType != -1:
		player.initUnit(iUnitType, ProfessionTypes.NO_PROFESSION, kTriggeredData.iPlotX, kTriggeredData.iPlotY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH, 0)

def getHelpRuins5(argsList):
	UnitClass = gc.getUnitClassInfo(CvUtil.findInfoTypeNum('UNITCLASS_CARRIER'))
	szHelp = localText.getText("TXT_KEY_EVENT_BONUS_UNIT", (1, UnitClass.getTextKey(), ))
	return szHelp

######## Native Wagon Trade Quest with seasoned native trader ###########

NATIVE_WAGON_TRADE_YIELD = "YIELD_TRADE_GOODS"
NATIVE_WAGON_TRADE_AMOUNT = 200


def _hasNativeWagonTradeSharedBorder(player, nativeCity):
	if player.isNone() or nativeCity is None or nativeCity.isNone():
		return False

	iPlayer = player.getID()

	for iDX in range(-4, 5):
		for iDY in range(-4, 5):
			if iDX == 0 and iDY == 0:
				continue

			loopPlot = plotXY(nativeCity.getX(), nativeCity.getY(), iDX, iDY)
			if loopPlot is None or loopPlot.isNone():
				continue

			if loopPlot.getOwner() == iPlayer:
				return True

	return False


def _getNativeWagonTradeActiveKey():
	return "[[WTP_NATIVE_WAGON_ACTIVE]]"


def _getNativeWagonTradeCompletedKey():
	return "[[WTP_NATIVE_WAGON_COMPLETED]]"


def _getNativeWagonTradeFailedKey(nativePlayer):
	return "[[WTP_NATIVE_WAGON_FAILED_%d]]" % nativePlayer.getID()


def _getNativeWagonTradeTargetKey():
	return "[[WTP_NATIVE_WAGON_TARGET="


def _getNativeWagonTradeScaledAmount():
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	return max(1, NATIVE_WAGON_TRADE_AMOUNT * Speed.getStoragePercent() / 100)


def _nativeWagonTradeData(player):
	if player.isNone():
		return ""
	szData = player.getScriptData()
	if szData is None:
		return ""
	return szData


def _nativeWagonTradeHas(player, key):
	return key in _nativeWagonTradeData(player)


def _nativeWagonTradeAdd(player, key):
	szData = _nativeWagonTradeData(player)
	if key not in szData:
		player.setScriptData(szData + key)


def _nativeWagonTradeRemove(player, key):
	szData = _nativeWagonTradeData(player)
	if key in szData:
		player.setScriptData(szData.replace(key, ""))


def _getNativeWagonTradeNumber(player, key):
	szData = _nativeWagonTradeData(player)
	iStart = szData.find(key)
	if iStart == -1:
		return -1

	iStart += len(key)
	iEnd = szData.find("]]", iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1


def _setNativeWagonTradeNumber(player, key, iValue):
	szData = _nativeWagonTradeData(player)

	iStart = szData.find(key)
	if iStart != -1:
		iEnd = szData.find("]]", iStart)
		if iEnd != -1:
			szData = szData[:iStart] + szData[iEnd + 2:]

	player.setScriptData(szData + "%s%d]]" % (key, iValue))


def _removeNativeWagonTradeNumber(player, key):
	szData = _nativeWagonTradeData(player)

	iStart = szData.find(key)
	if iStart != -1:
		iEnd = szData.find("]]", iStart)
		if iEnd != -1:
			player.setScriptData(szData[:iStart] + szData[iEnd + 2:])


def _getNativeWagonTradeData(kTriggeredData, bRequireWagon):
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return (None, None, None)

	plot = CyMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	if plot is None or plot.isNone() or not plot.isCity():
		return (None, None, None)

	nativeCity = plot.getPlotCity()
	if nativeCity is None or nativeCity.isNone():
		return (None, None, None)

	nativePlayer = gc.getPlayer(nativeCity.getOwner())
	if nativePlayer.isNone() or not nativePlayer.isNative():
		return (None, None, None)

	if gc.getTeam(player.getTeam()).isAtWar(nativePlayer.getTeam()):
		return (None, None, None)

	if nativePlayer.AI_getAttitude(player.getID()) < AttitudeTypes.ATTITUDE_CAUTIOUS:
		return (None, None, None)

	if not _hasNativeWagonTradeSharedBorder(player, nativeCity):
		return (None, None, None)

	if bRequireWagon:
		iWagonClass = gc.getInfoTypeForString("UNITCLASS_WAGON_TRAIN")
		bHasWagon = False

		for i in range(plot.getNumUnits()):
			unit = plot.getUnit(i)
			if unit.isNone():
				continue

			if unit.getOwner() == player.getID() and unit.getUnitClassType() == iWagonClass:
				bHasWagon = True
				break

		if not bHasWagon:
			return (None, None, None)

	return (player, nativePlayer, nativeCity)


def canTriggerNativeWagonTrade(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _getNativeWagonTradeData(kTriggeredData, False)
	if player is None:
		return False

	if _nativeWagonTradeHas(player, _getNativeWagonTradeCompletedKey()):
		return False

	if _nativeWagonTradeHas(player, _getNativeWagonTradeActiveKey()):
		return False

	if _nativeWagonTradeHas(player, _getNativeWagonTradeFailedKey(nativePlayer)):
		return False

	if CyGame().getSorenRandNum(100, "Native Wagon Trade trigger") >= 100:
		return False

	return True


def applyNativeWagonTradeStart(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _getNativeWagonTradeData(kTriggeredData, False)
	if player is None:
		return

	iYield = gc.getInfoTypeForString(NATIVE_WAGON_TRADE_YIELD)
	iTarget = nativeCity.getYieldStored(iYield) + _getNativeWagonTradeScaledAmount()

	_nativeWagonTradeAdd(player, _getNativeWagonTradeActiveKey())
	_setNativeWagonTradeNumber(player, _getNativeWagonTradeTargetKey(), iTarget)


def isExpiredNativeWagonTrade(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	if CyGame().getGameTurn() < kTriggeredData.iTurn + event.getGenericParameter(1):
		return False

	player, nativePlayer, nativeCity = _getNativeWagonTradeData(kTriggeredData, False)

	if player is not None:
		_nativeWagonTradeAdd(player, _getNativeWagonTradeFailedKey(nativePlayer))
		_nativeWagonTradeRemove(player, _getNativeWagonTradeActiveKey())
		_removeNativeWagonTradeNumber(player, _getNativeWagonTradeTargetKey())

	return True


def getHelpNativeWagonTrade(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player, nativePlayer, nativeCity = _getNativeWagonTradeData(kTriggeredData, False)
	if player is None:
		return u""

	UnitClass = gc.getUnitClassInfo(CvUtil.findInfoTypeNum('UNITCLASS_WAGON_TRAIN'))
	iAmount = _getNativeWagonTradeScaledAmount()

	return localText.getText(
		"TXT_KEY_EVENT_NATIVE_TRADE_WAGON_HELP",
		(UnitClass.getTextKey(), nativeCity.getNameKey(), event.getGenericParameter(1), iAmount)
	)


def canTriggerNativeWagonTradeDone(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _getNativeWagonTradeData(kTriggeredData, False)
	if player is None:
		return False

	if not _nativeWagonTradeHas(player, _getNativeWagonTradeActiveKey()):
		return False

	iTarget = _getNativeWagonTradeNumber(player, _getNativeWagonTradeTargetKey())
	if iTarget < 0:
		return False

	iYield = gc.getInfoTypeForString(NATIVE_WAGON_TRADE_YIELD)
	if nativeCity.getYieldStored(iYield) < iTarget:
		return False

	return True


def applyNativeWagonTradeDone(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _getNativeWagonTradeData(kTriggeredData, False)
	if player is None:
		return

	if not _nativeWagonTradeHas(player, _getNativeWagonTradeActiveKey()):
		return

	iTarget = _getNativeWagonTradeNumber(player, _getNativeWagonTradeTargetKey())
	if iTarget < 0:
		return

	iYield = gc.getInfoTypeForString(NATIVE_WAGON_TRADE_YIELD)
	if nativeCity.getYieldStored(iYield) < iTarget:
		return

	_nativeWagonTradeRemove(player, _getNativeWagonTradeActiveKey())
	_removeNativeWagonTradeNumber(player, _getNativeWagonTradeTargetKey())
	_nativeWagonTradeAdd(player, _getNativeWagonTradeCompletedKey())


def applyNativeWagonTradeDone1(argsList):
	ChangeFatherPoints(argsList)
	applyNativeWagonTradeDone(argsList)


def getHelpNativeWagonTradeDone1(argsList):
	return getHelpChangeFatherPoints(argsList)


def getHelpNativeWagonTradeDone2(argsList):
	return u""


def getHelpNativeWagonTradeDone3(argsList):
	return u""


def applyNativeWagonTrade5(argsList):
	eEvent = argsList[1]
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	iUnitClassType = CvUtil.findInfoTypeNum('UNITCLASS_WAGON_TRAIN')
	iUnitType = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationUnits(iUnitClassType)

	if iUnitType != -1:
		player.initUnit(iUnitType, ProfessionTypes.NO_PROFESSION, kTriggeredData.iPlotX, kTriggeredData.iPlotY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH, 0)


def getHelpNativeWagonTrade5(argsList):
	UnitClass = gc.getUnitClassInfo(CvUtil.findInfoTypeNum('UNITCLASS_WAGON_TRAIN'))
	return localText.getText("TXT_KEY_EVENT_BONUS_UNIT", (1, UnitClass.getTextKey(), ))
    

######## Native Wagon Trade Quest - native expert trader in native village ###########

NATIVE_NEIGHBOR_TRADE_YIELD = "YIELD_TRADE_GOODS"
NATIVE_NEIGHBOR_TRADE_AMOUNT = 200


def _nntData(player):
	if player.isNone():
		return ""
	szData = player.getScriptData()
	if szData is None:
		return ""
	return szData


def _nntHas(player, key):
	return key in _nntData(player)


def _nntAdd(player, key):
	szData = _nntData(player)
	if key not in szData:
		player.setScriptData(szData + key)


def _nntRemove(player, key):
	szData = _nntData(player)
	if key in szData:
		player.setScriptData(szData.replace(key, ""))


def _nntKeyActive():
	return "[[WTP_NATIVE_NEIGHBOR_TRADE_ACTIVE]]"


def _nntKeyCompleted():
	return "[[WTP_NATIVE_NEIGHBOR_TRADE_COMPLETED]]"


def _nntKeyFailed(nativePlayer):
	return "[[WTP_NATIVE_NEIGHBOR_TRADE_FAILED_%d]]" % nativePlayer.getID()


def _nntKeyTarget():
	return "[[WTP_NATIVE_NEIGHBOR_TRADE_TARGET="


def _nntGetNumber(player, key):
	szData = _nntData(player)
	iStart = szData.find(key)
	if iStart == -1:
		return -1

	iStart += len(key)
	iEnd = szData.find("]]", iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1


def _nntSetNumber(player, key, iValue):
	szData = _nntData(player)

	iStart = szData.find(key)
	if iStart != -1:
		iEnd = szData.find("]]", iStart)
		if iEnd != -1:
			szData = szData[:iStart] + szData[iEnd + 2:]

	player.setScriptData(szData + "%s%d]]" % (key, iValue))


def _nntRemoveNumber(player, key):
	szData = _nntData(player)

	iStart = szData.find(key)
	if iStart != -1:
		iEnd = szData.find("]]", iStart)
		if iEnd != -1:
			player.setScriptData(szData[:iStart] + szData[iEnd + 2:])


def _nntScaledAmount():
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	return max(1, NATIVE_NEIGHBOR_TRADE_AMOUNT * Speed.getStoragePercent() / 100)


def _nntHasSharedBorder(player, nativeCity):
	if player.isNone() or nativeCity is None or nativeCity.isNone():
		return False

	iPlayer = player.getID()

	for iDX in range(-4, 5):
		for iDY in range(-4, 5):
			if iDX == 0 and iDY == 0:
				continue

			loopPlot = plotXY(nativeCity.getX(), nativeCity.getY(), iDX, iDY)
			if loopPlot is None or loopPlot.isNone():
				continue

			if loopPlot.getOwner() == iPlayer:
				return True

	return False


def _nntGetContext(kTriggeredData):
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return (None, None, None)

	plot = CyMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	if plot is None or plot.isNone() or not plot.isCity():
		return (None, None, None)

	nativeCity = plot.getPlotCity()
	if nativeCity is None or nativeCity.isNone():
		return (None, None, None)

	nativePlayer = gc.getPlayer(nativeCity.getOwner())
	if nativePlayer.isNone() or not nativePlayer.isNative():
		return (None, None, None)

	if gc.getTeam(player.getTeam()).isAtWar(nativePlayer.getTeam()):
		return (None, None, None)

	if nativePlayer.AI_getAttitude(player.getID()) < AttitudeTypes.ATTITUDE_CAUTIOUS:
		return (None, None, None)

	if not _nntHasSharedBorder(player, nativeCity):
		return (None, None, None)

	return (player, nativePlayer, nativeCity)


def canTriggerNativeNeighborTrade(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _nntGetContext(kTriggeredData)
	if player is None:
		return False

	if _nntHas(player, _nntKeyCompleted()):
		return False

	if _nntHas(player, _nntKeyActive()):
		return False

	if _nntHas(player, _nntKeyFailed(nativePlayer)):
		return False

	if CyGame().getSorenRandNum(100, "Native Neighbor Trade trigger") >= 20:
		return False

	return True


def applyNativeNeighborTradeStart(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _nntGetContext(kTriggeredData)
	if player is None:
		return

	iYield = gc.getInfoTypeForString(NATIVE_NEIGHBOR_TRADE_YIELD)
	iTarget = nativeCity.getYieldStored(iYield) + _nntScaledAmount()

	_nntAdd(player, _nntKeyActive())
	_nntSetNumber(player, _nntKeyTarget(), iTarget)


def isExpiredNativeNeighborTrade(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	if CyGame().getGameTurn() < kTriggeredData.iTurn + event.getGenericParameter(1):
		return False

	player, nativePlayer, nativeCity = _nntGetContext(kTriggeredData)

	if player is not None:
		_nntAdd(player, _nntKeyFailed(nativePlayer))
		_nntRemove(player, _nntKeyActive())
		_nntRemoveNumber(player, _nntKeyTarget())

	return True


def canTriggerNativeNeighborTradeDone(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _nntGetContext(kTriggeredData)
	if player is None:
		return False

	if not _nntHas(player, _nntKeyActive()):
		return False

	iTarget = _nntGetNumber(player, _nntKeyTarget())
	if iTarget < 0:
		return False

	iYield = gc.getInfoTypeForString(NATIVE_NEIGHBOR_TRADE_YIELD)
	if nativeCity.getYieldStored(iYield) < iTarget:
		return False

	return True


def applyNativeNeighborTradeDone(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _nntGetContext(kTriggeredData)
	if player is None:
		return

	if not _nntHas(player, _nntKeyActive()):
		return

	iTarget = _nntGetNumber(player, _nntKeyTarget())
	if iTarget < 0:
		return

	iYield = gc.getInfoTypeForString(NATIVE_NEIGHBOR_TRADE_YIELD)
	if nativeCity.getYieldStored(iYield) < iTarget:
		return

	_nntRemove(player, _nntKeyActive())
	_nntRemoveNumber(player, _nntKeyTarget())
	_nntAdd(player, _nntKeyCompleted())


def applyNativeNeighborTradeDone1(argsList):
	ChangeFatherPoints(argsList)
	applyNativeNeighborTradeDone(argsList)


def applyNativeNeighborTradeDone2(argsList):
	ChangeFatherPoints(argsList)
	applyNativeNeighborTradeDone(argsList)


def applyNativeNeighborTradeDone3(argsList):
	applyNativeNeighborTradeDone(argsList)


def getHelpNativeNeighborTrade(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player, nativePlayer, nativeCity = _nntGetContext(kTriggeredData)
	if player is None:
		return u""

	UnitClass = gc.getUnitClassInfo(CvUtil.findInfoTypeNum('UNITCLASS_WAGON_TRAIN'))
	iAmount = _nntScaledAmount()

	return localText.getText(
		"TXT_KEY_EVENT_FRIENDLY_TRADE_WITH_NATIVE_NEIGHBORS_HELP",
		(UnitClass.getTextKey(), nativeCity.getNameKey(), event.getGenericParameter(1), iAmount)
	)


def getHelpNativeNeighborTradeDone1(argsList):
	return getHelpChangeFatherPoints(argsList)


def getHelpNativeNeighborTradeDone2(argsList):
	return getHelpChangeFatherPoints(argsList)


def getHelpNativeNeighborTradeDone3(argsList):
	return u""


def applyNativeNeighborTrade5(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	iUnitClassType = CvUtil.findInfoTypeNum('UNITCLASS_EXPERT_TRADER')
	iUnitType = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationUnits(iUnitClassType)

	if iUnitType != -1:
		player.initUnit(iUnitType, ProfessionTypes.NO_PROFESSION, kTriggeredData.iPlotX, kTriggeredData.iPlotY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH, 0)


def getHelpNativeNeighborTrade5(argsList):
	UnitClass = gc.getUnitClassInfo(CvUtil.findInfoTypeNum('UNITCLASS_EXPERT_TRADER'))
	return localText.getText("TXT_KEY_EVENT_BONUS_UNIT", (1, UnitClass.getTextKey(), ))


######## Native Neighbor Trade Quest 2 - larger follow-up trade ###########

NATIVE_NEIGHBOR_TRADE_2_YIELD = "YIELD_TRADE_GOODS"
NATIVE_NEIGHBOR_TRADE_2_AMOUNT = 400


def _nnt2KeyActive():
	return "[[WTP_NATIVE_NEIGHBOR_TRADE_2_ACTIVE]]"


def _nnt2KeyCompleted():
	return "[[WTP_NATIVE_NEIGHBOR_TRADE_2_COMPLETED]]"


def _nnt2KeyFailed(nativePlayer):
	return "[[WTP_NATIVE_NEIGHBOR_TRADE_2_FAILED_%d]]" % nativePlayer.getID()


def _nnt2KeyTarget():
	return "[[WTP_NATIVE_NEIGHBOR_TRADE_2_TARGET="


def _nnt2ScaledAmount():
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	return max(1, NATIVE_NEIGHBOR_TRADE_2_AMOUNT * Speed.getStoragePercent() / 100)


def canTriggerNativeNeighborTrade2(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _nntGetContext(kTriggeredData)
	if player is None:
		return False

	if _nntHas(player, _nnt2KeyCompleted()):
		return False

	if _nntHas(player, _nnt2KeyActive()):
		return False

	if _nntHas(player, _nnt2KeyFailed(nativePlayer)):
		return False

	if CyGame().getSorenRandNum(100, "Native Neighbor Trade 2 trigger") >= 20:
		return False

	return True


def applyNativeNeighborTradeStart2(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _nntGetContext(kTriggeredData)
	if player is None:
		return

	iYield = gc.getInfoTypeForString(NATIVE_NEIGHBOR_TRADE_2_YIELD)
	iTarget = nativeCity.getYieldStored(iYield) + _nnt2ScaledAmount()

	_nntAdd(player, _nnt2KeyActive())
	_nntSetNumber(player, _nnt2KeyTarget(), iTarget)


def isExpiredNativeNeighborTrade2(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	if CyGame().getGameTurn() < kTriggeredData.iTurn + event.getGenericParameter(1):
		return False

	player, nativePlayer, nativeCity = _nntGetContext(kTriggeredData)

	if player is not None:
		_nntAdd(player, _nnt2KeyFailed(nativePlayer))
		_nntRemove(player, _nnt2KeyActive())
		_nntRemoveNumber(player, _nnt2KeyTarget())

	return True


def canTriggerNativeNeighborTradeDone2(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _nntGetContext(kTriggeredData)
	if player is None:
		return False

	if not _nntHas(player, _nnt2KeyActive()):
		return False

	iTarget = _nntGetNumber(player, _nnt2KeyTarget())
	if iTarget < 0:
		return False

	iYield = gc.getInfoTypeForString(NATIVE_NEIGHBOR_TRADE_2_YIELD)
	if nativeCity.getYieldStored(iYield) < iTarget:
		return False

	return True


def _applyNativeNeighborTradeDone2(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _nntGetContext(kTriggeredData)
	if player is None:
		return

	if not _nntHas(player, _nnt2KeyActive()):
		return

	iTarget = _nntGetNumber(player, _nnt2KeyTarget())
	if iTarget < 0:
		return

	iYield = gc.getInfoTypeForString(NATIVE_NEIGHBOR_TRADE_2_YIELD)
	if nativeCity.getYieldStored(iYield) < iTarget:
		return

	_nntRemove(player, _nnt2KeyActive())
	_nntRemoveNumber(player, _nnt2KeyTarget())
	_nntAdd(player, _nnt2KeyCompleted())


def applyNativeNeighborTradeDone2_1(argsList):
	ChangeFatherPoints(argsList)
	_applyNativeNeighborTradeDone2(argsList)


def applyNativeNeighborTradeDone2_2(argsList):
	ChangeFatherPoints(argsList)
	_applyNativeNeighborTradeDone2(argsList)


def applyNativeNeighborTradeDone2_3(argsList):
	_applyNativeNeighborTradeDone2(argsList)


def getHelpNativeNeighborTrade2(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player, nativePlayer, nativeCity = _nntGetContext(kTriggeredData)
	if player is None:
		return u""

	UnitClass = gc.getUnitClassInfo(CvUtil.findInfoTypeNum("UNITCLASS_TREK"))
	iAmount = _nnt2ScaledAmount()

	return localText.getText(
		"TXT_KEY_EVENT_FRIENDLY_TRADE_WITH_NATIVE_NEIGHBORS_HELP",
		(UnitClass.getTextKey(), nativeCity.getNameKey(), event.getGenericParameter(1), iAmount)
	)


def getHelpNativeNeighborTradeDone2_1(argsList):
	return getHelpChangeFatherPoints(argsList)


def getHelpNativeNeighborTradeDone2_2(argsList):
	return getHelpChangeFatherPoints(argsList)


def getHelpNativeNeighborTradeDone2_3(argsList):
	return u""


######### Trade With Natives Betrayal Quest ###########

TRADE_WITH_NATIVES_BETRAYAL_YIELD = "YIELD_TRADE_GOODS"
TRADE_WITH_NATIVES_BETRAYAL_AMOUNT = 400


def _twbKeyActive():
	return "[[WTP_TRADE_WITH_NATIVES_BETRAYAL_ACTIVE]]"


def _twbKeyCompleted():
	return "[[WTP_TRADE_WITH_NATIVES_BETRAYAL_COMPLETED]]"


def _twbKeyTarget():
	return "[[WTP_TRADE_WITH_NATIVES_BETRAYAL_TARGET="


def _twbKeyNativePlayer():
	return "[[WTP_TRADE_WITH_NATIVES_BETRAYAL_NATIVE_PLAYER="


def _twbScaledAmount():
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	return max(1, TRADE_WITH_NATIVES_BETRAYAL_AMOUNT * Speed.getStoragePercent() / 100)

def _twbHasSharedBorder(player, city):
	if player.isNone() or city is None or city.isNone():
		return False

	iPlayer = player.getID()

	for dx in range(-2, 3):
		for dy in range(-2, 3):
			plot = plotXY(city.getX(), city.getY(), dx, dy)
			if plot is None or plot.isNone():
				continue

			if plot.getOwner() != iPlayer:
				continue

			if plotDistance(city.getX(), city.getY(), plot.getX(), plot.getY()) <= 2:
				return True

	return False

def _twbGetContext(kTriggeredData):
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return (None, None, None)

	plot = CyMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	if plot is None or plot.isNone() or not plot.isCity():
		return (None, None, None)

	nativeCity = plot.getPlotCity()
	if nativeCity is None or nativeCity.isNone():
		return (None, None, None)

	nativePlayer = gc.getPlayer(nativeCity.getOwner())
	if nativePlayer.isNone() or not nativePlayer.isNative():
		return (None, None, None)

	iRequiredNativePlayer = _nntGetNumber(player, _twbKeyNativePlayer())
	if iRequiredNativePlayer < 0:
		return (None, None, None)

	if nativePlayer.getID() != iRequiredNativePlayer:
		return (None, None, None)

	if gc.getTeam(player.getTeam()).isAtWar(nativePlayer.getTeam()):
		return (None, None, None)

	if nativePlayer.AI_getAttitude(player.getID()) < AttitudeTypes.ATTITUDE_CAUTIOUS:
		return (None, None, None)

	if not _twbHasSharedBorder(player, nativeCity):
		return (None, None, None)

	return (player, nativePlayer, nativeCity)


def canTriggerTradeWithNativesBetrayalIntro(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	if _nntHas(player, _twbKeyCompleted()):
		return False

	if _nntHas(player, _twbKeyActive()):
		return False

	nativePlayer = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if nativePlayer.isNone() or not nativePlayer.isNative():
		return False

	if gc.getTeam(player.getTeam()).isAtWar(nativePlayer.getTeam()):
		return False

	if nativePlayer.AI_getAttitude(player.getID()) < AttitudeTypes.ATTITUDE_CAUTIOUS:
		return False

	return True


def applyTradeWithNativesBetrayalIntro(argsList):
	ChangeFatherPoints(argsList)

	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	nativePlayer = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if nativePlayer.isNone() or not nativePlayer.isNative():
		return

	_nntSetNumber(player, _twbKeyNativePlayer(), nativePlayer.getID())


def canTriggerTradeWithNativesBetrayal(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _twbGetContext(kTriggeredData)
	if player is None:
		return False

	if _nntHas(player, _twbKeyCompleted()):
		return False

	if _nntHas(player, _twbKeyActive()):
		return False

	return True


def applyTradeWithNativesBetrayalStart(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _twbGetContext(kTriggeredData)
	if player is None:
		return

	iYield = gc.getInfoTypeForString(TRADE_WITH_NATIVES_BETRAYAL_YIELD)
	iTarget = nativeCity.getYieldStored(iYield) + _twbScaledAmount()

	_nntAdd(player, _twbKeyActive())
	_nntSetNumber(player, _twbKeyTarget(), iTarget)


def isExpiredTradeWithNativesBetrayal(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	if CyGame().getGameTurn() < kTriggeredData.iTurn + event.getGenericParameter(1):
		return False

	player, nativePlayer, nativeCity = _twbGetContext(kTriggeredData)

	if player is not None:
		_nntRemove(player, _twbKeyActive())
		_nntRemoveNumber(player, _twbKeyTarget())
		_nntRemoveNumber(player, _twbKeyNativePlayer())

	return True


def canTriggerTradeWithNativesBetrayalDone(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _twbGetContext(kTriggeredData)
	if player is None:
		return False

	if not _nntHas(player, _twbKeyActive()):
		return False

	iTarget = _nntGetNumber(player, _twbKeyTarget())
	if iTarget < 0:
		return False

	iYield = gc.getInfoTypeForString(TRADE_WITH_NATIVES_BETRAYAL_YIELD)
	if nativeCity.getYieldStored(iYield) < iTarget:
		return False

	return True


def applyTradeWithNativesBetrayalDone(argsList):
	ChangeFatherPoints(argsList)

	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _twbGetContext(kTriggeredData)
	if player is None:
		return

	if not _nntHas(player, _twbKeyActive()):
		return

	iTarget = _nntGetNumber(player, _twbKeyTarget())
	if iTarget < 0:
		return

	iYield = gc.getInfoTypeForString(TRADE_WITH_NATIVES_BETRAYAL_YIELD)
	if nativeCity.getYieldStored(iYield) < iTarget:
		return

	_nntRemove(player, _twbKeyActive())
	_nntRemoveNumber(player, _twbKeyTarget())
	_nntRemoveNumber(player, _twbKeyNativePlayer())
	_nntAdd(player, _twbKeyCompleted())


def getHelpTradeWithNativesBetrayalIntro(argsList):
	szHelp = getHelpChangeFatherPoints(argsList)

	kTriggeredData = argsList[0]
	nativePlayer = gc.getPlayer(kTriggeredData.eOtherPlayer)

	if not nativePlayer.isNone():
		if len(szHelp) > 0:
			szHelp += u"\n\n"

		szHelp += localText.getText(
			"TXT_KEY_EVENT_TRADE_WITH_NATIVE_BETRAYAL_INTRO_HELP",
			(nativePlayer.getCivilizationDescription(0),)
		)

	return szHelp


def getHelpTradeWithNativesBetrayal(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player, nativePlayer, nativeCity = _twbGetContext(kTriggeredData)
	if player is None:
		return u""

	UnitClass = gc.getUnitClassInfo(CvUtil.findInfoTypeNum("UNITCLASS_TREK"))
	iAmount = _twbScaledAmount()

	return localText.getText(
		"TXT_KEY_EVENT_TRADE_WITH_NATIVE_BETRAYAL_HELP",
		(UnitClass.getTextKey(), nativeCity.getNameKey(), event.getGenericParameter(1), iAmount)
	)

# ============================================================
# PIRATES
# ============================================================

PIRATES_SOFT_COOLDOWN_PREFIX = "[[WTP_PIRATES_READY_TURN="
PIRATES_SOFT_COOLDOWN_SUFFIX = "]]"
PIRATES_FOUR_TREASURES_OVERLAP_PREFIX = "[[WTP_PIRATES_FOUR_TREASURES_OVERLAP="
PIRATES_FOUR_TREASURES_OVERLAP_SUFFIX = "]]"

def _getPiratesSoftCooldownReadyTurn(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(PIRATES_SOFT_COOLDOWN_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(PIRATES_SOFT_COOLDOWN_PREFIX)
	iEnd = szData.find(PIRATES_SOFT_COOLDOWN_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1

def _setPiratesSoftCooldownReadyTurn(player, iReadyTurn):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(PIRATES_SOFT_COOLDOWN_PREFIX)
	if iStart != -1:
		iEnd = szData.find(PIRATES_SOFT_COOLDOWN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(PIRATES_SOFT_COOLDOWN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szMarker = "%s%d%s" % (
		PIRATES_SOFT_COOLDOWN_PREFIX,
		iReadyTurn,
		PIRATES_SOFT_COOLDOWN_SUFFIX
	)

	szData += szMarker
	player.setScriptData(szData)

def _getPiratesScaledTurns(iBaseTurns):
	gameSpeedType = CyGame().getGameSpeedType()
	iPercent = gc.getGameSpeedInfo(gameSpeedType).getGrowthPercent()
	return max(1, int((iBaseTurns * iPercent) / 100))

def _startPiratesSoftCooldown(player, iBaseTurns):
	if player.isNone():
		return

	iReadyTurn = CyGame().getGameTurn() + _getPiratesScaledTurns(iBaseTurns)
	_setPiratesSoftCooldownReadyTurn(player, iReadyTurn)

def _isPiratesSoftCooldownActive(player):
	if player.isNone():
		return False

	iReadyTurn = _getPiratesSoftCooldownReadyTurn(player)
	return iReadyTurn > CyGame().getGameTurn()

def _getPiratesFourTreasuresOverlapLock(player):
	if player is None or player.isNone():
		return False

	szData = player.getScriptData()
	if szData is None or szData == "":
		return False

	iStart = szData.find(PIRATES_FOUR_TREASURES_OVERLAP_PREFIX)
	if iStart == -1:
		return False

	iStart += len(PIRATES_FOUR_TREASURES_OVERLAP_PREFIX)
	iEnd = szData.find(PIRATES_FOUR_TREASURES_OVERLAP_SUFFIX, iStart)
	if iEnd == -1:
		return False

	try:
		return int(szData[iStart:iEnd]) == 1
	except:
		return False

def _setPiratesFourTreasuresOverlapLock(player, bLocked):
	if player is None or player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(PIRATES_FOUR_TREASURES_OVERLAP_PREFIX)
	if iStart != -1:
		iEnd = szData.find(PIRATES_FOUR_TREASURES_OVERLAP_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(PIRATES_FOUR_TREASURES_OVERLAP_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	iValue = 0
	if bLocked:
		iValue = 1

	szMarker = "%s%d%s" % (
		PIRATES_FOUR_TREASURES_OVERLAP_PREFIX,
		iValue,
		PIRATES_FOUR_TREASURES_OVERLAP_SUFFIX
	)

	szData += szMarker
	player.setScriptData(szData)

def _clearPiratesFourTreasuresOverlapLock(player):
	if player is None or player.isNone():
		return

	_setPiratesFourTreasuresOverlapLock(player, False)

def _getPiratesScaledStorageAmount(iBaseAmount):
	if iBaseAmount == 0:
		return 0

	iPercent = gc.getGameSpeedInfo(CyGame().getGameSpeedType()).getStoragePercent()
	iAmount = int((abs(iBaseAmount) * iPercent) / 100)

	if iAmount <= 0:
		iAmount = 1

	if iBaseAmount < 0:
		return -iAmount

	return iAmount

def _plotHasAdjacentSeaWater(plot):
	if plot is None or plot.isNone():
		return False

	for iDirection in range(DirectionTypes.NUM_DIRECTION_TYPES):
		adjPlot = plotDirection(plot.getX(), plot.getY(), DirectionTypes(iDirection))
		if adjPlot and not adjPlot.isNone():
			if adjPlot.isWater() and not adjPlot.isLake():
				return True

	return False

def _cityHasEuropeAccess(city):
	if city.isNone():
		return False

	if city.isCoastal(gc.getMIN_WATER_SIZE_FOR_OCEAN()):
		return True

	if _plotHasAdjacentSeaWater(city.plot()):
		return True

	return False

def _countPiratesTreasureCities(player):
	if player is None or player.isNone():
		return 0

	eTreasureClass = gc.getInfoTypeForString("UNITCLASS_TREASURE")
	iCityCount = 0

	(city, iter) = player.firstCity(True)
	while city:
		if not city.isNone():
			plot = city.plot()
			if plot is not None and not plot.isNone():
				bHasTreasure = False

				for i in range(plot.getNumUnits()):
					unit = plot.getUnit(i)
					if unit is None or unit.isNone():
						continue
					if unit.getOwner() != player.getID():
						continue

					eUnitClass = gc.getUnitInfo(unit.getUnitType()).getUnitClassType()
					if eUnitClass == eTreasureClass:
						bHasTreasure = True
						break

				if bHasTreasure:
					iCityCount += 1

		(city, iter) = player.nextCity(iter, True)

	return iCityCount

def _hasPiratesRequiredTreasureCities(player):
	return _countPiratesTreasureCities(player) >= 3

def _getPiratesValidatedTriggerData(kTriggeredData, bRequireTreasureOnCityPlot):
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return (None, None, None)

	if not player.isPlayable():
		return (None, None, None)

	if player.isNative():
		return (None, None, None)

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return (None, None, None)

	if city.getOwner() != kTriggeredData.ePlayer:
		return (None, None, None)

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return (None, None, None)

	iTreasureClass = gc.getInfoTypeForString("UNITCLASS_TREASURE")
	if unit.getUnitClassType() != iTreasureClass:
		return (None, None, None)

	plot = unit.plot()
	if plot is None or plot.isNone():
		return (None, None, None)

	if bRequireTreasureOnCityPlot:
		if city.getX() != unit.getX() or city.getY() != unit.getY():
			return (None, None, None)

		if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
			return (None, None, None)

	return (player, city, unit)

def canTriggerPirates(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	if _isPiratesSoftCooldownActive(player):
		return False

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return False

	if city.getOwner() != kTriggeredData.ePlayer:
		return False

	if not _cityHasEuropeAccess(city):
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False

	iTreasureClass = gc.getInfoTypeForString("UNITCLASS_TREASURE")
	if unit.getUnitClassType() != iTreasureClass:
		return False

	if city.getX() != unit.getX() or city.getY() != unit.getY():
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return False

	# Require treasure in at least 3 different own cities
	if not _hasPiratesRequiredTreasureCities(player):
		return False

	# One-time overlap lock:
	# If Four Treasures is currently valid too, block Pirates only once per overlap phase.
	bFourTreasuresValidNow = canTriggerFourTreasures(argsList)
	bOverlapAlreadyLocked = _getPiratesFourTreasuresOverlapLock(player)

	if bFourTreasuresValidNow:
		if not bOverlapAlreadyLocked:
			_setPiratesFourTreasuresOverlapLock(player, True)
			return False
	else:
		if bOverlapAlreadyLocked:
			_clearPiratesFourTreasuresOverlapLock(player)

	# Start cooldown on trigger intentionally.
	# This event has a delayed branch and the player must have time to move the treasure away
	# without the trigger reappearing immediately every turn.
	_startPiratesSoftCooldown(player, 25)
	return True

def getHelpPirates1(argsList):
	return localText.getText("TXT_KEY_EVENT_PIRATES_1_HELP", ())

def CanDoPirates3(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)

	player, city, unit = _getPiratesValidatedTriggerData(kTriggeredData, True)
	if player is None:
		return False

	iYield = gc.getInfoTypeForString("YIELD_BLADES")
	iAmount = _getPiratesScaledStorageAmount(event.getGenericParameter(1))

	if iAmount >= 0:
		return True

	if city.getYieldStored(iYield) < -iAmount:
		return False

	return True

def CanDoPirates4(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)

	player, city, unit = _getPiratesValidatedTriggerData(kTriggeredData, True)
	if player is None:
		return False

	iYield = gc.getInfoTypeForString("YIELD_MUSKETS")
	iAmount = _getPiratesScaledStorageAmount(event.getGenericParameter(1))

	if iAmount >= 0:
		return True

	if city.getYieldStored(iYield) < -iAmount:
		return False

	return True

def applyPirates3(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)

	player, city, unit = _getPiratesValidatedTriggerData(kTriggeredData, True)
	if player is None:
		return

	iYield = gc.getInfoTypeForString("YIELD_BLADES")
	iAmount = _getPiratesScaledStorageAmount(event.getGenericParameter(1))

	if iAmount < 0 and city.getYieldStored(iYield) < -iAmount:
		return

	if iAmount != 0:
		city.changeYieldStored(iYield, iAmount)

def applyPirates4(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)

	player, city, unit = _getPiratesValidatedTriggerData(kTriggeredData, True)
	if player is None:
		return

	iYield = gc.getInfoTypeForString("YIELD_MUSKETS")
	iAmount = _getPiratesScaledStorageAmount(event.getGenericParameter(1))

	if iAmount < 0 and city.getYieldStored(iYield) < -iAmount:
		return

	if iAmount != 0:
		city.changeYieldStored(iYield, iAmount)

def getHelpPirates3(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)

	player, city, unit = _getPiratesValidatedTriggerData(kTriggeredData, True)
	if player is None:
		return u""

	iYield = gc.getInfoTypeForString("YIELD_BLADES")
	iAmount = _getPiratesScaledStorageAmount(event.getGenericParameter(1))

	szHelp = u""
	if iAmount != 0:
		szHelp = localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (iAmount, gc.getYieldInfo(iYield).getChar(), city.getNameKey()))

	return szHelp

def getHelpPirates4(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)

	player, city, unit = _getPiratesValidatedTriggerData(kTriggeredData, True)
	if player is None:
		return u""

	iYield = gc.getInfoTypeForString("YIELD_MUSKETS")
	iAmount = _getPiratesScaledStorageAmount(event.getGenericParameter(1))

	szHelp = u""
	if iAmount != 0:
		szHelp = localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (iAmount, gc.getYieldInfo(iYield).getChar(), city.getNameKey()))

	return szHelp

def isExpiredPirates1a(argsList):
	kTriggeredData = argsList[0]

	player, city, unit = _getPiratesValidatedTriggerData(kTriggeredData, False)
	if player is None:
		return True

	return False

def applyPirates1a(argsList):
	kTriggeredData = argsList[0]

	player, city, unit = _getPiratesValidatedTriggerData(kTriggeredData, False)
	if player is None:
		return

	if city.getX() == unit.getX() and city.getY() == unit.getY():
		iX = city.getX()
		iY = city.getY()
		szCityName = city.getName()

		unit.kill(False)

		if player.isHuman():
			CyInterface().addMessage(
				kTriggeredData.ePlayer,
				False,
				gc.getEVENT_MESSAGE_TIME(),
				CyTranslator().changeTextColor(
					localText.getText("TXT_KEY_EVENT_PIRATES_1A_STOLEN", (szCityName,)),
					ColorTypes(7)
				),
				None,
				InterfaceMessageTypes.MESSAGE_TYPE_MINOR_EVENT,
				None,
				ColorTypes(7),
				iX,
				iY,
				True,
				True
			)
		return

	iPirateCutterClass = gc.getInfoTypeForString("UNITCLASS_PIRATE_CUTTER")
	if iPirateCutterClass == -1:
		return

	city.spawnBarbarianUnitOnAdjacentPlotOfCity(iPirateCutterClass)

	if player.isHuman():
		CyInterface().addMessage(
			kTriggeredData.ePlayer,
			False,
			gc.getEVENT_MESSAGE_TIME(),
			CyTranslator().changeTextColor(
				localText.getText("TXT_KEY_EVENT_PIRATES_1A_CUTTER", (city.getName(),)),
				ColorTypes(7)
			),
			None,
			InterfaceMessageTypes.MESSAGE_TYPE_MINOR_EVENT,
			None,
			ColorTypes(7),
			city.getX(),
			city.getY(),
			True,
			True
		)

        
######## Superstitious Pirates Event ###########

def canTriggerSupersitiousPirates(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone() or unit.getOwner() != player.getID():
		return False

	if unit.getX() != city.getX() or unit.getY() != city.getY():
		return False

	if unit.getUnitClassType() not in (
		gc.getInfoTypeForString("UNITCLASS_PRIVATEER"),
		gc.getInfoTypeForString("UNITCLASS_PIRATE_CUTTER"),
		gc.getInfoTypeForString("UNITCLASS_PIRATE_FRIGATE"),
	):
		return False

	return True

def canDoSupersitiousPirates2(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return False

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return False

	iYield = gc.getInfoTypeForString("YIELD_RUM")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent() / 100

	if city.getYieldStored(iYield) < -quantity:
		return False

	return True

def applySupersitiousPirates2(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return

	iYield = gc.getInfoTypeForString("YIELD_RUM")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent() / 100

	if city.getYieldStored(iYield) < -quantity:
		return

	city.changeYieldStored(iYield, quantity)

def getHelpSupersitiousPirates2(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return u""

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return u""

	iYield = gc.getInfoTypeForString("YIELD_RUM")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent() / 100

	if quantity != 0:
		return localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity, gc.getYieldInfo(iYield).getChar(), city.getNameKey()))

	return u""

def applySupersitiousPirates3(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return

	# Visual fire effect
	CyEngine().triggerEffect(
		gc.getInfoTypeForString("EFFECT_CITY_BIG_BURNING_SMOKE"),
		city.plot().getPoint()
	)

####### TAC Events - General Functions########

######## Units Funktionen ###########

def CreateTreasure(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	iUnitClassType = CvUtil.findInfoTypeNum('UNITCLASS_TREASURE')
	iTreasure = event.getGenericParameter(1) + gc.getGame().getSorenRandNum(event.getGenericParameter(2), "Ronnar")
	iUnitType = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationUnits(iUnitClassType)
	if iUnitType != -1:
		player.initUnit(iUnitType, 0, kTriggeredData.iPlotX, kTriggeredData.iPlotY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH, iTreasure)

def getHelpCreateTreasure(argsList):
	UnitClass = gc.getUnitClassInfo(CvUtil.findInfoTypeNum('UNITCLASS_TREASURE'))
	szHelp = localText.getText("TXT_KEY_EVENT_BONUS_UNIT", (1, UnitClass.getTextKey(), ))
	return szHelp

def countUnits(argsList, iUnitType):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	iUnitsCurrent = 0
	(loopUnit, iter) = player.firstUnit()
	while(loopUnit):
		if iUnitType == loopUnit.getUnitType():
			iUnitsCurrent += 1
		(loopUnit, iter) = player.nextUnit(iter)

	for i in range(player.getNumEuropeUnits()):
		loopUnit = player.getEuropeUnit(i)
		if iUnitType == loopUnit.getUnitType():
			iUnitsCurrent += 1

	(city, iter) = player.firstCity(True)
	while(city):
		for iCitizen in range(city.getPopulation()):
			Unit = city.getPopulationUnitByIndex(iCitizen)
			if iUnitType == Unit.getUnitType():
				iUnitsCurrent += 1
		(city, iter) = player.nextCity(iter, True)
	return iUnitsCurrent

def CheckCarpenter(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if not player.isPlayable():
		return False

	iUnitType = CvUtil.findInfoTypeNum('UNIT_CARPENTER')
	iUnitsCurrent = countUnits(argsList, iUnitType)
	if iUnitsCurrent > 0:
		return True
	return False


######## Helper Method to count Units in all Cities ###########
######## This can also be used for PLOT TRIGGER or UNIT TRIGGER ###########

def countUnitsColonies(argsList, iUnitType):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	iUnitsCurrent = 0
	(city, iter) = player.firstCity(True)
	while(city):
		for iCitizen in range(city.getPopulation()):
			Unit = city.getPopulationUnitByIndex(iCitizen)
			if iUnitType == Unit.getUnitType():
				iUnitsCurrent += 1
		(city, iter) = player.nextCity(iter, True)
	return iUnitsCurrent

######## Helper Method to count Units in specific City ###########
######## This requires a CITY TRIGGER ###########
def countUnitsInCityForCityTrigger(argsList, iUnitType):
	ePlayer = argsList[1]
	player = gc.getPlayer(ePlayer)
	iCity = argsList[2]
	city = player.getCity(iCity)

	iUnitsCurrent = 0
	for iCitizen in range(city.getPopulation()):
		Unit = city.getPopulationUnitByIndex(iCitizen)
		if iUnitType == Unit.getUnitType():
			iUnitsCurrent += 1

	return iUnitsCurrent

###### Cheese Maker Event ######

def CheckCheesemakerInCity(argsList):
	ePlayer = argsList[1]
	player = gc.getPlayer(ePlayer)

	if player.isNone():
		return False

	if not player.isPlayable() or player.isNative():
		return False

	iUnitType = CvUtil.findInfoTypeNum('UNIT_CHEESE_MAKER')
	if iUnitType == -1:
		return False

	iUnitsCurrent = countUnitsInCityForCityTrigger(argsList, iUnitType)

	if iUnitsCurrent <= 0:
		return False

	return True

######## Bonus Funktionen ###########

def CanApplyBonus(argsList):
	eEvent = argsList[1]
	kTriggeredData = argsList[0]
	event = gc.getEventInfo(eEvent)
	plot = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	bonustype = event.getGenericParameter(1)
	# CyInterface().addImmediateMessage(str(kTriggeredData.iPlotX) + ", " + str(kTriggeredData.iPlotY), "")
	if plot.isNone():
		return False
	if not plot.canHaveBonus(bonustype, False):
		return False
	#if not plot.isBeingWorked():
	#	return False
	return True

def CanApplyBonusOcean(argsList):
	eEvent = argsList[1]
	kTriggeredData = argsList[0]
	event = gc.getEventInfo(eEvent)
	plot = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	bonustype = event.getGenericParameter(1)
	# CyInterface().addImmediateMessage(str(kTriggeredData.iPlotX) + ", " + str(kTriggeredData.iPlotY), "")
	if plot.isNone():
		return False
	if not plot.canHaveBonus(bonustype, False):
		return False
	return True

def SetBonus(argsList):
	eEvent = argsList[1]
	kTriggeredData = argsList[0]
	event = gc.getEventInfo(eEvent)
	plot = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	bonustype = event.getGenericParameter(1)
	if not plot.isNone() and plot.canHaveBonus(bonustype, False):
		plot.setBonusType(bonustype)

def getHelpBonus(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	bonustype = event.getGenericParameter(1)
	szHelp = ""
	if bonustype != -1 :
		szHelp = localText.getText("TXT_KEY_EVENT_BONUS_HELP", (gc.getBonusInfo(bonustype).getTextKey(),))
	return szHelp

######## Landmark Funktionen ###########

def CheckLandmark(argsList):
	eEvent = argsList[1]
	kTriggeredData = argsList[0]
	event = gc.getEventInfo(eEvent)
	szLandmark = "TXT_KEY_%s_LANDMARK"%(event.getType())
	for i in range (CyEngine().getNumSigns()):
		Sign = CyEngine().getSignByIndex(i)
		if (Sign.getPlot().getX() == kTriggeredData.iPlotX and Sign.getPlot().getY() == kTriggeredData.iPlotY):
			return False
	return True

def SetLandmark(argsList):
	eEvent = argsList[1]
	kTriggeredData = argsList[0]
	event = gc.getEventInfo(eEvent)
	if GlobalDefines.SHOW_LANDMARKS == 1:
		szLandmark = "TXT_KEY_%s_LANDMARK"%(event.getType())
		plot = gc.getMap().plot(kTriggeredData.iPlotX,  kTriggeredData.iPlotY)
		CyEngine().addSign(plot, -1, szLandmark)

######## Founding Father Functions ###########

def ChangeFatherPoints(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	team = gc.getTeam(player.getTeam())
	FatherPointChange = 0
	if event.getGenericParameter(1)!=-1:
		Handicap = gc.getHandicapInfo(CyGame().getHandicapType())
		Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
		FatherPointChange = event.getGenericParameter(2)*Speed.getFatherPercent()/100*Handicap.getFatherPercent()/100
		team.changeFatherPoints(event.getGenericParameter(1), FatherPointChange)

def getHelpChangeFatherPoints(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	FatherPointChange = 0
	szHelp = ""
	if event.getGenericParameter(1)!=-1:
		Handicap = gc.getHandicapInfo(CyGame().getHandicapType())
		Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
		FatherPointChange = event.getGenericParameter(2)*Speed.getFatherPercent()/100*Handicap.getFatherPercent()/100
		szHelp = localText.getText("TXT_KEY_EVENT_FATHER_POINTS", (FatherPointChange, gc.getFatherPointInfo(event.getGenericParameter(1)).getChar(), gc.getFatherPointInfo(event.getGenericParameter(1)).getDescription()))
	return szHelp

######## Trigger Funktionen ###########

def hasAllBuildings(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	trigger = gc.getEventTriggerInfo(kTriggeredData.eTrigger)
	iNumTriggerBuildings = trigger.getNumBuildingsRequired()
	if city.isNone() or iNumTriggerBuildings<=0:
		return False
	bHasAllBuildings = True
	i = 0
	for i in range(iNumTriggerBuildings):
		iBuilding = trigger.getBuildingRequired(i)
		eBuilding = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationBuildings(iBuilding)
		#CyInterface().addImmediateMessage("iBuilding "+str(iBuilding) + "eBuilding "+str(eBuilding)+str(city.isHasBuilding(eBuilding)) , "")
		if not city.isHasBuilding(eBuilding):
			bHasAllBuildings = False
	return bHasAllBuildings

def has_plot_this_bonus(*bonus_strings):

	def bonus_check(argsList):
		pTriggeredData = argsList[0]
		player = gc.getPlayer(pTriggeredData.ePlayer)
		if not player.isPlayable():
			return False
		plot = gc.getMap().plot(pTriggeredData.iPlotX, pTriggeredData.iPlotY)
		bonustypes =[ gc.getInfoTypeForString(bonus_string) for bonus_string in bonus_strings ]
		if plot.getOwner() != pTriggeredData.ePlayer:
			return False
		if plot.getBonusType() in bonustypes:
			return True
		return False

	return bonus_check

hasSilverBonus = has_plot_this_bonus("BONUS_SILVER")
hasGoldBonus = has_plot_this_bonus("BONUS_GOLD")
hasFurBonus = has_plot_this_bonus("BONUS_FUR")
hasCottonBonus = has_plot_this_bonus("BONUS_COTTON")
hasSugarBonus = has_plot_this_bonus("BONUS_SUGAR")
hasTobaccoBonus = has_plot_this_bonus("BONUS_TOBACCO")
hasIronBonus = has_plot_this_bonus("BONUS_IRON")
hasCocoaBonus = has_plot_this_bonus("BONUS_COCOA")
hasMineralBonus = has_plot_this_bonus("BONUS_MINERALS")
hasTimberBonus = has_plot_this_bonus("BONUS_TIMBER")
# 2023-11-xx : please put all relevant bonus
hasFoodBonus = has_plot_this_bonus("BONUS_POTATO","BONUS_BEANS","BONUS_CORN","BONUS_IGUANA","BONUS_TOMATO","BONUS_PUMPKIN","BONUS_SQUASH","BONUS_MUSHROOMS")
hasSeaFoodBonus = has_plot_this_bonus("BONUS_PEARLS","BONUS_CRAB","BONUS_FISH")
hasPumpkinBonus = has_plot_this_bonus("BONUS_PUMPKIN")
hasTurkeyBonus = has_plot_this_bonus("BONUS_TURKEYS")
hasGiantTreeBonus = has_plot_this_bonus("BONUS_GIANT_TREE")

######## HALLOWEEN Event Start ###########

def _playerHasPumpkinBonus(player):
	if player.isNone():
		return False
	if not player.isPlayable():
		return False

	iPumpkinBonus = gc.getInfoTypeForString("BONUS_PUMPKIN")
	map = gc.getMap()

	for iPlot in range(map.numPlots()):
		plot = map.plotByIndex(iPlot)
		if plot is None:
			continue
		if plot.isNone():
			continue
		if plot.getOwner() != player.getID():
			continue
		if plot.getBonusType() == iPumpkinBonus:
			return True

	return False


def _isHalloweenSeasonNow():
	game = CyGame()
	gameSpeed = gc.getGameSpeedInfo(game.getGameSpeedType())
	iMonthIncrement = gameSpeed.getGameTurnInfo(0).iMonthIncrement
	iCurrentTurn = game.getGameTurn()
	szDate = CyGameTextMgr().getTimeStr(iCurrentTurn + 1, True)

	October = localText.getText("TXT_KEY_MONTH_OCTOBER", ())

	# Month-based speeds: Halloween only in October
	if iMonthIncrement < 6:
		if October in szDate:
			return True
		return False

	# Half-year speeds: Halloween only in the second half of the year
	if iMonthIncrement == 6:
		if October in szDate:
			return True

		# Fallback if month names are not clearly exposed in the date string
		if (iCurrentTurn % 2) == 1:
			return True

		return False

	# Year-based speeds: Halloween can occur once per year
	return True


def _getHalloweenTriggerCity(kTriggeredData, player):
	if player.isNone():
		return None

	plot = CyMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	if plot is None:
		return None
	if plot.isNone():
		return None

	city = plot.getWorkingCity()
	if city is None:
		return None
	if city.isNone():
		return None
	if city.getOwner() != player.getID():
		return None

	return city


def canTriggerHalloween(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False
	if not player.isPlayable():
		return False
	if player.isNative():
		return False

	# Keep the pumpkin requirement, but player-wide instead of plot-only
	if not _playerHasPumpkinBonus(player):
		return False

	if not _isHalloweenSeasonNow():
		return False

	# The settlement connected to the triggering farm plot must have enough food
	city = _getHalloweenTriggerCity(kTriggeredData, player)
	if city is None:
		return False

	iFood = gc.getInfoTypeForString("YIELD_FOOD")
	if city.getYieldStored(iFood) < 50:
		return False

	return True
    
######## HALLOWEEN Event End ###########

######## THANKSGIVING Event Start ###########

def _playerHasTurkeyBonus(player):
	if player.isNone():
		return False
	if not player.isPlayable():
		return False

	iTurkeyBonus = gc.getInfoTypeForString("BONUS_TURKEYS")
	map = gc.getMap()

	for iPlot in range(map.numPlots()):
		plot = map.plotByIndex(iPlot)
		if plot is None:
			continue
		if plot.isNone():
			continue
		if plot.getOwner() != player.getID():
			continue
		if plot.getBonusType() == iTurkeyBonus:
			return True

	return False


def _isThanksgivingSeasonNow():
	game = CyGame()
	gameSpeed = gc.getGameSpeedInfo(game.getGameSpeedType())
	iMonthIncrement = gameSpeed.getGameTurnInfo(0).iMonthIncrement
	iCurrentTurn = game.getGameTurn()
	szDate = CyGameTextMgr().getTimeStr(iCurrentTurn + 1, True)

	November = localText.getText("TXT_KEY_MONTH_NOVEMBER", ())

	# Month-based speeds: Thanksgiving only in November
	if iMonthIncrement < 6:
		if November in szDate:
			return True
		return False

	# Half-year speeds: Thanksgiving only in the second half of the year
	if iMonthIncrement == 6:
		if November in szDate:
			return True

		# Fallback if month names are not clearly exposed in the date string
		if (iCurrentTurn % 2) == 1:
			return True

		return False

	# Year-based speeds: Thanksgiving can occur once per year
	return True


def _getThanksgivingTriggerCity(kTriggeredData, player):
	if player.isNone():
		return None

	plot = CyMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	if plot is None:
		return None
	if plot.isNone():
		return None

	city = plot.getWorkingCity()
	if city is None:
		return None
	if city.isNone():
		return None
	if city.getOwner() != player.getID():
		return None

	return city


def canTriggerThanksgiving(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False
	if not player.isPlayable():
		return False
	if player.isNative():
		return False

	# Keep the original turkey requirement, but player-wide instead of plot-only
	if not _playerHasTurkeyBonus(player):
		return False

	if not _isThanksgivingSeasonNow():
		return False

	# The settlement connected to the triggering farm plot must have enough food
	city = _getThanksgivingTriggerCity(kTriggeredData, player)
	if city is None:
		return False

	iFood = gc.getInfoTypeForString("YIELD_FOOD")
	if city.getYieldStored(iFood) < 100:
		return False

	return True
######## THANKSGIVING Event End ###########

def hasNoBonus(argsList):
	pTriggeredData = argsList[0]
	player = gc.getPlayer(pTriggeredData.ePlayer)
	if not player.isPlayable():
		return False
	plot = gc.getMap().plot(pTriggeredData.iPlotX, pTriggeredData.iPlotY)
	#if (plot.getOwner() != pTriggeredData.ePlayer):
	#	return False
	if (plot.getBonusType() == -1):
		return True
	return False

def hasNoBonusAndIsPlayable(argsList):
	pTriggeredData = argsList[0]
	player = gc.getPlayer(pTriggeredData.ePlayer)
	if not player.isPlayable():
		return False
	plot = gc.getMap().plot(pTriggeredData.iPlotX, pTriggeredData.iPlotY)
	if (plot.getOwner() != pTriggeredData.ePlayer):
		return False
	if (plot.getBonusType() == -1):
		return True
	return False

def isPlayable(argsList):
	pTriggeredData = argsList[0]
	player = gc.getPlayer(pTriggeredData.ePlayer)
	if player.isPlayable():
		return True
	else:
		return False

def isHuman(argsList):
	pTriggeredData = argsList[0]
	player = gc.getPlayer(pTriggeredData.ePlayer)
	if player.isHuman():
		return True
	else:
		return False

def TriggerChance(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if not player.isPlayable():
		return False
	# Read parameter 3 from the first event as random chance
	eventtrigger = gc.getEventTriggerInfo(kTriggeredData.eTrigger)
	eEvent = eventtrigger.getEvent(0)
	event = gc.getEventInfo(eEvent)
	if kTriggeredData.getRandomNumberForIndex(0) < event.getGenericParameter(3):
		return True
	return False


######## Prisoner of War at Monastery ###########

PRISONER_OF_WAR_AT_MONASTERY_SOFT_COOLDOWN_PREFIX = "[[WTP_PRISONER_OF_WAR_AT_MONASTERY_SOFT_READY_TURN="
PRISONER_OF_WAR_AT_MONASTERY_SOFT_COOLDOWN_SUFFIX = "]]"

def _getPrisonerOfWarAtMonasterySoftCooldownReadyTurn(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(PRISONER_OF_WAR_AT_MONASTERY_SOFT_COOLDOWN_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(PRISONER_OF_WAR_AT_MONASTERY_SOFT_COOLDOWN_PREFIX)
	iEnd = szData.find(PRISONER_OF_WAR_AT_MONASTERY_SOFT_COOLDOWN_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1


def _setPrisonerOfWarAtMonasterySoftCooldownReadyTurn(player, iReadyTurn):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(PRISONER_OF_WAR_AT_MONASTERY_SOFT_COOLDOWN_PREFIX)
	if iStart != -1:
		iEnd = szData.find(PRISONER_OF_WAR_AT_MONASTERY_SOFT_COOLDOWN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(PRISONER_OF_WAR_AT_MONASTERY_SOFT_COOLDOWN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szMarker = "%s%d%s" % (
		PRISONER_OF_WAR_AT_MONASTERY_SOFT_COOLDOWN_PREFIX,
		iReadyTurn,
		PRISONER_OF_WAR_AT_MONASTERY_SOFT_COOLDOWN_SUFFIX
	)

	szData += szMarker
	player.setScriptData(szData)


def _startPrisonerOfWarAtMonasterySoftCooldown(player):
	if player.isNone():
		return

	_setPrisonerOfWarAtMonasterySoftCooldownReadyTurn(
		player,
		CyGame().getGameTurn() + _scaleTurnsByGameSpeed(15)
	)


def _isPrisonerOfWarAtMonasterySoftCooldownActive(player):
	if player.isNone():
		return False

	iReadyTurn = _getPrisonerOfWarAtMonasterySoftCooldownReadyTurn(player)
	return iReadyTurn > CyGame().getGameTurn()


def canTriggerPrisonerOfWarAtMonastery(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	if _isPrisonerOfWarAtMonasterySoftCooldownActive(player):
		return False

	return True


def applyPrisonerOfWarAtMonasteryCooldown(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	_startPrisonerOfWarAtMonasterySoftCooldown(player)


######## Prisoner of War at Fort ###########

PRISONER_OF_WAR_AT_FORT_SOFT_COOLDOWN_PREFIX = "[[WTP_PRISONER_OF_WAR_AT_FORT_SOFT_READY_TURN="
PRISONER_OF_WAR_AT_FORT_SOFT_COOLDOWN_SUFFIX = "]]"

def _getPrisonerOfWarAtFortSoftCooldownReadyTurn(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(PRISONER_OF_WAR_AT_FORT_SOFT_COOLDOWN_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(PRISONER_OF_WAR_AT_FORT_SOFT_COOLDOWN_PREFIX)
	iEnd = szData.find(PRISONER_OF_WAR_AT_FORT_SOFT_COOLDOWN_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1


def _setPrisonerOfWarAtFortSoftCooldownReadyTurn(player, iReadyTurn):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(PRISONER_OF_WAR_AT_FORT_SOFT_COOLDOWN_PREFIX)
	if iStart != -1:
		iEnd = szData.find(PRISONER_OF_WAR_AT_FORT_SOFT_COOLDOWN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(PRISONER_OF_WAR_AT_FORT_SOFT_COOLDOWN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szMarker = "%s%d%s" % (
		PRISONER_OF_WAR_AT_FORT_SOFT_COOLDOWN_PREFIX,
		iReadyTurn,
		PRISONER_OF_WAR_AT_FORT_SOFT_COOLDOWN_SUFFIX
	)

	szData += szMarker
	player.setScriptData(szData)


def _startPrisonerOfWarAtFortSoftCooldown(player):
	if player.isNone():
		return

	_setPrisonerOfWarAtFortSoftCooldownReadyTurn(
		player,
		CyGame().getGameTurn() + _scaleTurnsByGameSpeed(15)
	)


def _isPrisonerOfWarAtFortSoftCooldownActive(player):
	if player.isNone():
		return False

	iReadyTurn = _getPrisonerOfWarAtFortSoftCooldownReadyTurn(player)
	return iReadyTurn > CyGame().getGameTurn()


def canTriggerPrisonerOfWarAtFort(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	if _isPrisonerOfWarAtFortSoftCooldownActive(player):
		return False

	return True


def applyPrisonerOfWarAtFortCooldown(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	_startPrisonerOfWarAtFortSoftCooldown(player)
    
    
######## ORLANTH EVENTS ########

def canTriggerKingFurious(argsList):
	return False
	ePlayer = argsList[1]
	player = gc.getPlayer(ePlayer)
	if not player.isPlayable():
		return False
	if player.isNative():
		return False
	king = gc.getPlayer(player.getParent())
	if not king.isEurope():
		return False
	if king.AI_getAttitude(ePlayer) > 0:
		return False
	if player.isInRevolution():
		return False
	return True

def canTriggerKingHappy(argsList):
	return False
	ePlayer = argsList[1]
	player = gc.getPlayer(ePlayer)
	if not player.isPlayable():
		return False
	if player.isNative():
		return False
	king = gc.getPlayer(player.getParent())
	if not king.isEurope():
		return False
	if king.AI_getAttitude(ePlayer) > 4:
		return False
	if player.isInRevolution():
		return False
	return True

def canDoNotInRevolution(argsList):
	ePlayer = argsList[1]
	player = gc.getPlayer(ePlayer)
	if not player.isPlayable():
		return False
	if player.isNative():
		return False
	king = gc.getPlayer(player.getParent())
	if not king.isEurope():
		return False
	if player.isInRevolution():
		return False
	return True

def canDoInRevolution(argsList):
	ePlayer = argsList[1]
	player = gc.getPlayer(ePlayer)
	if player.isNative():
		return False
	if player.isInRevolution():
		return True
	return False

def canTriggerDeliverLumber(argsList):
	pTriggeredData = argsList[0]
	ePlayer = argsList[1]
	iCity = argsList[2]
	player = gc.getPlayer(ePlayer)
	if not player.isPlayable():
		return False
	king = gc.getPlayer(player.getParent())
	if not king.isEurope():
		return False
	city = player.getCity(iCity)
	if city.isNone():
		return False
	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_DELIVER_LUMBER")
	event = gc.getEventInfo(eEvent)
	iYield = gc.getInfoTypeForString("YIELD_LUMBER")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	if city.getYieldStored(iYield) < -quantity:
		return False
	return True

def canTriggerDeliverCoats(argsList):
	pTriggeredData = argsList[0]
	ePlayer = argsList[1]
	iCity = argsList[2]
	player = gc.getPlayer(ePlayer)
	if not player.isPlayable():
		return False
	king = gc.getPlayer(player.getParent())
	if not king.isEurope():
		return False
	city = player.getCity(iCity)
	if city.isNone():
		return False
	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_DELIVER_COATS")
	event = gc.getEventInfo(eEvent)
	iYield = gc.getInfoTypeForString("YIELD_COATS")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	if city.getYieldStored(iYield) < -quantity:
		return False
	return True

def CanDoRequisitionDeliver(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	yields = {
		1 : "YIELD_LUMBER",
		2 : "YIELD_COATS",
		3 : "YIELD_CLOTH"
		}
	iChoose = yields[event.getGenericParameter(2)]
	iYield = gc.getInfoTypeForString(iChoose)
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return False
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	if city.getYieldStored(iYield) < -quantity:
		return False
	return True

def applyRequisitionDeliver(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	city = player.getCity(kTriggeredData.iCityId)
	player.AI_changeAttitudeExtra(eking, event.getGenericParameter(3))
	king.AI_changeAttitudeExtra(kTriggeredData.ePlayer, event.getGenericParameter(3))
	yields = {
		1 : "YIELD_LUMBER",
		2 : "YIELD_COATS",
		3 : "YIELD_CLOTH"
		}
	iChoose = yields[event.getGenericParameter(2)]
	iYield = gc.getInfoTypeForString(iChoose)
	iPrice = king.getYieldBuyPrice(iYield)

	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity:
		return
	city.changeYieldStored(iYield, quantity)
	king.setYieldBuyPrice(iYield, iPrice+event.getGenericParameter(4), 1)

def getHelpRequisition(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	city = player.getCity(kTriggeredData.iCityId)
	yields = {
		1 : "YIELD_LUMBER",
		2 : "YIELD_COATS",
		3 : "YIELD_CLOTH"
		}
	iChoose = yields[event.getGenericParameter(2)]
	iYield = gc.getInfoTypeForString(iChoose)
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	szHelp = localText.getText("TXT_KEY_EVENT_REQUISITION_HELP", ())
	if event.getGenericParameter(1) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_GAIN", (quantity, gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
	if event.getGenericParameter(1) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity, gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
	if event.getGenericParameter(4) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_PRICE_INCREASE", (event.getGenericParameter(4), gc.getYieldInfo(iYield).getChar(), king.getCivilizationShortDescriptionKey()))
	if event.getGenericParameter(4) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_PRICE_DECREASE", (event.getGenericParameter(4), gc.getYieldInfo(iYield).getChar(), king.getCivilizationShortDescriptionKey()))
	if event.getGenericParameter(3) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_INCREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	if event.getGenericParameter(3) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_DECREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	return szHelp

def applyKingPleased(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	player.AI_changeAttitudeExtra(eking, event.getGenericParameter(3))
	king.AI_changeAttitudeExtra(kTriggeredData.ePlayer, event.getGenericParameter(3))
	if event.getGenericParameter(4) == 1 :
		player.NBMOD_DecreaseMaxTaxRate()

def getHelpKingPleased(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	if event.getGenericParameter(3) > 0 :
		szHelp = localText.getText("TXT_KEY_EVENT_KING_PLEASED_HELP", ())
	if event.getGenericParameter(3) < 0 :
		szHelp = localText.getText("TXT_KEY_EVENT_KING_ANGRY_HELP", ())
	if event.getGenericParameter(4) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_MAXTAXDECREASE", (-GlobalDefines.DECREASE_MAX_TAX_RATE, player.NBMOD_GetMaxTaxRate()-GlobalDefines.DECREASE_MAX_TAX_RATE))
	if event.getGenericParameter(4) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_MAXTAXINCREASE", (-GlobalDefines.INCREASE_MAX_TAX_RATE, player.NBMOD_GetMaxTaxRate()-GlobalDefines.INCREASE_MAX_TAX_RATE))
	if event.getGenericParameter(3) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_INCREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	if event.getGenericParameter(3) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_DECREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	if 'szHelp' in locals():
		return szHelp
	else:
		sys.stderr.write(event.getType() + " has PythonHelp getHelpKingPleased without setting generic parameters 3/4 to use it to generate output")

def applyKingAngry(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	player.AI_changeAttitudeExtra(eking, event.getGenericParameter(3))
	king.AI_changeAttitudeExtra(kTriggeredData.ePlayer, event.getGenericParameter(3))
	if event.getGenericParameter(4) == 1 :
		player.NBMOD_IncreaseMaxTaxRate()

def getHelpKingAngry(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	szHelp = localText.getText("TXT_KEY_EVENT_KING_ANGRY_HELP", ())
	if event.getGenericParameter(4) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_MAXTAXINCREASE", (-GlobalDefines.INCREASE_MAX_TAX_RATE, player.NBMOD_GetMaxTaxRate()-GlobalDefines.INCREASE_MAX_TAX_RATE))
	if event.getGenericParameter(3) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_DECREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	return szHelp

def canTriggerHorsethief(argsList):
	eEvent = gc.getInfoTypeForString("EVENT_HORSETHIEF_2")
	event = gc.getEventInfo(eEvent)
	ePlayer = argsList[1]
	iCity = argsList[2]
	player = gc.getPlayer(ePlayer)
	city = player.getCity(iCity)
	if not player.isPlayable():
		return False
	if city.isNone():
		return False
	iYield = gc.getInfoTypeForString("YIELD_HORSES")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity:
		return False
	return True

def canTriggerCattlethief(argsList):
	eEvent = gc.getInfoTypeForString("EVENT_CATTLETHIEF_1")
	event = gc.getEventInfo(eEvent)
	ePlayer = argsList[1]
	iCity = argsList[2]
	player = gc.getPlayer(ePlayer)
	city = player.getCity(iCity)
	if not player.isPlayable():
		return False
	if city.isNone():
		return False
	iYield = gc.getInfoTypeForString("YIELD_CATTLE")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity:
		return False
	return True

def applyHorsethief_2(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	iYield = gc.getInfoTypeForString("YIELD_HORSES")
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity:
		return
	city.changeYieldStored(iYield, quantity)
	return True

def applyCattlethief_1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	iYield = gc.getInfoTypeForString("YIELD_CATTLE")
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity:
		return
	city.changeYieldStored(iYield, quantity)
	return True

def getHelpHorsethief_2(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_HORSES")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	szHelp = "Gain 1 Petty Criminal."
	szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity,  gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
	return szHelp

def getHelpCattlethief_1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_CATTLE")

	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	szHelp = localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity,  gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
	return szHelp

def canTriggerArchbishopric(argsList):
	ePlayer = argsList[1]
	player = gc.getPlayer(ePlayer)
	if not player.isPlayable():
		return False
	iAchieve = gc.getInfoTypeForString("ACHIEVE_THREE_CHURCHES")
	if player.isAchieveGained(iAchieve):
		iAchieve = gc.getInfoTypeForString("ACHIEVE_TEN_CROSSES")
		if player.isAchieveGained(iAchieve):
			return True
	return False

def canTriggerNativeTrade(argsList):
	ePlayer = argsList[1]
	player = gc.getPlayer(ePlayer)
	if not player.isPlayable():
		return False
	iAchieve = gc.getInfoTypeForString("ACHIEVE_FIVE_NATIVE_CONTACT")
	if player.isAchieveGained(iAchieve):
		return True
	return False

def canTriggerEuroTrade(argsList):
	ePlayer = argsList[1]
	player = gc.getPlayer(ePlayer)
	if not player.isPlayable():
		return False
	iAchieve = gc.getInfoTypeForString("ACHIEVE_FOUR_EURO_CONTACT")
	if player.isAchieveGained(iAchieve):
		return True
	return False

def canTriggerPirateAttack1(argsList):
	eTrigger = argsList[0]
	ePlayer = argsList[1]
	iCityId = argsList[2]

	player = gc.getPlayer(ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	iAchieve = gc.getInfoTypeForString("ACHIEVE_TENTHOUSAND_TRADE")
	if not player.isAchieveGained(iAchieve):
		return False

	city = player.getCity(iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return False

	if not city.isCoastal(gc.getMIN_WATER_SIZE_FOR_OCEAN()):
		return False

	return True

def canTriggerPirateAttack2(argsList):
	eTrigger = argsList[0]
	ePlayer = argsList[1]
	iCityId = argsList[2]

	player = gc.getPlayer(ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	iAchieve = gc.getInfoTypeForString("ACHIEVE_HUNDREDTHOUSAND_TRADE")
	if not player.isAchieveGained(iAchieve):
		return False

	city = player.getCity(iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return False

	if not city.isCoastal(gc.getMIN_WATER_SIZE_FOR_OCEAN()):
		return False

	return True

def _getPirateAttackSpawnPlot(city):
	if city.isNone():
		return None

	iOcean = gc.getInfoTypeForString("TERRAIN_OCEAN")

	for iDX in range(-2, 3):
		for iDY in range(-2, 3):
			pPlot = plotXY(city.getX(), city.getY(), iDX, iDY)

			if pPlot is None or pPlot.isNone():
				continue

			if not pPlot.isWater():
				continue

			if pPlot.getTerrainType() == iOcean:
				return pPlot

	return None


def doPirateAttack1(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return

	pPlot = _getPirateAttackSpawnPlot(city)
	if pPlot is None or pPlot.isNone():
		return

	bPlayer = gc.getPlayer(gc.getGame().getBarbarianPlayer())

	bPlayer.initUnit(gc.getInfoTypeForString("UNIT_PRIVATEER"), ProfessionTypes.NO_PROFESSION, pPlot.getX(), pPlot.getY(), UnitAITypes.UNITAI_PIRATE_SEA, DirectionTypes.DIRECTION_SOUTH, 0)
	bPlayer.initUnit(gc.getInfoTypeForString("UNIT_PIRATE_CUTTER"), ProfessionTypes.NO_PROFESSION, pPlot.getX(), pPlot.getY(), UnitAITypes.UNITAI_PIRATE_SEA, DirectionTypes.DIRECTION_SOUTH, 0)

def doPirateAttack2(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return

	pPlot = _getPirateAttackSpawnPlot(city)
	if pPlot is None or pPlot.isNone():
		return

	bPlayer = gc.getPlayer(gc.getGame().getBarbarianPlayer())

	bPlayer.initUnit(gc.getInfoTypeForString("UNIT_PIRATE_FRIGATE"), ProfessionTypes.NO_PROFESSION, pPlot.getX(), pPlot.getY(), UnitAITypes.UNITAI_PIRATE_SEA, DirectionTypes.DIRECTION_SOUTH, 0)
	bPlayer.initUnit(gc.getInfoTypeForString("UNIT_PIRATE_FRIGATE"), ProfessionTypes.NO_PROFESSION, pPlot.getX(), pPlot.getY(), UnitAITypes.UNITAI_PIRATE_SEA, DirectionTypes.DIRECTION_SOUTH, 0)
	bPlayer.initUnit(gc.getInfoTypeForString("UNIT_PRIVATEER"), ProfessionTypes.NO_PROFESSION, pPlot.getX(), pPlot.getY(), UnitAITypes.UNITAI_PIRATE_SEA, DirectionTypes.DIRECTION_SOUTH, 0)
	bPlayer.initUnit(gc.getInfoTypeForString("UNIT_PIRATE_CUTTER"), ProfessionTypes.NO_PROFESSION, pPlot.getX(), pPlot.getY(), UnitAITypes.UNITAI_PIRATE_SEA, DirectionTypes.DIRECTION_SOUTH, 0)

def canTriggerTavernVsChapel(argsList):
	ePlayer = argsList[1]
	iCity = argsList[2]
	player = gc.getPlayer(ePlayer)
	city = player.getCity(iCity)
	if not player.isPlayable():
		return False
	if city.isNone():
		return False
	iSpecialBuildingTavern = gc.getInfoTypeForString("SPECIALBUILDING_TAVERN")
	iSpecialBuildingChapel = gc.getInfoTypeForString("SPECIALBUILDING_CROSSES")
	for iBuilding in range(gc.getNumBuildingInfos()):
		if gc.getBuildingInfo(iBuilding).getSpecialBuildingType() == iSpecialBuildingTavern:
			if city.isHasBuilding(iBuilding):
				return False
		if gc.getBuildingInfo(iBuilding).getSpecialBuildingType() == iSpecialBuildingChapel:
			if city.isHasBuilding(iBuilding):
				return False
	return True

######## Privateer Events ###########

def canTriggerPrivateer(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False

	if unit.getUnitClassType() != gc.getInfoTypeForString("UNITCLASS_PRIVATEER"):
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	if not plot.isWater():
		return False

	if plot.isCity():
		return False

	return True

def applyPrivateer1(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return

	if unit.getUnitClassType() != gc.getInfoTypeForString("UNITCLASS_PRIVATEER"):
		return

	plot = unit.plot()
	if plot is None or plot.isNone():
		return

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return

	if not plot.isWater():
		return

	unit.kill(False)
    
######## BEER ROBBERY ###########

def canTriggerBeerRobbery(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	player2 = gc.getPlayer(kTriggeredData.eOtherPlayer)

	if player.isNone() or player2.isNone():
		return False

	if not player.isPlayable() or player.isNative():
		return False

	if not player2.isPlayable() or player2.isNative():
		return False

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return False

	eEvent1 = gc.getInfoTypeForString("EVENT_BEER_ROBBERY_1")
	event1 = gc.getEventInfo(eEvent1)

	iYield = gc.getInfoTypeForString("YIELD_BEER")
	quantity = event1.getGenericParameter(1)

	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent() / 100

	if quantity <= 0:
		return False

	if city.getYieldStored(iYield) < quantity:
		return False

	return True


def applyBeerRobbery1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	player2 = gc.getPlayer(kTriggeredData.eOtherPlayer)

	if player.isNone() or player2.isNone():
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return

	iYield = gc.getInfoTypeForString("YIELD_BEER")
	quantity = event.getGenericParameter(1)

	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent() / 100

	if quantity <= 0:
		return

	if city.getYieldStored(iYield) < quantity:
		return

	city.changeYieldStored(iYield, -quantity)


def getHelpBeerRobbery1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return u""

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return u""

	iYield = gc.getInfoTypeForString("YIELD_BEER")
	quantity = event.getGenericParameter(1)

	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent() / 100

	if quantity <= 0:
		return u""

	szHelp = localText.getText(
		"TXT_KEY_EVENT_YIELD_LOOSE",
		(-quantity, gc.getYieldInfo(iYield).getChar(), city.getNameKey())
	)

	return szHelp


def canApplyBeerRobbery3(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return False

	iYield = gc.getInfoTypeForString("YIELD_BEER")
	quantity = event.getGenericParameter(1)

	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent() / 100

	if quantity <= 0:
		return False

	if city.getYieldStored(iYield) < quantity:
		return False

	return True

######## WINE THEFT ###########

def canTriggerWineTheftCity(argsList):
	eTrigger = argsList[0]
	ePlayer = argsList[1]
	iCityId = argsList[2]

	player = gc.getPlayer(ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	city = player.getCity(iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return False

	iYield = gc.getInfoTypeForString("YIELD_WINE")

	eEvent1 = gc.getInfoTypeForString("EVENT_WINE_THEFT_1")
	event1 = gc.getEventInfo(eEvent1)

	quantity = event1.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent() / 100

	if city.getYieldStored(iYield) < quantity:
		return False

	return True

def applyWineTheft1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return

	iYield = gc.getInfoTypeForString("YIELD_WINE")

	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent() / 100

	if city.getYieldStored(iYield) < quantity:
		return

	city.changeYieldStored(iYield, -quantity)

def getHelpWineTheft1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return u""

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return u""

	iYield = gc.getInfoTypeForString("YIELD_WINE")

	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent() / 100

	if quantity == 0:
		return u""

	return localText.getText(
		"TXT_KEY_EVENT_YIELD_LOOSE",
		(-quantity, gc.getYieldInfo(iYield).getChar(), city.getNameKey())
	)

def applyWineTheft2(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return

	iEnemyClass = event.getGenericParameter(1)
	iAmount = event.getGenericParameter(2)

	if iEnemyClass == -1 or iAmount <= 0:
		return

	iBarbarian = gc.getGame().getBarbarianPlayer()
	barbPlayer = gc.getPlayer(iBarbarian)
	if barbPlayer.isNone():
		return

	iEnemyType = gc.getCivilizationInfo(barbPlayer.getCivilizationType()).getCivilizationUnits(iEnemyClass)
	if iEnemyType == -1:
		return

	for i in range(iAmount):
		for iDX in range(-1, 2):
			for iDY in range(-1, 2):
				if iDX == 0 and iDY == 0:
					continue

				plot = plotXY(city.getX(), city.getY(), iDX, iDY)
				if plot is None or plot.isNone():
					continue
				if plot.isWater():
					continue
				if plot.isPeak():
					continue
				if plot.isImpassable():
					continue
				if plot.isCity():
					continue
				if plot.isUnit():
					continue

				barbPlayer.initUnit(
					iEnemyType,
					ProfessionTypes.NO_PROFESSION,
					plot.getX(),
					plot.getY(),
					UnitAITypes.NO_UNITAI,
					DirectionTypes.DIRECTION_SOUTH,
					0
				)
				break

def getHelpWineTheft2(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)

	iUnitClass = event.getGenericParameter(1)
	iAmount = event.getGenericParameter(2)

	if iUnitClass == -1:
		return u""

	return localText.getText(
		"TXT_KEY_EVENT_WINE_THEFT_2_HELP",
		(iAmount, gc.getUnitClassInfo(iUnitClass).getTextKey())
	)

######## LUXURY GOODS ###########

def canTriggerLuxuryGoodsCity(argsList):
	eTrigger = argsList[0]
	ePlayer = argsList[1]
	iCityId = argsList[2]

	player = gc.getPlayer(ePlayer)
	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	city = player.getCity(iCityId)
	if city.isNone():
		return False

	iYield = gc.getInfoTypeForString("YIELD_LUXURY_GOODS")

	eEvent1 = gc.getInfoTypeForString("EVENT_LUXURY_GOODS_1")
	event1 = gc.getEventInfo(eEvent1)

	quantity = event1.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent() / 100

	if city.getYieldStored(iYield) < -quantity:
		return False

	return True

def canApplyLuxuryGoods1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return False

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return False

	iYield = gc.getInfoTypeForString("YIELD_LUXURY_GOODS")

	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	if city.getYieldStored(iYield) < -quantity:
		return False

	return True

def applyLuxuryGoods1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return

	iYield = gc.getInfoTypeForString("YIELD_LUXURY_GOODS")

	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	if city.getYieldStored(iYield) < -quantity:
		return

	city.changeYieldStored(iYield, quantity)


def getHelpLuxuryGoods1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return u""

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return u""

	iYield = gc.getInfoTypeForString("YIELD_LUXURY_GOODS")

	szHelp = ""
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	if event.getGenericParameter(1) <> 0:
		szHelp = localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity, gc.getYieldInfo(iYield).getChar(), city.getNameKey()))

	return szHelp

######## Cattle and Sheep ###########

def canTriggerCattleAndSheep(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	if player.isNone() :
		return False
	if not player.isPlayable():
		return False
	if city.isNone():
		return False
	# Read Parameter 1 from the first event and check if enough yield is stored in city
	eEvent1 = gc.getInfoTypeForString("EVENT_CATTLE_AND_SHEEP_1")
	event1 = gc.getEventInfo(eEvent1)
	iYield = gc.getInfoTypeForString("YIELD_CATTLE")

	quantity1 = event1.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity1 = quantity1 * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity1*2 :
		return False
	# Read Parameter 1 from the second event and check if enough yield is stored in city
	eEvent2 = gc.getInfoTypeForString("EVENT_CATTLE_AND_SHEEP_2")
	event2 = gc.getEventInfo(eEvent2)
	iYield = gc.getInfoTypeForString("YIELD_SHEEP")
	quantity2 = event2.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity2 = quantity2 * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity2*2 :
		return False
	return True

def applyCattleAndSheep1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_CATTLE")
	iYield2 = gc.getInfoTypeForString("YIELD_FOOD")
	iYield3 = gc.getInfoTypeForString("YIELD_HIDES")

	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) >= -quantity :
		city.changeYieldStored(iYield, quantity)
		city.changeYieldStored(iYield2, -quantity)
		city.changeYieldStored(iYield3, -quantity)

def getHelpCattleAndSheep1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_CATTLE")
	iYield2 = gc.getInfoTypeForString("YIELD_FOOD")
	iYield3 = gc.getInfoTypeForString("YIELD_HIDES")
	szHelp = ""
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if event.getGenericParameter(1) <> 0 :
		szHelp = localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity,  gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_GAIN", (-quantity,  gc.getYieldInfo(iYield2).getChar(), city.getNameKey()))
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_GAIN", (-quantity,  gc.getYieldInfo(iYield3).getChar(), city.getNameKey()))
	return szHelp

def applyCattleAndSheep2(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_SHEEP")
	iYield2 = gc.getInfoTypeForString("YIELD_FOOD")
	iYield3 = gc.getInfoTypeForString("YIELD_WOOL")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) >= -quantity :
		city.changeYieldStored(iYield, quantity)
		city.changeYieldStored(iYield2, -quantity)
		city.changeYieldStored(iYield3, -quantity)

def getHelpCattleAndSheep2(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_SHEEP")
	iYield2 = gc.getInfoTypeForString("YIELD_FOOD")
	iYield3 = gc.getInfoTypeForString("YIELD_WOOL")
	szHelp=""
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if event.getGenericParameter(1) <> 0 :
		szHelp = localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity,  gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_GAIN", (-quantity,  gc.getYieldInfo(iYield2).getChar(), city.getNameKey()))
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_GAIN", (-quantity,  gc.getYieldInfo(iYield3).getChar(), city.getNameKey()))
	return szHelp

######## Horse Deal ###########

def canTriggerHorseDeal(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	if player.isNone() :
		return False
	if not player.isPlayable():
		return False
	if city.isNone():
		return False
	# Read Parameter 1 from the first event and check if enough yield is stored in city
	eEvent1 = gc.getInfoTypeForString("EVENT_HORSE_DEAL_1")
	event1 = gc.getEventInfo(eEvent1)
	iYield = gc.getInfoTypeForString("YIELD_HORSES")
	quantity = event1.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity*2 :
		return False
	return True

def applyHorseDeal1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_HORSES")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity :
		return
	city.changeYieldStored(iYield, quantity)

def getHelpHorseDeal1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_HORSES")
	szHelp = ""

	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if event.getGenericParameter(1) <> 0 :
		szHelp = localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity,  gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
	return szHelp

######## Seasoned Trader Trade Gathering Event ###########

SEASONED_TRADER_MEETING_SOFT_COOLDOWN_PREFIX = "[[WTP_SEASONED_TRADER_MEETING_SOFT_READY_TURN="
SEASONED_TRADER_MEETING_SOFT_COOLDOWN_SUFFIX = "]]"

def _getSeasonedTraderMeetingSoftCooldownReadyTurn(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(SEASONED_TRADER_MEETING_SOFT_COOLDOWN_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(SEASONED_TRADER_MEETING_SOFT_COOLDOWN_PREFIX)
	iEnd = szData.find(SEASONED_TRADER_MEETING_SOFT_COOLDOWN_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1


def _setSeasonedTraderMeetingSoftCooldownReadyTurn(player, iReadyTurn):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(SEASONED_TRADER_MEETING_SOFT_COOLDOWN_PREFIX)
	if iStart != -1:
		iEnd = szData.find(SEASONED_TRADER_MEETING_SOFT_COOLDOWN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(SEASONED_TRADER_MEETING_SOFT_COOLDOWN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szData += SEASONED_TRADER_MEETING_SOFT_COOLDOWN_PREFIX + str(iReadyTurn) + SEASONED_TRADER_MEETING_SOFT_COOLDOWN_SUFFIX
	player.setScriptData(szData)


def _startSeasonedTraderMeetingSoftCooldown(player, iBaseTurns):
	if player.isNone():
		return

	_setSeasonedTraderMeetingSoftCooldownReadyTurn(player, CyGame().getGameTurn() + 30)


def _isSeasonedTraderMeetingSoftCooldownActive(player):
	if player.isNone():
		return False

	iReadyTurn = _getSeasonedTraderMeetingSoftCooldownReadyTurn(player)
	return iReadyTurn > CyGame().getGameTurn()


def _getSeasonedTraderMeetingHorseYield():
	return gc.getInfoTypeForString("YIELD_HORSES")


def _getSeasonedTraderMeetingRequiredTriggerHorses():
	return 100


def _getSeasonedTraderMeetingCity(player, kTriggeredData):
	if player.isNone():
		return None

	city = player.getCity(kTriggeredData.iCityId)
	if city is None or city.isNone():
		return None

	return city


def _getSeasonedTraderMeetingUnit(player, kTriggeredData):
	if player.isNone():
		return None

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit is None or unit.isNone():
		return None

	return unit


def _isValidSeasonedTraderMeetingContext(player, city, unit, kTriggeredData):
	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	if city is None or city.isNone():
		return False

	if unit is None or unit.isNone():
		return False

	if city.getID() != kTriggeredData.iCityId:
		return False

	if unit.getX() != city.getX() or unit.getY() != city.getY():
		return False

	if unit.getX() != kTriggeredData.iPlotX or unit.getY() != kTriggeredData.iPlotY:
		return False

	iExpertTraderClass = gc.getInfoTypeForString("UNITCLASS_EXPERT_TRADER")
	if unit.getUnitClassType() != iExpertTraderClass:
		return False

	iHorseYield = _getSeasonedTraderMeetingHorseYield()
	if iHorseYield == -1:
		return False

	if city.getYieldStored(iHorseYield) < _getSeasonedTraderMeetingRequiredTriggerHorses():
		return False

	otherPlayer = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if otherPlayer.isNone():
		return False

	if not otherPlayer.isNative():
		return False

	otherCity = otherPlayer.getCity(kTriggeredData.iOtherPlayerCityId)
	if otherCity is None or otherCity.isNone():
		return False

	return True


def canTriggerTraderMeeting(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	city = _getSeasonedTraderMeetingCity(player, kTriggeredData)
	unit = _getSeasonedTraderMeetingUnit(player, kTriggeredData)

	if not _isValidSeasonedTraderMeetingContext(player, city, unit, kTriggeredData):
		return False

	if _isSeasonedTraderMeetingSoftCooldownActive(player):
		return False

	if CyGame().getSorenRandNum(100, "Seasoned Trader Meeting normal chance") >= 20:
		return False

	return True


def canDoSeasonedTraderHorseOption(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return False

	city = _getSeasonedTraderMeetingCity(player, kTriggeredData)
	unit = _getSeasonedTraderMeetingUnit(player, kTriggeredData)

	if not _isValidSeasonedTraderMeetingContext(player, city, unit, kTriggeredData):
		return False

	iHorseCost = abs(event.getGenericParameter(1))
	if iHorseCost <= 0:
		return False

	iHorseYield = _getSeasonedTraderMeetingHorseYield()
	if iHorseYield == -1:
		return False

	if city.getYieldStored(iHorseYield) < iHorseCost:
		return False

	return True


def applyHorseGift1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city is None or city.isNone():
		return

	iYield = gc.getInfoTypeForString("YIELD_HORSES")

	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent() / 100

	if city.getYieldStored(iYield) < -quantity:
		return

	city.changeYieldStored(iYield, quantity)

	_startSeasonedTraderMeetingSoftCooldown(player, 30)


def applyHorseGift3(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city is None or city.isNone():
		return

	iYield = gc.getInfoTypeForString("YIELD_HORSES")

	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent() / 100

	if city.getYieldStored(iYield) < -quantity:
		return

	city.changeYieldStored(iYield, quantity)

	_startSeasonedTraderMeetingSoftCooldown(player, 30)


def _spawnSeasonedTraderMeetingRaidersAdjacent(cityPlot, iUnitClass, iNumUnits):
	if cityPlot is None or cityPlot.isNone():
		return []

	iBarbarian = gc.getGame().getBarbarianPlayer()
	barbPlayer = gc.getPlayer(iBarbarian)
	if barbPlayer.isNone():
		return []

	iUnitType = gc.getCivilizationInfo(barbPlayer.getCivilizationType()).getCivilizationUnits(iUnitClass)
	if iUnitType == UnitTypes.NO_UNIT:
		return []

	spawnedUnits = []

	for i in range(iNumUnits):
		for iDX in range(-1, 2):
			for iDY in range(-1, 2):
				if iDX == 0 and iDY == 0:
					continue

				pLoop = plotXY(cityPlot.getX(), cityPlot.getY(), iDX, iDY)

				if pLoop is None or pLoop.isNone():
					continue
				if pLoop.isWater() or pLoop.isImpassable() or pLoop.isPeak():
					continue
				if pLoop.isCity() or pLoop.isUnit():
					continue

				newUnit = barbPlayer.initUnit(
					iUnitType,
					ProfessionTypes.NO_PROFESSION,
					pLoop.getX(),
					pLoop.getY(),
					UnitAITypes.NO_UNITAI,
					DirectionTypes.DIRECTION_SOUTH,
					0
				)

				if newUnit is not None and not newUnit.isNone():
					spawnedUnits.append(newUnit)

				break

	return spawnedUnits


def applySeasonedTraderMeetingRaid(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	_startSeasonedTraderMeetingSoftCooldown(player, 30)

	city = _getSeasonedTraderMeetingCity(player, kTriggeredData)
	if city is None or city.isNone():
		return

	cityPlot = city.plot()
	if cityPlot is None or cityPlot.isNone():
		return

	iHostileUnitClass = event.getGenericParameter(1)
	iNumRaiders = event.getGenericParameter(2)

	raiders = _spawnSeasonedTraderMeetingRaidersAdjacent(cityPlot, iHostileUnitClass, iNumRaiders)

	for raider in raiders:
		if raider is None or raider.isNone():
			continue

		if raider.canMoveInto(cityPlot, True, False, False):
			raider.attack(cityPlot, False)


def getHelpHorseGift1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_HORSES")

	szHelp = u""

	quantity = abs(event.getGenericParameter(1))
	if quantity <> 0:
		szHelp += localText.getText(
			"TXT_KEY_EVENT_YIELD_LOOSE",
			(-quantity, gc.getYieldInfo(iYield).getChar(), city.getNameKey())
		)

	szHelp += u"\n"
	szHelp += localText.getText("TXT_KEY_EVENT_SEASONED_TRADER_UPGRADE_HELP", ())

	return szHelp


def getHelpHorseGift3(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_HORSES")

	szHelp = u""

	quantity = abs(event.getGenericParameter(1))
	if quantity <> 0:
		szHelp += localText.getText(
			"TXT_KEY_EVENT_YIELD_LOOSE",
			(-quantity, gc.getYieldInfo(iYield).getChar(), city.getNameKey())
		)

	return szHelp


def getHelpSeasonedTraderRaid(argsList):
	return localText.getText("TXT_KEY_EVENT_SEASONED_TRADER_MEETING_HELP", ())



######## Wild Animal ###########

def canApplyWildAnimal1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	if player.isNone():
		return False
	if not player.isPlayable():
		return False
	if city.isNone():
		return False
	# Read Parameter 1 from event and check if enough yield is stored in city
	iYield = gc.getInfoTypeForString("YIELD_MUSKETS")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity :
		return False
	return True

def applyWildAnimal1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_MUSKETS")

	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity :
		return
	city.changeYieldStored(iYield, quantity)

def getHelpWildAnimal1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_MUSKETS")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	szHelp = ""
	if event.getGenericParameter(1) <> 0 :
		szHelp = localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity,  gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
	return szHelp

######## Native Events ###########

######## Discovery Failed Trader Change ###########

DISCOVERY_FAILED_TRADER_CHANGE_SOFT_COOLDOWN_PREFIX = "[[WTP_DISCOVERY_FAILED_TRADER_CHANGE_SOFT_READY_TURN="
DISCOVERY_FAILED_TRADER_CHANGE_SOFT_COOLDOWN_SUFFIX = "]]"

DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_UNIT_PREFIX = "[[WTP_DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_UNIT="
DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_UNIT_SUFFIX = "]]"

DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_PLOT_PREFIX = "[[WTP_DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_PLOT="
DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_PLOT_SUFFIX = "]]"

def _getDiscoveryFailedTraderChangeSoftCooldownReadyTurn(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(DISCOVERY_FAILED_TRADER_CHANGE_SOFT_COOLDOWN_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(DISCOVERY_FAILED_TRADER_CHANGE_SOFT_COOLDOWN_PREFIX)
	iEnd = szData.find(DISCOVERY_FAILED_TRADER_CHANGE_SOFT_COOLDOWN_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1

def _setDiscoveryFailedTraderChangeSoftCooldownReadyTurn(player, iReadyTurn):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(DISCOVERY_FAILED_TRADER_CHANGE_SOFT_COOLDOWN_PREFIX)
	if iStart != -1:
		iEnd = szData.find(DISCOVERY_FAILED_TRADER_CHANGE_SOFT_COOLDOWN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(DISCOVERY_FAILED_TRADER_CHANGE_SOFT_COOLDOWN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szMarker = "%s%d%s" % (
		DISCOVERY_FAILED_TRADER_CHANGE_SOFT_COOLDOWN_PREFIX,
		iReadyTurn,
		DISCOVERY_FAILED_TRADER_CHANGE_SOFT_COOLDOWN_SUFFIX
	)

	szData += szMarker
	player.setScriptData(szData)

def _startDiscoveryFailedTraderChangeSoftCooldown(player, iBaseTurns):
	if player.isNone():
		return

	iCooldown = _getDiscoveryFailedTraderChangeScaledTurns(iBaseTurns)
	_setDiscoveryFailedTraderChangeSoftCooldownReadyTurn(player, CyGame().getGameTurn() + iCooldown)

def _isDiscoveryFailedTraderChangeSoftCooldownActive(player):
	if player.isNone():
		return False

	iReadyTurn = _getDiscoveryFailedTraderChangeSoftCooldownReadyTurn(player)
	return iReadyTurn > CyGame().getGameTurn()

def _getDiscoveryFailedTraderChangeScaledTurns(iBaseTurns):
	gameSpeedType = CyGame().getGameSpeedType()
	iPercent = gc.getGameSpeedInfo(gameSpeedType).getGrowthPercent()
	return max(1, int((iBaseTurns * iPercent) / 100))

def _getUnitClass(unit):
	if unit is None or unit.isNone():
		return -1
	return gc.getUnitInfo(unit.getUnitType()).getUnitClassType()

def _setDiscoveryFailedTraderSelectedUnitData(player, iUnitId, iPlotX, iPlotY):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_UNIT_PREFIX)
	if iStart != -1:
		iEnd = szData.find(DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_UNIT_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_UNIT_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	iStart = szData.find(DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_PLOT_PREFIX)
	if iStart != -1:
		iEnd = szData.find(DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_PLOT_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_PLOT_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szData += "%s%d%s" % (
		DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_UNIT_PREFIX,
		iUnitId,
		DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_UNIT_SUFFIX
	)

	szData += "%s%d,%d%s" % (
		DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_PLOT_PREFIX,
		iPlotX,
		iPlotY,
		DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_PLOT_SUFFIX
	)

	player.setScriptData(szData)

def _clearDiscoveryFailedTraderSelectedUnitData(player):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		return

	iStart = szData.find(DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_UNIT_PREFIX)
	if iStart != -1:
		iEnd = szData.find(DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_UNIT_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_UNIT_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	iStart = szData.find(DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_PLOT_PREFIX)
	if iStart != -1:
		iEnd = szData.find(DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_PLOT_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_PLOT_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	player.setScriptData(szData)

def _getDiscoveryFailedTraderSelectedUnitId(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_UNIT_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_UNIT_PREFIX)
	iEnd = szData.find(DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_UNIT_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1

def _getDiscoveryFailedTraderSelectedPlot(player):
	if player.isNone():
		return (-1, -1)

	szData = player.getScriptData()
	if szData is None or szData == "":
		return (-1, -1)

	iStart = szData.find(DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_PLOT_PREFIX)
	if iStart == -1:
		return (-1, -1)

	iStart += len(DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_PLOT_PREFIX)
	iEnd = szData.find(DISCOVERY_FAILED_TRADER_CHANGE_SELECTED_PLOT_SUFFIX, iStart)
	if iEnd == -1:
		return (-1, -1)

	try:
		szCoords = szData[iStart:iEnd]
		aParts = szCoords.split(",")
		if len(aParts) != 2:
			return (-1, -1)
		return (int(aParts[0]), int(aParts[1]))
	except:
		return (-1, -1)

def _isValidFailedTraderChangeUnit(player, unit, ePlayer):
	if player.isNone():
		return False
	if not player.isPlayable():
		return False
	if player.isNative():
		return False

	if unit is None or unit.isNone():
		return False

	iFailedTraderClass = gc.getInfoTypeForString("UNITCLASS_FAILED_TRADER")
	if _getUnitClass(unit) != iFailedTraderClass:
		return False

	eColonistProfession = gc.getInfoTypeForString("PROFESSION_COLONIST")
	if unit.getProfession() != eColonistProfession:
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False
	if plot.isWater():
		return False
	if plot.isCity():
		return False
	if plot.getOwner() != ePlayer:
		return False

	return True

def _findDiscoveryFailedTraderChangeCandidate(player):
	if player.isNone():
		return None

	(unit, iter) = player.firstUnit()
	while unit:
		if _isValidFailedTraderChangeUnit(player, unit, player.getID()):
			return unit
		(unit, iter) = player.nextUnit(iter)

	return None

def canTriggerDiscoveryFailedTraderChange(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False
	if not player.isPlayable():
		return False
	if player.isNative():
		return False

	if _isDiscoveryFailedTraderChangeSoftCooldownActive(player):
		return False

	unit = _findDiscoveryFailedTraderChangeCandidate(player)
	if unit is None or unit.isNone():
		_clearDiscoveryFailedTraderSelectedUnitData(player)
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		_clearDiscoveryFailedTraderSelectedUnitData(player)
		return False

	_setDiscoveryFailedTraderSelectedUnitData(player, unit.getID(), plot.getX(), plot.getY())
	return True

def _getStoredDiscoveryFailedTraderUnit(player):
	if player.isNone():
		return None

	iUnitId = _getDiscoveryFailedTraderSelectedUnitId(player)
	if iUnitId < 0:
		return None

	unit = player.getUnit(iUnitId)
	if unit is None or unit.isNone():
		return None

	return unit

def changeFailedTraderToExpertTrader(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return
	if not player.isPlayable():
		return
	if player.isNative():
		return

	oldUnit = _getStoredDiscoveryFailedTraderUnit(player)
	if oldUnit is None or oldUnit.isNone():
		return

	if not _isValidFailedTraderChangeUnit(player, oldUnit, kTriggeredData.ePlayer):
		return

	plot = oldUnit.plot()
	if plot is None or plot.isNone():
		return

	iStoredPlotX, iStoredPlotY = _getDiscoveryFailedTraderSelectedPlot(player)
	if plot.getX() != iStoredPlotX or plot.getY() != iStoredPlotY:
		return

	iTargetUnitClass = event.getGenericParameter(1)
	if iTargetUnitClass <= 0:
		return

	bSeasoned = False
	iSeasonedChance = event.getGenericParameter(2)
	iSeasonedChance = max(0, min(100, iSeasonedChance))

	if iSeasonedChance > 0:
		if CyGame().getSorenRandNum(100, "Failed Trader seasoned chance") < iSeasonedChance:
			iSeasonedTraderClass = gc.getInfoTypeForString("UNITCLASS_SEASONED_TRADER")
			if iSeasonedTraderClass != UnitClassTypes.NO_UNITCLASS:
				iTargetUnitClass = iSeasonedTraderClass
				bSeasoned = True

	iNewUnitType = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationUnits(iTargetUnitClass)
	if iNewUnitType == UnitTypes.NO_UNIT:
		return

	iX = oldUnit.getX()
	iY = oldUnit.getY()
	iDamage = oldUnit.getDamage()
	iExperience = oldUnit.getExperience()
	iMoves = oldUnit.movesLeft()

	szName = u""
	if oldUnit.getNameNoDesc():
		szName = oldUnit.getNameNoDesc()

	aPromotions = []
	for iPromotion in range(gc.getNumPromotionInfos()):
		if oldUnit.isHasPromotion(iPromotion):
			aPromotions.append(iPromotion)

	newUnit = player.initUnit(
		iNewUnitType,
		ProfessionTypes.NO_PROFESSION,
		iX,
		iY,
		UnitAITypes.NO_UNITAI,
		DirectionTypes.NO_DIRECTION,
		0
	)
	if newUnit.isNone():
		return

	eNativeTraderProfession = gc.getInfoTypeForString("PROFESSION_NATIVE_TRADER")
	if eNativeTraderProfession != ProfessionTypes.NO_PROFESSION:
		newUnit.setProfession(eNativeTraderProfession)

	if iDamage > 0:
		newUnit.setDamage(iDamage, PlayerTypes.NO_PLAYER)

	if iExperience > 0:
		newUnit.setExperience(iExperience, -1)

	for iPromotion in aPromotions:
		if newUnit.canAcquirePromotion(iPromotion):
			newUnit.setHasRealPromotion(iPromotion, True)

	if szName != u"":
		newUnit.setName(szName)

	try:
		newUnit.setMoves(iMoves)
	except:
		pass

	oldUnit.kill(False)

	_startDiscoveryFailedTraderChangeSoftCooldown(player, 30)
	_clearDiscoveryFailedTraderSelectedUnitData(player)

	if bSeasoned:
		CyInterface().addMessage(
			kTriggeredData.ePlayer,
			True,
			10,
			localText.getText("TXT_KEY_EVENT_DISCOVERY_FAILED_TRADER_SEASONED_RESULT", ()),
			"",
			0,
			"",
			ColorTypes(8),
			iX,
			iY,
			True,
			True
		)

def getHelpDiscoveryFailedTraderChange(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return u""

	iGoldCost = abs(event.getGold())
	iSeasonedChance = event.getGenericParameter(2)
	iCooldown = _getDiscoveryFailedTraderChangeScaledTurns(30)

	return localText.getText(
		"TXT_KEY_EVENT_DISCOVERY_EVENTS_FAILED_TRADER_CHANGE_HELP",
		(iGoldCost, iSeasonedChance, iCooldown)
	)

def doDiscoveryFailedTraderChangeDecline(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return
	if not player.isPlayable():
		return
	if player.isNative():
		return

	unit = _getStoredDiscoveryFailedTraderUnit(player)
	if unit is None or unit.isNone():
		return

	if not _isValidFailedTraderChangeUnit(player, unit, kTriggeredData.ePlayer):
		return

	plot = unit.plot()
	if plot is None or plot.isNone():
		return

	iStoredPlotX, iStoredPlotY = _getDiscoveryFailedTraderSelectedPlot(player)
	if plot.getX() != iStoredPlotX or plot.getY() != iStoredPlotY:
		return

	_startDiscoveryFailedTraderChangeSoftCooldown(player, 30)
	_clearDiscoveryFailedTraderSelectedUnitData(player)

def getHelpDiscoveryFailedTraderChangeDecline(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return u""

	iCooldown = _getDiscoveryFailedTraderChangeScaledTurns(30)

	return localText.getText(
		"TXT_KEY_EVENT_DISCOVERY_EVENTS_FAILED_TRADER_CHANGE_DECLINE_HELP",
		(iCooldown,)
	)

######## Discovery Failed Missionary Change ###########

DISCOVERY_FAILED_MISSIONARY_CHANGE_SOFT_COOLDOWN_PREFIX = "[[WTP_DISCOVERY_FAILED_MISSIONARY_CHANGE_SOFT_READY_TURN="
DISCOVERY_FAILED_MISSIONARY_CHANGE_SOFT_COOLDOWN_SUFFIX = "]]"

DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_UNIT_PREFIX = "[[WTP_DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_UNIT="
DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_UNIT_SUFFIX = "]]"

DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_PLOT_PREFIX = "[[WTP_DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_PLOT="
DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_PLOT_SUFFIX = "]]"

def _getDiscoveryFailedMissionaryChangeSoftCooldownReadyTurn(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(DISCOVERY_FAILED_MISSIONARY_CHANGE_SOFT_COOLDOWN_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(DISCOVERY_FAILED_MISSIONARY_CHANGE_SOFT_COOLDOWN_PREFIX)
	iEnd = szData.find(DISCOVERY_FAILED_MISSIONARY_CHANGE_SOFT_COOLDOWN_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1

def _setDiscoveryFailedMissionaryChangeSoftCooldownReadyTurn(player, iReadyTurn):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(DISCOVERY_FAILED_MISSIONARY_CHANGE_SOFT_COOLDOWN_PREFIX)
	if iStart != -1:
		iEnd = szData.find(DISCOVERY_FAILED_MISSIONARY_CHANGE_SOFT_COOLDOWN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(DISCOVERY_FAILED_MISSIONARY_CHANGE_SOFT_COOLDOWN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szMarker = "%s%d%s" % (
		DISCOVERY_FAILED_MISSIONARY_CHANGE_SOFT_COOLDOWN_PREFIX,
		iReadyTurn,
		DISCOVERY_FAILED_MISSIONARY_CHANGE_SOFT_COOLDOWN_SUFFIX
	)

	szData += szMarker
	player.setScriptData(szData)

def _getDiscoveryFailedMissionaryChangeScaledTurns(iBaseTurns):
	gameSpeedType = CyGame().getGameSpeedType()
	iPercent = gc.getGameSpeedInfo(gameSpeedType).getGrowthPercent()
	return max(1, int((iBaseTurns * iPercent) / 100))

def _startDiscoveryFailedMissionaryChangeSoftCooldown(player, iBaseTurns):
	if player.isNone():
		return

	iCooldown = _getDiscoveryFailedMissionaryChangeScaledTurns(iBaseTurns)
	_setDiscoveryFailedMissionaryChangeSoftCooldownReadyTurn(player, CyGame().getGameTurn() + iCooldown)

def _isDiscoveryFailedMissionaryChangeSoftCooldownActive(player):
	if player.isNone():
		return False

	iReadyTurn = _getDiscoveryFailedMissionaryChangeSoftCooldownReadyTurn(player)
	return iReadyTurn > CyGame().getGameTurn()

def _getDiscoveryFailedMissionaryUnitClass(unit):
	if unit is None or unit.isNone():
		return -1
	return gc.getUnitInfo(unit.getUnitType()).getUnitClassType()

def _setDiscoveryFailedMissionarySelectedUnitData(player, iUnitId, iPlotX, iPlotY):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_UNIT_PREFIX)
	if iStart != -1:
		iEnd = szData.find(DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_UNIT_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_UNIT_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	iStart = szData.find(DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_PLOT_PREFIX)
	if iStart != -1:
		iEnd = szData.find(DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_PLOT_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_PLOT_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szData += "%s%d%s" % (
		DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_UNIT_PREFIX,
		iUnitId,
		DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_UNIT_SUFFIX
	)

	szData += "%s%d,%d%s" % (
		DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_PLOT_PREFIX,
		iPlotX,
		iPlotY,
		DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_PLOT_SUFFIX
	)

	player.setScriptData(szData)

def _clearDiscoveryFailedMissionarySelectedUnitData(player):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		return

	iStart = szData.find(DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_UNIT_PREFIX)
	if iStart != -1:
		iEnd = szData.find(DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_UNIT_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_UNIT_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	iStart = szData.find(DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_PLOT_PREFIX)
	if iStart != -1:
		iEnd = szData.find(DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_PLOT_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_PLOT_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	player.setScriptData(szData)

def _getDiscoveryFailedMissionarySelectedUnitId(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_UNIT_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_UNIT_PREFIX)
	iEnd = szData.find(DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_UNIT_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1

def _getDiscoveryFailedMissionarySelectedPlot(player):
	if player.isNone():
		return (-1, -1)

	szData = player.getScriptData()
	if szData is None or szData == "":
		return (-1, -1)

	iStart = szData.find(DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_PLOT_PREFIX)
	if iStart == -1:
		return (-1, -1)

	iStart += len(DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_PLOT_PREFIX)
	iEnd = szData.find(DISCOVERY_FAILED_MISSIONARY_CHANGE_SELECTED_PLOT_SUFFIX, iStart)
	if iEnd == -1:
		return (-1, -1)

	try:
		szCoords = szData[iStart:iEnd]
		aParts = szCoords.split(",")
		if len(aParts) != 2:
			return (-1, -1)
		return (int(aParts[0]), int(aParts[1]))
	except:
		return (-1, -1)

def _isValidFailedMissionaryChangeUnit(player, unit, ePlayer):
	if player.isNone():
		return False
	if not player.isPlayable():
		return False
	if player.isNative():
		return False

	if unit is None or unit.isNone():
		return False

	iFailedMissionaryClass = gc.getInfoTypeForString("UNITCLASS_FAILED_MISSIONARY")
	if _getDiscoveryFailedMissionaryUnitClass(unit) != iFailedMissionaryClass:
		return False

	eColonistProfession = gc.getInfoTypeForString("PROFESSION_COLONIST")
	if unit.getProfession() != eColonistProfession:
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False
	if plot.isWater():
		return False
	if plot.isCity():
		return False

	return True

def _findDiscoveryFailedMissionaryChangeCandidate(player):
	if player.isNone():
		return None

	(unit, iter) = player.firstUnit()
	while unit:
		if _isValidFailedMissionaryChangeUnit(player, unit, player.getID()):
			return unit
		(unit, iter) = player.nextUnit(iter)

	return None

def _getStoredDiscoveryFailedMissionaryUnit(player):
	if player.isNone():
		return None

	iUnitId = _getDiscoveryFailedMissionarySelectedUnitId(player)
	if iUnitId < 0:
		return None

	unit = player.getUnit(iUnitId)
	if unit is None or unit.isNone():
		return None

	return unit

def _getBestMissionaryTargetForPlayer(player):
	iMissionaryClass = gc.getInfoTypeForString("UNITCLASS_CHRISTIAN_MISSIONARY")
	if iMissionaryClass == -1:
		return (-1, -1, -1)

	iUnitType = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationUnits(iMissionaryClass)
	if iUnitType == UnitTypes.NO_UNIT:
		return (-1, -1, -1)

	eMissionaryProfession = gc.getInfoTypeForString("PROFESSION_MISSIONARY")
	if eMissionaryProfession == -1:
		return (-1, -1, -1)

	return (iMissionaryClass, iUnitType, eMissionaryProfession)

def canTriggerDiscoveryFailedMissionaryChange(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False
	if not player.isPlayable():
		return False
	if player.isNative():
		return False

	if _isDiscoveryFailedMissionaryChangeSoftCooldownActive(player):
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit is None or unit.isNone():
		_clearDiscoveryFailedMissionarySelectedUnitData(player)
		return False

	if not _isValidFailedMissionaryChangeUnit(player, unit, kTriggeredData.ePlayer):
		_clearDiscoveryFailedMissionarySelectedUnitData(player)
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		_clearDiscoveryFailedMissionarySelectedUnitData(player)
		return False

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		_clearDiscoveryFailedMissionarySelectedUnitData(player)
		return False

	# Fixed trigger chance on valid plots
	if CyGame().getSorenRandNum(100, "Failed Missionary Change trigger") >= 30:
		_clearDiscoveryFailedMissionarySelectedUnitData(player)
		return False

	_setDiscoveryFailedMissionarySelectedUnitData(player, unit.getID(), plot.getX(), plot.getY())
	return True

def doDiscoveryFailedMissionaryChange(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	oldUnit = _getStoredDiscoveryFailedMissionaryUnit(player)
	if oldUnit is None or oldUnit.isNone():
		return

	if oldUnit.getID() != kTriggeredData.iUnitId:
		return

	if not _isValidFailedMissionaryChangeUnit(player, oldUnit, kTriggeredData.ePlayer):
		return

	plot = oldUnit.plot()
	if plot is None or plot.isNone():
		return

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return

	iStoredPlotX, iStoredPlotY = _getDiscoveryFailedMissionarySelectedPlot(player)
	if plot.getX() != iStoredPlotX or plot.getY() != iStoredPlotY:
		return

	iTargetUnitClass, iNewUnitType, eTargetProfession = _getBestMissionaryTargetForPlayer(player)
	if iTargetUnitClass == -1 or iNewUnitType == -1 or eTargetProfession == -1:
		return

	iX = oldUnit.getX()
	iY = oldUnit.getY()
	iDamage = oldUnit.getDamage()
	iExperience = oldUnit.getExperience()
	iMoves = oldUnit.movesLeft()

	szName = u""
	if oldUnit.getNameNoDesc():
		szName = oldUnit.getNameNoDesc()

	aPromotions = []
	for iPromotion in range(gc.getNumPromotionInfos()):
		if oldUnit.isHasPromotion(iPromotion):
			aPromotions.append(iPromotion)

	newUnit = player.initUnit(
		iNewUnitType,
		ProfessionTypes.NO_PROFESSION,
		iX,
		iY,
		UnitAITypes.NO_UNITAI,
		DirectionTypes.NO_DIRECTION,
		0
	)
	if newUnit.isNone():
		return

	if eTargetProfession != ProfessionTypes.NO_PROFESSION:
		newUnit.setProfession(eTargetProfession)

	if iDamage > 0:
		newUnit.setDamage(iDamage, PlayerTypes.NO_PLAYER)

	if iExperience > 0:
		newUnit.setExperience(iExperience, -1)

	for iPromotion in aPromotions:
		if newUnit.canAcquirePromotion(iPromotion):
			newUnit.setHasRealPromotion(iPromotion, True)

	if szName != u"":
		newUnit.setName(szName)

	try:
		newUnit.setMoves(iMoves)
	except:
		pass

	oldUnit.kill(False)

	_startDiscoveryFailedMissionaryChangeSoftCooldown(player, 30)
	_clearDiscoveryFailedMissionarySelectedUnitData(player)

def doDiscoveryFailedMissionaryChangeDecline(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	unit = _getStoredDiscoveryFailedMissionaryUnit(player)
	if unit is None or unit.isNone():
		return

	if unit.getID() != kTriggeredData.iUnitId:
		return

	if not _isValidFailedMissionaryChangeUnit(player, unit, kTriggeredData.ePlayer):
		return

	plot = unit.plot()
	if plot is None or plot.isNone():
		return

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return

	iStoredPlotX, iStoredPlotY = _getDiscoveryFailedMissionarySelectedPlot(player)
	if plot.getX() != iStoredPlotX or plot.getY() != iStoredPlotY:
		return

	_startDiscoveryFailedMissionaryChangeSoftCooldown(player, 30)
	_clearDiscoveryFailedMissionarySelectedUnitData(player)

def getHelpDiscoveryFailedMissionaryChange(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return u""

	iCooldown = _getDiscoveryFailedMissionaryChangeScaledTurns(30)

	return localText.getText(
		"TXT_KEY_EVENT_DISCOVERY_EVENTS_FAILED_MISSIONARY_CHANGE_HELP",
		(iCooldown,)
	)

def getHelpDiscoveryFailedMissionaryChangeDecline(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return u""

	iCooldown = _getDiscoveryFailedMissionaryChangeScaledTurns(30)

	return localText.getText(
		"TXT_KEY_EVENT_DISCOVERY_EVENTS_FAILED_MISSIONARY_CHANGE_DECLINE_HELP",
		(iCooldown,)
	)

######## Failed Missionary Classic ###########

FAILED_MISSIONARY_CLASSIC_SOFT_COOLDOWN_PREFIX = "[[WTP_FAILED_MISSIONARY_CLASSIC_SOFT_READY_TURN="
FAILED_MISSIONARY_CLASSIC_SOFT_COOLDOWN_SUFFIX = "]]"

FAILED_MISSIONARY_CLASSIC_SELECTED_UNIT_PREFIX = "[[WTP_FAILED_MISSIONARY_CLASSIC_SELECTED_UNIT="
FAILED_MISSIONARY_CLASSIC_SELECTED_UNIT_SUFFIX = "]]"

FAILED_MISSIONARY_CLASSIC_SELECTED_CITY_PREFIX = "[[WTP_FAILED_MISSIONARY_CLASSIC_SELECTED_CITY="
FAILED_MISSIONARY_CLASSIC_SELECTED_CITY_SUFFIX = "]]"

def _getFailedMissionaryClassicSoftCooldownReadyTurn(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(FAILED_MISSIONARY_CLASSIC_SOFT_COOLDOWN_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(FAILED_MISSIONARY_CLASSIC_SOFT_COOLDOWN_PREFIX)
	iEnd = szData.find(FAILED_MISSIONARY_CLASSIC_SOFT_COOLDOWN_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1

def _setFailedMissionaryClassicSoftCooldownReadyTurn(player, iReadyTurn):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(FAILED_MISSIONARY_CLASSIC_SOFT_COOLDOWN_PREFIX)
	if iStart != -1:
		iEnd = szData.find(FAILED_MISSIONARY_CLASSIC_SOFT_COOLDOWN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(FAILED_MISSIONARY_CLASSIC_SOFT_COOLDOWN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szMarker = "%s%d%s" % (
		FAILED_MISSIONARY_CLASSIC_SOFT_COOLDOWN_PREFIX,
		iReadyTurn,
		FAILED_MISSIONARY_CLASSIC_SOFT_COOLDOWN_SUFFIX
	)

	szData += szMarker
	player.setScriptData(szData)

def _getFailedMissionaryClassicScaledTurns(iBaseTurns):
	gameSpeedType = CyGame().getGameSpeedType()
	iPercent = gc.getGameSpeedInfo(gameSpeedType).getGrowthPercent()
	return max(1, int((iBaseTurns * iPercent) / 100))

def _startFailedMissionaryClassicSoftCooldown(player, iBaseTurns):
	if player.isNone():
		return

	iCooldown = _getFailedMissionaryClassicScaledTurns(iBaseTurns)
	_setFailedMissionaryClassicSoftCooldownReadyTurn(player, CyGame().getGameTurn() + iCooldown)

def _isFailedMissionaryClassicSoftCooldownActive(player):
	if player.isNone():
		return False

	iReadyTurn = _getFailedMissionaryClassicSoftCooldownReadyTurn(player)
	return iReadyTurn > CyGame().getGameTurn()

def _setFailedMissionaryClassicSelectedUnitData(player, iUnitId, iCityId):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(FAILED_MISSIONARY_CLASSIC_SELECTED_UNIT_PREFIX)
	if iStart != -1:
		iEnd = szData.find(FAILED_MISSIONARY_CLASSIC_SELECTED_UNIT_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(FAILED_MISSIONARY_CLASSIC_SELECTED_UNIT_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	iStart = szData.find(FAILED_MISSIONARY_CLASSIC_SELECTED_CITY_PREFIX)
	if iStart != -1:
		iEnd = szData.find(FAILED_MISSIONARY_CLASSIC_SELECTED_CITY_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(FAILED_MISSIONARY_CLASSIC_SELECTED_CITY_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szData += "%s%d%s" % (
		FAILED_MISSIONARY_CLASSIC_SELECTED_UNIT_PREFIX,
		iUnitId,
		FAILED_MISSIONARY_CLASSIC_SELECTED_UNIT_SUFFIX
	)

	szData += "%s%d%s" % (
		FAILED_MISSIONARY_CLASSIC_SELECTED_CITY_PREFIX,
		iCityId,
		FAILED_MISSIONARY_CLASSIC_SELECTED_CITY_SUFFIX
	)

	player.setScriptData(szData)

def _clearFailedMissionaryClassicSelectedUnitData(player):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		return

	iStart = szData.find(FAILED_MISSIONARY_CLASSIC_SELECTED_UNIT_PREFIX)
	if iStart != -1:
		iEnd = szData.find(FAILED_MISSIONARY_CLASSIC_SELECTED_UNIT_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(FAILED_MISSIONARY_CLASSIC_SELECTED_UNIT_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	iStart = szData.find(FAILED_MISSIONARY_CLASSIC_SELECTED_CITY_PREFIX)
	if iStart != -1:
		iEnd = szData.find(FAILED_MISSIONARY_CLASSIC_SELECTED_CITY_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(FAILED_MISSIONARY_CLASSIC_SELECTED_CITY_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	player.setScriptData(szData)

def _getFailedMissionaryClassicSelectedUnitId(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(FAILED_MISSIONARY_CLASSIC_SELECTED_UNIT_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(FAILED_MISSIONARY_CLASSIC_SELECTED_UNIT_PREFIX)
	iEnd = szData.find(FAILED_MISSIONARY_CLASSIC_SELECTED_UNIT_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1

def _getFailedMissionaryClassicSelectedCityId(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(FAILED_MISSIONARY_CLASSIC_SELECTED_CITY_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(FAILED_MISSIONARY_CLASSIC_SELECTED_CITY_PREFIX)
	iEnd = szData.find(FAILED_MISSIONARY_CLASSIC_SELECTED_CITY_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1

def _getFailedMissionaryClassicUnitOnCityPlot(player, city):
	if player.isNone() or city.isNone():
		return None

	plot = city.plot()
	if plot is None or plot.isNone():
		return None

	iFailedMissionaryClass = gc.getInfoTypeForString("UNITCLASS_FAILED_MISSIONARY")
	eColonistProfession = gc.getInfoTypeForString("PROFESSION_COLONIST")

	for i in range(plot.getNumUnits()):
		unit = plot.getUnit(i)
		if unit.isNone():
			continue
		if unit.getOwner() != player.getID():
			continue
		if gc.getUnitInfo(unit.getUnitType()).getUnitClassType() != iFailedMissionaryClass:
			continue
		if unit.getProfession() != eColonistProfession:
			continue
		return unit

	return None

def _findFailedMissionaryClassicCandidate(player):
	if player.isNone():
		return (None, None)

	(city, iter) = player.firstCity(True)
	while city:
		unit = _getFailedMissionaryClassicUnitOnCityPlot(player, city)
		if unit is not None and not unit.isNone():
			return (city, unit)
		(city, iter) = player.nextCity(iter, True)

	return (None, None)

def _getStoredFailedMissionaryClassicUnit(player):
	if player.isNone():
		return None

	iUnitId = _getFailedMissionaryClassicSelectedUnitId(player)
	if iUnitId < 0:
		return None

	unit = player.getUnit(iUnitId)
	if unit is None or unit.isNone():
		return None

	return unit

def _getStoredFailedMissionaryClassicCity(player):
	if player.isNone():
		return None

	iCityId = _getFailedMissionaryClassicSelectedCityId(player)
	if iCityId < 0:
		return None

	city = player.getCity(iCityId)
	if city is None or city.isNone():
		return None

	return city

def _isValidFailedMissionaryClassicUnit(player, city, unit):
	if player.isNone():
		return False
	if not player.isPlayable():
		return False
	if player.isNative():
		return False

	if city is None or city.isNone():
		return False

	if unit is None or unit.isNone():
		return False

	plot = city.plot()
	if plot is None or plot.isNone():
		return False

	if unit.getX() != city.getX() or unit.getY() != city.getY():
		return False

	iFailedMissionaryClass = gc.getInfoTypeForString("UNITCLASS_FAILED_MISSIONARY")
	if gc.getUnitInfo(unit.getUnitType()).getUnitClassType() != iFailedMissionaryClass:
		return False

	eColonistProfession = gc.getInfoTypeForString("PROFESSION_COLONIST")
	if unit.getProfession() != eColonistProfession:
		return False

	return True

def canTriggerFailedMissionaryClassic(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False
	if not player.isPlayable():
		return False
	if player.isNative():
		return False

	if _isFailedMissionaryClassicSoftCooldownActive(player):
		return False

	city, unit = _findFailedMissionaryClassicCandidate(player)
	if city is None or city.isNone() or unit is None or unit.isNone():
		_clearFailedMissionaryClassicSelectedUnitData(player)
		return False

	# Fixed trigger chance on valid city plots
	if CyGame().getSorenRandNum(100, "Failed Missionary Classic trigger") >= 30:
		_clearFailedMissionaryClassicSelectedUnitData(player)
		return False

	_setFailedMissionaryClassicSelectedUnitData(player, unit.getID(), city.getID())
	return True

def doEventFailedMissionary1(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	city = _getStoredFailedMissionaryClassicCity(player)
	unit = _getStoredFailedMissionaryClassicUnit(player)

	if city is None or city.isNone():
		return
	if unit is None or unit.isNone():
		return

	if not _isValidFailedMissionaryClassicUnit(player, city, unit):
		return

	# Original reward must stay
	ChangeFatherPoints(argsList)

	_startFailedMissionaryClassicSoftCooldown(player, 30)
	_clearFailedMissionaryClassicSelectedUnitData(player)

def doEventFailedMissionary2(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	city = _getStoredFailedMissionaryClassicCity(player)
	unit = _getStoredFailedMissionaryClassicUnit(player)

	if city is None or city.isNone():
		return
	if unit is None or unit.isNone():
		return

	if not _isValidFailedMissionaryClassicUnit(player, city, unit):
		return

	_startFailedMissionaryClassicSoftCooldown(player, 30)
	_clearFailedMissionaryClassicSelectedUnitData(player)

def doEventFailedMissionary3(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	city = _getStoredFailedMissionaryClassicCity(player)
	unit = _getStoredFailedMissionaryClassicUnit(player)

	if city is None or city.isNone():
		return
	if unit is None or unit.isNone():
		return

	if not _isValidFailedMissionaryClassicUnit(player, city, unit):
		return

	_startFailedMissionaryClassicSoftCooldown(player, 30)
	_clearFailedMissionaryClassicSelectedUnitData(player)

def expireFailedMissionaryEvent(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return True

	city = _getStoredFailedMissionaryClassicCity(player)
	unit = _getStoredFailedMissionaryClassicUnit(player)

	if city is None or city.isNone():
		return True
	if unit is None or unit.isNone():
		return True

	if not _isValidFailedMissionaryClassicUnit(player, city, unit):
		return True

	return False

def getHelpFailedMissionaryClassic1(argsList):
	iCooldown = _getFailedMissionaryClassicScaledTurns(30)

	szHelp = getHelpChangeFatherPoints(argsList)
	szHelp += u"\n" + localText.getText(
		"TXT_KEY_EVENT_FAILED_MISSIONARY_CLASSIC_COOLDOWN_HELP",
		(iCooldown,)
	)
	return szHelp

def getHelpFailedMissionaryClassic2(argsList):
	iCooldown = _getFailedMissionaryClassicScaledTurns(30)

	return localText.getText(
		"TXT_KEY_EVENT_FAILED_MISSIONARY_CLASSIC_COOLDOWN_HELP",
		(iCooldown,)
	)

def getHelpFailedMissionaryClassic3(argsList):
	iCooldown = _getFailedMissionaryClassicScaledTurns(30)

	return localText.getText(
		"TXT_KEY_EVENT_FAILED_MISSIONARY_CLASSIC_COOLDOWN_HELP",
		(iCooldown,)
	)

######## Native Events ###########

def isNativeVillage(argsList):
	pTriggeredData = argsList[0]
	plot = gc.getMap().plot(pTriggeredData.iPlotX, pTriggeredData.iPlotY)
	player = gc.getPlayer(pTriggeredData.ePlayer)
	if not player.isPlayable():
		return False
	if not plot.isCity():
		return False
	if not gc.getPlayer(plot.getOwner()).isNative():
		return False
	return True

def isNativeVillageAndHuman(argsList):
	pTriggeredData = argsList[0]
	plot = gc.getMap().plot(pTriggeredData.iPlotX, pTriggeredData.iPlotY)
	player = gc.getPlayer(pTriggeredData.ePlayer)
	if not player.isHuman():
		return False
	if not plot.isCity():
		return False
	if not gc.getPlayer(plot.getOwner()).isNative():
		return False
	return True

def isNativeVillageAndHumanTrade(argsList):
	pTriggeredData = argsList[0]
	plot = gc.getMap().plot(pTriggeredData.iPlotX, pTriggeredData.iPlotY)
	player = gc.getPlayer(pTriggeredData.ePlayer)
	if not player.isHuman():
		return False
	if not plot.isCity():
		return False
	if not gc.getPlayer(plot.getOwner()).isNative():
		return False
	iUnitType = CvUtil.findInfoTypeNum('UNIT_TREK')
	iUnitsCurrent = countUnits(argsList, iUnitType)
	if not iUnitsCurrent > 5:
		return False
	return True

######## Apply Chiefs Daughter ###########

def applyChiefsDaughter1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	iUnitClass = event.getGenericParameter(1)
	if iUnitClass == -1:
		return

	iUnitType = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationUnits(iUnitClass)
	if iUnitType == -1:
		return

	player.initUnit(
		iUnitType,
		ProfessionTypes.NO_PROFESSION,
		kTriggeredData.iPlotX,
		kTriggeredData.iPlotY,
		UnitAITypes.NO_UNITAI,
		DirectionTypes.DIRECTION_SOUTH,
		0
	)

def getHelpChiefsDaughter1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)

	iUnitClass = event.getGenericParameter(1)
	if iUnitClass == -1:
		return u""

	iActivePlayer = gc.getGame().getActivePlayer()
	player = gc.getPlayer(iActivePlayer)

	iUnitType = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationUnits(iUnitClass)
	if iUnitType == -1:
		return u""

	szUnit = gc.getUnitInfo(iUnitType).getDescription()

	return localText.getText(
		"TXT_KEY_EVENT_CHIEF_DAUGHTER_HELP",
		(szUnit,)
	)

######## Native Training Event ###########

NATIVE_TRAINING_STATE_PREFIX = "[[WTP_NATIVE_TRAINING_STATE="
NATIVE_TRAINING_STATE_SUFFIX = "]]"

def _getNativeTrainingState(unit):
	if unit is None or unit.isNone():
		return (0, [])

	szData = unit.getScriptData()
	if szData is None or szData == "":
		return (0, [])

	iStart = szData.find(NATIVE_TRAINING_STATE_PREFIX)
	if iStart == -1:
		return (0, [])

	iStart += len(NATIVE_TRAINING_STATE_PREFIX)
	iEnd = szData.find(NATIVE_TRAINING_STATE_SUFFIX, iStart)
	if iEnd == -1:
		return (0, [])

	szPayload = szData[iStart:iEnd]
	if szPayload == "":
		return (0, [])

	aParts = szPayload.split("|", 1)

	try:
		iTrainedCount = int(aParts[0])
	except:
		iTrainedCount = 0

	aHandledVillages = []
	if len(aParts) > 1 and aParts[1] != "":
		for szEntry in aParts[1].split(","):
			if szEntry == "":
				continue
			aHandledVillages.append(szEntry)

	return (iTrainedCount, aHandledVillages)

def _setNativeTrainingState(unit, iTrainedCount, aHandledVillages):
	if unit is None or unit.isNone():
		return

	if iTrainedCount < 0:
		iTrainedCount = 0

	aCleanHandledVillages = []
	for szVillageKey in aHandledVillages:
		if szVillageKey == "":
			continue
		if szVillageKey not in aCleanHandledVillages:
			aCleanHandledVillages.append(szVillageKey)

	szData = unit.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(NATIVE_TRAINING_STATE_PREFIX)
	if iStart != -1:
		iEnd = szData.find(NATIVE_TRAINING_STATE_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(NATIVE_TRAINING_STATE_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szHandled = ",".join(aCleanHandledVillages)
	szMarker = "%s%d|%s%s" % (
		NATIVE_TRAINING_STATE_PREFIX,
		iTrainedCount,
		szHandled,
		NATIVE_TRAINING_STATE_SUFFIX
	)

	szData += szMarker
	unit.setScriptData(szData)

def _getNativeTrainingCount(unit):
	iTrainedCount, aHandledVillages = _getNativeTrainingState(unit)
	return iTrainedCount

def _setNativeTrainingCount(unit, iCount):
	iTrainedCount, aHandledVillages = _getNativeTrainingState(unit)
	_setNativeTrainingState(unit, iCount, aHandledVillages)

def _getNativeTrainingHandledVillages(unit):
	iTrainedCount, aHandledVillages = _getNativeTrainingState(unit)
	return aHandledVillages

def _setNativeTrainingHandledVillages(unit, aHandledVillages):
	iTrainedCount, aOldHandledVillages = _getNativeTrainingState(unit)
	_setNativeTrainingState(unit, iTrainedCount, aHandledVillages)

def _getNativeTrainingVillageKey(plot):
	if plot is None or plot.isNone():
		return ""

	if not plot.isCity():
		return ""

	city = plot.getPlotCity()
	if city is None or city.isNone():
		return ""

	return "%d:%d" % (plot.getOwner(), city.getID())

def _hasNativeTrainingHandledVillage(unit, szVillageKey):
	if szVillageKey == "":
		return False

	aHandledVillages = _getNativeTrainingHandledVillages(unit)
	return szVillageKey in aHandledVillages

def _markNativeTrainingHandledVillage(unit, szVillageKey):
	if szVillageKey == "":
		return

	aHandledVillages = _getNativeTrainingHandledVillages(unit)
	if szVillageKey not in aHandledVillages:
		aHandledVillages.append(szVillageKey)
		_setNativeTrainingHandledVillages(unit, aHandledVillages)

def _showNativeTrainingProgressMessage(player, unit, iCount):
	if player.isNone() or unit is None or unit.isNone():
		return

	if not player.isHuman():
		return

	if iCount <= 1:
		szText = localText.getText("TXT_KEY_EVENT_NATIVE_TRAINING_PROGRESS_1", (unit.getNameKey(),))
	elif iCount == 2:
		szText = localText.getText("TXT_KEY_EVENT_NATIVE_TRAINING_PROGRESS_2", (unit.getNameKey(),))
	else:
		szText = localText.getText("TXT_KEY_EVENT_NATIVE_TRAINING_PROGRESS_3", (unit.getNameKey(),))

	CyInterface().addMessage(
		player.getID(),
		True,
		10,
		szText,
		"",
		0,
		"",
		ColorTypes(8),
		unit.getX(),
		unit.getY(),
		True,
		True
	)

def _showNativeTrainingVisitedVillageMessage(player, unit):
	if player.isNone() or unit is None or unit.isNone():
		return

	if not player.isHuman():
		return

	szText = localText.getText("TXT_KEY_EVENT_NATIVE_TRAINING_PROGRESS_VISITED_ONLY", (unit.getNameKey(),))

	CyInterface().addMessage(
		player.getID(),
		True,
		10,
		szText,
		"",
		0,
		"",
		ColorTypes(8),
		unit.getX(),
		unit.getY(),
		True,
		True
	)

def _getNativeTrainingValidatedData(kTriggeredData):
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return (None, None, None, None, "", None)

	if not player.isPlayable():
		return (None, None, None, None, "", None)

	if player.isNative():
		return (None, None, None, None, "", None)

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return (None, None, None, None, "", None)

	plot = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	if plot.isNone():
		return (None, None, None, None, "", None)

	if unit.getX() != plot.getX() or unit.getY() != plot.getY():
		return (None, None, None, None, "", None)

	if not plot.isCity():
		return (None, None, None, None, "", None)

	city = plot.getPlotCity()
	if city is None or city.isNone():
		return (None, None, None, None, "", None)

	iTribePlayer = plot.getOwner()
	if iTribePlayer < 0:
		return (None, None, None, None, "", None)

	tribePlayer = gc.getPlayer(iTribePlayer)
	if tribePlayer.isNone():
		return (None, None, None, None, "", None)

	if not tribePlayer.isNative():
		return (None, None, None, None, "", None)

	if gc.getTeam(player.getTeam()).isAtWar(tribePlayer.getTeam()):
		return (None, None, None, None, "", None)

	if tribePlayer.AI_getAttitude(player.getID()) < AttitudeTypes.ATTITUDE_CAUTIOUS:
		return (None, None, None, None, "", None)

	if _getNativeTrainingCount(unit) >= 3:
		return (None, None, None, None, "", None)

	szVillageKey = _getNativeTrainingVillageKey(plot)
	if szVillageKey == "":
		return (None, None, None, None, "", None)

	if _hasNativeTrainingHandledVillage(unit, szVillageKey):
		return (None, None, None, None, "", None)

	return (player, unit, plot, city, szVillageKey, tribePlayer)

def canTriggerNativeTraining(argsList):
	kTriggeredData = argsList[0]

	player, unit, plot, city, szVillageKey, tribePlayer = _getNativeTrainingValidatedData(kTriggeredData)
	if player is None:
		return False

	# No extra random chance:
	# once all conditions are fulfilled, the event should trigger reliably
	return True

def applyNativeTraining1(argsList):
	kTriggeredData = argsList[0]

	player, unit, plot, city, szVillageKey, tribePlayer = _getNativeTrainingValidatedData(kTriggeredData)
	if player is None:
		return

	_markNativeTrainingHandledVillage(unit, szVillageKey)

	iNewCount = _getNativeTrainingCount(unit) + 1
	_setNativeTrainingCount(unit, iNewCount)

	_showNativeTrainingProgressMessage(player, unit, iNewCount)

def applyNativeTraining2(argsList):
	kTriggeredData = argsList[0]

	player, unit, plot, city, szVillageKey, tribePlayer = _getNativeTrainingValidatedData(kTriggeredData)
	if player is None:
		return

	_markNativeTrainingHandledVillage(unit, szVillageKey)
	_showNativeTrainingVisitedVillageMessage(player, unit)


####### Stirred up natives events ########
######## Stirred Up Natives Initial event ###########

STIRRED_UP_NATIVES_INITIAL_MIN_MILITARY = 45


def _sunGetContext(kTriggeredData):
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return (None, None, None)

	nativePlayer = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if nativePlayer.isNone() or not nativePlayer.isNative():
		return (None, None, None)

	nativeCity = nativePlayer.getCity(kTriggeredData.iOtherPlayerCityId)
	if nativeCity is None or nativeCity.isNone():
		return (None, None, None)

	if gc.getTeam(player.getTeam()).isAtWar(nativePlayer.getTeam()):
		return (None, None, None)

	if nativePlayer.AI_getAttitude(player.getID()) <= AttitudeTypes.ATTITUDE_FURIOUS:
		return (None, None, None)

	return (player, nativePlayer, nativeCity)


def _sunHasNearbyBorder(player, nativeCity):
	if player.isNone() or nativeCity is None or nativeCity.isNone():
		return False

	iPlayer = player.getID()

	for dx in range(-2, 3):
		for dy in range(-2, 3):
			plot = plotXY(nativeCity.getX(), nativeCity.getY(), dx, dy)
			if plot is None or plot.isNone():
				continue

			if plot.getOwner() == iPlayer:
				if plotDistance(nativeCity.getX(), nativeCity.getY(), plot.getX(), plot.getY()) <= 2:
					return True

	return False


def _sunGetMilitaryPresence(player):
	if player.isNone():
		return 0

	iCount = 0

	aMilitaryProfessions = [
		gc.getInfoTypeForString("PROFESSION_COLONIAL_MILITIA"),
		gc.getInfoTypeForString("PROFESSION_LINE_INFANTRY"),
		gc.getInfoTypeForString("PROFESSION_ROYAL_LIGHT_INFANTRY"),
		gc.getInfoTypeForString("PROFESSION_ROYAL_LINE_INFANTRY"),
		gc.getInfoTypeForString("PROFESSION_DRAGOON"),
		gc.getInfoTypeForString("PROFESSION_ROYAL_DRAGOON"),
		gc.getInfoTypeForString("PROFESSION_HEAVY_CAVALRY"),
		gc.getInfoTypeForString("PROFESSION_ROYAL_CAVALRY"),
		gc.getInfoTypeForString("PROFESSION_LIGHT_ARTILLERY"),
		gc.getInfoTypeForString("PROFESSION_HEAVY_ARTILLERY"),
		gc.getInfoTypeForString("PROFESSION_ROYAL_ARTILLERY"),
	]

	aMilitaryUnitClasses = [
		gc.getInfoTypeForString("UNITCLASS_MORTAR"),
		gc.getInfoTypeForString("UNITCLASS_HESSIAN"),
		gc.getInfoTypeForString("UNITCLASS_CONTINENTAL_GUARD"),
		gc.getInfoTypeForString("UNITCLASS_CONQUISTADOR"),
		gc.getInfoTypeForString("UNITCLASS_MOUNTED_CONQUISTADOR"),
	]

	(unit, iter) = player.firstUnit()
	while unit:
		if unit.getDomainType() == DomainTypes.DOMAIN_LAND:
			if unit.getProfession() in aMilitaryProfessions:
				iCount += 1
			elif unit.getUnitClassType() in aMilitaryUnitClasses:
				iCount += 1

			if iCount >= STIRRED_UP_NATIVES_INITIAL_MIN_MILITARY:
				return iCount

		(unit, iter) = player.nextUnit(iter)

	return iCount


def canTriggerStirredUpNativesInitial(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _sunGetContext(kTriggeredData)
	if player is None:
		return False

	if not _sunHasNearbyBorder(player, nativeCity):
		return False

	if _sunGetMilitaryPresence(player) < STIRRED_UP_NATIVES_INITIAL_MIN_MILITARY:
		return False

	return True
 

######## Stirred Up Natives Horses event ###########

STIRRED_UP_NATIVES_HORSES_YIELD = "YIELD_HORSES"
STIRRED_UP_NATIVES_HORSES_DELIVERY_AMOUNT = 100
STIRRED_UP_NATIVES_HORSES_IMMEDIATE_AMOUNT = 50


def _sunHorsesKeyActive():
	return "[[WTP_STIRRED_UP_NATIVES_HORSES_ACTIVE]]"


def _sunHorsesKeyCompleted():
	return "[[WTP_STIRRED_UP_NATIVES_HORSES_COMPLETED]]"


def _sunHorsesKeyTarget():
	return "[[WTP_STIRRED_UP_NATIVES_HORSES_TARGET="


def _sunHorsesKeyNativePlayer():
	return "[[WTP_STIRRED_UP_NATIVES_HORSES_NATIVE_PLAYER="


def _sunHorsesKeyNativeCity():
	return "[[WTP_STIRRED_UP_NATIVES_HORSES_NATIVE_CITY="


def _sunHorsesKeyPlotX():
	return "[[WTP_STIRRED_UP_NATIVES_HORSES_PLOT_X="


def _sunHorsesKeyPlotY():
	return "[[WTP_STIRRED_UP_NATIVES_HORSES_PLOT_Y="


def _sunHorsesData(player):
	if player.isNone():
		return ""

	szData = player.getScriptData()
	if szData is None:
		return ""

	return szData


def _sunHorsesHas(player, key):
	return key in _sunHorsesData(player)


def _sunHorsesAdd(player, key):
	szData = _sunHorsesData(player)
	if key not in szData:
		player.setScriptData(szData + key)


def _sunHorsesRemove(player, key):
	szData = _sunHorsesData(player)
	if key in szData:
		player.setScriptData(szData.replace(key, ""))


def _sunHorsesGetNumber(player, key):
	szData = _sunHorsesData(player)
	iStart = szData.find(key)
	if iStart == -1:
		return -1

	iStart += len(key)
	iEnd = szData.find("]]", iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1


def _sunHorsesSetNumber(player, key, iValue):
	szData = _sunHorsesData(player)

	iStart = szData.find(key)
	if iStart != -1:
		iEnd = szData.find("]]", iStart)
		if iEnd != -1:
			szData = szData[:iStart] + szData[iEnd + 2:]

	player.setScriptData(szData + "%s%d]]" % (key, iValue))


def _sunHorsesRemoveNumber(player, key):
	szData = _sunHorsesData(player)

	iStart = szData.find(key)
	if iStart != -1:
		iEnd = szData.find("]]", iStart)
		if iEnd != -1:
			player.setScriptData(szData[:iStart] + szData[iEnd + 2:])


def _sunHorsesScaledAmount(iBaseAmount):
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	return max(1, iBaseAmount * Speed.getStoragePercent() / 100)


def _sunGetHorsesContext(kTriggeredData):
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return (None, None, None)

	nativePlayer = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if nativePlayer.isNone() or not nativePlayer.isNative():
		return (None, None, None)

	nativeCity = nativePlayer.getCity(kTriggeredData.iOtherPlayerCityId)
	if nativeCity is None or nativeCity.isNone():
		return (None, None, None)

	if gc.getTeam(player.getTeam()).isAtWar(nativePlayer.getTeam()):
		return (None, None, None)

	if not _sunHasNearbyBorder(player, nativeCity):
		return (None, None, None)

	return (player, nativePlayer, nativeCity)


def canTriggerStirredUpNativesHorses(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _sunGetHorsesContext(kTriggeredData)
	if player is None:
		return False

	if _sunHorsesHas(player, _sunHorsesKeyCompleted()):
		return False

	if _sunHorsesHas(player, _sunHorsesKeyActive()):
		return False

	return True


def applyStirredUpNativesHorsesQuestStart(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _sunGetHorsesContext(kTriggeredData)
	if player is None:
		return

	iYield = gc.getInfoTypeForString(STIRRED_UP_NATIVES_HORSES_YIELD)
	iAmount = _sunHorsesScaledAmount(STIRRED_UP_NATIVES_HORSES_DELIVERY_AMOUNT)
	iTarget = nativeCity.getYieldStored(iYield) + iAmount

	_sunHorsesAdd(player, _sunHorsesKeyActive())
	_sunHorsesSetNumber(player, _sunHorsesKeyTarget(), iTarget)
	_sunHorsesSetNumber(player, _sunHorsesKeyNativePlayer(), nativePlayer.getID())
	_sunHorsesSetNumber(player, _sunHorsesKeyNativeCity(), nativeCity.getID())
	_sunHorsesSetNumber(player, _sunHorsesKeyPlotX(), nativeCity.getX())
	_sunHorsesSetNumber(player, _sunHorsesKeyPlotY(), nativeCity.getY())

	if player.isHuman():
		CyInterface().addMessage(
			kTriggeredData.ePlayer,
			True,
			gc.getEVENT_MESSAGE_TIME(),
			localText.getText("TXT_KEY_EVENT_STIRRED_UP_NATIVES_HORSES_TARGET_MARKER", (nativeCity.getNameKey(),)),
			"",
			InterfaceMessageTypes.MESSAGE_TYPE_INFO,
			"",
			ColorTypes(8),
			nativeCity.getX(),
			nativeCity.getY(),
			True,
			True
		)


def isExpiredStirredUpNativesHorses(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return True

	if not _sunHorsesHas(player, _sunHorsesKeyActive()):
		return True

	iExpireTurns = event.getGenericParameter(1)
	if iExpireTurns < 1:
		iExpireTurns = 20

	if CyGame().getGameTurn() < kTriggeredData.iTurn + iExpireTurns:
		return False

	iRequiredNativePlayer = _sunHorsesGetNumber(player, _sunHorsesKeyNativePlayer())
	if iRequiredNativePlayer >= 0:
		nativePlayer = gc.getPlayer(iRequiredNativePlayer)
		if not nativePlayer.isNone() and nativePlayer.isNative():
			iAttitude = event.getGenericParameter(2)
			if iAttitude == 0:
				iAttitude = -5

			player.AI_changeAttitudeExtra(nativePlayer.getID(), iAttitude)
			nativePlayer.AI_changeAttitudeExtra(player.getID(), iAttitude)

	_sunHorsesRemove(player, _sunHorsesKeyActive())
	_sunHorsesRemoveNumber(player, _sunHorsesKeyTarget())
	_sunHorsesRemoveNumber(player, _sunHorsesKeyNativePlayer())
	_sunHorsesRemoveNumber(player, _sunHorsesKeyNativeCity())
	_sunHorsesRemoveNumber(player, _sunHorsesKeyPlotX())
	_sunHorsesRemoveNumber(player, _sunHorsesKeyPlotY())

	return True


def canDoStirredUpNativesHorsesImmediate(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _sunGetHorsesContext(kTriggeredData)
	if player is None:
		return False

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return False

	iYield = gc.getInfoTypeForString(STIRRED_UP_NATIVES_HORSES_YIELD)
	iAmount = _sunHorsesScaledAmount(STIRRED_UP_NATIVES_HORSES_IMMEDIATE_AMOUNT)

	if city.getYieldStored(iYield) < iAmount:
		return False

	return True


def applyStirredUpNativesHorsesImmediate(argsList):
	kTriggeredData = argsList[0]

	player, nativePlayer, nativeCity = _sunGetHorsesContext(kTriggeredData)
	if player is None:
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return

	iYield = gc.getInfoTypeForString(STIRRED_UP_NATIVES_HORSES_YIELD)
	iAmount = _sunHorsesScaledAmount(STIRRED_UP_NATIVES_HORSES_IMMEDIATE_AMOUNT)

	if city.getYieldStored(iYield) < iAmount:
		return

	city.changeYieldStored(iYield, -iAmount)
	nativeCity.changeYieldStored(iYield, iAmount)


def canTriggerStirredUpNativesHorsesDone(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	if not _sunHorsesHas(player, _sunHorsesKeyActive()):
		return False

	iRequiredNativePlayer = _sunHorsesGetNumber(player, _sunHorsesKeyNativePlayer())
	if iRequiredNativePlayer < 0:
		return False

	nativePlayer = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if nativePlayer.isNone() or not nativePlayer.isNative():
		return False

	if nativePlayer.getID() != iRequiredNativePlayer:
		return False

	if gc.getTeam(player.getTeam()).isAtWar(nativePlayer.getTeam()):
		return False

	iRequiredNativeCity = _sunHorsesGetNumber(player, _sunHorsesKeyNativeCity())
	if iRequiredNativeCity < 0:
		return False

	nativeCity = nativePlayer.getCity(iRequiredNativeCity)
	if nativeCity is None or nativeCity.isNone():
		return False

	iTarget = _sunHorsesGetNumber(player, _sunHorsesKeyTarget())
	if iTarget < 0:
		return False

	iYield = gc.getInfoTypeForString(STIRRED_UP_NATIVES_HORSES_YIELD)
	if nativeCity.getYieldStored(iYield) < iTarget:
		return False

	return True


def applyStirredUpNativesHorsesDone(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return

	if not _sunHorsesHas(player, _sunHorsesKeyActive()):
		return

	iRequiredNativePlayer = _sunHorsesGetNumber(player, _sunHorsesKeyNativePlayer())
	if iRequiredNativePlayer < 0:
		return

	nativePlayer = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if nativePlayer.isNone() or not nativePlayer.isNative():
		return

	if nativePlayer.getID() != iRequiredNativePlayer:
		return

	iRequiredNativeCity = _sunHorsesGetNumber(player, _sunHorsesKeyNativeCity())
	if iRequiredNativeCity < 0:
		return

	nativeCity = nativePlayer.getCity(iRequiredNativeCity)
	if nativeCity is None or nativeCity.isNone():
		return

	iTarget = _sunHorsesGetNumber(player, _sunHorsesKeyTarget())
	if iTarget < 0:
		return

	iYield = gc.getInfoTypeForString(STIRRED_UP_NATIVES_HORSES_YIELD)
	if nativeCity.getYieldStored(iYield) < iTarget:
		return

	_sunHorsesRemove(player, _sunHorsesKeyActive())
	_sunHorsesRemoveNumber(player, _sunHorsesKeyTarget())
	_sunHorsesRemoveNumber(player, _sunHorsesKeyNativePlayer())
	_sunHorsesRemoveNumber(player, _sunHorsesKeyNativeCity())
	_sunHorsesRemoveNumber(player, _sunHorsesKeyPlotX())
	_sunHorsesRemoveNumber(player, _sunHorsesKeyPlotY())
	_sunHorsesAdd(player, _sunHorsesKeyCompleted())


def getHelpStirredUpNativesHorsesQuestStart(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player, nativePlayer, nativeCity = _sunGetHorsesContext(kTriggeredData)
	if player is None:
		return u""

	iAmount = _sunHorsesScaledAmount(STIRRED_UP_NATIVES_HORSES_DELIVERY_AMOUNT)
	iTurns = event.getGenericParameter(1)

	return localText.getText(
		"TXT_KEY_EVENT_STIRRED_UP_NATIVES_HORSES_QUEST_HELP",
		(iAmount, iTurns, nativeCity.getNameKey())
	)

def getHelpStirredUpNativesHorsesImmediate(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _sunGetHorsesContext(kTriggeredData)
	if player is None:
		return u""

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return u""

	iYield = gc.getInfoTypeForString(STIRRED_UP_NATIVES_HORSES_YIELD)
	iAmount = _sunHorsesScaledAmount(STIRRED_UP_NATIVES_HORSES_IMMEDIATE_AMOUNT)

	return localText.getText(
		"TXT_KEY_EVENT_STIRRED_UP_NATIVES_HORSES_IMMEDIATE_HELP",
		(iAmount, gc.getYieldInfo(iYield).getDescription(), city.getNameKey())
	)


######## Stirred Up Natives Muskets event ###########

STIRRED_UP_NATIVES_MUSKETS_YIELD = "YIELD_MUSKETS"
STIRRED_UP_NATIVES_MUSKETS_DELIVERY_AMOUNT = 100
STIRRED_UP_NATIVES_MUSKETS_IMMEDIATE_AMOUNT = 50


def _sunMusketsKeyActive():
	return "[[WTP_STIRRED_UP_NATIVES_MUSKETS_ACTIVE]]"


def _sunMusketsKeyCompleted():
	return "[[WTP_STIRRED_UP_NATIVES_MUSKETS_COMPLETED]]"


def _sunMusketsKeyTarget():
	return "[[WTP_STIRRED_UP_NATIVES_MUSKETS_TARGET="


def _sunMusketsKeyNativePlayer():
	return "[[WTP_STIRRED_UP_NATIVES_MUSKETS_NATIVE_PLAYER="


def _sunMusketsKeyNativeCity():
	return "[[WTP_STIRRED_UP_NATIVES_MUSKETS_NATIVE_CITY="


def _sunMusketsKeyPlotX():
	return "[[WTP_STIRRED_UP_NATIVES_MUSKETS_PLOT_X="


def _sunMusketsKeyPlotY():
	return "[[WTP_STIRRED_UP_NATIVES_MUSKETS_PLOT_Y="


def _sunMusketsData(player):
	if player.isNone():
		return ""

	szData = player.getScriptData()
	if szData is None:
		return ""

	return szData


def _sunMusketsHas(player, key):
	return key in _sunMusketsData(player)


def _sunMusketsAdd(player, key):
	szData = _sunMusketsData(player)
	if key not in szData:
		player.setScriptData(szData + key)


def _sunMusketsRemove(player, key):
	szData = _sunMusketsData(player)
	if key in szData:
		player.setScriptData(szData.replace(key, ""))


def _sunMusketsGetNumber(player, key):
	szData = _sunMusketsData(player)
	iStart = szData.find(key)
	if iStart == -1:
		return -1

	iStart += len(key)
	iEnd = szData.find("]]", iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1


def _sunMusketsSetNumber(player, key, iValue):
	szData = _sunMusketsData(player)

	iStart = szData.find(key)
	if iStart != -1:
		iEnd = szData.find("]]", iStart)
		if iEnd != -1:
			szData = szData[:iStart] + szData[iEnd + 2:]

	player.setScriptData(szData + "%s%d]]" % (key, iValue))


def _sunMusketsRemoveNumber(player, key):
	szData = _sunMusketsData(player)

	iStart = szData.find(key)
	if iStart != -1:
		iEnd = szData.find("]]", iStart)
		if iEnd != -1:
			player.setScriptData(szData[:iStart] + szData[iEnd + 2:])


def _sunMusketsScaledAmount(iBaseAmount):
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	return max(1, iBaseAmount * Speed.getStoragePercent() / 100)


def _sunGetMusketsContext(kTriggeredData):
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return (None, None, None)

	nativePlayer = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if nativePlayer.isNone() or not nativePlayer.isNative():
		return (None, None, None)

	nativeCity = nativePlayer.getCity(kTriggeredData.iOtherPlayerCityId)
	if nativeCity is None or nativeCity.isNone():
		return (None, None, None)

	if gc.getTeam(player.getTeam()).isAtWar(nativePlayer.getTeam()):
		return (None, None, None)

	if not _sunHasNearbyBorder(player, nativeCity):
		return (None, None, None)

	return (player, nativePlayer, nativeCity)


def canTriggerStirredUpNativesMuskets(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _sunGetMusketsContext(kTriggeredData)
	if player is None:
		return False

	if _sunMusketsHas(player, _sunMusketsKeyCompleted()):
		return False

	if _sunMusketsHas(player, _sunMusketsKeyActive()):
		return False

	return True


def applyStirredUpNativesMusketsQuestStart(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _sunGetMusketsContext(kTriggeredData)
	if player is None:
		return

	iYield = gc.getInfoTypeForString(STIRRED_UP_NATIVES_MUSKETS_YIELD)
	iAmount = _sunMusketsScaledAmount(STIRRED_UP_NATIVES_MUSKETS_DELIVERY_AMOUNT)
	iTarget = nativeCity.getYieldStored(iYield) + iAmount

	_sunMusketsAdd(player, _sunMusketsKeyActive())
	_sunMusketsSetNumber(player, _sunMusketsKeyTarget(), iTarget)
	_sunMusketsSetNumber(player, _sunMusketsKeyNativePlayer(), nativePlayer.getID())
	_sunMusketsSetNumber(player, _sunMusketsKeyNativeCity(), nativeCity.getID())
	_sunMusketsSetNumber(player, _sunMusketsKeyPlotX(), nativeCity.getX())
	_sunMusketsSetNumber(player, _sunMusketsKeyPlotY(), nativeCity.getY())

	if player.isHuman():
		CyInterface().addMessage(
			kTriggeredData.ePlayer,
			True,
			gc.getEVENT_MESSAGE_TIME(),
			localText.getText("TXT_KEY_EVENT_STIRRED_UP_NATIVES_MUSKETS_TARGET_MARKER", (nativeCity.getNameKey(),)),
			"",
			InterfaceMessageTypes.MESSAGE_TYPE_INFO,
			"",
			ColorTypes(8),
			nativeCity.getX(),
			nativeCity.getY(),
			True,
			True
		)


def canDoStirredUpNativesMusketsImmediate(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _sunGetMusketsContext(kTriggeredData)
	if player is None:
		return False

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return False

	iYield = gc.getInfoTypeForString(STIRRED_UP_NATIVES_MUSKETS_YIELD)
	iAmount = _sunMusketsScaledAmount(STIRRED_UP_NATIVES_MUSKETS_IMMEDIATE_AMOUNT)

	if city.getYieldStored(iYield) < iAmount:
		return False

	return True


def applyStirredUpNativesMusketsImmediate(argsList):
	kTriggeredData = argsList[0]

	player, nativePlayer, nativeCity = _sunGetMusketsContext(kTriggeredData)
	if player is None:
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return

	iYield = gc.getInfoTypeForString(STIRRED_UP_NATIVES_MUSKETS_YIELD)
	iAmount = _sunMusketsScaledAmount(STIRRED_UP_NATIVES_MUSKETS_IMMEDIATE_AMOUNT)

	if city.getYieldStored(iYield) < iAmount:
		return

	city.changeYieldStored(iYield, -iAmount)
	nativeCity.changeYieldStored(iYield, iAmount)


def isExpiredStirredUpNativesMuskets(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return True

	if not _sunMusketsHas(player, _sunMusketsKeyActive()):
		return True

	iExpireTurns = event.getGenericParameter(1)
	if iExpireTurns < 1:
		iExpireTurns = 20

	if CyGame().getGameTurn() < kTriggeredData.iTurn + iExpireTurns:
		return False

	iRequiredNativePlayer = _sunMusketsGetNumber(player, _sunMusketsKeyNativePlayer())
	if iRequiredNativePlayer >= 0:
		nativePlayer = gc.getPlayer(iRequiredNativePlayer)
		if not nativePlayer.isNone() and nativePlayer.isNative():
			iAttitude = event.getGenericParameter(2)
			if iAttitude == 0:
				iAttitude = -5

			player.AI_changeAttitudeExtra(nativePlayer.getID(), iAttitude)
			nativePlayer.AI_changeAttitudeExtra(player.getID(), iAttitude)

	_sunMusketsRemove(player, _sunMusketsKeyActive())
	_sunMusketsRemoveNumber(player, _sunMusketsKeyTarget())
	_sunMusketsRemoveNumber(player, _sunMusketsKeyNativePlayer())
	_sunMusketsRemoveNumber(player, _sunMusketsKeyNativeCity())
	_sunMusketsRemoveNumber(player, _sunMusketsKeyPlotX())
	_sunMusketsRemoveNumber(player, _sunMusketsKeyPlotY())

	return True


def canTriggerStirredUpNativesMusketsDone(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	if not _sunMusketsHas(player, _sunMusketsKeyActive()):
		return False

	iRequiredNativePlayer = _sunMusketsGetNumber(player, _sunMusketsKeyNativePlayer())
	if iRequiredNativePlayer < 0:
		return False

	nativePlayer = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if nativePlayer.isNone() or not nativePlayer.isNative():
		return False

	if nativePlayer.getID() != iRequiredNativePlayer:
		return False

	if gc.getTeam(player.getTeam()).isAtWar(nativePlayer.getTeam()):
		return False

	iRequiredNativeCity = _sunMusketsGetNumber(player, _sunMusketsKeyNativeCity())
	if iRequiredNativeCity < 0:
		return False

	nativeCity = nativePlayer.getCity(iRequiredNativeCity)
	if nativeCity is None or nativeCity.isNone():
		return False

	iTarget = _sunMusketsGetNumber(player, _sunMusketsKeyTarget())
	if iTarget < 0:
		return False

	iYield = gc.getInfoTypeForString(STIRRED_UP_NATIVES_MUSKETS_YIELD)
	if nativeCity.getYieldStored(iYield) < iTarget:
		return False

	return True


def applyStirredUpNativesMusketsDone(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return

	if not _sunMusketsHas(player, _sunMusketsKeyActive()):
		return

	iRequiredNativePlayer = _sunMusketsGetNumber(player, _sunMusketsKeyNativePlayer())
	if iRequiredNativePlayer < 0:
		return

	nativePlayer = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if nativePlayer.isNone() or not nativePlayer.isNative():
		return

	if nativePlayer.getID() != iRequiredNativePlayer:
		return

	iRequiredNativeCity = _sunMusketsGetNumber(player, _sunMusketsKeyNativeCity())
	if iRequiredNativeCity < 0:
		return

	nativeCity = nativePlayer.getCity(iRequiredNativeCity)
	if nativeCity is None or nativeCity.isNone():
		return

	iTarget = _sunMusketsGetNumber(player, _sunMusketsKeyTarget())
	if iTarget < 0:
		return

	iYield = gc.getInfoTypeForString(STIRRED_UP_NATIVES_MUSKETS_YIELD)
	if nativeCity.getYieldStored(iYield) < iTarget:
		return

	_sunMusketsRemove(player, _sunMusketsKeyActive())
	_sunMusketsRemoveNumber(player, _sunMusketsKeyTarget())
	_sunMusketsRemoveNumber(player, _sunMusketsKeyNativePlayer())
	_sunMusketsRemoveNumber(player, _sunMusketsKeyNativeCity())
	_sunMusketsRemoveNumber(player, _sunMusketsKeyPlotX())
	_sunMusketsRemoveNumber(player, _sunMusketsKeyPlotY())
	_sunMusketsAdd(player, _sunMusketsKeyCompleted())


def getHelpStirredUpNativesMusketsQuestStart(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player, nativePlayer, nativeCity = _sunGetMusketsContext(kTriggeredData)
	if player is None:
		return u""

	iAmount = _sunMusketsScaledAmount(STIRRED_UP_NATIVES_MUSKETS_DELIVERY_AMOUNT)
	iTurns = event.getGenericParameter(1)

	return localText.getText(
		"TXT_KEY_EVENT_STIRRED_UP_NATIVES_MUSKETS_QUEST_HELP",
		(iAmount, iTurns, nativeCity.getNameKey())
	)


def getHelpStirredUpNativesMusketsImmediate(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _sunGetMusketsContext(kTriggeredData)
	if player is None:
		return u""

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return u""

	iAmount = _sunMusketsScaledAmount(STIRRED_UP_NATIVES_MUSKETS_IMMEDIATE_AMOUNT)

	return localText.getText(
		"TXT_KEY_EVENT_STIRRED_UP_NATIVES_MUSKETS_IMMEDIATE_HELP",
		(iAmount, city.getNameKey())
	)


def getHelpStirredUpNativesMusketsDone(argsList):
	return localText.getText(
		"TXT_KEY_EVENT_STIRRED_UP_NATIVES_MUSKETS_DONE_HELP",
		(1,)
	)


######## Native Land Return Event ###########

NATIVE_LAND_RETURN_MIN_OWNED_PLOTS = 3
NATIVE_LAND_RETURN_PLOTS_TO_RETURN = 2
NATIVE_LAND_RETURN_RADIUS = 2
NATIVE_LAND_RETURN_BASE_GOLD = 1000
NATIVE_LAND_RETURN_LOCK_TURNS = 30


def _nlrCompletedKey():
	return "[[WTP_NATIVE_LAND_RETURN_COMPLETED]]"


def _nlrLockKeyX(iIndex):
	return "[[WTP_NATIVE_LAND_RETURN_LOCK_X_%d=" % iIndex


def _nlrLockKeyY(iIndex):
	return "[[WTP_NATIVE_LAND_RETURN_LOCK_Y_%d=" % iIndex


def _nlrLockKeyOwner(iIndex):
	return "[[WTP_NATIVE_LAND_RETURN_LOCK_OWNER_%d=" % iIndex


def _nlrLockKeyCity(iIndex):
	return "[[WTP_NATIVE_LAND_RETURN_LOCK_CITY_%d=" % iIndex


def _nlrLockKeyUntil(iIndex):
	return "[[WTP_NATIVE_LAND_RETURN_LOCK_UNTIL_%d=" % iIndex


def _nlrData(player):
	if player.isNone():
		return ""

	szData = player.getScriptData()
	if szData is None:
		return ""

	return szData


def _nlrHas(player, key):
	return key in _nlrData(player)


def _nlrAdd(player, key):
	szData = _nlrData(player)
	if key not in szData:
		player.setScriptData(szData + key)


def _nlrGetNumber(player, key):
	szData = _nlrData(player)

	iStart = szData.find(key)
	if iStart == -1:
		return -1

	iStart += len(key)
	iEnd = szData.find("]]", iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1


def _nlrRemoveNumber(player, key):
	szData = _nlrData(player)

	while True:
		iStart = szData.find(key)
		if iStart == -1:
			break

		iEnd = szData.find("]]", iStart)
		if iEnd == -1:
			break

		szData = szData[:iStart] + szData[iEnd + 2:]

	player.setScriptData(szData)


def _nlrSetNumber(player, key, iValue):
	_nlrRemoveNumber(player, key)

	szData = _nlrData(player)
	player.setScriptData(szData + "%s%d]]" % (key, iValue))


def _nlrClearReturnedPlot(player, iIndex):
	_nlrRemoveNumber(player, _nlrLockKeyX(iIndex))
	_nlrRemoveNumber(player, _nlrLockKeyY(iIndex))
	_nlrRemoveNumber(player, _nlrLockKeyOwner(iIndex))
	_nlrRemoveNumber(player, _nlrLockKeyCity(iIndex))
	_nlrRemoveNumber(player, _nlrLockKeyUntil(iIndex))


def _nlrClearAllReturnedPlots(player):
	for iIndex in range(NATIVE_LAND_RETURN_PLOTS_TO_RETURN):
		_nlrClearReturnedPlot(player, iIndex)


def _nlrScaledGold():
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	return max(1, NATIVE_LAND_RETURN_BASE_GOLD * Speed.getStoragePercent() / 100)


def _nlrContext(kTriggeredData):
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isHuman() or player.isNative():
		return (None, None, None)

	if _nlrHas(player, _nlrCompletedKey()):
		return (None, None, None)

	nativePlayer = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if nativePlayer.isNone() or not nativePlayer.isNative():
		return (None, None, None)

	nativeCity = nativePlayer.getCity(kTriggeredData.iOtherPlayerCityId)
	if nativeCity is None or nativeCity.isNone():
		return (None, None, None)

	if gc.getTeam(player.getTeam()).isAtWar(nativePlayer.getTeam()):
		return (None, None, None)

	return (player, nativePlayer, nativeCity)


def _nlrPlots(player, nativeCity):
	aPlots = []

	for iDX in range(-NATIVE_LAND_RETURN_RADIUS, NATIVE_LAND_RETURN_RADIUS + 1):
		for iDY in range(-NATIVE_LAND_RETURN_RADIUS, NATIVE_LAND_RETURN_RADIUS + 1):
			plot = plotXY(nativeCity.getX(), nativeCity.getY(), iDX, iDY)

			if plot is None or plot.isNone():
				continue

			if plot.isWater() or plot.isCity():
				continue

			if plot.getOwner() == player.getID():
				aPlots.append(plot)

	return aPlots


def _nlrForcePlotToNative(player, nativePlayer, plot):
	if player.isNone() or nativePlayer.isNone():
		return

	if plot is None or plot.isNone():
		return

	if plot.isCity():
		return

	iPlayer = player.getID()
	iNative = nativePlayer.getID()

	try:
		plot.setCulture(iPlayer, 0, True)
	except:
		try:
			plot.setCulture(iPlayer, 0)
		except:
			pass

	try:
		plot.changeCulture(iNative, 1000, True)
	except:
		try:
			plot.changeCulture(iNative, 1000)
		except:
			pass

	plot.setOwner(iNative)


def _nlrSaveReturnedPlot(player, iIndex, plot, nativePlayer, nativeCity):
	_nlrSetNumber(player, _nlrLockKeyX(iIndex), plot.getX())
	_nlrSetNumber(player, _nlrLockKeyY(iIndex), plot.getY())
	_nlrSetNumber(player, _nlrLockKeyOwner(iIndex), nativePlayer.getID())
	_nlrSetNumber(player, _nlrLockKeyCity(iIndex), nativeCity.getID())
	_nlrSetNumber(player, _nlrLockKeyUntil(iIndex), CyGame().getGameTurn() + NATIVE_LAND_RETURN_LOCK_TURNS)


def _nlrMaintainReturnedPlots(player):
	if player.isNone():
		return False

	iCurrentTurn = CyGame().getGameTurn()

	for iIndex in range(NATIVE_LAND_RETURN_PLOTS_TO_RETURN):
		iX = _nlrGetNumber(player, _nlrLockKeyX(iIndex))
		iY = _nlrGetNumber(player, _nlrLockKeyY(iIndex))
		iOwner = _nlrGetNumber(player, _nlrLockKeyOwner(iIndex))
		iCity = _nlrGetNumber(player, _nlrLockKeyCity(iIndex))
		iUntil = _nlrGetNumber(player, _nlrLockKeyUntil(iIndex))

		if iX < 0 or iY < 0 or iOwner < 0 or iCity < 0 or iUntil < 0:
			continue

		# Lock expired: apply penalty to the original native owner and stop touching the plots.
		if iCurrentTurn >= iUntil:
			nativePlayer = gc.getPlayer(iOwner)

			if not nativePlayer.isNone() and nativePlayer.isNative() and nativePlayer.isAlive():
				nativePlayer.AI_changeAttitudeExtra(player.getID(), -4)

				if player.isHuman():
					CyInterface().addMessage(
						player.getID(),
						True,
						gc.getEVENT_MESSAGE_TIME(),
						localText.getText(
							"TXT_KEY_EVENT_NATIVE_LAND_RETURN_MAINTAIN",
							(nativePlayer.getCivilizationDescription(0),)
						),
						"",
						InterfaceMessageTypes.MESSAGE_TYPE_INFO,
						"",
						gc.getInfoTypeForString("COLOR_RED"),
						iX,
						iY,
						True,
						True
					)

			_nlrClearAllReturnedPlots(player)
			return False

		plot = CyMap().plot(iX, iY)
		if plot is None or plot.isNone():
			_nlrClearReturnedPlot(player, iIndex)
			continue

		if plot.isCity():
			_nlrClearReturnedPlot(player, iIndex)
			continue

		nativePlayer = gc.getPlayer(iOwner)
		if nativePlayer.isNone() or not nativePlayer.isNative() or not nativePlayer.isAlive():
			_nlrClearAllReturnedPlots(player)
			return False

		nativeCity = nativePlayer.getCity(iCity)
		if nativeCity is None or nativeCity.isNone():
			_nlrClearAllReturnedPlots(player)
			return False

		# Keep returned plots under native ownership and suppress European culture during lock period.
		_nlrForcePlotToNative(player, nativePlayer, plot)

	return False


def canTriggerNativeLandReturn(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _nlrContext(kTriggeredData)
	if player is None:
		return False

	return len(_nlrPlots(player, nativeCity)) >= NATIVE_LAND_RETURN_MIN_OWNED_PLOTS


def canDoNativeLandReturnPlots(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _nlrContext(kTriggeredData)
	if player is None:
		return False

	return len(_nlrPlots(player, nativeCity)) >= NATIVE_LAND_RETURN_PLOTS_TO_RETURN


def _nlrMoveUnits(player, sourcePlot):
	targetPlot = None

	for iRadius in range(1, 6):
		for iDX in range(-iRadius, iRadius + 1):
			for iDY in range(-iRadius, iRadius + 1):
				plot = plotXY(sourcePlot.getX(), sourcePlot.getY(), iDX, iDY)

				if plot is None or plot.isNone():
					continue

				if plot.isWater():
					continue

				if plot.getOwner() != player.getID():
					continue

				targetPlot = plot
				break

			if targetPlot is not None:
				break

		if targetPlot is not None:
			break

	if targetPlot is None:
		return

	aUnits = []

	for i in range(sourcePlot.getNumUnits()):
		unit = sourcePlot.getUnit(i)
		if not unit.isNone() and unit.getOwner() == player.getID():
			aUnits.append(unit)

	for unit in aUnits:
		unit.setXY(targetPlot.getX(), targetPlot.getY())


def applyNativeLandReturnPlots(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _nlrContext(kTriggeredData)
	if player is None:
		return

	iReturned = 0

	for plot in _nlrPlots(player, nativeCity):
		if iReturned >= NATIVE_LAND_RETURN_PLOTS_TO_RETURN:
			break

		_nlrMoveUnits(player, plot)

		CyEngine().triggerEffect(
			gc.getInfoTypeForString("EFFECT_CITY_FIRE"),
			plot.getPoint()
		)

		if plot.getImprovementType() != -1:
			plot.setImprovementType(-1)

		_nlrForcePlotToNative(player, nativePlayer, plot)
		_nlrSaveReturnedPlot(player, iReturned, plot, nativePlayer, nativeCity)

		iReturned += 1

	if iReturned > 0:
		_nlrAdd(player, _nlrCompletedKey())


def canDoNativeLandReturnGold(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _nlrContext(kTriggeredData)
	if player is None:
		return False

	return player.getGold() >= _nlrScaledGold()


def applyNativeLandReturnGold(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _nlrContext(kTriggeredData)
	if player is None:
		return

	iGold = _nlrScaledGold()
	if player.getGold() < iGold:
		return

	player.changeGold(-iGold)
	nativePlayer.changeGold(iGold)
	_nlrAdd(player, _nlrCompletedKey())


def applyNativeLandReturnRefuse(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _nlrContext(kTriggeredData)
	if player is None:
		return

	_nlrAdd(player, _nlrCompletedKey())


def canTriggerNativeLandReturnMaintain(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone() or not player.isAlive():
		return False

	return _nlrMaintainReturnedPlots(player)


def getHelpNativeLandReturnPlots(argsList):
	return localText.getText(
		"TXT_KEY_EVENT_NATIVE_LAND_RETURN_HELP_PLOTS",
		(NATIVE_LAND_RETURN_PLOTS_TO_RETURN,)
	)


def getHelpNativeLandReturnRefuse(argsList):
	return localText.getText(
		"TXT_KEY_EVENT_NATIVE_LAND_RETURN_HELP_REFUSE",
		(4,)
	)


######## Stirred Up Natives Landgrabbing event ###########

STIRRED_UP_NATIVES_LANDGRABBING_MIN_OWNED_PLOTS = 5
STIRRED_UP_NATIVES_LANDGRABBING_RADIUS = 2
STIRRED_UP_NATIVES_LANDGRABBING_BASE_GOLD = 4000


def _sunLandgrabbingScaledGold():
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	return max(1, STIRRED_UP_NATIVES_LANDGRABBING_BASE_GOLD * Speed.getStoragePercent() / 100)


def _sunLandgrabbingContext(kTriggeredData):
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return (None, None, None)

	if not player.isPlayable():
		return (None, None, None)

	if player.isNative():
		return (None, None, None)

	nativePlayer = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if nativePlayer.isNone():
		return (None, None, None)

	if not nativePlayer.isNative():
		return (None, None, None)

	if not nativePlayer.isAlive():
		return (None, None, None)

	if gc.getTeam(player.getTeam()).isAtWar(nativePlayer.getTeam()):
		return (None, None, None)

	nativeCity = nativePlayer.getCity(kTriggeredData.iOtherPlayerCityId)
	if nativeCity is None or nativeCity.isNone():
		return (None, None, None)

	return (player, nativePlayer, nativeCity)


def _sunLandgrabbingPlots(player, nativeCity):
	aPlots = []

	if player.isNone() or nativeCity is None or nativeCity.isNone():
		return aPlots

	iPlayer = player.getID()

	for iDX in range(-STIRRED_UP_NATIVES_LANDGRABBING_RADIUS, STIRRED_UP_NATIVES_LANDGRABBING_RADIUS + 1):
		for iDY in range(-STIRRED_UP_NATIVES_LANDGRABBING_RADIUS, STIRRED_UP_NATIVES_LANDGRABBING_RADIUS + 1):
			plot = plotXY(nativeCity.getX(), nativeCity.getY(), iDX, iDY)

			if plot is None or plot.isNone():
				continue

			if plotDistance(nativeCity.getX(), nativeCity.getY(), plot.getX(), plot.getY()) > STIRRED_UP_NATIVES_LANDGRABBING_RADIUS:
				continue

			if plot.isWater():
				continue

			if plot.isCity():
				continue

			if plot.getOwner() == iPlayer:
				aPlots.append(plot)

	return aPlots


def canTriggerStirredUpNativesLandgrabbing(argsList):
	kTriggeredData = argsList[0]

	player, nativePlayer, nativeCity = _sunLandgrabbingContext(kTriggeredData)
	if player is None:
		return False

	return len(_sunLandgrabbingPlots(player, nativeCity)) >= STIRRED_UP_NATIVES_LANDGRABBING_MIN_OWNED_PLOTS


def canDoStirredUpNativesLandgrabbing(argsList):
	kTriggeredData = argsList[0]

	player, nativePlayer, nativeCity = _sunLandgrabbingContext(kTriggeredData)
	if player is None:
		return False

	return len(_sunLandgrabbingPlots(player, nativeCity)) >= STIRRED_UP_NATIVES_LANDGRABBING_MIN_OWNED_PLOTS


def canDoStirredUpNativesLandgrabbingGold(argsList):
	kTriggeredData = argsList[0]

	player, nativePlayer, nativeCity = _sunLandgrabbingContext(kTriggeredData)
	if player is None:
		return False

	if len(_sunLandgrabbingPlots(player, nativeCity)) < STIRRED_UP_NATIVES_LANDGRABBING_MIN_OWNED_PLOTS:
		return False

	return player.getGold() >= _sunLandgrabbingScaledGold()


def _sunLandgrabbingSpawnHostileAdjacentToCity(city, iHostileUnitClass):
	if city is None or city.isNone():
		return None

	if iHostileUnitClass == -1:
		return None

	iBarbarian = gc.getGame().getBarbarianPlayer()
	barbPlayer = gc.getPlayer(iBarbarian)
	if barbPlayer.isNone():
		return None

	barbCiv = gc.getCivilizationInfo(barbPlayer.getCivilizationType())
	iUnitType = barbCiv.getCivilizationUnits(iHostileUnitClass)
	if iUnitType == UnitTypes.NO_UNIT:
		return None

	for iDX in range(-1, 2):
		for iDY in range(-1, 2):
			if iDX == 0 and iDY == 0:
				continue

			pLoop = plotXY(city.getX(), city.getY(), iDX, iDY)
			if pLoop is None or pLoop.isNone():
				continue
			if pLoop.isWater():
				continue
			if pLoop.isImpassable():
				continue
			if pLoop.isPeak():
				continue
			if pLoop.isCity():
				continue

			pUnit = barbPlayer.initUnit(
				iUnitType,
				ProfessionTypes.NO_PROFESSION,
				pLoop.getX(),
				pLoop.getY(),
				UnitAITypes.NO_UNITAI,
				DirectionTypes.DIRECTION_SOUTH,
				0
			)

			if pUnit is not None and not pUnit.isNone():
				return pUnit

	return None


def applyStirredUpNativesLandgrabbingAttack(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player, nativePlayer, nativeCity = _sunLandgrabbingContext(kTriggeredData)
	if player is None:
		return

	if len(_sunLandgrabbingPlots(player, nativeCity)) < STIRRED_UP_NATIVES_LANDGRABBING_MIN_OWNED_PLOTS:
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city is None or city.isNone():
		return

	cityPlot = city.plot()
	if cityPlot is None or cityPlot.isNone():
		return

	iHostileUnitClass = event.getGenericParameter(1)
	iNumHostiles = event.getGenericParameter(2)

	if iNumHostiles < 1:
		iNumHostiles = 1

	for i in range(iNumHostiles):
		hostileUnit = _sunLandgrabbingSpawnHostileAdjacentToCity(city, iHostileUnitClass)

		if hostileUnit is not None and not hostileUnit.isNone():
			if hostileUnit.canMoveInto(cityPlot, True, False, False):
				hostileUnit.attack(cityPlot, False)


def applyStirredUpNativesLandgrabbingGold(argsList):
	kTriggeredData = argsList[0]

	player, nativePlayer, nativeCity = _sunLandgrabbingContext(kTriggeredData)
	if player is None:
		return

	if len(_sunLandgrabbingPlots(player, nativeCity)) < STIRRED_UP_NATIVES_LANDGRABBING_MIN_OWNED_PLOTS:
		return

	iGold = _sunLandgrabbingScaledGold()
	if player.getGold() < iGold:
		return

	player.changeGold(-iGold)
	nativePlayer.changeGold(iGold)

######## Native Sacred Grounds Event ###########

NATIVE_SACRED_GROUNDS_MIN_OWNED_PLOTS = 5
NATIVE_SACRED_GROUNDS_RADIUS = 2
NATIVE_SACRED_GROUNDS_BASE_LOCK_TURNS = 40
NATIVE_SACRED_GROUNDS_BASE_RELIGION_POINTS = 300
NATIVE_SACRED_GROUNDS_BURIAL_IMPROVEMENT = "IMPROVEMENT_GOODY_HUT"


def _nsgScaledTurns():
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	return max(1, NATIVE_SACRED_GROUNDS_BASE_LOCK_TURNS * Speed.getGrowthPercent() / 100)


def _nsgScaledReligionPoints():
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	Handicap = gc.getHandicapInfo(CyGame().getHandicapType())
	return max(1, NATIVE_SACRED_GROUNDS_BASE_RELIGION_POINTS * Speed.getFatherPercent() / 100 * Handicap.getFatherPercent() / 100)


def _nsgData(player):
	if player.isNone():
		return ""
	szData = player.getScriptData()
	if szData is None:
		return ""
	return szData


def _nsgHas(player, key):
	return key in _nsgData(player)


def _nsgAdd(player, key):
	szData = _nsgData(player)
	if key not in szData:
		player.setScriptData(szData + key)


def _nsgRemove(player, key):
	szData = _nsgData(player)
	if key in szData:
		player.setScriptData(szData.replace(key, ""))


def _nsgGetNumber(player, key):
	szData = _nsgData(player)
	iStart = szData.find(key)
	if iStart == -1:
		return -1
	iStart += len(key)
	iEnd = szData.find("]]", iStart)
	if iEnd == -1:
		return -1
	try:
		return int(szData[iStart:iEnd])
	except:
		return -1


def _nsgSetNumber(player, key, iValue):
	szData = _nsgData(player)
	iStart = szData.find(key)
	if iStart != -1:
		iEnd = szData.find("]]", iStart)
		if iEnd != -1:
			szData = szData[:iStart] + szData[iEnd + 2:]
	player.setScriptData(szData + "%s%d]]" % (key, iValue))


def _nsgRemoveNumber(player, key):
	szData = _nsgData(player)
	iStart = szData.find(key)
	if iStart != -1:
		iEnd = szData.find("]]", iStart)
		if iEnd != -1:
			player.setScriptData(szData[:iStart] + szData[iEnd + 2:])


def _nsgCompletedKey():
	return "[[WTP_NATIVE_SACRED_GROUNDS_COMPLETED]]"


def _nsgReturnedKey():
	return "[[WTP_NATIVE_SACRED_GROUNDS_RETURNED]]"


def _nsgLockXKey():
	return "[[WTP_NATIVE_SACRED_GROUNDS_LOCK_X="


def _nsgLockYKey():
	return "[[WTP_NATIVE_SACRED_GROUNDS_LOCK_Y="


def _nsgLockOwnerKey():
	return "[[WTP_NATIVE_SACRED_GROUNDS_LOCK_OWNER="


def _nsgLockCityKey():
	return "[[WTP_NATIVE_SACRED_GROUNDS_LOCK_CITY="


def _nsgLockUntilKey():
	return "[[WTP_NATIVE_SACRED_GROUNDS_LOCK_UNTIL="


def _nsgContext(kTriggeredData):
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return (None, None, None)

	if _nsgHas(player, _nsgCompletedKey()):
		return (None, None, None)

	nativePlayer = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if nativePlayer.isNone() or not nativePlayer.isAlive() or not nativePlayer.isNative():
		return (None, None, None)

	if gc.getTeam(player.getTeam()).isAtWar(nativePlayer.getTeam()):
		return (None, None, None)

	nativeCity = nativePlayer.getCity(kTriggeredData.iOtherPlayerCityId)
	if nativeCity is None or nativeCity.isNone():
		return (None, None, None)

	return (player, nativePlayer, nativeCity)


def _nsgIsSacredPlot(plot):
	if plot is None or plot.isNone():
		return False
	if plot.getBonusType() != -1:
		return True
	if plot.isRiver():
		return True
	return False


def _nsgPlots(player, nativeCity):
	aPlots = []
	if player.isNone() or nativeCity is None or nativeCity.isNone():
		return aPlots

	iPlayer = player.getID()

	for iDX in range(-NATIVE_SACRED_GROUNDS_RADIUS, NATIVE_SACRED_GROUNDS_RADIUS + 1):
		for iDY in range(-NATIVE_SACRED_GROUNDS_RADIUS, NATIVE_SACRED_GROUNDS_RADIUS + 1):
			plot = plotXY(nativeCity.getX(), nativeCity.getY(), iDX, iDY)
			if plot is None or plot.isNone():
				continue
			if plotDistance(nativeCity.getX(), nativeCity.getY(), plot.getX(), plot.getY()) > NATIVE_SACRED_GROUNDS_RADIUS:
				continue
			if plot.isWater():
				continue
			if plot.isCity():
				continue
			if plot.getOwner() != iPlayer:
				continue
			aPlots.append(plot)

	return aPlots


def _nsgSacredPlots(player, nativeCity):
	aPlots = []
	for plot in _nsgPlots(player, nativeCity):
		if _nsgIsSacredPlot(plot):
			aPlots.append(plot)
	return aPlots


def _nsgBestSacredPlot(player, nativeCity, bPreferImprovement):
	aPlots = _nsgSacredPlots(player, nativeCity)
	if len(aPlots) == 0:
		return None

	if bPreferImprovement:
		for plot in aPlots:
			if plot.getImprovementType() != -1:
				return plot

	for plot in aPlots:
		if plot.getBonusType() != -1:
			return plot

	return aPlots[0]


def canTriggerNativeSacredGrounds(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _nsgContext(kTriggeredData)
	if player is None:
		return False

	if len(_nsgPlots(player, nativeCity)) < NATIVE_SACRED_GROUNDS_MIN_OWNED_PLOTS:
		return False

	if len(_nsgSacredPlots(player, nativeCity)) < 1:
		return False

	return True


def canDoNativeSacredGrounds(argsList):
	return canTriggerNativeSacredGrounds(argsList)


def _nsgFindNearestOwnPlot(player, sourcePlot):
	if player.isNone() or sourcePlot is None or sourcePlot.isNone():
		return None

	iPlayer = player.getID()

	for iRange in range(1, 8):
		for iDX in range(-iRange, iRange + 1):
			for iDY in range(-iRange, iRange + 1):
				plot = plotXY(sourcePlot.getX(), sourcePlot.getY(), iDX, iDY)
				if plot is None or plot.isNone():
					continue
				if plot.getOwner() != iPlayer:
					continue
				if plot.isWater():
					continue
				if plot.isImpassable():
					continue
				return plot

	return None


def _nsgMoveUnits(player, plot):
	if player.isNone() or plot is None or plot.isNone():
		return

	targetPlot = _nsgFindNearestOwnPlot(player, plot)
	if targetPlot is None or targetPlot.isNone():
		return

	iPlayer = player.getID()
	aUnits = []

	for i in range(plot.getNumUnits()):
		unit = plot.getUnit(i)
		if unit.isNone():
			continue
		if unit.getOwner() == iPlayer:
			aUnits.append(unit)

	for unit in aUnits:
		if not unit.isNone():
			unit.setXY(targetPlot.getX(), targetPlot.getY(), False, True, True)


def _nsgForcePlotToNative(plot, player, nativePlayer, bRemoveImprovement):
	if plot is None or plot.isNone():
		return
	if player.isNone() or nativePlayer.isNone():
		return

	if bRemoveImprovement and plot.getImprovementType() != -1:
		plot.setImprovementType(-1)

	plot.setCulture(player.getID(), 0, True)
	plot.setCulture(nativePlayer.getID(), 10000, True)
	plot.setOwner(nativePlayer.getID())


def _nsgSaveLock(player, plot, nativePlayer, nativeCity):
	if player.isNone() or plot is None or plot.isNone():
		return

	_nsgSetNumber(player, _nsgLockXKey(), plot.getX())
	_nsgSetNumber(player, _nsgLockYKey(), plot.getY())
	_nsgSetNumber(player, _nsgLockOwnerKey(), nativePlayer.getID())
	_nsgSetNumber(player, _nsgLockCityKey(), nativeCity.getID())
	_nsgSetNumber(player, _nsgLockUntilKey(), CyGame().getGameTurn() + _nsgScaledTurns())


def _nsgClearLock(player):
	_nsgRemoveNumber(player, _nsgLockXKey())
	_nsgRemoveNumber(player, _nsgLockYKey())
	_nsgRemoveNumber(player, _nsgLockOwnerKey())
	_nsgRemoveNumber(player, _nsgLockCityKey())
	_nsgRemoveNumber(player, _nsgLockUntilKey())


def applyNativeSacredGroundsReturn(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _nsgContext(kTriggeredData)
	if player is None:
		return

	plot = _nsgBestSacredPlot(player, nativeCity, False)
	if plot is None or plot.isNone():
		return

	_nsgMoveUnits(player, plot)
	_nsgForcePlotToNative(plot, player, nativePlayer, True)

	iBurial = gc.getInfoTypeForString(NATIVE_SACRED_GROUNDS_BURIAL_IMPROVEMENT)
	if iBurial != -1:
		plot.setImprovementType(iBurial)

	_nsgSaveLock(player, plot, nativePlayer, nativeCity)
	_nsgAdd(player, _nsgCompletedKey())
	_nsgAdd(player, _nsgReturnedKey())

	iReligion = gc.getInfoTypeForString("FATHER_POINT_RELIGION")
	if iReligion != -1:
		gc.getTeam(player.getTeam()).changeFatherPoints(iReligion, _nsgScaledReligionPoints())


def applyNativeSacredGroundsCompensation(argsList):
	kTriggeredData = argsList[0]
	player, nativePlayer, nativeCity = _nsgContext(kTriggeredData)
	if player is None:
		return

	plot = _nsgBestSacredPlot(player, nativeCity, True)
	if plot is None or plot.isNone():
		return

	if plot.getImprovementType() != -1:
		plot.setImprovementType(-1)

	_nsgAdd(player, _nsgCompletedKey())


def _nsgSpawnHostileAdjacentToCity(city, iHostileUnitClass):
	if city is None or city.isNone():
		return None

	if iHostileUnitClass == -1:
		return None

	iBarbarian = gc.getGame().getBarbarianPlayer()
	barbPlayer = gc.getPlayer(iBarbarian)
	if barbPlayer.isNone():
		return None

	barbCiv = gc.getCivilizationInfo(barbPlayer.getCivilizationType())
	iUnitType = barbCiv.getCivilizationUnits(iHostileUnitClass)
	if iUnitType == UnitTypes.NO_UNIT:
		return None

	for iDX in range(-1, 2):
		for iDY in range(-1, 2):
			if iDX == 0 and iDY == 0:
				continue

			pLoop = plotXY(city.getX(), city.getY(), iDX, iDY)
			if pLoop is None or pLoop.isNone():
				continue
			if pLoop.isWater():
				continue
			if pLoop.isImpassable():
				continue
			if pLoop.isPeak():
				continue
			if pLoop.isCity():
				continue
			if pLoop.isUnit():
				continue

			pUnit = barbPlayer.initUnit(
				iUnitType,
				ProfessionTypes.NO_PROFESSION,
				pLoop.getX(),
				pLoop.getY(),
				UnitAITypes.NO_UNITAI,
				DirectionTypes.DIRECTION_SOUTH,
				0
			)

			if pUnit is not None and not pUnit.isNone():
				return pUnit

	return None


def applyNativeSacredGroundsRefuse(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player, nativePlayer, nativeCity = _nsgContext(kTriggeredData)
	if player is None:
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city is None or city.isNone():
		return

	cityPlot = city.plot()
	if cityPlot is None or cityPlot.isNone():
		return

	iHostileUnitClass = event.getGenericParameter(1)
	iNumHostiles = event.getGenericParameter(2)

	if iNumHostiles < 1:
		iNumHostiles = 1

	for i in range(iNumHostiles):
		hostileUnit = _nsgSpawnHostileAdjacentToCity(city, iHostileUnitClass)
		if hostileUnit is not None and not hostileUnit.isNone():
			if hostileUnit.canMoveInto(cityPlot, True, False, False):
				hostileUnit.attack(cityPlot, False)

	_nsgAdd(player, _nsgCompletedKey())


def canTriggerNativeSacredGroundsMaintain(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	_nsgMaintainLockedPlot(player)

	return False


def _nsgCheckBurialViolation(player, nativePlayer, plot):
	iBurial = gc.getInfoTypeForString(NATIVE_SACRED_GROUNDS_BURIAL_IMPROVEMENT)
	if iBurial == -1:
		return False

	if not _nsgHas(player, _nsgReturnedKey()):
		return False

	if plot.getImprovementType() == iBurial:
		return False

	for i in range(plot.getNumUnits()):
		unit = plot.getUnit(i)
		if unit.isNone():
			continue

		violator = gc.getPlayer(unit.getOwner())
		if violator.isNone():
			continue
		if not violator.isPlayable() or violator.isNative():
			continue

		if not gc.getTeam(nativePlayer.getTeam()).isAtWar(violator.getTeam()):
			gc.getTeam(nativePlayer.getTeam()).declareWar(
				violator.getTeam(),
				False,
				WarPlanTypes.WARPLAN_TOTAL
			)

		_nsgRemove(player, _nsgReturnedKey())
		_nsgClearLock(player)
		return True

	return False


def _nsgMaintainLockedPlot(player):
	if player.isNone():
		return

	iX = _nsgGetNumber(player, _nsgLockXKey())
	iY = _nsgGetNumber(player, _nsgLockYKey())
	iNative = _nsgGetNumber(player, _nsgLockOwnerKey())
	iCity = _nsgGetNumber(player, _nsgLockCityKey())
	iUntil = _nsgGetNumber(player, _nsgLockUntilKey())

	if iX < 0 or iY < 0 or iNative < 0 or iUntil < 0:
		return

	nativePlayer = gc.getPlayer(iNative)
	if nativePlayer.isNone() or not nativePlayer.isAlive():
		_nsgRemove(player, _nsgReturnedKey())
		_nsgClearLock(player)
		return

	nativeCity = nativePlayer.getCity(iCity)
	if nativeCity is None or nativeCity.isNone():
		_nsgRemove(player, _nsgReturnedKey())
		_nsgClearLock(player)
		return

	if CyGame().getGameTurn() >= iUntil:
		if _nsgHas(player, _nsgReturnedKey()):
			nativePlayer.AI_changeAttitudeExtra(player.getID(), -4)
			_nsgRemove(player, _nsgReturnedKey())

			CyInterface().addMessage(
				player.getID(),
				True,
				20,
				localText.getText(
					"TXT_KEY_EVENT_NATIVE_SACRED_GROUNDS_RELATIONS_WORSEN",
					(nativeCity.getNameKey(),)
				),
				None,
				InterfaceMessageTypes.MESSAGE_TYPE_MINOR_EVENT,
				None,
				ColorTypes(7),
				-1,
				-1,
				False,
				False
			)

		_nsgClearLock(player)
		return

	plot = CyMap().plot(iX, iY)
	if plot is None or plot.isNone():
		_nsgRemove(player, _nsgReturnedKey())
		_nsgClearLock(player)
		return

	if _nsgCheckBurialViolation(player, nativePlayer, plot):
		return

	_nsgForcePlotToNative(plot, player, nativePlayer, False)


def getHelpNativeSacredGroundsReturn(argsList):
	return localText.getText(
		"TXT_KEY_EVENT_NATIVE_SACRED_GROUNDS_RETURN_HELP",
		(_nsgScaledTurns(), _nsgScaledReligionPoints())
	)


def getHelpNativeSacredGroundsCompensation(argsList):
	return localText.getText(
		"TXT_KEY_EVENT_NATIVE_SACRED_GROUNDS_COMPENSATION_HELP",
		()
	)


def getHelpNativeSacredGroundsRefuse(argsList):
	return localText.getText(
		"TXT_KEY_EVENT_NATIVE_SACRED_GROUNDS_REFUSE_HELP",
		()
	)


######## Initial Native Trade Event ###########

######## Initial Trade With Natives ###########

def _getInitialNativeTradeScaledAmount(iBaseAmount):
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	return (iBaseAmount * Speed.getStoragePercent()) / 100


def _doInitialNativeTradeCitiesShareBorder(city, nativeCity):
	if city is None or city.isNone():
		return False

	if nativeCity is None or nativeCity.isNone():
		return False

	iPlayer = city.getOwner()
	iNativePlayer = nativeCity.getOwner()

	# Check plots around own city for plots owned by the selected native player
	for iDX in range(-2, 3):
		for iDY in range(-2, 3):
			pLoop = plotXY(city.getX(), city.getY(), iDX, iDY)
			if pLoop is None or pLoop.isNone():
				continue

			if pLoop.getOwner() == iNativePlayer:
				return True

	# Symmetric safety check around the native city
	for iDX in range(-2, 3):
		for iDY in range(-2, 3):
			pLoop = plotXY(nativeCity.getX(), nativeCity.getY(), iDX, iDY)
			if pLoop is None or pLoop.isNone():
				continue

			if pLoop.getOwner() == iPlayer:
				return True

	return False


def _getInitialNativeTradeValidatedData(kTriggeredData, iRequiredTradeGoods):
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player is None or player.isNone():
		return None

	if not player.isPlayable():
		return None

	if player.isNative():
		return None

	city = player.getCity(kTriggeredData.iCityId)
	if city is None or city.isNone():
		return None

	if city.getOwner() != player.getID():
		return None

	nativePlayer = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if nativePlayer is None or nativePlayer.isNone():
		return None

	if not nativePlayer.isNative():
		return None

	nativeCity = nativePlayer.getCity(kTriggeredData.iOtherPlayerCityId)
	if nativeCity is None or nativeCity.isNone():
		return None

	if nativeCity.getOwner() != nativePlayer.getID():
		return None

	# Hard validation for exactly the selected native player / city
	if not _doInitialNativeTradeCitiesShareBorder(city, nativeCity):
		return None

	# At least cautious relations, no war
	if gc.getTeam(player.getTeam()).isAtWar(nativePlayer.getTeam()):
		return None

	if nativePlayer.AI_getAttitude(player.getID()) < AttitudeTypes.ATTITUDE_CAUTIOUS:
		return None

	if player.AI_getAttitude(nativePlayer.getID()) < AttitudeTypes.ATTITUDE_CAUTIOUS:
		return None

	iTradeGoods = gc.getInfoTypeForString("YIELD_TRADE_GOODS")
	if city.getYieldStored(iTradeGoods) < iRequiredTradeGoods:
		return None

	return (player, city, nativePlayer, nativeCity)


def canTriggerInitialNativeTrade(argsList):
	kTriggeredData = argsList[0]

	eEvent1 = gc.getInfoTypeForString("EVENT_INITIAL_TRADE_WITH_NATIVES_1")
	event1 = gc.getEventInfo(eEvent1)

	iRequiredTradeGoods = _getInitialNativeTradeScaledAmount(-event1.getGenericParameter(1))

	if _getInitialNativeTradeValidatedData(kTriggeredData, iRequiredTradeGoods) is None:
		return False

	return True


def canApplyInitialNativeTrade3(argsList):
	kTriggeredData = argsList[0]

	eEvent3 = gc.getInfoTypeForString("EVENT_INITIAL_TRADE_WITH_NATIVES_3")
	event3 = gc.getEventInfo(eEvent3)

	iRequiredTradeGoods = _getInitialNativeTradeScaledAmount(-event3.getGenericParameter(1))

	if _getInitialNativeTradeValidatedData(kTriggeredData, iRequiredTradeGoods) is None:
		return False

	return True


def applyInitialNativeTrade(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	iRequiredTradeGoods = _getInitialNativeTradeScaledAmount(-event.getGenericParameter(1))

	data = _getInitialNativeTradeValidatedData(kTriggeredData, iRequiredTradeGoods)
	if data is None:
		return

	player, city, nativePlayer, nativeCity = data

	iTradeGoods = gc.getInfoTypeForString("YIELD_TRADE_GOODS")

	# Failsafe: re-check just before changing storage
	if city.getYieldStored(iTradeGoods) < iRequiredTradeGoods:
		return

	city.changeYieldStored(iTradeGoods, -iRequiredTradeGoods)
	nativeCity.changeYieldStored(iTradeGoods, iRequiredTradeGoods)

def getHelpInitialNativeTrade1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	player2 = gc.getPlayer(kTriggeredData.eOtherPlayer)
	nativecity = player2.getCity(kTriggeredData.iOtherPlayerCityId)
	iYield = gc.getInfoTypeForString("YIELD_TRADE_GOODS")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	szHelp = ""
	if event.getGenericParameter(1) <> 0 :
		szHelp = localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity,  gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_GAIN", (-quantity,  gc.getYieldInfo(iYield).getChar(), nativecity.getNameKey()))
	return szHelp

######## Coca Events ###########
def canTriggerCocaEvent(argsList):
	ePlayer = argsList[1]
	iCityId = argsList[2]
	player = gc.getPlayer(ePlayer)
	city = player.getCity(iCityId)
	if player.isNone():
		return False
	if not player.isPlayable():
		return False
	if city.isNone():
		return False
	# Read Parameter 1 from the first event and check if enough yield is stored in city
	eEvent1 = gc.getInfoTypeForString("EVENT_COCA_TRADE_1")
	event1 = gc.getEventInfo(eEvent1)
	iYield = gc.getInfoTypeForString("YIELD_COCA_LEAVES")
	quantity = event1.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity :
		return False
	return True

def applyCocaEvent1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)

	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	iYield = gc.getInfoTypeForString("YIELD_COCA_LEAVES")
	if city.getYieldStored(iYield) < -quantity :
		return
	city.changeYieldStored(iYield, quantity)

def getHelpCocaEvent1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_COCA_LEAVES")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	szHelp = ""
	if event.getGenericParameter(1) <> 0 :
		szHelp = localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity,  gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
	return szHelp

######## EUROPE TRADE Events ###########

## Explanations ##
# use this as info for XML Event setup
# The Generic Parameters are all configured in the Events the Event Triggers offer

#Start Quest Event: The Trigger for it needs to be setup as "City Trigger"
# Generic Parameter 1: Amount to start the Quest
# Generic Parameter 2: Yield ID used for the Quest
# Generic Parameter 3: Amount to successfully finish the Quest

#Done Quest Event: The Trigger for it needs to be setup as "City Trigger"
# Generic Parameter 1: Amount to successfully finish the Quest
# Generic Parameter 2: Yield ID used for the Quest
# Generic Parameter 3: King Relations Change
# Generic Parameter 4: Yield Price Change

# This is generic function called by the specific functions of the Trigger! - Not directly by the Trigger XML.
# It uses the argsList of the Events forwarded by the Trigger as function Parameter
def CanDoEuropeTrade(argsList, iYieldID, iQuantity):

	ePlayer = argsList[1]

	# safety checks to make sure it is a colonial player
	player = gc.getPlayer(ePlayer)
	if not player.isPlayable():
		return False

	# this here should not be needed because isPlayable but since we have Asserts ...
	if player.isNative():
		return False

	king = gc.getPlayer(player.getParent())
	if not king.isEurope():
		return False

	# This would break immersion and make event unlogical
	if player.isInRevolution():
		return False

	# because we might want to do something with the City
	iCityId = argsList[2]
	city = player.getCity(iCityId)
	if city.isNone():
		return False

	# here we select the Amount of the Yield from function argument iQuantity
	quantity = iQuantity
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	# here we check Handicap Setting for AI only to avoid confusing number in texts when Difficulty changes
	Handicap = gc.getHandicapInfo(CyGame().getHandicapType())
	#for AI
	if not player.isHuman():
		quantity = quantity * Handicap.getAITrainPercent()/100

	# now we check if enough of the Yield has been traded with Europe using function argument iYieldID
	if player.getYieldSoldTotal(TradeLocationTypes.TRADE_LOCATION_EUROPE, iYieldID) < quantity:
		return False
	return True

# This is the Function for the Event Target Yield and Target Amount
# This Function is only used for the "Quest Start"
def getHelpQuestStartEuropeTradeYieldAndAmount(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	# getting Player and King
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)

	# we get the Yield as Parameter from Event

	# First we get the Yield for this Event
	iYield = event.getGenericParameter(2)

	# Second we get the Target Quantity to deliver and of course also consider gamespeed
	quantity = event.getGenericParameter(3)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	# here we check Handicap Setting for AI only to avoid confusing number in texts
	Handicap = gc.getHandicapInfo(CyGame().getHandicapType())
	#for AI
	if not player.isHuman():
		quantity = quantity * Handicap.getAITrainPercent()/100

	# Now we construct the Help Text
	szHelp = ""
	if quantity > 0 :
		if iYield != gc.getInfoTypeForString("YIELD_TRADE_GOODS") and iYield != gc.getInfoTypeForString("YIELD_LUXURY_GOODS"):
			szHelp += "\n" + localText.getText("TXT_KEY_EVENT_EUROPE_TRADE_YIELD_AND_TARGET_AMOUNT_HELP", (quantity, gc.getYieldInfo(iYield).getChar()))
		elif iYield == gc.getInfoTypeForString("YIELD_TRADE_GOODS") or iYield == gc.getInfoTypeForString("YIELD_LUXURY_GOODS"):
			szHelp += "\n" + localText.getText("TXT_KEY_EVENT_EUROPE_TRADE_YIELD_AND_TARGET_AMOUNT_HELP_BUY", (quantity, gc.getYieldInfo(iYield).getChar()))
	return szHelp

# This is the Function for the Event Help Text for Price and Attitude
# This Function is only used for the "Quest Done"
def getHelpQuestDoneEuropeTradePriceAndAttitude(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	# getting Player and King
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)

	# we get the Yield as Parameter from Event
	iYield = event.getGenericParameter(2)

	#szHelp = localText.getText("TXT_KEY_EVENT_EUROPE_TRADE_PRICE_AND_ATTITUDE_HELP", ())
	szHelp = ""
	if event.getGenericParameter(4) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_PRICE_INCREASE", (event.getGenericParameter(4), gc.getYieldInfo(iYield).getChar(), king.getCivilizationShortDescriptionKey()))
	if event.getGenericParameter(4) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_PRICE_DECREASE", (event.getGenericParameter(4), gc.getYieldInfo(iYield).getChar(), king.getCivilizationShortDescriptionKey()))
	if event.getGenericParameter(3) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_INCREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	if event.getGenericParameter(3) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_DECREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	return szHelp

# This is the Function for the Event Help to apply Price and Attitude changes
# This Function is only used for the "Quest DONE"
def applyQuestDoneEuropeTradePriceAndAttitude(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	# getting King and Player
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)

	# changing the Attitude
	player.AI_changeAttitudeExtra(eking, event.getGenericParameter(3))
	king.AI_changeAttitudeExtra(kTriggeredData.ePlayer, event.getGenericParameter(3))

	# getting the Yield for the Price Change
	iYield = event.getGenericParameter(2)

	# changing the Price
	iPrice = king.getYieldBuyPrice(iYield)
	king.setYieldBuyPrice(iYield, iPrice+event.getGenericParameter(4), 1)

####### Here start all the EUROPE QUEST TRIGGERS Functions #######
# These are the specific checks for the specific Event Triggers

def canTriggerEuropeTradeQuest_SUGAR_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_SUGAR_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_SUGAR_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_SUGAR_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_TOBACCO_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_TOBACCO_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_TOBACCO_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_TOBACCO_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_FOOD_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_FOOD_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_FOOD_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_FOOD_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_LUMBER_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_LUMBER_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_LUMBER_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_LUMBER_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_ORE_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_ORE_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_ORE_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_ORE_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_STONE_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_STONE_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_STONE_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_STONE_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_HEMP_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_HEMP_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_HEMP_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_HEMP_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_SHEEP_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_SHEEP_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_SHEEP_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_SHEEP_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_CATTLE_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_CATTLE_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_CATTLE_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_CATTLE_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_HORSES_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_HORSES_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_HORSES_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_HORSES_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_COCOA_FRUITS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_COCOA_FRUITS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_COCOA_FRUITS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_COCOA_FRUITS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_COCA_LEAVES_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_COCA_LEAVES_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_COCA_LEAVES_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_COCA_LEAVES_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_FUR_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_FUR_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_FUR_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_FUR_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_WOOL_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_WOOL_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_WOOL_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_WOOL_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_COTTON_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_COTTON_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_COTTON_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_COTTON_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_INDIGO_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_INDIGO_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_INDIGO_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_INDIGO_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_COFFEE_BERRIES_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_COFFEE_BERRIES_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_COFFEE_BERRIES_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_COFFEE_BERRIES_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_HIDES_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_HIDES_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_HIDES_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_HIDES_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_PREMIUM_FUR_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_PREMIUM_FUR_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_PREMIUM_FUR_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_PREMIUM_FUR_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_RAW_SALT_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_RAW_SALT_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_RAW_SALT_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_RAW_SALT_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_RED_PEPPER_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_RED_PEPPER_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_RED_PEPPER_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_RED_PEPPER_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_BARLEY_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_BARLEY_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_BARLEY_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_BARLEY_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_GRAPES_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_GRAPES_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_GRAPES_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_GRAPES_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_WHALE_BLUBBER_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_WHALE_BLUBBER_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_WHALE_BLUBBER_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_WHALE_BLUBBER_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_VALUABLE_WOOD_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_VALUABLE_WOOD_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_VALUABLE_WOOD_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_VALUABLE_WOOD_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_TRADE_GOODS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_TRADE_GOODS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_TRADE_GOODS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_TRADE_GOODS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_ROPE_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_ROPE_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_ROPE_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_ROPE_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_SAILCLOTH_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_SAILCLOTH_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_SAILCLOTH_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_SAILCLOTH_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_CLOTH_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_CLOTH_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_CLOTH_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_CLOTH_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_TOOLS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_TOOLS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_TOOLS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_TOOLS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_BLADES_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_BLADES_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_BLADES_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_BLADES_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_MUSKETS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_MUSKETS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_MUSKETS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_MUSKETS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_CANNONS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_CANNONS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_CANNONS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_CANNONS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_SILVER_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_SILVER_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_SILVER_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_SILVER_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_GOLD_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_GOLD_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_GOLD_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_GOLD_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_COCOA_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_COCOA_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_COCOA_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_COCOA_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_COFFEE_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_COFFEE_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_COFFEE_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_COFFEE_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_CIGARS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_CIGARS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_CIGARS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_CIGARS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_GEMS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_GEMS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_GEMS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_GEMS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_WOOL_CLOTH_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_WOOL_CLOTH_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_WOOL_CLOTH_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_WOOL_CLOTH_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_COLOURED_CLOTH_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_COLOURED_CLOTH_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_COLOURED_CLOTH_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_COLOURED_CLOTH_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_LEATHER_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_LEATHER_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_LEATHER_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_LEATHER_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_COATS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_COATS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_COATS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_COATS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_PREMIUM_COATS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_PREMIUM_COATS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_PREMIUM_COATS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_PREMIUM_COATS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_SPICES_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_SPICES_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_SPICES_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_SPICES_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_BEER_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_BEER_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_BEER_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_BEER_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_WINE_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_WINE_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_WINE_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_WINE_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_WHALE_OIL_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_WHALE_OIL_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_WHALE_OIL_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_WHALE_OIL_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_FURNITURE_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_FURNITURE_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_FURNITURE_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_FURNITURE_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_SALT_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_SALT_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_SALT_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_SALT_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_LUXURY_GOODS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_LUXURY_GOODS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_LUXURY_GOODS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_LUXURY_GOODS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_TRADE_GOODS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_TRADE_GOODS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_TRADE_GOODS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_TRADE_GOODS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_CLAY_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_CLAY_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_CLAY_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_CLAY_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger


def canTriggerEuropeTradeQuest_PEAT_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_PEAT_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_PEAT_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_PEAT_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_RICE_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_RICE_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_RICE_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_RICE_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_CASSAVA_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_CASSAVA_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_CASSAVA_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_CASSAVA_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_HARDWOOD_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_HARDWOOD_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_HARDWOOD_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_HARDWOOD_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger


def canTriggerEuropeTradeQuest_FLAX_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_FLAX_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_FLAX_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_FLAX_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_PEANUTS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_PEANUTS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_PEANUTS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_PEANUTS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_FRUITS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_FRUITS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_FRUITS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_FRUITS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_YERBA_LEAVES_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_YERBA_LEAVES_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_YERBA_LEAVES_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_YERBA_LEAVES_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger


def canTriggerEuropeTradeQuest_LOGWOOD_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_LOGWOOD_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_LOGWOOD_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_LOGWOOD_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_COCHINEAL_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_COCHINEAL_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_COCHINEAL_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_COCHINEAL_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_VANILLA_PODS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_VANILLA_PODS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_VANILLA_PODS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_VANILLA_PODS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_MAPLE_SIRUP_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_MAPLE_SIRUP_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_MAPLE_SIRUP_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_MAPLE_SIRUP_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_KAUTSCHUK_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_KAUTSCHUK_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_KAUTSCHUK_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_KAUTSCHUK_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_COAL_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_COAL_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_COAL_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_COAL_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger


def canTriggerEuropeTradeQuest_CHAR_COAL_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_CHAR_COAL_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_CHAR_COAL_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_CHAR_COAL_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_BAKERY_GOODS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_BAKERY_GOODS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_BAKERY_GOODS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_BAKERY_GOODS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_BLACK_POWDER_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_BLACK_POWDER_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_BLACK_POWDER_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_BLACK_POWDER_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_GEESE_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_GEESE_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_GEESE_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_GEESE_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_CHICKEN_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_CHICKEN_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_CHICKEN_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_CHICKEN_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_PIGS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_PIGS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_PIGS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_PIGS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_GOATS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_GOATS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_GOATS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_GOATS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_OLIVES_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_OLIVES_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_OLIVES_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_OLIVES_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger


def canTriggerEuropeTradeQuest_RAPE_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_RAPE_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_RAPE_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_RAPE_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_WILD_FEATHERS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_WILD_FEATHERS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_WILD_FEATHERS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_WILD_FEATHERS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_MILK_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_MILK_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_MILK_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_MILK_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_GOAT_HIDES_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_GOAT_HIDES_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_GOAT_HIDES_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_GOAT_HIDES_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_PIG_SKIN_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_PIG_SKIN_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_PIG_SKIN_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_PIG_SKIN_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_DOWNS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_DOWNS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_DOWNS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_DOWNS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_ROASTED_PEANUTS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_ROASTED_PEANUTS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_ROASTED_PEANUTS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_ROASTED_PEANUTS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger


def canTriggerEuropeTradeQuest_CHEESE_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_CHEESE_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_CHEESE_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_CHEESE_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_YERBA_TEA_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_YERBA_TEA_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_YERBA_TEA_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_YERBA_TEA_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_CHOCOLATE_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_CHOCOLATE_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_CHOCOLATE_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_CHOCOLATE_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_RUM_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_RUM_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_RUM_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_RUM_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_HOOCH_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_HOOCH_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_HOOCH_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_HOOCH_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_OLIVE_OIL_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_OLIVE_OIL_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_OLIVE_OIL_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_OLIVE_OIL_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_RAPE_OIL_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_RAPE_OIL_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_RAPE_OIL_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_RAPE_OIL_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger


def canTriggerEuropeTradeQuest_EVERYDAY_CLOTHES_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_EVERYDAY_CLOTHES_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_EVERYDAY_CLOTHES_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_EVERYDAY_CLOTHES_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger


def canTriggerEuropeTradeQuest_FESTIVE_CLOTHES_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_FESTIVE_CLOTHES_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_FESTIVE_CLOTHES_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_FESTIVE_CLOTHES_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_COLOURED_WOOL_CLOTH_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_COLOURED_WOOL_CLOTH_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_COLOURED_WOOL_CLOTH_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_COLOURED_WOOL_CLOTH_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_GOAT_HIDE_BOOTS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_GOAT_HIDE_BOOTS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_GOAT_HIDE_BOOTS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_GOAT_HIDE_BOOTS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_PIG_LEATHER_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_PIG_LEATHER_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_PIG_LEATHER_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_PIG_LEATHER_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_PADDED_LEATHER_COATS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_PADDED_LEATHER_COATS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_PADDED_LEATHER_COATS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_PADDED_LEATHER_COATS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_VANILLA_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_VANILLA_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_VANILLA_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_VANILLA_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_POTTERY_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_POTTERY_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_POTTERY_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_POTTERY_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_PADDED_FURNITURE_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_PADDED_FURNITURE_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_PADDED_FURNITURE_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_PADDED_FURNITURE_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_FIELD_WORKER_TOOLS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_FIELD_WORKER_TOOLS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_FIELD_WORKER_TOOLS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_FIELD_WORKER_TOOLS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_HOUSEHOLD_GOODS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_HOUSEHOLD_GOODS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerEuropeTradeQuest_HOUSEHOLD_GOODS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_EUROPE_TRADE_QUEST_HOUSEHOLD_GOODS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoEuropeTrade(argsList, iYieldID, iQuantity)

	return bTrigger

######## AFRICA TRADE Events ###########

## Explanations ##
# use this as info for XML Event setup
# The Generic Parameters are all configured in the Events the Event Triggers offer

#Start Quest Event: The Trigger for it needs to be setup as "City Trigger"
# Generic Parameter 1: Amount to start the Quest
# Generic Parameter 2: Yield ID used for the Quest
# Generic Parameter 3: Amount to successfully finish the Quest

#Done Quest Event: The Trigger for it needs to be setup as "City Trigger"
# Generic Parameter 1: Amount to successfully finish the Quest
# Generic Parameter 2: Yield ID used for the Quest
# Generic Parameter 3: King Relations Change
# Generic Parameter 4: Yield Price Change

# This is generic function called by the specific functions of the Trigger! - Not directly by the Trigger XML.
# It uses the argsList of the Events forwarded by the Trigger as function Parameter
def CanDoAfricaTrade(argsList, iYieldID, iQuantity):

	ePlayer = argsList[1]

	# safety checks to make sure it is a colonial player
	player = gc.getPlayer(ePlayer)
	if not player.isPlayable():
		return False

	# this here should not be needed because isPlayable but since we have Asserts ...
	if player.isNative():
		return False

	king = gc.getPlayer(player.getParent())
	if not king.isEurope():
		return False

	# This would break immersion and make event unlogical
	if player.isInRevolution():
		return False

	# because we might want to do something with the City
	iCityId = argsList[2]
	city = player.getCity(iCityId)
	if city.isNone():
		return False

	# here we select the Amount of the Yield from function argument iQuantity
	quantity = iQuantity
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	# here we check Handicap Setting for AI only to avoid confusing number in texts when Difficulty changes
	Handicap = gc.getHandicapInfo(CyGame().getHandicapType())
	#for AI
	if not player.isHuman():
		quantity = quantity * Handicap.getAITrainPercent()/100

	# now we check if enough of the Yield has been traded with Africa using function argument iYieldID
	if player.getYieldSoldTotal(TradeLocationTypes.TRADE_LOCATION_AFRICA, iYieldID) < quantity:
		return False
	return True

# This is the Function for the Event Target Yield and Target Amount
# This Function is only used for the "Quest Start"
def getHelpQuestStartAfricaTradeYieldAndAmount(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	# getting Player and King
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)

	# we get the Yield as Parameter from Event
	iYield = event.getGenericParameter(2)

	# Second we get the Target Quantity to deliver and of course also consider gamespeed
	quantity = event.getGenericParameter(3)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	# here we check Handicap Setting for AI only to avoid confusing number in texts
	Handicap = gc.getHandicapInfo(CyGame().getHandicapType())
	#for AI
	if not player.isHuman():
		quantity = quantity * Handicap.getAITrainPercent()/100

	# Now we construct the Help Text
	szHelp = ""
	if quantity > 0 :
		if iYield != gc.getInfoTypeForString("YIELD_TRADE_GOODS") and iYield != gc.getInfoTypeForString("YIELD_LUXURY_GOODS"):
			szHelp += "\n" + localText.getText("TXT_KEY_EVENT_AFRICA_TRADE_YIELD_AND_TARGET_AMOUNT_HELP", (quantity, gc.getYieldInfo(iYield).getChar()))
		elif iYield == gc.getInfoTypeForString("YIELD_TRADE_GOODS") or iYield== event.getGenericParameter(2) != gc.getInfoTypeForString("YIELD_LUXURY_GOODS"):
			szHelp += "\n" + localText.getText("TXT_KEY_EVENT_AFRICA_TRADE_YIELD_AND_TARGET_AMOUNT_HELP_BUY", (quantity, gc.getYieldInfo(iYield).getChar()))
	return szHelp

# This is the Function for the Event Help Text for Price and Attitude
# This Function is only used for the "Quest Done"
def getHelpQuestDoneAfricaTradePriceAndAttitude(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	# getting Player and King
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)

	# we get the Yield as Parameter from Event
	iYield = event.getGenericParameter(2)

	#szHelp = localText.getText("TXT_KEY_EVENT_EUROPE_TRADE_PRICE_AND_ATTITUDE_HELP", ())
	szHelp = ""
	if event.getGenericParameter(4) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_PRICE_INCREASE_AFRICA", (event.getGenericParameter(4), gc.getYieldInfo(iYield).getChar(), king.getCivilizationShortDescriptionKey()))
	if event.getGenericParameter(4) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_PRICE_DECREASE_AFRICA", (event.getGenericParameter(4), gc.getYieldInfo(iYield).getChar(), king.getCivilizationShortDescriptionKey()))
	if event.getGenericParameter(3) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_INCREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	if event.getGenericParameter(3) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_DECREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	return szHelp

# This is the Function for the Event Help to apply Price and Attitude changes
# This Function is only used for the "Quest DONE"
def applyQuestDoneAfricaTradePriceAndAttitude(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	# getting King and Player
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)

	# changing the Attitude
	player.AI_changeAttitudeExtra(eking, event.getGenericParameter(3))
	king.AI_changeAttitudeExtra(kTriggeredData.ePlayer, event.getGenericParameter(3))

	# getting the Yield for the Price Change
	iYield = event.getGenericParameter(2)

	# careful, uses Africa methods here
	iPrice = king.getYieldAfricaBuyPriceNoModifier(iYield)
	king.setYieldAfricaBuyPrice(iYield, iPrice+event.getGenericParameter(4), 1)

####### Here start all the AFICA QUEST TRIGGERS Functions #######
# These are the specific checks for the specific Event Triggers


######## PORT ROYAL TRADE Events ###########

## Explanations ##
# use this as info for XML Event setup
# The Generic Parameters are all configured in the Events the Event Triggers offer

#Start Quest Event: The Trigger for it needs to be setup as "City Trigger"
# Generic Parameter 1: Amount to start the Quest
# Generic Parameter 2: Yield ID used for the Quest
# Generic Parameter 3: Amount to successfully finish the Quest

#Done Quest Event: The Trigger for it needs to be setup as "City Trigger"
# Generic Parameter 1: Amount to successfully finish the Quest
# Generic Parameter 2: Yield ID used for the Quest
# Generic Parameter 3: King Relations Change
# Generic Parameter 4: Yield Price Change

# This is generic function called by the specific functions of the Trigger! - Not directly by the Trigger XML.
# It uses the argsList of the Events forwarded by the Trigger as function Parameter
def CanDoPortRoyalTrade(argsList, iYieldID, iQuantity):

	ePlayer = argsList[1]

	# safety checks to make sure it is a colonial player
	player = gc.getPlayer(ePlayer)
	if not player.isPlayable():
		return False

	# this here should not be needed because isPlayable but since we have Asserts ...
	if player.isNative():
		return False

	king = gc.getPlayer(player.getParent())

	if king is None:
		return False

	if not king.isEurope():
		return False

	# For Port Royal this is not needed, because Trade is also possible during Revolution
	#if player.isInRevolution():
	#	return False

	# because we might want to do something with the City
	iCityId = argsList[2]
	city = player.getCity(iCityId)
	if city.isNone():
		return False

	# here we select the Amount of the Yield from function argument iQuantity
	quantity = iQuantity
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	# here we check Handicap Setting for AI only to avoid confusing number in texts when Difficulty changes
	Handicap = gc.getHandicapInfo(CyGame().getHandicapType())
	#for AI
	if not player.isHuman():
		quantity = quantity * Handicap.getAITrainPercent()/100

	# now we check if enough of the Yield has been traded with Port Royal using function argument iYieldID
	if player.getYieldSoldTotal(TradeLocationTypes.TRADE_LOCATION_PORT_ROYAL, iYieldID) < quantity:
		return False
	return True

# This is the Function for the Event Target Yield and Target Amount
# This Function is only used for the "Quest Start"
def getHelpQuestStartPortRoyalTradeYieldAndAmount(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	# getting Player and King
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)

	# we get the Yield as Parameter from Event
	iYield = event.getGenericParameter(2)

	# Second we get the Target Quantity to deliver and of course also consider gamespeed
	quantity = event.getGenericParameter(3)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	# here we check Handicap Setting for AI only to avoid confusing number in texts
	Handicap = gc.getHandicapInfo(CyGame().getHandicapType())
	#for AI
	if not player.isHuman():
		quantity = quantity * Handicap.getAITrainPercent()/100

	# Now we construct the Help Text
	szHelp = ""
	if quantity > 0 :
		if iYield != gc.getInfoTypeForString("YIELD_TRADE_GOODS") and iYield != gc.getInfoTypeForString("YIELD_LUXURY_GOODS"):
			szHelp += "\n" + localText.getText("TXT_KEY_EVENT_PORTROYAL_TRADE_YIELD_AND_TARGET_AMOUNT_HELP", (quantity, gc.getYieldInfo(iYield).getChar()))
		elif iYield == gc.getInfoTypeForString("YIELD_TRADE_GOODS") or iYield == gc.getInfoTypeForString("YIELD_LUXURY_GOODS"):
			szHelp += "\n" + localText.getText("TXT_KEY_EVENT_PORTROYAL_TRADE_YIELD_AND_TARGET_AMOUNT_HELP_BUY", (quantity, gc.getYieldInfo(iYield).getChar()))
	return szHelp

# This is the Function for the Event Help Text for Price and Attitude
# This Function is only used for the "Quest Done"
def getHelpQuestDonePortRoyalTradePriceAndAttitude(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	# getting Player and King
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)

	# we get the Yield as Parameter from Event
	iYield = event.getGenericParameter(2)

	#szHelp = localText.getText("TXT_KEY_EVENT_EUROPE_TRADE_PRICE_AND_ATTITUDE_HELP", ())
	szHelp = ""
	if event.getGenericParameter(4) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_PRICE_INCREASE_PORT_ROYAL", (event.getGenericParameter(4), gc.getYieldInfo(iYield).getChar(), king.getCivilizationShortDescriptionKey()))
	if event.getGenericParameter(4) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_PRICE_DECREASE_PORT_ROYAL", (event.getGenericParameter(4), gc.getYieldInfo(iYield).getChar(), king.getCivilizationShortDescriptionKey()))
	if event.getGenericParameter(3) > 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_INCREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	if event.getGenericParameter(3) < 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_DECREASE", (event.getGenericParameter(3), king.getCivilizationAdjectiveKey()))
	return szHelp

# This is the Function for the Event Help to apply Price and Attitude changes
# This Function is only used for the "Quest DONE"
def applyQuestDonePortRoyalTradePriceAndAttitude(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	# getting King and Player
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)

	# changing the Attitude
	player.AI_changeAttitudeExtra(eking, event.getGenericParameter(3))
	king.AI_changeAttitudeExtra(kTriggeredData.ePlayer, event.getGenericParameter(3))

	# getting the Yield for the Price Change
	iYield = event.getGenericParameter(2)

	# careful, uses Port Royal methods here
	iPrice = king.getYieldPortRoyalBuyPriceNoModifier(iYield)
	king.setYieldPortRoyalBuyPrice(iYield, iPrice+event.getGenericParameter(4), 1)

####### Here start all the PORT ROYAL QUEST TRIGGERS Functions #######
# These are the specific checks for the specific Event Triggers

def canTriggerPortRoyalTradeQuest_BLADES_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_BLADES_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_BLADES_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_BLADES_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_SAILCLOTH_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_SAILCLOTH_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_SAILCLOTH_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_SAILCLOTH_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_CANNONS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_CANNONS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_CANNONS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_CANNONS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger
def canTriggerPortRoyalTradeQuest_GUNS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_GUNS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_GUNS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_GUNS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_ROPES_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_ROPES_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_ROPES_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_ROPES_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_CIGARS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_CIGARS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_CIGARS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_CIGARS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_SPICES_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_SPICES_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_SPICES_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_SPICES_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_SALT_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_SALT_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_SALT_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_SALT_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_RUM_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_RUM_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_RUM_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_RUM_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_GOLD_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_GOLD_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_GOLD_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_GOLD_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_GEMS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_GEMS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_GEMS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_GEMS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_FURNITURE_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_FURNITURE_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_FURNITURE_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_FURNITURE_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_COFFEE_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_COFFEE_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_COFFEE_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_COFFEE_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_COCOA_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_COCOA_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_COCOA_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_COCOA_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_COCA_LEAVES_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_COCA_LEAVES_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_COCA_LEAVES_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_COCA_LEAVES_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_WINE_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_WINE_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_WINE_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_WINE_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_SILVER_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_SILVER_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_SILVER_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_SILVER_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_HARDWOOD_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_HARDWOOD_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_HARDWOOD_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_HARDWOOD_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_FOOD_START(argsList):

	# Read Parameters 12 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_FOOD_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_FOOD_DONE(argsList):

	# Read Parameters 12 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_FOOD_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_STONE_START(argsList):

	# Read Parameters 12 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_STONE_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_STONE_DONE(argsList):

	# Read Parameters 12 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_STONE_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_LUMBER_START(argsList):

	# Read Parameters 12 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_LUMBER_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_LUMBER_DONE(argsList):

	# Read Parameters 12 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_LUMBER_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_CLAY_START(argsList):

	# Read Parameters 12 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_CLAY_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_CLAY_DONE(argsList):

	# Read Parameters 12 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_CLAY_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_BAKERY_GOODS_START(argsList):

	# Read Parameters 12 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_BAKERY_GOODS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_BAKERY_GOODS_DONE(argsList):

	# Read Parameters 12 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_BAKERY_GOODS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_BEER_START(argsList):

	# Read Parameters 12 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_BEER_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_BEER_DONE(argsList):

	# Read Parameters 12 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_BEER_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_HOOCH_START(argsList):

	# Read Parameters 12 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_HOOCH_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_HOOCH_DONE(argsList):

	# Read Parameters 12 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_HOOCH_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger


def canTriggerPortRoyalTradeQuest_YERBA_TEA_START(argsList):

	# Read Parameters 12 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_YERBA_TEA_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_YERBA_TEA_DONE(argsList):

	# Read Parameters 12 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_YERBA_TEA_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_BLACK_POWDER_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_BLACK_POWDER_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_BLACK_POWDER_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_BLACK_POWDER_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_RICE_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_RICE_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_RICE_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_RICE_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_CASSAVA_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_CASSAVA_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerPortRoyalTradeQuest_CASSAVA_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_PORTROYAL_TRADE_QUEST_CASSAVA_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoPortRoyalTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_FOOD_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_FOOD_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_FOOD_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_FOOD_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_LUMBER_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_LUMBER_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_LUMBER_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_LUMBER_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_STONE_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_STONE_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_STONE_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_STONE_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_HORSES_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_HORSES_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_HORSES_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_HORSES_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_ORE_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_ORE_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_ORE_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_ORE_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_SILVER_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_SILVER_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_SILVER_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_SILVER_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_CATTLE_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_CATTLE_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_CATTLE_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_CATTLE_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_SHEEP_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_SHEEP_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_SHEEP_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_SHEEP_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_TOOLS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_TOOLS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_TOOLS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_TOOLS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_GUNS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_GUNS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_GUNS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_GUNS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_CLOTH_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_CLOTH_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_CLOTH_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_CLOTH_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_SALT_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_SALT_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_SALT_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_SALT_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_SPICES_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_SPICES_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_SPICES_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_SPICES_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_COCA_LEAVES_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_COCA_LEAVES_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_COCA_LEAVES_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_COCA_LEAVES_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_TRADINGGOODS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_TRADINGGOODS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_TRADINGGOODS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_TRADINGGOODS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_COTTON_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_COTTON_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_COTTON_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_COTTON_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger


def canTriggerAfricaTradeQuest_INDIGO_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_INDIGO_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_INDIGO_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_INDIGO_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_LOGWOOD_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_LOGWOOD_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_LOGWOOD_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_LOGWOOD_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_COCHINEAL_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_COCHINEAL_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_COCHINEAL_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_COCHINEAL_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_COFFEE_BERRIES_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_COFFEE_BERRIES_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_COFFEE_BERRIES_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_COFFEE_BERRIES_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_PEANUTS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_PEANUTS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_PEANUTS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_PEANUTS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_RED_PEPPER_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_RED_PEPPER_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_RED_PEPPER_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_RED_PEPPER_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_VANILLA_PODS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_VANILLA_PODS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_VANILLA_PODS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_VANILLA_PODS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_MAPLE_SIRUP_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_MAPLE_SIRUP_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_MAPLE_SIRUP_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_MAPLE_SIRUP_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_KAUTSCHUK_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_KAUTSCHUK_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_KAUTSCHUK_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_KAUTSCHUK_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_HARDWOOD_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_HARDWOOD_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_HARDWOOD_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_HARDWOOD_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_BLACK_POWDER_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_BLACK_POWDER_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_BLACK_POWDER_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_BLACK_POWDER_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_BLADES_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_BLADES_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_BLADES_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_BLADES_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_CANNONS_START(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_CANNONS_START")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Start this should be e.g. 200

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

def canTriggerAfricaTradeQuest_CANNONS_DONE(argsList):

	# Read Parameters 1+2 from the two events and check if enough yield is stored in city
	eEvent = gc.getInfoTypeForString("EVENT_AFRICA_TRADE_QUEST_CANNONS_DONE_1")
	event = gc.getEventInfo(eEvent)
	iYieldID = event.getGenericParameter(2)
	iQuantity = event.getGenericParameter(1) # for Quest Done this should be e.g. 1000

	# Now we call the Generic Helper Function
	bTrigger = CanDoAfricaTrade(argsList, iYieldID, iQuantity)

	return bTrigger

#######################################################
######## SPAWNING UNITS - friendly and hostile ########
#######################################################

### PART A1) UNIT Trigger Check Methods Blueprints
#######################################################
### Those are for the UnitTrigger Triggers to check ###
### ! Just Blueprints to be implemented by Trigger !###
#######################################################

# check for own units
def checkOwnPlayerUnitOnAdjacentPlotOfUnit(argsList): ### When you copy rename specically for your actuall EventTrigger
	eEvent = gc.getInfoTypeForString("EVENT_THAT_STORES_THE_PARAMETERS_TO_CHECK") ### When you copy put in actual Event to read parameters
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unitThatTriggered = player.getUnit(kTriggeredData.iUnitId)
	iOwnUnitClassTypeToCheck = event.getGenericParameter(1)
	found = unitThatTriggered.isOwnPlayerUnitOnAdjacentPlotOfUnit(iOwnUnitClassTypeToCheck)
	if (found):
		return True
	return False

# check for Barbarian Units
def checkBarbarianUnitOnAdjacentPlotOfUnit(argsList): ### When you copy rename specically for your actuall EventTrigger
	eEvent = gc.getInfoTypeForString("EVENT_THAT_STORES_THE_PARAMETERS_TO_CHECK") ### When you copy put in actual Event to read parameters
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unitThatTriggered = player.getUnit(kTriggeredData.iUnitId)
	iBarbarianUnitClassTypeToCheck = event.getGenericParameter(1)
	found = unitThatTriggered.isBarbarianUnitOnAdjacentPlotOfUnit(iBarbarianUnitClassTypeToCheck)
	if (found):
		return True
	return False


### PART A2) UNIT Trigger Spawn Methods
#####################################################
### Those are for the UnitTrigger Events to spawn ###
#####################################################

### Barbarians Units - either same Plot or adjacent Plot
### GenericParameter1: UnitClassType to spawn
### GenericParameter2: Number of Units to spawn

# same Plot
def spawnBarbarianUnitOnSamePlotAsUnit(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unitThatTriggered = player.getUnit(kTriggeredData.iUnitId)
	iHostileUnitClassTypeToSpawn = event.getGenericParameter(1)
	iNumHostilesToSpawn = event.getGenericParameter(2)
	for iX in range(iNumHostilesToSpawn):
		unitThatTriggered.spawnBarbarianUnitOnPlotOfUnit(iHostileUnitClassTypeToSpawn)

# adjacent Plot
def spawnBarbarianUnitAdjacentToUnit(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unitThatTriggered = player.getUnit(kTriggeredData.iUnitId)
	iHostileUnitClassTypeToSpawn = event.getGenericParameter(1)
	iNumHostilesToSpawn = event.getGenericParameter(2)
	for iX in range(iNumHostilesToSpawn):
		unitThatTriggered.spawnBarbarianUnitOnAdjacentPlotOfUnit(iHostileUnitClassTypeToSpawn)

### Own Player Units - either same Plot or adjacent Plot
### GenericParameter1: UnitClassType to spawn
### GenericParameter2: Number of Units to spawn

# same Plot
def spawnOwnPlayerUnitOnSamePlotAsUnit(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unitThatTriggered = player.getUnit(kTriggeredData.iUnitId)
	iOwnUnitClassTypeToSpawn = event.getGenericParameter(1)
	iNumOwnToSpawn = event.getGenericParameter(2)
	for iX in range(iNumOwnToSpawn):
		unitThatTriggered.spawnOwnPlayerUnitOnPlotOfUnit(iOwnUnitClassTypeToSpawn)

# adjacent Plot
def spawnOwnPlayerUnitAdjacentToUnit(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unitThatTriggered = player.getUnit(kTriggeredData.iUnitId)
	iOwnUnitClassTypeToSpawn = event.getGenericParameter(1)
	iNumOwnToSpawn = event.getGenericParameter(2)
	for iX in range(iNumOwnToSpawn):
		unitThatTriggered.spawnOwnPlayerUnitOnAdjacentPlotOfUnit(iOwnUnitClassTypeToSpawn)

### PART B1) CITY Trigger Check Methods Blueprints
#######################################################
### Those are for the CityTrigger Triggers to check ###
### ! Just Blueprints to be implemented by Trigger !###
#######################################################

# check for own units
def checkOwnPlayerUnitOnAdjacentPlotOfCity(argsList): ### When you copy rename specically for your actuall EventTrigger
	eEvent = gc.getInfoTypeForString("EVENT_THAT_STORES_THE_PARAMETERS_TO_CHECK") ### When you copy put in actual Event to read parameters
	event = gc.getEventInfo(eEvent)
	ePlayer = argsList[1]
	iCityIdThatTriggered = argsList[2]
	player = gc.getPlayer(ePlayer)
	city = player.getCity(iCityIdThatTriggered)

	iOwnUnitClassTypeToCheck = event.getGenericParameter(1)
	found = city.isOwnPlayerUnitOnAdjacentPlotOfCity(iOwnUnitClassTypeToCheck)
	if (found):
		return True
	return False


# check for Barbarian Units
def checkBarbarianUnitOnAdjacentPlotOfCity(argsList): ### When you copy rename specically for your actuall EventTrigger
	eEvent = gc.getInfoTypeForString("EVENT_THAT_STORES_THE_PARAMETERS_TO_CHECK") ### When you copy put in actual Event to read parameters
	event = gc.getEventInfo(eEvent)
	ePlayer = argsList[1]
	iCityIdThatTriggered = argsList[2]
	player = gc.getPlayer(ePlayer)
	city = player.getCity(iCityIdThatTriggered)

	iBarbarianUnitClassTypeToCheck = event.getGenericParameter(1)
	found = city.isBarbarianUnitOnAdjacentPlotOfCity(iBarbarianUnitClassTypeToCheck)
	if (found):
		return True
	return False


### PART B2) CITY Trigger Spawn Methods
#####################################################
### Those are for the CityTrigger Events to spawn ###
#####################################################

### Barbarians Units - either same Plot or adjacent Plot
### GenericParameter1: UnitClassType to spawn
### GenericParameter2: Number of Units to spawn

# same Plot # CAREFUL !!!, will take over City
def spawnBarbarianUnitOnSamePlotAsCity(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)

	iHostileUnitClassTypeToSpawn = event.getGenericParameter(1)
	iNumHostilesToSpawn = event.getGenericParameter(2)
	for iX in range(iNumHostilesToSpawn):
		city.spawnBarbarianUnitOnPlotOfCity(iHostileUnitClassTypeToSpawn)

# adjacent Plot
def spawnBarbarianUnitAdjacentToCity(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)

	iHostileUnitClassTypeToSpawn = event.getGenericParameter(1)
	iNumHostilesToSpawn = event.getGenericParameter(2)
	for iX in range(iNumHostilesToSpawn):
		city.spawnBarbarianUnitOnAdjacentPlotOfCity(iHostileUnitClassTypeToSpawn)

### Own Player Units - either same Plot or adjacent Plot
### GenericParameter1: UnitClassType to spawn
### GenericParameter2: Number of Units to spawn

# same Plot
def spawnOwnPlayerUnitOnSamePlotAsCity(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)

	iOwnUnitClassTypeToSpawn = event.getGenericParameter(1)
	iNumOwnToSpawn = event.getGenericParameter(2)
	for iX in range(iNumOwnToSpawn):
		city.spawnOwnPlayerUnitOnPlotOfCity(iOwnUnitClassTypeToSpawn)

# adjacent Plot
def spawnOwnPlayerUnitAdjacentToCity(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)

	iOwnUnitClassTypeToSpawn = event.getGenericParameter(1)
	iNumOwnToSpawn = event.getGenericParameter(2)
	for iX in range(iNumOwnToSpawn):
		city.spawnOwnPlayerUnitOnAdjacentPlotOfCity(iOwnUnitClassTypeToSpawn)

### PART C1) PLOT Trigger Check Methods Blueprints
#######################################################
### Those are for the PlotTrigger Triggers to check ###
### ! Just Blueprints to be implemented by Trigger !###
#######################################################

# check for own units
def checkOwnPlayerUnitOnAdjacentPlotOfPlot(argsList): ### When you copy rename specically for your actuall EventTrigger
	eEvent = gc.getInfoTypeForString("EVENT_THAT_STORES_THE_PARAMETERS_TO_CHECK") ### When you copy put in actual Event to read parameters
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	ePlayer = kTriggeredData.ePlayer
	plotThatTriggered = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	iOwnUnitClassTypeToCheck = event.getGenericParameter(1)
	found = plotThatTriggered.isPlayerUnitOnAdjacentPlot(ePlayer, iOwnUnitClassTypeToCheck)
	if (found):
		return True
	return False

# check for Barbarian Units
def checkBarbarianUnitOnAdjacentPlotOfPlot(argsList): ### When you copy rename specically for your actuall EventTrigger
	eEvent = gc.getInfoTypeForString("EVENT_THAT_STORES_THE_PARAMETERS_TO_CHECK") ### When you copy put in actual Event to read parameters
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	plotThatTriggered = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	iBarbarianUnitClassTypeToCheck = event.getGenericParameter(1)
	found = plotThatTriggered.isBarbarianUnitOnAdjacentPlot(iBarbarianUnitClassTypeToCheck)
	if (found):
		return True
	return False

### PART C2) PLOT Trigger Spawn Methods
#####################################################
### Those are for the PlotTrigger Events to spawn ###
#####################################################

### Barbarians Units - either same Plot or adjacent Plot
### GenericParameter1: UnitClassType to spawn
### GenericParameter2: Number of Units to spawn

# same Plot
def spawnBarbarianUnitOnSamePlotAsPlot(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	plotThatTriggered = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	iHostileUnitClassTypeToSpawn = event.getGenericParameter(1)
	iNumHostilesToSpawn = event.getGenericParameter(2)
	for iX in range(iNumHostilesToSpawn):
		plotThatTriggered.spawnBarbarianUnitOnPlot(iHostileUnitClassTypeToSpawn)

# adjacent Plot
def spawnBarbarianUnitAdjacentToPlot(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	plotThatTriggered = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	iHostileUnitClassTypeToSpawn = event.getGenericParameter(1)
	iNumHostilesToSpawn = event.getGenericParameter(2)
	for iX in range(iNumHostilesToSpawn):
		plotThatTriggered.spawnBarbarianUnitOnAdjacentPlot(iHostileUnitClassTypeToSpawn)

### Own Player Units - either same Plot or adjacent Plot
### GenericParameter1: UnitClassType to spawn
### GenericParameter2: Number of Units to spawn

# same Plot
def spawnOwnPlayerUnitOnSamePlotAsPlot(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	ePlayer = kTriggeredData.ePlayer
	plotThatTriggered = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	iOwnUnitClassTypeToSpawn = event.getGenericParameter(1)
	iNumOwnToSpawn = event.getGenericParameter(2)
	for iX in range(iNumOwnToSpawn):
		plotThatTriggered.spawnPlayerUnitOnPlot(ePlayer, iOwnUnitClassTypeToSpawn)

# adjacent Plot
def spawnOwnPlayerUnitAdjacentToPlot(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	ePlayer = kTriggeredData.ePlayer
	plotThatTriggered = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	iOwnUnitClassTypeToSpawn = event.getGenericParameter(1)
	iNumOwnToSpawn = event.getGenericParameter(2)
	for iX in range(iNumOwnToSpawn):
		plotThatTriggered.spawnPlayerUnitOnAdjacentPlot(ePlayer, iOwnUnitClassTypeToSpawn)


######## Generic Helper Methods for specific City UnitClass Checks ###########
######## These helpers are intended for CITY TRIGGERS ###########

def countUnitClassInSpecificCityPopulation(argsList, iUnitClass):
	ePlayer = argsList[1]
	player = gc.getPlayer(ePlayer)
	iCity = argsList[2]
	city = player.getCity(iCity)

	if city.isNone():
		return 0

	iUnitsCurrent = 0

	for iCitizen in range(city.getPopulation()):
		loopUnit = city.getPopulationUnitByIndex(iCitizen)
		if loopUnit.isNone():
			continue
		if loopUnit.getUnitClassType() == iUnitClass:
			iUnitsCurrent += 1

	return iUnitsCurrent


def countUnitClassOnSpecificCityPlot(argsList, iUnitClass):
	ePlayer = argsList[1]
	player = gc.getPlayer(ePlayer)
	iCity = argsList[2]
	city = player.getCity(iCity)

	if city.isNone():
		return 0

	plot = city.plot()
	if plot is None or plot.isNone():
		return 0

	iUnitsCurrent = 0

	for i in range(plot.getNumUnits()):
		loopUnit = plot.getUnit(i)
		if loopUnit.isNone():
			continue
		if loopUnit.getOwner() != ePlayer:
			continue
		if loopUnit.getUnitClassType() == iUnitClass:
			iUnitsCurrent += 1

	return iUnitsCurrent


def countUnitClassInSpecificCityPopulationAndPlot(argsList, iUnitClass, bCheckPopulation, bCheckPlot):
	iUnitsCurrent = 0

	if bCheckPopulation:
		iUnitsCurrent += countUnitClassInSpecificCityPopulation(argsList, iUnitClass)

	if bCheckPlot:
		iUnitsCurrent += countUnitClassOnSpecificCityPlot(argsList, iUnitClass)

	return iUnitsCurrent


def canTriggerSpecificCityUnitClassFromEventParams(argsList):
	eTrigger = argsList[0]
	ePlayer = argsList[1]
	iCity = argsList[2]

	player = gc.getPlayer(ePlayer)
	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	city = player.getCity(iCity)
	if city.isNone():
		return False

	trigger = gc.getEventTriggerInfo(eTrigger)
	if trigger is None:
		return False

	if trigger.getNumEvents() <= 0:
		return False

	eEvent = trigger.getEvent(0)
	if eEvent == -1:
		return False

	event = gc.getEventInfo(eEvent)
	if event is None:
		return False

	iUnitClass = event.getGenericParameter(1)
	iMinUnits = event.getGenericParameter(2)
	bCheckPopulation = (event.getGenericParameter(3) > 0)
	bCheckPlot = (event.getGenericParameter(4) > 0)

	if iUnitClass == -1:
		return False

	if iMinUnits <= 0:
		iMinUnits = 1

	# Failsafe: if both checks are disabled, do not allow the trigger
	if not bCheckPopulation and not bCheckPlot:
		return False

	iUnitsCurrent = countUnitClassInSpecificCityPopulationAndPlot(
		argsList,
		iUnitClass,
		bCheckPopulation,
		bCheckPlot
	)

	if iUnitsCurrent < iMinUnits:
		return False

	return True

######## Generic Helper Methods for specific City UnitClass Event Handling ###########
######## These helpers are intended for CITY TRIGGERS and CITY EVENTS ###########

def _getSpecificCityFromTriggeredData(kTriggeredData):
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return None

	city = player.getCity(kTriggeredData.iCityId)
	if city is None or city.isNone():
		return None

	return city


def _removeFirstMatchingUnitClassFromSpecificCityPopulation(city, iUnitClass):
	if city is None or city.isNone():
		return False

	for iCitizen in range(city.getPopulation()):
		loopUnit = city.getPopulationUnitByIndex(iCitizen)
		if loopUnit.isNone():
			continue
		if loopUnit.getUnitClassType() != iUnitClass:
			continue

		# removePopulationUnit releases the citizen from population,
		# but does not necessarily delete the unit.
		# Therefore we immediately kill the released unit afterwards.
		city.removePopulationUnit(loopUnit, False, ProfessionTypes.NO_PROFESSION)

		if loopUnit is not None and not loopUnit.isNone():
			loopUnit.kill(False)

		return True

	return False


def _removeFirstMatchingUnitClassFromSpecificCityPlot(player, city, iUnitClass):
	if player.isNone():
		return False

	if city is None or city.isNone():
		return False

	plot = city.plot()
	if plot is None or plot.isNone():
		return False

	for i in range(plot.getNumUnits()):
		loopUnit = plot.getUnit(i)
		if loopUnit.isNone():
			continue
		if loopUnit.getOwner() != player.getID():
			continue
		if loopUnit.getUnitClassType() != iUnitClass:
			continue

		loopUnit.kill(False)
		return True

	return False


def removeFirstMatchingUnitClassFromSpecificCity(player, city, iUnitClass, bCheckPopulation, bCheckPlot):
	if player.isNone():
		return False

	if city is None or city.isNone():
		return False

	if iUnitClass == -1:
		return False

	# Prefer city population first because this is usually the more thematic case
	if bCheckPopulation:
		if _removeFirstMatchingUnitClassFromSpecificCityPopulation(city, iUnitClass):
			return True

	if bCheckPlot:
		if _removeFirstMatchingUnitClassFromSpecificCityPlot(player, city, iUnitClass):
			return True

	return False


def _getSpecificCityUnitClassEventParams(event):
	if event is None:
		return (-1, 1, True, True)

	iUnitClass = event.getGenericParameter(1)
	iMinUnits = event.getGenericParameter(2)
	bCheckPopulation = (event.getGenericParameter(3) > 0)
	bCheckPlot = (event.getGenericParameter(4) > 0)

	if iMinUnits <= 0:
		iMinUnits = 1

	return (iUnitClass, iMinUnits, bCheckPopulation, bCheckPlot)


######## Generic Helper Methods for specific City UnitClass Event Effects ###########
######## These helpers are intended for CITY EVENTS ###########

def _isValidSpecificCityUnitClassEventContext(player, city, iUnitClass):
	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	if city is None or city.isNone():
		return False

	if iUnitClass == -1:
		return False

	return True


def applyRemoveSpecificCityUnitClassFromEventParams(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = _getSpecificCityFromTriggeredData(kTriggeredData)

	iUnitClass, iMinUnits, bCheckPopulation, bCheckPlot = _getSpecificCityUnitClassEventParams(event)

	if not _isValidSpecificCityUnitClassEventContext(player, city, iUnitClass):
		return

	iUnitsCurrent = countUnitClassInSpecificCityPopulationAndPlot(
		(kTriggeredData.eTrigger, kTriggeredData.ePlayer, kTriggeredData.iCityId),
		iUnitClass,
		bCheckPopulation,
		bCheckPlot
	)

	if iUnitsCurrent < iMinUnits:
		return

	removeFirstMatchingUnitClassFromSpecificCity(
		player,
		city,
		iUnitClass,
		bCheckPopulation,
		bCheckPlot
	)


def applyRemoveSpecificCityUnitClassFromEventParamsAndSpawnBarbarianAdjacentToCity(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = _getSpecificCityFromTriggeredData(kTriggeredData)

	iUnitClass, iMinUnits, bCheckPopulation, bCheckPlot = _getSpecificCityUnitClassEventParams(event)

	if not _isValidSpecificCityUnitClassEventContext(player, city, iUnitClass):
		return

	iUnitsCurrent = countUnitClassInSpecificCityPopulationAndPlot(
		(kTriggeredData.eTrigger, kTriggeredData.ePlayer, kTriggeredData.iCityId),
		iUnitClass,
		bCheckPopulation,
		bCheckPlot
	)

	if iUnitsCurrent < iMinUnits:
		return

	if not removeFirstMatchingUnitClassFromSpecificCity(
		player,
		city,
		iUnitClass,
		bCheckPopulation,
		bCheckPlot
	):
		return

	spawnBarbarianUnitAdjacentToCity(argsList)

######## Indentured Servant Steals from Employer ###########
getHelpIndenturedServantStealsFromEmployer1 = get_simple_help("TXT_KEY_EVENT_INDENTURED_SERVANT_STEALS_FROM_EMPLOYER_1_HELP")
getHelpIndenturedServantStealsFromEmployer2 = get_simple_help("TXT_KEY_EVENT_INDENTURED_SERVANT_STEALS_FROM_EMPLOYER_2_HELP")

######## Check for city if granary already has been built ###########

def canTriggerBurningCornChamberCity(argsList):
	ePlayer = argsList[1]
	iCity = argsList[2]

	player = gc.getPlayer(ePlayer)
	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	city = player.getCity(iCity)
	if city.isNone():
		return False

	iCornChamberClass = gc.getInfoTypeForString("BUILDINGCLASS_CORN_CHAMBER")
	iGranaryClass = gc.getInfoTypeForString("BUILDINGCLASS_GRANARY")

	if iCornChamberClass == -1 or iGranaryClass == -1:
		return False

	iCornChamber = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationBuildings(iCornChamberClass)
	iGranary = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationBuildings(iGranaryClass)

	if iCornChamber == -1 or iGranary == -1:
		return False

	if not city.isHasBuilding(iCornChamber):
		return False

	if city.isHasBuilding(iGranary):
		return False

	return True


######## Native Trader Attack ###########

def _spawnNativeTraderAttackHostileAdjacent(plot, iUnitClass):
	if plot is None or plot.isNone():
		return None

	if iUnitClass == -1 or iUnitClass == 0:
		return None

	iBarbarian = gc.getGame().getBarbarianPlayer()
	barbPlayer = gc.getPlayer(iBarbarian)
	if barbPlayer.isNone():
		return None

	iUnitType = gc.getCivilizationInfo(barbPlayer.getCivilizationType()).getCivilizationUnits(iUnitClass)
	if iUnitType == UnitTypes.NO_UNIT:
		return None

	for iDX in range(-1, 2):
		for iDY in range(-1, 2):
			if iDX == 0 and iDY == 0:
				continue

			pLoop = plotXY(plot.getX(), plot.getY(), iDX, iDY)

			if pLoop is None or pLoop.isNone():
				continue
			if pLoop.isWater():
				continue
			if pLoop.isImpassable():
				continue
			if pLoop.isPeak():
				continue
			if pLoop.isCity():
				continue
			if pLoop.isUnit():
				continue

			return barbPlayer.initUnit(
				iUnitType,
				ProfessionTypes.NO_PROFESSION,
				pLoop.getX(),
				pLoop.getY(),
				UnitAITypes.NO_UNITAI,
				DirectionTypes.DIRECTION_SOUTH,
				0
			)

	return None


def canTriggerNativeTraderAttack(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit is None or unit.isNone():
		return False

	iExpertTraderClass = gc.getInfoTypeForString("UNITCLASS_EXPERT_TRADER")
	if unit.getUnitClassType() != iExpertTraderClass:
		return False

	eNativeTraderProfession = gc.getInfoTypeForString("PROFESSION_NATIVE_TRADER")
	if unit.getProfession() != eNativeTraderProfession:
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return False

	if plot.isWater():
		return False

	if plot.isCity():
		return False

	return True


def applyNativeTraderAttack(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	if not player.isPlayable():
		return

	if player.isNative():
		return

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit is None or unit.isNone():
		return

	iExpertTraderClass = gc.getInfoTypeForString("UNITCLASS_EXPERT_TRADER")
	if unit.getUnitClassType() != iExpertTraderClass:
		return

	eNativeTraderProfession = gc.getInfoTypeForString("PROFESSION_NATIVE_TRADER")
	if unit.getProfession() != eNativeTraderProfession:
		return

	plot = unit.plot()
	if plot is None or plot.isNone():
		return

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return

	if plot.isWater():
		return

	if plot.isCity():
		return

	iHostileUnitClass = event.getGenericParameter(1)
	iNumHostiles = event.getGenericParameter(2)

	# Freeze native trader for current turn + next turn
	unit.setMoves(0)
	unit.setImmobileTimer(1)

	if iNumHostiles < 1:
		iNumHostiles = 1
	if iNumHostiles > 1:
		iNumHostiles = 1

	hostileUnit = _spawnNativeTraderAttackHostileAdjacent(plot, iHostileUnitClass)
	if hostileUnit is not None and not hostileUnit.isNone():
		if hostileUnit.canMoveInto(plot, True, False, False):
			hostileUnit.attack(plot, False)

getHelpNativeTraderAttack = get_simple_help("TXT_KEY_EVENT_NATIVE_TRADER_ATTACK_HELP")

######## Conquistador Ambush ###########

def _spawnConquistadorAmbushHostileAdjacent(plot, iUnitClass):
	if plot is None or plot.isNone():
		return None

	if iUnitClass == -1 or iUnitClass == 0:
		return None

	iBarbarian = gc.getGame().getBarbarianPlayer()
	barbPlayer = gc.getPlayer(iBarbarian)
	if barbPlayer.isNone():
		return None

	iUnitType = gc.getCivilizationInfo(barbPlayer.getCivilizationType()).getCivilizationUnits(iUnitClass)
	if iUnitType == UnitTypes.NO_UNIT:
		return None

	for iDX in range(-1, 2):
		for iDY in range(-1, 2):
			if iDX == 0 and iDY == 0:
				continue

			pLoop = plotXY(plot.getX(), plot.getY(), iDX, iDY)

			if pLoop is None or pLoop.isNone():
				continue
			if pLoop.isWater():
				continue
			if pLoop.isImpassable():
				continue
			if pLoop.isPeak():
				continue
			if pLoop.isCity():
				continue
			if pLoop.isUnit():
				continue

			return barbPlayer.initUnit(
				iUnitType,
				ProfessionTypes.NO_PROFESSION,
				pLoop.getX(),
				pLoop.getY(),
				UnitAITypes.NO_UNITAI,
				DirectionTypes.DIRECTION_SOUTH,
				0
			)

	return None


def canTriggerConquistadorAmbush(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit is None or unit.isNone():
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	# Sync check to prevent desync issues
	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return False

	if plot.isWater():
		return False

	if plot.isCity():
		return False

	# Allow both conquistador unit classes
	iConq = gc.getInfoTypeForString("UNITCLASS_CONQUISTADOR")
	iMountedConq = gc.getInfoTypeForString("UNITCLASS_MOUNTED_CONQUISTADOR")

	iUnitClass = unit.getUnitClassType()

	if iUnitClass != iConq and iUnitClass != iMountedConq:
		return False

	return True


def applyConquistadorAmbush(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	if not player.isPlayable():
		return

	if player.isNative():
		return

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit is None or unit.isNone():
		return

	plot = unit.plot()
	if plot is None or plot.isNone():
		return

	# Sync check to ensure correct plot
	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return

	if plot.isWater():
		return

	if plot.isCity():
		return

	# Validate unit class again (safety check)
	iConq = gc.getInfoTypeForString("UNITCLASS_CONQUISTADOR")
	iMountedConq = gc.getInfoTypeForString("UNITCLASS_MOUNTED_CONQUISTADOR")

	iUnitClass = unit.getUnitClassType()

	if iUnitClass != iConq and iUnitClass != iMountedConq:
		return

	iHostileUnitClass = event.getGenericParameter(1)

	hostileUnit = _spawnConquistadorAmbushHostileAdjacent(
		plot,
		iHostileUnitClass
	)

	if hostileUnit is None or hostileUnit.isNone():
		return

	# Trigger real combat via DLL
	if hostileUnit.canMoveInto(plot, True, False, False):
		hostileUnit.attack(plot, False)


getHelpConquistadorAmbush = get_simple_help("TXT_KEY_EVENT_CONQUISTADOR_AMBUSH_HELP")

######## Criminal Attacks City ###########

CRIMINALS_BLACKMAIL_CITY_COOLDOWN_PREFIX = "[[WTP_CRIMINALS_BLACKMAIL_CITY_READY_TURN="
CRIMINALS_BLACKMAIL_CITY_COOLDOWN_SUFFIX = "]]"


def _getCriminalsBlackmailCityCooldownReadyTurn(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(CRIMINALS_BLACKMAIL_CITY_COOLDOWN_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(CRIMINALS_BLACKMAIL_CITY_COOLDOWN_PREFIX)
	iEnd = szData.find(CRIMINALS_BLACKMAIL_CITY_COOLDOWN_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	sValue = szData[iStart:iEnd]
	if sValue == "":
		return -1

	try:
		return int(sValue)
	except:
		return -1


def _setCriminalsBlackmailCityCooldownReadyTurn(player, iReadyTurn):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iOldStart = szData.find(CRIMINALS_BLACKMAIL_CITY_COOLDOWN_PREFIX)
	if iOldStart != -1:
		iOldEnd = szData.find(CRIMINALS_BLACKMAIL_CITY_COOLDOWN_SUFFIX, iOldStart)
		if iOldEnd != -1:
			iOldEnd += len(CRIMINALS_BLACKMAIL_CITY_COOLDOWN_SUFFIX)
			szData = szData[:iOldStart] + szData[iOldEnd:]

	szData += CRIMINALS_BLACKMAIL_CITY_COOLDOWN_PREFIX + str(iReadyTurn) + CRIMINALS_BLACKMAIL_CITY_COOLDOWN_SUFFIX
	player.setScriptData(szData)


def _isCriminalsBlackmailCityCooldownActive(player):
	if player.isNone():
		return False

	iReadyTurn = _getCriminalsBlackmailCityCooldownReadyTurn(player)
	if iReadyTurn < 0:
		return False

	return CyGame().getGameTurn() < iReadyTurn


def _startCriminalsBlackmailCityCooldown(player, iTurns):
	if player.isNone():
		return

	iReadyTurn = CyGame().getGameTurn() + iTurns
	_setCriminalsBlackmailCityCooldownReadyTurn(player, iReadyTurn)


def canTriggerCriminalsBlackmailCity(argsList):
	ePlayer = argsList[1]
	iCity = argsList[2]

	player = gc.getPlayer(ePlayer)
	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	city = player.getCity(iCity)
	if city.isNone():
		return False

	if city.getOwner() != ePlayer:
		return False

	if city.isOccupation():
		return False

	iHappiness = city.getCityHappiness()
	iUnhappiness = city.getCityUnHappiness()

	if iUnhappiness <= iHappiness:
		return False

	if city.getPopulation() < 3:
		return False

	iChance = 20
	if _isCriminalsBlackmailCityCooldownActive(player):
		iChance = 1

	if CyGame().getSorenRandNum(100, "Criminals Blackmail City trigger chance") >= iChance:
		return False

	return True


def canDoCriminalsBlackmailCityGive(argsList):
	if len(argsList) < 1:
		return False

	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return False

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return False

	iFoodCost = 50
	gameSpeed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	iFoodCost = iFoodCost * gameSpeed.getStoragePercent() / 100

	if city.getFood() < iFoodCost:
		return False

	return True


def _cityHasAnyDefenderOnCityPlot(city):
	if city.isNone():
		return False

	pCityPlot = city.plot()
	if pCityPlot is None:
		return False

	for i in range(pCityPlot.getNumUnits()):
		loopUnit = pCityPlot.getUnit(i)

		if loopUnit is None or loopUnit.isNone():
			continue
		if loopUnit.getOwner() != city.getOwner():
			continue
		if loopUnit.isDead():
			continue
		if loopUnit.getDomainType() != DomainTypes.DOMAIN_LAND:
			continue
		if not loopUnit.canDefend(pCityPlot):
			continue

		return True

	return False

def _spawnRevoltingCriminalAdjacentToCity(city):
	if city.isNone():
		return None

	player = gc.getPlayer(city.getOwner())
	if player.isNone():
		return None

	iBarbarian = gc.getGame().getBarbarianPlayer()
	barbPlayer = gc.getPlayer(iBarbarian)
	if barbPlayer.isNone():
		return None

	iUnitClass = gc.getInfoTypeForString("UNITCLASS_REVOLTING_CRIMINAL")
	if iUnitClass == -1:
		return None

	iUnitType = gc.getCivilizationInfo(barbPlayer.getCivilizationType()).getCivilizationUnits(iUnitClass)
	if iUnitType == -1:
		return None

	pCityPlot = city.plot()
	if pCityPlot is None:
		return None

	for iDX in range(-1, 2):
		for iDY in range(-1, 2):
			if iDX == 0 and iDY == 0:
				continue

			pLoop = plotXY(city.getX(), city.getY(), iDX, iDY)
			if pLoop is None or pLoop.isNone():
				continue
			if pLoop.isWater():
				continue
			if pLoop.isImpassable():
				continue
			if pLoop.isPeak():
				continue
			if pLoop.isCity():
				continue
			if pLoop.isUnit():
				continue

			pUnit = barbPlayer.initUnit(
				iUnitType,
				ProfessionTypes.NO_PROFESSION,
				pLoop.getX(),
				pLoop.getY(),
				UnitAITypes.NO_UNITAI,
				DirectionTypes.DIRECTION_SOUTH,
				0
			)

			if pUnit is not None and not pUnit.isNone():
				return pUnit

	return None

def applyGiveFood(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return False

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return False

	iYield = gc.getInfoTypeForString("YIELD_FOOD")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent() / 100

	if city.getYieldStored(iYield) < quantity:
		return False

	city.changeYieldStored(iYield, -quantity)

	_startCriminalsBlackmailCityCooldown(player, 75)
	return True


def getHelpGiveFood(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)

	iYield = gc.getInfoTypeForString("YIELD_FOOD")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent() / 100

	szHelp = ""
	if event.getGenericParameter(1) <> 0:
		szHelp = localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (-quantity, gc.getYieldInfo(iYield).getChar(), city.getNameKey()))

	return szHelp


def _applyDirectSuccessfulCriminalRaid(city):
	if city.isNone():
		return False

	pCityPlot = city.plot()
	if pCityPlot is None:
		return False

	player = gc.getPlayer(city.getOwner())
	if player.isNone():
		return False

	eKing = player.getParent()
	king = gc.getPlayer(eKing)
	if king is None or king.isNone():
		return False

	bestYield = -1
	bestValue = 0
	bestLoot = 0
	bestType = "none"

	iFoodYield = gc.getInfoTypeForString("YIELD_FOOD")

	# --- GOLD OPTION ---
	iGold = player.getGold()
	if iGold > 0:
		iGoldLoot = min(iGold, 80 + CyGame().getSorenRandNum(121, "Criminal raid gold"))
		if iGoldLoot > 0:
			if iGoldLoot > bestValue:
				bestValue = iGoldLoot
				bestLoot = iGoldLoot
				bestType = "gold"

	# --- YIELD OPTIONS ---
	for iYield in range(YieldTypes.NUM_YIELD_TYPES):
		eYield = YieldTypes(iYield)

		if eYield == iFoodYield:
			continue

		iStored = city.getYieldStored(eYield)
		if iStored <= 0:
			continue

		iMaxLoot = min(iStored, 20 + CyGame().getSorenRandNum(31, "Criminal raid goods"))
		if iMaxLoot <= 0:
			continue

		iPrice = king.getYieldBuyPrice(eYield)
		iTotalValue = iPrice * iMaxLoot

		if iTotalValue > bestValue:
			bestValue = iTotalValue
			bestYield = eYield
			bestLoot = iMaxLoot
			bestType = "yield"

	# --- EXECUTE RAID ---

	if bestType == "gold" and bestLoot > 0:
		player.changeGold(-bestLoot)

		CyInterface().addMessage(
			player.getID(),
			True,
			gc.getEVENT_MESSAGE_TIME(),
			localText.getText(
				"TXT_KEY_EVENT_CRIMINALS_BLACKMAIL_CITY_DIRECT_RAID_GOLD",
				(city.getNameKey(), bestLoot)
			),
			"AS2D_CITYRAID",
			InterfaceMessageTypes.MESSAGE_TYPE_MINOR_EVENT,
			None,
			gc.getInfoTypeForString("COLOR_RED"),
			pCityPlot.getX(),
			pCityPlot.getY(),
			True,
			True
		)
		return True

	if bestType == "yield" and bestYield != -1 and bestLoot > 0:
		city.changeYieldStored(bestYield, -bestLoot)

		sYieldName = gc.getYieldInfo(bestYield).getDescription()

		CyInterface().addMessage(
			player.getID(),
			True,
			gc.getEVENT_MESSAGE_TIME(),
			localText.getText(
				"TXT_KEY_EVENT_CRIMINALS_BLACKMAIL_CITY_DIRECT_RAID_GOODS",
				(
					city.getNameKey(),
					bestLoot,
					sYieldName
				)
			),
			"AS2D_CITYRAID",
			InterfaceMessageTypes.MESSAGE_TYPE_MINOR_EVENT,
			gc.getYieldInfo(bestYield).getButton(),
			gc.getInfoTypeForString("COLOR_RED"),
			pCityPlot.getX(),
			pCityPlot.getY(),
			True,
			True
		)
		return True

	CyInterface().addMessage(
		player.getID(),
		True,
		gc.getEVENT_MESSAGE_TIME(),
		localText.getText(
			"TXT_KEY_EVENT_CRIMINALS_BLACKMAIL_CITY_DIRECT_RAID_NOTHING",
			(city.getNameKey(),)
		),
		"AS2D_CITYRAID",
		InterfaceMessageTypes.MESSAGE_TYPE_MINOR_EVENT,
		None,
		gc.getInfoTypeForString("COLOR_RED"),
		pCityPlot.getX(),
		pCityPlot.getY(),
		True,
		True
	)

	return True


def applyCriminalsBlackmailCityRefuse(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return False

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return False

	_startCriminalsBlackmailCityCooldown(player, 50)

	# Always spawn one visible criminal next to the city if possible
	_spawnRevoltingCriminalAdjacentToCity(city)

	# Defender on city plot -> 50/50 outcome
	if _cityHasAnyDefenderOnCityPlot(city):
		iRoll = CyGame().getSorenRandNum(100, "Criminals Blackmail City defended outcome")

		# 50%: city successfully defended
		if iRoll < 50:
			CyInterface().addMessage(
				player.getID(),
				True,
				gc.getEVENT_MESSAGE_TIME(),
				localText.getText(
					"TXT_KEY_EVENT_CRIMINALS_BLACKMAIL_CITY_DEFENDED_ABORTED",
					(city.getNameKey(),)
				),
				"AS2D_GOODNEWS",
				InterfaceMessageTypes.MESSAGE_TYPE_MINOR_EVENT,
				None,
				gc.getInfoTypeForString("COLOR_GREEN"),
				city.getX(),
				city.getY(),
				True,
				True
			)
			return True

		# 50%: defenders fail to prevent the raid
		CyInterface().addMessage(
			player.getID(),
			True,
			gc.getEVENT_MESSAGE_TIME(),
			localText.getText(
				"TXT_KEY_EVENT_CRIMINALS_BLACKMAIL_CITY_DEFENDED_BUT_RAIDED",
				(city.getNameKey(),)
			),
			"AS2D_CITYRAID",
			InterfaceMessageTypes.MESSAGE_TYPE_MINOR_EVENT,
			None,
			gc.getInfoTypeForString("COLOR_RED"),
			city.getX(),
			city.getY(),
			True,
			True
		)

		return _applyDirectSuccessfulCriminalRaid(city)

	# No defender on city plot -> direct successful raid
	return _applyDirectSuccessfulCriminalRaid(city)

getHelpCriminalsBlackmailCityRefuse = get_simple_help("TXT_KEY_EVENT_CRIMINALS_BLACKMAIL_CITY_REFUSE_HELP")

######## Ferry Station Robbers ###########

######## Ferry Station Robbers ###########

def canTriggerFerryStationRobbers(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	plot = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	if plot is None or plot.isNone():
		return False

	if plot.getOwner() != player.getID():
		return False

	iFerryStation = gc.getInfoTypeForString("IMPROVEMENT_RAFT_STATION")
	if iFerryStation == -1:
		return False

	if plot.getImprovementType() != iFerryStation:
		return False

	# Keep existing chance logic
	if not TriggerChance(argsList):
		return False

	return True


def _getFerryStationBattlePlot(plot):
	if plot is None or plot.isNone():
		return None

	for iDX in range(-1, 2):
		for iDY in range(-1, 2):
			if iDX == 0 and iDY == 0:
				continue

			pLoop = plotXY(plot.getX(), plot.getY(), iDX, iDY)

			if pLoop is None or pLoop.isNone():
				continue
			if pLoop.isWater():
				continue
			if pLoop.isImpassable():
				continue
			if pLoop.isPeak():
				continue
			if pLoop.isCity():
				continue

			return pLoop

	return None


def _spawnFerryStationRobbersHostileAdjacent(plot, iUnitClass, iNumUnits):
	if plot is None or plot.isNone():
		return None

	if iUnitClass == -1 or iUnitClass == 0:
		return None

	iBarbarian = gc.getGame().getBarbarianPlayer()
	barbPlayer = gc.getPlayer(iBarbarian)
	if barbPlayer.isNone():
		return None

	iUnitType = gc.getCivilizationInfo(barbPlayer.getCivilizationType()).getCivilizationUnits(iUnitClass)
	if iUnitType == UnitTypes.NO_UNIT:
		return None

	if iNumUnits < 1:
		iNumUnits = 1
	if iNumUnits > 3:
		iNumUnits = 3

	spawnedUnit = None

	for i in range(iNumUnits):
		bSpawnedThisOne = False

		for iDX in range(-1, 2):
			for iDY in range(-1, 2):
				if iDX == 0 and iDY == 0:
					continue

				pLoop = plotXY(plot.getX(), plot.getY(), iDX, iDY)

				if pLoop is None or pLoop.isNone():
					continue
				if pLoop.isImpassable():
					continue
				if pLoop.isPeak():
					continue
				if pLoop.isCity():
					continue
				if pLoop.isUnit():
					continue

				unit = barbPlayer.initUnit(
					iUnitType,
					ProfessionTypes.NO_PROFESSION,
					pLoop.getX(),
					pLoop.getY(),
					UnitAITypes.NO_UNITAI,
					DirectionTypes.DIRECTION_SOUTH,
					0
				)

				if spawnedUnit is None:
					spawnedUnit = unit

				bSpawnedThisOne = True
				break

			if bSpawnedThisOne:
				break

	return spawnedUnit


def _spawnFerryStationDefendersOnPlot(plot, ePlayer, iUnitClass, iNumUnits):
	if plot is None or plot.isNone():
		return

	player = gc.getPlayer(ePlayer)
	if player.isNone():
		return

	if iUnitClass == -1 or iUnitClass == 0:
		return

	if iNumUnits < 1:
		iNumUnits = 1
	if iNumUnits > 3:
		iNumUnits = 3

	iUnitType = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationUnits(iUnitClass)
	if iUnitType == UnitTypes.NO_UNIT:
		return

	for i in range(iNumUnits):
		player.initUnit(
			iUnitType,
			ProfessionTypes.NO_PROFESSION,
			plot.getX(),
			plot.getY(),
			UnitAITypes.NO_UNITAI,
			DirectionTypes.DIRECTION_SOUTH,
			0
		)


def _spawnFerryStationDefendersWithFallback(ferryPlot, ePlayer, iUnitClass, iNumUnits):
	if ferryPlot is None or ferryPlot.isNone():
		return None

	player = gc.getPlayer(ePlayer)
	if player.isNone():
		return None

	if iUnitClass == -1 or iUnitClass == 0:
		return None

	if iNumUnits < 1:
		iNumUnits = 1
	if iNumUnits > 3:
		iNumUnits = 3

	iUnitType = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationUnits(iUnitClass)
	if iUnitType == UnitTypes.NO_UNIT:
		return None

	# First try directly on the ferry plot
	firstUnit = player.initUnit(
		iUnitType,
		ProfessionTypes.NO_PROFESSION,
		ferryPlot.getX(),
		ferryPlot.getY(),
		UnitAITypes.NO_UNITAI,
		DirectionTypes.DIRECTION_SOUTH,
		0
	)

	if firstUnit is not None and not firstUnit.isNone():
		for i in range(1, iNumUnits):
			player.initUnit(
				iUnitType,
				ProfessionTypes.NO_PROFESSION,
				ferryPlot.getX(),
				ferryPlot.getY(),
				UnitAITypes.NO_UNITAI,
				DirectionTypes.DIRECTION_SOUTH,
				0
			)
		return ferryPlot

	# Fallback to adjacent land plot
	battlePlot = _getFerryStationBattlePlot(ferryPlot)
	if battlePlot is None or battlePlot.isNone():
		return None

	for i in range(iNumUnits):
		player.initUnit(
			iUnitType,
			ProfessionTypes.NO_PROFESSION,
			battlePlot.getX(),
			battlePlot.getY(),
			UnitAITypes.NO_UNITAI,
			DirectionTypes.DIRECTION_SOUTH,
			0
		)

	return battlePlot

def _sendFerryStationRobbersMessage(ePlayer, szTextKey, plot, iColor):
	if plot is None or plot.isNone():
		return

	CyInterface().addMessage(
		ePlayer,
		True,
		gc.getEVENT_MESSAGE_TIME(),
		localText.getText(szTextKey, ()),
		None,
		0,
		None,
		ColorTypes(iColor),
		plot.getX(),
		plot.getY(),
		True,
		True
	)
    
def applyFerryStationRobbers1(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	if not player.isPlayable():
		return

	if player.isNative():
		return

	ferryPlot = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	if ferryPlot is None or ferryPlot.isNone():
		return

	if ferryPlot.getOwner() != player.getID():
		return

	iFerryStation = gc.getInfoTypeForString("IMPROVEMENT_RAFT_STATION")
	if iFerryStation == -1:
		return

	if ferryPlot.getImprovementType() != iFerryStation:
		return

	_sendFerryStationRobbersMessage(
		kTriggeredData.ePlayer,
		"TXT_KEY_EVENT_FERRY_STATION_ROBBERS_DEFEND_CHOICE",
		ferryPlot,
		10  # weiß
	)

	iHostileUnitClass = event.getGenericParameter(1)
	iNumHostiles = event.getGenericParameter(2)
	iDefenderUnitClass = event.getGenericParameter(3)
	iNumDefenders = event.getGenericParameter(4)

	# Spawn defenders (prefer ferryPlot, fallback if needed)
	defenderPlot = _spawnFerryStationDefendersWithFallback(
		ferryPlot,
		kTriggeredData.ePlayer,
		iDefenderUnitClass,
		iNumDefenders
	)

	if defenderPlot is None or defenderPlot.isNone():
		return

	# Spawn attackers adjacent to defender plot
	hostileUnit = _spawnFerryStationRobbersHostileAdjacent(
		defenderPlot,
		iHostileUnitClass,
		iNumHostiles
	)

	if hostileUnit is None or hostileUnit.isNone():
		return

	# 100% direct attack
	if hostileUnit.canMoveInto(defenderPlot, True, False, False):
		hostileUnit.attack(defenderPlot, False)

def applyFerryStationRobbers2(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	if not player.isPlayable():
		return

	if player.isNative():
		return

	ferryPlot = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	if ferryPlot is None or ferryPlot.isNone():
		return

	if ferryPlot.getOwner() != player.getID():
		return

	iFerryStation = gc.getInfoTypeForString("IMPROVEMENT_RAFT_STATION")
	if iFerryStation == -1:
		return

	if ferryPlot.getImprovementType() != iFerryStation:
		return

	_sendFerryStationRobbersMessage(
		kTriggeredData.ePlayer,
		"TXT_KEY_EVENT_FERRY_STATION_ROBBERS_DESTROY_CHOICE",
		ferryPlot,
		7  # rot
	)

	iHostileUnitClass = event.getGenericParameter(1)
	iNumHostiles = event.getGenericParameter(2)

	# Spawn attackers adjacent to the ferry station
	_spawnFerryStationRobbersHostileAdjacent(
		ferryPlot,
		iHostileUnitClass,
		iNumHostiles
	)

	# Destroy ferry station
	ferryPlot.setImprovementType(ImprovementTypes.NO_IMPROVEMENT)

	# Smoke / fire effect
	CyEngine().triggerEffect(
		gc.getInfoTypeForString("EFFECT_CITY_BIG_BURNING_SMOKE"),
		ferryPlot.getPoint()
	)

	# Plunder / destruction sound
	CyInterface().playGeneralSound("AS2D_CITYRAID")


def getHelpFerryStationRobbers(argsList):
	return localText.getText("TXT_KEY_EVENT_FERRY_STATION_ROBBERS_HELP", ())

######## Buccanners attack Silver Mine ###########

getHelpBuccanneersAttackMine = get_simple_help("TXT_KEY_EVENT_BUCCANNERS_ATTACK_MINE_HELP")

getHelpMilitiaDefend = get_simple_help("TXT_KEY_EVENT_MILITIA_DEFENDS_MINE_HELP")

######## Discovery Start Event  ###########

getHelpDiscoveryConquistador = get_simple_help("TXT_KEY_EVENT_DISCOVERY_EVENTS_START_CONQUISTADOR_HELP")

getHelpDiscoveryMissionary = get_simple_help("TXT_KEY_EVENT_DISCOVERY_EVENTS_START_MISSIONARY_HELP")

getHelpDiscoveryTrader = get_simple_help("TXT_KEY_EVENT_DISCOVERY_EVENTS_START_SEASONED_TRADER_HELP")

getHelpDiscoveryOxcart = get_simple_help("TXT_KEY_EVENT_DISCOVERY_EVENTS_START_Oxcart_HELP")

######## Treasure Attack Event ###########

TREASURE_ATTACK_SOFT_COOLDOWN_PREFIX = "[[WTP_TREASURE_ATTACK_SOFT_READY_TURN="
TREASURE_ATTACK_SOFT_COOLDOWN_SUFFIX = "]]"

def _getTreasureAttackSoftCooldownReadyTurn(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(TREASURE_ATTACK_SOFT_COOLDOWN_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(TREASURE_ATTACK_SOFT_COOLDOWN_PREFIX)
	iEnd = szData.find(TREASURE_ATTACK_SOFT_COOLDOWN_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1

def _setTreasureAttackSoftCooldownReadyTurn(player, iReadyTurn):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(TREASURE_ATTACK_SOFT_COOLDOWN_PREFIX)
	if iStart != -1:
		iEnd = szData.find(TREASURE_ATTACK_SOFT_COOLDOWN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(TREASURE_ATTACK_SOFT_COOLDOWN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szMarker = "%s%d%s" % (
		TREASURE_ATTACK_SOFT_COOLDOWN_PREFIX,
		iReadyTurn,
		TREASURE_ATTACK_SOFT_COOLDOWN_SUFFIX
	)

	szData += szMarker
	player.setScriptData(szData)

def _startTreasureAttackSoftCooldown(player):
	if player.isNone():
		return

	_setTreasureAttackSoftCooldownReadyTurn(player, CyGame().getGameTurn() + 50)

def _isTreasureAttackSoftCooldownActive(player):
	if player.isNone():
		return False

	iReadyTurn = _getTreasureAttackSoftCooldownReadyTurn(player)
	return iReadyTurn > CyGame().getGameTurn()

def getDistanceToOwnTerritory(plot, player, iMaxRange):
	for iRange in range(1, iMaxRange + 1):
		for iDX in range(-iRange, iRange + 1):
			for iDY in range(-iRange, iRange + 1):
				if abs(iDX) != iRange and abs(iDY) != iRange:
					continue

				pLoop = plotXY(plot.getX(), plot.getY(), iDX, iDY)
				if pLoop is None or pLoop.isNone():
					continue

				if pLoop.getOwner() == player.getID():
					return iRange

	return iMaxRange + 1

def _getTreasureAttackChance(player, plot):
	iDistance = getDistanceToOwnTerritory(plot, player, 10)

	if _isTreasureAttackSoftCooldownActive(player):
		if iDistance <= 5:
			return 0
		elif iDistance <= 10:
			return 2
		else:
			return 5

	if iDistance <= 5:
		return 5
	elif iDistance <= 10:
		return 10
	else:
		return 15

def _spawnTreasureAttackHostileAdjacent(plot, iUnitClass):
	if plot is None or plot.isNone():
		return None

	if iUnitClass == -1:
		return None

	iBarbarian = gc.getGame().getBarbarianPlayer()
	barbPlayer = gc.getPlayer(iBarbarian)
	if barbPlayer.isNone():
		return None

	barbCiv = gc.getCivilizationInfo(barbPlayer.getCivilizationType())
	iUnitType = barbCiv.getCivilizationUnits(iUnitClass)
	if iUnitType == UnitTypes.NO_UNIT:
		return None

	for iDX in range(-1, 2):
		for iDY in range(-1, 2):
			if iDX == 0 and iDY == 0:
				continue

			pLoop = plotXY(plot.getX(), plot.getY(), iDX, iDY)
			if pLoop is None or pLoop.isNone():
				continue
			if pLoop.isWater():
				continue
			if pLoop.isImpassable():
				continue
			if pLoop.isPeak():
				continue
			if pLoop.isCity():
				continue
			if pLoop.isUnit():
				continue

			unit = barbPlayer.initUnit(
				iUnitType,
				ProfessionTypes.NO_PROFESSION,
				pLoop.getX(),
				pLoop.getY(),
				UnitAITypes.NO_UNITAI,
				DirectionTypes.DIRECTION_SOUTH,
				0
			)

			if unit is not None and not unit.isNone():
				return unit

	return None

def _immobilizeTreasureStack(plot, iPlayer):
	for i in range(plot.getNumUnits()):
		unit = plot.getUnit(i)
		if unit.isNone():
			continue
		if unit.getOwner() == iPlayer:
			unit.setMoves(0)
			unit.setImmobileTimer(1)

def _forceUnitToMoveToPlot(unit, targetPlot):
	if unit is None or unit.isNone():
		return

	group = unit.getGroup()
	if group is None:
		return

	group.pushMission(
		MissionTypes.MISSION_MOVE_TO,
		targetPlot.getX(),
		targetPlot.getY(),
		0,
		False,
		True,
		MissionAITypes.NO_MISSIONAI,
		targetPlot,
		unit
	)

def canTriggerTreasureAttack(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False

	# Exact unit type check
	if unit.getUnitClassType() != gc.getInfoTypeForString("UNITCLASS_TREASURE"):
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	# Must be land
	if plot.isWater():
		return False

	# Wilderness only: no city plot
	if plot.isCity():
		return False

	# Wilderness only: not on own territory
	if plot.getOwner() == player.getID():
		return False

	iChance = _getTreasureAttackChance(player, plot)

	if CyGame().getSorenRandNum(100, "Treasure Attack Trigger") >= iChance:
		return False

	# Start soft cooldown here so both event choices get it,
	# including the XML-only pay option without Python callback.
	_startTreasureAttackSoftCooldown(player)

	return True

def applyTreasureAttack(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return

	# Ensure we still operate on a treasure unit
	if unit.getUnitClassType() != gc.getInfoTypeForString("UNITCLASS_TREASURE"):
		return

	plot = unit.plot()
	if plot is None or plot.isNone():
		return

	if plot.isWater():
		return

	if plot.isCity():
		return

	iHostileUnitClass = event.getGenericParameter(1)
	iNumHostiles = event.getGenericParameter(2)
	iImmobilize = event.getGenericParameter(3)

	if iImmobilize > 0:
		_immobilizeTreasureStack(plot, kTriggeredData.ePlayer)

	if iNumHostiles < 1:
		iNumHostiles = 1
	if iNumHostiles > 1:
		iNumHostiles = 1

	hostileUnit = _spawnTreasureAttackHostileAdjacent(plot, iHostileUnitClass)

	if hostileUnit is not None and not hostileUnit.isNone():
		if hostileUnit.canMoveInto(plot, True, False, False):
			hostileUnit.attack(plot, False)

def getHelpDiscoveryTreasureAttack(argsList):
	return localText.getText("TXT_KEY_EVENT_DISCOVERY_EVENTS_TREASURE_ATTACK_HELP", ())

######## Slave Hunter Offers Service ###########

def checkRunawaySlavesOnAdjacentPlotOfCity(argsList): ### When you copy rename specically for your actuall EventTrigger
	eEvent = gc.getInfoTypeForString("EVENT_SLAVE_HUNTER_SERVICE_ACCEPT") ### When you copy put in actual Event to read parameters
	event = gc.getEventInfo(eEvent)
	ePlayer = argsList[1]
	iCityIdThatTriggered = argsList[2]
	player = gc.getPlayer(ePlayer)
	city = player.getCity(iCityIdThatTriggered)

	iBarbarianUnitClassTypeToCheck = event.getGenericParameter(1)
	found = city.isBarbarianUnitOnAdjacentPlotOfCity(iBarbarianUnitClassTypeToCheck)
	if (found):
		return True

	iBarbarianUnitClassTypeToCheck2 = event.getGenericParameter(2)
	found = city.isBarbarianUnitOnAdjacentPlotOfCity(iBarbarianUnitClassTypeToCheck2)
	if (found):
		return True

	iBarbarianUnitClassTypeToCheck3 = event.getGenericParameter(2)
	found = city.isBarbarianUnitOnAdjacentPlotOfCity(iBarbarianUnitClassTypeToCheck3)
	if (found):
		return True
	return False

######## General Attack Function ###########

def canTriggerIsPlayableWithTriggerChance(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if not player.isPlayable():
		return False
	# Read parameter 3 from the event as random chance
	if TriggerChance(argsList):
		return True
	return False

# adjacent Plot for Barbarian, same Plot for own Unit
def spawnBarbarianUnitAdjacentToUnitAndFriendlyOnSamePlot(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unitThatTriggered = player.getUnit(kTriggeredData.iUnitId)
	# This part spawns the Barbarian
	iHostileUnitClassTypeToSpawn = event.getGenericParameter(1)
	iNumHostilesToSpawn = event.getGenericParameter(2)
	for iX in range(iNumHostilesToSpawn):
		unitThatTriggered.spawnBarbarianUnitOnAdjacentPlotOfUnit(iHostileUnitClassTypeToSpawn)
	# This Part spawns the Friendly
	iOwnUnitClassTypeToSpawn = event.getGenericParameter(4)
	iNumOwnToSpawn = event.getGenericParameter(2)
	for iX in range(iNumOwnToSpawn):
		unitThatTriggered.spawnOwnPlayerUnitOnPlotOfUnit(iOwnUnitClassTypeToSpawn)

######## Whale Attack ###########

def canTriggerWhaleAttack(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return False

	if not plot.isWater():
		return False

	# One-time event -> no cooldown required
	return True


def _spawnWhaleAttackHostileAdjacent(plot, iUnitClass):
	if plot is None or plot.isNone():
		return None

	if iUnitClass == -1:
		return None

	iBarbarian = gc.getGame().getBarbarianPlayer()
	barbPlayer = gc.getPlayer(iBarbarian)
	if barbPlayer.isNone():
		return None

	iUnitType = gc.getCivilizationInfo(barbPlayer.getCivilizationType()).getCivilizationUnits(iUnitClass)
	if iUnitType == UnitTypes.NO_UNIT:
		return None

	for iDX in range(-1, 2):
		for iDY in range(-1, 2):
			if iDX == 0 and iDY == 0:
				continue

			pLoop = plotXY(plot.getX(), plot.getY(), iDX, iDY)

			if pLoop is None or pLoop.isNone():
				continue
			if not pLoop.isWater():
				continue
			if pLoop.isImpassable():
				continue
			if pLoop.isCity():
				continue
			if pLoop.isUnit():
				continue

			return barbPlayer.initUnit(
				iUnitType,
				ProfessionTypes.NO_PROFESSION,
				pLoop.getX(),
				pLoop.getY(),
				UnitAITypes.NO_UNITAI,
				DirectionTypes.DIRECTION_SOUTH,
				0
			)

	return None


def applyWhaleAttack(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	if not player.isPlayable():
		return

	if player.isNative():
		return

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return

	plot = unit.plot()
	if plot is None or plot.isNone():
		return

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return

	if not plot.isWater():
		return

	iHostileUnitClass = event.getGenericParameter(1)
	iNumHostiles = event.getGenericParameter(2)

	if iNumHostiles < 1:
		iNumHostiles = 1
	if iNumHostiles > 1:
		iNumHostiles = 1

	hostileUnit = _spawnWhaleAttackHostileAdjacent(plot, iHostileUnitClass)
	if hostileUnit is None or hostileUnit.isNone():
		return

	if hostileUnit.canMoveInto(plot, True, False, False):
		hostileUnit.attack(plot, False)

	# Reward whale blubber only if ship survived and whale died
	unitAfterCombat = player.getUnit(kTriggeredData.iUnitId)
	if unitAfterCombat is None or unitAfterCombat.isNone():
		return

	if hostileUnit is not None and not hostileUnit.isNone():
		return

	iYieldWhaleBlubber = gc.getInfoTypeForString("YIELD_WHALE_BLUBBER")
	if iYieldWhaleBlubber == -1:
		return

	iReward = event.getGenericParameter(3)
	if iReward <= 0:
		iReward = 20

	unitAfterCombat.changeYieldStored(iYieldWhaleBlubber, iReward)

getHelpWhaleAttack = get_simple_help("TXT_KEY_WHALE_ATTACK_HELP")

######## Ranger Bear Attack ###########

def canTriggerRangerBearAttack(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	ranger = player.getUnit(kTriggeredData.iUnitId)
	if ranger.isNone():
		return False

	plot = ranger.plot()
	if plot is None or plot.isNone():
		return False

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return False

	# Optional: only allow land plots
	if plot.isWater():
		return False

	# One-time event → no cooldown required
	return True


def applyRangerBearAttack(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	if not player.isPlayable():
		return

	if player.isNative():
		return

	ranger = player.getUnit(kTriggeredData.iUnitId)
	if ranger.isNone():
		return

	plot = ranger.plot()
	if plot is None or plot.isNone():
		return

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return

	if plot.isWater():
		return

	# =========================
	# Spawn grizzly (hostile unit on adjacent plot)
	# =========================
	iBarbarian = gc.getGame().getBarbarianPlayer()
	barbPlayer = gc.getPlayer(iBarbarian)

	if barbPlayer.isNone():
		return

	iGrizzlyType = gc.getInfoTypeForString("UNIT_GRIZZLY")
	if iGrizzlyType == -1:
		return

	hostileUnit = None

	for iDX in range(-1, 2):
		for iDY in range(-1, 2):
			if iDX == 0 and iDY == 0:
				continue

			pLoop = plotXY(plot.getX(), plot.getY(), iDX, iDY)

			if pLoop is None or pLoop.isNone():
				continue
			if pLoop.isWater():
				continue
			if pLoop.isImpassable():
				continue
			if pLoop.isPeak():
				continue
			if pLoop.isCity():
				continue
			if pLoop.isUnit():
				continue

			hostileUnit = barbPlayer.initUnit(
				iGrizzlyType,
				ProfessionTypes.NO_PROFESSION,
				pLoop.getX(),
				pLoop.getY(),
				UnitAITypes.NO_UNITAI,
				DirectionTypes.DIRECTION_SOUTH,
				0
			)
			break
		if hostileUnit is not None and not hostileUnit.isNone():
			break

	# =========================
	# Freeze ranger (current turn + next turn)
	# =========================
	ranger.setMoves(0)
	ranger.setImmobileTimer(1)

	# =========================
	# Direct combat against the ranger
	# =========================
	if hostileUnit is not None and not hostileUnit.isNone():
		pTargetPlot = ranger.plot()
		if pTargetPlot is not None and not pTargetPlot.isNone():
			hostileUnit.attack(pTargetPlot, False)

	# =========================
	# Spawn missionary only if ranger survived
	# =========================
	iMissionaryClass = event.getGenericParameter(4)

	if iMissionaryClass != -1:
		rangerAfterCombat = player.getUnit(kTriggeredData.iUnitId)
		if rangerAfterCombat is not None and not rangerAfterCombat.isNone():
			pMissionaryPlot = rangerAfterCombat.plot()
			if pMissionaryPlot is not None and not pMissionaryPlot.isNone():
				iMissionaryType = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationUnits(iMissionaryClass)

				if iMissionaryType != UnitTypes.NO_UNIT:
					player.initUnit(
						iMissionaryType,
						ProfessionTypes.NO_PROFESSION,
						pMissionaryPlot.getX(),
						pMissionaryPlot.getY(),
						UnitAITypes.NO_UNITAI,
						DirectionTypes.NO_DIRECTION,
						0
					)


def getHelpRangerBearAttack(argsList):
	return localText.getText("TXT_KEY_EVENT_RANGER_BEAR_ATTACK_HELP", ())

######## Highwayman Attack ###########

HIGHWAYMAN_ATTACK_SOFT_COOLDOWN_PREFIX = "[[WTP_HIGHWAYMAN_ATTACK_SOFT_READY_TURN="
HIGHWAYMAN_ATTACK_SOFT_COOLDOWN_SUFFIX = "]]"

HIGHWAYMAN_ATTACK_BRIBE_GOLD = 500

def _getHighwaymanAttackSoftCooldownReadyTurn(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(HIGHWAYMAN_ATTACK_SOFT_COOLDOWN_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(HIGHWAYMAN_ATTACK_SOFT_COOLDOWN_PREFIX)
	iEnd = szData.find(HIGHWAYMAN_ATTACK_SOFT_COOLDOWN_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1

def _setHighwaymanAttackSoftCooldownReadyTurn(player, iReadyTurn):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(HIGHWAYMAN_ATTACK_SOFT_COOLDOWN_PREFIX)
	if iStart != -1:
		iEnd = szData.find(HIGHWAYMAN_ATTACK_SOFT_COOLDOWN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(HIGHWAYMAN_ATTACK_SOFT_COOLDOWN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szMarker = "%s%d%s" % (
		HIGHWAYMAN_ATTACK_SOFT_COOLDOWN_PREFIX,
		iReadyTurn,
		HIGHWAYMAN_ATTACK_SOFT_COOLDOWN_SUFFIX
	)

	szData += szMarker
	player.setScriptData(szData)

def _isHighwaymanAttackSoftCooldownActive(player):
	if player.isNone():
		return False

	iReadyTurn = _getHighwaymanAttackSoftCooldownReadyTurn(player)
	return iReadyTurn > CyGame().getGameTurn()

def _startHighwaymanAttackSoftCooldown(player, iBaseTurns):
	if player.isNone():
		return

	iCooldownTurns = _scaleTurnsByGameSpeed(iBaseTurns)
	_setHighwaymanAttackSoftCooldownReadyTurn(
		player,
		CyGame().getGameTurn() + iCooldownTurns
	)

def _spawnHighwaymanHostileAdjacent(plot, iUnitClass):
	if plot is None or plot.isNone():
		return None

	if iUnitClass == -1:
		return None

	iBarbarian = gc.getGame().getBarbarianPlayer()
	barbPlayer = gc.getPlayer(iBarbarian)
	if barbPlayer.isNone():
		return None

	iUnitType = gc.getCivilizationInfo(barbPlayer.getCivilizationType()).getCivilizationUnits(iUnitClass)
	if iUnitType == UnitTypes.NO_UNIT:
		return None

	for iDX in range(-1, 2):
		for iDY in range(-1, 2):
			if iDX == 0 and iDY == 0:
				continue

			pLoop = plotXY(plot.getX(), plot.getY(), iDX, iDY)
			if pLoop is None or pLoop.isNone():
				continue
			if pLoop.isWater():
				continue
			if pLoop.isImpassable():
				continue
			if pLoop.isPeak():
				continue
			if pLoop.isCity():
				continue
			if pLoop.isUnit():
				continue

			return barbPlayer.initUnit(
				iUnitType,
				ProfessionTypes.NO_PROFESSION,
				pLoop.getX(),
				pLoop.getY(),
				UnitAITypes.NO_UNITAI,
				DirectionTypes.DIRECTION_SOUTH,
				0
			)

	return None

def canTriggerHighwaymanAttack(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	if _isHighwaymanAttackSoftCooldownActive(player):
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False

	if unit.getUnitClassType() != gc.getInfoTypeForString("UNITCLASS_STAGECOACH"):
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return False

	if plot.isWater():
		return False

	iRouteType = plot.getRouteType()
	if iRouteType not in (
		gc.getInfoTypeForString("ROUTE_ROAD"),
		gc.getInfoTypeForString("ROUTE_COUNTRY_ROAD"),
		gc.getInfoTypeForString("ROUTE_PLASTERED_ROAD")
	):
		return False

	if CyGame().getSorenRandNum(100, "Highwayman Attack Trigger") >= 20:
		return False

	return True

def applyHighwaymanAttack(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	if not player.isPlayable():
		return

	if player.isNative():
		return

	stagecoach = player.getUnit(kTriggeredData.iUnitId)
	if stagecoach.isNone():
		return

	if stagecoach.getUnitClassType() != gc.getInfoTypeForString("UNITCLASS_STAGECOACH"):
		return

	plot = stagecoach.plot()
	if plot is None or plot.isNone():
		return

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return

	if plot.isWater():
		return

	iRouteType = plot.getRouteType()
	if iRouteType not in (
		gc.getInfoTypeForString("ROUTE_ROAD"),
		gc.getInfoTypeForString("ROUTE_COUNTRY_ROAD"),
		gc.getInfoTypeForString("ROUTE_PLASTERED_ROAD")
	):
		return

	iHostileUnitClass = event.getGenericParameter(1)
	iNumHostiles = event.getGenericParameter(2)

	# Freeze stagecoach for current turn and next turn
	stagecoach.setMoves(0)
	stagecoach.setImmobileTimer(1)

	if iNumHostiles < 1:
		iNumHostiles = 1
	if iNumHostiles > 1:
		iNumHostiles = 1

	hostileUnit = _spawnHighwaymanHostileAdjacent(plot, iHostileUnitClass)
	if hostileUnit is not None and not hostileUnit.isNone():
		if hostileUnit.canMoveInto(plot, True, False, False):
			hostileUnit.attack(plot, False)

	_startHighwaymanAttackSoftCooldown(player, 50)

def applyHighwaymanAttackBribe(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	if not player.isPlayable():
		return

	if player.isNative():
		return

	_startHighwaymanAttackSoftCooldown(player, 50)

def getHelpHighwaymanAttack(argsList):
	return localText.getText("TXT_KEY_EVENT_HIGHWAYMAN_ATTACK_HELP", ())

######## Land Transport Attack ###########

LANDTRANSPORT_ATTACK_SOFT_COOLDOWN_PREFIX = "[[WTP_LANDTRANSPORT_ATTACK_SOFT_READY_TURN="
LANDTRANSPORT_ATTACK_SOFT_COOLDOWN_SUFFIX = "]]"

LANDTRANSPORT_ATTACK_BRIBE_GOLD = 500

def _getLandtransportAttackSoftCooldownReadyTurn(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(LANDTRANSPORT_ATTACK_SOFT_COOLDOWN_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(LANDTRANSPORT_ATTACK_SOFT_COOLDOWN_PREFIX)
	iEnd = szData.find(LANDTRANSPORT_ATTACK_SOFT_COOLDOWN_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1

def _setLandtransportAttackSoftCooldownReadyTurn(player, iReadyTurn):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(LANDTRANSPORT_ATTACK_SOFT_COOLDOWN_PREFIX)
	if iStart != -1:
		iEnd = szData.find(LANDTRANSPORT_ATTACK_SOFT_COOLDOWN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(LANDTRANSPORT_ATTACK_SOFT_COOLDOWN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szMarker = "%s%d%s" % (
		LANDTRANSPORT_ATTACK_SOFT_COOLDOWN_PREFIX,
		iReadyTurn,
		LANDTRANSPORT_ATTACK_SOFT_COOLDOWN_SUFFIX
	)

	szData += szMarker
	player.setScriptData(szData)

def _isLandtransportAttackSoftCooldownActive(player):
	if player.isNone():
		return False

	iReadyTurn = _getLandtransportAttackSoftCooldownReadyTurn(player)
	return iReadyTurn > CyGame().getGameTurn()

def _startLandtransportAttackSoftCooldown(player, iBaseTurns):
	if player.isNone():
		return

	iCooldownTurns = _scaleTurnsByGameSpeed(iBaseTurns)
	_setLandtransportAttackSoftCooldownReadyTurn(
		player,
		CyGame().getGameTurn() + iCooldownTurns
	)

def _spawnLandtransportHostileAdjacent(plot, iUnitClass):
	if plot is None or plot.isNone():
		return None

	if iUnitClass == -1:
		return None

	iBarbarian = gc.getGame().getBarbarianPlayer()
	barbPlayer = gc.getPlayer(iBarbarian)
	if barbPlayer.isNone():
		return None

	iUnitType = gc.getCivilizationInfo(barbPlayer.getCivilizationType()).getCivilizationUnits(iUnitClass)
	if iUnitType == UnitTypes.NO_UNIT:
		return None

	for iDX in range(-1, 2):
		for iDY in range(-1, 2):
			if iDX == 0 and iDY == 0:
				continue

			pLoop = plotXY(plot.getX(), plot.getY(), iDX, iDY)
			if pLoop is None or pLoop.isNone():
				continue
			if pLoop.isWater():
				continue
			if pLoop.isImpassable():
				continue
			if pLoop.isPeak():
				continue
			if pLoop.isCity():
				continue
			if pLoop.isUnit():
				continue

			return barbPlayer.initUnit(
				iUnitType,
				ProfessionTypes.NO_PROFESSION,
				pLoop.getX(),
				pLoop.getY(),
				UnitAITypes.NO_UNITAI,
				DirectionTypes.DIRECTION_SOUTH,
				0
			)

	return None

def canTriggerLandtransportAttack(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	bCooldownActive = _isLandtransportAttackSoftCooldownActive(player)

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return False

	if plot.isWater():
		return False

	iRouteType = plot.getRouteType()
	if iRouteType not in (
		gc.getInfoTypeForString("ROUTE_ROAD"),
		gc.getInfoTypeForString("ROUTE_COUNTRY_ROAD"),
		gc.getInfoTypeForString("ROUTE_PLASTERED_ROAD")
	):
		return False

	iChance = 20
	if bCooldownActive:
		iChance = 1

	if CyGame().getSorenRandNum(100, "Landtransport Attack Trigger") >= iChance:
		return False

	return True

def applyLandtransportAttack(argsList):
	kTriggeredData = argsList[0]
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	if not player.isPlayable():
		return

	if player.isNative():
		return

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return

	plot = unit.plot()
	if plot is None or plot.isNone():
		return

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return

	if plot.isWater():
		return

	iRouteType = plot.getRouteType()
	if iRouteType not in (
		gc.getInfoTypeForString("ROUTE_ROAD"),
		gc.getInfoTypeForString("ROUTE_COUNTRY_ROAD"),
		gc.getInfoTypeForString("ROUTE_PLASTERED_ROAD")
	):
		return

	iHostileUnitClass = event.getGenericParameter(1)
	iNumHostiles = event.getGenericParameter(2)

	# Freeze land transport for current turn and next turn
	unit.setMoves(0)
	unit.setImmobileTimer(1)

	if iNumHostiles < 1:
		iNumHostiles = 1
	if iNumHostiles > 1:
		iNumHostiles = 1

	hostileUnit = _spawnLandtransportHostileAdjacent(plot, iHostileUnitClass)
	if hostileUnit is not None and not hostileUnit.isNone():
		if hostileUnit.canMoveInto(plot, True, False, False):
			hostileUnit.attack(plot, False)

	_startLandtransportAttackSoftCooldown(player, 50)

def applyLandtransportAttackBribe(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	if not player.isPlayable():
		return

	if player.isNative():
		return

	_startLandtransportAttackSoftCooldown(player, 50)

def getHelpLandtransportAttack(argsList):
	return localText.getText("TXT_KEY_EVENT_LANDTRANSPORT_ATTACK_HELP", ())

######## Milkmaid in Need ###########

def canTriggerMilkmaidInNeedCity(argsList):
	eTrigger = argsList[0]
	ePlayer = argsList[1]
	iCityId = argsList[2]

	player = gc.getPlayer(ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	city = player.getCity(iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return False

	trigger = gc.getEventTriggerInfo(eTrigger)

	for iDX in range(-2, 3):
		for iDY in range(-2, 3):
			plot = plotXY(city.getX(), city.getY(), iDX, iDY)
			if plot is None or plot.isNone():
				continue

			if plot.getOwner() != player.getID():
				continue

			if plotDistance(city.getX(), city.getY(), plot.getX(), plot.getY()) > 2:
				continue

			iImprovement = plot.getImprovementType()

			for i in range(trigger.getNumImprovementsRequired()):
				if iImprovement == trigger.getImprovementRequired(i):
					return True

	return False


def applyMilkmaidInNeed1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return

	iEnemyClass = event.getGenericParameter(1)
	iAmount = event.getGenericParameter(2)

	if iEnemyClass == -1 or iAmount <= 0:
		return

	iBarbarian = gc.getGame().getBarbarianPlayer()
	barbPlayer = gc.getPlayer(iBarbarian)
	if barbPlayer.isNone():
		return

	iEnemyType = gc.getCivilizationInfo(barbPlayer.getCivilizationType()).getCivilizationUnits(iEnemyClass)
	if iEnemyType == -1:
		return

	for i in range(iAmount):
		enemyUnit = None

		for iDX in range(-1, 2):
			for iDY in range(-1, 2):
				if iDX == 0 and iDY == 0:
					continue

				loopPlot = plotXY(city.getX(), city.getY(), iDX, iDY)
				if loopPlot is None or loopPlot.isNone():
					continue
				if loopPlot.isWater():
					continue
				if loopPlot.isImpassable():
					continue
				if loopPlot.isPeak():
					continue
				if loopPlot.isCity():
					continue
				if loopPlot.isUnit():
					continue

				enemyUnit = barbPlayer.initUnit(
					iEnemyType,
					ProfessionTypes.NO_PROFESSION,
					loopPlot.getX(),
					loopPlot.getY(),
					UnitAITypes.NO_UNITAI,
					DirectionTypes.DIRECTION_SOUTH,
					0
				)
				break

			if enemyUnit is not None:
				break

		if enemyUnit is not None and not enemyUnit.isNone():
			if enemyUnit.canMoveInto(city.plot(), True, False, False):
				enemyUnit.attack(city.plot(), False)

getHelpMilkmaidInNeed = get_simple_help("TXT_KEY_EVENT_MILKMAID_IN_NEED_HELP")

######## Whale Attack ###########

getHelpWhaleAttack = get_simple_help("TXT_KEY_WHALE_ATTACK_HELP")

######## Pig Herder in Need ###########

def canTriggerHerderInNeedCity(argsList):
	eTrigger = argsList[0]
	ePlayer = argsList[1]
	iCityId = argsList[2]

	player = gc.getPlayer(ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	city = player.getCity(iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return False

	trigger = gc.getEventTriggerInfo(eTrigger)

	for iDX in range(-2, 3):
		for iDY in range(-2, 3):
			plot = plotXY(city.getX(), city.getY(), iDX, iDY)
			if plot is None or plot.isNone():
				continue

			if plot.getOwner() != player.getID():
				continue

			if plotDistance(city.getX(), city.getY(), plot.getX(), plot.getY()) > 2:
				continue

			iImprovement = plot.getImprovementType()

			for i in range(trigger.getNumImprovementsRequired()):
				if iImprovement == trigger.getImprovementRequired(i):
					return True

	return False


def _getHerderInNeedImprovementPlot(city, player, trigger):
	if city.isNone() or player.isNone():
		return None

	for iDX in range(-2, 3):
		for iDY in range(-2, 3):
			plot = plotXY(city.getX(), city.getY(), iDX, iDY)
			if plot is None or plot.isNone():
				continue

			if plot.getOwner() != player.getID():
				continue

			if plotDistance(city.getX(), city.getY(), plot.getX(), plot.getY()) > 2:
				continue

			iImprovement = plot.getImprovementType()

			for i in range(trigger.getNumImprovementsRequired()):
				if iImprovement == trigger.getImprovementRequired(i):
					return plot

	return None


def applyHerderInNeed1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return

	trigger = gc.getEventTriggerInfo(kTriggeredData.eTrigger)

	targetPlot = _getHerderInNeedImprovementPlot(city, player, trigger)
	if targetPlot is None or targetPlot.isNone():
		return

	iEnemyClass = event.getGenericParameter(1)
	iEnemyAmount = event.getGenericParameter(2)
	iFriendlyClass = event.getGenericParameter(4)

	defenderUnit = None
	if iFriendlyClass != -1:
		iFriendlyType = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationUnits(iFriendlyClass)

		if iFriendlyType != -1:
			defenderUnit = player.initUnit(
				iFriendlyType,
				ProfessionTypes.NO_PROFESSION,
				city.getX(),
				city.getY(),
				UnitAITypes.NO_UNITAI,
				DirectionTypes.DIRECTION_SOUTH,
				0
			)

	if defenderUnit is None or defenderUnit.isNone():
		return

	if iEnemyClass == -1 or iEnemyAmount <= 0:
		return

	iBarbarian = gc.getGame().getBarbarianPlayer()
	barbPlayer = gc.getPlayer(iBarbarian)
	if barbPlayer.isNone():
		return

	iEnemyType = gc.getCivilizationInfo(barbPlayer.getCivilizationType()).getCivilizationUnits(iEnemyClass)
	if iEnemyType == -1:
		return

	for i in range(iEnemyAmount):
		hostileUnit = None

		for iDX in range(-1, 2):
			for iDY in range(-1, 2):
				if iDX == 0 and iDY == 0:
					continue

				spawnPlot = plotXY(city.getX(), city.getY(), iDX, iDY)
				if spawnPlot is None or spawnPlot.isNone():
					continue
				if spawnPlot.isWater():
					continue
				if spawnPlot.isPeak():
					continue
				if spawnPlot.isImpassable():
					continue
				if spawnPlot.isCity():
					continue
				if spawnPlot.isUnit():
					continue

				hostileUnit = barbPlayer.initUnit(
					iEnemyType,
					ProfessionTypes.NO_PROFESSION,
					spawnPlot.getX(),
					spawnPlot.getY(),
					UnitAITypes.NO_UNITAI,
					DirectionTypes.DIRECTION_SOUTH,
					0
				)
				break

			if hostileUnit is not None:
				break

		if hostileUnit is not None and not hostileUnit.isNone():
			if hostileUnit.canMoveInto(city.plot(), True, False, False):
				hostileUnit.attack(city.plot(), False)


getHelpHerderInNeed = get_simple_help("TXT_KEY_EVENT_HERDER_IN_NEED_HELP")

# adjacent Plot
def spawnBarbarianUnitAdjacentToPlotAndFriendlyOnSamePlot(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	ePlayer = kTriggeredData.ePlayer
	plotThatTriggered = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	# this spawns the barbarian Unit
	iHostileUnitClassTypeToSpawn = event.getGenericParameter(1)
	iNumHostilesToSpawn = event.getGenericParameter(2)
	for iX in range(iNumHostilesToSpawn):
		plotThatTriggered.spawnBarbarianUnitOnAdjacentPlot(iHostileUnitClassTypeToSpawn)
	# this spawns the friendly Unit
	iOwnUnitClassTypeToSpawn = event.getGenericParameter(4)
	iNumOwnToSpawn = event.getGenericParameter(2)
	for iX in range(iNumOwnToSpawn):
		plotThatTriggered.spawnPlayerUnitOnPlot(ePlayer, iOwnUnitClassTypeToSpawn)

######## Pig Herder in Need ###########

######## Slave and Planation Owner Daughter ###########
getHelpSlaveAndPlanationOwnerDaughter1 = get_simple_help("TXT_KEY_EVENT_SLAVE_AND_PLANATION_OWNER_DAUGHTER_1_HELP")

######## Liet Event Training ###########

def checkCityAbovePopulation(numPop):

	def canTrigger(argsList):
		ePlayer = argsList[1]
		iCity = argsList[2]
		player = gc.getPlayer(ePlayer)
		city = player.getCity(iCity)

		if not player.isPlayable():
			return False
		if city.isNone():
			return False
		if city.getPopulation() < numPop:
			return False
		return True

	return canTrigger

canTriggerAtCityPopulationOf10 = checkCityAbovePopulation(10)
canTriggerAtCityPopulationOf20 = checkCityAbovePopulation(20)

###### Revolutionary Events ######

def canTriggerRevolutionaryRoyalTroops(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	iRebelPercent = gc.getTeam(player.getTeam()).getRebelPercent()
	if iRebelPercent <= 20:
		return False

	return True

def canTriggerRevolutionaryDrunkenSpeeches(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	iRebelPercent = gc.getTeam(player.getTeam()).getRebelPercent()
	if iRebelPercent <= 25:
		return False

	return True

def canTriggerRevolutionaryEventsFakeNews(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	iRebelPercent = gc.getTeam(player.getTeam()).getRebelPercent()
	if iRebelPercent <= 35:
		return False

	return True

def canTriggerRevolutionaryLibertyorDeath(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	iRebelPercent = gc.getTeam(player.getTeam()).getRebelPercent()
	if iRebelPercent <= 50:
		return False

	return True

def CheckJudgeInCity(argsList):
	ePlayer = argsList[1]
	player = gc.getPlayer(ePlayer)

	if not player.isPlayable():
		return False

	# you could add checks for several Units like this
	iUnitType = CvUtil.findInfoTypeNum('UNIT_JUDGE')
	iUnitsCurrent = countUnitsInCityForCityTrigger(argsList, iUnitType)
	if iUnitsCurrent == 0:
		return False

	return True

def canTriggerRevolutionaryEventsStart(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	iRebelPercent = gc.getTeam(player.getTeam()).getRebelPercent()
	if iRebelPercent <= 45:
		return False

	return True

def canTriggerRevolutionaryEventsStartDone(argsList):
	ePlayer = argsList[1]
	iCityId = argsList[2]

	player = gc.getPlayer(ePlayer)
	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	city = player.getCity(iCityId)
	if city.isNone():
		return False

	iUnitType = gc.getInfoTypeForString("UNIT_NOBLE")
	if iUnitType == -1:
		return False

	iUnitsCurrent = countUnitsInCityForCityTrigger(argsList, iUnitType)
	if iUnitsCurrent < 1:
		return False

	return True

def CheckNobleInCity(argsList):
	ePlayer = argsList[1]
	player = gc.getPlayer(ePlayer)

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	king = gc.getPlayer(player.getParent())
	if not king.isEurope():
		return False

	if player.isInRevolution():
		return False

	# you could add checks for several Units like this
	iUnitType = CvUtil.findInfoTypeNum('UNIT_NOBLE')
	iUnitsCurrent = countUnitsInCityForCityTrigger(argsList, iUnitType)
	if iUnitsCurrent == 0:
		return False

	return True

def isExpiredRevolutionaryQuest(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	plot = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	if (plot.getOwner() != kTriggeredData.ePlayer):
		return True
	if gc.getGame().getGameTurn() >= kTriggeredData.iTurn + event.getGenericParameter(1):
		return True
	return False

def getHelpRevolutionaryQuest(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	UnitClass = gc.getUnitClassInfo(CvUtil.findInfoTypeNum('UNITCLASS_NOBLE'))
	szHelp = localText.getText("TXT_KEY_EVENT_REVOLUTIONARY_START_EVENT_HELP", (UnitClass.getTextKey(), city.getNameKey(), event.getGenericParameter(1)))
	return szHelp

def applyKingMad(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	player.AI_changeAttitudeExtra(eking, event.getGenericParameter(3))
	king.AI_changeAttitudeExtra(kTriggeredData.ePlayer, event.getGenericParameter(3))
	if event.getGenericParameter(4) == 1 :
		player.NBMOD_DecreaseMaxTaxRate()

def getHelpKingMad(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	szHelp = localText.getText("TXT_KEY_EVENT_KING_MAD_HELP", ())
	if (player.getTaxRate() + event.getGenericParameter(1) <= player.NBMOD_GetMaxTaxRate()) and event.getGenericParameter(1) <>0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_TAX_INCREASE", (event.getGenericParameter(1), player.getTaxRate() + event.getGenericParameter(1)))
	if (player.getTaxRate() + event.getGenericParameter(1) > player.NBMOD_GetMaxTaxRate()) and event.getGenericParameter(1) <>0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_MAXTAX_INCREASE", (GlobalDefines.INCREASE_MAX_TAX_RATE, player.NBMOD_GetMaxTaxRate()+GlobalDefines.INCREASE_MAX_TAX_RATE))
	if event.getGenericParameter(2) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_DECREASE", (event.getGenericParameter(2), king.getCivilizationAdjectiveKey()))
	return szHelp

def getHelpRevolution1(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	szHelp = localText.getText("TXT_KEY_EVENT_REVOLUTION_1_HELP", ())
	if (player.getTaxRate() + event.getGenericParameter(1) <= player.NBMOD_GetMaxTaxRate()) and event.getGenericParameter(1) <>0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_TAX_INCREASE", (event.getGenericParameter(1), player.getTaxRate() + event.getGenericParameter(1)))
	if (player.getTaxRate() + event.getGenericParameter(1) > player.NBMOD_GetMaxTaxRate()) and event.getGenericParameter(1) <>0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_MAXTAX_INCREASE", (GlobalDefines.INCREASE_MAX_TAX_RATE, player.NBMOD_GetMaxTaxRate()+GlobalDefines.INCREASE_MAX_TAX_RATE))
	if event.getGenericParameter(2) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_RELATION_KING_DECREASE", (event.getGenericParameter(2), king.getCivilizationAdjectiveKey()))
	return szHelp

######## Infantry Mutiny Event ###########

getHelpInfantryMutiny = get_simple_help("TXT_KEY_EVENT_INFANTRY_MUTINY_HELP")

REVOLUTIONARY_MUTINY_INFANTRY_COOLDOWN_PREFIX = "[[WTP_REVOLUTIONARY_MUTINY_INFANTRY_READY_TURN="
REVOLUTIONARY_MUTINY_INFANTRY_COOLDOWN_SUFFIX = "]]"


def _getRevolutionaryMutinyInfantryReadyTurn(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(REVOLUTIONARY_MUTINY_INFANTRY_COOLDOWN_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(REVOLUTIONARY_MUTINY_INFANTRY_COOLDOWN_PREFIX)
	iEnd = szData.find(REVOLUTIONARY_MUTINY_INFANTRY_COOLDOWN_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1


def _setRevolutionaryMutinyInfantryReadyTurn(player, iReadyTurn):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(REVOLUTIONARY_MUTINY_INFANTRY_COOLDOWN_PREFIX)
	if iStart != -1:
		iEnd = szData.find(REVOLUTIONARY_MUTINY_INFANTRY_COOLDOWN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(REVOLUTIONARY_MUTINY_INFANTRY_COOLDOWN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szData += "%s%d%s" % (
		REVOLUTIONARY_MUTINY_INFANTRY_COOLDOWN_PREFIX,
		iReadyTurn,
		REVOLUTIONARY_MUTINY_INFANTRY_COOLDOWN_SUFFIX
	)

	player.setScriptData(szData)


def _startRevolutionaryMutinyInfantryCooldown(player):
	if player.isNone():
		return

	_setRevolutionaryMutinyInfantryReadyTurn(
		player,
		CyGame().getGameTurn() + _scaleTurnsByGameSpeed(10)
	)


def canTriggerRevolutionaryMutinyInfantry(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	iReadyTurn = _getRevolutionaryMutinyInfantryReadyTurn(player)
	if iReadyTurn > CyGame().getGameTurn():
		return False

	if CyGame().getSorenRandNum(100, "Revolutionary Mutiny Infantry Chance") >= 10:
		return False

	_startRevolutionaryMutinyInfantryCooldown(player)

	return True

######## The Royals Events ###########

def canTriggerTheRoyals(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	king = gc.getPlayer(player.getParent())
	if king.isNone():
		return False

	if not king.isEurope():
		return False

	iRebelPercent = gc.getTeam(player.getTeam()).getRebelPercent()
	if iRebelPercent <= 40:
		return False

	return True

getHelpTheRoyals1  = get_simple_help("TXT_KEY_EVENT_THE_ROYALS_1PYTHON")
getHelpTheRoyals2  = get_simple_help("TXT_KEY_EVENT_THE_ROYALS_2PYTHON")
getHelpTheRoyals3  = get_simple_help("TXT_KEY_EVENT_THE_ROYALS_3PYTHON")
getHelpTheRoyals4  = get_simple_help("TXT_KEY_EVENT_THE_ROYALS_4PYTHON")
getHelpTheRoyals2a = get_simple_help("TXT_KEY_EVENT_THE_ROYALS_2aPYTHON")

def canTriggerRevolutionaryMovement(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	king = gc.getPlayer(player.getParent())
	if king.isNone():
		return False

	if not king.isEurope():
		return False

	iRebelPercent = gc.getTeam(player.getTeam()).getRebelPercent()
	if iRebelPercent <= 30:
		return False

	return True

def _removeRandomPopulationUnitFromCity(city):
	if city is None or city.isNone():
		return False

	if city.getPopulation() <= 0:
		return False

	iIndex = CyGame().getSorenRandNum(city.getPopulation(), "Remove random settler from city")
	unit = city.getPopulationUnitByIndex(iIndex)

	if unit is None or unit.isNone():
		return False

	city.removePopulationUnit(unit, True, ProfessionTypes.NO_PROFESSION)

	return True


def applyKingPleasedAndRemoveThreeSettlers(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city is None or city.isNone():
		return

	applyKingPleased(argsList)

	for i in range(3):
		if not _removeRandomPopulationUnitFromCity(city):
			break

######## Whaling Trip Quest ###########

def isExpiredWhalingTrip(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if gc.getGame().getGameTurn() >= kTriggeredData.iTurn + event.getGenericParameter(1):
		return True
	if not player.isPlayable():
		return True
	return False

getHelpWhalingTripDone  = get_simple_help("TXT_KEY_EVENT_WHALING_TRIP_HELP")
getHelpWhalingTripDone2  = get_simple_help("TXT_KEY_EVENT_WHALING_TRIP_DONE_PYTHON")

######## Send Dragoons to Frontier Quest ###########

def CheckAfricanSlaveInCity(argsList):
	ePlayer = argsList[1]
	player = gc.getPlayer(ePlayer)

	if not player.isPlayable():
		return False

	# you could add checks for several Units like this
	iUnitType = CvUtil.findInfoTypeNum('UNIT_AFRICAN_SLAVE')
	iUnitsCurrent = countUnitsInCityForCityTrigger(argsList, iUnitType)
	if not iUnitsCurrent > 3:
		return False

	iUnitType = CvUtil.findInfoTypeNum('UNIT_VETERAN_DRAGOON')
	iUnitsCurrent = countUnitsInCityForCityTrigger(argsList, iUnitType)
	if not iUnitsCurrent == 0:
		return False

	iUnitType = CvUtil.findInfoTypeNum('UNIT_VETERAN_CAVALRY')
	iUnitsCurrent = countUnitsInCityForCityTrigger(argsList, iUnitType)
	if not iUnitsCurrent == 0:
		return False

	return True

def isExpiredDragoonstoFrontier(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if gc.getGame().getGameTurn() >= kTriggeredData.iTurn + event.getGenericParameter(1):
		return True
	if not player.isPlayable():
		return True
	return False

getHelpDragoonstoFrontierDone  = get_simple_help("TXT_KEY_EVENT_DRAGOONS_TO_FRONTIER_HELP")

# FOUR TREASURES – POSTPONE / RETURN SYSTEM #

FOUR_TREASURES_RETURN_REQUIRED_GOLD = 2000
FOUR_TREASURES_STATE_PREFIX = "[[WTP_FOUR_TREASURES_STATE="
FOUR_TREASURES_STATE_SUFFIX = "]]"


# -----------------------------------------
# Storage handling
# -----------------------------------------

def _getFourTreasuresState(player):
	if player is None or player.isNone():
		return (False, False, -1)

	szData = player.getScriptData()
	if szData is None or szData == "":
		return (False, False, -1)

	iStart = szData.find(FOUR_TREASURES_STATE_PREFIX)
	if iStart == -1:
		return (False, False, -1)

	iStart += len(FOUR_TREASURES_STATE_PREFIX)
	iEnd = szData.find(FOUR_TREASURES_STATE_SUFFIX, iStart)
	if iEnd == -1:
		return (False, False, -1)

	try:
		szValue = szData[iStart:iEnd]
		aParts = szValue.split("|")
		if len(aParts) != 3:
			return (False, False, -1)

		bPostponed = (int(aParts[0]) == 1)
		bResolved = (int(aParts[1]) == 1)
		iCityId = int(aParts[2])

		return (bPostponed, bResolved, iCityId)
	except:
		return (False, False, -1)


def _setFourTreasuresState(player, bPostponed, bResolved, iCityId):
	if player is None or player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(FOUR_TREASURES_STATE_PREFIX)
	if iStart != -1:
		iEnd = szData.find(FOUR_TREASURES_STATE_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(FOUR_TREASURES_STATE_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	iPostponed = 0
	if bPostponed:
		iPostponed = 1

	iResolved = 0
	if bResolved:
		iResolved = 1

	szMarker = "%s%d|%d|%d%s" % (
		FOUR_TREASURES_STATE_PREFIX,
		iPostponed,
		iResolved,
		iCityId,
		FOUR_TREASURES_STATE_SUFFIX
	)

	szData += szMarker
	player.setScriptData(szData)


def _clearFourTreasuresState(player):
	if player is None or player.isNone():
		return

	_setFourTreasuresState(player, False, True, -1)


def _isFourTreasuresPostponed(player):
	return _getFourTreasuresState(player)[0]


def _isFourTreasuresResolved(player):
	return _getFourTreasuresState(player)[1]


def _getStoredFourTreasuresCityId(player):
	return _getFourTreasuresState(player)[2]


# -----------------------------------------
# City / plot validation
# -----------------------------------------

def _plotHasOceanAccess(plot):
	if plot is None or plot.isNone():
		return False

	for iDirection in range(DirectionTypes.NUM_DIRECTION_TYPES):
		adjPlot = plotDirection(plot.getX(), plot.getY(), DirectionTypes(iDirection))
		if adjPlot and not adjPlot.isNone():
			if adjPlot.isWater() and not adjPlot.isLake():
				return True

	return False


def _isValidFourTreasuresCity(player, city):
	if player is None or player.isNone():
		return False
	if city is None or city.isNone():
		return False
	if city.getOwner() != player.getID():
		return False
	if not city.isCoastal(gc.getMIN_WATER_SIZE_FOR_OCEAN()):
		return False
	if not _plotHasOceanAccess(city.plot()):
		return False

	return True


def _getFourTreasuresCity(player, iCityId):
	if player is None or player.isNone():
		return None
	if iCityId == -1:
		return None

	city = player.getCity(iCityId)
	if city is None or city.isNone():
		return None

	if city.getOwner() != player.getID():
		return None

	return city


# -----------------------------------------
# Treasure checks
# -----------------------------------------

def _countTreasureUnitsOnCityPlot(player, city):
	if player is None or player.isNone():
		return 0
	if city is None or city.isNone():
		return 0

	pPlot = city.plot()
	if pPlot is None or pPlot.isNone():
		return 0

	eTreasureClass = gc.getInfoTypeForString("UNITCLASS_TREASURE")
	iCount = 0

	for i in range(pPlot.getNumUnits()):
		unit = pPlot.getUnit(i)
		if unit is None or unit.isNone():
			continue
		if unit.getOwner() != player.getID():
			continue

		eUnitClass = gc.getUnitInfo(unit.getUnitType()).getUnitClassType()
		if eUnitClass == eTreasureClass:
			iCount += 1

	return iCount


def _hasFourTreasuresOnCityPlot(player, city):
	return _countTreasureUnitsOnCityPlot(player, city) >= 4


def _countTreasureUnitsInAllOwnCities(player):
	if player is None or player.isNone():
		return 0

	iCount = 0
	(city, iter) = player.firstCity(True)
	while city:
		iCount += _countTreasureUnitsOnCityPlot(player, city)
		(city, iter) = player.nextCity(iter, True)

	return iCount


def _hasFourTreasuresInAnyOwnCities(player):
	return _countTreasureUnitsInAllOwnCities(player) >= 4


# -----------------------------------------
# Initial trigger
# -----------------------------------------

def canTriggerFourTreasures(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False
	if not player.isAlive():
		return False
	if not player.isPlayable():
		return False

	city = _getFourTreasuresCity(player, kTriggeredData.iCityId)
	if city is None or city.isNone():
		return False

	if city.getOwner() != kTriggeredData.ePlayer:
		return False

	if not _isValidFourTreasuresCity(player, city):
		return False

	if not _hasFourTreasuresInAnyOwnCities(player):
		return False

	return True


# -----------------------------------------
# Postpone
# -----------------------------------------

def applyFourTreasuresPostpone(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	city = _getFourTreasuresCity(player, kTriggeredData.iCityId)
	if city is None or city.isNone():
		return

	if not _isValidFourTreasuresCity(player, city):
		return

	_setFourTreasuresState(player, True, False, city.getID())
	_changeKingRelation(argsList, -1)


def getHelpFourTreasuresPostpone(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)

	szHelp = localText.getText("TXT_KEY_EVENT_FOUR_TREASURES_DELAY_HELP", ())
	szHelp += u"\n" + localText.getText(
		"TXT_KEY_EVENT_RELATION_KING_DECREASE",
		(-1, king.getCivilizationAdjectiveKey())
	)

	return szHelp


# -----------------------------------------
# Return trigger
# -----------------------------------------

def canTriggerFourTreasuresReturn(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False
	if not player.isAlive():
		return False
	if not player.isPlayable():
		return False

	if _isFourTreasuresResolved(player):
		return False

	if not _isFourTreasuresPostponed(player):
		return False

	iStoredCityId = _getStoredFourTreasuresCityId(player)
	if iStoredCityId == -1:
		_clearFourTreasuresState(player)
		return False

	if kTriggeredData.iCityId != iStoredCityId:
		return False

	city = _getFourTreasuresCity(player, iStoredCityId)
	if city is None or city.isNone():
		_clearFourTreasuresState(player)
		return False

	if city.getOwner() != kTriggeredData.ePlayer:
		_clearFourTreasuresState(player)
		return False

	if not _isValidFourTreasuresCity(player, city):
		_clearFourTreasuresState(player)
		return False

	if not _hasFourTreasuresInAnyOwnCities(player):
		_clearFourTreasuresState(player)
		return False

	if player.getGold() < FOUR_TREASURES_RETURN_REQUIRED_GOLD:
		return False

	return True


def canTriggerFourTreasuresReturnCity(argsList):
	# Robust wrapper because callback signatures may differ depending on call path.

	if len(argsList) >= 3:
		ePlayer = argsList[1]
		iCityId = argsList[2]

		player = gc.getPlayer(ePlayer)
		if player.isNone():
			return False
		if not player.isAlive():
			return False
		if not player.isPlayable():
			return False

		class TriggerData:
			pass

		kTriggeredData = TriggerData()
		kTriggeredData.ePlayer = ePlayer
		kTriggeredData.iCityId = iCityId

		return canTriggerFourTreasuresReturn([kTriggeredData])

	if len(argsList) >= 1:
		kTriggeredData = argsList[0]

		if not hasattr(kTriggeredData, "ePlayer"):
			return False
		if not hasattr(kTriggeredData, "iCityId"):
			return False

		return canTriggerFourTreasuresReturn([kTriggeredData])

	return False


# -----------------------------------------
# Return – buy
# -----------------------------------------

def applyFourTreasuresReturnBuy(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	iStoredCityId = _getStoredFourTreasuresCityId(player)
	city = _getFourTreasuresCity(player, iStoredCityId)
	if city is None or city.isNone():
		_clearFourTreasuresState(player)
		return

	if city.getOwner() != kTriggeredData.ePlayer:
		_clearFourTreasuresState(player)
		return

	if not _isValidFourTreasuresCity(player, city):
		_clearFourTreasuresState(player)
		return

	if not _hasFourTreasuresInAnyOwnCities(player):
		_clearFourTreasuresState(player)
		return

	if player.getGold() < FOUR_TREASURES_RETURN_REQUIRED_GOLD:
		return

	iUnitClassType = gc.getInfoTypeForString("UNITCLASS_GALLEON")
	iUnitType = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationUnits(iUnitClassType)

	if iUnitType == UnitTypes.NO_UNIT:
		return

	player.changeGold(-FOUR_TREASURES_RETURN_REQUIRED_GOLD)

	player.initUnit(
		iUnitType,
		0,
		city.getX(),
		city.getY(),
		UnitAITypes.NO_UNITAI,
		DirectionTypes.DIRECTION_SOUTH,
		0
	)

	_changeKingRelation(argsList, 1)
	_clearFourTreasuresState(player)


def getHelpFourTreasuresReturnBuy(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)

	city = _getFourTreasuresCity(player, kTriggeredData.iCityId)

	if city is not None and not city.isNone():
		szCityName = city.getName()
	else:
		szCityName = u"-"

	szHelp = localText.getText(
		"TXT_KEY_EVENT_FOUR_TREASURES_RETURN_1_HELP",
		(FOUR_TREASURES_RETURN_REQUIRED_GOLD, szCityName)
	)

	szHelp += u"\n" + localText.getText(
		"TXT_KEY_EVENT_RELATION_KING_INCREASE",
		(1, king.getCivilizationAdjectiveKey())
	)

	return szHelp


# -----------------------------------------
# Return – decline
# -----------------------------------------

def applyFourTreasuresReturnDecline(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	_changeKingRelation(argsList, -2)
	_clearFourTreasuresState(player)


def getHelpFourTreasuresReturnDecline(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)

	szHelp = localText.getText(
		"TXT_KEY_EVENT_FOUR_TREASURES_RETURN_2_HELP",
		()
	)

	szHelp += u"\n" + localText.getText(
		"TXT_KEY_EVENT_RELATION_KING_DECREASE",
		(-2, king.getCivilizationAdjectiveKey())
	)

	return szHelp

######## Treasure Protection Event ###########

TREASURE_PROTECTION_SOFT_COOLDOWN_PREFIX = "[[WTP_TREASURE_PROTECTION_READY_TURN="
TREASURE_PROTECTION_SOFT_COOLDOWN_SUFFIX = "]]"

def _getTreasureProtectionSoftCooldownReadyTurn(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(TREASURE_PROTECTION_SOFT_COOLDOWN_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(TREASURE_PROTECTION_SOFT_COOLDOWN_PREFIX)
	iEnd = szData.find(TREASURE_PROTECTION_SOFT_COOLDOWN_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1

def _setTreasureProtectionSoftCooldownReadyTurn(player, iReadyTurn):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(TREASURE_PROTECTION_SOFT_COOLDOWN_PREFIX)
	if iStart != -1:
		iEnd = szData.find(TREASURE_PROTECTION_SOFT_COOLDOWN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(TREASURE_PROTECTION_SOFT_COOLDOWN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szMarker = "%s%d%s" % (
		TREASURE_PROTECTION_SOFT_COOLDOWN_PREFIX,
		iReadyTurn,
		TREASURE_PROTECTION_SOFT_COOLDOWN_SUFFIX
	)

	szData += szMarker
	player.setScriptData(szData)

def _getTreasureProtectionScaledTurns(iBaseTurns):
	gameSpeedType = CyGame().getGameSpeedType()
	iPercent = gc.getGameSpeedInfo(gameSpeedType).getGrowthPercent()
	return max(1, int((iBaseTurns * iPercent) / 100))

def _startTreasureProtectionSoftCooldown(player, iBaseTurns):
	if player.isNone():
		return

	iReadyTurn = CyGame().getGameTurn() + _getTreasureProtectionScaledTurns(iBaseTurns)
	_setTreasureProtectionSoftCooldownReadyTurn(player, iReadyTurn)

def _isTreasureProtectionSoftCooldownActive(player):
	if player.isNone():
		return False

	iReadyTurn = _getTreasureProtectionSoftCooldownReadyTurn(player)
	return iReadyTurn > CyGame().getGameTurn()

def _isTreasureProtectionEscortUnit(loopUnit):
	if loopUnit is None or loopUnit.isNone():
		return False

	iLoopUnitType = loopUnit.getUnitType()
	iLoopProfession = loopUnit.getProfession()

	protectedUnits = (
		gc.getInfoTypeForString("UNIT_EUROPEAN_LINE_INFANTRY"),
		gc.getInfoTypeForString("UNIT_MORTAR"),
		gc.getInfoTypeForString("UNIT_HESSIAN"),
		gc.getInfoTypeForString("UNIT_MILITIA"),
		gc.getInfoTypeForString("UNIT_CONTINENTAL_GUARD"),
		gc.getInfoTypeForString("UNIT_NATIVE_MERC"),
		gc.getInfoTypeForString("UNIT_RANGER"),
		gc.getInfoTypeForString("UNIT_CONQUISTADOR"),
		gc.getInfoTypeForString("UNIT_MOUNTED_CONQUISTADOR"),
		gc.getInfoTypeForString("UNIT_BUCCANNEER"),
	)

	protectedProfessions = (
		gc.getInfoTypeForString("PROFESSION_SCOUT"),
		gc.getInfoTypeForString("PROFESSION_DRAGOON"),
		gc.getInfoTypeForString("PROFESSION_PIONEER"),
		gc.getInfoTypeForString("PROFESSION_MISSIONARY"),
		gc.getInfoTypeForString("PROFESSION_PREACHER"),
		gc.getInfoTypeForString("PROFESSION_NATIVE_TRADER"),
		gc.getInfoTypeForString("PROFESSION_TOWN_GUARD"),
		gc.getInfoTypeForString("PROFESSION_ARMED_BRAVE"),
		gc.getInfoTypeForString("PROFESSION_ARMED_MOUNTED_BRAVE"),
		gc.getInfoTypeForString("PROFESSION_ROYAL_HALBERDIER"),
		gc.getInfoTypeForString("PROFESSION_COLONIAL_MILITIA"),
		gc.getInfoTypeForString("PROFESSION_LINE_INFANTRY"),
		gc.getInfoTypeForString("PROFESSION_ROYAL_LIGHT_INFANTRY"),
		gc.getInfoTypeForString("PROFESSION_ROYAL_LINE_INFANTRY"),
		gc.getInfoTypeForString("PROFESSION_HEAVY_CAVALRY"),
		gc.getInfoTypeForString("PROFESSION_ROYAL_CAVALRY"),
		gc.getInfoTypeForString("PROFESSION_LIGHT_ARTILLERY"),
		gc.getInfoTypeForString("PROFESSION_HEAVY_ARTILLERY"),
		gc.getInfoTypeForString("PROFESSION_ROYAL_ARTILLERY"),
	)

	if iLoopUnitType in protectedUnits:
		return True

	if iLoopProfession in protectedProfessions:
		return True

	return False

def _getTreasureProtectionDistanceToOwnTerritory(plot, player, iMaxRange):
	for iRange in range(1, iMaxRange + 1):
		for iDX in range(-iRange, iRange + 1):
			for iDY in range(-iRange, iRange + 1):
				if abs(iDX) != iRange and abs(iDY) != iRange:
					continue

				pLoop = plotXY(plot.getX(), plot.getY(), iDX, iDY)
				if pLoop is None or pLoop.isNone():
					continue

				if pLoop.getOwner() == player.getID():
					return iRange

	return iMaxRange + 1

def _getTreasureProtectionValidatedPlotAndPlayer(kTriggeredData):
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return (None, None)

	if not player.isPlayable():
		return (None, None)

	if player.isNative():
		return (None, None)

	plotThatTriggered = CyMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	if plotThatTriggered is None or plotThatTriggered.isNone():
		return (None, None)

	if plotThatTriggered.isWater():
		return (None, None)

	iTreasureClass = CvUtil.findInfoTypeNum('UNITCLASS_TREASURE')
	bHasTreasure = False

	for i in range(plotThatTriggered.getNumUnits()):
		loopUnit = plotThatTriggered.getUnit(i)
		if loopUnit.isNone():
			continue
		if loopUnit.getOwner() != player.getID():
			continue

		iLoopClass = gc.getUnitInfo(loopUnit.getUnitType()).getUnitClassType()

		if iLoopClass == iTreasureClass:
			bHasTreasure = True
			continue

		if _isTreasureProtectionEscortUnit(loopUnit):
			return (None, None)

	if not bHasTreasure:
		return (None, None)

	return (player, plotThatTriggered)

def canTriggerTreasureProtection(argsList):
	if not canTriggerIsPlayableWithTriggerChance(argsList):
		return False

	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	if _isTreasureProtectionSoftCooldownActive(player):
		return False

	plotThatTriggered = CyMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	if plotThatTriggered is None or plotThatTriggered.isNone():
		return False

	if plotThatTriggered.isWater():
		return False

	iTreasureClass = CvUtil.findInfoTypeNum('UNITCLASS_TREASURE')
	bHasTreasure = False

	for i in range(plotThatTriggered.getNumUnits()):
		loopUnit = plotThatTriggered.getUnit(i)
		if loopUnit.isNone():
			continue
		if loopUnit.getOwner() != player.getID():
			continue

		iLoopClass = gc.getUnitInfo(loopUnit.getUnitType()).getUnitClassType()

		if iLoopClass == iTreasureClass:
			bHasTreasure = True
			continue

		if _isTreasureProtectionEscortUnit(loopUnit):
			return False

	if not bHasTreasure:
		return False

	iDistanceToOwnTerritory = _getTreasureProtectionDistanceToOwnTerritory(plotThatTriggered, player, 10)

	if iDistanceToOwnTerritory <= 5:
		iChance = 25
	elif iDistanceToOwnTerritory <= 10:
		iChance = 50
	else:
		iChance = 75

	if CyGame().getSorenRandNum(100, "Treasure Protection Trigger") >= iChance:
		return False

	return True

def getHelpTreasureProtectionRules():
	return localText.getText("TXT_KEY_EVENT_TREASURE_PROTECTION_RULES_HELP", ())

def getHelpTreasureProtection1(argsList):
	szHelp = getHelpNewMountedConquistador(argsList)
	if szHelp is None:
		szHelp = u""
	if len(szHelp) > 0:
		szHelp += u"\n"
	szHelp += getHelpTreasureProtectionRules()
	return szHelp

def getHelpTreasureProtection2(argsList):
	szHelp = getHelpNewMilitia(argsList)
	if szHelp is None:
		szHelp = u""
	if len(szHelp) > 0:
		szHelp += u"\n"
	szHelp += getHelpTreasureProtectionRules()
	return szHelp

def getHelpTreasureProtection3(argsList):
	return getHelpKingPleased(argsList)

def applyTreasureProtection1(argsList):
	kTriggeredData = argsList[0]

	player, plotThatTriggered = _getTreasureProtectionValidatedPlotAndPlayer(kTriggeredData)
	if player is None:
		return

	spawnOwnPlayerUnitOnSamePlotAsPlot(argsList)
	_startTreasureProtectionSoftCooldown(player, 30)

def applyTreasureProtection2(argsList):
	kTriggeredData = argsList[0]

	player, plotThatTriggered = _getTreasureProtectionValidatedPlotAndPlayer(kTriggeredData)
	if player is None:
		return

	spawnOwnPlayerUnitOnSamePlotAsPlot(argsList)
	_startTreasureProtectionSoftCooldown(player, 30)

def applyTreasureProtection3(argsList):
	kTriggeredData = argsList[0]

	player, plotThatTriggered = _getTreasureProtectionValidatedPlotAndPlayer(kTriggeredData)
	if player is None:
		return

	_startTreasureProtectionSoftCooldown(player, 30)

getHelpNewMountedConquistador = get_simple_help("TXT_KEY_EVENT_TREASURE_PROTECTION_NEW_MOUNTED_CONQUISTADOR_HELP")
getHelpNewMilitia = get_simple_help("TXT_KEY_EVENT_TREASURE_PROTECTION_NEW_MILITIA_HELP")

######## Happy Hunting ###########

HAPPY_HUNTING_COOLDOWN_PREFIX = "[[HAPPY_HUNTING_READY_TURN="
HAPPY_HUNTING_COOLDOWN_SUFFIX = "]]"

HAPPY_HUNTING_VALID_FEATURES = [
	gc.getInfoTypeForString("FEATURE_FOREST"),
	gc.getInfoTypeForString("FEATURE_FOREST_TUNDRA"),
	gc.getInfoTypeForString("FEATURE_FOREST_EVERGREEN"),
]

def _getHappyHuntingReadyTurn(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if not szData:
		return -1

	iStart = szData.find(HAPPY_HUNTING_COOLDOWN_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(HAPPY_HUNTING_COOLDOWN_PREFIX)
	iEnd = szData.find(HAPPY_HUNTING_COOLDOWN_SUFFIX, iStart)

	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1


def _setHappyHuntingReadyTurn(player, iTurn):
	if player.isNone():
		return

	szData = player.getScriptData()
	if not szData:
		szData = ""

	iStart = szData.find(HAPPY_HUNTING_COOLDOWN_PREFIX)
	if iStart != -1:
		iEnd = szData.find(HAPPY_HUNTING_COOLDOWN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(HAPPY_HUNTING_COOLDOWN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szData += HAPPY_HUNTING_COOLDOWN_PREFIX + str(iTurn) + HAPPY_HUNTING_COOLDOWN_SUFFIX
	player.setScriptData(szData)


def _isHappyHuntingCooldownActive(player):
	iReadyTurn = _getHappyHuntingReadyTurn(player)
	return iReadyTurn > gc.getGame().getGameTurn()


def _startHappyHuntingCooldown(player, iTurns):
	iTurn = gc.getGame().getGameTurn() + iTurns
	_setHappyHuntingReadyTurn(player, iTurn)


def canTriggerHappyHunting(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False
	if not isPlayable(argsList):
		return False

	plot = CyMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	if plot is None:
		return False
	if plot.isWater():
		return False
	if plot.isCity():
		return False
	if plot.getOwner() != kTriggeredData.ePlayer:
		return False
	if plot.getFeatureType() not in HAPPY_HUNTING_VALID_FEATURES:
		return False

	if _isHappyHuntingCooldownActive(player):
		if CyGame().getSorenRandNum(100, "Happy Hunting Cooldown") >= 1:
			return False

	return True


def canTriggerHappyHuntingCity(argsList):
	eTrigger = argsList[0]
	ePlayer = argsList[1]
	iCityId = argsList[2]

	player = gc.getPlayer(ePlayer)
	if player.isNone():
		return False

	city = player.getCity(iCityId)
	if city.isNone():
		return False

	if city.getPopulation() >= 5:
		return False

	# Check whether this city actually has at least one valid owned forest plot
	# in its working radius, so the event can meaningfully belong to it.
	for i in range(gc.getNUM_CITY_PLOTS()):
		pPlot = city.getCityIndexPlot(i)
		if pPlot is None:
			continue
		if pPlot.isNone():
			continue
		if pPlot.getOwner() != ePlayer:
			continue
		if pPlot.isWater():
			continue
		if pPlot.isCity():
			continue
		if pPlot.getFeatureType() not in HAPPY_HUNTING_VALID_FEATURES:
			continue
		return True

	return False


def doTriggerHappyHunting(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	_startHappyHuntingCooldown(player, 50)


######## Failed Trader Daughter ###########

def canTriggerFailedTraderDaughter(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False

	if unit.getUnitClassType() != gc.getInfoTypeForString("UNITCLASS_FAILED_TRADER"):
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	# Must be the exact plot that triggered the event
	if plot.getX() != kTriggeredData.iPlotX:
		return False
	if plot.getY() != kTriggeredData.iPlotY:
		return False

	# Must stand on the city plot
	if plot.getX() != city.getX():
		return False
	if plot.getY() != city.getY():
		return False

	return True

######## Failed Trader Revenge ###########

def canTriggerFailedTraderRevenge(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False

	if unit.getUnitClassType() != gc.getInfoTypeForString("UNITCLASS_FAILED_TRADER"):
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	# Must be the exact plot that triggered the event
	if plot.getX() != kTriggeredData.iPlotX:
		return False
	if plot.getY() != kTriggeredData.iPlotY:
		return False

	# Must stand on the city plot
	if plot.getX() != city.getX():
		return False
	if plot.getY() != city.getY():
		return False

	return True

######## Experienced Sailors ###########

def canTriggerExperiencedSailors(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	if plot.getX() != kTriggeredData.iPlotX:
		return False
	if plot.getY() != kTriggeredData.iPlotY:
		return False

	if plot.getX() != city.getX():
		return False
	if plot.getY() != city.getY():
		return False

	return True

######## Experienced Sailors ###########

def applyExperiencedSailors2(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return

	iNav1 = gc.getInfoTypeForString("PROMOTION_NAVIGATION1")
	iNav2 = gc.getInfoTypeForString("PROMOTION_NAVIGATION2")
	iNav3 = gc.getInfoTypeForString("PROMOTION_NAVIGATION3")

	if iNav1 == -1 or iNav2 == -1 or iNav3 == -1:
		return

	if not unit.isHasPromotion(iNav1):
		unit.setHasRealPromotion(iNav1, True)
	elif not unit.isHasPromotion(iNav2):
		unit.setHasRealPromotion(iNav2, True)
	elif not unit.isHasPromotion(iNav3):
		unit.setHasRealPromotion(iNav3, True)

def getHelpExperiencedSailors2(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return u""

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return u""

	iNav1 = gc.getInfoTypeForString("PROMOTION_NAVIGATION1")
	iNav2 = gc.getInfoTypeForString("PROMOTION_NAVIGATION2")
	iNav3 = gc.getInfoTypeForString("PROMOTION_NAVIGATION3")

	iPromotion = -1

	if iNav1 != -1 and not unit.isHasPromotion(iNav1):
		iPromotion = iNav1
	elif iNav2 != -1 and not unit.isHasPromotion(iNav2):
		iPromotion = iNav2
	elif iNav3 != -1 and not unit.isHasPromotion(iNav3):
		iPromotion = iNav3

	if iPromotion == -1:
		return u""

	return localText.getText(
		"TXT_KEY_EVENT_EXPERIENCED_SAILORS_2_HELP",
		(gc.getPromotionInfo(iPromotion).getTextKey(),)
	)

######## Rotten Meat ###########

def canTriggerRottenMeat(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return False

	if city.getOwner() != player.getID():
		return False

	# The event text says the butcher arrived in the harbour.
	if not city.isCoastal(gc.getMIN_WATER_SIZE_FOR_OCEAN()):
		return False

	return True

######## Event Jade ###########

def canTriggerJadeCity(argsList):
	eTrigger = argsList[0]
	ePlayer = argsList[1]
	iCityId = argsList[2]

	player = gc.getPlayer(ePlayer)
	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	city = player.getCity(iCityId)
	if city.isNone():
		return False

	iMine = gc.getInfoTypeForString("IMPROVEMENT_MINE")
	iDeepMine = gc.getInfoTypeForString("IMPROVEMENT_DEEP_MINE")

	for iDX in range(-2, 3):
		for iDY in range(-2, 3):
			plot = plotXY(city.getX(), city.getY(), iDX, iDY)
			if plot is None or plot.isNone():
				continue

			if plot.getOwner() != player.getID():
				continue

			if plotDistance(city.getX(), city.getY(), plot.getX(), plot.getY()) > 2:
				continue

			iImprovement = plot.getImprovementType()
			if iImprovement == iMine or iImprovement == iDeepMine:
				return True

	return False

######## Lost Ship ###########

LOST_SHIP_1_SOFT_COOLDOWN_PREFIX = "[[WTP_LOST_SHIP_1_SOFT_READY_TURN="
LOST_SHIP_1_SOFT_COOLDOWN_SUFFIX = "]]"


def _getLostShip1SoftCooldownReadyTurn(player):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None or szData == "":
		return -1

	iStart = szData.find(LOST_SHIP_1_SOFT_COOLDOWN_PREFIX)
	if iStart == -1:
		return -1

	iStart += len(LOST_SHIP_1_SOFT_COOLDOWN_PREFIX)
	iEnd = szData.find(LOST_SHIP_1_SOFT_COOLDOWN_SUFFIX, iStart)
	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1


def _setLostShip1SoftCooldownReadyTurn(player, iReadyTurn):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(LOST_SHIP_1_SOFT_COOLDOWN_PREFIX)
	if iStart != -1:
		iEnd = szData.find(LOST_SHIP_1_SOFT_COOLDOWN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(LOST_SHIP_1_SOFT_COOLDOWN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szData += "%s%d%s" % (
		LOST_SHIP_1_SOFT_COOLDOWN_PREFIX,
		iReadyTurn,
		LOST_SHIP_1_SOFT_COOLDOWN_SUFFIX
	)

	player.setScriptData(szData)


def _startLostShip1SoftCooldown(player):
	if player.isNone():
		return

	iReadyTurn = CyGame().getGameTurn() + _scaleTurnsByGameSpeed(30)
	_setLostShip1SoftCooldownReadyTurn(player, iReadyTurn)


def _isLostShip1SoftCooldownActive(player):
	if player.isNone():
		return False

	return _getLostShip1SoftCooldownReadyTurn(player) > CyGame().getGameTurn()


def canTriggerLostShip1(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	if _isLostShip1SoftCooldownActive(player):
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	if plot.getX() != kTriggeredData.iPlotX:
		return False
	if plot.getY() != kTriggeredData.iPlotY:
		return False

	if plot.getTerrainType() != gc.getInfoTypeForString("TERRAIN_COAST"):
		return False

	if plot.isCity():
		return False

	return True


def applyLostShip1(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return

	_startLostShip1SoftCooldown(player)


######## Man from the Wilderness ###########

def canTriggerManFromTheWilderness(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	# hard bind to trigger plot
	if not unit.at(kTriggeredData.iPlotX, kTriggeredData.iPlotY):
		return False

	iImprovement = plot.getImprovementType()
	if iImprovement not in (
		gc.getInfoTypeForString("IMPROVEMENT_FORT"),
		gc.getInfoTypeForString("IMPROVEMENT_LARGE_FORT"),
	):
		return False

	return True

def applyManFromTheWildernessUnit(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	iUnitClass = event.getGenericParameter(1)
	if iUnitClass == -1:
		return

	iProfession = event.getGenericParameter(2)
	if iProfession == -1:
		iProfession = ProfessionTypes.NO_PROFESSION

	iUnitType = gc.getCivilizationInfo(
		player.getCivilizationType()
	).getCivilizationUnits(iUnitClass)

	if iUnitType == -1:
		return

	player.initUnit(
		iUnitType,
		iProfession,
		kTriggeredData.iPlotX,
		kTriggeredData.iPlotY,
		UnitAITypes.NO_UNITAI,
		DirectionTypes.DIRECTION_SOUTH,
		0
	)

def getHelpManFromTheWildernessUnit(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)

	iUnitClass = event.getGenericParameter(1)
	if iUnitClass == -1:
		return u""

	UnitClassInfo = gc.getUnitClassInfo(iUnitClass)

	return localText.getText(
		"TXT_KEY_EVENT_MAN_FROM_THE_WILDERNESS_HELP",
		(UnitClassInfo.getTextKey(),)
	)

######## Witch Trial ###########

def canApplyWitchTrial1(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return False

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return False

	if city.getOwner() != player.getID():
		return False

	return True

######## PLANTS ###########

def canTriggerPlants(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	city = player.getCity(kTriggeredData.iCityId)
	if city is None or city.isNone() or city.getOwner() != player.getID():
		return False

	plot = CyMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	if plot is None or plot.isNone():
		return False

	if plot.getOwner() != player.getID():
		return False

	if plot.getBonusType() != -1:
		return False

	# Plot must be inside this exact city's radius
	if plotDistance(city.getX(), city.getY(), plot.getX(), plot.getY()) > 2:
		return False

	return True


def _cacaoBonusPlotBordersNativePlayer(plot, iNativePlayer):
	if plot is None or plot.isNone():
		return False

	for iDX in range(-2, 3):
		for iDY in range(-2, 3):
			loopPlot = plotXY(plot.getX(), plot.getY(), iDX, iDY)

			if loopPlot is None or loopPlot.isNone():
				continue

			if loopPlot.getOwner() == iNativePlayer:
				return True

	return False

def canTriggerCacaoBonus(argsList):
	kTriggeredData = argsList[0]

	if not canTriggerPlants(argsList):
		return False

	nativePlayer = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if nativePlayer.isNone() or not nativePlayer.isNative():
		return False

	plot = CyMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	if plot is None or plot.isNone():
		return False

	if not _cacaoBonusPlotBordersNativePlayer(plot, nativePlayer.getID()):
		return False

	return True


######## Washed Out ###########

WASHED_OUT_BASE_COOLDOWN_TURNS = 50

def _washedOutScaledCooldown():
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	return max(1, WASHED_OUT_BASE_COOLDOWN_TURNS * Speed.getGrowthPercent() / 100)


def canTriggerWashedOut(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	if _isWashedOutCooldownActive(player):
		return False

	plot = CyMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	if plot is None or plot.isNone():
		return False

	if plot.getOwner() != player.getID():
		return False

	if plot.getRouteType() not in (
		gc.getInfoTypeForString("ROUTE_ROAD"),
		gc.getInfoTypeForString("ROUTE_COUNTRY_ROAD"),
		gc.getInfoTypeForString("ROUTE_PLASTERED_ROAD"),
	):
		return False

	return True


def applyWashedOutCooldown(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	_startWashedOutCooldown(player)


def _isWashedOutCooldownActive(player):
	return CyGame().getGameTurn() < _getWashedOutReadyTurn(player)


def _getWashedOutReadyTurn(player):
	szData = player.getScriptData()
	szKey = "WASHED_OUT_READY_TURN="

	for part in szData.split(";"):
		if part.startswith(szKey):
			return int(part[len(szKey):])

	return 0


def _startWashedOutCooldown(player):
	iReadyTurn = CyGame().getGameTurn() + _washedOutScaledCooldown()

	szKey = "WASHED_OUT_READY_TURN="
	szData = player.getScriptData()
	parts = []

	for part in szData.split(";"):
		if part and not part.startswith(szKey):
			parts.append(part)

	parts.append(szKey + str(iReadyTurn))
	player.setScriptData(";".join(parts))

######## At the sword ###########

AT_THE_SWORD_BASE_COOLDOWN_TURNS = 30

def _atTheSwordScaledCooldown():
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	return max(1, AT_THE_SWORD_BASE_COOLDOWN_TURNS * Speed.getGrowthPercent() / 100)


def canTriggerAtTheSword(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	if _isAtTheSwordCooldownActive(player):
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	if plot.getX() != kTriggeredData.iPlotX:
		return False

	if plot.getY() != kTriggeredData.iPlotY:
		return False

	if plot.getOwner() != player.getID():
		return False

	return True


def applyAtTheSwordCooldown(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	_startAtTheSwordCooldown(player)


def _isAtTheSwordCooldownActive(player):
	return CyGame().getGameTurn() < _getAtTheSwordReadyTurn(player)


def _getAtTheSwordReadyTurn(player):
	szData = player.getScriptData()
	szKey = "AT_THE_SWORD_READY_TURN="

	for part in szData.split(";"):
		if part.startswith(szKey):
			return int(part[len(szKey):])

	return 0


def _startAtTheSwordCooldown(player):
	iReadyTurn = CyGame().getGameTurn() + _atTheSwordScaledCooldown()

	szKey = "AT_THE_SWORD_READY_TURN="
	szData = player.getScriptData()
	parts = []

	for part in szData.split(";"):
		if part and not part.startswith(szKey):
			parts.append(part)

	parts.append(szKey + str(iReadyTurn))
	player.setScriptData(";".join(parts))

######## Bisons ###########

def canTriggerBisons(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	plot = CyMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	if plot is None or plot.isNone():
		return False

	if plot.getOwner() != player.getID():
		return False

	if plot.getBonusType() != gc.getInfoTypeForString("BONUS_BISON"):
		return False

	if plot.getImprovementType() not in (
		gc.getInfoTypeForString("IMPROVEMENT_TRAPPER_HUT"),
		gc.getInfoTypeForString("IMPROVEMENT_TRAPPER_CAMP"),
	):
		return False

	city = plot.getWorkingCity()
	if city is None or city.isNone():
		return False

	if city.getOwner() != player.getID():
		return False

	otherPlayer = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if otherPlayer.isNone():
		return False

	if not otherPlayer.isNative():
		return False

	return True

######## Bishop Appointment ###########

def _bishopAppointmentCityHasChurch(player, city):
	if player.isNone() or city.isNone():
		return False

	for szBuildingClass in (
		"BUILDINGCLASS_CHURCH",
		"BUILDINGCLASS_CATHEDRAL",
		"BUILDINGCLASS_GREAT_CATHEDRAL",
		"BUILDINGCLASS_MINSTER",
	):
		iBuildingClass = gc.getInfoTypeForString(szBuildingClass)
		if iBuildingClass == -1:
			continue

		iBuilding = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationBuildings(iBuildingClass)
		if iBuilding != -1 and city.isHasBuilding(iBuilding):
			return True

	return False


def _bishopAppointmentCountPreachersInCity(city):
	if city.isNone():
		return 0

	iPreacherClass = gc.getInfoTypeForString("UNITCLASS_PREACHER")
	iCount = 0

	for i in range(city.getPopulation()):
		unit = city.getPopulationUnitByIndex(i)
		if unit is None or unit.isNone():
			continue

		if unit.getUnitClassType() == iPreacherClass:
			iCount += 1

	return iCount


def _bishopAppointmentGetPreacherInCity(city):
	if city.isNone():
		return None

	iPreacherClass = gc.getInfoTypeForString("UNITCLASS_PREACHER")

	for i in range(city.getPopulation()):
		unit = city.getPopulationUnitByIndex(i)
		if unit is None or unit.isNone():
			continue

		if unit.getUnitClassType() == iPreacherClass:
			return unit

	return None


def canTriggerBishopAppointmentCity(argsList):
	eTrigger = argsList[0]
	ePlayer = argsList[1]
	iCityId = argsList[2]

	player = gc.getPlayer(ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	city = player.getCity(iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return False

	if not _bishopAppointmentCityHasChurch(player, city):
		return False

	if _bishopAppointmentCountPreachersInCity(city) < 2:
		return False

	return True


def canApplyBishopAppointment1(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return False

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return False

	if player.getGold() < 500:
		return False

	if not _bishopAppointmentCityHasChurch(player, city):
		return False

	if _bishopAppointmentCountPreachersInCity(city) < 2:
		return False

	return True


def applyBishopAppointment1(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return

	if not _bishopAppointmentCityHasChurch(player, city):
		return

	if _bishopAppointmentCountPreachersInCity(city) < 2:
		return

	preacher = _bishopAppointmentGetPreacherInCity(city)
	if preacher is None or preacher.isNone():
		return

	iBishopClass = gc.getInfoTypeForString("UNITCLASS_BISHOP")
	iBishop = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationUnits(iBishopClass)
	if iBishop == -1:
		return

	city.removePopulationUnit(preacher, True, ProfessionTypes.NO_PROFESSION)

	bishop = player.initUnit(
		iBishop,
		ProfessionTypes.NO_PROFESSION,
		city.getX(),
		city.getY(),
		UnitAITypes.NO_UNITAI,
		DirectionTypes.DIRECTION_SOUTH,
		0
	)

	if bishop is None or bishop.isNone():
		return

	city.addPopulationUnit(bishop, ProfessionTypes.NO_PROFESSION)

def getHelpBishopAppointment1(argsList):
	return localText.getText(
		"TXT_KEY_EVENT_BISHOP_APPOINTMENT_HELP_1",
		()
	)
    
######## Book Mormon ###########

def canTriggerBookMormon(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return False

	plot = CyMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	if plot is None or plot.isNone():
		return False

	if plot.getOwner() != player.getID():
		return False

	workingCity = plot.getWorkingCity()
	if workingCity is None or workingCity.isNone():
		return False

	if workingCity.getOwner() != city.getOwner():
		return False

	if workingCity.getID() != city.getID():
		return False

	return True

def applyBookMormon3(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return

	iBuildingClass = gc.getInfoTypeForString("BUILDINGCLASS_CATHEDRAL")
	if iBuildingClass == -1:
		return

	iBuilding = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationBuildings(iBuildingClass)
	if iBuilding == -1:
		return

	if city.isHasBuilding(iBuilding):
		return

	city.setHasRealBuilding(iBuilding, True)
    
def getHelpBookMormon3(argsList):
	player = gc.getPlayer(argsList[0].ePlayer)

	iBuildingClass = gc.getInfoTypeForString("BUILDINGCLASS_CATHEDRAL")
	if iBuildingClass == -1:
		return u""

	iBuilding = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationBuildings(iBuildingClass)
	if iBuilding == -1:
		return u""

	return localText.getText(
		"TXT_KEY_EVENT_BOOK_MORMON_HELP",
		(gc.getBuildingInfo(iBuilding).getTextKey(),)
	)

######## Tornado ###########

TORNADO_COOLDOWN_TURNS = 20

def _getTornadoCooldown(player):

	szKey = "[[TORNADO_READY_TURN="

	scriptData = player.getScriptData()

	iPos = scriptData.find(szKey)
	if iPos == -1:
		return 0

	iStart = iPos + len(szKey)
	iEnd = scriptData.find("]]", iStart)

	if iEnd == -1:
		return 0

	return int(scriptData[iStart:iEnd])


def _setTornadoCooldown(player, iTurn):

	scriptData = player.getScriptData()

	szKey = "[[TORNADO_READY_TURN="

	iPos = scriptData.find(szKey)

	if iPos != -1:
		iEnd = scriptData.find("]]", iPos)
		if iEnd != -1:
			scriptData = scriptData[:iPos] + scriptData[iEnd + 2:]

	scriptData += "[[TORNADO_READY_TURN=%d]]" % iTurn

	player.setScriptData(scriptData)


def canTriggerTornado(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	iCurrentTurn = CyGame().getGameTurn()

	if iCurrentTurn < _getTornadoCooldown(player):
		return False

	return True


def applyTornado1(argsList):
	iEvent = argsList[1]
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)

	plot = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)

	if plot is not None and not plot.isNone():
		plot.setFeatureType(gc.getInfoTypeForString('FEATURE_TORNADO'), 0)

	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	iCooldown = TORNADO_COOLDOWN_TURNS * Speed.getGrowthPercent() / 100

	_setTornadoCooldown(player, CyGame().getGameTurn() + iCooldown)


def applyTornado2(argsList):
	iEvent = argsList[1]
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)

	plot = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)

	if plot is not None and not plot.isNone():
		plot.setFeatureType(gc.getInfoTypeForString('FEATURE_TORNADO'), 0)

	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	iCooldown = TORNADO_COOLDOWN_TURNS * Speed.getGrowthPercent() / 100

	_setTornadoCooldown(player, CyGame().getGameTurn() + iCooldown)
    
######## Build Monastery Quest ###########

def _countPlayerUnitsByUnitClassOnMap(player, iUnitClass):
	if player.isNone():
		return 0

	iCount = 0

	(unit, iter) = player.firstUnit()
	while unit:
		if not unit.isNone():
			if unit.getUnitClassType() == iUnitClass:
				iCount += 1

		(unit, iter) = player.nextUnit(iter)

	return iCount


def canTriggerBuildMonasteryStart(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	plot = CyMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)

	if plot is None or plot.isNone():
		return False

	# Replacement for isNoCity
	if plot.isCity():
		return False

	# Must be owned by player
	if plot.getOwner() != player.getID():
		return False

	# Plot must fully belong to player's team
	if plot.getTeam() != player.getTeam():
		return False

	# No foreign cities adjacent
	for iDX in range(-1, 2):
		for iDY in range(-1, 2):

			if iDX == 0 and iDY == 0:
				continue

			loopPlot = plotXY(plot.getX(), plot.getY(), iDX, iDY)

			if loopPlot is None or loopPlot.isNone():
				continue

			if loopPlot.isCity():

				city = loopPlot.getPlotCity()

				if city is not None and not city.isNone():

					if city.getOwner() != player.getID():
						return False

	# Read required unit data from first event
	eEvent = gc.getEventTriggerInfo(
		kTriggeredData.eTrigger
	).getEvent(0)

	event = gc.getEventInfo(eEvent)

	iRequiredUnitClass = event.getGenericParameter(2)
	iRequiredAmount = event.getGenericParameter(3)

	if iRequiredUnitClass == -1 or iRequiredAmount <= 0:
		return False

	if _countPlayerUnitsByUnitClassOnMap(
		player,
		iRequiredUnitClass
	) < iRequiredAmount:
		return False

	return True

def isExpiredBuildMonastery(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if gc.getGame().getGameTurn() >= kTriggeredData.iTurn + event.getGenericParameter(1):
		return True
	if not player.isPlayable():
		return True
	return False

getHelpBuildMonasteryDone  = get_simple_help("TXT_KEY_EVENT_BUILD_MONASTERY_HELP")

######## Monastery Hiding Slaves ###########

def _monasteryHidingSlavesPlotBordersOtherPlayer(plot, iOtherPlayer):
	if plot is None or plot.isNone():
		return False

	for iDX in range(-2, 3):
		for iDY in range(-2, 3):
			loopPlot = plotXY(plot.getX(), plot.getY(), iDX, iDY)

			if loopPlot is None or loopPlot.isNone():
				continue

			if loopPlot.getOwner() == iOtherPlayer:
				return True

	return False

def canTriggerMonasteryHidingSlaves(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	otherPlayer = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if otherPlayer.isNone() or not otherPlayer.isPlayable() or otherPlayer.isNative():
		return False

	if gc.getTeam(player.getTeam()).isAtWar(otherPlayer.getTeam()):
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	if plot.getX() != kTriggeredData.iPlotX:
		return False

	if plot.getY() != kTriggeredData.iPlotY:
		return False

	if plot.getImprovementType() not in (
		gc.getInfoTypeForString("IMPROVEMENT_MONASTERY"),
		gc.getInfoTypeForString("IMPROVEMENT_LARGE_MONASTERY"),
	):
		return False

	if not _monasteryHidingSlavesPlotBordersOtherPlayer(plot, otherPlayer.getID()):
		return False

	return True

######## Monastery Trading Food ###########

def _monasteryTradingFoodPlotBordersNativePlayer(plot, iNativePlayer):
	if plot is None or plot.isNone():
		return False

	for iDX in range(-2, 3):
		for iDY in range(-2, 3):
			loopPlot = plotXY(plot.getX(), plot.getY(), iDX, iDY)

			if loopPlot is None or loopPlot.isNone():
				continue

			if loopPlot.getOwner() == iNativePlayer:
				return True

	return False

def canTriggerMonasteryTradingFood(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	nativePlayer = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if nativePlayer.isNone() or not nativePlayer.isNative():
		return False

	if gc.getTeam(player.getTeam()).isAtWar(nativePlayer.getTeam()):
		return False

	plot = CyMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	if plot is None or plot.isNone():
		return False

	if plot.getImprovementType() not in (
		gc.getInfoTypeForString("IMPROVEMENT_MONASTERY"),
		gc.getInfoTypeForString("IMPROVEMENT_LARGE_MONASTERY"),
	):
		return False

	if not _monasteryTradingFoodPlotBordersNativePlayer(plot, nativePlayer.getID()):
		return False

	return True

######## Eccentric Architect ###########

ECCENTRIC_ARCHITECT_COOLDOWN_TURNS = 40
ECCENTRIC_ARCHITECT_COOLDOWN_PREFIX = "[[WTP_ECCENTRIC_ARCHITECT_READY_TURN="
ECCENTRIC_ARCHITECT_COOLDOWN_SUFFIX = "]]"


def _getEccentricArchitectCooldown(player):
	if player.isNone():
		return 0

	szData = player.getScriptData()
	if szData is None:
		return 0

	iStart = szData.find(ECCENTRIC_ARCHITECT_COOLDOWN_PREFIX)
	if iStart == -1:
		return 0

	iStart += len(ECCENTRIC_ARCHITECT_COOLDOWN_PREFIX)

	iEnd = szData.find(
		ECCENTRIC_ARCHITECT_COOLDOWN_SUFFIX,
		iStart
	)

	if iEnd == -1:
		return 0

	try:
		return int(szData[iStart:iEnd])
	except:
		return 0


def _setEccentricArchitectCooldown(player, iTurn):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(ECCENTRIC_ARCHITECT_COOLDOWN_PREFIX)

	if iStart != -1:
		iEnd = szData.find(
			ECCENTRIC_ARCHITECT_COOLDOWN_SUFFIX,
			iStart
		)

		if iEnd != -1:
			iEnd += len(ECCENTRIC_ARCHITECT_COOLDOWN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szData += "%s%d%s" % (
		ECCENTRIC_ARCHITECT_COOLDOWN_PREFIX,
		iTurn,
		ECCENTRIC_ARCHITECT_COOLDOWN_SUFFIX
	)

	player.setScriptData(szData)


def canTriggerEccentricArchitect(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	# Cooldown active
	if CyGame().getGameTurn() < _getEccentricArchitectCooldown(player):
		return False

	# Start cooldown immediately after successful trigger
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())

	iCooldown = max(
		1,
		ECCENTRIC_ARCHITECT_COOLDOWN_TURNS * Speed.getGrowthPercent() / 100
	)

	_setEccentricArchitectCooldown(
		player,
		CyGame().getGameTurn() + iCooldown
	)

	return True

######## Eccentric Architect BAILIFF ###########
 
def removeFirstUnitClassFromCity(player, city, iUnitClass):
	if player.isNone() or city.isNone():
		return False

	# 1) Remove unit if it is working as city population / specialist
	for i in range(city.getPopulation()):
		unit = city.getPopulationUnitByIndex(i)

		if unit is None or unit.isNone():
			continue

		if unit.getOwner() != player.getID():
			continue

		if unit.getUnitClassType() == iUnitClass:
			city.removePopulationUnit(unit, True, ProfessionTypes.NO_PROFESSION)
			return True

	# 2) Fallback: remove unit if it stands on the city plot
	plot = city.plot()
	if plot is None or plot.isNone():
		return False

	for i in range(plot.getNumUnits()):
		unit = plot.getUnit(i)

		if unit is None or unit.isNone():
			continue

		if unit.getOwner() != player.getID():
			continue

		if unit.getUnitClassType() == iUnitClass:
			unit.kill(False)
			return True

	return False

def applyRemoveEccentricArchitectFromCity(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return

	iUnitClass = gc.getInfoTypeForString("UNITCLASS_ECCENTRIC_ARCHITECT")
	if iUnitClass == -1:
		return

	removeFirstUnitClassFromCity(player, city, iUnitClass)

def getHelpArchitectBailiffCommit(argsList):
	iUnitClass = gc.getInfoTypeForString("UNITCLASS_ECCENTRIC_ARCHITECT")
	if iUnitClass == -1:
		return u""

	UnitClass = gc.getUnitClassInfo(iUnitClass)

	return localText.getText(
		"TXT_KEY_EVENT_ARCHITECT_BAILIFF_COMMIT_HELP",
		(UnitClass.getTextKey(),)
	)

getHelpBailiffsAttackCity = get_simple_help("TXT_KEY_EVENT_ARCHITECT_BAILIFF_HELP")

def applyArchitectBailiffRefuseAttack(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return

	cityPlot = city.plot()
	if cityPlot is None or cityPlot.isNone():
		return

	iHostileUnitClass = event.getGenericParameter(1)
	if iHostileUnitClass == -1:
		return

	iBarbarian = gc.getGame().getBarbarianPlayer()
	barbPlayer = gc.getPlayer(iBarbarian)
	if barbPlayer.isNone():
		return

	iUnitType = gc.getCivilizationInfo(barbPlayer.getCivilizationType()).getCivilizationUnits(iHostileUnitClass)
	if iUnitType == UnitTypes.NO_UNIT:
		return

	for iDX in range(-1, 2):
		for iDY in range(-1, 2):
			if iDX == 0 and iDY == 0:
				continue

			spawnPlot = plotXY(cityPlot.getX(), cityPlot.getY(), iDX, iDY)
			if spawnPlot is None or spawnPlot.isNone():
				continue
			if spawnPlot.isWater():
				continue
			if spawnPlot.isImpassable():
				continue
			if spawnPlot.isPeak():
				continue
			if spawnPlot.isCity():
				continue
			if spawnPlot.isUnit():
				continue

			hostileUnit = barbPlayer.initUnit(
				iUnitType,
				ProfessionTypes.NO_PROFESSION,
				spawnPlot.getX(),
				spawnPlot.getY(),
				UnitAITypes.NO_UNITAI,
				DirectionTypes.DIRECTION_SOUTH,
				0
			)

			if hostileUnit is not None and not hostileUnit.isNone():
				if hostileUnit.canMoveInto(cityPlot, True, False, False):
					hostileUnit.attack(cityPlot, False)

			return

######## Sick Immigrants ###########

def applyCityHealthPenalty(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return

	city.changeCityHealth(-2)

def getHelpCityHealthPenalty(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return u""

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return u""

	return localText.getText(
		"TXT_KEY_EVENT_CITY_HEALTH_PENALTY",
		(2, city.getNameKey())
	)

######## Spoiled Grain ###########

def applyCityFoodPercentLoss(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return

	iFood = gc.getInfoTypeForString("YIELD_FOOD")
	if iFood == -1:
		return

	iStored = city.getYieldStored(iFood)
	if iStored <= 0:
		return

	iPercent = event.getGenericParameter(1)

	iLoss = (iStored * iPercent) / 100

	if iLoss <= 0:
		return

	city.changeYieldStored(iFood, -iLoss)
    
def getHelpCityFoodPercentLoss(argsList):
	eEvent = argsList[1]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return u""

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return u""

	iFood = gc.getInfoTypeForString("YIELD_FOOD")
	iStored = city.getYieldStored(iFood)

	iPercent = event.getGenericParameter(1)
	iLoss = (iStored * iPercent) / 100

	return localText.getText(
		"TXT_KEY_EVENT_CITY_FOOD_PERCENT_LOST",
		(iLoss, iPercent, city.getNameKey())
	)

######## Farm Bandits ###########

def canTriggerFarmBandits(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	# 1. Plot must have one of the defined food bonuses
	if not hasFoodBonus(argsList):
		return False

	# 2. The linked city must have more than 60 food stored
	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return False

	iFood = gc.getInfoTypeForString("YIELD_FOOD")
	if city.getYieldStored(iFood) <= 60:
		return False

	return True

######## Officers Wife ###########

getHelpOfficerDuel = get_simple_help("TXT_KEY_EVENT_OFFICER_DUEL_HELP")

getHelpOfficerNoDuel = get_simple_help("TXT_KEY_EVENT_OFFICER_NODUEL_HELP")

OFFICERS_WIFE_CITY_PREFIX = "[[WTP_OFFICERS_WIFE_CITY_ID="
OFFICERS_WIFE_CITY_SUFFIX = "]]"

OFFICERS_WIFE_TURN_PREFIX = "[[WTP_OFFICERS_WIFE_TURN="
OFFICERS_WIFE_TURN_SUFFIX = "]]"


def _setOfficersWifeData(player, iCityId):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	for szPrefix, szSuffix in (
		(OFFICERS_WIFE_CITY_PREFIX, OFFICERS_WIFE_CITY_SUFFIX),
		(OFFICERS_WIFE_TURN_PREFIX, OFFICERS_WIFE_TURN_SUFFIX),
	):
		iStart = szData.find(szPrefix)
		if iStart != -1:
			iEnd = szData.find(szSuffix, iStart)
			if iEnd != -1:
				iEnd += len(szSuffix)
				szData = szData[:iStart] + szData[iEnd:]

	szData += "%s%d%s" % (OFFICERS_WIFE_CITY_PREFIX, iCityId, OFFICERS_WIFE_CITY_SUFFIX)
	szData += "%s%d%s" % (OFFICERS_WIFE_TURN_PREFIX, CyGame().getGameTurn(), OFFICERS_WIFE_TURN_SUFFIX)

	player.setScriptData(szData)


def _getOfficersWifeNumber(player, szPrefix, szSuffix):
	if player.isNone():
		return -1

	szData = player.getScriptData()
	if szData is None:
		return -1

	iStart = szData.find(szPrefix)
	if iStart == -1:
		return -1

	iStart += len(szPrefix)
	iEnd = szData.find(szSuffix, iStart)

	if iEnd == -1:
		return -1

	try:
		return int(szData[iStart:iEnd])
	except:
		return -1


def _getOfficersWifeCity(player):
	return _getOfficersWifeNumber(player, OFFICERS_WIFE_CITY_PREFIX, OFFICERS_WIFE_CITY_SUFFIX)


def _getOfficersWifeTurn(player):
	return _getOfficersWifeNumber(player, OFFICERS_WIFE_TURN_PREFIX, OFFICERS_WIFE_TURN_SUFFIX)


def applyOfficersWife1(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return

	_setOfficersWifeData(player, city.getID())


def canDoCityOfficersWifeAfterDuel(argsList):
	eTrigger = argsList[0]
	ePlayer = argsList[1]
	iCityId = argsList[2]

	player = gc.getPlayer(ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	city = player.getCity(iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return False

	iStoredCityId = _getOfficersWifeCity(player)
	if iStoredCityId == -1:
		return False

	if city.getID() != iStoredCityId:
		return False

	iStoredTurn = _getOfficersWifeTurn(player)
	if iStoredTurn == -1:
		return False

	if CyGame().getGameTurn() < iStoredTurn + 5:
		return False

	return True


OFFICERS_WIFE_ASSIGNMENT_CITY_PREFIX = "[[WTP_OFFICERS_WIFE_ASSIGNMENT_CITY_ID="
OFFICERS_WIFE_ASSIGNMENT_CITY_SUFFIX = "]]"

OFFICERS_WIFE_ASSIGNMENT_TURN_PREFIX = "[[WTP_OFFICERS_WIFE_ASSIGNMENT_TURN="
OFFICERS_WIFE_ASSIGNMENT_TURN_SUFFIX = "]]"


def _setOfficersWifeAssignmentData(player, iCityId):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	for szPrefix, szSuffix in (
		(OFFICERS_WIFE_ASSIGNMENT_CITY_PREFIX, OFFICERS_WIFE_ASSIGNMENT_CITY_SUFFIX),
		(OFFICERS_WIFE_ASSIGNMENT_TURN_PREFIX, OFFICERS_WIFE_ASSIGNMENT_TURN_SUFFIX),
	):
		iStart = szData.find(szPrefix)
		if iStart != -1:
			iEnd = szData.find(szSuffix, iStart)
			if iEnd != -1:
				iEnd += len(szSuffix)
				szData = szData[:iStart] + szData[iEnd:]

	szData += "%s%d%s" % (
		OFFICERS_WIFE_ASSIGNMENT_CITY_PREFIX,
		iCityId,
		OFFICERS_WIFE_ASSIGNMENT_CITY_SUFFIX
	)

	szData += "%s%d%s" % (
		OFFICERS_WIFE_ASSIGNMENT_TURN_PREFIX,
		CyGame().getGameTurn(),
		OFFICERS_WIFE_ASSIGNMENT_TURN_SUFFIX
	)

	player.setScriptData(szData)


def _getOfficersWifeAssignmentCity(player):
	return _getOfficersWifeNumber(
		player,
		OFFICERS_WIFE_ASSIGNMENT_CITY_PREFIX,
		OFFICERS_WIFE_ASSIGNMENT_CITY_SUFFIX
	)


def _getOfficersWifeAssignmentTurn(player):
	return _getOfficersWifeNumber(
		player,
		OFFICERS_WIFE_ASSIGNMENT_TURN_PREFIX,
		OFFICERS_WIFE_ASSIGNMENT_TURN_SUFFIX
	)


def applyOfficersWife2(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city.isNone():
		return

	_setOfficersWifeAssignmentData(player, city.getID())


def canDoCityOfficersWifeAssignment(argsList):
	eTrigger = argsList[0]
	ePlayer = argsList[1]
	iCityId = argsList[2]

	player = gc.getPlayer(ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	city = player.getCity(iCityId)
	if city.isNone() or city.getOwner() != player.getID():
		return False

	iStoredCityId = _getOfficersWifeAssignmentCity(player)
	if iStoredCityId == -1:
		return False

	if city.getID() != iStoredCityId:
		return False

	iStoredTurn = _getOfficersWifeAssignmentTurn(player)
	if iStoredTurn == -1:
		return False

	if CyGame().getGameTurn() < iStoredTurn + 5:
		return False

	return True


OFFICERS_WIFE_ARRIVAL_TURN_PREFIX = "[[WTP_OFFICERS_WIFE_ARRIVAL_TURN="
OFFICERS_WIFE_ARRIVAL_TURN_SUFFIX = "]]"


def _setOfficersWifeArrivalData(player):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(OFFICERS_WIFE_ARRIVAL_TURN_PREFIX)
	if iStart != -1:
		iEnd = szData.find(OFFICERS_WIFE_ARRIVAL_TURN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(OFFICERS_WIFE_ARRIVAL_TURN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szData += "%s%d%s" % (
		OFFICERS_WIFE_ARRIVAL_TURN_PREFIX,
		CyGame().getGameTurn(),
		OFFICERS_WIFE_ARRIVAL_TURN_SUFFIX
	)

	player.setScriptData(szData)


def _getOfficersWifeArrivalTurn(player):
	return _getOfficersWifeNumber(
		player,
		OFFICERS_WIFE_ARRIVAL_TURN_PREFIX,
		OFFICERS_WIFE_ARRIVAL_TURN_SUFFIX
	)


def applyOfficersWifeAssignment(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	_setOfficersWifeArrivalData(player)


def getHelpOfficersWifeAssignment(argsList):
	return localText.getText(
		"TXT_KEY_EVENT_OFFICERS_WIFE_ASSIGNMENT_HELP",
		(5,)
	)


def canTriggerOfficersWifeArrival(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	iStoredTurn = _getOfficersWifeArrivalTurn(player)
	if iStoredTurn == -1:
		return False

	if CyGame().getGameTurn() < iStoredTurn + 5:
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False

	if unit.getUnitClassType() != gc.getInfoTypeForString("UNITCLASS_BRAVE_LIEUTENANT"):
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return False

	if plot.getOwner() != player.getID():
		return False

	if plot.getImprovementType() not in (
		gc.getInfoTypeForString("IMPROVEMENT_FORT"),
	):
		return False

	return True

OFFICERS_WIFE_UPGRADE_TURN_PREFIX = "[[WTP_OFFICERS_WIFE_UPGRADE_TURN="
OFFICERS_WIFE_UPGRADE_TURN_SUFFIX = "]]"


def _setOfficersWifeUpgradeData(player):
	if player.isNone():
		return

	szData = player.getScriptData()
	if szData is None:
		szData = ""

	iStart = szData.find(OFFICERS_WIFE_UPGRADE_TURN_PREFIX)
	if iStart != -1:
		iEnd = szData.find(OFFICERS_WIFE_UPGRADE_TURN_SUFFIX, iStart)
		if iEnd != -1:
			iEnd += len(OFFICERS_WIFE_UPGRADE_TURN_SUFFIX)
			szData = szData[:iStart] + szData[iEnd:]

	szData += "%s%d%s" % (
		OFFICERS_WIFE_UPGRADE_TURN_PREFIX,
		CyGame().getGameTurn(),
		OFFICERS_WIFE_UPGRADE_TURN_SUFFIX
	)

	player.setScriptData(szData)


def _getOfficersWifeUpgradeTurn(player):
	return _getOfficersWifeNumber(
		player,
		OFFICERS_WIFE_UPGRADE_TURN_PREFIX,
		OFFICERS_WIFE_UPGRADE_TURN_SUFFIX
	)


def applyOfficersWifeArrival(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	_setOfficersWifeUpgradeData(player)


def getHelpOfficersWifeArrival(argsList):
	return localText.getText(
		"TXT_KEY_EVENT_OFFICERS_WIFE_ARRIVAL_HELP",
		(5,)
	)


def canTriggerOfficersWifeUpgrade(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	iStoredTurn = _getOfficersWifeUpgradeTurn(player)
	if iStoredTurn == -1:
		return False

	if CyGame().getGameTurn() < iStoredTurn + 5:
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False

	if unit.getUnitClassType() != gc.getInfoTypeForString("UNITCLASS_BRAVE_LIEUTENANT"):
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	if plot.getX() != kTriggeredData.iPlotX or plot.getY() != kTriggeredData.iPlotY:
		return False

	if plot.getOwner() != player.getID():
		return False

	if plot.getImprovementType() != gc.getInfoTypeForString("IMPROVEMENT_FORT"):
		return False

	return True

####### Slave Hunter Offers Service ###########

def _isPlotAdjacentToCityCulture(city, plot):
	if city.isNone() or plot is None or plot.isNone():
		return False

	iOwner = city.getOwner()

	for iDX in range(-1, 2):
		for iDY in range(-1, 2):

			if iDX == 0 and iDY == 0:
				continue

			adjPlot = plotXY(plot.getX(), plot.getY(), iDX, iDY)

			if adjPlot is None or adjPlot.isNone():
				continue

			if adjPlot.getOwner() != iOwner:
				continue

			workingCity = adjPlot.getWorkingCity()

			if workingCity is None or workingCity.isNone():
				continue

			if workingCity.getOwner() == city.getOwner() and workingCity.getID() == city.getID():
				return True

	return False


def checkRunawaySlavesOnAdjacentPlotOfCity(argsList):
	eTrigger = argsList[0]
	ePlayer = argsList[1]
	iCityId = argsList[2]

	player = gc.getPlayer(ePlayer)

	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	city = player.getCity(iCityId)

	if city.isNone() or city.getOwner() != player.getID():
		return False

	eEvent = gc.getInfoTypeForString("EVENT_SLAVE_HUNTER_SERVICE_ACCEPT")
	event = gc.getEventInfo(eEvent)

	aUnitClasses = (
		event.getGenericParameter(1),
		event.getGenericParameter(2),
		event.getGenericParameter(3),
	)

	barbarianPlayer = gc.getPlayer(gc.getGame().getBarbarianPlayer())

	(unit, iter) = barbarianPlayer.firstUnit()

	while unit:

		if unit.getUnitClassType() in aUnitClasses:

			plot = unit.plot()

			if _isPlotAdjacentToCityCulture(city, plot):
				return True

		(unit, iter) = barbarianPlayer.nextUnit(iter)

	return False
   
####### Slave and Plantation Owner Daughter ###########

def canTriggerSlaveAndPlantationOwnerDaughter(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)

	if player.isNone():
		return False

	if not player.isPlayable():
		return False

	if player.isNative():
		return False

	plot = CyMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)

	if plot is None or plot.isNone():
		return False

	if plot.getOwner() != player.getID():
		return False

	if plot.getImprovementType() not in (
		gc.getInfoTypeForString("IMPROVEMENT_PLANTATION"),
		gc.getInfoTypeForString("IMPROVEMENT_LARGE_PLANTATION"),
	):
		return False

	city = plot.getWorkingCity()

	if city is None or city.isNone():
		return False

	# City must still belong to the triggering player
	if city.getOwner() != player.getID():
		return False

	return True

def doSlaveAndPlantationOwnerDaughter1(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return

	plot = CyMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	if plot is None or plot.isNone():
		return

	if plot.getOwner() != player.getID():
		return

	city = plot.getWorkingCity()
	if city is None or city.isNone():
		return

	if city.getOwner() != player.getID():
		return

	cityPlot = city.plot()

	eEvent = gc.getInfoTypeForString("EVENT_SLAVE_AND_PLANATION_OWNER_DAUGHTER_1")
	event = gc.getEventInfo(eEvent)

	iUnitClass = event.getGenericParameter(1)
	iNumUnits = event.getGenericParameter(2)

	barbarianPlayer = gc.getPlayer(gc.getGame().getBarbarianPlayer())
	iUnitType = gc.getCivilizationInfo(barbarianPlayer.getCivilizationType()).getCivilizationUnits(iUnitClass)

	if iUnitType == UnitTypes.NO_UNIT:
		return

	for i in range(iNumUnits):
		for iDirection in range(DirectionTypes.NUM_DIRECTION_TYPES):
			spawnPlot = plotDirection(cityPlot.getX(), cityPlot.getY(), DirectionTypes(iDirection))

			if spawnPlot is None or spawnPlot.isNone():
				continue

			if spawnPlot.isWater() or spawnPlot.isCity():
				continue

			unit = barbarianPlayer.initUnit(
				iUnitType,
				ProfessionTypes.NO_PROFESSION,
				spawnPlot.getX(),
				spawnPlot.getY(),
				UnitAITypes.NO_UNITAI,
				DirectionTypes.DIRECTION_SOUTH,
				0
			)

			if unit.isNone():
				continue

			if unit.canMoveInto(cityPlot, True, False, False):
				unit.attack(cityPlot, False)

			break

####### Trade Fort Event ###########

def canTriggerTradeFort(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone() or unit.getOwner() != player.getID():
		return False

	if unit.getUnitClassType() not in (
		gc.getInfoTypeForString("UNITCLASS_EXPERT_TRADER"),
		gc.getInfoTypeForString("UNITCLASS_SEASONED_TRADER"),
	):
		return False

	plot = unit.plot()
	if plot is None or plot.isNone():
		return False

	if plot.getOwner() != player.getID():
		return False

	if plot.isWater() or plot.isCity():
		return False

	if plot.getImprovementType() != -1:
		return False

	return True

####### Corrupt Statesman Event ###########

def _countUnitClassInCityPopulation(city, iUnitClass):
	if city is None or city.isNone():
		return 0

	iCount = 0

	for i in range(city.getPopulation()):
		unit = city.getPopulationUnitByIndex(i)

		if unit is None or unit.isNone():
			continue

		if unit.getUnitClassType() == iUnitClass:
			iCount += 1

	return iCount


def _removeFirstUnitClassFromCityPopulation(city, iUnitClass):
	if city is None or city.isNone():
		return False

	for i in range(city.getPopulation()):
		unit = city.getPopulationUnitByIndex(i)

		if unit is None or unit.isNone():
			continue

		if unit.getUnitClassType() == iUnitClass:
			city.removePopulationUnit(unit, True, ProfessionTypes.NO_PROFESSION)
			return True

	return False


def canTriggerThreeStatesmenInCity(argsList):
	eTrigger = argsList[0]
	ePlayer = argsList[1]
	iCityId = argsList[2]

	player = gc.getPlayer(ePlayer)
	if player.isNone() or not player.isPlayable() or player.isNative():
		return False

	city = player.getCity(iCityId)
	if city is None or city.isNone() or city.getOwner() != player.getID():
		return False

	iStatesmanClass = gc.getInfoTypeForString("UNITCLASS_STATESMAN")
	if iStatesmanClass == -1:
		return False

	if _countUnitClassInCityPopulation(city, iStatesmanClass) < 3:
		return False

	return True


def applyKingAngryAndRemoveOneStatesman(argsList):
	kTriggeredData = argsList[0]

	player = gc.getPlayer(kTriggeredData.ePlayer)
	if player.isNone():
		return

	city = player.getCity(kTriggeredData.iCityId)
	if city is None or city.isNone() or city.getOwner() != player.getID():
		return

	applyKingAngry(argsList)

	iStatesmanClass = gc.getInfoTypeForString("UNITCLASS_STATESMAN")
	if iStatesmanClass == -1:
		return

	if _countUnitClassInCityPopulation(city, iStatesmanClass) < 3:
		return

	_removeFirstUnitClassFromCityPopulation(city, iStatesmanClass)


def getHelpKingAngryAndRemoveOneStatesman(argsList):
	szHelp = getHelpKingAngry(argsList)

	if szHelp:
		szHelp += u"\n"

	szHelp += localText.getText(
		"TXT_KEY_EVENT_CORRUPT_STATESMAN_REMOVE_ONE_STATESMAN",
		()
	)

	return szHelp