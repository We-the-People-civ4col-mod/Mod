## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
import CvMainInterface
import CvDomesticAdvisor
import CvFoundingFatherScreen
import CvForeignAdvisor
import CvCongressAdvisor
# Achievements START
import CvAchieveAdvisor
# Achievements END
# TAC - Trade Route Advisor - koma13 - START
import CvTradeRoutesAdvisor
# TAC - Trade Route Advisor - koma13 - END

import CvRevolutionAdvisor
import CvMilitaryAdvisor
import CvEuropeScreen
import CvVictoryScreen
##TRIANGLETRADE 10/31/08 by DPII
import CvAfricaScreen
######################
#R&R, ray, Port Royal
import CvPortRoyalScreen
#R&R, ray, Port Royal- END

import CvOptionsScreen
import CvReplayScreen
import CvHallOfFameScreen

import CvDawnOfMan
import CvInfoScreen

import CvIntroMovieScreen
import CvVictoryMovieScreen
import CvWonderMovieScreen

import CvPediaMain
import CvPediaHistory

import CvWorldBuilderScreen
import CvWorldBuilderDiplomacyScreen

import xmlEditorScreen

import CvDebugTools

import CvUtil
import CvEventInterface
import CvPopupInterface
import CvScreenUtilsInterface
import ScreenInput as PyScreenInput
from CvScreenEnums import *
from CvPythonExtensions import *

g_bIsScreenActive = -1
gc = CyGlobalContext()

def toggleSetNoScreens():
	global g_bIsScreenActive
	print "SCREEN OFF"
	g_bIsScreenActive = -1

def toggleSetScreenOn(argsList):
	global g_bIsScreenActive
	print "%s SCREEN TURNED ON" %(argsList[0],)
	g_bIsScreenActive = argsList[0]

#diplomacyScreen = CvDiplomacy.CvDiplomacy()

mainInterface = CvMainInterface.CvMainInterface()
def showMainInterface():
	mainInterface.interfaceScreen()

def numPlotListButtons():
	return mainInterface.numPlotListButtons()

fatherScreen = CvFoundingFatherScreen.CvFoundingFatherScreen()
def showFoundingFatherScreen(argsList):
	if (-1 != CyGame().getActivePlayer()):
		fatherScreen.interfaceScreen(argsList[0])

hallOfFameScreen = CvHallOfFameScreen.CvHallOfFameScreen(HALL_OF_FAME)
def showHallOfFame(argsList):
	hallOfFameScreen.interfaceScreen(argsList[0])

optionsScreen = CvOptionsScreen.CvOptionsScreen()
def showOptionsScreen():
	optionsScreen.interfaceScreen()

foreignAdvisor = CvForeignAdvisor.CvForeignAdvisor()
def showForeignAdvisorScreen(argsList):
	if (-1 != CyGame().getActivePlayer()):
		foreignAdvisor.interfaceScreen(argsList[0])

congressAdvisor = CvCongressAdvisor.CvCongressAdvisor()
def showCongressAdvisorScreen(argsList):
	if (-1 != CyGame().getActivePlayer()):
		congressAdvisor.interfaceScreen()

# Achievements START
achieveAdvisor = CvAchieveAdvisor.CvAchieveAdvisor()
def showAchieveAdvisorScreen():
	if (-1 != CyGame().getActivePlayer()):
		achieveAdvisor.interfaceScreen()
# Achievements END

# TAC - Trade Routes Advisor - koma13 - START
tradeRoutesAdvisor = CvTradeRoutesAdvisor.CvTradeRoutesAdvisor()
def showTradeRoutesAdvisor(argsList):
	if (-1 != CyGame().getActivePlayer()):
		tradeRoutesAdvisor.interfaceScreen()
# TAC - Trade Routes Advisor - koma13 - END

revolutionAdvisor = CvRevolutionAdvisor.CvRevolutionAdvisor()
def showRevolutionAdvisorScreen(argsList):
	if (-1 != CyGame().getActivePlayer()):
		revolutionAdvisor.interfaceScreen()

europeScreen = CvEuropeScreen.CvEuropeScreen()
def showEuropeScreen(argsList):
	if (-1 != CyGame().getActivePlayer()):
		europeScreen.interfaceScreen()

##TRIANGLETRADE 10/24/08 by DPII
africaScreen = CvAfricaScreen.CvAfricaScreen()
def showAfricaScreen(argsList):
	if (-1 != CyGame().getActivePlayer()):
		africaScreen.interfaceScreen()
##TRIANGLETRADE END

