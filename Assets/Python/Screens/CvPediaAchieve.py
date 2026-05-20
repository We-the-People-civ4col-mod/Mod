## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
## Prepared by Schmiddie for WTP

from CvPythonExtensions import *
import CvUtil
import CvScreenEnums
import CvPediaScreen

gc = CyGlobalContext()
localText = CyTranslator()
ArtFileMgr = CyArtFileMgr()


class CvPediaAchieve(CvPediaScreen.CvPediaScreen):

	def __init__(self, main):
		self.iAchieve = -1
		self.top = main
		self.ACHIEVE_LINK_WIDGET = 9999

	def _getTranslatedText(self, szKey, szFallback):
		if szKey == "":
			return szFallback

		try:
			szText = localText.getText(szKey, ())
			if szText == szKey:
				return szFallback
			return szText
		except:
			return szFallback

	def _getAchieveAdvisorText(self, szKey, szFallback):
		if szKey == "":
			return szFallback

		try:
			szText = localText.getText("%s1", (szKey, ()))
			if szText == "" or szText == "%s1":
				return szFallback
			return szText
		except:
			return szFallback

	def _buildInfoDescriptionMap(self, iCount, infoGetter):
		typeMap = {}
		for i in range(iCount):
			try:
				info = infoGetter(i)
				typeMap[info.getType()] = info.getDescription()
			except:
				pass
		return typeMap

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

	def _getSimpleTagInt(self, blockText, tagName, defaultValue):
		try:
			return int(self._getSimpleTagValue(blockText, tagName, str(defaultValue)))
		except:
			return defaultValue

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

			block = parentText[iStart:iEnd]
			results.append(block)
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

	def _getLocalizedReqText(self, szKey, szFallback, args=()):
		try:
			szText = localText.getText(szKey, args)
			if szText == "" or szText == szKey:
				return szFallback
			return szText
		except:
			return szFallback

	def _formatReqColonies(self, iValue):
		return self._getLocalizedReqText(
			"TXT_KEY_PEDIA_ACHIEVE_REQ_COLONIES",
			u"Found %d colonies" % iValue,
			(iValue,)
		)

	def _formatReqEuroContacts(self, iValue):
		return self._getLocalizedReqText(
			"TXT_KEY_PEDIA_ACHIEVE_REQ_EURO_CONTACTS",
			u"Meet %d European nations" % iValue,
			(iValue,)
		)

	def _formatReqNativeContacts(self, iValue):
		return self._getLocalizedReqText(
			"TXT_KEY_PEDIA_ACHIEVE_REQ_NATIVE_CONTACTS",
			u"Meet %d native tribes" % iValue,
			(iValue,)
		)

	def _formatReqGoods(self, iValue):
		return self._getLocalizedReqText(
			"TXT_KEY_PEDIA_ACHIEVE_REQ_GOODS",
			u"Trade %d goods" % iValue,
			(iValue,)
		)

	def _formatReqCombats(self, iValue):
		return self._getLocalizedReqText(
			"TXT_KEY_PEDIA_ACHIEVE_REQ_COMBATS",
			u"Win %d combats" % iValue,
			(iValue,)
		)

	def _formatReqSeaCombats(self, iValue):
		return self._getLocalizedReqText(
			"TXT_KEY_PEDIA_ACHIEVE_REQ_SEA_COMBATS",
			u"Win %d sea combats" % iValue,
			(iValue,)
		)

	def _formatReqDiscoverLand(self):
		return self._getLocalizedReqText(
			"TXT_KEY_PEDIA_ACHIEVE_REQ_DISCOVER_LAND",
			u"Discover land"
		)

	def _formatReqDiscoverEast(self):
		return self._getLocalizedReqText(
			"TXT_KEY_PEDIA_ACHIEVE_REQ_DISCOVER_EAST",
			u"Reach the eastern border"
		)

	def _formatReqDiscoverWest(self):
		return self._getLocalizedReqText(
			"TXT_KEY_PEDIA_ACHIEVE_REQ_DISCOVER_WEST",
			u"Reach the western border"
		)

	def _formatReqUnits(self, iValue, szName):
		return self._getLocalizedReqText(
			"TXT_KEY_PEDIA_ACHIEVE_REQ_UNITS",
			u"Recruit %d %s" % (iValue, szName),
			(iValue, szName)
		)

	def _joinRequirementAlternatives(self, items):
		if len(items) == 0:
			return u""

		if len(items) == 1:
			return items[0]

		szOr = self._getLocalizedReqText(
			"TXT_KEY_PEDIA_ACHIEVE_REQ_OR",
			u"or"
		)

		return (u" " + szOr + u" ").join(items)

	def _formatReqBuildings(self, iValue, szName):
		return self._getLocalizedReqText(
			"TXT_KEY_PEDIA_ACHIEVE_REQ_BUILDINGS",
			u"Build %d %s" % (iValue, szName),
			(iValue, szName)
		)

	def _formatReqProfessions(self, iValue, szName):
		return self._getLocalizedReqText(
			"TXT_KEY_PEDIA_ACHIEVE_REQ_PROFESSIONS",
			u"Have %d %s" % (iValue, szName),
			(iValue, szName)
		)

	def _formatReqYield(self, iValue, szName):
		return self._getLocalizedReqText(
			"TXT_KEY_PEDIA_ACHIEVE_REQ_YIELD",
			u"Produce %d %s" % (iValue, szName),
			(iValue, szName)
		)

	def _getXmlBlockText(self, szBaseKey):
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

	def _buildRequirementsFromXml(self, szBaseKey):
		try:
			blockText = self._getXmlBlockText(szBaseKey)
			if blockText == "":
				return u"No explicit requirements listed."

			lines = []

			unitMap = self._buildInfoDescriptionMap(gc.getNumUnitInfos(), gc.getUnitInfo)
			buildingMap = self._buildInfoDescriptionMap(gc.getNumBuildingInfos(), gc.getBuildingInfo)
			professionMap = self._buildInfoDescriptionMap(gc.getNumProfessionInfos(), gc.getProfessionInfo)
			yieldMap = self._buildInfoDescriptionMap(gc.getNumYieldInfos(), gc.getYieldInfo)

			iColonies = self._getSimpleTagInt(blockText, "iNumColonies", 0)
			if iColonies > 0:
				lines.append(u"- " + self._formatReqColonies(iColonies))

			iEuro = self._getSimpleTagInt(blockText, "iNumEuroContacts", 0)
			if iEuro > 0:
				lines.append(u"- " + self._formatReqEuroContacts(iEuro))

			iNative = self._getSimpleTagInt(blockText, "iNumNativeContacts", 0)
			if iNative > 0:
				lines.append(u"- " + self._formatReqNativeContacts(iNative))

			iGoods = self._getSimpleTagInt(blockText, "iNumGoodsTraded", -1)
			if iGoods > 0:
				lines.append(u"- " + self._formatReqGoods(iGoods))

			iCombat = self._getSimpleTagInt(blockText, "iNumCombatsWonNeeded", -1)
			if iCombat > 0:
				lines.append(u"- " + self._formatReqCombats(iCombat))

			iSeaCombat = self._getSimpleTagInt(blockText, "iNumSeaCombatsWonNeeded", -1)
			if iSeaCombat > 0:
				lines.append(u"- " + self._formatReqSeaCombats(iSeaCombat))

			if self._getSimpleTagInt(blockText, "bLandDiscovered", 0) > 0:
				lines.append(u"- " + self._formatReqDiscoverLand())

			if self._getSimpleTagInt(blockText, "bDiscoverEast", 0) > 0:
				lines.append(u"- " + self._formatReqDiscoverEast())

			if self._getSimpleTagInt(blockText, "bDiscoverWest", 0) > 0:
				lines.append(u"- " + self._formatReqDiscoverWest())

			unitBlocks = self._getUnitRequirementBlocks(blockText)
			if len(unitBlocks) > 0:
				unitAlternatives = []

				for entry in unitBlocks:
					unitType = self._getSimpleTagValue(entry, "UnitType", "")
					num = self._getSimpleTagInt(entry, "iNumber", 0)

					if unitType != "" and num > 0:
						desc = unitMap.get(unitType, unitType)
						unitAlternatives.append(self._formatReqUnits(num, desc))

				if len(unitAlternatives) == 1:
					lines.append(u"- " + unitAlternatives[0])
				elif len(unitAlternatives) > 1:
					lines.append(u"- " + self._joinRequirementAlternatives(unitAlternatives))

			numBuildingsBlock = self._getSimpleTagValue(blockText, "NumBuildings", "")
			if numBuildingsBlock != "":
				buildingBlocks = self._getNestedBlocks(numBuildingsBlock, "NumBuilding")
				for entry in buildingBlocks:
					buildingType = self._getSimpleTagValue(entry, "BuildingType", "")
					num = self._getSimpleTagInt(entry, "iNumber", 0)

					if buildingType != "" and num > 0:
						desc = buildingMap.get(buildingType, buildingType)
						lines.append(u"- " + self._formatReqBuildings(num, desc))

			numProfessionsBlock = self._getSimpleTagValue(blockText, "NumProfessions", "")
			if numProfessionsBlock != "":
				profBlocks = self._getNestedBlocks(numProfessionsBlock, "NumProfession")
				for entry in profBlocks:
					profType = self._getSimpleTagValue(entry, "ProfessionType", "")
					num = self._getSimpleTagInt(entry, "iNumber", 0)

					if profType != "" and num > 0:
						desc = professionMap.get(profType, profType)
						lines.append(u"- " + self._formatReqProfessions(num, desc))

			yieldBlock = self._getSimpleTagValue(blockText, "YieldProduced", "")
			if yieldBlock != "":
				yieldEntries = self._getNestedBlocks(yieldBlock, "YieldProduce")
				for entry in yieldEntries:
					yieldType = self._getSimpleTagValue(entry, "YieldType", "")
					num = self._getSimpleTagInt(entry, "iNumber", 0)

					if yieldType != "" and num > 0:
						desc = yieldMap.get(yieldType, yieldType)
						lines.append(u"- " + self._formatReqYield(num, desc))

			if len(lines) == 0:
				return u"No explicit requirements listed."

			return u"\n".join(lines)

		except:
			return u"No explicit requirements listed."

	def interfaceScreen(self, iAchieve, x, y, h, w):

		self.iAchieve = iAchieve
		self.top.iLastScreen = CvScreenEnums.PEDIA_ACHIEVE

		screen = self.top.getScreen()

		if not screen.isActive():
			self.top.setPediaCommonWidgets()

		self.top.deleteAllWidgets()

		info = gc.getAchieveInfo(iAchieve)

		try:
			szName = info.getDescription()
		except:
			szName = u""

		try:
			szBaseKey = info.getTextKey()
		except:
			szBaseKey = u""

		if szBaseKey != "":
			szHeaderKey = szBaseKey + u"_HEAD"
			szBodyKey = szBaseKey + u"_TEXT"
		else:
			szHeaderKey = u""
			szBodyKey = u""

		self.szDescription = szName
		self.szHeaderText = self._getAchieveAdvisorText(szHeaderKey, u"")
		self.szBodyText = self._getAchieveAdvisorText(szBodyKey, u"")
		self.szRequirements = self._buildRequirementsFromXml(szBaseKey)

		if self.szHeaderText == "":
			self.szHeaderText = szName

		if self.szBodyText == "":
			self.szBodyText = szName

		screen.setLabel(self.top.getNextWidgetName(), "Background",
			u"<font=4b>" + szName.upper() + u"</font>",
			CvUtil.FONT_CENTER_JUSTIFY,
			self.top.X_SCREEN, self.top.Y_TITLE, 0,
			FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		screen.setText(self.top.TOP_ID, "Background", self.top.MENU_TEXT,
			CvUtil.FONT_LEFT_JUSTIFY,
			self.top.X_MENU, self.top.Y_MENU, 0,
			FontTypes.TITLE_FONT,
			WidgetTypes.WIDGET_PEDIA_MAIN, 0, -1)

		self.leftX = x + 20
		self.leftY = y + 20
		self.leftW = 620
		self.leftH = h - 40

		self.rightX = self.leftX + self.leftW + 20
		self.rightY = y + 20
		self.rightW = w - (self.rightX - x) - 20
		self.rightH = h - 40

		self.picW = 400
		self.picH = 267
		self.picX = self.leftX + (self.leftW - self.picW) / 2
		self.picY = self.leftY + 10

		self.frameX = self.picX - 138
		self.frameY = self.picY - 47
		self.frameW = self.picW + 284
		self.frameH = self.picH + 100

		self.descX = self.leftX + 10
		self.descY = self.picY + self.picH + 65
		self.descW = self.leftW - 20
		self.descH = 90

		self.headX = self.leftX + 10
		self.headY = self.descY + self.descH + 10
		self.headW = self.leftW - 20
		self.headH = 180

		self.reqX = self.leftX + 10
		self.reqY = self.headY + self.headH + 10
		self.reqW = self.leftW - 20
		self.reqH = self.leftH - (self.reqY - self.leftY) - 10

		if self.reqH < 90:
			self.reqH = 90

		self.bodyX = self.rightX
		self.bodyY = self.rightY + 10
		self.bodyW = self.rightW
		self.bodyH = self.rightH - 10

		self.placePicture()
		self.placeLeftTexts()
		self.placeRequirements()
		self.placeBodyText()
		self.placeLinks(True)

	def placePicture(self):

		screen = self.top.getScreen()
		info = gc.getAchieveInfo(self.iAchieve)

		try:
			szPicture = info.getPictureFileName()
		except:
			szPicture = ""

		if szPicture == "":
			return

		screen.setImageButtonAt(self.top.getNextWidgetName(), "Background",
			szPicture,
			self.picX, self.picY,
			self.picW, self.picH,
			WidgetTypes.WIDGET_GENERAL, -1, -1)

		screen.setImageButtonAt(self.top.getNextWidgetName(), "Background",
			ArtFileMgr.getInterfaceArtInfo("INTERFACE_ACHIEVE_FRAME").getPath(),
			self.frameX, self.frameY,
			self.frameW, self.frameH,
			WidgetTypes.WIDGET_GENERAL, -1, -1)

	def placeLeftTexts(self):

		screen = self.top.getScreen()

		screen.addPanel(self.top.getNextWidgetName(),
			"", "", True, True,
			self.descX, self.descY, self.descW, self.descH,
			PanelStyles.PANEL_STYLE_BLUE50,
			WidgetTypes.WIDGET_GENERAL, -1, -1)

		screen.addMultilineText(self.top.getNextWidgetName(),
			u"<font=4b>" + self.szDescription + u"</font>",
			self.descX + 10, self.descY + 10,
			self.descW - 20, self.descH - 20,
			WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

		screen.addPanel(self.top.getNextWidgetName(),
			"", "", True, True,
			self.headX, self.headY, self.headW, self.headH,
			PanelStyles.PANEL_STYLE_BLUE50,
			WidgetTypes.WIDGET_GENERAL, -1, -1)

		screen.addMultilineText(self.top.getNextWidgetName(),
			u"<font=3>" + self.szHeaderText + u"</font>",
			self.headX + 10, self.headY + 10,
			self.headW - 20, self.headH - 20,
			WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

	def placeRequirements(self):

		screen = self.top.getScreen()

		screen.addPanel(self.top.getNextWidgetName(),
			"", "", True, True,
			self.reqX, self.reqY, self.reqW, self.reqH,
			PanelStyles.PANEL_STYLE_BLUE50,
			WidgetTypes.WIDGET_GENERAL, -1, -1)

		szTitle = self._getLocalizedReqText(
			"TXT_KEY_PEDIA_ACHIEVE_REQUIREMENTS",
			u"Requirements"
		)

		szText = u"<font=4b>" + szTitle + u"</font>\n\n"
		szText += u"<font=3>" + self.szRequirements + u"</font>"

		screen.addMultilineText(self.top.getNextWidgetName(),
			szText,
			self.reqX + 15, self.reqY + 15,
			self.reqW - 30, self.reqH - 30,
			WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

	def placeBodyText(self):

		screen = self.top.getScreen()

		screen.addPanel(self.top.getNextWidgetName(),
			"", "", True, True,
			self.bodyX, self.bodyY, self.bodyW, self.bodyH,
			PanelStyles.PANEL_STYLE_BLUE50,
			WidgetTypes.WIDGET_GENERAL, -1, -1)

		szTitle = self._getLocalizedReqText(
			"TXT_KEY_PEDIA_ACHIEVE_BODY_TITLE",
			u"Achievement Text"
		)

		szText = u"<font=4b>" + szTitle + u"</font>\n\n"
		szText += u"<font=3>" + self.szBodyText + u"</font>"

		screen.addMultilineText(self.top.getNextWidgetName(),
			szText,
			self.bodyX + 15, self.bodyY + 15,
			self.bodyW - 30, self.bodyH - 30,
			WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

	def placeLinks(self, bRedraw):

		screen = self.top.getScreen()

		if bRedraw:
			screen.clearListBoxGFC(self.top.LIST_ID)

		listSorted = self.top.getSortedList(gc.getNumAchieveInfos(), gc.getAchieveInfo)

		iSelected = 0
		iIndex = 0

		for item in listSorted:
			iAchieve = item[1]

			if bRedraw:
				screen.appendListBoxStringNoUpdate(
					self.top.LIST_ID,
					item[0],
					WidgetTypes.WIDGET_GENERAL,
					self.ACHIEVE_LINK_WIDGET,
					iAchieve,
					CvUtil.FONT_LEFT_JUSTIFY)

			if iAchieve == self.iAchieve:
				iSelected = iIndex

			iIndex += 1

		if bRedraw:
			screen.updateListBox(self.top.LIST_ID)

		if iIndex > 0:
			screen.setSelectedListBoxStringGFC(self.top.LIST_ID, iSelected)

	def handleInput(self, inputClass):

		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED or inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED):

			if (inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL):

				if (inputClass.getData1() == self.ACHIEVE_LINK_WIDGET):

					return self.top.pediaJump(
						CvScreenEnums.PEDIA_ACHIEVE,
						inputClass.getData2(),
						True)

		return 0