## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
##
## #####   WARNING - MODIFYING THE FUNCTION NAMES OF THIS FILE IS PROHIBITED  #####
##
## The app specifically calls the functions as they are named. Use this file to pass
## args to another file that contains your modifications
##
## MODDERS - If you create a GameUtils file, update the CvGameInterfaceFile reference to point to your new file

#
import CvUtil
import CvGameUtils
import CvGameInterfaceFile
import CvEventInterface
from CvPythonExtensions import *

# globals
gc = CyGlobalContext()
normalGameUtils = CvGameInterfaceFile.GameUtils

def gameUtils():
	' replace normalGameUtils with your mod version'
	return normalGameUtils

def isVictoryTest():
	#CvUtil.pyPrint( "CvGameInterface.isVictoryTest" )
	return gameUtils().isVictoryTest()

def isVictory(argsList):
	return gameUtils().isVictory(argsList)

def skipProductionPopup(argsList):
	#CvUtil.pyPrint( "CvGameInterface.skipProductionPopup" )
	return gameUtils().skipProductionPopup(argsList)

def canRazeCity(argsList):
	#CvUtil.pyPrint( "CvGameInterface.canRazeCity" )
	return gameUtils().canRazeCity(argsList)

def canDeclareWar(argsList):
	#CvUtil.pyPrint( "CvGameInterface.canRazeCity" )
	return gameUtils().canDeclareWar(argsList)

def showExamineCityButton(argsList):
	#CvUtil.pyPrint( "CvGameInterface.showExamineCityButton" )
	return gameUtils().showExamineCityButton(argsList)

def getRecommendedUnit(argsList):
	#CvUtil.pyPrint( "CvGameInterface.getRecommendedUnit" )
	return gameUtils().getRecommendedUnit(argsList)

def getRecommendedBuilding(argsList):
	#CvUtil.pyPrint( "CvGameInterface.getRecommendedBuilding" )
	return gameUtils().getRecommendedBuilding(argsList)

def updateColoredPlots():
	#CvUtil.pyPrint( "CvGameInterface.updateColoredPlots" )
	return gameUtils().updateColoredPlots()

def isActionRecommended(argsList):
	#CvUtil.pyPrint( "CvGameInterface.isActionRecommended" )
	return gameUtils().isActionRecommended(argsList)

def unitCannotMoveInto(argsList):
	return gameUtils().unitCannotMoveInto(argsList)

def cannotHandleAction(argsList):
	#CvUtil.pyPrint( "CvGameInterface.cannotHandleAction" )
	return gameUtils().cannotHandleAction(argsList)

def canBuild(argsList):
	#CvUtil.pyPrint( "CvGameInterface.canBuild" )
	return gameUtils().canBuild(argsList)

def cannotSelectionListMove(argsList):
	#CvUtil.pyPrint( "CvGameInterface.cannotSelectionListMove" )
	return gameUtils().cannotSelectionListMove(argsList)

def cannotSelectionListGameNetMessage(argsList):
	#CvUtil.pyPrint( "CvGameInterface.cannotSelectionListGameNetMessage" )
	return gameUtils().cannotSelectionListGameNetMessage(argsList)

def cannotDoControl(argsList):
	#CvUtil.pyPrint( "CvGameInterface.cannotDoControl" )
	return gameUtils().cannotDoControl(argsList)
def canDoCivic(argsList):
	#CvUtil.pyPrint( "CvGameInterface.canDoCivic" )
	return gameUtils().canDoCivic(argsList)

def cannotDoCivic(argsList):
	#CvUtil.pyPrint( "CvGameInterface.cannotDoCivic" )
	return gameUtils().cannotDoCivic(argsList)

def canTrain(argsList):
	#CvUtil.pyPrint( "CvGameInterface.canTrain" )
	return gameUtils().canTrain(argsList)

def cannotTrain(argsList):
	#CvUtil.pyPrint( "CvGameInterface.cannotTrain" )
	return gameUtils().cannotTrain(argsList)

def canConstruct(argsList):
	#CvUtil.pyPrint( "CvGameInterface.canConstruct" )
	return gameUtils().canConstruct(argsList)

def cannotConstruct(argsList):
	#CvUtil.pyPrint( "CvGameInterface.cannotConstruct" )
	return gameUtils().cannotConstruct(argsList)

def AI_chooseProduction(argsList):
	'AI chooses city production'
	#CvUtil.pyPrint( "CvGameInterface.AI_chooseProduction" )
	return gameUtils().AI_chooseProduction(argsList)

def AI_unitUpdate(argsList):
	'AI moves units - return 0 to let AI handle it, return 1 to say that the move is handled in python '
	#CvUtil.pyPrint( "CvGameInterface.AI_unitUpdate" )
	return gameUtils().AI_unitUpdate(argsList)

def AI_doWar(argsList):
	'AI decides whether to make war or peace - return 0 to let AI handle it, return 1 to say that the move is handled in python '
	#CvUtil.pyPrint( "CvGameInterface.AI_doWar" )
	return gameUtils().AI_doWar(argsList)

def AI_doDiplo(argsList):
	'AI decides does diplomacy for the turn - return 0 to let AI handle it, return 1 to say that the move is handled in python '
	#CvUtil.pyPrint( "CvGameInterface.AI_doDiplo" )
	return gameUtils().AI_doDiplo(argsList)

def calculateScore(argsList):
	return gameUtils().calculateScore(argsList)

def doGold(argsList):
	#CvUtil.pyPrint( "CvGameInterface.doGold" )
	return gameUtils().doGold(argsList)
def doGoody(argsList):
	#CvUtil.pyPrint( "CvGameInterface.doGoody" )
	return gameUtils().doGoody(argsList)

def doGrowth(argsList):
	#CvUtil.pyPrint( "CvGameInterface.doGrowth" )
	return gameUtils().doGrowth(argsList)

def doProduction(argsList):
	#CvUtil.pyPrint( "CvGameInterface.doProduction" )
	return gameUtils().doProduction(argsList)

def doCulture(argsList):
	#CvUtil.pyPrint( "CvGameInterface.doCulture" )
	return gameUtils().doCulture(argsList)

def doPlotCulture(argsList):
	#CvUtil.pyPrint( "CvGameInterface.doPlotCulture" )
	return gameUtils().doPlotCulture(argsList)

def doReviveActivePlayer(argsList):
	return gameUtils().doReviveActivePlayer(argsList)

def doPillageGold(argsList):
	return gameUtils().doPillageGold(argsList)

def doCityCaptureGold(argsList):
	return gameUtils().doCityCaptureGold(argsList)

def citiesDestroyFeatures(argsList):
	return gameUtils().citiesDestroyFeatures(argsList)

def canFoundCitiesOnWater(argsList):
	return gameUtils().canFoundCitiesOnWater(argsList)

def doCombat(argsList):
	return gameUtils().doCombat(argsList)

def getCityFoundValue(argsList):
	return gameUtils().getCityFoundValue(argsList)

def canPickPlot(argsList):
	return gameUtils().canPickPlot(argsList)

def getUnitCostMod(argsList):
	return gameUtils().getUnitCostMod(argsList)

def getBuildingCostMod(argsList):
	return gameUtils().getBuildingCostMod(argsList)

def canUpgradeAnywhere(argsList):
	return gameUtils().canUpgradeAnywhere(argsList)

def getUpgradePriceOverride(argsList):
	return gameUtils().getUpgradePriceOverride(argsList)

def getExperienceNeeded(argsList):
	return gameUtils().getExperienceNeeded(argsList)