#R&R, ray, Port Royal
portroyalScreen = CvPortRoyalScreen.CvPortRoyalScreen()
def showPortRoyalScreen(argsList):
	if (-1 != CyGame().getActivePlayer()):
		portroyalScreen.interfaceScreen()
#R&R, ray, Port Royal- END

domesticAdvisor = CvDomesticAdvisor.CvDomesticAdvisor()
def showDomesticAdvisor(argsList):
	if (-1 != CyGame().getActivePlayer()):
		domesticAdvisor.interfaceScreen()

militaryAdvisor = CvMilitaryAdvisor.CvMilitaryAdvisor(MILITARY_ADVISOR)
def showMilitaryAdvisor(argsList):
	if (-1 != CyGame().getActivePlayer()):
		militaryAdvisor.interfaceScreen()

dawnOfMan = CvDawnOfMan.CvDawnOfMan(DAWN_OF_MAN)
def showDawnOfMan(argsList):
	dawnOfMan.interfaceScreen()

introMovie = CvIntroMovieScreen.CvIntroMovieScreen()
def showIntroMovie(argsList):
	introMovie.interfaceScreen()

victoryMovie = CvVictoryMovieScreen.CvVictoryMovieScreen()
def showVictoryMovie(argsList):
	victoryMovie.interfaceScreen(argsList[0])

wonderMovie = CvWonderMovieScreen.CvWonderMovieScreen()
def showWonderMovie(argsList):
	wonderMovie.interfaceScreen(argsList[0], argsList[1])

replayScreen = CvReplayScreen.CvReplayScreen(REPLAY_SCREEN)
def showReplay(argsList):
	if argsList[0] > -1:
		CyGame().saveReplay(argsList[0])
	replayScreen.showScreen(argsList[4])

infoScreen = CvInfoScreen.CvInfoScreen(INFO_SCREEN)
def showInfoScreen(argsList):
	if (-1 != CyGame().getActivePlayer()):
		iTabID = argsList[0]
		iEndGame = argsList[1]
		infoScreen.showScreen(-1, iTabID, iEndGame)

victoryScreen = CvVictoryScreen.CvVictoryScreen()
def showVictoryScreen(argsList):
	if (-1 != CyGame().getActivePlayer()):
		victoryScreen.interfaceScreen()

### XML editor - start - Nightinggale
xmlEditor = xmlEditorScreen.xmlEditorScreen()
def showXmlEditor():
	xmlEditor.interfaceScreen()

def editorScreenDragOn(argsList):
	xmlEditor.handleDrag(argsList[0], argsList[1], argsList[2], argsList[3])
### XML editor - end - Nightinggale

#################################################
## Civilopedia
#################################################
pediaMainScreen = CvPediaMain.CvPediaMain()
def linkToPedia(argsList):
	pediaMainScreen.link(argsList[0])

def pediaShow():
	return pediaMainScreen.pediaShow()

def pediaBack():
	return pediaMainScreen.back()

def pediaForward():
	pediaMainScreen.forward()

def pediaMain(argsList):
	pediaMainScreen.pediaJump(PEDIA_MAIN, argsList[0], True)

def pediaJumpToUnit(argsList):
	pediaMainScreen.pediaJump(PEDIA_UNIT, argsList[0], True)

def pediaJumpToProfession(argsList):
	pediaMainScreen.pediaJump(PEDIA_PROFESSION, argsList[0], True)

def pediaJumpToBuilding(argsList):
	pediaMainScreen.pediaJump(PEDIA_BUILDING, argsList[0], True)

def pediaJumpToPromotion(argsList):
	pediaMainScreen.pediaJump(PEDIA_PROMOTION, argsList[0], True)

def pediaJumpToBonus(argsList):
	pediaMainScreen.pediaJump(PEDIA_BONUS, argsList[0], True)

def pediaJumpToTerrain(argsList):
	pediaMainScreen.pediaJump(PEDIA_TERRAIN, argsList[0], True)

def pediaJumpToYields(argsList):
	pediaMainScreen.pediaJump(PEDIA_YIELDS, argsList[0], True)

def pediaJumpToFeature(argsList):
	pediaMainScreen.pediaJump(PEDIA_FEATURE, argsList[0], True)

def pediaJumpToFather(argsList):
	pediaMainScreen.pediaJump(PEDIA_FATHER, argsList[0], True)

def pediaJumpToImprovement(argsList):
	pediaMainScreen.pediaJump(PEDIA_IMPROVEMENT, argsList[0], True)

