## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
#
# Code for saving and loading a high-level description of the world.
# Used by WorldBuilder
#

from CvPythonExtensions import *
import os
import sys
import CvUtil
from array import *

# globals
gc = CyGlobalContext()
version = 11
fileencoding = "cp1252"	# aka "iso-8859-1"

#############
def getPlayer(idx):
	"helper function which wraps get player in case of bad index"
	if (gc.getPlayer(idx).isAlive()):
		return gc.getPlayer(idx)
	return None

#############
class CvWBParser:
	"parser functions for WB desc"
	def getTokens(self, line):
		"return a list of (comma separated) tokens from the line.  Strip whitespace on each token"
		if line==None:
			return list()
		toks=line.split(",")
		toksOut=list()
		for tok in toks:
			toksOut.append(tok.strip())
		return toksOut

	def findToken(self, toks, item):
		"return true if item exists in list of tokens"
		for tok in toks:
			if (tok==item):
				return true
		return false

	def findTokenValue(self, toks, item):
		"Search for a token of the form item=value in the list of toks, and return value, or -1 if not found"
		for tok in toks:
			l=tok.split("=")
			if (item==l[0]):
				if (len(l)==1):
					return item
				return l[1]
		return -1		# failed

	def getNextLine(self, f):
		"return the next line from the list of lines"
		return f.readline()

	def findNextToken(self, f, item):
		"Find the next line that contains the token item, return false if not found"
		while True:
			line = self.getNextLine(f)
			if (not line):
				return false	# EOF
			toks=self.getTokens(line)
			if (self.findToken(toks, item)):
				return true
		return false

	def findNextTokenValue(self, f, item):
		"Find the next line that contains item=value, return value or -1 if not found"
		while True:
			line = self.getNextLine(f)
			if (not line):
				return -1		# EOF
			toks=self.getTokens(line)
			val=self.findTokenValue(toks, item)
			if (val != -1):
				return val
		return -1

#############
class CvGameDesc:
	"class for serializing game data"
	def __init__(self):
		self.eraType = "NONE"
		self.speedType = "NONE"
		self.calendarType = "CALENDAR_DEFAULT"
		self.options = ()
		self.mpOptions = ()
		self.forceControls = ()
		self.victories = ()
		self.gameTurn = 0
		self.maxTurns = 0
		self.maxCityElimination = 0
		self.numAdvancedStartPoints = 0
		self.targetScore = 0
		self.iStartYear = gc.getDefineINT("START_YEAR")
		self.szDescription = ""
		self.szModPath = ""
		self.iRandom = 0
# RR, ray, Correct Geographical Placement of Natives - START
		self.WBNorthAmericanNative = 0
		self.WBSouthAmericanNative = 0
		self.WBCentralAmericanNative = 0
# RR, ray, Correct Geographical Placement of Natives - END

	def apply(self):
		"after reading, apply the game data"
		gc.getGame().setStartYear(self.iStartYear)
# RR, ray, Correct Geographical Placement of Natives - START
		gc.getGame().setWBNorthAmericanNative(self.WBNorthAmericanNative)
		gc.getGame().setWBSouthAmericanNative(self.WBSouthAmericanNative)
		gc.getGame().setWBCentralAmericanNative(self.WBCentralAmericanNative)
# RR, ray, Correct Geographical Placement of Natives - END

	def write(self, f):
		"write out game data"
		f.write("BeginGame\n")
		f.write("\tEra=%s\n" %(gc.getEraInfo(gc.getGame().getStartEra()).getType(),))
		f.write("\tSpeed=%s\n" %(gc.getGameSpeedInfo(gc.getGame().getGameSpeedType()).getType(),))
		f.write("\tCalendar=%s\n" %(gc.getCalendarInfo(gc.getGame().getCalendar()).getType(),))

		# write options
		for i in range(gc.getNumGameOptionInfos()):
			if (gc.getGame().isOption(i)):
				f.write("\tOption=%s\n" %(gc.getGameOptionInfo(i).getType()))

		# write mp options
		for i in range(gc.getNumMPOptionInfos()):
			if (gc.getGame().isMPOption(i)):
				f.write("\tMPOption=%s\n" %(gc.getMPOptionInfo(i).getType()))

		# write force controls
		for i in range(gc.getNumForceControlInfos()):
			if (gc.getGame().isForcedControl(i)):
				f.write("\tForceControl=%s\n" %(gc.getForceControlInfo(i).getType()))

		# write victories
		for i in range(gc.getNumVictoryInfos()):
			if (gc.getGame().isVictoryValid(i)):
				if (not gc.getVictoryInfo(i).isPermanent()):
					f.write("\tVictory=%s\n" %(gc.getVictoryInfo(i).getType()))

		f.write("\tGameTurn=%d\n" %(gc.getGame().getGameTurn(),))
		f.write("\tMaxTurns=%d\n" %(gc.getGame().getMaxTurns(),))
		f.write("\tMaxCityElimination=%d\n" %(gc.getGame().getMaxCityElimination(),))
		f.write("\tNumAdvancedStartPoints=%d\n" %(gc.getGame().getNumAdvancedStartPoints(),))
		f.write("\tTargetScore=%d\n" %(gc.getGame().getTargetScore(),))

		f.write("\tStartYear=%d\n" %(gc.getGame().getStartYear(),))
		f.write("\tDescription=%s\n" % (self.szDescription,))
		f.write("\tModPath=%s\n" % (self.szModPath,))
# RR, ray, Correct Geographical Placement of Natives - START
		f.write("\tWBNorthAmericanNative=%d\n" % (gc.getGame().isWBNorthAmericanNative(),))
		f.write("\tWBSouthAmericanNative=%d\n" % (gc.getGame().isWBSouthAmericanNative(),))
		f.write("\tWBCentralAmericanNative=%d\n" % (gc.getGame().isWBCentralAmericanNative(),))
# RR, ray, Correct Geographical Placement of Natives - END
		f.write("EndGame\n")

	def read(self, f):
		"read in game data"
		self.__init__()

		parser = CvWBParser()
		if (parser.findNextTokenValue(f, "BeginGame")!=-1):
			while (true):
				nextLine = parser.getNextLine(f)
				toks = parser.getTokens(nextLine)
				if (len(toks)==0):
					break

				v = parser.findTokenValue(toks, "Era")
				if v!=-1:
					self.eraType = v
					continue

				v = parser.findTokenValue(toks, "Speed")
				if v!=-1:
					self.speedType = v
					continue

				v = parser.findTokenValue(toks, "Calendar")
				if v!=-1:
					self.calendarType = v
					continue

				v = parser.findTokenValue(toks, "Option")
				if v!=-1:
					self.options = self.options + (v,)
					continue

				v = parser.findTokenValue(toks, "MPOption")
				if v!=-1:
					self.mpOptions = self.mpOptions + (v,)
					continue

				v = parser.findTokenValue(toks, "ForceControl")
				if v!=-1:
					self.forceControls = self.forceControls + (v,)
					continue

				v = parser.findTokenValue(toks, "Victory")
				if v!=-1:
					self.victories = self.victories + (v,)
					continue

				v = parser.findTokenValue(toks, "GameTurn")
				if v!=-1:
					self.gameTurn = int(v)
					continue

				v = parser.findTokenValue(toks, "MaxTurns")
				if v!=-1:
					self.maxTurns = int(v)
					continue

				v = parser.findTokenValue(toks, "MaxCityElimination")
				if v!=-1:
					self.maxCityElimination = int(v)
					continue

				v = parser.findTokenValue(toks, "NumAdvancedStartPoints")
				if v!=-1:
					self.numAdvancedStartPoints = int(v)
					continue

				v = parser.findTokenValue(toks, "TargetScore")
				if v!=-1:
					self.targetScore = int(v)
					continue

				v = parser.findTokenValue(toks, "StartYear")
				if v!=-1:
					self.iStartYear = int(v)
					continue

				v = parser.findTokenValue(toks, "Description")
				if v!=-1:
					self.szDescription = v
					continue

				v = parser.findTokenValue(toks, "ModPath")
				if v!=-1:
					self.szModPath = v
					continue

				v = parser.findTokenValue(toks, "Random")
				if v!=-1:
					self.iRandom = int(v)
					continue

