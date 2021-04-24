#
#	FILE:	 Caribbean.py
#	AUTHOR:  Liam Collins (155)
#	PURPOSE: w00t! Ocean front commerce!
#-----------------------------------------------------------------------------
#	Copyright (c) 2008 Firaxis Games, Inc. All rights reserved.
#-----------------------------------------------------------------------------
#

from CvPythonExtensions import *
import CvUtil
import CvMapGeneratorUtil
import random
import sys
from math import sqrt
from CvMapGeneratorUtil import FractalWorld
from CvMapGeneratorUtil import TerrainGenerator
from CvMapGeneratorUtil import FeatureGenerator

def getDescription():
	return "TXT_KEY_MAP_SCRIPT_CARIBBEAN_DESCR"

def isAdvancedMap():
	"This map should show up in simple mode"
	return 0
	
def getWrapX():
	return False
def getWrapY():
	return False


def getGridSize(argsList):
	"Reduce grid sizes by one level."
	
	# set city catchment radius
	mmap = CyGlobalContext().getMap()
	mmap.setCityCatchmentRadiusMapMaker(mmap.getCustomMapOption(0))
	
	grid_sizes = {
		WorldSizeTypes.WORLDSIZE_TINY:		(40,24),
		WorldSizeTypes.WORLDSIZE_SMALL:		(52,32),
		WorldSizeTypes.WORLDSIZE_STANDARD:	(64,40),
		WorldSizeTypes.WORLDSIZE_LARGE:		(84,48),
		WorldSizeTypes.WORLDSIZE_HUGE:		(104,64),
		WorldSizeTypes.WORLDSIZE_GIGANTIC:	(204,124)
	}

	if (argsList[0] == -1): # (-1,) is passed to function on loads
		return []
	[eWorldSize] = argsList
	return grid_sizes[eWorldSize]

def generatePlotTypes():
	NiTextOut("Setting Plot Types (Python Caribbean) ...")
	gc = CyGlobalContext()
	map = CyMap()
	dice = gc.getGame().getMapRand()
	iFlags = 0  # Disallow FRAC_POLAR flag, to prevent "zero row" problems.
	iW = map.getGridWidth()
	iH = map.getGridHeight()
	plotTypes = [PlotTypes.PLOT_LAND] * (iW*iH)

# Regional boundaries (LEFT IS SMALLER THAN RIGHT, BOTTOM SMALLER THAN TOP)

	FloridaLandYTop = 1.00
	FloridaLandYBottom = 0.88
	FloridaLandXLeft = 0.19
	FloridaLandXRight = 0.24
	
	FloridaWaterWestYTop = 0.89
	FloridaWaterWestYBottom = 0.87
	FloridaWaterWestXLeft = 0.19
	FloridaWaterWestXRight = 0.21
	
	FloridaWaterEastYTop = 0.89
	FloridaWaterEastYBottom = 0.87
	FloridaWaterEastXLeft = 0.22
	FloridaWaterEastXRight = 0.24

	OkeechobeeYTop = 0.94
	OkeechobeeYBottom = 0.92
	OkeechobeeXLeft = 0.22
	OkeechobeeXRight = 0.23
	
	BahamasNorthYTop = 0.98
	BahamasNorthYBottom = 0.78
	BahamasNorthXLeft = 0.26
	BahamasNorthXRight = 0.4
	
	BahamasSouthYTop = 0.8
	BahamasSouthYBottom = 0.69
	BahamasSouthXLeft = 0.38
	BahamasSouthXRight = 0.5

	BahamasWaterYTop = 0
	BahamasWaterYBottom = 0
	BahamasWaterXLeft = 0
	BahamasWaterXRight = 0

	CubaBaseYTop = 0.73
	CubaBaseYBottom = 0.63
	CubaBaseXLeft = 0.1
	CubaBaseXRight = 0.32

	CubaMidYTop = 0.75
	CubaMidYBottom = 0.73
	CubaMidXLeft = 0.1
	CubaMidXRight = 0.24
	
	CubaTipYTop = 0.79
	CubaTipYBottom = 0.75
	CubaTipXLeft = 0.08
	CubaTipXRight = 0.17

	CubaWaterEastBaseYTop = 0.74
	CubaWaterEastBaseYBottom = 0.68
	CubaWaterEastBaseXLeft = 0.3
	CubaWaterEastBaseXRight = 0.34

	CubaWaterBaseYTop = 0.69
	CubaWaterBaseYBottom = 0.63
	CubaWaterBaseXLeft = 0.1
	CubaWaterBaseXRight = 0.24

	CubaWaterMidYTop = 0.72
	CubaWaterMidYBottom = 0.69
	CubaWaterMidXLeft = 0.1
	CubaWaterMidXRight = 0.2	

	CubaWaterTipYTop = 0.76
	CubaWaterTipYBottom = 0.63
	CubaWaterTipXLeft = 0.09
	CubaWaterTipXRight = 0.15	

	CubaWaterBaseEndYTop = 0.72
	CubaWaterBaseEndYBottom = 0.7
	CubaWaterBaseEndXLeft = 0.28
	CubaWaterBaseEndXRight = 0.32	

	HispanolaYTop = 0.63
	HispanolaYBottom = 0.54
	HispanolaXLeft = 0.4
	HispanolaXRight = 0.55

	HispanolaWaterUpWestYTop = 0.63
	HispanolaWaterUpWestYBottom = 0.57
	HispanolaWaterUpWestXLeft = 0.4
	HispanolaWaterUpWestXRight = 0.42

	HispanolaWaterLowWestYTop = 0.6
	HispanolaWaterLowWestYBottom = 0.56
	HispanolaWaterLowWestXLeft = 0.4
	HispanolaWaterLowWestXRight = 0.45

	HispanolaWaterUpEastYTop = 0.63
	HispanolaWaterUpEastYBottom = 0.6
	HispanolaWaterUpEastXLeft = 0.51
	HispanolaWaterUpEastXRight = 0.55

	HispanolaWaterLowEastYTop = 0.6
	HispanolaWaterLowEastYBottom = 0.57
	HispanolaWaterLowEastXLeft = 0.52
	HispanolaWaterLowEastXRight = 0.55

	PuertoRicoYTop = 0.57
	PuertoRicoYBottom = 0.54
	PuertoRicoXLeft = 0.59
	PuertoRicoXRight = 0.65
	
	AntillesVerYTop = 0.58
	AntillesVerYBottom = 0.29   
	AntillesVerXLeft = 0.7   
	AntillesVerXRight = 0.8

	AntillesCurveVerYTop = 0.52
	AntillesCurveVerYBottom = 0.32   
	AntillesCurveVerXLeft = 0.68   
	AntillesCurveVerXRight = 0.75
	
	JamaicaYTop = 0.55
	JamaicaYBottom = 0.52
	JamaicaXLeft = 0.23
	JamaicaXRight = 0.27
	
	SouthAmeYTop = 0.17
	SouthAmeYBottom = 0.0
	SouthAmeXLeft = 0.35
	SouthAmeXRight = 0.92
	
	SouthAmePeninYTop = 0.29 
	SouthAmePeninYBottom = 0.17
	SouthAmePeninXLeft = 0.51
	SouthAmePeninXRight = 0.83
	
	SouthAmeTipYTop = 0.34 
	SouthAmeTipYBottom = 0.17
	SouthAmeTipXLeft = 0.4
	SouthAmeTipXRight = 0.49

	SouthAmeEasternmostYTop = 0.06 
	SouthAmeEasternmostYBottom = 0.0
	SouthAmeEasternmostXLeft = 0.91
	SouthAmeEasternmostXRight = 0.95

	SouthAmeRoundWestYTop = 0.17
	SouthAmeRoundWestYBottom = 0.13
	SouthAmeRoundWestXLeft = 0.35
	SouthAmeRoundWestXRight = 0.38

	SouthAmeRoundUpperEastYTop = 0.29
	SouthAmeRoundUpperEastYBottom = 0.22
	SouthAmeRoundUpperEastXLeft = 0.65
	SouthAmeRoundUpperEastXRight = 0.75
	
	SouthAmeRoundLowerEastYTop = 0.17
	SouthAmeRoundLowerEastYBottom = 0.11
	SouthAmeRoundLowerEastXLeft = 0.88
	SouthAmeRoundLowerEastXRight = 0.92

	SouthAmeCoastVarWestYTop = 0.34
	SouthAmeCoastVarWestYBottom = 0.26
	SouthAmeCoastVarWestXLeft = 0.4
	SouthAmeCoastVarWestXRight = 0.46

	SouthAmeCoastVarMidYTop = 0.23
	SouthAmeCoastVarMidYBottom = 0.16
	SouthAmeCoastVarMidXLeft = 0.82
	SouthAmeCoastVarMidXRight = 0.85
	
	SouthAmeCoastVarNEYTop = 0.29
	SouthAmeCoastVarNEYBottom = 0.23
	SouthAmeCoastVarNEXLeft = 0.78
	SouthAmeCoastVarNEXRight = 0.83
	
	SouthAmeCoastVarTopYTop = 0.25
	SouthAmeCoastVarTopYBottom = 0.2
	SouthAmeCoastVarTopXLeft = 0.63
	SouthAmeCoastVarTopXRight = 0.75

	SouthAmeCoastVarBayYTop = 0.23
	SouthAmeCoastVarBayYBottom = 0.19
	SouthAmeCoastVarBayXLeft = 0.47
	SouthAmeCoastVarBayXRight = 0.53
	
	SouthAmeCoastVarTopmostYTop = 0.3
	SouthAmeCoastVarTopmostYBottom = 0.25
	SouthAmeCoastVarTopmostXLeft = 0.57
	SouthAmeCoastVarTopmostXRight = 0.63

	PanamaBaseYTop = 0.08
	PanamaBaseYBottom =  0.05
	PanamaBaseXLeft = 0.15
	PanamaBaseXRight = 0.37
	
	PanamaMidYTop = 0.12
	PanamaMidYBottom =  0.08
	PanamaMidXLeft = 0.2
	PanamaMidXRight = 0.3

	PanamaTipYTop = 0.15
	PanamaTipYBottom =  0.12
	PanamaTipXLeft = 0.21
	PanamaTipXRight = 0.28

	PanamaEndYTop = 0.1
	PanamaEndYBottom =  0.07
	PanamaEndXLeft = 0.1
	PanamaEndXRight = 0.15

	PacificBaseYTop = 0.11
	PacificBaseYBottom = 0.05
	PacificBaseXLeft = 0.23
	PacificBaseXRight = 0.26
	
	CentralAmericaYTop = 0.45
	CentralAmericaYBottom = 0.07
	CentralAmericaXLeft = 0.0
	CentralAmericaXRight = 0.1
	
	CentralAmericaWaterTopYTop = 0.31
	CentralAmericaWaterTopYBottom = 0.21
	CentralAmericaWaterTopXLeft = 0.0
	CentralAmericaWaterTopXRight = 0.03
	
	CentralAmericaWaterMidYTop = 0.21
	CentralAmericaWaterMidYBottom = 0.15
	CentralAmericaWaterMidXLeft = 0.0
	CentralAmericaWaterMidXRight = 0.06
	
	CentralAmericaWaterLowYTop = 0.15
	CentralAmericaWaterLowYBottom = 0.08
	CentralAmericaWaterLowXLeft = 0.0
	CentralAmericaWaterLowXRight = 0.08
	
	CentralAmericaWaterBaseYTop = 0.08
	CentralAmericaWaterBaseYBottom = 0.06
	CentralAmericaWaterBaseXLeft = 0.0
	CentralAmericaWaterBaseXRight = 0.11
	
	for x in range(iW):
		for y in range(iH):
			i = y*iW + x