def pediaJumpToCivic(argsList):
	pediaMainScreen.pediaJump(PEDIA_CIVIC, argsList[0], True)

def pediaJumpToCiv(argsList):
	pediaMainScreen.pediaJump(PEDIA_CIVILIZATION, argsList[0], True)

def pediaJumpToLeader(argsList):
	pediaMainScreen.pediaJump(PEDIA_LEADER, argsList[0], True)

def pediaShowHistorical(argsList):
	iEntryId = pediaMainScreen.pediaHistorical.getIdFromEntryInfo(argsList[0], argsList[1])
	pediaMainScreen.pediaJump(PEDIA_HISTORY, iEntryId, True)
	return

#################################################
## Worldbuilder
#################################################
worldBuilderScreen = CvWorldBuilderScreen.CvWorldBuilderScreen()
def getWorldBuilderScreen():
	return worldBuilderScreen

def showWorldBuilderScreen():
	worldBuilderScreen.interfaceScreen()

def hideWorldBuilderScreen():
	worldBuilderScreen.killScreen()

def WorldBuilderToggleUnitEditCB():
	worldBuilderScreen.toggleUnitEditCB()

def WorldBuilderAllPlotsCB():
	CvEventInterface.beginEvent(CvUtil.EventWBAllPlotsPopup)

def WorldBuilderEraseCB():
	worldBuilderScreen.eraseCB()

def WorldBuilderLandmarkCB():
	worldBuilderScreen.landmarkModeCB()

def WorldBuilderExitCB():
	worldBuilderScreen.Exit()

def WorldBuilderToggleCityEditCB():
	worldBuilderScreen.toggleCityEditCB()

def WorldBuilderNormalPlayerTabModeCB():
	worldBuilderScreen.normalPlayerTabModeCB()

def WorldBuilderNormalMapTabModeCB():
	worldBuilderScreen.normalMapTabModeCB()

def WorldBuilderRevealTabModeCB():
	worldBuilderScreen.revealTabModeCB()

def WorldBuilderDiplomacyModeCB():
	worldBuilderScreen.diplomacyModeCB()

def WorldBuilderRevealAllCB():
	worldBuilderScreen.revealAll(True)

def WorldBuilderUnRevealAllCB():
	worldBuilderScreen.revealAll(False)

def WorldBuilderHandleUnitCB( argsList ):
	worldBuilderScreen.handleUnitCB(argsList)

def WorldBuilderHandleTerrainCB( argsList ):
	worldBuilderScreen.handleTerrainCB(argsList)

def WorldBuilderHandleFeatureCB(argsList):
	worldBuilderScreen.handleFeatureCB(argsList)

def WorldBuilderHandleBonusCB( argsList ):
	worldBuilderScreen.handleBonusCB(argsList)

def WorldBuilderHandleImprovementCB(argsList):
	worldBuilderScreen.handleImprovementCB(argsList)

def WorldBuilderHandleTerritoryCB(argsList):
	worldBuilderScreen.handleTerritoryCB(argsList)

def WorldBuilderHandlePlotTypeCB( argsList ):
	worldBuilderScreen.handlePlotTypeCB(argsList)

def WorldBuilderHandleAllPlotsCB( argsList ):
	worldBuilderScreen.handleAllPlotsCB(argsList)

def WorldBuilderHandleUnitEditExperienceCB( argsList ):
	worldBuilderScreen.handleUnitEditExperienceCB(argsList)

def WorldBuilderHandleUnitEditLevelCB( argsList ):
	worldBuilderScreen.handleUnitEditLevelCB(argsList)

def WorldBuilderHandleUnitEditValueCB( argsList ):
	worldBuilderScreen.handleUnitEditValueCB(argsList)

def WorldBuilderHandleUnitEditNameCB( argsList ):
	worldBuilderScreen.handleUnitEditNameCB(argsList)

def WorldBuilderHandleCityEditPopulationCB( argsList ):
	worldBuilderScreen.handleCityEditPopulationCB(argsList)

def WorldBuilderHandleCityEditCultureCB( argsList ):
	worldBuilderScreen.handleCityEditCultureCB(argsList)

def WorldBuilderHandleCityEditGoldCB( argsList ):
	worldBuilderScreen.handleCityEditGoldCB(argsList)

def WorldBuilderHandleCityOccupationTimerCB( argsList ):
	worldBuilderScreen.handleCityOccupationTimerCB(argsList)

def WorldBuilderHandleCityEditAddScriptCB( argsList ):
	worldBuilderScreen.getCityScript()

