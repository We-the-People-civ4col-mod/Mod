## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
#
# CIV Python Tools Code
#
import CvUtil
import CvScreenEnums
from CvPythonExtensions import *

# globals
gc = CyGlobalContext()
iLastSelectedObject = -1
gSetUnit = 0

def toggleDebugMode():
	return


############################### DEBUG TOOLS ####################################
class CvDebugTools:
	def __init__(self):
		self._bDebugMode = False
		self.iActiveEffect = -1
		self.pEffectPlot = None

	def getDebugMode( self ):
		return self._bDebugMode

	def setDebugMode( self, bVal ):
		self._bDebugMode = bVal
		CyInterface().addImmediateMessage( "CvDebugTools.setDebugMode set to %s" % self.bDebugMode, "" )

	def notifyInput( self, argsList ):
		#print "Python Debug Mode Notify"
		return 0

	def initEffectViewer( self, argsList ):
		px,py = argsList
		pPlot = CyMap().plot(px,py)
		popup = CyPopup(CvUtil.PopupTypeEffectViewer, EventContextTypes.EVENTCONTEXT_SELF, True)
		popup.setSize(550,300)
		popup.setUserData( (px,py) )
		popup.setHeaderString( "Python Debug Tools: Object Placer", CvUtil.FONT_CENTER_JUSTIFY )
		# Pulldown0 - Player Selection
		numEffects = gc.getNumEffectInfos()	# get total # of units from Game

		popup.createPythonPullDown("Choose an Effect", 0)
		for i in range(gc.getNumEffectInfos()):
			popup.addPullDownString(gc.getEffectInfo(i).getType(), i, 0)

		popup.createPythonEditBox( "Default", "Modify the scale of the effect", 0 )
		popup.createPythonEditBox( "Default", "Modify the update rate", 0 )

		# Launch Popup
		popup.launch(true, PopupStates.POPUPSTATE_IMMEDIATE)
		return 0

	def applyEffectViewer(self, playerID, userData, popupReturn):
		px,py = userData
		self.pEffectPlot = CyMap().plot(px,py)

		if self.pEffectPlot.isNone():
			return 0

		self.iActiveEffect = popupReturn.getSelectedPullDownValue( 0 )

		CyEngine().triggerEffect(self.iActiveEffect, self.pEffectPlot.getPoint())
		#scale = popupReturn.getSelectedListBoxValue( 0 )
		#updateRate = int( popupReturn.getEditBoxString( 0 ) )

	############################
	## UNIT / CITY PLAYER
	############################
	def initUnitPicker( self, argsList ):
		'initUnitPicker - for placing units & cities'
		px,py = argsList
		pPlot = CyMap().plot(px,py)
		popup = CyPopup(CvUtil.EventPlaceObject, EventContextTypes.EVENTCONTEXT_ALL, True)
		popup.setSize(400,600)
		popup.setPosition(600,25)
		popup.setUserData( (px,py) )
		popup.setHeaderString( "Python Debug Tools: Object Placer", CvUtil.FONT_CENTER_JUSTIFY )

		# Pulldown0 - Player Selection
		iNumUnits = gc.getNumUnitInfos()	# get total # of units from Game
		iOwner = pPlot.getOwner()
		if ( iOwner == PlayerTypes.NO_PLAYER ):
			iOwner = gc.getGame().getActivePlayer()
		popup.createPythonPullDown("Choose a Player", 0)
		popup.addPullDownString(gc.getPlayer(iOwner).getName(), iOwner, 0)
		for i in range(gc.getMAX_PLAYERS()):
			if (gc.getPlayer(i).isEverAlive()):
				if (i != iOwner):
					popup.addPullDownString(gc.getPlayer(i).getName(), i, 0)

		popup.addSeparator()

		popup.createPythonListBox( "", 0 )
		popup.addListBoxString( 'Nothing', iNumUnits + 1, 0 )   # for clean exit
		popup.addSeparator()
		lastSelection = u""
		if ( iLastSelectedObject == -1 ):
			pass
		elif ( iLastSelectedObject == iNumUnits+1 ):
			lastSelection = u'Nothing'
		elif ( iLastSelectedObject == iNumUnits ):
			lastSelection = u'City'
		else:
			lastSelection = gc.getUnitInfo(iLastSelectedObject).getDescription()
		if ( not iLastSelectedObject == -1 ):
			popup.addListBoxString( lastSelection, iLastSelectedObject, 0 )
		popup.addListBoxString( u'City', iNumUnits, 0 )    	# list City first

		# sort units alphabetically
		unitsList=[(0,0)]*iNumUnits
		for j in range( iNumUnits ):
			unitsList[j] = (gc.getUnitInfo(j).getDescription(), j)
		unitsList.sort()

		for j in range( iNumUnits ):
			popup.addListBoxString( unitsList[j][0], unitsList[j][1], 0)

		# EditBox0 - Customize how many units to build
		popup.createPythonEditBox( "1", "This allows you to create multiple units.", 0 )

		# Launch Popup
		popup.launch(true, PopupStates.POPUPSTATE_IMMEDIATE)
		return 0

	def applyUnitPicker( self, argsList ):
		'Apply Unit Picker'
		popupReturn, userData = argsList
		px,py = userData
		pPlot = CyMap().plot(px,py)

		if pPlot.isNone():
			return 0

		# UNIT DEBUG MENU
		playerID = popupReturn.getSelectedPullDownValue( 0 )
		selectedObject = popupReturn.getSelectedListBoxValue( 0 )
		iSpawnNum = int( popupReturn.getEditBoxString( 0 ) )

		player = gc.getPlayer(playerID)
		if ( player.isNone() ):
			return -1   # Error

		iNumUnits = gc.getNumUnitInfos()
		global iLastSelectedObject
		iLastSelectedObject = selectedObject
		if ( selectedObject != iNumUnits + 1 ):# Nothing

			if ( selectedObject == iNumUnits ):# City"
				pCity = player.initCity( px,py )
				pCity.setPopulation(1)
			else:
				iYieldAmount = 0;
				for i in range(YieldTypes.NUM_YIELD_TYPES):
					if gc.getYieldInfo(i).getUnitClass() == gc.getUnitInfo(selectedObject).getUnitClassType() or gc.getUnitInfo(selectedObject).isTreasure():
						iYieldAmount = gc.getGame().getCargoYieldCapacity();

				for i in range(iSpawnNum):
					pUnit = player.initUnit(selectedObject, gc.getUnitInfo(selectedObject).getDefaultProfession(), px, py, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION, iYieldAmount)

		else:
			iLastSelectedObject = -1

		return 0

	############################
	## GOLD CHEAT POPUP
	############################
	def cheatGold( self ):
		'Cheat gold to the players'
		popup = CyPopup(CvUtil.EventAwardGold, EventContextTypes.EVENTCONTEXT_ALL, True)
		popup.setHeaderString( "Gold Cheat!", CvUtil.FONT_CENTER_JUSTIFY )
		popup.createPullDown(0)
		popup.addPullDownString( "All", gc.getMAX_CIV_PLAYERS(), 0 )
		for i in range( gc.getMAX_CIV_PLAYERS() ):
			if ( gc.getPlayer(i).isAlive() ):
				popup.addPullDownString( gc.getPlayer(i).getName(), i, 0 )
		popup.setBodyString( "Gold Amount", CvUtil.FONT_LEFT_JUSTIFY )
		popup.createPythonEditBox( "0", "Integer value (positive or negative)", 0 )

		# Loop through Era Infos and add names
		for i in range(gc.getNumEraInfos()):
			popup.addButton(gc.getEraInfo(i).getDescription())
			
		# founding fathers
		popup.createPullDown(1)
		popup.addPullDownString( "No Father", gc.getNumFatherInfos(), 1 )
		foundingFatherNames = []
		for i in range( gc.getNumFatherInfos() ):
			foundingFatherNames.append((gc.getFatherInfo(i).getDescription(), i))
			
		foundingFatherNames.sort()
		for i in range( gc.getNumFatherInfos() ):
			popup.addPullDownString( foundingFatherNames[i][0], foundingFatherNames[i][1], 1 )

		popup.launch(true, PopupStates.POPUPSTATE_IMMEDIATE)

	def applyGoldCheat( self, argsList ):
		'Apply Gold Cheat'
		popupReturn = argsList
		playerID = popupReturn.getSelectedPullDownValue( 0 )
		if playerID == gc.getMAX_CIV_PLAYERS():
			player = self.getCivPlayerList()
		else:
			player = [gc.getPlayer( playerID )]

		era = popupReturn.getButtonClicked()
		if era != EraTypes.NO_ERA:
			for i in range(len(player)):
				player[i].setCurrentEra(era)
				
		fatherID = popupReturn.getSelectedPullDownValue( 1 )
		if fatherID != gc.getNumFatherInfos():
			gc.getGame().setFatherTeam(fatherID, player[0].getTeam())

		try:
			goldChange = int( popupReturn.getEditBoxString( 0 ) )
		except:
			return 0

		if goldChange:
			for i in range(len(player)):
				player[i].changeGold(goldChange)
				
	def RotateUnit(self, Direction, px, py ):
		if ( px != -1 and py != -1 ):
			unit = CyMap().plot(px, py).getUnit(0)
			if ( not unit.isNone() ):
				unitEntity = CyUnitEntity(unit)
				dir = unitEntity.GetUnitFacingDirection( )
				dir += Direction * 0.05;
				unitEntity.SetUnitFacingDirection( dir )

	def resetUnitMovement( self ):
		global g_bDebugMode
		if g_bDebugMode == 0:
			return
		for i in range(gc.getMAX_PLAYERS()):
			(unit, iter) = gc.getPlayer(i).firstUnit()
			while (unit):
				unit.setMoves(0)
				(unit, iter) = gc.getPlayer(i).nextUnit(iter)

	def allBonuses( self ):
		iNBonuses = gc.getNumBonusInfos()
		map = CyMap()
		if ( iNBonuses < map.getGridWidth() * map.getGridHeight() ):
			for x in range(map.getGridWidth()):
				for y in range((iNBonuses/map.getGridWidth())+1):
					map.plot(x,y).setBonusType( (x + y * map.getGridWidth())%iNBonuses );

	def allImprovements( self ):
		iNImprovements = gc.getNumImprovementInfos()
		map = CyMap()
		if ( iNImprovements < map.getGridWidth() * map.getGridHeight() ):
			for x in range(map.getGridWidth()):
				for y in range((iNImprovements/map.getGridWidth())+1):
					map.plot(x,y).setImprovementType( (x + y * map.getGridWidth())%iNImprovements );

	def wonderMovie( self ):
		'ShowWonder Movie'

		popup = CyPopup(CvUtil.EventShowWonder, EventContextTypes.EVENTCONTEXT_ALL, True)
		popup.setHeaderString( "Wonder Movie", CvUtil.FONT_CENTER_JUSTIFY )
		popup.createPullDown(0)
		for i in range(gc.getNumBuildingInfos()):
			szMovieFile = gc.getBuildingInfo(i).getMovie()
			if (szMovieFile != None and len(szMovieFile) > 0):
				popup.addPullDownString( gc.getBuildingInfo(i).getDescription(), i, 0 )

		popup.launch(true, PopupStates.POPUPSTATE_IMMEDIATE)

	def applyWonderMovie( self, argsList ):
		'Apply Wonder Movie'
		popupReturn = argsList
		wonderID = popupReturn.getSelectedPullDownValue( 0 )

		popupInfo = CyPopupInfo()
		popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
		popupInfo.setData2(-1)
		popupInfo.setText(u"showWonderMovie")
		popupInfo.setData1(wonderID)
		popupInfo.addPopup(0)

	def getCivPlayerList(self):
		"objlist - List of valid and Alive players"
		playerList = []
		for i in range(gc.getMAX_CIV_PLAYERS()):
			if (gc.getPlayer(i).isAlive()):
				playerList.append(gc.getPlayer(i))
		return playerList

g_CvDebugTools = CvDebugTools()
