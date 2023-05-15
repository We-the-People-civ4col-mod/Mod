## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import CvScreensInterface
import ScreenInput
import CvEventInterface
import CvScreenEnums
import time

gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class CvWorldBuilderScreen:
	"World Builder Screen"

	def __init__ (self) :
		print("init-ing world builder screen")
		self.m_advancedStartTabCtrl = None
		self.m_normalPlayerTabCtrl = 0
		self.m_normalMapTabCtrl = 0
		self.m_tabCtrlEdit = 0
		self.m_flyoutMenu = 0
		self.m_bCtrlEditUp = False
		self.m_bUnitEdit = False
		self.m_bCityEdit = False
		self.m_bNormalPlayer = True
		self.m_bNormalMap = False
		self.m_bReveal = False
		self.m_bDiplomacy = False
		self.m_bLandmark = False
		self.m_bEraseAll = False
		self.m_bUnitEditCtrl = False
		self.m_bCityEditCtrl = False
		self.m_bShowBigBrush = False
		self.m_bLeftMouseDown = False
		self.m_bRightMouseDown = False
		self.m_bChangeFocus = False
		self.m_iNormalPlayerCurrentIndexes = []
		self.m_iNormalMapCurrentIndexes = []
		self.m_iNormalMapCurrentList = []
		self.m_iAdvancedStartCurrentIndexes = []
		self.m_iAdvancedStartCurrentList = []
		self.m_iCurrentPlayer = 0
		self.m_iCurrentTeam = 0
		self.m_iCurrentUnitPlayer = 0
		self.m_iCurrentUnit = 0
		self.m_iCurrentX = -1
		self.m_iCurrentY = -1
		self.m_pCurrentPlot = 0
		self.m_pActivePlot = 0
		self.m_pRiverStartPlot = -1

		self.m_iUnitTabID = -1
		self.m_iBuildingTabID = -1
		self.m_iImprovementTabID = -1
		self.m_iBonusTabID = -1
		self.m_iImprovementListID = -1
		self.m_iBonusListID = -1
		self.m_iTerrainTabID = -1
		self.m_iTerrainListID = -1
		self.m_iFeatureListID = -1
		self.m_iPlotTypeListID = -1
		self.m_iRouteListID = -1
		self.m_iTerritoryTabID = -1
		self.m_iTerritoryListID = -1
		self.m_iEuropeListID = -1

		self.m_iASUnitTabID = -1
		self.m_iASUnitListID = -1
		self.m_iASCityTabID = -1
		self.m_iASCityListID = -1
		self.m_iASBuildingsListID = -1
		self.m_iASAutomateListID = -1
		self.m_iASImprovementsTabID = -1
		self.m_iASRoutesListID = -1
		self.m_iASImprovementsListID = -1
		self.m_iASVisibilityTabID = -1
		self.m_iASVisibilityListID = -1
		self.m_iBrushSizeTabID = -1
		self.m_iBrushWidth = 1
		self.m_iBrushHeight = 1
		self.m_iFlyoutEditUnit = 1
		self.m_iFlyoutEditCity = 0
		self.m_iFlyoutAddScript = -1
		self.m_iFlyoutChangeStartYear = -2
		self.m_pFlyoutPlot = 0
		self.m_bFlyout = False
		self.m_pUnitToScript = -1
		self.m_pCityToScript = -1
		self.m_pPlotToScript = -1
		self.m_iUnitEditCheckboxID = -1
		self.m_iCityEditCheckboxID = -1
		self.m_iNormalPlayerCheckboxID = -1
		self.m_iNormalMapCheckboxID = -1
		self.m_iRevealTileCheckboxID = -1
		self.m_iDiplomacyCheckboxID = -1
		self.m_iLandmarkCheckboxID = -1
		self.m_iEraseCheckboxID = -1
		self.iScreenWidth = 228

		self.m_bSideMenuDirty = false
		self.m_bASItemCostDirty = false
		self.m_iCost = 0

	def interfaceScreen (self):
		# This is the main interface screen, create it as such
		self.initVars()
		screen = CyGInterfaceScreen( "WorldBuilderScreen", CvScreenEnums.WORLDBUILDER_SCREEN )
		screen.setCloseOnEscape(False)
		screen.setAlwaysShown(True)

		self.setSideMenu()
		self.refreshSideMenu()

		#add interface items
		self.refreshPlayerTabCtrl()

		self.refreshAdvancedStartTabCtrl(false)

		if (CyInterface().isInAdvancedStart()):
			pPlayer = gc.getPlayer(self.m_iCurrentPlayer)
			pPlot = pPlayer.getStartingPlot()
			CyCamera().JustLookAtPlot(pPlot)

		self.m_normalMapTabCtrl = getWBToolNormalMapTabCtrl()

		self.m_normalMapTabCtrl.setNumColumns((gc.getNumBonusInfos()/10)+1);
		self.m_normalMapTabCtrl.addTabSection(localText.getText("TXT_KEY_WB_IMPROVEMENTS",()));
		self.m_iImprovementTabID = 0
		self.m_iNormalMapCurrentIndexes.append(0)

		self.m_iNormalMapCurrentList.append(0)
		self.m_iImprovementListID = 0

		self.m_normalMapTabCtrl.addTabSection(localText.getText("TXT_KEY_WB_BONUSES", ()));
		self.m_iBonusTabID = 1
		self.m_iNormalMapCurrentIndexes.append(0)

		self.m_iNormalMapCurrentList.append(0)
		self.m_iBonusListID = 0

		self.m_normalMapTabCtrl.setNumColumns((gc.getNumTerrainInfos()/10)+1);
		self.m_normalMapTabCtrl.addTabSection(localText.getText("TXT_KEY_WB_TERRAINS",()))
		self.m_iTerrainTabID = 2
		self.m_iNormalMapCurrentIndexes.append(0)

		self.m_iNormalMapCurrentList.append(0)
		self.m_iTerrainListID = 0
		self.m_iPlotTypeListID = 1
		self.m_iFeatureListID = 2
		self.m_iRouteListID = 3

		# Territory

		self.m_normalMapTabCtrl.setNumColumns(8);
		self.m_normalMapTabCtrl.addTabSection(localText.getText("TXT_KEY_WB_TERRITORY",()))
		self.m_iTerritoryTabID = 3
		self.m_iNormalMapCurrentIndexes.append(0)

		self.m_iNormalMapCurrentList.append(0)
		self.m_iTerritoryListID = 0
		self.m_iEuropeListID = 1

		# This should be a forced redraw screen
		screen.setForcedRedraw( True )

		screen.setDimensions( 0, 0, screen.getXResolution(), screen.getYResolution() )
		# This should show the screen immidiately and pass input to the game
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, True)

		setWBInitialCtrlTabPlacement()
		return 0

	def killScreen(self):
		if (self.m_tabCtrlEdit != 0):
			self.m_tabCtrlEdit.destroy()
			self.m_tabCtrlEdit = 0

		screen = CyGInterfaceScreen( "WorldBuilderScreen", CvScreenEnums.WORLDBUILDER_SCREEN )
		screen.hideScreen()
		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_HIGHLIGHT_PLOT)
		CvScreensInterface.hideWorldBuilderDiplomacyScreen()

		# autodetect lakes - start
		gc.getMap().updateWaterPlotTerrainTypes()
		# autodetect lakes - end

	def handleInput (self, inputClass):
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED) and inputClass.isShiftKeyDown() and inputClass.isCtrlKeyDown():
			return 1
		elif (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CHARACTER):
			key = inputClass.getData()
			if key == int(InputTypes.KB_ESCAPE):
				if (self.m_bDiplomacy):
					self.normalPlayerTabModeCB()
				return 1
		elif (inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED):
			if (inputClass.getFunctionName() == "WorldBuilderPlayerChoice"):
				self.handlePlayerUnitPullDownCB(inputClass.getData())
			elif(inputClass.getFunctionName() == "WorldBuilderBrushSize"):
				self.handleBrushHeightCB(inputClass.getData())
				self.handleBrushWidthCB(inputClass.getData())
			elif(inputClass.getFunctionName() == "WorldBuilderTeamChoice"):
				self.handleSelectTeamPullDownCB(inputClass.getData())
		return 1

	def mouseOverPlot (self, argsList):

		if (self.m_bReveal):
			self.m_pCurrentPlot = CyInterface().getMouseOverPlot()
			if (CyInterface().isLeftMouseDown() and self.m_bLeftMouseDown):
				self.setMultipleReveal(True)
			elif(CyInterface().isRightMouseDown() and self.m_bRightMouseDown):
				self.setMultipleReveal(False)
		else: #if ((self.m_tabCtrlEdit == 0) or (not self.m_tabCtrlEdit.isEnabled())):
			self.m_pCurrentPlot = CyInterface().getMouseOverPlot()
			self.m_iCurrentX = self.m_pCurrentPlot.getX()
			self.m_iCurrentY = self.m_pCurrentPlot.getY()
			if (CyInterface().isLeftMouseDown() and self.m_bLeftMouseDown):
				if (self.useLargeBrush()):
					self.placeMultipleObjects()
				else:
					self.placeObject()
			elif (CyInterface().isRightMouseDown() and self.m_bRightMouseDown):
				if (not (self.m_bCityEdit or self.m_bUnitEdit)):
					if (self.useLargeBrush()):
						self.removeMultipleObjects()
					else:
						self.removeObject()
		return

	def getHighlightPlot (self, argsList):

		self.refreshASItemCost()

		if (self.m_pCurrentPlot != 0):
