## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
##
## New TAC - Interface
## Version 1.02
## by koma13 & Fankman
##


from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
# Achievements START
import CvScreensInterface
# Achievements END
import CvEventInterface
import time
import math

# GLOBAL SYSTEM SHORTCUTS
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

# GLOBAL INFOCLASS COUNTS
g_NumEmphasizeInfos = 0
g_NumHurryInfos = 0
g_NumActionInfos = 0
g_eEndTurnButtonState = -1

# GLOBAL INITIALIZATION BOOLEAN
INITIALIZED = false
abandon_city_ZeroRedraw = true # TAC abandon city - Do we redraw if a pop 0 city is viewed?

# GLOBAL RESOLUTION VALUES
xResolution = -1
yResolution = -1

# GLOBAL INTERFACE FUNDAMENTAL CONSTRUCTS
FLAG_WIDTH = -1
FLAG_PERCENT_WIDTH = 8

MINI_MAP_WIDTH = -1
MINI_MAP_PERCENT_WIDTH = 30

LOWER_RIGHT_CORNER_BACKGROUND_WIDTH = MINI_MAP_WIDTH

SADDLE_HEIGHT = -1
SADDLE_HEIGHT_PERCENT = 31

LOWER_LEFT_CORNER_BACKGROUND_WIDTH = -1
LOWER_LEFT_CORNER_BACKGROUND_PERCENT_WIDTH = 22

LOWER_LEFT_CORNER_BACKGROUND_HEIGHT = -1
LOWER_LEFT_CORNER_BACKGROUND_PERCENT_HEIGHT = 25

LOWER_RIGHT_CORNER_BACKGROUND_WIDTH = -1
LOWER_RIGHT_CORNER_BACKGROUND_WIDTH_PERCENT = 19

LOWER_RIGHT_CORNER_BACKGROUND_HEIGHT = -1
LOWER_RIGHT_CORNER_BACKGROUND_HEIGHT_PERCENT = 10

BOTTOM_CENTER_HUD_HEIGHT = -1
BOTTOM_CENTER_HUD_PERCENT_HEIGHT = 15

TOP_CORNER_HUD_WIDTHS = -1
TOP_CORNER_HUD_PERCENT_WIDTH = 20

TOP_CORNER_HUD_HEIGHT = -1
TOP_CORNER_HUD_PERCENT_HEIGHT = 8

TOP_CENTER_HUD_HEIGHT = -1
TOP_CENTER_HUD_PERCENT_HEIGHT = 8

TOP_CENTER_HUD_WIDTH = -1
TOP_CENTER_HUD_PERCENT_WIDTH = 45

CITIZEN_BAR_HEIGHT = -1
CITIZEN_BAR_PERCENT_HEIGHT = 10

CITY_TITLE_BAR_HEIGHT = -1
CITY_TITLE_BAR_PERCENT_HEIGHT = 5

TRANSPORT_AREA_HEIGHT = -1

CITIZEN_BAR_WIDTH = -1
CITIZEN_BAR_WIDTH_PERCENT = 80

STACK_BAR_HEIGHT = -1
STACK_BAR_PERCENT_HEIGHT = 2.7

EJECT_AREA_WIDTH = -1
EJECT_AREA_PERCENT_WIDTH = 6

MINI_MAP_RADIUS = -1
MINI_MAP_PERCENT_RADIUS = 31

LEFT_PLOT_LIST_EDGE = -1
LEFT_PLOT_LIST_PERCENT_EDGE = 40

RIGHT_PLOT_LIST_EDGE = -1
RIGHT_PLOT_LIST_PERCENT_EDGE = 25

MAIN_LARGE_PORTRAIT_HEIGHT = -1
MAIN_LARGE_PORTRAIT_PERCENT_HEIGHT = 22

MAIN_LARGE_PORTRAIT_WIDTH = -1
MAIL_LARGE_PORTRAIT_PERCENT_WIDTH = 10

UNIT_PORTRAIT_HEIGHT = -1
UNIT_PORTRAIT_PERCENT_HEIGHT = 15

UNIT_HEALTH_BAR_WIDTH = -1
UNIT_HEALHT_BAR_PERCENT_WIDTH = 17

CITY_VIEW_BOX_HEIGHT_AND_WIDTH = -1
CITY_VIEW_BOX_PERCENT_HEIGHT = 38
CITY_VIEW_BOX_MODIFIED_PERCENT_HEIGHT_AND_WIDTH = -1

CITY_MULTI_TAB_AREA_HEIGHT = -1
CITY_MULTI_TAB_SIZE = -1

MAP_EDGE_MARGIN_WIDTH = -1
MAP_EDGE_MARGIN_PERCENT_WIDTH = 0.7

BUILD_AREA_WIDTH = -1
BUILD_AREA_HEIGHT = -1

SMALL_BUTTON_SIZE = -1
MEDIUM_BUTTON_SIZE = -1
LARGE_BUTTON_SIZE = -1
STANDARD_FONT_SIZE = -1

ASPECT_RATIO = -1
ASPECT_ADJUSTMENT = -1

RESOURCE_TABLE_COLUMN_WIDTH = -1
RESOURCE_TABLE_MARGIN = -1

AVOID_GROWTH = -1
SHOW_ALL_YIELDS = -1

HELPTEXT_AREA_Y_MARGIN = 0
HELTTEXT_AREA_X_MARGIN = 7
HELPTEXT_AREA_MAX_WIDTH = 400
HELPTEXT_AREA_MIN_WIDTH = 150

# CITY BUILDINGS GRID MAP
BUILDING_DATA = ["", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""]

BUILDING_DATA[0] = [30, 18, 20, 12] #Bells
BUILDING_DATA[1] = [72, 2, 20, 12] #Courthouse
BUILDING_DATA[2] = [44, 20, 20, 12] #Hammers
BUILDING_DATA[3] = [58, 18, 20, 12] #Meat
BUILDING_DATA[4] = [30, 36, 20, 12] #Coats
BUILDING_DATA[5] = [44, 38, 20, 12] #Leather
BUILDING_DATA[6] = [44, 2, 20, 12] #Cloth
BUILDING_DATA[7] = [58, 36, 20, 12] #Coloured Cloth
BUILDING_DATA[8] = [16, 38, 20, 12] #Beer
BUILDING_DATA[9] = [58, 0, 20, 12] #Rum
BUILDING_DATA[10] = [58, 54, 20, 12] #Wine
BUILDING_DATA[11] = [16, 20, 20, 12] #Cigars
BUILDING_DATA[12] = [86, 18, 20, 12] #Whale oil
BUILDING_DATA[13] = [30, 54, 20, 12] #Furniture
BUILDING_DATA[14] = [2, 18, 20, 12] #Tools
BUILDING_DATA[15] = [30, 0, 20, 12] #Crosses
BUILDING_DATA[16] = [2, 36, 20, 12] #Muskets
BUILDING_DATA[17] = [2, 0, 20, 12] #Fireworker
BUILDING_DATA[18] = [30, 72, 20, 12] #Horses
BUILDING_DATA[19] = [86, 36, 20, 12] #Dock
BUILDING_DATA[20] = [86, 54, 20, 12] #WareHouse
BUILDING_DATA[21] = [2, 54, 20, 12] #Spices
BUILDING_DATA[22] = [44, 74, 20, 12] #Domestic Market
BUILDING_DATA[23] = [86, 0, 20, 12] #Tavern
BUILDING_DATA[24] = [16, 2, 20, 12] #Education
BUILDING_DATA[25] = [87, 78, 20, 10] #Wall
BUILDING_DATA[26] = [72, 74, 20, 10] #Print
BUILDING_DATA[27] = [2, 89, 20, 9] #Mill
BUILDING_DATA[28] = [72, 86, 20, 10] #Trainstation
BUILDING_DATA[29] = [32, 89, 20, 7] #Corn
BUILDING_DATA[30] = [72, 38, 20, 12] #Medical Care
BUILDING_DATA[31] = [18, 88, 20, 8] #Well
BUILDING_DATA[32] = [84, 73, 20, 10] #Defense
BUILDING_DATA[33] = [44, 56, 20, 12] #Confectioner
BUILDING_DATA[34] = [16, 56, 20, 12] #Leather Padder
BUILDING_DATA[35] = [2, 72, 20, 12] #Cooking Oil Trader
BUILDING_DATA[36] = [58, 72, 20, 12] #Peanut Roaster
BUILDING_DATA[37] = [72, 56, 20, 12] #Potter
BUILDING_DATA[38] = [72, 20, 20, 12] #Tailor
BUILDING_DATA[39] = [16, 74, 20, 12] #Baker
BUILDING_DATA[40] = [81, 88, 20, 7] #Statue
BUILDING_DATA[41] = [45, 88, 20, 10] #Graveyard
BUILDING_DATA[42] = [59, 86, 20, 10] #Jail

BUILDING_GRID = ["", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""]

BUILDING_AREA_WIDTH = -1
BUILDING_AREA_HEIGHT = -1

# PANEL TOGGLE CONTROLS
BUILDING_MANAGMENT_TOGGLE = 1000
AUTOMATION_MANAGMENT_TOGGLE = 1001
MAP_MANAGMENT_TOGGLE = 1002
GO_TO_CITY = 1003

# Achievements START
ACHIEVE_ADVISOR_SCREEN_MI = 4200
# Achievements END

# R&R, Robert Surcouf VET NewCapacity Help - Start
VET_NEW_CAPACITY = 4201
# R&R, Robert Surcouf VET NewCapacity Help - End

TIMETEXT_ID = 4202

# CITY HAPPINESS vs UNHAPPINESS - START
HELP_TOTAL_HAPPINESS_POSITIVE = 4301
HELP_TOTAL_HAPPINESS_NEGATIVE = 4302
HELP_TOTAL_HAPPINESS_ZERO = 4303
# CITY HAPPINESS vs UNHAPPINESS - END

NEW_HARBOUR_SYSTEM = 4304 # WTP, ray, new Harbour System - START
NEW_BARRACKS_SYSTEM = 4305 # WTP, ray, new Barracks System - START

# CRIME and LAW - START
HELP_TOTAL_LAW_POSITIVE = 4401
HELP_TOTAL_LAW_NEGATIVE = 4402
HELP_TOTAL_LAW_ZERO = 4403
# CRIME and LAW - END

BUILDING_MANAGMENT_PANEL_UP = True
BUILDING_CHANGE_MANAGMENT_PANEL_UP = False
SHIP_MANAGMENT_PANEL_UP = False
AUTOMATION_MANAGMENT_PANEL_UP = False
MAP_MANAGMENT_PANEL_UP = False
GARRISON_MANAGMENT_PANEL_UP = False
SHOW_ALL_YIELDS = True

# HIDE TYPE
HIDE_TYPE_CITY = 0
HIDE_TYPE_MAP = 1
HIDE_TYPE_GLOBAL = 2
HIDE_TYPE_ADVANCED_START = 3
HIDE_TYPE_MINI_MAP = 4
NUM_HIDE_TYPES = 5

# HIDE LEVEL
HIDE_LEVEL_OFF = 0
HIDE_LEVEL_ALL = 1
HIDE_LEVEL_HIDE = 2
HIDE_LEVEL_NORMAL = 3
HIDE_LEVEL_UBER = 4
NUM_HIDE_LEVELS = 5

# BONUS HIDE GROUPS
RESOURCE_TABLE_HIDE = (NUM_HIDE_TYPES * NUM_HIDE_LEVELS) + 1
CITIZEN_HIDE = (NUM_HIDE_TYPES * NUM_HIDE_LEVELS) + 2
ACTION_BUTTON_HIDE = (NUM_HIDE_TYPES * NUM_HIDE_LEVELS) + 3

# EMPHASIZE DATA
EMPHASIZEYIELDS = []

# INIT MASTER HIDE LIST
MasterHideList = []
for i in range(NUM_HIDE_TYPES):
	SubList = []
	for j in range(NUM_HIDE_LEVELS):
		SubSubList = []
		SubList.append(SubSubList)
	MasterHideList.append(SubList)
	
# PLOT LIST BUTTONS
NUM_PLOT_LIST_BUTTONS = 1

# GLOBE LAYER OPTION POSITIONING
iGlobeLayerOptionsY_Minimal = 38 # distance from bottom edge
iGlobeLayerOptionHeight = 24

g_pSelectedUnit = 0