def WorldBuilderHandleUnitEditAddScriptCB( argsList ):
	worldBuilderScreen.getUnitScript()

def WorldBuilderHandleCityEditNameCB( argsList ):
	worldBuilderScreen.handleCityEditNameCB(argsList)

def WorldBuilderHandleUnitEditPullDownCB( argsList ):
	worldBuilderScreen.handleUnitEditPullDownCB(argsList)

def WorldBuilderHandleUnitAITypeEditPullDownCB( argsList ):
	worldBuilderScreen.handleUnitAITypeEditPullDownCB(argsList)

def WorldBuilderHandleUnitProfessionEditPullDownCB( argsList ):
	worldBuilderScreen.handleUnitProfessionEditPullDownCB(argsList)

def WorldBuilderHandleFlyoutMenuCB( argsList ):
	worldBuilderScreen.handleFlyoutMenuCB(argsList)

def WorldBuilderGetHighlightPlot(argsList):
	return worldBuilderScreen.getHighlightPlot(argsList)

def WorldBuilderOnAdvancedStartBrushSelected(argsList):
	iList,iIndex,iTab = argsList;
	print("WB Advanced Start brush selected, iList=%d, iIndex=%d, type=%d" %(iList,iIndex,iTab))
	if (iTab == worldBuilderScreen.m_iASCityTabID and iList == worldBuilderScreen.m_iASAutomateListID):
		CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_AUTOMATE, worldBuilderScreen.m_iCurrentPlayer, -1, -1, -1, True)
	if (worldBuilderScreen.setCurrentAdvancedStartIndex(iIndex)):
		if (worldBuilderScreen.setCurrentAdvancedStartList(iList)):
			return 1
	return 0

def WorldBuilderOnNormalPlayerBrushSelected(argsList):
	iList,iIndex,iTab = argsList;
	print("WB brush selected, iList=%d, iIndex=%d, type=%d" %(iList,iIndex,iTab))
	if (worldBuilderScreen.setCurrentNormalPlayerIndex(iIndex)):
		return 1
	return 0

def WorldBuilderOnNormalMapBrushSelected(argsList):
	iList,iIndex,iTab = argsList;
	print("WB brush selected, iList=%d, iIndex=%d, type=%d" %(iList,iIndex,iTab))
	if (worldBuilderScreen.setCurrentNormalMapIndex(iIndex)):
		if (worldBuilderScreen.setCurrentNormalMapList(iList)):
			return 1
	return 0

def WorldBuilderOnWBEditBrushSelected(argsList):
	iList,iIndex,iTab = argsList;
	if (worldBuilderScreen.setEditButtonClicked(iIndex)):
		return 1
	return 0

def WorldBuilderOnAllPlotsBrushSelected(argsList):
	if (worldBuilderScreen.handleAllPlotsCB(argsList)):
		return 1
	return 0

def WorldBuilderGetASUnitTabID():
	return worldBuilderScreen.getASUnitTabID()

def WorldBuilderGetASCityTabID():
	return worldBuilderScreen.getASCityTabID()

def WorldBuilderGetASCityListID():
	return worldBuilderScreen.getASCityListID()

def WorldBuilderGetASBuildingsListID():
	return worldBuilderScreen.getASBuildingsListID()

def WorldBuilderGetASAutomateListID():
	return worldBuilderScreen.getASAutomateListID()

def WorldBuilderGetASImprovementsTabID():
	return worldBuilderScreen.getASImprovementsTabID()

def WorldBuilderGetASRoutesListID():
	return worldBuilderScreen.getASRoutesListID()

def WorldBuilderGetASImprovementsListID():
	return worldBuilderScreen.getASImprovementsListID()

def WorldBuilderGetASVisibilityTabID():
	return worldBuilderScreen.getASVisibilityTabID()
def WorldBuilderGetUnitTabID():
	return worldBuilderScreen.getUnitTabID()

def WorldBuilderGetBuildingTabID():
	return worldBuilderScreen.getBuildingTabID()
def WorldBuilderGetImprovementTabID():
	return worldBuilderScreen.getImprovementTabID()

def WorldBuilderGetBonusTabID():
	return worldBuilderScreen.getBonusTabID()

def WorldBuilderGetImprovementListID():
	return worldBuilderScreen.getImprovementListID()

def WorldBuilderGetBonusListID():
	return worldBuilderScreen.getBonusListID()

def WorldBuilderGetTerrainTabID():
	return worldBuilderScreen.getTerrainTabID()

def WorldBuilderGetTerrainListID():
	return worldBuilderScreen.getTerrainListID()