# RR, ray, Correct Geographical Placement of Natives - START
				v = parser.findTokenValue(toks, "WBNorthAmericanNative")
				if v!=-1:
					self.WBNorthAmericanNative = int(v)
					continue

				v = parser.findTokenValue(toks, "WBSouthAmericanNative")
				if v!=-1:
					self.WBSouthAmericanNative = int(v)
					continue
				
				v = parser.findTokenValue(toks, "WBCentralAmericanNative")
				if v!=-1:
					self.WBCentralAmericanNative = int(v)
					continue
# RR, ray, Correct Geographical Placement of Natives - START
				
				if parser.findTokenValue(toks, "EndGame") != -1:
					break

#############
class CvTeamDesc:
	def __init__(self):
		self.techTypes = ()
		self.bContactWithTeamList = ()
		self.bWarWithTeamList = ()
		self.bPermanentWarPeaceList = ()
		self.bOpenBordersWithTeamList = ()
		self.bDefensivePactWithTeamList = ()
		self.bRevealMap = 0

	def write(self, f, idx):
		"write out team data"
		f.write("BeginTeam\n")

		# Team ID (to make things easier to mess with in the text)
		f.write("\tTeamID=%d\n" %(idx))
		# write met other teams
		for i in range(gc.getMAX_CIV_TEAMS()):
			if (gc.getTeam(idx).isHasMet(i)):
				f.write("\tContactWithTeam=%d\n" %(i))

		# write warring teams
		for i in range(gc.getMAX_CIV_TEAMS()):
			if (gc.getTeam(idx).isAtWar(i)):
				f.write("\tAtWar=%d\n" %(i))

		# write permanent war/peace teams
		for i in range(gc.getMAX_CIV_TEAMS()):
			if (gc.getTeam(idx).isPermanentWarPeace(i)):
				f.write("\tPermanentWarPeace=%d\n" %(i))

		# write open borders other teams
		for i in range(gc.getMAX_CIV_TEAMS()):
			if (gc.getTeam(idx).isOpenBorders(i)):
				f.write("\tOpenBordersWithTeam=%d\n" %(i))

		# write defensive pact other teams
		for i in range(gc.getMAX_CIV_TEAMS()):
			if (gc.getTeam(idx).isDefensivePact(i)):
				f.write("\tDefensivePactWithTeam=%d\n" %(i))

		f.write("\tRevealMap=%d\n" %(0))

		f.write("EndTeam\n")

	def read(self, f):
		"read in team data"
		self.__init__()

		parser = CvWBParser()
		if (parser.findNextTokenValue(f, "BeginTeam")!=-1):
			while (true):
				nextLine = parser.getNextLine(f)
				toks = parser.getTokens(nextLine)
				if (len(toks)==0):
					break
				v = parser.findTokenValue(toks, "ContactWithTeam")
				if v!=-1:
					self.bContactWithTeamList = self.bContactWithTeamList + (int(v),)
					continue

				v = parser.findTokenValue(toks, "AtWar")
				if v!=-1:
					self.bWarWithTeamList = self.bWarWithTeamList + (int(v),)
					continue

				v = parser.findTokenValue(toks, "PermanentWarPeace")
				if v!=-1:
					self.bPermanentWarPeaceList = self.bPermanentWarPeaceList + (int(v),)
					continue

				v = parser.findTokenValue(toks, "OpenBordersWithTeam")
				if v!=-1:
					self.bOpenBordersWithTeamList = self.bOpenBordersWithTeamList + (int(v),)
					continue

				v = parser.findTokenValue(toks, "DefensivePactWithTeam")
				if v!=-1:
					self.bDefensivePactWithTeamList = self.bDefensivePactWithTeamList + (int(v),)
					continue

				v = parser.findTokenValue(toks, "RevealMap")
				if v!=-1:
					self.bRevealMap = int(v)
					continue

				if parser.findTokenValue(toks, "EndTeam") != -1:
					return true		# completed successfully

		return false	# failed

