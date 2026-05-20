## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import string
import CvUtil
import ScreenInput
import CvScreenEnums
import CvPediaScreen		# base class
import CvPediaUnit
import CvPediaProfession
import CvPediaFather
import CvPediaBuilding
import CvPediaPromotion
import CvPediaBonus
import CvPediaTerrain
import CvPediaFeature
import CvPediaImprovement
import CvPediaCivic
import CvPediaCivilization
import CvPediaLeader
import CvPediaHistory
import CvPediaYields
import CvPediaAchieve                    

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class CvPediaMain( CvPediaScreen.CvPediaScreen ):
	"Civilopedia Main Screen"

	def __init__(self):

		self.PEDIA_MAIN_SCREEN_NAME = "PediaMainScreen"

		self.WIDGET_ID = "PediaMainWidget"
		self.EXIT_ID = "PediaMainExitWidget"
		self.BACKGROUND_ID = "PediaMainBackground"
		self.TOP_PANEL_ID = "PediaMainTopPanel"
		self.BOTTOM_PANEL_ID = "PediaMainBottomPanel"
		self.BACK_ID = "PediaMainBack"
		self.NEXT_ID = "PediaMainForward"
		self.TOP_ID = "PediaMainTop"
		self.LIST_ID = "PediaMainList"
# TAC Start
		self.CREDITS_ID = 11
		self.RAEDME_ID = 12
# TAC Ende
		
		self.nWidgetCount = 0

		# screen instances
		self.pediaUnitScreen = CvPediaUnit.CvPediaUnit(self)
		self.pediaProfessionScreen = CvPediaProfession.CvPediaProfession(self)
		self.pediaFatherScreen = CvPediaFather.CvPediaFather(self)
		self.pediaBuildingScreen = CvPediaBuilding.CvPediaBuilding(self)
		self.pediaPromotionScreen = CvPediaPromotion.CvPediaPromotion(self)
		self.pediaBonus = CvPediaBonus.CvPediaBonus(self)
		self.pediaTerrain = CvPediaTerrain.CvPediaTerrain(self)
		self.pediaYields = CvPediaYields.CvPediaYields(self)
		self.pediaFeature = CvPediaFeature.CvPediaFeature(self)
		self.pediaImprovement = CvPediaImprovement.CvPediaImprovement(self)
		self.pediaCivic = CvPediaCivic.CvPediaCivic(self)
		self.pediaCivilization = CvPediaCivilization.CvPediaCivilization(self)
		self.pediaLeader = CvPediaLeader.CvPediaLeader(self)
		self.pediaHistorical = CvPediaHistory.CvPediaHistory(self)
		self.pediaAchieve = CvPediaAchieve.CvPediaAchieve(self)

		# used for search function
		self.ACHIEVE_SEARCH_CATEGORY_WIDGET = 9951
		self.ACHIEVE_SEARCH_VALUE_WIDGET = 9952

		self.achieveSearchCategory = "ALL"
		self.achieveSearchValue = ""

		self.achieveSearchCategories = [
			("ALL", "TXT_KEY_PEDIA_ACHIEVE_FILTER_ALL", "All"),
			("UNITS", "TXT_KEY_PEDIA_ACHIEVE_FILTER_UNITS", "Units"),
			("BUILDINGS", "TXT_KEY_PEDIA_ACHIEVE_FILTER_BUILDINGS", "Buildings"),
			("PROFESSIONS", "TXT_KEY_PEDIA_ACHIEVE_FILTER_PROFESSIONS", "Professions"),
			("GOODS", "TXT_KEY_PEDIA_ACHIEVE_FILTER_GOODS", "Goods"),
			("MISC", "TXT_KEY_PEDIA_ACHIEVE_FILTER_MISC", "Miscellaneous"),
		]                                                         

		# used for navigating "forward" and "back" in civilopedia
		self.pediaHistory = []
		self.pediaFuture = []

		self.listCategories = []

		self.iCategory = -1
		self.iLastScreen = -1
		self.iActivePlayer = -1
		self.CATEGORY_ACHIEVEMENTS = CivilopediaPageTypes.CIVILOPEDIA_PAGE_HINTS + 1
		self.mapCategories = {
			CivilopediaPageTypes.CIVILOPEDIA_PAGE_UNIT	: self.placeUnits,
			CivilopediaPageTypes.CIVILOPEDIA_PAGE_PROFESSION	: self.placeProfessions,
			CivilopediaPageTypes.CIVILOPEDIA_PAGE_FATHER	: self.placeFathers,
			CivilopediaPageTypes.CIVILOPEDIA_PAGE_BUILDING	: self.placeBuildings,
			CivilopediaPageTypes.CIVILOPEDIA_PAGE_TERRAIN	: self.placeTerrains,
			CivilopediaPageTypes.CIVILOPEDIA_PAGE_YIELDS	: self.placeYields,
			CivilopediaPageTypes.CIVILOPEDIA_PAGE_FEATURE	: self.placeFeatures,
			CivilopediaPageTypes.CIVILOPEDIA_PAGE_BONUS	: self.placeBoni,
			CivilopediaPageTypes.CIVILOPEDIA_PAGE_IMPROVEMENT	: self.placeImprovements,
			CivilopediaPageTypes.CIVILOPEDIA_PAGE_PROMOTION	: self.placePromotions,
			CivilopediaPageTypes.CIVILOPEDIA_PAGE_CIV	: self.placeCivs,
			CivilopediaPageTypes.CIVILOPEDIA_PAGE_LEADER	: self.placeLeaders,
			CivilopediaPageTypes.CIVILOPEDIA_PAGE_CIVIC	: self.placeCivics,
			CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT	: self.placeConcepts,
			CivilopediaPageTypes.CIVILOPEDIA_PAGE_HINTS	: self.placeHints,
			self.CATEGORY_ACHIEVEMENTS	: self.placeAchievements,
			}

	def getScreen(self):
		return CyGInterfaceScreen(self.PEDIA_MAIN_SCREEN_NAME, CvScreenEnums.PEDIA_MAIN)
	
	def setPediaCommonWidgets(self):
		self.EXIT_TEXT = u"<font=4>" + localText.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + "</font>"
		self.BACK_TEXT = u"<font=4>" + localText.getText("TXT_KEY_PEDIA_SCREEN_BACK", ()).upper() + "</font>"
		self.FORWARD_TEXT = u"<font=4>" + localText.getText("TXT_KEY_PEDIA_SCREEN_FORWARD", ()).upper() + "</font>"
		self.MENU_TEXT = u"<font=4>" + localText.getText("TXT_KEY_PEDIA_SCREEN_TOP", ()).upper() + "</font>"

# TAC Start
# Credits Bildschirm Start
# alt:		self.CREDITS_TEXT = u"<font=4>" + localText.getText("TXT_KEY_CREDITS_HEAD", ()).upper() + "</font>"
		self.CREDITS_TEXT = u"<font=4>" + localText.getText("[COLOR_FONT_GOLD]", ()) + localText.getText("TXT_KEY_CREDITS_BUTTON", ()).upper() + "</font>"
# Credits Bildschirm Ende
# Readme Bildschirm Start
# alt:		self.README_TEXT = u"<font=4>" + localText.getText("TXT_KEY_README_BUTTON", ()).upper() + "</font>"
		self.README_TEXT = u"<font=4>" + localText.getText("[COLOR_FONT_RED]", ()) + localText.getText("TXT_KEY_README_BUTTON", ()).upper() + "</font>"
