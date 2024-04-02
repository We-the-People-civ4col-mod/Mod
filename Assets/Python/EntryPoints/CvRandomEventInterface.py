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
import CvUtil
import CvScreensInterface
from CvPythonExtensions import *

gc = CyGlobalContext()
localText = CyTranslator()

def get_simple_help(text_key):
	""" This function constructs another function that returns the fixed localized text  """
	def get_help(argsList):
		szHelp = localText.getText(text_key, ())
		return szHelp

	return get_help


def doEventEndTutorial(argsList):
	eEvent = argsList[0]
	pTriggeredData = argsList[1]
	player = gc.getPlayer(pTriggeredData.ePlayer)
	CyMessageControl().sendPlayerOption(PlayerOptionTypes.PLAYEROPTION_TUTORIAL, False)

def isExpiredFoundColony(argsList):
	eEvent = argsList[0]
	pTriggeredData = argsList[1]
	player = gc.getPlayer(pTriggeredData.ePlayer)
	if player.getNumCities() > 0:
		return True
	return False

def doEventCivilopediaSettlement(argsList):
	eEvent = argsList[0]
	pTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	pTriggeredData = argsList[1]
	CvScreensInterface.pediaShowHistorical((CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT, gc.getInfoTypeForString("CONCEPT_EUROPE")))

def doEventCivilopediaImmigration(argsList):
	eEvent = argsList[0]
	pTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	pTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	pTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	pTriggeredData = argsList[1]
	CvScreensInterface.pediaShowHistorical((CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT, gc.getInfoTypeForString("CONCEPT_FATHERS")))

def doEventCivilopediaNativeVillages(argsList):
	eEvent = argsList[0]
	pTriggeredData = argsList[1]
	CvScreensInterface.pediaShowHistorical((CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT, gc.getInfoTypeForString("CONCEPT_NATIVES")))

def canDoTriggerRevolution( argsList ):
	pTriggeredData = argsList[ 0 ]
	player = gc.getPlayer( pTriggeredData.ePlayer )

	if gc.getTeam(player.getTeam()).canDoRevolution():
		return True

	return False

def doEventCivilopediaRevolution(argsList):
	eEvent = argsList[0]
	pTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	pTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	pTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	pTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	if not player.isHuman():
		city = player.firstCity(True)[0]
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	iYield1 = gc.getInfoTypeForString("YIELD_BLADES")
	city.changeYieldStored(iYield1, event.getGenericParameter(1)*Speed.getTrainPercent()/100)