#############
class CvPlayerDesc:
	def __init__(self):
		self.szCivDesc = ""
		self.szCivShortDesc = ""
		self.szLeaderName = ""
		self.szCivAdjective = ""

		self.leaderType = "NONE"
		self.civType = "NONE"
		self.handicap = gc.getHandicapInfo(gc.getDefineINT("STANDARD_HANDICAP")).getType()
		self.team = -1		# team index
		self.color = "NONE"
		self.artStyle = "NONE"
		self.isPlayableCiv = 1
		self.isMinorNationCiv = 0
		self.iStartingGold = 0
		self.iStartingX = -1
		self.iStartingY = -1
		self.szStartingEra = ""
		self.bRandomStartLocation = "false"

		self.aaiCivics = []
		self.aaiAttitudeExtras = []
		self.aszCityList = []

	def write(self, f, idx):
		"write out player data"
		f.write("BeginPlayer\n")

		# write team
		f.write("\tTeam=%d\n" %(int(gc.getPlayer(idx).getTeam())))

		# write leader and Civ Description info
		if (gc.getPlayer(idx).getLeaderType() == LeaderHeadTypes.NO_LEADER):
			f.write("\tLeaderType=NONE\n")

		else:
			f.write("\tLeaderType=%s\n" %(gc.getLeaderHeadInfo(gc.getPlayer(idx).getLeaderType()).getType()))

		# write civ, color, artStyle, isPlayableCiv, isMinorNation, StartingGold
		if (gc.getPlayer(idx).getCivilizationType() == CivilizationTypes.NO_CIVILIZATION):
			f.write("\tCivType=NONE\n")
			f.write("\tColor=NONE\n")
			f.write("\tArtStyle=NONE\n")
		else:
			f.write("\tLeaderName=%s\n" %(gc.getPlayer(idx).getNameKey().encode(fileencoding)))
			f.write("\tCivDesc=%s\n" %(gc.getPlayer(idx).getCivilizationDescriptionKey().encode(fileencoding)))
			f.write("\tCivShortDesc=%s\n" %(gc.getPlayer(idx).getCivilizationShortDescriptionKey().encode(fileencoding)))
			f.write("\tCivAdjective=%s\n" %(gc.getPlayer(idx).getCivilizationAdjectiveKey().encode(fileencoding)))
			f.write("\tCivType=%s\n" %(gc.getCivilizationInfo(gc.getPlayer(idx).getCivilizationType()).getType()))
			f.write("\tColor=%s\n" %(gc.getPlayerColorInfo(gc.getPlayer(idx).getPlayerColor()).getType()))
			f.write("\tArtStyle=%s\n" %(gc.getArtStyleTypes(gc.getPlayer(idx).getArtStyleType())))
			f.write("\tPlayableCiv=%d\n" %(int(gc.getPlayer(idx).isPlayable())))
			f.write("\tStartingGold=%d\n" %(gc.getPlayer(idx).getGold()))

			if gc.getPlayer(idx).isAlive():
				pPlot = gc.getPlayer(idx).getStartingPlot()
				if (not pPlot.isNone()):
					f.write("\tStartingX=%d, StartingY=%d\n" %(pPlot.getX(), pPlot.getY()))

			f.write("\tStartingEra=%s\n" %(gc.getEraInfo(gc.getPlayer(idx).getCurrentEra()).getType()))

			f.write("\tRandomStartLocation=false\n")

			# write Civics
			for iCivicOptionLoop in range(gc.getNumCivicOptionInfos()):
				for iCivicLoop in range(gc.getNumCivicInfos()):
					iCivic = gc.getPlayer(idx).getCivic(iCivicOptionLoop)
					if (iCivicLoop == iCivic):
						f.write("\tCivicOption=%s, Civic=%s\n" %(gc.getCivicOptionInfo(iCivicOptionLoop).getType(), gc.getCivicInfo(iCivicLoop).getType()))

			# write Attitude Extra
			for i in range(gc.getMAX_CIV_PLAYERS()):
				if (gc.getPlayer(idx).AI_getAttitudeExtra(i) != 0):
					f.write("\tAttitudePlayer=%d, AttitudeExtra=%d\n" %(i, gc.getPlayer(idx).AI_getAttitudeExtra(i)))

			# write City List
			for i in range(gc.getPlayer(idx).getNumCityNames()):
				#f.write("\tCityList=%s\n" %(gc.getPlayer(idx).getCityName(i)))
				f.write(CyTranslator().getText("\tCityList=%s\n",(gc.getPlayer(idx).getCityName(i),())))

		if (gc.getPlayer(idx).getHandicapType() == HandicapTypes.NO_HANDICAP):
			f.write("\tHandicap=%s\n" %(gc.getHandicapInfo(gc.getDefineINT("STANDARD_HANDICAP")).getType()))
		else:
			f.write("\tHandicap=%s\n" %(gc.getHandicapInfo(gc.getPlayer(idx).getHandicapType()).getType()))

		f.write("EndPlayer\n")

	def read(self, f):
		"read in player data"
		self.__init__()
		parser = CvWBParser()
		if (parser.findNextTokenValue(f, "BeginPlayer")!=-1):
			while (true):
				nextLine = parser.getNextLine(f)
				toks = parser.getTokens(nextLine)
				if (len(toks)==0):
					break

				v = parser.findTokenValue(toks, "CivDesc")
				if v!=-1:
					self.szCivDesc = v.decode(fileencoding)
					continue

				v = parser.findTokenValue(toks, "CivShortDesc")
				if v!=-1:
					self.szCivShortDesc = v.decode(fileencoding)
					continue

				v = parser.findTokenValue(toks, "LeaderName")
				if v!=-1:
					self.szLeaderName = v.decode(fileencoding)
					continue

				v = parser.findTokenValue(toks, "CivAdjective")
				if v!=-1:
					self.szCivAdjective = v.decode(fileencoding)
					continue

				v = parser.findTokenValue(toks, "LeaderType")
				if v!=-1:
					self.leaderType = v
					continue

				v = parser.findTokenValue(toks, "CivType")
				if v!=-1:
					self.civType = v
					continue

				v = parser.findTokenValue(toks, "Team")
				if v!=-1:
					self.team = int(v)
					continue

				v = parser.findTokenValue(toks, "Handicap")
				if v!=-1:
					self.handicap = v
					continue

				v = parser.findTokenValue(toks, "Color")
				if v!=-1:
					self.color = v
					continue

				v = parser.findTokenValue(toks, "ArtStyle")
				if v!=-1:
					self.artStyle = v
					continue

				v = parser.findTokenValue(toks, "PlayableCiv")
				if v!=-1:
					self.isPlayableCiv = int(v)
					continue

				v = parser.findTokenValue(toks, "StartingGold")
				if v!=-1:
					self.iStartingGold = int(v)
					continue

				vX = parser.findTokenValue(toks, "StartingX")
				vY = parser.findTokenValue(toks, "StartingY")
				if vX!=-1 and vY!=-1:
					self.iStartingX = int(vX)
					self.iStartingY = int(vY)
					continue

				v = parser.findTokenValue(toks, "StartingEra")
				if v!=-1:
					self.szStartingEra = v
					continue

				v = parser.findTokenValue(toks, "RandomStartLocation")
				if v!=-1:
					self.bRandomStartLocation = v
					continue

				v = parser.findTokenValue(toks, "CivicOption")
				if v!=-1:
					iCivicOptionType = gc.getInfoTypeForString(v)

					v = parser.findTokenValue(toks, "Civic")
					if v!=-1:
						iCivicType = gc.getInfoTypeForString(v)
						self.aaiCivics.append([iCivicOptionType,iCivicType])
						continue

				v = parser.findTokenValue(toks, "AttitudePlayer")
				if v!=-1:
					iPlayer = int(v)

					iExtra = int(parser.findTokenValue(toks, "AttitudeExtra"))
					self.aaiAttitudeExtras.append([iPlayer,iExtra])
					continue

				v = parser.findTokenValue(toks, "CityList")
				if v!=-1:
					self.aszCityList.append(v)
					continue

				if parser.findTokenValue(toks, "EndPlayer") != -1:
					break

