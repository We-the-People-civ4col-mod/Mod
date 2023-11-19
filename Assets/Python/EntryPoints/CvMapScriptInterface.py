## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil

"""
MAP SCRIPT INTERFACE

This file contains stubs for all the functions that a map script can override. To create a map script,
copy the imports at the top of this file into a new file.

Then you can implement as many of the below functions as you want. If one of the functions is not implemented
in your map script, or if the function calls CyPythonMgr().allowDefaultImpl(), then the Civ game code will handle
that aspect of map generation in the default manner.

Note: for the most part the functions in this file have a "flat" structure; i.e. they are called one by one by
the game code during map generation, in the approximate order that the functions are listed in this file.
However, there are a few functions that will not be called under certain circumstances. These functions
are notated with a WARNING in their docstring below. For example, see generateRandomMap() and findStartingArea() below.

-aszybalski 20 aug 2004
"""

"""
ADDITIONAL NOTES FROM SIRIAN

With my urging, Soren decided to move more of the map script defaults from
C++ to Python, better exposing the workings for map scripters to figure out.

As of July 2005, some functions listed here in CvMapScriptInterface.py have
become Mandatory Functions. That is, your script must contain versions of
these functions, as the entire function is handled in Python. Without a
version of a Mandatory Function, your script will produce errors. The rest
of the functions listed in this file remain Optional Overrides.

For a complete list of Mandatory Functions, check the default minimum map
script, Continents.py that is located in the /PublicMaps/ folder with the
rest of the scripts. Except where noted, all functions used in that script
compose the list of Mandatory Functions.

You can also create a new map script by copying an existing script in to a
new file under a new filename and then modifying it. For the numerous map
scripts that I produced to ship with the game, I used this shortcut rather
than building each new script from scratch. The danger of copying from an
existing script is dragging along unneeded or unwanted bits of code, but I
found it much easier to build from work already done by others. A lot can
be learned fairly quickly this way. Would-be map scripters, for instance,
can build from my work in some cases, rather than having first to duplicate
functions that I had to build from scratch.

We have tried to make map scripting as user friendly as possible without
compromising any of its power. We hope that you find the balance we have
chosen to your liking. Good luck with any map scripting projects you undertake.

- Bob Thomas   July 4, 2005
"""

"""
ORDER OF OPERATIONS

Key:	+ Category of functions
		* Function is called once
		! Function is (or can be) called multiple times

The following list indicates the order in which wrapped functions are executed.

+ Game Properties functions are read when Civ4 is launched.
+ User Defined Map Options functions are read when a user selects that script
  from the list of available scripts during the game setup phase. I persuaded
  Soren to allow map scripters to set up customizable user options. The result
  can be seen in the enhanced power of scripts like Highlands, Mirror, etc.
+ Everything else is called during the map generation process, upon launch.

* beforeInit()						# Can set up globals here to span variables across functions.
* getGridSize()						# Each grid cell is a block of 4x4 plots.
* getTopLatitude()					# Affects automatic bonus resource placement method.
* getBottomLatitude()				# Use the pair of getLatitude options to enjoy logical bonus appearance on maps that simulate a region rather than an entire planet.
* isBonusIgnoreLatitude()			# Can disable automatic restrictions on where bonus resources can appear.
* getWrapX() and getWrapY()			# MUST be called as a pair, or not at all.
* beforeGeneration()				# Similar to beforeInit() but can make use of some instructions that are unavailable until Grid Size, etc, has been processed.
* generateRandomMap()				# You can call this to DISABLE generatePlotTypes() and generateTerrain(), to whatever creative ends you may come up with.
* generatePlotTypes()				# First layer of data creation for the plot array, setting plot types (Ocean, Land, Hills, Peak). Plot types are hardcoded.
* generateTerrain()					# Second layer of data creation for the plot array.
* addRivers()						# Third layer of data creation for the plot array.
! getRiverStartCardinalDirection()	# Called once per river! (Only if using default river generation).
! getRiverAltitude()				# Called for EACH PLOT of a river, during river generation. (Only if using default river generation).
* addLakes()						# Fourth layer of data creation for the plot array.
* addFeatures()						# Fifth layer of data creation for the plot array.
* addBonuses()						# Sixth layer of data creation for the plot array.
! addBonusType()					# Called separately for each bonus entry found in the applicable XML file.
! canPlaceBonusAt()					# If overridden, called for each plot, for each bonus.
* addGoodies()						# Seventh (and final) layer of data creation for the plot array. NOTE: Goodies are "improvements", not "features".
! canPlaceGoodyAt()					# If overridden, called for each plot.
* afterGeneration()					# Last chance to fiddle with the map before the starting plots (for the civs) are determined.

+ All that remains are starting plot functions.

* minStartingDistanceModifier()		# Can increase or decrease (by a percentage) the standard values for minimum distance between any two start plots.
* assignStartingPlots()				# This is the only main start plot function that is called only once. If you use any process that results in determining
									# specific plots in which civs will start, this is where to handle it. Use setStartPlot() to specify a start plot. If you
									# use this function, the other "find" functions will not be called automatically, but you can call them manually if you need them.
! findStartingPlot()				# The version in CvMapGeneratorUtil is NOT the default! That version looks for the best local area only. The default
									# implementation first looks for the best continent, uninhabited portion of a continent, or best island, THEN also looks
									# for a rich local area in which to start this civ. ... The function is called once per civ.
! findStartingArea()				# Each land mass (even one plot island) has its own areaID. This function is part of the default process. You can override
									# this function and leave the others intact if you only want to specify which areaID(s) in which a given civ should appear.
									# The default start location functions are very powerful, as this is an area that got high priority and lots of personal
									# attention from Soren and I both. Pretty much the sky is the limit: if you can build a better mousetrap, have at it!
* normalizeStartingPlotLocations()	# This function only matters if any teams of more than one civ (aka permanent alliances) exist at game launch. This function
									# works to group team members together as best it can. If you hardwire any start plots for specific players and don't want
									# the game to mess with them, shuffing them around, then you need to override (disable) this function.


- Bob Thomas   September 23, 2005
"""