def WorldBuilderGetFeatureListID():
	return worldBuilderScreen.getFeatureListID()

def WorldBuilderGetPlotTypeListID():
	return worldBuilderScreen.getPlotTypeListID()

def WorldBuilderGetRouteListID():
	return worldBuilderScreen.getRouteListID()

def WorldBuilderGetTerritoryTabID():
	return worldBuilderScreen.getTerritoryTabID()

def WorldBuilderGetTerritoryListID():
	return worldBuilderScreen.getTerritoryListID()

def WorldBuilderGetEuropeListID():
	return worldBuilderScreen.getEuropeListID()

def WorldBuilderHasPromotion(argsList):
	iPromotion = argsList[0]
	return worldBuilderScreen.hasPromotion(iPromotion)

def WorldBuilderHasBuilding(argsList):
	iBuilding = argsList[0]
	return worldBuilderScreen.isHasBuilding(iBuilding)

def WorldBuilderHandleDiploPlayerDropdownCB( argsList ):
	worldBuilderScreen.handleDiploPlayerDropdownCB(argsList)

##### WORLDBUILDER DIPLOMACY SCREEN #####

worldBuilderDiplomacyScreen = CvWorldBuilderDiplomacyScreen.CvWorldBuilderDiplomacyScreen()
def showWorldBuilderDiplomacyScreen():
	worldBuilderDiplomacyScreen.interfaceScreen()

def hideWorldBuilderDiplomacyScreen():
	worldBuilderDiplomacyScreen.killScreen()

def handleWorldBuilderDiplomacyPlayerPullDownCB(argsList):
	worldBuilderDiplomacyScreen.handlePlayerPullDownCB(int(argsList[0]))

def handleWorldBuilderDiplomacyAtWarPullDownCB(argsList):
	worldBuilderDiplomacyScreen.handleAtWarPullDownCB(argsList)

def handleWorldBuilderDiplomacyAIWeightPullDownCB(argsList):
	worldBuilderDiplomacyScreen.handleAIWeightPullDownCB(argsList)

def handleWorldBuilderDiplomacyAIWeightResetAllCB(argsList):
	worldBuilderDiplomacyScreen.handleAIWeightResetAll()

def handleWorldBuilderDiplomacyExitCB(argsList):
	worldBuilderDiplomacyScreen.killScreen()

#################################################
## Utility Functions (can be overridden by CvScreenUtilsInterface
#################################################

def movieDone(argsList):
	# allows overides for mods
	if (CvScreenUtilsInterface.getScreenUtils().movieDone(argsList)):
		return

	if (argsList[0] == INTRO_MOVIE_SCREEN):
		introMovie.hideScreen()

	if (argsList[0] == VICTORY_MOVIE_SCREEN):
		victoryMovie.hideScreen()

def leftMouseDown (argsList):
	# allows overides for mods
	if (CvScreenUtilsInterface.getScreenUtils().leftMouseDown(argsList)):
		return

	if ( argsList[0] == WORLDBUILDER_SCREEN ):
		worldBuilderScreen.leftMouseDown(argsList[1:])
		return 1
	return 0

def rightMouseDown (argsList):
	# allows overides for mods
	if (CvScreenUtilsInterface.getScreenUtils().rightMouseDown(argsList)):
		return

	if ( argsList[0] == WORLDBUILDER_SCREEN ):
		worldBuilderScreen.rightMouseDown(argsList)
		return 1
	return 0

def mouseOverPlot (argsList):
	# allows overides for mods
	if (CvScreenUtilsInterface.getScreenUtils().mouseOverPlot(argsList)):
		return

	if (WORLDBUILDER_SCREEN == argsList[0]):
		worldBuilderScreen.mouseOverPlot(argsList)

def handleInput (argsList):
	' handle input is called when a screen is up '
	inputClass = PyScreenInput.ScreenInput(argsList)

	# allows overides for mods
	ret = CvScreenUtilsInterface.getScreenUtils().handleInput( (inputClass.getPythonFile(),inputClass) )

	# get the screen that is active from the HandleInputMap Dictionary
	screen = HandleInputMap.get( inputClass.getPythonFile() )

	# call handle input on that screen
	if ( screen and not ret):
		return screen.handleInput(inputClass)
	return 0

def update (argsList):
	# allows overides for mods
	if (CvScreenUtilsInterface.getScreenUtils().update(argsList)):
		return

	if (HandleInputMap.has_key(argsList[0])):
		screen = HandleInputMap.get(argsList[0])
		screen.update(argsList[1])