#############
class CvUnitDesc:
	"unit WB serialization"
	def __init__(self):
		self.plotX = -1
		self.plotY = -1
		self.unitType = None
		self.profession = None
		self.leaderUnitType = None
		self.owner =-1
		self.damage = 0
		self.level = -1
		self.experience = -1
		self.promotionType = []
		self.facingDirection = DirectionTypes.NO_DIRECTION;
		self.isSleep = False
		self.szUnitAIType = "NO_UNITAI"
		self.szScriptData = "NONE"
		self.iYieldStored = 0

	def apply(self):
		"after reading, this will actually apply the data"
		player = getPlayer(self.owner)
		if (player):
			CvUtil.pyAssert(self.plotX>=0 and self.plotY>=0, "invalid plot coords")
			unitTypeNum = CvUtil.findInfoTypeNum(self.unitType)
			if (unitTypeNum < 0):
				unit = None
			else:
				if (self.szUnitAIType != "NO_UNITAI"):
					eUnitAI = CvUtil.findInfoTypeNum(self.szUnitAIType)
				else:
					eUnitAI = UnitAITypes.NO_UNITAI

				unit = player.initUnit(unitTypeNum, ProfessionTypes.NO_PROFESSION, self.plotX, self.plotY, UnitAITypes(eUnitAI), self.facingDirection, self.iYieldStored)
				
			if (unit):
				#profession
				if(self.profession != None):
					unit.setProfession(CvUtil.findInfoTypeNum(self.profession))

				#leader unit type
				if(self.leaderUnitType != None):
					leaderUnitTypeNum = CvUtil.findInfoTypeNum(self.leaderUnitType)
					if leaderUnitTypeNum >= 0:
						unit.setLeaderUnitType(leaderUnitTypeNum);

				#other properties
				if self.damage != 0:
					unit.setDamage(self.damage)
				if self.level != -1:
					unit.setLevel(self.level)
				if self.experience != -1:
					unit.setExperience(self.experience, -1)
				for promo in self.promotionType:
					promotionTypeNum = CvUtil.findInfoTypeNum(promo)
					unit.setHasRealPromotion(promotionTypeNum, true)
				if self.isSleep:
					unit.getGroup().setActivityType(ActivityTypes.ACTIVITY_SLEEP)
				if self.szScriptData != "NONE":
					unit.setScriptData(self.szScriptData)

	def read(self, f, pX, pY):
		"read in unit data - at this point the first line 'BeginUnit' has already been read"
		self.__init__()
		self.plotX = pX
		self.plotY = pY
		CvUtil.pyAssert(self.plotX>=0 and self.plotY>=0, "invalid plot coords")

		parser = CvWBParser()
		while (true):
			nextLine = parser.getNextLine(f)
			toks = parser.getTokens(nextLine)
			if (len(toks)==0):
				break

			v = parser.findTokenValue(toks, "UnitType")
			vOwner = parser.findTokenValue(toks, "UnitOwner")
			if (v!=-1 and vOwner != -1):
				self.unitType = v
				self.owner = int(vOwner)
				continue

			v = parser.findTokenValue(toks, "Profession")
			if (v != -1):
				self.profession = v
				continue

			v = parser.findTokenValue(toks, "LeaderUnitType")
			if (v != -1):
				self.leaderUnitType = v
				continue

			v = parser.findTokenValue(toks, "Damage")
			if (v != -1):
				self.damage = (int(v))
				continue

			v = parser.findTokenValue(toks, "Level")
			if (v != -1):
				self.level = (int(v))
				self.experience = int(parser.findTokenValue(toks, "Experience"))
				continue

			# Units - Promotions
			v = parser.findTokenValue(toks, "PromotionType")
			if v!=-1:
				self.promotionType.append(v)
				continue

			v = parser.findTokenValue(toks, "FacingDirection")
			if (v != -1):
				self.facingDirection = (DirectionTypes(v))
				continue

			if (parser.findTokenValue(toks, "Sleep"))!=-1:
				self.isSleep = True
				continue

			v = parser.findTokenValue(toks, "UnitAIType")
			if (v != -1):
				self.szUnitAIType = v
				continue

			v = parser.findTokenValue(toks, "ScriptData")
			if v!=-1:
				print("found script data: %s" %(v))
				self.szScriptData = v
				continue

			v = parser.findTokenValue(toks, "YieldStored")
			if v != -1:
				self.iYieldStored = (int(v))
				continue

			if parser.findTokenValue(toks, "EndUnit") != -1:
				break

	def write(self, f, unit, plot):
		"save unit desc to a file"
		unitType = gc.getUnitInfo(unit.getUnitType()).getType()
		unitOwner= unit.getOwner()
		f.write("\tBeginUnit\n")
		f.write("\t\tUnitType=%s, UnitOwner=%d\n" %(unitType,unitOwner))
		if unit.getProfession() != -1:
			f.write("\t\tProfession=%s\n" %(gc.getProfessionInfo(unit.getProfession()).getType()))
		if unit.getLeaderUnitType() != -1:
			f.write("\t\tLeaderUnitType=%s\n" %(gc.getUnitInfo(unit.getLeaderUnitType()).getType()))
		f.write("\t\tDamage=%d\n" %(unit.getDamage(),))
		f.write("\t\tLevel=%d, Experience=%d\n" %(unit.getLevel(), unit.getExperience()))
		for i in range(gc.getNumPromotionInfos()):
			if unit.isHasRealPromotion(i):
				f.write("\t\tPromotionType=%s\n" %(gc.getPromotionInfo(i).getType()))

		f.write("\t\tFacingDirection=%d\n" %(unit.getFacingDirection(),))
		if (unit.getGroup().getActivityType() == ActivityTypes.ACTIVITY_SLEEP):
			f.write("\t\tSleep\n")
		f.write("\t\tUnitAIType=%s\n" %(gc.getUnitAIInfo(unit.getUnitAIType()).getType()))
		if unit.getScriptData():
			f.write("\t\tScriptData=%s\n" %unit.getScriptData() )
		if unit.getYieldStored() != 0:
			f.write("\t\tYieldStored=%d\n" %(unit.getYieldStored(),))
		f.write("\tEndUnit\n")

############
class CvCityDesc:
	"serializes city data"
	def __init__(self):
		self.city = None
		self.owner = None
		self.name = None
		self.population = 0
		self.productionUnit = "NONE"
		self.productionBuilding = "NONE"
		self.culture = 0
		self.bldgType = []
		self.citizenList = []
		self.plotX=-1
		self.plotY=-1
		self.szScriptData = "NONE"
		self.aiPlayerCulture = []
		for iPlayerLoop in range(gc.getMAX_CIV_PLAYERS()):
			self.aiPlayerCulture.append(0)

	def apply(self):
		"after reading, this will actually apply the data"
		player = getPlayer(self.owner)
		if (player):
			self.city = player.initCity(self.plotX, self.plotY)

		if (self.name != None):
			self.city.setName(self.name, False)

		if (self.culture):
			self.city.setCulture(self.city.getOwner(), self.culture, True)

		for bldg in (self.bldgType):
			bldgTypeNum = CvUtil.findInfoTypeNum(bldg)
			self.city.setHasRealBuilding(bldgTypeNum, true)

		for unit in (self.citizenList):
			unitTypeNum = CvUtil.findInfoTypeNum(unit)
			pUnit = player.initUnit(unitTypeNum, ProfessionTypes.NO_PROFESSION, self.plotX, self.plotY, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION, 0)
			self.city.addPopulationUnit(pUnit, ProfessionTypes.NO_PROFESSION)

		if (self.population):
			self.city.setPopulation(self.population)

		for iPlayerLoop in range(gc.getMAX_CIV_PLAYERS()):
			iPlayerCulture = self.aiPlayerCulture[iPlayerLoop]
			if (iPlayerCulture > 0):
				self.city.setCulture(iPlayerLoop, iPlayerCulture, true)

		unitTypeNum = CvUtil.findInfoTypeNum(self.productionUnit)
		buildingTypeNum = CvUtil.findInfoTypeNum(self.productionBuilding)

		if (unitTypeNum != UnitTypes.NO_UNIT):
			self.city.pushOrder(OrderTypes.ORDER_TRAIN, unitTypeNum, -1, False, False, False, True)
		elif (buildingTypeNum != BuildingTypes.NO_BUILDING):
			self.city.pushOrder(OrderTypes.ORDER_CONSTRUCT, buildingTypeNum, -1, False, False, False, True)

		if (self.szScriptData != "NONE"):
			self.city.setScriptData(self.szScriptData)

	def write(self, f, plot):
		"write out city data"
		city = plot.getPlotCity()
		CvUtil.pyAssert(city.isNone()==0, "null city?")
		f.write("\tBeginCity\n")
		f.write("\t\tCityOwner=%d\n" %(city.getOwner(),))
		f.write("\t\tCityName=%s\n" %(city.getNameKey().encode(fileencoding),))
		f.write("\t\tCityPopulation=%d\n" %(city.getPopulation(),))
		if (city.isProductionUnit()):
			f.write("\t\tProductionUnit=%s\n" %(gc.getUnitInfo(city.getProductionUnit()).getType(),))
		elif (city.isProductionBuilding()):
			f.write("\t\tProductionBuilding=%s\n" %(gc.getBuildingInfo(city.getProductionBuilding()).getType(),))

		for iI in range(gc.getNumBuildingInfos()):
			if city.isHasRealBuilding(iI):
				f.write("\t\tBuildingType=%s\n" %(gc.getBuildingInfo(iI).getType()))

		for iI in range(city.getPopulation()):
			pUnit = city.getPopulationUnitByIndex(iI)
			f.write("\t\tCitizenType=%s\n" %(gc.getUnitInfo(pUnit.getUnitType()).getType()))

		if city.getScriptData():
			f.write("\t\tScriptData=%s\n" %city.getScriptData())

		# Player culture
		for iPlayerLoop in range(gc.getMAX_CIV_PLAYERS()):
			iPlayerCulture = city.getCulture(iPlayerLoop)
			if (iPlayerCulture > 0):
				f.write("\t\tPlayer%dCulture=%d\n" %(iPlayerLoop, iPlayerCulture))

		f.write("\tEndCity\n")

	def read(self, f, iX, iY):
		"read in city data - at this point the first line 'BeginCity' has already been read"
		self.__init__()
		self.plotX=iX
		self.plotY=iY
		parser = CvWBParser()
		while (true):
			nextLine = parser.getNextLine(f)
			toks = parser.getTokens(nextLine)
			if (len(toks)==0):
				break

			# City - Owner
			vOwner=parser.findTokenValue(toks, "CityOwner")
			if (vOwner != -1):
				self.owner = int(vOwner)
				continue

			# City - Name
			vName=parser.findTokenValue(toks, "CityName")
			if (vName != -1):
				self.name = (vName).decode(fileencoding)
				continue

			# City - Population
			v=parser.findTokenValue(toks, "CityPopulation")
			if v!=-1:
				self.population = (int(v))
				continue

			# City - Production Unit
			v=parser.findTokenValue(toks, "ProductionUnit")
			if v!=-1:
				self.productionUnit = v
				continue

			# City - Production Building
			v=parser.findTokenValue(toks, "ProductionBuilding")
			if v!=-1:
				self.productionBuilding = v
				continue

			# City - Culture XXX obsolete...
			v=parser.findTokenValue(toks, "CityCulture")
			if v!=-1:
				self.culture = int(v)
				continue

			# City - Buildings
			v=parser.findTokenValue(toks, "BuildingType")
			if v!=-1:
				self.bldgType.append(v)
				continue

			# City - Citizens
			v=parser.findTokenValue(toks, "CitizenType")
			if v!=-1:
				self.citizenList.append(v)
				continue

			# City - ScriptData
			v=parser.findTokenValue(toks, "ScriptData")
			if v!=-1:
				self.szScriptData = v
				continue

			# Player Culture
			for iPlayerLoop in range(gc.getMAX_CIV_PLAYERS()):
				szCityTag = ("Player%dCulture" %(iPlayerLoop))
				v = parser.findTokenValue(toks, szCityTag)
				if v!=-1:
					self.aiPlayerCulture[iPlayerLoop] = int(v)
					continue

			if parser.findTokenValue(toks, "EndCity")!=-1:
				break