"""
GAME PROPERTIES
"""

def getDescription():
	"""
	A map's Description is displayed in the main menu when players go to begin a game.
	For no description return an empty string.
	"""
	return ""

def isAdvancedMap():
	"""
	Advanced maps only show up in the map script pulldown on the advanced menu.
	Return 0 if you want your map to show up in the simple singleplayer menu
	"""
	return 1

def getModPath():
	"""
	Returns the path of the mod this map is associated with
	"""
	return ""

"""
USER DEFINED MAP OPTIONS
"""

def isClimateMap():
	"""
	Uses the Climate options
	"""
	return 1

def isSeaLevelMap():
	"""
	Uses the Sea Level options
	"""
	return 1

def getNumCustomMapOptions():
	"""
	Number of different user-defined options for this map
	Return an integer
	"""
	return 0

def getNumHiddenCustomMapOptions():
	"""
	Number of different user-defined options for this map
	Return an integer
	"""
	return 0

def getCustomMapOptionName(argsList):
	"""
	Returns name of specified option
	argsList[0] is Option ID (int)
	Return a Unicode string
	"""
	return u""

def getNumCustomMapOptionValues(argsList):
	"""
	Number of different choices for a particular setting
	argsList[0] is Option ID (int)
	Return an integer
	"""
	return 0

def getCustomMapOptionDescAt(argsList):
	"""
	Returns name of value of option at specified row
	argsList[0] is Option ID (int)
	argsList[1] is Selection Value ID (int)
	Return a Unicode string
	"""
	return u""

def getCustomMapOptionDefault(argsList):
	"""
	Returns default value of specified option
	argsList[0] is Option ID (int)
	Return an integer
	"""
	return 0

def isRandomCustomMapOption(argsList):
	"""
	Returns a flag indicating whether a random option should be provided
	argsList[0] is Option ID (int)
	Return a bool
	"""
	return true

"""
MAP SIZE AND WRAPPING FUNCTIONS
"""

# note: you must override both getWrap...() functions or neither

def getWrapX():
	"Can be overridden to change whether the map wraps around in the X direction. Default is true"
	CyPythonMgr().allowDefaultImpl()

def getWrapY():
	"Can be overridden to change whether the map wraps around in the Y direction. Default is false"
	CyPythonMgr().allowDefaultImpl()

def getTopLatitude():
	"Can be overridden to change whether the the top latitude. Default is 90"
	CyPythonMgr().allowDefaultImpl()

def getBottomLatitude():
	"Can be overridden to change whether the the bottom latitude. Default is -90"
	CyPythonMgr().allowDefaultImpl()

def isBonusIgnoreLatitude():
	"Can be overridden to determine whether bonus generation ignores latitude. Default is false"
	CyPythonMgr().allowDefaultImpl()

def isEuropeStart(argsList):
	[europeInfo] = argsList
	CyPythonMgr().allowDefaultImpl()