#			if (CyInterface().isInAdvancedStart() and self.m_pCurrentPlot.isAdjacentNonrevealed(CyGame().getActiveTeam())):
#				if (self.getASActiveVisibility() == -1):
#					return []
			if (CyInterface().isInAdvancedStart()):
				if (self.m_iCost <= 0):
					return []

		if ((self.m_pCurrentPlot != 0) and not self.m_bShowBigBrush and not self.m_bDiplomacy and isMouseOverGameSurface()):
			return (self.m_pCurrentPlot.getX(), self.m_pCurrentPlot.getY())

		return []

	def leftMouseDown (self, argsList):
		bShift, bCtrl, bAlt = argsList
		self.m_bLeftMouseDown = True

		if CyInterface().isInAdvancedStart():
			self.placeObject()
			return 1

		if (bAlt and bCtrl) or  (self.m_bUnitEdit):
			if (self.m_pCurrentPlot.getNumUnits() > 0):
				self.m_iCurrentUnit = 0
				self.setUnitEditInfo(False)
			return 1
		elif (bCtrl) or (self.m_bCityEdit):
			if (self.m_pCurrentPlot.isCity()):
				self.initCityEditScreen()
			return 1
		elif (self.m_bReveal):
			if (self.m_pCurrentPlot != 0):
				self.setMultipleReveal(True)
		elif (bShift and not bCtrl and not bAlt):
			self.createFlyoutMenu()
			return 1

		if (self.useLargeBrush()):
			self.placeMultipleObjects()
		else:
			self.placeObject()
		return 1

	def rightMouseDown (self, argsList):
		self.m_bRightMouseDown = True

		if CyInterface().isInAdvancedStart():
			self.removeObject()
			return 1

		if (self.m_bCityEdit or self.m_bUnitEdit):
			self.createFlyoutMenu()
		elif (self.m_bReveal):
			if (self.m_pCurrentPlot != 0):
				self.setMultipleReveal(False)
		else:
			if (self.useLargeBrush()):
				self.removeMultipleObjects()
			else:
				self.removeObject()

		return 1

	def update(self, fDelta):
		if (not CyInterface().isLeftMouseDown()):
			self.m_bLeftMouseDown = False
		if (not CyInterface().isRightMouseDown()):
			self.m_bRightMouseDown = False

		if (not self.m_bChangeFocus) and (not isMouseOverGameSurface()):
			self.m_bChangeFocus = True

		if (self.m_bChangeFocus and isMouseOverGameSurface() and (not self.m_bUnitEdit and not self.m_bCityEdit)):
			self.m_bChangeFocus = False
			setFocusToCVG()
		return

	# Will update the screen (every 250 MS)
	def updateScreen(self):
		screen = CyGInterfaceScreen( "WorldBuilderScreen", CvScreenEnums.WORLDBUILDER_SCREEN )

		if (CyInterface().isInAdvancedStart()):
			if (self.m_bSideMenuDirty):
				self.refreshSideMenu()
			if (self.m_bASItemCostDirty):
				self.refreshASItemCost()

		if (CyInterface().isDirty(InterfaceDirtyBits.Advanced_Start_DIRTY_BIT) and not CyInterface().isFocusedWidget()):
			self.refreshAdvancedStartTabCtrl(true)
			CyInterface().setDirty(InterfaceDirtyBits.Advanced_Start_DIRTY_BIT, false)

		if (self.useLargeBrush()):
			self.m_bShowBigBrush = True
		else:
			self.m_bShowBigBrush = False

		if (self.m_bCtrlEditUp):
			if ( (not self.m_bUnitEdit) and (not self.m_bCityEdit) and (not self.m_tabCtrlEdit.isEnabled()) and not CyInterface().isInAdvancedStart()):
				if (self.m_bNormalMap):
					self.m_normalMapTabCtrl.enable(True)
				if (self.m_bNormalPlayer):
					self.m_normalPlayerTabCtrl.enable(True)
				self.m_bCtrlEditUp = False
				return 0
		if ((self.m_bNormalMap) and(self.m_normalMapTabCtrl.getActiveTab() == self.m_iTerrainTabID) and (self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iRouteListID)):
			if (self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()] == gc.getNumRouteInfos()):
				if (self.m_pRiverStartPlot != -1):
					self.setRiverHighlights()
					return 0
		self.highlightBrush()
		return 0

	def redraw( self ):
		return 0
	def handleAllPlotsCB ( self, popupReturn ) :
		iButton = popupReturn.getButtonClicked()
		if (iButton < PlotTypes.NUM_PLOT_TYPES):
			iTempVal = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
			self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()] = iButton
			self.setAllPlots()
			self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()] = iTempVal
		if (not (self.m_bUnitEdit or self.m_bCityEdit)):
			self.m_normalPlayerTabCtrl.enable(self.m_bNormalPlayer)
			self.m_normalMapTabCtrl.enable(self.m_bNormalMap)
		else:
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
		return 1

	def allPlotsCB(self):
		self.m_normalPlayerTabCtrl.enable(False)
		self.m_normalMapTabCtrl.enable(False)
		if (self.m_tabCtrlEdit != 0):
			self.m_tabCtrlEdit.enable(False)

		popup = CyPopup(CvUtil.EventWBAllPlotsPopup, EventContextTypes.EVENTCONTEXT_ALL, True)
		iPopupWidth = 200
		iPopupHeight = 50*PlotTypes.NUM_PLOT_TYPES
		popup.setSize(iPopupWidth, iPopupHeight)

		popup.setHeaderString(localText.getText("TXT_KEY_WB_CHANGE_ALL_PLOTS",()), CvUtil.FONT_CENTER_JUSTIFY)

		for i in range (PlotTypes.NUM_PLOT_TYPES):
			if (i==0):
				popup.addButton(localText.getText("TXT_KEY_WB_ADD_MOUNTAIN",()))
			elif(i==1):
				popup.addButton(localText.getText("TXT_KEY_WB_ADD_HILL",()))
			elif(i==2):
				popup.addButton(localText.getText("TXT_KEY_WB_ADD_GRASS",()))
			elif(i==3):
				popup.addButton(localText.getText("TXT_KEY_WB_ADD_OCEAN",()))

		popup.addButton(localText.getText("TXT_KEY_SCREEN_CANCEL", ()))
		popup.launch(False, PopupStates.POPUPSTATE_IMMEDIATE)
		return 1

	def refreshReveal ( self ) :
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		for i in range (CyMap().getGridWidth()):
			for j in range (CyMap().getGridHeight()):
				pPlot = CyMap().plot(i,j)
				if (not pPlot.isNone()):
					self.showRevealed(pPlot)
		return 1

	def setAllPlots ( self ) :
		iPlotType = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
		CyMap().setAllPlotTypes(iPlotType)
		#for i in range (CyMap().getGridWidth()):
			#for j in range (CyMap().getGridHeight()):
				#CyMap().plot(i,j).setPlotType(PlotTypes(iPlotType), True, True)
		return 1

	def handleUnitEditExperienceCB (self, argsList) :
		iNewXP = int(argsList[0])
		self.m_pActivePlot.getUnit(self.m_iCurrentUnit).setExperience(iNewXP,-1)
		return 1

	def handleUnitEditLevelCB (self, argsList) :
		iNewLevel = int(argsList[0])
		self.m_pActivePlot.getUnit(self.m_iCurrentUnit).setLevel(iNewLevel)
		return 1

	def handleUnitEditValueCB (self, argsList) :
		iNewValue = int(argsList[0])
		self.m_pActivePlot.getUnit(self.m_iCurrentUnit).setYieldStored(iNewValue)
		return 1

	def handleUnitEditNameCB (self, argsList) :
		if ((len(argsList[0]) < 1) or (self.m_pActivePlot == 0) or (self.m_iCurrentUnit < 0) or (self.m_pActivePlot.getNumUnits() <= self.m_iCurrentUnit)):
			return 1
		szNewName = argsList[0]
		unit = self.m_pActivePlot.getUnit(self.m_iCurrentUnit)
		if (unit):
			unit.setName(szNewName)
		return 1

	def handleCityEditPopulationCB (self, argsList) :
		iNewPop = int(argsList[0])
		self.m_pActivePlot.getPlotCity().setPopulation(iNewPop)
		return 1

	def handleCityEditCultureCB (self, argsList) :
		iNewCulture = int(argsList[0])
		self.m_pActivePlot.getPlotCity().setCulture(self.m_pActivePlot.getPlotCity().getOwner(), iNewCulture, True)
		return 1

	def handleCityEditGoldCB (self, argsList) :
		iNewGold = int(argsList[0])
		gc.getPlayer(self.m_iCurrentPlayer).setGold(iNewGold)
		return 1

	def handleCityOccupationTimerCB (self, argsList) :
		iOccupationTimer = int(argsList[0])
		self.m_pActivePlot.getPlotCity().setOccupationTimer(iOccupationTimer)
		return 1

	def handleCityEditNameCB (self, argsList) :
		if ((len(argsList[0]) < 1) or (not self.m_pActivePlot.isCity())):
			return 1
		szNewName = argsList[0]
		city = self.m_pActivePlot.getPlotCity()
		if (city):
			city.setName(szNewName, False)
		return 1

	def handleUnitEditPullDownCB ( self, argsList ) :
		self.m_iCurrentUnit = int(argsList[0])
		self.m_iCurrentUnitPlayer = self.m_pActivePlot.getUnit(self.m_iCurrentUnit).getOwner()
		self.setUnitEditInfo(True)
		self.setEditUnitTabs()
		return 1

	def handleUnitAITypeEditPullDownCB ( self, argsList ) :
		iNewAIType = int(argsList[0])
		self.m_pActivePlot.getUnit(self.m_iCurrentUnit).setUnitAIType(iNewAIType)
		return 1

	def handleUnitProfessionEditPullDownCB ( self, argsList ) :
		iNewProfession = 0
		for i in range(gc.getNumProfessionInfos()):
			if self.m_pActivePlot.getUnit(self.m_iCurrentUnit).canHaveProfession(i):
				if iNewProfession == int(argsList[0]):
					self.m_pActivePlot.getUnit(self.m_iCurrentUnit).setProfession(i)
					return 1
				iNewProfession += 1
		return 0

	def handlePlayerUnitPullDownCB ( self, argsList ) :
		iIndex = int(argsList)
		iCount = -1
		for i in range( gc.getMAX_CIV_PLAYERS() ):
			if ( gc.getPlayer(i).isEverAlive() ):
				iCount = iCount + 1
				if (iCount == iIndex):
					self.m_iCurrentPlayer = i
					self.refreshPlayerTabCtrl()
					return 1

		i = i + 1
		self.m_iCurrentPlayer = i
		self.refreshPlayerTabCtrl()
		return 1
	def handleSelectTeamPullDownCB ( self, argsList ) :
		iIndex = int(argsList)
		iCount = -1
		for i in range( gc.getMAX_CIV_TEAMS() ):
			if ( gc.getTeam(i).isEverAlive() ):
				iCount = iCount + 1
				if (iCount == iIndex):
					self.m_iCurrentTeam = i

		self.refreshReveal()
		return 1

	def hasPromotion(self, iPromotion):
		return self.m_pActivePlot.getUnit(self.m_iCurrentUnit).isHasRealPromotion(iPromotion)

	def isHasBuilding(self, iBuilding):
		return self.m_pActivePlot.getPlotCity().isHasRealBuilding(iBuilding)

	def handleBrushWidthCB (self, argsList):
		if (int(argsList) == 0):
			self.m_iBrushWidth = int(1)
		elif (int(argsList) == 1):
			self.m_iBrushWidth = int(2)
		elif (int(argsList) == 2):
			self.m_iBrushWidth = int(3)
		return 1

	def handleBrushHeightCB (self, argsList):
		if (int(argsList) == 0):
			self.m_iBrushHeight = int(1)
		elif (int(argsList) == 1):
			self.m_iBrushHeight = int(2)
		elif (int(argsList) == 2):
			self.m_iBrushHeight = int(3)
		return 1

	########################################################
	### Advanced Start Stuff
	########################################################

	def refreshASItemCost(self):

		if (CyInterface().isInAdvancedStart()):

			self.m_iCost = 0

			if (self.m_pCurrentPlot != 0):