###########
class CvPlotDesc:
	"serializes plot data"
	def __init__(self):
		self.iX = -1
		self.iY = -1
		self.riverNSDirection = CardinalDirectionTypes.NO_CARDINALDIRECTION
		self.isNOfRiver = 0
		self.riverWEDirection = CardinalDirectionTypes.NO_CARDINALDIRECTION
		self.isWOfRiver = 0
		self.isStartingPlot = 0
		self.europeType = None
		self.bonusType = None
		self.improvementType = None
		self.featureType = None
		self.featureVariety = 0
		self.routeType = None
		self.terrainType = None
		self.plotType = PlotTypes.NO_PLOT
		self.unitDescs = list()
		self.cityDesc = None
		self.szLandmark = ""
		self.szScriptData = "NONE"
		self.abTeamPlotRevealed = [0]*gc.getMAX_CIV_TEAMS()

	def needToWritePlot(self, plot):
		"returns true if this plot needs to be written out."
		return True

	def preApply(self):
		"apply plot and terrain type"
		plot = CyMap().plot(self.iX, self.iY)
		if (self.plotType != PlotTypes.NO_PLOT):
			plot.setPlotType(self.plotType, False, False)
		if (self.terrainType):
			terrainTypeNum = CvUtil.findInfoTypeNum(self.terrainType)
			plot.setTerrainType(terrainTypeNum, False, False)

	def apply(self):
		"after reading, this will actually apply the data"
		plot = CyMap().plot(self.iX, self.iY)
		plot.setNOfRiver(self.isNOfRiver, self.riverWEDirection)
		plot.setWOfRiver(self.isWOfRiver, self.riverNSDirection)
		plot.setStartingPlot(self.isStartingPlot)
		if (self.europeType):
			plot.setEurope(CvUtil.findInfoTypeNum(self.europeType))
		if (self.bonusType):
			bonusTypeNum = CvUtil.findInfoTypeNum(self.bonusType)
			plot.setBonusType(bonusTypeNum)
		if (self.improvementType):
			improvementTypeNum = CvUtil.findInfoTypeNum(self.improvementType)
			plot.setImprovementType(improvementTypeNum)
		if (self.featureType):
			featureTypeNum = CvUtil.findInfoTypeNum(self.featureType)
			plot.setFeatureType(featureTypeNum, self.featureVariety)
		if (self.routeType):
			routeTypeNum = CvUtil.findInfoTypeNum(self.routeType)
			plot.setRouteType(routeTypeNum)

		if (self.szLandmark != ""):
			CyEngine().addLandmark(CyMap().plot(self.iX, self.iY), "%s" %(self.szLandmark))

		if (self.szScriptData != "NONE"):
			plot.setScriptData(self.szScriptData)

	def applyUnits(self):
		for u in self.unitDescs:
			u.apply()

	def applyCity(self):
		if self.cityDesc:
			self.cityDesc.apply()

	def write(self, f, plot):
		"save plot desc to a file"
		f.write("BeginPlot\n")
		f.write("\tx=%d,y=%d\n" %(plot.getX(), plot.getY()))

		# scriptData
		if (plot.getScriptData() != ""):
			f.write("\tScriptData=%s\n" %plot.getScriptData())
		# rivers
		if (plot.getRiverNSDirection() != CardinalDirectionTypes.NO_CARDINALDIRECTION):
			f.write("\tRiverNSDirection=%d\n" %(int(plot.getRiverNSDirection()),) )
		if (plot.isNOfRiver()):
			f.write("\tisNOfRiver\n")
		if (plot.getRiverWEDirection() != CardinalDirectionTypes.NO_CARDINALDIRECTION):
			f.write("\tRiverWEDirection=%d\n" %(int(plot.getRiverWEDirection()),) )
		if (plot.isWOfRiver()):
			f.write("\tisWOfRiver\n")
		# extras
		if (plot.isStartingPlot()):
			f.write("\tStartingPlot\n")
		if (plot.getEurope()!=-1):
			f.write("\tEuropeType=%s\n" %(gc.getEuropeInfo(plot.getEurope()).getType()) )
		if (plot.getBonusType()!=-1):
			f.write("\tBonusType=%s\n" %(gc.getBonusInfo(plot.getBonusType()).getType()) )
		if (plot.getImprovementType()!=-1):
			f.write("\tImprovementType=%s\n" %(gc.getImprovementInfo(plot.getImprovementType()).getType()) )
		if (plot.getFeatureType()!=-1):
			f.write("\tFeatureType=%s, FeatureVariety=%d\n"
			%(gc.getFeatureInfo(plot.getFeatureType()).getType(), plot.getFeatureVariety(), ) )
		if (plot.getRouteType()!=-1):
			f.write("\tRouteType=%s\n" %(gc.getRouteInfo(plot.getRouteType()).getType()) )
		if (plot.getTerrainType()!=-1):
			f.write("\tTerrainType=%s\n" %(gc.getTerrainInfo(plot.getTerrainType()).getType()) )
		if (plot.getPlotType()!=PlotTypes.NO_PLOT):
			f.write("\tPlotType=%d\n" %(int(plot.getPlotType()),) )

		# units
		for i in range(plot.getNumUnits()):
			unit=plot.getUnit(i)
			if unit.getUnitType() == -1:
				continue
			CvUnitDesc().write(f, unit, plot)
		# city
		if (plot.isCity()):
			CvCityDesc().write(f, plot)

		# Fog of War
		bFirstReveal=true
		for iTeamLoop in range(gc.getMAX_CIV_TEAMS()):
			if (gc.getTeam(iTeamLoop).isAlive()):
				if (plot.isRevealed(iTeamLoop,0)):
					# Plot is revealed for this Team so write out the fact that it is; if not revealed don't write anything
					if (bFirstReveal):
						f.write("\tTeamReveal=")
						bFirstReveal=false
					f.write("%d," %(iTeamLoop))
		if (bFirstReveal==false):
			f.write("\n")	# terminate reveal line

		f.write("EndPlot\n")

	def read(self, f):
		"read in a plot desc"
		self.__init__()
		parser = CvWBParser()
		if parser.findNextToken(f, "BeginPlot")==false:
			return false	# no more plots
		while (true):
			nextLine = parser.getNextLine(f)
			toks = parser.getTokens(nextLine)
			if (len(toks)==0):
				break

			x = parser.findTokenValue(toks, "x")
			y = parser.findTokenValue(toks, "y")
			if (x!=-1 and y!=-1):
				self.iX = int(x)
				self.iY = int(y)
				continue

			v = parser.findTokenValue(toks, "Landmark")
			if v!=-1:
				self.szLandmark=v
				continue

			v = parser.findTokenValue(toks, "ScriptData")
			if v!=-1:
				self.szScriptData=v
				continue

			v = parser.findTokenValue(toks, "RiverNSDirection")
			if v!=-1:
				self.riverNSDirection = (CardinalDirectionTypes(v))
				continue

			if (parser.findTokenValue(toks, "isNOfRiver"))!=-1:
				self.isNOfRiver = (true)
				continue

			v = parser.findTokenValue(toks, "RiverWEDirection")
			if v!=-1:
				self.riverWEDirection = (CardinalDirectionTypes(v))
				continue

			if (parser.findTokenValue(toks, "isWOfRiver"))!=-1:
				self.isWOfRiver = (true)
				continue

			if (parser.findTokenValue(toks, "StartingPlot"))!=-1:
				self.isStartingPlot = (true)
				continue

			v = parser.findTokenValue(toks, "EuropeType")
			if v!=-1:
				self.europeType = v
				continue

			v = parser.findTokenValue(toks, "BonusType")
			if v!=-1:
				self.bonusType = v
				continue

			v = parser.findTokenValue(toks, "ImprovementType")
			if v!=-1:
				self.improvementType = v
				continue

			v = parser.findTokenValue(toks, "FeatureType")
			if v!=-1:
				self.featureType = v
				v = parser.findTokenValue(toks, "FeatureVariety")
				if v!=-1:
					self.featureVariety = int(v)
				continue

			v = parser.findTokenValue(toks, "RouteType")
			if v!=-1:
				self.routeType = v
				continue

			v = parser.findTokenValue(toks, "TerrainType")
			if v!=-1:
				self.terrainType = v
				continue

			v = parser.findTokenValue(toks, "PlotType")
			if v!=-1:
				self.plotType = PlotTypes(v)
				continue

			# Units
			v = parser.findTokenValue(toks, "BeginUnit")
			if v!=-1:
				unitDesc = CvUnitDesc()
				unitDesc.read(f, self.iX, self.iY)
				self.unitDescs.append(unitDesc)
				continue

			# City
			v = parser.findTokenValue(toks, "BeginCity")
			if v!=-1:
				self.cityDesc = CvCityDesc()
				self.cityDesc.read(f, self.iX, self.iY)
				continue

			# Fog of War

			v = parser.findTokenValue(toks, "TeamReveal")
			if v!=-1:
				for iTeamLoop in toks:
					iTeamLoop = iTeamLoop.lstrip('TeamReveal=')
					if len(iTeamLoop):
						self.abTeamPlotRevealed[int(iTeamLoop)] = true
				continue

			if parser.findTokenValue(toks, "EndPlot")!=-1:
				break
		return True