class CvMainInterface:
	"Main Interface Screen"

	def numPlotListButtons( self ):
		return NUM_PLOT_LIST_BUTTONS

	# Adds the Widget to a list for hiding and unhiding as the screen updates
	def appendtoHideState( self, screen, Name, Type, Level ):
		global MasterHideList
		screen.hide(Name)
		MasterHideList[Type][Level].append(Name)

	# Shows Widgets of the specified Type at and above a particular Hiding level
	def SetHideState( self, screen, Type, Level ):

		for T in range(NUM_HIDE_TYPES):
			if T != Type:
				for L in range(NUM_HIDE_LEVELS):
					List = (T * NUM_HIDE_LEVELS) +  L
					screen.hideList(List)

		for T in range(NUM_HIDE_TYPES):
			if T == Type:
				for L in range(NUM_HIDE_LEVELS):
					if (L > Level):
						List = (T * NUM_HIDE_LEVELS) +  L
						screen.hideList(List)
					else:
						List = (T * NUM_HIDE_LEVELS) +  L
						screen.showList(List)

	def SetHideLists( self, screen ):
		for T in range(NUM_HIDE_TYPES):
			for L in range(NUM_HIDE_TYPES):
				HideIndex = (T * NUM_HIDE_LEVELS) + L
				screen.registerHideList(MasterHideList[T][L], len(MasterHideList[T][L]), HideIndex)
				
		screen.registerHideList([], 0, NUM_HIDE_TYPES * NUM_HIDE_LEVELS)

	# Sets Global screen construct values based on our Resolution
	def SetGlobals ( self, screen ):

	# GET RESOLUTION
		global xResolution
		global yResolution

		xResolution = screen.getXResolution()
		yResolution = screen.getYResolution()

	# ADJUST FOR RESOLUTION
		global FLAG_WIDTH
		FLAG_WIDTH = int((FLAG_PERCENT_WIDTH * xResolution) / 100)

		global MINI_MAP_WIDTH
		MINI_MAP_WIDTH = int((MINI_MAP_PERCENT_WIDTH * xResolution) / 100)

		global SADDLE_HEIGHT
		SADDLE_HEIGHT = int((SADDLE_HEIGHT_PERCENT * yResolution) / 100)

		global MINIMAP_HEIGHT
		MINIMAP_HEIGHT = SADDLE_HEIGHT - 5

		global LOWER_RIGHT_CORNER_BACKGROUND_WIDTH
		LOWER_RIGHT_CORNER_BACKGROUND_WIDTH = int((LOWER_RIGHT_CORNER_BACKGROUND_WIDTH_PERCENT * xResolution) / 100)

		global LOWER_LEFT_CORNER_BACKGROUND_WIDTH
		LOWER_LEFT_CORNER_BACKGROUND_WIDTH = int((LOWER_LEFT_CORNER_BACKGROUND_PERCENT_WIDTH * xResolution) / 100)

		global LOWER_LEFT_CORNER_BACKGROUND_HEIGHT
		LOWER_LEFT_CORNER_BACKGROUND_HEIGHT = int((LOWER_LEFT_CORNER_BACKGROUND_PERCENT_HEIGHT * yResolution) / 100)

		global LOWER_RIGHT_CORNER_BACKGROUND_HEIGHT
		LOWER_RIGHT_CORNER_BACKGROUND_HEIGHT = int((LOWER_RIGHT_CORNER_BACKGROUND_HEIGHT_PERCENT * yResolution) / 100)

		global BOTTOM_CENTER_HUD_HEIGHT
		BOTTOM_CENTER_HUD_HEIGHT = int((BOTTOM_CENTER_HUD_PERCENT_HEIGHT * yResolution) / 100)

		global TOP_CORNER_HUD_WIDTHS
		TOP_CORNER_HUD_WIDTHS = int((TOP_CORNER_HUD_PERCENT_WIDTH * xResolution) / 100)

		global TOP_CORNER_HUD_HEIGHT
		TOP_CORNER_HUD_HEIGHT = int((TOP_CORNER_HUD_PERCENT_HEIGHT * yResolution) / 100)

		global TOP_CENTER_HUD_HEIGHT
		TOP_CENTER_HUD_HEIGHT = int((TOP_CENTER_HUD_PERCENT_HEIGHT * yResolution) / 100)

		global TOP_CENTER_HUD_WIDTH
		TOP_CENTER_HUD_WIDTH = max(400, int((TOP_CENTER_HUD_PERCENT_WIDTH * xResolution) / 100))

		global CITY_TITLE_BAR_HEIGHT
		CITY_TITLE_BAR_HEIGHT = int((CITY_TITLE_BAR_PERCENT_HEIGHT * yResolution) / 100)

		global CITIZEN_BAR_HEIGHT
		CITIZEN_BAR_HEIGHT = int((CITIZEN_BAR_PERCENT_HEIGHT * yResolution) / 100)

		global STACK_BAR_HEIGHT
		STACK_BAR_HEIGHT = int((STACK_BAR_PERCENT_HEIGHT * yResolution) / 100)

		global EJECT_AREA_WIDTH
		EJECT_AREA_WIDTH = int((EJECT_AREA_PERCENT_WIDTH * xResolution) / 100)

		global MINI_MAP_RADIUS
		MINI_MAP_RADIUS = int((MINI_MAP_PERCENT_RADIUS * xResolution) / 100)

		global LEFT_PLOT_LIST_EDGE
		LEFT_PLOT_LIST_EDGE = int((LEFT_PLOT_LIST_PERCENT_EDGE * xResolution) / 100)

		global RIGHT_PLOT_LIST_EDGE
		RIGHT_PLOT_LIST_EDGE = int((RIGHT_PLOT_LIST_PERCENT_EDGE * xResolution) / 100)

		global MAIN_LARGE_PORTRAIT_HEIGHT
		MAIN_LARGE_PORTRAIT_HEIGHT = int((MAIN_LARGE_PORTRAIT_PERCENT_HEIGHT * yResolution) / 100)

		global MAIN_LARGE_PORTRAIT_WIDTH
		MAIN_LARGE_PORTRAIT_WIDTH = int((MAIL_LARGE_PORTRAIT_PERCENT_WIDTH * xResolution) / 100)

		global UNIT_PORTRAIT_HEIGHT
		UNIT_PORTRAIT_HEIGHT = int((UNIT_PORTRAIT_PERCENT_HEIGHT * yResolution) / 100)

		global UNIT_HEALTH_BAR_WIDTH
		UNIT_HEALTH_BAR_WIDTH = int((UNIT_HEALHT_BAR_PERCENT_WIDTH * xResolution) / 100)

		global SMALL_BUTTON_SIZE
		global MEDIUM_BUTTON_SIZE
		global LARGE_BUTTON_SIZE
		global STANDARD_FONT_SIZE

		if xResolution > 1600 and yResolution > 1024:
			SMALL_BUTTON_SIZE = 36
			MEDIUM_BUTTON_SIZE = 42
			LARGE_BUTTON_SIZE =	48
			STANDARD_FONT_SIZE = 4
		elif xResolution > 1280 and yResolution > 800:
			SMALL_BUTTON_SIZE = 32
			MEDIUM_BUTTON_SIZE = 36
			LARGE_BUTTON_SIZE = 40
			STANDARD_FONT_SIZE = 3
		else:
			SMALL_BUTTON_SIZE = 24
			MEDIUM_BUTTON_SIZE = 32
			LARGE_BUTTON_SIZE = 36
			STANDARD_FONT_SIZE = 3

		global ASPECT_RATIO
		ASPECT_RATIO = xResolution / yResolution

		global ASPECT_ADJUSTMENT
		ASPECT_ADJUSTMENT = ((float(xResolution) / float(yResolution)) * 3.0) / 4.0

		AspectAdjustment = 1 - ((1 - ASPECT_ADJUSTMENT) / 3) 
		global CITY_VIEW_BOX_MODIFIED_PERCENT_HEIGHT_AND_WIDTH
		CITY_VIEW_BOX_MODIFIED_PERCENT_HEIGHT_AND_WIDTH = int(CITY_VIEW_BOX_PERCENT_HEIGHT * AspectAdjustment)
	
		global MAP_EDGE_MARGIN_WIDTH
		MAP_EDGE_MARGIN_WIDTH = int((MAP_EDGE_MARGIN_PERCENT_WIDTH * xResolution) / 100)

		global CITY_VIEW_BOX_HEIGHT_AND_WIDTH
		CITY_VIEW_BOX_HEIGHT_AND_WIDTH = int((CITY_VIEW_BOX_MODIFIED_PERCENT_HEIGHT_AND_WIDTH * yResolution) / 100)

		global CITIZEN_BAR_WIDTH
		CITIZEN_BAR_WIDTH = int((CITIZEN_BAR_WIDTH_PERCENT * (xResolution - CITY_VIEW_BOX_HEIGHT_AND_WIDTH)) / 100)

		global BUILD_AREA_WIDTH
		BUILD_AREA_WIDTH = int(((100 - CITIZEN_BAR_WIDTH_PERCENT) * (xResolution - CITY_VIEW_BOX_HEIGHT_AND_WIDTH )) / 100)
	
		global BUILD_AREA_HEIGHT
		BUILD_AREA_HEIGHT = CITY_VIEW_BOX_HEIGHT_AND_WIDTH - STACK_BAR_HEIGHT
		
		global BUILDING_AREA_WIDTH
		BUILDING_AREA_WIDTH = int(CITIZEN_BAR_WIDTH - (STACK_BAR_HEIGHT * 2))

		global BUILDING_AREA_HEIGHT 
		BUILDING_AREA_HEIGHT = yResolution - BOTTOM_CENTER_HUD_HEIGHT - STACK_BAR_HEIGHT - CITY_TITLE_BAR_HEIGHT

		global TRANSPORT_AREA_HEIGHT
		TRANSPORT_AREA_HEIGHT = (yResolution - BOTTOM_CENTER_HUD_HEIGHT - CITY_TITLE_BAR_HEIGHT - CITY_VIEW_BOX_HEIGHT_AND_WIDTH) / 2

		global TRANSPORT_AREA_WIDTH
		TRANSPORT_AREA_WIDTH = (xResolution - CITIZEN_BAR_WIDTH) * 7 / 10

		global CITY_MULTI_TAB_AREA_HEIGHT
		CITY_MULTI_TAB_AREA_HEIGHT = ((yResolution - BOTTOM_CENTER_HUD_HEIGHT - CITY_TITLE_BAR_HEIGHT - CITY_VIEW_BOX_HEIGHT_AND_WIDTH) / 2) - (STACK_BAR_HEIGHT * 3 / 2)

		global CITY_MULTI_TAB_SIZE
		CITY_MULTI_TAB_SIZE = (TRANSPORT_AREA_HEIGHT - STACK_BAR_HEIGHT - MAP_EDGE_MARGIN_WIDTH) / 3

		global RIGHT_PANEL_WIDTH
		RIGHT_PANEL_WIDTH = LOWER_RIGHT_CORNER_BACKGROUND_WIDTH * 6 / 5

		global MINIMAP_WIDTH
		MINIMAP_WIDTH = MINIMAP_HEIGHT * 87 / 100
		
		global UPPER_SADDLE_HEIGHT
		UPPER_SADDLE_HEIGHT = SADDLE_HEIGHT * 29 / 56
		
		global LOWER_SADDLE_HEIGHT
		LOWER_SADDLE_HEIGHT = SADDLE_HEIGHT * 28 / 100
		
		global DOMESTIC_ADVISOR_HEIGHT
		DOMESTIC_ADVISOR_HEIGHT = yResolution * 69 / 100
		
		global DOMESTIC_ADVISOR_SPACE
		DOMESTIC_ADVISOR_SPACE = yResolution - DOMESTIC_ADVISOR_HEIGHT
		
		global BOTTOM_BUTTON_SIZE
		BOTTOM_BUTTON_SIZE = (BUILD_AREA_WIDTH - (STACK_BAR_HEIGHT * 3 / 2)) / 4

		global BUILDING_GRID
		BUILDING_GRID = []
		for iBUILDING_TYPE in range(len(BUILDING_DATA)):
			TypeArray = []
			for iData in range(len(BUILDING_DATA[iBUILDING_TYPE])):
				if  iData == 0 :
					Dimention = BUILDING_AREA_WIDTH
				elif iData == 1:
					Dimention = BUILDING_AREA_HEIGHT
				elif iData == 2:
					Dimention = BUILDING_AREA_HEIGHT
				else:
					Dimention = BUILDING_AREA_WIDTH

				TypeArray.append(int((BUILDING_DATA[iBUILDING_TYPE][iData] * Dimention) / 100))

			BUILDING_GRID.append(TypeArray)

		global AVOID_GROWTH
		for iEmphasize in range(gc.getNumEmphasizeInfos()):
			if gc.getEmphasizeInfo(iEmphasize).isAvoidGrowth():
				AVOID_GROWTH = iEmphasize
				break

	# EMPHASIZE YIELDS
		global EMPHASIZEYIELDS
		EMPHASIZEYIELDS = []
		for iYield in range(YieldTypes.NUM_YIELD_TYPES):
			for iEmp in range(gc.getNumEmphasizeInfos()):
				if gc.getEmphasizeInfo(iEmp).getYieldChange(iYield) != 0:
					EMPHASIZEYIELDS.append(iYield)
					break

	# LAST CACHED UNITS COUNT
		global LAST_CACHED_GARRISON_UNITS_COUNT
		LAST_CACHED_GARRISON_UNITS_COUNT = 0
		
		global LAST_CACHED_TRANSPORT_UNITS_COUNT
		LAST_CACHED_TRANSPORT_UNITS_COUNT = 0
		
	# SET INITIALIZATION
		global INITIALIZED
		INITIALIZED = True
	
		global HELPTEXT_AREA_Y_MARGIN
		if gc.getGame().isOption(GameOptionTypes.GAMEOPTION_NO_MORE_VARIABLES_HIDDEN):
			HELPTEXT_AREA_Y_MARGIN = 40
		elif yResolution<800:
			HELPTEXT_AREA_Y_MARGIN = 10
		else:
			HELPTEXT_AREA_Y_MARGIN = 0
	
	# Wraps <Font> tags around text
	def setFontSize( self, Text, relativeSize ):
		size = STANDARD_FONT_SIZE + relativeSize
		if size > 4:
			size = 4
		return "<font=" + str(size) + ">" + Text + "</font>"

	# Will Initialize the majority of Background panels and Widgets
	def interfaceScreen ( self ):
		if (CyGame().isPitbossHost()):
			return

		self.TableYields = []
		for iYield in range(YieldTypes.NUM_YIELD_TYPES):
			if gc.getYieldInfo(iYield).isCargo():
				self.TableYields.append(iYield)
	# GLOBAL NUM VARIABLES SET
		global g_NumEmphasizeInfos
		global g_NumHurryInfos
		global g_NumActionInfos

		g_NumEmphasizeInfos = gc.getNumEmphasizeInfos()
		g_NumHurryInfos = gc.getNumHurryInfos()
		g_NumActionInfos = gc.getNumActionInfos()

	# INIT MAIN INTERFACE SCREEN
		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE )
		self.SetGlobals (screen)
		screen.setForcedRedraw(True)
		screen.setMainInterface(True)

	# CITY BURLAP BACKGROUND PANELS
		screen.addPanel("CityTopBackground", u"", u"", True, False, 0, 0, xResolution, CITY_TITLE_BAR_HEIGHT, PanelStyles.PANEL_STYLE_STANDARD, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDrawControl("CityTopBackground", ArtFileMgr.getInterfaceArtInfo("INTERFACE_CITY_BG_TOP").getPath(), 0, 0, xResolution, CITY_TITLE_BAR_HEIGHT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		self.appendtoHideState(screen, "CityTopBackground", HIDE_TYPE_CITY, HIDE_LEVEL_ALL)

		screen.addPanel("CityLeftBackground", u"", u"", True, False, 0, CITY_TITLE_BAR_HEIGHT, CITIZEN_BAR_WIDTH, yResolution, PanelStyles.PANEL_STYLE_STANDARD, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDrawControl("CityLeftBackground", ArtFileMgr.getInterfaceArtInfo("INTERFACE_CITY_BG_LEFT").getPath(), 0, CITY_TITLE_BAR_HEIGHT, CITIZEN_BAR_WIDTH, yResolution, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		self.appendtoHideState(screen, "CityLeftBackground", HIDE_TYPE_CITY, HIDE_LEVEL_ALL)

		screen.addPanel("CityLowBackground", u"", u"", True, False, CITIZEN_BAR_WIDTH, CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2), xResolution - CITIZEN_BAR_WIDTH, yResolution - CITY_VIEW_BOX_HEIGHT_AND_WIDTH - CITY_TITLE_BAR_HEIGHT + MAP_EDGE_MARGIN_WIDTH, PanelStyles.PANEL_STYLE_STANDARD, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDrawControl("CityLowBackground", ArtFileMgr.getInterfaceArtInfo("INTERFACE_CITY_BG_LOW").getPath(), CITIZEN_BAR_WIDTH, CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2), xResolution - CITIZEN_BAR_WIDTH, yResolution - CITY_VIEW_BOX_HEIGHT_AND_WIDTH - CITY_TITLE_BAR_HEIGHT + (MAP_EDGE_MARGIN_WIDTH * 2), WidgetTypes.WIDGET_GENERAL, -1, -1 )
		self.appendtoHideState(screen, "CityLowBackground", HIDE_TYPE_CITY, HIDE_LEVEL_ALL)
		
		screen.addPanel("CityRightBackground", u"", u"", True, False, CITIZEN_BAR_WIDTH + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2), CITY_TITLE_BAR_HEIGHT, BUILD_AREA_WIDTH + (MAP_EDGE_MARGIN_WIDTH * 2), CITY_VIEW_BOX_HEIGHT_AND_WIDTH, PanelStyles.PANEL_STYLE_STANDARD, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDrawControl("CityRightBackground", ArtFileMgr.getInterfaceArtInfo("INTERFACE_CITY_BG_RIGHT").getPath(), CITIZEN_BAR_WIDTH + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2), CITY_TITLE_BAR_HEIGHT, BUILD_AREA_WIDTH + (MAP_EDGE_MARGIN_WIDTH * 2) + 10, CITY_VIEW_BOX_HEIGHT_AND_WIDTH, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		self.appendtoHideState(screen, "CityRightBackground", HIDE_TYPE_CITY, HIDE_LEVEL_ALL)

	# CITY FRAMING PANELS
		screen.addPanel("CityTitlePanel", u"", u"", True, False, 0, 0, xResolution, CITY_TITLE_BAR_HEIGHT, PanelStyles.PANEL_STYLE_STANDARD, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDrawControl("CityTitlePanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCREEN_TITLE").getPath(), 0, 0, xResolution, int(CITY_TITLE_BAR_HEIGHT * 1.3), WidgetTypes.WIDGET_GENERAL, -1, -1 )
		self.appendtoHideState(screen, "CityTitlePanel", HIDE_TYPE_CITY, HIDE_LEVEL_ALL)	

	# RESOURCE TABLE BACKGROUND
		screen.addScrollPanel( "CityTransportBackGroundPanel", u"", xResolution * 511 / 512 - xResolution * 73 / 256 , yResolution - BOTTOM_CENTER_HUD_HEIGHT - TRANSPORT_AREA_HEIGHT * 9 / 8 - 2, xResolution * 19 / 64, TRANSPORT_AREA_HEIGHT + (STACK_BAR_HEIGHT / 3) + SMALL_BUTTON_SIZE * 11 / 8, PanelStyles.PANEL_STYLE_MAIN, false, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		#screen.addDrawControl("CityTransportBackGroundPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_WIDE_BOX").getPath(),  xResolution * 511 / 512 - xResolution * 73 / 256 , yResolution - BOTTOM_CENTER_HUD_HEIGHT - TRANSPORT_AREA_HEIGHT * 9 / 8 - 2, xResolution * 19 / 64, TRANSPORT_AREA_HEIGHT + (STACK_BAR_HEIGHT / 3) + SMALL_BUTTON_SIZE * 11 / 8, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		self.appendtoHideState(screen, "CityTransportBackGroundPanel", HIDE_TYPE_CITY, HIDE_LEVEL_ALL)	

		screen.addPanel("CityBuildFramePanel", u"", u"", True, False, CITIZEN_BAR_WIDTH + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - MAP_EDGE_MARGIN_WIDTH, CITY_TITLE_BAR_HEIGHT, BUILD_AREA_WIDTH, CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2), PanelStyles.PANEL_STYLE_STANDARD, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDrawControl("CityBuildFramePanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUILD_BOX").getPath(), CITIZEN_BAR_WIDTH + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - MAP_EDGE_MARGIN_WIDTH, CITY_TITLE_BAR_HEIGHT, BUILD_AREA_WIDTH, CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2), WidgetTypes.WIDGET_GENERAL, -1, -1 )
		self.appendtoHideState(screen, "CityBuildFramePanel", HIDE_TYPE_CITY, HIDE_LEVEL_ALL)	

		screen.addPanel("CityMapFramePanel", u"", u"", True, False, CITIZEN_BAR_WIDTH, CITY_TITLE_BAR_HEIGHT, CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2), CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2) , PanelStyles.PANEL_STYLE_STANDARD, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDrawControl("CityMapFramePanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_MAP_FRAME").getPath(), CITIZEN_BAR_WIDTH, CITY_TITLE_BAR_HEIGHT, CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2), CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2), WidgetTypes.WIDGET_GENERAL, -1, -1 )
		self.appendtoHideState(screen, "CityMapFramePanel", HIDE_TYPE_CITY, HIDE_LEVEL_ALL)	

		screen.addPanel("CityMultiTabFramePanel", u"", u"", True, False, CITIZEN_BAR_WIDTH, CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH, xResolution - CITIZEN_BAR_WIDTH - MAP_EDGE_MARGIN_WIDTH, TRANSPORT_AREA_HEIGHT - STACK_BAR_HEIGHT, PanelStyles.PANEL_STYLE_STANDARD, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDrawControl("CityMultiTabFramePanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_WIDE_BOX").getPath(), CITIZEN_BAR_WIDTH, CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - MAP_EDGE_MARGIN_WIDTH , xResolution - CITIZEN_BAR_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 1), TRANSPORT_AREA_HEIGHT - STACK_BAR_HEIGHT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		self.appendtoHideState(screen, "CityMultiTabFramePanel", HIDE_TYPE_CITY, HIDE_LEVEL_ALL)	
		
		screen.addScrollPanel("CityGarrisonBackGroundPanel", u"", CITIZEN_BAR_WIDTH, yResolution - BOTTOM_CENTER_HUD_HEIGHT - TRANSPORT_AREA_HEIGHT * 9 / 8 - 2, xResolution - CITIZEN_BAR_WIDTH - TRANSPORT_AREA_WIDTH + STACK_BAR_HEIGHT + 15, TRANSPORT_AREA_HEIGHT + (STACK_BAR_HEIGHT / 3) + SMALL_BUTTON_SIZE * 11 / 8, PanelStyles.PANEL_STYLE_STANDARD, false, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDrawControl("CityGarrisonBackGroundPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_WIDE_BOX").getPath(), CITIZEN_BAR_WIDTH, yResolution - BOTTOM_CENTER_HUD_HEIGHT - TRANSPORT_AREA_HEIGHT * 9 / 8 - 2, xResolution - CITIZEN_BAR_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 1), TRANSPORT_AREA_HEIGHT + (STACK_BAR_HEIGHT / 1) , WidgetTypes.WIDGET_GENERAL, -1, -1 )
		self.appendtoHideState(screen, "CityGarrisonBackGroundPanel", HIDE_TYPE_CITY, HIDE_LEVEL_ALL)	

		# WTP, ray, Center Plot specific Backgrounds - START
		screen.addPanel("CityManagerBackground", u"", u"", True, False, 0, CITY_TITLE_BAR_HEIGHT, int(CITIZEN_BAR_WIDTH * 1.07), int((yResolution - (BOTTOM_CENTER_HUD_HEIGHT + CITY_TITLE_BAR_HEIGHT)) * 1.07), PanelStyles.PANEL_STYLE_STANDARD, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		# screen.addDrawControl("CityManagerBackground", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCROLL_BG").getPath(), 0, CITY_TITLE_BAR_HEIGHT, int(CITIZEN_BAR_WIDTH * 1.07), int((yResolution - (BOTTOM_CENTER_HUD_HEIGHT + CITY_TITLE_BAR_HEIGHT)) * 1.04), WidgetTypes.WIDGET_GENERAL, -1, -1 )
		self.appendtoHideState(screen, "CityManagerBackground", HIDE_TYPE_CITY, HIDE_LEVEL_ALL)
		# WTP, ray, Center Plot specific Backgrounds - END

	# MAP BACKGROUND SADDLE

		screen.addPanel("InterfaceMapViewSaddleLeft", u"", u"", True, False, 0, yResolution - UPPER_SADDLE_HEIGHT, xResolution - RIGHT_PANEL_WIDTH, SADDLE_HEIGHT * 4 / 7, PanelStyles.PANEL_STYLE_STANDARD, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDrawControl("InterfaceMapViewSaddleLeft", ArtFileMgr.getInterfaceArtInfo("SCREEN_BG_MAP_SADDLE_LEFT").getPath(), 0, yResolution - UPPER_SADDLE_HEIGHT, xResolution, SADDLE_HEIGHT * 4 / 7, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		self.appendtoHideState(screen, "InterfaceMapViewSaddleLeft", HIDE_TYPE_MAP, HIDE_LEVEL_NORMAL)

		screen.addPanel("InterfaceMapViewSaddleRight", u"", u"", True, False, xResolution - RIGHT_PANEL_WIDTH, yResolution - UPPER_SADDLE_HEIGHT, RIGHT_PANEL_WIDTH, SADDLE_HEIGHT * 4 / 7, PanelStyles.PANEL_STYLE_STANDARD, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDrawControl("InterfaceMapViewSaddleRight", ArtFileMgr.getInterfaceArtInfo("SCREEN_BG_MAP_SADDLE_RIGHT").getPath(), xResolution - RIGHT_PANEL_WIDTH, yResolution - UPPER_SADDLE_HEIGHT, RIGHT_PANEL_WIDTH, SADDLE_HEIGHT * 4 / 7, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		self.appendtoHideState(screen, "InterfaceMapViewSaddleRight", HIDE_TYPE_MAP, HIDE_LEVEL_NORMAL)


	# MULITPANEL TABS
		iCumulativeY = (STACK_BAR_HEIGHT * 3 / 2) + (MAP_EDGE_MARGIN_WIDTH / 2)
		RelativeButtonSize = 130

		screen.setImageButton("MapHighlightButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_HIGHLIGHTED_BUTTON").getPath(), xResolution - (MAP_EDGE_MARGIN_WIDTH * 2) - CITY_MULTI_TAB_SIZE - ((CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100) / 2) + (CITY_MULTI_TAB_SIZE / 2), CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2) + iCumulativeY - (CITY_MULTI_TAB_SIZE / 2) - ((CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100) / 2) + (CITY_MULTI_TAB_SIZE / 2), CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100, CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setImageButton("MapToggle", ArtFileMgr.getInterfaceArtInfo("INTERFACE_CITY_MAP_BUTTON").getPath(), xResolution - (MAP_EDGE_MARGIN_WIDTH * 2) - CITY_MULTI_TAB_SIZE, CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2) + iCumulativeY - (CITY_MULTI_TAB_SIZE / 2), CITY_MULTI_TAB_SIZE, CITY_MULTI_TAB_SIZE, WidgetTypes.WIDGET_GENERAL, MAP_MANAGMENT_TOGGLE, -1)
		self.appendtoHideState(screen, "MapToggle", HIDE_TYPE_CITY, HIDE_LEVEL_ALL)
		self.appendtoHideState(screen, "MapHighlightButton", HIDE_TYPE_CITY, HIDE_LEVEL_ALL)
		iCumulativeY += CITY_MULTI_TAB_SIZE
		
		screen.setImageButton("BuildHighlightButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_HIGHLIGHTED_BUTTON").getPath(), xResolution - (MAP_EDGE_MARGIN_WIDTH * 2) - CITY_MULTI_TAB_SIZE - ((CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100) / 2) + (CITY_MULTI_TAB_SIZE / 2), CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2) + iCumulativeY - (CITY_MULTI_TAB_SIZE / 2) - ((CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100) / 2) + (CITY_MULTI_TAB_SIZE / 2), CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100, CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setImageButton("BuildToggle", ArtFileMgr.getInterfaceArtInfo("INTERFACE_CITY_BUILD_BUTTON").getPath(), xResolution - (MAP_EDGE_MARGIN_WIDTH * 2) - CITY_MULTI_TAB_SIZE, CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2) + iCumulativeY - (CITY_MULTI_TAB_SIZE / 2), CITY_MULTI_TAB_SIZE, CITY_MULTI_TAB_SIZE, WidgetTypes.WIDGET_GENERAL, BUILDING_MANAGMENT_TOGGLE, -1)
		self.appendtoHideState(screen, "BuildToggle", HIDE_TYPE_CITY, HIDE_LEVEL_ALL)
		self.appendtoHideState(screen, "BuildHighlightButton", HIDE_TYPE_CITY, HIDE_LEVEL_ALL)
		iCumulativeY += CITY_MULTI_TAB_SIZE
		
		screen.setImageButton("GovernorHighlightButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_HIGHLIGHTED_BUTTON").getPath(), xResolution - (MAP_EDGE_MARGIN_WIDTH * 2) - CITY_MULTI_TAB_SIZE - ((CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100) / 2) + (CITY_MULTI_TAB_SIZE / 2), CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2) + iCumulativeY - (CITY_MULTI_TAB_SIZE / 2) - ((CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100) / 2) + (CITY_MULTI_TAB_SIZE / 2), CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100, CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setImageButton("GovernorToggle", ArtFileMgr.getInterfaceArtInfo("INTERFACE_CITY_GOVENOR_BUTTON").getPath(), xResolution - (MAP_EDGE_MARGIN_WIDTH * 2) - CITY_MULTI_TAB_SIZE, CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2) + iCumulativeY - (CITY_MULTI_TAB_SIZE / 2), CITY_MULTI_TAB_SIZE, CITY_MULTI_TAB_SIZE, WidgetTypes.WIDGET_GENERAL, AUTOMATION_MANAGMENT_TOGGLE, -1)
		self.appendtoHideState(screen, "GovernorToggle", HIDE_TYPE_CITY, HIDE_LEVEL_ALL)
		self.appendtoHideState(screen, "GovernorHighlightButton", HIDE_TYPE_CITY, HIDE_LEVEL_ALL)
		iCumulativeY += CITY_MULTI_TAB_SIZE

	# CITY BUILDING MULTILIST
		self.BOTTOM_BUTTON_ROWS = 3

		screen.addMultiListControlGFC("CityBuildingSelectionMultiList", u"", CITIZEN_BAR_WIDTH + CITY_VIEW_BOX_HEIGHT_AND_WIDTH + (STACK_BAR_HEIGHT / 2) - MAP_EDGE_MARGIN_WIDTH, CITY_TITLE_BAR_HEIGHT + (STACK_BAR_HEIGHT / 2), BUILD_AREA_WIDTH, BUILD_AREA_HEIGHT - STACK_BAR_HEIGHT, self.BOTTOM_BUTTON_ROWS, BOTTOM_BUTTON_SIZE, BOTTOM_BUTTON_SIZE, TableStyles.TABLE_STYLE_STANDARD )
		self.appendtoHideState(screen, "CityBuildingSelectionMultiList", HIDE_TYPE_CITY, HIDE_LEVEL_HIDE)

		screen.addDDSGFC("MapCityLeftPanelShadow", "Art/Interface/Game Hud/shadow_box_2.dds", 0, DOMESTIC_ADVISOR_HEIGHT, MINIMAP_WIDTH, DOMESTIC_ADVISOR_SPACE - LOWER_SADDLE_HEIGHT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.hide("MapCityLeftPanelShadow")
		screen.addDDSGFC("MapCityRightPanelShadow", "Art/Interface/Game Hud/shadow_box_2.dds", xResolution - MINIMAP_WIDTH, DOMESTIC_ADVISOR_HEIGHT, MINIMAP_WIDTH, DOMESTIC_ADVISOR_SPACE - LOWER_SADDLE_HEIGHT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.hide("MapCityRightPanelShadow")
		
	# CITY BUILDING MAP CONSTRUCTION BAR
		screen.addStackedBarGFC("MapCityProductionBar", xResolution - MINIMAP_WIDTH + 3, yResolution - LOWER_SADDLE_HEIGHT - STACK_BAR_HEIGHT * 3 / 2 - 6, MINIMAP_WIDTH - 6, STACK_BAR_HEIGHT * 3 / 2, InfoBarTypes.NUM_INFOBAR_TYPES, WidgetTypes.WIDGET_HELP_SELECTED, 0, -1 )
		screen.setStackedBarColors("MapCityProductionBar", InfoBarTypes.INFOBAR_STORED, gc.getInfoTypeForString("COLOR_GREAT_PEOPLE_STORED") )
		screen.setStackedBarColors("MapCityProductionBar", InfoBarTypes.INFOBAR_RATE, gc.getInfoTypeForString("COLOR_GREAT_PEOPLE_RATE") )
		screen.setStackedBarColors("MapCityProductionBar", InfoBarTypes.INFOBAR_RATE_EXTRA, gc.getInfoTypeForString("COLOR_EMPTY") )
		screen.setStackedBarColors("MapCityProductionBar", InfoBarTypes.INFOBAR_EMPTY, gc.getInfoTypeForString("COLOR_EMPTY") )
		screen.hide("MapCityProductionBar")
	
	# EMPHASIZE TABLE
		screen.addTableControlGFC("EmphasizeTable", 1, CITIZEN_BAR_WIDTH + CITY_VIEW_BOX_HEIGHT_AND_WIDTH + (STACK_BAR_HEIGHT / 2) - MAP_EDGE_MARGIN_WIDTH, CITY_TITLE_BAR_HEIGHT + (STACK_BAR_HEIGHT / 2), BUILD_AREA_WIDTH, BUILD_AREA_HEIGHT - STACK_BAR_HEIGHT, False, False, MEDIUM_BUTTON_SIZE, MEDIUM_BUTTON_SIZE, TableStyles.TABLE_STYLE_STANDARD)
		self.appendtoHideState(screen, "EmphasizeTable", HIDE_TYPE_CITY, HIDE_LEVEL_HIDE)
		screen.setStyle("EmphasizeTable", "Table_EmptyScroll_Style")
		screen.appendTableRow("EmphasizeTable")
		for iYield in EMPHASIZEYIELDS:
			screen.appendTableRow("EmphasizeTable")

	# HELP TEXT AREA
		screen.setHelpTextArea( HELPTEXT_AREA_MAX_WIDTH, FontTypes.SMALL_FONT, HELTTEXT_AREA_X_MARGIN, yResolution - SADDLE_HEIGHT, -0.1, False, "", True, False, CvUtil.FONT_LEFT_JUSTIFY, HELPTEXT_AREA_MIN_WIDTH)

	# ADVISOR BUTTONS

		SpaceAvailable = (xResolution / 2)
		self.ADVISOR_BUTTON_SPACING = SpaceAvailable / 15
		self.ADVISOR_BUTTON_SIZE = SMALL_BUTTON_SIZE * 2
		iBtnX = xResolution / 6 - self.ADVISOR_BUTTON_SPACING

		screen.setImageButton("MainMenuButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_GENERAL_MENU_ICON").getPath(), iBtnX, (TOP_CENTER_HUD_HEIGHT - self.ADVISOR_BUTTON_SIZE) / 2, self.ADVISOR_BUTTON_SIZE, self.ADVISOR_BUTTON_SIZE, WidgetTypes.WIDGET_MENU_ICON, -1, -1 )
		screen.setImageShape("MainMenuButton", ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
		screen.setHitMargins("MainMenuButton", self.ADVISOR_BUTTON_SIZE / 6, self.ADVISOR_BUTTON_SIZE / 6)
		self.appendtoHideState(screen, "MainMenuButton", HIDE_TYPE_MAP, HIDE_LEVEL_HIDE)
		iBtnX += self.ADVISOR_BUTTON_SPACING

		screen.setImageButton("InterfaceHelpButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_GENERAL_CIVILOPEDIA_ICON").getPath(), iBtnX, (TOP_CENTER_HUD_HEIGHT - self.ADVISOR_BUTTON_SIZE) / 2, self.ADVISOR_BUTTON_SIZE, self.ADVISOR_BUTTON_SIZE, WidgetTypes.WIDGET_ACTION, gc.getControlInfo(ControlTypes.CONTROL_CIVILOPEDIA).getActionInfoIndex(), -1 )
		screen.setImageShape("InterfaceHelpButton", ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
		screen.setHitMargins("InterfaceHelpButton", self.ADVISOR_BUTTON_SIZE / 6, self.ADVISOR_BUTTON_SIZE / 6)
		self.appendtoHideState(screen, "InterfaceHelpButton", HIDE_TYPE_MAP, HIDE_LEVEL_HIDE)
		iBtnX += self.ADVISOR_BUTTON_SPACING * 2

		screen.setImageButton("AchieveAdvisorButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_ACHIEVE_BUTTON").getPath(), iBtnX, (TOP_CENTER_HUD_HEIGHT - self.ADVISOR_BUTTON_SIZE) / 2, self.ADVISOR_BUTTON_SIZE, self.ADVISOR_BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, ACHIEVE_ADVISOR_SCREEN_MI, -1 )
		screen.setImageShape("AchieveAdvisorButton", ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
		screen.setHitMargins("AchieveAdvisorButton", self.ADVISOR_BUTTON_SIZE / 6, self.ADVISOR_BUTTON_SIZE / 6)
		self.appendtoHideState(screen, "AchieveAdvisorButton", HIDE_TYPE_MAP, HIDE_LEVEL_HIDE)
		iBtnX += self.ADVISOR_BUTTON_SPACING

		screen.setImageButton("TurnLogButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_GENERAL_TURNLOG_ICON").getPath(), iBtnX, (TOP_CENTER_HUD_HEIGHT - self.ADVISOR_BUTTON_SIZE) / 2, self.ADVISOR_BUTTON_SIZE, self.ADVISOR_BUTTON_SIZE, WidgetTypes.WIDGET_ACTION, gc.getControlInfo(ControlTypes.CONTROL_TURN_LOG).getActionInfoIndex(), -1 )
		screen.setImageShape("TurnLogButton", ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
		screen.setHitMargins("TurnLogButton", self.ADVISOR_BUTTON_SIZE / 6, self.ADVISOR_BUTTON_SIZE / 6)
		self.appendtoHideState(screen, "TurnLogButton", HIDE_TYPE_MAP, HIDE_LEVEL_HIDE)
		iBtnX += self.ADVISOR_BUTTON_SPACING

		screen.setImageButton("CongressAdvisorButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_CONTINENTAL_CONGRESS").getPath(), iBtnX, (TOP_CENTER_HUD_HEIGHT - self.ADVISOR_BUTTON_SIZE) / 2, self.ADVISOR_BUTTON_SIZE, self.ADVISOR_BUTTON_SIZE, WidgetTypes.WIDGET_ACTION, gc.getControlInfo(ControlTypes.CONTROL_CONGRESS_SCREEN).getActionInfoIndex(), -1 )
		screen.setImageShape("CongressAdvisorButton", ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
		screen.setHitMargins("CongressAdvisorButton", self.ADVISOR_BUTTON_SIZE / 6, self.ADVISOR_BUTTON_SIZE / 6)
		self.appendtoHideState(screen, "CongressAdvisorButton", HIDE_TYPE_MAP, HIDE_LEVEL_HIDE)
		iBtnX += self.ADVISOR_BUTTON_SPACING

		screen.setImageButton("InfoAdvisorButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_INFO").getPath(), iBtnX, (TOP_CENTER_HUD_HEIGHT - self.ADVISOR_BUTTON_SIZE) / 2, self.ADVISOR_BUTTON_SIZE, self.ADVISOR_BUTTON_SIZE, WidgetTypes.WIDGET_ACTION, gc.getControlInfo(ControlTypes.CONTROL_INFO).getActionInfoIndex(), -1 )
		screen.setImageShape("InfoAdvisorButton", ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
		screen.setHitMargins("InfoAdvisorButton", self.ADVISOR_BUTTON_SIZE / 6, self.ADVISOR_BUTTON_SIZE / 6)
		self.appendtoHideState(screen, "InfoAdvisorButton", HIDE_TYPE_MAP, HIDE_LEVEL_HIDE)
		iBtnX += self.ADVISOR_BUTTON_SPACING

		screen.setImageButton( "SettingsAdvisorButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_VICTORY_ADVISOR").getPath(), iBtnX, (TOP_CENTER_HUD_HEIGHT - self.ADVISOR_BUTTON_SIZE) / 2, self.ADVISOR_BUTTON_SIZE, self.ADVISOR_BUTTON_SIZE, WidgetTypes.WIDGET_ACTION, gc.getControlInfo(ControlTypes.CONTROL_VICTORY_SCREEN).getActionInfoIndex(), -1 )
		screen.setImageShape("SettingsAdvisorButton", ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
		screen.setHitMargins("SettingsAdvisorButton", self.ADVISOR_BUTTON_SIZE / 6, self.ADVISOR_BUTTON_SIZE / 6)
		self.appendtoHideState(screen, "SettingsAdvisorButton", HIDE_TYPE_MAP, HIDE_LEVEL_HIDE)
		iBtnX += self.ADVISOR_BUTTON_SPACING * 2

		screen.setImageButton("EuropeScreenButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_EUROPE").getPath(), iBtnX, (TOP_CENTER_HUD_HEIGHT - self.ADVISOR_BUTTON_SIZE) / 2, self.ADVISOR_BUTTON_SIZE, self.ADVISOR_BUTTON_SIZE, WidgetTypes.WIDGET_ACTION, gc.getControlInfo(ControlTypes.CONTROL_EUROPE_SCREEN).getActionInfoIndex(), -1 )
		screen.setImageShape("EuropeScreenButton", ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
		screen.setHitMargins("EuropeScreenButton", self.ADVISOR_BUTTON_SIZE / 6, self.ADVISOR_BUTTON_SIZE / 6)
		self.appendtoHideState(screen, "EuropeScreenButton", HIDE_TYPE_MAP, HIDE_LEVEL_HIDE)
		iBtnX += self.ADVISOR_BUTTON_SPACING

		screen.setImageButton("AfricaScreenButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_AFRICA").getPath(), iBtnX, (TOP_CENTER_HUD_HEIGHT - self.ADVISOR_BUTTON_SIZE) / 2, self.ADVISOR_BUTTON_SIZE, self.ADVISOR_BUTTON_SIZE, WidgetTypes.WIDGET_ACTION, gc.getControlInfo(ControlTypes.CONTROL_AFRICA_SCREEN).getActionInfoIndex(), -1 )
		screen.setImageShape("AfricaScreenButton", ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
		screen.setHitMargins("AfricaScreenButton", self.ADVISOR_BUTTON_SIZE / 6, self.ADVISOR_BUTTON_SIZE / 6)
		self.appendtoHideState(screen, "AfricaScreenButton", HIDE_TYPE_MAP, HIDE_LEVEL_HIDE)
		iBtnX += self.ADVISOR_BUTTON_SPACING

		screen.setImageButton("PortRoyalScreenButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_PORT_ROYAL").getPath(), iBtnX, (TOP_CENTER_HUD_HEIGHT - self.ADVISOR_BUTTON_SIZE) / 2, self.ADVISOR_BUTTON_SIZE, self.ADVISOR_BUTTON_SIZE, WidgetTypes.WIDGET_ACTION, gc.getControlInfo(ControlTypes.CONTROL_PORT_ROYAL_SCREEN).getActionInfoIndex(), -1 )
		screen.setImageShape("PortRoyalScreenButton", ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
		screen.setHitMargins("PortRoyalScreenButton", self.ADVISOR_BUTTON_SIZE / 6, self.ADVISOR_BUTTON_SIZE / 6)
		self.appendtoHideState(screen, "PortRoyalScreenButton", HIDE_TYPE_MAP, HIDE_LEVEL_HIDE)
		iBtnX += self.ADVISOR_BUTTON_SPACING

		screen.setImageButton("DiplomacyButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_GENERAL_DIPLOMACY").getPath(), iBtnX, (TOP_CENTER_HUD_HEIGHT - self.ADVISOR_BUTTON_SIZE) / 2, self.ADVISOR_BUTTON_SIZE, self.ADVISOR_BUTTON_SIZE, WidgetTypes.WIDGET_ACTION, gc.getControlInfo(ControlTypes.CONTROL_DIPLOMACY).getActionInfoIndex(), -1 )
		screen.setImageShape("DiplomacyButton", ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
		screen.setHitMargins("DiplomacyButton", self.ADVISOR_BUTTON_SIZE / 6, self.ADVISOR_BUTTON_SIZE / 6)
		self.appendtoHideState(screen, "DiplomacyButton", HIDE_TYPE_MAP, HIDE_LEVEL_HIDE)
		iBtnX += self.ADVISOR_BUTTON_SPACING * 2

		screen.setImageButton("DomesticAdvisorButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_DOMESTIC_ADVISOR").getPath(), iBtnX, (TOP_CENTER_HUD_HEIGHT - self.ADVISOR_BUTTON_SIZE) / 2, self.ADVISOR_BUTTON_SIZE, self.ADVISOR_BUTTON_SIZE, WidgetTypes.WIDGET_ACTION, gc.getControlInfo(ControlTypes.CONTROL_DOMESTIC_SCREEN).getActionInfoIndex(), -1 )
		screen.setImageShape("DomesticAdvisorButton", ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
		screen.setHitMargins("DomesticAdvisorButton", self.ADVISOR_BUTTON_SIZE / 6, self.ADVISOR_BUTTON_SIZE / 6)
		self.appendtoHideState(screen, "DomesticAdvisorButton", HIDE_TYPE_MAP, HIDE_LEVEL_HIDE)
		iBtnX += self.ADVISOR_BUTTON_SPACING

		screen.setImageButton("RevolutionAdvisorButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_REVOLUTION_ADVISOR").getPath(), iBtnX, (TOP_CENTER_HUD_HEIGHT - self.ADVISOR_BUTTON_SIZE) / 2, self.ADVISOR_BUTTON_SIZE, self.ADVISOR_BUTTON_SIZE, WidgetTypes.WIDGET_ACTION, gc.getControlInfo(ControlTypes.CONTROL_REVOLUTION_SCREEN).getActionInfoIndex(), -1 )
		screen.setImageShape("RevolutionAdvisorButton", ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
		screen.setHitMargins("RevolutionAdvisorButton", self.ADVISOR_BUTTON_SIZE / 6, self.ADVISOR_BUTTON_SIZE / 6)
		self.appendtoHideState(screen, "RevolutionAdvisorButton", HIDE_TYPE_MAP, HIDE_LEVEL_HIDE)
		iBtnX += self.ADVISOR_BUTTON_SPACING

		screen.setImageButton("ForeignAdvisorButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_FOREIGN_ADVISOR").getPath(), iBtnX, (TOP_CENTER_HUD_HEIGHT - self.ADVISOR_BUTTON_SIZE) / 2, self.ADVISOR_BUTTON_SIZE, self.ADVISOR_BUTTON_SIZE, WidgetTypes.WIDGET_ACTION, gc.getControlInfo(ControlTypes.CONTROL_FOREIGN_SCREEN).getActionInfoIndex(), -1 )
		screen.setImageShape("ForeignAdvisorButton", ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
		screen.setHitMargins("ForeignAdvisorButton", self.ADVISOR_BUTTON_SIZE / 6, self.ADVISOR_BUTTON_SIZE / 6)
		self.appendtoHideState(screen, "ForeignAdvisorButton", HIDE_TYPE_MAP, HIDE_LEVEL_HIDE)
		iBtnX += self.ADVISOR_BUTTON_SPACING

		screen.setImageButton("MilitaryAdvisorButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_MILITARY_ADVISOR").getPath(), iBtnX, (TOP_CENTER_HUD_HEIGHT - self.ADVISOR_BUTTON_SIZE) / 2, self.ADVISOR_BUTTON_SIZE, self.ADVISOR_BUTTON_SIZE, WidgetTypes.WIDGET_ACTION, gc.getControlInfo(ControlTypes.CONTROL_MILITARY_SCREEN).getActionInfoIndex(), -1 )
		screen.setImageShape("MilitaryAdvisorButton", ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
		screen.setHitMargins("MilitaryAdvisorButton", self.ADVISOR_BUTTON_SIZE / 6, self.ADVISOR_BUTTON_SIZE / 6)
		self.appendtoHideState(screen, "MilitaryAdvisorButton", HIDE_TYPE_MAP, HIDE_LEVEL_HIDE)
		iBtnX += self.ADVISOR_BUTTON_SPACING * 2

		screen.setImageButton( "FatherAdvisorButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_FOUNDING_FATHER").getPath(), iBtnX, (TOP_CENTER_HUD_HEIGHT - self.ADVISOR_BUTTON_SIZE) / 2, self.ADVISOR_BUTTON_SIZE, self.ADVISOR_BUTTON_SIZE, WidgetTypes.WIDGET_ACTION, gc.getControlInfo(ControlTypes.CONTROL_FATHER_SCREEN).getActionInfoIndex(), -1 )
		screen.setImageShape("FatherAdvisorButton", ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
		screen.setHitMargins("FatherAdvisorButton", self.ADVISOR_BUTTON_SIZE / 6, self.ADVISOR_BUTTON_SIZE / 6)
		self.appendtoHideState(screen, "FatherAdvisorButton", HIDE_TYPE_MAP, HIDE_LEVEL_HIDE)
		iBtnX += self.ADVISOR_BUTTON_SPACING

	# MINIMAP RING
		self.MINIMAP_SIDE_MARGIN = MINIMAP_HEIGHT * 13 / 100
		
		screen.addPanel("MiniMapRing", u"", u"", True, False, 0, yResolution - SADDLE_HEIGHT + self.MINIMAP_SIDE_MARGIN, SADDLE_HEIGHT, SADDLE_HEIGHT, PanelStyles.PANEL_STYLE_STANDARD, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDrawControl("MiniMapRing", ArtFileMgr.getInterfaceArtInfo("SCREEN_MINI_MAP_RING").getPath(), 0, yResolution - SADDLE_HEIGHT + self.MINIMAP_SIDE_MARGIN, SADDLE_HEIGHT, SADDLE_HEIGHT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		self.appendtoHideState(screen, "MiniMapRing", HIDE_TYPE_MAP, HIDE_LEVEL_HIDE)
		self.appendtoHideState(screen, "MiniMapRing", HIDE_TYPE_GLOBAL, HIDE_LEVEL_HIDE)

	# MINIMAP INITIALIZATION
		screen.initMinimap(self.MINIMAP_SIDE_MARGIN, MINIMAP_HEIGHT - self.MINIMAP_SIDE_MARGIN, yResolution - MINIMAP_HEIGHT + self.MINIMAP_SIDE_MARGIN, yResolution - self.MINIMAP_SIDE_MARGIN, -0.1, true )
		self.appendtoHideState(screen, "_FXS_Screen_Bogus_Minimap_Name", HIDE_TYPE_MAP, HIDE_LEVEL_ALL)
		self.appendtoHideState(screen, "_FXS_Screen_Bogus_Minimap_Name", HIDE_TYPE_CITY, HIDE_LEVEL_ALL)
		self.appendtoHideState(screen, "_FXS_Screen_Bogus_Minimap_Name", HIDE_TYPE_GLOBAL, HIDE_LEVEL_ALL)

	# UNIT HEALTH BAR
		screen.addStackedBarGFC("UnitHealthBar", xResolution - UNIT_HEALTH_BAR_WIDTH - 10, yResolution - LOWER_RIGHT_CORNER_BACKGROUND_HEIGHT - STACK_BAR_HEIGHT, UNIT_HEALTH_BAR_WIDTH, STACK_BAR_HEIGHT, InfoBarTypes.NUM_INFOBAR_TYPES, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setStackedBarColors("UnitHealthBar", InfoBarTypes.INFOBAR_STORED, gc.getInfoTypeForString("COLOR_CULTURE_STORED") )
		screen.setStackedBarColors("UnitHealthBar", InfoBarTypes.INFOBAR_RATE, gc.getInfoTypeForString("COLOR_CULTURE_RATE") )
		screen.setStackedBarColors("UnitHealthBar", InfoBarTypes.INFOBAR_RATE_EXTRA, gc.getInfoTypeForString("COLOR_EMPTY") )
		screen.setStackedBarColors("UnitHealthBar", InfoBarTypes.INFOBAR_EMPTY, gc.getInfoTypeForString("COLOR_EMPTY") )
		screen.hide("UnitHealthBar")

	# MINIMAP BUTTONS
		self.MINIMAP_BUTTON_SPACING = MEDIUM_BUTTON_SIZE
		self.MINIMAP_BUTTON_LEFT_OFFSET = 49

		self.ANGLE_SEPERATION = 90 / 4
		iAngle = 0
		iRadius = SADDLE_HEIGHT * 0.43
		
		self.MINI_MAP_BUTTON_SIZE = SMALL_BUTTON_SIZE * 15 / 8

		self.ButtonList = ["INTERFACE_MAP_UNIT", "INTERFACE_MAP_GRID_TOGGLE", "INTERFACE_MAP_SHOW_WORLD", "INTERFACE_MAP_INFO", "INTERFACE_MAP_ZOOM", "INTERFACE_MAP_YIELD", "INTERFACE_MAP_LIST"]
		self.ControlList = [ControlTypes.CONTROL_UNIT_ICONS, ControlTypes.CONTROL_GRID, ControlTypes.CONTROL_BARE_MAP, ControlTypes.CONTROL_RESOURCE_ALL, ControlTypes.CONTROL_GLOBELAYER, ControlTypes.CONTROL_YIELDS, ControlTypes.CONTROL_SCORES] 
		self.ButtonNames = ["UnitIcons", "Grid", "BareMap", "ResourceIcons", "GlobeToggle", "Yields", "ScoresVisible"]
		
		for iButtonIndex in range(len(self.ButtonList)):
			Xcord = -math.sin(math.radians(self.ANGLE_SEPERATION * iButtonIndex + 45)) * int(iRadius) + int(SADDLE_HEIGHT / 2)
			Ycord = math.cos(math.radians(self.ANGLE_SEPERATION * iButtonIndex + 45)) * int(iRadius) - int(SADDLE_HEIGHT / 2) + yResolution + 3 + self.MINIMAP_SIDE_MARGIN
			
			szName = self.ButtonNames[iButtonIndex]
			screen.setImageButton(szName, ArtFileMgr.getInterfaceArtInfo(self.ButtonList[iButtonIndex]).getPath(), int(Xcord) - (self.MINI_MAP_BUTTON_SIZE / 2), int(Ycord) - (self.MINI_MAP_BUTTON_SIZE / 2), self.MINI_MAP_BUTTON_SIZE, self.MINI_MAP_BUTTON_SIZE, WidgetTypes.WIDGET_ACTION, gc.getControlInfo(self.ControlList[iButtonIndex]).getActionInfoIndex(), -1)
			screen.setImageShape(szName, ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
			screen.setHitMargins(szName, 5, 5)
			self.appendtoHideState(screen, szName, HIDE_TYPE_GLOBAL, HIDE_LEVEL_ALL)
			self.appendtoHideState(screen, szName, HIDE_TYPE_MAP, HIDE_LEVEL_HIDE)

	# GLOBAL VIEW BUTTONS
		for i in range (CyGlobeLayerManager().getNumLayers()):
			szButtonID = "GlobeLayer" + str(i)
			szStyle = ArtFileMgr.getInterfaceArtInfo(CyGlobeLayerManager().getLayer(i).getButtonStyle()).getPath()
			
			Xcord = -math.sin(math.radians(self.ANGLE_SEPERATION * i + 45)) * int(iRadius) + int(SADDLE_HEIGHT / 2)
			Ycord = math.cos(math.radians(self.ANGLE_SEPERATION * i + 45)) * int(iRadius) - int(SADDLE_HEIGHT / 2) + yResolution + 3 + self.MINIMAP_SIDE_MARGIN
			
			screen.setImageButton( szButtonID, szStyle, int(Xcord) - (self.MINI_MAP_BUTTON_SIZE / 2), int(Ycord) - (self.MINI_MAP_BUTTON_SIZE / 2), self.MINI_MAP_BUTTON_SIZE, self.MINI_MAP_BUTTON_SIZE, WidgetTypes.WIDGET_GLOBELAYER, i, -1)
			screen.setImageShape(szButtonID, ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
			screen.setHitMargins(szButtonID, 5, 5)
			screen.hide( szButtonID )
			
	# GOLD PILE
		screen.addDDSGFC("GoldPile", ArtFileMgr.getInterfaceArtInfo("SCREEN_GOLD_PILE").getPath(), 60, 8, 50, 50, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setImageShape("GoldPile", ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
		screen.setHitMargins("GoldPile", 30, 30)
		self.appendtoHideState(screen, "GoldPile", HIDE_TYPE_MAP, HIDE_LEVEL_HIDE)
		
		# desync log writing button
		# TODO figure out how to make it appear as needed instead of always or never
		if (0):
			screen.setImageButton("DesyncButton", "pink", 100, 100, 100, 100, WidgetTypes.WIDGET_NETWORK_DESYNC, -1, -1)
		
	# AUTOMATE PRODUCTION & CITIZEN BUTTON
		screen.addCheckBoxGFC("AutomateProduction", ArtFileMgr.getInterfaceArtInfo("INTERFACE_CITY_AUTOMATE_PRODUCTION").getPath(), ArtFileMgr.getInterfaceArtInfo("INTERFACE_HIGHLIGHTED_BUTTON").getPath(), CITIZEN_BAR_WIDTH + ((xResolution - CITIZEN_BAR_WIDTH) * 3 / 7) , yResolution - BOTTOM_CENTER_HUD_HEIGHT - TRANSPORT_AREA_HEIGHT - (STACK_BAR_HEIGHT * 2) - (SMALL_BUTTON_SIZE * 3 / 2), SMALL_BUTTON_SIZE * 2, SMALL_BUTTON_SIZE * 2, WidgetTypes.WIDGET_AUTOMATE_PRODUCTION, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL )
		self.appendtoHideState(screen, "AutomateProduction", HIDE_TYPE_CITY, HIDE_LEVEL_HIDE)

		screen.setImageButton("AutomateCitizens", ArtFileMgr.getInterfaceArtInfo("INTERFACE_CITY_AUTOMATE_CITIZENS").getPath(), CITIZEN_BAR_WIDTH - (SMALL_BUTTON_SIZE * 3), yResolution - BOTTOM_CENTER_HUD_HEIGHT - (STACK_BAR_HEIGHT * 2) - SMALL_BUTTON_SIZE, SMALL_BUTTON_SIZE * 2, SMALL_BUTTON_SIZE * 2, WidgetTypes.WIDGET_AUTOMATE_CITIZENS, 1, -1)
		self.appendtoHideState(screen, "AutomateCitizens", HIDE_TYPE_CITY, HIDE_LEVEL_HIDE)

		screen.setImageButton("LockCitizens", ArtFileMgr.getInterfaceArtInfo("INTERFACE_CITY_CITIZEN_LOCK").getPath(), CITIZEN_BAR_WIDTH - (SMALL_BUTTON_SIZE * 5), yResolution - BOTTOM_CENTER_HUD_HEIGHT - (STACK_BAR_HEIGHT * 2) - SMALL_BUTTON_SIZE, SMALL_BUTTON_SIZE * 2, SMALL_BUTTON_SIZE * 2, WidgetTypes.WIDGET_AUTOMATE_CITIZENS, 0, -1)
		self.appendtoHideState(screen, "LockCitizens", HIDE_TYPE_CITY, HIDE_LEVEL_HIDE)

	# RESOURCE TABLE
		screen.addScrollPanel("ResourceTable", u"", 0, yResolution - BOTTOM_CENTER_HUD_HEIGHT, xResolution, BOTTOM_CENTER_HUD_HEIGHT, PanelStyles.PANEL_STYLE_STANDARD, false, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.hide("ResourceTable")
		
		self.RESOURCE_LABELS = ["", ""]
		self.RESOURCE_LABELS[0] = localText.getText("TXT_KEY_SCREEN_STORED", ())
		self.RESOURCE_LABELS[1] = localText.getText("TXT_KEY_SCREEN_NET", ())
		
		global RESOURCE_TABLE_COLUMN_WIDTH
		RESOURCE_TABLE_COLUMN_WIDTH = int((xResolution) / len(self.TableYields))
		global RESOURCE_TABLE_MARGIN
		RESOURCE_TABLE_MARGIN = int((xResolution - RESOURCE_TABLE_COLUMN_WIDTH * len(self.TableYields)) / 2)
		ArtPath = ""
		xDelta = 0
		ixDelta = 0
		iDelta = 0
		iK1 = 1
		iK2 = 1
		iK3 = 1
		iMax = (xResolution * 2) / (yResolution * 33)
		iNum = len(self.TableYields)
		iY = yResolution - BOTTOM_CENTER_HUD_HEIGHT / iK1
		if iNum > iMax:
			if iNum % 3 == 1:
				iNum += 2
			if iNum % 3 == 2:
				iNum += 1
			iNum /= 3
			RESOURCE_TABLE_COLUMN_WIDTH = (xResolution - STACK_BAR_HEIGHT) / iNum
			iK2 = 3
			iK3 = 7
			if len(self.TableYields) % 3 == 1:
				ixDelta = RESOURCE_TABLE_COLUMN_WIDTH / 2
		self.iIconWidth = RESOURCE_TABLE_COLUMN_WIDTH
		if iK2 == 3:
			iMax = yResolution / 20
			if RESOURCE_TABLE_COLUMN_WIDTH > iMax:
				self.iIconWidth = iMax
		for iYield in self.TableYields:
			szName = "BonusPane" + str(iYield)

			# WTP ray
			if iYield >= 2 * iNum:
				xDelta = ixDelta
				iDelta = iNum * 2
				iK1 = 3
				iY = yResolution - BOTTOM_CENTER_HUD_HEIGHT / 3

			elif iYield >= iNum:
				xDelta = ixDelta
				iDelta = iNum
				iK1 = 2
				iY = yResolution - (2 * BOTTOM_CENTER_HUD_HEIGHT / 3)
			#screen.attachPanelAt("ResourceTable", szName, u"", u"", True, True, PanelStyles.PANEL_STYLE_STANDARD, STACK_BAR_HEIGHT + (iYield * RESOURCE_TABLE_COLUMN_WIDTH), yResolution - BOTTOM_CENTER_HUD_HEIGHT, RESOURCE_TABLE_COLUMN_WIDTH, BOTTOM_CENTER_HUD_HEIGHT - STACK_BAR_HEIGHT, WidgetTypes.WIDGET_RECEIVE_MOVE_CARGO_TO_CITY, -1, -1 )
			iX = STACK_BAR_HEIGHT + ((iYield - iDelta) * RESOURCE_TABLE_COLUMN_WIDTH) + xDelta
			# WTP, ray adjustment, 3 rows
			screen.attachPanelAt("ResourceTable", szName, u"", u"", True, True, PanelStyles.PANEL_STYLE_STANDARD, iX, iY, RESOURCE_TABLE_COLUMN_WIDTH, (BOTTOM_CENTER_HUD_HEIGHT - STACK_BAR_HEIGHT) / iK2, WidgetTypes.WIDGET_RECEIVE_MOVE_CARGO_TO_CITY, -1, -1 )

			if (iYield - iDelta) == 0:
				ArtPath = ArtFileMgr.getInterfaceArtInfo("INTERFACE_BOX_START").getPath()
			elif (iYield - iDelta) == (len(self.TableYields) / 3 - 1): # WTP, ray, adjustments
				ArtPath = ArtFileMgr.getInterfaceArtInfo("INTERFACE_BOX_END").getPath()
			elif (iYield - iDelta) == ((len(self.TableYields) / 3) * 2 - 1): # WTP, ray, adjustments
				ArtPath = ArtFileMgr.getInterfaceArtInfo("INTERFACE_BOX_END").getPath()
			else:
				ArtPath = ArtFileMgr.getInterfaceArtInfo("INTERFACE_BOX_MID").getPath()
			iX = STACK_BAR_HEIGHT + ((iYield - iDelta) * RESOURCE_TABLE_COLUMN_WIDTH) + xDelta
			screen.addDrawControl(szName, ArtPath, iX, iY, RESOURCE_TABLE_COLUMN_WIDTH, (BOTTOM_CENTER_HUD_HEIGHT - (STACK_BAR_HEIGHT * 3 / 2)) * iK2 / iK3 + 2, WidgetTypes.WIDGET_RECEIVE_MOVE_CARGO_TO_CITY, -1, -1)
			self.appendtoHideState(screen, szName, HIDE_TYPE_CITY, HIDE_LEVEL_NORMAL)

			szName = "BonusPanePos" + str(iYield)
			iX = STACK_BAR_HEIGHT + ((iYield - iDelta) * RESOURCE_TABLE_COLUMN_WIDTH) + xDelta
			screen.attachPanelAt("ResourceTable", szName, u"", u"", True, True, PanelStyles.PANEL_STYLE_STANDARD, iX, iY, RESOURCE_TABLE_COLUMN_WIDTH, (BOTTOM_CENTER_HUD_HEIGHT - STACK_BAR_HEIGHT) / iK2, WidgetTypes.WIDGET_RECEIVE_MOVE_CARGO_TO_CITY, -1, -1 )

			if (iYield - iDelta) == 0:
				ArtPath = ArtFileMgr.getInterfaceArtInfo("INTERFACE_BOX_POS_START").getPath()
			elif (iYield - iDelta) == (len(self.TableYields) - 1):
				ArtPath = ArtFileMgr.getInterfaceArtInfo("INTERFACE_BOX_POS_END").getPath()
			else:
				ArtPath = ArtFileMgr.getInterfaceArtInfo("INTERFACE_BOX_POS_MID").getPath()

			iX = STACK_BAR_HEIGHT + ((iYield - iDelta) * RESOURCE_TABLE_COLUMN_WIDTH) + xDelta
			screen.addDrawControl(szName, ArtPath, iX, iY, RESOURCE_TABLE_COLUMN_WIDTH, (BOTTOM_CENTER_HUD_HEIGHT - (STACK_BAR_HEIGHT * 3 / 2)) * iK2 / iK3 + 2, WidgetTypes.WIDGET_RECEIVE_MOVE_CARGO_TO_CITY, -1, -1 )
			self.appendtoHideState(screen, szName, HIDE_TYPE_CITY, HIDE_LEVEL_NORMAL)
			
			szName = "BonusPaneNeg" + str(iYield)
			iX = STACK_BAR_HEIGHT + ((iYield - iDelta) * RESOURCE_TABLE_COLUMN_WIDTH) + xDelta
			screen.attachPanelAt("ResourceTable", szName, u"", u"", True, True, PanelStyles.PANEL_STYLE_STANDARD, iX, iY, RESOURCE_TABLE_COLUMN_WIDTH, (BOTTOM_CENTER_HUD_HEIGHT - STACK_BAR_HEIGHT) / iK2, WidgetTypes.WIDGET_RECEIVE_MOVE_CARGO_TO_CITY, -1, -1 )

			if (iYield - iDelta) == 0:
				ArtPath = ArtFileMgr.getInterfaceArtInfo("INTERFACE_BOX_NEG_START").getPath()
			elif (iYield - iDelta) == (len(self.TableYields) - 1):
				ArtPath = ArtFileMgr.getInterfaceArtInfo("INTERFACE_BOX_NEG_END").getPath()
			else:
				ArtPath = ArtFileMgr.getInterfaceArtInfo("INTERFACE_BOX_NEG_MID").getPath()

			iX = STACK_BAR_HEIGHT + ((iYield - iDelta) * RESOURCE_TABLE_COLUMN_WIDTH) + xDelta
			screen.addDrawControl(szName, ArtPath, iX, iY, RESOURCE_TABLE_COLUMN_WIDTH, (BOTTOM_CENTER_HUD_HEIGHT - (STACK_BAR_HEIGHT * 3 / 2)) * iK2 / iK3 + 2, WidgetTypes.WIDGET_RECEIVE_MOVE_CARGO_TO_CITY, -1, -1 )
			self.appendtoHideState(screen, szName, HIDE_TYPE_CITY, HIDE_LEVEL_NORMAL)
			iX = STACK_BAR_HEIGHT + ((iYield - iDelta) * RESOURCE_TABLE_COLUMN_WIDTH) + (RESOURCE_TABLE_COLUMN_WIDTH / 2) + xDelta

			iZ = yResolution - (STACK_BAR_HEIGHT * 3) / iK1
			screen.setLabel("YieldStoredlabel" + str(iYield), "ResourceTable", "", CvUtil.FONT_CENTER_JUSTIFY, iX, iZ, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_RECEIVE_MOVE_CARGO_TO_CITY, -1, -1 )
			iZ = yResolution - (STACK_BAR_HEIGHT * 2) / iK1
			screen.setLabel("YieldNetlabel" + str(iYield), "ResourceTable", "", CvUtil.FONT_CENTER_JUSTIFY, iX, iZ, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_RECEIVE_MOVE_CARGO_TO_CITY, -1, -1 )

			iX = STACK_BAR_HEIGHT + ((iYield - iDelta) * RESOURCE_TABLE_COLUMN_WIDTH) + (RESOURCE_TABLE_COLUMN_WIDTH - self.iIconWidth) / 2 + xDelta

			screen.addDragableButton("YieldIcon" + str(iYield), gc.getYieldInfo(iYield).getIcon(), "", iX, iY, self.iIconWidth, self.iIconWidth, WidgetTypes.WIDGET_MOVE_CARGO_TO_TRANSPORT, iYield, -1, ButtonStyles.BUTTON_STYLE_IMAGE )
			self.appendtoHideState(screen, "YieldIcon" + str(iYield), HIDE_TYPE_CITY, HIDE_LEVEL_NORMAL)

		screen.addScrollPanel("CityReciveCargo", u"", 0, yResolution - BOTTOM_CENTER_HUD_HEIGHT, xResolution, BOTTOM_CENTER_HUD_HEIGHT, PanelStyles.PANEL_STYLE_MAIN, false, WidgetTypes.WIDGET_RECEIVE_MOVE_CARGO_TO_CITY, -1, -1 )
		self.appendtoHideState(screen, "CityReciveCargo", HIDE_TYPE_CITY, HIDE_LEVEL_NORMAL)

		for iYield in self.TableYields:
			screen.moveToFront("YieldIcon" + str(iYield))

	# MAP YIELD EMPHASIZE
		BOX_WIDTH = xResolution / len(EMPHASIZEYIELDS)
		BOX_HEIGHT = BOX_WIDTH
		xLocation = (xResolution - BOX_WIDTH * len(EMPHASIZEYIELDS)) / 2
		yLocation = yResolution - SADDLE_HEIGHT * 13 / 100 - BOX_HEIGHT / 2
		
		for iYield in EMPHASIZEYIELDS:
			screen.addDDSGFC("MapYieldBox" + str(iYield), ArtFileMgr.getInterfaceArtInfo("INTERFACE_EUROPE_BOX_PRICE").getPath(), xLocation, yLocation, BOX_WIDTH, BOX_HEIGHT, WidgetTypes.WIDGET_GENERAL, iYield, -1 )
			screen.hide("MapYieldBox" + str(iYield))
			screen.addDDSGFC("MapYieldEmphasize" + str(iYield), "Art/Interface/Game Hud/box_green.dds", xLocation, yLocation, BOX_WIDTH, BOX_HEIGHT, WidgetTypes.WIDGET_GENERAL, iYield, -1 )
			screen.hide("MapYieldEmphasize" + str(iYield))
			screen.addDDSGFC("MapYieldDe-Emphasize" + str(iYield), "Art/Interface/Game Hud/box_red.dds", xLocation, yLocation, BOX_WIDTH, BOX_HEIGHT, WidgetTypes.WIDGET_GENERAL, iYield, -1 )
			screen.hide("MapYieldDe-Emphasize" + str(iYield))
			screen.setImageButton("MapYieldIcon" + str(iYield), gc.getYieldInfo(iYield).getIcon(), xLocation + BOX_WIDTH / 14, yLocation + BOX_HEIGHT / 14, BOX_WIDTH * 6 / 7, BOX_WIDTH * 6 / 7, WidgetTypes.WIDGET_EMPHASIZE, iYield, -1)
			screen.hide("MapYieldIcon" + str(iYield))
			
			xLocation += BOX_WIDTH
	
	# YIELD LOZENGE
		screen.addDDSGFC("BonusPaneNeg" + str(int(YieldTypes.YIELD_BELLS)), ArtFileMgr.getInterfaceArtInfo("INTERFACE_DEEMPHASIZE_LOZENGE").getPath(), (xResolution * 30 / 100) - STACK_BAR_HEIGHT, STACK_BAR_HEIGHT / 3, STACK_BAR_HEIGHT * 2, STACK_BAR_HEIGHT , WidgetTypes.WIDGET_GENERAL, -1, -1)
		self.appendtoHideState(screen, "BonusPaneNeg" + str(int(YieldTypes.YIELD_BELLS)), HIDE_TYPE_CITY, HIDE_LEVEL_NORMAL)
		screen.addDDSGFC("BonusPaneNeg" + str(int(YieldTypes.YIELD_CROSSES)), ArtFileMgr.getInterfaceArtInfo("INTERFACE_DEEMPHASIZE_LOZENGE").getPath(), (xResolution * 70 / 100) - STACK_BAR_HEIGHT, STACK_BAR_HEIGHT / 3, STACK_BAR_HEIGHT * 2, STACK_BAR_HEIGHT , WidgetTypes.WIDGET_GENERAL, -1, -1)
		self.appendtoHideState(screen, "BonusPaneNeg" + str(int(YieldTypes.YIELD_CROSSES)), HIDE_TYPE_CITY, HIDE_LEVEL_NORMAL)
		screen.addDDSGFC("BonusPaneNeg" + str(int(YieldTypes.YIELD_HAMMERS)), ArtFileMgr.getInterfaceArtInfo("INTERFACE_DEEMPHASIZE_LOZENGE").getPath(), (xResolution * 25 / 100) - STACK_BAR_HEIGHT, STACK_BAR_HEIGHT / 3, STACK_BAR_HEIGHT * 2, STACK_BAR_HEIGHT , WidgetTypes.WIDGET_GENERAL, -1, -1)
		self.appendtoHideState(screen, "BonusPaneNeg" + str(int(YieldTypes.YIELD_HAMMERS)), HIDE_TYPE_CITY, HIDE_LEVEL_NORMAL)
		screen.addDDSGFC("BonusPaneNeg" + str(int(YieldTypes.YIELD_EDUCATION)), ArtFileMgr.getInterfaceArtInfo("INTERFACE_DEEMPHASIZE_LOZENGE").getPath(), (xResolution * 75 / 100) - STACK_BAR_HEIGHT, STACK_BAR_HEIGHT / 3, STACK_BAR_HEIGHT * 2, STACK_BAR_HEIGHT , WidgetTypes.WIDGET_GENERAL, -1, -1)
		self.appendtoHideState(screen, "BonusPaneNeg" + str(int(YieldTypes.YIELD_EDUCATION)), HIDE_TYPE_CITY, HIDE_LEVEL_NORMAL)

		screen.addDDSGFC("BonusPanePos" + str(int(YieldTypes.YIELD_BELLS)), ArtFileMgr.getInterfaceArtInfo("INTERFACE_EMPHASIZE_LOZENGE").getPath(), (xResolution * 30 / 100) - STACK_BAR_HEIGHT, STACK_BAR_HEIGHT / 3, STACK_BAR_HEIGHT * 2, STACK_BAR_HEIGHT , WidgetTypes.WIDGET_GENERAL, -1, -1)
		self.appendtoHideState(screen, "BonusPanePos" + str(int(YieldTypes.YIELD_BELLS)), HIDE_TYPE_CITY, HIDE_LEVEL_NORMAL)
		screen.addDDSGFC("BonusPanePos" + str(int(YieldTypes.YIELD_CROSSES)), ArtFileMgr.getInterfaceArtInfo("INTERFACE_EMPHASIZE_LOZENGE").getPath(), (xResolution * 70 / 100) - STACK_BAR_HEIGHT, STACK_BAR_HEIGHT / 3, STACK_BAR_HEIGHT * 2, STACK_BAR_HEIGHT , WidgetTypes.WIDGET_GENERAL, -1, -1)
		self.appendtoHideState(screen, "BonusPanePos" + str(int(YieldTypes.YIELD_CROSSES)), HIDE_TYPE_CITY, HIDE_LEVEL_NORMAL)
		screen.addDDSGFC("BonusPanePos" + str(int(YieldTypes.YIELD_HAMMERS)), ArtFileMgr.getInterfaceArtInfo("INTERFACE_EMPHASIZE_LOZENGE").getPath(), (xResolution * 25 / 100) - STACK_BAR_HEIGHT, STACK_BAR_HEIGHT / 3, STACK_BAR_HEIGHT * 2, STACK_BAR_HEIGHT , WidgetTypes.WIDGET_GENERAL, -1, -1)
		self.appendtoHideState(screen, "BonusPanePos" + str(int(YieldTypes.YIELD_HAMMERS)), HIDE_TYPE_CITY, HIDE_LEVEL_NORMAL)
		screen.addDDSGFC("BonusPanePos" + str(int(YieldTypes.YIELD_EDUCATION)), ArtFileMgr.getInterfaceArtInfo("INTERFACE_EMPHASIZE_LOZENGE").getPath(), (xResolution * 75 / 100) - STACK_BAR_HEIGHT, STACK_BAR_HEIGHT / 3, STACK_BAR_HEIGHT * 2, STACK_BAR_HEIGHT , WidgetTypes.WIDGET_GENERAL, -1, -1)
		self.appendtoHideState(screen, "BonusPanePos" + str(int(YieldTypes.YIELD_EDUCATION)), HIDE_TYPE_CITY, HIDE_LEVEL_NORMAL)
		
	# MAP PLOT LIST BUTTONS
		self.END_TURN_TEXT_BOTTOM_MARGIN = 55
		self.PLOT_LIST_BUTTON_SPACER = 6
		self.PLOT_LIST_HEALTH_BAR_SHORTENING_CORRECTION = 6
		self.PLOT_LIST_HEALTH_BAR_HEIGHT = 11
		self.PLOT_LIST_ICON_SIZE = 12
		
		self.PLOT_LIST_BUTTON_AREA_MARGIN = 10
		Width = xResolution - (SADDLE_HEIGHT + RIGHT_PANEL_WIDTH)
		Height = LARGE_BUTTON_SIZE + self.PLOT_LIST_BUTTON_SPACER
		global NUM_PLOT_LIST_BUTTONS
		NUM_PLOT_LIST_BUTTONS = int(Width / Height) 
		iMargin = (Width - NUM_PLOT_LIST_BUTTONS * Height) / 2
		
		for j in range(gc.getMAX_PLOT_LIST_ROWS()):
			iRow = gc.getMAX_PLOT_LIST_ROWS() - (j + 1)
			yPixel = yResolution - LOWER_SADDLE_HEIGHT - (LARGE_BUTTON_SIZE + 4) - iRow * (LARGE_BUTTON_SIZE + self.PLOT_LIST_BUTTON_SPACER)
			xPixel = SADDLE_HEIGHT + iMargin
			szStringPanel = "PlotListPanelMulti" + str(j)
			screen.addPanel(szStringPanel, u"", u"", True, False, xPixel, yPixel, Width, Height, PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)
			self.appendtoHideState(screen, "PlotListPanelMulti", HIDE_TYPE_MAP, HIDE_LEVEL_ALL)

			for i in range(self.numPlotListButtons()):
				k = j * self.numPlotListButtons() + i
				xOffset = i * (LARGE_BUTTON_SIZE + self.PLOT_LIST_BUTTON_SPACER)

				szString = "PlotListButtonMulti" + str(k)
				screen.addCheckBoxGFCAt(szStringPanel, szString, ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_GOVERNOR").getPath(), ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), xOffset, 0, LARGE_BUTTON_SIZE, LARGE_BUTTON_SIZE, WidgetTypes.WIDGET_PLOT_LIST, k, -1, ButtonStyles.BUTTON_STYLE_LABEL )
				screen.hide( szString )

				szStringHealth = szString + "Health"
				screen.addStackedBarGFCAt( szStringHealth, szStringPanel, xOffset, LARGE_BUTTON_SIZE - self.PLOT_LIST_HEALTH_BAR_SHORTENING_CORRECTION, LARGE_BUTTON_SIZE, self.PLOT_LIST_HEALTH_BAR_HEIGHT, InfoBarTypes.NUM_INFOBAR_TYPES, WidgetTypes.WIDGET_GENERAL, k, -1 )
				screen.hide( szStringHealth )

				szStringIcon = szString + "Icon"
				szFileName = ArtFileMgr.getInterfaceArtInfo("OVERLAY_MOVE").getPath()
				screen.addDDSGFCAt( szStringIcon, szStringPanel, szFileName, xOffset, 0, self.PLOT_LIST_ICON_SIZE, self.PLOT_LIST_ICON_SIZE, WidgetTypes.WIDGET_PLOT_LIST, k, -1, False )
				screen.hide( szStringIcon )

	# ENDTURN TEXT & BUTTON
		screen.setLabel("EndTurnText", "Background", u"", CvUtil.FONT_CENTER_JUSTIFY, 0, yResolution - LOWER_SADDLE_HEIGHT - 12, -0.1, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setHitTest("EndTurnText", HitTestTypes.HITTEST_NOHIT )

		self.CENTER_POINT_HEIGHT = 1.45
		self.CENTER_HUD_HEIGHT = 0.095

		iRadius = SADDLE_HEIGHT * 0.43
		Xcord = math.sin(math.radians(135)) * int(iRadius) + int(SADDLE_HEIGHT / 2)
		Ycord = math.cos(math.radians(135)) * int(iRadius) - int(SADDLE_HEIGHT / 2) + yResolution + self.MINIMAP_SIDE_MARGIN

		screen.addPanel("EndTurnRing", u"", u"", True, False, int(Xcord) - LARGE_BUTTON_SIZE * 3 / 4, int(Ycord) - LARGE_BUTTON_SIZE * 3 / 4, LARGE_BUTTON_SIZE * 3 / 2, LARGE_BUTTON_SIZE * 3 / 2, PanelStyles.PANEL_STYLE_STANDARD, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addDrawControl("EndTurnRing", ArtFileMgr.getInterfaceArtInfo("SCREEN_END_TURN_RING").getPath(), int(Xcord) - LARGE_BUTTON_SIZE, int(Ycord) - LARGE_BUTTON_SIZE, LARGE_BUTTON_SIZE * 2, LARGE_BUTTON_SIZE * 2, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setHitTest("EndTurnRing", HitTestTypes.HITTEST_NOHIT)
		self.appendtoHideState(screen, "EndTurnRing", HIDE_TYPE_MAP, HIDE_LEVEL_HIDE)
		self.appendtoHideState(screen, "EndTurnRing", HIDE_TYPE_GLOBAL, HIDE_LEVEL_HIDE)

		screen.setImageButton("EndTurnButton", "", int(Xcord) - LARGE_BUTTON_SIZE, int(Ycord) - LARGE_BUTTON_SIZE, LARGE_BUTTON_SIZE * 2, LARGE_BUTTON_SIZE * 2, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setHitTest("EndTurnButton", HitTestTypes.HITTEST_NOHIT)
		screen.setStyle("EndTurnButton", "Button_HUDEndTurn_Style")
		screen.setEndTurnState("EndTurnButton", "Red")
		screen.hide("EndTurnButton")
		
		screen.setImageButton("EndTurnLayer", "", int(Xcord) - LARGE_BUTTON_SIZE, int(Ycord) - LARGE_BUTTON_SIZE, LARGE_BUTTON_SIZE * 2, LARGE_BUTTON_SIZE * 2, WidgetTypes.WIDGET_END_TURN, -1, -1 )
		screen.setImageShape("EndTurnLayer", ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
		screen.setHitMargins("EndTurnLayer", LARGE_BUTTON_SIZE / 3, LARGE_BUTTON_SIZE / 3)
		screen.hide("EndTurnLayer")
		

	# BUILDING CONSTRUCTION BAR
		screen.addStackedBarGFC("CityProductionBar", CITIZEN_BAR_WIDTH - 2 + CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100 - SMALL_BUTTON_SIZE / 4, CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2) + (STACK_BAR_HEIGHT * 3 / 2) + (MAP_EDGE_MARGIN_WIDTH / 2) - (CITY_MULTI_TAB_SIZE / 2) - ((CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100) / 2) + (CITY_MULTI_TAB_SIZE / 2) - (STACK_BAR_HEIGHT / 2) + CITY_MULTI_TAB_SIZE * RelativeButtonSize / 200, xResolution - (MAP_EDGE_MARGIN_WIDTH * 2) - CITY_MULTI_TAB_SIZE - ((CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100) / 2) + (CITY_MULTI_TAB_SIZE / 2) - CITIZEN_BAR_WIDTH - CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100 + SMALL_BUTTON_SIZE / 2 + 4, STACK_BAR_HEIGHT, InfoBarTypes.NUM_INFOBAR_TYPES, WidgetTypes.WIDGET_HELP_SELECTED, 0, -1 )
		screen.setStackedBarColors("CityProductionBar", InfoBarTypes.INFOBAR_STORED, gc.getInfoTypeForString("COLOR_GREAT_PEOPLE_STORED") )
		screen.setStackedBarColors("CityProductionBar", InfoBarTypes.INFOBAR_RATE, gc.getInfoTypeForString("COLOR_GREAT_PEOPLE_RATE") )
		screen.setStackedBarColors("CityProductionBar", InfoBarTypes.INFOBAR_RATE_EXTRA, gc.getInfoTypeForString("COLOR_EMPTY") )
		screen.setStackedBarColors("CityProductionBar", InfoBarTypes.INFOBAR_EMPTY, gc.getInfoTypeForString("COLOR_EMPTY") )
		self.appendtoHideState(screen, "CityProductionBar", HIDE_TYPE_CITY, HIDE_LEVEL_HIDE)

		screen.addPanel("CityProductionBarFrame", u"", u"", True, False, CITIZEN_BAR_WIDTH - 2 + CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100 - SMALL_BUTTON_SIZE / 4, CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2) + (STACK_BAR_HEIGHT * 3 / 2) + (MAP_EDGE_MARGIN_WIDTH / 2) - (CITY_MULTI_TAB_SIZE / 2) - ((CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100) / 2) + (CITY_MULTI_TAB_SIZE / 2) - (STACK_BAR_HEIGHT / 2) + CITY_MULTI_TAB_SIZE * RelativeButtonSize / 200, xResolution - (MAP_EDGE_MARGIN_WIDTH * 2) - CITY_MULTI_TAB_SIZE - ((CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100) / 2) + (CITY_MULTI_TAB_SIZE / 2) - CITIZEN_BAR_WIDTH - CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100 + SMALL_BUTTON_SIZE / 2 + 4, STACK_BAR_HEIGHT, PanelStyles.PANEL_STYLE_STANDARD, WidgetTypes.WIDGET_HELP_SELECTED, 0, -1 )
		screen.addDrawControl("CityProductionBarFrame", ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUILD_PROGRESS_BAR").getPath(), CITIZEN_BAR_WIDTH - 2 + CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100 - SMALL_BUTTON_SIZE / 4, CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2) + (STACK_BAR_HEIGHT * 3 / 2) + (MAP_EDGE_MARGIN_WIDTH / 2) - (CITY_MULTI_TAB_SIZE / 2) - ((CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100) / 2) + (CITY_MULTI_TAB_SIZE / 2) - (STACK_BAR_HEIGHT / 2) + CITY_MULTI_TAB_SIZE * RelativeButtonSize / 200, xResolution - (MAP_EDGE_MARGIN_WIDTH * 2) - CITY_MULTI_TAB_SIZE - ((CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100) / 2) + (CITY_MULTI_TAB_SIZE / 2) - CITIZEN_BAR_WIDTH - CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100 + SMALL_BUTTON_SIZE / 2 + 4, STACK_BAR_HEIGHT, WidgetTypes.WIDGET_HELP_SELECTED, 0, -1 )
		self.appendtoHideState(screen, "CityProductionBarFrame", HIDE_TYPE_CITY, HIDE_LEVEL_HIDE)	
		screen.setLabelAt("ProductionText", "CityProductionBar", "", CvUtil.FONT_CENTER_JUSTIFY, (xResolution - CITIZEN_BAR_WIDTH - BUILD_AREA_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2)) / 2, 0, -1.3, FontTypes.GAME_FONT, WidgetTypes.WIDGET_HELP_SELECTED, 0, -1 )

	# CITY REBEL BAR
		screen.addStackedBarGFC("RebelBar", STACK_BAR_HEIGHT, yResolution - BOTTOM_CENTER_HUD_HEIGHT - STACK_BAR_HEIGHT, CITIZEN_BAR_WIDTH - (STACK_BAR_HEIGHT * 2), STACK_BAR_HEIGHT, InfoBarTypes.NUM_INFOBAR_TYPES, WidgetTypes.WIDGET_HELP_REBEL, 1, -1 )
		screen.setStackedBarColors("RebelBar", InfoBarTypes.INFOBAR_STORED, gc.getInfoTypeForString("COLOR_BLUE") )
		screen.setStackedBarColors("RebelBar", InfoBarTypes.INFOBAR_RATE, gc.getInfoTypeForString("COLOR_BLUE") )
		screen.setStackedBarColors("RebelBar", InfoBarTypes.INFOBAR_RATE_EXTRA, gc.getInfoTypeForString("COLOR_EMPTY") )
		screen.setStackedBarColors("RebelBar", InfoBarTypes.INFOBAR_EMPTY, gc.getInfoTypeForString("COLOR_EMPTY") )
		self.appendtoHideState(screen, "RebelBar", HIDE_TYPE_CITY, HIDE_LEVEL_HIDE)

		screen.addPanel("RebelBarFrame", u"", u"", True, False, STACK_BAR_HEIGHT, yResolution - BOTTOM_CENTER_HUD_HEIGHT - STACK_BAR_HEIGHT, CITIZEN_BAR_WIDTH - (STACK_BAR_HEIGHT * 2), STACK_BAR_HEIGHT, PanelStyles.PANEL_STYLE_STANDARD, WidgetTypes.WIDGET_HELP_REBEL, 1, -1 )
		screen.addDrawControl("RebelBarFrame", ArtFileMgr.getInterfaceArtInfo("INTERFACE_REBEL_PROGRESS_BAR").getPath(), STACK_BAR_HEIGHT, yResolution - BOTTOM_CENTER_HUD_HEIGHT - STACK_BAR_HEIGHT, CITIZEN_BAR_WIDTH - (STACK_BAR_HEIGHT * 2), STACK_BAR_HEIGHT, WidgetTypes.WIDGET_HELP_REBEL, 1, -1 )
		self.appendtoHideState(screen, "RebelBarFrame", HIDE_TYPE_CITY, HIDE_LEVEL_HIDE)	

		screen.setLabelAt("RebelText", "RebelBar", "", CvUtil.FONT_CENTER_JUSTIFY, (CITIZEN_BAR_WIDTH - (STACK_BAR_HEIGHT * 3 / 2)) / 2, 0, -1.3, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
	
	# CITY BUILDING GRID	
		#Androrc Multiple Professions per Building
		pHeadSelectedCity = CyInterface().getHeadSelectedCity()
		for iSpecial in range(gc.getNumSpecialBuildingInfos()):
			iTargetBuilding = -1
			for iBuilding in range(gc.getNumBuildingInfos()):
				if gc.getCivilizationInfo(gc.getActivePlayer().getCivilizationType()).getCivilizationBuildings(gc.getBuildingInfo(iBuilding).getBuildingClassType()) == iBuilding:
					if(gc.getBuildingInfo(iBuilding).getSpecialBuildingType() == iSpecial):
						iTargetBuilding = iBuilding
						break

			screen.setImageButton("CityBuildingGraphic" + str(iSpecial), "", BUILDING_GRID[iSpecial][0] + STACK_BAR_HEIGHT, BUILDING_GRID[iSpecial][1] + CITY_TITLE_BAR_HEIGHT, BUILDING_GRID[iSpecial][3], BUILDING_GRID[iSpecial][3], WidgetTypes.WIDGET_ASSIGN_CITIZEN_TO_BUILDING, iSpecial, iTargetBuilding)
			screen.addDDSGFC("ProductionBox" + str(iSpecial), ArtFileMgr.getInterfaceArtInfo("INTERFACE_PRODUCTION_BOX").getPath(), BUILDING_GRID[iSpecial][0] + STACK_BAR_HEIGHT, BUILDING_GRID[iSpecial][1] + BUILDING_GRID[iSpecial][2], (BUILDING_GRID[iSpecial][3] + STACK_BAR_HEIGHT)*3/4, STACK_BAR_HEIGHT*3/4, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.hide("ProductionBox" + str(iSpecial))

	# BUTTONS
		screen.setImageButton("HurryGold", ArtFileMgr.getInterfaceArtInfo("INTERFACE_EUROPE_PURCHASE_UNIT").getPath(), CITIZEN_BAR_WIDTH - 2, CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2) + (STACK_BAR_HEIGHT * 3 / 2) + (MAP_EDGE_MARGIN_WIDTH / 2) - (CITY_MULTI_TAB_SIZE / 2) - ((CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100) / 2) + (CITY_MULTI_TAB_SIZE / 2), CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100, CITY_MULTI_TAB_SIZE * RelativeButtonSize / 100, WidgetTypes.WIDGET_HURRY, gc.getInfoTypeForString("HURRY_GOLD"), -1)
		self.appendtoHideState(screen, "HurryGold", HIDE_TYPE_CITY, HIDE_LEVEL_HIDE)

	# CITY AND PLOT SCROLL BUTTONS
		ScrollButtonSize = MEDIUM_BUTTON_SIZE
		# R&R, Robert Surcouf Screen Resolution/Ratio - Start
		#iXmodifier = xResolution * 5 / 100 - (xResolution - 1000)/200 					# More or less: 1024 -> 5% 1280 -> 4% / 1600 -> 2% / 1980 -> 0%
		#screen.setImageButton("CityScrollMinus",ArtFileMgr.getInterfaceArtInfo("INTERFACE_CITY_LEFT_ARROW").getPath(), (xResolution * 35 / 100) - (ScrollButtonSize / 2), (STACK_BAR_HEIGHT / 2) - (ScrollButtonSize / 3), ScrollButtonSize, ScrollButtonSize, WidgetTypes.WIDGET_CITY_SCROLL, -1, -1)
		screen.setImageButton("CityScrollMinus",ArtFileMgr.getInterfaceArtInfo("INTERFACE_CITY_LEFT_ARROW").getPath(), (xResolution * 35 / 100) - (ScrollButtonSize / 2) +xResolution * 5 / 100 - (xResolution - 1000)/200 , (STACK_BAR_HEIGHT / 2) - (ScrollButtonSize / 3), ScrollButtonSize, ScrollButtonSize, WidgetTypes.WIDGET_CITY_SCROLL, -1, -1)
		self.appendtoHideState(screen, "CityScrollMinus", HIDE_TYPE_CITY, HIDE_LEVEL_HIDE)
		#screen.setImageButton("CityScrollPlus",ArtFileMgr.getInterfaceArtInfo("INTERFACE_CITY_RIGHT_ARROW").getPath(), (xResolution * 65 / 100) - (ScrollButtonSize / 2), (STACK_BAR_HEIGHT / 2) - (ScrollButtonSize / 3), ScrollButtonSize, ScrollButtonSize, WidgetTypes.WIDGET_CITY_SCROLL, 1, -1)
		screen.setImageButton("CityScrollPlus",ArtFileMgr.getInterfaceArtInfo("INTERFACE_CITY_RIGHT_ARROW").getPath(), (xResolution * 65 / 100) - (ScrollButtonSize / 2)-xResolution * 5 / 100 - (xResolution - 1000)/200 , (STACK_BAR_HEIGHT / 2) - (ScrollButtonSize / 3), ScrollButtonSize, ScrollButtonSize, WidgetTypes.WIDGET_CITY_SCROLL, 1, -1)
		self.appendtoHideState(screen, "CityScrollPlus", HIDE_TYPE_CITY, HIDE_LEVEL_HIDE)
		# R&R, Robert Surcouf Screen Resolution/Ratio - End
		
		screen.setButtonGFC("PlotListDown", u"", "", xResolution - RIGHT_PANEL_WIDTH, yResolution - UPPER_SADDLE_HEIGHT - MEDIUM_BUTTON_SIZE * 3 / 2, MEDIUM_BUTTON_SIZE, MEDIUM_BUTTON_SIZE, WidgetTypes.WIDGET_PLOT_LIST_SHIFT, -1, self.numPlotListButtons(), ButtonStyles.BUTTON_STYLE_ARROW_UP )
		screen.hide("PlotListDown")
		screen.setButtonGFC("PlotListUp", u"", "", xResolution - RIGHT_PANEL_WIDTH, yResolution - UPPER_SADDLE_HEIGHT - MEDIUM_BUTTON_SIZE / 2, MEDIUM_BUTTON_SIZE, MEDIUM_BUTTON_SIZE, WidgetTypes.WIDGET_PLOT_LIST_SHIFT, 1, self.numPlotListButtons(), ButtonStyles.BUTTON_STYLE_ARROW_DOWN )
		screen.hide("PlotListUp")
		
	# EXIT BUTTON
		screen.setText("CityExitText", "", u"<font=4>" + localText.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + u"</font>", CvUtil.FONT_RIGHT_JUSTIFY, xResolution - 10, CITY_TITLE_BAR_HEIGHT / 12, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )
		self.appendtoHideState(screen, "CityExitText", HIDE_TYPE_CITY, HIDE_LEVEL_HIDE)

	#Erik Resource table toggle Yields
		screen.setImageButton("ShowOrHideYields", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SHOW_OR_HIDE_YIELDS").getPath(), 0, yResolution * 82 / 100, ScrollButtonSize, ScrollButtonSize, WidgetTypes.WIDGET_HELP_SHOW_OR_HIDE_YIELDS, -1, -1)
		self.appendtoHideState(screen, "ShowOrHideYields", HIDE_TYPE_CITY, HIDE_LEVEL_HIDE)

	# GROWTH EMPHASIZE/DEMPHASIZE
		screen.setImageButton("AvoidGrowth", ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL").getPath(), 0, CITY_TITLE_BAR_HEIGHT / 10, ScrollButtonSize, ScrollButtonSize, WidgetTypes.WIDGET_EMPHASIZE, -1, AVOID_GROWTH)
		self.appendtoHideState(screen, "AvoidGrowth", HIDE_TYPE_CITY, HIDE_LEVEL_HIDE)
		
	# Garrison and Transport Panel
		#screen.addScrollPanel("CityGarrisonPanel", u"", CITIZEN_BAR_WIDTH + (SMALL_BUTTON_SIZE / 8), yResolution - BOTTOM_CENTER_HUD_HEIGHT - TRANSPORT_AREA_HEIGHT * 9 / 8 - 2, xResolution - CITIZEN_BAR_WIDTH - TRANSPORT_AREA_WIDTH + STACK_BAR_HEIGHT * 4 / 8, TRANSPORT_AREA_HEIGHT - (STACK_BAR_HEIGHT / 3), PanelStyles.PANEL_STYLE_EMPTY, false, WidgetTypes.WIDGET_EJECT_CITIZEN, -1, -1 )	
		#self.appendtoHideState(screen, "CityGarrisonPanel", HIDE_TYPE_CITY, HIDE_LEVEL_HIDE)
		
		#screen.addScrollPanel("CityTransportPanel", u"", xResolution - TRANSPORT_AREA_WIDTH + MAP_EDGE_MARGIN_WIDTH - SMALL_BUTTON_SIZE / 2, yResolution - BOTTOM_CENTER_HUD_HEIGHT - TRANSPORT_AREA_HEIGHT * 9 / 8 - 2, TRANSPORT_AREA_WIDTH * 126 / 128, TRANSPORT_AREA_HEIGHT - (STACK_BAR_HEIGHT / 3), PanelStyles.PANEL_STYLE_MAIN, false, WidgetTypes.WIDGET_RECEIVE_MOVE_CARGO_TO_CITY, -1, -1 )
		#self.appendtoHideState(screen, "CityTransportPanel", HIDE_TYPE_CITY, HIDE_LEVEL_HIDE)
		
	# SCORES TABLE
		screen.addPanel("ScoreBackground", u"", u"", True, False, 0, 0, 0, 0, PanelStyles.PANEL_STYLE_HUD_HELP, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		for i in range(gc.getMAX_PLAYERS()):
			szName = "ScoreText" + str(i)
			screen.setText( szName, "Background", u"", CvUtil.FONT_RIGHT_JUSTIFY, 996, 622, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_CONTACT_CIV, i, -1 )
			screen.hide( szName )

		# This should be a forced redraw screen
		screen.setForcedRedraw(True)
		self.SetHideLists(screen)

		# This should show the screen immidiately and pass input to the game
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, True)

	# Will update the screen (every 250 MS)
	def updateScreen( self ):

		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE )

		messageControl = CyMessageControl()

		# Hide all interface widgets
		bShow = False
		if ( CyInterface().getShowInterface() != InterfaceVisibility.INTERFACE_HIDE_ALL and CyInterface().getShowInterface() != InterfaceVisibility.INTERFACE_MINIMAP_ONLY ):
			if (gc.getGame().isPaused()):
				# Pause overrides other messages
				acOutput = localText.getText("SYSTEM_GAME_PAUSED", (gc.getPlayer(gc.getGame().getPausePlayer()).getNameKey(), ))
				screen.setEndTurnState("EndTurnText", acOutput )
				bShow = True
			elif (messageControl.GetFirstBadConnection() != -1):
				# Waiting on a bad connection to resolve
				if (messageControl.GetConnState(messageControl.GetFirstBadConnection()) == 1):
					if (gc.getGame().isMPOption(MultiplayerOptionTypes.MPOPTION_ANONYMOUS)):
						acOutput = localText.getText("SYSTEM_WAITING_FOR_PLAYER", (gc.getPlayer(messageControl.GetFirstBadConnection()).getNameKey(), 0))
					else:
						acOutput = localText.getText("SYSTEM_WAITING_FOR_PLAYER", (gc.getPlayer(messageControl.GetFirstBadConnection()).getNameKey(), (messageControl.GetFirstBadConnection() + 1)))
					screen.setEndTurnState("EndTurnText", acOutput )
					bShow = True
				elif (messageControl.GetConnState(messageControl.GetFirstBadConnection()) == 2):
					if (gc.getGame().isMPOption(MultiplayerOptionTypes.MPOPTION_ANONYMOUS)):
						acOutput = localText.getText("SYSTEM_PLAYER_JOINING", (gc.getPlayer(messageControl.GetFirstBadConnection()).getNameKey(), 0))
					else:
						acOutput = localText.getText("SYSTEM_PLAYER_JOINING", (gc.getPlayer(messageControl.GetFirstBadConnection()).getNameKey(), (messageControl.GetFirstBadConnection() + 1)))
					screen.setEndTurnState("EndTurnText", acOutput )
					bShow = True
			else:
				# Flash select messages if no popups are present
				if ( CyInterface().shouldDisplayWaitingOthers() ):
					acOutput = localText.getText("SYSTEM_WAITING", ())
					screen.setEndTurnState("EndTurnText", acOutput )
					bShow = True
				elif ( CyInterface().shouldDisplayEndTurn() ):
					acOutput = localText.getText("SYSTEM_END_TURN", ())
					screen.setEndTurnState("EndTurnText", acOutput )
					bShow = True
				elif ( CyInterface().shouldDisplayWaitingYou() ):
					acOutput = localText.getText("SYSTEM_WAITING_FOR_YOU", ())
					screen.setEndTurnState("EndTurnText", acOutput )
					bShow = True

		if ( bShow ):
			screen.showEndTurn("EndTurnText")
			if ( CyInterface().isCityScreenUp() ):
				screen.moveItem("EndTurnText", 0, yResolution - 194, -0.1 )
			else:
				screen.moveItem("EndTurnText", 0, yResolution - LOWER_SADDLE_HEIGHT - 24, -0.1 )
		else:
			screen.hideEndTurn("EndTurnText")

		self.updateEndTurnButton()

		if (CyInterface().getShowInterface() != InterfaceVisibility.INTERFACE_HIDE_ALL and CyInterface().getShowInterface() != InterfaceVisibility.INTERFACE_ADVANCED_START):
			if (CyInterface().isCityScreenUp()):
				screen.hide("ClockText")
			else:
				if (CyUserProfile().isClockOn()):
					screen.setLabel("ClockText", "Background", self.setFontSize(getClockText(), 0), CvUtil.FONT_RIGHT_JUSTIFY, xResolution - 24, 42, -0.3, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
					screen.show("ClockText")
				else:
					screen.hide("ClockText")
		else:
			screen.hide("ClockText")

		return 0

	# Will redraw the interface
	def redraw( self ):

		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE )

		if not INITIALIZED:
			self.SetGlobals(screen)

		# Check Dirty Bits, see what we need to redraw...
		if ( CyInterface().isDirty(InterfaceDirtyBits.MiscButtons_DIRTY_BIT) ):
			# Miscellaneous buttons (civics screen, etc)
			self.updateMiscButtons()
			CyInterface().setDirty(InterfaceDirtyBits.MiscButtons_DIRTY_BIT, False)
		if ( CyInterface().isDirty(InterfaceDirtyBits.InfoPane_DIRTY_BIT) ):
			# Info Pane Dirty Bit
			# This must come before updatePlotListButtons so that the entity widget appears in front of the stats
			self.updateInfoPaneStrings()
			CyInterface().setDirty(InterfaceDirtyBits.InfoPane_DIRTY_BIT, False)
		if ( CyInterface().isDirty(InterfaceDirtyBits.PlotListButtons_DIRTY_BIT) ):
			# Plot List Buttons Dirty
			self.updatePlotListButtons()
			CyInterface().setDirty(InterfaceDirtyBits.PlotListButtons_DIRTY_BIT, False)
		if ( CyInterface().isDirty(InterfaceDirtyBits.SelectionButtons_DIRTY_BIT) ):
			# Selection Buttons Dirty
			self.updateSelectionButtons()
			CyInterface().setDirty(InterfaceDirtyBits.SelectionButtons_DIRTY_BIT, False)
		if ( CyInterface().isDirty(InterfaceDirtyBits.GameData_DIRTY_BIT) ):
			# Game Data Strings Dirty
			self.updateGameDataStrings()
			CyInterface().setDirty(InterfaceDirtyBits.GameData_DIRTY_BIT, False)
		if ( CyInterface().isDirty(InterfaceDirtyBits.Help_DIRTY_BIT) ):
			# Help Dirty bit
			self.updateHelpStrings()
			CyInterface().setDirty(InterfaceDirtyBits.Help_DIRTY_BIT, False)
		if ( CyInterface().isDirty(InterfaceDirtyBits.CityScreen_DIRTY_BIT) ):
			# Selection Data Dirty Bit
			self.updateCityScreen()
			#CyInterface().setDirty(InterfaceDirtyBits.Domestic_Advisor_DIRTY_BIT, True)
			CyInterface().setDirty(InterfaceDirtyBits.CityScreen_DIRTY_BIT, False)
		if ( CyInterface().isDirty(InterfaceDirtyBits.ResourceTable_DIRTY_BIT) ):
			# Resource Table
			self.updateResourceTable()
			CyInterface().setDirty(InterfaceDirtyBits.ResourceTable_DIRTY_BIT, False)
		if ( CyInterface().isDirty(InterfaceDirtyBits.CitizenButtons_DIRTY_BIT) ):
			# Citizen Buttons Dirty
			self.updateCitizenButtons()
			CyInterface().setDirty(InterfaceDirtyBits.CitizenButtons_DIRTY_BIT, False)
		if ( CyInterface().isDirty(InterfaceDirtyBits.Score_DIRTY_BIT) or CyInterface().checkFlashUpdate() ):
			# Scores!
			self.updateScoreStrings()
			CyInterface().setDirty(InterfaceDirtyBits.Score_DIRTY_BIT, False)
		if ( CyInterface().isDirty(InterfaceDirtyBits.GlobeInfo_DIRTY_BIT) ):
			# Globeview and Globelayer buttons
			CyInterface().setDirty(InterfaceDirtyBits.GlobeInfo_DIRTY_BIT, False)
			self.updateGlobeviewButtons()

		return 0

	# Will update the end Turn Button
	def updateEndTurnButton( self ):

		global g_eEndTurnButtonState
		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE )

		pHeadSelectedCity = CyInterface().getHeadSelectedCity()

		if (CyInterface().shouldDisplayEndTurnButton() and CyInterface().getShowInterface() <= InterfaceVisibility.INTERFACE_HIDE and (not pHeadSelectedCity or CyEngine().isGlobeviewUp())):
			eState = CyInterface().getEndTurnState()
			bShow = False

			if (eState == EndTurnButtonStates.END_TURN_OVER_HIGHLIGHT):
				screen.setEndTurnState("EndTurnButton", u"Red")
				bShow = True
			elif (eState == EndTurnButtonStates.END_TURN_OVER_DARK):
				screen.setEndTurnState("EndTurnButton", u"Red")
				bShow = True
			elif (eState == EndTurnButtonStates.END_TURN_GO ):
				screen.setEndTurnState("EndTurnButton", u"Green")
				bShow = True

			if (bShow):
				screen.showEndTurn("EndTurnButton")
				screen.show("EndTurnLayer")
			else:
				screen.hideEndTurn("EndTurnButton")
				screen.hide("EndTurnLayer")
				
			if (g_eEndTurnButtonState == eState):
				return

			g_eEndTurnButtonState = eState

		else:
			screen.hideEndTurn("EndTurnButton")
			screen.hide("EndTurnLayer")
			
		return 0

	# Update the miscellaneous buttons
	def updateMiscButtons( self ):
		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE )
		
		if (CyInterface().shouldDisplayFlag() and CyInterface().getShowInterface() == InterfaceVisibility.INTERFACE_SHOW):
			screen.show("CivilizationFlag")
		else:
			screen.hide("CivilizationFlag")

		if (CyInterface().getShowInterface() == InterfaceVisibility.INTERFACE_SHOW):
			iLevel = 3
		if (CyInterface().getShowInterface() == InterfaceVisibility.INTERFACE_HIDE):
			iLevel = 2
		if (CyInterface().getShowInterface() == InterfaceVisibility.INTERFACE_HIDE_ALL):
			iLevel = 1
		elif (CyInterface().getShowInterface() == InterfaceVisibility.INTERFACE_ADVANCED_START):
			iLevel = 0
		elif (CyInterface().getShowInterface() == InterfaceVisibility.INTERFACE_MINIMAP_ONLY):
			iLevel = 1

		if (CyInterface().isCityScreenUp()):
			self.SetHideState(screen, HIDE_TYPE_CITY, iLevel)
		elif (CyEngine().isGlobeviewUp()):
			self.SetHideState(screen, HIDE_TYPE_GLOBAL, iLevel)
		else:
			self.SetHideState(screen, HIDE_TYPE_MAP, iLevel)

		screen.updateMinimapVisibility()

		return 0

	# Update plot List Buttons
	def updatePlotListButtons( self ):
		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE )
		xResolution = screen.getXResolution()
		yResolution = screen.getYResolution()
		pHeadSelectedCity = CyInterface().getHeadSelectedCity()
		
		bHandled = False
		if (CyInterface().shouldDisplayUnitModel() and not CyEngine().isGlobeviewUp() and CyInterface().getShowInterface() != InterfaceVisibility.INTERFACE_HIDE_ALL and not pHeadSelectedCity):
			if (CyInterface().isCityScreenUp()):
				if ( not bHandled ):
					screen.hide("InterfaceUnitModel")
					bHandled = True

				screen.moveToFront("CityBuildQueue")
				screen.moveToFront("SelectedUnitText")
				screen.moveToFront("SelectedUnitName")

			elif (CyInterface().getHeadSelectedUnit()):
				screen.hide("InterfaceUnitModel")
				screen.moveToFront("SelectedUnitText")
				screen.moveToFront("SelectedUnitName")
				if (not pHeadSelectedCity):
					for iYield in EMPHASIZEYIELDS:
						screen.hide("MapYieldBox" + str(iYield))
						screen.hide("MapYieldIcon" + str(iYield))
						screen.hide("MapYieldEmphasize" + str(iYield))
						screen.hide("MapYieldDe-Emphasize" + str(iYield))
						
					screen.hide("MapBuildingSelectionMultiList")
					screen.hide("MapCityProductionBar")
					screen.hide("MapCityBuildQueue")
					screen.hide("MapCityLeftPanelShadow")
					screen.hide("MapCityRightPanelShadow")

			else:
				screen.hide("InterfaceUnitModel")
				if (not pHeadSelectedCity):
					for iYield in EMPHASIZEYIELDS:
						screen.hide("MapYieldBox" + str(iYield))
						screen.hide("MapYieldIcon" + str(iYield))
						screen.hide("MapYieldEmphasize" + str(iYield))
						screen.hide("MapYieldDe-Emphasize" + str(iYield))
						
					screen.hide("MapBuildingSelectionMultiList")
					screen.hide("MapCityProductionBar")
					screen.hide("MapCityBuildQueue")
					screen.hide("MapCityLeftPanelShadow")
					screen.hide("MapCityRightPanelShadow")
					
		else:
			screen.hide("InterfaceUnitModel")
			if (not pHeadSelectedCity):
				for iYield in EMPHASIZEYIELDS:
					screen.hide("MapYieldBox" + str(iYield))
					screen.hide("MapYieldIcon" + str(iYield))
					screen.hide("MapYieldEmphasize" + str(iYield))
					screen.hide("MapYieldDe-Emphasize" + str(iYield))
					
				screen.hide("MapBuildingSelectionMultiList")
				screen.hide("MapCityProductionBar")
				screen.hide("MapCityBuildQueue")
				screen.hide("MapCityLeftPanelShadow")
				screen.hide("MapCityRightPanelShadow")

		pPlot = CyInterface().getSelectionPlot()

		screen.hide("PlotListDown")
		screen.hide("PlotListUp")
		screen.hide("MapBuildingSelectionMultiList")
		screen.hide("MapCityProductionBar")
		screen.hide("MapCityBuildQueue")
		screen.hide("MapCityLeftPanelShadow")
		screen.hide("MapCityRightPanelShadow")

		for j in range(gc.getMAX_PLOT_LIST_ROWS()):
			for i in range(self.numPlotListButtons()):
				szString = "PlotListButtonMulti" + str(j * self.numPlotListButtons() + i)
				screen.hide( szString )
				screen.hide( szString + "Health")
				screen.hide( szString + "Icon")
				
		if ( pPlot and CyInterface().getShowInterface() != InterfaceVisibility.INTERFACE_HIDE_ALL and not CyEngine().isGlobeviewUp() and not CyInterface().isCityScreenUp() and CyInterface().getShowInterface() != InterfaceVisibility.INTERFACE_MINIMAP_ONLY and CyInterface().getShowInterface() != InterfaceVisibility.INTERFACE_ADVANCED_START):

			iVisibleUnits = CyInterface().getNumVisibleUnits()
			iCount = -(CyInterface().getPlotListColumn())
			bDownArrow = False
			bUpArrow = False

			pPlayer = gc.getPlayer(gc.getGame().getActivePlayer())
			if not CyInterface().isPlotListMultiRow():
				pPlayer.toggleMultiRowPlotList()
		
			iMaxRows = gc.getMAX_PLOT_LIST_ROWS()
			szPlotListButtonName = "PlotListButtonMulti"
				
			iCount += CyInterface().getPlotListOffset()
			iSkipped = 0

			CyInterface().cacheInterfacePlotUnits(pPlot)
			for i in range(CyInterface().getNumCachedInterfacePlotUnits()):
				pLoopUnit = CyInterface().getCachedInterfacePlotUnit(i)
				if (pLoopUnit):
					if ((iCount == 0) and (CyInterface().getPlotListColumn() > 0)):
						bDownArrow = True
					elif ((iCount == (iMaxRows * self.numPlotListButtons() - 1)) and ((iVisibleUnits - iCount - CyInterface().getPlotListColumn() + iSkipped) > 1)):
						bUpArrow = True

					if ((iCount >= 0) and (iCount <  self.numPlotListButtons() * iMaxRows)):
						if ((pLoopUnit.getTeam() != gc.getGame().getActiveTeam()) or pLoopUnit.isWaiting()):
							szFileName = ArtFileMgr.getInterfaceArtInfo("OVERLAY_FORTIFY").getPath()

						elif (pLoopUnit.canMove()):
							if (pLoopUnit.hasMoved()):
								szFileName = ArtFileMgr.getInterfaceArtInfo("OVERLAY_HASMOVED").getPath()
							else:
								szFileName = ArtFileMgr.getInterfaceArtInfo("OVERLAY_MOVE").getPath()
						else:
							szFileName = ArtFileMgr.getInterfaceArtInfo("OVERLAY_NOMOVE").getPath()

						szString = szPlotListButtonName + str(iCount)
						screen.changeImageButton( szString, pLoopUnit.getButton() )
						if ( pLoopUnit.getOwner() == gc.getGame().getActivePlayer() ):
							screen.enable(szString, True)
						else:
							screen.enable(szString, False)

						screen.setState(szString, pLoopUnit.IsSelected())
						screen.show( szString )

						# place the health bar
						if (pLoopUnit.isFighting()):
							bShowHealth = False
						else:
							bShowHealth = pLoopUnit.canFight()

						if bShowHealth:
							szStringHealth = szString + "Health"
							screen.setBarPercentage( szStringHealth, InfoBarTypes.INFOBAR_STORED, float( pLoopUnit.currHitPoints() ) / float( pLoopUnit.maxHitPoints() ) )
							if (pLoopUnit.getDamage() >= ((pLoopUnit.maxHitPoints() * 2) / 3)):
								screen.setStackedBarColors(szStringHealth, InfoBarTypes.INFOBAR_STORED, gc.getInfoTypeForString("COLOR_RED"))
							elif (pLoopUnit.getDamage() >= (pLoopUnit.maxHitPoints() / 3)):
								screen.setStackedBarColors(szStringHealth, InfoBarTypes.INFOBAR_STORED, gc.getInfoTypeForString("COLOR_YELLOW"))
							else:
								screen.setStackedBarColors(szStringHealth, InfoBarTypes.INFOBAR_STORED, gc.getInfoTypeForString("COLOR_GREEN"))
							screen.show( szStringHealth )

						# Adds the overlay first
						szStringIcon = szString + "Icon"
						screen.changeDDSGFC( szStringIcon, szFileName )
						screen.show( szStringIcon )

					iCount += 1
	
			if (iVisibleUnits > self.numPlotListButtons() * iMaxRows):
				screen.enable("PlotListDown", bDownArrow)
				screen.show("PlotListDown")
				screen.enable("PlotListUp", bUpArrow)
				screen.show("PlotListUp")
				
		if (not CyInterface().isCityScreenUp() and not CyEngine().isGlobeviewUp()):
			if (pHeadSelectedCity):
				screen.hide("InterfaceMapViewSaddleRight")
				screen.hide("MiniMapRing")
								
				screen.hide("_FXS_Screen_Bogus_Minimap_Name")
				screen.hide("EndTurnRing")
				screen.hide("EndTurnText")
				screen.show("MapCityLeftPanelShadow")
				screen.show("MapCityRightPanelShadow")
			
				for i in EMPHASIZEYIELDS:
					screen.show("MapYieldBox" + str(i))
					screen.show("MapYieldIcon" + str(i))
					screen.hide("MapYieldEmphasize" + str(i))
					screen.hide("MapYieldDe-Emphasize" + str(i))
					
					if pHeadSelectedCity.AI_getEmphasizeYieldCount(i) > 0:
						screen.show("MapYieldEmphasize" + str(i))
					elif pHeadSelectedCity.AI_getEmphasizeYieldCount(i) < 0:
						screen.show("MapYieldDe-Emphasize" + str(i))

				screen.addMultiListControlGFC("MapBuildingSelectionMultiList", u"", 0, DOMESTIC_ADVISOR_HEIGHT + 3, MINIMAP_WIDTH - 3, DOMESTIC_ADVISOR_SPACE - LOWER_SADDLE_HEIGHT - 6, self.BOTTOM_BUTTON_ROWS, BOTTOM_BUTTON_SIZE, BOTTOM_BUTTON_SIZE, TableStyles.TABLE_STYLE_STANDARD )
				screen.clearMultiList("MapBuildingSelectionMultiList")
				
				iCount = 0
				iRow = 0
				bFound = False
				for i in range (gc.getNumUnitClassInfos()):
					eLoopUnit = gc.getCivilizationInfo(pHeadSelectedCity.getCivilizationType()).getCivilizationUnits(i)
					if (eLoopUnit != UnitTypes.NO_UNIT):
						if (pHeadSelectedCity.canTrain(eLoopUnit, False, True)):
							szButton = gc.getPlayer(pHeadSelectedCity.getOwner()).getUnitButton(eLoopUnit)
							screen.appendMultiListButton("MapBuildingSelectionMultiList", szButton, iRow, WidgetTypes.WIDGET_TRAIN, i, -1, False)
							if ( not pHeadSelectedCity.canTrain(eLoopUnit, False, False) ):
								screen.disableMultiListButton("MapBuildingSelectionMultiList", iRow, iCount, szButton)
							elif pHeadSelectedCity.getUnitProduction(eLoopUnit) > 0:
								bUnitTypeInQueue = false
								for i in range(CyInterface().getNumOrdersQueued()):
									if (CyInterface().getOrderNodeType(i)  == OrderTypes.ORDER_TRAIN):
										if CyInterface().getOrderNodeData1(i) == eLoopUnit:
											bUnitTypeInQueue = true
											break
								if not bUnitTypeInQueue:
									screen.enableMultiListPulse("MapBuildingSelectionMultiList", true, iRow, iCount)
								
							iCount += 1
							bFound = True

				iCount = 0
				iRow += 1
				bFound = False
				for i in range (gc.getNumBuildingClassInfos()):
					eLoopBuilding = gc.getCivilizationInfo(pHeadSelectedCity.getCivilizationType()).getCivilizationBuildings(i)
					if (eLoopBuilding != BuildingTypes.NO_BUILDING):
						if (pHeadSelectedCity.canConstruct(eLoopBuilding, False, True, False)):
							screen.appendMultiListButton("MapBuildingSelectionMultiList", gc.getBuildingInfo(eLoopBuilding).getButton(), iRow, WidgetTypes.WIDGET_CONSTRUCT, i, -1, False )
							if (not pHeadSelectedCity.canConstruct(eLoopBuilding, False, False, False)):
								screen.disableMultiListButton("MapBuildingSelectionMultiList", iRow, iCount, gc.getBuildingInfo(eLoopBuilding).getButton())
							elif pHeadSelectedCity.getBuildingProduction(eLoopBuilding) > 0:
								screen.enableMultiListPulse("MapBuildingSelectionMultiList", true, iRow, iCount)
							
							iCount += 1
							bFound = True

				iCount = 0
				iRow += 1
				bFound = False
				for i in range (gc.getNumFatherPointInfos()):
					if (pHeadSelectedCity.canConvince(i, False, True)):
						screen.appendMultiListButton("MapBuildingSelectionMultiList", gc.getFatherPointInfo(i).getButton(), iRow, WidgetTypes.WIDGET_CONVINCE, i, -1, False )
						if (not pHeadSelectedCity.canConvince(i, False, False)):
							screen.disableMultiListButton("MapBuildingSelectionMultiList", iRow, iCount, gc.getFatherPointInfo(i).getButton())
						iCount += 1
						bFound = True

				screen.show("MapBuildingSelectionMultiList")
			
			# MAP BUILD QUEUE
				QueueWidth = LOWER_RIGHT_CORNER_BACKGROUND_WIDTH
				screen.addTableControlGFC("MapCityBuildQueue", 2, xResolution - MINIMAP_WIDTH + 3, DOMESTIC_ADVISOR_HEIGHT + STACK_BAR_HEIGHT + 9, MINIMAP_WIDTH, DOMESTIC_ADVISOR_SPACE - LOWER_SADDLE_HEIGHT - STACK_BAR_HEIGHT * 5 / 2 - 18, False, False, self.SELECTION_PANEL_ROW_HEIGHT, self.SELECTION_PANEL_ROW_HEIGHT, TableStyles.TABLE_STYLE_STANDARD)
				screen.setStyle("MapCityBuildQueue", "Table_EmptyScroll_Style")
				screen.hide("MapCityBuildQueue")
				screen.setTableColumnHeader("MapCityBuildQueue", 0, u"", MINIMAP_WIDTH - (QueueWidth * 3 / 10))
				screen.setTableColumnHeader("MapCityBuildQueue", 1, u"", QueueWidth * 3 / 10)
				
				iOrders = CyInterface().getNumOrdersQueued()
				iRow = 0
				
				for i in range(iOrders):
					szLeftBuffer = u""
					szRightBuffer = u""

					if ( CyInterface().getOrderNodeType(i) == OrderTypes.ORDER_TRAIN ):
						szLeftBuffer = gc.getUnitInfo(CyInterface().getOrderNodeData1(i)).getDescription()
						szRightBuffer = str(pHeadSelectedCity.getUnitProductionTurnsLeft(CyInterface().getOrderNodeData1(i), i))

						if (CyInterface().getOrderNodeSave(i)):
							szLeftBuffer = u"*" + szLeftBuffer

					elif ( CyInterface().getOrderNodeType(i) == OrderTypes.ORDER_CONSTRUCT ):
						szLeftBuffer = gc.getBuildingInfo(CyInterface().getOrderNodeData1(i)).getDescription()
						szRightBuffer = str(pHeadSelectedCity.getBuildingProductionTurnsLeft(CyInterface().getOrderNodeData1(i), i))

					elif ( CyInterface().getOrderNodeType(i) == OrderTypes.ORDER_CONVINCE ):
						szLeftBuffer = gc.getFatherPointInfo(CyInterface().getOrderNodeData1(i)).getDescription()

					if i == 0:
						szLeftBuffer = u"<color=255,223,0>" + szLeftBuffer + u"</color>"
						szRightBuffer = u"<color=255,223,0>" + szRightBuffer + u"</color>"
					
					screen.appendTableRow("MapCityBuildQueue")
					screen.setTableText("MapCityBuildQueue", 0, iRow, szLeftBuffer, "", WidgetTypes.WIDGET_HELP_SELECTED, i, -1, CvUtil.FONT_LEFT_JUSTIFY )
					screen.setTableText("MapCityBuildQueue", 1, iRow, szRightBuffer + "  ", "", WidgetTypes.WIDGET_HELP_SELECTED, i, -1, CvUtil.FONT_RIGHT_JUSTIFY )

					iRow += 1
				screen.show("MapCityBuildQueue")
				
				
			# CURRENT PRODUCTION BAR FILL
				fProductionNeeded = float(pHeadSelectedCity.getProductionNeeded(YieldTypes.YIELD_HAMMERS))
				iProductionDiffNoFood = pHeadSelectedCity.getCurrentProductionDifference(True)
				iProductionDiffJustFood = 0
				if (fProductionNeeded > 0):
					iFirst = ((float(pHeadSelectedCity.getProduction())) / fProductionNeeded)
					screen.setBarPercentage("MapCityProductionBar", InfoBarTypes.INFOBAR_STORED, iFirst )
					if ( iFirst == 1 ):
						iSecond = (((float(iProductionDiffNoFood)) / fProductionNeeded) )
					else:
						iSecond = (((float(iProductionDiffNoFood)) / fProductionNeeded) ) / ( 1 - iFirst )
					screen.setBarPercentage("MapCityProductionBar", InfoBarTypes.INFOBAR_RATE, iSecond )
					if ( iFirst + iSecond == 1 ):
						screen.setBarPercentage("MapCityProductionBar", InfoBarTypes.INFOBAR_RATE_EXTRA, (((float(iProductionDiffJustFood)) / fProductionNeeded) ) )
					else:
						screen.setBarPercentage("MapCityProductionBar", InfoBarTypes.INFOBAR_RATE_EXTRA, (( ((float(iProductionDiffJustFood)) / fProductionNeeded) ) ) / ( 1 - ( iFirst + iSecond ) ) )

			# CURRENT PRODUCTION BAR TEXT
				if (pHeadSelectedCity.isProductionBuilding() or pHeadSelectedCity.isProductionUnit()):
					szBuffer = u"%s" % pHeadSelectedCity.getProductionName()
				elif pHeadSelectedCity.isProduction():
					szBuffer = pHeadSelectedCity.getProductionName()
				else:
					szBuffer = u""
					
				RedText = false
				for iYield in range(YieldTypes.NUM_YIELD_TYPES):
					if iYield != YieldTypes.YIELD_HAMMERS:
						if (CyInterface().getOrderNodeType(0) == OrderTypes.ORDER_TRAIN ):
							YieldRequired = gc.getUnitInfo(CyInterface().getOrderNodeData1(0)).getYieldCost(iYield)
						elif (CyInterface().getOrderNodeType(0) == OrderTypes.ORDER_CONSTRUCT):
							YieldRequired = gc.getBuildingInfo(CyInterface().getOrderNodeData1(0)).getYieldCost(iYield)
						else:
							YieldRequired = 0
							
						if (YieldRequired > 0):
							if (YieldRequired >  pHeadSelectedCity.getYieldStored(iYield) + pHeadSelectedCity.getYieldRushed(iYield)):
								RedText = true
								break
				if (RedText):
					screen.setStackedBarColors("MapCityProductionBar", InfoBarTypes.INFOBAR_STORED, gc.getInfoTypeForString("COLOR_RED") )
					screen.setStackedBarColors("MapCityProductionBar", InfoBarTypes.INFOBAR_RATE, gc.getInfoTypeForString("COLOR_WARNING_RATE") )
				else:
					screen.setStackedBarColors("MapCityProductionBar", InfoBarTypes.INFOBAR_STORED, gc.getInfoTypeForString("COLOR_GREAT_PEOPLE_STORED") )
					screen.setStackedBarColors("MapCityProductionBar", InfoBarTypes.INFOBAR_RATE, gc.getInfoTypeForString("COLOR_GREAT_PEOPLE_RATE") )
					
				screen.setLabelAt("ProductionText", "MapCityProductionBar", szBuffer, CvUtil.FONT_CENTER_JUSTIFY, UNIT_HEALTH_BAR_WIDTH / 2, STACK_BAR_HEIGHT * 3 / 4, -1.3, FontTypes.GAME_FONT, WidgetTypes.WIDGET_HELP_SELECTED, 0, -1 )
				screen.show("MapCityProductionBar")
				
			self.updateScoreStrings()

			
		return 0

	# Will hide and show the selection buttons and their associated buttons
	def updateSelectionButtons( self ):
		global g_pSelectedUnit
		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE )

		# Find out our resolution
		pHeadSelectedCity = CyInterface().getHeadSelectedCity()
		pHeadSelectedUnit = CyInterface().getHeadSelectedUnit()
		xResolution = screen.getXResolution()
		yResolution = screen.getYResolution()

		# All of the hides...
		screen.clearMultiList("CityBuildingSelectionMultiList")
		screen.hide("AutomateProduction")
		#screen.hide("CityTransportPanel")
		#screen.hide("CityGarrisonPanel")
		screen.hide("TradeRouteText")
		screen.hide("ImportTradeRouteText")
		screen.hide("ExportTradeRouteText")
		screen.hide("ImportButton")
		screen.hide("ExportButton")
		# teacher list - start - Nightinggale
		screen.hide("TeacherListButton")
		# teacher list - end - Nightinggale
		# R&R, Robert Surcouf, Custom House Popup-Screen START
		screen.hide("CustomHouseButton")
		screen.hide("DomesticMarketButton")
		# R&R, Robert Surcouf, Custom House Popup-Screen END
		screen.hide("ImportLabel")
		screen.hide("ExportLabel")
		screen.hide("CityBuildQueue")
		screen.hide("EmphasizeTable")

		self.setMinimapButtonVisibility(False)
		screen.hideList(ACTION_BUTTON_HIDE)

		for iYield in range(YieldTypes.NUM_YIELD_TYPES):
			screen.hide("YieldCost" + str(iYield))

		if (not CyInterface().isCityScreenUp() and not CyEngine().isGlobeviewUp() and pHeadSelectedCity):
			for i in EMPHASIZEYIELDS:
				screen.show("MapYieldIcon" + str(i))
				screen.hide("MapYieldEmphasize" + str(i))
				screen.hide("MapYieldDe-Emphasize" + str(i))
				
				if pHeadSelectedCity.AI_getEmphasizeYieldCount(i) > 0:
					screen.show("MapYieldEmphasize" + str(i))
				elif pHeadSelectedCity.AI_getEmphasizeYieldCount(i) < 0:
					screen.show("MapYieldDe-Emphasize" + str(i))

		if (not CyEngine().isGlobeviewUp() and CyInterface().isCityScreenUp() and pHeadSelectedCity):
			self.setMinimapButtonVisibility(True)

			self.updateGarrisonAndTransports()
			g_pSelectedUnit = 0
			screen.enable("HurryGold", pHeadSelectedCity.canHurry(0, False))
			
			pPlayer = gc.getPlayer(gc.getGame().getActivePlayer())
			iProductionBarSize = BOTTOM_BUTTON_SIZE + 3
			iProductionBarMargin = 0
			
		# BUILDING SELECTION BUTTONS
			iCount = 0
			iRow = 0
			bFound = False
			for i in range (gc.getNumUnitClassInfos()):
				eLoopUnit = gc.getCivilizationInfo(pHeadSelectedCity.getCivilizationType()).getCivilizationUnits(i)
				if (eLoopUnit != UnitTypes.NO_UNIT):
					if (pHeadSelectedCity.canTrain(eLoopUnit, False, True)):
						szButton = gc.getPlayer(pHeadSelectedCity.getOwner()).getUnitButton(eLoopUnit)
						screen.appendMultiListButton("CityBuildingSelectionMultiList", szButton, iRow, WidgetTypes.WIDGET_TRAIN, i, -1, False)
						if ( not pHeadSelectedCity.canTrain(eLoopUnit, False, False) ):
							screen.disableMultiListButton("CityBuildingSelectionMultiList", iRow, iCount, szButton)
						elif pHeadSelectedCity.getUnitProduction(eLoopUnit) > 0:
							bUnitTypeInQueue = false
							for i in range(CyInterface().getNumOrdersQueued()):
								if (CyInterface().getOrderNodeType(i)  == OrderTypes.ORDER_TRAIN):
									if CyInterface().getOrderNodeData1(i) == eLoopUnit:
										bUnitTypeInQueue = true
										break
							if not bUnitTypeInQueue:
								screen.enableMultiListPulse("CityBuildingSelectionMultiList", true, iRow, iCount)
						iCount += 1
						bFound = True

			iCount = 0
			iRow += 1
			bFound = False
			for i in range (gc.getNumBuildingClassInfos()):
				eLoopBuilding = gc.getCivilizationInfo(pHeadSelectedCity.getCivilizationType()).getCivilizationBuildings(i)
				if (eLoopBuilding != BuildingTypes.NO_BUILDING):
					if (pHeadSelectedCity.canConstruct(eLoopBuilding, False, True, False)):
						screen.appendMultiListButton("CityBuildingSelectionMultiList", gc.getBuildingInfo(eLoopBuilding).getButton(), iRow, WidgetTypes.WIDGET_CONSTRUCT, i, -1, False )
						if (not pHeadSelectedCity.canConstruct(eLoopBuilding, False, False, False)):
							screen.disableMultiListButton("CityBuildingSelectionMultiList", iRow, iCount, gc.getBuildingInfo(eLoopBuilding).getButton())
						elif pHeadSelectedCity.getBuildingProduction(eLoopBuilding) > 0:
							screen.enableMultiListPulse("CityBuildingSelectionMultiList", true, iRow, iCount)
							
						iCount += 1
						bFound = True

			iCount = 0
			iRow += 1
			bFound = False
			for i in range (gc.getNumFatherPointInfos()):
				if (pHeadSelectedCity.canConvince(i, False, True)):
					screen.appendMultiListButton("CityBuildingSelectionMultiList", gc.getFatherPointInfo(i).getButton(), iRow, WidgetTypes.WIDGET_CONVINCE, i, -1, False )
					if (not pHeadSelectedCity.canConvince(i, False, False)):
						screen.disableMultiListButton("CityBuildingSelectionMultiList", iRow, iCount, gc.getFatherPointInfo(i).getButton())
					iCount += 1
					bFound = True

		# GOVENOR MANAGMENT PANEL
			if AUTOMATION_MANAGMENT_PANEL_UP:
				screen.hide("InterfaceUnitModel")
				screen.hide("_FXS_Screen_Bogus_Minimap_Name")
				screen.hide("CityList")
				screen.hide("CityBuildingSelectionMultiList")
#TAC --->
				screen.hide("CityProductionBar")
				screen.hide("CityProductionBarFrame")
				screen.hide("HurryGold")
#<--- TAC
				screen.hide("MapHighlightButton")
				screen.hide("BuildHighlightButton")
				screen.show("GovernorHighlightButton")

			# EMPHASIZE TABLE
				iRow = 0
				screen.setTableText("EmphasizeTable", 0, iRow, self.setFontSize(localText.getText("TXT_KEY_EMPHASIZE", ()), 1), "", WidgetTypes.WIDGET_EMPHASIZE, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
				for iYield in EMPHASIZEYIELDS:
					iEmphasize = pHeadSelectedCity.AI_getEmphasizeYieldCount(iYield)
					if iEmphasize == 0:
						szText = localText.getText("TXT_KEY_EMPHASIS_NEUTRAL", (gc.getYieldInfo(iYield).getChar(), ))
					elif iEmphasize > 0:
						szText = localText.getText("TXT_KEY_EMPHASIZED", (gc.getYieldInfo(iYield).getChar(), ))
					else:
						szText = localText.getText("TXT_KEY_DEEMPHASIZED", (gc.getYieldInfo(iYield).getChar(), ))
					iRow += 1
					screen.setTableText("EmphasizeTable", 0, iRow, u"<font=3>%s</font>" % szText, "", WidgetTypes.WIDGET_EMPHASIZE, iYield, -1, CvUtil.FONT_LEFT_JUSTIFY )
				screen.show("EmphasizeTable")
				
				# R&R, Robert Surcouf, Custom House Popup-Screen START
				screen.setButtonGFC("ImportButton", self.setFontSize(localText.getText("TXT_KEY_DEMO_SCREEN_IMPORTS_TEXT", ()), 1), "", CITIZEN_BAR_WIDTH + (MAP_EDGE_MARGIN_WIDTH * 2), CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH, BUILD_AREA_WIDTH - MEDIUM_BUTTON_SIZE+20, 3 * STACK_BAR_HEIGHT / 2, WidgetTypes.WIDGET_YIELD_IMPORT_EXPORT, true, -1, ButtonStyles.BUTTON_STYLE_STANDARD)
				
				#screen.setButtonGFC("ImportButton", self.setFontSize(localText.getText("TXT_KEY_DEMO_SCREEN_IMPORTS_TEXT", ()), 1), "", CITIZEN_BAR_WIDTH + (MAP_EDGE_MARGIN_WIDTH * 2), CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH, BUILD_AREA_WIDTH - MEDIUM_BUTTON_SIZE, 3 * STACK_BAR_HEIGHT / 2, WidgetTypes.WIDGET_YIELD_IMPORT_EXPORT, true, -1, ButtonStyles.BUTTON_STYLE_STANDARD)
				# R&R, Robert Surcouf, Custom House Popup-Screen END
				screen.addTableControlGFC("ImportTradeRouteText", 1, CITIZEN_BAR_WIDTH + (MAP_EDGE_MARGIN_WIDTH * 2), CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH + 3 * STACK_BAR_HEIGHT / 2, BUILD_AREA_WIDTH, TRANSPORT_AREA_HEIGHT - (STACK_BAR_HEIGHT * 3), False, False, MEDIUM_BUTTON_SIZE, MEDIUM_BUTTON_SIZE, TableStyles.TABLE_STYLE_STANDARD)
				screen.setStyle("ImportTradeRouteText", "Table_EmptyScroll_Style")
				
				# R&R, Robert Surcouf, Custom House Popup-Screen START
				# R&R mod, vetiarvind, max yield import limit - start
				screen.setButtonGFC("ExportButton", self.setFontSize(localText.getText("TXT_KEY_DEMO_SCREEN_EXPORTS_TEXT", ()), 1), "", CITIZEN_BAR_WIDTH + (MAP_EDGE_MARGIN_WIDTH * 2) + BUILD_AREA_WIDTH, CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH, BUILD_AREA_WIDTH - MEDIUM_BUTTON_SIZE+20, 3 * STACK_BAR_HEIGHT / 2, WidgetTypes.WIDGET_CONDENSED_YIELD_IMPORT_EXPORT, false, -1, ButtonStyles.BUTTON_STYLE_STANDARD)				
				# R&R mod, vetiarvind, max yield import limit - end
				#screen.setButtonGFC("ExportButton", self.setFontSize(localText.getText("TXT_KEY_DEMO_SCREEN_EXPORTS_TEXT", ()), 1), "", CITIZEN_BAR_WIDTH + (MAP_EDGE_MARGIN_WIDTH * 2) + BUILD_AREA_WIDTH, CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH, BUILD_AREA_WIDTH - MEDIUM_BUTTON_SIZE, 3 * STACK_BAR_HEIGHT / 2, WidgetTypes.WIDGET_YIELD_IMPORT_EXPORT, false, -1, ButtonStyles.BUTTON_STYLE_STANDARD)
				# R&R, Robert Surcouf, Custom House Popup-Screen END
				screen.addTableControlGFC("ExportTradeRouteText", 1, CITIZEN_BAR_WIDTH + (MAP_EDGE_MARGIN_WIDTH * 2) + BUILD_AREA_WIDTH, CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH + 3 * STACK_BAR_HEIGHT / 2, BUILD_AREA_WIDTH, TRANSPORT_AREA_HEIGHT - (STACK_BAR_HEIGHT * 3), False, False, MEDIUM_BUTTON_SIZE, MEDIUM_BUTTON_SIZE, TableStyles.TABLE_STYLE_STANDARD)
				screen.setStyle("ExportTradeRouteText", "Table_EmptyScroll_Style")

				for iYield in range(YieldTypes.NUM_YIELD_TYPES):
					# transport feeder - start - Nightinggale
					iAutoThreshold = pHeadSelectedCity.getAutoMaintainThreshold(iYield)
					iThreshold     = pHeadSelectedCity.getMaintainLevel(iYield)
					szThreshold = ""
					if (iAutoThreshold > 0):
						szThreshold += " ("
						if (iThreshold < iAutoThreshold):
							szThreshold += unicode(iAutoThreshold) + "/"
						szThreshold += unicode(iThreshold) + ")"
					# transport feeder - end - Nightinggale
				
					if (pHeadSelectedCity.isExport(iYield)):
						iExportRow = screen.appendTableRow("ExportTradeRouteText")
						szExportText = u"<font=3>%c %s</font>" % (gc.getYieldInfo(iYield).getChar(), gc.getYieldInfo(iYield).getDescription())
						# transport feeder - start - Nightinggale
						#if pHeadSelectedCity.getMaintainLevel(iYield) > 0:
						#	szExportText += " (%s %d)" % (localText.getText("TXT_KEY_TRADE_ROUTE_MAINTAIN", ()), pHeadSelectedCity.getMaintainLevel(iYield))
						szExportText += szThreshold
						# transport feeder - end - Nightinggale
						screen.setTableText("ExportTradeRouteText", 0, iExportRow, u"<font=3>%s</font>" % szExportText, "", WidgetTypes.WIDGET_YIELD_IMPORT_EXPORT, false, -1, CvUtil.FONT_LEFT_JUSTIFY )

					if (pHeadSelectedCity.isImport(iYield)):
						iImportRow = screen.appendTableRow("ImportTradeRouteText")
						szImportText = u"<font=3>%c %s</font>" % (gc.getYieldInfo(iYield).getChar(), gc.getYieldInfo(iYield).getDescription())
						# R&R mod, vetiarvind, max yield import limit - start
						if pHeadSelectedCity.getImportsLimit(iYield) > 0:
							szImportText += " (%s %d)" % (localText.getText("TXT_KEY_TRADE_ROUTE_LIMIT", ()), pHeadSelectedCity.getImportsLimit(iYield))
						# R&R mod, vetiarvind, max yield import limit - end
						# transport feeder - start - Nightinggale
						# turn text green or red for feeder serviced yields
						if (pHeadSelectedCity.isImportFeeder(iYield)):
							if (pHeadSelectedCity.isAutoImportStopped(iYield)):
								szImportText = localText.getText("TXT_KEY_COLOR_NEGATIVE", ()) + szImportText
							else:
								szImportText = localText.getText("TXT_KEY_COLOR_POSITIVE", ()) + szImportText
							szImportText += szThreshold
							szImportText += localText.getText("TXT_KEY_COLOR_REVERT", ())
						# transport feeder - end - Nightinggale
						screen.setTableText("ImportTradeRouteText", 0, iImportRow, szImportText, "", WidgetTypes.WIDGET_YIELD_IMPORT_EXPORT, true, -1, CvUtil.FONT_LEFT_JUSTIFY )
				
				# teacher list - start - Nightinggale
				screen.setImageButton("TeacherListButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_TEACHER_LIST").getPath(), xResolution - (MAP_EDGE_MARGIN_WIDTH * 2) - 2 * CITY_MULTI_TAB_SIZE, CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2) + (STACK_BAR_HEIGHT * 3 / 2) + (MAP_EDGE_MARGIN_WIDTH / 2)-(CITY_MULTI_TAB_SIZE / 2), CITY_MULTI_TAB_SIZE, CITY_MULTI_TAB_SIZE, WidgetTypes.WIDGET_TEACHER_LIST, -1, -1)
				# teacher list - end - Nightinggale
				# R&R, Robert Surcouf, Custom House Popup-Screen START
				#screen.setButtonGFC("CustomHouseButton", self.setFontSize(localText.getText("TXT_KEY_EDIT_CUSTOM_HOUSE_TEXT", ()), 1), "", CITIZEN_BAR_WIDTH + (MAP_EDGE_MARGIN_WIDTH * 2) + BUILD_AREA_WIDTH/4, CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH + 50, BUILD_AREA_WIDTH - MEDIUM_BUTTON_SIZE+90, 3 * STACK_BAR_HEIGHT / 2, WidgetTypes.WIDGET_CUSTOM_HOUSE, false, -1, ButtonStyles.BUTTON_STYLE_STANDARD)
				screen.setImageButton("CustomHouseButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_CUSTOM_HOUSE").getPath(), xResolution - (MAP_EDGE_MARGIN_WIDTH * 2) - 2 * CITY_MULTI_TAB_SIZE, CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2) + (STACK_BAR_HEIGHT * 3 / 2) + (MAP_EDGE_MARGIN_WIDTH / 2)+(CITY_MULTI_TAB_SIZE / 2), CITY_MULTI_TAB_SIZE, CITY_MULTI_TAB_SIZE, WidgetTypes.WIDGET_CUSTOM_HOUSE, -1, -1)
				#screen.setButtonGFC("DomesticMarketButton", self.setFontSize(localText.getText("TXT_KEY_DOMESTIC_MARKET", ()), 1), "", CITIZEN_BAR_WIDTH + (MAP_EDGE_MARGIN_WIDTH * 2) + BUILD_AREA_WIDTH/4, CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH + 90, BUILD_AREA_WIDTH - MEDIUM_BUTTON_SIZE+90, 3 * STACK_BAR_HEIGHT / 2, WidgetTypes.WIDGET_DOMESTIC_MARKET, false, -1, ButtonStyles.BUTTON_STYLE_STANDARD)
				screen.setImageButton("DomesticMarketButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_DOMESTIC_MARKET").getPath(), xResolution - (MAP_EDGE_MARGIN_WIDTH * 2) - 2 * CITY_MULTI_TAB_SIZE, CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2) + (STACK_BAR_HEIGHT * 3 / 2) + (MAP_EDGE_MARGIN_WIDTH / 2)+ 3*(CITY_MULTI_TAB_SIZE / 2), CITY_MULTI_TAB_SIZE, CITY_MULTI_TAB_SIZE, WidgetTypes.WIDGET_DOMESTIC_MARKET, -1, -1)
				# R&R, Robert Surcouf, Custom House Popup-Screen END
				
		# BUILDING MANGMENT PANEL
			elif BUILDING_MANAGMENT_PANEL_UP:
				screen.hide("_FXS_Screen_Bogus_Minimap_Name")
				screen.hide("CityList")
				screen.setState("AutomateProduction", pHeadSelectedCity.isProductionAutomated())
				
				screen.hide("GovernorHighlightButton")
				screen.hide("MapHighlightButton")
				screen.show("BuildHighlightButton")

				screen.show("AutomateProduction")
				screen.show("CityBuildingSelectionMultiList")
#TAC --->
				screen.show("CityProductionBar")
				screen.show("CityProductionBarFrame")
				screen.show("HurryGold")
#<--- TAC				
				screen.hide("EmphasizeTable")

				if (CyInterface().shouldDisplayUnitModel() and CyInterface().getShowInterface() != InterfaceVisibility.INTERFACE_HIDE_ALL ):
					if (CyInterface().isCitySelection()):
						iOrders = CyInterface().getNumOrdersQueued()
						if (iOrders > 0):
							eOrderNodeType = CyInterface().getOrderNodeType(0)
							if (eOrderNodeType  == OrderTypes.ORDER_TRAIN):
								screen.addUnitGraphicGFC("InterfaceUnitModel", CyInterface().getOrderNodeData1(0), -1, CITIZEN_BAR_WIDTH + ((xResolution - CITIZEN_BAR_WIDTH) * 3 / 7) + (SMALL_BUTTON_SIZE * 2), CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2), BUILD_AREA_WIDTH, CITY_MULTI_TAB_AREA_HEIGHT + (MAP_EDGE_MARGIN_WIDTH * 2), WidgetTypes.WIDGET_HELP_SELECTED, -1, -1, -100, 0, 1, False )
							elif (eOrderNodeType == OrderTypes.ORDER_CONSTRUCT):
								screen.addBuildingGraphicGFC("InterfaceUnitModel", CyInterface().getOrderNodeData1(0), CITIZEN_BAR_WIDTH + ((xResolution - CITIZEN_BAR_WIDTH) * 3 / 7) + (SMALL_BUTTON_SIZE * 2), CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2), BUILD_AREA_WIDTH, CITY_MULTI_TAB_AREA_HEIGHT + (MAP_EDGE_MARGIN_WIDTH * 2), WidgetTypes.WIDGET_HELP_SELECTED, -1, -1, -20, 30, 0.9, False )
							screen.moveToFront("AutomateProduction")
						else:
							screen.hide("ProductionText")
							screen.hide("AutomateProduction")
						screen.show("CityBuildQueue")

		# MAP MANAGMENT PANEL
			elif MAP_MANAGMENT_PANEL_UP:
				screen.hide("InterfaceUnitModel")
				screen.hide("CityBuildingSelectionMultiList")
				screen.hide("EmphasizeTable")
#TAC --->
				screen.hide("CityProductionBar")
				screen.hide("CityProductionBarFrame")
				screen.hide("HurryGold")
#<--- TAC

				screen.addTableControlGFC("CityList", 2, CITIZEN_BAR_WIDTH + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - MAP_EDGE_MARGIN_WIDTH, CITY_TITLE_BAR_HEIGHT + (STACK_BAR_HEIGHT / 2), BUILD_AREA_WIDTH, BUILD_AREA_HEIGHT - STACK_BAR_HEIGHT, False, False, self.SELECTION_PANEL_ROW_HEIGHT, self.SELECTION_PANEL_ROW_HEIGHT, TableStyles.TABLE_STYLE_STANDARD)
				screen.setStyle("CityList", "Table_EmptyScroll_Style")

				screen.setTableColumnHeader("CityList", 0, u"", BUILD_AREA_WIDTH - (BUILD_AREA_WIDTH * 3 / 10))
				screen.setTableColumnHeader("CityList", 1, u"", BUILD_AREA_WIDTH * 3 / 10)
				screen.appendTableRow("CityList")

				pPlayer = gc.getPlayer(gc.getGame().getActivePlayer())
				iRow = 0
				for iCity in range(gc.getActivePlayer().getNumCities()):
					pCity = pPlayer.getCity(iCity)
					if (pCity.getID() != pHeadSelectedCity.getID()):
						screen.appendTableRow("CityList")
						screen.setTableText("CityList", 0, iRow, pCity.getName(), "", WidgetTypes.WIDGET_GOTO_CITY, iCity, -1, CvUtil.FONT_LEFT_JUSTIFY )
						screen.setTableText("CityList", 1, iRow, u" (%d)" %(pCity.getPopulation()), "", WidgetTypes.WIDGET_GOTO_CITY, iCity, -1, CvUtil.FONT_RIGHT_JUSTIFY )
						iRow += 1

				screen.show("CityList")
				screen.show("_FXS_Screen_Bogus_Minimap_Name")
				
				screen.hide("GovernorHighlightButton")
				screen.hide("BuildHighlightButton")
				screen.show("MapHighlightButton")

		elif (not CyEngine().isGlobeviewUp() and pHeadSelectedUnit and CyInterface().getShowInterface() != InterfaceVisibility.INTERFACE_HIDE_ALL and CyInterface().getShowInterface() != InterfaceVisibility.INTERFACE_MINIMAP_ONLY):
			if pHeadSelectedCity and not CyInterface().isCityScreenUp():
				self.setMinimapButtonVisibility(False)
			else:
				self.setMinimapButtonVisibility(True)
				
			if (CyInterface().getInterfaceMode() == InterfaceModeTypes.INTERFACEMODE_SELECTION):
				if (pHeadSelectedUnit.getOwner() == gc.getGame().getActivePlayer() and g_pSelectedUnit != pHeadSelectedUnit):
					g_pSelectedUnit = pHeadSelectedUnit
					iCount = 0
					actions = CyInterface().getActionsToShow()
					self.SADDLE_BUTTON_SIZE = LARGE_BUTTON_SIZE * 2

					iTotalActions = 0
					if CyInterface().canCreateGroup():
						iTotalActions += 1
					if CyInterface().canDeleteGroup():
						iTotalActions += 1
					iTotalActions += len(actions)

					ACTION_BUTTON_X_BEGIN = xResolution - RIGHT_PANEL_WIDTH
					ACTION_BUTTON_X_STEP = LARGE_BUTTON_SIZE * 3 / 2
					
					iMaxSpace = ACTION_BUTTON_X_BEGIN - SADDLE_HEIGHT
					iSpaceNeeded = len(range(iTotalActions)) * ACTION_BUTTON_X_STEP
					
					if iSpaceNeeded > iMaxSpace:
						ACTION_BUTTON_X_STEP = ACTION_BUTTON_X_STEP * iMaxSpace / iSpaceNeeded
						
					ActionButtonList = []
					for i in actions:
						if CyInterface().canHandleAction(i, True):
							Xcord = ACTION_BUTTON_X_BEGIN - iCount * ACTION_BUTTON_X_STEP - LARGE_BUTTON_SIZE
							Ycord = yResolution - SADDLE_HEIGHT / 8
							szName = "ActionButton" + str(iCount)
							screen.setImageButton(szName, gc.getActionInfo(i).getButton(), int(Xcord) - LARGE_BUTTON_SIZE, int(Ycord) - LARGE_BUTTON_SIZE, LARGE_BUTTON_SIZE * 2, LARGE_BUTTON_SIZE * 2, WidgetTypes.WIDGET_ACTION, i, -1)
							screen.setImageShape(szName, ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
							screen.setHitMargins(szName, 18, 18)
							if not CyInterface().canHandleAction(i, False):
								screen.overlayButtonGFC(szName, ArtFileMgr.getInterfaceArtInfo("INTERFACE_DISABLED_BUTTON").getPath())
							elif pHeadSelectedUnit.isActionRecommended(i):
								screen.overlayButtonGFC(szName, ArtFileMgr.getInterfaceArtInfo("INTERFACE_HIGHLIGHTED_BUTTON").getPath())
							ActionButtonList.append(szName)

							iCount += 1

					if (CyInterface().canCreateGroup()):
						Xcord = ACTION_BUTTON_X_BEGIN - iCount * ACTION_BUTTON_X_STEP - LARGE_BUTTON_SIZE
						Ycord = yResolution - SADDLE_HEIGHT / 8
						screen.setImageButton("CreateGroupButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_CREATEGROUP").getPath(), int(Xcord) - LARGE_BUTTON_SIZE, int(Ycord) - LARGE_BUTTON_SIZE, LARGE_BUTTON_SIZE * 2, LARGE_BUTTON_SIZE * 2, WidgetTypes.WIDGET_CREATE_GROUP, -1, -1)
						screen.setImageShape("CreateGroupButton", ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
						screen.setHitMargins("CreateGroupButton", 18, 18)
						ActionButtonList.append("CreateGroupButton")

						iCount += 1

					if (CyInterface().canDeleteGroup()):
						Xcord = ACTION_BUTTON_X_BEGIN - iCount * ACTION_BUTTON_X_STEP - LARGE_BUTTON_SIZE
						Ycord = yResolution - SADDLE_HEIGHT / 8
						screen.setImageButton("DeleteGroupButton", ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_SPLITGROUP").getPath(), int(Xcord) - LARGE_BUTTON_SIZE, int(Ycord) - LARGE_BUTTON_SIZE, LARGE_BUTTON_SIZE * 2, LARGE_BUTTON_SIZE * 2, WidgetTypes.WIDGET_DELETE_GROUP, -1, -1)
						screen.setImageShape("DeleteGroupButton", ImageShapes.IMAGE_SHAPE_ELLIPSE, -1)
						screen.setHitMargins("DeleteGroupButton", 18, 18)
						ActionButtonList.append("DeleteGroupButton")

						iCount += 1

					screen.registerHideList(ActionButtonList, len(ActionButtonList), ACTION_BUTTON_HIDE)

		elif (CyInterface().getShowInterface() != InterfaceVisibility.INTERFACE_HIDE_ALL and CyInterface().getShowInterface() != InterfaceVisibility.INTERFACE_MINIMAP_ONLY):
			screen.hide("CityBuildingSelectionMultiList")
			screen.hide("EmphasizeTable")
			self.setMinimapButtonVisibility(True)

		return 0
	# Will update the citizen buttons
	def updateCitizenButtons( self ):
		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE )
		xResolution = screen.getXResolution()
		yResolution = screen.getYResolution()

		screen.hideList(CITIZEN_HIDE)

		if (CyInterface().isCityScreenUp()):
			CitizenHideList = []

		# ON PLOT WORKERS
###=====City Radius 2 (NeverMind)===1/2=====###
			city_radius = gc.getCityDiameter()
			pHeadSelectedCity = CyInterface().getHeadSelectedCity()
			ButtonSize = (CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 4 ))  / city_radius 
			
			if (pHeadSelectedCity.getPopulation() == 0): # TAC abandon city
				return # TAC abandon city- Required to correct graphical bugginess when viewing a pop 0 city
			
			if (pHeadSelectedCity and CyInterface().getShowInterface() == InterfaceVisibility.INTERFACE_SHOW):
				for iPlotIndex in range(gc.getNUM_CITY_PLOTS()):
					# removed an if statement. Added a bogus one to avoid touching all lines due to indenting
					if 1:
						pUnit = pHeadSelectedCity.getUnitWorkingPlot(iPlotIndex)
						pPlot = pHeadSelectedCity.getCityIndexPlot(iPlotIndex)
						if not pPlot.isNone():
							worldPosition = pPlot.getPoint()
							worldPosition.x -= gc.getPLOT_SIZE() / 2 
							# Two Plots, Robert Surcouf, Screen yResolution - Start
							if (city_radius == 5 and (xResolution * 10) / yResolution > 15): 
								worldPosition.x +=(xResolution * 13)/yResolution
								worldPosition.y -=(xResolution * 13)/yResolution
							# Two Plots, Robert Surcouf, Screen yResolution - End
							screenPosition = CyEngine().worldPointToScreenPoint(worldPosition)

							# PLOT DRAG ON PANELS
###=====City Radius 2 (NeverMind)===2/2=====###

							x = int(screenPosition.x)
							y =	int(screenPosition.y) - ButtonSize / (city_radius - 1)
							size_x = CITY_VIEW_BOX_HEIGHT_AND_WIDTH / city_radius
							size_y = CITY_VIEW_BOX_HEIGHT_AND_WIDTH / city_radius
							
							if iPlotIndex == gc.getCITY_HOME_PLOT():
								screen.setImageButton("PlotDragOn" + str(iPlotIndex), "", x, y, size_x, size_y, WidgetTypes.WIDGET_CITY_CENTER_PLOT, -1, -1)
								CitizenHideList.append("PlotDragOn" + str(iPlotIndex))
								continue
								
							screen.addDDSGFC("PlotDragOn" + str(iPlotIndex), "", x, y, size_x, size_y, WidgetTypes.WIDGET_ASSIGN_CITIZEN_TO_PLOT, iPlotIndex, -1) 
							CitizenHideList.append("PlotDragOn" + str(iPlotIndex))

							# city plot mouse over help - inaiwae - START
							screen.addDDSGFC("CityPlotInfo" + str(iPlotIndex), "", x-(size_x/32), y-(size_y/4), size_x, size_y, WidgetTypes.WIDGET_CITY_PLOT_INFO, iPlotIndex, -1) 
							CitizenHideList.append("CityPlotInfo" + str(iPlotIndex))
							# city plot mouse over help - inaiwae - END

							if (not pUnit.isNone()):
								if (pUnit.isColonistLocked()):
									szName = "CitizenButtonLock" + str(pUnit.getID())
									screen.addDDSGFC(szName, ArtFileMgr.getInterfaceArtInfo("INTERFACE_CITY_CITIZEN_LOCK").getPath(), int(screenPosition.x) - 5, int(screenPosition.y) - ButtonSize / 2 - 5, STACK_BAR_HEIGHT * 3 / 2, STACK_BAR_HEIGHT * 3 / 2, WidgetTypes.WIDGET_GENERAL, -1, -1)
									CitizenHideList.append(szName)

								szName = "WorkerButton" + str(iPlotIndex)
								screen.addDragableButton(szName, pUnit.getFullLengthIcon(), "", int(screenPosition.x), int(screenPosition.y) - ButtonSize / 2, ButtonSize / 2, ButtonSize, WidgetTypes.WIDGET_CITIZEN, pUnit.getID(), -1, ButtonStyles.BUTTON_STYLE_IMAGE )
								CitizenHideList.append(szName)
							elif (pPlot.getOwner() == pHeadSelectedCity.getOwner()):
								szName = "WorkerSlot" + str(iPlotIndex)
								screen.addDDSGFC(szName, ArtFileMgr.getInterfaceArtInfo("INTERFACE_CITIZEN_SLOT").getPath(), int(screenPosition.x), int(screenPosition.y) - ButtonSize / 2, ButtonSize / 2, ButtonSize, WidgetTypes.WIDGET_ASSIGN_CITIZEN_TO_PLOT, iPlotIndex, -1)
								CitizenHideList.append(szName)

		# IN CITY WORKERS
			pHeadSelectedCity = CyInterface().getHeadSelectedCity()
			pHeadSelectedUnit = CyInterface().getHeadSelectedUnit()
			
			if (pHeadSelectedCity and CyInterface().getShowInterface() == InterfaceVisibility.INTERFACE_SHOW):
				CitizenProfessionIndexArray = []
				CitizenSpecialBuildingIndexArray = []
				CitizenBarCount = 0
				for iProfession in range(gc.getNumProfessionInfos()):
					CitizenProfessionIndexArray.append([])
						
				for iSpecialBuilding in range(gc.getNumSpecialBuildingInfos()):
					CitizenSpecialBuildingIndexArray.append([])
					
				for j in range(pHeadSelectedCity.getPopulation()):
					# R&R, Robert Surcouf, Rebellion Fix START
					pCitizen = pHeadSelectedCity.getPopulationUnitByIndex(j)
					if pCitizen.getProfession() != ProfessionTypes.NO_PROFESSION:
						CitizenProfessionIndexArray[pCitizen.getProfession()].append(pCitizen)
						CitizenSpecialBuildingIndexArray[gc.getProfessionInfo(pCitizen.getProfession()).getSpecialBuilding()].append(pCitizen)
					CitizenBarCount += 1
					# R&R, Robert Surcouf, Rebellion Fix START
#MultipleYieldsProduced Start
				for iProfession in range(gc.getNumProfessionInfos()):
					if (not gc.getProfessionInfo(iProfession).isWorkPlot() and gc.getProfessionInfo(iProfession).isCitizen()):
						if gc.getCivilizationInfo(pHeadSelectedCity.getCivilizationType()).isValidProfession(iProfession):
							iSpecialBuildingType = gc.getProfessionInfo(iProfession).getSpecialBuilding()
							iYield = gc.getProfessionInfo(iProfession).getYieldsProduced(0)
							CityBuilding = -1
#MultipleYieldsProduced End
					
				for iBuilding in range(gc.getNumBuildingInfos()):
					if (pHeadSelectedCity.isHasBuilding(iBuilding)):
						iSpecialBuildingType = gc.getBuildingInfo(iBuilding).getSpecialBuildingType()
						CityBuilding = iBuilding
						
						iProfession = -1
						iSecondYield = -1
						bFirstYield = true
						if (gc.getBuildingInfo(CityBuilding).getMaxWorkers() >= 0):
							for iArrayProfession in range(gc.getNumProfessionInfos()):
								if (gc.getProfessionInfo(iArrayProfession).getSpecialBuilding() == iSpecialBuildingType):
									if (not gc.getProfessionInfo(iArrayProfession).isWorkPlot() and gc.getProfessionInfo(iArrayProfession).isCitizen()):
										if gc.getCivilizationInfo(pHeadSelectedCity.getCivilizationType()).isValidProfession(iArrayProfession):
											if (bFirstYield == true):
												iYield = gc.getProfessionInfo(iArrayProfession).getYieldsProduced(0) #MultipleYieldsProduced Start
												iProfession = iArrayProfession
												bFirstYield = false
											else:
												iSecondYield = gc.getProfessionInfo(iArrayProfession).getYieldsProduced(0) #MultipleYieldsProduced Start
							
							if (CityBuilding != -1 and iProfession != -1):
								ButtonSize = LARGE_BUTTON_SIZE * 5 / 2

								
								bHasYield = (pHeadSelectedCity.getBaseRawYieldProduced(iYield) != 0 or pHeadSelectedCity.getRawYieldConsumed(iYield) != 0)

								if (gc.getBuildingInfo(CityBuilding).getMaxWorkers() > 0):
									CitizenSpacing = BUILDING_GRID[iSpecialBuildingType][3] / gc.getBuildingInfo(iBuilding).getMaxWorkers()
								else:
									CitizenSpacing = ButtonSize / 2

								#RWL for SpecialBuilding Trade	
								if (CityBuilding != -1):
									screen.show("ProductionBox" + str(iSpecialBuildingType))
									szName = "YieldOutPutIcon" + str(iYield)
									if (iSecondYield < 0 or iSecondYield >= YieldTypes.NUM_YIELD_TYPES or iSecondYield == iYield):
										screen.addDDSGFC(szName, gc.getYieldInfo(iYield).getIcon(), BUILDING_GRID[iSpecialBuildingType][0] + (STACK_BAR_HEIGHT / 2), BUILDING_GRID[iSpecialBuildingType][1] + BUILDING_GRID[iSpecialBuildingType][2] - (BUILDING_GRID[iSpecialBuildingType][2] / 6) + STACK_BAR_HEIGHT, STACK_BAR_HEIGHT * 3 / 2, STACK_BAR_HEIGHT * 3 / 2, WidgetTypes.WIDGET_HELP_YIELD, iYield, -1)
									else:
										# R&R, we do not show the first icon any more, if we have MultipleProfessionsPerBuilding
										# screen.addDDSGFC(szName, gc.getYieldInfo(iYield).getIcon(), BUILDING_GRID[iSpecialBuildingType][0] + (STACK_BAR_HEIGHT / 2), BUILDING_GRID[iSpecialBuildingType][1] + BUILDING_GRID[iSpecialBuildingType][2] - (BUILDING_GRID[iSpecialBuildingType][2] / 6) + STACK_BAR_HEIGHT, STACK_BAR_HEIGHT * 3 / 2, STACK_BAR_HEIGHT * 3 / 2, WidgetTypes.WIDGET_HELP_TWO_YIELDS, iYield, iSecondYield)
										szSecondName = "SecondYieldOutPutIcon" + str(iSecondYield) #the second icon must not use "szName", or it replaces the former icon
										screen.addDDSGFC(szSecondName, gc.getYieldInfo(iSecondYield).getCombiIcon(), BUILDING_GRID[iSpecialBuildingType][0] + (STACK_BAR_HEIGHT / 2), BUILDING_GRID[iSpecialBuildingType][1] + BUILDING_GRID[iSpecialBuildingType][2] - (BUILDING_GRID[iSpecialBuildingType][2] / 6) + STACK_BAR_HEIGHT, STACK_BAR_HEIGHT * 3 / 2, STACK_BAR_HEIGHT * 3 / 2, WidgetTypes.WIDGET_HELP_TWO_YIELDS, iYield, iSecondYield)
										CitizenHideList.append(szSecondName)
									CitizenHideList.append(szName)
									
								else:
									screen.hide("ProductionBox" + str(iSpecialBuildingType))

								ProfessionCount = len(CitizenSpecialBuildingIndexArray[iSpecialBuildingType])
								for GroupIndex in range(ProfessionCount):
									pCitizen = CitizenSpecialBuildingIndexArray[iSpecialBuildingType][GroupIndex]

									if (pCitizen.isColonistLocked()):
										szName = "CitizenButtonLock" + str(pCitizen.getID())
										screen.addDDSGFC(szName, ArtFileMgr.getInterfaceArtInfo("INTERFACE_CITY_CITIZEN_LOCK").getPath(), BUILDING_GRID[iSpecialBuildingType][0] + (CitizenSpacing * GroupIndex) + (CitizenSpacing / 2) - (STACK_BAR_HEIGHT / 3), BUILDING_GRID[iSpecialBuildingType][1] + BUILDING_GRID[iSpecialBuildingType][2] - (ButtonSize) - (STACK_BAR_HEIGHT * 2 / 3), STACK_BAR_HEIGHT * 4 / 3, STACK_BAR_HEIGHT * 4 / 3, WidgetTypes.WIDGET_GENERAL, -1, -1)
										CitizenHideList.append(szName)

									szName = "CitizenButton" + str(iProfession) + "-" + str(GroupIndex)
									screen.addDragableButton(szName, pCitizen.getFullLengthIcon(), "", BUILDING_GRID[iSpecialBuildingType][0] + (CitizenSpacing * GroupIndex) + (CitizenSpacing/ 2), BUILDING_GRID[iSpecialBuildingType][1] + BUILDING_GRID[iSpecialBuildingType][2] - (ButtonSize), ButtonSize / 2, ButtonSize, WidgetTypes.WIDGET_CITIZEN, pCitizen.getID(), -1, ButtonStyles.BUTTON_STYLE_IMAGE)
									CitizenHideList.append(szName)

								for iSlot in range (gc.getBuildingInfo(CityBuilding).getMaxWorkers() - ProfessionCount):
									szName = "CitizenSlot" + str(iProfession) + "-" + str(iSlot)
									screen.addDDSGFC(szName, ArtFileMgr.getInterfaceArtInfo("INTERFACE_CITIZEN_SLOT").getPath(), BUILDING_GRID[iSpecialBuildingType][0] + (CitizenSpacing * (iSlot + ProfessionCount)) + (CitizenSpacing/ 2), BUILDING_GRID[iSpecialBuildingType][1] + BUILDING_GRID[iSpecialBuildingType][2] - ButtonSize, ButtonSize / 2, ButtonSize, WidgetTypes.WIDGET_ASSIGN_CITIZEN_TO_BUILDING, -1, CityBuilding)
									CitizenHideList.append(szName)

								SzText = ""
								bTwoYields = false
								for iArrayProfession in range(gc.getNumProfessionInfos()):
									if (gc.getProfessionInfo(iArrayProfession).getSpecialBuilding() == iSpecialBuildingType):
										if (not gc.getProfessionInfo(iArrayProfession).isWorkPlot() and gc.getProfessionInfo(iArrayProfession).isCitizen()):
											if gc.getCivilizationInfo(pHeadSelectedCity.getCivilizationType()).isValidProfession(iArrayProfession):
												iAmountYield = gc.getProfessionInfo(iArrayProfession).getYieldsProduced(0) #MultipleYieldsProduced Start
												ProducedYield = pHeadSelectedCity.getBaseRawYieldProduced(iAmountYield)
												UnproducedYield = ProducedYield - pHeadSelectedCity.calculateActualYieldProduced(iAmountYield)
												if (bTwoYields == false):
													if (ProducedYield > 0):
														bTwoYields = true
														SzText += u"<color=0,255,0> +" + str(ProducedYield) + "</color>"
													if (UnproducedYield > 0):
														SzText += u"<color=255,0,0> -" + str(UnproducedYield) + "</color>"
												else:
													if (iAmountYield != iYield):
														if (ProducedYield > 0):
															SzText += u"<color=255,255,255> /" + "</color>"
															SzText += u"<color=0,255,0> +" + str(ProducedYield) + "</color>"
														if (UnproducedYield > 0):
															SzText += u"<color=255,0,0> -" + str(UnproducedYield) + "</color>"

								szName = "WorkerOutputText" + str(iYield)
								if (iSecondYield == -1 or iSecondYield == iYield):
									screen.setLabelAt(szName, "ProductionBox" + str(iSpecialBuildingType), self.setFontSize(SzText , 0), CvUtil.FONT_RIGHT_JUSTIFY, (BUILDING_GRID[iSpecialBuildingType][3] + STACK_BAR_HEIGHT) *2 / 3, STACK_BAR_HEIGHT *2 /5, -1.3, FontTypes.GAME_FONT, WidgetTypes.WIDGET_HELP_YIELD, iYield, -1)
								else:
									screen.setLabelAt(szName, "ProductionBox" + str(iSpecialBuildingType), self.setFontSize(SzText , 0), CvUtil.FONT_RIGHT_JUSTIFY, (BUILDING_GRID[iSpecialBuildingType][3] + STACK_BAR_HEIGHT) *2 / 3, STACK_BAR_HEIGHT *2 /5, -1.3, FontTypes.GAME_FONT, WidgetTypes.WIDGET_HELP_TWO_YIELDS, iYield, iSecondYield)

								CitizenHideList.append(szName)
								CitizenHideList.append("ProductionBox" + str(iSpecialBuildingType)) # R&R, Robert Surcouf Fix Production box
							else:
								screen.hide("ProductionBox" + str(iSpecialBuildingType)) 
				#Androrc End
					
				iSpace = CITIZEN_BAR_WIDTH - (LARGE_BUTTON_SIZE * 3)
				iSeperation = iSpace / CitizenBarCount
				if (iSeperation > (LARGE_BUTTON_SIZE * 2)):
					iSeperation = (LARGE_BUTTON_SIZE * 2)
				iCount = 0
	
				for iProfession in range(gc.getNumProfessionInfos()):
					for GroupIndex in range(len(CitizenProfessionIndexArray[iProfession])):
						pCitizen = CitizenProfessionIndexArray[iProfession][GroupIndex]
						szName = "PopulationButton" + str(iCount)
						ButtonSize = LARGE_BUTTON_SIZE * 3
						screen.show(szName)
						screen.setState(szName, pCitizen.isColonistLocked())
						CitizenHideList.append(szName)
						iCount += 1

			screen.registerHideList(CitizenHideList, len(CitizenHideList), CITIZEN_HIDE)
			self.updateGarrisonAndTransports()
			self.updateResourceTable()

		return 0

	# Will update the Garrison and Transport Panels
	def updateGarrisonAndTransports( self ):
	
		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE )
		pHeadSelectedCity = CyInterface().getHeadSelectedCity()

#TAC --->
	# TRANSPORT MANAGMENT
		TransportButtonSize = LARGE_BUTTON_SIZE * 4 / 3
		CargoButtonSize = MEDIUM_BUTTON_SIZE
		yLocation = STACK_BAR_HEIGHT	* 3 / 2
		PanelHeight = TransportButtonSize * 6 / 5
		CyInterface().cacheInterfacePlotUnits(pHeadSelectedCity.plot())		
		#screen.addScrollPanel("CityGarrisonPanel", u"", CITIZEN_BAR_WIDTH + (SMALL_BUTTON_SIZE / 2), yResolution - BOTTOM_CENTER_HUD_HEIGHT - TRANSPORT_AREA_HEIGHT * 9 / 8 - 2, xResolution - CITIZEN_BAR_WIDTH - TRANSPORT_AREA_WIDTH + STACK_BAR_HEIGHT, TRANSPORT_AREA_HEIGHT + (STACK_BAR_HEIGHT / 3), PanelStyles.PANEL_STYLE_EMPTY, false, WidgetTypes.WIDGET_EJECT_CITIZEN, -1, -1 )	
		#screen.addScrollPanel("CityTransportPanel", u"", xResolution - TRANSPORT_AREA_WIDTH + MAP_EDGE_MARGIN_WIDTH - SMALL_BUTTON_SIZE / 2, yResolution - BOTTOM_CENTER_HUD_HEIGHT - TRANSPORT_AREA_HEIGHT * 9 / 8 - 2, TRANSPORT_AREA_WIDTH * 126 / 128, TRANSPORT_AREA_HEIGHT + (STACK_BAR_HEIGHT / 3), PanelStyles.PANEL_STYLE_MAIN, false, WidgetTypes.WIDGET_RECEIVE_MOVE_CARGO_TO_CITY, -1, -1 )
		screen.setLabelAt("TransportPanelLabel", "CityTransportPanel", "    " + self.setFontSize((localText.getColorText("TXT_KEY_TRANSPORT_PANEL", (), gc.getInfoTypeForString("COLOR_FONT_CREAM"))).upper(), 0), CvUtil.FONT_LEFT_JUSTIFY, 0, STACK_BAR_HEIGHT / 2, -1.3, FontTypes.GAME_FONT, WidgetTypes.WIDGET_RECEIVE_MOVE_CARGO_TO_CITY, -1, -1 )
		
		global LAST_CACHED_TRANSPORT_UNITS_COUNT
		for i in range(LAST_CACHED_TRANSPORT_UNITS_COUNT):
			screen.hide("VisitingShipPanel" + str(i))
			screen.hide("VisitingShipIcon" + str(i))
			screen.hide("TransportAutomated" + str(i))
			for j in range(10):
				screen.hide("CargoCell" + str(i) + "-" + str(j))
			screen.hide("CargoUnit" + str(i))
			screen.hide("CargoCount" + str(i))	
		LAST_CACHED_TRANSPORT_UNITS_COUNT = CyInterface().getNumCachedInterfacePlotUnits()
				
		for i in range(CyInterface().getNumCachedInterfacePlotUnits()):
			pLoopUnit = CyInterface().getCachedInterfacePlotUnit(i)
			if (pLoopUnit and pLoopUnit.getOwner() == pHeadSelectedCity.getOwner()):
				if (pLoopUnit.cargoSpace() > 0 and not pLoopUnit.isCargo()):
					visitingShipWidth = TransportButtonSize + pLoopUnit.cargoSpace() * (CargoButtonSize * 11 / 10)
					xPosition = (TRANSPORT_AREA_WIDTH * 116 / 128 - visitingShipWidth) / 2
					screen.addDDSGFCAt("VisitingShipPanel" + str(i), "CityTransportPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_EUROPE_IN_PORT_BOX").getPath(), xPosition, yLocation, TransportButtonSize + pLoopUnit.cargoSpace() * (CargoButtonSize * 11 / 10), PanelHeight, WidgetTypes.WIDGET_RECEIVE_MOVE_CARGO_TO_TRANSPORT, pLoopUnit.getID(), -1, False)
					screen.addCheckBoxGFCAt("CityTransportPanel", "VisitingShipIcon" + str(i), pLoopUnit.getButton(), "", xPosition, yLocation + (PanelHeight / 2) - (TransportButtonSize / 2), TransportButtonSize, TransportButtonSize, WidgetTypes.WIDGET_RECEIVE_MOVE_CARGO_TO_TRANSPORT, pLoopUnit.getID(), -1, ButtonStyles.BUTTON_STYLE_LABEL)
					if pLoopUnit.isAutomated():
						screen.addDDSGFCAt("TransportAutomated" + str(i), "CityTransportPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_AUTOMATE").getPath(), xPosition + (CargoButtonSize * 3 / 4), yLocation + (PanelHeight / 2) - (TransportButtonSize / 2) + (CargoButtonSize * 3 / 4), TransportButtonSize * 3 / 4, TransportButtonSize * 3 / 4, WidgetTypes.WIDGET_RECEIVE_MOVE_CARGO_TO_TRANSPORT, pLoopUnit.getID(), -1, False)
					yLocation += (PanelHeight / 2) - (CargoButtonSize / 2)
					xPosition += TransportButtonSize 
					
					for j in range(pLoopUnit.cargoSpace()):
						screen.addDDSGFCAt("CargoCell" + str(i) + "-" + str(j), "CityTransportPanel", ArtFileMgr.getInterfaceArtInfo("INTERFACE_EUROPE_BOX_CARGO").getPath(), xPosition + (j * (CargoButtonSize * 11 / 10)), yLocation - (CargoButtonSize / 4), CargoButtonSize, CargoButtonSize, WidgetTypes.WIDGET_RECEIVE_MOVE_CARGO_TO_TRANSPORT, pLoopUnit.getID(), -1, False)

					for j in range(CyInterface().getNumCachedInterfacePlotUnits()):
						CargoUnit = CyInterface().getCachedInterfacePlotUnit(j)
						transportUnit = CargoUnit.getTransportUnit()
						if (not transportUnit.isNone() and transportUnit.getID() == pLoopUnit.getID()):
							iYield = CargoUnit.getYield()
							screen.addDragableButtonAt("CityTransportPanel", "CargoUnit" + str(j), CargoUnit.getButton(), "", xPosition, yLocation - (CargoButtonSize / 4), CargoButtonSize, CargoButtonSize, WidgetTypes.WIDGET_MOVE_CARGO_TO_CITY, pLoopUnit.getID(), CargoUnit.getID(), ButtonStyles.BUTTON_STYLE_LABEL)
							if CargoUnit.isGoods():
								szText = u"<font=3>%s</font>" % CargoUnit.getYieldStored()
								screen.setLabelAt("CargoCount" + str(j), "CityTransportPanel", szText, CvUtil.FONT_CENTER_JUSTIFY, xPosition + (CargoButtonSize / 2), yLocation + (TransportButtonSize * 4 / 6), -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
							xPosition += (CargoButtonSize * 11 / 10)
					yLocation += (PanelHeight / 2) + (CargoButtonSize / 2) + (CargoButtonSize / 10)

	# GARISSON MANAGMENT PANEL
		GarrisonButtonSize = LARGE_BUTTON_SIZE
		self.PLOT_LIST_ICON_SIZE = 12
		self.PLOT_LIST_HEALTH_BAR_HEIGHT = 11
		self.PLOT_LIST_HEALTH_BAR_SHORTENING_CORRECTION = 6
		CyInterface().cacheInterfacePlotUnits(pHeadSelectedCity.plot())
		#screen.addScrollPanel("CityGarrisonPanel", u"", CITIZEN_BAR_WIDTH + (SMALL_BUTTON_SIZE / 8), yResolution - BOTTOM_CENTER_HUD_HEIGHT - TRANSPORT_AREA_HEIGHT * 9 / 8 - 2, xResolution - CITIZEN_BAR_WIDTH - TRANSPORT_AREA_WIDTH + STACK_BAR_HEIGHT * 4 / 8, TRANSPORT_AREA_HEIGHT + (STACK_BAR_HEIGHT / 3), PanelStyles.PANEL_STYLE_EMPTY, false, WidgetTypes.WIDGET_EJECT_CITIZEN, -1, -1 )	
		screen.setLabelAt("CityGarrisonLabel", "CityGarrisonPanel", self.setFontSize((localText.getColorText("TXT_KEY_GARRISON_PANEL", (), gc.getInfoTypeForString("COLOR_FONT_CREAM"))).upper(), 0), CvUtil.FONT_LEFT_JUSTIFY, SMALL_BUTTON_SIZE / 4, STACK_BAR_HEIGHT / 2, -1.3, FontTypes.GAME_FONT, WidgetTypes.WIDGET_EJECT_CITIZEN, -1, -1 )
		
		global LAST_CACHED_GARRISON_UNITS_COUNT
		for i in range(LAST_CACHED_GARRISON_UNITS_COUNT):
			screen.hide("GarrisonUnit" + str(i))
			screen.hide("GarrisonHealth" + str(i))
			screen.hide("GarrisonMoveIcon" + str(i))
		LAST_CACHED_GARRISON_UNITS_COUNT = CyInterface().getNumCachedInterfacePlotUnits()
		
		xPosition = 0
		GarrisonUnitsWidth = GarrisonButtonSize * 3
		xPositionBase = (xResolution - CITIZEN_BAR_WIDTH - TRANSPORT_AREA_WIDTH - STACK_BAR_HEIGHT - GarrisonUnitsWidth) / 2
				
		yPosition = 0
		for i in range(CyInterface().getNumCachedInterfacePlotUnits()):
			pLoopUnit = CyInterface().getCachedInterfacePlotUnit(i)
			if (pLoopUnit):
				if (pLoopUnit.getOwner() == pHeadSelectedCity.getOwner() and pLoopUnit.cargoSpace() == 0):
					if (pLoopUnit.getYield() == YieldTypes.NO_YIELD ):
						if (pLoopUnit.getTransportUnit().isNone()):
							cityGarrisonX = xPosition * GarrisonButtonSize + xPositionBase
#<--- TAC
							cityGarrisonY = STACK_BAR_HEIGHT * 3 / 2 + (yPosition * (GarrisonButtonSize + self.PLOT_LIST_HEALTH_BAR_SHORTENING_CORRECTION))
							screen.addDragableButtonAt("CityGarrisonPanel", "GarrisonUnit" + str(i), pLoopUnit.getButton(), "", cityGarrisonX, cityGarrisonY, GarrisonButtonSize, GarrisonButtonSize, WidgetTypes.WIDGET_EJECT_CITIZEN, pLoopUnit.getID(), -1, ButtonStyles.BUTTON_STYLE_LABEL)
		
						# HEALTH BAR
							HealthBar = "GarrisonHealth" + str(i)
							screen.addStackedBarGFCAt( HealthBar, "CityGarrisonPanel", cityGarrisonX, cityGarrisonY + GarrisonButtonSize - (self.PLOT_LIST_HEALTH_BAR_HEIGHT / 2), GarrisonButtonSize, self.PLOT_LIST_HEALTH_BAR_HEIGHT, InfoBarTypes.NUM_INFOBAR_TYPES, WidgetTypes.WIDGET_EJECT_CITIZEN, pLoopUnit.getID(), -1 )
							screen.setBarPercentage( HealthBar, InfoBarTypes.INFOBAR_STORED, float( pLoopUnit.currHitPoints() ) / float( pLoopUnit.maxHitPoints() ) )
							
							if (pLoopUnit.getDamage() >= ((pLoopUnit.maxHitPoints() * 2) / 3)):
								screen.setStackedBarColors(HealthBar, InfoBarTypes.INFOBAR_STORED, gc.getInfoTypeForString("COLOR_RED"))
							elif (pLoopUnit.getDamage() >= (pLoopUnit.maxHitPoints() / 3)):
								screen.setStackedBarColors(HealthBar, InfoBarTypes.INFOBAR_STORED, gc.getInfoTypeForString("COLOR_YELLOW"))
							else:
								screen.setStackedBarColors(HealthBar, InfoBarTypes.INFOBAR_STORED, gc.getInfoTypeForString("COLOR_GREEN"))

						# MOVE ICON
							if pLoopUnit.isWaiting():
								szFileName = ArtFileMgr.getInterfaceArtInfo("OVERLAY_FORTIFY").getPath()
							elif (pLoopUnit.canMove()):
								if (pLoopUnit.hasMoved()):
									szFileName = ArtFileMgr.getInterfaceArtInfo("OVERLAY_HASMOVED").getPath()
								else:
									szFileName = ArtFileMgr.getInterfaceArtInfo("OVERLAY_MOVE").getPath()
							else:
								szFileName = ArtFileMgr.getInterfaceArtInfo("OVERLAY_NOMOVE").getPath()
							screen.addDDSGFCAt( "GarrisonMoveIcon" + str(i), "CityGarrisonPanel", szFileName, cityGarrisonX, (STACK_BAR_HEIGHT * 3 / 2) + (yPosition * (GarrisonButtonSize + self.PLOT_LIST_HEALTH_BAR_SHORTENING_CORRECTION)), self.PLOT_LIST_ICON_SIZE, self.PLOT_LIST_ICON_SIZE, WidgetTypes.WIDGET_EJECT_CITIZEN, -1, -1, False )										
							
							xPosition += 1
							if (xPosition == 3):
								xPosition = 0
								yPosition += 1

		return 0
		
	# Will update the game data strings
	def updateGameDataStrings( self ):

		screen = CyGInterfaceScreen( "MainInterface", CvScreenEnums.MAIN_INTERFACE )
		screen.hide("ClockText")
		bShift = CyInterface().shiftKey()

		xResolution = screen.getXResolution()
		yResolution = screen.getYResolution()
		pHeadSelectedCity = CyInterface().getHeadSelectedCity()

		if (pHeadSelectedCity):
			ePlayer = pHeadSelectedCity.getOwner()
		else:
			ePlayer = gc.getGame().getActivePlayer()

		if ( ePlayer < 0 or ePlayer >= gc.getMAX_PLAYERS() ):
			return 0

		if ( CyInterface().getShowInterface() != InterfaceVisibility.INTERFACE_HIDE_ALL and CyInterface().getShowInterface() != InterfaceVisibility.INTERFACE_MINIMAP_ONLY  and CyInterface().getShowInterface() != InterfaceVisibility.INTERFACE_ADVANCED_START):
			szTimeText = unicode(CyGameTextMgr().getInterfaceTimeStr(gc.getGame().getActivePlayer()))
			screen.setLabel("TimeText", "Background", self.setFontSize(szTimeText, 1), CvUtil.FONT_RIGHT_JUSTIFY, xResolution - 17, 17, -0.3, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, TIMETEXT_ID, -1 )
			if (CyUserProfile().isClockOn()):
				screen.setLabel("ClockText", "Background", self.setFontSize(getClockText(), 0), CvUtil.FONT_RIGHT_JUSTIFY, xResolution - 24, 42, -0.3, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
				screen.show("ClockText")

			if (gc.getPlayer(ePlayer).isAlive()):
				szText = CyGameTextMgr().getGoldStr(ePlayer)
				screen.setLabel("GoldText", "Background", self.setFontSize(szText, 1), CvUtil.FONT_LEFT_JUSTIFY, 17, 17, -0.3, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, TIMETEXT_ID, -1 )

		return 0


	# Will update the selection Data Strings
	def updateCityScreen( self ):
		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE )
		pHeadSelectedCity = CyInterface().getHeadSelectedCity()

		# Find out our resolution
		xResolution = screen.getXResolution()
		yResolution = screen.getYResolution()
		bShift = CyInterface().shiftKey()
		
	# INITILIZE CITY VIEW CAMERA
		x = ((xResolution * 1.0) - ((CITY_VIEW_BOX_HEIGHT_AND_WIDTH / 2) + BUILD_AREA_WIDTH + (MAP_EDGE_MARGIN_WIDTH))) / xResolution 
		y = 1.00 - ((CITY_TITLE_BAR_PERCENT_HEIGHT + (CITY_VIEW_BOX_PERCENT_HEIGHT - (MAP_EDGE_MARGIN_PERCENT_WIDTH * 2)) / 2) / 100.0)
		CyCamera().SetCityViewPortCenter(x, y)
		
		screen.hide("ClockText")
		screen.hide("DefenseText")
		screen.hide("LibertyText")
		screen.hide("HammerText")
		screen.hide("CrossesText")
		screen.hide("HealthText")
		screen.hide("TotalHappinessText") # CITY HAPPINESS vs UNHAPPINESS
		screen.hide("HappinessText") # CITY HAPPINESS vs UNHAPPINESS
		screen.hide("UnhappinessText") # CITY HAPPINESS vs UNHAPPINESS
		screen.hide("TotalLawText") # CITY LAW vs CRIME
		screen.hide("LawText") # CITY LAW vs CRIME
		screen.hide("CrimeText") # CITY LAW vs CRIME
		screen.hide("EducationText")
		screen.hide("CultureText")
		screen.hide("HarbourText")
		screen.hide("BarracksText")
		screen.hide("CityNameText")
		screen.hide("PopulationText")
		screen.hide("ProductionInputText")

		for iProfession in range(gc.getNumProfessionInfos()):
			if( not gc.getProfessionInfo(iProfession).isCitizen()):
				screen.hide("EjectProfession" + str(iProfession))

	# CITY SCREEN UP
		if (CyInterface().isCityScreenUp()):
			# WTP, ray, Center Plot specific Backgrounds - START
			if (gc.getDefineINT("SHOW_TERRAIN_SPECIFIC_CITY_BACKGROUNDS") == 1 and pHeadSelectedCity and pHeadSelectedCity.getCenterPlotTerrainType() == TerrainTypes.TERRAIN_GRASS):
				screen.addDrawControl("CityManagerBackground", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCROLL_BG_GRASS").getPath(), 0, CITY_TITLE_BAR_HEIGHT, int(CITIZEN_BAR_WIDTH * 1.07), int((yResolution - (BOTTOM_CENTER_HUD_HEIGHT + CITY_TITLE_BAR_HEIGHT)) * 1.04), WidgetTypes.WIDGET_GENERAL, -1, -1 )
			elif (gc.getDefineINT("SHOW_TERRAIN_SPECIFIC_CITY_BACKGROUNDS") == 1 and pHeadSelectedCity and pHeadSelectedCity.getCenterPlotTerrainType() == TerrainTypes.TERRAIN_PLAINS_FERTILE):
				screen.addDrawControl("CityManagerBackground", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCROLL_BG_PLAINS_FERTILE").getPath(), 0, CITY_TITLE_BAR_HEIGHT, int(CITIZEN_BAR_WIDTH * 1.07), int((yResolution - (BOTTOM_CENTER_HUD_HEIGHT + CITY_TITLE_BAR_HEIGHT)) * 1.04), WidgetTypes.WIDGET_GENERAL, -1, -1 )
			elif (gc.getDefineINT("SHOW_TERRAIN_SPECIFIC_CITY_BACKGROUNDS") == 1 and pHeadSelectedCity and pHeadSelectedCity.getCenterPlotTerrainType() == TerrainTypes.TERRAIN_ROCK_STEPPES):
				screen.addDrawControl("CityManagerBackground", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCROLL_BG_ROCK_STEPPES").getPath(), 0, CITY_TITLE_BAR_HEIGHT, int(CITIZEN_BAR_WIDTH * 1.07), int((yResolution - (BOTTOM_CENTER_HUD_HEIGHT + CITY_TITLE_BAR_HEIGHT)) * 1.04), WidgetTypes.WIDGET_GENERAL, -1, -1 )
			elif (gc.getDefineINT("SHOW_TERRAIN_SPECIFIC_CITY_BACKGROUNDS") == 1 and pHeadSelectedCity and pHeadSelectedCity.getCenterPlotTerrainType() == TerrainTypes.TERRAIN_WETLAND):
				screen.addDrawControl("CityManagerBackground", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCROLL_BG_WETLAND").getPath(), 0, CITY_TITLE_BAR_HEIGHT, int(CITIZEN_BAR_WIDTH * 1.07), int((yResolution - (BOTTOM_CENTER_HUD_HEIGHT + CITY_TITLE_BAR_HEIGHT)) * 1.04), WidgetTypes.WIDGET_GENERAL, -1, -1 )
			elif (gc.getDefineINT("SHOW_TERRAIN_SPECIFIC_CITY_BACKGROUNDS") == 1 and pHeadSelectedCity and pHeadSelectedCity.getCenterPlotTerrainType() == TerrainTypes.TERRAIN_PLAINS):
				screen.addDrawControl("CityManagerBackground", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCROLL_BG_PLAINS").getPath(), 0, CITY_TITLE_BAR_HEIGHT, int(CITIZEN_BAR_WIDTH * 1.07), int((yResolution - (BOTTOM_CENTER_HUD_HEIGHT + CITY_TITLE_BAR_HEIGHT)) * 1.04), WidgetTypes.WIDGET_GENERAL, -1, -1 )
			elif (gc.getDefineINT("SHOW_TERRAIN_SPECIFIC_CITY_BACKGROUNDS") == 1 and pHeadSelectedCity and pHeadSelectedCity.getCenterPlotTerrainType() == TerrainTypes.TERRAIN_DESERT):
				screen.addDrawControl("CityManagerBackground", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCROLL_BG_DESERT").getPath(), 0, CITY_TITLE_BAR_HEIGHT, int(CITIZEN_BAR_WIDTH * 1.07), int((yResolution - (BOTTOM_CENTER_HUD_HEIGHT + CITY_TITLE_BAR_HEIGHT)) * 1.04), WidgetTypes.WIDGET_GENERAL, -1, -1 )
			elif (gc.getDefineINT("SHOW_TERRAIN_SPECIFIC_CITY_BACKGROUNDS") == 1 and pHeadSelectedCity and pHeadSelectedCity.getCenterPlotTerrainType() == TerrainTypes.TERRAIN_SHRUBLAND):
				screen.addDrawControl("CityManagerBackground", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCROLL_BG_SHRUBLAND").getPath(), 0, CITY_TITLE_BAR_HEIGHT, int(CITIZEN_BAR_WIDTH * 1.07), int((yResolution - (BOTTOM_CENTER_HUD_HEIGHT + CITY_TITLE_BAR_HEIGHT)) * 1.04), WidgetTypes.WIDGET_GENERAL, -1, -1 )
			elif (gc.getDefineINT("SHOW_TERRAIN_SPECIFIC_CITY_BACKGROUNDS") == 1 and pHeadSelectedCity and pHeadSelectedCity.getCenterPlotTerrainType() == TerrainTypes.TERRAIN_SAVANNAH):
				screen.addDrawControl("CityManagerBackground", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCROLL_BG_SAVANNAH").getPath(), 0, CITY_TITLE_BAR_HEIGHT, int(CITIZEN_BAR_WIDTH * 1.07), int((yResolution - (BOTTOM_CENTER_HUD_HEIGHT + CITY_TITLE_BAR_HEIGHT)) * 1.04), WidgetTypes.WIDGET_GENERAL, -1, -1 )
			elif (gc.getDefineINT("SHOW_TERRAIN_SPECIFIC_CITY_BACKGROUNDS") == 1 and pHeadSelectedCity and pHeadSelectedCity.getCenterPlotTerrainType() == TerrainTypes.TERRAIN_MARSH):
				screen.addDrawControl("CityManagerBackground", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCROLL_BG_MARSH").getPath(), 0, CITY_TITLE_BAR_HEIGHT, int(CITIZEN_BAR_WIDTH * 1.07), int((yResolution - (BOTTOM_CENTER_HUD_HEIGHT + CITY_TITLE_BAR_HEIGHT)) * 1.04), WidgetTypes.WIDGET_GENERAL, -1, -1 )
			elif (gc.getDefineINT("SHOW_TERRAIN_SPECIFIC_CITY_BACKGROUNDS") == 1 and pHeadSelectedCity and pHeadSelectedCity.getCenterPlotTerrainType() == TerrainTypes.TERRAIN_TAIGA):
				screen.addDrawControl("CityManagerBackground", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCROLL_BG_TAIGA").getPath(), 0, CITY_TITLE_BAR_HEIGHT, int(CITIZEN_BAR_WIDTH * 1.07), int((yResolution - (BOTTOM_CENTER_HUD_HEIGHT + CITY_TITLE_BAR_HEIGHT)) * 1.04), WidgetTypes.WIDGET_GENERAL, -1, -1 )
			elif (gc.getDefineINT("SHOW_TERRAIN_SPECIFIC_CITY_BACKGROUNDS") == 1 and pHeadSelectedCity and pHeadSelectedCity.getCenterPlotTerrainType() == TerrainTypes.TERRAIN_TUNDRA):
				screen.addDrawControl("CityManagerBackground", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCROLL_BG_TUNDRA").getPath(), 0, CITY_TITLE_BAR_HEIGHT, int(CITIZEN_BAR_WIDTH * 1.07), int((yResolution - (BOTTOM_CENTER_HUD_HEIGHT + CITY_TITLE_BAR_HEIGHT)) * 1.04), WidgetTypes.WIDGET_GENERAL, -1, -1 )
			elif (gc.getDefineINT("SHOW_TERRAIN_SPECIFIC_CITY_BACKGROUNDS") == 1 and pHeadSelectedCity and pHeadSelectedCity.getCenterPlotTerrainType() == TerrainTypes.TERRAIN_SNOW):
				screen.addDrawControl("CityManagerBackground", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCROLL_BG_SNOW").getPath(), 0, CITY_TITLE_BAR_HEIGHT, int(CITIZEN_BAR_WIDTH * 1.07), int((yResolution - (BOTTOM_CENTER_HUD_HEIGHT + CITY_TITLE_BAR_HEIGHT)) * 1.04), WidgetTypes.WIDGET_GENERAL, -1, -1 )
			else:
				screen.addDrawControl("CityManagerBackground", ArtFileMgr.getInterfaceArtInfo("INTERFACE_SCROLL_BG").getPath(), 0, CITY_TITLE_BAR_HEIGHT, int(CITIZEN_BAR_WIDTH * 1.07), int((yResolution - (BOTTOM_CENTER_HUD_HEIGHT + CITY_TITLE_BAR_HEIGHT)) * 1.04), WidgetTypes.WIDGET_GENERAL, -1, -1 )

			# WTP, ray: had to add this here
			# CITY BUILDING GRID	
			#Androrc Multiple Professions per Building
			for iSpecial in range(gc.getNumSpecialBuildingInfos()):
				screen.moveToFront("CityBuildingGraphic" + str(iSpecial))
				screen.moveToFront("ProductionBox" + str(iSpecial))
			# WTP, ray, Center Plot specific Backgrounds - END

			for iYield in EMPHASIZEYIELDS:
				screen.hide("MapYieldBox" + str(iYield))
				screen.hide("MapYieldIcon" + str(iYield))
				screen.hide("MapYieldEmphasize" + str(iYield))
				screen.hide("MapYieldDe-Emphasize" + str(iYield))

			minimapWidth = min(xResolution - MAP_EDGE_MARGIN_WIDTH - CITY_MULTI_TAB_SIZE - CITIZEN_BAR_WIDTH, yResolution - BOTTOM_CENTER_HUD_HEIGHT - TRANSPORT_AREA_HEIGHT - (STACK_BAR_HEIGHT *2) - CITY_TITLE_BAR_HEIGHT - CITY_VIEW_BOX_HEIGHT_AND_WIDTH)
			xCenter = (CITIZEN_BAR_WIDTH + MAP_EDGE_MARGIN_WIDTH + xResolution - CITY_MULTI_TAB_SIZE) / 2
			yCenter = (CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH + yResolution - BOTTOM_CENTER_HUD_HEIGHT - TRANSPORT_AREA_HEIGHT - (STACK_BAR_HEIGHT *2)) / 2
			screen.placeMinimap(xCenter - minimapWidth / 2, xCenter + minimapWidth / 2, yCenter - minimapWidth / 2, yCenter + minimapWidth / 2)
			if (pHeadSelectedCity):

				self.updateSelectionButtons()

				for j in range(gc.getMAX_PLOT_LIST_ROWS()):
					for i in range(self.numPlotListButtons()):
						szString = "PlotListButtonMulti" + str(j * self.numPlotListButtons() + i)
						screen.hide( szString )
						screen.hide( szString + "Health")
						screen.hide( szString + "Icon")
				
				screen.show("ResourceTable")
				#for iYield in range(YieldTypes.NUM_YIELD_TYPES):
				#	screen.show("YieldIcon" + str(iYield))

			# CITY SCROLL BUTTONS
				if (pHeadSelectedCity.getTeam() == gc.getGame().getActiveTeam()):
					if (gc.getActivePlayer().getNumCities() < 2):
						screen.hide("CityScrollMinus")
						screen.hide("CityScrollPlus")

			# CITY NAME HEADER
				szBuffer = u"<font=4>"
				if (pHeadSelectedCity.isCapital()):
					szBuffer += u"%c" %(CyGame().getSymbolID(FontSymbols.STAR_CHAR))
				szBuffer += u"%s: %d" %(pHeadSelectedCity.getName(), pHeadSelectedCity.getPopulation())
				if (pHeadSelectedCity.isOccupation()):
					szBuffer += u" (%c:%d)" %(CyGame().getSymbolID(FontSymbols.OCCUPATION_CHAR), pHeadSelectedCity.getOccupationTimer())
				# Robert Surcouf, moved VET NewCapacity

				szBuffer += u"</font>"
				
				localText.changeTextColor(szBuffer, gc.getInfoTypeForString("COLOR_FONT_CREAM"))
				screen.setText("CityNameText", "Background", szBuffer, CvUtil.FONT_CENTER_JUSTIFY, xResolution / 2 , CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.GAME_FONT, WidgetTypes.WIDGET_CITY_NAME, -1, -1 )

			# POPULATION GROWTH/STARVATION TEXT
				iFoodDifference = pHeadSelectedCity.foodDifference()
				iProductionDiffNoFood = pHeadSelectedCity.getCurrentProductionDifference(True)
				iProductionDiffJustFood = 0
				szBuffer = u"<font=3>"
				if (iFoodDifference > 0):
					szBuffer = localText.getText("INTERFACE_CITY_GROWING", (pHeadSelectedCity.getFoodTurnsLeft(), ))
				elif (iFoodDifference < 0):
					szBuffer = localText.getText("INTERFACE_CITY_STARVING", ())
				else:
					szBuffer = localText.getText("INTERFACE_CITY_STAGNANT", ())
				szBuffer += u"</font>"
				screen.setText("PopulationText", "Background", szBuffer, CvUtil.FONT_LEFT_JUSTIFY, xResolution * 4 / 100, CITY_TITLE_BAR_HEIGHT / 8, -0.3, FontTypes.GAME_FONT, WidgetTypes.WIDGET_HELP_POPULATION, -1, -1 )
				screen.setStyle("PopulationText", "Button_Stone_Style")

			# CURRENT PRODUCTION BAR FILL
				fProductionNeeded = float(pHeadSelectedCity.getProductionNeeded(YieldTypes.YIELD_HAMMERS))
				if (fProductionNeeded > 0):
					iFirst = ((float(pHeadSelectedCity.getProduction())) / fProductionNeeded)
					screen.setBarPercentage("CityProductionBar", InfoBarTypes.INFOBAR_STORED, iFirst )
					if ( iFirst == 1 ):
						iSecond = (((float(iProductionDiffNoFood)) / fProductionNeeded) )
					else:
						iSecond = (((float(iProductionDiffNoFood)) / fProductionNeeded) ) / ( 1 - iFirst )
					screen.setBarPercentage("CityProductionBar", InfoBarTypes.INFOBAR_RATE, iSecond )
					if ( iFirst + iSecond == 1 ):
						screen.setBarPercentage("CityProductionBar", InfoBarTypes.INFOBAR_RATE_EXTRA, (((float(iProductionDiffJustFood)) / fProductionNeeded) ) )
					else:
						screen.setBarPercentage("CityProductionBar", InfoBarTypes.INFOBAR_RATE_EXTRA, (( ((float(iProductionDiffJustFood)) / fProductionNeeded) ) ) / ( 1 - ( iFirst + iSecond ) ) )

			# CURRENT PRODUCTION BAR TEXT
				if (pHeadSelectedCity.isProductionBuilding() or pHeadSelectedCity.isProductionUnit()):
					szBuffer = localText.getText("INTERFACE_CITY_PRODUCTION", (pHeadSelectedCity.getProductionNameKey(), pHeadSelectedCity.getProductionTurnsLeft()))
				elif pHeadSelectedCity.isProduction():
					szBuffer = pHeadSelectedCity.getProductionName()
				else:
					szBuffer = u""
				
			# YIELD REQUIREMENT INFO
				szYieldSymbols = u""
				for iYield in range(YieldTypes.NUM_YIELD_TYPES):
					if iYield != YieldTypes.YIELD_HAMMERS:
						if (CyInterface().getOrderNodeType(0) == OrderTypes.ORDER_TRAIN ):
							iYieldRequired = gc.getPlayer(pHeadSelectedCity.getOwner()).getUnitYieldProductionNeeded(CyInterface().getOrderNodeData1(0), iYield)
						elif (CyInterface().getOrderNodeType(0) == OrderTypes.ORDER_CONSTRUCT):
							iYieldRequired = gc.getPlayer(pHeadSelectedCity.getOwner()).getBuildingYieldProductionNeeded(CyInterface().getOrderNodeData1(0), iYield)
						else:
							iYieldRequired = 0
							
						iYieldStored = pHeadSelectedCity.getYieldStored(iYield) + pHeadSelectedCity.getYieldRushed(iYield)
						if (iYieldRequired > iYieldStored):
							if (len(szYieldSymbols) > 0):
								szYieldSymbols += u","
							szYieldSymbols += u"% i%c" % (iYieldRequired - iYieldStored, gc.getYieldInfo(iYield).getChar())
							
				if len(szYieldSymbols) > 0:
					screen.setStackedBarColors("CityProductionBar", InfoBarTypes.INFOBAR_STORED, gc.getInfoTypeForString("COLOR_RED") )
					screen.setStackedBarColors("CityProductionBar", InfoBarTypes.INFOBAR_RATE, gc.getInfoTypeForString("COLOR_WARNING_RATE") )
					szBuffer += u" " + localText.getText("TXT_KEY_PEDIA_REQUIRES", ()) + szYieldSymbols
				else:
					screen.setStackedBarColors("CityProductionBar", InfoBarTypes.INFOBAR_STORED, gc.getInfoTypeForString("COLOR_GREAT_PEOPLE_STORED") )
					screen.setStackedBarColors("CityProductionBar", InfoBarTypes.INFOBAR_RATE, gc.getInfoTypeForString("COLOR_GREAT_PEOPLE_RATE") )
#TAC --->					
				xPostionProductionBar = (xResolution - CITIZEN_BAR_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 3) - (SMALL_BUTTON_SIZE * 4) + 5) / 2
				screen.setLabelAt("ProductionText", "CityProductionBar", szBuffer, CvUtil.FONT_CENTER_JUSTIFY, xPostionProductionBar, STACK_BAR_HEIGHT / 2, -1.3, FontTypes.GAME_FONT, WidgetTypes.WIDGET_HELP_SELECTED, 0, -1 )
#<--- TAC
			# 3 D BUILDINGS
				for iSpecial in range(gc.getNumSpecialBuildingInfos()):
					BuildingPresent = False
					for iBuilding in range(gc.getNumBuildingInfos()):
						if (pHeadSelectedCity.isHasBuilding(iBuilding)):
							if(gc.getBuildingInfo(iBuilding).getSpecialBuildingType() == iSpecial):
								BuildingPresent = True
								break

					if (BuildingPresent):
						Texture = gc.getBuildingInfo(iBuilding).getArtInfo().getCityTexture()
						screen.changeImageButton("CityBuildingGraphic" + str(iSpecial), gc.getBuildingInfo(iBuilding).getArtInfo().getCityTexture())
						screen.show("CityBuildingGraphic" + str(iSpecial))
					else:
						#screen.hide("CityBuildingGraphic" + str(iSpecial))
						szTexture = gc.getSpecialBuildingInfo(iSpecial).getNatureObject()
						print szTexture
						screen.changeImageButton("CityBuildingGraphic" + str(iSpecial), szTexture)
						screen.show("CityBuildingGraphic" + str(iSpecial))
				
				# More or less: 1024 -> 5% 1280 -> 4% / 1600 -> 2% / 1980 -> 0%
				iXmodifier = max(xResolution * 5 / 100 - (xResolution - 1000)/200 , 0)

			# CITIY DEFENSE MODIFIER
				iDefenseModifier = pHeadSelectedCity.getDefenseModifier()
				if (iDefenseModifier != 0):
					szBuffer = u"+%s%%%c" % (str(iDefenseModifier), CyGame().getSymbolID(FontSymbols.DEFENSE_CHAR))  #localText.getText("TXT_KEY_MAIN_CITY_DEFENSE", (CyGame().getSymbolID(FontSymbols.DEFENSE_CHAR), iDefenseModifier))
					if (pHeadSelectedCity.getDefenseDamage() > 0):
						szTempBuffer = u" (%d%%)" %(( ( gc.getMAX_CITY_DEFENSE_DAMAGE() - pHeadSelectedCity.getDefenseDamage() ) * 100 ) / gc.getMAX_CITY_DEFENSE_DAMAGE() )
						szBuffer = szBuffer + szTempBuffer
					szBuffer = "<font=3>" + szBuffer + "</font>"
					screen.setLabel("DefenseText", "Background", szBuffer, CvUtil.FONT_RIGHT_JUSTIFY, xResolution * 89 / 100 -iXmodifier , CITY_TITLE_BAR_HEIGHT / 8, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_HELP_DEFENSE, -1, -1 )
					screen.show("DefenseText")

			# CITY HAMMER PRODUCTION
				iHammers = pHeadSelectedCity.getCurrentProductionDifference(True)
				szBuffer = u"<font=3>" + u"%i%c" % (iHammers, gc.getYieldInfo(YieldTypes.YIELD_HAMMERS).getChar()) + u"</font>"
				screen.setLabel("HammerText", "Background", szBuffer, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 11 / 100 +iXmodifier , CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_PRODUCTION_MOD_HELP, -1, -1 )

			# CITY LIBERTYBELL PRODUCTION
				iLiberty = pHeadSelectedCity.calculateNetYield(YieldTypes.YIELD_BELLS)
				szBuffer = u"<font=3>" + u"%i%c" % (iLiberty, gc.getYieldInfo(YieldTypes.YIELD_BELLS).getChar()) + u"</font>"
				screen.setLabel("LibertyText", "Background", szBuffer, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 14 / 100 +iXmodifier, CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_HELP_YIELD, YieldTypes.YIELD_BELLS, -1 )

			# CITY CROSS PRODUCTION
				iCrosses = pHeadSelectedCity.calculateNetYield(YieldTypes.YIELD_CROSSES)
				szBuffer = u"<font=3>" + u"%i%c" % (iCrosses, gc.getYieldInfo(YieldTypes.YIELD_CROSSES).getChar()) + u"</font>"
				screen.setLabel("CrossesText", "Background", szBuffer, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 17 / 100 +iXmodifier, CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_HELP_YIELD, YieldTypes.YIELD_CROSSES, -1 )

			# CITY EDUCATION PRODUCTION
				iBooks = pHeadSelectedCity.calculateNetYield(YieldTypes.YIELD_EDUCATION)
				szBuffer = u"<font=3>" + u"%i%c" % (iBooks, gc.getYieldInfo(YieldTypes.YIELD_EDUCATION).getChar()) + u"</font>"
				screen.setLabel("EducationText", "Background", szBuffer, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 20 / 100 +iXmodifier, CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_HELP_YIELD, YieldTypes.YIELD_EDUCATION, -1 )

			# CITY Culture PRODUCTION
				iCulture = pHeadSelectedCity.getCultureRate() # RaR, ray, small correction
				szBuffer = u"<font=3>" + u"%i%c" % (iCulture, gc.getYieldInfo(YieldTypes.YIELD_CULTURE).getChar()) + u"</font>"
				screen.setLabel("CultureText", "Background", szBuffer, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 23 / 100 +iXmodifier, CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_HELP_YIELD, YieldTypes.YIELD_CULTURE, -1 )

			# CITY HEALTH PRODUCTION
				#get the coloured string for current City Health
				iHealth = pHeadSelectedCity.getCityHealth()
				szBufferCurrentHealth = u"<font=3>" + u"<color="
				if iHealth > 0: # Green
					szBufferCurrentHealth += u"0,255,0" + u">" +str(iHealth) + u"</color>" + u"</font>"
				elif iHealth < 0: # Red
					szBufferCurrentHealth += u"255,0,0" + u">" + str(iHealth) + u"</color>" + u"</font>"
				else: # Yellow
					szBufferCurrentHealth += u"255,255,0" + u">" + str(iHealth) + u"</color>" + u"</font>"
				
				#get the coloured string for change of City Health
				iHealthChange = pHeadSelectedCity.getCityHealthChange()
				szBufferHealthChange = u"<font=3>" + u"<color="
				if iHealthChange > 0: # Green
					#not enough space for a + sign
					#szBufferHealthChange += u"0,255,0" + u">(+"+str(iHealthChange)+u")" + u"</color>" + u"</font>"
					szBufferHealthChange += u"0,255,0" + u">("+str(iHealthChange)+u")" + u"</color>" + u"</font>"
				elif iHealthChange < 0: # Red
					szBufferHealthChange += u"255,0,0" + u">("+str(iHealthChange)+u")" + u"</color>" + u"</font>"
				else: # Yellow
					szBufferHealthChange += u"255,255,0" + u">("+str(iHealthChange)+u")" + u"</color>" + u"</font>"
				
				#get the Health Icon String
				szBufferHealthIcon = u"<font=3>" + u"%c" % (gc.getYieldInfo(YieldTypes.YIELD_HEALTH).getChar()) + u"</font>"
					
				#now add them all together and display on top of the Screen
				szBuffer = szBufferCurrentHealth + szBufferHealthChange +szBufferHealthIcon
				screen.setLabel("HealthText", "Background", szBuffer, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 68 / 100 +iXmodifier, CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_HELP_YIELD, YieldTypes.YIELD_HEALTH, -1 )

			# CITY HAPPINESS vs UNHAPPINESS
				pHeadSelectedCity.updateCityHappiness()
				pHeadSelectedCity.updateCityUnHappiness()
				iHappiness = pHeadSelectedCity.getCityHappiness()
				iUnhappiness = pHeadSelectedCity.getCityUnHappiness()
				
				if iHappiness > iUnhappiness: # Green and we use Icon Happiness
					iTotalHap = iHappiness - iUnhappiness
					szBufferTotal = u"<font=3>" + u"<color="u"0,255,0" + u">"+str(iTotalHap) + u"</color>" + u"</font>"
					szBufferTotal += u"<font=3>" + u"%c" % (gc.getYieldInfo(YieldTypes.YIELD_HAPPINESS).getChar()) + u"</font>"
					szBufferTotal += u"<font=3>" + u"=" + u"</font>"
					screen.setLabel("TotalHappinessText", "Background", szBufferTotal, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 69 / 100 -iXmodifier, CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, HELP_TOTAL_HAPPINESS_POSITIVE, -1 )
				elif iUnhappiness > iHappiness: # Red and we use Icon Unhappiness
					iTotalUnhap = iUnhappiness - iHappiness
					szBufferTotal = u"<font=3>" + u"<color="u"255,0,0" + u">"+str(iTotalUnhap) + u"</color>" + u"</font>"
					szBufferTotal += u"<font=3>" + u"%c" % (gc.getYieldInfo(YieldTypes.YIELD_UNHAPPINESS).getChar()) + u"</font>"
					szBufferTotal += u"<font=3>" + u"=" + u"</font>"
					screen.setLabel("TotalHappinessText", "Background", szBufferTotal, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 69 / 100-iXmodifier, CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, HELP_TOTAL_HAPPINESS_NEGATIVE, -1 )
				else: # Yellow and we use Icon Happiness
					iTotalZeroHap = 0
					szBufferTotal = u"<font=3>" + u"<color="u"255,255,0" + u">" + str(iTotalZeroHap) + u"</color>" + u"</font>"
					szBufferTotal += u"<font=3>" + u"%c" % (gc.getYieldInfo(YieldTypes.YIELD_HAPPINESS).getChar()) + u"</font>"
					szBufferTotal += u"<font=3>" + u"=" + u"</font>"
					screen.setLabel("TotalHappinessText", "Background", szBufferTotal, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 69 / 100-iXmodifier, CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, HELP_TOTAL_HAPPINESS_ZERO, -1 )
				
				szBufferHappiness = u"<font=3>" + u"%c" % (gc.getYieldInfo(YieldTypes.YIELD_HAPPINESS).getChar()) + u"</font>"
				szBufferHappiness += u"<font=3>" + u"-" + u"</font>"
				screen.setLabel("HappinessText", "Background", szBufferHappiness, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 72 / 100-iXmodifier, CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_HELP_YIELD, YieldTypes.YIELD_HAPPINESS, -1 )
				
				szBufferUnHappiness = u"<font=3>" + u"%c" % (gc.getYieldInfo(YieldTypes.YIELD_UNHAPPINESS).getChar()) + u" </font>"
				screen.setLabel("UnhappinessText", "Background", szBufferUnHappiness, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 74 / 100-iXmodifier, CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_HELP_YIELD, YieldTypes.YIELD_UNHAPPINESS, -1 )
				
			# CITY LAW vs CRIME
				pHeadSelectedCity.updateCityLaw()
				pHeadSelectedCity.updateCityCrime()
				iLaw = pHeadSelectedCity.getCityLaw()
				iCrime = pHeadSelectedCity.getCityCrime()
				
				if iLaw > iCrime: # Green and we use LAW Icon
					iTotalLaw = iLaw - iCrime
					szBufferTotal = u"<font=3>" + u"<color="u"0,255,0" + u">"+str(iTotalLaw) + u"</color>" + u"</font>"
					szBufferTotal += u"<font=3>" + u"%c" % (gc.getYieldInfo(YieldTypes.YIELD_LAW).getChar()) + u"</font>"
					szBufferTotal += u"<font=3>" + u"=" + u"</font>"
					screen.setLabel("TotalLawText", "Background", szBufferTotal, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 37 / 100 -iXmodifier, CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, HELP_TOTAL_LAW_POSITIVE, -1 )
				elif iCrime > iLaw: # Red and we use CRIME Icon
					iTotalCrime = iCrime - iLaw
					szBufferTotal = u"<font=3>" + u"<color="u"255,0,0" + u">"+str(iTotalCrime) + u"</color>" + u"</font>"
					szBufferTotal += u"<font=3>" + u"%c" % (gc.getYieldInfo(YieldTypes.YIELD_CRIME).getChar()) + u"</font>"
					szBufferTotal += u"<font=3>" + u"=" + u"</font>"
					screen.setLabel("TotalLawText", "Background", szBufferTotal, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 37 / 100-iXmodifier, CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, HELP_TOTAL_LAW_NEGATIVE, -1 )
				else: # Yellow and we use LAW Icon
					iTotalZeroLaw = 0
					szBufferTotal = u"<font=3>" + u"<color="u"255,255,0" + u">" + str(iTotalZeroLaw) + u"</color>" + u"</font>"
					szBufferTotal += u"<font=3>" + u"%c" % (gc.getYieldInfo(YieldTypes.YIELD_LAW).getChar()) + u"</font>"
					szBufferTotal += u"<font=3>" + u"=" + u"</font>"
					screen.setLabel("TotalLawText", "Background", szBufferTotal, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 37 / 100-iXmodifier, CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, HELP_TOTAL_LAW_ZERO, -1 )
				
				szBufferLaw = u"<font=3>" + u"%c" % (gc.getYieldInfo(YieldTypes.YIELD_LAW).getChar()) + u"</font>"
				szBufferLaw += u"<font=3>" + u"-" + u"</font>"
				screen.setLabel("LawText", "Background", szBufferLaw, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 40 / 100-iXmodifier, CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_HELP_YIELD, YieldTypes.YIELD_LAW, -1 )
				
				szBufferCrime = u"<font=3>" + u"%c" % (gc.getYieldInfo(YieldTypes.YIELD_CRIME).getChar()) + u" </font>"
				screen.setLabel("CrimeText", "Background", szBufferCrime, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 42 / 100-iXmodifier, CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_HELP_YIELD, YieldTypes.YIELD_CRIME, -1 )
				
			# WTP, ray, new Harbour System - START
				if (pHeadSelectedCity.bShouldShowCityHarbourSystem()):
				
					iMaxHarbourSpace = pHeadSelectedCity.getCityHarbourSpace()
					iUsedHarbourSpace = pHeadSelectedCity.getCityHarbourSpaceUsed()
					iHalfMaxHarbourSpace = (iMaxHarbourSpace / 2)
	
					#green: less than 50 percent of the harbour space is full
					# we remove the numbers and put them in Mouse over to save space
					# careful, 2nd line of buffer now "=" instead "+="
					if (iUsedHarbourSpace <= iHalfMaxHarbourSpace):
						#szBuffer = u"<font=3>" + u" <color="u"0,255,0" + u">"+ str(iUsedHarbourSpace) + u"(" + str(iMaxHarbourSpace) + u")" + u"</color>" + u"</font>"
						szBuffer = u"<font=3>" + (u" %c" % CyGame().getSymbolID(FontSymbols.ANCHOR_CHAR)) + "</font>"
						screen.setLabel("HarbourText", "Background", szBuffer, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 81 / 100 -iXmodifier, CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_HELP_HARBOUR_SYSTEM, NEW_HARBOUR_SYSTEM, -1 )

					# red: the harbour is completely full
					elif (iUsedHarbourSpace >= iMaxHarbourSpace):
						#szBuffer = u"<font=3>" + u" <color="u"255,0,0" + u">"+ str(iUsedHarbourSpace) + u"(" + str(iMaxHarbourSpace) + u")" + u"</color>" + u"</font>"
						szBuffer = u"<font=3>" + (u" %c" % CyGame().getSymbolID(FontSymbols.NO_ANCHOR_CHAR)) + "</font>"
						screen.setLabel("HarbourText", "Background", szBuffer, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 81 / 100 -iXmodifier, CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_HELP_HARBOUR_SYSTEM, NEW_HARBOUR_SYSTEM, -1 )

					# yellow: more than half is full, but not completely
					else:
						#szBuffer = u"<font=3>" + u" <color="u"255,255,0" + u">" + str(iUsedHarbourSpace) + u"(" + str(iMaxHarbourSpace) + u")" + u"</color>" + u"</font>"
						szBuffer = u"<font=3>" + (u" %c" % CyGame().getSymbolID(FontSymbols.ANCHOR_CHAR)) + "</font>"
						screen.setLabel("HarbourText", "Background", szBuffer, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 81 / 100 -iXmodifier, CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_HELP_HARBOUR_SYSTEM, NEW_HARBOUR_SYSTEM, -1 )

			# WTP, ray, new Barracks System - START
				if (pHeadSelectedCity.bShouldShowCityBarracksSystem()):

					iMaxBarracksSpace = pHeadSelectedCity.getCityBarracksSpace()
					iUsedBarracksSpace = pHeadSelectedCity.getCityBarracksSpaceUsed()
					iHalfMaxBarracksSpace = (iMaxBarracksSpace / 2)

					#green: less than 50 percent of the barracks space is full
					# we remove the numbers and put them in Mouse over to save space
					# careful, 2nd line of buffer now "=" instead "+="
					if (iUsedBarracksSpace <= iHalfMaxBarracksSpace):
						#szBuffer = u"<font=3>" + u" <color="u"0,255,0" + u">"+ str(iUsedBarracksSpace) + u"(" + str(iMaxBarracksSpace) + u")" + u"</color>" + u"</font>"
						szBuffer = u"<font=3>" + (u" %c" % CyGame().getSymbolID(FontSymbols.BARRACKS_CHAR)) + "</font>"
						screen.setLabel("BarracksText", "Background", szBuffer, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 83 / 100 -iXmodifier, CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_HELP_BARRACKS_SYSTEM, NEW_BARRACKS_SYSTEM, -1 )

					# red: the barracks are completely full
					elif (iUsedBarracksSpace >= iMaxBarracksSpace):
						#szBuffer = u"<font=3>" + u" <color="u"255,0,0" + u">"+ str(iUsedBarracksSpace) + u"(" + str(iMaxBarracksSpace) + u")" + u"</color>" + u"</font>"
						szBuffer = u"<font=3>" + (u" %c" % CyGame().getSymbolID(FontSymbols.NO_BARRACKS_CHAR)) + "</font>"
						screen.setLabel("BarracksText", "Background", szBuffer, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 83 / 100 -iXmodifier, CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_HELP_BARRACKS_SYSTEM, NEW_BARRACKS_SYSTEM, -1 )

					# yellow: more than half is full, but not completely
					else:
						#szBuffer = u"<font=3>" + u" <color="u"255,255,0" + u">" + str(iUsedBarracksSpace) + u"(" + str(iMaxBarracksSpace) + u")" + u"</color>" + u"</font>"
						szBuffer = u"<font=3>" + (u" %c" % CyGame().getSymbolID(FontSymbols.BARRACKS_CHAR)) + "</font>"
						screen.setLabel("BarracksText", "Background", szBuffer, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 83 / 100 -iXmodifier, CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_HELP_BARRACKS_SYSTEM, NEW_BARRACKS_SYSTEM, -1 )

			# REBEL BAR FILL PERCENTAGE
				fPercentage = float(pHeadSelectedCity.getRebelPercent() / 100.0)
				screen.setBarPercentage("RebelBar", InfoBarTypes.INFOBAR_STORED, fPercentage)

			# REBEL BAR TEXT
				iRebel = pHeadSelectedCity.getRebelPercent()
				szBuffer = localText.getText("TXT_KEY_MISC_REBEL", (iRebel, ))
				screen.setLabelAt("RebelText", "RebelBar", szBuffer, CvUtil.FONT_CENTER_JUSTIFY, (CITIZEN_BAR_WIDTH - (STACK_BAR_HEIGHT * 3 / 2)) / 2, STACK_BAR_HEIGHT / 2, -1.3, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
				screen.setHitTest("RebelText", HitTestTypes.HITTEST_NOHIT)
				screen.show("RebelText")

			# PRODUCTION
				szBuffer = u"<font=3>"
				aiProducedYields = [[]] * YieldTypes.NUM_YIELD_TYPES
				for iYield in range(YieldTypes.NUM_YIELD_TYPES):
					aiProducedYields[iYield] = 0

				#iTotalYield = 0
				for iYield in range(YieldTypes.NUM_YIELD_TYPES):
					iUnproducedYield = pHeadSelectedCity.calculateActualYieldProduced(iYield) - pHeadSelectedCity.getBaseRawYieldProduced(iYield)
					if iUnproducedYield < 0:
						for iProfession in range(gc.getNumProfessionInfos()):
							if gc.getProfessionInfo(iProfession).getYieldsProduced(0) == iYield: #MultipleYieldsProduced Start
								iNeedYield = gc.getProfessionInfo(iProfession).getYieldsConsumed(0) #MultipleYieldsProduced Start
								aiProducedYields[iNeedYield] += iUnproducedYield
					iProducedYield = pHeadSelectedCity.calculateNetYield(iYield)
					aiProducedYields[iYield] += iProducedYield

				iProdusedYield = 0
				for iYield in range(YieldTypes.NUM_YIELD_TYPES):
					if (not gc.getYieldInfo(iYield).isIgnoredForStorageCapacity() and gc.getYieldInfo(iYield).isCargo()):
						iProdusedYield += aiProducedYields[iYield]

				szBuffer += u" <color="
				iMaxYield = pHeadSelectedCity.getMaxYieldCapacity()
				iTotalYield = pHeadSelectedCity.getTotalYieldStored()
				if iTotalYield + iProdusedYield > iMaxYield:
					szBuffer += u"255,0,0"
				elif iTotalYield + (2 * iProdusedYield) > iMaxYield:
					szBuffer += u"255,255,0"
				else:
					szBuffer += u"0,255,0"
				szBuffer +=  u">("
				szBuffer += str(iTotalYield)
				if iProdusedYield > 0:
					szBuffer += u"+" + str(iProdusedYield)
				elif iProdusedYield < 0:
					szBuffer += str(iProdusedYield)
				szBuffer += u"/" + str(iMaxYield) + u")"
				szBuffer += u"</font>"

				# WTP, ray, adding the Yield Icon for Trade Goods to Storage Capacity
				szBuffer += u"<font=3>" + u"%c" % (gc.getYieldInfo(YieldTypes.YIELD_TRADE_GOODS).getChar()) + u"</font>"
				screen.setLabel("StorageCapacityText", "Background", szBuffer, CvUtil.FONT_CENTER_JUSTIFY, xResolution * 90 / 100 , CITY_TITLE_BAR_HEIGHT / 12, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, VET_NEW_CAPACITY, -1 )
				
			screen.hide("TimeText")
			screen.hide("GoldText")
			
		else: # CITY SCREEN IS DOWN
			self.MINIMAP_SIDE_MARGIN = MINIMAP_HEIGHT * 13 / 100
			screen.placeMinimap(self.MINIMAP_SIDE_MARGIN, MINIMAP_HEIGHT - self.MINIMAP_SIDE_MARGIN, yResolution - MINIMAP_HEIGHT + self.MINIMAP_SIDE_MARGIN * 2, yResolution)
			screen.setHelpTextArea( HELPTEXT_AREA_MAX_WIDTH, FontTypes.SMALL_FONT, HELTTEXT_AREA_X_MARGIN, yResolution - SADDLE_HEIGHT+HELPTEXT_AREA_Y_MARGIN, -0.1, False, "", True, False, CvUtil.FONT_LEFT_JUSTIFY, HELPTEXT_AREA_MIN_WIDTH )

			if ( CyInterface().getShowInterface() == InterfaceVisibility.INTERFACE_SHOW ):
				self.setMinimapButtonVisibility(True)
				pPlayer = gc.getPlayer(gc.getGame().getActivePlayer())

				screen.hide("ResourceTable")
				for iYield in range(YieldTypes.NUM_YIELD_TYPES):
					screen.hide("YieldIcon" + str(iYield))
					screen.hide("YieldStoredlabel" + str(iYield))
					screen.hide("YieldNetlabel" + str(iYield))

				screen.hide("LibertyText")
				screen.hide("CrossesText")
				screen.hide("HealthText")
				screen.hide("TotalHappinessText") # CITY HAPPINESS vs UNHAPPINESS
				screen.hide("HappinessText") # CITY HAPPINESS vs UNHAPPINESS
				screen.hide("UnhappinessText") # CITY HAPPINESS vs UNHAPPINESS
				screen.hide("TotalLawText") # CITY LAW vs CRIME
				screen.hide("LawText") # CITY LAW vs CRIME
				screen.hide("CrimeText") # CITY LAW vs CRIME
				screen.hide("HammerText")
				screen.hide("EducationText")
				screen.hide("CultureText")
				screen.hide("HarbourText")
				screen.hide("BarracksText")
				screen.hide("StorageCapacityText")

			for iSpecial in range(gc.getNumSpecialBuildingInfos()):
				screen.hide("CityBuildingGraphic" + str(iSpecial))
				screen.hide("ProductionBox" + str(iSpecial))

			screen.hideList(RESOURCE_TABLE_HIDE)
			screen.hide("CityList")

		# WTP, ray, Center Plot specific Backgrounds - START
		# WTP, ray: had to add this here
		# Garrison and Transport Panel
		screen.addScrollPanel("CityGarrisonPanel", u"", CITIZEN_BAR_WIDTH + (SMALL_BUTTON_SIZE / 8), yResolution - BOTTOM_CENTER_HUD_HEIGHT - TRANSPORT_AREA_HEIGHT * 9 / 8 - 2, xResolution - CITIZEN_BAR_WIDTH - TRANSPORT_AREA_WIDTH + STACK_BAR_HEIGHT * 4 / 8, TRANSPORT_AREA_HEIGHT - (STACK_BAR_HEIGHT / 3), PanelStyles.PANEL_STYLE_EMPTY, false, WidgetTypes.WIDGET_EJECT_CITIZEN, -1, -1 )
		screen.addScrollPanel("CityTransportPanel", u"", xResolution - TRANSPORT_AREA_WIDTH + MAP_EDGE_MARGIN_WIDTH - SMALL_BUTTON_SIZE / 2, yResolution - BOTTOM_CENTER_HUD_HEIGHT - TRANSPORT_AREA_HEIGHT * 9 / 8 - 2, TRANSPORT_AREA_WIDTH * 126 / 128, TRANSPORT_AREA_HEIGHT - (STACK_BAR_HEIGHT / 3), PanelStyles.PANEL_STYLE_MAIN, false, WidgetTypes.WIDGET_RECEIVE_MOVE_CARGO_TO_CITY, -1, -1 )

		if (CyInterface().isCityScreenUp()):
			screen.show("CityGarrisonPanel")
			screen.show("CityTransportPanel")
			screen.moveToFront("ShowOrHideYields")
			screen.moveToFront("AutomateProduction")
			screen.moveToFront("AutomateCitizens")
			screen.moveToFront("LockCitizens")
			for iYield in self.TableYields:
				screen.moveToFront("YieldIcon" + str(iYield))
		else:
			screen.hide("CityManagerBackground")
			screen.hide("CityGarrisonPanel")
			screen.hide("CityTransportPanel")
		# WTP, ray, Center Plot specific Backgrounds - END

		return 0

	# Will set the table to display the out put of a city or the players whole empire
	def updateResourceTable( self ):

		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE )
		ResourceHideList = []

		if not CyInterface().isCityScreenUp():
			screen.hideList(RESOURCE_TABLE_HIDE)
			return
			

		xDelta = 0
		ixDelta = 0 
		iDelta = 0
		yDelta = CyInterface().determineWidth(self.setFontSize(u"0", 0)) * 3 / 2
		iK1 = 2
		iMax = (xResolution * 2) / (yResolution * 33) 
		iNum = len(self.TableYields) 
		if iNum > iMax:
			if iNum % 3 == 1:
				iNum += 1
			iNum /= 3
			RESOURCE_TABLE_COLUMN_WIDTH = (xResolution - STACK_BAR_HEIGHT) / iNum
			iK1 = 1
			if len(self.TableYields) % 3 == 1:
				ixDelta = RESOURCE_TABLE_COLUMN_WIDTH / 2

		pCity = CyInterface().getHeadSelectedCity()
		if pCity != None:
			for index in range(len(self.TableYields)):
				if index >= 2 * iNum:
					xDelta = ixDelta
					iDelta = iNum * 2
					iK1 = 3
				elif index >= iNum:
					xDelta = ixDelta
					iDelta = iNum
					iK1 = 2

				i = self.TableYields[index]
				iStored = pCity.getYieldStored(i)
				iRate = pCity.calculateNetYield(i)
				
				bIgnoredForStorageCapacity = gc.getYieldInfo(i).isIgnoredForStorageCapacity()
				szStored = ""
				if (iStored > pCity.getMaxYieldCapacity() and not bIgnoredForStorageCapacity):
					szStored = u"<color=255,0,0>%d</color>" %(iStored)
				else:
					if iStored == 0:
						szStored = u""
					else:
						szStored = u"<color=0,255,255>%d</color>" %(iStored)
				szRate = u"%d" %(iRate)
				if(iRate > 0):
					szRate = u"<color=0,255,0>+" + szRate + u"</color>"
				if(iRate == 0):
					szRate = u""
				if(iRate < 0):
					szRate = u"<color=255,255,0>" + szRate + u"</color>"

				szStorageLabel = "YieldStoredlabel" + str(i)
				iX = STACK_BAR_HEIGHT + ((index - iDelta) * RESOURCE_TABLE_COLUMN_WIDTH) + (RESOURCE_TABLE_COLUMN_WIDTH / 2) + xDelta
				iY = yResolution - BOTTOM_CENTER_HUD_HEIGHT / iK1 + self.iIconWidth - yResolution / 25
				# WTP, ray, 3 rows
				if (iK1 == 2):
					iY = yResolution - (2 * BOTTOM_CENTER_HUD_HEIGHT / 3) + self.iIconWidth - yResolution / 25
				elif (iK1 == 3):
					iY = yResolution - BOTTOM_CENTER_HUD_HEIGHT / 3 + self.iIconWidth - yResolution / 25
				screen.setLabel(szStorageLabel, "", self.setFontSize(szStored, 0), CvUtil.FONT_CENTER_JUSTIFY, iX, iY, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_RECEIVE_MOVE_CARGO_TO_CITY, -1, -1 )
				ResourceHideList.append(szStorageLabel)
				szRateLabel = "YieldNetlabel" + str(i)
				iY += yDelta
				screen.setLabel(szRateLabel, "", self.setFontSize(szRate, 0), CvUtil.FONT_CENTER_JUSTIFY, iX, iY, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_RECEIVE_MOVE_CARGO_TO_CITY, -1, -1 )
				ResourceHideList.append(szRateLabel)
				
				if SHOW_ALL_YIELDS == False and iStored == 0 and iRate == 0:
					screen.hide("YieldIcon" + str(index))
				else:
					screen.show("YieldIcon" + str(index))

				screen.registerHideList(ResourceHideList, len(ResourceHideList), RESOURCE_TABLE_HIDE)
			
			# REBEL BAR FILL PERCENTAGE
				fPercentage = float(pCity.getRebelPercent() / 100.0)
				screen.setBarPercentage("RebelBar", InfoBarTypes.INFOBAR_STORED, fPercentage)

			# REBEL BAR TEXT
				iRebel = pCity.getRebelPercent()
				szBuffer = localText.getText("TXT_KEY_MISC_REBEL", (iRebel, ))
				screen.setLabelAt("RebelText", "RebelBar", szBuffer, CvUtil.FONT_CENTER_JUSTIFY, (CITIZEN_BAR_WIDTH - (STACK_BAR_HEIGHT * 3 / 2)) / 2, STACK_BAR_HEIGHT / 2, -1.3, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
				screen.setHitTest("RebelText", HitTestTypes.HITTEST_NOHIT)
				screen.show("RebelText")
				
			# GOLD
				screen.hide("TimeText")
				screen.hide("GoldText")
				
		# EMPHISIZE DISPLAY
			for i in range(YieldTypes.NUM_YIELD_TYPES):
				if pCity.AI_getEmphasizeYieldCount(i) > 0:
					screen.hide("BonusPane" + str(i))
					screen.show("BonusPanePos" + str(i))
					screen.hide("BonusPaneNeg" + str(i))
				elif pCity.AI_getEmphasizeYieldCount(i) < 0:
					screen.hide("BonusPane" + str(i))
					screen.hide("BonusPanePos" + str(i))
					screen.show("BonusPaneNeg" + str(i))
				else:
					screen.show("BonusPane" + str(i))
					screen.hide("BonusPanePos" + str(i))
					screen.hide("BonusPaneNeg" + str(i))

			if pCity.AI_isEmphasize(AVOID_GROWTH):
				screen.overlayButtonGFC("AvoidGrowth", ArtFileMgr.getInterfaceArtInfo("INTERFACE_HIGHLIGHTED_BUTTON").getPath())
			else:
				screen.overlayButtonGFC("AvoidGrowth", None)

	# Will update the info pane strings
	def updateInfoPaneStrings( self ):
		
		iRow = 0
		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE )

		pHeadSelectedCity = CyInterface().getHeadSelectedCity()
		pHeadSelectedUnit = CyInterface().getHeadSelectedUnit()

		xResolution = screen.getXResolution()
		yResolution = screen.getYResolution()

		bShift = CyInterface().shiftKey()

		self.SELECTION_PALEL_HEIGHT = LOWER_RIGHT_CORNER_BACKGROUND_HEIGHT - (LOWER_RIGHT_CORNER_BACKGROUND_HEIGHT / 10)
		self.SELECTION_PANEL_ROW_HEIGHT = self.SELECTION_PALEL_HEIGHT / 3
		self.SELECTION_PALEL_LEFT_COLUMN_WIDTH = 60

		screen.addPanel("SelectedUnitPanel", u"", u"", True, False, xResolution - LOWER_RIGHT_CORNER_BACKGROUND_WIDTH, yResolution - LOWER_RIGHT_CORNER_BACKGROUND_HEIGHT, LOWER_RIGHT_CORNER_BACKGROUND_WIDTH, LOWER_RIGHT_CORNER_BACKGROUND_HEIGHT - (LOWER_RIGHT_CORNER_BACKGROUND_HEIGHT / 10), PanelStyles.PANEL_STYLE_EMPTY, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.hide("SelectedUnitPanel")
		
		bUnitHasPromotions = false
				
		if pHeadSelectedUnit != None:
			if pHeadSelectedUnit.getProfession() != ProfessionTypes.NO_PROFESSION:
				for i in range(gc.getNumPromotionInfos()):
					if (pHeadSelectedUnit.isHasPromotion(i) and not gc.getPromotionInfo(i).isGraphicalOnly()):
						bUnitHasPromotions = true
						break
							
		if bUnitHasPromotions:
			iHeight = (yResolution / 45)
		else:
			iHeight = (yResolution / 33) - 1
				
		screen.addTableControlGFC("SelectedUnitName", 1, xResolution - LOWER_RIGHT_CORNER_BACKGROUND_WIDTH * 105 / 100, yResolution - LOWER_RIGHT_CORNER_BACKGROUND_HEIGHT, LOWER_RIGHT_CORNER_BACKGROUND_WIDTH * 105 / 100 + 5, iHeight, False, False, self.SELECTION_PANEL_ROW_HEIGHT, self.SELECTION_PANEL_ROW_HEIGHT, TableStyles.TABLE_STYLE_STANDARD)
		screen.setStyle("SelectedUnitName", "Table_EmptyScroll_Style")
		screen.hide("SelectedUnitName")
		
		screen.addTableControlGFC("SelectedUnitText", 3, xResolution - LOWER_RIGHT_CORNER_BACKGROUND_WIDTH * 105 / 100, yResolution - LOWER_RIGHT_CORNER_BACKGROUND_HEIGHT + iHeight, LOWER_RIGHT_CORNER_BACKGROUND_WIDTH * 105 / 100 + 5, LOWER_RIGHT_CORNER_BACKGROUND_HEIGHT - (LOWER_RIGHT_CORNER_BACKGROUND_HEIGHT / 10), False, False, self.SELECTION_PANEL_ROW_HEIGHT, self.SELECTION_PANEL_ROW_HEIGHT, TableStyles.TABLE_STYLE_STANDARD)
		screen.setStyle("SelectedUnitText", "Table_EmptyScroll_Style")
		screen.hide("SelectedUnitText")
		screen.hide("SelectedUnitLabel")
		
		QueueWidth = (xResolution - CITIZEN_BAR_WIDTH) * 3 / 7
		screen.addTableControlGFC("CityBuildQueue", 2, CITIZEN_BAR_WIDTH + 5, CITY_TITLE_BAR_HEIGHT + CITY_VIEW_BOX_HEIGHT_AND_WIDTH - (MAP_EDGE_MARGIN_WIDTH * 2) + (STACK_BAR_HEIGHT * 3 / 2) + (MAP_EDGE_MARGIN_WIDTH / 2) - (CITY_MULTI_TAB_SIZE / 2) - ((CITY_MULTI_TAB_SIZE * 130 / 100) / 2) + (CITY_MULTI_TAB_SIZE / 2) - (STACK_BAR_HEIGHT / 2) + CITY_MULTI_TAB_SIZE * 130 / 100, QueueWidth, TRANSPORT_AREA_HEIGHT - (STACK_BAR_HEIGHT * 2) - SMALL_BUTTON_SIZE * 3 / 2 + 5, False, False, self.SELECTION_PANEL_ROW_HEIGHT, self.SELECTION_PANEL_ROW_HEIGHT, TableStyles.TABLE_STYLE_STANDARD)
		screen.setStyle("CityBuildQueue", "Table_EmptyScroll_Style")
		screen.hide("CityBuildQueue")

	# UNIT HEALTH BAR
		if (pHeadSelectedUnit and pHeadSelectedUnit.getUnitTravelState() == UnitTravelStates.NO_UNIT_TRAVEL_STATE and not pHeadSelectedCity and CyInterface().getShowInterface() == InterfaceVisibility.INTERFACE_SHOW and not CyEngine().isGlobeviewUp()):
			fPercentage = 1 - float(pHeadSelectedUnit.getDamage() / 100.0)
			screen.setBarPercentage("UnitHealthBar", InfoBarTypes.INFOBAR_STORED, fPercentage)
			if (fPercentage >= .666):
				screen.setStackedBarColors("UnitHealthBar", InfoBarTypes.INFOBAR_STORED, gc.getInfoTypeForString("COLOR_GREEN"))
			elif (fPercentage >= .333):
				screen.setStackedBarColors("UnitHealthBar", InfoBarTypes.INFOBAR_STORED, gc.getInfoTypeForString("COLOR_YELLOW"))
			else:
				screen.setStackedBarColors("UnitHealthBar", InfoBarTypes.INFOBAR_STORED, gc.getInfoTypeForString("COLOR_RED"))
			screen.show("UnitHealthBar")
		else:
			screen.hide("UnitHealthBar")

		if (CyEngine().isGlobeviewUp() or CyInterface().getShowInterface() == InterfaceVisibility.INTERFACE_HIDE_ALL):
			screen.hide("TimeText")
			screen.hide("GoldText")
			for iYield in EMPHASIZEYIELDS:
				screen.hide("MapYieldBox" + str(iYield))
				screen.hide("MapYieldIcon" + str(iYield))
				screen.hide("MapYieldEmphasize" + str(iYield))
				screen.hide("MapYieldDe-Emphasize" + str(iYield))
				
			screen.hide("MapBuildingSelectionMultiList")
			screen.hide("MapCityProductionBar")
			screen.hide("MapCityBuildQueue")
			screen.hide("MapCityLeftPanelShadow")
			screen.hide("MapCityRightPanelShadow")
			
			return
		else:
			screen.show("TimeText")
			screen.show("GoldText")
			screen.setTableColumnHeader("CityBuildQueue", 0, u"", QueueWidth - (QueueWidth * 3 / 10))
			screen.setTableColumnHeader("CityBuildQueue", 1, u"", QueueWidth * 3 / 10)

		if (pHeadSelectedCity):
			iOrders = CyInterface().getNumOrdersQueued()
			for i in range(iOrders):
				if (not i == 0):

					szLeftBuffer = u""
					szRightBuffer = u""
	
					if ( CyInterface().getOrderNodeType(i) == OrderTypes.ORDER_TRAIN ):
						szLeftBuffer = gc.getUnitInfo(CyInterface().getOrderNodeData1(i)).getDescription()
						szRightBuffer = str(pHeadSelectedCity.getUnitProductionTurnsLeft(CyInterface().getOrderNodeData1(i), i))
	
						if (CyInterface().getOrderNodeSave(i)):
							szLeftBuffer = u"*" + szLeftBuffer
	
					elif ( CyInterface().getOrderNodeType(i) == OrderTypes.ORDER_CONSTRUCT ):
						szLeftBuffer = gc.getBuildingInfo(CyInterface().getOrderNodeData1(i)).getDescription()
						szRightBuffer = str(pHeadSelectedCity.getBuildingProductionTurnsLeft(CyInterface().getOrderNodeData1(i), i))
	
					elif ( CyInterface().getOrderNodeType(i) == OrderTypes.ORDER_CONVINCE ):
						szLeftBuffer = gc.getFatherPointInfo(CyInterface().getOrderNodeData1(i)).getDescription()
	
					screen.appendTableRow("CityBuildQueue")
					screen.setTableText("CityBuildQueue", 0, iRow, szLeftBuffer, "", WidgetTypes.WIDGET_HELP_SELECTED, i, -1, CvUtil.FONT_LEFT_JUSTIFY )
					screen.setTableText("CityBuildQueue", 1, iRow, szRightBuffer + "  ", "", WidgetTypes.WIDGET_HELP_SELECTED, i, -1, CvUtil.FONT_RIGHT_JUSTIFY )
	
					iRow += 1

		elif (pHeadSelectedUnit and CyInterface().getShowInterface() == InterfaceVisibility.INTERFACE_SHOW and not pHeadSelectedCity):
			screen.setTableColumnHeader("SelectedUnitName", 0, u"", LOWER_RIGHT_CORNER_BACKGROUND_WIDTH * 105 / 100 + 5)
			screen.setTableColumnHeader("SelectedUnitText", 0, u"", LOWER_RIGHT_CORNER_BACKGROUND_WIDTH * 105 / 100 - self.SELECTION_PALEL_LEFT_COLUMN_WIDTH)
			screen.setTableColumnHeader("SelectedUnitText", 1, u"", self.SELECTION_PALEL_LEFT_COLUMN_WIDTH)

			if (CyInterface().mirrorsSelectionGroup()):
				pSelectedGroup = pHeadSelectedUnit.getGroup()
			else:
				pSelectedGroup = 0

			if (CyInterface().getLengthSelectionList() > 1):
				screen.setText("SelectedUnitLabel", "Background", localText.getText("TXT_KEY_UNIT_STACK", (CyInterface().getLengthSelectionList(), )), CvUtil.FONT_LEFT_JUSTIFY, xResolution - LOWER_RIGHT_CORNER_BACKGROUND_WIDTH, yResolution - LOWER_RIGHT_CORNER_BACKGROUND_HEIGHT , -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_UNIT_NAME, -1, -1 )
				screen.hide("UnitHealthBar")

				iRow += 1

				if ((pSelectedGroup == 0) or (pSelectedGroup.getLengthMissionQueue() <= 1)):
					if (pHeadSelectedUnit):
						for i in range(gc.getNumUnitInfos()):
							iCount = CyInterface().countEntities(i)

							if (iCount > 0):
								szRightBuffer = u""
								szLeftBuffer = gc.getUnitInfo(i).getDescription()

								if (iCount > 1):
									szRightBuffer = u"(" + str(iCount) + u")"

								szBuffer = szLeftBuffer + u"  " + szRightBuffer
								iRow = screen.appendTableRow("SelectedUnitText")
								screen.show("SelectedUnitText")
								screen.show("SelectedUnitPanel")

			else:
				if (pHeadSelectedUnit.getHotKeyNumber() == -1):
					szBuffer = localText.getText("INTERFACE_PANE_UNIT_NAME", (pHeadSelectedUnit.getName(), ))
				else:
					szBuffer = localText.getText("INTERFACE_PANE_UNIT_NAME_HOT_KEY", (pHeadSelectedUnit.getHotKeyNumber(), pHeadSelectedUnit.getName()))
				szBuffer = "<font=2b>" + szBuffer + "</font>"

				iRow = screen.appendTableRow("SelectedUnitName")
				
				screen.setTableRowHeight("SelectedUnitText", iRow, iHeight)
				screen.setTableRowHeight("SelectedUnitName", iRow, iHeight)
				
				screen.setTableText("SelectedUnitName", 0, iRow, szBuffer, "", WidgetTypes.WIDGET_UNIT_NAME, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
				screen.show("SelectedUnitName")
				screen.show("SelectedUnitText")

				if (pHeadSelectedUnit.getProfession() != -1):
					szBuffer = localText.getText("INTERFACE_PANE_UNIT_NAME", (gc.getProfessionInfo(pHeadSelectedUnit.getProfession()).getDescription(), ))
					szBuffer = "<font=2b>" + szBuffer + "</font>"

					iRow = screen.appendTableRow("SelectedUnitText")
					screen.setTableText("SelectedUnitText", 0, iRow, szBuffer, "", WidgetTypes.WIDGET_UNIT_NAME, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
					screen.show("SelectedUnitText")

				if ((pSelectedGroup == 0) or (pSelectedGroup.getLengthMissionQueue() <= 1)):
					screen.show("SelectedUnitText")
					screen.show("SelectedUnitPanel")

					szBuffer, szLeftBuffer, szRightBuffer = u"", u"", u""

					if (pHeadSelectedUnit.canFight()):
						szLeftBuffer = localText.getText("INTERFACE_PANE_STRENGTH", ())
						if (pHeadSelectedUnit.isFighting()):
							szRightBuffer = u"?/%d%c" %(pHeadSelectedUnit.baseCombatStr(), CyGame().getSymbolID(FontSymbols.STRENGTH_CHAR))
						elif (pHeadSelectedUnit.isHurt()):
							szRightBuffer = u"%.1f/%d%c" %(((float(pHeadSelectedUnit.baseCombatStr() * pHeadSelectedUnit.currHitPoints())) / (float(pHeadSelectedUnit.maxHitPoints()))), pHeadSelectedUnit.baseCombatStr(), CyGame().getSymbolID(FontSymbols.STRENGTH_CHAR))
						else:
							szRightBuffer = u"%d%c" %(pHeadSelectedUnit.baseCombatStr(), CyGame().getSymbolID(FontSymbols.STRENGTH_CHAR))

					if pHeadSelectedUnit.baseCombatStr() > 0:
						szBuffer = szLeftBuffer + szRightBuffer
						screen.setLabelAt("StrengthText", "UnitHealthBar", szBuffer, CvUtil.FONT_CENTER_JUSTIFY, UNIT_HEALTH_BAR_WIDTH / 2, STACK_BAR_HEIGHT / 2, -1.3, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
					else:
						screen.hide("UnitHealthBar")

					szLeftBuffer = u""
					szRightBuffer = u""

					if ((pHeadSelectedUnit.movesLeft() % gc.getMOVE_DENOMINATOR()) > 0 ):
						iDenom = 1
					else:
						iDenom = 0
					iCurrMoves = ((pHeadSelectedUnit.movesLeft() / gc.getMOVE_DENOMINATOR()) + iDenom )
					szLeftBuffer = localText.getText("INTERFACE_PANE_MOVEMENT", ())
					if (pHeadSelectedUnit.baseMoves() == iCurrMoves):
						szRightBuffer = u"%d%c" %(pHeadSelectedUnit.baseMoves(), CyGame().getSymbolID(FontSymbols.MOVES_CHAR) )
					else:
						szRightBuffer = u"%d/%d%c" %(iCurrMoves, pHeadSelectedUnit.baseMoves(), CyGame().getSymbolID(FontSymbols.MOVES_CHAR) )

					iRow = screen.appendTableRow("SelectedUnitText")
					screen.setTableText("SelectedUnitText", 0, iRow, szLeftBuffer, "", WidgetTypes.WIDGET_HELP_SELECTED, -1, -1, CvUtil.FONT_LEFT_JUSTIFY )
					screen.setTableText("SelectedUnitText", 1, iRow, szRightBuffer, "", WidgetTypes.WIDGET_HELP_SELECTED, -1, -1, CvUtil.FONT_RIGHT_JUSTIFY )
					screen.show("SelectedUnitText")
					screen.show("SelectedUnitPanel")

					szLeftBuffer = ""
					iMaxWidthPromotions = LOWER_RIGHT_CORNER_BACKGROUND_WIDTH * 105 / 100 - self.SELECTION_PALEL_LEFT_COLUMN_WIDTH
					iCurrentWidthPromotions = 0
					for i in range(gc.getNumPromotionInfos()):
						if (pHeadSelectedUnit.isHasPromotion(i) and not gc.getPromotionInfo(i).isGraphicalOnly()):
							if iCurrentWidthPromotions + 18 > iMaxWidthPromotions:
								iRow = screen.appendTableRow("SelectedUnitText")
								screen.setTableText("SelectedUnitText", 0, iRow, szLeftBuffer, "", WidgetTypes.WIDGET_HELP_UNIT_PROMOTION, pHeadSelectedUnit.getOwner(), pHeadSelectedUnit.getID(), CvUtil.FONT_LEFT_JUSTIFY )
								iCurrentWidthPromotions = 0
								szLeftBuffer = ""
							iCurrentWidthPromotions += 18
							szLeftBuffer += "<img=%s size=16></img>" % (gc.getPromotionInfo(i).getButton(), )

					if len(szLeftBuffer) > 0:
						iRow = screen.appendTableRow("SelectedUnitText")
						screen.setTableText("SelectedUnitText", 0, iRow, szLeftBuffer, "", WidgetTypes.WIDGET_HELP_UNIT_PROMOTION, pHeadSelectedUnit.getOwner(), pHeadSelectedUnit.getID(), CvUtil.FONT_LEFT_JUSTIFY )

			if (pSelectedGroup):
				iNodeCount = pSelectedGroup.getLengthMissionQueue()
				if (iNodeCount > 1):
					for i in range( iNodeCount ):
						szLeftBuffer = u""
						szRightBuffer = u""

						if (gc.getMissionInfo(pSelectedGroup.getMissionType(i)).isBuild()):
							if (i == 0):
								szLeftBuffer = gc.getBuildInfo(pSelectedGroup.getMissionData1(i)).getDescription()
								szRightBuffer = localText.getText("INTERFACE_CITY_TURNS", (pSelectedGroup.plot().getBuildTurnsLeft(pSelectedGroup.getMissionData1(i), 0, 0), ))
							else:
								szLeftBuffer = u"%s..." %(gc.getBuildInfo(pSelectedGroup.getMissionData1(i)).getDescription())
						else:
							szLeftBuffer = u"%s..." %(gc.getMissionInfo(pSelectedGroup.getMissionType(i)).getDescription())

						szBuffer = szLeftBuffer + "  " + szRightBuffer
						screen.appendTableRow("SelectedUnitText")
						screen.setTableText("SelectedUnitText", 0, iRow, szLeftBuffer, "", WidgetTypes.WIDGET_HELP_SELECTED, i, -1, CvUtil.FONT_LEFT_JUSTIFY )
						screen.setTableText("SelectedUnitText", 1, iRow, szRightBuffer, "", WidgetTypes.WIDGET_HELP_SELECTED, i, -1, CvUtil.FONT_RIGHT_JUSTIFY )
						screen.show("SelectedUnitText")
						screen.show("SelectedUnitPanel")
						iRow += 1

	# Will update the scores
	def updateScoreStrings( self ):
		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE )

		xResolution = screen.getXResolution()
		yResolution = screen.getYResolution()

		screen.hide("ScoreBackground")

		for i in range( gc.getMAX_PLAYERS() ):
			szName = "ScoreText" + str(i)
			screen.hide( szName )

		self.SCORE_BACKGROUND_SIDE_MARGIN = 21
		self.SCORE_BACKGROUND_BOTTOM_MARGIN_LARGE = 18
		self.SCORE_BACKGROUND_BOTTOM_MARGIN_SMALL = -100
		self.SCORE_TEXT_BOTTOM_MARGIN_LARGE = 38
		self.SCORE_TEXT_BOTTOM_MARGIN_SMALL = -80

		pHeadSelectedCity = CyInterface().getHeadSelectedCity()

		if ((CyInterface().getShowInterface() != InterfaceVisibility.INTERFACE_HIDE_ALL and CyInterface().getShowInterface() != InterfaceVisibility.INTERFACE_MINIMAP_ONLY)):
			if (CyInterface().isScoresVisible() and not CyInterface().isCityScreenUp() and not CyEngine().isGlobeviewUp() ):

				iCount = 8
				iBtnHeight = 22
				iWidth = int((25 * xResolution) / 100)
				yCoord = yResolution - SADDLE_HEIGHT * 14/16 - self.SCORE_TEXT_BOTTOM_MARGIN_SMALL

				# Scrollable Scoreboard
				if (not pHeadSelectedCity):
					screen.addTableControlGFC("ScoreBackground", 1, xResolution - self.SCORE_BACKGROUND_SIDE_MARGIN/2 - iWidth, yCoord - (iBtnHeight * (iCount - 1)), iWidth, (iBtnHeight * (iCount - 1)), False, False, 0, 0, TableStyles.TABLE_STYLE_STANDARD)
					screen.enableSelect("ScoreBackground", False)
					screen.setTableColumnHeader("ScoreBackground", 0, "", iWidth - 10)
			
				i = 0
				while (i < gc.getMAX_CIV_TEAMS()):
					eTeam = gc.getGame().getRankTeam(i)
					if (gc.getTeam(gc.getGame().getActiveTeam()).isHasMet(eTeam) or gc.getTeam(eTeam).isHuman() or gc.getGame().isDebugMode()):
						#j = gc.getMAX_CIV_PLAYERS() - 1
						#while (j > -1):
						j = 0
						while (j < gc.getMAX_CIV_PLAYERS()):
							ePlayer = gc.getGame().getRankPlayer(j)
							if (gc.getPlayer(ePlayer).isAlive()):
								# < JAnimals Mod Start 1/1 > AND # R&R, ray, Church
								if (gc.getGame().isBarbarianPlayer(ePlayer) or gc.getGame().isChurchPlayer(ePlayer)):
									#j = j - 1
									j = j + 1
									continue
								# < JAnimals Mod End 1/1 >
								if (gc.getPlayer(ePlayer).getTeam() == eTeam):
									szBuffer = u"<font=2>"

									if (gc.getGame().isGameMultiPlayer()):
										if (not (gc.getPlayer(ePlayer).isTurnActive())):
											szBuffer = szBuffer + "*"

									if gc.getGame().getPlayerScore(ePlayer) > 0:
										szBuffer += u"%d: " % gc.getGame().getPlayerScore(ePlayer)
									
									if (gc.getTeam(eTeam).isParentOf(gc.getGame().getActiveTeam())):
											szTempBuffer = u"%c" %(CyGame().getSymbolID(FontSymbols.ANCHOR_EUROPE_CHAR))
											szBuffer = szBuffer +  szTempBuffer  + " "
									
									if (not CyInterface().isFlashingPlayer(ePlayer) or CyInterface().shouldFlash(ePlayer)):
										if (ePlayer == gc.getGame().getActivePlayer()):
											szTempBuffer = u"[<color=%d,%d,%d,%d>%s</color>]" %(gc.getPlayer(ePlayer).getPlayerTextColorR(), gc.getPlayer(ePlayer).getPlayerTextColorG(), gc.getPlayer(ePlayer).getPlayerTextColorB(), gc.getPlayer(ePlayer).getPlayerTextColorA(), gc.getPlayer(ePlayer).getName())
										else:
											szTempBuffer = u"<color=%d,%d,%d,%d>%s</color>" %(gc.getPlayer(ePlayer).getPlayerTextColorR(), gc.getPlayer(ePlayer).getPlayerTextColorG(), gc.getPlayer(ePlayer).getPlayerTextColorB(), gc.getPlayer(ePlayer).getPlayerTextColorA(), gc.getPlayer(ePlayer).getName())
									else:
										szTempBuffer = u"%s" %(gc.getPlayer(ePlayer).getName())
									szBuffer = szBuffer + szTempBuffer

									if (gc.getTeam(eTeam).isAlive()):
										if ( not (gc.getTeam(gc.getGame().getActiveTeam()).isHasMet(eTeam)) ):
											szBuffer = szBuffer + (" ?")
										if (gc.getTeam(eTeam).isAtWar(gc.getGame().getActiveTeam())):
											szBuffer = szBuffer + "("  + localText.getColorText("TXT_KEY_WAR", (), gc.getInfoTypeForString("COLOR_RED")).upper() + ")"
										if (not gc.getPlayer(ePlayer).isHuman() and ePlayer != gc.getGame().getActivePlayer()):
											iAtt = gc.getPlayer(ePlayer).AI_getAttitude(gc.getGame().getActivePlayer())
											cAtt = unichr(CyGame().getSymbolID(FontSymbols.ATTITUDE_FURIOUS_CHAR) + iAtt)
											szBuffer += " " +cAtt #+ " (" +u"%d" %iGold  + localText.getText("[ICON_GOLD]", ()) + ")"
										else:
											szBuffer += "    "
										if (gc.getTeam(eTeam).isOpenBorders(gc.getGame().getActiveTeam())):
											szTempBuffer = u"%c" %(CyGame().getSymbolID(FontSymbols.OPEN_BORDERS_CHAR))
											szBuffer = szBuffer + szTempBuffer
										if (gc.getTeam(eTeam).isDefensivePact(gc.getGame().getActiveTeam())):
											szTempBuffer = u"%c" %(CyGame().getSymbolID(FontSymbols.DEFENSIVE_PACT_CHAR))
											szBuffer = szBuffer + szTempBuffer
										# Alignement 
										elif (not gc.getTeam(eTeam).isOpenBorders(gc.getGame().getActiveTeam())):
											szBuffer = szBuffer + "    "
										if (not gc.getPlayer(ePlayer).isHuman() and ePlayer != gc.getGame().getActivePlayer() and not gc.getPlayer(ePlayer).isEurope() and (not gc.getPlayer(ePlayer).isNative() or gc.getGame().isOption(GameOptionTypes.GAMEOPTION_NO_MORE_VARIABLES_HIDDEN))):
											iGold = gc.getPlayer(ePlayer).getGold()
											# Alignement issues 
											if iGold<10:
												szBuffer += "        (" +u"%d" %iGold  + localText.getText("[ICON_GOLD]", ()) + ")"
											elif iGold<100:
												szBuffer += "     (" +u"%d" %iGold  + localText.getText("[ICON_GOLD]", ()) + ")"
											elif iGold<1000:
												szBuffer += "   (" +u"%d" %iGold  + localText.getText("[ICON_GOLD]", ()) + ")"
											else:	
												szBuffer += " (" +u"%d" %iGold  + localText.getText("[ICON_GOLD]", ()) + ")"
										else:
											szBuffer += "               "
									if (CyGame().isNetworkMultiPlayer()):
										szBuffer = szBuffer + CyGameTextMgr().getNetStats(ePlayer)

									if (gc.getPlayer(ePlayer).isHuman() and CyInterface().isOOSVisible()):
										szTempBuffer = u" <color=255,0,0>* %s *</color>" %(CyGameTextMgr().getOOSSeeds(ePlayer))
										szBuffer = szBuffer + szTempBuffer

									szBuffer = szBuffer + "</font>"

									if ( CyInterface().determineWidth( szBuffer ) > iWidth ):
										iWidth = CyInterface().determineWidth( szBuffer )

									szName = "ScoreText" + str(ePlayer)
									if ( CyInterface().getShowInterface() == InterfaceVisibility.INTERFACE_SHOW or CyInterface().isInAdvancedStart() or pHeadSelectedCity != None):
										yCoord = yResolution - SADDLE_HEIGHT * 38 / 100 - 20
									else:
										yCoord = yResolution - SADDLE_HEIGHT - self.SCORE_TEXT_BOTTOM_MARGIN_SMALL

									if not pHeadSelectedCity:
										iRow = screen.appendTableRow("ScoreBackground")
										screen.setTableText("ScoreBackground", 0, iRow, szBuffer, "", WidgetTypes.WIDGET_CONTACT_CIV, ePlayer, -1, CvUtil.FONT_RIGHT_JUSTIFY)

									CyInterface().checkFlashReset(ePlayer)

									iCount += 1
							#j = j - 1
							j = j + 1
					#i = i - 1
					i = i + 1
				
				if ( CyInterface().getShowInterface() == InterfaceVisibility.INTERFACE_SHOW or CyInterface().isInAdvancedStart() or pHeadSelectedCity != None):
					yCoord = yResolution - SADDLE_HEIGHT * 38 / 100
				else:
					yCoord = yResolution - SADDLE_HEIGHT - self.SCORE_BACKGROUND_BOTTOM_MARGIN_SMALL

				if not pHeadSelectedCity:
					screen.show("ScoreBackground")

	# Will update the help Strings
	def updateHelpStrings( self ):
		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE )

		if ( CyInterface().getShowInterface() == InterfaceVisibility.INTERFACE_HIDE_ALL ):
			screen.setHelpTextString("")
		else:
			screen.setHelpTextString( CyInterface().getHelpString() )

		return 0

	# Will set the promotion button position
	def setPromotionButtonPosition( self, szName, iPromotionCount ):
		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE )
		yResolution = screen.getYResolution()

		if ( CyInterface().getShowInterface() == InterfaceVisibility.INTERFACE_SHOW ):
			screen.moveItem( szName, 266 - (24 * (iPromotionCount / 6)), yResolution - 144 + (24 * (iPromotionCount % 6)), -0.3 )

	# Will set the selection button position
	def setScoreTextPosition( self, szButtonID, iWhichLine ):

		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE )
		yResolution = screen.getYResolution()
		if ( CyInterface().getShowInterface() == InterfaceVisibility.INTERFACE_SHOW ):
			yCoord = yResolution - 180
		else:
			yCoord = yResolution - 88
		screen.moveItem( szButtonID, 996, yCoord - (iWhichLine * 18), -0.3 )

	# Will build the globeview UI
	def updateGlobeviewButtons( self ):
		kInterface = CyInterface()
		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE )
		xResolution = screen.getXResolution()
		yResolution = screen.getYResolution()

		kEngine = CyEngine()
		kGLM = CyGlobeLayerManager()
		iCurrentLayerID = kGLM.getCurrentLayerID()

		# Positioning things based on the visibility of the globe
		#screen.setHelpTextArea( HELPTEXT_AREA_MAX_WIDTH, FontTypes.SMALL_FONT, HELTTEXT_AREA_X_MARGIN, yResolution - (MINI_MAP_RADIUS - 80), -0.1, False, "", True, False, CvUtil.FONT_LEFT_JUSTIFY, HELPTEXT_AREA_MIN_WIDTH)
		# Set base Y position for the LayerOptions, if we find them
		if CyInterface().getShowInterface() == InterfaceVisibility.INTERFACE_HIDE:
			iY = yResolution - iGlobeLayerOptionsY_Minimal
		else:
			iY = yResolution - LOWER_RIGHT_CORNER_BACKGROUND_HEIGHT

		# Hide the layer options ... all of them
		for i in range (20):
			szName = "GlobeLayerOption" + str(i)
			screen.hide(szName)

		# Setup the GlobeLayer panel
		if kEngine.isGlobeviewUp() and CyInterface().getShowInterface() != InterfaceVisibility.INTERFACE_HIDE_ALL:
			# set up panel
			if iCurrentLayerID != -1 and kGLM.getLayer(iCurrentLayerID).getNumOptions() != 0:
				bHasOptions = True
			else:
				bHasOptions = False
				screen.hide("ScoreBackground")

			# set up toggle button
			screen.overlayButtonGFC("GlobeToggle", ArtFileMgr.getInterfaceArtInfo("INTERFACE_HIGHLIGHTED_BUTTON").getPath())

			# Set GlobeLayer indicators correctly
			for i in range(kGLM.getNumLayers()):
				szButtonID = "GlobeLayer" + str(i)
				if (iCurrentLayerID == i):
					screen.overlayButtonGFC(szButtonID, ArtFileMgr.getInterfaceArtInfo("INTERFACE_HIGHLIGHTED_BUTTON").getPath())
				else:
					screen.overlayButtonGFC(szButtonID, None)

			# Set up options pane
			if bHasOptions:
				kLayer = kGLM.getLayer(iCurrentLayerID)

				iCurY = iY
				iNumOptions = kLayer.getNumOptions()
				iCurOption = kLayer.getCurrentOption()
				iMaxTextWidth = -1
				for iTmp in range(iNumOptions):
					iOption = iTmp
					szName = "GlobeLayerOption" + str(iOption)
					szCaption = kLayer.getOptionName(iOption)
					if(iOption == iCurOption):
						szBuffer = "  <color=0,255,0>%s</color>  " % (szCaption)
					else:
						szBuffer = "  %s  " % (szCaption)
					iTextWidth = CyInterface().determineWidth( szBuffer )

					screen.setText( szName, "Background", szBuffer, CvUtil.FONT_LEFT_JUSTIFY, xResolution - 9 - iTextWidth, iCurY-iGlobeLayerOptionHeight - 10, -0.3, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GLOBELAYER_OPTION, iOption, -1 )
					screen.show( szName )

					iCurY -= iGlobeLayerOptionHeight

					if iTextWidth > iMaxTextWidth:
						iMaxTextWidth = iTextWidth

				#make extra space
				iCurY -= iGlobeLayerOptionHeight;
				iPanelWidth = iMaxTextWidth + 32
				iPanelHeight = iY - iCurY
				iPanelX = xResolution - 14 - iPanelWidth
				iPanelY = iCurY
#				screen.setPanelSize("ScoreBackground", iPanelX, iPanelY, iPanelWidth, iPanelHeight )
#				screen.show("ScoreBackground")

		else:
			if iCurrentLayerID != -1:
				kLayer = kGLM.getLayer(iCurrentLayerID)
				if kLayer.getName() == "RESOURCES":
					screen.overlayButtonGFC("ResourceIcons", ArtFileMgr.getInterfaceArtInfo("INTERFACE_HIGHLIGHTED_BUTTON").getPath())
				else:
					screen.overlayButtonGFC("ResourceIcons", None)

				if kLayer.getName() == "UNITS":
					screen.overlayButtonGFC("UnitIcons", ArtFileMgr.getInterfaceArtInfo("INTERFACE_HIGHLIGHTED_BUTTON").getPath())
				else:
					screen.overlayButtonGFC("UnitIcons", None)
			else:
				screen.overlayButtonGFC("ResourceIcons", None)
				screen.overlayButtonGFC("UnitIcons", None)

			if CyUserProfile().getGrid():
				screen.overlayButtonGFC("Grid", ArtFileMgr.getInterfaceArtInfo("INTERFACE_HIGHLIGHTED_BUTTON").getPath())
			else:
				screen.overlayButtonGFC("Grid", None)
			if CyUserProfile().getMap():
				screen.overlayButtonGFC("BareMap", ArtFileMgr.getInterfaceArtInfo("INTERFACE_HIGHLIGHTED_BUTTON").getPath())
			else:
				screen.overlayButtonGFC("BareMap", None)
			if CyUserProfile().getYields():
				screen.overlayButtonGFC("Yields", ArtFileMgr.getInterfaceArtInfo("INTERFACE_HIGHLIGHTED_BUTTON").getPath())
			else:
				screen.overlayButtonGFC("Yields", None)
			if CyUserProfile().getScores():
				screen.overlayButtonGFC("ScoresVisible", ArtFileMgr.getInterfaceArtInfo("INTERFACE_HIGHLIGHTED_BUTTON").getPath())
			else:
				screen.overlayButtonGFC("ScoresVisible", None)

			screen.hide("InterfaceGlobeLayerPanel")
			screen.overlayButtonGFC("GlobeToggle", None)

	# Update minimap buttons
	def setMinimapButtonVisibility( self, bVisible ):
		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE )
		kInterface = CyInterface()
		kGLM = CyGlobeLayerManager()
		xResolution = screen.getXResolution()
		yResolution = screen.getYResolution()

		if (CyInterface().isCityScreenUp() or (CyInterface().getHeadSelectedCity() != None and not CyEngine().isGlobeviewUp())):
			bVisible = False

		kMainButtons = ["UnitIcons", "Grid", "BareMap", "Yields", "ScoresVisible", "ResourceIcons"]
		kGlobeButtons = []
		for i in range(kGLM.getNumLayers()):
			szButtonID = "GlobeLayer" + str(i)
			kGlobeButtons.append(szButtonID)

		if bVisible:
			if CyEngine().isGlobeviewUp():
				kHide = kMainButtons
				kShow = kGlobeButtons
			else:
				kHide = kGlobeButtons
				kShow = kMainButtons
			screen.show("GlobeToggle")

		else:
			kHide = kMainButtons + kGlobeButtons
			kShow = []
			screen.hide("GlobeToggle")

		for szButton in kHide:
			screen.hide(szButton)

		if len(kShow) > 0:
			for szButton in kShow:
				screen.show(szButton)

	# Will handle the input for this screen...
	def handleInput ( self, inputClass ):

		global BUILDING_MANAGMENT_PANEL_UP
		global AUTOMATION_MANAGMENT_PANEL_UP
		global MAP_MANAGMENT_PANEL_UP
		global SHOW_ALL_YIELDS
		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE )

		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):

			if (inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL and inputClass.getData1() == BUILDING_MANAGMENT_TOGGLE):
				BUILDING_MANAGMENT_PANEL_UP = True
				AUTOMATION_MANAGMENT_PANEL_UP, MAP_MANAGMENT_PANEL_UP = False, False
				self.updateSelectionButtons()

			elif (inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL and inputClass.getData1() == AUTOMATION_MANAGMENT_TOGGLE):
				AUTOMATION_MANAGMENT_PANEL_UP = True
				BUILDING_MANAGMENT_PANEL_UP, MAP_MANAGMENT_PANEL_UP = False, False
				self.updateSelectionButtons()

			elif (inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL and inputClass.getData1() == MAP_MANAGMENT_TOGGLE):
				MAP_MANAGMENT_PANEL_UP = True
				AUTOMATION_MANAGMENT_PANEL_UP, BUILDING_MANAGMENT_PANEL_UP = False, False
				self.updateSelectionButtons()

			elif (inputClass.getButtonType() == WidgetTypes.WIDGET_CLOSE_SCREEN):
				CyInterface().clearSelectedCities()		

			elif (inputClass.getButtonType() == WidgetTypes.WIDGET_HELP_SHOW_OR_HIDE_YIELDS):
				if SHOW_ALL_YIELDS == True:
					SHOW_ALL_YIELDS = False
					screen.overlayButtonGFC("ShowOrHideYields", ArtFileMgr.getInterfaceArtInfo("INTERFACE_HIGHLIGHTED_BUTTON").getPath())
				else:
					SHOW_ALL_YIELDS = True
 					screen.overlayButtonGFC("ShowOrHideYields", None)
				self.updateResourceTable()

			elif (inputClass.getButtonType() == WidgetTypes.WIDGET_GENERAL and inputClass.getData1() == ACHIEVE_ADVISOR_SCREEN_MI):
				CvScreensInterface.showAchieveAdvisorScreen()

		return 0
	
	# Updates the Screen
	def update( self, fDelta ):
		return

	# Adds Mouse Over Help to General Widgets
	def getWidgetHelp( self, argsList ):
		iScreen, eWidgetType, iData1, iData2, bOption = argsList

		if eWidgetType == WidgetTypes.WIDGET_GENERAL:
			if iData1 == BUILDING_MANAGMENT_TOGGLE:
				return localText.getText("TXT_KEY_INTERFACE_BUILD_TOGGLE", ());
			elif iData1 == MAP_MANAGMENT_TOGGLE:
				return localText.getText("TXT_KEY_INTERFACE_MAP_TOGGLE", ());
			elif iData1 == AUTOMATION_MANAGMENT_TOGGLE:
				return localText.getText("TXT_KEY_INTERFACE_GOVERNOR_TOGGLE", ());
			elif iData1 == ACHIEVE_ADVISOR_SCREEN_MI:
				return localText.getText("TXT_KEY_INTERFACE_ACHIEVE_ADVISOR", ("F10", ""));
			elif iData1 == VET_NEW_CAPACITY:
				return localText.getText("TXT_KEY_NEW_STORAGE_CITY_SCREEN", ());
			elif iData1 == NEW_HARBOUR_SYSTEM:
				return localText.getText("TXT_KEY_CONCEPT_NEW_HARBOUR_SYSTEM", ());
			elif iData1 == NEW_BARRACKS_SYSTEM:
				return localText.getText("TXT_KEY_CONCEPT_NEW_BARRACKS_SYSTEM", ());
			elif iData1 == HELP_TOTAL_HAPPINESS_POSITIVE:
				return localText.getText("TXT_KEY_TOTAL_HAPPINESS_POSITIVE_EFFECTS", ());
			elif iData1 == HELP_TOTAL_HAPPINESS_NEGATIVE:
				return localText.getText("TXT_KEY_TOTAL_HAPPINESS_NEGATIVE_EFFECTS", ());
			elif iData1 == HELP_TOTAL_HAPPINESS_ZERO:
				return localText.getText("TXT_KEY_TOTAL_HAPPINESS_ZERO_EFFECTS", ());
			elif iData1 == HELP_TOTAL_LAW_POSITIVE:
				return localText.getText("TXT_KEY_TOTAL_LAW_POSITIVE_EFFECTS", ());
			elif iData1 == HELP_TOTAL_LAW_NEGATIVE:
				return localText.getText("TXT_KEY_TOTAL_LAW_NEGATIVE_EFFECTS", ());
			elif iData1 == HELP_TOTAL_LAW_ZERO:
				return localText.getText("TXT_KEY_TOTAL_LAW_ZERO_EFFECTS", ());
			elif iData1 == TIMETEXT_ID:
				return self.timeTextHelp()

		return u""
	
	def timeTextHelp(self):
		player = gc.getPlayer(gc.getGame().getActivePlayer())
		team = gc.getTeam(player.getTeam())
		
		bMessage = False
		szMessage  = localText.getText("TXT_KEY_TIMETEXT_TURN_NO_LIMIT", (CyGame().getGameTurn() + 1,))
		for iLoopVC in range(gc.getNumVictoryInfos()):
			victory = gc.getVictoryInfo(iLoopVC)
			if gc.getGame().isVictoryValid(iLoopVC):
				if (victory.isEndScore()):
					szMessage  = localText.getText("TXT_KEY_TIMETEXT_TURN", (CyGame().getGameTurn() + 1,CyGame().getEstimateEndTurn() +1))
					bMessage  = True
					break
		if not bMessage: 
			for i in range(gc.getMAX_CIV_PLAYERS()):
				if gc.getPlayer(i).isInRevolution():
					szMessage  = localText.getText("TXT_KEY_TIMETEXT_TURN", (CyGame().getGameTurn() + 1,CyGame().getEstimateEndTurn() +1))
					break

		if gc.getGame().isOption(GameOptionTypes.GAMEOPTION_NO_MORE_VARIABLES_HIDDEN):
			szMessage += localText.getText("TXT_KEY_CURRENT_ERA", (gc.getEraInfo(player.getCurrentEra()).getText(),()))
		szMessage += localText.getText("TXT_KEY_TIMETEXT_IMMIGRATION_TITLE", ())
		szMessage += localText.getText("TXT_KEY_TIMETEXT_IMMIGRATION", (player.getCrossesStored(), player.immigrationThreshold(), gc.getYieldInfo(YieldTypes.YIELD_CROSSES).getChar(), player.getYieldRate(YieldTypes.YIELD_CROSSES)))
		szMessage += localText.getText("TXT_KEY_TIMETEXT_REBELS_TITLE", ())
		szMessage += localText.getText("TXT_KEY_TIMETEXT_REBELS", (team.getRebelPercent(),))
		szMessage += localText.getText("TXT_KEY_TIMETEXT_GENERALS_TITLE", ())
		szMessage += localText.getText("TXT_KEY_TIMETEXT_GENERALS", (player.getCombatExperience(), player.greatGeneralThreshold()))
		
		Fathers = []
		for i in range(gc.getNumFatherCategoryInfos()):
			Fathers.append([])
		
		for i in range(gc.getNumFatherInfos()):
			if gc.getGame().getFatherTeam(i) == TeamTypes.NO_TEAM:
				if not team.isFatherIgnore(i):
					iType = gc.getFatherInfo(i).getFatherCategory()
					Fathers[iType].append(i)
		
		for category in Fathers:
			category.sort(lambda x, y: team.getFatherPointCost(x, gc.getFatherInfo(x).getFatherCategory()) - team.getFatherPointCost(y, gc.getFatherInfo(y).getFatherCategory()))
		
		for iCategory in range(gc.getNumFatherCategoryInfos()):
			szCategory = gc.getFatherCategoryInfo(iCategory).getDescription()
			if Fathers[iCategory] > []:
				iFather = Fathers[iCategory][0]
				szFather = gc.getFatherInfo(iFather).getDescription()
				szMessage += localText.getText("TXT_KEY_TIMETEXT_FF", (szCategory, szFather))
				for iFatherPoint in range(gc.getNumFatherPointInfos()):
					iCost = team.getFatherPointCost(iFather, iFatherPoint)
					if iCost > 0:
						iPoints = team.getFatherPoints(iFatherPoint)
						szMessage += localText.getText("TXT_KEY_TIMETEXT_FF_POINTS", (iPoints, iCost, gc.getFatherPointInfo(iFatherPoint).getChar()))
			
		szMessage += localText.getText("TXT_KEY_TIMETEXT_CLOCK", (getClockText(),()))
				
		return szMessage

