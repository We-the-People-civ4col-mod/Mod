## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
#
# for error reporting
import traceback

# for file ops
import os
import sys
import time

# For Civ game code access
from CvPythonExtensions import *

# For exception handling
SHOWEXCEPTIONS = 1

# for C++ compatibility
false=False
true=True

# globals
gc = CyGlobalContext()
FontIconMap = {}
localText = CyTranslator()

#
# Popup context enums, values greater than 999 are reserved for events
#

# DEBUG TOOLS
PopupTypeEntityEventTest = 4
PopupTypeEffectViewer = 5

# HELP SCREENS
PopupTypeMilitaryAdvisor = 103
PopupTypePlayerSelect = 104

# WORLD BUILDER
PopupTypeWBContextStart = 200
PopupTypeWBEditCity = PopupTypeWBContextStart
PopupTypeWBEditUnit = 201
PopupTypeWBContextEnd	= 299

# EVENT ID VALUES (also used in popup contexts)
EventEditCityName = 5000
EventEditCity = 5001
EventPlaceObject = 5002
EventAwardGold = 5003
EventEditUnitName = 5006
EventCityWarning = 5007
EventWBAllPlotsPopup = 5008
EventWBLandmarkPopup = 5009
EventWBScriptPopup = 5010
EventWBStartYearPopup = 5011
EventShowWonder = 5012
EventCreateTradeRoute = 5013
EventEditTradeRoute = 5014

# Dale - AoD: AoDCheatMenu START
EventAoDCheatMenu = 6000
# Dale - AoD: AoDCheatMenu END

# EuropeScreen START
EventDoEuropeScreen = 5995
# EuropeScreen END

EventLButtonDown=1
EventLcButtonDblClick=2
EventRButtonDown=3
EventBack=4
EventForward=5
EventKeyDown=6
EventKeyUp=7

# List of unreported Events
# Dale - AoD: AoDCheatMenu START
SilentEvents = [EventEditCityName, EventEditUnitName, EventCreateTradeRoute, EventEditTradeRoute, EventAoDCheatMenu, EventDoEuropeScreen]
# Dale - AoD: AoDCheatMenu END

# Popup defines (TODO: Expose these from C++)
FONT_CENTER_JUSTIFY=1<<2
FONT_RIGHT_JUSTIFY=1<<1
FONT_LEFT_JUSTIFY=1<<0

def convertToUnicode(s):
	"if the string is non unicode, convert it to unicode by decoding it using 8859-1, latin_1"
	if (isinstance(s, str)):
		return s.decode("latin_1")
	return s

def convertToStr(s):
	"if the string is unicode, convert it to str by encoding it using 8859-1, latin_1"
	if (isinstance(s, unicode)):
		return s.encode("latin_1")
	return s

class Profile:
	def __init__(self):
		self.timeStack = []		
	
	def reset(self):
		self.timeStack = []

	def startTime(self):
		fTime = time.clock()
		self.timeStack.append(fTime)
	
	def endTime(self, text):
		fEndTime = time.clock()
		fStartTime = self.timeStack.pop(len(self.timeStack) - 1)
		strIndent = ""
		for i in range(len(self.timeStack) - 1):
			strIndent += "  "
		print "%s%s: %.3fs" % (strIndent, text, fEndTime - fStartTime)
		
class RedirectDebug:
	"""Send Debug Messages to Civ Engine"""
	def __init__(self):
		self.m_PythonMgr = CyPythonMgr()
	def write(self, stuff):
		# if str is non unicode and contains encoded unicode data, supply the right encoder to encode it into a unicode object
		if (isinstance(stuff, unicode)):
			self.m_PythonMgr.debugMsgWide(stuff)
		else:
			self.m_PythonMgr.debugMsg(stuff)

class RedirectError:
	"""Send Error Messages to Civ Engine"""
	def __init__(self):
		self.m_PythonMgr = CyPythonMgr()
	def write(self, stuff):
		# if str is non unicode and contains encoded unicode data, supply the right encoder to encode it into a unicode object
		if (isinstance(stuff, unicode)):
			self.m_PythonMgr.errorMsgWide(stuff)
		else:
			self.m_PythonMgr.errorMsg(stuff)