################
class CvMapDesc:
	"serialize map data"
	def __init__(self):
		self.iGridW = 0
		self.iGridH = 0
		self.iTopLatitude = 0
		self.iBottomLatitude = 0
		self.bWrapX = 0
		self.bWrapY = 0
		self.worldSize = None
		self.climate = None
		self.seaLevel = None
		self.numPlotsWritten = 0
		self.numSignsWritten = 0
		self.bRandomizeFeatures = "false" #WTP, ray, Randomize Features Map Option
		self.bRandomizeResources = "false"
		self.bRandomizeGoodies = "false"
		self.iCityRadius = 0 # 0 means value from UserSettings.txt

	def write(self, f):
		"write map data"
		map = CyMap()
		iGridW = map.getGridWidth()
		iGridH = map.getGridHeight()
		iNumPlots = iGridW * iGridH
		iNumSigns = CyEngine().getNumSigns()

		f.write("BeginMap\n")
		f.write("\tgrid width=%d\n" %(map.getGridWidth(),))
		f.write("\tgrid height=%d\n" %(map.getGridHeight(),))
		f.write("\ttop latitude=%d\n" %(map.getTopLatitude(),))
		f.write("\tbottom latitude=%d\n" %(map.getBottomLatitude(),))
		f.write("\twrap X=%d\n" %(map.isWrapX(),))
		f.write("\twrap Y=%d\n" %(map.isWrapY(),))
		f.write("\tworld size=%s\n" %(gc.getWorldInfo(map.getWorldSize()).getType(),))
		f.write("\tclimate=%s\n" %(gc.getClimateInfo(map.getClimate()).getType(),))
		f.write("\tsealevel=%s\n" %(gc.getSeaLevelInfo(map.getSeaLevel()).getType(),))
		f.write("\tnum plots written=%d\n" %(iNumPlots,))
		f.write("\tnum signs written=%d\n" %(iNumSigns,))
		f.write("\tRandomize Features=false\n") #WTP, ray, Randomize Features Map Option
		f.write("\tRandomize Resources=false\n")
		f.write("\tCity Catchment Radius=%d\n" %(map.getCityCatchmentRadius(),))
		f.write("EndMap\n")

	def read(self, f):
		"read map data"
		self.__init__()
		parser = CvWBParser()
		if parser.findNextToken(f, "BeginMap")==false:
			print "can't find map"
			return
		while (true):
			nextLine = parser.getNextLine(f)
			toks = parser.getTokens(nextLine)
			if (len(toks)==0):
				break

			v = parser.findTokenValue(toks, "grid width")
			if v!=-1:
				self.iGridW = int(v)
				continue

			v = parser.findTokenValue(toks, "grid height")
			if v!=-1:
				self.iGridH = int(v)
				continue

			v = parser.findTokenValue(toks, "top latitude")
			if v!=-1:
				self.iTopLatitude = int(v)
				continue

			v = parser.findTokenValue(toks, "bottom latitude")
			if v!=-1:
				self.iBottomLatitude = int(v)
				continue

			v = parser.findTokenValue(toks, "wrap X")
			if v!=-1:
				self.bWrapX = int(v)
				continue

			v = parser.findTokenValue(toks, "wrap Y")
			if v!=-1:
				self.bWrapY = int(v)
				continue

			v = parser.findTokenValue(toks, "world size")
			if v!=-1:
				self.worldSize = v
				continue

			v = parser.findTokenValue(toks, "climate")
			if v!=-1:
				self.climate = v
				continue

			v = parser.findTokenValue(toks, "sealevel")
			if v!=-1:
				self.seaLevel = v
				continue

			v = parser.findTokenValue(toks, "num plots written")
			if v!=-1:
				self.numPlotsWritten = int(v)
				continue

			v = parser.findTokenValue(toks, "num signs written")
			if v!=-1:
				self.numSignsWritten = int(v)
				continue

			v = parser.findTokenValue(toks, "Randomize Features")
			if v!=-1:
				self.bRandomizeFeatures = v
				continue

			v = parser.findTokenValue(toks, "Randomize Resources")
			if v!=-1:
				self.bRandomizeResources = v
				continue
				
			v = parser.findTokenValue(toks, "Randomize Goodies")
			if v!=-1:
				self.bRandomizeGoodies = v
				continue

			v = parser.findTokenValue(toks, "City Catchment Radius")
			if v!=-1:
				self.iCityRadius = int(v)
				continue

			if parser.findTokenValue(toks, "EndMap")!=-1:
				break


