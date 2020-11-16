from CvPythonExtensions import *
import CvUtil
import CvMapGeneratorUtil
from CvMapGeneratorUtil import FractalWorld
from CvMapGeneratorUtil import TerrainGenerator
from CvMapGeneratorUtil import FeatureGenerator

def getDescription():
	return "TXT_KEY_MAP_SCRIPT_ANEWWORLD_DESCR"
	
def isAdvancedMap():
	"This map should show up in simple mode"
	return 0

def getWrapX():
	return false
	
def getWrapY():
	return false
	
def getGridSize(argsList):
	"Reduce grid sizes by one level."
	
	# set city catchment radius
	mmap = CyGlobalContext().getMap()
	mmap.setCityCatchmentRadiusMapMaker(mmap.getCustomMapOption(0))

def generatePlotTypes():
	NiTextOut("Setting Plot Types (Python Fractal) ...")
	fractal_world = FractalWorld( 4, 4 )
	fractal_world.initFractal(continent_grain = 3, rift_grain = 4, has_center_rift = False, polar = True)
	retVal = fractal_world.generatePlotTypes(70)
	sinkEasternShore(fractal_world, 5)
	return retVal

def generateTerrainTypes():
	NiTextOut("Generating Terrain (Python Fractal) ...")
	terraingen = TerrainGenerator(grain_amount=6)
	terrainTypes = terraingen.generateTerrain()
	return terrainTypes

def addFeatures():
	NiTextOut("Adding Features (Python Fractal) ...")
	featuregen = FeatureGenerator()
	featuregen.addFeatures()
	return 0
	
def sinkEasternShore(fractal_world, iWidth):
	for x in range(iWidth):
		for y in range(fractal_world.iNumPlotsY):
			i = y * fractal_world.iNumPlotsX + fractal_world.iNumPlotsX - 1 - x 
			fractal_world.plotTypes[i] = PlotTypes.PLOT_OCEAN
				
	return 0

def getNumCustomMapOptions():
	"""
	Number of different user-defined options for this map
	Return an integer
	"""
	return 1
	
def getCustomMapOptionName(argsList):
	"""
	Returns name of specified option
	argsList[0] is Option ID (int)
	Return a Unicode string
	"""
	optionID = argsList[0]
	if optionID == 0:
		 return CyTranslator().getText("TXT_KEY_MAP_CUSTOM_OPTION_CITY_CATCHMENT_RADIUS", ())
	return u""
	
def getNumCustomMapOptionValues(argsList):
	"""
	Number of different choices for a particular setting
	argsList[0] is Option ID (int)
	Return an integer
	"""
	optionID = argsList[0]
	if optionID == 0:
		return 2
	
	return 0
	
def getCustomMapOptionDescAt(argsList):
	"""
	Returns name of value of option at specified row
	argsList[0] is Option ID (int)
	argsList[1] is Selection Value ID (int)
	Return a Unicode string
	"""
	optionID = argsList[0]
	selectionID = argsList[1]
	if optionID == 0:
		if selectionID == 0:
			return CyTranslator().getText("TXT_KEY_MAP_CUSTOM_OPTION_CITY_CATCHMENT_RADIUS_1_PLOT", ())
		elif selectionID == 1:
			return CyTranslator().getText("TXT_KEY_MAP_CUSTOM_OPTION_CITY_CATCHMENT_RADIUS_2_PLOT", ())

	return u""

def getCustomMapOptionDefault(argsList):
	"""
	Returns default value of specified option
	argsList[0] is Option ID (int)
	Return an integer
	"""
	optionID = argsList[0]
	if optionID == 0:
		return CyGlobalContext().getDefaultCityCatchmentRadius()-1
	return 0

def isRandomCustomMapOption(argsList):
	"""
	Returns a flag indicating whether a random option should be provided
	argsList[0] is Option ID (int)
	Return a bool
	"""
	optionID = argsList[0]
	if optionID == 0:
		return False

	return False