def myExceptHook(type, value, tb):
	lines=traceback.format_exception(type, value, tb)
	#pre= "---------------------Traceback lines-----------------------\n"
	mid="\n".join(lines)
	#post="-----------------------------------------------------------"
	#total = pre+mid+post
	total=mid
	if SHOWEXCEPTIONS:
		sys.stderr.write(total)
	else:
		sys.stdout.write(total)

def pyPrint(stuff):
	stuff = 'PY:' + stuff + "\n"
	sys.stdout.write(stuff)

def pyAssert(cond, msg):
	if (cond==False):
		sys.stderr.write(msg)
	assert(cond, msg)

def getScoreComponent(iRawScore, iInitial, iMax, iFactor, bExponential, bFinal, bVictory):

	if gc.getGame().getEstimateEndTurn() == 0:
		return 0

	if bFinal and bVictory:
		fTurnRatio = float(gc.getGame().getGameTurn()) / float(gc.getGame().getEstimateEndTurn())
		if bExponential and (iInitial != 0):
			fRatio = iMax / iInitial
			iMax = iInitial * pow(fRatio, fTurnRatio)
		else:
			iMax = iInitial + fTurnRatio * (iMax - iInitial)

	iFree = (gc.getDefineINT("SCORE_FREE_PERCENT") * iMax) / 100
	if (iFree + iMax) != 0:
		iScore = (iFactor * (iRawScore + iFree)) / (iFree + iMax)
	else:
		iScore = iFactor

	if bVictory:
		iScore = ((100 + gc.getDefineINT("SCORE_VICTORY_PERCENT")) * iScore) / 100

	if bFinal:
		iScore = ((100 + gc.getDefineINT("SCORE_HANDICAP_PERCENT_OFFSET") + (gc.getGame().getHandicapType() * gc.getDefineINT("SCORE_HANDICAP_PERCENT_PER"))) * iScore) / 100

	return int(iScore)

def shuffle(num, rand):
	"returns a tuple of size num of shuffled numbers"
	piShuffle = [0]*num
	shuffleList(num, rand, piShuffle)	# implemented in C for speed
	return piShuffle

def findInfoTypeNum(typeStr):
	if (typeStr == 'NONE'):
		return -1
	idx = gc.getInfoTypeForString(typeStr)
	pyAssert(idx != -1, "Can't find type enum for type tag %s" %(typeStr,))
	return idx

def AdjustBuilding(add, all, BuildingIdx, pCity): # adds/removes buildings from a city
	"Function for toggling buildings in cities"
	if (BuildingIdx!= -1):
		if (all):                #Add/Remove ALL
			for i in range(BuildingIdx):
				pCity.setHasRealBuilding(i,add)
		else:
			pCity.setHasRealBuilding(BuildingIdx,add)
	return 0

def getIcon(iconEntry):						# returns Font Icons
	global FontIconMap

	iconEntry = iconEntry.lower()
	if (FontIconMap.has_key(iconEntry)):
		return 	FontIconMap.get(iconEntry)
	else:
		return (u"%c" %(191,))