################
class CvSignDesc:
	"serialize map data"
	def __init__(self):
		self.iPlotX = 0
		self.iPlotY = 0
		self.playerType = 0
		self.szCaption = ""

	def apply(self):
		plot = CyMap().plot(self.iPlotX, self.iPlotY)
		CyEngine().addSign(plot, self.playerType, self.szCaption)

	def write(self, f, sign):
		"write sign data"
		f.write("BeginSign\n")
		f.write("\tplotX=%d\n" %(sign.getPlot().getX(),))
		f.write("\tplotY=%d\n" %(sign.getPlot().getY(),))
		f.write("\tplayerType=%d\n" %(sign.getPlayerType(),))
		f.write("\tcaption=%s\n" %(sign.getCaption(),))
		f.write("EndSign\n")

	def read(self, f):
		"read sign data"
		self.__init__()
		parser = CvWBParser()
		if parser.findNextToken(f, "BeginSign")==false:
			print "can't find sign"
			return
		while (true):
			nextLine = parser.getNextLine(f)
			toks = parser.getTokens(nextLine)
			if (len(toks)==0):
				break

			v = parser.findTokenValue(toks, "plotX")
			if v!=-1:
				self.iPlotX = int(v)
				continue

			v = parser.findTokenValue(toks, "plotY")
			if v!=-1:
				self.iPlotY = int(v)
				continue

			v = parser.findTokenValue(toks, "playerType")
			if v!=-1:
				self.playerType = int(v)
				continue

			v = parser.findTokenValue(toks, "caption")
			if v!=-1:
				self.szCaption = v
				continue

			if parser.findTokenValue(toks, "EndSign")!=-1:
				break

		return True