# Readme Bildschirm Ende
# TAC Ende

		self.szCategoryFather = localText.getText("TXT_KEY_PEDIA_CATEGORY_FATHERS", ())
		self.szCategoryUnit = localText.getText("TXT_KEY_PEDIA_CATEGORY_UNIT", ())
		self.szCategoryProfession = localText.getText("TXT_KEY_PEDIA_CATEGORY_PROFESSION", ())
		self.szCategoryBuilding = localText.getText("TXT_KEY_PEDIA_CATEGORY_BUILDING", ())
		self.szCategoryBonus = localText.getText("TXT_KEY_PEDIA_CATEGORY_BONUS", ())
		self.szCategoryTerrain = localText.getText("TXT_KEY_PEDIA_CATEGORY_TERRAIN", ())
		self.szCategoryYields = localText.getText("TXT_KEY_PEDIA_CATEGORY_YIELDS", ())
		self.szCategoryFeature = localText.getText("TXT_KEY_PEDIA_CATEGORY_FEATURE", ())
		self.szCategoryImprovement = localText.getText("TXT_KEY_PEDIA_CATEGORY_IMPROVEMENT", ())
		self.szCategoryPromotion = localText.getText("TXT_KEY_PEDIA_CATEGORY_PROMOTION", ())
		self.szCategoryCiv = localText.getText("TXT_KEY_PEDIA_CATEGORY_CIV", ())
		self.szCategoryLeader = localText.getText("TXT_KEY_PEDIA_CATEGORY_LEADER", ())
		self.szCategoryCivic = localText.getText("TXT_KEY_PEDIA_CATEGORY_CIVIC", ())
		self.szCategoryConcept = localText.getText("TXT_KEY_PEDIA_CATEGORY_CONCEPT", ())
		self.szCategoryHints = localText.getText("TXT_KEY_PEDIA_CATEGORY_HINTS", ())
		self.szCategoryAchievements = localText.getText("TXT_KEY_PEDIA_CATEGORY_ACHIEVEMENTS", ())			
		
		self.listCategories = [ self.szCategoryUnit,
								self.szCategoryProfession,
								self.szCategoryFather,
								self.szCategoryBuilding,
								self.szCategoryTerrain,
								self.szCategoryYields,
								self.szCategoryFeature,
								self.szCategoryBonus,
								self.szCategoryImprovement,
								self.szCategoryPromotion,
								self.szCategoryCiv,
								self.szCategoryLeader,
								self.szCategoryCivic,
								self.szCategoryConcept,
								self.szCategoryHints, 
								self.szCategoryAchievements]                                    
		
								
		screen = self.getScreen()
		self.nWidgetCount = 0

		self.deleteAllWidgets()

		self.X_SCREEN = screen.getXResolution() / 2
		self.W_SCREEN = screen.getXResolution()
		self.H_SCREEN = screen.getYResolution()
		self.Y_TITLE = 8

		self.Y_EXIT = self.H_SCREEN - 36
		self.X_EXIT = self.W_SCREEN - 30

		self.X_BACK = 50
		self.Y_BACK = self.H_SCREEN - 36

		self.X_FORWARD = 200
		self.Y_FORWARD = self.H_SCREEN - 36

		self.X_MENU = 350
		self.Y_MENU = self.H_SCREEN - 36

		self.BUTTON_SIZE = 96
		self.BUTTON_COLUMNS = 9

		self.X_ITEMS_PANE = 40
		self.Y_ITEMS_PANE = 80
		self.H_ITEMS_PANE = self.H_SCREEN - 160
		self.W_ITEMS_PANE = self.W_SCREEN - 350
		self.ITEMS_MARGIN = 18
		self.ITEMS_SEPARATION = 2

		self.X_LINKS = self.W_SCREEN - 275
		self.Y_LINKS = 80
		
		self.H_LINKS = self.H_SCREEN - 160
		self.W_LINKS = 225

		# Create a new screen
		screen.setRenderInterfaceOnly(True);
		screen.setScreenGroup(1)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		# Set background
		screen.addDDSGFC(self.BACKGROUND_ID, ArtFileMgr.getInterfaceArtInfo("SCREEN_BG_OPAQUE").getPath(), 0, 0, self.W_SCREEN, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDDSGFC("TopPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TITLE").getPath(), 0, 0, self.W_SCREEN, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDDSGFC("BottomPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_OFF").getPath(), 0, self.H_SCREEN - 55, self.W_SCREEN, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setDimensions(0, 0, self.W_SCREEN, self.H_SCREEN)

		# Exit button
		screen.setText(self.EXIT_ID, "Background", self.EXIT_TEXT, CvUtil.FONT_RIGHT_JUSTIFY, self.X_EXIT, self.Y_EXIT, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)

		# Back
		screen.setText(self.BACK_ID, "Background", self.BACK_TEXT, CvUtil.FONT_LEFT_JUSTIFY, self.X_BACK, self.Y_BACK, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_PEDIA_BACK, 1, -1)

		# Forward
		screen.setText(self.NEXT_ID, "Background", self.FORWARD_TEXT, CvUtil.FONT_LEFT_JUSTIFY, self.X_FORWARD, self.Y_FORWARD, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_PEDIA_FORWARD, 1, -1)

# TAC Start
# Credits Bildschirm Start
		screen.setText("CreditsButton", "Background", self.CREDITS_TEXT, CvUtil.FONT_RIGHT_JUSTIFY, self.X_EXIT - self.W_SCREEN / 8, self.Y_EXIT, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, self.CREDITS_ID, -1)
# Credits Bildschirm Ende
# Readme Bildschirm Start
		screen.setText("ReadmeButton", "Background", self.README_TEXT, CvUtil.FONT_RIGHT_JUSTIFY, self.X_EXIT - self.W_SCREEN * 5 / 17, self.Y_EXIT, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, self.RAEDME_ID, -1)
# Readme Bildschirm Ende
# TAC Ende
		
		# List of items on the right
		screen.addListBoxGFC(self.LIST_ID, "", self.X_LINKS, self.Y_LINKS, self.W_LINKS, self.H_LINKS, TableStyles.TABLE_STYLE_STANDARD)
		screen.enableSelect(self.LIST_ID, True)
		screen.setStyle(self.LIST_ID, "Table_StandardCiv_Style")
# Achieve Search function start
	def _getAchieveSearchLabel(self, szCategory):
		for item in self.achieveSearchCategories:
			if item[0] == szCategory:
				try:
					szText = localText.getText(item[1], ())
					if szText != "" and szText != item[1]:
						return szText
				except:
					pass
				return item[2]
		return szCategory

	def _findTextBetween(self, text, startTag, endTag, startPos=0):
		iStart = text.find(startTag, startPos)
		if iStart == -1:
			return "", -1

		iStart += len(startTag)
		iEnd = text.find(endTag, iStart)
		if iEnd == -1:
			return "", -1

		return text[iStart:iEnd], iEnd + len(endTag)

	def _getSimpleTagValue(self, blockText, tagName, defaultValue=""):
		startTag = "<%s>" % tagName
		endTag = "</%s>" % tagName
		value, dummy = self._findTextBetween(blockText, startTag, endTag, 0)
		if value == "":
			return defaultValue
		return value.strip()

	def _getNestedBlocks(self, parentText, childTag):
		results = []
		startTag = "<%s>" % childTag
		endTag = "</%s>" % childTag
		pos = 0

		while True:
			iStart = parentText.find(startTag, pos)
			if iStart == -1:
				break

			iStart += len(startTag)
			iEnd = parentText.find(endTag, iStart)
			if iEnd == -1:
				break

			results.append(parentText[iStart:iEnd])
			pos = iEnd + len(endTag)

		return results

	def _getUnitRequirementBlocks(self, blockText):
		results = []
		pos = 0

		while True:
			iUnitType = blockText.find("<UnitType>", pos)
			if iUnitType == -1:
				break

			iStart = blockText.rfind("<NumUnits>", 0, iUnitType)
			if iStart == -1:
				break

			iStart += len("<NumUnits>")
			iEnd = blockText.find("</NumUnits>", iUnitType)
			if iEnd == -1:
				break

			results.append(blockText[iStart:iEnd])
			pos = iEnd + len("</NumUnits>")

		return results

	def _getAchieveXmlBlockText(self, szBaseKey):
		try:
			import os

			gameRoot = os.path.normpath(
				os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..")
			)

			modsRoot = os.path.join(gameRoot, "Mods")
			if not os.path.isdir(modsRoot):
				return ""

			xmlText = ""
			searchTag = "<Description>%s</Description>" % szBaseKey

			for modName in os.listdir(modsRoot):
				try:
					candidates = [
						os.path.join(modsRoot, modName, "Assets", "XML", "Events", "CIV4AchieveInfos.xml"),
					]   

					foundCandidate = ""
					for candidate in candidates:
						if os.path.isfile(candidate):
							foundCandidate = candidate
							break

					if foundCandidate == "":
						continue

					f = open(foundCandidate, "r")
					candidateText = f.read()
					f.close()

					if candidateText.find(searchTag) != -1:
						xmlText = candidateText
						break
				except:
					pass

			if xmlText == "":
				return ""

			iDesc = xmlText.find(searchTag)
			if iDesc == -1:
				return ""

			iAchieveStart = xmlText.rfind("<AchieveInfo>", 0, iDesc)
			iAchieveEnd = xmlText.find("</AchieveInfo>", iDesc)
			if iAchieveStart == -1 or iAchieveEnd == -1:
				return ""

			return xmlText[iAchieveStart:iAchieveEnd]

		except:
			return ""

	def _getAchieveSearchData(self, iAchieve):
		data = {
			"units": [],
			"buildings": [],
			"professions": [],
			"goods": [],
			"misc": []
		}

		try:
			info = gc.getAchieveInfo(iAchieve)
			szBaseKey = info.getTextKey()
		except:
			return data

		if szBaseKey == "":
			return data

		blockText = self._getAchieveXmlBlockText(szBaseKey)
		if blockText == "":
			return data

		unitBlocks = self._getUnitRequirementBlocks(blockText)
		for entry in unitBlocks:
			unitType = self._getSimpleTagValue(entry, "UnitType", "")
			if unitType != "":
				data["units"].append(unitType)

		numBuildingsBlock = self._getSimpleTagValue(blockText, "NumBuildings", "")
		if numBuildingsBlock != "":
			buildingBlocks = self._getNestedBlocks(numBuildingsBlock, "NumBuilding")
			for entry in buildingBlocks:
				buildingType = self._getSimpleTagValue(entry, "BuildingType", "")
				if buildingType != "":
					data["buildings"].append(buildingType)

		numProfessionsBlock = self._getSimpleTagValue(blockText, "NumProfessions", "")
		if numProfessionsBlock != "":
			profBlocks = self._getNestedBlocks(numProfessionsBlock, "NumProfession")
			for entry in profBlocks:
				profType = self._getSimpleTagValue(entry, "ProfessionType", "")
				if profType != "":
					data["professions"].append(profType)

		yieldBlock = self._getSimpleTagValue(blockText, "YieldProduced", "")
		if yieldBlock != "":
			yieldEntries = self._getNestedBlocks(yieldBlock, "YieldProduce")
			for entry in yieldEntries:
				yieldType = self._getSimpleTagValue(entry, "YieldType", "")
				if yieldType != "":
					data["goods"].append(yieldType)

		iGoodsTraded = -1
		try:
			iGoodsTraded = int(self._getSimpleTagValue(blockText, "iNumGoodsTraded", "-1"))
		except:
			iGoodsTraded = -1

		if iGoodsTraded > 0:
			if "TRADED_GOODS" not in data["goods"]:
				data["goods"].append("TRADED_GOODS")

		if self._getSimpleTagValue(blockText, "bLandDiscovered", "0") == "1":
			data["misc"].append("LAND_DISCOVERED")

		if self._getSimpleTagValue(blockText, "bDiscoverEast", "0") == "1":
			data["misc"].append("DISCOVER_EAST")

		if self._getSimpleTagValue(blockText, "bDiscoverWest", "0") == "1":
			data["misc"].append("DISCOVER_WEST")

		if self._getSimpleTagValue(blockText, "iNumColonies", "0") != "0":
			try:
				if int(self._getSimpleTagValue(blockText, "iNumColonies", "0")) > 0:
					data["misc"].append("NUM_COLONIES")
			except:
				pass

		if self._getSimpleTagValue(blockText, "iNumEuroContacts", "0") != "0":
			try:
				if int(self._getSimpleTagValue(blockText, "iNumEuroContacts", "0")) > 0:
					data["misc"].append("EURO_CONTACTS")
			except:
				pass

		if self._getSimpleTagValue(blockText, "iNumNativeContacts", "0") != "0":
			try:
				if int(self._getSimpleTagValue(blockText, "iNumNativeContacts", "0")) > 0:
					data["misc"].append("NATIVE_CONTACTS")
			except:
				pass

		try:
			if int(self._getSimpleTagValue(blockText, "iNumCombatsWonNeeded", "-1")) > 0:
				data["misc"].append("COMBATS_WON")
		except:
			pass

		try:
			if int(self._getSimpleTagValue(blockText, "iNumSeaCombatsWonNeeded", "-1")) > 0:
				data["misc"].append("SEA_COMBATS_WON")
		except:
			pass

		return data

	def _getAchieveSearchValueDisplayName(self, szCategory, szValue):
		try:
			if szCategory == "UNITS":
				for i in range(gc.getNumUnitInfos()):
					info = gc.getUnitInfo(i)
					if info.getType() == szValue:
						return info.getDescription()

			elif szCategory == "BUILDINGS":
				for i in range(gc.getNumBuildingInfos()):
					info = gc.getBuildingInfo(i)
					if info.getType() == szValue:
						return info.getDescription()

			elif szCategory == "PROFESSIONS":
				for i in range(gc.getNumProfessionInfos()):
					info = gc.getProfessionInfo(i)
					if info.getType() == szValue:
						return info.getDescription()

			elif szCategory == "GOODS":
				if szValue == "TRADED_GOODS":
					szText = localText.getText("TXT_KEY_PEDIA_ACHIEVE_FILTER_TRADED_GOODS", ())
					if szText != "" and szText != "TXT_KEY_PEDIA_ACHIEVE_FILTER_TRADED_GOODS":
						return szText
					return u"Traded goods"

				for i in range(gc.getNumYieldInfos()):
					info = gc.getYieldInfo(i)
					if info.getType() == szValue:
						return info.getDescription()

			elif szCategory == "MISC":
				miscMap = {
					"LAND_DISCOVERED": "TXT_KEY_PEDIA_ACHIEVE_MISC_LAND_DISCOVERED",
					"DISCOVER_EAST": "TXT_KEY_PEDIA_ACHIEVE_MISC_DISCOVER_EAST",
					"DISCOVER_WEST": "TXT_KEY_PEDIA_ACHIEVE_MISC_DISCOVER_WEST",
					"NUM_COLONIES": "TXT_KEY_PEDIA_ACHIEVE_MISC_NUM_COLONIES",
					"EURO_CONTACTS": "TXT_KEY_PEDIA_ACHIEVE_MISC_EURO_CONTACTS",
					"NATIVE_CONTACTS": "TXT_KEY_PEDIA_ACHIEVE_MISC_NATIVE_CONTACTS",
					"COMBATS_WON": "TXT_KEY_PEDIA_ACHIEVE_MISC_COMBATS_WON",
					"SEA_COMBATS_WON": "TXT_KEY_PEDIA_ACHIEVE_MISC_SEA_COMBATS_WON",
				}

				if miscMap.has_key(szValue):
					szText = localText.getText(miscMap[szValue], ())
					if szText != "" and szText != miscMap[szValue]:
						return szText
		except:
			pass

		return szValue

	def _getAvailableAchieveSearchValues(self):
		if self.achieveSearchCategory == "ALL":
			return []

		values = []

		for iAchieve in range(gc.getNumAchieveInfos()):
			data = self._getAchieveSearchData(iAchieve)

			if self.achieveSearchCategory == "UNITS":
				for value in data["units"]:
					if value not in values:
						values.append(value)

			elif self.achieveSearchCategory == "BUILDINGS":
				for value in data["buildings"]:
					if value not in values:
						values.append(value)

			elif self.achieveSearchCategory == "PROFESSIONS":
				for value in data["professions"]:
					if value not in values:
						values.append(value)

			elif self.achieveSearchCategory == "GOODS":
				for value in data["goods"]:
					if value not in values:
						values.append(value)

			elif self.achieveSearchCategory == "MISC":
				for value in data["misc"]:
					if value not in values:
						values.append(value)

		values.sort()
		return values

	def _matchesAchieveSearchFilter(self, iAchieve):
		data = self._getAchieveSearchData(iAchieve)

		# Global: alles anzeigen
		if self.achieveSearchCategory == "ALL":
			return True

		# Units
		if self.achieveSearchCategory == "UNITS":
			if self.achieveSearchValue == "":
				return len(data["units"]) > 0
			return self.achieveSearchValue in data["units"]

		# Buildings
		if self.achieveSearchCategory == "BUILDINGS":
			if self.achieveSearchValue == "":
				return len(data["buildings"]) > 0
			return self.achieveSearchValue in data["buildings"]

		# Professions
		if self.achieveSearchCategory == "PROFESSIONS":
			if self.achieveSearchValue == "":
				return len(data["professions"]) > 0
			return self.achieveSearchValue in data["professions"]

		# Goods (inkl. Trade!)
		if self.achieveSearchCategory == "GOODS":
			if self.achieveSearchValue == "":
				return len(data["goods"]) > 0
			return self.achieveSearchValue in data["goods"]

		# Misc (Discovery etc.)
		if self.achieveSearchCategory == "MISC":
			if self.achieveSearchValue == "":
				return len(data["misc"]) > 0
			return self.achieveSearchValue in data["misc"]

		return True
# Achieve Search function end                               

	# Screen construction function
	def showScreen(self, iCategory):
		self.iCategory = iCategory

		self.deleteAllWidgets()

		screen = self.getScreen()

		bNotActive = (not screen.isActive())
		if bNotActive:
			self.setPediaCommonWidgets()

		# Header...
		szHeader = u"<font=4b>" +localText.getText("TXT_KEY_WIDGET_HELP", ()).upper() + u"</font>"
		szHeaderId = self.getNextWidgetName()
		screen.setLabel(szHeaderId, "Background", szHeader, CvUtil.FONT_CENTER_JUSTIFY, self.W_SCREEN / 2, self.Y_TITLE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_PEDIA_DESCRIPTION, -1, -1)

		self.panelName = self.getNextWidgetName()
		screen.addPanel(self.panelName, "", "", False, False, self.X_ITEMS_PANE, self.Y_ITEMS_PANE, self.W_ITEMS_PANE, self.H_ITEMS_PANE, PanelStyles.PANEL_STYLE_BLUE50, WidgetTypes.WIDGET_GENERAL, -1, -1)
		if self.iLastScreen	!= CvScreenEnums.PEDIA_MAIN or bNotActive:
			self.placeLinks(True)
			self.iLastScreen = CvScreenEnums.PEDIA_MAIN
		else:
			self.placeLinks(True)

		if (self.mapCategories.has_key(iCategory)):
			self.mapCategories.get(iCategory)()

	def placeUnits(self):
		screen = self.getScreen()
		# Create and place a unit pane
		list = self.getSortedList( gc.getNumUnitInfos(), gc.getUnitInfo )

		if GlobalDefines.CIVILOPEDIA_SHOW_ACTIVE_CIVS_ONLY and gc.getGame().isFinalInitialized():
			listCopy = list[:]
			for item in listCopy:
				if not gc.getGame().isUnitEverActive(item[1]):
					list.remove(item)

		ButtonSize = 24
		nColumns = 4
		nEntries = len(list)
		nRows = nEntries // nColumns
		if (nEntries % nColumns):
			nRows += 1
		tableName = self.getNextWidgetName()
		screen.addTableControlGFC(tableName, nColumns, self.X_ITEMS_PANE, self.Y_ITEMS_PANE + 5, self.W_ITEMS_PANE, self.H_ITEMS_PANE-5, False, False, ButtonSize, ButtonSize, TableStyles.TABLE_STYLE_STANDARD);
		screen.enableSelect(tableName, False)
		for i in range(nColumns):
			screen.setTableColumnHeader(tableName, i, "", self.W_ITEMS_PANE/nColumns)

		iCounter = 0
		iNumRows = 0
		for item in list:
			iRow = iCounter % nRows
			iColumn = iCounter // nRows
			if iRow >= iNumRows:
				iNumRows += 1
				screen.appendTableRow(tableName)
				screen.setTableRowHeight(tableName, iRow, ButtonSize)
			szButton = gc.getUnitInfo(item[1]).getButton()
			if self.iActivePlayer != -1:
				szButton = gc.getPlayer(self.iActivePlayer).getUnitButton(item[1])
			screen.setTableText(tableName, iColumn, iRow, u"<font=3>" + item[0] + u"</font>", szButton, WidgetTypes.WIDGET_PEDIA_JUMP_TO_UNIT, item[1], -1, CvUtil.FONT_LEFT_JUSTIFY)
			iCounter += 1

	def placeProfessions(self):
		screen = self.getScreen()
		# Create and place a profession pane
		list = self.getSortedList( gc.getNumProfessionInfos(), gc.getProfessionInfo )

		ButtonSize = 24
		nColumns = 4
		nEntries = len(list)
		nEntries -= 1	# TAC - Hide Whaling Profession - koma13
		nRows = nEntries // nColumns
		if (nEntries % nColumns):
			nRows += 1
		tableName = self.getNextWidgetName()
		screen.addTableControlGFC(tableName, nColumns, self.X_ITEMS_PANE, self.Y_ITEMS_PANE + 5, self.W_ITEMS_PANE, self.H_ITEMS_PANE-5, False, False, ButtonSize, ButtonSize, TableStyles.TABLE_STYLE_STANDARD);
		screen.enableSelect(tableName, False)
		for i in range(nColumns):
			screen.setTableColumnHeader(tableName, i, "", self.W_ITEMS_PANE/nColumns)

		iCounter = 0
		iNumRows = 0
		for item in list:
			if not self.pediaProfessionScreen.isWhalingProfession(item[1]):		# TAC - Hide Whaling Profession - koma13
				iRow = iCounter % nRows
				iColumn = iCounter // nRows
				if iRow >= iNumRows:
					iNumRows += 1
					screen.appendTableRow(tableName)
					screen.setTableRowHeight(tableName, iRow, ButtonSize)
				screen.setTableText(tableName, iColumn, iRow, u"<font=3>" + item[0] + u"</font>", gc.getProfessionInfo(item[1]).getButton(), WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROFESSION, item[1], 1, CvUtil.FONT_LEFT_JUSTIFY)
				iCounter += 1

	def placeBuildings(self):
		screen = self.getScreen()
		# Create and place a building pane
		list = self.pediaBuildingScreen.getBuildingSortedList()

		if GlobalDefines.CIVILOPEDIA_SHOW_ACTIVE_CIVS_ONLY and gc.getGame().isFinalInitialized():
			listCopy = list[:]
			for item in listCopy:
				if not gc.getGame().isBuildingEverActive(item[1]):
					list.remove(item)

		ButtonSize = 24
		nColumns = 4
		nEntries = len(list)
		nRows = nEntries // nColumns
		if (nEntries % nColumns):
			nRows += 1
		tableName = self.getNextWidgetName()
		screen.addTableControlGFC(tableName, nColumns, self.X_ITEMS_PANE, self.Y_ITEMS_PANE+5, self.W_ITEMS_PANE, self.H_ITEMS_PANE-5, False, False, ButtonSize, ButtonSize, TableStyles.TABLE_STYLE_STANDARD);
		screen.enableSelect(tableName, False)
		for i in range(nColumns):
			screen.setTableColumnHeader(tableName, i, "", self.W_ITEMS_PANE/nColumns)

		iCounter = 0
		iNumRows = 0
		for item in list:
			iRow = iCounter % nRows
			iColumn = iCounter // nRows
			if iRow >= iNumRows:
				iNumRows += 1
				screen.appendTableRow(tableName)
				screen.setTableRowHeight(tableName, iRow, ButtonSize)
			screen.setTableText(tableName, iColumn, iRow, u"<font=3>" + item[0] + u"</font>", gc.getBuildingInfo(item[1]).getButton(), WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING, item[1], 1, CvUtil.FONT_LEFT_JUSTIFY)
			iCounter += 1

	def placeBoni(self):
		screen = self.getScreen()
		# Create and place a boni pane
		list = self.getSortedList( gc.getNumBonusInfos(), gc.getBonusInfo )

		ButtonSize = 24
		nColumns = 3
		nEntries = len(list)
		nRows = nEntries // nColumns
		if (nEntries % nColumns):
			nRows += 1
		tableName = self.getNextWidgetName()
		screen.addTableControlGFC(tableName, nColumns, self.X_ITEMS_PANE, self.Y_ITEMS_PANE+5, self.W_ITEMS_PANE, self.H_ITEMS_PANE-5, False, False, ButtonSize, ButtonSize, TableStyles.TABLE_STYLE_STANDARD);
		screen.enableSelect(tableName, False)
		for i in range(nColumns):
			screen.setTableColumnHeader(tableName, i, "", self.W_ITEMS_PANE/nColumns)

		iCounter = 0
		iNumRows = 0
		for item in list:
			iRow = iCounter % nRows
			iColumn = iCounter // nRows
			if iRow >= iNumRows:
				iNumRows += 1
				screen.appendTableRow(tableName)
				screen.setTableRowHeight(tableName, iRow, ButtonSize)
			screen.setTableText(tableName, iColumn, iRow, u"<font=3>" + item[0] + u"</font>", gc.getBonusInfo(item[1]).getButton(), WidgetTypes.WIDGET_PEDIA_JUMP_TO_BONUS, item[1], 1, CvUtil.FONT_LEFT_JUSTIFY)
			iCounter += 1

	def placeImprovements(self):
		screen = self.getScreen()
		# Create and place a improvements pane
		list = self.getSortedList( gc.getNumImprovementInfos(), gc.getImprovementInfo )

		ButtonSize = 24
		nColumns = 2
		nEntries = len(list)
		nRows = nEntries // nColumns
		if (nEntries % nColumns):
			nRows += 1
		tableName = self.getNextWidgetName()
		screen.addTableControlGFC(tableName, nColumns, self.X_ITEMS_PANE, self.Y_ITEMS_PANE+5, self.W_ITEMS_PANE, self.H_ITEMS_PANE-5, False, False, ButtonSize, ButtonSize, TableStyles.TABLE_STYLE_STANDARD);
		screen.enableSelect(tableName, False)
		for i in range(nColumns):
			screen.setTableColumnHeader(tableName, i, "", self.W_ITEMS_PANE/nColumns)

		iCounter = 0
		iNumRows = 0
		for item in list:
			iRow = iCounter % nRows
			iColumn = iCounter // nRows
			if iRow >= iNumRows:
				iNumRows += 1
				screen.appendTableRow(tableName)
				screen.setTableRowHeight(tableName, iRow, ButtonSize)
			screen.setTableText(tableName, iColumn, iRow, u"<font=3>" + item[0] + u"</font>", gc.getImprovementInfo(item[1]).getButton(), WidgetTypes.WIDGET_PEDIA_JUMP_TO_IMPROVEMENT, item[1], 1, CvUtil.FONT_LEFT_JUSTIFY)
			iCounter += 1

	def placePromotions(self):
		screen = self.getScreen()
		# Create and place a promotions pane
		list = self.getSortedList( gc.getNumPromotionInfos(), gc.getPromotionInfo )

		ButtonSize = 24
		nColumns = 3
		nEntries = len(list)
		nRows = nEntries // nColumns
		if (nEntries % nColumns):
			nRows += 1
		tableName = self.getNextWidgetName()
		screen.addTableControlGFC(tableName, nColumns, self.X_ITEMS_PANE, self.Y_ITEMS_PANE+5, self.W_ITEMS_PANE, self.H_ITEMS_PANE-5, False, False, ButtonSize, ButtonSize, TableStyles.TABLE_STYLE_STANDARD);
		screen.enableSelect(tableName, False)
		for i in range(nColumns):
			screen.setTableColumnHeader(tableName, i, "", self.W_ITEMS_PANE/nColumns)

		iCounter = 0
		iNumRows = 0
		for item in list:
			iRow = iCounter % nRows
			iColumn = iCounter // nRows
			if iRow >= iNumRows:
				iNumRows += 1
				screen.appendTableRow(tableName)
				screen.setTableRowHeight(tableName, iRow, ButtonSize)
			screen.setTableText(tableName, iColumn, iRow, u"<font=3>" + item[0] + u"</font>", gc.getPromotionInfo(item[1]).getButton(), WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROMOTION, item[1], 1, CvUtil.FONT_LEFT_JUSTIFY)
			iCounter += 1

	def placeCivs(self):
		screen = self.getScreen()
		# Create and place a civs pane
		list = self.getSortedList( gc.getNumCivilizationInfos(), gc.getCivilizationInfo )

		if GlobalDefines.CIVILOPEDIA_SHOW_ACTIVE_CIVS_ONLY and gc.getGame().isFinalInitialized():
			listCopy = list[:]
			for item in listCopy:
				if not gc.getGame().isCivEverActive(item[1]):
					list.remove(item)

		ButtonSize = 24
		nColumns = 2
		nEntries = len(list)
		nRows = nEntries // nColumns
		if (nEntries % nColumns):
			nRows += 1
		tableName = self.getNextWidgetName()
		screen.addTableControlGFC(tableName, nColumns, self.X_ITEMS_PANE, self.Y_ITEMS_PANE+5, self.W_ITEMS_PANE, self.H_ITEMS_PANE-5, False, False, ButtonSize, ButtonSize, TableStyles.TABLE_STYLE_STANDARD);
		screen.enableSelect(tableName, False)
		for i in range(nColumns):
			screen.setTableColumnHeader(tableName, i, "", self.W_ITEMS_PANE/nColumns)

		iCounter = 0
		iNumRows = 0
		for item in list:
			iRow = iCounter % nRows
			iColumn = iCounter // nRows
			# R&R, ray, Church
			# if (not gc.getCivilizationInfo(item[1]).isEurope() and not item[1] == gc.getInfoTypeForString("CIVILIZATION_BARBARIAN")):
			if not gc.getCivilizationInfo(item[1]).isEurope() and not item[1] == CivilizationTypes.CIVILIZATION_BARBARIAN and not item[1] == CivilizationTypes.CIVILIZATION_CHURCH:
				if iRow >= iNumRows:
					iNumRows += 1
					screen.appendTableRow(tableName)
					screen.setTableRowHeight(tableName, iRow, ButtonSize)
				screen.setTableText(tableName, iColumn, iRow, u"<font=3>" + item[0] + u"</font>", gc.getCivilizationInfo(item[1]).getButton(), WidgetTypes.WIDGET_PEDIA_JUMP_TO_CIV, item[1], 1, CvUtil.FONT_LEFT_JUSTIFY)
				iCounter += 1

	def placeLeaders(self):
		screen = self.getScreen()
		# Create and place a leaders pane
		list = self.getSortedList( gc.getNumLeaderHeadInfos(), gc.getLeaderHeadInfo )
		listCopy = list[:]
		for item in listCopy:
			if GlobalDefines.CIVILOPEDIA_SHOW_ACTIVE_CIVS_ONLY and gc.getGame().isFinalInitialized():
				if not gc.getGame().isLeaderEverActive(item[1]):
					list.remove(item)

		ButtonSize = 24
		nColumns = 2
		nEntries = len(list)
		nRows = nEntries // nColumns
		if (nEntries % nColumns):
			nRows += 1
		tableName = self.getNextWidgetName()
		screen.addTableControlGFC(tableName, nColumns, self.X_ITEMS_PANE, self.Y_ITEMS_PANE+5, self.W_ITEMS_PANE, self.H_ITEMS_PANE-5, False, False, ButtonSize, ButtonSize, TableStyles.TABLE_STYLE_STANDARD);
		screen.enableSelect(tableName, False)
		for i in range(nColumns):
			screen.setTableColumnHeader(tableName, i, "", self.W_ITEMS_PANE/nColumns)

		iCounter = 0
		iNumRows = 0
		for item in list:
			iRow = iCounter % nRows
			iColumn = iCounter // nRows
			if iRow >= iNumRows:
				iNumRows += 1
				screen.appendTableRow(tableName)
				screen.setTableRowHeight(tableName, iRow, ButtonSize)

			iNumCivs = 0
			iLeaderCiv = -1
			for iCiv in range(gc.getNumCivilizationInfos()):
				civ = gc.getCivilizationInfo(iCiv)
				if civ.isLeaders(item[1]):
					iNumCivs += 1
					iLeaderCiv = iCiv

			if iNumCivs != 1:
				iLeaderCiv = -1

			screen.setTableText(tableName, iColumn, iRow, u"<font=3>" + item[0] + u"</font>", gc.getLeaderHeadInfo(item[1]).getButton(), WidgetTypes.WIDGET_PEDIA_JUMP_TO_LEADER, item[1], iLeaderCiv, CvUtil.FONT_LEFT_JUSTIFY)
			iCounter += 1

	def placeCivics(self):
		screen = self.getScreen()
		# Create and place a civics pane
		list = self.getSortedList( gc.getNumCivicInfos(), gc.getCivicInfo )

		ButtonSize = 24
		nColumns = 1
		nEntries = len(list)
		nRows = nEntries // nColumns
		if (nEntries % nColumns):
			nRows += 1
		tableName = self.getNextWidgetName()
		screen.addTableControlGFC(tableName, nColumns, self.X_ITEMS_PANE, self.Y_ITEMS_PANE+5, self.W_ITEMS_PANE, self.H_ITEMS_PANE-5, False, False, ButtonSize, ButtonSize, TableStyles.TABLE_STYLE_STANDARD);
		screen.enableSelect(tableName, False)
		for i in range(nColumns):
			screen.setTableColumnHeader(tableName, i, "", self.W_ITEMS_PANE/nColumns)

		iCounter = 0
		iNumRows = 0
		for item in list:
			iRow = iCounter % nRows
			iColumn = iCounter // nRows
			if iRow >= iNumRows:
				iNumRows += 1
				screen.appendTableRow(tableName)
				screen.setTableRowHeight(tableName, iRow, ButtonSize)
			screen.setTableText(tableName, iColumn, iRow, u"<font=3>" + item[0] + u"</font>", gc.getCivicInfo(item[1]).getButton(), WidgetTypes.WIDGET_PEDIA_JUMP_TO_CIVIC, item[1], 1, CvUtil.FONT_LEFT_JUSTIFY)
			iCounter += 1

	def placeTerrains(self):
		screen = self.getScreen()
		# Create and place a terrain pane
		list = self.getSortedList( gc.getNumTerrainInfos(), gc.getTerrainInfo )

		ButtonSize = 24
		nColumns = 1
		nEntries = len(list)
		nRows = nEntries // nColumns
		if (nEntries % nColumns):
			nRows += 1
		tableName = self.getNextWidgetName()
		screen.addTableControlGFC(tableName, nColumns, self.X_ITEMS_PANE, self.Y_ITEMS_PANE+5, self.W_ITEMS_PANE, self.H_ITEMS_PANE-5, False, False, ButtonSize, ButtonSize, TableStyles.TABLE_STYLE_STANDARD);
		screen.enableSelect(tableName, False)
		for i in range(nColumns):
			screen.setTableColumnHeader(tableName, i, "", self.W_ITEMS_PANE/nColumns)

		iCounter = 0
		iNumRows = 0
		for item in list:
			iColumn = iCounter // nRows
			iRow = iCounter % nRows
			if iRow >= iNumRows:
				iNumRows += 1
				screen.appendTableRow(tableName)
				screen.setTableRowHeight(tableName, iRow, ButtonSize)
			screen.setTableText(tableName, iColumn, iRow, u"<font=3>" + item[0] + u"</font>", gc.getTerrainInfo(item[1]).getButton(), WidgetTypes.WIDGET_PEDIA_JUMP_TO_TERRAIN, item[1], 1, CvUtil.FONT_LEFT_JUSTIFY)
			iCounter += 1

	def placeYields(self):
		screen = self.getScreen()
		# Create and place a terrain pane
		list = self.getSortedList( gc.getNumYieldInfos(), gc.getYieldInfo )

		ButtonSize = 24
		nColumns = 3
		nEntries = len(list)
		nRows = nEntries // nColumns
		if (nEntries % nColumns):
			nRows += 1
		tableName = self.getNextWidgetName()
		screen.addTableControlGFC(tableName, nColumns, self.X_ITEMS_PANE, self.Y_ITEMS_PANE+5, self.W_ITEMS_PANE, self.H_ITEMS_PANE-5, False, False, ButtonSize, ButtonSize, TableStyles.TABLE_STYLE_STANDARD);
		screen.enableSelect(tableName, False)
		for i in range(nColumns):
			screen.setTableColumnHeader(tableName, i, "", self.W_ITEMS_PANE/nColumns)

		iCounter = 0
		iNumRows = 0
		for item in list:
			iColumn = iCounter // nRows
			iRow = iCounter % nRows
			if iRow >= iNumRows:
				iNumRows += 1
				screen.appendTableRow(tableName)
				screen.setTableRowHeight(tableName, iRow, ButtonSize)
			screen.setTableText(tableName, iColumn, iRow, u"<font=3>" + item[0] + u"</font>", gc.getYieldInfo(item[1]).getButton(), WidgetTypes.WIDGET_PEDIA_JUMP_TO_YIELDS, item[1], 1, CvUtil.FONT_LEFT_JUSTIFY)
			iCounter += 1

	def placeFeatures(self):
		screen = self.getScreen()
		# Create and place a features pane
		list = self.getSortedList( gc.getNumFeatureInfos(), gc.getFeatureInfo )

		ButtonSize = 24
		nColumns = 2
		nEntries = len(list)
		nRows = nEntries // nColumns
		if (nEntries % nColumns):
			nRows += 1
		tableName = self.getNextWidgetName()
		screen.addTableControlGFC(tableName, nColumns, self.X_ITEMS_PANE, self.Y_ITEMS_PANE+5, self.W_ITEMS_PANE, self.H_ITEMS_PANE-5, False, False, ButtonSize, ButtonSize, TableStyles.TABLE_STYLE_STANDARD);
		screen.enableSelect(tableName, False)
		for i in range(nColumns):
			screen.setTableColumnHeader(tableName, i, "", self.W_ITEMS_PANE/nColumns)

		iCounter = 0
		iNumRows = 0
		for item in list:
			iColumn = iCounter // nRows
			iRow = iCounter % nRows
			if iRow >= iNumRows:
				iNumRows += 1
				screen.appendTableRow(tableName)
				screen.setTableRowHeight(tableName, iRow, ButtonSize)
			screen.setTableText(tableName, iColumn, iRow, u"<font=3>" + item[0] + u"</font>", gc.getFeatureInfo(item[1]).getButton(), WidgetTypes.WIDGET_PEDIA_JUMP_TO_FEATURE, item[1], 1, CvUtil.FONT_LEFT_JUSTIFY)
			iCounter += 1

	def placeFathers(self):
		screen = self.getScreen()
		# Create and place a features pane
		list = self.getSortedList( gc.getNumFatherInfos(), gc.getFatherInfo )

		ButtonSize = 24
		nColumns = 4
		nEntries = len(list)
		nRows = nEntries // nColumns
		if (nEntries % nColumns):
			nRows += 1
		tableName = self.getNextWidgetName()
		screen.addTableControlGFC(tableName, nColumns, self.X_ITEMS_PANE, self.Y_ITEMS_PANE+5, self.W_ITEMS_PANE, self.H_ITEMS_PANE-5, False, False, ButtonSize, ButtonSize, TableStyles.TABLE_STYLE_STANDARD);
		screen.enableSelect(tableName, False)
		for i in range(nColumns):
			screen.setTableColumnHeader(tableName, i, "", self.W_ITEMS_PANE/nColumns)

		iCounter = 0
		iNumRows = 0
		for item in list:
			iColumn = iCounter // nRows
			iRow = iCounter % nRows
			if iRow >= iNumRows:
				iNumRows += 1
				screen.appendTableRow(tableName)
				screen.setTableRowHeight(tableName, iRow, ButtonSize)
			screen.setTableText(tableName, iColumn, iRow, u"<font=3>" + item[0] + u"</font>", gc.getFatherInfo(item[1]).getButton(), WidgetTypes.WIDGET_PEDIA_JUMP_TO_FATHER, item[1], -1, CvUtil.FONT_LEFT_JUSTIFY)
			iCounter += 1

	def placeConcepts(self):
		screen = self.getScreen()
		# Create and place a concepts pane
		list = self.getSortedList( gc.getNumConceptInfos(), gc.getConceptInfo )

		ButtonSize = 24
		nColumns = 3
		nEntries = len(list)
		nRows = nEntries // nColumns
		if (nEntries % nColumns):
			nRows += 1
		tableName = self.getNextWidgetName()
		screen.addTableControlGFC(tableName, nColumns, self.X_ITEMS_PANE, self.Y_ITEMS_PANE + 5, self.W_ITEMS_PANE, self.H_ITEMS_PANE-5, False, False, ButtonSize, ButtonSize, TableStyles.TABLE_STYLE_STANDARD);
		screen.enableSelect(tableName, False)
		for i in range(nColumns):
			screen.setTableColumnHeader(tableName, i, "", self.W_ITEMS_PANE/nColumns)

		iCounter = 0
		iNumRows = 0
		for item in list:
			iColumn = iCounter // nRows
			iRow = iCounter % nRows
			if iRow >= iNumRows:
				iNumRows += 1
				screen.appendTableRow(tableName)
				screen.setTableRowHeight(tableName, iRow, ButtonSize)
			screen.setTableText(tableName, iColumn, iRow, u"<font=3>" + item[0] + u"</font>", gc.getConceptInfo(item[1]).getButton(), WidgetTypes.WIDGET_PEDIA_DESCRIPTION, CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT, item[1], CvUtil.FONT_LEFT_JUSTIFY)
			iCounter += 1

	def placeAchievements(self):
		screen = self.getScreen()

		list = self.getSortedList(gc.getNumAchieveInfos(), gc.getAchieveInfo)

		filterPanelX = self.X_ITEMS_PANE + 10
		filterPanelY = self.Y_ITEMS_PANE + 10
		filterPanelW = self.W_ITEMS_PANE - 20
		filterPanelH = 140

		categoryListX = filterPanelX + 10
		categoryListY = filterPanelY + 28
		categoryListW = 260
		categoryListH = 96

		valueListX = categoryListX + categoryListW + 15
		valueListY = categoryListY
		valueListW = 420
		valueListH = 96

		screen.addPanel(self.getNextWidgetName(),
			"", "", True, True,
			filterPanelX, filterPanelY, filterPanelW, filterPanelH,
			PanelStyles.PANEL_STYLE_BLUE50,
			WidgetTypes.WIDGET_GENERAL, -1, -1)

		szCategoryTitle = localText.getText("TXT_KEY_PEDIA_ACHIEVE_FILTER_CATEGORY", ())
		if szCategoryTitle == "TXT_KEY_PEDIA_ACHIEVE_FILTER_CATEGORY":
			szCategoryTitle = u"Category"

		szValueTitle = localText.getText("TXT_KEY_PEDIA_ACHIEVE_FILTER_VALUE", ())
		if szValueTitle == "TXT_KEY_PEDIA_ACHIEVE_FILTER_VALUE":
			szValueTitle = u"Entry"

		screen.setLabel(self.getNextWidgetName(), "Background",
			u"<font=3b>" + szCategoryTitle + u"</font>",
			CvUtil.FONT_LEFT_JUSTIFY,
			categoryListX, filterPanelY + 8, 0,
			FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		screen.setLabel(self.getNextWidgetName(), "Background",
			u"<font=3b>" + szValueTitle + u"</font>",
			CvUtil.FONT_LEFT_JUSTIFY,
			valueListX, filterPanelY + 8, 0,
			FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		categoryListName = self.getNextWidgetName()
		screen.addListBoxGFC(categoryListName, "", categoryListX, categoryListY, categoryListW, categoryListH, TableStyles.TABLE_STYLE_STANDARD)
		screen.enableSelect(categoryListName, True)
		screen.setStyle(categoryListName, "Table_StandardCiv_Style")

		iSelectedCategory = 0
		for iIndex, item in enumerate(self.achieveSearchCategories):
			szCode = item[0]
			szText = self._getAchieveSearchLabel(szCode)

			screen.appendListBoxStringNoUpdate(
				categoryListName,
				u"<font=3>" + szText + u"</font>",
				WidgetTypes.WIDGET_GENERAL,
				self.ACHIEVE_SEARCH_CATEGORY_WIDGET,
				iIndex,
				CvUtil.FONT_LEFT_JUSTIFY)

			if szCode == self.achieveSearchCategory:
				iSelectedCategory = iIndex

		screen.updateListBox(categoryListName)
		screen.setSelectedListBoxStringGFC(categoryListName, iSelectedCategory)

		valueListName = self.getNextWidgetName()
		screen.addListBoxGFC(valueListName, "", valueListX, valueListY, valueListW, valueListH, TableStyles.TABLE_STYLE_STANDARD)
		screen.enableSelect(valueListName, True)
		screen.setStyle(valueListName, "Table_StandardCiv_Style")

		values = self._getAvailableAchieveSearchValues()

		# Dynamic "All" label depending on category
		if self.achieveSearchCategory == "UNITS":
			szAll = localText.getText("TXT_KEY_PEDIA_ACHIEVE_FILTER_ALL_UNITS", ())

		elif self.achieveSearchCategory == "BUILDINGS":
			szAll = localText.getText("TXT_KEY_PEDIA_ACHIEVE_FILTER_ALL_BUILDINGS", ())

		elif self.achieveSearchCategory == "PROFESSIONS":
			szAll = localText.getText("TXT_KEY_PEDIA_ACHIEVE_FILTER_ALL_PROFESSIONS", ())

		elif self.achieveSearchCategory == "GOODS":
			szAll = localText.getText("TXT_KEY_PEDIA_ACHIEVE_FILTER_ALL_GOODS", ())

		elif self.achieveSearchCategory == "MISC":
			szAll = localText.getText("TXT_KEY_PEDIA_ACHIEVE_FILTER_ALL_MISC", ())

		else:
			szAll = localText.getText("TXT_KEY_PEDIA_ACHIEVE_FILTER_ALL", ())

		screen.appendListBoxStringNoUpdate(
			valueListName,
			u"<font=3>" + szAll + u"</font>",
			WidgetTypes.WIDGET_GENERAL,
			self.ACHIEVE_SEARCH_VALUE_WIDGET,
			-1,
			CvUtil.FONT_LEFT_JUSTIFY)

		iSelectedValue = 0
		for iIndex, value in enumerate(values):
			szDisplay = self._getAchieveSearchValueDisplayName(self.achieveSearchCategory, value)

			screen.appendListBoxStringNoUpdate(
				valueListName,
				u"<font=3>" + szDisplay + u"</font>",
				WidgetTypes.WIDGET_GENERAL,
				self.ACHIEVE_SEARCH_VALUE_WIDGET,
				iIndex,
				CvUtil.FONT_LEFT_JUSTIFY)

			if value == self.achieveSearchValue:
				iSelectedValue = iIndex + 1

		screen.updateListBox(valueListName)
		screen.setSelectedListBoxStringGFC(valueListName, iSelectedValue)

		filteredList = []
		for item in list:
			if self._matchesAchieveSearchFilter(item[1]):
				filteredList.append(item)

		nColumns = 3
		nEntries = len(filteredList)
		nRows = nEntries // nColumns
		if (nEntries % nColumns):
			nRows += 1

		colSpacing = 10
		totalW = self.W_ITEMS_PANE - 20
		colW = (totalW - (colSpacing * (nColumns - 1))) / nColumns
		colH = self.H_ITEMS_PANE - 20 - filterPanelH - 10
		startX = self.X_ITEMS_PANE + 10
		startY = self.Y_ITEMS_PANE + 10 + filterPanelH + 10

		listNames = []
		for i in range(nColumns):
			colX = startX + i * (colW + colSpacing)
			listName = self.getNextWidgetName()
			listNames.append(listName)

			screen.addListBoxGFC(
				listName,
				"",
				colX,
				startY,
				colW,
				colH,
				TableStyles.TABLE_STYLE_STANDARD)

			screen.enableSelect(listName, True)
			screen.setStyle(listName, "Table_StandardCiv_Style")

		iIndex = 0
		for iColumn in range(nColumns):
			for iRow in range(nRows):
				if iIndex >= nEntries:
					break

				item = filteredList[iIndex]
				screen.appendListBoxStringNoUpdate(
					listNames[iColumn],
					u"<font=3>" + item[0] + u"</font>",
					WidgetTypes.WIDGET_GENERAL,
					CvScreenEnums.PEDIA_ACHIEVE,
					item[1],
					CvUtil.FONT_LEFT_JUSTIFY)

				iIndex += 1

			screen.updateListBox(listNames[iColumn])

	def placeHints(self):
		screen = self.getScreen()

		self.szAreaId = self.getNextWidgetName()
		screen.addListBoxGFC( self.szAreaId, "", self.X_ITEMS_PANE, self.Y_ITEMS_PANE, self.W_ITEMS_PANE, self.H_ITEMS_PANE, TableStyles.TABLE_STYLE_STANDARD )

		szHintsText = CyGameTextMgr().buildHintsList()
		hintText = string.split( szHintsText, "\n" )
		for hint in hintText:
			if len( hint ) != 0:
				screen.appendListBoxStringNoUpdate( self.szAreaId, hint, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )

		screen.updateListBox(self.szAreaId)

		
	def placeLinks(self, bRedraw):

		screen = self.getScreen()

		if bRedraw:
			screen.clearListBoxGFC(self.LIST_ID)

		i = 0
		for szCategory in self.listCategories:
			if bRedraw:
				screen.appendListBoxStringNoUpdate(self.LIST_ID, szCategory, WidgetTypes.WIDGET_PEDIA_MAIN, i, 0, CvUtil.FONT_LEFT_JUSTIFY )
			i += 1

		if bRedraw:
			screen.updateListBox(self.LIST_ID)

		screen.setSelectedListBoxStringGFC(self.LIST_ID, self.iCategory)

	# returns a unique ID for a widget in this screen
	def getNextWidgetName(self):
		szName = self.WIDGET_ID + str(self.nWidgetCount)
		self.nWidgetCount += 1
		return szName

	def pediaJump(self, iScreen, iEntry, bRemoveFwdList):
		
		if (iEntry < 0):
			return

		self.iActivePlayer = gc.getGame().getActivePlayer()
		self.setPediaCommonWidgets()

		self.pediaHistory.append((iScreen, iEntry))
		if (bRemoveFwdList):
			self.pediaFuture = []

		if (iScreen == CvScreenEnums.PEDIA_MAIN):
			self.showScreen(iEntry)
		elif (iScreen == CvScreenEnums.PEDIA_UNIT):
			self.pediaUnitScreen.interfaceScreen(iEntry, self.X_ITEMS_PANE, self.Y_ITEMS_PANE, self.H_ITEMS_PANE, self.W_ITEMS_PANE)
		elif (iScreen == CvScreenEnums.PEDIA_PROFESSION):
			self.pediaProfessionScreen.interfaceScreen(iEntry, self.X_ITEMS_PANE, self.Y_ITEMS_PANE, self.H_ITEMS_PANE, self.W_ITEMS_PANE)
		elif (iScreen == CvScreenEnums.PEDIA_BUILDING):
			self.pediaBuildingScreen.interfaceScreen(iEntry, self.X_ITEMS_PANE, self.Y_ITEMS_PANE, self.H_ITEMS_PANE, self.W_ITEMS_PANE)
		elif (iScreen == CvScreenEnums.PEDIA_PROMOTION):
			self.pediaPromotionScreen.interfaceScreen(iEntry, self.X_ITEMS_PANE, self.Y_ITEMS_PANE, self.H_ITEMS_PANE, self.W_ITEMS_PANE)
		elif (iScreen == CvScreenEnums.PEDIA_BONUS):
			self.pediaBonus.interfaceScreen(iEntry, self.X_ITEMS_PANE, self.Y_ITEMS_PANE, self.H_ITEMS_PANE, self.W_ITEMS_PANE)
		elif (iScreen == CvScreenEnums.PEDIA_TERRAIN):
			self.pediaTerrain.interfaceScreen(iEntry, self.X_ITEMS_PANE, self.Y_ITEMS_PANE, self.H_ITEMS_PANE, self.W_ITEMS_PANE)
		elif (iScreen == CvScreenEnums.PEDIA_YIELDS):
			self.pediaYields.interfaceScreen(iEntry, self.X_ITEMS_PANE, self.Y_ITEMS_PANE, self.H_ITEMS_PANE, self.W_ITEMS_PANE)
		elif (iScreen == CvScreenEnums.PEDIA_FEATURE):
			self.pediaFeature.interfaceScreen(iEntry, self.X_ITEMS_PANE, self.Y_ITEMS_PANE, self.H_ITEMS_PANE, self.W_ITEMS_PANE)
		elif (iScreen == CvScreenEnums.PEDIA_FATHER):
			self.pediaFatherScreen.interfaceScreen(iEntry, self.X_ITEMS_PANE, self.Y_ITEMS_PANE, self.H_ITEMS_PANE, self.W_ITEMS_PANE)
		elif (iScreen == CvScreenEnums.PEDIA_IMPROVEMENT):
			self.pediaImprovement.interfaceScreen(iEntry, self.X_ITEMS_PANE, self.Y_ITEMS_PANE, self.H_ITEMS_PANE, self.W_ITEMS_PANE)
		elif (iScreen == CvScreenEnums.PEDIA_CIVIC):
			self.pediaCivic.interfaceScreen(iEntry, self.X_ITEMS_PANE, self.Y_ITEMS_PANE, self.H_ITEMS_PANE, self.W_ITEMS_PANE)
		elif (iScreen == CvScreenEnums.PEDIA_CIVILIZATION):
			self.pediaCivilization.interfaceScreen(iEntry, self.X_ITEMS_PANE, self.Y_ITEMS_PANE, self.H_ITEMS_PANE, self.W_ITEMS_PANE)
		elif (iScreen == CvScreenEnums.PEDIA_LEADER):
			self.pediaLeader.interfaceScreen(iEntry, self.X_ITEMS_PANE, self.Y_ITEMS_PANE, self.H_ITEMS_PANE, self.W_ITEMS_PANE)
		elif (iScreen == CvScreenEnums.PEDIA_HISTORY):
			self.pediaHistorical.interfaceScreen(iEntry, self.X_ITEMS_PANE, self.Y_ITEMS_PANE, self.H_ITEMS_PANE, self.W_ITEMS_PANE)
		elif (iScreen == CvScreenEnums.PEDIA_ACHIEVE):
			self.pediaAchieve.interfaceScreen(iEntry, self.X_ITEMS_PANE, self.Y_ITEMS_PANE, self.H_ITEMS_PANE, self.W_ITEMS_PANE)                                                
	def back(self):
		if (len(self.pediaHistory) > 1):
			self.pediaFuture.append(self.pediaHistory.pop())
			current = self.pediaHistory.pop()
			self.pediaJump(current[0], current[1], False)
		return 1

	def forward(self):
		if (self.pediaFuture):
			current = self.pediaFuture.pop()
			self.pediaJump(current[0], current[1], False)
		return 1

	def pediaShow(self):
		if (not self.pediaHistory):
			self.pediaHistory.append((CvScreenEnums.PEDIA_MAIN, 0))

		current = self.pediaHistory.pop()

		# erase history so it doesn't grow too large during the game
		self.pediaFuture = []
		self.pediaHistory = []

		self.setPediaCommonWidgets()
		# jump to the last screen that was up
		self.pediaJump(current[0], current[1], False)

	def link(self, szLink):
		if (szLink == "PEDIA_MAIN_UNIT"):
			return self.pediaJump(CvScreenEnums.PEDIA_MAIN, int(CivilopediaPageTypes.CIVILOPEDIA_PAGE_UNIT), True)
		if (szLink == "PEDIA_MAIN_ACHIEVEMENTS"):
			return self.pediaJump(CvScreenEnums.PEDIA_MAIN, self.CATEGORY_ACHIEVEMENTS, True)
		if (szLink == "PEDIA_MAIN_PROFESSION"):
			return self.pediaJump(CvScreenEnums.PEDIA_MAIN, int(CivilopediaPageTypes.CIVILOPEDIA_PAGE_PROFESSION), True)
		if (szLink == "PEDIA_MAIN_BUILDING"):
			return self.pediaJump(CvScreenEnums.PEDIA_MAIN, int(CivilopediaPageTypes.CIVILOPEDIA_PAGE_BUILDING), True)
		if (szLink == "PEDIA_MAIN_TERRAIN"):
			return self.pediaJump(CvScreenEnums.PEDIA_MAIN, int(CivilopediaPageTypes.CIVILOPEDIA_PAGE_TERRAIN), True)
		if (szLink == "PEDIA_MAIN_YIELDS"):
			return self.pediaJump(CvScreenEnums.PEDIA_MAIN, int(CivilopediaPageTypes.CIVILOPEDIA_PAGE_YIELDS), True)
		if (szLink == "PEDIA_MAIN_FEATURE"):
			return self.pediaJump(CvScreenEnums.PEDIA_MAIN, int(CivilopediaPageTypes.CIVILOPEDIA_PAGE_FEATURE), True)
		if (szLink == "CIVILOPEDIA_PAGE_FATHER"):
			return self.pediaJump(CvScreenEnums.PEDIA_MAIN, int(CivilopediaPageTypes.CIVILOPEDIA_PAGE_FATHER), True)
		if (szLink == "PEDIA_MAIN_BONUS"):
			return self.pediaJump(CvScreenEnums.PEDIA_MAIN, int(CivilopediaPageTypes.CIVILOPEDIA_PAGE_BONUS), True)
		if (szLink == "PEDIA_MAIN_IMPROVEMENT"):
			return self.pediaJump(CvScreenEnums.PEDIA_MAIN, int(CivilopediaPageTypes.CIVILOPEDIA_PAGE_IMPROVEMENT), True)
		if (szLink == "PEDIA_MAIN_PROMOTION"):
			return self.pediaJump(CvScreenEnums.PEDIA_MAIN, int(CivilopediaPageTypes.CIVILOPEDIA_PAGE_PROMOTION), True)
		if (szLink == "PEDIA_MAIN_CIV"):
			return self.pediaJump(CvScreenEnums.PEDIA_MAIN, int(CivilopediaPageTypes.CIVILOPEDIA_PAGE_CIV), True)
		if (szLink == "PEDIA_MAIN_LEADER"):
			return self.pediaJump(CvScreenEnums.PEDIA_MAIN, int(CivilopediaPageTypes.CIVILOPEDIA_PAGE_LEADER), True)
		if (szLink == "PEDIA_MAIN_CIVIC"):
			return self.pediaJump(CvScreenEnums.PEDIA_MAIN, int(CivilopediaPageTypes.CIVILOPEDIA_PAGE_CIVIC), True)
		if (szLink == "PEDIA_MAIN_CONCEPT"):
			return self.pediaJump(CvScreenEnums.PEDIA_MAIN, int(CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT), True)
		if (szLink == "PEDIA_MAIN_HINTS"):
			return self.pediaJump(CvScreenEnums.PEDIA_MAIN, int(CivilopediaPageTypes.CIVILOPEDIA_PAGE_HINTS), True)
		
		
		for i in range(gc.getNumConceptInfos()):
			if (gc.getConceptInfo(i).isMatchForLink(szLink, False)):
				iEntryId = self.pediaHistorical.getIdFromEntryInfo(CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT, i)
				return self.pediaJump(CvScreenEnums.PEDIA_HISTORY, iEntryId, True)
		for i in range(gc.getNumUnitInfos()):
			if (gc.getUnitInfo(i).isMatchForLink(szLink, False)):
				return self.pediaJump(CvScreenEnums.PEDIA_UNIT, i, True)
		for i in range(gc.getNumProfessionInfos()):
			if (gc.getProfessionInfo(i).isMatchForLink(szLink, False)):
				return self.pediaJump(CvScreenEnums.PEDIA_PROFESSION, i, True)
		for i in range(gc.getNumBuildingInfos()):
			if (gc.getBuildingInfo(i).isMatchForLink(szLink, False)):
				return self.pediaJump(CvScreenEnums.PEDIA_BUILDING, i, True)
		for i in range(gc.getNumPromotionInfos()):
			if (gc.getPromotionInfo(i).isMatchForLink(szLink, False)):
				return self.pediaJump(CvScreenEnums.PEDIA_PROMOTION, i, True)
		for i in range(gc.getNumBonusInfos()):
			if (gc.getBonusInfo(i).isMatchForLink(szLink, False)):
				return self.pediaJump(CvScreenEnums.PEDIA_BONUS, i, True)
		for i in range(gc.getNumTerrainInfos()):
			if (gc.getTerrainInfo(i).isMatchForLink(szLink, False)):
				return self.pediaJump(CvScreenEnums.PEDIA_TERRAIN, i, True)
		for i in range(gc.getNumYieldInfos()):
			if (gc.getYieldInfo(i).isMatchForLink(szLink, False)):
				return self.pediaJump(CvScreenEnums.PEDIA_YIELDS, i, True)
		for i in range(gc.getNumFeatureInfos()):
			if (gc.getFeatureInfo(i).isMatchForLink(szLink, False)):
				return self.pediaJump(CvScreenEnums.PEDIA_FEATURE, i, True)
		for i in range(gc.getNumFatherInfos()):
			if (gc.getFatherInfo(i).isMatchForLink(szLink, False)):
				return self.pediaJump(CvScreenEnums.PEDIA_FATHER, i, True)
		for i in range(gc.getNumImprovementInfos()):
			if (gc.getImprovementInfo(i).isMatchForLink(szLink, False)):
				return self.pediaJump(CvScreenEnums.PEDIA_IMPROVEMENT, i, True)
		for i in range(gc.getNumCivicInfos()):
			if (gc.getCivicInfo(i).isMatchForLink(szLink, False)):
				return self.pediaJump(CvScreenEnums.PEDIA_CIVIC, i, True)
		for i in range(gc.getNumCivilizationInfos()):
			if (gc.getCivilizationInfo(i).isMatchForLink(szLink, False)):
				return self.pediaJump(CvScreenEnums.PEDIA_CIVILIZATION, i, True)
		for i in range(gc.getNumLeaderHeadInfos()):
			if (gc.getLeaderHeadInfo(i).isMatchForLink(szLink, False)):
				return self.pediaJump(CvScreenEnums.PEDIA_LEADER, i, True)

	def deleteAllWidgets(self):
		screen = self.getScreen()
		iNumWidgets = self.nWidgetCount
		self.nWidgetCount = 0
		for i in range(iNumWidgets):
			screen.deleteWidget(self.getNextWidgetName())
		self.nWidgetCount = 0

	# Will handle the input for this screen...
	def handleInput (self, inputClass):
        
# TAC Start
		screen = self.getScreen()			
		TextExit_ID = 13

		if (self.W_SCREEN >= 1360):
			TextFontSize = u"<font=4>"
		else:
			TextFontSize = u"<font=3>"

		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if (inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL):
# Credits Bildschirm Start
				if (inputClass.getData1() == self.CREDITS_ID):

					screen.addDDSGFC("TextBackgroundImage", ArtFileMgr.getInterfaceArtInfo("MAINMENU_SLIDESHOW_LOAD").getPath(), 0, 0, self.W_SCREEN, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
					screen.addDDSGFC("TextTopPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TITLE").getPath(), 0, 0, self.W_SCREEN, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
					screen.addDDSGFC("TextBottomPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_OFF").getPath(), 0, self.H_SCREEN - 55, self.W_SCREEN, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
					screen.addDDSGFC("TextBackgroundShadow", ArtFileMgr.getInterfaceArtInfo("INTERFACE_EUROPE_SHADOW_BOX").getPath(), 0, 0, self.W_SCREEN, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
					
					szTextTitle = u"<font=4b>" + localText.getText("TXT_KEY_CREDITS_HEAD", ()).upper() + u"</font>"
					szTextData = TextFontSize + localText.getText("[COLOR_FONT_GOLD]", ())
					
					#WTP
					list = []
					list.append("Nightinggale")
					list.append("Devolution")
					list.append("raystuttgart")
					list.append("Schmiddie")
					list.append("Mr.ZorG")
					list.append("Kendon")
					list.append("TXT_KEY_CREDITS_RAUWURGER")
					list.append("Liet")
					list.append("Tucker8")
					list.append("Aemon")
					list.append("sneed")
					list.append("Barthoze")
					list.append("victorxucp")
					list.append("TXT_KEY_CREDITS_FURSTBISCHOF")
					list.append("jooe")
					list.append("FlaviusBelisarius")
					
					szTextData += localText.getText("TXT_KEY_CREDITS_WTP", ())
					szTextData += localText.getText("[NEWLINE]", ())
					
					for szName in (list):
						szTextData += localText.getText("[ICON_BULLET] ", ())
						szTextData += localText.getText(szName, ())
						szTextData += localText.getText("[NEWLINE]", ())
					
					# special credits
					list = []
					list.append("TXT_KEY_CREDITS_f1rpo")
					list.append("TXT_KEY_CREDITS_karadoc")
					
					szTextData += localText.getText("TXT_KEY_CREDITS_SPECIAL", ())
					szTextData += localText.getText("[NEWLINE]", ())
					
					for szName in (list):
						szTextData += localText.getText("[ICON_BULLET] ", ())
						szTextData += localText.getText(szName, ())
						szTextData += localText.getText("[NEWLINE]", ())
					
					# RAR
					list = []
					list.append("raystuttgart")
					list.append("Robert Surcouf")
					list.append("colonialfan")
					list.append("KJ Jansson")
					list.append("Androrc")
					list.append("Schmiddie (Robert E. Lee)")
					list.append("orlanth")
					
					szTextData += localText.getText("TXT_KEY_CREDITS_RAR", ())
					szTextData += localText.getText("[NEWLINE]", ())
					szTextData += localText.getText("[NEWLINE]", ())
					for szName in list:
						szTextData += localText.getText("[ICON_BULLET] " + szName + "[NEWLINE]", ())
					
					szTextData += localText.getText("[NEWLINE]", ())
					
					# TAC
					list = []
					list.append("Akropolis")
					list.append("Elwood")
					list.append("Fankman")
					list.append("koma13")
					list.append("TXT_KEY_CREDITS_MELCHER_KURZER")
					list.append("Ronnar")
					list.append("TXT_KEY_CREDITS_STOPSEL")
					list.append("Writing Bull")
					list.append("xXstrikerXx")
					
					szTextData += localText.getText("TXT_KEY_CREDITS_TAC", ())
					szTextData += localText.getText("[NEWLINE]", ())
					
					for szName in (list):
						szTextData += localText.getText("[ICON_BULLET] ", ())
						szTextData += localText.getText(szName, ())
						szTextData += localText.getText("[NEWLINE]", ())
					
					list = []
					list.append("raystuttgart")
					list.append("Pfeffersack")
					list.append("Willi Tell")
					list.append("King MB")
					list.append("Robert E. Lee")
					
					for szName in (list):
						szTextData += localText.getText("[ICON_BULLET] ", ())
						szTextData += localText.getText(szName, ())
						szTextData += u" "
						szTextData += localText.getText("TXT_KEY_CREDITS_FORMER_MEMBER", ())
						szTextData += localText.getText("[NEWLINE]", ())
					
					szTextData += localText.getText("[NEWLINE]", ())
					
					szTextData += localText.getText("TXT_KEY_CREDITS_MUSIC", ())
					szTextData += localText.getText("[NEWLINE]", ())
					szTextData += localText.getText("[NEWLINE]", ())
					
					szTextData += localText.getText("TXT_KEY_CREDITS_OTHER_PROJECTS", ())
					szTextData += localText.getText("[NEWLINE]", ())
					
					# other, RAR
					list = []
					list.append("Isabelxxx")
					list.append("Willi Tell")
					list.append("Writing Bull")
					list.append("Dale")
					list.append("Kailric")
					list.append("Aymerick")
					list.append("Jeckel")
					list.append("TC01")
					list.append("Dom Pedro II")
					list.append("Trachmyr")
					list.append("NeseryozniyVET")
					list.append("Dazio")
					list.append("M07")
					list.append("bernie14")
					list.append("ekmek")
					list.append("SaibotLieh")
					list.append("Sanotra")
					list.append("Aeterna")
					list.append("Roamty")
					list.append("Kathy")
					list.append("agnat86")
					list.append("Nightinggale")
					list.append("Marla_Singer")
					list.append("Bakuel")
					list.append("Tre59")
					list.append("Commander Bello")
					list.append("vetiarvind")
					list.append("TXT_KEY_CREDITS_OTHER_NAME_1")
					list.append("TXT_KEY_CREDITS_OTHER_NAME_2")
					list.append("TXT_KEY_CREDITS_OTHER_NAME_3")
					list.append("TXT_KEY_CREDITS_OTHER_NAME_4")
					
					for szName in (list):
						szTextData += localText.getText("[ICON_BULLET] ", ())
						szTextData += localText.getText(szName, ())
						szTextData += localText.getText("[NEWLINE]", ())
					
					szTextData += localText.getText("[NEWLINE]", ())
					
					szTextData += localText.getText("TXT_KEY_CREDITS_OTHER_PROJECTS_TAC", ())
					szTextData += localText.getText("[NEWLINE]", ())
					szTextData += localText.getText("[NEWLINE]", ())
					
					szTextData += localText.getText("TXT_KEY_CREDITS_END", ())
					
					szTextData += u"</font>"
					
					screen.setLabel("TextTitle", "TextTopPanel", szTextTitle, CvUtil.FONT_CENTER_JUSTIFY, self.W_SCREEN / 2, self.Y_TITLE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_PEDIA_DESCRIPTION, -1, -1)
					screen.attachMultilineTextAt("TextBackgroundShadow", "TextData", szTextData, self.W_SCREEN / 16, self.H_SCREEN / 8, self.W_SCREEN - self.W_SCREEN / 8, self.H_SCREEN - self.H_SCREEN / 4, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_CENTER_JUSTIFY)
					screen.setText("TextExitButton", "Background", self.EXIT_TEXT, CvUtil.FONT_CENTER_JUSTIFY, self.W_SCREEN / 2, self.Y_EXIT, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, TextExit_ID, -1)
# Credits Bildschirm Ende
# Readme Bildschirm Start
				if (inputClass.getData1() == self.RAEDME_ID):
					screen.addDDSGFC("TextBackgroundImage", ArtFileMgr.getInterfaceArtInfo("MAINMENU_SLIDESHOW_LOAD").getPath(), 0, 0, self.W_SCREEN, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
					screen.addDDSGFC("TextTopPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TITLE").getPath(), 0, 0, self.W_SCREEN, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
					screen.addDDSGFC("TextBottomPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TAB_OFF").getPath(), 0, self.H_SCREEN - 55, self.W_SCREEN, 55, WidgetTypes.WIDGET_GENERAL, -1, -1 )
					screen.addDDSGFC("TextBackgroundShadow", ArtFileMgr.getInterfaceArtInfo("INTERFACE_EUROPE_SHADOW_BOX").getPath(), 0, 0, self.W_SCREEN, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
					
					szTextTitle = u"<font=4b>" + localText.getText("TXT_KEY_README_HEAD", ()).upper() + u"</font>"
					szTextData = TextFontSize + localText.getText("[COLOR_FONT_GOLD]", ()) + localText.getText("TXT_KEY_README_TEXT", ()) + u"</font>"
										
					screen.setLabel("TextTitle", "TextTopPanel", szTextTitle, CvUtil.FONT_CENTER_JUSTIFY, self.W_SCREEN / 2, self.Y_TITLE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_PEDIA_DESCRIPTION, -1, -1)
					screen.attachMultilineTextAt("TextBackgroundShadow", "TextData", szTextData, self.W_SCREEN / 16, self.H_SCREEN / 8, self.W_SCREEN - self.W_SCREEN / 8, self.H_SCREEN - self.H_SCREEN / 4, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
					screen.setText("TextExitButton", "Background", self.EXIT_TEXT, CvUtil.FONT_CENTER_JUSTIFY, self.W_SCREEN / 2, self.Y_EXIT, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, TextExit_ID, -1)
# Readme Bildschirm Ende

				if (inputClass.getData1() == TextExit_ID):
										
					screen.hide("TextExitButton")
					screen.hide("TextData")
					screen.hide("TextBottomPanel")
					screen.hide("TextTitle")
					screen.hide("TextTopPanel")
					screen.hide("TextBackgroundShadow")
					screen.hide("TextBackgroundImage")
# TAC Ende

		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED or inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED):
			if (inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL):

				if (inputClass.getData1() == self.ACHIEVE_SEARCH_CATEGORY_WIDGET):
					iIndex = inputClass.getData2()

					if iIndex >= 0 and iIndex < len(self.achieveSearchCategories):
						self.achieveSearchCategory = self.achieveSearchCategories[iIndex][0]
						self.achieveSearchValue = ""
						self.showScreen(self.CATEGORY_ACHIEVEMENTS)
						return 1

				elif (inputClass.getData1() == self.ACHIEVE_SEARCH_VALUE_WIDGET):
					iIndex = inputClass.getData2()

					if iIndex == -1:
						self.achieveSearchValue = ""
					else:
						values = self._getAvailableAchieveSearchValues()
						if iIndex >= 0 and iIndex < len(values):
							self.achieveSearchValue = values[iIndex]

					self.showScreen(self.CATEGORY_ACHIEVEMENTS)
					return 1

				elif (inputClass.getData1() == CvScreenEnums.PEDIA_ACHIEVE):
					return self.pediaJump(CvScreenEnums.PEDIA_ACHIEVE, inputClass.getData2(), True)
		# redirect to proper screen if necessary
		if (self.iLastScreen == CvScreenEnums.PEDIA_UNIT):
			return self.pediaUnitScreen.handleInput(inputClass)
		if (self.iLastScreen == CvScreenEnums.PEDIA_PROFESSION):
			return self.pediaProfessionScreen.handleInput(inputClass)
		if (self.iLastScreen == CvScreenEnums.PEDIA_BUILDING):
			return self.pediaBuildingScreen.handleInput(inputClass)
		if (self.iLastScreen == CvScreenEnums.PEDIA_PROMOTION):
			return self.pediaPromotionScreen.handleInput(inputClass)
		if (self.iLastScreen == CvScreenEnums.PEDIA_BONUS):
			return self.pediaBonus.handleInput(inputClass)
		if (self.iLastScreen == CvScreenEnums.PEDIA_TERRAIN):
			return self.pediaTerrain.handleInput(inputClass)
		if (self.iLastScreen == CvScreenEnums.PEDIA_YIELDS):
			return self.pediaYields.handleInput(inputClass)
		if (self.iLastScreen == CvScreenEnums.PEDIA_FEATURE):
			return self.pediaFeature.handleInput(inputClass)
		if (self.iLastScreen == CvScreenEnums.PEDIA_FATHER):
			return self.pediaFatherScreen.handleInput(inputClass)
		if (self.iLastScreen == CvScreenEnums.PEDIA_IMPROVEMENT):
			return self.pediaImprovement.handleInput(inputClass)
		if (self.iLastScreen == CvScreenEnums.PEDIA_CIVIC):
			return self.pediaCivic.handleInput(inputClass)
		if (self.iLastScreen == CvScreenEnums.PEDIA_CIVILIZATION):
			return self.pediaCivilization.handleInput(inputClass)
		if (self.iLastScreen == CvScreenEnums.PEDIA_LEADER):
			return self.pediaLeader.handleInput(inputClass)
		if (self.iLastScreen == CvScreenEnums.PEDIA_HISTORY):
			return self.pediaHistorical.handleInput(inputClass)
		if (self.iLastScreen == CvScreenEnums.PEDIA_ACHIEVE):
			return self.pediaAchieve.handleInput(inputClass)                                                      
		return 0