#				if (not self.m_pCurrentPlot.isAdjacentNonrevealed(CyGame().getActiveTeam()) and self.m_pCurrentPlot.isRevealed(CyGame().getActiveTeam(), false)):
				if (self.m_pCurrentPlot.isRevealed(CyGame().getActiveTeam(), false)):

					# Unit mode
					if (self.getASActiveUnit() != -1):
						self.m_iCost = gc.getPlayer(self.m_iCurrentPlayer).getAdvancedStartUnitCost(self.getASActiveUnit(), true, self.m_pCurrentPlot)
					elif (self.getASActiveCity() != -1):
						self.m_iCost = gc.getPlayer(self.m_iCurrentPlayer).getAdvancedStartCityCost(true, self.m_pCurrentPlot)
					elif (self.getASActivePop() != -1 and self.m_pCurrentPlot.isCity()):
						self.m_iCost = gc.getPlayer(self.m_iCurrentPlayer).getAdvancedStartPopCost(true, self.m_pCurrentPlot.getPlotCity())
					elif (self.getASActiveCulture() != -1 and self.m_pCurrentPlot.isCity()):
						self.m_iCost = gc.getPlayer(self.m_iCurrentPlayer).getAdvancedStartCultureCost(true, self.m_pCurrentPlot.getPlotCity())
					elif (self.getASActiveBuilding() != -1 and self.m_pCurrentPlot.isCity()):
						self.m_iCost = gc.getPlayer(self.m_iCurrentPlayer).getAdvancedStartBuildingCost(self.getASActiveBuilding(), true, self.m_pCurrentPlot.getPlotCity())
					elif (self.getASActiveRoute() != -1):
						self.m_iCost = gc.getPlayer(self.m_iCurrentPlayer).getAdvancedStartRouteCost(self.getASActiveRoute(), true, self.m_pCurrentPlot)
					elif (self.getASActiveImprovement() != -1):
						self.m_iCost = gc.getPlayer(self.m_iCurrentPlayer).getAdvancedStartImprovementCost(self.getASActiveImprovement(), true, self.m_pCurrentPlot)

				elif (self.m_pCurrentPlot.isAdjacentNonrevealed(CyGame().getActiveTeam())):
					if (self.getASActiveVisibility() != -1):
						self.m_iCost = gc.getPlayer(self.m_iCurrentPlayer).getAdvancedStartVisibilityCost(true, self.m_pCurrentPlot)

			if (self.m_iCost < 0):
				self.m_iCost = 0

			self.refreshSideMenu()

	def getASActiveUnit(self):
		# Unit Tab
		if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASUnitTabID):
			iUnitType = getASUnit(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])
			return iUnitType

		return -1

	def getASActiveCity(self):
		# City Tab
		if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASCityTabID):
			# City List
			if (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASCityListID):
				iOptionID = self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()]
				# Place City
				if (iOptionID == 0):
					return 1

		return -1

	def getASActivePop(self):
		# City Tab
		if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASCityTabID):
			# City List
			if (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASCityListID):
				iOptionID = self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()]
				# Place Pop
				if (iOptionID == 1):
					return 1

		return -1

	def getASActiveCulture(self):
		# City Tab
		if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASCityTabID):
			# City List
			if (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASCityListID):
				iOptionID = self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()]
				# Place Culture
				if (iOptionID == 2):
					return 1

		return -1

	def getASActiveBuilding(self):
		# Building Tab
		if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASCityTabID):
			# Buildings List
			if (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASBuildingsListID):
				iBuildingType = getASBuilding(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])
				return iBuildingType

		return -1

	def getASActiveRoute(self):
		# Improvements Tab
		if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASImprovementsTabID):
			# Routes List
			if (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASRoutesListID):
				iRouteType = getASRoute(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])
				if -1 == iRouteType:
					self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] = self.m_iASImprovementsListID
				return iRouteType

		return -1

	def getASActiveImprovement(self):
		# Improvements Tab
		if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASImprovementsTabID):
			# Improvements List
			if (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASImprovementsListID):
				iImprovementType = getASImprovement(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])
				if -1 == iImprovementType:
					self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] = self.m_iASRoutesListID
				return iImprovementType

		return -1

	def getASActiveVisibility(self):
		# Visibility Tab
		if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASVisibilityTabID):
			return 1
		return -1

	def placeObject( self ) :

		# Advanced Start
		if (CyInterface().isInAdvancedStart()):

			pPlayer = gc.getPlayer(self.m_iCurrentPlayer)
			pPlot = CyMap().plot(self.m_iCurrentX, self.m_iCurrentY)

			iActiveTeam = CyGame().getActiveTeam()
			if (self.m_pCurrentPlot.isRevealed(iActiveTeam, false)):

				# City Tab
				if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASCityTabID):

					# City List
					if (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASCityListID):

						iOptionID = self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()]

						# Place City
						if (iOptionID == 0):

							# Cost -1 means may not be placed here
							if (pPlayer.getAdvancedStartCityCost(true, pPlot) != -1):

								CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_CITY, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, -1, true)	#Action, Player, X, Y, Data, bAdd

						# City Population
						elif (iOptionID == 1):

							if (pPlot.isCity()):
								pCity = pPlot.getPlotCity()

								# Cost -1 means may not be placed here
								if (pPlayer.getAdvancedStartPopCost(true, pCity) != -1):

										CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_POP, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, -1, true)	#Action, Player, X, Y, Data, bAdd

						# City Culture
						elif (iOptionID == 2):

							if (pPlot.isCity()):
								pCity = pPlot.getPlotCity()

								# Cost -1 means may not be placed here
								if (pPlayer.getAdvancedStartCultureCost(true, pCity) != -1):

									CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_CULTURE, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, -1, true)	#Action, Player, X, Y, Data, bAdd

					# Buildings List
					elif (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASBuildingsListID):

							if (pPlot.isCity()):
								pCity = pPlot.getPlotCity()

								iBuildingType = getASBuilding(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])

								# Cost -1 means may not be placed here
								if (iBuildingType != -1 and pPlayer.getAdvancedStartBuildingCost(iBuildingType, true, pCity) != -1):

									CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_BUILDING, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, iBuildingType, true)	#Action, Player, X, Y, Data, bAdd

				# Unit Tab
				elif (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASUnitTabID):
					iUnitType = getASUnit(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])

					# Cost -1 means may not be placed here
					if (iUnitType != -1 and pPlayer.getAdvancedStartUnitCost(iUnitType, true, pPlot) != -1):

						CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_UNIT, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, iUnitType, true)	#Action, Player, X, Y, Data, bAdd

				# Improvements Tab
				elif (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASImprovementsTabID):

					# Routes List
					if (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASRoutesListID):

						iRouteType = getASRoute(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])

						# Cost -1 means may not be placed here
						if (iRouteType != -1 and pPlayer.getAdvancedStartRouteCost(iRouteType, true, pPlot) != -1):

							CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_ROUTE, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, iRouteType, true)	#Action, Player, X, Y, Data, bAdd

					# Improvements List
					elif (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASImprovementsListID):

						iImprovementType = getASImprovement(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])

						# Cost -1 means may not be placed here
						if (pPlayer.getAdvancedStartImprovementCost(iImprovementType, true, pPlot) != -1):

							CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_IMPROVEMENT, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, iImprovementType, true)	#Action, Player, X, Y, Data, bAdd

			# Adjacent nonrevealed
			else:

				# Visibility Tab
				if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASVisibilityTabID):

					# Cost -1 means may not be placed here
					if (pPlayer.getAdvancedStartVisibilityCost(true, pPlot) != -1):

						CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_VISIBILITY, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, -1, true)	#Action, Player, X, Y, Data, bAdd

			self.m_bSideMenuDirty = true
			self.m_bASItemCostDirty = true

			return 1

		if ((self.m_iNormalPlayerCurrentIndexes[self.m_normalPlayerTabCtrl.getActiveTab()] == -1) or (self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()] == -1) or (self.m_iCurrentX == -1) or (self.m_iCurrentY == -1) or (self.m_iCurrentPlayer == -1)):
			return 1

		if (self.m_bEraseAll):
			self.eraseAll()
		elif ((self.m_bNormalPlayer) and (self.m_normalPlayerTabCtrl.getActiveTab() == self.m_iUnitTabID)):
			iUnitType = self.m_iNormalPlayerCurrentIndexes[self.m_normalPlayerTabCtrl.getActiveTab()]
			iProfession = gc.getUnitInfo(iUnitType).getDefaultProfession()
			pPlayer = gc.getPlayer(self.m_iCurrentPlayer)
			iPlotX = self.m_iCurrentX
			iPlotY = self.m_iCurrentY

			#initialize to 100 cargo
			iYieldAmount = 0;
			if gc.getUnitInfo(iUnitType).isTreasure():
				iYieldAmount = gc.getGame().getCargoYieldCapacity();
			else:
				for i in range(YieldTypes.NUM_YIELD_TYPES):
					if gc.getYieldInfo(i).getUnitClass() == gc.getUnitInfo(iUnitType).getUnitClassType():
						iYieldAmount = gc.getGame().getCargoYieldCapacity();
						break;

			pUnit = pPlayer.initUnit(iUnitType, iProfession, iPlotX, iPlotY, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION, iYieldAmount)

		elif ((self.m_bNormalPlayer) and (self.m_normalPlayerTabCtrl.getActiveTab() == self.m_iBuildingTabID)):
			iBuildingType = self.m_iNormalPlayerCurrentIndexes[self.m_normalPlayerTabCtrl.getActiveTab()]
			if ((self.m_pCurrentPlot.isCity()) and (iBuildingType != 0)):
				self.m_pCurrentPlot.getPlotCity().setHasRealBuilding(iBuildingType-1, true)
			if (iBuildingType == 0):
				if (not self.m_pCurrentPlot.isCity()):
					pPlayer = gc.getPlayer(self.m_iCurrentPlayer)
					iX = self.m_pCurrentPlot.getX()
					iY = self.m_pCurrentPlot.getY()
					pCity = pPlayer.initCity(iX, iY)
					pCity.setPopulation(1)
		elif ((self.m_bNormalMap) and (self.m_normalMapTabCtrl.getActiveTab() == self.m_iImprovementTabID)):
			iImprovementType = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
			iIndex = -1
			iCounter = -1
			while ((iIndex < iImprovementType) and (iCounter < gc.getNumImprovementInfos())):
				iCounter = iCounter + 1
				if (not gc.getImprovementInfo(iCounter).isGraphicalOnly()):
					iIndex = iIndex + 1
			if (iIndex > -1):
				self.m_pCurrentPlot.setImprovementType(iCounter)
		elif ((self.m_bNormalMap) and (self.m_normalMapTabCtrl.getActiveTab() == self.m_iBonusTabID)):
			iBonusType = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
			self.m_pCurrentPlot.setBonusType(iBonusType)
		elif ((self.m_bNormalMap) and (self.m_normalMapTabCtrl.getActiveTab() == self.m_iTerrainTabID)):
			if (self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iTerrainListID):
				iTerrainType = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
				self.m_pCurrentPlot.setTerrainType(iTerrainType, True, True)
			elif (self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iFeatureListID):
				iButtonIndex = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
				iCount = -1
				for i in range (gc.getNumFeatureInfos()):
					for j in range (gc.getFeatureInfo(i).getNumVarieties()):
						iCount = iCount + 1
						if (iCount == iButtonIndex):
							self.m_pCurrentPlot.setFeatureType(i, j)
			elif (self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iPlotTypeListID):
				iPlotType = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
				if (iPlotType >= 0) and (iPlotType < PlotTypes.NUM_PLOT_TYPES):
					self.m_pCurrentPlot.setPlotType(PlotTypes(iPlotType), True, True)
			elif (self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iRouteListID):
				iRouteType = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
				if (iRouteType == gc.getNumRouteInfos()):
					if (self.m_pRiverStartPlot == self.m_pCurrentPlot):
						self.m_pRiverStartPlot = -1
						CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
						return 1
					if (self.m_pRiverStartPlot != -1):
						iXDiff = 0
						iYDiff = 0
						if (self.m_pRiverStartPlot.getX() < self.m_pCurrentPlot.getX()):
							iXDiff = self.m_pCurrentPlot.getX() - self.m_pRiverStartPlot.getX()
						elif (self.m_pRiverStartPlot.getX() > self.m_pCurrentPlot.getX()):
							iXDiff = self.m_pRiverStartPlot.getX() - self.m_pCurrentPlot.getX()
						if (self.m_pRiverStartPlot.getY() < self.m_pCurrentPlot.getY()):
							iYDiff = self.m_pCurrentPlot.getY() - self.m_pRiverStartPlot.getY()
						elif (self.m_pRiverStartPlot.getY() > self.m_pCurrentPlot.getY()):
							iYDiff = self.m_pRiverStartPlot.getY() - self.m_pCurrentPlot.getY()

						if ((iXDiff == iYDiff) and (iXDiff == 1) and (self.m_pRiverStartPlot.getX() > self.m_pCurrentPlot.getX()) and (self.m_pRiverStartPlot.getY() < self.m_pCurrentPlot.getY())):
							self.placeRiverNW(True)
							self.m_pRiverStartPlot = CyMap().plot(self.m_pRiverStartPlot.getX()-1, self.m_pRiverStartPlot.getY()+1)
						elif ((iXDiff == 0) and (iYDiff == 1) and (self.m_pRiverStartPlot.getY() < self.m_pCurrentPlot.getY())):
							self.placeRiverN(True)
							self.m_pRiverStartPlot = self.m_pCurrentPlot
						elif ((iXDiff == iYDiff) and (iXDiff == 1) and (self.m_pRiverStartPlot.getX() < self.m_pCurrentPlot.getX()) and (self.m_pRiverStartPlot.getY() < self.m_pCurrentPlot.getY())):
							self.placeRiverNE(True)
							self.m_pRiverStartPlot = self.m_pCurrentPlot
						elif ((iXDiff == 1) and (iYDiff == 0) and (self.m_pRiverStartPlot.getX() > self.m_pCurrentPlot.getX())):
							self.placeRiverW(True)
							self.m_pRiverStartPlot = self.m_pCurrentPlot
						elif ((iXDiff == 1) and (iYDiff == 0) and (self.m_pRiverStartPlot.getX() < self.m_pCurrentPlot.getX())):
							self.placeRiverE(True)
							self.m_pRiverStartPlot = self.m_pCurrentPlot
						elif ((iXDiff == iYDiff) and (iXDiff == 1) and (self.m_pRiverStartPlot.getX() > self.m_pCurrentPlot.getX()) and (self.m_pRiverStartPlot.getY() > self.m_pCurrentPlot.getY())):
							self.placeRiverSW(True)
							self.m_pRiverStartPlot = self.m_pCurrentPlot
						elif ((iXDiff == 0) and (iYDiff == 1) and (self.m_pRiverStartPlot.getY() > self.m_pCurrentPlot.getY())):
							self.placeRiverS(True)
							self.m_pRiverStartPlot = self.m_pCurrentPlot
						elif ((iXDiff == iYDiff) and (iXDiff == 1) and (self.m_pRiverStartPlot.getX() < self.m_pCurrentPlot.getX()) and (self.m_pRiverStartPlot.getY() > self.m_pCurrentPlot.getY())):
							self.placeRiverSE(True)
							self.m_pRiverStartPlot = self.m_pCurrentPlot
						else:
							self.m_pRiverStartPlot = self.m_pCurrentPlot
					else:
						self.m_pRiverStartPlot = self.m_pCurrentPlot
				else:
					self.m_pCurrentPlot.setRouteType(iRouteType)
		elif ((self.m_bNormalMap) and (self.m_normalMapTabCtrl.getActiveTab() == self.m_iTerritoryTabID)):
			iSelectedIndex = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
			if (self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iTerritoryListID):
				if (gc.getPlayer(iSelectedIndex).isEverAlive()):
					self.m_pCurrentPlot.setOwner(iSelectedIndex)
			elif (self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iEuropeListID):
				self.m_pCurrentPlot.setEurope(iSelectedIndex)
		elif (self.m_bLandmark):
			CvEventInterface.beginEvent(CvUtil.EventWBLandmarkPopup)
		return 1

	def removeObject( self ):

		# Advanced Start
		if (CyInterface().isInAdvancedStart()):

			pPlayer = gc.getPlayer(self.m_iCurrentPlayer)
			pPlot = CyMap().plot(self.m_iCurrentX, self.m_iCurrentY)

			iActiveTeam = CyGame().getActiveTeam()