# Laying down base land masses
			if (x <= (iW*CubaBaseXRight) and x >= (iW*CubaBaseXLeft)) and (y >= (iH*CubaBaseYBottom) and y <= (CubaBaseYTop*iH)):
				iTempInt = dice.get(30, "Making Cuba")
				iTempIntB = dice.get(10, "Making Cuba")				
				if iTempInt > 5:
					if iTempIntB > 8:
						plotTypes[i] = PlotTypes.PLOT_PEAK
					elif iTempIntB > 6:
						plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						plotTypes[i] = PlotTypes.PLOT_LAND
					print "Making Cuba Land"
				else:
					plotTypes[i] = PlotTypes.PLOT_OCEAN
					print "Making Cuba Ocean"

			elif (x <= (iW*CubaMidXRight) and x >= (iW*CubaMidXLeft)) and (y >= (iH*CubaMidYBottom) and y <= (CubaMidYTop*iH)):
				iTempInt = dice.get(100, "Making Cuba")				
				iTempIntB = dice.get(10, "Making Cuba")				
				if iTempInt > 1:
					if iTempIntB > 8:
						plotTypes[i] = PlotTypes.PLOT_PEAK
					elif iTempIntB > 6:
						plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						plotTypes[i] = PlotTypes.PLOT_LAND
					print "Making Cuba Land"
				else:
					plotTypes[i] = PlotTypes.PLOT_OCEAN
					print "Making Cuba Ocean"

			elif (x <= (iW*CubaTipXRight) and x >= (iW*CubaTipXLeft)) and (y >= (iH*CubaTipYBottom) and y <= (CubaTipYTop*iH)):
				iTempInt = dice.get(100, "Making Cuba")				
				iTempIntB = dice.get(10, "Making Cuba")				
				if iTempInt > 1:
					if iTempIntB > 8:
						plotTypes[i] = PlotTypes.PLOT_PEAK
					elif iTempIntB > 6:
						plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						plotTypes[i] = PlotTypes.PLOT_LAND
					print "Making Cuba Land"
				else:
					plotTypes[i] = PlotTypes.PLOT_OCEAN
					print "Making Cuba Ocean"

			elif (x <= (iW*HispanolaXRight) and x >= (iW*HispanolaXLeft)) and (y >= (iH*HispanolaYBottom) and y <= (HispanolaYTop*iH)):
				iTempInt = dice.get(100, "Making Hispanola")				
				iTempIntB = dice.get(10, "Making Cuba")				
				if iTempInt > 1:
					if iTempIntB > 8:
						plotTypes[i] = PlotTypes.PLOT_PEAK
					elif iTempIntB > 6:
						plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						plotTypes[i] = PlotTypes.PLOT_LAND
					print "Making Cuba Land"
				else:
					plotTypes[i] = PlotTypes.PLOT_OCEAN
					print "Making Cuba Ocean"

			elif (x <= (iW*HispanolaXRight) and x >= (iW*HispanolaXLeft)) and (y >= (iH*HispanolaYBottom) and y <= (HispanolaYTop*iH)):
				iTempInt = dice.get(100, "Making Hispanola")				
				iTempIntB = dice.get(10, "Making Cuba")				
				if iTempInt > 1:
					if iTempIntB > 8:
						plotTypes[i] = PlotTypes.PLOT_PEAK
					elif iTempIntB > 6:
						plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						plotTypes[i] = PlotTypes.PLOT_LAND
					print "Making Cuba Land"
				else:
					plotTypes[i] = PlotTypes.PLOT_OCEAN
					print "Making Cuba Ocean"
			
			elif (x <= (iW*PuertoRicoXRight) and x >= (iW*PuertoRicoXLeft)) and (y >= (iH*PuertoRicoYBottom) and y <= (PuertoRicoYTop*iH)):
				iTempInt = dice.get(100, "Making Puerto Rico")				
				iTempIntB = dice.get(10, "Making Cuba")				
				if iTempInt > 1:
					if iTempIntB > 8:
						plotTypes[i] = PlotTypes.PLOT_PEAK
					elif iTempIntB > 6:
						plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						plotTypes[i] = PlotTypes.PLOT_LAND
					print "Making Cuba Land"
				else:
					plotTypes[i] = PlotTypes.PLOT_OCEAN
					print "Making Cuba Ocean"

			elif (x <= (iW*AntillesVerXRight) and x >= (iW*AntillesVerXLeft)) and (y >= (iH*AntillesVerYBottom) and y <= (AntillesVerYTop*iH)):
				iTempInt = dice.get(20, "Making Antilles")				
				iTempIntB = dice.get(10, "Making Cuba")				
				if iTempInt > 15:
					if iTempIntB > 8:
						plotTypes[i] = PlotTypes.PLOT_PEAK
					elif iTempIntB > 6:
						plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						plotTypes[i] = PlotTypes.PLOT_LAND
					print "Making Cuba Land"
				else:
					plotTypes[i] = PlotTypes.PLOT_OCEAN
					print "Making Cuba Ocean"

			elif (x <= (iW*JamaicaXRight) and x >= (iW*JamaicaXLeft)) and (y >= (iH*JamaicaYBottom) and y <= (JamaicaYTop*iH)):
				iTempInt = dice.get(100, "Making Jamaica")				
				iTempIntB = dice.get(10, "Making Cuba")				
				if iTempInt > 1:
					if iTempIntB > 8:
						plotTypes[i] = PlotTypes.PLOT_PEAK
					elif iTempIntB > 6:
						plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						plotTypes[i] = PlotTypes.PLOT_LAND
					print "Making Cuba Land"
				else:
					plotTypes[i] = PlotTypes.PLOT_OCEAN
					print "Making Cuba Ocean"

			elif (x <= (iW*SouthAmeXRight) and x >= (iW*SouthAmeXLeft)) and (y >= SouthAmeYBottom and y <= (SouthAmeYTop*iH)):
				iTempInt = dice.get(50, "Making South America")				
				iTempIntB = dice.get(10, "Making Cuba")				
				if iTempInt > 1:
					if iTempIntB > 8:
						plotTypes[i] = PlotTypes.PLOT_PEAK
					elif iTempIntB > 6:
						plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						plotTypes[i] = PlotTypes.PLOT_LAND
					print "Making Cuba Land"
				else:
					plotTypes[i] = PlotTypes.PLOT_OCEAN
					print "Making Cuba Ocean"
					
			elif (x <= (iW*SouthAmePeninXRight) and x >= (iW*SouthAmePeninXLeft)) and (y >= (SouthAmePeninYBottom*iH) and y <= (SouthAmePeninYTop*iH)):
				iTempInt = dice.get(50, "Making South America")				
				iTempIntB = dice.get(10, "Making Cuba")				
				if iTempInt > 1:
					if iTempIntB > 8:
						plotTypes[i] = PlotTypes.PLOT_PEAK
					elif iTempIntB > 6:
						plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						plotTypes[i] = PlotTypes.PLOT_LAND
				else:
					plotTypes[i] = PlotTypes.PLOT_OCEAN
					
			elif (x <= (iW*SouthAmeTipXRight) and x >= (iW*SouthAmeTipXLeft)) and (y >= SouthAmeTipYBottom and y <= (SouthAmeTipYTop*iH)):
				iTempInt = dice.get(20, "Making South America")				
				iTempIntB = dice.get(10, "Making Cuba")				
				if iTempInt > 1:
					if iTempIntB > 8:
						plotTypes[i] = PlotTypes.PLOT_PEAK
					elif iTempIntB > 6:
						plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						plotTypes[i] = PlotTypes.PLOT_LAND
				else:
					plotTypes[i] = PlotTypes.PLOT_OCEAN

			elif (x <= (iW*SouthAmeEasternmostXRight) and x >= (iW*SouthAmeEasternmostXLeft)) and (y >= (SouthAmeEasternmostYBottom*iH) and y <= (SouthAmeEasternmostYTop*iH)):
				iTempInt = dice.get(20, "Making South America")				
				iTempIntB = dice.get(10, "Making Cuba")				
				if iTempInt > 1:
					if iTempIntB > 8:
						plotTypes[i] = PlotTypes.PLOT_PEAK
					elif iTempIntB > 6:
						plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						plotTypes[i] = PlotTypes.PLOT_LAND
				else:
					plotTypes[i] = PlotTypes.PLOT_OCEAN

			elif (x <= (iW*PanamaBaseXRight) and x >= (iW*PanamaBaseXLeft)) and (y >= (iH*PanamaBaseYBottom) and y <= (PanamaBaseYTop*iH)):
				iTempInt = dice.get(20, "Making Panama")				
				iTempIntB = dice.get(10, "Making Cuba")				
				if iTempInt > 1:
					if iTempIntB > 8:
						plotTypes[i] = PlotTypes.PLOT_PEAK
					elif iTempIntB > 6:
						plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						plotTypes[i] = PlotTypes.PLOT_LAND
				else:
					plotTypes[i] = PlotTypes.PLOT_OCEAN

			elif (x <= (iW*PanamaMidXRight) and x >= (iW*PanamaMidXLeft)) and (y >= (iH*PanamaMidYBottom) and y <= (PanamaMidYTop*iH)):
				iTempInt = dice.get(20, "Making Panama")				
				iTempIntB = dice.get(10, "Making Cuba")				
				if iTempInt > 1:
					if iTempIntB > 8:
						plotTypes[i] = PlotTypes.PLOT_PEAK
					elif iTempIntB > 6:
						plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						plotTypes[i] = PlotTypes.PLOT_LAND
				else:
					plotTypes[i] = PlotTypes.PLOT_OCEAN

			elif (x <= (iW*CentralAmericaXRight) and x >= (iW*CentralAmericaXLeft)) and (y >= (iH*CentralAmericaYBottom) and y <= (CentralAmericaYTop*iH)):
				iTempInt = dice.get(20, "Making Panama")				
				iTempIntB = dice.get(10, "Making Cuba")				
				if iTempInt > 1:
					if iTempIntB > 8:
						plotTypes[i] = PlotTypes.PLOT_PEAK
					elif iTempIntB > 6:
						plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						plotTypes[i] = PlotTypes.PLOT_LAND
				else:
					plotTypes[i] = PlotTypes.PLOT_OCEAN

			elif (x <= (iW*PanamaEndXRight) and x >= (iW*PanamaEndXLeft)) and (y >= (iH*PanamaEndYBottom) and y <= (PanamaEndYTop*iH)):
				iTempInt = dice.get(20, "Making Panama")				
				iTempIntB = dice.get(10, "Making Cuba")				
				if iTempInt > 1:
					if iTempIntB > 8:
						plotTypes[i] = PlotTypes.PLOT_PEAK
					elif iTempIntB > 6:
						plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						plotTypes[i] = PlotTypes.PLOT_LAND
				else:
					plotTypes[i] = PlotTypes.PLOT_OCEAN

			elif (x <= (iW*PanamaTipXRight) and x >= (iW*PanamaTipXLeft)) and (y >= (iH*PanamaTipYBottom) and y <= (PanamaTipYTop*iH)):
				iTempInt = dice.get(20, "Making Panama")				
				iTempIntB = dice.get(10, "Making Cuba")				
				if iTempInt > 1:
					if iTempIntB > 8:
						plotTypes[i] = PlotTypes.PLOT_PEAK
					elif iTempIntB > 6:
						plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						plotTypes[i] = PlotTypes.PLOT_LAND
				else:
					plotTypes[i] = PlotTypes.PLOT_OCEAN

			elif (x <= (iW*FloridaLandXRight) and x >= (iW*FloridaLandXLeft)) and (y >= (iH*FloridaLandYBottom) and y <= (FloridaLandYTop*iH)):
				iTempInt = dice.get(20, "Making Panama")				
				iTempIntB = dice.get(10, "Making Cuba")				
				if iTempInt > 1:
					if iTempIntB > 8:
						plotTypes[i] = PlotTypes.PLOT_PEAK
					elif iTempIntB > 6:
						plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						plotTypes[i] = PlotTypes.PLOT_LAND
				else:
					plotTypes[i] = PlotTypes.PLOT_OCEAN

			elif (x <= (iW*BahamasNorthXRight) and x >= (iW*BahamasNorthXLeft)) and (y >= (iH*BahamasNorthYBottom) and y <= (BahamasNorthYTop*iH)):
				iTempInt = dice.get(20, "Making Panama")				
				iTempIntB = dice.get(10, "Making Cuba")				
				if iTempInt > 17:
					if iTempIntB > 8:
						plotTypes[i] = PlotTypes.PLOT_PEAK
					elif iTempIntB > 6:
						plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						plotTypes[i] = PlotTypes.PLOT_LAND
				else:
					plotTypes[i] = PlotTypes.PLOT_OCEAN
					
			elif (x <= (iW*BahamasSouthXRight) and x >= (iW*BahamasSouthXLeft)) and (y >= (iH*BahamasSouthYBottom) and y <= (BahamasSouthYTop*iH)):
				iTempInt = dice.get(20, "Making Panama")				
				iTempIntB = dice.get(10, "Making Cuba")				
				if iTempInt > 17:
					if iTempIntB > 8:
						plotTypes[i] = PlotTypes.PLOT_PEAK
					elif iTempIntB > 6:
						plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						plotTypes[i] = PlotTypes.PLOT_LAND
				else:
					plotTypes[i] = PlotTypes.PLOT_OCEAN

			else:		
				plotTypes[i] = PlotTypes.PLOT_OCEAN