def combatDetailMessageBuilder(cdUnit, ePlayer, iChange):
	if (cdUnit.iExtraCombatPercent != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_EXTRA_COMBAT_PERCENT",(cdUnit.iExtraCombatPercent * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

	if (cdUnit.iNativeCombatModifierTB != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_NATIVE_COMBAT",(cdUnit.iNativeCombatModifierTB * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

	if (cdUnit.iNativeCombatModifierAB != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_NATIVE_COMBAT",(cdUnit.iNativeCombatModifierAB * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

	if (cdUnit.iPlotDefenseModifier != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_PLOT_DEFENSE",(cdUnit.iPlotDefenseModifier * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

	if (cdUnit.iFortifyModifier != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_FORTIFY",(cdUnit.iFortifyModifier * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

	if (cdUnit.iCityDefenseModifier != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_CITY_DEFENSE",(cdUnit.iCityDefenseModifier * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

	if (cdUnit.iHillsAttackModifier != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_HILLS_ATTACK",(cdUnit.iHillsAttackModifier * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

	if (cdUnit.iHillsDefenseModifier != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_HILLS",(cdUnit.iHillsDefenseModifier * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

	if (cdUnit.iFeatureAttackModifier != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_FEATURE_ATTACK",(cdUnit.iFeatureAttackModifier * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

	if (cdUnit.iFeatureDefenseModifier != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_FEATURE",(cdUnit.iFeatureDefenseModifier * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

	if (cdUnit.iTerrainAttackModifier != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_TERRAIN_ATTACK",(cdUnit.iTerrainAttackModifier * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

	if (cdUnit.iTerrainDefenseModifier != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_TERRAIN",(cdUnit.iTerrainDefenseModifier * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

	if (cdUnit.iCityAttackModifier != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_CITY_ATTACK",(cdUnit.iCityAttackModifier * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

	if (cdUnit.iDomainDefenseModifier != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_CITY_DOMAIN_DEFENSE",(cdUnit.iDomainDefenseModifier * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

	if (cdUnit.iClassDefenseModifier != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_CLASS_DEFENSE",(cdUnit.iClassDefenseModifier * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

	if (cdUnit.iClassAttackModifier != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_CLASS_ATTACK",(cdUnit.iClassAttackModifier * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

	if (cdUnit.iCombatModifierT != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_CLASS_COMBAT",(cdUnit.iCombatModifierT * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

	if (cdUnit.iCombatModifierA != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_CLASS_COMBAT",(cdUnit.iCombatModifierA * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

	if (cdUnit.iDomainModifierA != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_CLASS_DOMAIN",(cdUnit.iDomainModifierA * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

	if (cdUnit.iDomainModifierT != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_CLASS_DOMAIN",(cdUnit.iDomainModifierT * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

	if (cdUnit.iRiverAttackModifier != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_CLASS_RIVER_ATTACK",(cdUnit.iRiverAttackModifier * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

	if (cdUnit.iAmphibAttackModifier != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_CLASS_AMPHIB_ATTACK",(cdUnit.iAmphibAttackModifier * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

	if (cdUnit.iRebelPercentModifier != 0):
		msg=localText.getText("TXT_KEY_COMBAT_MESSAGE_CLASS_REBEL_SENTIMENT",(cdUnit.iRebelPercentModifier * iChange,))
		CyInterface().addCombatMessage(ePlayer,msg)

def combatMessageBuilder(cdAttacker, cdDefender, iCombatOdds):

	if (cdAttacker.eVisualOwner != PlayerTypes.UNKNOWN_PLAYER):
		combatMessage = "%s's %s (%.2f)" %(gc.getPlayer(cdAttacker.eVisualOwner).getName(),cdAttacker.sUnitName,cdAttacker.iCurrCombatStr/100.0,)
	else:
		combatMessage = "%s (%.2f)" %(cdAttacker.sUnitName,cdAttacker.iCurrCombatStr/100.0,)
	combatMessage += " " + localText.getText("TXT_KEY_COMBAT_MESSAGE_VS", ()) + " "
	if (cdDefender.eVisualOwner != PlayerTypes.UNKNOWN_PLAYER):
		combatMessage += "%s's %s (%.2f)" %(gc.getPlayer(cdDefender.eOwner).getName(),cdDefender.sUnitName,cdDefender.iCurrCombatStr/100.0,)
	else:
		combatMessage += "%s (%.2f)" %(cdDefender.sUnitName,cdDefender.iCurrCombatStr/100.0,)
	CyInterface().addCombatMessage(cdAttacker.eOwner,combatMessage)
	CyInterface().addCombatMessage(cdDefender.eOwner,combatMessage)
	combatMessage = "%s %.1f%%" %(localText.getText("TXT_KEY_COMBAT_MESSAGE_ODDS", ()),iCombatOdds/10.0,)
	CyInterface().addCombatMessage(cdAttacker.eOwner,combatMessage)
	CyInterface().addCombatMessage(cdDefender.eOwner,combatMessage)
	combatDetailMessageBuilder(cdAttacker,cdAttacker.eOwner,-1)
	combatDetailMessageBuilder(cdDefender,cdAttacker.eOwner,1)
	combatDetailMessageBuilder(cdAttacker,cdDefender.eOwner,-1)
	combatDetailMessageBuilder(cdDefender,cdDefender.eOwner,1)

def initDynamicFontIcons():
	global FontIconMap

	info = ""
	desc = ""
	# add Yield Icons
	for i in range(YieldTypes.NUM_YIELD_TYPES):
		info = gc.getYieldInfo(i)
		desc = info.getDescription().lower()
		addIconToMap(info.getChar, desc)
	for key in OtherFontIcons.keys():
		#print key
		FontIconMap[key] = (u"%c" % CyGame().getSymbolID(OtherFontIcons.get(key)))

	#print FontIconMap

def addIconToMap(infoChar, desc):
	global FontIconMap
	desc = convertToStr(desc)
	print "%s - %s" %(infoChar(), desc)
	uc = infoChar()
	if (uc>=0):
		FontIconMap[desc] = u"%c" %(uc,)

OtherFontIcons = { 'happy' : FontSymbols.HAPPY_CHAR,
				'unhappy' : FontSymbols.UNHAPPY_CHAR,
				'healthy' : FontSymbols.HEALTHY_CHAR,
				'unhealthy' : FontSymbols.UNHEALTHY_CHAR,
				'bullet' : FontSymbols.BULLET_CHAR,
				'strength' : FontSymbols.STRENGTH_CHAR,
				'moves' : FontSymbols.MOVES_CHAR,
				'religion' : FontSymbols.RELIGION_CHAR,
				'star' : FontSymbols.STAR_CHAR,
				'silver star' : FontSymbols.SILVER_STAR_CHAR,
				'trade' : FontSymbols.TRADE_CHAR,
				'defense' : FontSymbols.DEFENSE_CHAR,
				'greatpeople' : FontSymbols.GREAT_PEOPLE_CHAR,
				'badgold' : FontSymbols.BAD_GOLD_CHAR,
				'badfood' : FontSymbols.BAD_FOOD_CHAR,
				'eatenfood' : FontSymbols.EATEN_FOOD_CHAR,
				'goldenage' : FontSymbols.GOLDEN_AGE_CHAR,
				'angrypop' : FontSymbols.ANGRY_POP_CHAR,
				'openBorders' : FontSymbols.OPEN_BORDERS_CHAR,
				'defensivePact' : FontSymbols.DEFENSIVE_PACT_CHAR,
				'map' : FontSymbols.MAP_CHAR,
				'occupation' : FontSymbols.OCCUPATION_CHAR,
				'power' : FontSymbols.POWER_CHAR,
				'gold' : FontSymbols.GOLD_CHAR,
				'rebel' : FontSymbols.REBEL_CHAR,
				# TAC - Trade Routes Advisor - koma13 - START
				'checkbox' : FontSymbols.CHECKBOX_CHAR,
				'checkboxSelected' : FontSymbols.CHECKBOX_SELECTED_CHAR,
				'anchor' : FontSymbols.ANCHOR_CHAR,
				'anchorEurope' : FontSymbols.ANCHOR_EUROPE_CHAR,
				'export' : FontSymbols.EXPORT_CHAR,
				'import' : FontSymbols.IMPORT_CHAR,
				'exportImport' : FontSymbols.EXPORT_IMPORT_CHAR,
				'noAnchor' : FontSymbols.NO_ANCHOR_CHAR,
				# TAC - Trade Routes Advisor - koma13 - END
				}

GlobalInfosMap = {	'bonus': {'NUM': gc.getNumBonusInfos, 'GET': gc.getBonusInfo},
					'improvement': {'NUM': gc.getNumImprovementInfos, 'GET': gc.getImprovementInfo},
					'yield': {'NUM': YieldTypes.NUM_YIELD_TYPES, 'GET': gc.getYieldInfo},
					'unit': {'NUM': gc.getNumUnitInfos, 'GET': gc.getUnitInfo},
					'civic': {'NUM': gc.getNumCivicInfos, 'GET': gc.getCivicInfo},
					'building': {'NUM': gc.getNumBuildingInfos, 'GET': gc.getBuildingInfo},
					'terrain': {'NUM': gc.getNumTerrainInfos, 'GET': gc.getTerrainInfo},
					'trait': {'NUM': gc.getNumTraitInfos, 'GET': gc.getTraitInfo},
					'feature' : {'NUM': gc.getNumFeatureInfos, 'GET': gc.getFeatureInfo},
					'route': {'NUM': gc.getNumRouteInfos, 'GET': gc.getRouteInfo},
					'promotion': {'NUM':gc.getNumPromotionInfos, 'GET': gc.getPromotionInfo},
				}