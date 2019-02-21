## Sid Meier's Civilization 4
## Copyright Firaxis Games 2007

from CvPythonExtensions import *
import random

gc = CyGlobalContext()

class NaturalWonders:
	def __init__(self,):
		self.lBigWonder = ["FEATURE_PLATY_GREAT_BARRIER"]	## List of Natural Wonders that occupy 2 Tiles
		self.iNoNearbyRadius = 2				## Cannot have other Natural Wonders within X Tiles
		self.iPlaceChance = (CyMap().getWorldSize() +1) * 10	## Chance of Spawning each Natural Wonder
		self.iMaxYield = 4					## (0 to X) Yield
		self.lLatitude = {"FEATURE_PLATY_AURORA": (70, 90)}	## Min Latitude, Max Latitude

	def placeNaturalWonders(self):
		for iFeature in xrange(gc.getNumFeatureInfos()):
			FeatureInfo = gc.getFeatureInfo(iFeature)
			sType = FeatureInfo.getType()
			if sType.find("FEATURE_PLATY_") == -1: continue
			if random.randint(1, 100) >= self.iPlaceChance: continue

			WonderPlot = []
			for i in xrange(CyMap().numPlots()):
				pPlot = CyMap().plotByIndex(i)
				#if pPlot.getBonusType(-1) > -1: continue
				if pPlot.getBonusType() > -1: continue

				## Nearby Plot Check ##
				bUnsuitable = False
				iRadius = self.iNoNearbyRadius
				bAdjacentPlot = True

				## Big Wonders ##
				if sType in self.lBigWonder:
					iRadius += 1
					bAdjacentPlot = False

				for x in xrange(pPlot.getX() - iRadius, pPlot.getX() + iRadius + 1):
					for y in xrange(pPlot.getY() - iRadius, pPlot.getY() + iRadius + 1):
						pAdjacentPlot = CyMap().plot(x, y)
						if pAdjacentPlot.getFeatureType() > -1 and gc.getFeatureInfo(pAdjacentPlot.getFeatureType()).getType().find("FEATURE_PLATY_") > -1:
							bUnsuitable = True
							break
					## Big Wonders ##
						if bAdjacentPlot == False and pAdjacentPlot.canHaveFeature(iFeature):
							if abs(pAdjacentPlot.getX() - pPlot.getX()) > 1: continue
							if abs(pAdjacentPlot.getY() - pPlot.getY()) > 1: continue
							#if pAdjacentPlot.getBonusType(-1) > -1: continue
							if pAdjacentPlot.getBonusType() > -1: continue
							bAdjacentPlot = True
					if bUnsuitable: break
				if bUnsuitable: continue

				## Latitude Check ##
				if sType in self.lLatitude:
					if pPlot.getLatitude() < self.lLatitude[sType][0] or pPlot.getLatitude() > self.lLatitude[sType][1]:
						bUnsuitable = True
				if bUnsuitable: continue

				## Suitable Plot ##
				if pPlot.canHaveFeature(iFeature) and bAdjacentPlot:
					WonderPlot.append(pPlot)

			bWonder = False
			while WonderPlot:
				iWonderPlot = random.randint(0, len(WonderPlot) - 1)
				pPlot = WonderPlot[iWonderPlot]

				## Big Wonders ##
				if sType in self.lBigWonder:
					AdjacentPlot = []
					for x in xrange(pPlot.getX() - 1, pPlot.getX() + 2):
						for y in xrange(pPlot.getY() - 1, pPlot.getY() + 2):
							if x == pPlot.getX() and y == pPlot.getY(): continue
							pAdjacentPlot = CyMap().plot(x, y)
							#if pAdjacentPlot.getBonusType(-1) > -1: continue
							if pAdjacentPlot.getBonusType() > -1: continue
							if pAdjacentPlot.canHaveFeature(iFeature):
								AdjacentPlot.append(pAdjacentPlot)
					if not AdjacentPlot:
						del WonderPlot[iWonderPlot]
						if WonderPlot:
							continue
						else:
							break
					pAdjacentPlot = AdjacentPlot[random.randint(0, len(AdjacentPlot) - 1)]
					pAdjacentPlot.setFeatureType(iFeature, 0)
					#CyGame().setPlotExtraYield(pAdjacentPlot.getX(), pAdjacentPlot.getY(), YieldTypes.YIELD_FOOD, random.randint(0, self.iMaxYield))
					#CyGame().setPlotExtraYield(pAdjacentPlot.getX(), pAdjacentPlot.getY(), YieldTypes.YIELD_PRODUCTION, random.randint(0, self.iMaxYield))
					#CyGame().setPlotExtraYield(pAdjacentPlot.getX(), pAdjacentPlot.getY(), YieldTypes.YIELD_COMMERCE, random.randint(0, self.iMaxYield))
					bWonder = True
					break

				## Standard Wonders ##
				bWonder = True
				break

			if bWonder:
				pPlot.setFeatureType(iFeature, 0)
				#CyGame().setPlotExtraYield(pPlot.getX(), pPlot.getY(), YieldTypes.YIELD_FOOD, random.randint(0, self.iMaxYield))
				#CyGame().setPlotExtraYield(pPlot.getX(), pPlot.getY(), YieldTypes.YIELD_PRODUCTION, random.randint(0, self.iMaxYield))
				#CyGame().setPlotExtraYield(pPlot.getX(), pPlot.getY(), YieldTypes.YIELD_COMMERCE, random.randint(0, self.iMaxYield))

				sNature = sType[sType.find("_PLATY_") + 7:]
				sBuildingType = "BUILDING_" + sNature
				iBuilding = gc.getInfoTypeForString(sBuildingType)
				if iBuilding == -1: continue
				lNaturalCity = []
				lNaturalCity = self.addSuitableCity(pPlot, -1, lNaturalCity)
				if sType in self.lBigWonder:
					lNaturalCity = self.addSuitableCity(pAdjacentPlot, -1, lNaturalCity)
				if lNaturalCity:
					pLuckyCity = lNaturalCity[random.randint(0, len(lNaturalCity) - 1)]
					pLuckyCity.setHasFreeBuilding(iBuilding, 1)
					#pLuckyCity.setNumRealBuilding(iBuilding, 1)
		return

	def checkReveal(self, pPlot, iTeam):
		iFeature = pPlot.getFeatureType()
		if iFeature == -1: return
		pTeam = gc.getTeam(iTeam)
		# WTP: Adapt this later
		#if pTeam.isBarbarian(): return
		FeatureInfo = gc.getFeatureInfo(iFeature)
		sType = FeatureInfo.getType()
		if sType.find("FEATURE_PLATY_") == -1: return

		if sType in self.lBigWonder:
			bFound = False
			for x in xrange(pPlot.getX() - 1, pPlot.getX() + 2):
				for y in xrange(pPlot.getY() - 1, pPlot.getY() + 2):
					pAdjacentPlot = CyMap().plot(x, y)
					if x == pPlot.getX() and y == pPlot.getY(): continue
					if pAdjacentPlot.getFeatureType() == iFeature:
						bFound = True
						break
				if bFound: break
			if pAdjacentPlot.isRevealed(iTeam, False): return

		for iPlayerX in xrange(gc.getMAX_CIV_PLAYERS()):
			pPlayerX = gc.getPlayer(iPlayerX)
			if pPlayerX.getTeam() == iTeam:
				# Erik: disabling this for testing purposes
				if pPlayerX.isHuman(): #and not CyGame().GetWorldBuilderMode():
					popupInfo = CyPopupInfo()
					popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
					popupInfo.setData1(iFeature)
					# Erik: The wonderscreen only takes 2 parameters!
					popupInfo.setData2(3)
					
					popupInfo.setData3(3)
					
					popupInfo.setText(u"showWonderMovie")
					# Different name in COL
					popupInfo.setText(u"wonderMovie")
					
					
					popupInfo.addPopup(iPlayerX)
					CyInterface().addMessage(iPlayerX,True,10,CyTranslator().getText("TXT_KEY_WONDERDISCOVERED_YOU",(FeatureInfo.getDescription(),)),'',0, FeatureInfo.getButton(),ColorTypes(11),pPlot.getX(),pPlot.getY(), True,True)
		return

	def placeWonderBuilding(self, pCity):
		for i in xrange(21):
			pPlot = pCity.getCityIndexPlot(i)
			iFeature = pPlot.getFeatureType()
			if iFeature == -1: continue
			sType = gc.getFeatureInfo(iFeature).getType()
			if sType.find("FEATURE_PLATY_") == -1: continue
			sNature = sType[sType.find("_PLATY_") + 7:]
			sBuildingType = "BUILDING_" + sNature
			iBuilding = gc.getInfoTypeForString(sBuildingType)
			if iBuilding == -1: continue
			if self.checkWonderBuilt(iBuilding): continue
			#pCity.setNumRealBuilding(iBuilding, 1)
			pCity.setHasFreeBuilding(iBuilding, 1)
		return

	def checkWonderBuilt(self, iBuilding):
		iBuildingClass = gc.getBuildingInfo(iBuilding).getBuildingClassType()
		for iTeamX in xrange(gc.getMAX_TEAMS()):
			pTeamX = gc.getTeam(iTeamX)
			if pTeamX.getBuildingClassCount(iBuildingClass) > 0: return True
		return False

	def findNewCity(self, pCity):
		for i in xrange(21):
			pPlot = pCity.getCityIndexPlot(i)
			iFeature = pPlot.getFeatureType()
			if iFeature == -1: continue
			sType = gc.getFeatureInfo(iFeature).getType()
			if sType.find("FEATURE_PLATY_") == -1: continue
			sNature = sType[sType.find("_PLATY_") + 7:]
			sBuildingType = "BUILDING_" + sNature
			iBuilding = gc.getInfoTypeForString(sBuildingType)
			if iBuilding == -1: continue
			if pCity.getNumRealBuilding(iBuilding) == 0: continue
			lNaturalCity = []
			lNaturalCity = self.addSuitableCity(pPlot, pCity, lNaturalCity)

			if sType in self.lBigWonder:
				bFound = False
				iX = pPlot.getX()
				iY = pPlot.getY()
				for x in xrange(iX - 1, iX + 2):
					for y in xrange(iY - 1, iY + 2):
						pAdjacentPlot = CyMap().plot(x, y)
						if x == iX and y == iY: continue
						if pAdjacentPlot.getFeatureType() == iFeature:
							bFound = True
							break
					if bFound: break
				lNaturalCity = self.addSuitableCity(pAdjacentPlot, pCity, lNaturalCity)

			if lNaturalCity:
				pLuckyCity = lNaturalCity[random.randint(0, len(lNaturalCity) - 1)]
				pLuckyCity.setHasFreeBuilding(iBuilding, 1)
				#pLuckyCity.setNumRealBuilding(iBuilding, 1)
		return

	def addSuitableCity(self, pPlot, pCity, lNaturalCity):
		iX = pPlot.getX()
		iY = pPlot.getY()
		for x in xrange(iX - 2, iX + 3):
			for y in xrange(iY - 2, iY + 3):
				if pCity != -1:
					if x == pCity.getX() and y == pCity.getY(): continue
				if abs(x - iX) == 2 and abs(y - iY) == 2: continue
				pLoopPlot = CyMap().plot(x, y)
				if pLoopPlot.isNone(): continue
				if pLoopPlot.isCity():
					pLoopCity = pLoopPlot.getPlotCity()
					if pLoopCity in lNaturalCity: continue
					lNaturalCity.append(pLoopCity)
		return lNaturalCity

	def showNaturalWonders(self, bAlwaysShow):
		iPlayer = CyGame().getActivePlayer()
		pPlayer = gc.getPlayer(iPlayer)
		for i in xrange(CyMap().numPlots()):
			pPlot = CyMap().plotByIndex(i)
			iFeature = pPlot.getFeatureType()
			if iFeature == -1: continue
			if bAlwaysShow or CyGame().GetWorldBuilderMode() or pPlot.isRevealed(pPlayer.getTeam(), False):
				FeatureInfo = gc.getFeatureInfo(iFeature)
				if FeatureInfo.getType().find("FEATURE_PLATY_") == -1: continue
				sText = FeatureInfo.getDescription() + " (" + str(pPlot.getX()) + ", " + str(pPlot.getY()) +")"
				CyInterface().addMessage(iPlayer,True,60,sText,'',0, FeatureInfo.getButton(),ColorTypes(11),pPlot.getX(),pPlot.getY(), True,True)
		return