# Carving aquatic curves into landscape
			if (x <= (iW*PacificBaseXRight) and x >= (iW*PacificBaseXLeft)) and (y >= (iH*PacificBaseYBottom) and y <= (PacificBaseYTop*iH)):
				iTempInt = dice.get(30, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND
							
			elif (x <= (iW*SouthAmeRoundWestXRight) and x >= (iW*SouthAmeRoundWestXLeft)) and (y >= (iH*SouthAmeRoundWestYBottom) and y <= (SouthAmeRoundWestYTop*iH)):
				iTempInt = dice.get(30, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND
			
			elif (x <= (iW*SouthAmeRoundUpperEastXRight) and x >= (iW*SouthAmeRoundUpperEastXLeft)) and (y >= (iH*SouthAmeRoundUpperEastYBottom) and y <= (SouthAmeRoundUpperEastYTop*iH)):
				iTempInt = dice.get(30, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND

			elif (x <= (iW*SouthAmeRoundLowerEastXRight) and x >= (iW*SouthAmeRoundLowerEastXLeft)) and (y >= (iH*SouthAmeRoundLowerEastYBottom) and y <= (SouthAmeRoundLowerEastYTop*iH)):
				iTempInt = dice.get(30, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND

			elif (x <= (iW*SouthAmeCoastVarWestXRight) and x >= (iW*SouthAmeCoastVarWestXLeft)) and (y >= (iH*SouthAmeCoastVarWestYBottom) and y <= (SouthAmeCoastVarWestYTop*iH)):
				iTempInt = dice.get(15, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND

			elif (x <= (iW*SouthAmeCoastVarMidXRight) and x >= (iW*SouthAmeCoastVarMidXLeft)) and (y >= (iH*SouthAmeCoastVarMidYBottom) and y <= (SouthAmeCoastVarMidYTop*iH)):
				iTempInt = dice.get(15, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND

			elif (x <= (iW*SouthAmeCoastVarNEXRight) and x >= (iW*SouthAmeCoastVarNEXLeft)) and (y >= (iH*SouthAmeCoastVarNEYBottom) and y <= (SouthAmeCoastVarNEYTop*iH)):
				iTempInt = dice.get(15, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND

			elif (x <= (iW*SouthAmeCoastVarTopXRight) and x >= (iW*SouthAmeCoastVarTopXLeft)) and (y >= (iH*SouthAmeCoastVarTopYBottom) and y <= (SouthAmeCoastVarTopYTop*iH)):
				iTempInt = dice.get(10, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND

			elif (x <= (iW*SouthAmeCoastVarBayXRight) and x >= (iW*SouthAmeCoastVarBayXLeft)) and (y >= (iH*SouthAmeCoastVarBayYBottom) and y <= (SouthAmeCoastVarBayYTop*iH)):
				iTempInt = dice.get(30, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND

			elif (x <= (iW*SouthAmeCoastVarTopmostXRight) and x >= (iW*SouthAmeCoastVarTopmostXLeft)) and (y >= (iH*SouthAmeCoastVarTopmostYBottom) and y <= (SouthAmeCoastVarTopmostYTop*iH)):
				iTempInt = dice.get(10, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND

			elif (x <= (iW*CubaWaterEastBaseXRight) and x >= (iW*CubaWaterEastBaseXLeft)) and (y >= (iH*CubaWaterEastBaseYBottom) and y <= (CubaWaterEastBaseYTop*iH)):
				iTempInt = dice.get(30, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND

			elif (x <= (iW*CubaWaterBaseXRight) and x >= (iW*CubaWaterBaseXLeft)) and (y >= (iH*CubaWaterBaseYBottom) and y <= (CubaWaterBaseYTop*iH)):
				iTempInt = dice.get(30, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND
					
			elif (x <= (iW*CubaWaterMidXRight) and x >= (iW*CubaWaterMidXLeft)) and (y >= (iH*CubaWaterMidYBottom) and y <= (CubaWaterMidYTop*iH)):
				iTempInt = dice.get(30, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND
					
			elif (x <= (iW*CubaWaterTipXRight) and x >= (iW*CubaWaterTipXLeft)) and (y >= (iH*CubaWaterTipYBottom) and y <= (CubaWaterTipYTop*iH)):
				iTempInt = dice.get(60, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND

			elif (x <= (iW*CubaWaterBaseEndXRight) and x >= (iW*CubaWaterBaseEndXLeft)) and (y >= (iH*CubaWaterBaseEndYBottom) and y <= (CubaWaterBaseEndYTop*iH)):
				iTempInt = dice.get(30, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND

			elif (x <= (iW*HispanolaWaterUpWestXRight) and x >= (iW*HispanolaWaterUpWestXLeft)) and (y >= (iH*HispanolaWaterUpWestYBottom) and y <= (HispanolaWaterUpWestYTop*iH)):
				iTempInt = dice.get(30, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND

			elif (x <= (iW*HispanolaWaterLowWestXRight) and x >= (iW*HispanolaWaterLowWestXLeft)) and (y >= (iH*HispanolaWaterLowWestYBottom) and y <= (HispanolaWaterLowWestYTop*iH)):
				iTempInt = dice.get(30, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND

			elif (x <= (iW*HispanolaWaterUpEastXRight) and x >= (iW*HispanolaWaterUpEastXLeft)) and (y >= (iH*HispanolaWaterUpEastYBottom) and y <= (HispanolaWaterUpEastYTop*iH)):
				iTempInt = dice.get(30, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND

			elif (x <= (iW*HispanolaWaterLowEastXRight) and x >= (iW*HispanolaWaterLowEastXLeft)) and (y >= (iH*HispanolaWaterLowEastYBottom) and y <= (HispanolaWaterLowEastYTop*iH)):
				iTempInt = dice.get(30, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND

			elif (x <= (iW*AntillesCurveVerXRight) and x >= (iW*AntillesCurveVerXLeft)) and (y >= (iH*AntillesCurveVerYBottom) and y <= (AntillesCurveVerYTop*iH)):
				iTempInt = dice.get(1000, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND

			elif (x <= (iW*CentralAmericaWaterTopXRight) and x >= (iW*CentralAmericaWaterTopXLeft)) and (y >= (iH*CentralAmericaWaterTopYBottom) and y <= (CentralAmericaWaterTopYTop*iH)):
				iTempInt = dice.get(30, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND

			elif (x <= (iW*CentralAmericaWaterMidXRight) and x >= (iW*CentralAmericaWaterMidXLeft)) and (y >= (iH*CentralAmericaWaterMidYBottom) and y <= (CentralAmericaWaterMidYTop*iH)):
				iTempInt = dice.get(30, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND

			elif (x <= (iW*CentralAmericaWaterLowXRight) and x >= (iW*CentralAmericaWaterLowXLeft)) and (y >= (iH*CentralAmericaWaterLowYBottom) and y <= (CentralAmericaWaterLowYTop*iH)):
				iTempInt = dice.get(30, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND
		
			elif (x <= (iW*CentralAmericaWaterBaseXRight) and x >= (iW*CentralAmericaWaterBaseXLeft)) and (y >= (iH*CentralAmericaWaterBaseYBottom) and y <= (CentralAmericaWaterBaseYTop*iH)):
				iTempInt = dice.get(30, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND

			elif (x <= (iW*FloridaWaterWestXRight) and x >= (iW*FloridaWaterWestXLeft)) and (y >= (iH*FloridaWaterWestYBottom) and y <= (FloridaWaterWestYTop*iH)):
				iTempInt = dice.get(30, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND
					
			elif (x <= (iW*FloridaWaterEastXRight) and x >= (iW*FloridaWaterEastXLeft)) and (y >= (iH*FloridaWaterEastYBottom) and y <= (FloridaWaterEastYTop*iH)):
				iTempInt = dice.get(30, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND
					
			elif (x <= (iW*OkeechobeeXRight) and x >= (iW*OkeechobeeXLeft)) and (y >= (iH*OkeechobeeYBottom) and y <= (OkeechobeeYTop*iH)):
				iTempInt = dice.get(30, "Making Panama")				
				if (iTempInt > 1):
					plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					plotTypes[i] = PlotTypes.PLOT_LAND

			if x > (iW-5):
				plotTypes[i] = PlotTypes.PLOT_OCEAN
	
# Ensures that there are no useless single tile islands
	for x in range(iW-2):
		for y in range(iH-2):
			i = (y+1)*iW + x+1
			if ((i+iW)+1) < (((iH*iW)+iW)-1) and ((i-iW)-1) > 0:
				if plotTypes[i] == PlotTypes.PLOT_LAND or plotTypes[i] == PlotTypes.PLOT_HILLS or plotTypes[i] == PlotTypes.PLOT_PEAK:
					if plotTypes[i+1] == PlotTypes.PLOT_OCEAN:
						if plotTypes[i-1] == PlotTypes.PLOT_OCEAN:
							if plotTypes[((i+iW)-1)] == PlotTypes.PLOT_OCEAN:
								if plotTypes[((i+iW)+1)] == PlotTypes.PLOT_OCEAN:
									if plotTypes[((i-iW)-1)] == PlotTypes.PLOT_OCEAN:
										if plotTypes[((i-iW)+1)] == PlotTypes.PLOT_OCEAN:
											if plotTypes[((i-iW))] == PlotTypes.PLOT_OCEAN:
												if plotTypes[((i+iW))] == PlotTypes.PLOT_OCEAN:												
													iTempC = dice.get(8, "No single islands")
													iTempD = iTempC
													iTempC = dice.get(8, "No single islands")

													if iTempC == iTempD:
														iTempC = dice.get(8, "No single islands")
														if iTempC == iTempD:
															iTempC = dice.get(8, "No single islands")
															if iTempC == iTempD:
																plotTypes[i] = PlotTypes.PLOT_OCEAN
															
													if iTempC == 0:
														iTempE = dice.get(10, "No single islands")
														if iTempE > 8:
															plotTypes[i+1] = PlotTypes.PLOT_PEAK
														elif iTempE > 6:
															plotTypes[i+1] = PlotTypes.PLOT_HILLS
														else:
															plotTypes[i+1] = PlotTypes.PLOT_LAND
													if iTempC == 1:
														iTempE = dice.get(10, "No single islands")
														if iTempE > 8:
															plotTypes[i-1] = PlotTypes.PLOT_PEAK
														elif iTempE > 6:
															plotTypes[i-1] = PlotTypes.PLOT_HILLS
														else:
															plotTypes[i-1] = PlotTypes.PLOT_LAND
													if iTempC == 2:
														iTempE = dice.get(10, "No single islands")
														if iTempE > 8:
															plotTypes[(i-iW)] = PlotTypes.PLOT_PEAK
														elif iTempE > 6:
															plotTypes[(i-iW)] = PlotTypes.PLOT_HILLS
														else:
															plotTypes[(i-iW)] = PlotTypes.PLOT_LAND
													if iTempC == 3:
														iTempE = dice.get(10, "No single islands")
														if iTempE > 8:
															plotTypes[(i+iW)] = PlotTypes.PLOT_PEAK
														elif iTempE > 6:
															plotTypes[(i+iW)] = PlotTypes.PLOT_HILLS
														else:
															plotTypes[(i+iW)] = PlotTypes.PLOT_LAND
													if iTempC == 4:
														iTempE = dice.get(10, "No single islands")
														if iTempE > 8:
															plotTypes[((i+iW)+1)] = PlotTypes.PLOT_PEAK
														elif iTempE > 6:
															plotTypes[((i+iW)+1)] = PlotTypes.PLOT_HILLS
														else:
															plotTypes[((i+iW)+1)] = PlotTypes.PLOT_LAND
													if iTempC == 5:
														iTempE = dice.get(10, "No single islands")
														if iTempE > 8:
															plotTypes[((i-iW)+1)] = PlotTypes.PLOT_PEAK
														elif iTempE > 6:
															plotTypes[((i-iW)+1)] = PlotTypes.PLOT_HILLS
														else:
															plotTypes[((i-iW)+1)] = PlotTypes.PLOT_LAND
													if iTempC == 6:
														iTempE = dice.get(10, "No single islands")
														if iTempE > 8:
															plotTypes[((i+iW)-1)] = PlotTypes.PLOT_PEAK
														elif iTempE > 6:
															plotTypes[((i+iW)-1)] = PlotTypes.PLOT_HILLS
														else:
															plotTypes[((i+iW)-1)] = PlotTypes.PLOT_LAND
													if iTempC == 7:
														iTempE = dice.get(10, "No single islands")
														if iTempE > 8:
															plotTypes[((i-iW)-1)] = PlotTypes.PLOT_PEAK
														elif iTempE > 6:
															plotTypes[((i-iW)-1)] = PlotTypes.PLOT_HILLS
														else:
															plotTypes[((i-iW)-1)] = PlotTypes.PLOT_LAND
														
													if iTempD == 0:
														iTempE = dice.get(10, "No single islands")
														if iTempE > 8:
															plotTypes[i+1] = PlotTypes.PLOT_PEAK
														elif iTempE > 6:
															plotTypes[i+1] = PlotTypes.PLOT_HILLS
														else:
															plotTypes[i+1] = PlotTypes.PLOT_LAND
													if iTempD == 1:
														iTempE = dice.get(10, "No single islands")
														if iTempE > 8:
															plotTypes[i-1] = PlotTypes.PLOT_PEAK
														elif iTempE > 6:
															plotTypes[i-1] = PlotTypes.PLOT_HILLS
														else:
															plotTypes[i-1] = PlotTypes.PLOT_LAND
													if iTempD == 2:
														iTempE = dice.get(10, "No single islands")
														if iTempE > 8:
															plotTypes[(i-iW)] = PlotTypes.PLOT_PEAK
														elif iTempE > 6:
															plotTypes[(i-iW)] = PlotTypes.PLOT_HILLS
														else:
															plotTypes[(i-iW)] = PlotTypes.PLOT_LAND
													if iTempD == 3:
														iTempE = dice.get(10, "No single islands")
														if iTempE > 8:
															plotTypes[(i+iW)] = PlotTypes.PLOT_PEAK
														elif iTempE > 6:
															plotTypes[(i+iW)] = PlotTypes.PLOT_HILLS
														else:
															plotTypes[(i+iW)] = PlotTypes.PLOT_LAND
													if iTempD == 4:
														iTempE = dice.get(10, "No single islands")
														if iTempE > 8:
															plotTypes[((i+iW)+1)] = PlotTypes.PLOT_PEAK
														elif iTempE > 6:
															plotTypes[((i+iW)+1)] = PlotTypes.PLOT_HILLS
														else:
															plotTypes[((i+iW)+1)] = PlotTypes.PLOT_LAND
													if iTempD == 5:
														iTempE = dice.get(10, "No single islands")
														if iTempE > 8:
															plotTypes[((i-iW)+1)] = PlotTypes.PLOT_PEAK
														elif iTempE > 6:
															plotTypes[((i-iW)+1)] = PlotTypes.PLOT_HILLS
														else:
															plotTypes[((i-iW)+1)] = PlotTypes.PLOT_LAND
													if iTempD == 6:
														iTempE = dice.get(10, "No single islands")
														if iTempE > 8:
															plotTypes[((i+iW)-1)] = PlotTypes.PLOT_PEAK
														elif iTempE > 6:
															plotTypes[((i+iW)-1)] = PlotTypes.PLOT_HILLS
														else:
															plotTypes[((i+iW)-1)] = PlotTypes.PLOT_LAND
													if iTempD == 7:
														iTempE = dice.get(10, "No single islands")
														if iTempE > 8:
															plotTypes[((i-iW)-1)] = PlotTypes.PLOT_PEAK
														elif iTempE > 6:
															plotTypes[((i-iW)-1)] = PlotTypes.PLOT_HILLS
														else:
															plotTypes[((i-iW)-1)] = PlotTypes.PLOT_LAND


	return plotTypes
	
class CaribbeanTerrainGenerator(CvMapGeneratorUtil.TerrainGenerator):
	"If iDesertPercent=35, then about 35% of all land will be desert. Plains is similar. \
	Note that all percentages are approximate, as values have to be roughened to achieve a natural look."
	
	def __init__(self, iDesertPercent=5, iPlainsPercent=20, iMarshPercent=70,
	             fSnowLatitude=1.3, fTundraLatitude=1.0,
	             #R&R
	             #Androrc Savannah
#	             fGrassLatitude=0.1, fDesertBottomLatitude=0.2,
	             fGrassLatitude=0.1, fDesertBottomLatitude=0.2, fSavannahLatitude=1.0,
	             #Androrc End
	             #R&R End
	             fDesertTopLatitude=0.5, fracXExp=-1,
	             fracYExp=-1, grain_amount=4):
		
		self.gc = CyGlobalContext()
		self.map = CyMap()

		grain_amount += self.gc.getWorldInfo(self.map.getWorldSize()).getTerrainGrainChange()
		
		self.grain_amount = grain_amount

		self.iWidth = self.map.getGridWidth()
		self.iHeight = self.map.getGridHeight()

		self.mapRand = self.gc.getGame().getMapRand()
		
		self.iFlags = 0  # Disallow FRAC_POLAR flag, to prevent "zero row" problems.
		if self.map.isWrapX(): self.iFlags += CyFractal.FracVals.FRAC_WRAP_X
		if self.map.isWrapY(): self.iFlags += CyFractal.FracVals.FRAC_WRAP_Y

		self.deserts=CyFractal()
		self.plains=CyFractal()
		self.marsh=CyFractal()
		self.variation=CyFractal()

		iDesertPercent += self.gc.getClimateInfo(self.map.getClimate()).getDesertPercentChange()
		iDesertPercent = min(iDesertPercent, 100)
		iDesertPercent = max(iDesertPercent, 0)

		self.iDesertPercent = iDesertPercent
		self.iPlainsPercent = iPlainsPercent
		self.iMarshPercent = iMarshPercent

		self.iDesertTopPercent = 100
		self.iDesertBottomPercent = max(0,int(100-iDesertPercent))
		self.iMarshTopPercent = 100
		self.iMarshBottomPercent = max(0,int(100-iDesertPercent-iMarshPercent))
		self.iPlainsTopPercent = 100
		self.iPlainsBottomPercent = max(0,int(100-iDesertPercent-iMarshPercent-iPlainsPercent))
		
		self.iMountainTopPercent = 75
		self.iMountainBottomPercent = 60

		fSnowLatitude += self.gc.getClimateInfo(self.map.getClimate()).getSnowLatitudeChange()
		fSnowLatitude = min(fSnowLatitude, 1.0)
		fSnowLatitude = max(fSnowLatitude, 0.0)
		self.fSnowLatitude = fSnowLatitude

		fTundraLatitude += self.gc.getClimateInfo(self.map.getClimate()).getTundraLatitudeChange()
		fTundraLatitude = min(fTundraLatitude, 1.0)
		fTundraLatitude = max(fTundraLatitude, 0.0)
		self.fTundraLatitude = fTundraLatitude

		fGrassLatitude += self.gc.getClimateInfo(self.map.getClimate()).getGrassLatitudeChange()
		fGrassLatitude = min(fGrassLatitude, 1.0)
		fGrassLatitude = max(fGrassLatitude, 0.0)
		self.fGrassLatitude = fGrassLatitude

		#R&R
		#Androrc Savannah
		fSavannahLatitude = min(fSavannahLatitude, 1.0)
		fSavannahLatitude = max(fSavannahLatitude, 0.0)
		self.fSavannahLatitude = fSavannahLatitude
		#Androrc End
		#R&R End

		fDesertBottomLatitude += self.gc.getClimateInfo(self.map.getClimate()).getDesertBottomLatitudeChange()
		fDesertBottomLatitude = min(fDesertBottomLatitude, 1.0)
		fDesertBottomLatitude = max(fDesertBottomLatitude, 0.0)
		self.fDesertBottomLatitude = fDesertBottomLatitude

		fDesertTopLatitude += self.gc.getClimateInfo(self.map.getClimate()).getDesertTopLatitudeChange()
		fDesertTopLatitude = min(fDesertTopLatitude, 1.0)
		fDesertTopLatitude = max(fDesertTopLatitude, 0.0)
		self.fDesertTopLatitude = fDesertTopLatitude
		
		self.fracXExp = fracXExp
		self.fracYExp = fracYExp

		self.initFractals()
		
	def initFractals(self):
		self.deserts.fracInit(self.iWidth, self.iHeight, self.grain_amount, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)
		self.iDesertTop = self.deserts.getHeightFromPercent(self.iDesertTopPercent)
		self.iDesertBottom = self.deserts.getHeightFromPercent(self.iDesertBottomPercent)

		self.plains.fracInit(self.iWidth, self.iHeight, self.grain_amount+1, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)
		self.iPlainsTop = self.plains.getHeightFromPercent(self.iPlainsTopPercent)
		self.iPlainsBottom = self.plains.getHeightFromPercent(self.iPlainsBottomPercent)

		self.marsh.fracInit(self.iWidth, self.iHeight, self.grain_amount, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)
		self.iMarshTop = self.marsh.getHeightFromPercent(self.iMarshTopPercent)
		self.iMarshBottom = self.marsh.getHeightFromPercent(self.iMarshBottomPercent)

		self.variation.fracInit(self.iWidth, self.iHeight, self.grain_amount, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)

		self.terrainDesert = self.gc.getInfoTypeForString("TERRAIN_DESERT")
		self.terrainPlains = self.gc.getInfoTypeForString("TERRAIN_PLAINS")
		self.terrainIce = self.gc.getInfoTypeForString("TERRAIN_SNOW")
		self.terrainTundra = self.gc.getInfoTypeForString("TERRAIN_TUNDRA")
		self.terrainGrass = self.gc.getInfoTypeForString("TERRAIN_GRASS")
		#R&R
		#Androrc Savannah
		self.terrainSavannah = self.gc.getInfoTypeForString("TERRAIN_SAVANNAH")
		#Androrc End
		#R&R End
		self.terrainMarsh = self.gc.getInfoTypeForString("TERRAIN_MARSH")

	def getLatitudeAtPlot(self, iX, iY):
		"""given a point (iX,iY) such that (0,0) is in the NW,
		returns a value between 0.0 (tropical) and 1.0 (polar).
		This function can be overridden to change the latitudes; for example,
		to make an entire map have temperate terrain, or to make terrain change from east to west
		instead of from north to south"""
		lat = abs((self.iHeight / 2) - iY)/float(self.iHeight/2) # 0.0 = equator, 1.0 = pole

		# Adjust latitude using self.variation fractal, to mix things up:
		lat += (128 - self.variation.getHeight(iX, iY))/(255.0 * 5.0)

		# Limit to the range [0.3, 0.45] for a marshier landscape:
		if lat < 0.3:
			lat = 0.3
		if lat > 0.45:
			lat = 0.45

		return lat

	def generateTerrain(self):		
		terrainData = [0]*(self.iWidth*self.iHeight)
		for x in range(self.iWidth):
			for y in range(self.iHeight):
				iI = y*self.iWidth + x
				terrain = self.generateTerrainAtPlot(x, y)
				terrainData[iI] = terrain
		
		#remove marsh next to desert
		for x in range(self.iWidth):
			for y in range(self.iHeight):
				iIndex = y * self.iWidth + x
								
				for iDirection in range(CardinalDirectionTypes.NUM_CARDINALDIRECTION_TYPES):
					pPlot = plotCardinalDirection(x, y, CardinalDirectionTypes(iDirection))
					
					if not pPlot.isNone():
						iOtherIndex = pPlot.getY() * self.iWidth + pPlot.getX()
						
						if ((terrainData[iIndex] == self.terrainDesert) and (terrainData[iOtherIndex] == self.terrainMarsh)) or ((terrainData[iIndex] == self.terrainMarsh) and (terrainData[iOtherIndex] == self.terrainDesert)):
							terrainData[iIndex] = self.terrainPlains
							break
		
		return terrainData

	def generateTerrainAtPlot(self,iX,iY):
		lat = self.getLatitudeAtPlot(iX,iY)

		plot = self.map.plot(iX, iY)

		if (plot.isWater()):
			return self.map.plot(iX, iY).getTerrainType()

		terrainVal = self.terrainGrass

		if lat >= self.fSnowLatitude:
			terrainVal = self.terrainIce
		elif lat >= self.fTundraLatitude:
			terrainVal = self.terrainTundra
		elif lat < self.fGrassLatitude:
			terrainVal = self.terrainGrass
		else:
			desertVal = self.deserts.getHeight(iX, iY)
			plainsVal = self.plains.getHeight(iX, iY)
			marshVal = self.marsh.getHeight(iX, iY)
			if ((desertVal >= self.iDesertBottom) and (desertVal <= self.iDesertTop) and (lat >= self.fDesertBottomLatitude) and (lat < self.fDesertTopLatitude)):
				terrainVal = self.terrainDesert
			elif ((marshVal >= self.iMarshBottom) and (marshVal <= self.iMarshTop) and plot.isFlatlands() and (lat >= self.fDesertBottomLatitude) and (lat < self.fDesertTopLatitude)):
				terrainVal = self.terrainMarsh
			elif ((plainsVal >= self.iPlainsBottom) and (plainsVal <= self.iPlainsTop)):
				terrainVal = self.terrainPlains

		#R&R
		#Androrc Savannah
		if ((lat < self.fSavannahLatitude) and (terrainVal == self.terrainGrass) and (self.mapRand.get(100, "Add Feature PYTHON") <= 50)):
			terrainVal = self.terrainSavannah

		if ((self.mapRand.get(100, "Add Feature PYTHON") < 50) and (terrainVal == self.terrainMarsh)):
			if (self.mapRand.get(100, "Add Feature PYTHON") <= 50):
				terrainVal = self.terrainSavannah
			else:
				terrainVal = self.terrainGrass
		#Androrc End
		#R&R End

		if (terrainVal == TerrainTypes.NO_TERRAIN):
			return self.map.plot(iX, iY).getTerrainType()

		return terrainVal

def generateTerrainTypes():
	NiTextOut("Generating Terrain (Python Caribbean) ...")
	terraingen = CaribbeanTerrainGenerator()
	terrainTypes = terraingen.generateTerrain()
	return terrainTypes


class CaribbeanFeatureGenerator(CvMapGeneratorUtil.FeatureGenerator):
	def __init__(self, iJunglePercent=90, iForestPercent=0, iLightForestPercent=15,
	             jungle_grain=5, forest_grain=6, 
	             fracXExp=-1, fracYExp=-1):
		
		self.gc = CyGlobalContext()
		self.map = CyMap()
		self.mapRand = self.gc.getGame().getMapRand()
		self.jungles = CyFractal()
		self.forests = CyFractal()
		self.lightForests = CyFractal()
		iW = self.map.getGridWidth()
		iH = self.map.getGridHeight()
		
		self.iFlags = 0  # Disallow FRAC_POLAR flag, to prevent "zero row" problems.
		if self.map.isWrapX(): self.iFlags += CyFractal.FracVals.FRAC_WRAP_X
		if self.map.isWrapY(): self.iFlags += CyFractal.FracVals.FRAC_WRAP_Y

		self.iGridW = self.map.getGridWidth()
		self.iGridH = self.map.getGridHeight()
		
		self.iJunglePercent = iJunglePercent
		self.iForestPercent = iForestPercent
		self.iLightForestPercent = iLightForestPercent

		jungle_grain += self.gc.getWorldInfo(self.map.getWorldSize()).getFeatureGrainChange()
		forest_grain += self.gc.getWorldInfo(self.map.getWorldSize()).getFeatureGrainChange()

		self.jungle_grain = jungle_grain
		self.forest_grain = forest_grain

		self.fracXExp = fracXExp
		self.fracYExp = fracYExp

		self.__initFractals()
		self.__initFeatureTypes()
	
	def __initFractals(self):
		self.jungles.fracInit(self.iGridW, self.iGridH, self.jungle_grain, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)
		self.forests.fracInit(self.iGridW, self.iGridH, self.forest_grain, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)
		self.lightForests.fracInit(self.iGridW, self.iGridH, self.forest_grain, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)
		
		self.iJungleBottom = self.jungles.getHeightFromPercent(50 - (self.iJunglePercent / 2))
		self.iJungleTop = self.jungles.getHeightFromPercent(50 + (self.iJunglePercent / 2))
		self.iForestLevel = self.forests.getHeightFromPercent(100 - self.iForestPercent)
		self.iLightForestLevel = self.lightForests.getHeightFromPercent(100 - self.iLightForestPercent)
		
	def __initFeatureTypes(self):
		self.featureIce = self.gc.getInfoTypeForString("FEATURE_ICE")
		self.featureJungle = self.gc.getInfoTypeForString("FEATURE_JUNGLE")
		self.featureForest = self.gc.getInfoTypeForString("FEATURE_FOREST")
		self.featureLightForest = self.gc.getInfoTypeForString("FEATURE_LIGHT_FOREST")

	def addFeatures(self):
		
		self.gc = CyGlobalContext()
		self.map = CyMap()
		self.mapRand = self.gc.getGame().getMapRand()
		iW = self.map.getGridWidth()
		iH = self.map.getGridHeight()
		
		"adds features to all plots as appropriate"
		for iX in range(iW):
			for iY in range(iH):					
				self.addFeaturesAtPlot(iX, iY)

	def getLatitudeAtPlot(self, iX, iY):
		"returns a value in the range of 0.0 (tropical) to 1.0 (polar)"
		iLatTemp = abs((self.iGridH/2) - iY)/float(self.iGridH/2) # 0.0 = equator, 1.0 = pole
		if(iLatTemp < 0.3):
			iLatTemp = 0.3
		if(iLatTemp > 0.45):
			iLatTemp = 0.45
					
		return iLatTemp

	def addFeaturesAtPlot(self, iX, iY):
		"adds any appropriate features at the plot (iX, iY) where (0,0) is in the SW"
		lat = self.getLatitudeAtPlot(iX, iY)
		pPlot = self.map.sPlot(iX, iY)

		for iI in range(self.gc.getNumFeatureInfos()):
			if pPlot.canHaveFeature(iI):
				if self.mapRand.get(10000, "Add Feature PYTHON") < self.gc.getFeatureInfo(iI).getAppearanceProbability():
					pPlot.setFeatureType(iI, -1)

		if (pPlot.getFeatureType() == FeatureTypes.NO_FEATURE):
			self.addIceAtPlot(pPlot, iX, iY, lat)
			
		if (pPlot.getFeatureType() == FeatureTypes.NO_FEATURE):
			self.addJunglesAtPlot(pPlot, iX, iY, lat)
			
		if (pPlot.getFeatureType() == FeatureTypes.NO_FEATURE):
			self.addLightForestsAtPlot(pPlot, iX, iY, lat)
			
		if (pPlot.getFeatureType() == FeatureTypes.NO_FEATURE):
			self.addForestsAtPlot(pPlot, iX, iY, lat)
		
	def addIceAtPlot(self, pPlot, iX, iY, lat):
		if pPlot.canHaveFeature(self.featureIce):
			if (self.map.isWrapX() and self.map.isWrapY()) and (iY == 0 or iY == self.iGridH - 1):
				pPlot.setFeatureType(self.featureIce, -1)

	def addJunglesAtPlot(self, pPlot, iX, iY, lat):
		if pPlot.canHaveFeature(self.featureJungle):
			iJungleHeight = self.jungles.getHeight(iX, iY)
			if (self.iJungleTop >= iJungleHeight >= self.iJungleBottom + (self.iJungleTop - self.iJungleBottom)*lat):
				pPlot.setFeatureType(self.featureJungle, -1)
	
	def addLightForestsAtPlot(self, pPlot, iX, iY, lat):
		if pPlot.canHaveFeature(self.featureLightForest):
			if (self.lightForests.getHeight(iX, iY) >= self.iLightForestLevel):
				pPlot.setFeatureType(self.featureLightForest, -1)
	
	def addForestsAtPlot(self, pPlot, iX, iY, lat):
		if pPlot.canHaveFeature(self.featureForest):
			if (self.forests.getHeight(iX, iY) >= self.iForestLevel):
				pPlot.setFeatureType(self.featureForest, -1)

def addFeatures():
	global featuregen
	NiTextOut("Adding Features (Python Highlands) ...")
	featuregen = CaribbeanFeatureGenerator()
	featuregen.addFeatures()
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
