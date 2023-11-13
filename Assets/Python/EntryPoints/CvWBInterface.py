## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvWBPopups
import CvUtil
import CvWBDesc

# globals
WBDesc = CvWBDesc.CvWBDesc()
lastFileRead = None
gc = CyGlobalContext()

def writeDesc(argsList):
	"Save out a high-level desc of the world, for WorldBuilder"
	fileName = argsList[0]
	global lastFileRead
	lastFileRead=None
	return WBDesc.write(fileName)

def readAndApplyDesc(argsList):
	"Read in and apply a high-level desc of the world.  In-game load only"
	fileName = argsList[0]
	if WBDesc.read(fileName) < 0:
		return -1
	if (WBDesc.applyMap() < 0):
		return -1
	return WBDesc.applyInitialItems()

def readDesc(argsList):
	"Read in a high-level desc of the world, for WorldBuilder.  Must call applyMap and applyInitialItems to finish the process"
	global lastFileRead
	fileName = argsList[0]
	if (fileName!=lastFileRead):
		ret=WBDesc.read(fileName)
		if (ret==0):
			lastFileRead=fileName
	else:
		ret=0
	return ret
def applyMapDesc():
	"After reading, applies the map loaded data"
	return WBDesc.applyMap()
def applyInitialItems():
	"After reading, applies player units, cities"
	return WBDesc.applyInitialItems()

def getAssignedStartingPlots():
	"Reads in starting plots for random players"
	return WBDesc.getAssignedStartingPlots()

def initWBEditor(argsList):
	"Called from the Worldbuilder app - sends to CvWBPopups for handling"
	return CvWBPopups.CvWBPopups().initWB(argsList)

def getGameData():
	"after reading a save file, return game/player data as a tuple"
	t=()
	gameTurn = WBDesc.gameDesc.gameTurn
	maxTurns = WBDesc.gameDesc.maxTurns
	maxCityElimination = WBDesc.gameDesc.maxCityElimination
	numAdvancedStartPoints = WBDesc.gameDesc.numAdvancedStartPoints
	targetScore = WBDesc.gameDesc.targetScore
	worldSizeType = CvUtil.findInfoTypeNum(WBDesc.mapDesc.worldSize)
	climateType = CvUtil.findInfoTypeNum(WBDesc.mapDesc.climate)
	seaLevelType = CvUtil.findInfoTypeNum(WBDesc.mapDesc.seaLevel)
	eraType = CvUtil.findInfoTypeNum(WBDesc.gameDesc.eraType)
	gameSpeedType = CvUtil.findInfoTypeNum(WBDesc.gameDesc.speedType)
	calendarType = CvUtil.findInfoTypeNum(WBDesc.gameDesc.calendarType)

	t=t+(worldSizeType,)
	t=t+(climateType,)
	t=t+(seaLevelType,)
	t=t+(eraType,)
	t=t+(gameSpeedType,)
	t=t+(calendarType,)

	t=t+(len(WBDesc.gameDesc.options),)
	for i in range(len(WBDesc.gameDesc.options)):
		option = CvUtil.findInfoTypeNum(WBDesc.gameDesc.options[i])
		t=t+(option,)

	t=t+(len(WBDesc.gameDesc.mpOptions),)
	for i in range(len(WBDesc.gameDesc.mpOptions)):
		mpOption = CvUtil.findInfoTypeNum(WBDesc.gameDesc.mpOptions[i])
		t=t+(mpOption,)

	t=t+(len(WBDesc.gameDesc.forceControls),)
	for i in range(len(WBDesc.gameDesc.forceControls)):
		forceControl = CvUtil.findInfoTypeNum(WBDesc.gameDesc.forceControls[i])
		t=t+(forceControl,)

	t=t+(len(WBDesc.gameDesc.victories),)
	for i in range(len(WBDesc.gameDesc.victories)):
		victory = CvUtil.findInfoTypeNum(WBDesc.gameDesc.victories[i])
		t=t+(victory,)

	t=t+(gameTurn,)
	t=t+(maxTurns,)
	t=t+(maxCityElimination,)
	t=t+(numAdvancedStartPoints,)
	t=t+(targetScore,)

	return t

def getModPath():
	"Returns the path for the Mod that this scenario should load (if applicable)"
	return (WBDesc.gameDesc.szModPath)

def getMapDescriptionKey():
	"Returns the TXT_KEY Description of the map to be displayed in the map/mod selection screen"
	return (WBDesc.gameDesc.szDescription)

def isRandomMap():
	"If true, this is really a mod, not a scenario"
	return (WBDesc.gameDesc.iRandom)

def getPlayerData():
	"after reading a save file, return player data as a tuple, terminated by -1"
	t=()
	for i in range(gc.getMAX_CIV_PLAYERS()):
		leaderType = CvUtil.findInfoTypeNum(WBDesc.playersDesc[i].leaderType)
		civType = CvUtil.findInfoTypeNum(WBDesc.playersDesc[i].civType)
		handicapType = CvUtil.findInfoTypeNum(WBDesc.playersDesc[i].handicap)
		color = CvUtil.findInfoTypeNum(WBDesc.playersDesc[i].color)
		artStyle = gc.getInfoTypeForString(WBDesc.playersDesc[i].artStyle)

		t=t+(civType,)
		t=t+(WBDesc.playersDesc[i].isPlayableCiv,)
		t=t+(leaderType,)
		t=t+(handicapType,)
		t=t+(WBDesc.playersDesc[i].team,)
		t=t+(color,)
		t=t+(artStyle,)
		t=t+(WBDesc.playersDesc[i].isMinorNationCiv,)

	return t

def getPlayerDesc():
	"after reading a save file, return player description data (wide strings) as a tuple"
	t=()
	for i in range(gc.getMAX_CIV_PLAYERS()):
		t=t+(WBDesc.playersDesc[i].szCivDesc,)
		t=t+(WBDesc.playersDesc[i].szCivShortDesc,)
		t=t+(WBDesc.playersDesc[i].szLeaderName,)
		t=t+(WBDesc.playersDesc[i].szCivAdjective,)

	return t