def onClose (argsList):
	# allows overides for mods
	if (CvScreenUtilsInterface.getScreenUtils().onClose(argsList)):
		return

	if (HandleCloseMap.has_key(argsList[0])):
		screen = HandleCloseMap.get(argsList[0])
		screen.onClose()

# Forced screen update
def forceScreenUpdate (argsList):
	# allows overides for mods
	if (CvScreenUtilsInterface.getScreenUtils().forceScreenUpdate(argsList)):
		return
	# Main interface Screen
	if ( argsList[0] == MAIN_INTERFACE ):
		mainInterface.updateScreen()
	# world builder Screen
	elif ( argsList[0] == WORLDBUILDER_SCREEN ):
		worldBuilderScreen.updateScreen()
	# world builder diplomacy Screen
	elif ( argsList[0] == WORLDBUILDER_DIPLOMACY_SCREEN ):
		worldBuilderDiplomacyScreen.updateScreen()

# Forced redraw
def forceScreenRedraw (argsList):
	# allows overides for mods
	if (CvScreenUtilsInterface.getScreenUtils().forceScreenRedraw(argsList)):
		return

	# Main Interface Screen
	if ( argsList[0] == MAIN_INTERFACE ):
		mainInterface.redraw()
	elif ( argsList[0] == WORLDBUILDER_SCREEN ):
		worldBuilderScreen.redraw()
	elif ( argsList[0] == WORLDBUILDER_DIPLOMACY_SCREEN ):
		worldBuilderDiplomacyScreen.redraw()

def minimapClicked (argsList):
	# allows overides for mods
	if (CvScreenUtilsInterface.getScreenUtils().minimapClicked(argsList)):
		return

	if (MILITARY_ADVISOR == argsList[0]):
		militaryAdvisor.minimapClicked()
	return

############################################################################
## Misc Functions
############################################################################

def handleBack(screens):
	for iScreen in screens:
		if (HandleNavigationMap.has_key(iScreen)):
			screen = HandleNavigationMap.get( iScreen )
			screen.back()
	return 0

def handleForward(screens):
	for iScreen in screens:
		if (HandleNavigationMap.has_key(iScreen)):
			screen = HandleNavigationMap.get( iScreen )
			screen.forward()
	return 0

def getWidgetHelp(argsList):
	iScreen = argsList[0]
	if (HandleMouseoverMap.has_key(iScreen)):
		screen = HandleMouseoverMap.get(iScreen)
		return screen.getWidgetHelp(argsList)
	return u""

def refreshMilitaryAdvisor (argsList):
	if (1 == argsList[0]):
		militaryAdvisor.refreshSelectedGroup(argsList[1])
	elif (2 == argsList[0]):
		militaryAdvisor.refreshSelectedLeader(argsList[1])
	elif (3 == argsList[0]):
		militaryAdvisor.drawCombatExperience()
	elif (argsList[0] <= 0):
		militaryAdvisor.refreshSelectedUnit(-argsList[0], argsList[1])

def updateMusicPath (argsList):
    szPathName = argsList[0]
    optionsScreen.updateMusicPath(szPathName)

def refreshOptionsScreen():
	optionsScreen.refreshScreen()

def cityWarningOnClickedCallback(argsList):
	iButtonId = argsList[0]
	iData1 = argsList[1]
	iData2 = argsList[2]
	iData3 = argsList[3]
	iData4 = argsList[4]
	szText = argsList[5]
	bOption1 = argsList[6]
	bOption2 = argsList[7]
	city = CyGlobalContext().getPlayer(CyGlobalContext().getGame().getActivePlayer()).getCity(iData1)
	if (not city.isNone()):
		if (iButtonId == 0):
			CyMessageControl().sendPushOrder(iData1, iData2, iData3, False, True, False)
		elif (iButtonId == 2):
			CyInterface().selectCity(city, False)

def cityWarningOnFocusCallback(argsList):
	CyInterface().playGeneralSound("AS2D_ADVISOR_SUGGEST")
	CyInterface().lookAtCityOffset(argsList[0])
	return 0

def liberateOnClickedCallback(argsList):
	iButtonId = argsList[0]
	iData1 = argsList[1]
	iData2 = argsList[2]
	iData3 = argsList[3]
	iData4 = argsList[4]
	szText = argsList[5]
	bOption1 = argsList[6]
	bOption2 = argsList[7]
	city = CyGlobalContext().getPlayer(CyGlobalContext().getGame().getActivePlayer()).getCity(iData1)
	if (not city.isNone()):
		if (iButtonId == 0):
			CyMessageControl().sendDoTask(iData1, TaskTypes.TASK_LIBERATE, 0, -1, False, False, False, False)
		elif (iButtonId == 2):
			CyInterface().selectCity(city, False)