def getEuropeWidthPercent(argsList):
	[europeInfo] = argsList
	CyPythonMgr().allowDefaultImpl()

def getEuropeMinLandDistance(argsList):
	[europeInfo] = argsList
	CyPythonMgr().allowDefaultImpl()

def getGridSize(argsList):
	"""Given an argument of [worldSize], where worldSize is of type WorldSizeTypes.
	Can be overridden to return a (width, height) tuple representing the number of terrain cells
	the world should have. (a terrain cell is a chunk of 4x4 plots)"""
	[worldSize] = argsList
	CyPythonMgr().allowDefaultImpl()
	#return (width, height)

"""
MAP GENERATION FUNCTIONS
"""

def generateRandomMap():
	"""Can be overridden if you want to manually control the sequence in which plot types, heights, and terrain are generated.

	WARNING: If you override this function, then none of the following functions will be automatically called for you:
		generateTerrain()
		generatePlotTypes()

	However, unless you have some specific reason to change the sequence of map generation, you should probably just override
	one or more of the functions listed above.
	"""
	CyPythonMgr().allowDefaultImpl()

def generateTerrain(argsList):
	"returns a list of terrain data for all pixels on the map, indexed with (0,0) in NW"
	CyPythonMgr().allowDefaultImpl()

def generatePlotTypes():
	"""Can override to return a list with a PlotTypes value for each plots on the map.
	Returned list should have size CyMapGenerator().numPlots()."""
	CyPythonMgr().allowDefaultImpl()

"""
GAME ELEMENT PLACEMENT FUNCTIONS
"""

def beforeInit():
	"Called before the map is initialized"

def beforeGeneration():
	"Called before the map generation starts"

def addRivers():
	"Should add all desired rivers to the map."
	CyPythonMgr().allowDefaultImpl()

def getRiverStartCardinalDirection(argsList):
	pPlot = argsList[0]
	"Returns the cardinal direction of the first river segment."
	CyPythonMgr().allowDefaultImpl()

def getRiverAltitude(argsList):
	pPlot = argsList[0]
	"Returns the altitude level of pPlot (rivers will flow to the lowest point)."
	CyPythonMgr().allowDefaultImpl()

def addLakes():
	"Can override to control where lakes are added on the map"
	CyPythonMgr().allowDefaultImpl()

def addFeatures():
	"Can override to control where features (i.e. forest, etc.) are added on the map"
	CyPythonMgr().allowDefaultImpl()

def addBonuses():
	"Can override to control where bonuses are added on the map"
	CyPythonMgr().allowDefaultImpl()

def addBonusType(argsList):
	"Can override to control where a type of bonus is added on the map"
	"""WARNING: If addBonuses() is overriden then this function will not automatically be called."""
	[iBonusType] = argsList
	CyPythonMgr().allowDefaultImpl()

def canPlaceBonusAt(argsList):
	"Can override to determine if a bonus can be placed at the specified plot"
	"""WARNING: If addBonuses() is overriden then this function will not automatically be called."""
	pPlot = argsList[0]
	CyPythonMgr().allowDefaultImpl()

def addGoodies():
	"Can override to control where goodies are added on the map"
	CyPythonMgr().allowDefaultImpl()

def canPlaceGoodyAt(argsList):
	pPlot = argsList[0]
	"Can override to determine if a goody can be placed at the specified plot"
	CyPythonMgr().allowDefaultImpl()

def afterGeneration():
	"Called after the map generation is finished"

"""
STARTING PLOT FUNCTIONS
"""

def minStartingDistanceModifier():
	"Returns a percent modification for the minimum distance between players (return 50 would mean +50%)"
	return 0

def assignStartingPlots():
	"Can assign as many starting locations as desired"
	CyPythonMgr().allowDefaultImpl()

def findStartingArea(argsList):
	"""Can override to return the ID of the area in which the given player should start. Return an areaID to
	force the player to start in a certain area, or -1 to specify that the player should start in the best
	location, irrespective of which area it is in. If not overridden, the default implementation will try
	to maximize the size of the player's part of his area

	WARNING: If findStartingPlot() is overriden then this function will not automatically be called."""
	playerID = argsList[0]
	CyPythonMgr().allowDefaultImpl()

def findStartingPlot(argsList):
	"Can override to return the plot index at which the given player should start"
	playerID = argsList[0]
	CyPythonMgr().allowDefaultImpl()

def normalizeStartingPlotLocations():
	"Can override to change how starting plots are distributed. (note, this function is called after all starting locs are distributed...)"
	CyPythonMgr().allowDefaultImpl()