class CvWBDesc:
	"handles saving/loading a worldbuilder description file"
	def __init__(self):
		# game data
		self.gameDesc = CvGameDesc()
		self.playersDesc = None
		self.plotDesc = None	# list
		self.signDesc = None	# list
		self.mapDesc = CvMapDesc()

	def getVersion(self):
		return version

	def getDescFileName(self, fileName):
		return fileName+getWBSaveExtension()

	def write(self, fileName):
		"Save out a high-level desc of the world, and height/terrainmaps"
		fileName = os.path.normpath(fileName)
		fileName,ext = os.path.splitext(fileName)
		CvUtil.pyPrint( 'saveDesc:%s, curDir:%s' %(fileName,os.getcwd()) )

		f = file(self.getDescFileName(fileName), "w")		# open text file
		f.write("Version=%d\n" %(self.getVersion(),))
		self.gameDesc.write(f)	# write game info

		for i in range(gc.getMAX_CIV_TEAMS()):
			CvTeamDesc().write(f, i)		# write team info

		for i in range(gc.getMAX_CIV_PLAYERS()):
			CvPlayerDesc().write(f, i)		# write player info

		self.mapDesc.write(f)	# write map info

		f.write("\n### Plot Info ###\n")
		iGridW = CyMap().getGridWidth()
		iGridH = CyMap().getGridHeight()
		for iX in range(iGridW):
			for iY in range(iGridH):
				plot = CyMap().plot(iX, iY)
				pDesc = CvPlotDesc()
				if pDesc.needToWritePlot(plot):
					pDesc.write(f, plot)

		f.write("\n### Sign Info ###\n")
		iNumSigns = CyEngine().getNumSigns()
		for i in range(iNumSigns):
			sign = CyEngine().getSignByIndex(i)
			pDesc = CvSignDesc()
			pDesc.write(f, sign)

		f.close()

		print("WBSave done\n")
		return 0	# success

	def applyMap(self):
		"after reading setup the map"

		self.gameDesc.apply()

		# recreate map
		worldSizeType = CvUtil.findInfoTypeNum(self.mapDesc.worldSize)
		climateType = CvUtil.findInfoTypeNum(self.mapDesc.climate)
		seaLevelType = CvUtil.findInfoTypeNum(self.mapDesc.seaLevel)
		CyMap().rebuild(self.mapDesc.iGridW, self.mapDesc.iGridH, self.mapDesc.iTopLatitude, self.mapDesc.iBottomLatitude, self.mapDesc.bWrapX, self.mapDesc.bWrapY, WorldSizeTypes(worldSizeType), ClimateTypes(climateType), SeaLevelTypes(seaLevelType), 0, None)

		# update the city catchment radius
		CyMap().setCityCatchmentRadiusNoMapMaker(self.mapDesc.iCityRadius)

		for pDesc in self.plotDesc:
			pDesc.preApply()	# set plot type / terrain type

		CyMap().recalculateAreas()

		for pDesc in self.plotDesc:
			pDesc.apply()

		for pDesc in self.signDesc:
			pDesc.apply()

		if (self.mapDesc.bRandomizeFeatures != "false"): #WTP, ray, Randomize Features Map Option
			CyMapGenerator().eraseFeaturesOnLand()
			CyMapGenerator().addFeaturesOnLand()
			
		if (self.mapDesc.bRandomizeResources != "false"):
			for iPlotLoop in range(CyMap().numPlots()):
				pPlot = CyMap().plotByIndex(iPlotLoop)
				pPlot.setBonusType(BonusTypes.NO_BONUS)
			CyMapGenerator().addBonuses()
			
		if (self.mapDesc.bRandomizeGoodies != "false"):
			CyMapGenerator().eraseGoodies()
			CyMapGenerator().addGoodies()

		return 0	# ok

	def getAssignedStartingPlots(self):
		"add player starting plots if using random civs"

		# Player stuff
		for iPlayerLoop in range(gc.getMAX_CIV_PLAYERS()):

			pPlayer = gc.getPlayer(iPlayerLoop)
			pWBPlayer = self.playersDesc[iPlayerLoop]

			# Random Start Location
			if (pPlayer.getLeaderType() != -1 and pWBPlayer.bRandomStartLocation != "false"):
				pPlayer.setStartingPlot(pPlayer.findStartingPlot(true), True)
			else:

				# Player's starting plot
				if ((pWBPlayer.iStartingX != -1) and (pWBPlayer.iStartingY != -1)):
					pPlayer.setStartingPlot(CyMap().plot(pWBPlayer.iStartingX, pWBPlayer.iStartingY), True)

		return 0	# ok

	def applyInitialItems(self):
		"add player objects in a last pass"
		self.identifyValidPlayersAndTeams()
		self.applyInitialTeams1()
		self.applyInitialPlayers()
		self.applyInitialCities()
		self.applyInitialTeams2()
		self.applyInitialPlots()
		
		return 0	# ok

	def identifyValidPlayersAndTeams(self):
		self.validPlayers = [0] * gc.getMAX_CIV_PLAYERS()
		self.validTeams   = [0] * gc.getMAX_CIV_TEAMS()
		
		for iPlayerLoop in range(gc.getMAX_CIV_PLAYERS()):
			if (self.playersDesc[iPlayerLoop]):
				pWBPlayer = self.playersDesc[iPlayerLoop]
				if (pWBPlayer.civType != 'NONE' and self.validTeams[pWBPlayer.team] != -1):
					self.validPlayers[iPlayerLoop] = 1
					self.validTeams[pWBPlayer.team] = 1

	def applyInitialTeams1(self):
		# Team stuff
		if (len(self.teamsDesc)) :
			for iTeamLoop in range(gc.getMAX_CIV_TEAMS()):

				if (self.validTeams[iTeamLoop] and self.teamsDesc[iTeamLoop]):

					pTeam = gc.getTeam(iTeamLoop)
					pWBTeam = self.teamsDesc[iTeamLoop]
					# Contact with Other Teams
					for meetTeam in self.teamsDesc[iTeamLoop].bContactWithTeamList:
						if (self.validTeams[meetTeam]):
							gc.getTeam(iTeamLoop).meet(meetTeam, false)

					# Wars
					for warTeam in self.teamsDesc[iTeamLoop].bWarWithTeamList:
						if (self.validTeams[warTeam]):
							gc.getTeam(iTeamLoop).declareWar(warTeam, false, WarPlanTypes.NO_WARPLAN)

					# Permanent War/Peace
					for permanentWarPeaceTeam in self.teamsDesc[iTeamLoop].bPermanentWarPeaceList:
						if (self.validTeams[permanentWarPeaceTeam]):
							gc.getTeam(iTeamLoop).setPermanentWarPeace(permanentWarPeaceTeam, true)

					# Open Borders
					for openBordersTeam in self.teamsDesc[iTeamLoop].bOpenBordersWithTeamList:
						if (self.validTeams[openBordersTeam]):
							gc.getTeam(iTeamLoop).signOpenBorders(openBordersTeam)

					# Defensive Pacts
					for defensivePactTeam in self.teamsDesc[iTeamLoop].bDefensivePactWithTeamList:
						if (self.validTeams[defensivePactTeam]):
							gc.getTeam(iTeamLoop).signDefensivePact(defensivePactTeam)

	def applyInitialPlayers(self):
		# Player stuff
		if (len(self.playersDesc)) :
			for iPlayerLoop in range(gc.getMAX_CIV_PLAYERS()):

				if (self.validPlayers[iPlayerLoop] and self.playersDesc[iPlayerLoop]):

					pPlayer = gc.getPlayer(iPlayerLoop)
					pWBPlayer = self.playersDesc[iPlayerLoop]

					# Player's starting gold
					pPlayer.setGold(pWBPlayer.iStartingGold)

					# Player's starting plot
					if ((pWBPlayer.iStartingX != -1) and (pWBPlayer.iStartingY != -1)):
						pPlayer.setStartingPlot(CyMap().plot(pWBPlayer.iStartingX, pWBPlayer.iStartingY), True)

					# Starting Era
					if (pWBPlayer.szStartingEra != ""):
						iStartingEra = gc.getInfoTypeForString(pWBPlayer.szStartingEra)
						pPlayer.setCurrentEra(iStartingEra)

					# Random Start Location
					if (pWBPlayer.bRandomStartLocation != "false"):
						pPlayer.setStartingPlot(pPlayer.findStartingPlot(true), True)
						print("Setting player %d starting location to (%d,%d)", pPlayer.getID(), pPlayer.getStartingPlot().getX(), pPlayer.getStartingPlot().getY())

					# Civics
					for iCivicLoop in range(len(pWBPlayer.aaiCivics)):
						iCivicOption = pWBPlayer.aaiCivics[iCivicLoop][0]
						iCivic = pWBPlayer.aaiCivics[iCivicLoop][1]
						pPlayer.setCivic(iCivicOption,iCivic)

					# Attitude Extras
					for iAttitudeLoop in range(len(pWBPlayer.aaiAttitudeExtras)):
						iPlayer = pWBPlayer.aaiAttitudeExtras[iAttitudeLoop][0]
						iExtra = pWBPlayer.aaiAttitudeExtras[iAttitudeLoop][1]
						pPlayer.AI_setAttitudeExtra(iPlayer,iExtra)

					# City List
					for iCityListLoop in range(len(pWBPlayer.aszCityList)):
						pPlayer.addCityName(pWBPlayer.aszCityList[iCityListLoop])

	def applyInitialCities(self):
		# cities
		for pDesc in self.plotDesc:
			pDesc.applyCity()

	def applyInitialTeams2(self):
		# Team stuff
		if (len(self.teamsDesc)) :
			for iTeamLoop in range(gc.getMAX_CIV_TEAMS()):

				if (self.validTeams[iTeamLoop] and self.teamsDesc[iTeamLoop]):

					pTeam = gc.getTeam(iTeamLoop)
					pWBTeam = self.teamsDesc[iTeamLoop]

					# Reveal whole map
					if (pWBTeam.bRevealMap == 1):

						for iPlotX in range(CyMap().getGridWidth()):
							for iPlotY in range(CyMap().getGridHeight()):
							    pPlot = CyMap().plot(iPlotX,iPlotY)
							    pPlot.setRevealed(pTeam.getID(), True, False, TeamTypes.NO_TEAM)

	def applyInitialPlots(self):
		# Per plot stuff
		for iPlotLoop in range(self.mapDesc.numPlotsWritten):

			pWBPlot = self.plotDesc[iPlotLoop]

			# Reveal Fog of War for teams
			for iTeamLoop in range(gc.getMAX_CIV_TEAMS()):
				if (self.validTeams[iTeamLoop]):
					pTeam = gc.getTeam(iTeamLoop)
					if (pWBPlot.abTeamPlotRevealed[iTeamLoop] == 1):
						pPlot = CyMap().plot(pWBPlot.iX, pWBPlot.iY)
						pPlot.setRevealed(pTeam.getID(), True, False, TeamTypes.NO_TEAM)
		# units
		for pDesc in self.plotDesc:
			pDesc.applyUnits()

	def read(self, fileName):
		"Load in a high-level desc of the world, and height/terrainmaps"
		fileName = os.path.normpath(fileName)
		fileName,ext=os.path.splitext(fileName)
		if len(ext) == 0:
			ext = getWBSaveExtension()
		CvUtil.pyPrint( 'loadDesc:%s, curDir:%s' %(fileName,os.getcwd()) )

		if (not os.path.isfile(fileName+ext)):
			CvUtil.pyPrint("Error: file %s does not exist" %(fileName+ext,))
			return -1	# failed
		f=file(fileName+ext, "r")		# open text file

		parser = CvWBParser()
		version = int(parser.findNextTokenValue(f, "Version"))
		if (version != self.getVersion()):
			CvUtil.pyPrint("Error: wrong WorldBuilder save version.  Expected %d, got %d" %(self.getVersion(), version))
			return -1	# failed
		print "Reading game desc"
		self.gameDesc.read(f)	# read game info

		print "Reading teams desc"
		filePos = f.tell()
		self.teamsDesc = []
		for i in range(gc.getMAX_CIV_TEAMS()):
			print ("reading team %d" %(i))
			teamsDesc = CvTeamDesc()
			if (teamsDesc.read(f)==false):					# read team info
				f.seek(filePos)								# abort and backup
				break
			self.teamsDesc.append(teamsDesc)

		print "Reading players desc"
		self.playersDesc = []
		for i in range(gc.getMAX_CIV_PLAYERS()):
			playerDesc = CvPlayerDesc()
			playerDesc.read(f)				# read player info
			self.playersDesc.append(playerDesc)

		print "Reading map desc"
		self.mapDesc.read(f)	# read map info

		print("Reading/creating %d plot descs" %(self.mapDesc.numPlotsWritten,))
		self.plotDesc = []
		for i in range(self.mapDesc.numPlotsWritten):
			pDesc = CvPlotDesc()
			if pDesc.read(f)==True:
				self.plotDesc.append(pDesc)
			else:
				break

		print("Reading/creating %d sign descs" %(self.mapDesc.numSignsWritten,))
		self.signDesc = []
		for i in range(self.mapDesc.numSignsWritten):
			pDesc = CvSignDesc()
			if pDesc.read(f)==True:
				self.signDesc.append(pDesc)
			else:
				break

		f.close()
		print ("WB read done\n")
		return 0