def featAccomplishedOnClickedCallback(argsList):
	iButtonId = argsList[0]
	iData1 = argsList[1]
	iData2 = argsList[2]
	iData3 = argsList[3]
	iData4 = argsList[4]
	szText = argsList[5]
	bOption1 = argsList[6]
	bOption2 = argsList[7]
	
	if iData1 == FeatTypes.FEAT_CITY_MISSING_YIELD:
		if (iButtonId == 1):
			CyMessageControl().sendPlayerAction(CyGlobalContext().getGame().getActivePlayer(), PlayerActionTypes.PLAYER_ACTION_FEAT, iData1, 1, -1);
	elif iData1 == FeatTypes.FEAT_GOTO_EUROPE:
		if (iButtonId == 0):
			CyMessageControl().sendDoCommand(iData2, CommandTypes.COMMAND_SAIL_TO_EUROPE, UnitTravelStates.UNIT_TRAVEL_STATE_TO_EUROPE, -1, False);
		elif (iButtonId == 2):
			CyMessageControl().sendPlayerAction(CyGlobalContext().getGame().getActivePlayer(), PlayerActionTypes.PLAYER_ACTION_FEAT, iData1, 1, -1);
	elif iData1 == FeatTypes.FEAT_EUROPE_SHIPS:
		if (iButtonId == 1):
			showEuropeScreen((-1,))
		elif (iButtonId == 2):
			CyMessageControl().sendPlayerAction(CyGlobalContext().getGame().getActivePlayer(), PlayerActionTypes.PLAYER_ACTION_FEAT, iData1, 1, -1);
	elif iData1 == FeatTypes.FEAT_CITY_SCREEN:
		if (iButtonId == 1):
			CyMessageControl().sendPlayerOption(PlayerOptionTypes.PLAYEROPTION_TUTORIAL, False)

def featAccomplishedOnFocusCallback(argsList):
	iData1 = argsList[0]
	iData2 = argsList[1]
	iData3 = argsList[2]
	iData4 = argsList[3]
	szText = argsList[4]
	bOption1 = argsList[5]
	bOption2 = argsList[6]

	CyInterface().playGeneralSound("AS2D_POSITIVE_DINK")
	if (iData1 == FeatTypes.FEAT_TREASURE or iData1 == FeatTypes.FEAT_TREASURE_IN_PORT):
		CyInterface().lookAtUnit(iData2)

	return 0


#######################################################################################
## Handle Close Map
#######################################################################################
HandleCloseMap = {  DAWN_OF_MAN : dawnOfMan,
					MILITARY_ADVISOR : militaryAdvisor,
				# add new screens here
				# TAC - Trade Routes Advisor - koma13 - START
					TRADE_ROUTES_ADVISOR : tradeRoutesAdvisor,
				# TAC - Trade Routes Advisor - koma13 - END
				}
#######################################################################################
## Handle Mouseover Map
#######################################################################################
HandleMouseoverMap = {  EUROPE_SCREEN : europeScreen,
						MAIN_INTERFACE : mainInterface,
						DOMESTIC_ADVISOR : domesticAdvisor,
						MILITARY_ADVISOR : militaryAdvisor,
						REVOLUTION_ADVISOR : revolutionAdvisor,
						##TRIANGLETRADE 10/24/08 by DPII
						AFRICA_SCREEN : africaScreen,
						######################
						#R&R, ray, Port Royal
						PORT_ROYAL_SCREEN : portroyalScreen,
						#R&R, ray, Port Royal- END
				# add new screens here
# Achievements START
				ACHIEVE_ADVISOR : achieveAdvisor,
# Achievements END
				# TAC - Trade Routes Advisor - koma13 - START
						TRADE_ROUTES_ADVISOR : tradeRoutesAdvisor,
				# TAC - Trade Routes Advisor - koma13 - END
						XML_EDITOR : xmlEditor,
				}