def getHelpSecondCity2(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	city = player.getCity(kTriggeredData.iCityId)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	iYield1 = gc.getInfoTypeForString("YIELD_BLADES")
	szHelp = localText.getText("TXT_KEY_EVENT_SECONDCOLONY_2_HELP", (king.getCivilizationAdjectiveKey(), ))
	if event.getGenericParameter(1) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_GAIN", (event.getGenericParameter(1)*Speed.getTrainPercent()/100,  gc.getYieldInfo(iYield1).getChar(), city.getNameKey()))
		overflow = event.getGenericParameter(1)*Speed.getTrainPercent()/100 + city.getYieldStored(iYield1) - city.getMaxYieldCapacity()
		if overflow > 0:
			szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_OVERFLOW", (overflow,  gc.getYieldInfo(iYield1).getChar(), city.getNameKey()))
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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

def canTriggerWinter(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if not player.isPlayable():
		return False
	king = gc.getPlayer(player.getParent())
	if not king.isEurope():
		return False
	#iCurrentTurn = CyGame().getGameTurn()
	#szDate = CyGameTextMgr().getTimeStr(iCurrentTurn+1, true)
	#January = localText.getText("TXT_KEY_MONTH_JANUARY", ())
	#February = localText.getText("TXT_KEY_MONTH_FEBRUARY", ())
	#December = localText.getText("TXT_KEY_MONTH_DECEMBER", ())
	#November = localText.getText("TXT_KEY_MONTH_NOVEMBER", ())
	#October = localText.getText("TXT_KEY_MONTH_OCTOBER", ())
	#if (gc.getGameSpeedInfo(gc.getGame().getGameSpeedType()).getGameTurnInfo(0).iMonthIncrement != 12):
	#	if (January in szDate or February in szDate or December in szDate or November in szDate or October in szDate):
	#		return true
	#return False
	return True

def applyWinter(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	iYield1 = gc.getInfoTypeForString("YIELD_COATS")
	iYield2 = gc.getInfoTypeForString("YIELD_FUR")
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	iPrice1 = king.getYieldBuyPrice(iYield1)
	king.setYieldBuyPrice(iYield1, iPrice1+event.getGenericParameter(1), 1)
	iPrice2 = king.getYieldBuyPrice(iYield2)
	king.setYieldBuyPrice(iYield2, iPrice2+event.getGenericParameter(2), 1)

def getHelpWinter(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	iYield1 = gc.getInfoTypeForString("YIELD_COATS")
	iYield2 = gc.getInfoTypeForString("YIELD_FUR")
	szHelp = localText.getText("TXT_KEY_EVENT_WINTER_HELP", (king.getCivilizationShortDescriptionKey(),))
	if event.getGenericParameter(1) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_PRICE_INCREASE", (event.getGenericParameter(2), gc.getYieldInfo(iYield1).getChar(), king.getCivilizationShortDescriptionKey()))
	if event.getGenericParameter(2) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_PRICE_INCREASE", (event.getGenericParameter(2), gc.getYieldInfo(iYield2).getChar(), king.getCivilizationShortDescriptionKey()))
	return szHelp

def canEndWinter(argsList):
	#iCurrentTurn = CyGame().getGameTurn()
	#kTriggeredData = argsList[1]
	#player = gc.getPlayer(kTriggeredData.ePlayer)
	#eEvent = gc.getInfoTypeForString("EVENT_WINTER_1")
	#kEventdata = player.getEventOccured(eEvent)
	#iWinterTurn = kEventdata.iTurn
	#CyInterface().addImmediateMessage(str(iWinterTurn)+" Winter Start", "")
	#CyInterface().addImmediateMessage(str(iCurrentTurn)+" Aktuell", "")
	#if iCurrentTurn <= (iWinterTurn + 3) :
	#	return False
	#szDate = CyGameTextMgr().getTimeStr(iCurrentTurn+1, true)
	#January = localText.getText("TXT_KEY_MONTH_JANUARY", ())
	#February = localText.getText("TXT_KEY_MONTH_FEBRUARY", ())
	#December = localText.getText("TXT_KEY_MONTH_DECEMBER", ())
	#November = localText.getText("TXT_KEY_MONTH_NOVEMBER", ())
	#October = localText.getText("TXT_KEY_MONTH_OCTOBER", ())
	#if (gc.getGameSpeedInfo(gc.getGame().getGameSpeedType()).getGameTurnInfo(0).iMonthIncrement != 12):
		#if not (January in szDate or February in szDate or December in szDate or November in szDate or October in szDate):
	#		return true
	#return False
	return True

def applyEndWinter(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	iYield1 = gc.getInfoTypeForString("YIELD_COATS")
	iYield2 = gc.getInfoTypeForString("YIELD_FUR")
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	iPrice1 = king.getYieldBuyPrice(iYield1)
	king.setYieldBuyPrice(iYield1, iPrice1+event.getGenericParameter(1), 1)
	iPrice2 = king.getYieldBuyPrice(iYield2)
	king.setYieldBuyPrice(iYield2, iPrice2+event.getGenericParameter(2), 1)

def getHelpEndWinter(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	iYield1 = gc.getInfoTypeForString("YIELD_COATS")
	iYield2 = gc.getInfoTypeForString("YIELD_FUR")
	szHelp = localText.getText("TXT_KEY_EVENT_END_WINTER_HELP", (king.getCivilizationShortDescriptionKey(), ))
	if event.getGenericParameter(1) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_PRICE_DECREASE", (event.getGenericParameter(2), gc.getYieldInfo(iYield1).getChar(), king.getCivilizationShortDescriptionKey()))
	if event.getGenericParameter(2) <> 0 :
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_PRICE_DECREASE", (event.getGenericParameter(2), gc.getYieldInfo(iYield2).getChar(), king.getCivilizationShortDescriptionKey()))
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
	kTriggeredData = argsList[1]
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
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	iYield1 = gc.getInfoTypeForString("YIELD_MUSKETS")
	iYield2 = gc.getInfoTypeForString("YIELD_FOOD")
	iPriceChange = 2
	szHelp = localText.getText("TXT_KEY_EVENT_PEASANT_WARPREP_HELP", (iPriceChange, gc.getYieldInfo(iYield1).getChar(), king.getCivilizationDescriptionKey(), iPriceChange, gc.getYieldInfo(iYield2).getChar(), king.getCivilizationDescriptionKey()))
	return szHelp

######## Discovery Events for Scouts ###########

def canTriggerDiscoveryStart(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if not player.isPlayable():
		return False
	unit = player.getUnit(kTriggeredData.iUnitId)
	eScout = gc.getInfoTypeForString("PROFESSION_SCOUT")
	if unit.getProfession() != eScout:
		return False
	return True


def canTriggerDiscovery(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if not player.isPlayable():
		return False
	unit = player.getUnit(kTriggeredData.iUnitId)
	eScout = gc.getInfoTypeForString("PROFESSION_SCOUT")
	if unit.getProfession() != eScout:
		return False
	# Read parameter 3 from the event as random chance
	if TriggerChance(argsList):
		return True
	return False

######## The Lost Tribe ###########

def canTriggerLostTribe(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if not player.isPlayable():
		return False
	unit = player.getUnit(kTriggeredData.iUnitId)
	eScout = gc.getInfoTypeForString("PROFESSION_SCOUT")
	if unit.getProfession() != eScout:
		return False
	# Read parameter 3 from the event as random chance
	if TriggerChance(argsList):
		return True
	return False
 
def canDoLostTribe4(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	(unit, iter) = player.firstUnit()
	while (unit):
		if unit.getUnitClassType() == CvUtil.findInfoTypeNum('UNITCLASS_SCOUT'):
			return False
		(unit, iter) = player.nextUnit(iter)
	return True

def getHelpLostTribe4(argsList):
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unit = player.getUnit(kTriggeredData.iUnitId)
	szHelp = getHelpChangeFatherPoints(argsList)
	UnitClass = gc.getUnitClassInfo(CvUtil.findInfoTypeNum('UNITCLASS_SCOUT'))
	UnitClass2 = gc.getUnitClassInfo(unit.getUnitClassType())
	UnitProf1 = gc.getProfessionInfo(unit.getProfession())
	szHelp += "\n" + localText.getText("TXT_KEY_EVENT_LOST_TRIBE_4_HELP", (UnitClass2.getTextKey(), UnitProf1.getTextKey(), UnitClass.getTextKey())) 
	if not canDoLostTribe4(argsList):
		szHelp += "\n\n" + localText.getText("TXT_KEY_EVENT_LOST_TRIBE_4B_HELP", (UnitClass.getTextKey(),))
	return szHelp

def applyLostTribe4(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	ChangeFatherPoints(argsList)
	iUnitClassType = CvUtil.findInfoTypeNum('UNITCLASS_SCOUT')
	iProfession = CvUtil.findInfoTypeNum("PROFESSION_SCOUT")
	iUnitType = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationUnits(iUnitClassType)
	if iUnitType != -1:
		player.initUnit(iUnitType, iProfession, kTriggeredData.iPlotX, kTriggeredData.iPlotY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH, 0)
	(unitnew, iter) = player.firstUnit()
	while (unitnew):
		if unitnew.getUnitClassType() == CvUtil.findInfoTypeNum('UNITCLASS_SCOUT'):
			break
		(unitnew, iter) = player.nextUnit(iter)
	unit = player.getUnit(kTriggeredData.iUnitId)
	unitnew.convert(unit)
 
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
	iEvent = argsList[0]
	kTriggeredData = argsList[1]
	
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
	iEvent = argsList[0]
	kTriggeredData = argsList[1]
	
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
	if gc.getGame().getSorenRandNum(100, "Volcano event dormant") < 25:
		return True
	return False

def applyVolcanoDormant1(argsList):
	iEvent = argsList[0]
	kTriggeredData = argsList[1]
	
	plot = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	plot.setPlotType(PlotTypes.PLOT_PEAK, True, True)

######## TORNADO ###########

def applyTornado1(argsList):
	iEvent = argsList[0]
	kTriggeredData = argsList[1]
	
	plot = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	plot.setFeatureType(gc.getInfoTypeForString('FEATURE_TORNADO'), 0)

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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	if event.getGenericParameter(2) <> 0 :
		szHelp = localText.getText("TXT_KEY_EVENT_BABY_BOOM_HELP", (event.getGenericParameter(2),))
	return szHelp

######## Flaute ###########

def canApplyCalm(argsList):
	eEvent = argsList[0]
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unit = player.getUnit(kTriggeredData.iUnitId)
	if unit.isNone():
		return False
	if (unit.getUnitTravelState() == UnitTravelStates.UNIT_TRAVEL_STATE_IN_EUROPE):
		return False
	return True

def applyCalm(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unit = player.getUnit(kTriggeredData.iUnitId)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	turn = Speed.getStoragePercent()/100
	if not unit.isNone():
		if (unit.getUnitTravelState() == UnitTravelStates.UNIT_TRAVEL_STATE_FROM_EUROPE) or (unit.getUnitTravelState() == UnitTravelStates.UNIT_TRAVEL_STATE_TO_EUROPE):
			unit.setUnitTravelTimer(unit.getUnitTravelTimer() + turn)

def getHelpCalm(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unit = player.getUnit(kTriggeredData.iUnitId)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	turn = Speed.getStoragePercent()/100
	szHelp = ""
	if not unit.isNone():
		if (unit.getUnitTravelState() == UnitTravelStates.UNIT_TRAVEL_STATE_FROM_EUROPE) or (unit.getUnitTravelState() == UnitTravelStates.UNIT_TRAVEL_STATE_TO_EUROPE):
				szHelp = localText.getText("TXT_KEY_EVENT_CALM_HELP", (turn, unit.getName()))
	return szHelp

######## Tailwind ###########

def applyTailwind(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unit = player.getUnit(kTriggeredData.iUnitId)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	turn = Speed.getStoragePercent()/100
	if not unit.isNone():
		if event.getGenericParameter(1) > 0 :
			if (unit.getUnitTravelState() == UnitTravelStates.UNIT_TRAVEL_STATE_FROM_EUROPE) or (unit.getUnitTravelState() == UnitTravelStates.UNIT_TRAVEL_STATE_TO_EUROPE):
				if unit.getUnitTravelTimer() > turn :
					unit.setUnitTravelTimer(unit.getUnitTravelTimer() - turn)
				else:
					unit.setUnitTravelTimer(1)
			else:
				unit.changeMoves(-60 * event.getGenericParameter(1))

def canApplyTailwind(argsList):
	eEvent = argsList[0]
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unit = player.getUnit(kTriggeredData.iUnitId)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	turn = Speed.getStoragePercent()/100
	if unit.isNone():
		return False
	if (unit.getUnitTravelState() == UnitTravelStates.UNIT_TRAVEL_STATE_IN_EUROPE):
		return False
	if (unit.getUnitTravelState() == UnitTravelStates.UNIT_TRAVEL_STATE_FROM_EUROPE) or (unit.getUnitTravelState() == UnitTravelStates.UNIT_TRAVEL_STATE_TO_EUROPE):
		if unit.getUnitTravelTimer() <= 1 :
			return False
	return True

def getHelpTailwind(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unit = player.getUnit(kTriggeredData.iUnitId)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	turn = Speed.getStoragePercent()/100
	szHelp = ""
	if not unit.isNone():
		if event.getGenericParameter(1) > 0 :
			if (unit.getUnitTravelState() == UnitTravelStates.UNIT_TRAVEL_STATE_FROM_EUROPE) or (unit.getUnitTravelState() == UnitTravelStates.UNIT_TRAVEL_STATE_TO_EUROPE):
				szHelp = localText.getText("TXT_KEY_EVENT_TAILWIND_HELP_2", (turn, unit.getName()))
			else:
				szHelp = localText.getText("TXT_KEY_EVENT_TAILWIND_HELP_1", (event.getGenericParameter(1), unit.getName()))
	return szHelp

######## RUNAWAY - Entlaufene Pferde ###########

def canTriggerRunAway(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if not player.isPlayable():
		return False
	city = player.getCity(kTriggeredData.iCityId)
	player2 = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if player.isNone() or player2.isNone() :
		return False
	if city.isNone():
		return False
	# Read Parameter 1 from the first event and check if enough yield is stored in city
	eEvent1 = gc.getInfoTypeForString("EVENT_RUNAWAY_1")
	event1 = gc.getEventInfo(eEvent1)
	iYield = gc.getInfoTypeForString("YIELD_HORSES")
	quantity = event1.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100

	if city.getYieldStored(iYield) < -quantity*2 :
		return False
	return True

def applyRunAway1(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	kTriggeredData = argsList[1]
	pPlot = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	CyEngine().triggerEffect(gc.getInfoTypeForString("EFFECT_SETTLERSMOKE"), pPlot.getPoint())

######## Cargospace ###########

def canTriggerCargoSpace(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	unit = player.getUnit(kTriggeredData.iUnitId)
	if city.getX() == unit.getX() and city.getY() == unit.getY():
		return True
	return False

def applyCargoSpace(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unit = player.getUnit(kTriggeredData.iUnitId)
	if gc.getGame().getSorenRandNum(100, "(c) TAC 2010 Events") < event.getGenericParameter(3):
		if event.getGenericParameter(1) >= 0:
			unit.changeFreePromotionCount(event.getGenericParameter(1), 1)
			CyInterface().addImmediateMessage(localText.getText("TXT_KEY_EVENT_CARGOSPACE_SUCCESS", (unit.getName(),)), "")
	else:
		CyInterface().addImmediateMessage(localText.getText("TXT_KEY_EVENT_CARGOSPACE_FAILED", ()), "")
	if event.getGenericParameter(2) > 0:
		unit.setImmobileTimer(event.getGenericParameter(2)) 

def helpCargoSpace(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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

def canTriggerRumBlossom(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if not player.isPlayable():
		return False
	city = player.getCity(kTriggeredData.iCityId)
	player2 = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if player.isNone() or player2.isNone() :
		return False
	if city.isNone():
		return False
	# Read Parameter 1 from the first event and check if enough yield is stored in city
	eEvent1 = gc.getInfoTypeForString("EVENT_RUM_BLOSSOM_1")
	event1 = gc.getEventInfo(eEvent1)
	iYield = gc.getInfoTypeForString("YIELD_RUM")
	quantity = event1.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity :
		return False
	return True

def applyRumBlossom1(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	player2 = gc.getPlayer(kTriggeredData.eOtherPlayer)
	nativecity = player2.getCity(kTriggeredData.iOtherPlayerCityId)
	iYield = gc.getInfoTypeForString("YIELD_RUM")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity:
		return
	city.changeYieldStored(iYield, quantity)
	nativecity.changeYieldStored(iYield, -quantity)

def getHelpRumBlossom1(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	player2 = gc.getPlayer(kTriggeredData.eOtherPlayer)
	nativecity = player2.getCity(kTriggeredData.iOtherPlayerCityId)
	iYield = gc.getInfoTypeForString("YIELD_RUM")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	szHelp = ""
	if event.getGenericParameter(1) <> 0 :
		szHelp = localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity,  gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_GAIN", (-quantity,  gc.getYieldInfo(iYield).getChar(), nativecity.getNameKey()))
	return szHelp

def canApplyRumBlossom3(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	player2 = gc.getPlayer(kTriggeredData.eOtherPlayer)
	city = player.getCity(kTriggeredData.iCityId)
	if player.isNone() or player2.isNone() :
		return False
	if city.isNone():
		return False
	# Read Parameter 1 from event and check if enough yield is stored in city
	iYield = gc.getInfoTypeForString("YIELD_RUM")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity :
		return False
	return True

######## Ruins Quest ###########

def isExpiredRuins(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	UnitClass = gc.getUnitClassInfo(CvUtil.findInfoTypeNum('UNITCLASS_SCOUT'))
	szHelp = localText.getText("TXT_KEY_EVENT_RUINS_HELP", (UnitClass.getTextKey(), city.getNameKey(), event.getGenericParameter(1)))
	return szHelp

def applyRuins5(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	iUnitClassType = CvUtil.findInfoTypeNum('UNITCLASS_CARRIER')
	iUnitType = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationUnits(iUnitClassType)
	if iUnitType != -1:
		player.initUnit(iUnitType, 0, kTriggeredData.iPlotX, kTriggeredData.iPlotY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH, 0)

def getHelpRuins5(argsList):
	UnitClass = gc.getUnitClassInfo(CvUtil.findInfoTypeNum('UNITCLASS_CARRIER'))
	szHelp = localText.getText("TXT_KEY_EVENT_BONUS_UNIT", (1, UnitClass.getTextKey(), ))
	return szHelp

######## Native Trade with Wagons Quest ###########

def isExpiredNativeWagonTrade(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if gc.getGame().getGameTurn() >= kTriggeredData.iTurn + event.getGenericParameter(1):
		return True
	if not player.isPlayable():
		return True
	return False

def getHelpNativeWagonTrade(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	UnitClass = gc.getUnitClassInfo(CvUtil.findInfoTypeNum('UNITCLASS_WAGON_TRAIN'))
	szHelp = localText.getText("TXT_KEY_EVENT_NATIVE_TRADE_WAGON_HELP", (UnitClass.getTextKey(), city.getNameKey(), event.getGenericParameter(1)))
	return szHelp

def applyNativeWagonTrade5(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	iUnitClassType = CvUtil.findInfoTypeNum('UNITCLASS_WAGON_TRAIN')
	iUnitType = gc.getCivilizationInfo(player.getCivilizationType()).getCivilizationUnits(iUnitClassType)
	if iUnitType != -1:
		player.initUnit(iUnitType, 0, kTriggeredData.iPlotX, kTriggeredData.iPlotY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH, 0)

def getHelpNativeWagonTrade5(argsList):
	UnitClass = gc.getUnitClassInfo(CvUtil.findInfoTypeNum('UNITCLASS_WAGON_TRAIN'))
	szHelp = localText.getText("TXT_KEY_EVENT_BONUS_UNIT", (1, UnitClass.getTextKey(), ))
	return szHelp
	
####### The Royals Event ########
	
getHelpTheRoyals1  = get_simple_help("TXT_KEY_EVENT_THE_ROYALS_1PYTHON")
getHelpTheRoyals2  = get_simple_help("TXT_KEY_EVENT_THE_ROYALS_2PYTHON")
getHelpTheRoyals3  = get_simple_help("TXT_KEY_EVENT_THE_ROYALS_3PYTHON")
getHelpTheRoyals4  = get_simple_help("TXT_KEY_EVENT_THE_ROYALS_4PYTHON")
getHelpTheRoyals2a = get_simple_help("TXT_KEY_EVENT_THE_ROYALS_2aPYTHON")


	
####### Pirates Event ########

def canTriggerPirates(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if not player.isPlayable():
		return False
	city = player.getCity(kTriggeredData.iCityId)
	unit = player.getUnit(kTriggeredData.iUnitId)
	if city.getX() == unit.getX() and city.getY() == unit.getY():
		return True
	return False

def CanDoPirates3(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	iYield = gc.getInfoTypeForString("YIELD_HORSES")
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity :
		return False
	return True

def CanDoPirates4(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	iYield = gc.getInfoTypeForString("YIELD_MUSKETS")
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity :
		return False
	return True
	
def applyPirates3(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_HORSES")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity:
		return
	city.changeYieldStored(iYield, quantity)

def applyPirates4(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_MUSKETS")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity:
		return
	city.changeYieldStored(iYield, quantity)

def getHelpPirates3(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_HORSES")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	
	szHelp = ""
	if event.getGenericParameter(1) <> 0 :
		szHelp = localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity,  gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
	return szHelp

def getHelpPirates4(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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

######## Superstitious Pirates Event ###########

def canTriggerSupersitiousPirates(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	if player.isNone() :
		return False
	if not player.isPlayable():
		return False
	if city.isNone():
		return False
	city = player.getCity(kTriggeredData.iCityId)
	unit = player.getUnit(kTriggeredData.iUnitId)
	if city.getX() == unit.getX() and city.getY() == unit.getY():
		return True
	return False
	# Read Parameter 1 from the first event and check if enough yield is stored in city
	eEvent1 = gc.getInfoTypeForString("EVENT_SUPERSTITIOUS_PIRATES_2")
	event1 = gc.getEventInfo(eEvent1)
	iYield = gc.getInfoTypeForString("YIELD_RUM")
	quantity = event1.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity*2 :
		return False
	return True

def applySupersitiousPirates2(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_RUM")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity:
		return
	city.changeYieldStored(iYield, quantity)

def getHelpSupersitiousPirates2(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_RUM")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	
	szHelp = ""
	if event.getGenericParameter(1) <> 0 :
		szHelp = localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity,  gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
	return szHelp

####### TAC Events - General Functions########

######## Units Funktionen ###########

def CreateTreasure(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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

	if not player.isPlayable():
		return False

	# you could add checks for several Units like this
	iUnitType = CvUtil.findInfoTypeNum('UNIT_CHEESE_MAKER')
	iUnitsCurrent = countUnitsInCityForCityTrigger(argsList, iUnitType)
	if iUnitsCurrent == 0:
		return False

	return True


######## Bonus Funktionen ###########

def CanApplyBonus(argsList):
	eEvent = argsList[0]
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	kTriggeredData = argsList[1]
	event = gc.getEventInfo(eEvent)
	plot = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	bonustype = event.getGenericParameter(1)
	if not plot.isNone() and plot.canHaveBonus(bonustype, False):
		plot.setBonusType(bonustype)

def getHelpBonus(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	bonustype = event.getGenericParameter(1)
	szHelp = ""
	if bonustype != -1 :
		szHelp = localText.getText("TXT_KEY_EVENT_BONUS_HELP", (gc.getBonusInfo(bonustype).getTextKey(),))
	return szHelp

######## Landmark Funktionen ###########

def CheckLandmark(argsList):
	eEvent = argsList[0]
	kTriggeredData = argsList[1]
	event = gc.getEventInfo(eEvent)
	szLandmark = "TXT_KEY_%s_LANDMARK"%(event.getType())
	for i in range (CyEngine().getNumSigns()):
		Sign = CyEngine().getSignByIndex(i)
		if (Sign.getPlot().getX() == kTriggeredData.iPlotX and Sign.getPlot().getY() == kTriggeredData.iPlotY):
			return False
	return True

def SetLandmark(argsList):
	eEvent = argsList[0]
	kTriggeredData = argsList[1]
	event = gc.getEventInfo(eEvent)
	if GlobalDefines.SHOW_LANDMARKS == 1:
		szLandmark = "TXT_KEY_%s_LANDMARK"%(event.getType())
		plot = gc.getMap().plot(kTriggeredData.iPlotX,  kTriggeredData.iPlotY)
		CyEngine().addSign(plot, -1, szLandmark)

######## Founding Father Functions ###########

def ChangeFatherPoints(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	team = gc.getTeam(player.getTeam())
	FatherPointChange = 0
	if event.getGenericParameter(1)!=-1:
		Handicap = gc.getHandicapInfo(CyGame().getHandicapType())
		Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
		FatherPointChange = event.getGenericParameter(2)*Speed.getFatherPercent()/100*Handicap.getFatherPercent()/100
		team.changeFatherPoints(event.getGenericParameter(1), FatherPointChange)

def getHelpChangeFatherPoints(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
hasFoodBonus = has_plot_this_bonus("BONUS_POTATO","BONUS_BANANA","BONUS_CORN")
hasSeaFoodBonus = has_plot_this_bonus("BONUS_PEARLS","BONUS_CRAB","BONUS_FISH")
hasBisonBonus = has_plot_this_bonus("BONUS_BISON")
hasPumpkinBonus = has_plot_this_bonus("BONUS_PUMPKIN")
hasTurkeyBonus = has_plot_this_bonus("BONUS_TURKEYS")
hasGiantTreeBonus = has_plot_this_bonus("BONUS_GIANT_TREE")


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
	if gc.getGame().getSorenRandNum(1000, "(c) TAC 2010 Events") < event.getGenericParameter(3):
		return True
	return False
	
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	player.AI_changeAttitudeExtra(eking, event.getGenericParameter(3))
	king.AI_changeAttitudeExtra(kTriggeredData.ePlayer, event.getGenericParameter(3))
	if event.getGenericParameter(4) == 1 :
		player.NBMOD_DecreaseMaxTaxRate()

def getHelpKingPleased(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	return szHelp

def applyKingAngry(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	eking = player.getParent()
	king = gc.getPlayer(eking)
	player.AI_changeAttitudeExtra(eking, event.getGenericParameter(3))
	king.AI_changeAttitudeExtra(kTriggeredData.ePlayer, event.getGenericParameter(3))
	if event.getGenericParameter(4) == 1 :
		player.NBMOD_IncreaseMaxTaxRate()

def getHelpKingAngry(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	ePlayer = argsList[1]
	player = gc.getPlayer(ePlayer)
	if not player.isPlayable():
		return False
	iAchieve = gc.getInfoTypeForString("ACHIEVE_TENTHOUSAND_TRADE")
	if not player.isPlayable():
		return False
	if player.isAchieveGained(iAchieve):
		return True
	return False

def canTriggerPirateAttack2(argsList):
	ePlayer = argsList[1]
	player = gc.getPlayer(ePlayer)
	if not player.isPlayable():
		return False
	iAchieve = gc.getInfoTypeForString("ACHIEVE_HUNDREDTHOUSAND_TRADE")
	if not player.isPlayable():
		return False
	if player.isAchieveGained(iAchieve):
		return True
	return False

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

def doPirateAttack1(argsList):
	iEvent = argsList[0]
	kTriggeredData = argsList[1]
	pPlot = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	bPlayer = gc.getPlayer(gc.getGame().getBarbarianPlayer())
	if pPlot.isNone() == False:
		newUnit = bPlayer.initUnit(gc.getInfoTypeForString('UNIT_PRIVATEER'), -1, pPlot.getX(), pPlot.getY(), UnitAITypes.UNITAI_PIRATE_SEA, DirectionTypes.DIRECTION_SOUTH, 0)
		newUnit2 = bPlayer.initUnit(gc.getInfoTypeForString('UNIT_PIRATE_CUTTER'), -1, pPlot.getX(), pPlot.getY(), UnitAITypes.UNITAI_PIRATE_SEA, DirectionTypes.DIRECTION_SOUTH, 0)

def doPirateAttack2(argsList):
	iEvent = argsList[0]
	kTriggeredData = argsList[1]
	pPlot = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	bPlayer = gc.getPlayer(gc.getGame().getBarbarianPlayer())
	if pPlot.isNone() == False:
		newUnit = bPlayer.initUnit(gc.getInfoTypeForString('UNIT_PIRATE_FRIGATE'), -1, pPlot.getX(), pPlot.getY(), UnitAITypes.UNITAI_PIRATE_SEA, DirectionTypes.DIRECTION_SOUTH, 0)
		newUnit2 = bPlayer.initUnit(gc.getInfoTypeForString('UNIT_PIRATE_FRIGATE'), -1, pPlot.getX(), pPlot.getY(), UnitAITypes.UNITAI_PIRATE_SEA, DirectionTypes.DIRECTION_SOUTH, 0)
		newUnit = bPlayer.initUnit(gc.getInfoTypeForString('UNIT_PRIVATEER'), -1, pPlot.getX(), pPlot.getY(), UnitAITypes.UNITAI_PIRATE_SEA, DirectionTypes.DIRECTION_SOUTH, 0)
		newUnit2 = bPlayer.initUnit(gc.getInfoTypeForString('UNIT_PIRATE_CUTTER'), -1, pPlot.getX(), pPlot.getY(), UnitAITypes.UNITAI_PIRATE_SEA, DirectionTypes.DIRECTION_SOUTH, 0)

######## BEER ROBBERY ###########

def canTriggerBeerRobbery(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	player2 = gc.getPlayer(kTriggeredData.eOtherPlayer)
	if player.isNone() or player2.isNone() :
		return False
	if not player.isPlayable():
		return False
	if city.isNone():
		return False
	# Read Parameter 1 from the first event and check if enough yield is stored in city
	eEvent1 = gc.getInfoTypeForString("EVENT_BEER_ROBBERY_1")
	event1 = gc.getEventInfo(eEvent1)
	iYield = gc.getInfoTypeForString("YIELD_BEER")
	quantity = event1.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	
	if city.getYieldStored(iYield) < -quantity :
		return False
	return True

def applyBeerRobbery1(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	player2 = gc.getPlayer(kTriggeredData.eOtherPlayer)
	othercity = player2.getCity(kTriggeredData.iOtherPlayerCityId)
	iYield = gc.getInfoTypeForString("YIELD_BEER")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity :
		return
	city.changeYieldStored(iYield, quantity)
	othercity.changeYieldStored(iYield, -quantity)

def getHelpBeerRobbery1(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	player2 = gc.getPlayer(kTriggeredData.eOtherPlayer)
	othercity = player2.getCity(kTriggeredData.iOtherPlayerCityId)
	iYield = gc.getInfoTypeForString("YIELD_BEER")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	szHelp = ""
	if event.getGenericParameter(1) <> 0 :
		szHelp = localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity,  gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
		szHelp += "\n" + localText.getText("TXT_KEY_EVENT_YIELD_GAIN", (-quantity,  gc.getYieldInfo(iYield).getChar(), othercity.getNameKey()))
	return szHelp

def canApplyBeerRobbery3(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	player2 = gc.getPlayer(kTriggeredData.eOtherPlayer)
	city = player.getCity(kTriggeredData.iCityId)
	if player.isNone() or player2.isNone() :
		return False
	if city.isNone():
		return False
	# Read Parameter 1 from event and check if enough yield is stored in city
	
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	iYield = gc.getInfoTypeForString("YIELD_BEER")
	if city.getYieldStored(iYield) < -quantity :
		return False
	return True

######## WINE THEFT ###########

def canTriggerWineTheft(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	if player.isNone():
		return False
	if not player.isPlayable():
		return False
	if city.isNone():
		return False
	# Read Parameter 1 from the first event and check if enough yield is stored in city
	eEvent1 = gc.getInfoTypeForString("EVENT_WINE_THEFT_1")
	event1 = gc.getEventInfo(eEvent1)
	iYield = gc.getInfoTypeForString("YIELD_WINE")
	quantity = event1.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity :
		return False
	return True

def applyWineTheft1(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_WINE")
	
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity :
		return
	city.changeYieldStored(iYield, quantity)

def getHelpWineTheft1(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_WINE")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	szHelp = ""
	if event.getGenericParameter(1) <> 0 :
		szHelp = localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity,  gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
	return szHelp
	
######## LUXURY GOODS ###########

def canTriggerLuxuryGoods(argsList):
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
	eEvent1 = gc.getInfoTypeForString("EVENT_LUXURY_GOODS_1")
	event1 = gc.getEventInfo(eEvent1)
	iYield = gc.getInfoTypeForString("YIELD_LUXURY_GOODS")

	quantity = event1.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity :
		return False
	return True

def applyLuxuryGoods1(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_LUXURY_GOODS")

	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity :
		return
	city.changeYieldStored(iYield, quantity)

def getHelpLuxuryGoods1(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_LUXURY_GOODS")
	szHelp = ""
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if event.getGenericParameter(1) <> 0 :
		szHelp = localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (quantity,  gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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

######## Seasoned Trader Horse Gift and Event Help ###########

def canTriggerHorseGift(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	if player.isNone() :
		return False
	if not player.isPlayable():
		return False
	if city.isNone():
		return False
	city = player.getCity(kTriggeredData.iCityId)
	unit = player.getUnit(kTriggeredData.iUnitId)
	if city.getX() == unit.getX() and city.getY() == unit.getY():
		return True
	return False
	# Read Parameter 1 from the first event and check if enough yield is stored in city
	eEvent1 = gc.getInfoTypeForString("EVENT_SEASONED_TRADER_MEETING_1")
	event1 = gc.getEventInfo(eEvent1)
	iYield = gc.getInfoTypeForString("YIELD_HORSES")
	quantity = event1.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity*2 :
		return False
	return True

def applyHorseGift1(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_HORSES")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < -quantity :
		return
	city.changeYieldStored(iYield, quantity)

def getHelpHorseGift1(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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

getHelpSeasonedTraderNo = get_simple_help("TXT_KEY_EVENT_SEASONED_TRADER_MEETING_HELP")

getHelpSeasonedScoutNativeCity = get_simple_help("TXT_KEY_EVENT_SEASONED_SCOUT_NATIVE_CITY_HELP")

######## Wild Animal ###########

def canApplyWildAnimal1(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	if player.getYieldTradedTotalINT(iYieldID) < quantity:
		return False
	return True

# This is the Function for the Event Target Yield and Target Amount
# This Function is only used for the "Quest Start"
def getHelpQuestStartEuropeTradeYieldAndAmount(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	
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
	if player.getYieldTradedTotalINTAfrica(iYieldID) < quantity:
		return False
	return True

# This is the Function for the Event Target Yield and Target Amount
# This Function is only used for the "Quest Start"
def getHelpQuestStartAfricaTradeYieldAndAmount(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	
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
	if player.getYieldTradedTotalINTPortRoyal(iYieldID) < quantity:
		return False
	return True

# This is the Function for the Event Target Yield and Target Amount
# This Function is only used for the "Quest Start"
def getHelpQuestStartPortRoyalTradeYieldAndAmount(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unitThatTriggered = player.getUnit(kTriggeredData.iUnitId)
	iHostileUnitClassTypeToSpawn = event.getGenericParameter(1)
	iNumHostilesToSpawn = event.getGenericParameter(2)
	for iX in range(iNumHostilesToSpawn):
		unitThatTriggered.spawnBarbarianUnitOnPlotOfUnit(iHostileUnitClassTypeToSpawn)

# adjacent Plot
def spawnBarbarianUnitAdjacentToUnit(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unitThatTriggered = player.getUnit(kTriggeredData.iUnitId)
	iOwnUnitClassTypeToSpawn = event.getGenericParameter(1)
	iNumOwnToSpawn = event.getGenericParameter(2)
	for iX in range(iNumOwnToSpawn):
		unitThatTriggered.spawnOwnPlayerUnitOnPlotOfUnit(iOwnUnitClassTypeToSpawn)

# adjacent Plot
def spawnOwnPlayerUnitAdjacentToUnit(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)

	iHostileUnitClassTypeToSpawn = event.getGenericParameter(1)
	iNumHostilesToSpawn = event.getGenericParameter(2)
	for iX in range(iNumHostilesToSpawn):
		city.spawnBarbarianUnitOnPlotOfCity(iHostileUnitClassTypeToSpawn)

# adjacent Plot
def spawnBarbarianUnitAdjacentToCity(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)

	iOwnUnitClassTypeToSpawn = event.getGenericParameter(1)
	iNumOwnToSpawn = event.getGenericParameter(2)
	for iX in range(iNumOwnToSpawn):
		city.spawnOwnPlayerUnitOnPlotOfCity(iOwnUnitClassTypeToSpawn)

# adjacent Plot
def spawnOwnPlayerUnitAdjacentToCity(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	plotThatTriggered = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	iHostileUnitClassTypeToSpawn = event.getGenericParameter(1)
	iNumHostilesToSpawn = event.getGenericParameter(2)
	for iX in range(iNumHostilesToSpawn):
		plotThatTriggered.spawnBarbarianUnitOnPlot(iHostileUnitClassTypeToSpawn)

# adjacent Plot
def spawnBarbarianUnitAdjacentToPlot(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	ePlayer = kTriggeredData.ePlayer
	plotThatTriggered = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	iOwnUnitClassTypeToSpawn = event.getGenericParameter(1)
	iNumOwnToSpawn = event.getGenericParameter(2)
	for iX in range(iNumOwnToSpawn):
		plotThatTriggered.spawnPlayerUnitOnPlot(ePlayer, iOwnUnitClassTypeToSpawn)

# adjacent Plot
def spawnOwnPlayerUnitAdjacentToPlot(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	ePlayer = kTriggeredData.ePlayer
	plotThatTriggered = gc.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	iOwnUnitClassTypeToSpawn = event.getGenericParameter(1)
	iNumOwnToSpawn = event.getGenericParameter(2)
	for iX in range(iNumOwnToSpawn):
		plotThatTriggered.spawnPlayerUnitOnAdjacentPlot(ePlayer, iOwnUnitClassTypeToSpawn)

######## Native Trader Attack ###########

def canTriggerNativeTraderAttack(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if not player.isPlayable():
		return False
	unit = player.getUnit(kTriggeredData.iUnitId)
	eScout = gc.getInfoTypeForString("PROFESSION_NATIVE_TRADER")
	if unit.getProfession() != eScout:
		return False
	# Read parameter 3 from the event as random chance
	if TriggerChance(argsList):
		return True
	return False

getHelpNativeTraderAttack = get_simple_help("TXT_KEY_EVENT_NATIVE_TRADER_ATTACK_HELP")

######## Criminal Attacks City ###########

def canTriggerCriminalsAttackCity(argsList):
	ePlayer = argsList[1]
	iCity = argsList[2]
	
	player = gc.getPlayer(ePlayer)
	city = player.getCity(iCity)

	if city.isNone():
		return False
	
	if not player.isPlayable():
		return False
	
	iHappiness = city.getCityHappiness()
	iUnhappiness = city.getCityUnHappiness()

	# Happiness Check
	if iHappiness >= iUnhappiness:
		return False

	# Food Check
	eEvent = gc.getInfoTypeForString("EVENT_CRIMINALS_BLACKMAIL_CITY_GIVE")
	event = gc.getEventInfo(eEvent)
	iYield = gc.getInfoTypeForString("YIELD_FOOD")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < quantity :
		return False

	return True

def applyGiveFood(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	if not player.isHuman():
		city = player.firstCity(True)[0]
	iYield = gc.getInfoTypeForString("YIELD_FOOD")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	if city.getYieldStored(iYield) < quantity :
		return False
	city.changeYieldStored(iYield, -quantity)

def getHelpGiveFood(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	city = player.getCity(kTriggeredData.iCityId)
	iYield = gc.getInfoTypeForString("YIELD_FOOD")
	quantity = event.getGenericParameter(1)
	Speed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	quantity = quantity * Speed.getStoragePercent()/100
	szHelp = ""
	if event.getGenericParameter(1) <> 0 :
		szHelp = localText.getText("TXT_KEY_EVENT_YIELD_LOOSE", (-quantity, gc.getYieldInfo(iYield).getChar(), city.getNameKey()))
	return szHelp

getHelpCriminalsAttackCity = get_simple_help("TXT_KEY_EVENT_CRIMINALS_REVOLT_HELP")

######## Officer duel ###########

getHelpOfficerDuel = get_simple_help("TXT_KEY_EVENT_OFFICER_DUEL_HELP")

getHelpOfficerNoDuel = get_simple_help("TXT_KEY_EVENT_OFFICER_NODUEL_HELP")
	
######## Bailiffs search for Architect and attack city ###########

getHelpBailiffsAttackCity = get_simple_help("TXT_KEY_EVENT_ARCHITECT_BAILIFF_HELP")

######## Buccanners attack Silver Mine ###########

getHelpBuccanneersAttackMine = get_simple_help("TXT_KEY_EVENT_BUCCANNERS_ATTACK_MINE_HELP")

getHelpMilitiaDefend = get_simple_help("TXT_KEY_EVENT_MILITIA_DEFENDS_MINE_HELP")

######## Officer arrives at fort ###########

getHelpOfficerAtFort = get_simple_help("TXT_KEY_EVENT_OFFICER_ARRIVAL_AT_FORT_HELP")

######## Discovery Start Event  ###########

getHelpDiscoveryConquistador = get_simple_help("TXT_KEY_EVENT_DISCOVERY_EVENTS_START_CONQUISTADOR_HELP")

getHelpDiscoveryMissionary = get_simple_help("TXT_KEY_EVENT_DISCOVERY_EVENTS_START_MISSIONARY_HELP")

getHelpDiscoveryTrader = get_simple_help("TXT_KEY_EVENT_DISCOVERY_EVENTS_START_SEASONED_TRADER_HELP")

getHelpDiscoveryOxcart = get_simple_help("TXT_KEY_EVENT_DISCOVERY_EVENTS_START_Oxcart_HELP")

getHelpDiscoveryMutiny = get_simple_help("TXT_KEY_EVENT_DISCOVERY_EVENTS_MUTINY_HELP")

getHelpDiscoveryTreasureAttack = get_simple_help("TXT_KEY_EVENT_DISCOVERY_EVENTS_TREASURE_ATTACK_HELP")

getHelpDiscoveryNoNewScout = get_simple_help("TXT_KEY_EVENT_DISCOVERY_EVENTS_NONEW_SCOUT_HELP")

getHelpDiscoveryNewScout = get_simple_help("TXT_KEY_EVENT_DISCOVERY_EVENTS_NEW_SCOUT_HELP")

getHelpDiscoveryFailedTraderChange = get_simple_help("TXT_KEY_EVENT_DISCOVERY_EVENTS_FALIED_TRADER_CHANGE_HELP")

getHelpDiscoveryFailedMissionaryChange = get_simple_help("TXT_KEY_EVENT_DISCOVERY_EVENTS_FALIED_MISSIONARY_CHANGE_HELP")


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

######## Ranger Bear Attack ###########

def canTriggerIsPlayableWithTriggerChance(argsList):
	kTriggeredData = argsList[0]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	if not player.isPlayable():
		return False
	# Read parameter 3 from the event as random chance
	if TriggerChance(argsList):
		return True
	return False

getHelpRangerBearAttack = get_simple_help("TXT_KEY_EVENT_RANGER_BEAR_ATTACK_HELP")

# adjacent Plot for Barbarian, same Plot for own Unit
def spawnBarbarianUnitAdjacentToUnitAndFriendlyOnSamePlot(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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

######## Highwayman Attack ###########

getHelpHighwaymanAttack = get_simple_help("TXT_KEY_EVENT_HIGHWAYMAN_ATTACK_HELP")

######## Land Transport Attack ###########

def spawnNativeUnitAdjacentToUnitAndFriendlyOnSamePlot(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
	player = gc.getPlayer(kTriggeredData.ePlayer)
	unitThatTriggered = player.getUnit(kTriggeredData.iUnitId)
	# This part spawns the Barbarian
	iHostileUnitClassTypeToSpawn = event.getGenericParameter(1)
	iNumHostilesToSpawn = event.getGenericParameter(2)
	for iX in range(iNumHostilesToSpawn):
		unitThatTriggered.spawnBarbarianUnitOnAdjacentPlotOfUnit(iHostileUnitClassTypeToSpawn)
	# This Part spawns the Friendly
	iOwnUnitClassTypeToSpawn = event.getGenericParameter(4)
	iNumOwnToSpawn = event.getGenericParameter(3)
	for iX in range(iNumOwnToSpawn):
		unitThatTriggered.spawnOwnPlayerUnitOnPlotOfUnit(iOwnUnitClassTypeToSpawn)

getHelpLandTransportAttack = get_simple_help("TXT_KEY_EVENT_LANDTRANSPORT_ATTACK_HELP")

######## Milkmaid in Need ###########

hasCattleBonus = has_plot_this_bonus("BONUS_CATTLE")

getHelpMilkmaidInNeed = get_simple_help("TXT_KEY_EVENT_MILKMAID_IN_NEED_HELP")

######## Whale Attack ###########

getHelpWhaleAttack = get_simple_help("TXT_KEY_WHALE_ATTACK_HELP")

######## Pig Herder in Need ###########

hasPigBonus = has_plot_this_bonus("BONUS_PIG")

# adjacent Plot
def spawnBarbarianUnitAdjacentToPlotAndFriendlyOnSamePlot(argsList):
	eEvent = argsList[0]
	event = gc.getEventInfo(eEvent)
	kTriggeredData = argsList[1]
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
getHelpFerryStationRobbers = get_simple_help("TXT_KEY_EVENT_FERRY_STATION_ROBBERS_HELP")

######## Slave and Planation Owner Daughter ###########
getHelpSlaveAndPlanationOwnerDaughter1 = get_simple_help("TXT_KEY_EVENT_SLAVE_AND_PLANATION_OWNER_DAUGHTER_1_HELP")

######## Indentured Servant Steals from Employer ###########
getHelpIndenturedServantStealsFromEmployer = get_simple_help("TXT_KEY_EVENT_INDENTURED_SERVANT_STEALS_FROM_EMPLOYER_1_HELP")



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