#			if (not self.m_pCurrentPlot.isAdjacentNonrevealed(iActiveTeam) and self.m_pCurrentPlot.isRevealed(iActiveTeam, false)):
			if (self.m_pCurrentPlot.isRevealed(iActiveTeam, false)):

				# City Tab
				if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASCityTabID):

					# City List
					if (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASCityListID):

						iOptionID = self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()]

						# Place City
						if (iOptionID == 0):

							# Ability to remove cities not allowed because of 'sploitz (visibility, chopping down jungle, etc.)
							return 1

							if (self.m_pCurrentPlot.isCity()):

								if (self.m_pCurrentPlot.getPlotCity().getOwner() == self.m_iCurrentPlayer):

									CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_CITY, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, -1, false)	#Action, Player, X, Y, Data, bAdd

						# City Population
						elif (iOptionID == 1):

							if (pPlot.isCity()):
								if (pPlot.getPlotCity().getOwner() == self.m_iCurrentPlayer):

									CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_POP, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, -1, false)	#Action, Player, X, Y, Data, bAdd

						# City Culture
						elif (iOptionID == 2):

							# Ability to remove cities not allowed because of 'sploitz (visibility)
							return 1

							if (pPlot.isCity()):
								if (pPlot.getPlotCity().getOwner() == self.m_iCurrentPlayer):

									CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_CULTURE, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, -1, false)	#Action, Player, X, Y, Data, bAdd

					# Buildings List
					elif (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASBuildingsListID):

						if (pPlot.isCity()):
							if (pPlot.getPlotCity().getOwner() == self.m_iCurrentPlayer):

								iBuildingType = getASBuilding(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])

								if -1 != iBuildingType:
									CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_BUILDING, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, iBuildingType, false)	#Action, Player, X, Y, Data, bAdd

				# Unit Tab
				elif (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASUnitTabID):

					iUnitType = getASUnit(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])

					if -1 != iUnitType:
						CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_UNIT, self.m_iCurrentPlayer, self.m_pCurrentPlot.getX(), self.m_pCurrentPlot.getY(), iUnitType, false)	#Action, Player, X, Y, Data, bAdd

				# Improvements Tab
				elif (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASImprovementsTabID):

					# Routes List
					if (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASRoutesListID):

						iRouteType = getASRoute(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])

						if -1 != iRouteType:
							CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_ROUTE, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, iRouteType, false)	#Action, Player, X, Y, Data, bAdd

					# Improvements List
					elif (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASImprovementsListID):

						iImprovementType = getASImprovement(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])

						if -1 != iImprovementType:
							CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_IMPROVEMENT, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, iImprovementType, false)	#Action, Player, X, Y, Data, bAdd

			# Adjacent nonrevealed
			else:

				# Visibility Tab
				if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASVisibilityTabID):

					# Ability to remove sight not allowed because of 'sploitz
					return 1

					# Remove Visibility
					if (pPlot.isRevealed(iActiveTeam, false)):

						CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_VISIBILITY, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, -1, false)	#Action, Player, X, Y, Data, bAdd

			self.m_bSideMenuDirty = true
			self.m_bASItemCostDirty = true

			return 1

		if ((self.m_iNormalPlayerCurrentIndexes[self.m_normalPlayerTabCtrl.getActiveTab()] == -1) or (self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()] == -1) or (self.m_iCurrentX == -1) or (self.m_iCurrentY == -1) or (self.m_iCurrentPlayer == -1)):
			return 1

		if (self.m_bEraseAll):
			self.eraseAll()
		elif ((self.m_bNormalPlayer) and (self.m_normalPlayerTabCtrl.getActiveTab() == self.m_iUnitTabID)):
			for i in range (self.m_pCurrentPlot.getNumUnits()):
				pUnit = self.m_pCurrentPlot.getUnit(i)
				if (pUnit.getUnitType() == self.m_iNormalPlayerCurrentIndexes[self.m_normalPlayerTabCtrl.getActiveTab()]):
					pUnit.kill(false)
					return 1
			if (self.m_pCurrentPlot.getNumUnits() > 0):
				pUnit = self.m_pCurrentPlot.getUnit(0)
				pUnit.kill(false)
				return 1
		elif ((self.m_bNormalPlayer) and (self.m_normalPlayerTabCtrl.getActiveTab() == self.m_iBuildingTabID)):
			if (self.m_pCurrentPlot.isCity()):
				iBuildingType = self.m_iNormalPlayerCurrentIndexes[self.m_normalPlayerTabCtrl.getActiveTab()]
				if (iBuildingType == 0) :
					self.m_pCurrentPlot.getPlotCity().kill()
				else:
					self.m_pCurrentPlot.getPlotCity().setHasRealBuilding(iBuildingType-1, false)
		elif ((self.m_bNormalMap) and (self.m_normalMapTabCtrl.getActiveTab() == self.m_iImprovementTabID)):
			self.m_pCurrentPlot.setImprovementType(-1)
			return 1
		elif ((self.m_bNormalMap) and (self.m_normalMapTabCtrl.getActiveTab() == self.m_iBonusTabID)):
			iBonusType = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
			self.m_pCurrentPlot.setBonusType(-1)
			return 1
		elif ((self.m_bNormalMap) and (self.m_normalMapTabCtrl.getActiveTab() == self.m_iTerrainTabID)):
			if (self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iTerrainListID):
				return 1
			elif (self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iFeatureListID):
				iFeatureType = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
				self.m_pCurrentPlot.setFeatureType(FeatureTypes.NO_FEATURE, -1)
				return 1
			elif (self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iPlotTypeListID):
				return 1
			elif (self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iRouteListID):
				iRouteType = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
				if (iRouteType == gc.getNumRouteInfos()):
					if (self.m_pRiverStartPlot != -1):
						self.m_pRiverStartPlot = -1
						CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
					else:
						self.m_pCurrentPlot.setNOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
						self.m_pCurrentPlot.setWOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
				else:
					self.m_pCurrentPlot.setRouteType(-1)
		elif ((self.m_bNormalMap) and (self.m_normalMapTabCtrl.getActiveTab() == self.m_iTerritoryTabID)):
			if (self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iTerritoryListID):
				self.m_pCurrentPlot.setOwner(-1)
			elif (self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iEuropeListID):
				self.m_pCurrentPlot.setEurope(-1)
			return 1
		elif (self.m_bLandmark):
			self.removeLandmarkCB()
		return 1

	def handleClicked( self ):
		return

	def setEditUnitTabs ( self ):
		self.m_tabCtrlEdit.setDropDownSelection("Choose Unit", "Current_Unit", self.m_iCurrentUnit)
		self.m_tabCtrlEdit.setDropDownSelection("Choose Profession", "Unit_Profession", self.getUnitProfessionIndex())
		self.m_tabCtrlEdit.setDropDownSelection("Choose Unit", "Unit_AI_Type", self.m_pActivePlot.getUnit(self.m_iCurrentUnit).getUnitAIType())
		return

	def isIntString ( self, arg ):
		for i in range (len(arg)):
			if (arg[i] > '9') :
				return False
			elif (arg[i] < '0') :
				return False
		return True

	def placeRiverNW ( self, bUseCurrent ):
		if (bUseCurrent):
			pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if (not pRiverStepPlot.isNone()):
				pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_WEST)

		pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX()-1, self.m_pRiverStartPlot.getY())
		if (not pRiverStepPlot.isNone()):
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
		pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX()-1, self.m_pRiverStartPlot.getY()+1)
		if (not pRiverStepPlot.isNone()):
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
		return

	def placeRiverN ( self, bUseCurrent ):
		if (bUseCurrent):
			pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if (not pRiverStepPlot.isNone()):
				pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_NORTH)

		pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY()+1)
		if (not pRiverStepPlot.isNone()):
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
		return

	def placeRiverNE ( self, bUseCurrent ):
		if (bUseCurrent):
			pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if (not pRiverStepPlot.isNone()):
				pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_EAST)

		pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX()+1, self.m_pRiverStartPlot.getY())
		if (not pRiverStepPlot.isNone()):
			pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_EAST)
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
		pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX()+1, self.m_pRiverStartPlot.getY()+1)
		if (not pRiverStepPlot.isNone()):
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
		return

	def placeRiverW ( self, bUseCurrent ):
		if (bUseCurrent):
			pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if (not pRiverStepPlot.isNone()):
				pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_WEST)

		pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX()-1, self.m_pRiverStartPlot.getY())
		if (not pRiverStepPlot.isNone()):
			pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_WEST)
		return

	def placeRiverE ( self, bUseCurrent ):
		if (bUseCurrent):
			pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if (not pRiverStepPlot.isNone()):
				pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_EAST)

		pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX()+1, self.m_pRiverStartPlot.getY())
		if (not pRiverStepPlot.isNone()):
			pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_EAST)
		return

	def placeRiverSW ( self, bUseCurrent ):
		if (bUseCurrent):
			pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if (not pRiverStepPlot.isNone()):
				pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_WEST)

		pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX()-1, self.m_pRiverStartPlot.getY()-1)
		if (not pRiverStepPlot.isNone()):
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)
		return

	def placeRiverS ( self, bUseCurrent ):
		if (bUseCurrent):
			pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if (not pRiverStepPlot.isNone()):
				pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)

		pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY()-1)
		if (not pRiverStepPlot.isNone()):
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)
		return

	def placeRiverSE ( self, bUseCurrent ):
		if (bUseCurrent):
			pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if (not pRiverStepPlot.isNone()):
				pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_EAST)

		pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX()+1, self.m_pRiverStartPlot.getY())
		if (not pRiverStepPlot.isNone()):
			pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_EAST)
		pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX()+1, self.m_pRiverStartPlot.getY()-1)
		if (not pRiverStepPlot.isNone()):
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)
		return

	def setUnitEditInfo(self, bSamePlot):
		initWBToolEditCtrl()
		self.m_tabCtrlEdit = getWBToolEditTabCtrl()

		self.m_bUnitEditCtrl = True
		self.m_bCityEditCtrl = False

		if (self.m_bFlyout):
			self.m_bFlyout = False
			self.m_pCurrentPlot = self.m_pFlyoutPlot

		if (not bSamePlot):
			self.m_pActivePlot = self.m_pCurrentPlot

		self.m_tabCtrlEdit.setNumColumns((gc.getNumPromotionInfos()/10)+1)
		self.m_tabCtrlEdit.setColumnLength(20)
		self.m_tabCtrlEdit.addTabSection(localText.getText("TXT_KEY_WB_CHOOSE_UNIT",()))
		strTest = ()
		for i in range(self.m_pActivePlot.getNumUnits()):
			if (len(self.m_pActivePlot.getUnit(i).getNameNoDesc())):
				strTest = strTest + (self.m_pActivePlot.getUnit(i).getNameNoDesc(),)
			else:
				strTest = strTest + (self.m_pActivePlot.getUnit(i).getName(),)

		self.m_tabCtrlEdit.addSectionDropdown("Current_Unit", strTest, "CvScreensInterface", "WorldBuilderHandleUnitEditPullDownCB", "UnitEditPullDown", 0, self.m_iCurrentUnit)

		if (len(self.m_pActivePlot.getUnit(self.m_iCurrentUnit).getNameNoDesc())):
			strName = self.m_pActivePlot.getUnit(self.m_iCurrentUnit).getNameNoDesc()
		else:
			strName = self.m_pActivePlot.getUnit(self.m_iCurrentUnit).getName()
		self.m_tabCtrlEdit.addSectionEditCtrl(strName, "CvScreensInterface", "WorldBuilderHandleUnitEditNameCB", "UnitEditName", 0)
		self.m_tabCtrlEdit.addSectionLabel(localText.getText("TXT_KEY_WB_EXPERIENCE",()),  0)
		strExperience = str("UnitEditExperienceCB")
		self.m_tabCtrlEdit.addSectionSpinner(
			strExperience,
			"CvScreensInterface",
			"WorldBuilderHandleUnitEditExperienceCB",
			"UnitEditExperience",
			0,
			0.0,
			1000.0,
			1.0,
			self.m_pActivePlot.getUnit(self.m_iCurrentUnit).getExperience(),
			0,
			0)
		self.m_tabCtrlEdit.addSectionLabel(localText.getText("TXT_KEY_WB_LEVEL",()),  0)
		strLevel = str("UnitEditLevelCB")
		self.m_tabCtrlEdit.addSectionSpinner(
			strLevel,
			"CvScreensInterface",
			"WorldBuilderHandleUnitEditLevelCB",
			"UnitEditLevel",
			0,
			1.0,
			1000.0,
			1.0,
			self.m_pActivePlot.getUnit(self.m_iCurrentUnit).getLevel(),
			0,
			0)

		self.m_tabCtrlEdit.addSectionLabel(localText.getText("TXT_KEY_DEMO_SCREEN_VALUE_TEXT",()),  0)
		strValue = str("UnitEditValueCB")
		self.m_tabCtrlEdit.addSectionSpinner(
			strValue,
			"CvScreensInterface",
			"WorldBuilderHandleUnitEditValueCB",
			"UnitEditValue",
			0,
			1.0,
			1000.0,
			1.0,
			self.m_pActivePlot.getUnit(self.m_iCurrentUnit).getYieldStored(),
			0,
			0)

		strTest = ()
		for i in range(gc.getNumProfessionInfos()):
			if self.m_pActivePlot.getUnit(self.m_iCurrentUnit).canHaveProfession(i):
				strTest = strTest + (gc.getProfessionInfo(i).getDescription(),)

		self.m_tabCtrlEdit.addSectionDropdown("Unit_Profession", strTest, "CvScreensInterface", "WorldBuilderHandleUnitProfessionEditPullDownCB", "UnitProfessionEditPullDown", 0, self.getUnitProfessionIndex())

		strTest = ()
		for i in range(UnitAITypes.NUM_UNITAI_TYPES):
			strTest = strTest + (gc.getUnitAIInfo(i).getDescription(),)
		self.m_tabCtrlEdit.addSectionDropdown("Unit_AI_Type", strTest, "CvScreensInterface", "WorldBuilderHandleUnitAITypeEditPullDownCB", "UnitAITypeEditPullDown", 0, self.m_pActivePlot.getUnit(self.m_iCurrentUnit).getUnitAIType())

		self.m_tabCtrlEdit.addSectionButton(localText.getText("TXT_KEY_WB_ADD_SCRIPT",()), "CvScreensInterface", "WorldBuilderHandleUnitEditAddScriptCB", "UnitEditAddScript", 0)

		initWBToolEditCtrlTab(True)

		if (not self.m_tabCtrlEdit.isNone()):
			print("Enabling map control 4")
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
			self.m_bCtrlEditUp = True
		return

	def getUnitProfessionIndex(self):
		currentProfessionIndex = 0
		iIndex = 0
		for i in range(gc.getNumProfessionInfos()):
			if self.m_pActivePlot.getUnit(self.m_iCurrentUnit).canHaveProfession(i):
				if self.m_pActivePlot.getUnit(self.m_iCurrentUnit).getProfession() == i:
					currentProfessionIndex = iIndex
				iIndex += 1

		return currentProfessionIndex

	def setCityEditInfo(self):
		self.m_bUnitEditCtrl = False
		self.m_bCityEditCtrl = True

		if (self.m_bFlyout):
			self.m_bFlyout = False
			self.m_pCurrentPlot = self.m_pFlyoutPlot

		initWBToolEditCtrl()
		self.m_tabCtrlEdit = getWBToolEditTabCtrl()
		self.m_pActivePlot = self.m_pCurrentPlot

		self.m_tabCtrlEdit.setNumColumns((gc.getNumBuildingInfos()/10)+2)
		self.m_tabCtrlEdit.setColumnLength(20)
		self.m_tabCtrlEdit.addTabSection(localText.getText("TXT_KEY_WB_CITY_DATA",()))
		strName = self.m_pActivePlot.getPlotCity().getName()
		self.m_tabCtrlEdit.addSectionEditCtrl(strName, "CvScreensInterface", "WorldBuilderHandleCityEditNameCB", "CityEditName", 0)
		self.m_tabCtrlEdit.addSectionLabel(localText.getText("TXT_KEY_WB_POPULATION",()),  0)
		strPopulation = str("CityEditPopulationCB")
		self.m_tabCtrlEdit.addSectionSpinner(
			strPopulation,
			"CvScreensInterface",
			"WorldBuilderHandleCityEditPopulationCB",
			"CityEditPopulation",
			0,
			1.0,
			1000.0,
			1.0,
			self.m_pActivePlot.getPlotCity().getPopulation(),
			0,
			0)
		self.m_tabCtrlEdit.addSectionLabel(localText.getText("TXT_KEY_WB_CULTURE",()),  0)
		strCulture = str("CityEditCultureCB")
		self.m_tabCtrlEdit.addSectionSpinner(
			strCulture,
			"CvScreensInterface",
			"WorldBuilderHandleCityEditCultureCB",
			"CityEditCulture",
			0,
			1.0,
			100000000.0,
			1.0,
			self.m_pActivePlot.getPlotCity().getCulture(self.m_pActivePlot.getPlotCity().getOwner()),
			0,
			0)
		self.m_tabCtrlEdit.addSectionLabel(localText.getText("TXT_KEY_WB_GOLD",()),  0)
		strGold = str("CityEditGoldCB")
		self.m_tabCtrlEdit.addSectionSpinner(
			strGold,
			"CvScreensInterface",
			"WorldBuilderHandleCityEditGoldCB",
			"CityEditGold",
			0,
			-1000.0,
			5000.0,
			1.0,
			gc.getPlayer(self.m_iCurrentPlayer).getGold(),
			0,
			0)
		self.m_tabCtrlEdit.addSectionLabel("Occupation timer",  0)
		strOccupationTimer = str("CityEditOccupationTimerCB")
		self.m_tabCtrlEdit.addSectionSpinner(
			strOccupationTimer,
			"CvScreensInterface",
			"WorldBuilderHandleCityOccupationTimerCB",
			"CityEditOccupationTimer",
			0,
			-1000.0,
			5000.0,
			1.0,
			0,
			0,
			0)
		self.m_tabCtrlEdit.addSectionButton(localText.getText("TXT_KEY_WB_ADD_SCRIPT",()), "CvScreensInterface", "WorldBuilderHandleCityEditAddScriptCB", "CityEditAddScript", 0)

		initWBToolEditCtrlTab(False)

		if (not self.m_tabCtrlEdit.isNone()):
			print("Enabling map control 5")
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
			self.m_bCtrlEditUp = True
		return

	def initCityEditScreen(self):
		self.setCityEditInfo()
		return

	def toggleUnitEditCB(self):
		self.m_bUnitEdit = True
		self.m_bCityEdit = False
		self.m_bNormalPlayer = False
		self.m_bNormalMap = False
		self.m_bReveal = False
		self.m_bDiplomacy = False
		self.m_bLandmark = False
		self.m_bEraseAll = False
		CvScreensInterface.hideWorldBuilderDiplomacyScreen()

		if (self.m_tabCtrlEdit != 0):
			print("Enabling map control 6")
			self.m_tabCtrlEdit.enable(False)

		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)

		self.refreshSideMenu()
		self.setCurrentModeCheckbox(self.m_iUnitEditCheckboxID)
		print("Enabling map control 7")
		self.m_normalPlayerTabCtrl.enable(False)
		self.m_normalMapTabCtrl.enable(False)
		if (self.m_tabCtrlEdit != 0):
			self.m_tabCtrlEdit.destroy()
		return

	def toggleCityEditCB(self):
		self.m_bCityEdit = True
		self.m_bUnitEdit = False
		self.m_bNormalPlayer = False
		self.m_bNormalMap = False
		self.m_bReveal = False
		self.m_bDiplomacy = False
		self.m_bLandmark = False
		self.m_bEraseAll = False
		CvScreensInterface.hideWorldBuilderDiplomacyScreen()

		if (self.m_tabCtrlEdit != 0):
			print("Enabling map control 8")
			self.m_tabCtrlEdit.enable(False)

		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)

		self.refreshSideMenu()
		self.setCurrentModeCheckbox(self.m_iCityEditCheckboxID)
		print("Enabling map control 9")
		self.m_normalPlayerTabCtrl.enable(False)
		self.m_normalMapTabCtrl.enable(False)
		if (self.m_tabCtrlEdit != 0):
			self.m_tabCtrlEdit.destroy()
		return

	def normalPlayerTabModeCB(self):
		self.m_bCityEdit = False
		self.m_bUnitEdit = False
		self.m_bNormalPlayer = True
		self.m_bNormalMap = False
		self.m_bReveal = False
		self.m_bDiplomacy = False
		self.m_bLandmark = False
		self.m_bEraseAll = False
		CvScreensInterface.hideWorldBuilderDiplomacyScreen()

		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)

		self.refreshSideMenu()
		self.setCurrentModeCheckbox(self.m_iNormalPlayerCheckboxID)
		if (self.m_normalMapTabCtrl):
			print("Disabling Map Tab")
			self.m_normalMapTabCtrl.enable(False)
		if (not self.m_normalPlayerTabCtrl.isEnabled() and not CyInterface().isInAdvancedStart()):
			print("Enabling Player Tab")
			self.m_normalPlayerTabCtrl.enable(True)
			if (self.m_tabCtrlEdit):
				self.m_tabCtrlEdit.enable(False)
			self.m_bCtrlEditUp = False
		return

	def normalMapTabModeCB(self):
		self.m_bCityEdit = False
		self.m_bUnitEdit = False
		self.m_bNormalPlayer = False
		self.m_bNormalMap = True
		self.m_bReveal = False
		self.m_bDiplomacy = False
		self.m_bLandmark = False
		self.m_bEraseAll = False
		CvScreensInterface.hideWorldBuilderDiplomacyScreen()

		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)

		self.refreshSideMenu()
		self.setCurrentModeCheckbox(self.m_iNormalMapCheckboxID)
		if (self.m_normalPlayerTabCtrl):
			print("Disabling Player Tab")
			self.m_normalPlayerTabCtrl.enable(False)
		if (not self.m_normalMapTabCtrl.isEnabled() and not CyInterface().isInAdvancedStart()):
			print("Enabling Map Tab")
			self.m_normalMapTabCtrl.enable(True)
			if (self.m_tabCtrlEdit):
				self.m_tabCtrlEdit.enable(False)
			self.m_bCtrlEditUp = False
		return

	def revealTabModeCB(self):
		self.m_bCtrlEditUp = False
		self.m_bCityEdit = False
		self.m_bUnitEdit = False
		self.m_bNormalPlayer = False
		self.m_bNormalMap = False
		self.m_bReveal = True
		self.m_bDiplomacy = False
		self.m_bLandmark = False
		self.m_bEraseAll = False
		CvScreensInterface.hideWorldBuilderDiplomacyScreen()

		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)
		self.refreshReveal()
		self.refreshSideMenu()
		self.setCurrentModeCheckbox(self.m_iRevealTileCheckboxID)
		if (self.m_normalPlayerTabCtrl):
			self.m_normalPlayerTabCtrl.enable(False)
		if (self.m_normalMapTabCtrl):
			self.m_normalMapTabCtrl.enable(False)
		if (self.m_tabCtrlEdit):
			self.m_tabCtrlEdit.enable(False)
		return

	def diplomacyModeCB(self):
		self.m_bCtrlEditUp = False
		self.m_bCityEdit = False
		self.m_bUnitEdit = False
		self.m_bNormalPlayer = False
		self.m_bNormalMap = False
		self.m_bReveal = False
		self.m_bDiplomacy = True
		self.m_bLandmark = False
		self.m_bEraseAll = False

		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)
		self.refreshSideMenu()
		self.setCurrentModeCheckbox(self.m_iDiplomacyCheckboxID)
		if (self.m_normalPlayerTabCtrl != 0):
			self.m_normalPlayerTabCtrl.enable(False)
		if (self.m_normalMapTabCtrl != 0):
			self.m_normalMapTabCtrl.enable(False)
		if (self.m_tabCtrlEdit != 0):
			self.m_tabCtrlEdit.enable(False)

		CvScreensInterface.showWorldBuilderDiplomacyScreen()
		return

	def landmarkModeCB(self):
		self.m_bCtrlEditUp = False
		self.m_bCityEdit = False
		self.m_bUnitEdit = False
		self.m_bNormalPlayer = False
		self.m_bNormalMap = False
		self.m_bReveal = False
		self.m_bDiplomacy = False
		self.m_bLandmark = True
		self.m_bEraseAll = False
		CvScreensInterface.hideWorldBuilderDiplomacyScreen()

		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)
		self.refreshSideMenu()
		self.setCurrentModeCheckbox(self.m_iLandmarkCheckboxID)
		if (self.m_normalPlayerTabCtrl != 0):
			self.m_normalPlayerTabCtrl.enable(False)
		if (self.m_normalMapTabCtrl != 0):
			self.m_normalMapTabCtrl.enable(False)
		if (self.m_tabCtrlEdit != 0):
			self.m_tabCtrlEdit.enable(False)
		return

	def eraseCB(self):
		self.m_bCtrlEditUp = False
		self.m_bCityEdit = False
		self.m_bUnitEdit = False
		self.m_bNormalPlayer = False
		self.m_bNormalMap = False
		self.m_bReveal = False
		self.m_bDiplomacy = False
		self.m_bLandmark = False
		self.m_bEraseAll = True
		self.m_pRiverStartPlot = -1
		CvScreensInterface.hideWorldBuilderDiplomacyScreen()

		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)
		self.refreshSideMenu()
		self.setCurrentModeCheckbox(self.m_iEraseCheckboxID)
		if (self.m_normalPlayerTabCtrl != 0):
			self.m_normalPlayerTabCtrl.enable(False)
		if (self.m_normalMapTabCtrl != 0):
			self.m_normalMapTabCtrl.enable(False)
		if (self.m_tabCtrlEdit != 0):
			self.m_tabCtrlEdit.enable(False)
		return

	def createFlyoutMenu(self):
		if (self.m_pCurrentPlot == 0):
			return
		self.m_flyoutMenu = CyGFlyoutMenu()
		self.m_pFlyoutPlot = self.m_pCurrentPlot
		if (self.m_pFlyoutPlot.getNumUnits() > 0):
			for i in range( self.m_pFlyoutPlot.getNumUnits() ):
				if (len(self.m_pFlyoutPlot.getUnit(i).getNameNoDesc())):
					strName = self.m_pFlyoutPlot.getUnit(i).getNameNoDesc()
				else:
					strName = self.m_pFlyoutPlot.getUnit(i).getName()
				self.m_flyoutMenu.addTextItem(strName, "CvScreensInterface", "WorldBuilderHandleFlyoutMenuCB", i+1)
		if (self.m_pFlyoutPlot.isCity()):
			self.m_flyoutMenu.addTextItem(localText.getText("TXT_KEY_WB_EDIT_CITY",()), "CvScreensInterface", "WorldBuilderHandleFlyoutMenuCB", self.m_iFlyoutEditCity)

		self.m_flyoutMenu.addTextItem(localText.getText("TXT_KEY_WB_ADD_SCRIPT_TO_PLOT",()), "CvScreensInterface", "WorldBuilderHandleFlyoutMenuCB", self.m_iFlyoutAddScript)
		self.m_flyoutMenu.addTextItem(localText.getText("TXT_KEY_WB_CHANGE_START_YEAR",()), "CvScreensInterface", "WorldBuilderHandleFlyoutMenuCB", self.m_iFlyoutChangeStartYear)
		self.m_flyoutMenu.show()
		return

	def destroyFlyoutMenu(self):
		if (self.m_flyoutMenu != 0):
			self.m_flyoutMenu.destroy()
			self.m_flyoutMenu = 0
		return

	def handleFlyoutMenuCB ( self, argsList ) :
		iFlyoutIndex = int(argsList[0])
		if (self.m_tabCtrlEdit != 0):
			self.m_tabCtrlEdit.enable(False)
		if (iFlyoutIndex == self.m_iFlyoutAddScript):
			self.m_pPlotToScript = self.m_pFlyoutPlot
			self.getScript()
		elif (iFlyoutIndex == self.m_iFlyoutChangeStartYear):
			self.getNewStartYear()
		elif (iFlyoutIndex == self.m_iFlyoutEditCity):
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
			self.m_bFlyout = True
			self.initCityEditScreen()
		else:
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
			self.m_iCurrentUnit = iFlyoutIndex-1
			self.m_bFlyout = True
			self.setUnitEditInfo(False)
		return 1

	def setCurrentNormalPlayerIndex(self, argsList):
		iIndex = int(argsList)
		self.m_iNormalPlayerCurrentIndexes [self.m_normalPlayerTabCtrl.getActiveTab()] = int(argsList)
		return 1

	def setCurrentNormalMapIndex(self, argsList):
		iIndex = int(argsList)
		self.m_iNormalMapCurrentIndexes [self.m_normalMapTabCtrl.getActiveTab()] = int(argsList)
		return 1

	def setCurrentNormalMapList(self, argsList):
		self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] = int(argsList)
		return 1

	def setCurrentAdvancedStartIndex(self, argsList):
		iIndex = int(argsList)
		self.m_iAdvancedStartCurrentIndexes [self.m_advancedStartTabCtrl.getActiveTab()] = int(argsList)
		return 1

	def setCurrentAdvancedStartList(self, argsList):
		self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] = int(argsList)
		return 1

	def setEditButtonClicked(self, argsList):
		iIndex = int(argsList)
		if (self.m_bUnitEditCtrl):
			# isGraphicalOnly index bugfix - Nightinggale - start
			# exe bug workaround
			# the exe calls with iIndex as button index rather than PromotionTypes
			# since isGraphicalOnly will not get buttons, passing those will create an offset
			# the correct PromotionTypes value is iIndex +1 for each isGraphicalOnly promotion prior to the promotion in question
			for i in (range(gc.getNumPromotionInfos())):
				if (gc.getPromotionInfo(i).isGraphicalOnly()):
					iIndex += 1
				elif iIndex == i:
					break
			# isGraphicalOnly index bugfix - Nightinggale - start
			bOn = not self.m_pActivePlot.getUnit(self.m_iCurrentUnit).isHasRealPromotion(iIndex)
			self.m_pActivePlot.getUnit(self.m_iCurrentUnit).setHasRealPromotion(iIndex, bOn)
		elif (self.m_bCityEditCtrl):
			self.m_pActivePlot.getPlotCity().setHasRealBuilding(iIndex, not self.m_pActivePlot.getPlotCity().isHasRealBuilding(iIndex))
		return 1

	def getUnitTabID(self):
		return self.m_iUnitTabID

	def getBuildingTabID(self):
		return self.m_iBuildingTabID
	def getImprovementTabID(self):
		return self.m_iImprovementTabID

	def getBonusTabID(self):
		return self.m_iBonusTabID

	def getImprovementListID(self):
		return self.m_iImprovementListID

	def getBonusListID(self):
		return self.m_iBonusListID

	def getTerrainTabID(self):
		return self.m_iTerrainTabID

	def getTerrainListID(self):
		return self.m_iTerrainListID

	def getFeatureListID(self):
		return self.m_iFeatureListID

	def getPlotTypeListID(self):
		return self.m_iPlotTypeListID

	def getRouteListID(self):
		return self.m_iRouteListID

	def getTerritoryTabID(self):
		return self.m_iTerritoryTabID

	def getTerritoryListID(self):
		return self.m_iTerritoryListID

	def getEuropeListID(self):
		return self.m_iEuropeListID

	def getASUnitTabID(self):
		return self.m_iASUnitTabID

	def getASUnitListID(self):
		return self.m_iASUnitListID

	def getASCityTabID(self):
		return self.m_iASCityTabID

	def getASCityListID(self):
		return self.m_iASCityListID

	def getASBuildingsListID(self):
		return self.m_iASBuildingsListID

	def getASAutomateListID(self):
		return self.m_iASAutomateListID

	def getASImprovementsTabID(self):
		return self.m_iASImprovementsTabID

	def getASRoutesListID(self):
		return self.m_iASRoutesListID

	def getASImprovementsListID(self):
		return self.m_iASImprovementsListID

	def getASVisibilityTabID(self):
		return self.m_iASVisibilityTabID

	def getASVisibilityListID(self):
		return self.m_iASVisibilityListID
	def highlightBrush(self):

		if (self.m_bShowBigBrush):
			if (self.m_pCurrentPlot == 0):
				return

			CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)
			CyEngine().fillAreaBorderPlotAlt(self.m_pCurrentPlot.getX(), self.m_pCurrentPlot.getY(), AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER, "COLOR_GREEN", 1)
			for i in range( (self.m_iBrushWidth-1) ):
				for j in range((self.m_iBrushHeight)):
					pPlot = CyMap().plot(self.m_pCurrentPlot.getX()-(i+1), self.m_pCurrentPlot.getY()-(j))
					if (not pPlot.isNone()):
						CyEngine().fillAreaBorderPlotAlt(pPlot.getX(), pPlot.getY(), AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER, "COLOR_GREEN", 1)
					pPlot = CyMap().plot(self.m_pCurrentPlot.getX(), self.m_pCurrentPlot.getY()-(j))
					if (not pPlot.isNone()):
						CyEngine().fillAreaBorderPlotAlt(pPlot.getX(), pPlot.getY(), AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER, "COLOR_GREEN", 1)
					pPlot = CyMap().plot(self.m_pCurrentPlot.getX()+(i+1), self.m_pCurrentPlot.getY()-(j))
					if (not pPlot.isNone()):
						CyEngine().fillAreaBorderPlotAlt(pPlot.getX(), pPlot.getY(), AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER, "COLOR_GREEN", 1)
					pPlot = CyMap().plot(self.m_pCurrentPlot.getX()-(i+1), self.m_pCurrentPlot.getY()+(j))
					if (not pPlot.isNone()):
						CyEngine().fillAreaBorderPlotAlt(pPlot.getX(), pPlot.getY(), AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER, "COLOR_GREEN", 1)
					pPlot = CyMap().plot(self.m_pCurrentPlot.getX(), self.m_pCurrentPlot.getY()+(j))
					if (not pPlot.isNone()):
						CyEngine().fillAreaBorderPlotAlt(pPlot.getX(), pPlot.getY(), AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER, "COLOR_GREEN", 1)
					pPlot = CyMap().plot(self.m_pCurrentPlot.getX()+(i+1), self.m_pCurrentPlot.getY()+(j))
					if (not pPlot.isNone()):
						CyEngine().fillAreaBorderPlotAlt(pPlot.getX(), pPlot.getY(), AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER, "COLOR_GREEN", 1)
			if (not self.m_iBrushWidth):
				pPlot = CyMap().plot(self.m_pCurrentPlot.getX(), self.m_pCurrentPlot.getY())
				if (not pPlot.isNone()):
					CyEngine().fillAreaBorderPlotAlt(pPlot.getX(), pPlot.getY(), AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER, "COLOR_GREEN", 1)
				for j in range((self.m_iBrushHeight)):
					pPlot = CyMap().plot(self.m_pCurrentPlot.getX(), self.m_pCurrentPlot.getY()-(j))
					if (not pPlot.isNone()):
						CyEngine().fillAreaBorderPlotAlt(pPlot.getX(), pPlot.getY(), AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER, "COLOR_GREEN", 1)
					pPlot = CyMap().plot(self.m_pCurrentPlot.getX(), self.m_pCurrentPlot.getY()-(j))
					if (not pPlot.isNone()):
						CyEngine().fillAreaBorderPlotAlt(pPlot.getX(), pPlot.getY(), AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER, "COLOR_GREEN", 1)

		return

	def placeMultipleObjects(self):
		bInsideForLoop = False
		permCurrentPlot = self.m_pCurrentPlot
		for i in range( (self.m_iBrushWidth-1) ):
			for j in range((self.m_iBrushHeight)):
				bInsideForLoop = True
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX()-(i+1), permCurrentPlot.getY()-(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.placeObject()
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX(), permCurrentPlot.getY()-(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.placeObject()
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX()+(i+1), permCurrentPlot.getY()-(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.placeObject()
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX()-(i+1), permCurrentPlot.getY()+(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.placeObject()
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX(), permCurrentPlot.getY()+(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.placeObject()
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX()+(i+1), permCurrentPlot.getY()+(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.placeObject()
		if (not bInsideForLoop):
			self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX(), permCurrentPlot.getY())
			if (not self.m_pCurrentPlot.isNone()):
				self.placeObject()
			for j in range((self.m_iBrushHeight)):
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX(), permCurrentPlot.getY()-(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.placeObject()
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX(), permCurrentPlot.getY()-(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.placeObject()
		self.m_pCurrentPlot = permCurrentPlot
		return

	def removeMultipleObjects(self):
		bInsideForLoop = False
		permCurrentPlot = self.m_pCurrentPlot
		for i in range( (self.m_iBrushWidth-1) ):
			for j in range((self.m_iBrushHeight)):
				bInsideForLoop = True
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX()-(i+1), permCurrentPlot.getY()-(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.removeObject()
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX(), permCurrentPlot.getY()-(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.removeObject()
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX()+(i+1), permCurrentPlot.getY()-(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.removeObject()
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX()-(i+1), permCurrentPlot.getY()+(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.removeObject()
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX(), permCurrentPlot.getY()+(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.removeObject()
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX()+(i+1), permCurrentPlot.getY()+(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.removeObject()
		if (not bInsideForLoop):
			self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX(), permCurrentPlot.getY())
			if (not self.m_pCurrentPlot.isNone()):
				self.removeObject()
			for j in range((self.m_iBrushHeight)):
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX(), permCurrentPlot.getY()-(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.removeObject()
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX(), permCurrentPlot.getY()-(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.removeObject()
		self.m_pCurrentPlot = permCurrentPlot
		return

	def showMultipleReveal(self):
		print "showMultipleReveal"
		self.refreshReveal()
		return

	def setMultipleReveal(self, bReveal):
		print "setMultipleReveal"
		bInsideForLoop = False
		permCurrentPlot = self.m_pCurrentPlot
		for i in range( (self.m_iBrushWidth-1) ):
			for j in range((self.m_iBrushHeight)):
				bInsideForLoop = True
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX()-(i+1), permCurrentPlot.getY()-(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.RevealCurrentPlot(bReveal)
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX(), permCurrentPlot.getY()-(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.RevealCurrentPlot(bReveal)
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX()+(i+1), permCurrentPlot.getY()-(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.RevealCurrentPlot(bReveal)
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX()-(i+1), permCurrentPlot.getY()+(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.RevealCurrentPlot(bReveal)
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX(), permCurrentPlot.getY()+(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.RevealCurrentPlot(bReveal)
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX()+(i+1), permCurrentPlot.getY()+(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.RevealCurrentPlot(bReveal)
		if (not bInsideForLoop):
			self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX(), permCurrentPlot.getY())
			if (not self.m_pCurrentPlot.isNone()):
				self.RevealCurrentPlot(bReveal)
			for j in range((self.m_iBrushHeight)):
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX(), permCurrentPlot.getY()-(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.RevealCurrentPlot(bReveal)
				self.m_pCurrentPlot = CyMap().plot(permCurrentPlot.getX(), permCurrentPlot.getY()-(j))
				if (not self.m_pCurrentPlot.isNone()):
					self.RevealCurrentPlot(bReveal)
		self.m_pCurrentPlot = permCurrentPlot
		self.showMultipleReveal()
		return

	def useLargeBrush(self):
		if 	(
				(
					(self.m_bNormalMap) and
					(not self.m_bUnitEdit) and
					(not self.m_bCityEdit)
				)
				and
				(
					(
						(self.m_normalMapTabCtrl.getActiveTab() == self.m_iTerrainTabID) and
						(
							(self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iTerrainListID) or
							(self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iFeatureListID) or
							(self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iPlotTypeListID)
						)
					)
					or
					(
						(self.m_normalMapTabCtrl.getActiveTab() == self.m_iBonusTabID)
					)
					or
					(
						(self.m_normalMapTabCtrl.getActiveTab() == self.m_iTerritoryTabID)
					)
				)
			):
			return True
		elif (self.m_bReveal):
			return True
		else:
			return False

	def clearSideMenu(self):
		screen = CyGInterfaceScreen( "WorldBuilderScreen", CvScreenEnums.WORLDBUILDER_SCREEN )
		screen.deleteWidget("WorldBuilderMainPanel")
		screen.deleteWidget("WorldBuilderBackgroundPanel")

		screen.deleteWidget("WorldBuilderSaveButton")
		screen.deleteWidget("WorldBuilderLoadButton")
		screen.deleteWidget("WorldBuilderAllPlotsButton")
		screen.deleteWidget("WorldBuilderExitButton")

		screen.deleteWidget("WorldBuilderUnitEditMode")
		screen.deleteWidget("WorldBuilderCityEditMode")

		screen.deleteWidget("WorldBuilderNormalPlayerMode")
		screen.deleteWidget("WorldBuilderNormalMapMode")
		screen.deleteWidget("WorldBuilderRevealMode")

		screen.deleteWidget("WorldBuilderPlayerChoice")
		screen.deleteWidget("WorldBuilderTechByEra")
		screen.deleteWidget("WorldBuilderBrushSize")
		screen.deleteWidget("WorldBuilderRegenerateMap")
		screen.deleteWidget("WorldBuilderTeamChoice")

		screen.deleteWidget("WorldBuilderRevealAll")
		screen.deleteWidget("WorldBuilderUnrevealAll")
		screen.deleteWidget("WorldBuilderRevealPanel")

		screen.deleteWidget("WorldBuilderBackgroundBottomPanel")
		return

	def setSideMenu(self):
		screen = CyGInterfaceScreen( "WorldBuilderScreen", CvScreenEnums.WORLDBUILDER_SCREEN )

		iMaxScreenWidth = screen.getXResolution()
		iMaxScreenHeight = screen.getYResolution()
		iScreenHeight = 10+37+37

		iButtonWidth = 32
		iButtonHeight = 32
		iButtonX = 0
		iButtonY = 0

		if (CyInterface().isInAdvancedStart()):
			iX = 0
		else:
			iX = iMaxScreenWidth-self.iScreenWidth
		screen.addPanel( "WorldBuilderBackgroundPanel", "", "", True, True, iX, 0, self.iScreenWidth, iScreenHeight, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addScrollPanel( "WorldBuilderMainPanel", "", iX, 0, self.iScreenWidth, iScreenHeight, PanelStyles.PANEL_STYLE_MAIN, false, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		if (CyInterface().isInAdvancedStart()):

			iX = 50
			iY = 15
			szText = u"<font=4>" + localText.getText("TXT_KEY_WB_AS_POINTS", (gc.getPlayer(CyGame().getActivePlayer()).getAdvancedStartPoints(), )) + "</font>"
			screen.setLabel("AdvancedStartPointsText", "Background", szText, CvUtil.FONT_LEFT_JUSTIFY, iX, iY, -2, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

			iY += 30
			szText = localText.getText("TXT_KEY_ADVANCED_START_BEGIN_GAME", ())
			screen.setButtonGFC( "WorldBuilderExitButton", szText, "", iX, iY, 130, 28, WidgetTypes.WIDGET_WB_EXIT_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_STANDARD )

			szText = u"<font=4>" + localText.getText("TXT_KEY_WB_AS_COST_THIS_LOCATION", (self.m_iCost, )) + u"</font>"
			iY = 85
			screen.setLabel("AdvancedStartCostText", "Background", szText, CvUtil.FONT_LEFT_JUSTIFY, iX-20, iY, -2, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		else:

			iPanelWidth = 35*6
			screen.attachPanelAt(
				"WorldBuilderMainPanel",
				"WorldBuilderLoadSavePanel",
				"",
				"",
				False,
				True,
				PanelStyles.PANEL_STYLE_CITY_TANSHADE,
				70,
				0,
				iPanelWidth-70,
				35,
				WidgetTypes.WIDGET_GENERAL, -1, -1)

			screen.setImageButtonAt( "WorldBuilderAllPlotsButton", "WorldBuilderLoadSavePanel", ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_CHANGE_ALL_PLOTS").getPath(), iButtonX, iButtonY, iButtonWidth, iButtonHeight, WidgetTypes.WIDGET_WB_ALL_PLOTS_BUTTON, -1, -1)
			iButtonX = iButtonX + 35
			screen.setImageButtonAt( "WorldBuilderSaveButton", "WorldBuilderLoadSavePanel", ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_SAVE").getPath(), iButtonX, iButtonY, iButtonWidth, iButtonHeight, WidgetTypes.WIDGET_WB_SAVE_BUTTON, -1, -1)
			iButtonX = iButtonX + 35
			screen.setImageButtonAt( "WorldBuilderLoadButton", "WorldBuilderLoadSavePanel", ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_LOAD").getPath(), iButtonX, iButtonY, iButtonWidth, iButtonHeight, WidgetTypes.WIDGET_WB_LOAD_BUTTON, -1, -1)
			iButtonX = iButtonX + 35
			screen.setImageButtonAt( "WorldBuilderExitButton", "WorldBuilderLoadSavePanel", ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_EXIT").getPath(), iButtonX, iButtonY, iButtonWidth, iButtonHeight, WidgetTypes.WIDGET_WB_EXIT_BUTTON, -1, -1)

			iButtonWidth = 32
			iButtonHeight = 32
			iButtonX = 0
			iButtonY = 0
			self.m_iUnitEditCheckboxID = 0
			screen.addCheckBoxGFC(
				"WorldBuilderUnitEditModeButton",
				ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_TOGGLE_UNIT_EDIT_MODE").getPath(),
				ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				(iMaxScreenWidth-self.iScreenWidth)+8+iButtonX,
				(10+36),
				iButtonWidth,
				iButtonHeight,
				WidgetTypes.WIDGET_WB_UNIT_EDIT_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)

			iButtonX = iButtonX + 35
			self.m_iCityEditCheckboxID = 1
			screen.addCheckBoxGFC(
				"WorldBuilderCityEditModeButton",
				ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_TOGGLE_CITY_EDIT_MODE").getPath(),
				ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				(iMaxScreenWidth-self.iScreenWidth)+8+iButtonX,
				(10+36),
				iButtonWidth,
				iButtonHeight,
				WidgetTypes.WIDGET_WB_CITY_EDIT_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)

			iButtonX = iButtonX + 35
			self.m_iNormalPlayerCheckboxID = 2
			screen.addCheckBoxGFC(
				"WorldBuilderNormalPlayerModeButton",
				ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_NORMAL_UNIT_MODE").getPath(),
				ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				(iMaxScreenWidth-self.iScreenWidth)+8+iButtonX,
				(10+36),
				iButtonWidth,
				iButtonHeight,
				WidgetTypes.WIDGET_WB_NORMAL_PLAYER_TAB_MODE_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)

			iButtonX = iButtonX + 35
			self.m_iNormalMapCheckboxID = 3
			screen.addCheckBoxGFC(
				"WorldBuilderNormalMapModeButton",
				ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_NORMAL_MAP_MODE").getPath(),
				ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				(iMaxScreenWidth-self.iScreenWidth)+8+iButtonX,
				(10+36),
				iButtonWidth,
				iButtonHeight,
				WidgetTypes.WIDGET_WB_NORMAL_MAP_TAB_MODE_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)

			iButtonX = iButtonX + 35
			self.m_iRevealTileCheckboxID = 4
			screen.addCheckBoxGFC(
				"WorldBuilderRevealTileModeButton",
				ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_REVEAL_TILE_MODE").getPath(),
				ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				(iMaxScreenWidth-self.iScreenWidth)+8+iButtonX,
				(10+36),
				iButtonWidth,
				iButtonHeight,
				WidgetTypes.WIDGET_WB_REVEAL_TAB_MODE_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)

			iButtonX = iButtonX + 35
			self.m_iDiplomacyCheckboxID = 5
			screen.addCheckBoxGFC(
				"WorldBuilderDiplomacyModeButton",
				ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_DIPLOMACY_MODE").getPath(),
				ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				(iMaxScreenWidth-self.iScreenWidth)+8+iButtonX,
				(10+36),
				iButtonWidth,
				iButtonHeight,
				WidgetTypes.WIDGET_WB_DIPLOMACY_MODE_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)

			iButtonX = 0
			self.m_iLandmarkCheckboxID = 6
			screen.addCheckBoxGFC(
				"WorldBuilderLandmarkButton",
				ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_LANDMARK_MODE").getPath(),
				ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				(iMaxScreenWidth-self.iScreenWidth)+8+iButtonX,
				(10),
				iButtonWidth,
				iButtonHeight,
				WidgetTypes.WIDGET_WB_LANDMARK_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)

			iButtonX = iButtonX + 35
			self.m_iEraseCheckboxID = 7
			screen.addCheckBoxGFC(
				"WorldBuilderEraseButton",
				ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_ERASE").getPath(),
				ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				(iMaxScreenWidth-self.iScreenWidth)+8+iButtonX,
				(10),
				iButtonWidth,
				iButtonHeight,
				WidgetTypes.WIDGET_WB_ERASE_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)

			self.setCurrentModeCheckbox(self.m_iNormalPlayerCheckboxID)

		return

	def refreshSideMenu(self):
		screen = CyGInterfaceScreen( "WorldBuilderScreen", CvScreenEnums.WORLDBUILDER_SCREEN )

		iMaxScreenWidth = screen.getXResolution()
		iMaxScreenHeight = screen.getYResolution()
		iScreenHeight = 10+37+37

		if (CyInterface().isInAdvancedStart()):

			iX = 50
			iY = 15
			szText = u"<font=4>" + localText.getText("TXT_KEY_WB_AS_POINTS", (gc.getPlayer(CyGame().getActivePlayer()).getAdvancedStartPoints(), )) + "</font>"
			screen.setLabel("AdvancedStartPointsText", "Background", szText, CvUtil.FONT_LEFT_JUSTIFY, iX, iY, -2, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

			szText = u"<font=4>" + localText.getText("TXT_KEY_WB_AS_COST_THIS_LOCATION", (self.m_iCost, )) + u"</font>"
			iY = 85
			screen.setLabel("AdvancedStartCostText", "Background", szText, CvUtil.FONT_LEFT_JUSTIFY, iX-20, iY, -2, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		else:

			screen.deleteWidget("WorldBuilderPlayerChoice")
			screen.deleteWidget("WorldBuilderTechByEra")
			screen.deleteWidget("WorldBuilderBrushSize")
			screen.deleteWidget("WorldBuilderRegenerateMap")
			screen.deleteWidget("WorldBuilderTeamChoice")

			screen.deleteWidget("WorldBuilderRevealAll")
			screen.deleteWidget("WorldBuilderUnrevealAll")
			screen.deleteWidget("WorldBuilderRevealPanel")
			screen.deleteWidget("WorldBuilderBackgroundBottomPanel")

			iPanelWidth = 35*6

			if(self.m_bReveal or (self.m_bNormalPlayer and (not self.m_bUnitEdit) and (not self.m_bCityEdit))):
				screen.addPanel( "WorldBuilderBackgroundBottomPanel", "", "", True, True, iMaxScreenWidth-self.iScreenWidth, 10+32+32, self.iScreenWidth, 45+40, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
			else:
				screen.addPanel( "WorldBuilderBackgroundBottomPanel", "", "", True, True, iMaxScreenWidth-self.iScreenWidth, 10+32+32, self.iScreenWidth, 45, PanelStyles.PANEL_STYLE_MAIN, WidgetTypes.WIDGET_GENERAL, -1, -1 )

			if (self.m_bNormalPlayer and (not self.m_bUnitEdit) and (not self.m_bCityEdit)):
				szDropdownName = str("WorldBuilderPlayerChoice")
				screen.addDropDownBoxGFC(szDropdownName, (iMaxScreenWidth-self.iScreenWidth)+8, (10+36+36), iPanelWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
				for i in range( gc.getMAX_CIV_PLAYERS() ):
					if ( gc.getPlayer(i).isEverAlive() ):
						screen.addPullDownString(szDropdownName, gc.getPlayer(i).getName(), i, i, i == self.m_iCurrentPlayer )

			elif(self.m_bNormalMap and (not self.m_bUnitEdit) and (not self.m_bCityEdit)):
				iButtonWidth = 32
				iButtonHeight = 32
				iButtonX = 0
				iButtonY = 0
				screen.setImageButton( "WorldBuilderRegenerateMap", ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_REVEAL_ALL_TILES").getPath(), (iMaxScreenWidth-self.iScreenWidth)+8+iButtonX, (10+36+36), iButtonWidth, iButtonHeight, WidgetTypes.WIDGET_WB_REGENERATE_MAP, -1, -1)

				szDropdownName = str("WorldBuilderBrushSize")
				screen.addDropDownBoxGFC(szDropdownName, (iMaxScreenWidth-self.iScreenWidth)+48, (10+36+36), iPanelWidth-40, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
				bActive = False
				if (self.m_iBrushWidth == 1):
					bActive = True
				else:
					bActive = False
				screen.addPullDownString(szDropdownName, localText.getText("TXT_KEY_WB_1_BY_1",()), 1, 1, bActive )
				if (self.m_iBrushWidth == 2):
					bActive = True
				else:
					bActive = False
				screen.addPullDownString(szDropdownName, localText.getText("TXT_KEY_WB_3_BY_3",()), 2, 2, bActive )
				if (self.m_iBrushWidth == 3):
					bActive = True
				else:
					bActive = False
				screen.addPullDownString(szDropdownName, localText.getText("TXT_KEY_WB_5_BY_5",()), 3, 3, bActive )

			elif(self.m_bReveal):
				iPanelWidth = 35*6
				iButtonWidth = 32
				iButtonHeight = 32
				iButtonX = 0
				iButtonY = 0
				screen.setImageButton( "WorldBuilderRevealAll", ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_REVEAL_ALL_TILES").getPath(), (iMaxScreenWidth-self.iScreenWidth)+8+iButtonX, (10+36+36), iButtonWidth, iButtonHeight, WidgetTypes.WIDGET_WB_REVEAL_ALL_BUTTON, -1, -1)
				iButtonX = iButtonX + 35
				screen.setImageButton( "WorldBuilderUnrevealAll", ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_UNREVEAL_ALL_TILES").getPath(), (iMaxScreenWidth-self.iScreenWidth)+8+iButtonX, (10+36+36), iButtonWidth, iButtonHeight, WidgetTypes.WIDGET_WB_UNREVEAL_ALL_BUTTON, -1, -1)
				iButtonX = iButtonX + 35

				szDropdownName = str("WorldBuilderBrushSize")
				screen.addDropDownBoxGFC(szDropdownName, (iMaxScreenWidth-self.iScreenWidth)+8+80, (10+36+36), iPanelWidth-80, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
				bActive = False
				if (self.m_iBrushWidth == 1):
					bActive = True
				else:
					bActive = False
				screen.addPullDownString(szDropdownName, localText.getText("TXT_KEY_WB_1_BY_1",()), 1, 1, bActive )
				if (self.m_iBrushWidth == 2):
					bActive = True
				else:
					bActive = False
				screen.addPullDownString(szDropdownName, localText.getText("TXT_KEY_WB_3_BY_3",()), 2, 2, bActive )
				if (self.m_iBrushWidth == 3):
					bActive = True
				else:
					bActive = False
				screen.addPullDownString(szDropdownName, localText.getText("TXT_KEY_WB_5_BY_5",()), 3, 3, bActive )

				szDropdownName = str("WorldBuilderTeamChoice")
				screen.addDropDownBoxGFC(szDropdownName, (iMaxScreenWidth-self.iScreenWidth)+8, (10+36+36+36), iPanelWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
				for i in range( gc.getMAX_CIV_TEAMS() ):
					if (gc.getTeam(i).isEverAlive()):
						if (i == self.m_iCurrentTeam):
							screen.addPullDownString(szDropdownName, gc.getTeam(i).getName(), i, i, True )
						else:
							screen.addPullDownString(szDropdownName, gc.getTeam(i).getName(), i, i, False )

			else:
				screen.deleteWidget("WorldBuilderBackgroundBottomPanel")

		return

	def revealAll(self, bReveal):
		for i in range (CyMap().getGridWidth()):
			for j in range (CyMap().getGridHeight()):
				pPlot = CyMap().plot(i,j)
				if (not pPlot.isNone()):
					if bReveal or (not pPlot.isVisible(self.m_iCurrentTeam, False)):
						pPlot.setRevealed(self.m_iCurrentTeam, bReveal, False, -1);
		self.refreshReveal()
		return

	def RevealCurrentPlot(self, bReveal):
		if bReveal or (not self.m_pCurrentPlot.isVisible(self.m_iCurrentTeam, False)):
			self.m_pCurrentPlot.setRevealed(self.m_iCurrentTeam, bReveal, False, -1)
		return

	def showRevealed(self, pPlot):
		if (not pPlot.isRevealed(self.m_iCurrentTeam, False)):
			CyEngine().fillAreaBorderPlotAlt(pPlot.getX(), pPlot.getY(), AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS, "COLOR_BLACK", 1.0)
		return

	def getNumPlayers(self):
		iCount = 0
		for i in range( gc.getMAX_CIV_PLAYERS() ):
			if ( gc.getPlayer(i).isEverAlive() ):
				iCount = iCount + 1

		return iCount

	def Exit(self):
		CyInterface().setWorldBuilder(false)
		return

	def setLandmarkCB(self, szLandmark):
		self.m_pCurrentPlot = CyInterface().getMouseOverPlot()
		CyEngine().addLandmarkPopup(self.m_pCurrentPlot) # , u"%s" %(szLandmark))
		return

	def removeLandmarkCB(self):
		self.m_pCurrentPlot = CyInterface().getMouseOverPlot()
		CyEngine().removeLandmark(self.m_pCurrentPlot)
		return

	def refreshPlayerTabCtrl(self):

		initWBToolPlayerControl()

		self.m_normalPlayerTabCtrl = getWBToolNormalPlayerTabCtrl()

		self.m_normalPlayerTabCtrl.setNumColumns((gc.getNumUnitInfos()/10)+2);
		self.m_normalPlayerTabCtrl.addTabSection(localText.getText("TXT_KEY_WB_UNITS",()));
		self.m_iUnitTabID = 0
		self.m_iNormalPlayerCurrentIndexes.append(0)

		self.m_normalPlayerTabCtrl.setNumColumns((gc.getNumBuildingInfos()/10)+1);
		self.m_normalPlayerTabCtrl.addTabSection(localText.getText("TXT_KEY_WB_BUILDINGS",()));
		self.m_iBuildingTabID = 1
		self.m_iNormalPlayerCurrentIndexes.append(0)
		# ugly hack: disable the NumUnitInfos deception for the exe for this function only
		# so WB will only display the real units, not the placeholders
		gc.setExeXmlLengthOverride(false);
		addWBPlayerControlTabs()
		gc.setExeXmlLengthOverride(true);
		return

	def refreshAdvancedStartTabCtrl(self, bReuse):

		if (CyInterface().isInAdvancedStart()):

			if (self.m_advancedStartTabCtrl and bReuse):
				iActiveTab = self.m_advancedStartTabCtrl.getActiveTab()
				iActiveList = self.m_iAdvancedStartCurrentList[iActiveTab]
				iActiveIndex = self.m_iAdvancedStartCurrentIndexes[iActiveTab]
			else:
				iActiveTab = 0
				iActiveList = 0
				iActiveIndex = 0

			self.m_iCurrentPlayer = CyGame().getActivePlayer()
			self.m_iCurrentTeam = CyGame().getActiveTeam()
			self.m_iAdvancedStartCurrentIndexes = []
			self.m_iAdvancedStartCurrentList = []

			initWBToolAdvancedStartControl()

			self.m_advancedStartTabCtrl = getWBToolAdvancedStartTabCtrl()

			self.m_advancedStartTabCtrl.setNumColumns((gc.getNumBuildingInfos()/10)+2);
			self.m_advancedStartTabCtrl.addTabSection(localText.getText("TXT_KEY_WB_AS_CITIES",()));
			self.m_iASCityTabID = 0
			self.m_iAdvancedStartCurrentIndexes.append(0)

			self.m_iASCityListID = 0
			self.m_iASBuildingsListID = 2
			self.m_iASAutomateListID = 1
			self.m_iAdvancedStartCurrentList.append(self.m_iASCityListID)

			self.m_advancedStartTabCtrl.setNumColumns((gc.getNumUnitInfos()/10)+2);
			self.m_advancedStartTabCtrl.addTabSection(localText.getText("TXT_KEY_WB_AS_UNITS",()));
			self.m_iASUnitTabID = 1
			self.m_iAdvancedStartCurrentIndexes.append(0)

			self.m_iAdvancedStartCurrentList.append(0)
			self.m_iASUnitListID = 0

			self.m_advancedStartTabCtrl.setNumColumns((gc.getNumImprovementInfos()/10)+2);
			self.m_advancedStartTabCtrl.addTabSection(localText.getText("TXT_KEY_WB_AS_IMPROVEMENTS",()));
			self.m_iASImprovementsTabID = 2
			self.m_iAdvancedStartCurrentIndexes.append(0)

			self.m_iASRoutesListID = 0
			self.m_iASImprovementsListID = 1
			self.m_iAdvancedStartCurrentList.append(self.m_iASRoutesListID)

			self.m_advancedStartTabCtrl.setNumColumns(1);
			self.m_advancedStartTabCtrl.addTabSection(localText.getText("TXT_KEY_WB_AS_VISIBILITY",()));
			self.m_iASVisibilityTabID = 3
			self.m_iAdvancedStartCurrentIndexes.append(0)

			self.m_iAdvancedStartCurrentList.append(0)
			self.m_iASVisibilityListID = 0
			addWBAdvancedStartControlTabs()

			self.m_advancedStartTabCtrl.setActiveTab(iActiveTab)
			self.setCurrentAdvancedStartIndex(iActiveIndex)
			self.setCurrentAdvancedStartList(iActiveList)
		else:

			self.m_advancedStartTabCtrl = getWBToolAdvancedStartTabCtrl()

			self.m_advancedStartTabCtrl.enable(false)

		return

	def eraseAll(self):
		# kill all units on plot if one is selected
		if (self.m_pCurrentPlot != 0):
			while (self.m_pCurrentPlot.getNumUnits() > 0):
				pUnit = self.m_pCurrentPlot.getUnit(0)
				pUnit.kill(false)

			self.m_pCurrentPlot.setBonusType(-1)
			self.m_pCurrentPlot.setFeatureType(FeatureTypes.NO_FEATURE, -1)

			if (self.m_pCurrentPlot.isCity()):
				self.m_pCurrentPlot.getPlotCity().kill()

			self.m_pCurrentPlot.setRouteType(-1)
			self.m_pCurrentPlot.setNOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
			self.m_pCurrentPlot.setWOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
			self.m_pCurrentPlot.setImprovementType(-1)
			self.removeLandmarkCB()
		return

	def getUnitScript(self):
		self.m_pUnitToScript = self.m_pActivePlot.getUnit(self.m_iCurrentUnit)
		self.getScript()
		return

	def getCityScript(self):
		self.m_pCityToScript = self.m_pActivePlot.getPlotCity()
		self.getScript()
		return

	def getScript(self):
		CvEventInterface.beginEvent(CvUtil.EventWBScriptPopup)
		return

	def getNewStartYear(self):
		CvEventInterface.beginEvent(CvUtil.EventWBStartYearPopup)
		return

	def setScriptCB(self, szScript):
		if (self.m_pUnitToScript != -1):
			self.m_pUnitToScript.setScriptData(CvUtil.convertToStr(szScript))
			self.m_pUnitToScript = -1
			return

		if (self.m_pCityToScript != -1):
			self.m_pCityToScript.setScriptData(CvUtil.convertToStr(szScript))
			self.m_pCityToScript = -1
			return

		if (self.m_pPlotToScript != -1):
			self.m_pPlotToScript.setScriptData(CvUtil.convertToStr(szScript))
			self.m_pPlotToScript = -1
			return
		return

	def setStartYearCB(self, iStartYear):
		gc.getGame().setStartYear(iStartYear)
		return

	def getCurrentScript(self):
		if (self.m_pUnitToScript != -1):
			return self.m_pUnitToScript.getScriptData()

		if (self.m_pCityToScript != -1):
			return self.m_pCityToScript.getScriptData()

		if (self.m_pPlotToScript != -1):
			return self.m_pPlotToScript.getScriptData()

		return ""

	def setRiverHighlights(self):
		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().addColoredPlotAlt(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY(), PlotStyles.PLOT_STYLE_RIVER_SOUTH, PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS, "COLOR_GREEN", 1)

		fAlpha = .2
		CyEngine().addColoredPlotAlt(self.m_pRiverStartPlot.getX()-1, self.m_pRiverStartPlot.getY()+1, PlotStyles.PLOT_STYLE_BOX_FILL, PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS, "COLOR_WHITE", fAlpha)
		CyEngine().addColoredPlotAlt(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY()+1, PlotStyles.PLOT_STYLE_BOX_FILL, PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS, "COLOR_WHITE", fAlpha)
		CyEngine().addColoredPlotAlt(self.m_pRiverStartPlot.getX()+1, self.m_pRiverStartPlot.getY()+1, PlotStyles.PLOT_STYLE_BOX_FILL, PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS, "COLOR_WHITE", fAlpha)
		CyEngine().addColoredPlotAlt(self.m_pRiverStartPlot.getX()-1, self.m_pRiverStartPlot.getY(), PlotStyles.PLOT_STYLE_BOX_FILL, PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS, "COLOR_WHITE", fAlpha)

		CyEngine().addColoredPlotAlt(self.m_pRiverStartPlot.getX()+1, self.m_pRiverStartPlot.getY(), PlotStyles.PLOT_STYLE_BOX_FILL, PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS, "COLOR_WHITE", fAlpha)
		CyEngine().addColoredPlotAlt(self.m_pRiverStartPlot.getX()-1, self.m_pRiverStartPlot.getY()-1, PlotStyles.PLOT_STYLE_BOX_FILL, PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS, "COLOR_WHITE", fAlpha)
		CyEngine().addColoredPlotAlt(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY()-1, PlotStyles.PLOT_STYLE_BOX_FILL, PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS, "COLOR_WHITE", fAlpha)
		CyEngine().addColoredPlotAlt(self.m_pRiverStartPlot.getX()+1, self.m_pRiverStartPlot.getY()-1, PlotStyles.PLOT_STYLE_BOX_FILL, PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS, "COLOR_WHITE", fAlpha)
		return

	def setCurrentModeCheckbox(self, iButton):
		screen = CyGInterfaceScreen( "WorldBuilderScreen", CvScreenEnums.WORLDBUILDER_SCREEN )
		#print("iButton: %s" %(str(iButton)))

		#print("m_iUnitEditCheckboxID: %s" %(str(self.m_iUnitEditCheckboxID)))
		#print("m_iCityEditCheckboxID: %s" %(str(self.m_iCityEditCheckboxID)))
		#print("m_iNormalPlayerCheckboxID: %s" %(str(self.m_iNormalPlayerCheckboxID)))
		#print("m_iNormalMapCheckboxID: %s" %(str(self.m_iNormalMapCheckboxID)))
		#print("m_iRevealTileCheckboxID: %s" %(str(self.m_iRevealTileCheckboxID)))
		#print("m_iDiplomacyCheckboxID: %s" %(str(self.m_iDiplomacyCheckboxID)))

		if (iButton == self.m_iUnitEditCheckboxID):
			screen.setState("WorldBuilderUnitEditModeButton", True)
		else:
			screen.setState("WorldBuilderUnitEditModeButton", False)

		if (iButton == self.m_iCityEditCheckboxID):
			screen.setState("WorldBuilderCityEditModeButton", True)
		else:
			screen.setState("WorldBuilderCityEditModeButton", False)

		if (iButton == self.m_iNormalPlayerCheckboxID):
			screen.setState("WorldBuilderNormalPlayerModeButton", True)
		else:
			screen.setState("WorldBuilderNormalPlayerModeButton", False)

		if (iButton == self.m_iNormalMapCheckboxID):
			screen.setState("WorldBuilderNormalMapModeButton", True)
		else:
			screen.setState("WorldBuilderNormalMapModeButton", False)

		if (iButton == self.m_iRevealTileCheckboxID):
			screen.setState("WorldBuilderRevealTileModeButton", True)
		else:
			screen.setState("WorldBuilderRevealTileModeButton", False)

		if (iButton == self.m_iDiplomacyCheckboxID):
			screen.setState("WorldBuilderDiplomacyModeButton", True)
		else:
			screen.setState("WorldBuilderDiplomacyModeButton", False)

		if (iButton == self.m_iLandmarkCheckboxID):
			screen.setState("WorldBuilderLandmarkButton", True)
		else:
			screen.setState("WorldBuilderLandmarkButton", False)

		if (iButton == self.m_iEraseCheckboxID):
			screen.setState("WorldBuilderEraseButton", True)
		else:
			screen.setState("WorldBuilderEraseButton", False)

		return

	def initVars(self):
		self.m_normalPlayerTabCtrl = 0
		self.m_normalMapTabCtrl = 0
		self.m_tabCtrlEdit = 0
		self.m_flyoutMenu = 0
		self.m_bCtrlEditUp = False
		self.m_bUnitEdit = False
		self.m_bCityEdit = False
		self.m_bNormalPlayer = True
		self.m_bNormalMap = False
		self.m_bReveal = False
		self.m_bDiplomacy = False
		self.m_bLandmark = False
		self.m_bEraseAll = False
		self.m_bUnitEditCtrl = False
		self.m_bCityEditCtrl = False
		self.m_bShowBigBrush = False
		self.m_bLeftMouseDown = False
		self.m_bRightMouseDown = False
		self.m_bChangeFocus = False
		self.m_iNormalPlayerCurrentIndexes = []
		self.m_iNormalMapCurrentIndexes = []
		self.m_iNormalMapCurrentList = []
		self.m_iCurrentPlayer = 0
		self.m_iCurrentTeam = 0
		self.m_iCurrentUnitPlayer = 0
		self.m_iCurrentUnit = 0
		self.m_iCurrentX = -1
		self.m_iCurrentY = -1
		self.m_pCurrentPlot = 0
		self.m_pActivePlot = 0
		self.m_pRiverStartPlot = -1
		self.m_iUnitTabID = -1
		self.m_iBuildingTabID = -1
		self.m_iImprovementTabID = -1
		self.m_iBonusTabID = -1
		self.m_iImprovementListID = -1
		self.m_iBonusListID = -1
		self.m_iTerrainTabID = -1
		self.m_iTerrainListID = -1
		self.m_iFeatureListID = -1
		self.m_iPlotTypeListID = -1
		self.m_iRouteListID = -1
		self.m_iTerritoryTabID = -1
		self.m_iTerritoryListID = -1
		self.m_iEuropeListID = -1
		self.m_iBrushSizeTabID = -1
		self.m_iBrushWidth = 1
		self.m_iBrushHeight = 1
		self.m_iFlyoutEditUnit = 1
		self.m_iFlyoutEditCity = 0
		self.m_iFlyoutAddScript = -1
		self.m_iFlyoutChangeStartYear = -2
		self.m_pFlyoutPlot = 0
		self.m_bFlyout = False
		self.m_pUnitToScript = -1
		self.m_pCityToScript = -1
		self.m_pPlotToScript = -1
		self.m_iUnitEditCheckboxID = -1
		self.m_iCityEditCheckboxID = -1
		self.m_iNormalPlayerCheckboxID = -1
		self.m_iNormalMapCheckboxID = -1
		self.m_iRevealTileCheckboxID = -1
		self.m_iDiplomacyCheckboxID = -1
		self.m_iLandmarkCheckboxID = -1
		self.m_iEraseCheckboxID = -1
		self.iScreenWidth = 228
		return