#######################################################################################
## Handle Input Map
#######################################################################################
HandleInputMap = {  MAIN_INTERFACE : mainInterface,
					DOMESTIC_ADVISOR : domesticAdvisor,
					FATHER_SCREEN : fatherScreen,
					FOREIGN_ADVISOR : foreignAdvisor,
					CONGRESS_ADVISOR : congressAdvisor,
					REVOLUTION_ADVISOR : revolutionAdvisor,
					EUROPE_SCREEN : europeScreen,
					MILITARY_ADVISOR : militaryAdvisor,
					DAWN_OF_MAN : dawnOfMan,
					WONDER_MOVIE_SCREEN : wonderMovie,
					INTRO_MOVIE_SCREEN : introMovie,
					OPTIONS_SCREEN : optionsScreen,
					INFO_SCREEN : infoScreen,
					REPLAY_SCREEN : replayScreen,
					VICTORY_SCREEN : victoryScreen,
					HALL_OF_FAME : hallOfFameScreen,
					VICTORY_MOVIE_SCREEN : victoryMovie,
					##TRIANGLETRADE 10/24/08 by DPII
					AFRICA_SCREEN : africaScreen,
					#######################
					#R&R, ray, Port Royal
					PORT_ROYAL_SCREEN : portroyalScreen,
					#R&R, ray, Port Royal- END
					PEDIA_MAIN : pediaMainScreen,
					PEDIA_UNIT : pediaMainScreen,
					PEDIA_BUILDING : pediaMainScreen,
					PEDIA_PROMOTION : pediaMainScreen,
					PEDIA_BONUS : pediaMainScreen,
					PEDIA_IMPROVEMENT : pediaMainScreen,
					PEDIA_TERRAIN : pediaMainScreen,
					PEDIA_YIELDS : pediaMainScreen,
					PEDIA_FEATURE : pediaMainScreen,
					PEDIA_FATHER : pediaMainScreen,
					PEDIA_CIVIC : pediaMainScreen,
					PEDIA_CIVILIZATION : pediaMainScreen,
					PEDIA_LEADER : pediaMainScreen,
					PEDIA_HISTORY : pediaMainScreen,
					WORLDBUILDER_SCREEN : worldBuilderScreen,
					WORLDBUILDER_DIPLOMACY_SCREEN : worldBuilderDiplomacyScreen,

				# add new screens here
# Achievements START
				ACHIEVE_ADVISOR : achieveAdvisor,
# Achievements END
				# TAC - Trade Routes Advisor - koma13 - START
					TRADE_ROUTES_ADVISOR : tradeRoutesAdvisor,
				# TAC - Trade Routes Advisor - koma13 - END
					XML_EDITOR : xmlEditor,
				}

#######################################################################################
## Handle Navigation Map
#######################################################################################
HandleNavigationMap = {
					PEDIA_MAIN : pediaMainScreen,
					PEDIA_UNIT : pediaMainScreen,
					PEDIA_BUILDING : pediaMainScreen,
					PEDIA_PROMOTION : pediaMainScreen,
					PEDIA_BONUS : pediaMainScreen,
					PEDIA_IMPROVEMENT : pediaMainScreen,
					PEDIA_TERRAIN : pediaMainScreen,
					PEDIA_YIELDS : pediaMainScreen,
					PEDIA_FEATURE : pediaMainScreen,
					PEDIA_FATHER : pediaMainScreen,
					PEDIA_CIVIC : pediaMainScreen,
					PEDIA_CIVILIZATION : pediaMainScreen,
					PEDIA_LEADER : pediaMainScreen,
					PEDIA_HISTORY : pediaMainScreen,

				# add new screens here
				}

#TAC: EventTriggerMenu START

def selectOneEvent(argsList):
	iButtonId = argsList[0]
	iData1    = argsList[1]
		
	eventTriggerName = None
	eventTriggerNumber = -1

	if iButtonId == 0:
		return
	else:
		iButtonId = iButtonId - 1
	if iButtonId < gc.getNumEventTriggerInfos():
		eventTriggerName = gc.getEventTriggerInfo(iButtonId).getType()
		eventTriggerNumber = iButtonId
	if eventTriggerName == None:
		return
	if eventTriggerNumber == -1:
		return
	name = eventTriggerName.replace("EVENTTRIGGER_", "").replace("_", " ").title()
	message = 'Event: %s [%d]' % (name, eventTriggerNumber)
	CyInterface().addImmediateMessage(message, "")
	#message = 'pyPrint: You selected Event: %s[%d]' % (eventTriggerName, eventTriggerNumber)
	#CvUtil.pyPrint(message)
	#message = 'print: You selected Event: %s[%d]' % (eventTriggerName, eventTriggerNumber)
	#print message
	pPlayer = gc.getPlayer(iData1)
	pPlayer.trigger(eventTriggerNumber)

#TAC: EventTriggerMenu END
