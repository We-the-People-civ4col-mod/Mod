## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil

gc = CyGlobalContext()
local = CyTranslator()

def isWBPopup(context):
	"helper for determining if context is a WBPopup"
	return (context >= CvUtil.PopupTypeWBContextStart and context <= CvUtil.PopupTypeWBContextEnd)

class CvWBPopups:
	"World Builder App Popups"

	# These ar popup types, not popup context IDs
	WBPopupEditCity 		= WorldBuilderPopupTypes.WBPOPUP_CITY
	WBPopupEditUnit 		= WorldBuilderPopupTypes.WBPOPUP_UNIT
	WBPopupEditPlot 		= WorldBuilderPopupTypes.WBPOPUP_PLOT
	WBPopupEditPlayer		= WorldBuilderPopupTypes.WBPOPUP_PLAYER

	def __init__(self):
		"set global instance items"

		self.WBPopupDictionary = 	{
			self.WBPopupEditCity : 	{ 'Init' : self.initEditCity, 'Apply' : self.applyEditCity },
			}

	def initWB(self, argsList):
		"called from Interface - use the PopupDictionary for quick access"
		context = argsList[0]
		entry = self.WBPopupDictionary.get(context)

		if (entry):
			init = entry.get('Init')
			return init(argsList)
		return 0

	def applyWB(self, argsList):
		"Apply the appropriate Popup using the PopupDictionary"
		context = argsList[0]
		entry = self.WBPopupDictionary.get(context-CvUtil.PopupTypeWBContextStart+WorldBuilderPopupTypes.WBPOPUP_START)

		if (entry):
			apply = entry.get('Apply')
			return apply(argsList)
		return 0


	########################################################################################
	def initEditCity(self, argsList):
		' Initializes World Builder City Edit Popup '
		px,py = argsList
		pCity = CyMap().plot(px,py).getPlotCity()
		iOwner = pCity.getOwner()
		iID = pCity.getID()
		userData = (iOwner, iID)

		# create popup
		popup = CyPopup(CvUtil.EventEditCity, EventContextTypes.EVENTCONTEXT_ALL, True)
		popup.setSize(400,600)
		popup.setUserData( userData )
		popup.setHeaderString(local.getText("TXT_KEY_WB_HEADER_CITY_EDIT", ()), CvUtil.FONT_CENTER_JUSTIFY)
		# City Name - Editbox 0
		popup.createEditBox( pCity.getName(), 0 )
		# Population - Editbox 1
		popup.setBodyString(local.getText("TXT_KEY_WB_CITY_EDIT_POP", ()), CvUtil.FONT_CENTER_JUSTIFY)
		popup.createEditBox( '0', 1 )
		# Culture - Editbox 2
		popup.setBodyString(local.getText("TXT_KEY_WB_CITY_EDIT_CULTURE", (pCity.getCulture(iOwner),)), CvUtil.FONT_CENTER_JUSTIFY)
		popup.createEditBox( '0', 2)
		# Buildings - Listboxes Group
		popup.createListBox(0)
		iNumBuildings = gc.getNumBuildingInfos()
		lBuildings = []
		for i in range( iNumBuildings ):
			# ('Library', iIndex)
			if not gc.getBuildingInfo(i).isGraphicalOnly():
				lBuildings.append( (unicode(gc.getBuildingInfo(i).getDescription()), i) )
		lBuildings.sort()

		popup.addListBoxString( local.getText("TXT_KEY_WB_CITY_NOTHING", ()), -1, 0)
		popup.addListBoxString( local.getText("TXT_KEY_WB_CITY_ALL", ()), iNumBuildings, 0)        #for adding/removing every building
		for i in range( len(lBuildings) ):
			entry = lBuildings[i]
			popup.addListBoxString(entry[0], entry[1], 0)

		# Add/Remove - Pulldown Group 0
		popup.setBodyString(local.getText("TXT_KEY_WB_CITY_ADD_REMOVE", ()), CvUtil.FONT_CENTER_JUSTIFY)
		popup.createPullDown(0)
		popup.addPullDownString( local.getText("TXT_KEY_WB_CITY_ADD", ()), 1, 0)
		popup.addPullDownString( local.getText("TXT_KEY_WB_CITY_NOCHANGE", ()), 0, 0)  #for clean exit
		popup.addPullDownString( local.getText("TXT_KEY_WB_CITY_REMOVE", ()), 2, 0)

		# Launch Popup
		popup.launch(true, PopupStates.POPUPSTATE_IMMEDIATE)
		return 0

	def applyEditCity( self, argsList):
		' Apply World Builder City Edit'
		popupReturn, userData = argsList
		iOwner, iID = userData
		pCity = gc.getPlayer(iOwner).getCity(iID)

		if pCity.isNone():
			return 0

		# EDITABLE: Name, Population, Culture, Buildings
		# Name
		newName = unicode(popupReturn.getEditBoxString( 0 ))
		currName = pCity.getName()
		if ( newName != currName ):
			pCity.setName( newName, False )

		# Population
		PopModifier = int(popupReturn.getEditBoxString( 1 ))
		if PopModifier:
			pCity.changePopulation( PopModifier )

		iCultureModifier = int(popupReturn.getEditBoxString( 2 ))
		if iCultureModifier:
			pCity.setCulture(iOwner, iCultureModifier)

		# Buildings
		iNumBuildings = gc.getNumBuildingInfos()	#get total # of units from Game
		if ( popupReturn.getSelectedListBoxValue(0)>=0 and popupReturn.getSelectedPullDownValue(0)>0 ):
			BuildingIdx = popupReturn.getSelectedListBoxValue(0)
			if (BuildingIdx == iNumBuildings+1):
				return 0
			AddRemoveIdx = popupReturn.getSelectedPullDownValue(0)
			CvUtil.AdjustBuilding(AddRemoveIdx==1, BuildingIdx==iNumBuildings, BuildingIdx, pCity)
		return 0
