##############################################################################
## File: FaireWeather.py version 1.07
## Author: Rich Marinaccio / Revisions by TAC Team, RaR Team and WTP Team
## Copyright 2008 Rich Marinaccio
##############################################################################
##
##############################################################################
## Version History
##
## 1.11 - High seas distance limit parameter added. Allows customizing
## how far the high seas plots can at maximum extend from the map end.
## (by Zycosi)
## 
## 1.1  - Large rivers added. Complete ocean drainage modeling instead of 
## lakes functioning as sinkholes. Results in larger lakes and rich lake-river
## -systems. Lake, drainage, and river algorithms almost completely rewritten.
##
## REFACTORED to run independently of the game in development mode. Additionally,
## added color highlight to many map print commmands.
## (by Belisarius / FlaviusBelisarius @ civFanatics)
##
## 1.08 - Translation into English language added. XML defined in "CIV4GameText_TAC".
## 
## 1.07 - Exchanged order of lattitude and landform options to bypass the quickstart bug. (by koma13)
##
## 1.06 - Translation revision: Deutsche Uebersetzung korrigiert (by Writing Bull)
##
## 1.05 - Added new option allowing you to limit upper and lower map borders to a specific lattitude.
## Increased chances for jungle (temperature and altitude). (by koma13)
##
## 1.04 - Fixed a copy and paste bug that overwrote changes the land percent.
## Also, made separate filters for temperature and altitude filters. This allows
## some better definition to the mountain ranges.
##
## 1.03 - Fixed a problem with mountain ranges forming between the map
## content edge and the water band added to ensure distance to Europe. Fixed
## a problem where light forest was placed on a lake.
##
## 1.02 - Maps really were too large so I shrank them and made them taller
## than they are wide. Added an option for distance to Europe. Added a
## land forms option for more or less islands.
##
## 1.01 - Fix for multiplayer
##
## 1.0 - Ta Daaa!

from array import array
from random import random,randint,seed
import math
import sys
import time

# Belisarius
from sets import Set

# the script can be run without the game for development based on the following boolean switch
gameAccess = True
try:
    from CvPythonExtensions import *
    import CvUtil
    import CvMapGeneratorUtil 

    localText = CyTranslator() #neu
except Exception:
    gameAccess = False


class MapConstants :
    def __init__(self):
        self.mapOptionNames = []
        self.mapOptionNames.append("colony catchment radius")
        self.mapOptionNames.append("distance")
        self.mapOptionNames.append("border north")
        self.mapOptionNames.append("border south")
        self.mapOptionNames.append("land allocation")
        self.mapOptionNames.append("regularity")
        self.mapOptionNames.append("river commonality")
        self.mapOptionNames.append("river length")
        self.mapOptionNames.append("land percent")
        return

    def getMapOption(self, name):
        for i in range(len(self.mapOptionNames)):
            if self.mapOptionNames[i] == name:
                return i
        return -1


    def initialize(self):
##############################################################################
## GLOBAL TUNING VARIABLES: Change these to customize the map results

        #Percent of land vs. water
        self.landPercent = 0.48

        #This map tries to maintain a specific distance to Europe over
        #the entire coast. The distance is greater than the default so that
        #privateers can still be useful.
        self.distanceToEurope = 4
        
        #How many map squares will be above peak threshold and thus 'peaks'.
        self.PeakPercent = 0.05
        self.volcanoProbability = 1.0/150.0
        #How many map squares will be above hill threshold and thus 'hills' unless
        #they are also above peak threshold in which case they will be 'peaks'.
        self.HillPercent = 0.15

        #In addition to the relative peak and hill generation, there is also a
        #process that changes flats to hills or peaks based on altitude. This tends
        #to randomize the high altitude areas somewhat and improve their appearance.
        #These variables control the frequency of hills and peaks at the highest altitude.
        self.HillChanceAtOne = .60
        self.PeakChanceAtOne = .30

        #How many map squares will be below desert rainfall threshold. In this case,
        #rain levels close to zero are very likely to be desert, while rain levels close
        #to the desert threshold will more likely be plains.
        self.DesertPercent = 0.05

        #How many map squares will be below plains rainfall threshold. Rain levels close
        #to the desert threshold are likely to be plains, while those close to the plains
        #threshold are likely to be grassland. 
        self.PlainsPercent = 0.25
        
        #How many map squares will be below grass rainfall threshold. Rain levels close
        #to the plains threshold are likely to be grass, while those close to the plains
        #threshold are likely to be marsh. Any rain above grass threshold will be
        #temperature tested for marsh.
        self.GrassPercent = 0.30

        #---The following variables are not based on percentages. Because temperature
        #---is so strongly tied to latitude, using percentages for things like ice and
        #---tundra leads to very strange results if most of the worlds land lies near
        #---the equator

        #What temperature will be considered cold enough to be ice. Temperatures range
        #from coldest 0.0 to hottest 1.0.
        self.SnowTemp = .32

        #What temperature will be considered cold enough to be tundra. Temperatures range
        #from coldest 0.0 to hottest 1.0.
        self.TundraTemp = .44

        #Hotter than this temperature will be considered deciduous forest, colder will
        #be evergreen forest.Temperatures range from coldest 0.0 to hottest 1.0.
        self.ForestTemp = .51

        #Sugar grows best in tropical and subtropical climates, so marsh should be limited
        #by temperature. High temps will be more likely to allow marsh, while MinMarshTemp
        #will have no likelyhood of allowing marsh.
        self.MinMarshTemp = .6
        #When there is enough rain to create marsh, there is only a chance to create a
        #marsh based on temperature. At this temperature a marsh will always be created
        #on flat land when there is enough rain.
        self.alwaysMarshTemp = 0.8

        #What temperature will be considered hot enough to be jungle. Temperatures range
        #from coldest 0.0 to hottest 1.0.
        # TAC - Map scripts - koma13 - START
        #self.JungleTemp = .7
        self.JungleTemp = .57
        # TAC - Map scripts - koma13 - END
        
        #This is the chance for light forest to exist on plains. 
        self.chanceForLightForest = 0.1

        #Marsh appears where there is the most rainfall, which is also where there
        #are the most trees. Since marsh looks good without trees, it should
        #sometimes not have them.
        self.chanceForTreelessMarsh = 0.4

        #Savannah appears in the same climates as Marsh, which is also where there
        #are the most trees. Since savannah also looks good without trees, it should
        #sometimes not have them. #agnat86
        self.chanceForTreelessSavannah = 0.3
        
        #How many squares are added to a lake for each unit of drainage flowing
        #into it.
        self.LakeSizePerDrainage = 4.0 #6.0

        #This value modifies LakeSizePerRiverLength when a lake begins in desert
        self.DesertLakeModifier = .60
        
        #This value is used to decide if enough water has accumulated to form a river.
        #A lower value creates more rivers over the entire map.
        self.RiverThreshold = 7.0
        # Belisarius - Large Rivers
        self.LargeRiverThreshold = 55.0
        self.riverFordProbability = 1.0/8.0
        # Ramstormp longer rivers
        self.minLargeRiverLength = 7
                
	  #Degrees lattitude for the top and bottom of the map. This allows
        #for more specific climate zones
        self.topLattitude = 90
        self.bottomLattitude = -90

        #Horse lattitudes and polar fronts plus and minus in case you
        #want some zones to be compressed or emphasized.
        self.horseLattitude = 30
        self.polarFrontLattitude = 55

        #Tropics of Cancer and Capricorn plus and minus respectively
        self.tropicsLattitude = 26

        #Oceans are slow to gain and lose heat, so the max and min temps
        #are reduced and raised by this much.
        self.oceanTempClamp = .10

        #Minimum amount of rain dropped by default before other factors
        #add to the amount of rain dropped
        self.minimumRainCost = 0.01

        #Strength of geostrophic rainfall versus monsoon rainfall
        self.geostrophicFactor = 4.0
        
        #Height and Width of main climate and height maps. This does not
        #reflect the resulting map size. Both dimensions( + 1 if wrapping in
        #that dimension = False) must be evenly divisble by self.hmMaxGrain
        self.hmWidth = 65
        self.hmHeight = 129

        #Controls wrapping (not sure if this makes sense yet)
        self.WrapX = False
        self.WrapY = False
        
        #Size of largest map increment to begin midpoint displacement. Must
        #be a power of 2.
        self.hmMaxGrain = 16

        #Option to divide map into two continents as far as the midpoint
        #displacement is concerned. For guaranteed continent separation, further
        #steps will be needed but this option will cause more ocean in the
        #middle of the map. The possible choices are 0 = NO_SEPARATION,
        #1 = NORTH_SOUTH_SEPARATION and 2 = EAST_WEST_SEPARATION.
        self.hmSeparation = 0
        
        #If you sink the margins all the way to 0.0, they become too obvious.
        #This variable sets the maximum amount of sinking
        self.hmMarginDepth = 0.65

        #Margin of ocean around map edge when not wrapping and also through
        #middle when using separation.
        self.hmGrainMargin = 1

        #These are not mountain peaks, but points on the height map initialized
        #to 1.0 before the midpoint displacement process begins. This sets the
        #percentage of 'peaks' for points that are not on the grain margin.
        self.hmInitialPeakPercent = 0.30
        
        #Scales the heuristic for random midpoint displacement. A higher number
        #will create more noise(bumpy), a smaller number will make less
        #noise(smooth).
        self.hmNoiseLevel = 1.0

        #Number of tectonic plates
        self.hmNumberOfPlates = int(float(self.hmWidth * self.hmHeight) * 0.0016)

        #Influence of the plate map, or how much of it is added to the height map.
        self.plateMapScale = 2.00

        #Minimun distance from one plate seed to another
        self.minSeedRange = 15

        #Minimum distance from a plate seed to edge of map
        self.minEdgeRange = 5

        #Chance for plates to grow. Higher chance tends to make more regular
        #shapes. Lower chance makes more irregular shapes and takes longer.
        self.plateGrowthChanceX = 0.2
        self.plateGrowthChanceY = 0.33

        #This sets the amount that tectonic plates differ in altitude.
        self.plateStagger = 0.1

        #This sets the max amount a plate can be staggered up to on the heightmap
        self.plateStaggerRange = 1.0

        #This is the frequency of the cosine ripple near plate boundaries.
        self.rippleFrequency = 1.5

        #This is the amplitude of the ripples near plate boundaries.
        self.rippleAmplitude = 0.8

        #Filter size for temperature smoothing. Must be odd number
        self.filterSize = 15

        #Filter size for altitude smoothing and distance finding. Must be
        #odd number
        self.distanceFilterSize = 7

        #It is necessary to eliminate small inland lakes during the initial
        #heightmap generation. Keep in mind this number is in relation to
        #the initial large heightmap (mc.hmWidth, mc.hmHeight) before the
        #shrinking process
        self.minInlandSeaSize = 100
        
        #After generating the heightmap, bands of ocean can be added to the map
        #to allow a more consistent climate generation. These bands are useful
        #if you are generating part of a world where the weather might be coming
        #in from off the map. These bands can be kept if needed or cropped off
        #later in the process.
        self.northWaterBand = 2
        self.southWaterBand = 2
        self.eastWaterBand = 30
        self.westWaterBand = 30

        #These variables are intended for use with the above water band variables
        #but you can crop the map edge after climate generation for any reason.
        self.northCrop = 2
        self.southCrop = 2
        self.eastCrop = 30
        self.westCrop = 30
        
        #Decides whether to use the Python random generator or the one that is
        #intended for use with civ maps. The Python random has much higher precision
        #than the civ one. 53 bits for Python result versus 16 for getMapRand. The
        #rand they use is actually 32 bits, but they shorten the result to 16 bits.
        #However, the problem with using the Python random is that it may create
        #syncing issues for multi-player now or in the future, therefore it must
        #be optional.
        self.UsePythonRandom = True        
        
        #Below here are static defines. If you change these, the map won't work.
        #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        self.L = 0
        self.N = 1
        self.S = 2
        self.E = 3
        self.W = 4
        self.NE = 5
        self.NW = 6
        self.SE = 7
        self.SW = 8

        self.NO_SEPARATION = 0
        self.NORTH_SOUTH_SEPARATION = 1
        self.EAST_WEST_SEPARATION = 2

        self.width = 70
        self.height = 70

        self.OCEAN = 0
        self.LAND = 1
        self.HILLS = 2
        self.PEAK = 3

        self.OCEAN = 0
        self.COAST = 1
        self.MARSH = 2
        self.GRASS = 3
        self.PRAIRIE = 4
        self.DESERT = 5
        self.TUNDRA = 6
        self.SNOW = 7
        #Androrc Savannah
        self.SAVANNAH = 8
        #Androrc End
        self.PLAINS = 9
        self.SHALLOW_COAST = 10
        # Belisarius - Large Rivers
        self.LARGE_RIVER = 11
        self.LAKE = 12
  
        return
    
    def initInGameOptions(self):
        if gameAccess:
            gc = CyGlobalContext()
            mmap = gc.getMap()
    
            # set city catchment radius
            mmap.setCityCatchmentRadiusMapMaker(mmap.getCustomMapOption(mc.getMapOption("colony catchment radius")))
    
            #Distance to Europe
            selectionID = mmap.getCustomMapOption(self.getMapOption("distance"))
            if selectionID == 0:
                self.distanceToEurope = 3
            elif selectionID == 2:
                self.distanceToEurope = 6
            elif selectionID == 3:
                self.distanceToEurope = 8
            
            # TAC - Map scripts - koma13 - START
            #Lattitudes
            selectionID = mmap.getCustomMapOption(self.getMapOption("border north"))
            if selectionID == 1:
                self.topLattitude = 75
            elif selectionID == 2:
                self.topLattitude = 60
            elif selectionID == 3:
                self.topLattitude = 45
            elif selectionID == 4:
                self.topLattitude = 30
            elif selectionID == 5:
                self.topLattitude = 15
            elif selectionID == 6:
                self.topLattitude = 0
            elif selectionID == 7:
                self.topLattitude = -15
            elif selectionID == 8:
                self.topLattitude = -30
            elif selectionID == 9:
                self.topLattitude = -45
            elif selectionID == 10:
                self.topLattitude = -60
            elif selectionID == 11:
                self.topLattitude = -75
            elif selectionID == 12:
                self.topLattitude = -90
            
            selectionID = mmap.getCustomMapOption(self.getMapOption("border south"))
            if selectionID == 0:
                self.bottomLattitude = 90
            elif selectionID == 1:
                self.bottomLattitude = 75
            elif selectionID == 2:
                self.bottomLattitude = 60
            elif selectionID == 3:
                self.bottomLattitude = 45
            elif selectionID == 4:
                self.bottomLattitude = 30
            elif selectionID == 5:
                self.bottomLattitude = 15
            elif selectionID == 6:
                self.bottomLattitude = 0
            elif selectionID == 7:
                self.bottomLattitude = -15
            elif selectionID == 8:
                self.bottomLattitude = -30
            elif selectionID == 9:
                self.bottomLattitude = -45
            elif selectionID == 10:
                self.bottomLattitude = -60
            elif selectionID == 11:
                self.bottomLattitude = -75
            ## TAC - Map scripts - koma13 - END
    
            #Landforms
            selectionID = mmap.getCustomMapOption(self.getMapOption("land allocation"))
            if selectionID == 1:
                self.hmMaxGrain = 8
                self.hmNumberOfPlates = int(float(self.hmWidth * self.hmHeight) * 0.0030)
                self.minSeedRange = 10
                self.landPercent = 0.28
                self.plateMapScale = 0.5
                self.hmNoiseLevel = 1.5
            
            #Regularity
            selectionID = mmap.getCustomMapOption(self.getMapOption("regularity"))
            if selectionID == 0:
                self.plateGrowthChanceY = 0.22
            elif selectionID == 1:
                self.plateGrowthChanceY = 0.33
            elif selectionID == 2:
                self.plateGrowthChanceY = 0.55
            selectionID = mmap.getCustomMapOption(self.getMapOption("river commonality"))
            if selectionID == 0:
                self.LargeRiverThreshold = 99.9
            elif selectionID == 2:
                self.LargeRiverThreshold = 40.0
            selectionID = mmap.getCustomMapOption(self.getMapOption("river length"))
            if selectionID == 0:
                self.minLargeRiverLength = 3
            elif selectionID == 1:
                self.minLargeRiverLength = 5
            #elif selectionID == 2:
            #    self.minLargeRiverLength = 7
            elif selectionID == 3:
                self.minLargeRiverLength = 9
            elif selectionID == 4:
                self.minLargeRiverLength = 11
            elif selectionID == 5:
                self.minLargeRiverLength = 13
            #Landsize
            selectionID = mmap.getCustomMapOption(self.getMapOption("land percent"))
            if selectionID == 0:
                self.landPercent = 0.25
            elif selectionID == 1:
                self.landPercent = 0.38
            elif selectionID == 2:
                self.landPercent = 0.45
            elif selectionID == 3:
                self.landPercent = 0.55
           
        else:
            # typical choices here
            self.distanceToEurope = 6
            self.topLattitude = -90
            self.bottomLattitude = 90
    
            self.hmMaxGrain = 8
            self.hmNumberOfPlates = int(float(self.hmWidth * self.hmHeight) * 0.0030)
            self.minSeedRange = 10
            self.plateMapScale = 0.5
            self.hmNoiseLevel = 1.5
    
            self.plateGrowthChanceY = 0.33
            self.LargeRiverThreshold = 48.0
            self.minLargeRiverLength = 7
            self.landPercent = 0.45
        return
    
mc = MapConstants()

class PythonRandom :
    def __init__(self):
        return
    def seed(self):
        #Python randoms are not usable in network games.
        if mc.UsePythonRandom:
            self.usePR = True
        else:
            self.usePR = False
        if gameAccess and self.usePR and CyGame().isNetworkMultiPlayer():
            print "Detecting network game. Setting UsePythonRandom to False."
            self.usePR = False
        if self.usePR:
            # Python 'long' has unlimited precision, while the random generator
            # has 53 bits of precision, so I'm using a 53 bit integer to seed the map!
            seed() #Start with system time
            seedValue = randint(0,9007199254740991)
            seed(seedValue)
            print "Random seed (Using Python rands) for this map is %(s)20d" % {"s":seedValue}
            self.mapSeed = seedValue
            
##            seedValue = 5436076319370800
##            seed(seedValue)
##            print "Pre-set seed (Using Pyhon rands) for this map is %(s)20d" % {"s":seedValue}
        else:
            gc = CyGlobalContext()
            self.mapRand = gc.getGame().getMapRand()
            
            seedValue = self.mapRand.get(65535,"Seeding mapRand - FairWeather.py")
            self.mapRand.init(seedValue)
            print "Random seed (Using getMapRand) for this map is %(s)20d" % {"s":seedValue}
            
##            seedValue = 56870
##            self.mapRand.init(seedValue)
##            print "Pre-set seed (Using getMapRand) for this map is %(s)20d" % {"s":seedValue}
        return
    def random(self):
        if self.usePR:
            return random()
        else:
            #This formula is identical to the getFloat function in CvRandom. It
            #is not exposed to Python so I have to recreate it.
            fResult = float(self.mapRand.get(65535,"Getting float -FairWeather.py"))/float(65535)
#            print fResult
            return fResult
    def randint(self,rMin,rMax):
        #if rMin and rMax are the same, then return the only option
        if rMin == rMax:
            return rMin
        #returns a number between rMin and rMax inclusive
        if self.usePR:
            return randint(rMin,rMax)
        else:
            #mapRand.get() is not inclusive, so we must make it so
            return rMin + self.mapRand.get(rMax + 1 - rMin,"Getting a randint - FairWeather.py")
#Set up random number system for global access
PRand = PythonRandom()

################################################################################
## Global functions
################################################################################
#This function converts x and y to an index.
def GetIndex(x,y):
    #Check X for wrap
    if mc.WrapX == True:
        xx = x % mc.width
    elif x < 0 or x >= mc.width:
        return -1
    else:
        xx = x
    #Check y for wrap
    if mc.WrapY == True:
        yy = y % mc.height
    elif y < 0 or y >= mc.height:
        return -1
    else:
        yy = y

    i = yy * mc.width + xx
    return i

def GetHmIndex(x,y):
    #Check X for wrap
    if mc.WrapX == True:
        xx = x % mc.hmWidth
    elif x < 0 or x >= mc.hmWidth:
        return -1
    else:
        xx = x
    #Check y for wrap
    if mc.WrapY == True:
        yy = y % mc.hmHeight
    elif y < 0 or y >= mc.hmHeight:
        return -1
    else:
        yy = y

    i = yy * mc.hmWidth + xx
    return i

#Handles arbitrary size
def GetIndexGeneral(x,y,width,height):
    #Check X for wrap
    if mc.WrapX == True:
        xx = x % width
    elif x < 0 or x >= width:
        return -1
    else:
        xx = x
    #Check y for wrap
    if mc.WrapY == True:
        yy = y % height
    elif y < 0 or y >= height:
        return -1
    else:
        yy = y

    i = yy * width + xx
    return i


#This function scales a float map so that all values are between
#0.0 and 1.0.
def NormalizeMap(fMap,width,height):
    #find highest and lowest points
    maxAlt = 0.0
    minAlt = 0.0
    for y in range(height):
        for x in range(width):
            plot = fMap[GetIndexGeneral(x,y,width,height)]
            if plot > maxAlt:
                maxAlt = plot
            if plot < minAlt:
                minAlt = plot
    #normalize map so that all altitudes are between 1 and 0
    #first add minAlt to all values if necessary
    if minAlt < 0.0:
        for y in range(height):
            for x in range(width):
                fMap[GetIndexGeneral(x,y,width,height)] -= minAlt
    #add minAlt to maxAlt also before scaling entire map
    maxAlt -= minAlt
    scaler = 1.0/maxAlt
    for y in range(height):
        for x in range(width):
            fMap[GetIndexGeneral(x,y,width,height)] = fMap[GetIndexGeneral(x,y,width,height)] * scaler              
    return
def ShrinkMap(largeMap,lWidth,lHeight,sWidth,sHeight):
    smallMap = array('d')
    yScale = float(lHeight)/float(sHeight)
    xScale = float(lWidth)/float(sWidth)
    for y in range(sHeight):
        for x in range(sWidth):
##            print "x = %d, y = %d" % (x,y)
            weights = 0.0
            contributors = 0.0
            yyStart = int(y * yScale)
            yyStop = int((y + 1) * yScale)
            if yyStop < ((y + 1) * yScale):
                yyStop += 1
            for yy in range(yyStart,yyStop):
                xxStart = int(x * xScale)
                xxStop = int((x + 1) * xScale)
                if xxStop < ((x + 1) * xScale):
                    xxStop += 1
                for xx in range(xxStart,xxStop):
##                    print "  xx = %d, yy = %d" % (xx,yy)
                    weight = GetWeight(x,y,xx,yy,xScale,yScale)
##                    print "  weight = %f" % weight
                    i = yy * lWidth + xx
##                    print "  i = %d" % i
                    contributor = largeMap[i]
##                    print "  contributer = %f" % contributor
                    weights += weight
                    contributors += weight * contributor
##            print " final height = %f" % (contributors/weights)        
            smallMap.append(contributors/weights)
                    
    return smallMap
def GetWeight(x,y,xx,yy,xScale,yScale):
    xWeight = 1.0
##    print "   xScale = %f" % xScale
##    print "   x * xScale = %f, xx = %f" % ((x * xScale),xx)
    if float(xx) < x * xScale:
##        print "   first"
        xWeight = 1.0 - ((x * xScale) - float(xx))
    elif float(xx + 1) > (x + 1) * xScale:
##        print "   second"
        xWeight = ((x + 1) * xScale) - float(xx)
##    print "   xWeight = %f" % xWeight
        
    yWeight = 1.0
##    print "   yScale = %f" % yScale
##    print "   y * yScale = %f, yy = %f" % ((y * yScale),yy)
    if float(yy) < y * yScale:
##        print "   first"
        yWeight = 1.0 - ((y * yScale) - float(yy))
    elif float(yy + 1) > (y + 1) * yScale:
##        print "   second"
        yWeight = ((y + 1) * yScale) - float(yy)
##    print "   yWeight = %f" % yWeight
        
    return xWeight * yWeight

def CropMap(theMap):
    newMap = array('d')
    for y in range(mc.hmHeight):
        if y < mc.southCrop or y >= mc.hmHeight - mc.northCrop:
            continue
        for x in range(mc.hmWidth):
            if x < mc.westCrop or x >= mc.hmWidth - mc.eastCrop:
                continue
            i = GetHmIndex(x,y)
            newMap.append(theMap[i])
    return newMap

def AngleDifference(a1,a2):
    diff = a1 - a2
    while(diff < -180.0):
        diff += 360.0
    while(diff > 180.0):
        diff -= 360.0
    return diff
def AppendUnique(theList,newItem):
    if IsInList(theList,newItem) == False:
        theList.append(newItem)
    return

def IsInList(theList,newItem):
    itemFound = False
    for item in theList:
        if item == newItem:
            itemFound = True
            break
    return itemFound

def DeleteFromList(theList,oldItem):
    for n in range(len(theList)):
        if theList[n] == oldItem:
            del theList[n]
            break
    return  
    
def ShuffleList(theList):
        preshuffle = list()
        shuffled = list()
        numElements = len(theList)
        for i in range(numElements):
            preshuffle.append(theList[i])
        for i in range(numElements):
                n = PRand.randint(0,len(preshuffle)-1)
                shuffled.append(preshuffle[n])
                del preshuffle[n]
        return shuffled
    
def GetInfoType(string):
    cgc = CyGlobalContext()
    return cgc.getInfoTypeForString(string)
    
def GetDistance(x,y,dx,dy):
    distance = math.sqrt(abs((float(x - dx) * float(x - dx)) + (float(y - dy) * float(y - dy))))
    return distance

def GetOppositeDirection(direction):
    opposite = mc.L
    if direction == mc.N:
        opposite = mc.S
    elif direction == mc.S:
        opposite = mc.N
    elif direction == mc.E:
        opposite = mc.W
    elif direction == mc.W:
        opposite = mc.E
    elif direction == mc.NW:
        opposite = mc.SE
    elif direction == mc.SE:
        opposite = mc.NW
    elif direction == mc.SW:
        opposite = mc.NE
    elif direction == mc.NE:
        opposite = mc.SW
    return opposite

def GetXYFromDirection(x,y,direction):
    xx = x
    yy = y
    if direction == mc.N:
        yy += 1
    elif direction == mc.S:
        yy -= 1
    elif direction == mc.E:
        xx += 1
    elif direction == mc.W:
        xx -= 1
    elif direction == mc.NW:
        yy += 1
        xx -= 1
    elif direction == mc.NE:
        yy += 1
        xx += 1
    elif direction == mc.SW:
        yy -= 1
        xx -= 1
    elif direction == mc.SE:
        yy -= 1
        xx += 1
    return xx,yy

##This function is a general purpose value tuner. It finds a value that will be greater
##than or less than the desired percent of a whole map within a given tolerance. Map values
##should be between 0 and 1.
def FindValueFromPercent(mmap,width,height,percent,tolerance,greaterThan):
    inTolerance = False
    #to speed things up a little, lets take some time to find the middle value
    #in the dataset and use that to begin our search
    minV = 100.0
    maxV = 0.0
    for i in range(height*width):
        if minV > mmap[i]:
            minV = mmap[i]
        if maxV < mmap[i]:
            maxV = mmap[i]
    mid = (maxV - minV)/2.0 + minV
##    print "starting threshold = "
##    print mid
##    print "desired percent",percent
    threshold = mid
    thresholdChange = mid
    iterations = 0
    while not inTolerance:
        iterations += 1
        if(iterations > 500):
            print "can't find value within tolerance, end value = "
            print threshold, thresholdChange
            break #close enough
        matchCount = 0
##        print "threshold",threshold
        for i in range(height*width):
            if mmap[i] != 0.0:
                if greaterThan == True:
                    if(mmap[i] > threshold):
                        matchCount += 1
                else:
                    if(mmap[i] < threshold):  
                        matchCount += 1
##        print "matchCount",matchCount
        currentPercent = float(matchCount)/float(height*width)
##        print "currentPercent",currentPercent
        if currentPercent < percent + tolerance and \
           currentPercent > percent - tolerance:
            inTolerance = True
        elif greaterThan == True:
            if currentPercent < percent - tolerance:
                threshold -= thresholdChange
            else:
                threshold += thresholdChange
        else:
            if currentPercent > percent + tolerance:
                threshold -= thresholdChange
            else:
                threshold += thresholdChange
        if not inTolerance:
#                print "sealevel = " + str(self.seaLevel), "change = " + str(seaLevelChange)
            thresholdChange = thresholdChange/2.0

        #at this point value should be in tolerance or close to it
    return threshold
def isWaterMatch(x,y):
    result = hm.isBelowSeaLevel(x,y)
##    print "match function results - x = %d,y = %d,result = %d" % (x,y,result)
    return result

class HeightMap :
    def __init__(self):
        return
    
    def generateHeightMap(self):
        self.heightMap = array('d')
        for i in range(mc.hmWidth * mc.hmHeight):
            self.heightMap.append(0.0)

        self.generateMidpointDisplacement()
        return
    
    def checkMaxGrain(self):
        #hm map dimensions(minus 1 if no wrapping) must be evenly divisible
        #by max grain
        ok = True
        width = mc.hmWidth
        height = mc.hmHeight
        if mc.WrapX == False:
            width -= 1
        if mc.WrapY == False:
            height -= 1
            
        if 0 != width % mc.hmMaxGrain:
            ok = False
        if 0 != height % mc.hmMaxGrain:
            ok = False

        if ok == False:
            raise ValueError, "height map dimesions not divisible by mc.hmMaxGrain. also check wrapping options"
            
        return

    def isPlotOnMargin(self,x,y):
        #first check top and bottom
        if mc.WrapY == False: #no margin needed when wrapping
            if y < (mc.hmMaxGrain * mc.hmGrainMargin):
                return True
            if y > (mc.hmHeight - (mc.hmMaxGrain * mc.hmGrainMargin)):
                return True
        #check right and left
        if mc.WrapX == False:
            if x < (mc.hmMaxGrain * mc.hmGrainMargin):
                return True
            if x > (mc.hmWidth - (mc.hmMaxGrain * mc.hmGrainMargin)):
                return True

        #now check middle
        if mc.hmSeparation != mc.NO_SEPARATION:
            if mc.hmSeparation == mc.NORTH_SOUTH_SEPARATION:
                dimension = y
                middle = mc.hmHeight/2
            elif mc.hmSeparation == mc.EAST_WEST_SEPARATION:
                dimension = x
                middle = mc.hmWidth/2
            else:
                raise ValueError, "bad hmSeparation type"

            if dimension > middle - (mc.hmMaxGrain * mc.hmGrainMargin) \
            and dimension < middle + (mc.hmMaxGrain * mc.hmGrainMargin):
                return True

        return False
    
    def generateMidpointDisplacement(self):
        self.checkMaxGrain()
        
        #make list of map plots that aren't on margin for each
        #map quadrant. We want to place the initial peaks randomly, but we
        #also want to ensure fairly even distribution so that
        #not all the peaks are on one side of the map. For this purpose
        #we will treat each map quadrant separately.

        peaksNWList = list()
        peaksNEList = list()
        peaksSWList = list()
        peaksSEList = list()
        middleX = mc.hmWidth/2
        middleY = mc.hmHeight/2
        for y in range(0,mc.hmHeight,mc.hmMaxGrain):
            for x in range(0,mc.hmWidth,mc.hmMaxGrain):
                if not self.isPlotOnMargin(x,y):
                    if x < middleX and y < middleY:
                        peaksSWList.append((x,y))
                    elif x >= middleX and y < middleY:
                        peaksSEList.append((x,y))
                    elif x < middleX and y >= middleY:
                        peaksNWList.append((x,y))
                    elif x >= middleX and y >= middleY:
                        peaksNEList.append((x,y))
        #shuffle the lists
        peaksNWList = ShuffleList(peaksNWList)
        peaksNEList = ShuffleList(peaksNEList)
        peaksSWList = ShuffleList(peaksSWList)
        peaksSEList = ShuffleList(peaksSEList)

        #place desired number of peaks in each quadrant
        totalNonMargin = len(peaksNWList)
        totalNonMargin += len(peaksNEList)
        totalNonMargin += len(peaksSWList)
        totalNonMargin += len(peaksSEList)
        
        count = max(1,int(float(totalNonMargin) * mc.hmInitialPeakPercent * 0.25))
        print "peak count = %d" % (count)
        for n in range(count):
            x,y = peaksNWList[n]
            i = GetHmIndex(x,y)
            self.heightMap[i] = 1.0
            print "%d,%d = 1.0" % (x,y)
            
            x,y = peaksNEList[n]
            i = GetHmIndex(x,y)
            self.heightMap[i] = 1.0
            
            x,y = peaksSWList[n]
            i = GetHmIndex(x,y)
            self.heightMap[i] = 1.0
            
            x,y = peaksSEList[n]
            i = GetHmIndex(x,y)
            self.heightMap[i] = 1.0
            
        self.printInitialPeaks()

        #Now use a diamond-square algorithm(sort of) to generate the rest
        currentGrain = float(mc.hmMaxGrain)
        while currentGrain > 1.0:
            #h is scalar for random displacement
            h = (currentGrain/float(mc.hmMaxGrain)) * float(mc.hmNoiseLevel)
            #First do the 'square' pass
            for y in range(0,mc.hmHeight,int(currentGrain)):
                for x in range(0,mc.hmWidth,int(currentGrain)):
                    #on the square pass, GetHmIndex should handle all wrapping needs
                    topLeft = GetHmIndex(x,y)
                    topRight = GetHmIndex(x + int(currentGrain),y)
                    if topRight == -1:
                        continue #this means no wrap in x direction
                    bottomLeft = GetHmIndex(x,y + int(currentGrain))
                    if bottomLeft == -1:
                        continue #this means no wrap in y direction
                    bottomRight = GetHmIndex(x + int(currentGrain),y + int(currentGrain))
                    middle = GetHmIndex(x + int(currentGrain/2.0),y + int(currentGrain/2.0))
                    average = (self.heightMap[topLeft] + self.heightMap[topRight] \
                    + self.heightMap[bottomLeft] + self.heightMap[bottomRight])/4.0
                    displacement = h * PRand.random() - h/2.0
                    self.heightMap[middle] = average + displacement
                    #now add that heuristic to the four points to diminish
                    #artifacts. We don't need this on the diamond pass I don't think
                    displacement = h * PRand.random() - h/2.0
                    self.heightMap[topLeft] += displacement
                    displacement = h * PRand.random() - h/2.0
                    self.heightMap[topRight] += displacement
                    displacement = h * PRand.random() - h/2.0
                    self.heightMap[bottomLeft] += displacement
                    displacement = h * PRand.random() - h/2.0
                    self.heightMap[bottomRight] += displacement
            #Now do the 'diamond' pass, there are two diamonds for each x.
            #Possible wrapping is a big complication on this pass. Sorry!
            for y in range(0,mc.hmHeight,int(currentGrain)):
                for x in range(0,mc.hmWidth,int(currentGrain)):
                    #first do the right facing diamond
                    left = GetHmIndex(x,y)
                    right = GetHmIndex(x + int(currentGrain),y)
                    if right != -1: #if we're off map at this point go to next diamond
                        average = self.heightMap[left] + self.heightMap[right]
                        contributers = 2 #each diamond may have two or three contributers, 2 so far
                        top = GetHmIndex(x + int(currentGrain/2.0),y - int(currentGrain/2.0))
                        if top != -1:
                            contributers += 1
                            average += self.heightMap[top]
                        bottom = GetHmIndex(x + int(currentGrain/2.0),y + int(currentGrain/2.0))
                        if bottom != -1:
                            contributers += 1
                            average += self.heightMap[bottom]
                        average = average/float(contributers)
                        middle = GetHmIndex(x + int(currentGrain/2.0),y)
                        displacement = h * PRand.random() - h/2.0
                        self.heightMap[middle] = average + displacement
                    #now do the down facing diamond
                    top = GetHmIndex(x,y)
                    bottom = GetHmIndex(x,y + int(currentGrain))
                    if bottom != -1:
                        average = self.heightMap[top] + self.heightMap[bottom]
                        contributers = 2
                        right = GetHmIndex(x + int(currentGrain/2.0),y + int(currentGrain/2.0))
                        if right != -1:
                            contributers += 1
                            average += self.heightMap[right]
                        left = GetHmIndex(x - int(currentGrain/2.0),y + int(currentGrain/2.0))
                        if left != -1:
                            contributers += 1
                            average += self.heightMap[left]
                        average = average/float(contributers)
                        middle = GetHmIndex(x,y + int(currentGrain/2.0))
                        displacement = h * PRand.random() - h/2.0
                        self.heightMap[middle] = average + displacement
                        
            currentGrain = currentGrain/2.0

        NormalizeMap(self.heightMap,mc.hmWidth,mc.hmHeight)
        
        return
    
    def performTectonics(self):
        self.plateMap = list()
        borderMap = array('i')#this will help in later distance calculations
        self.plateHeightMap = array('d')
        preSmoothMap = array('d')
        growthPlotList = list()
        plateList = list()
        maxDistance = math.sqrt(pow(float(mc.distanceFilterSize/2),2) + pow(float(mc.distanceFilterSize/2),2))
        #initialize maps
        for y in range(mc.hmHeight):
            for x in range(mc.hmWidth):
                i = GetHmIndex(x,y)
                self.plateMap.append(PlatePlot(0,maxDistance))
                borderMap.append(False)
                self.plateHeightMap.append(0.0)
                preSmoothMap.append(0.0)

        plateList.append(Plate(0,-1,-1))#zero placeholder (very silly I know)
        #seed plates
        for i in range(1,mc.hmNumberOfPlates + 1):
            #first find a random seed point that is not blocked by
            #previous points
            iterations = 0
            while(True):
                iterations += 1
                if iterations > 10000:
                    raise ValueError, "endless loop in region seed placement"
                seedX = PRand.randint(0,mc.hmWidth + 1)
                seedY = PRand.randint(0,mc.hmHeight + 1)
                n = GetHmIndex(seedX,seedY)
                if self.isSeedBlocked(plateList,seedX,seedY) == False:
                    self.plateMap[n].plateID = i
                    plate = Plate(i,seedX,seedY)
                    plateList.append(plate)
                    #Now fill a 3x3 area to insure a minimum region size
                    for direction in range(1,9,1):
                        xx,yy = GetXYFromDirection(seedX,seedY,direction)
                        nn = GetHmIndex(xx,yy)
                        if nn != -1:
                            self.plateMap[nn].plateID = i
                            plot = (xx,yy,i)
                            growthPlotList.append(plot)

                    break
                
##        self.printPlateMap(self.plateMap)
        
        #Now cause the seeds to grow into plates
        iterations = 0
        while(len(growthPlotList) > 0):
            iterations += 1
            if iterations > 200000:
                self.printPlateMap(self.plateMap)
                print "length of growthPlotList = %d" % (len(growthPlotList))
                raise ValueError, "endless loop in plate growth"
            plot = growthPlotList[0]
            roomLeft = False
            for direction in range(1,5,1):
                x,y,plateID = plot
                i = GetHmIndex(x,y)
                xx,yy = GetXYFromDirection(x,y,direction)
                ii = GetHmIndex(xx,yy)
                if ii == -1:
                    plateList[plateID].isOnMapEdge = True
                    continue
                if self.plateMap[ii].plateID != plateID and self.plateMap[ii].plateID != 0:
                    borderMap[i] = True
                    borderMap[ii] = True                   
                elif self.plateMap[ii].plateID == 0:
                    roomLeft = True
                    if direction == mc.N or direction == mc.S:
                        growthChance = mc.plateGrowthChanceY
                    else:
                        growthChance = mc.plateGrowthChanceX
                    if PRand.random() < growthChance:
                        self.plateMap[ii].plateID = plateID
                        newPlot = (xx,yy,plateID)
                        growthPlotList.append(newPlot)
                
                    
            #move plot to the end of the list if room left, otherwise
            #delete it if no room left
            if roomLeft:
                growthPlotList.append(plot)
            del growthPlotList[0]
                            
##        self.printPlateMap(self.plateMap)
        
        #Stagger the plates somewhat to add interest
        steps = int(mc.plateStaggerRange/mc.plateStagger)
        for i in range(0,mc.hmHeight*mc.hmWidth):
            if plateList[self.plateMap[i].plateID].isOnMapEdge:
                preSmoothMap[i] = 0.0
            else:
                preSmoothMap[i] = float(self.plateMap[i].plateID % steps) * mc.plateStagger

##        self.printPreSmoothMap(preSmoothMap)

        #Now smooth the plate height map and create the distance map at the same time
        #Since the algorithm is the same
        for y in range(0,mc.hmHeight):
            for x in range(0,mc.hmWidth):
                contributers = 0
                avg = 0
                i = GetHmIndex(x,y)
                isBorder = False
                if borderMap[i] == True:
                    isBorder = True
                plateID = self.plateMap[i].plateID
                for yy in range(y - mc.distanceFilterSize/2,y + mc.distanceFilterSize/2 + 1,1):
                    for xx in range(x - mc.distanceFilterSize/2,x + mc.distanceFilterSize/2 + 1,1):
                        ii = GetHmIndex(xx,yy)
                        if ii == -1:
                            continue
                        contributers += 1
                        avg += preSmoothMap[ii]
                        if isBorder and plateID != self.plateMap[ii].plateID:
                            distance = math.sqrt(pow(float(y - yy),2) + pow(float(x - xx),2))
                            if distance < self.plateMap[ii].distanceList[plateID]:
                                self.plateMap[ii].distanceList[plateID] = distance
                avg = avg/float(contributers)
                self.plateHeightMap[i] = avg
                
##        self.printPlateHeightMap()
#        self.printDistanceMap(distanceMap,maxDistance)

        #Now add ripple formula to plateHeightMap
        for i in range(mc.hmWidth*mc.hmHeight):
            avgRippleTop = 0.0
            avgRippleBottom = 0.0
            for plateID in range(1,mc.hmNumberOfPlates + 1):
                distanceWeight = maxDistance - self.plateMap[i].distanceList[plateID]
#                print "a1 = %f, a2 = %f" % (plateList[self.plateMap[i].plateID].angle,plateList[plateID].angle)
                if plateList[plateID].seedX < plateList[self.plateMap[i].plateID].seedX:
                    angleDifference = AngleDifference(plateList[self.plateMap[i].plateID].angle,plateList[plateID].angle)
                else:
                    angleDifference = AngleDifference(plateList[plateID].angle,plateList[self.plateMap[i].plateID].angle)
#                print angleDifference
                ripple = (pow(math.cos(mc.rippleFrequency * self.plateMap[i].distanceList[plateID]) * \
                (-self.plateMap[i].distanceList[plateID]/maxDistance + 1),2) + (-self.plateMap[i].distanceList[plateID]/maxDistance + 1)) \
                * mc.rippleAmplitude * math.sin(math.radians(angleDifference))
                avgRippleTop += (ripple * distanceWeight)
                avgRippleBottom += distanceWeight
            if avgRippleBottom == 0.0:
                avgRipple = 0.0
            else:
                avgRipple = avgRippleTop/avgRippleBottom
            self.plateHeightMap[i] += avgRipple
            
        NormalizeMap(self.plateHeightMap,mc.hmWidth,mc.hmHeight)
##        self.printPlateHeightMap()


    def combineMaps(self):                    
        #Now add plateHeightMap to HeightMap
        for i in range(mc.hmWidth * mc.hmHeight):
            self.heightMap[i] += self.plateHeightMap[i] * mc.plateMapScale

        #depress margins, this time with brute force
        marginSize = mc.hmMaxGrain * mc.hmGrainMargin
        for y in range(mc.hmHeight):
            for x in range(mc.hmWidth):
                i = GetHmIndex(x,y)
                if mc.WrapX == False:
                    if x < marginSize:
                        self.heightMap[i] *= (float(x)/float(marginSize)) * (1.0 - mc.hmMarginDepth) + mc.hmMarginDepth

                    elif mc.hmWidth - x < marginSize:
                        self.heightMap[i] *= (float(mc.hmWidth - x)/float(marginSize)) * (1.0 - mc.hmMarginDepth) + mc.hmMarginDepth
                if mc.WrapY == False:
                    if y < marginSize:
                        self.heightMap[i] *= (float(y)/float(marginSize)) * (1.0 - mc.hmMarginDepth) + mc.hmMarginDepth

                    elif mc.hmHeight - y < marginSize:
                        self.heightMap[i] *= (float(mc.hmHeight - y)/float(marginSize)) * (1.0 - mc.hmMarginDepth) + mc.hmMarginDepth

                if mc.hmSeparation == mc.NORTH_SOUTH_SEPARATION:
                    difference = abs((mc.hmHeight/2) - y)
                    if difference < marginSize:
                        self.heightMap[i] *= (float(difference)/float(marginSize)) * (1.0 - mc.hmMarginDepth) + mc.hmMarginDepth

                elif mc.hmSeparation == mc.EAST_WEST_SEPARATION:
                    difference = abs((mc.hmWidth/2) - x)
                    if difference < marginSize:
                        self.heightMap[i] *= (float(difference)/float(marginSize)) * (1.0 - mc.hmMarginDepth) + mc.hmMarginDepth
                        
        #Now lets square the heightmap to simulate erosion
        for i in range(mc.hmWidth * mc.hmHeight):
            self.heightMap[i] = self.heightMap[i] * self.heightMap[i]

        NormalizeMap(self.heightMap,mc.hmWidth,mc.hmHeight)

    def addWaterBands(self):
        #validate water bands. Maps that wrap cannot have one in that direction
        if mc.WrapX and (mc.eastWaterBand != 0 or mc.westWaterBand != 0):
            raise ValueError,"east/west water bands cannot be used when wrapping in X direction."
        if mc.WrapY and (mc.northWaterBand != 0 or mc.southWaterBand != 0):
            raise ValueError,"north/south water bands cannot be used when wrapping in Y direction."
        
        newWidth = mc.hmWidth + mc.eastWaterBand + mc.westWaterBand
        newHeight = mc.hmHeight + mc.northWaterBand + mc.southWaterBand
        newHeightMap = array('d')
        for y in range(newHeight):
            for x in range(newWidth):
                oldX = x - mc.westWaterBand
                oldY = y - mc.southWaterBand
#                i = GetIndexGeneral(x,y,newWidth,newHeight)
                ii = GetHmIndex(oldX,oldY)
                if ii == -1:
                    newHeightMap.append(0.0)
                else:
                    newHeightMap.append(self.heightMap[ii])

        mc.hmWidth = newWidth
        mc.hmHeight = newHeight
        self.heightMap = newHeightMap
        
    def calculateSeaLevel(self):
        self.seaLevel = FindValueFromPercent(self.heightMap,mc.hmWidth,mc.hmHeight,mc.landPercent,0.02,True)
        return
    
    def isBelowSeaLevel(self,x,y):
        i = GetHmIndex(x,y)
##        print "heightMap = %f at %d,%d" % (self.heightMap[i],x,y)
##        print "seaLevel = %f" % self.seaLevel
        if self.heightMap[i] < self.seaLevel:
##            print "True"
            return True
##        print "False"
        return False
    
    ## This function returns altitude in relation to sea level with
    ## 0.0 being seaLevel and 1.0 being highest altitude
    def getAltitudeAboveSeaLevel(self,x,y):
        i = GetHmIndex(x,y)
        if i == -1:
            return 0.0
        altitude = self.heightMap[i]
        if altitude < self.seaLevel:
            return 0.0
        altitude = 1.0/(1.0 - self.seaLevel) * (altitude - self.seaLevel)
        return altitude
    
    def isSeedBlocked(self,plateList,seedX,seedY):
        for plate in plateList:
            if seedX > plate.seedX - mc.minSeedRange and seedX < plate.seedX + mc.minSeedRange:
                if seedY > plate.seedY - mc.minSeedRange and seedY < plate.seedY + mc.minSeedRange:
                    return True
        #Check for edge
        if seedX < mc.minEdgeRange or seedX >= (mc.hmWidth + 1) - mc.minEdgeRange:
            return True
        if seedY < mc.minEdgeRange or seedY >= (mc.hmHeight + 1) - mc.minEdgeRange:
            return True
        return False
    def GetInfluFromDistance(self,sinkValue,peakValue,searchRadius,distance):
        influence = peakValue
        maxDistance = math.sqrt(pow(float(searchRadius),2) + pow(float(searchRadius),2))
        #minDistance = 1.0
        influence -= ((peakValue - sinkValue)* (distance - 1.0))/(maxDistance - 1.0)
        return influence
    def FindDistanceToPlateBoundary(self,x,y,searchRadius):
        minDistance = 10.0
        i = self.GetIndex(x,y)
        for yy in range(y - searchRadius,y + searchRadius):
            for xx in range(x - searchRadius,x + searchRadius):
                ii = self.GetIndex(xx,yy)
                if self.plateMap[i] != self.plateMap[ii]:
                    distance = math.sqrt(pow(float(xx-x),2) + pow(float(yy-y),2))
                    if distance < minDistance:
                        minDistance = distance
                           
        if minDistance == 10.0:
            return 0.0
        
        return minDistance
    
    def fillInLakes(self):
        #smaller lakes need to be filled in for now. The river system will
        #most likely recreate them later due to drainage calculation
        #according to certain rules. This makes the lakes look much better
        #and more sensible.
        am = Areamap(mc.hmWidth,mc.hmHeight,True,True)
        am.defineAreas(isWaterMatch)
##        am.PrintAreaMap()
        oceanID = am.getOceanID()
        for y in range(mc.hmHeight):
            for x in range(mc.hmWidth):
                i = GetHmIndex(x,y)
                if self.isBelowSeaLevel(x,y) and am.areaMap[i] != oceanID:
                    #check the size of this body of water, if too small,
                    #change to land
                    for a in am.areaList:
                        if a.ID == am.areaMap[i] and a.size < mc.minInlandSeaSize:
                            self.heightMap[i] = self.seaLevel
        
        return
    
    def printInitialPeaks(self):
        lineString = "midpoint displacement peaks and margins"
        print lineString
        if not mc.WrapY:
            adjustedHeight = mc.hmHeight - 1
        else:
            adjustedHeight = mc.hmHeight - mc.hmMaxGrain
        for y in range(adjustedHeight,-1,-mc.hmMaxGrain):
            lineString = ""
            for x in range(0,mc.hmWidth,mc.hmMaxGrain):
                i = GetHmIndex(x,y)
                if self.isPlotOnMargin(x,y):
                    lineString += "*"
                elif self.heightMap[i] == 1.0:
                    lineString += "1"
                elif self.heightMap[i] == 0.0:
                    lineString += "0"
            print lineString
        lineString = " "
        print lineString
        
    def printHeightMap(self):
        lineString = "Height Map"
        print lineString
        for y in range(mc.hmHeight - 1,-1,-1):
            lineString = ""
            for x in range(0,mc.hmWidth,1):
                i = GetHmIndex(x,y)
                mapLoc = int((self.heightMap[i] - self.seaLevel)/(1.0 - self.seaLevel) * 10)
                #mapLoc = int(self.heightMap[i] * 10)
                if self.heightMap[i] < self.seaLevel:
                    lineString += '.'
                else:
                    lineString += chr(mapLoc + 48)
            print lineString
        lineString = " "
        print lineString
        
    def printPlateMap(self,plateMap):
        lineString = "Plate Map"
        print lineString
        for y in range(mc.hmHeight - 1,-1,-1):
            lineString = ""
            for x in range(0,mc.hmWidth,1):
                i = GetHmIndex(x,y)
                mapLoc = plateMap[i].plateID
                if mapLoc > 40:
                    mapLoc = 41
                lineString += chr(mapLoc + 48)
            print lineString
        lineString = " "
        print lineString
        
    def printPreSmoothMap(self,preSmoothMap):
        lineString = "Pre-Smooth Map"
        print lineString
        for y in range(mc.hmHeight - 1,-1,-1):
            lineString = ""
            for x in range(0,mc.hmWidth,1):
                i = GetHmIndex(x,y)
                mapLoc = int(preSmoothMap[i] * 40)
                lineString += chr(mapLoc + 48)
            print lineString
        lineString = " "
        print lineString
        
    def printPlateHeightMap(self):
        lineString = "Plate Height Map"
        print lineString
        for y in range(mc.hmHeight - 1,-1,-1):
            lineString = ""
            for x in range(0,mc.hmWidth,1):
                i = GetHmIndex(x,y)
                mapLoc = int(self.plateHeightMap[i] * 40)
                lineString += chr(mapLoc + 48)
            print lineString
        lineString = " "
        print lineString
        
    def printDistanceMap(self,distanceMap,maxDistance):
        lineString = "Plate Height Map"
        print lineString
        for y in range(mc.hmHeight - 1,-1,-1):
            lineString = ""
            for x in range(0,mc.hmWidth,1):
                i = GetHmIndex(x,y)
                mapLoc = int((distanceMap[i]/maxDistance) * 40)
                lineString += chr(mapLoc + 48)
            print lineString
        lineString = " "
        print lineString
class Plate :
    def __init__(self,ID,seedX,seedY):
        self.ID = ID
        self.seedX = seedX
        self.seedY = seedY
        self.isOnMapEdge = False
##        if PRand.random() < 0.5:
##            self.angle = 0.0 #plate movement vector
##        else:
##            self.angle = 180.0
        self.angle = (PRand.random() * 360) - 180
class PlatePlot :
    def __init__(self,plateID,maxDistance):
        self.plateID = plateID
        self.distanceList = list()
        for i in range(mc.hmNumberOfPlates + 1):
            self.distanceList.append(maxDistance)
            
class ClimateMap :
    def __init__(self):
        return
    def createClimateMaps(self):
        summerSunMap = array('d')
        winterSunMap = array('d')
        self.summerTempsMap = array('d')
        self.winterTempsMap = array('d')
        self.averageTempMap = array('d')
        self.moistureMap = array('d')
        self.rainFallMap = array('d')

        self.initializeTempMap(summerSunMap,mc.tropicsLattitude)
        self.initializeTempMap(winterSunMap,-mc.tropicsLattitude)

        #smooth both sun maps into the temp maps
        for y in range(0,mc.hmHeight):
            for x in range(0,mc.hmWidth):
                contributers = 0
                summerAvg = 0
                winterAvg = 0
                i = GetHmIndex(x,y)
                for yy in range(y - mc.filterSize/2,y + mc.filterSize/2 + 1,1):
                    for xx in range(x - mc.filterSize/2,x + mc.filterSize/2 + 1,1):
                        ii = GetHmIndex(xx,yy)
                        if ii == -1:
                            continue
                        contributers += 1
                        summerAvg += summerSunMap[ii]
                        winterAvg += winterSunMap[ii]
                summerAvg = summerAvg/float(contributers)
                winterAvg = winterAvg/float(contributers)
                self.summerTempsMap.append(summerAvg)
                self.winterTempsMap.append(winterAvg)
                
        #create average temp map
        for y in range(mc.hmHeight):
            for x in range(mc.hmWidth):
                i = GetHmIndex(x,y)
                #average summer and winter
                avgTemp = (self.summerTempsMap[i] + self.winterTempsMap[i])/2.0
                #cool map for altitude
                # TAC - Map scripts - koma13 - START
                #self.averageTempMap.append(avgTemp * (1.0 - hm.getAltitudeAboveSeaLevel(x,y)))
                self.averageTempMap.append(avgTemp * (1.0 - hm.getAltitudeAboveSeaLevel(x,y) ** 1.25))
                # TAC - Map scripts - koma13 - END
        
        #init moisture and rain maps
        for i in range(mc.hmHeight*mc.hmWidth):
            self.moistureMap.append(0.0)
            self.rainFallMap.append(0.0)
            
        #create sortable plot list for summer monsoon rains
        temperatureList = list()
        for y in range(mc.hmHeight):
            for x in range(mc.hmWidth):
                i = GetHmIndex(x,y)
                rainPlot = RainPlot(x,y,self.summerTempsMap[i],0)
                temperatureList.append(rainPlot)
        #sort by temperature, coldest first
        temperatureList.sort(lambda x,y:cmp(x.order,y.order))
          
        #Drop summer monsoon rains
        self.dropRain(temperatureList,self.summerTempsMap,False,None)

        #clear moisture map
        for i in range(mc.hmHeight*mc.hmWidth):
            self.moistureMap[i] = 0.0
            
        #create sortable plot list for winter monsoon rains
        temperatureList = list()
        for y in range(mc.hmHeight):
            for x in range(mc.hmWidth):
                i = GetHmIndex(x,y)
                rainPlot = RainPlot(x,y,self.winterTempsMap[i],0)
                temperatureList.append(rainPlot)
        #sort by temperature, coldest first
        temperatureList.sort(lambda x,y:cmp(x.order,y.order))
          
        #Drop winter monsoon rains
        self.dropRain(temperatureList,self.winterTempsMap,False,None)

        #clear moisture map
        for i in range(mc.hmHeight*mc.hmWidth):
            self.moistureMap[i] = 0.0
            
        #set up WindZones class
        wz = WindZones(mc.hmHeight,mc.topLattitude,mc.bottomLattitude)

        #create ordered list for geostrophic rain
        orderList = list()
        for zone in range(6):
            topY = wz.GetYFromZone(zone,True)
            bottomY = wz.GetYFromZone(zone,False)
            if topY == -1 and bottomY == -1:
                continue #This wind zone is not represented on this map at all so skip it
            if topY == -1: #top off map edge
                topY = mc.hmHeight - 1
            if bottomY == -1:
                bottomY = 0

            dx,dy = wz.GetWindDirectionsInZone(zone)
            if dy < 0:
                yStart = topY
                yStop = bottomY - 1
            else:
                yStart = bottomY
                yStop = topY + 1
            if dx < 0:
                xStart = mc.hmWidth - 1
                xStop = -1
            else:
                xStart = 0
                xStop = mc.hmWidth
            order = 0.0
            for y in range(yStart,yStop,dy):
                for x in range(xStart,xStop,dx):
                    rainPlot = RainPlot(x,y,order,abs(yStop - y))
                    orderList.append(rainPlot)
                    order += 1.0

        #Sort order list
        orderList.sort(lambda x,y:cmp(x.order,y.order))

        #drop geostrophic rain            
        self.dropRain(orderList,self.averageTempMap,True,wz)
        

        NormalizeMap(self.rainFallMap,mc.hmWidth,mc.hmHeight)

##        self.printRainFallMap(True)
##        self.printRainFallMap(False)
        
                
    def dropRain(self,plotList, tempMap, bGeostrophic, windZones):
        countRemaining = len(plotList)
        bDebug = False
        for plot in plotList:
            i = GetHmIndex(plot.x,plot.y)
            if bDebug:
                print "rainplot at %d,%d" % (plot.x,plot.y)
                print "order = %f" % (plot.order)
                print "initial moisture = %f" % (self.moistureMap[i])
            #First collect moisture from sea
            if hm.isBelowSeaLevel(plot.x,plot.y):
                self.moistureMap[i] += tempMap[i]
                if bDebug:
                    print "collecting %f moisture from sea" % (tempMap[i])
                    
            nList = list()
            if bGeostrophic:
                #make list of neighbors in geostrophic zone, even if off map
                zone = windZones.GetZone(plot.y)
                dx,dy = windZones.GetWindDirectionsInZone(zone)
                if bDebug:
                    if dy < 0:
                        yString = "v"
                    else:
                        yString = "^"
                    if dx < 0:
                        xString = "<"
                    else:
                        xString = ">"
                    print "Wind direction ------------------------------- %s%s - %s" % (xString,yString,windZones.GetZoneName(zone))
                nList.append((plot.x,plot.y + dy))
                nList.append((plot.x + dx,plot.y))
                nList.append((plot.x + dx,plot.y + dy))
                
            else:
                #make list of neighbors with higher temp
                for direction in range(1,9,1):
                    xx,yy = GetXYFromDirection(plot.x,plot.y,direction)
                    ii = GetHmIndex(xx,yy)
                    if ii != -1 and tempMap[i] <= tempMap[ii]:
                        nList.append((xx,yy))
                #divide moisture by number of neighbors for distribution
                if len(nList) == 0:
                    continue #dead end, dump appropriate rain
            moisturePerNeighbor = self.moistureMap[i]/float(len(nList))
            if bDebug:
                print "moisturePerNeighbor = %f for %d neighbors" % (moisturePerNeighbor,len(nList))

            geostrophicFactor = 1.0
            if bGeostrophic:
                geostrophicFactor = mc.geostrophicFactor
            for xx,yy in nList:
                ii = GetHmIndex(xx,yy)
                if bDebug:
                    print "  neighbor %d,%d" % (xx,yy)
                    print "  countRemaining = %d" % countRemaining
                #Get the rain cost to enter this plot. Cost is
                #percentage of present moisture available for this
                #neighbor
                if bGeostrophic:
                    cost = self.getRainCost(plot.x,plot.y,xx,yy,plot.uplift)
                else:
                    cost = self.getRainCost(plot.x,plot.y,xx,yy,countRemaining)
                    
                if bDebug:
                    print "  rain cost = %f" % cost

                #Convert moisture into rain
                #self.moistureMap[i] -= cost * moisturePerNeighbor (this line is unecessary actually, we are finished with moisture map for this plot) 
                self.rainFallMap[i] += cost * moisturePerNeighbor * geostrophicFactor #geostrophicFactor is not involved with moisture, only to weigh against monsoons
                if bDebug:
                    print "  dropping %f rain here" % (cost * moisturePerNeighbor)

                #send remaining moisture to neighbor
                if ii != -1:
                    self.moistureMap[ii] += moisturePerNeighbor - (cost * moisturePerNeighbor)
                    if bDebug:
                        print "  remaining moisture to neighbor = %f" % (moisturePerNeighbor - (cost * moisturePerNeighbor))

            if bDebug:
                print "total rainfall = %f" % self.rainFallMap[i]
            countRemaining -= 1
            
    def getRainCost(self,x1,y1,x2,y2,distanceToUplift):
        cost = mc.minimumRainCost
        cRange = 1.0 - mc.minimumRainCost/1.0#We don't want to go over 1.0 so the range is reduced
        upliftCost = (1.0/(float(distanceToUplift) + 1.0)) * cRange
        cost += max((hm.getAltitudeAboveSeaLevel(x2,y2) - hm.getAltitudeAboveSeaLevel(x1,y1)) * 2.0 * cRange,upliftCost)
        return cost
            
    def initializeTempMap(self,tempMap,tropic):
        for y in range(mc.hmHeight):
            for x in range(mc.hmWidth):
                tempMap.append(self.getInitialTemp(x,y,tropic))
        return
    def getInitialTemp(self,x,y,tropic):
        i = GetHmIndex(x,y)
        lat = self.getLattitude(y)
        latRange = float(90 + abs(tropic))
        latDifference = abs(float(lat - tropic))
        aboveSeaLevel = hm.heightMap[i] > hm.seaLevel
        if aboveSeaLevel:
            tempPerLatChange = 1.0/latRange
            temp = 1.0 - (tempPerLatChange * latDifference)
        else:
            tempPerLatChange = (1.0 - (2.0*mc.oceanTempClamp))/latRange
            temp = 1.0 - mc.oceanTempClamp - (tempPerLatChange * latDifference)

        return temp

    def getLattitude(self,y):
        latitudeRange = mc.topLattitude - mc.bottomLattitude
        degreesPerDY = float(latitudeRange)/float(mc.hmHeight)
        # TAC - Map scripts - koma13 - START
        #latitude = (mc.topLattitude - (int(round(float(y)* degreesPerDY)))) * -1
        latitude = (mc.bottomLattitude + (int(round(float(y) * degreesPerDY)))) * -1
        # TAC - Map scripts - koma13 - END
        return latitude
    
    def printRainFallMap(self,bOcean):
        lineString = "Rainfall Map"
        print lineString
        wz = WindZones(mc.hmHeight,mc.topLattitude,mc.bottomLattitude)
        for y in range(mc.hmHeight - 1,-1,-1):
            lineString = ""
            for x in range(0,mc.hmWidth,1):
                i = GetHmIndex(x,y)
                if bOcean:
                    mapLoc = int(self.rainFallMap[i] * 10)
                    lineString += chr(mapLoc + 48)
                else:
                    if hm.isBelowSeaLevel(x,y):
                        lineString += '.'
                    elif self.rainFallMap[i] < 0.00001:
                        lineString += 'X'
                    else:
                        mapLoc = int(self.rainFallMap[i] * 60)
                        lineString += chr(mapLoc + 48)
            z = wz.GetZone(y)
            dx,dy = wz.GetWindDirectionsInZone(z)
            lineString += ' - '
            if dx < 0:
                lineString += '<'
            else:
                lineString += '>'
            if dy < 0:
                lineString += 'v'
            else:
                lineString += '^'
            lineString += ' ' + wz.GetZoneName(z)
            print lineString
        lineString = " "
        print lineString
    def printTempMap(self,tempMap):
        lineString = "Temp Map"
        print lineString
        for y in range(mc.hmHeight - 1,-1,-1):
            lineString = ""
            for x in range(0,mc.hmWidth,1):
                i = GetHmIndex(x,y)
                mapLoc = int(tempMap[i] * 10)
                lineString += chr(mapLoc + 48)
            print lineString
        lineString = " "
        print lineString
        
class RainPlot :
    def __init__(self,x,y,order,uplift):
        self.x = x
        self.y = y
        self.order = order
        self.uplift = uplift
        
class WindZones :
    def __init__(self,mapHeight,topLat,botLat):
        self.NPOLAR = 0
        self.NTEMPERATE = 1
        self.NEQUATOR = 2
        self.SEQUATOR = 3
        self.STEMPERATE = 4
        self.SPOLAR = 5
        self.NOZONE = 99
        self.mapHeight = mapHeight
        self.topLat = topLat
        self.botLat = botLat
    def GetZone(self,y):
        if y < 0 or y >= self.mapHeight:
            return self.NOZONE
        lat = self.GetLatitude(y)
        if lat > mc.polarFrontLattitude:
            return self.NPOLAR
        elif lat > mc.horseLattitude:
            return self.NTEMPERATE
        elif lat > 0:
            return self.NEQUATOR
        elif lat > -mc.horseLattitude:
            return self.SEQUATOR
        elif lat > -mc.polarFrontLattitude:
            return self.STEMPERATE
        else:
            return self.SPOLAR
        return
    def GetZoneName(self,zone):
        if zone == self.NPOLAR:
            return "NPOLAR"
        elif zone == self.NTEMPERATE:
            return "NTEMPERATE"
        elif zone == self.NEQUATOR:
            return "NEQUATOR"
        elif zone == self.SEQUATOR:
            return "SEQUATOR"
        elif zone == self.STEMPERATE:
            return "STEMPERATE"
        else:
            return "SPOLAR"
        return
    def GetYFromZone(self,zone,bTop):
        if bTop:
            for y in range(mc.hmHeight - 1,-1,-1):
                if zone == self.GetZone(y):
                    return y
        else:
            for y in range(mc.hmHeight):
                if zone == self.GetZone(y):
                    return y
        return -1
    def GetZoneSize(self):
        latitudeRange = self.topLat - self.botLat
        degreesPerDY = float(latitudeRange)/float(self.mapHeight)
        size = 30.0/degreesPerDY
        return size
    def GetLatitude(self,y):
        latitudeRange = self.topLat - self.botLat
        degreesPerDY = float(latitudeRange)/float(self.mapHeight)
        # TAC - Map scripts - koma13 - START
        #latitude = (self.topLat - (int(round(float(y)* degreesPerDY)))) * -1
        latitude = (self.botLat + (int(round(float(y)* degreesPerDY)))) * -1
        # TAC - Map scripts - koma13 - END        
        return latitude
    def GetWindDirections(self,y):
        z = self.GetZone(y)
        #get x,y directions
        return self.GetWindDirectionsInZone(z)
    def GetWindDirectionsInZone(self,z):
        #get x,y directions
        if z == self.NPOLAR:
            return (-1,-1)
        elif z == self.NTEMPERATE:
            return (1,1)
        elif z == self.NEQUATOR:
            return (-1,-1)
        elif z == self.SEQUATOR:
            return (-1,1)
        elif z == self.STEMPERATE:
            return (1,-1)
        elif z == self.SPOLAR:
            return (-1,1)
        return (0,0)

def isSmallWaterMatch(x,y):
    return sm.isBelowSeaLevel(x,y)

class SmallMaps :
    def __init__(self):
        return
    def initialize(self):
        self.cropMaps()
        newHeightMap = ShrinkMap(hm.heightMap,mc.hmWidth ,mc.hmHeight,mc.width - (mc.distanceToEurope * 2),mc.height)
        newRainFallMap = ShrinkMap(cm.rainFallMap,mc.hmWidth,mc.hmHeight,mc.width - (mc.distanceToEurope * 2),mc.height)
        newAverageTempMap = ShrinkMap(cm.averageTempMap,mc.hmWidth,mc.hmHeight,mc.width - (mc.distanceToEurope * 2),mc.height)

        self.heightMap = array('d')
        self.rainFallMap = array('d')
        self.averageTempMap = array('d')

        for y in range(mc.height):
            for x in range(mc.width):
                oldX = x - mc.distanceToEurope
                i = GetIndexGeneral(oldX,y,mc.width - mc.distanceToEurope * 2,mc.height)
                if i != -1:
                    self.heightMap.append(newHeightMap[i])
                    self.rainFallMap.append(newRainFallMap[i])
                    self.averageTempMap.append(newAverageTempMap[i])
                else:
                    self.heightMap.append(hm.seaLevel - 0.000001)
                    self.rainFallMap.append(0.0)
                    self.averageTempMap.append(0.0)

        #Smooth coasts so there are fewer hills on coast
        for y in range(mc.height):
            for x in range(mc.width):
                if self.isBelowSeaLevel(x,y):
                    i = GetIndex(x,y)
                    self.heightMap[i] = hm.seaLevel - 0.000001
                    
        self.fillInLakes()

        self.createPlotMap()
        self.printPlotMap()
        self.createTerrainMap()

    def fillInLakes(self):
        #smaller lakes need to be filled in again because the map
        #shrinker sometimes creates lakes.
        am = Areamap(mc.width,mc.height,True,True)
        am.defineAreas(isSmallWaterMatch)
##        am.PrintAreaMap()
        oceanID = am.getOceanID()
        for y in range(mc.height):
            for x in range(mc.width):
                i = GetIndex(x,y)
                if self.isBelowSeaLevel(x,y) and am.areaMap[i] != oceanID:
                    #check the size of this body of water, if too small,
                    #change to land
                    for a in am.areaList:
                        if a.ID == am.areaMap[i] and a.size < mc.minInlandSeaSize:
                            self.heightMap[i] = hm.seaLevel
        
        return
    def isBelowSeaLevel(self,x,y):
        i = GetIndex(x,y)
        if self.heightMap[i] < hm.seaLevel:
            return True
        return False
    
    ## This function returns altitude in relation to sea level with
    ## 0.0 being seaLevel and 1.0 being highest altitude
    def getAltitudeAboveSeaLevel(self,x,y):
        i = GetIndex(x,y)
        if i == -1:
            return 0.0
        altitude = self.heightMap[i]
        if altitude < hm.seaLevel:
            return 0.0
        altitude = 1.0/(1.0 - hm.seaLevel) * (altitude - hm.seaLevel)
        return altitude
    

    def createPlotMap(self):
        self.plotMap = array('i')
        #create height difference map to allow for tuning
        diffMap = array('d')
        for i in range(0,mc.height*mc.width):
            diffMap.append(0.0)
        #I tried using a deviation from surrounding average altitude
        #to determine hills and peaks but I didn't like the
        #results. Therefore I an using lowest neighbor
        for y in range(mc.height):
            for x in range(mc.width):
                i = GetIndex(x,y)
                myAlt = self.heightMap[i]
                minAlt = 1.0
                for direction in range(1,9,1):
                    xx,yy = GetXYFromDirection(x,y,direction)
                    ii = GetIndex(xx,yy)
                    if ii == -1:
                        continue
                    if self.heightMap[ii] < minAlt:
                        minAlt = self.heightMap[ii]
                diffMap[i] = myAlt - minAlt

        NormalizeMap(diffMap,mc.width,mc.height)
                    
        peakHeight = FindValueFromPercent(diffMap,mc.width,mc.height,mc.PeakPercent,0.001,True)
        hillHeight = FindValueFromPercent(diffMap,mc.width,mc.height,mc.HillPercent,0.001,True)

        self.plotMap = array('i')
        #initialize map with 0CEAN
        for i in range(0,mc.height*mc.width):
            self.plotMap.append(mc.OCEAN)
        for y in range(mc.height):
            for x in range(mc.width):
                i = GetIndex(x,y)
                altDiff = diffMap[i]
                if self.heightMap[i] < hm.seaLevel:
                    self.plotMap[i] = mc.OCEAN
                elif altDiff < hillHeight:
                    self.plotMap[i] = mc.LAND
                elif altDiff < peakHeight:
                    self.plotMap[i] = mc.HILLS
                else:
                    self.plotMap[i] = mc.PEAK

        #Randomize high altitude areas
        for y in range(mc.height):
            for x in range(mc.width):
                i = GetIndex(x,y)
                if self.plotMap[i] == mc.LAND:
                    randomNum = PRand.random()
                    if randomNum < mc.PeakChanceAtOne * self.getAltitudeAboveSeaLevel(x,y):
                        self.plotMap[i] = mc.PEAK
                    elif randomNum < mc.HillChanceAtOne * self.getAltitudeAboveSeaLevel(x,y):
                        self.plotMap[i] = mc.HILLS

        #break up large clusters of hills and peaks
        for y in range(mc.height):
            for x in range(mc.width):
                i = GetIndex(x,y)
                if self.plotMap == mc.HILLS:
                    allHills = True
                    for direction in range(1,9,1):
                        xx,yy = GetXYFromDirection(x,y,direction)
                        ii = GetIndex(xx,yy)
                        if self.plotMap[ii] != mc.HILLS:
                            allHills = False
                    if allHills == True:
                        self.plotMap[i] = mc.LAND
                if self.plotMap == mc.PEAK:
                    allPeaks = True
                    for direction in range(1,9,1):
                        xx,yy = GetXYFromDirection(x,y,direction)
                        ii = GetIndex(xx,yy)
                        if self.plotMap[ii] != mc.PEAK:
                            allPeaks = False
                    if allPeaks == True:
                        self.plotMap[i] = mc.HILLS
        
        return
    def createTerrainMap(self):
        self.terrainMap = array('i')
        #initialize terrainMap with OCEAN
        for i in range(0,mc.height*mc.width):
            self.terrainMap.append(mc.OCEAN)

        #Find minimum rainfall on land
        minRain = 10.0
        for i in range(mc.width*mc.height):
            if self.plotMap[i] != mc.OCEAN:
                if self.rainFallMap[i] < minRain:
                    minRain = self.rainFallMap[i]
                    
        self.desertThreshold = FindValueFromPercent(self.rainFallMap,mc.width,mc.height,mc.DesertPercent,.0001,False)
        self.plainsThreshold = FindValueFromPercent(self.rainFallMap,mc.width,mc.height,mc.PlainsPercent,.0001,False)
        self.grassThreshold = FindValueFromPercent(self.rainFallMap,mc.width,mc.height,mc.GrassPercent,.0001,False)
        for y in range(mc.height):
            for x in range(mc.width):
                i = GetIndex(x,y)
                if self.plotMap[i] == mc.OCEAN:
                    for direction in range (1,9,1):
                        xx,yy = GetXYFromDirection(x,y,direction)
                        ii = GetIndex(xx,yy)
                        if self.plotMap[ii] != mc.OCEAN:
                            self.terrainMap[i] = mc.COAST

                #instead of harsh thresholds, allow a random deviation chance
                #based on how close to the threshold the rainfall is
                elif self.rainFallMap[i] < self.desertThreshold:
                    if self.averageTempMap[i] < mc.SnowTemp:
                        self.terrainMap[i] = mc.SNOW
                    elif self.averageTempMap[i] < mc.TundraTemp:
                        self.terrainMap[i] = mc.TUNDRA
                    else:
                        if self.rainFallMap[i] < (PRand.random() * (self.desertThreshold - minRain) + self.desertThreshold - minRain)/2.0 + minRain:
                            self.terrainMap[i] = mc.DESERT
                        else:
                            if PRand.random() <= 0.5:
                                self.terrainMap[i] = mc.PRAIRIE
                            else:
                                self.terrainMap[i] = mc.PLAINS
                elif self.rainFallMap[i] < self.plainsThreshold:
                    if self.averageTempMap[i] < mc.SnowTemp:
                        self.terrainMap[i] = mc.SNOW
                    elif self.averageTempMap[i] < mc.TundraTemp:
                        self.terrainMap[i] = mc.TUNDRA
                    else:
                        if self.rainFallMap[i] < ((PRand.random() * (self.plainsThreshold - self.desertThreshold) + self.plainsThreshold - self.desertThreshold))/2.0 + self.desertThreshold: 
                            if PRand.random() <= 0.5:
                                self.terrainMap[i] = mc.PRAIRIE
                            else:
                                self.terrainMap[i] = mc.PLAINS
                        else:
                            self.terrainMap[i] = mc.GRASS
                elif self.rainFallMap[i] < self.grassThreshold:
                    if self.averageTempMap[i] < mc.SnowTemp:
                        self.terrainMap[i] = mc.SNOW
                    elif self.averageTempMap[i] < mc.TundraTemp:
                        self.terrainMap[i] = mc.TUNDRA
                    else:
                        if self.rainFallMap[i] < ((PRand.random() * (self.grassThreshold - self.plainsThreshold) + self.grassThreshold - self.plainsThreshold))/2.0 + self.plainsThreshold: 
                            self.terrainMap[i] = mc.GRASS
                        else:
                            iRandTerrain = PRand.random()
                            if iRandTerrain <= 0.3: 
                                self.terrainMap[i] = mc.GRASS
                            else:
                                self.terrainMap[i] = mc.SAVANNAH
                else:
                    if self.averageTempMap[i] < mc.SnowTemp:
                        self.terrainMap[i] = mc.SNOW
                    elif self.averageTempMap[i] < mc.TundraTemp:
                        self.terrainMap[i] = mc.TUNDRA
                    elif self.plotMap[i] != mc.LAND:
                        self.terrainMap[i] = mc.GRASS
                    else:
                        if self.averageTempMap[i] > (PRand.random() * (mc.alwaysMarshTemp - mc.MinMarshTemp)) + mc.MinMarshTemp:
                            #ray Savannah
                            # Chose Terrain
                            iRandTerrain = PRand.random()
                            if iRandTerrain <= 0.7: 
                                self.terrainMap[i] = mc.MARSH
                            else:
                                self.terrainMap[i] = mc.SAVANNAH
                            # Generate some Hills
                            iRandPlotType = PRand.random()
                            if iRandPlotType <= 0.3: 
                                self.plotMap[i] = mc.HILLS
                            #ray Savannah End
                        else:
                            iRandTerrain = PRand.random()
                            if iRandTerrain <= 0.2: 
                                self.terrainMap[i] = mc.GRASS
                            elif iRandTerrain <= 0.4: 
                                self.terrainMap[i] = mc.PLAINS
                            else:
                                self.terrainMap[i] = mc.SAVANNAH
                            # Generate some Hills
                            iRandPlotType = PRand.random()
                            if iRandPlotType <= 0.3: 
                                self.plotMap[i] = mc.HILLS

        #Make sure ice is always higher than tundra, and tundra is always higher than
        #everything else
        for y in range(mc.height):
            for x in range(mc.width):
                i = GetIndex(x,y)
                if self.terrainMap[i] == mc.OCEAN or self.terrainMap[i] == mc.COAST:
                    continue
                if self.terrainMap[i] == mc.SNOW:
                    lowerFound = False
                    for direction in range(1,9,1):
                        xx,yy = GetXYFromDirection(x,y,direction)
                        ii = GetIndex(xx,yy)
                        if self.terrainMap[ii] != mc.OCEAN and self.terrainMap[ii] != mc.COAST \
                        and self.terrainMap[ii] != mc.SNOW:
                            lowerFound = True
                    if lowerFound and self.plotMap[i] == mc.LAND:
                        self.plotMap[i] = mc.HILLS
                elif self.terrainMap[i] == mc.TUNDRA:
                    lowerFound = False
                    for direction in range(1,9,1):
                        xx,yy = GetXYFromDirection(x,y,direction)
                        ii = GetIndex(xx,yy)
                        if self.terrainMap[ii] != mc.OCEAN and self.terrainMap[ii] != mc.COAST \
                        and self.terrainMap[ii] != mc.SNOW and self.terrainMap[ii] != mc.TUNDRA:
                            lowerFound = True
                    if lowerFound and self.plotMap[i] == mc.LAND:
                        if PRand.random() < 0.5:
                            self.plotMap[i] = mc.HILLS
                        else:
                            self.plotMap[i] = mc.PEAK
                else:
                    higherFound = False
                    for direction in range(1,9,1):
                        xx,yy = GetXYFromDirection(x,y,direction)
                        ii = GetIndex(xx,yy)
                        if self.terrainMap[ii] == mc.SNOW or self.terrainMap[ii] == mc.TUNDRA:
                            higherFound = True
                    if higherFound and self.plotMap[i] != mc.LAND:
                        self.plotMap[i] = mc.LAND
        return
    def cropMaps(self):
        hm.heightMap = CropMap(hm.heightMap)
        cm.averageTempMap = CropMap(cm.averageTempMap)
        cm.rainFallMap = CropMap(cm.rainFallMap)
        mc.hmWidth = mc.hmWidth - mc.westCrop - mc.eastCrop
        mc.hmHeight = mc.hmHeight - mc.northCrop - mc.southCrop

    def printHeightMap(self):
        lineString = "Height Map"
        print lineString
        for y in range(mc.height - 1,-1,-1):
            lineString = ""
            for x in range(0,mc.width,1):
                i = GetIndexGeneral(x,y,mc.width,mc.height)
                mapLoc = int((self.heightMap[i] - hm.seaLevel)/(1.0 - hm.seaLevel) * 10)
                #mapLoc = int(self.heightMap[i] * 10)
                if self.heightMap[i] < hm.seaLevel:
                    lineString += '.'
                else:
                    lineString += chr(mapLoc + 48)
            print lineString
        lineString = " "
        print lineString
        
    def printPlotMap(self):
        print "Plot Map"
        for y in range(mc.height - 1,-1,-1):
            lineString = ""
            for x in range(mc.width):
                mapLoc = self.plotMap[GetIndex(x,y)]
                if mapLoc == mc.PEAK:
                    lineString += 'A'
                elif mapLoc == mc.HILLS:
                    lineString += 'n'
                elif mapLoc == mc.LAND:
                    lineString += '+'
                else:
                    lineString += '.'
            print lineString
        lineString = " "
        print lineString
    def printTerrainMap(self):
        print "Terrain Map"
        wz = WindZones(mc.height,80,-80)
        for y in range(mc.height - 1,-1,-1):
            lineString = ""
            for x in range(mc.width):
                mapLoc = self.terrainMap[GetIndex(x,y)]
                if mapLoc == mc.OCEAN:
                    lineString += ','
                elif mapLoc == mc.COAST:
                    lineString += '.'
                elif mapLoc == mc.DESERT:
                    lineString += 'D'
                elif mapLoc == mc.GRASS:
                    lineString += '+'
                elif mapLoc == mc.MARSH:
                    lineString += 'M'
                elif mapLoc == mc.PRAIRIE:
                    lineString += 'P'
                elif mapLoc == mc.TUNDRA:
                    lineString += 'T'
                elif mapLoc == mc.SNOW:
                    lineString += 'I'
                elif mapLoc == mc.PLAINS:
                    lineString += 'L'    
            lineString += "-" + wz.GetZoneName(wz.GetZone(y))
            print lineString
        lineString = " "
        print lineString
        
class Areamap :
    def __init__(self,width,height,b8connected,bSwitch4Or8OnFalseMatch):
        self.mapWidth = width
        self.mapHeight = height
        self.areaMap = array('i')
        self.b8connected = b8connected
        self.bSwitch4Or8OnFalseMatch = bSwitch4Or8OnFalseMatch
        #initialize map with zeros
        for i in range(0,self.mapHeight*self.mapWidth):
            self.areaMap.append(0)
        return
    def defineAreas(self,matchFunction):
        #coastIsLand = True means that we are trying to find continents that
        #are not connected by coasts to the main landmasses, allowing us to
        #find continents suitable as a 'New World'. Otherwise, we
        #are just looking to fill in lakes and coast needs to be considered
        #water in that case
#        self.areaSizes = array('i')
##        starttime = time.clock()
        self.areaList = list()
        areaID = 0
        #make sure map is erased in case it is used multiple times
        for i in range(0,self.mapHeight*self.mapWidth):
            self.areaMap[i] = 0
#        for i in range(0,1):
        for i in range(0,self.mapHeight*self.mapWidth):
            if self.areaMap[i] == 0: #not assigned to an area yet
                areaID += 1
                areaSize,match = self.fillArea(i,areaID,matchFunction)
                area = Area(areaID,areaSize,match)
                self.areaList.append(area)

##        endtime = time.clock()
##        elapsed = endtime - starttime
##        print "defineAreas time ="
##        print elapsed
##        print

        return

    def isWater(self,x,y,coastIsLand):
        #coastIsLand = True means that we are trying to find continents that
        #are not connected by coasts to the main landmasses, allowing us to
        #find continents suitable as a 'New World'. Otherwise, we
        #are just looking to fill in lakes and coast needs to be considered
        #water in that case
        ii = self.getIndex(x,y)
        if ii == -1:
            return False
       # if coastIsLand:
       #     if hm.plotMap[ii] == hm.OCEAN and terr.terrainMap[ii] != terr.COAST:
       #         return True
       #     else:
       #         return False
        #else:
        if hm.isBelowSeaLevel(x,y):
            return True
        else:
            return False
            
        return False
    def getAreaByID(self,areaID):
        for i in range(len(self.areaList)):
            if self.areaList[i].ID == areaID:
                return self.areaList[i]
        return None
    def getOceanID(self):
#        self.areaList.sort(key=operator.attrgetter('size'),reverse=True)
        self.areaList.sort(lambda x,y:cmp(x.size,y.size))
        self.areaList.reverse()
        for a in self.areaList:
            if a.water == True:
                return a.ID
            
    def getContinentCenter(self,ID):
        #first find center in x direction
        changes = list()
        yMin = hm.mapHeight
        yMax = -1
        meridianOverlap = False
        onContinent = False
        for x in range(hm.mapWidth):
            continentFoundThisPass = False
            for y in range(hm.mapHeight):
                i = self.getIndex(x,y)
                if self.areaMap[i] == ID:
                    continentFoundThisPass = True
                    if y < yMin:
                        yMin = y
                    elif y > yMax:
                        yMax = y
            if x == 0 and continentFoundThisPass:
                meridianOverlap = True
                onContinent = True
            if onContinent and not continentFoundThisPass:
                changes.append(x)
                onContinent = False
            elif not onContinent and continentFoundThisPass:
                changes.append(x)
                onContinent = True
        changes.sort()
        xCenter = -1
        if len(changes) == 0: #continent is continuous
            xCenter = -1
        elif len(changes) == 1:#continent extends to map edge
            if meridianOverlap:
                xCenter = changes[0]/2
            else:
                xCenter = (hm.mapWidth - changes[0])/2 + changes[0]
        else:
            if meridianOverlap:
                xCenter = ((changes[1] - changes[0])/2 + changes[0] + (hm.mapWidth/2)) % hm.mapWidth
            else:
                xCenter = (changes[1] - changes[0])/2 + changes[0]
        yCenter = (yMax - yMin)/2 + yMin
        center = xCenter,yCenter
        return center    

    def isPangea(self):
##        starttime = time.clock()
        continentList = list()
        for a in self.areaList:
            if a.water == False:
                continentList.append(a)

        totalLand = 0             
        for c in continentList:
            totalLand += c.size
            
        #sort all the continents by size, largest first
        continentList.sort(lambda x,y:cmp(x.size,y.size))
        continentList.reverse()
        biggestSize = continentList[0].size
        if 0.70 < float(biggestSize)/float(totalLand):
##            endtime = time.clock()
##            elapsed = endtime - starttime
##            print "isPangea time = %(t)s" % {"t":str(elapsed)}
            return True
##        endtime = time.clock()
##        elapsed = endtime - starttime
##        print "isPangea time = "
##        print elapsed
##        print
        return False
    def getMeteorStrike(self):
##        starttime = time.clock()
        continentList = list()
        for a in self.areaList:
            if a.water == False:
                continentList.append(a)
            
        #sort all the continents by size, largest first
        continentList.sort(lambda x,y:cmp(x.size,y.size))
        continentList.reverse()
        biggestContinentID = continentList[0].ID
        
        chokeList = list()
        for y in range(hm.mapHeight):
            for x in range(hm.mapWidth):
                i = self.getIndex(x,y)
                if self.areaMap[i] == biggestContinentID:
                    if self.isChokePoint(x,y):
                        ap = AreaPlot(x,y)
                        chokeList.append(ap)
        #calculate distances to center
        center = self.getContinentCenter(biggestContinentID)
        xCenter,yCenter = center
        for n in range(len(chokeList)):
            distance = self.getDistance(chokeList[n].x,chokeList[n].y,xCenter,yCenter)
            chokeList[n].avgDistance = distance
            
        #sort plotList for most avg distance and chokeList for least
        #average distance
        chokeList.sort(lambda x,y:cmp(x.avgDistance,y.avgDistance))

        if len(chokeList) == 0:#return bad value if no chokepoints
##            endtime = time.clock()
##            elapsed = endtime - starttime
##            print "getMeteorStrike time = "
##            print elapsed
##            print
            return -1,-1

##        endtime = time.clock()
##        elapsed = endtime - starttime
##        print "getMeteorStrike time = "
##        print elapsed
##        print
        
        return chokeList[0].x,chokeList[0].y
                
    def isChokePoint(self,x,y):
        circlePoints = self.getCirclePoints(x,y,4)
        waterOpposite = False
        landOpposite = False
        for cp in circlePoints:
            if self.isWater(cp.x,cp.y,True):
                #Find opposite
                ox = x + (x - cp.x)
                oy = y + (y - cp.y)
                if self.isWater(ox,oy,True):
                    waterOpposite = True
            else:
                #Find opposite
                ox = x + (x - cp.x)
                oy = y + (y - cp.y)
                if not self.isWater(ox,oy,True):
                    landOpposite = True
        if landOpposite and waterOpposite:
            return True
        return False
    def getDistance(self,x,y,dx,dy):
        xx = x - dx
        if abs(xx) > hm.mapWidth/2:
            xx = hm.mapWidth - abs(xx)
            
        distance = max(abs(xx),abs(y - dy))
        return distance
        
    def getNewWorldID(self):
        nID = 0
        continentList = list()
        for a in self.areaList:
            if a.water == False:
                continentList.append(a)

        totalLand = 0             
        for c in continentList:
            totalLand += c.size
            
        print totalLand

        #sort all the continents by size, largest first
#        continentList.sort(key=operator.attrgetter('size'),reverse=True)
        continentList.sort(lambda x,y:cmp(x.size,y.size))
        continentList.reverse()
        
        print ''
        print "All continents"
        print self.PrintList(continentList)

        #now remove a percentage of the landmass to be considered 'Old World'
        oldWorldSize = 0
        #biggest continent is automatically 'Old World'
        oldWorldSize += continentList[0].size
        del continentList[0]

        #get the next largest continent and temporarily remove from list
        #add it back later and is automatically 'New World'
        biggestNewWorld = continentList[0]
        del continentList[0]
        
        #sort list by ID rather than size to make things
        #interesting and possibly bigger new worlds
#        continentList.sort(key=operator.attrgetter('ID'),reverse=True)
        continentList.sort(lambda x,y:cmp(x.ID,y.ID))
        continentList.reverse()
        
        for n in range(len(continentList)):
            oldWorldSize += continentList[0].size
            del continentList[0]
            if float(oldWorldSize)/float(totalLand) > 0.60:
                break

        #add back the biggestNewWorld continent
        continentList.append(biggestNewWorld)
        
        #what remains in the list will be considered 'New World'
        print ''
        print "New World Continents"
        print self.PrintList(continentList)

        #get ID for the next continent, we will use this ID for 'New World'
        #designation
        nID = continentList[0].ID
        del continentList[0] #delete to avoid unnecessary overwrite

        #now change all the remaining continents to also have nID as their ID
        for i in range(self.mapHeight*self.mapWidth):
            for c in continentList:
                if c.ID == self.areaMap[i]:
                    self.areaMap[i] = nID
 
        return nID
            
    def getIndex(self,x,y):
        #Check X for wrap
        if mc.WrapX == True:
            xx = x % self.mapWidth
        elif x < 0 or x >= self.mapWidth:
            return -1
        else:
            xx = x
        #Check y for wrap
        if mc.WrapY == True:
            yy = y % self.mapHeight
        elif y < 0 or y >= self.mapHeight:
            return -1
        else:
            yy = y

        i = yy * self.mapWidth + xx
        return i
    
    def fillArea(self,index,areaID,matchFunction):
        #first divide index into x and y
        y = index/self.mapWidth
        x = index%self.mapWidth
        #We check 8 neigbors for land,but 4 for water. This is because
        #the game connects land squares diagonally across water, but
        #water squares are not passable diagonally across land
        self.segStack = list()
        self.size = 0
        matchValue = matchFunction(x,y)
        #place seed on stack for both directions
        seg = LineSegment(y,x,x,1)
        self.segStack.append(seg) 
        seg = LineSegment(y+1,x,x,-1)
        self.segStack.append(seg) 
        while(len(self.segStack) > 0):
            seg = self.segStack.pop()
            self.scanAndFillLine(seg,areaID,matchValue,matchFunction)
##            if (seg.y < 8 and seg.y > 4) or (seg.y < 70 and seg.y > 64):
##            if (areaID == 4
##                PrintPlotMap(hm)
##                self.PrintAreaMap()
        
        return self.size,matchFunction(x,y)
    def scanAndFillLine(self,seg,areaID,matchValue,matchFunction):
        #check for y + dy being off map
        i = self.getIndex(seg.xLeft,seg.y + seg.dy)
        if i < 0:
##            print "scanLine off map ignoring",str(seg)
            return
        debugReport = False
##        if (seg.y < 8 and seg.y > 4) or (seg.y < 70 and seg.y > 64):
##        if (areaID == 4):
##            debugReport = True
        #for land tiles we must look one past the x extents to include
        #8-connected neighbors
        if self.b8connected:
            if self.bSwitch4Or8OnFalseMatch and matchValue:
                landOffset = 0
            else:
                landOffset = 1
        else:
            if self.bSwitch4Or8OnFalseMatch and matchValue:
                landOffset = 1
            else:
                landOffset = 0
        
        lineFound = False
        #first scan and fill any left overhang
        if debugReport:
            print ""
            print "areaID = %(a)4d" % {"a":areaID}
            print "matchValue = %(w)2d, landOffset = %(l)2d" % {"w":matchValue,"l":landOffset} 
            print str(seg)
            print "Going left"
        if mc.WrapX == True:
            xStop = 0 - (self.mapWidth*20)
        else:
            xStop = -1
        for xLeftExtreme in range(seg.xLeft - landOffset,xStop,-1):
            i = self.getIndex(xLeftExtreme,seg.y + seg.dy)
            if debugReport:
                print "xLeftExtreme = %(xl)4d" % {'xl':xLeftExtreme}
            if debugReport:
                print "i = %d, seg.y + seg.dy = %d" % (i,seg.y + seg.dy)
                print "areaMap[i] = %d, matchValue match = %d" % (self.areaMap[i],matchValue == matchFunction(xLeftExtreme,seg.y + seg.dy))
            if self.areaMap[i] == 0 and matchValue == matchFunction(xLeftExtreme,seg.y + seg.dy):
                self.areaMap[i] = areaID
                self.size += 1
                lineFound = True
            else:
                #if no line was found, then xLeftExtreme is fine, but if
                #a line was found going left, then we need to increment
                #xLeftExtreme to represent the inclusive end of the line
                if lineFound:
                    xLeftExtreme += 1
                break
        if debugReport:
            print "xLeftExtreme finally = %(xl)4d" % {'xl':xLeftExtreme}
            print "Going Right"
        #now scan right to find extreme right, place each found segment on stack
#        xRightExtreme = seg.xLeft - landOffset #needed sometimes? one time it was not initialized before use.
        xRightExtreme = seg.xLeft #needed sometimes? one time it was not initialized before use.
        if mc.WrapX == True:
            xStop = self.mapWidth*20
        else:
            xStop = self.mapWidth
        for xRightExtreme in range(seg.xLeft + lineFound - landOffset,xStop,1):
            if debugReport:            
                print "xRightExtreme = %(xr)4d" % {'xr':xRightExtreme}
            i = self.getIndex(xRightExtreme,seg.y + seg.dy)
            if debugReport:
                print "i = %d, seg.y + seg.dy = %d" % (i,seg.y + seg.dy)
                print "areaMap[i] = %d, matchValue match = %d" % (self.areaMap[i],matchValue == matchFunction(xRightExtreme,seg.y + seg.dy))
            if self.areaMap[i] == 0 and matchValue == matchFunction(xRightExtreme,seg.y + seg.dy):
                self.areaMap[i] = areaID
                self.size += 1
                if lineFound == False:
                    lineFound = True
                    xLeftExtreme = xRightExtreme #starting new line
                    if debugReport:
                        print "starting new line at xLeftExtreme= %(xl)4d" % {'xl':xLeftExtreme}
            elif lineFound == True: #found the right end of a line segment!                
                lineFound = False
                #put same direction on stack
                newSeg = LineSegment(seg.y + seg.dy,xLeftExtreme,xRightExtreme - 1,seg.dy)
                self.segStack.append(newSeg)
                if debugReport:
                    print "same direction to stack",str(newSeg)
                #determine if we must put reverse direction on stack
                if xLeftExtreme < seg.xLeft or xRightExtreme >= seg.xRight:
                    #out of shadow so put reverse direction on stack also
                    newSeg = LineSegment(seg.y + seg.dy,xLeftExtreme,xRightExtreme - 1,-seg.dy)
                    self.segStack.append(newSeg)
                    if debugReport:
                        print "opposite direction to stack",str(newSeg)
                if xRightExtreme >= seg.xRight + landOffset:
                    if debugReport:
                        print "finished with line"
                    break; #past the end of the parent line and this line ends
            elif lineFound == False and xRightExtreme >= seg.xRight + landOffset:
                if debugReport:
                    print "no additional lines found"
                break; #past the end of the parent line and no line found
            else:
                continue #keep looking for more line segments
        if lineFound == True: #still a line needing to be put on stack
            if debugReport:
                print "still needing to stack some segs"
            lineFound = False
            #put same direction on stack
            newSeg = LineSegment(seg.y + seg.dy,xLeftExtreme,xRightExtreme - 1,seg.dy)
            self.segStack.append(newSeg)
            if debugReport:
                print str(newSeg)
            #determine if we must put reverse direction on stack
            if xLeftExtreme < seg.xLeft or xRightExtreme - 1 > seg.xRight:
                #out of shadow so put reverse direction on stack also
                newSeg = LineSegment(seg.y + seg.dy,xLeftExtreme,xRightExtreme - 1,-seg.dy)
                self.segStack.append(newSeg)
                if debugReport:
                    print str(newSeg)
        
        return
    #for debugging
    def PrintAreaMap(self):
        
        print "Area Map"
        for y in range(self.mapHeight - 1,-1,-1):
            lineString = ""
            for x in range(self.mapWidth):
                mapLoc = self.areaMap[self.getIndex(x,y)]
                if mapLoc + 34 > 127:
                    mapLoc = 127 - 34
                lineString += chr(mapLoc + 34)
            lineString += "-" + str(y)
            print lineString
        oid = self.getOceanID()
        if oid == None or oid + 34 > 255:
            print "Ocean ID is unknown"
        else:
            print "Ocean ID is %(oid)4d or %(c)s" % {'oid':oid,'c':chr(oid + 34)}
        lineString = " "
        print lineString

        return
    def PrintList(self,s):
        for a in s:
            char = chr(a.ID + 34)
            lineString = str(a) + ' ' + char
            print lineString
            
    def getCirclePoints(self,xCenter,yCenter,radius):
        circlePointList = list()
        x = 0
        y = radius
        p = 1 - radius

        self.addCirclePoints(xCenter,yCenter,x,y,circlePointList)

        while (x < y):
            x += 1
            if p < 0:
                p += 2*x + 1
            else:
                y -= 1
                p += 2*(x - y) + 1
            self.addCirclePoints(xCenter,yCenter,x,y,circlePointList)
            
        return circlePointList
    
    def addCirclePoints(self,xCenter,yCenter,x,y,circlePointList):
        circlePointList.append(CirclePoint(xCenter + x,yCenter + y))
        circlePointList.append(CirclePoint(xCenter - x,yCenter + y))
        circlePointList.append(CirclePoint(xCenter + x,yCenter - y))
        circlePointList.append(CirclePoint(xCenter - x,yCenter - y))
        circlePointList.append(CirclePoint(xCenter + y,yCenter + x))
        circlePointList.append(CirclePoint(xCenter - y,yCenter + x))
        circlePointList.append(CirclePoint(xCenter + y,yCenter - x))
        circlePointList.append(CirclePoint(xCenter - y,yCenter - x))
        return
class CirclePoint :
    def __init__(self,x,y):
        self.x = x
        self.y = y

class LineSegment :
    def __init__(self,y,xLeft,xRight,dy):
        self.y = y
        self.xLeft = xLeft
        self.xRight = xRight
        self.dy = dy
    def __str__ (self):
        string = "y = %(y)3d, xLeft = %(xl)3d, xRight = %(xr)3d, dy = %(dy)2d" % \
        {'y':self.y,'xl':self.xLeft,'xr':self.xRight,'dy':self.dy}
        return string
                       
class Area :
    def __init__(self,iD,size,water):
        self.ID = iD
        self.size = size
        self.water = water

    def __str__(self):
        string = "{ID = %(i)4d, size = %(s)4d, water = %(w)1d}" % \
        {'i':self.ID,'s':self.size,'w':self.water}
        return string
class AreaPlot :
    def __init__(self,x,y):
        self.x = x
        self.y = y
        self.avgDistance = -1
        
#OK! now that directions N,S,E,W are important, we have to keep in mind that
#the map plots are ordered from 0,0 in the SOUTH west corner! NOT the northwest
#corner! That means that Y increases as you go north.
class RiverMap :
    # Coordinate reminder:
    # there are two coordinate systems: plot coordinates (p) and river coordinates (r)
    # river coordinates are at the four-plot intersections between plot as follows:
    #
    # r(-1,  1) |           | r( 0,  1) |
    # -----------------------------------------------
    #           | p( 0,  0) |           |  p( 1,  0) 
    # -----------------------------------------------
    # r(-1,  0) |           | r( 0,  0) |
    # -----------------------------------------------
    #           | p( 0, -1) |           |  p( 1, -1) 
    
    # flow directions 
    N = 1
    E = 2
    S = 3
    W = 4
    N2 = 5 # large river
    E2 = 6 # large river
    S2 = 7 # large river
    W2 = 8 # large river
    N3 = 9 # lake exit special location / large river source location
    E3 = 10# lake exit special location
    S3 = 11# lake exit special location
    W3 = 12# lake exit special location
    L = 13 #also denotes a 'pit' or 'flat'
    O = 14  #used for ocean or land without a river
    
    NE = 21
    NW = 22
    SE = 23
    SW = 24
    
    # lake maps
    lakeExit     = 0
    lakeToOcean  = -3
    lakeOcean    = -1
    lakeSource   = 1
    lakeSquare   = 2
    lakeExitFrom = 4
    
    turnLeft  = 1
    turnRight = 2
    turn180   = 3
    turnNone  = 4
    
    # river plot design map
    riverOcean =  2
    riverFalse =  0
    riverTrue  =  1
    riverLake  = -1
    
    # flow coordinates; x and y so that y is pointing ahead and x to the right vs the flow direction
    FC = {N:  [[1,0],[0,1]], S:  [[-1,0],[0,-1]], E:  [[0,-1],[1,0]], W:  [[0,1],[-1,0]],
          N2: [[1,0],[0,1]], S2: [[-1,0],[0,-1]], E2: [[0,-1],[1,0]], W2: [[0,1],[-1,0]],
          N3: [[1,0],[0,1]], S3: [[-1,0],[0,-1]], E3: [[0,-1],[1,0]], W3: [[0,1],[-1,0]],
          L:  [[0,0],[0,0]], O:  [[0,0],[0,0]]}
    
    # flow origin difference from corner (river coordinates) into plot coordinates
    # forward left is origin. Reduce the values given here to set the origin to 0,0
    FO_R2P = {N:  [0,0], S:  [1,-1], E:  [1,0], W:  [0,-1],
              N2: [0,0], S2: [1,-1], E2: [1,0], W2: [0,-1],
              N3: [0,0], S3: [1,-1], E3: [1,0], W3: [0,-1],
              L: [0,0], O: [0,0]}
    
    def __init__(self):
        #To provide global access without allocating alot of resources for
        #nothing, object initializer must be empty
        return
    def generateRiverMap(self):
        #averageHeightMap, flowMap, averageRainfallMap and drainageMap are offset from the other maps such that
        #each element coincides with a four tile intersection on the game map
        self.averageHeightMap = array('d')
        self.flowMap = array('i')
        self.flowMap2 = array('i')
        self.flowMap3 = array('i')
        self.averageRainfallMap = array('d')
        self.drainageMap = array('d')
        self.riverMap = array('i')
        self.riverPlotMap = array('i')
        self.riverIDs = array('i')
        self.lakeMap = array('i')
        self.lakeIDs = array('i')
        self.drainageLakeIDs = array('i')
        
        self.lakeExitXYs = {}
        
        lakeID = 1
        #initialize maps with zeros
        for i in range(0,mc.height*mc.width):
            self.averageHeightMap.append(0.0)
            self.flowMap.append(0)
            self.flowMap2.append(0)
            self.averageRainfallMap.append(0.0)
            self.drainageMap.append(0.0)
            self.riverMap.append(self.O)
            self.riverPlotMap.append(self.riverOcean)
            self.lakeMap.append(-1)
            self.lakeIDs.append(-1)
            self.riverIDs.append(-1)
            self.drainageLakeIDs.append(-1)
            
        #Get highest intersection neighbor
        for y in range(mc.height):
            for x in range(mc.width):
                i = GetIndex(x,y)
                heightSum = 0.0
                for yy in range(y,y-2,-1):
                    for xx in range(x,x+2):
                        ii = GetIndex(xx,yy)
                        #use an average hight of <0 to denote an ocean border
                        #this will save processing time later
                        if(sm.plotMap[ii] == mc.OCEAN):
                            heightSum = -100.0
                        elif heightSum >= 0:
                            heightSum += sm.heightMap[ii]
                self.averageHeightMap[i] = heightSum
                if sm.plotMap[i] != mc.OCEAN:
                    self.lakeMap[i] = 0
                    self.lakeIDs[i] = 0
                    self.riverIDs[i] = 0
                    self.riverPlotMap[i] = self.riverFalse
                    self.drainageLakeIDs[i] = 0
        
        #create flowMap pass 1 (no lakes or large rivers pass)
        for y in range(mc.height):
            for x in range(mc.width):
                i = GetIndex(x,y)
                lowestAlt = self.averageHeightMap[i]
                if (lowestAlt < 0.0):
                    #if height is <0 then that means this intersection is
                    #adjacent to an ocean and has no flow
                    self.flowMap[i] = self.O
                else:
                    #First assume this place is lowest, like a 'pit'. Then
                    #for each place that is lower, add it to a list to be
                    #randomly chosen as the drainage path
                    drainList = list()
                    self.flowMap[i] = self.L 
                    
                    # North?
                    alts = [sm.heightMap[GetIndex(x,y+1)], # downstream
                            sm.heightMap[GetIndex(x,y)], # river square
                            sm.heightMap[GetIndex(x,y-1)]] # upstream
                    if alts[2]>alts[0] and alts[2]>=alts[1] and alts[1]>=alts[0]:
                        drainList.append(self.N)
                    
                    # south?
                    alts = [sm.heightMap[GetIndex(x,y-2)], # downstream
                            sm.heightMap[GetIndex(x,y-1)], # river square
                            sm.heightMap[GetIndex(x,y)]] # upstream
                    if alts[2]>alts[0] and alts[2]>=alts[1] and alts[1]>=alts[0]:
                        drainList.append(self.S)
                    
                    # west?
                    alts = [sm.heightMap[GetIndex(x-1,y)], # downstream
                            sm.heightMap[GetIndex(x,y)], # river square
                            sm.heightMap[GetIndex(x+1,y)]] # upstream
                    if alts[2]>alts[0] and alts[2]>=alts[1] and alts[1]>=alts[0]:
                        drainList.append(self.W)
                    
                    # east?
                    alts = [sm.heightMap[GetIndex(x+2,y)], # downstream
                            sm.heightMap[GetIndex(x+1,y)], # river square
                            sm.heightMap[GetIndex(x,y)]] # upstream
                    if alts[2]>alts[0] and alts[2]>=alts[1] and alts[1]>=alts[0]:
                        drainList.append(self.E)
                    
                    count = len(drainList)
                    if count > 0:
                        choice = int(PRand.random()*count)
                        self.flowMap[i] = drainList[choice]
        
        # find lake bottoms and naive fill them
        lakeID = 1
        self.lakes = {}
        for y in range(mc.height):
            for x in range(mc.width):
                if(sm.plotMap[GetIndex(x,y)] == mc.OCEAN):
                    continue
                alts = [sm.heightMap[GetIndex(x,y)],
                        sm.heightMap[GetIndex(x,y+1)],
                        sm.heightMap[GetIndex(x,y-1)],
                        sm.heightMap[GetIndex(x+1,y)],
                        sm.heightMap[GetIndex(x-1,y)]]
                if min(alts) == alts[0]:
                    # pit square found
                    lakeRecord = self.createAndFillLake(x, y, lakeID)
                    if lakeRecord is not None:
                        print("Lake %i was created successfully." %(lakeID))
                        self.lakes[lakeID] = lakeRecord
                        lakeID +=1

        
        # find flow dead-ends and check if there is a lake
        print("")
        print("#################################")
        print("Finding additional lake locations")
        print("#################################")
        print("")
        for y in range(mc.height):
            for x in range(mc.width):
                i = GetIndex(x,y)
                # ignore ocean neighbor locations
                if self.averageHeightMap[i]<0:
                    continue
                lakePresent = False
                best_y = 0
                best_x = 0
                best_now = 9000000
                for yy in range(y,y-2,-1):
                    for xx in range(x,x+2):
                        ii = GetIndex(xx,yy)
                        if sm.heightMap[ii] < best_now:
                            best_now = sm.heightMap[ii]
                            best_x = xx
                            best_y = yy
                        if(self.lakeIDs[ii] > 0):
                            lakePresent = True
                if self.flowMap[i] == self.L and not lakePresent:
                    print("")
                    print("Flow end but no lake nearby at %i %i" % (x,y))
                    lakeRecord = self.createAndFillLake(best_x, best_y, lakeID)
                    if lakeRecord is not None:
                        print("Lake %i was created successfully." %(lakeID))
                        self.lakes[lakeID] = lakeRecord
                        lakeID +=1
        
        # lakes done, check their exit flows
        for k,v in self.lakes.items():
            x = v.exitFromX
            y = v.exitFromY
            
            print("Checking lake %i exit validity" % (k))
            exits = self.checkLakeExit(x,y)
            if len(exits) == 0:
                print("Fixing the exit...")
                self.fixExitOfExistingLake(v)
            else:
                count = len(exits)
                choice = int(PRand.random()*count)
                i = GetIndex(exits[choice][0], exits[choice][1])
                self.flowMap2[i] = exits[choice][2]
                v.exitRiverX = exits[choice][0]
                v.exitRiverY = exits[choice][1]
        
        # copy flowmap2 for the future (lake exit data saved)
        self.flowMap4 = array('i')
        for i in range(mc.height*mc.width):
            self.flowMap4.append(self.flowMap2[i])
        
        # fill the new oceans and link lakes to drainage intersections
        # in addition, place lakes in terrain map
        # prepare the riverPlotMap for convenience
        for y in range(mc.height):
            for x in range(mc.width):
                i = GetIndex(x,y)
                if self.lakeMap[i] == self.lakeToOcean:
                    sm.plotMap[i] = mc.OCEAN
                    sm.terrainMap[i] = mc.COAST
                    self.riverPlotMap[i] = self.riverOcean
                    for yy in range(y,y+2):
                        for xx in range(x,x-2,-1):
                            ii = GetIndex(xx,yy)
                            #use an average hight of <0 to denote an ocean border
                            #this will save processing time later
                            self.averageHeightMap[ii] = -100.0
                lakeID = self.lakeIDs[i]
                if lakeID > 0:
                    sm.terrainMap[i] = mc.LAKE
                    self.riverPlotMap[i] = self.riverLake
                    for yy in range(y,y+2):
                        for xx in range(x,x-2,-1):
                            ii = GetIndex(xx,yy)
                            self.drainageLakeIDs[ii] = lakeID
                # helps to not cause large river stoppage at lake exit
                if self.lakeMap[i] == self.lakeExitFrom:
                    self.riverPlotMap[i] = self.riverFalse
                
        
        #create flowMap pass 2 (lakes exist pass)
        for y in range(mc.height):
            for x in range(mc.width):
                i = GetIndex(x,y)
                lowestAlt = self.averageHeightMap[i]
                if (lowestAlt < 0.0):
                    #if height is <0 then that means this intersection is
                    #adjacent to an ocean and has no flow
                    self.flowMap2[i] = self.O
                else:
                    # handle existing values (lake exits) - keep them
                    if self.flowMap2[i] != 0:
                        continue
                    #First assume this place is lowest, like a 'pit'. Then
                    #for each place that is lower, add it to a list to be
                    #randomly chosen as the drainage path
                    drainList = list()
                    self.flowMap2[i] = self.L
                    
                    # North?
                    alts = [sm.heightMap[GetIndex(x,y+1)], # downstream
                            sm.heightMap[GetIndex(x,y)], # river square
                            sm.heightMap[GetIndex(x,y-1)]] # upstream
                    if alts[2]>alts[0] and alts[2]>=alts[1] and alts[1]>=alts[0]:
                        if not (self.lakeIDs[GetIndex(x,y)] or self.lakeIDs[GetIndex(x+1,y)]):
                            drainList.append(self.N)
                    
                    # south?
                    alts = [sm.heightMap[GetIndex(x,y-2)], # downstream
                            sm.heightMap[GetIndex(x,y-1)], # river square
                            sm.heightMap[GetIndex(x,y)]] # upstream
                    if alts[2]>alts[0] and alts[2]>=alts[1] and alts[1]>=alts[0]:
                        if not (self.lakeIDs[GetIndex(x,y-1)] or self.lakeIDs[GetIndex(x+1,y-1)]):
                            drainList.append(self.S)
                    
                    # west?
                    alts = [sm.heightMap[GetIndex(x-1,y)], # downstream
                            sm.heightMap[GetIndex(x,y)], # river square
                            sm.heightMap[GetIndex(x+1,y)]] # upstream
                    if alts[2]>alts[0] and alts[2]>=alts[1] and alts[1]>=alts[0]:
                        if not (self.lakeIDs[GetIndex(x,y)] or self.lakeIDs[GetIndex(x,y-1)]):
                            drainList.append(self.W)
                    
                    # east?
                    alts = [sm.heightMap[GetIndex(x+2,y)], # downstream
                            sm.heightMap[GetIndex(x+1,y)], # river square
                            sm.heightMap[GetIndex(x,y)]] # upstream
                    if alts[2]>alts[0] and alts[2]>=alts[1] and alts[1]>=alts[0]:
                        if not (self.lakeIDs[GetIndex(x+1,y)] or self.lakeIDs[GetIndex(x+1,y-1)]):
                            drainList.append(self.E)
                    
                    count = len(drainList)
                    if count > 0:
                        choice = int(PRand.random()*count)
                        self.flowMap2[i] = drainList[choice]
                        
        #print(1/0)
        #Create average rainfall map so that each intersection is an average
        #of the rainfall from rm.rainMap
        for y in range(mc.height):
            for x in range(mc.width):
                i = GetIndex(x,y)
                avg = 0.0;
                for yy in range(y,y-2,-1):
                    for xx in range(x,x+2):
                        ii = GetIndex(xx,yy)
                        avg += sm.rainFallMap[ii]
                avg = avg/4.0
                self.averageRainfallMap[i] = avg + 0.03 # encouraging longer rivers
                
        # cleanup copy flowmap3
        for i in self.flowMap2:
            if i >= self.N3 and i <= self.W3:
                self.flowMap3.append(i-8)
            else:
                self.flowMap3.append(i)
                
        #Now use the flowMap as a guide to distribute average rainfall.
        #Wherever the most rainfall ends up is where the rivers will be.
        print "Distributing rainfall"
        for y in range(mc.height):
            for x in range(mc.width):
                i = GetIndex(x,y)
                flow = self.flowMap3[i]
                rainFall = self.averageRainfallMap[i]
                xx = x
                yy = y
                iteration = 0
                while(flow != self.O):
                    iteration += 1
                    
                    xx += self.FC[flow][1][0]
                    yy += self.FC[flow][1][1]
                    # if(flow == self.N):
                    #     yy += 1
                    # elif(flow == self.S):
                    #     yy -= 1
                    # elif(flow == self.W):
                    #     xx -= 1
                    # elif(flow == self.E):
                    #     xx += 1
                    #wrap
                    if(xx < 0):
                        xx = mc.width - 1
                    elif(xx >= mc.width):
                        xx = 0
                    if(yy < 0):
                        yy = mc.height - 1
                    elif(yy >= mc.height):
                        yy = 0
                    # dump rainfall here
                    ii = GetIndex(xx,yy)
                    self.drainageMap[ii] += rainFall
                    # flow next step location: lake "teleports" to its exit
                    if flow == self.L:
                        lakeID = self.drainageLakeIDs[ii]
                        xx = self.lakes[lakeID].exitRiverX
                        yy = self.lakes[lakeID].exitRiverY
                        ii = GetIndex(xx,yy)
                        self.drainageMap[ii] += rainFall
                    
                    flow = self.flowMap3[ii]
                    
                    if iteration > 200:
                        print("Potential infinite loop broken!")
                        break
                    
        #Normalize drainageMap to between 0.0 and 1.0            
#        NormalizeMap(hm.mapWidth,hm.mapHeight,self.drainageMap)
        
        riverThreshold = sm.plainsThreshold * mc.RiverThreshold
        # Belisarius - Large Rivers
        largeRiverThreshold = sm.plainsThreshold * mc.LargeRiverThreshold
        for i in range(mc.height*mc.width):
            if(self.drainageMap[i] > largeRiverThreshold):
                if self.flowMap3[i] >= self.N and self.flowMap3[i] <= self.W:
                    self.riverMap[i] = self.flowMap3[i] + 4
                else:
                    self.riverMap[i] = self.flowMap3[i]
            elif(self.drainageMap[i] > riverThreshold):
##                    riverCount += 1
                self.riverMap[i] = self.flowMap3[i]
            else:
                self.riverMap[i] = self.O
                
        # finding large river sources / counting large rivers
        lrID = 1
        self.largeRivers = {}
        print "Locating large rivers"
        for y in range(mc.height):
            for x in range(mc.width):
                i = GetIndex(x,y)
                if self.riverMap[i] >= self.N2 and self.riverMap[i] <=self.W2:
                    isSource = True
                    cx = 1; cy = 0; ii=GetIndex(x+cx,y+cy)
                    if self.riverMap[ii] == self.W2 or self.riverMap[ii] == self.W3:
                        isSource = False
                    cx = -1; cy = 0; ii=GetIndex(x+cx,y+cy)
                    if self.riverMap[ii] == self.E2 or self.riverMap[ii] == self.E3:
                        isSource = False
                    cx = 0; cy = 1; ii=GetIndex(x+cx,y+cy)
                    if self.riverMap[ii] == self.S2 or self.riverMap[ii] == self.S3:
                        isSource = False
                    cx = 0; cy = -1; ii=GetIndex(x+cx,y+cy)
                    if self.riverMap[ii] == self.N2 or self.riverMap[ii] == self.N3:
                        isSource = False
                    if isSource:
                        self.largeRivers[lrID] = self.LargeRiver(x,y,lrID)
                        self.doRiverData(self.largeRivers[lrID])
                        self.riverMap[i] = self.riverMap[i] + 4
                        lrID += 1
    # Ramstormp long rivers only       
        # river planning in order: lake start first
        for k,v in self.largeRivers.items():
            if v.lakeStart and v.length >= mc.minLargeRiverLength:
                self.planLargeRiver(v)
        # lake end next
           #for k,v in self.largeRivers.items():
           #    if v.lakeEnd and v.side is None:
           #        self.planLargeRiver(v)
        # non-river connecting 
        for k,v in self.largeRivers.items():
           if not v.riverEnd and v.side is None and v.length >= mc.minLargeRiverLength:
                self.planLargeRiver(v)
        # the rest
        for k,v in self.largeRivers.items():
            if v.side is None and v.length >= mc.minLargeRiverLength:
                self.planLargeRiver(v)
        
        # copy river map
        self.riverMap2 = array('i')
        for i in range(mc.height*mc.width):
            self.riverMap2.append(self.riverMap[i])
        
        # large river cleanup
        for y in range(mc.height):
            for x in range(mc.width):
                # resolving 2x2 river blocks (delete extra one)
                if (self.riverPlotMap[GetIndex(x,y)] == self.riverTrue and 
                        self.riverPlotMap[GetIndex(x+1,y)] == self.riverTrue and 
                        self.riverPlotMap[GetIndex(x+1,y+1)] == self.riverTrue and
                        self.riverPlotMap[GetIndex(x,y+1)] == self.riverTrue):
                    # bottomleft ("origin")
                    if (self.riverPlotMap[GetIndex(x,y-1)] != self.riverTrue and 
                            self.riverPlotMap[GetIndex(x-1,y)] != self.riverTrue):
                        self.riverPlotMap[GetIndex(x,y)] = self.riverFalse
                    # topleft
                    elif (self.riverPlotMap[GetIndex(x-1,y+1)] != self.riverTrue and 
                            self.riverPlotMap[GetIndex(x,y+2)] != self.riverTrue):
                        self.riverPlotMap[GetIndex(x,y+1)] = self.riverFalse
                    # topright
                    elif (self.riverPlotMap[GetIndex(x+2,y+1)] != self.riverTrue and 
                            self.riverPlotMap[GetIndex(x+1,y+2)] != self.riverTrue):
                        self.riverPlotMap[GetIndex(x+1,y+1)] = self.riverFalse
                    # bottomright
                    elif (self.riverPlotMap[GetIndex(x+1,y-1)] != self.riverTrue and 
                            self.riverPlotMap[GetIndex(x+2,y)] != self.riverTrue):
                        self.riverPlotMap[GetIndex(x+1,y)] = self.riverFalse
                
                # resolving 2x2 river / lake mixed blocks (delete river)
                if ((self.riverPlotMap[GetIndex(x,y)] == self.riverTrue or self.lakeMap[GetIndex(x,y)] > 0) and 
                        (self.riverPlotMap[GetIndex(x+1,y)] == self.riverTrue or self.lakeMap[GetIndex(x+1,y)] > 0) and 
                        (self.riverPlotMap[GetIndex(x+1,y+1)] == self.riverTrue or self.lakeMap[GetIndex(x+1,y+1)] > 0) and
                        (self.riverPlotMap[GetIndex(x,y+1)] == self.riverTrue or self.lakeMap[GetIndex(x,y+1)] > 0)):
                    # bottomleft ("origin")
                    if (self.riverPlotMap[GetIndex(x,y)] == self.riverTrue and
                            self.riverPlotMap[GetIndex(x,y-1)] != self.riverTrue and 
                            self.riverPlotMap[GetIndex(x-1,y)] != self.riverTrue):
                        self.riverPlotMap[GetIndex(x,y)] = self.riverFalse
                    # topleft
                    elif (self.riverPlotMap[GetIndex(x,y+1)] == self.riverTrue and
                            self.riverPlotMap[GetIndex(x-1,y+1)] != self.riverTrue and 
                            self.riverPlotMap[GetIndex(x,y+2)] != self.riverTrue):
                        self.riverPlotMap[GetIndex(x,y+1)] = self.riverFalse
                    # topright
                    elif (self.riverPlotMap[GetIndex(x+1,y+1)] == self.riverTrue and
                            self.riverPlotMap[GetIndex(x+2,y+1)] != self.riverTrue and 
                            self.riverPlotMap[GetIndex(x+1,y+2)] != self.riverTrue):
                        self.riverPlotMap[GetIndex(x+1,y+1)] = self.riverFalse
                    # bottomright
                    elif (self.riverPlotMap[GetIndex(x+1,y)] == self.riverTrue and
                            self.riverPlotMap[GetIndex(x+1,y-1)] != self.riverTrue and 
                            self.riverPlotMap[GetIndex(x+2,y)] != self.riverTrue):
                        self.riverPlotMap[GetIndex(x+1,y)] = self.riverFalse
        
        # Large rivers done. Placing the plots
        for i in range(mc.height*mc.width):
            if self.riverPlotMap[i] == self.riverTrue:
                sm.terrainMap[i] = mc.LARGE_RIVER
                
        # removing bordering small rivers.
        for y in range(mc.height):
            for x in range(mc.width):
                if self.riverPlotMap[GetIndex(x,y)] == self.riverTrue:
                    for yy in range(y,y+2):
                        for xx in range(x,x-2,-1):
                            self.riverMap2[GetIndex(xx,yy)] = self.O
                
        #at this point river should be in tolerance or close to it
        #riverMap is ready for use
    class LargeRiver:
        def __init__(self, sourceX,sourceY, ID):
            self.sourceX = sourceX
            self.sourceY = sourceY
            self.ID = ID
            
            self.length = 0
            self.lakeStart = False
            self.lakeEnd = False
            self.riverEnd = False
            self.riverPlots = []
            self.riverIs = Set()
            self.side = None
    
    def doRiverData(self, k):
            if self.lakeMap[GetIndex(k.sourceX+1, k.sourceY)] > 0:
                k.lakeStart = True
            if self.lakeMap[GetIndex(k.sourceX+1, k.sourceY-1)] > 0:
                k.lakeStart = True
            if self.lakeMap[GetIndex(k.sourceX, k.sourceY-1)] > 0:
                k.lakeStart = True
            if self.lakeMap[GetIndex(k.sourceX, k.sourceY)] > 0:
                k.lakeStart = True
            
            xx = k.sourceX
            yy = k.sourceY
            while True:
                ii = GetIndex(xx, yy)
                
                riverStatus = self.riverIDs[ii]
                if riverStatus > 0:
                    if riverStatus == k.ID:
                        print("Large river loop! Something went wrong. ")
                    else:
                        print("River %i joins river %i." % (k.ID, riverStatus))
                    k.riverEnd = True
                    break
                
                flowNow = self.riverMap[ii]
                if flowNow >= self.N2 and flowNow <= self.W3:
                    k.riverPlots.append((xx,yy))
                    k.length += 1
                    self.riverIDs[ii] = k.ID
                else:
                    break
                
                xx += self.FC[flowNow][1][0]
                yy += self.FC[flowNow][1][1]
                # if flowNow == self.N2 or flowNow == self.N3:
                #     yy += 1
                # if flowNow == self.S2 or flowNow == self.S3:
                #     yy -= 1
                # if flowNow == self.W2 or flowNow == self.W3:
                #     xx -= 1
                # if flowNow == self.E2 or flowNow == self.E3:
                #     xx += 1
            
            if self.lakeMap[GetIndex(xx, yy)] > 0:
                k.lakeEnd = True
            if self.lakeMap[GetIndex(xx+1, yy)] > 0:
                k.lakeEnd = True
            if self.lakeMap[GetIndex(xx+1, yy-1)] > 0:
                k.lakeEnd = True
            if self.lakeMap[GetIndex(xx, yy-1)] > 0:
                k.lakeEnd = True
                
    def planLargeRiver(self, k):
        idx = 0
        plannedRiverPlots = []
        xx = k.riverPlots[idx][0]
        yy = k.riverPlots[idx][1]
        flow = self.riverMap[GetIndex(xx, yy)]
        side = None
        
        cx = self.FC[flow][1][0]
        cy = self.FC[flow][1][1]
        
        rx = self.FC[flow][0][0]
        ry = self.FC[flow][0][1]
        
        ox = self.FO_R2P[flow][0]
        oy = self.FO_R2P[flow][1]
        # start side selection logic
        
        # checking left side blocks (river / lake / ocean)
        blockL = Set()
        blockR = Set()
        # pattern:
        #  b b
        #  L b
        # >
        blockL.add(self.riverPlotMap[GetIndex(xx+ox-rx, yy+oy-ry)])
        blockL.add(self.riverPlotMap[GetIndex(xx+cx+ox, yy+cy+oy)])
        blockL.add(self.riverPlotMap[GetIndex(xx-rx+ox+cx, yy-ry+oy+cy)])
        
        # right side
        blockR.add(self.riverPlotMap[GetIndex(xx+ox+rx*2, yy+oy+ry*2)])
        blockR.add(self.riverPlotMap[GetIndex(xx+cx+ox+rx*2, yy+cy+oy+ry*2)])
        blockR.add(self.riverPlotMap[GetIndex(xx+rx+ox+cx, yy+ry+oy+cy)])
        
        if self.riverFalse in blockL: blockL.remove(self.riverFalse)
        if self.riverFalse in blockR: blockR.remove(self.riverFalse)
        
        # in a draw, select this (unless no blocks)
        select = None
        if len(blockL) < len(blockR):
            select = 'L'
        if len(blockR) < len(blockL):
            select = 'R'
        
        # for lake starts: choose one with the lake adjacent
        if k.lakeStart:
            Lok = self.lakeMap[GetIndex(xx-cx+ox, yy-cy+oy)] > 0
            Rok = self.lakeMap[GetIndex(xx-cx+rx+ox, yy-cy+ry+oy)] > 0
            # if both ok, avoid trouble by selecting the less blocked side
            if Lok and Rok and select is not None:
                side = select
            else:
                if Lok:
                    side = 'L'
                elif Rok:
                    side = 'R'
        else:
            if k.lakeEnd:
                if self.riverLake in blockL: blockL.remove(self.riverLake)
                if self.riverLake in blockR: blockR.remove(self.riverLake)
            
            elif k.riverEnd:
                if self.riverTrue in blockL: blockL.remove(self.riverTrue)
                if self.riverTrue in blockR: blockR.remove(self.riverTrue)
            else:
                # ocean end
                if self.riverOcean in blockL: blockL.remove(self.riverOcean)
                if self.riverOcean in blockR: blockR.remove(self.riverOcean)
            
            if len(blockL)==0 and len(blockR)>0:
                side = 'L'
            elif len(blockR)==0 and len(blockL)>0:
                side = 'R'
            
            if side is None and select is not None:
                side = select
        
        # if there was no logical reason to start either side, randomize
        if side is None:
            choice = int(PRand.random()*2)
            if choice == 0:
                side = 'L'
            else:
                side = 'R'
        
        def checkForCollision(k, xx, yy, flow):
            cx = self.FC[flow][1][0]
            cy = self.FC[flow][1][1]
            
            rx = self.FC[flow][0][0]
            ry = self.FC[flow][0][1]
            
            if (self.riverPlotMap[GetIndex(xx+cx, yy+cy)] > 0 or 
                self.riverPlotMap[GetIndex(xx-rx, yy-ry)] > 0 or 
                self.riverPlotMap[GetIndex(xx+rx, yy+ry)] > 0):
                return True
            else:
                return False
            
        def checkNarrowLake(xx,yy):
            neighbors = 0
            xt = xx+1; yt = yy
            if (self.lakeMap[GetIndex(xt, yt)] > 0 and 
                GetIndex(xt, yt) not in plannedRiverPlots):
                neighbors += 1
                xout = xt; yout = yt
            xt = xx-1; yt = yy
            if (self.lakeMap[GetIndex(xt, yt)] > 0 and 
                GetIndex(xt, yt) not in plannedRiverPlots):
                neighbors += 1
                xout = xt; yout = yt
            xt = xx; yt = yy+1
            if (self.lakeMap[GetIndex(xt, yt)] > 0 and 
                GetIndex(xt, yt) not in plannedRiverPlots):
                neighbors += 1
                xout = xt; yout = yt
            xt = xx; yt = yy-1
            if (self.lakeMap[GetIndex(xt, yt)] > 0 and 
                GetIndex(xt, yt) not in plannedRiverPlots):
                neighbors += 1
                xout = xt; yout = yt
                
            if neighbors == 1:
                return (True, xout, yout)
            elif neighbors == 0:
                return (True, -1, -1)
            else:
                return (False, -1, -1)
        
        k.side = side
        ready = False
        
        # reverse river run: transform narrow lakes into rivers upstream
        if k.lakeStart:
            if side == 'L':
                xxr = xx-cx+ox
                yyr = yy-cy+oy
                
            else:
                xxr = xx-cx+ox+rx
                yyr = yy-cy+oy+ry
            
            while True:
                (ok, xnew, ynew) = checkNarrowLake(xxr, yyr)
                if not ok:
                    break
                plannedRiverPlots.append(GetIndex(xxr, yyr))
                xxr = xnew
                yyr = ynew
                
                if xnew == -1:
                    break
        
        # print("starting river run")
        while True:
            # lookup
            # print(". %i" % (flow))
            cx = self.FC[flow][1][0]
            cy = self.FC[flow][1][1]
            nextFlow = self.riverMap[GetIndex(xx+cx, yy+cy)]
            
            rx = self.FC[flow][0][0]
            ry = self.FC[flow][0][1]
            
            ox = self.FO_R2P[flow][0]
            oy = self.FO_R2P[flow][1]
            if nextFlow > self.W3 or nextFlow < self.N2: # or idx >= k.length-1:
                # print("End. %i" % (nextFlow))
                
                if side == 'L':
                    #self.riverPlotMap[GetIndex(xx+ox, yy+oy)] = 1
                    plannedRiverPlots.append(GetIndex(xx+ox, yy+oy))
                if side == 'R':
                    #self.riverPlotMap[GetIndex(xx+rx+ox, yy+ry+oy)] = 1
                    plannedRiverPlots.append(GetIndex(xx+rx+ox, yy+ry+oy))
                break
            
            turn = self.riverTurn(flow, nextFlow)
            
            # natural turn: no tiles added
            if ((side == 'R' and turn == self.turnRight) or 
                (side == 'L' and turn == self.turnLeft)):
                pass
            # direct ahead
            elif turn == self.turnNone:
                if side == 'L':
                    #self.riverPlotMap[GetIndex(xx+ox, yy+oy)] = 1
                    plannedRiverPlots.append(GetIndex(xx+ox, yy+oy))
                    if checkForCollision(k, xx+ox, yy+oy, flow):
                        ready = True
                        break
                if side == 'R':
                    #self.riverPlotMap[GetIndex(xx+rx+ox, yy+ry+oy)] = 1
                    plannedRiverPlots.append(GetIndex(xx+rx+ox, yy+ry+oy))
                    if checkForCollision(k, xx+rx+ox, yy+ry+oy, flow):
                        ready = True
                        break
                
            # long turn
            else:
                if side == 'L':
                    #self.riverPlotMap[GetIndex(xx+ox, yy+oy)] = 1
                    plannedRiverPlots.append(GetIndex(xx+ox, yy+oy))
                    if checkForCollision(k, xx+ox, yy+oy, flow):
                        ready = True
                        break
                if side == 'R':
                    #self.riverPlotMap[GetIndex(xx+rx+ox, yy+ry+oy)] = 1
                    plannedRiverPlots.append(GetIndex(xx+rx+ox, yy+ry+oy))
                    if checkForCollision(k, xx+rx+ox, yy+ry+oy, flow):
                        ready = True
                        break
                
                if side == 'L':
                    #self.riverPlotMap[GetIndex(xx+ox+cx, yy+oy+cy)] = 1
                    plannedRiverPlots.append(GetIndex(xx+ox+cx, yy+oy+cy))
                    if checkForCollision(k, xx+cx+ox, yy+cy+oy, flow):
                        ready = True
                        break
                if side == 'R':
                    #self.riverPlotMap[GetIndex(xx+rx+ox+cx, yy+ry+oy+cy)] = 1
                    plannedRiverPlots.append(GetIndex(xx+rx+ox+cx, yy+ry+oy+cy))
                    if checkForCollision(k, xx+rx+ox+cx, yy+ry+oy+cy, flow):
                        ready = True
                        break
                
            idx += 1
            xx += cx
            yy += cy
            flow = self.riverMap[GetIndex(xx, yy)]
        
        if not ready:
            cx = self.FC[flow][1][0]
            cy = self.FC[flow][1][1]
                
            rx = self.FC[flow][0][0]
            ry = self.FC[flow][0][1]
            
            ox = self.FO_R2P[flow][0]
            oy = self.FO_R2P[flow][1]
            
            # end condition check
            if k.lakeEnd:
                # if lake is not found directly ahead, it is on the other side: add that
                if side == 'L':
                    if not self.lakeMap[GetIndex(xx+ox+cx, yy+oy+cy)] > 0:
                        self.riverPlotMap[GetIndex(xx+rx+ox, yy+ry+oy)] = 1
                if side == 'R':
                    if not self.lakeMap[GetIndex(xx+ox+rx+cx, yy+oy+ry+cy)] > 0:
                        self.riverPlotMap[GetIndex(xx+ox, yy+oy)] = 1
            elif k.riverEnd:
                if side == 'L':
                    if not self.riverPlotMap[GetIndex(xx+ox+cx, yy+oy+cy)] > 0:
                        self.riverPlotMap[GetIndex(xx+rx+ox, yy+ry+oy)] = 1
                if side == 'R':
                    if not self.riverPlotMap[GetIndex(xx+ox+rx+cx, yy+oy+ry+cy)] > 0:
                        self.riverPlotMap[GetIndex(xx+ox, yy+oy)] = 1
            else:
                # if ocean is not found directly ahead, it is on the other side: add that
                if side == 'L':
                    if not self.lakeMap[GetIndex(xx+ox+cx, yy+oy+cy)] < 0:
                        self.riverPlotMap[GetIndex(xx+rx+ox, yy+ry+oy)] = 1
                if side == 'R':
                    if not self.lakeMap[GetIndex(xx+ox+rx+cx, yy+oy+ry+cy)] < 0:
                        self.riverPlotMap[GetIndex(xx+ox, yy+oy)] = 1
        
        for ii in plannedRiverPlots:
            self.riverPlotMap[ii] = 1
    
    def riverTurn(self, first, second):
        diff = ((second-1) % 4 + 1) - ((first-1) % 4 + 1)
        if diff == 3:
            diff -= 4
        elif diff == -3:
            diff += 4
        
        if abs(diff)==2:
            return self.turn180
        elif diff == 1:
            return self.turnRight
        elif diff == -1:
            return self.turnLeft
        elif diff == 0:
            return self.turnNone
    
    class LakeRecord:
        def __init__(self, lakeIs, borderIs, borderXs, borderYs, borderHs, waterH, exitX, exitY, exitFromX, exitFromY, exitRiverX, exitRiverY, ID):
            self.lakeIs = lakeIs     # a Set
            self.borderIs = borderIs # a Set
            self.borderXs = borderXs
            self.borderYs = borderYs
            self.borderHs = borderHs
            self.waterH = waterH
            self.exitX = exitX # estimated outflow plot location (not lake square)
            self.exitY = exitY
            self.exitFromX = exitFromX # plot that the lake flows out from
            self.exitFromY = exitFromY
            self.exitRiverX = exitRiverX
            self.exitRiverY = exitRiverY
            self.ID = ID # -1 means invalid / destroyed lake
        
        # join another lake to this lake, this being the master lake afterwards
        def joinLake(self, new_lake, x, y):
            # simple add together operation
            self.lakeIs = self.lakeIs | new_lake.lakeIs
            self.borderIs = self.borderIs | new_lake.borderIs
            
            # lake borders can be shared!
            # thus, duplicates must be removed
            for new in range(0,len(new_lake.borderHs)):
                found = False
                for old in range(0,len(self.borderHs)):
                    if new_lake.borderXs[new] == self.borderXs[old] and new_lake.borderYs[new] == self.borderYs[old]:
                        found = True
                if not found:
                    self.borderHs.append(new_lake.borderHs[new])
                    self.borderXs.append(new_lake.borderXs[new])
                    self.borderYs.append(new_lake.borderYs[new])
            
            # remove the joining lake plot from the master lake's border
            for i in range(0,len(self.borderHs)):
                if self.borderXs[i] == x and self.borderYs[i] == y:
                    del self.borderHs[i]
                    del self.borderXs[i]
                    del self.borderYs[i]
                    break
            
            self.waterH = min(self.waterH,new_lake.waterH)
            
        def getLowestBorder(self):
            idx = self.borderHs.index(min(self.borderHs))
            height = self.borderHs[idx]
            x = self.borderXs[idx]
            y = self.borderYs[idx]
            return (height,x,y,idx)
        
        def deleteBorder(self, idx):
            del self.borderHs[idx]
            del self.borderXs[idx]
            del self.borderYs[idx]
        
        def addBorder(self, height, x, y, i):
            self.borderHs.append(height)
            self.borderXs.append(x)
            self.borderYs.append(y)
            self.borderIs.add(i)
    
    def checkLakeExit(self,x,y):
        exits = []
        #-- check the four flow corners around --
        # bottom right corner
        cx = 0; cy = 0
        if self.flowMap[GetIndex(x+cx, y+cy)] != self.O:
            # south
            if self.lakeMap[GetIndex(x+cx, y+cy-1)] <= 0 and self.lakeMap[GetIndex(x+cx+1, y+cy-1)] <= 0:
                alts = [sm.heightMap[GetIndex(x+cx, y+cy-2)], # downstream
                        sm.heightMap[GetIndex(x+cx, y+cy-1)], # river square
                        sm.heightMap[GetIndex(x+cx, y+cy)]] # upstream
                if alts[2]>alts[0] and alts[2]>=alts[1] and alts[1]>=alts[0]:
                    exits.append((x+cx, y+cy, self.S3))
            # east
            if self.lakeMap[GetIndex(x+cx+1, y+cy-1)] <= 0 and self.lakeMap[GetIndex(x+cx+1, y+cy)] <= 0:
                alts = [sm.heightMap[GetIndex(x+cx+2, y+cy)], # downstream
                        sm.heightMap[GetIndex(x+cx+1, y+cy)], # river square
                        sm.heightMap[GetIndex(x+cx,   y+cy)]] # upstream
                if alts[2]>alts[0] and alts[2]>=alts[1] and alts[1]>=alts[0]:
                    exits.append((x+cx, y+cy, self.E3))
        
        # bottom left corner
        cx = -1; cy = 0
        if self.flowMap[GetIndex(x+cx, y+cy)] != self.O:
            # south
            if self.lakeMap[GetIndex(x+cx, y+cy-1)] <= 0 and self.lakeMap[GetIndex(x+cx+1, y+cy-1)] <= 0:
                alts = [sm.heightMap[GetIndex(x+cx, y+cy-2)], # downstream
                        sm.heightMap[GetIndex(x+cx, y+cy-1)], # river square
                        sm.heightMap[GetIndex(x+cx, y+cy)]] # upstream
                if alts[2]>alts[0] and alts[2]>=alts[1] and alts[1]>=alts[0]:
                    exits.append((x+cx, y+cy, self.S3))
            # west
            if self.lakeMap[GetIndex(x+cx, y+cy)] <= 0 and self.lakeMap[GetIndex(x+cx, y+cy-1)] <= 0:
                alts = [sm.heightMap[GetIndex(x+cx-1, y+cy)], # downstream
                        sm.heightMap[GetIndex(x+cx,   y+cy)], # river square
                        sm.heightMap[GetIndex(x+cx+1, y+cy)]] # upstream
                if alts[2]>alts[0] and alts[2]>=alts[1] and alts[1]>=alts[0]:
                    exits.append((x+cx, y+cy, self.W3))
        
        # top left corner
        cx = -1; cy = 1
        if self.flowMap[GetIndex(x+cx, y+cy)] != self.O:
            # west
            if self.lakeMap[GetIndex(x+cx, y+cy)] <= 0 and self.lakeMap[GetIndex(x+cx, y+cy-1)] <= 0:
                alts = [sm.heightMap[GetIndex(x+cx-1, y+cy)], # downstream
                        sm.heightMap[GetIndex(x+cx,   y+cy)], # river square
                        sm.heightMap[GetIndex(x+cx+1, y+cy)]] # upstream
                if alts[2]>alts[0] and alts[2]>=alts[1] and alts[1]>=alts[0]:
                    exits.append((x+cx, y+cy, self.W3))
            # north
            if self.lakeMap[GetIndex(x+cx, y+cy)] <= 0 and self.lakeMap[GetIndex(x+cx+1, y+cy)] <= 0:
                alts = [sm.heightMap[GetIndex(x+cx, y+cy+1)], # downstream
                        sm.heightMap[GetIndex(x+cx, y+cy)], # river square
                        sm.heightMap[GetIndex(x+cx, y+cy-1)]] # upstream
                if alts[2]>alts[0] and alts[2]>=alts[1] and alts[1]>=alts[0]:
                    exits.append((x+cx, y+cy, self.N3))
        
        # top right corner
        cx = 0; cy = 1
        if self.flowMap[GetIndex(x+cx, y+cy)] != self.O:
            # east
            if self.lakeMap[GetIndex(x+cx+1, y+cy-1)] <= 0 and self.lakeMap[GetIndex(x+cx+1, y+cy)] <= 0:
                alts = [sm.heightMap[GetIndex(x+cx+2, y+cy)], # downstream
                        sm.heightMap[GetIndex(x+cx+1, y+cy)], # river square
                        sm.heightMap[GetIndex(x+cx,   y+cy)]] # upstream
                if alts[2]>alts[0] and alts[2]>=alts[1] and alts[1]>=alts[0]:
                    exits.append((x+cx, y+cy, self.E3))
            # north
            if self.lakeMap[GetIndex(x+cx, y+cy)] <= 0 and self.lakeMap[GetIndex(x+cx+1, y+cy)] <= 0:
                alts = [sm.heightMap[GetIndex(x+cx, y+cy+1)], # downstream
                        sm.heightMap[GetIndex(x+cx, y+cy)], # river square
                        sm.heightMap[GetIndex(x+cx, y+cy-1)]] # upstream
                if alts[2]>alts[0] and alts[2]>=alts[1] and alts[1]>=alts[0]:
                    exits.append((x+cx, y+cy, self.N3))
        if len(exits) > 0:
            print("Exit validated: %i found" % (len(exits)))
            return exits
        else:
            print("No exit found!!!!")
            return exits
        
    def createAndFillLake(self,x,y, lakeID):
        print("")
        print("Starting lake filling at %i %i" % (x,y))
        i = GetIndex(x,y)
        s1 = Set(); s1.add(i)
        s2 = Set(); s2.add(i)
        k = self.LakeRecord(s1,s2,[x],[y],[sm.heightMap[i]],sm.heightMap[i],-1,-1,-1,-1,-1,-1,lakeID)
        
        if not self.lakeIDs[i] == 0:
            print("Lake already under another lake")
            return None
        self.lakeMap[i] = self.lakeSource
        self.lakeIDs[i] = k.ID
        
        return self.fillExistingLake(k)
    
    def fillExistingLake(self,k):
        while True:
            # looking for the lowest border location
            (height,x,y,idx)=k.getLowestBorder()
            ii = GetIndex(x,y)
            
            if height < k.waterH:
                # drainage pathway found! Lake finished
                print("Drainage found at %i %i: %i" % (x,y, ii))
                self.lakeMap[ii] = self.lakeExit
                k.exitX = x
                k.exitY = y
                # find the lake square to exit from
                found = False
                for s in k.lakeIs:
                    if s == GetIndex(x+1,y): k.exitFromX = x+1; k.exitFromY = y; found = True
                    if s == GetIndex(x-1,y): k.exitFromX = x-1; k.exitFromY = y; found = True
                    if s == GetIndex(x,y+1): k.exitFromX = x; k.exitFromY = y+1; found = True
                    if s == GetIndex(x,y-1): k.exitFromX = x; k.exitFromY = y-1; found = True
                if not found:
                    print("###Unable to find the lake square to exit from!###")
                else:
                    self.lakeMap[GetIndex(k.exitFromX,k.exitFromY)] = self.lakeExitFrom
                return k
            else:
                print("filling with water at %i %i: %i" % (x,y, ii))
                # fill with water the lowest lake border square
                k.lakeIs.add(ii)
                k.waterH = height
                k.deleteBorder(idx)
                
                self.lakeMap[ii] = self.lakeSquare
                self.lakeIDs[ii] = k.ID
            
            newID = 0
            # adding new borders to the list
            xs=[ 0, 0,-1, 1]
            ys=[-1, 1, 0, 0]
            for j in range(0,4):
                ii = GetIndex(x+xs[j],y+ys[j])
                if(ii not in k.borderIs):
                    if self.lakeIDs[ii] == 0:
                        k.addBorder(sm.heightMap[ii],x+xs[j],y+ys[j],ii)
                    else:
                        # existing lake hit!
                        newID = self.lakeIDs[ii]
            
            if newID !=0:
                # hitting a previously sea converted old lake bottom
                if newID == self.lakeToOcean:
                    newID = self.lakeOcean
                
                # join an existing lake
                print("Joining lake %i" %(newID))
                for n in k.lakeIs:
                    self.lakeIDs[n] = newID
                if not newID == -1:
                    k2 = self.lakes[newID]
                    k2.joinLake(k,x,y)
                    
                    # joined at the exit plot! New must be found
                    if k2.exitX == x and k2.exitY == y:
                        print("Exit was overwritten, must find a new one for the compound lake.")
                        self.lakeMap[GetIndex(x,y)] = self.lakeSquare
                        self.lakeMap[GetIndex(k2.exitFromX,k2.exitFromY)] = self.lakeSquare
                        self.fillExistingLake(k2)
                else:
                    print("Lake hit the ocean and is integrated into it.")
                    k.ID = -1 # making the lake invalid to skip later checks
                    for n in k.lakeIs:
                        self.lakeMap[n] = self.lakeToOcean
                    
                        
                return None
            
        return k
    
    def fixExitOfExistingLake(self,k):
        if k.ID == -1:
            print("Invalid lake, will not be fixed.")
            return
        while True:
            # looking for the lowest border location
            (height,x,y,idx)=k.getLowestBorder()
            i = GetIndex(x,y)
            
            print("filling with water at %i %i: %i" % (x,y, i))
            # fill with water the lowest lake border square
            k.lakeIs.add(i)
            k.waterH = height
            k.deleteBorder(idx)
            
            self.lakeMap[i] = self.lakeSquare
            self.lakeMap[GetIndex(k.exitFromX,k.exitFromY)] = self.lakeSquare
            
            newID = 0
            # adding new borders to the list
            xs=[ 0, 0,-1, 1]
            ys=[-1, 1, 0, 0]
            for j in range(0,4):
                ii = GetIndex(x+xs[j],y+ys[j])
                if(ii not in k.borderIs):
                    if self.lakeIDs[ii] == 0:
                        k.addBorder(sm.heightMap[ii],x+xs[j],y+ys[j],ii)
                    else:
                        # existing lake hit!
                        newID = self.lakeIDs[ii]
            
            if newID !=0:
                # join an existing lake
                print("Joining lake %i" %(newID))
                for n in k.lakeIs:
                    self.lakeIDs[n] = newID
                if not newID == -1:
                    k2 = self.lakes[newID]
                    k2.joinLake(k,x,y)
                    
                    # joined at the exit plot! New must be found
                    if k2.exitX == x and k2.exitY == y:
                        print("Exit was overwritten, must find a new one for the compound lake.")
                        self.lakeMap[GetIndex(x,y)] = self.lakeSquare
                        self.lakeMap[GetIndex(k2.exitFromX,k2.exitFromY)] = self.lakeSquare
                        self.fillExistingLake(k2)
                else:
                    print("Lake hit the ocean and is integrated into it.")
                    for n in k.lakeIs:
                        self.lakeMap[n] = self.lakeToOcean
                    
                        
                return None
            
            self.lakeIDs[i] = k.ID
            
            exits = self.checkLakeExit(x, y)
            if len(exits)>0:
                # drainage pathway found! Lake finished
                print("Drainage found at %i %i: %i" % (x,y, i))
                self.lakeMap[i] = self.lakeExit
                k.exitX = x
                k.exitY = y
                k.exitFromX = x
                k.exitFromY = y
                self.lakeMap[GetIndex(k.exitFromX,k.exitFromY)] = self.lakeExitFrom
                
                count = len(exits)
                choice = int(PRand.random()*count)
                i = GetIndex(exits[choice][0], exits[choice][1])
                self.flowMap2[i] = exits[choice][2]
                k.exitRiverX = exits[choice][0]
                k.exitRiverY = exits[choice][1]
                return k
            
        return k

    def rxFromPlot(self,x,y,direction):
        if direction == self.NE:
            return x,y + 1
        if direction == self.SW:
            return x - 1,y 
        if direction == self.SE:
            return x,y 
        raise ValueError,"rxFromPlot using bad direction input"
    
    def printRiverMap(self):
        print "River Map"
        wz = WindZones(mc.height,80,-80)
        for y in range(mc.height - 1,-1,-1):
            lineString = ""
            for x in range(mc.width):
                mapLoc = self.riverMap[GetIndex(x,y)]
                if mapLoc == self.O:
                    lineString += '.'
                elif mapLoc == self.L:
                    lineString += 'L'
                elif mapLoc == self.N:
                    lineString += 'N'
                elif mapLoc == self.S:
                    lineString += 'S'
                elif mapLoc == self.E:
                    lineString += 'E'
                elif mapLoc == self.W:
                    lineString += 'W'
                elif mapLoc == self.N2:
                    lineString += '|'
                elif mapLoc == self.S2:
                    lineString += '!'
                elif mapLoc == self.E2:
                    lineString += '='
                elif mapLoc == self.W2:
                    lineString += '-'
                else:
                    lineString += '#'
            lineString += "-" + wz.GetZoneName(wz.GetZone(y))
            print lineString
        lineString = " "
        print lineString
    
    def printLakeMap(self):
        print "Lake Map"
        wz = WindZones(mc.height,80,-80)
        for y in range(mc.height - 1,-1,-1):
            lineString = ""
            for x in range(mc.width):
                mapLoc = self.lakeMap[GetIndex(x,y)]
                if mapLoc == -1:
                    lineString += '.'
                elif mapLoc == 0:
                    lineString += ':'
                elif mapLoc == self.lakeSource:
                    lineString += '^'
                elif mapLoc == self.lakeSquare:
                    lineString += 'o'
                elif mapLoc == self.lakeExit:
                    lineString += '~'
                elif mapLoc == self.lakeExitFrom:
                    lineString += 'u'
                elif mapLoc == self.lakeToOcean:
                    lineString += ','
                else:
                    lineString += '#'
            lineString += "-" + wz.GetZoneName(wz.GetZone(y))
            print lineString
        lineString = " "
        print lineString
        
    def printLakeIDMap(self):
        print "Lake ID Map"
        wz = WindZones(mc.height,80,-80)
        for y in range(mc.height - 1,-1,-1):
            lineString = ""
            for x in range(mc.width):
                mapLoc = self.lakeIDs[GetIndex(x,y)]
                if mapLoc == -1:
                    lineString += '.'
                elif mapLoc == 0:
                    lineString += ':'
                else:
                    lineString += "\x1b[38;5;14m" + chr(mapLoc + 48) + "\x1b[0m"
            lineString += "-" + wz.GetZoneName(wz.GetZone(y))
            print lineString
        lineString = " "
        print lineString
        
    def printRiverIDMap(self):
        print "Lake ID Map"
        wz = WindZones(mc.height,80,-80)
        for y in range(mc.height - 1,-1,-1):
            lineString = ""
            for x in range(mc.width):
                mapLoc = self.riverIDs[GetIndex(x,y)]
                if mapLoc == -1:
                    lineString += '.'
                elif mapLoc == 0:
                    lineString += ':'
                else:
                    lineString += "\x1b[38;5;14m" + chr(mapLoc + 48) + "\x1b[0m"
            lineString += "-" + wz.GetZoneName(wz.GetZone(y))
            print lineString
        lineString = " "
        print lineString
        
    def printRiverPlotMap(self):
        print "River plot Map"
        wz = WindZones(mc.height,80,-80)
        for y in range(mc.height - 1,-1,-1):
            lineString = ""
            for x in range(mc.width):
                mapLoc = self.riverPlotMap[GetIndex(x,y)]
                if mapLoc == 2:
                    lineString += '.'
                elif mapLoc == 0:
                    lineString += ':'
                elif mapLoc == 1:
                    lineString += 'X'
                elif mapLoc == -1:
                    lineString += 'o'
                else:
                    lineString += "#"
            lineString += "-" + wz.GetZoneName(wz.GetZone(y))
            print lineString
        lineString = " "
        print lineString
        
    def printFlowMap(self, stage = 1):
        if stage == 2:
            flowData = self.flowMap2
        else:
            flowData = self.flowMap
        print "Flow Map"
        wz = WindZones(mc.height,80,-80)
        for y in range(mc.height - 1,-1,-1):
            lineString = ""
            for x in range(mc.width):
                mapLoc = flowData[GetIndex(x,y)]
                if mapLoc == self.O:
                    lineString += '.'
                elif mapLoc == self.L:
                    lineString += 'L'
                elif mapLoc == self.N:
                    lineString += 'N'
                elif mapLoc == self.S:
                    lineString += 'S'
                elif mapLoc == self.E:
                    lineString += 'E'
                elif mapLoc == self.W:
                    lineString += 'W'
                elif mapLoc == self.N3:
                    lineString += 'N'
                elif mapLoc == self.S3:
                    lineString += 'S'
                elif mapLoc == self.E3:
                    lineString += 'E'
                elif mapLoc == self.W3:
                    lineString += 'W'
            lineString += "-" + wz.GetZoneName(wz.GetZone(y))
            print lineString
        lineString = " "
        print lineString
    def printFlowAndLakesAlign(self, stage = 1):
        if stage == 2:
            flowData = self.flowMap2
        else:
            flowData = self.flowMap
        print "Lake Flow Check"
        for y in range(mc.height - 1,-1,-1):
            lineString1 = ""
            lineString2 = ""
            for x in range(mc.width):
                mapLoc = self.lakeMap[GetIndex(x,y)]
                if mapLoc == -1:
                    lineString1 += '..'
                elif mapLoc == 0:
                    lineString1 += "\x1b[38;5;10m" ':.' + "\x1b[0m"
                elif mapLoc == self.lakeSource:
                    lineString1 += "\x1b[38;5;4m" 'S.' + "\x1b[0m"
                elif mapLoc == self.lakeSquare:
                    lineString1 += "\x1b[38;5;12m" '#.' + "\x1b[0m"
                elif mapLoc == self.lakeExit:
                    lineString1 += "\x1b[38;5;6m" '~.' + "\x1b[0m"
                elif mapLoc == self.lakeExitFrom:
                    lineString1 += "\x1b[38;5;12m" 'u.' + "\x1b[0m"
                elif mapLoc == self.lakeToOcean:
                    lineString1 += "\x1b[38;5;6m" ',.' + "\x1b[0m"
                else:
                    lineString1 += '!.'
                mapLoc = flowData[GetIndex(x,y)]
                if mapLoc == self.O:
                    lineString2 += '..'
                elif mapLoc == self.L:
                    lineString2 += "\x1b[38;5;3m" '.o' + "\x1b[0m"
                elif mapLoc == self.N:
                    lineString2 += "\x1b[38;5;14m" + '.^' + "\x1b[0m"
                elif mapLoc == self.S:
                    lineString2 += "\x1b[38;5;14m" + '.v' + "\x1b[0m"
                elif mapLoc == self.E:
                    lineString2 += "\x1b[38;5;14m" + '.>' + "\x1b[0m"
                elif mapLoc == self.W:
                    lineString2 += "\x1b[38;5;14m" + '.<' + "\x1b[0m"
                elif mapLoc == self.N3:
                    lineString2 += "\x1b[38;5;11m" + '.^' + "\x1b[0m"
                elif mapLoc == self.S3:
                    lineString2 += "\x1b[38;5;11m" + '.v' + "\x1b[0m"
                elif mapLoc == self.E3:
                    lineString2 += "\x1b[38;5;11m" + '.>' + "\x1b[0m"
                elif mapLoc == self.W3:
                    lineString2 += "\x1b[38;5;11m" + '.<' + "\x1b[0m"
                else:
                    lineString2 += '.#'
##            lineString1 += "-" + wz.GetZoneName(wz.GetZone(y))
##            lineString2 += "-" + wz.GetZoneName(wz.GetZone(y))
            print lineString1
            print lineString2
        lineString1 = " "
        print lineString1
    def printRiverAndTerrainAlign(self, river=2):
        if river ==1:
            data2 = rm.riverMap
        else:
            data2 = rm.riverMap2
        
        print "River Alignment Check"
        for y in range(mc.height - 1,-1,-1):
            lineString1 = ""
            lineString2 = ""
            for x in range(mc.width):
                mapLoc = sm.terrainMap[GetIndex(x,y)]
                toRiver = self.riverPlotMap[GetIndex(x,y)]
                if not toRiver == 1:
                    if mapLoc == mc.OCEAN:
                        lineString1 += "\x1b[38;5;4m" + ',.' + "\x1b[0m"
                    elif mapLoc == mc.COAST:
                        lineString1 += "\x1b[38;5;4m" + ',.' + "\x1b[0m"
                    elif mapLoc == mc.DESERT:
                        lineString1 += "\x1b[38;5;10m" + 'D.' + "\x1b[0m"
                    elif mapLoc == mc.GRASS:
                        lineString1 += "\x1b[38;5;10m" + 'R.' + "\x1b[0m"
                    elif mapLoc == mc.PLAINS:
                        lineString1 += "\x1b[38;5;10m" + 'P.' + "\x1b[0m"
                    elif mapLoc == mc.TUNDRA:
                        lineString1 += "\x1b[38;5;10m" + 'T.' + "\x1b[0m"
                    elif mapLoc == mc.MARSH:
                        lineString1 += "\x1b[38;5;10m" + 'M.' + "\x1b[0m"
                    elif mapLoc == mc.SNOW:
                        lineString1 += "\x1b[38;5;10m" + 'I.' + "\x1b[0m"
                    elif mapLoc == mc.SAVANNAH:
                        lineString1 += "\x1b[38;5;10m" + 'S.' + "\x1b[0m"
                    elif mapLoc == mc.LAKE:
                        lineString1 += "\x1b[38;5;15m" + 'O.' + "\x1b[0m"
                    elif mapLoc == mc.LARGE_RIVER:
                        lineString1 += "\x1b[38;5;12m" + 'X.' + "\x1b[0m"
                    else:
                        lineString1 += '##'
                else:
                    if mapLoc == mc.OCEAN:
                        lineString1 += "\x1b[38;5;13m" + ',.' + "\x1b[0m"
                    elif mapLoc == mc.COAST:
                        lineString1 += "\x1b[38;5;13m" + ',.' + "\x1b[0m"
                    elif mapLoc == mc.DESERT:
                        lineString1 += "\x1b[38;5;13m" + 'D.' + "\x1b[0m"
                    elif mapLoc == mc.GRASS:
                        lineString1 += "\x1b[38;5;13m" + 'R.' + "\x1b[0m"
                    elif mapLoc == mc.PRAIRIE:
                    	lineString1 += "\x1b[38;5;13m" + 'P.' + "\x1b[0m"
                    elif mapLoc == mc.PLAINS:
                        lineString1 += "\x1b[38;5;13m" + 'L.' + "\x1b[0m"
                    elif mapLoc == mc.TUNDRA:
                        lineString1 += "\x1b[38;5;13m" + 'T.' + "\x1b[0m"
                    elif mapLoc == mc.MARSH:
                        lineString1 += "\x1b[38;5;13m" + 'M.' + "\x1b[0m"
                    elif mapLoc == mc.SNOW:
                        lineString1 += "\x1b[38;5;13m" + 'I.' + "\x1b[0m"
                    elif mapLoc == mc.SAVANNAH:
                        lineString1 += "\x1b[38;5;13m" + 'S.' + "\x1b[0m"
                    elif mapLoc == mc.LAKE:
                        lineString1 += "\x1b[38;5;13m" + 'O.' + "\x1b[0m"
                    elif mapLoc == mc.LARGE_RIVER:
                        lineString1 += "\x1b[38;5;13m" + 'X.' + "\x1b[0m"
                    else:
                        lineString1 += '##'
                mapLoc = data2[GetIndex(x,y)]
                if mapLoc == rm.O:
                    lineString2 += "\x1b[38;5;10m" + '..' + "\x1b[0m"
                elif mapLoc == rm.L:
                    lineString2 += "\x1b[38;5;3m" + '.o' + "\x1b[0m"
                elif mapLoc == rm.N:
                    lineString2 += "\x1b[38;5;12m" + '.^' + "\x1b[0m"
                elif mapLoc == rm.S:
                    lineString2 += "\x1b[38;5;12m" + '.v' + "\x1b[0m"
                elif mapLoc == rm.E:
                    lineString2 += "\x1b[38;5;12m" + '.>' + "\x1b[0m"
                elif mapLoc == rm.W:
                    lineString2 += "\x1b[38;5;12m" + '.<' + "\x1b[0m"
                elif mapLoc == rm.N2:
                    lineString2 += "\x1b[38;5;11m" + '.^' + "\x1b[0m"
                elif mapLoc == rm.S2:
                    lineString2 += "\x1b[38;5;11m" + '.v' + "\x1b[0m"
                elif mapLoc == rm.E2:
                    lineString2 += "\x1b[38;5;11m" + '.>' + "\x1b[0m"
                elif mapLoc == rm.W2:
                    lineString2 += "\x1b[38;5;11m" + '.<' + "\x1b[0m"
                elif mapLoc == rm.N3:
                    lineString2 += "\x1b[38;5;9m" + '.^' + "\x1b[0m"
                elif mapLoc == rm.S3:
                    lineString2 += "\x1b[38;5;9m" + '.v' + "\x1b[0m"
                elif mapLoc == rm.E3:
                    lineString2 += "\x1b[38;5;9m" + '.>' + "\x1b[0m"
                elif mapLoc == rm.W3:
                    lineString2 += "\x1b[38;5;9m" + '.<' + "\x1b[0m"
                else:
                    lineString2 += '##'
##            lineString1 += "-" + wz.GetZoneName(wz.GetZone(y))
##            lineString2 += "-" + wz.GetZoneName(wz.GetZone(y))
            print lineString1
            print lineString2
        lineString1 = " "
        print lineString1


class EuropeMap :
    def __init__(self):
        return
    def initialize(self):
        self.europeMap = array('i')
        for i in range(mc.width*mc.height):
            self.europeMap.append(0)
        
hm = HeightMap()
cm = ClimateMap()
sm = SmallMaps()
rm = RiverMap()
em = EuropeMap()
###############################################################################     
#functions that civ is looking for
###############################################################################
def getDescription():
	"""
	A map's Description is displayed in the main menu when players go to begin a game.
	For no description return an empty string.
	"""
	return "TXT_KEY_MAP_SCRIPT_FW_DESCR"

def getWrapX():
	return False
	
def getWrapY():
	return False
    
def getNumCustomMapOptions():
	"""
	Number of different user-defined options for this map
	Return an integer
	"""
	# TAC - Map scripts - koma13 - START
	#return 2
	return len(mc.mapOptionNames)
	# TAC - Map scripts - koma13 - END
	
def getCustomMapOptionName(argsList):
        """
        Returns name of specified option
        argsList[0] is Option ID (int)
        Return a Unicode string
        """
        optionID = argsList[0]
        if mc.mapOptionNames[optionID] == "distance":
            return localText.getText("TXT_KEY_MAP_FAIRE_WEATHER_DISTANCE", ())
        # TAC - Map scripts - koma13 - START
        elif mc.mapOptionNames[optionID] == "border north":
			return localText.getText("TXT_KEY_MAP_FAIRE_WEATHER_NORTHERN_BORDER", ())
        elif mc.mapOptionNames[optionID] == "border south":
			return localText.getText("TXT_KEY_MAP_FAIRE_WEATHER_SOUTHERN_BORDER", ())
        # TAC - Map scripts - koma13 - END
        elif mc.mapOptionNames[optionID] == "land allocation":
			return localText.getText("TXT_KEY_MAP_FAIRE_WEATHER_LAND_ALLOCATION", ())
        elif mc.mapOptionNames[optionID] == "regularity":
			return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_LAND_REGULARITY", ())
        elif mc.mapOptionNames[optionID] == "colony catchment radius":
             return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_CITY_CATCHMENT_RADIUS", ())
        elif mc.mapOptionNames[optionID] == "river commonality":
             return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_LARGE_RIVER_PREVALENCE", ())
        elif mc.mapOptionNames[optionID] == "river length":
             return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_MIN_LARGE_RIVER_LENGTH", ())
        elif mc.mapOptionNames[optionID] == "land percent":
             return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_LAND_PERCENT", ())
       
        return u""
	
def getNumCustomMapOptionValues(argsList):
        """
        Number of different choices for a particular setting
        argsList[0] is Option ID (int)
        Return an integer
        """
        optionID = argsList[0]
        if mc.mapOptionNames[optionID] == "distance":
            return 4
        # TAC - Map scripts - koma13 - START
        elif mc.mapOptionNames[optionID] == "border north":
            return 13
        elif mc.mapOptionNames[optionID] == "border south":
            return 13
        # TAC - Map scripts - koma13 - START
        elif mc.mapOptionNames[optionID] == "land allocation":
            return 2
        elif mc.mapOptionNames[optionID] == "regularity":
            return 3
        elif mc.mapOptionNames[optionID] == "colony catchment radius":
            return 2
        elif mc.mapOptionNames[optionID] == "river commonality":
            return 3
        elif mc.mapOptionNames[optionID] == "river length":
            return 6
        elif mc.mapOptionNames[optionID] == "land percent":
            return 4
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
    if mc.mapOptionNames[optionID] == "distance":
        if selectionID == 0:
            return localText.getText("TXT_KEY_MAP_FAIRE_WEATHER_3_FIELDS", ())
        elif selectionID == 1:
            return localText.getText("TXT_KEY_MAP_FAIRE_WEATHER_4_FIELDS", ())
        elif selectionID == 2:
            return localText.getText("TXT_KEY_MAP_FAIRE_WEATHER_6_FIELDS", ())
        elif selectionID == 3:
            return localText.getText("TXT_KEY_MAP_FAIRE_WEATHER_8_FIELDS", ())
    
    # TAC - Map scripts - koma13 - START
    elif mc.mapOptionNames[optionID] == "border north":
        if selectionID == 0:
            return localText.getText("TXT_KEY_MAP_FAIRE_WEATHER_SELECT_NORTHERN_BORDER_0", ())
        elif selectionID == 1:
            return "75 N "
        elif selectionID == 2:
            return "60 N"
        elif selectionID == 3:
            return "45 N"
        elif selectionID == 4:
            return "30 N"
        elif selectionID == 5:
            return "15 N"
        elif selectionID == 6:
            return localText.getText("TXT_KEY_MAP_FAIRE_WEATHER_SELECT_EQUATOR", ())
        elif selectionID == 7:
            return "15 S"
        elif selectionID == 8:
            return "30 S"
        elif selectionID == 9:
            return "45 S"
        elif selectionID == 10:
            return "60 S"
        elif selectionID == 11:
            return "75 S"
        elif selectionID == 12:
            return localText.getText("TXT_KEY_MAP_FAIRE_WEATHER_SELECT_NORTHERN_BORDER_1", ())
    
    elif mc.mapOptionNames[optionID] == "border south":
        if selectionID == 0:
            return localText.getText("TXT_KEY_MAP_FAIRE_WEATHER_SELECT_SOUTHERN_BORDER_0", ())
        elif selectionID == 1:
            return "75 N "
        elif selectionID == 2:
            return "60 N"
        elif selectionID == 3:
            return "45 N"
        elif selectionID == 4:
            return "30 N"
        elif selectionID == 5:
            return "15 N"
        elif selectionID == 6:
            return localText.getText("TXT_KEY_MAP_FAIRE_WEATHER_SELECT_EQUATOR", ())
        elif selectionID == 7:
            return "15 S"
        elif selectionID == 8:
            return "30 S"
        elif selectionID == 9:
            return "45 S"
        elif selectionID == 10:
            return "60 S"
        elif selectionID == 11:
            return "75 S"
        elif selectionID == 12:
            return localText.getText("TXT_KEY_MAP_FAIRE_WEATHER_SELECT_SOUTHERN_BORDER_1", ())
    # TAC - Map scripts - koma13 - END

    elif mc.mapOptionNames[optionID] == "land allocation":
        if selectionID == 0:
            return localText.getText("TXT_KEY_MAP_FAIRE_WEATHER_MANY_ISLES_0", ())
        elif selectionID == 1:
            return localText.getText("TXT_KEY_MAP_FAIRE_WEATHER_MANY_ISLES_1", ())

    elif mc.mapOptionNames[optionID] == "regularity":
        if selectionID == 0:
            return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_LAND_REGULARITY_OPT_0", ())
        elif selectionID == 1:
            return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_LAND_REGULARITY_OPT_1", ())
        elif selectionID == 2:
            return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_LAND_REGULARITY_OPT_2", ())

    elif mc.mapOptionNames[optionID] == "colony catchment radius":
        if selectionID == 0:
            return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_CITY_CATCHMENT_RADIUS_1_PLOT", ())
        elif selectionID == 1:
            return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_CITY_CATCHMENT_RADIUS_2_PLOT", ())

    elif mc.mapOptionNames[optionID] == "river commonality":
        if selectionID == 0:
            return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_LARGE_RIVER_PREVALENCE_OPT_0", ())
        elif selectionID == 1:
            return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_LARGE_RIVER_PREVALENCE_OPT_1", ())
        elif selectionID == 2:
            return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_LARGE_RIVER_PREVALENCE_OPT_2", ())

    elif mc.mapOptionNames[optionID] == "river length":
        if selectionID == 0:
            return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_MIN_LARGE_RIVER_LENGTH_OPT_0", ())
        elif selectionID == 1:
            return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_MIN_LARGE_RIVER_LENGTH_OPT_1", ())
        elif selectionID == 2:
            return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_MIN_LARGE_RIVER_LENGTH_OPT_2", ())
        elif selectionID == 3:
            return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_MIN_LARGE_RIVER_LENGTH_OPT_3", ())
        elif selectionID == 4:
            return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_MIN_LARGE_RIVER_LENGTH_OPT_4", ())
        elif selectionID == 5:
            return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_MIN_LARGE_RIVER_LENGTH_OPT_5", ())

    elif mc.mapOptionNames[optionID] == "land percent":
        if selectionID == 0:
            return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_LAND_PERCENT_OPT_0", ())
        elif selectionID == 1:
            return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_LAND_PERCENT_OPT_1", ())
        elif selectionID == 2:
            return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_LAND_PERCENT_OPT_2", ())
        elif selectionID == 3:
            return localText.getText("TXT_KEY_MAP_CUSTOM_OPTION_LAND_PERCENT_OPT_3", ())
    return u""
	
def getCustomMapOptionDefault(argsList):
    """
    Returns default value of specified option
    argsList[0] is Option ID (int)
    Return an integer
    """
    optionID = argsList[0]
    if mc.mapOptionNames[optionID] == "distance":
        return 1
    elif mc.mapOptionNames[optionID] == "border north":
        return 0

    elif mc.mapOptionNames[optionID] == "border south":
        return 12

    elif mc.mapOptionNames[optionID] == "land allocation":
        return 0

	
    # TAC - Map scripts - koma13 - START
    elif mc.mapOptionNames[optionID] == "regularity":
        return 1
    # TAC - Map scripts - koma13 - END

    elif mc.mapOptionNames[optionID] == "colony catchment radius":
        return CyGlobalContext().getDefaultCityCatchmentRadius()-1
    
    elif mc.mapOptionNames[optionID] == "river commonality":
        return 1
    
    elif mc.mapOptionNames[optionID] == "river length":
        return 2
    
    elif mc.mapOptionNames[optionID] == "land percent":
        return 1

    return 0
    
def isRandomCustomMapOption(argsList):
    """
    Returns a flag indicating whether a random option should be provided
    argsList[0] is Option ID (int)
    Return a bool
    """
    optionID = argsList[0]
    if mc.mapOptionNames[optionID] == "distance":
        return False
    # TAC - Map scripts - koma13 - START
    elif mc.mapOptionNames[optionID] == "border north":
        return True
    elif mc.mapOptionNames[optionID] == "border south":
        return True
    # TAC - Map scripts - koma13 - END
    elif mc.mapOptionNames[optionID] == "land allocation":
        return True
    elif mc.mapOptionNames[optionID] == "regularity":
        return True
    elif mc.mapOptionNames[optionID] == "colony catchment radius":
        return False
    elif mc.mapOptionNames[optionID] == "river commonality":
        return True
    elif mc.mapOptionNames[optionID] == "river length":
        return True
    elif mc.mapOptionNames[optionID] == "land percent":
        return True

    return False
    
#This doesn't work with my river system so it is disabled. Some civs
#might start without a river. Boo hoo.
def normalizeAddRiver():
    return
def normalizeAddLakes():
    return
def normalizeAddGoodTerrain():
    return
def normalizeRemoveBadTerrain():
    return
def normalizeRemoveBadFeatures():
    return
def normalizeAddFoodBonuses():
    return
def normalizeAddExtras():
    return
def isAdvancedMap():
	"""
	Advanced maps only show up in the map script pulldown on the advanced menu.
	Return 0 if you want your map to show up in the simple singleplayer menu
	"""
	return 0
def isClimateMap():
	"""
	Uses the Climate options
	"""
	return 0
	
def isSeaLevelMap():
	"""
	Uses the Sea Level options
	"""
	return 0
    
def getTopLatitude():
	"Default is 90. 75 is past the Arctic Circle"
	# TAC - Map scripts - koma13 - START
	#return 90
	return mc.topLattitude
	# TAC - Map scripts - koma13 - END

def getBottomLatitude():
	"Default is -90. -75 is past the Antartic Circle"
	# TAC - Map scripts - koma13 - START
	#return -90
	return mc.bottomLattitude
	# TAC - Map scripts - koma13 - END
    
def getGridSize(argsList):
    "Colonization is different than Civ in this function. The numbers"
    "here are the actual map size, not divided by 4 as in Civ"
    mc.initialize()
    mc.initInGameOptions()
    grid_sizes = {
            WorldSizeTypes.WORLDSIZE_TINY:		(15 + mc.distanceToEurope * 2,30),
            WorldSizeTypes.WORLDSIZE_SMALL:		(20 + mc.distanceToEurope * 2,40),
            WorldSizeTypes.WORLDSIZE_STANDARD:	(30 + mc.distanceToEurope * 2,60),
            WorldSizeTypes.WORLDSIZE_LARGE:		(40 + mc.distanceToEurope * 2,80),
            WorldSizeTypes.WORLDSIZE_HUGE:		(58,100),
            WorldSizeTypes.WORLDSIZE_MASSIVE:	(85,137),
			WorldSizeTypes.WORLDSIZE_GIGANTIC:	(127,184)
    }
    if (argsList[0] == -1): # (-1,) is passed to function on loads
            return []
    [eWorldSize] = argsList
    return grid_sizes[eWorldSize]

def generatePlotTypes():
    gc = CyGlobalContext()
    mmap = gc.getMap()
    mc.width = mmap.getGridWidth()
    mc.height = mmap.getGridHeight()
    PRand.seed()
    hm.performTectonics()
    hm.generateHeightMap()
    hm.combineMaps()
    hm.calculateSeaLevel()
##    hm.printHeightMap()
    hm.fillInLakes()
    hm.addWaterBands()
##    hm.printHeightMap()
    cm.createClimateMaps()
    sm.initialize()
    rm.generateRiverMap()
    plotTypes = [PlotTypes.PLOT_OCEAN] * (mc.width*mc.height)

    for i in range(mc.width*mc.height):
        mapLoc = sm.plotMap[i]
        if mapLoc == mc.PEAK:
            plotTypes[i] = PlotTypes.PLOT_PEAK
        elif mapLoc == mc.HILLS:
            plotTypes[i] = PlotTypes.PLOT_HILLS
        elif mapLoc == mc.LAND:
            plotTypes[i] = PlotTypes.PLOT_LAND
        else:
            plotTypes[i] = PlotTypes.PLOT_OCEAN
    print "Finished generating plot types."         
    return plotTypes
def generateTerrainTypes():
    NiTextOut("Generating Terrain  ...")
    print "Adding Terrain"
    gc = CyGlobalContext()
    terrainDesert = gc.getInfoTypeForString("TERRAIN_DESERT")
    terrainPrairie = gc.getInfoTypeForString("TERRAIN_PLAINS")
    terrainIce = gc.getInfoTypeForString("TERRAIN_SNOW")
    terrainTundra = gc.getInfoTypeForString("TERRAIN_TUNDRA")
    terrainGrass = gc.getInfoTypeForString("TERRAIN_GRASS")
    terrainHill = gc.getInfoTypeForString("TERRAIN_HILL")
    terrainCoast = gc.getInfoTypeForString("TERRAIN_COAST")
    terrainShallowCoast = gc.getInfoTypeForString("TERRAIN_SHALLOW_COAST")
    terrainOcean = gc.getInfoTypeForString("TERRAIN_OCEAN")
    terrainPeak = gc.getInfoTypeForString("TERRAIN_PEAK")
    terrainMarsh = gc.getInfoTypeForString("TERRAIN_MARSH")
    #Androrc Savannah
    terrainSavannah = gc.getInfoTypeForString("TERRAIN_SAVANNAH")
    #Androrc End
    terrainPlains = gc.getInfoTypeForString("TERRAIN_PLAINS_FERTILE")
    # Belisarius - Large Rivers
    terrainLargeRiver = gc.getInfoTypeForString("TERRAIN_LARGE_RIVERS")

   
    terrainTypes = [0]*(mc.width*mc.height)
    for i in range(mc.width*mc.height):
        if sm.terrainMap[i] == mc.OCEAN:
            terrainTypes[i] = terrainOcean
        elif sm.terrainMap[i] == mc.COAST:
            terrainTypes[i] = terrainCoast
        elif sm.terrainMap[i] == mc.SHALLOW_COAST:
            terrainTypes[i] = terrainShallowCoast
        elif sm.terrainMap[i] == mc.DESERT:
            terrainTypes[i] = terrainDesert
        elif sm.terrainMap[i] == mc.PRAIRIE:
            terrainTypes[i] = terrainPrairie
        elif sm.terrainMap[i] == mc.GRASS:
            terrainTypes[i] = terrainGrass
        #Androrc Savannah
        elif sm.terrainMap[i] == mc.SAVANNAH:
            terrainTypes[i] = terrainSavannah
        #Androrc End
        elif sm.terrainMap[i] == mc.TUNDRA:
            terrainTypes[i] = terrainTundra
        elif sm.terrainMap[i] == mc.SNOW:
            terrainTypes[i] = terrainIce
        elif sm.terrainMap[i] == mc.MARSH:
            terrainTypes[i] = terrainMarsh
        elif sm.terrainMap[i] == mc.PLAINS:
            terrainTypes[i] = terrainPlains
        elif sm.terrainMap[i] == mc.LARGE_RIVER:
            terrainTypes[i] = terrainLargeRiver
        elif sm.terrainMap[i] == mc.LAKE:
            terrainTypes[i] = terrainIce
 
    print "Finished generating terrain types."
    return terrainTypes

def addRivers():
    NiTextOut("Adding Rivers....")
    print "Adding Rivers"
    gc = CyGlobalContext()
    pmap = gc.getMap()
    for y in range(mc.height):
        for x in range(mc.width):
            placeRiversInPlot(x,y)

##    rm.printRiverAndTerrainAlign()
            
    #peaks and rivers don't always mix well graphically, so lets eliminate
    #these potential glitches. Basically if there are adjacent peaks on both
    #sides of a river, either in a cardinal direction or diagonally, they
    #will look bad.
    for y in range(mc.height):
        for x in range(mc.width):
            plot = pmap.plot(x,y)
            if plot.isPeak() == True:
                if plot.isNOfRiver() == True:
                    for xx in range(x - 1,x + 2,1):
                        yy = y - 1
                        if yy < 0:
                            break
                        #wrap in x direction
                        if xx == -1:
                            xx = mc.width - 1
                        elif xx == mc.width:
                            xx = 0
                        newPlot = pmap.plot(xx,yy)
                        ii = GetIndex(xx,yy)
                        if newPlot.isPeak():
                            plot.setPlotType(PlotTypes.PLOT_HILLS,True,True)
                            sm.plotMap[ii] = mc.HILLS
                            break
            #possibly changed so checked again
            if plot.isPeak() == True:
                if plot.isWOfRiver() == True:
                    for yy in range(y - 1,y + 2,1):
                        xx = x + 1
                        if xx == mc.width:
                            xx = 0
                        #do not wrap in y direction
                        if yy == -1:
                            continue
                        elif yy == mc.height:
                            continue
                        newPlot = pmap.plot(xx,yy)
                        ii = GetIndex(xx,yy)
                        if newPlot.isPeak():
                            plot.setPlotType(PlotTypes.PLOT_HILLS,True,True)
                            sm.plotMap[ii] = mc.HILLS
                            break
    
def placeRiversInPlot(x,y):
    gc = CyGlobalContext()
    pmap = gc.getMap()
    plot = pmap.plot(x,y)
    #NE
    xx,yy = rm.rxFromPlot(x,y,rm.NE)
    ii = GetIndex(xx,yy)
    if ii != -1:
        if rm.riverMap2[ii] == rm.S:
            plot.setWOfRiver(True,CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)
    #SW
    xx,yy = rm.rxFromPlot(x,y,rm.SW)
    ii = GetIndex(xx,yy)
    if ii != -1:
        if rm.riverMap2[ii] == rm.E:
            plot.setNOfRiver(True,CardinalDirectionTypes.CARDINALDIRECTION_EAST)
    #SE
    xx,yy = rm.rxFromPlot(x,y,rm.SE)
    ii = GetIndex(xx,yy)
    if ii != -1:
        if rm.riverMap2[ii] == rm.N:
            plot.setWOfRiver(True,CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
        elif rm.riverMap2[ii] == rm.W:
            plot.setNOfRiver(True,CardinalDirectionTypes.CARDINALDIRECTION_WEST)

            
def addLakes():
    print "Adding Lakes"
    gc = CyGlobalContext()
    mmap = gc.getMap()
    terrainLake = gc.getInfoTypeForString("TERRAIN_LAKE")
#    PrintFlowMap()
    oceanMap = Areamap(mc.width,mc.height,True,True)
    oceanMap.defineAreas(isSmallWaterMatch)
##    oceanMap.PrintList(oceanMap.areaList)
##    oceanMap.PrintAreaMap()
    for y in range(mc.height):
        for x in range(mc.width):
            i = GetIndex(x,y)
            if sm.terrainMap[GetIndex(x,y)] == mc.LAKE:
                plot = mmap.plot(x,y)
                plot.setTerrainType(terrainLake,True,True)
                
    oceanMap.defineAreas(isSmallWaterMatch)
##    oceanMap.PrintList(oceanMap.areaList)
##    oceanMap.PrintAreaMap()

        
def addFeatures():
    NiTextOut("Generating Large Rivers  ...")
    print "Adding Large Rivers"
    gc = CyGlobalContext()
    mmap = gc.getMap()
    featureRiverFord = gc.getInfoTypeForString("FEATURE_RIVER_FORD")

    for y in range(mc.height):
        for x in range(mc.width):
            i = GetIndex(x,y)
            plot = mmap.plot(x,y)
            if sm.terrainMap[i] == mc.LARGE_RIVER:
                if PRand.random() < mc.riverFordProbability:
                    plot.setFeatureType(featureRiverFord,0)

## nothing should be done here anymore, because DLL is used instead
## the calls are here: def afterGeneration():                
    return

def europeMatch(x,y):
    i = GetIndex(x,y)
    if em.europeMap[i] == 1:
        return True
    return False

def isAllAdjacentPlotTerrainType(x, y, terrainType):
    gc = CyGlobalContext()
    mmap = gc.getMap()
    
    for direction in range(1,9,1):
        xx,yy = GetXYFromDirection(x, y, direction)
        plot = mmap.plot(xx,yy)
        if plot.getTerrainType() != terrainType:
            return False
    return True

def isAnyAdjacentPlotTerrainType(x, y, terrainType):
    gc = CyGlobalContext()
    mmap = gc.getMap()
    
    for direction in range(1,9,1):
        xx,yy = GetXYFromDirection(x, y, direction)
        plot = mmap.plot(xx,yy)
        if plot.getTerrainType() == terrainType:
            return True
    return False

def isAnyAdjacentPlotType(x, y, plotType):
    gc = CyGlobalContext()
    mmap = gc.getMap()
    
    for direction in range(1,9,1):
        xx,yy = GetXYFromDirection(x, y, direction)
        plot = mmap.plot(xx,yy)
        if plot.getPlotType() == plotType:
            return True
    return False

def generateShallowCoast():
    
    gc = CyGlobalContext()
    mmap = gc.getMap()
    am = Areamap(mc.hmWidth,mc.hmHeight,True,True)
    terrainCoast = gc.getInfoTypeForString("TERRAIN_COAST")
    terrainShallowCoast = gc.getInfoTypeForString("TERRAIN_SHALLOW_COAST")
    
    shallowCoastChance = 0.2 # Baseline chance for converting coast to shallow coast
    shallowCostAdjacentChance = 0.4 # Higher chance if there's already adjacent shallow coast
    
    # Convert some patches of (regular) coast to shallow coasts
    for y in range(mc.height):
        for x in range(mc.width):
            plot = mmap.plot(x,y)
            if not plot.getPlotType() == PlotTypes.PLOT_PEAK:
                if plot.getTerrainType() == terrainCoast and not am.isChokePoint(x,y):
                    if isAnyAdjacentPlotTerrainType(x, y, terrainShallowCoast):
                        if PRand.random() <= shallowCostAdjacentChance:
                            plot.setTerrainType(terrainShallowCoast, True, True)
                    elif PRand.random() <= shallowCoastChance:
                        plot.setTerrainType(terrainShallowCoast, True, True)

def generateShrubland():
    
    gc = CyGlobalContext()
    mmap = gc.getMap()
    terrainShrubland = gc.getInfoTypeForString("TERRAIN_SHRUBLAND")
    terrainPrairie = gc.getInfoTypeForString("TERRAIN_PLAINS")
    terrainPlains = gc.getInfoTypeForString("TERRAIN_PLAINS_FERTILE")
    terrainGrassland = gc.getInfoTypeForString("TERRAIN_GRASS")
    terrainDesert = gc.getInfoTypeForString("TERRAIN_DESERT")

    shrublandChance = 0.5 # Baseline chance for converting terrains around desert to shrubland
    shrublandNextToShrublandChance = 0.5 # Chance to create Shrubland next to other Shrubland
    
    # Convert some prairie that's adjaceant to desert to shrubland 
    for y in range(mc.height):
        for x in range(mc.width):
            plot = mmap.plot(x,y)
            if not plot.getPlotType() == PlotTypes.PLOT_PEAK:
                if plot.getTerrainType() == terrainPrairie or plot.getTerrainType() == terrainPlains or plot.getTerrainType() == terrainGrassland:
                    if isAnyAdjacentPlotTerrainType(x, y, terrainDesert):
                        if PRand.random() <= shrublandChance:
                            plot.setTerrainType(terrainShrubland, True, True)
                            iRandPlotType = PRand.random()
                            if iRandPlotType <= 0.3: 
                                plot.setPlotType(PlotTypes.PLOT_HILLS,True,True)
                    elif isAnyAdjacentPlotTerrainType(x, y, terrainShrubland):
                        if PRand.random() <= shrublandNextToShrublandChance:
                            plot.setTerrainType(terrainShrubland, True, True)
                            iRandPlotType = PRand.random()
                            if iRandPlotType <= 0.3: 
                                plot.setPlotType(PlotTypes.PLOT_HILLS,True,True)

def generateTaiga():
    
    gc = CyGlobalContext()
    mmap = gc.getMap()
    terrainTaiga = gc.getInfoTypeForString("TERRAIN_TAIGA")
    terrainGrassland = gc.getInfoTypeForString("TERRAIN_GRASS")
    terrainTundra = gc.getInfoTypeForString("TERRAIN_TUNDRA")
    terrainPrairie = gc.getInfoTypeForString("TERRAIN_PLAINS")
    terrainPlains = gc.getInfoTypeForString("TERRAIN_PLAINS_FERTILE")
    
    tundraToTaigaChance = 0.4 # Baseline chance to convert Tundra To Taiga
    taigaNearTundraChance = 0.1 # Baseline chance for converting other terrain next to Tundar to Taiga
    
    # Convert some plot to Taiga
    for y in range(mc.height):
        for x in range(mc.width):
            plot = mmap.plot(x,y)
            if not plot.getPlotType() == PlotTypes.PLOT_PEAK:
                if plot.getTerrainType() == terrainGrassland or plot.getTerrainType() == terrainPrairie or plot.getTerrainType() == terrainPlains:
                    if isAnyAdjacentPlotTerrainType(x, y, terrainTundra):
                        if PRand.random() <= taigaNearTundraChance:
                            plot.setTerrainType(terrainTaiga, True, True)
                            # Generate some Hills
                            iRandPlotType = PRand.random()
                            if iRandPlotType <= 0.3: 
                                plot.setPlotType(PlotTypes.PLOT_HILLS,True,True)
                if plot.getTerrainType() == terrainTundra:
                    if isAnyAdjacentPlotTerrainType(x, y, terrainTundra):
                        if PRand.random() <= tundraToTaigaChance:
                            plot.setTerrainType(terrainTaiga, True, True)
                            # Generate some Hills
                            iRandPlotType = PRand.random()
                            if iRandPlotType <= 0.2: 
                                plot.setPlotType(PlotTypes.PLOT_HILLS,True,True)

def generateRockSteppes():

    gc = CyGlobalContext()
    mmap = gc.getMap()
    terrainPrairie = gc.getInfoTypeForString("TERRAIN_PLAINS")
    terrainPlains = gc.getInfoTypeForString("TERRAIN_PLAINS_FERTILE")
    terrainRockSteppes = gc.getInfoTypeForString("TERRAIN_ROCK_STEPPES")
    
    rockSteppesChance = 0.25 # Baseline chance for converting terrain
    
    # Convert 3x3 of flat fertile river-less land to rock steppes to reflect arid land  
    for y in range(mc.height):
        for x in range(mc.width):
            plot = mmap.plot(x,y)
            if not plot.getPlotType() == PlotTypes.PLOT_PEAK:
                if not plot.isRiver():
                    if not isAnyAdjacentPlotType(x, y, PlotTypes.PLOT_OCEAN):
                        if plot.getTerrainType() == terrainPrairie or plot.getTerrainType() == terrainPlains:
                            if isAllAdjacentPlotTerrainType(x, y, terrainPrairie) or isAllAdjacentPlotTerrainType(x, y, terrainPlains):
                                if PRand.random() <= rockSteppesChance * 0.2:
                                    plot.setTerrainType(terrainRockSteppes, True, True)
                                    # Generate some Hills
                                    iRandPlotType = PRand.random()
                                    if iRandPlotType <= 0.3: 
                                        plot.setPlotType(PlotTypes.PLOT_HILLS,True,True)

    
    for y in range(mc.height):
        for x in range(mc.width):
            plot = mmap.plot(x,y)
            if not plot.getPlotType() == PlotTypes.PLOT_PEAK:
                if not plot.isRiver():
                    if not isAnyAdjacentPlotType(x, y, PlotTypes.PLOT_OCEAN):
                        if plot.getTerrainType() == terrainPrairie or plot.getTerrainType() == terrainPlains:
                            if PRand.random() <= rockSteppesChance:
                                plot.setTerrainType(terrainRockSteppes, True, True)
                                iRandPlotType = PRand.random()
                                if iRandPlotType <= 0.3: 
                                    plot.setPlotType(PlotTypes.PLOT_HILLS,True,True)

def generateWetland():

    gc = CyGlobalContext()
    mmap = gc.getMap()
    terrainWetland = gc.getInfoTypeForString("TERRAIN_WETLAND")
    terrainDesert = gc.getInfoTypeForString("TERRAIN_DESERT")
    terrainTundra = gc.getInfoTypeForString("TERRAIN_TUNDRA")
    
    wetlandChance = 0.1 # Baseline chance for converting terrain
    wetlandAdjacentChance = 0.5 # Higher chance if there's already adjacent wetland
    wetlandHillChance = 0.5

    # Convert non-dry river plots
    for y in range(mc.height):
        for x in range(mc.width):
            plot = mmap.plot(x,y)
            if not plot.getPlotType() == PlotTypes.PLOT_PEAK:
                if plot.isRiver():
                    if not isAnyAdjacentPlotTerrainType(x, y, terrainDesert) and not isAnyAdjacentPlotTerrainType(x, y, terrainTundra):
                        if isAnyAdjacentPlotTerrainType(x, y, terrainWetland):
                            if PRand.random() <= wetlandAdjacentChance:
                                plot.setTerrainType(terrainWetland, True, True)
                                if PRand.random() <= wetlandHillChance:
                                	plot.setPlotType(PlotTypes.PLOT_HILLS,True,True)
                        else:
                            if PRand.random() <= wetlandChance:
                                plot.setTerrainType(terrainWetland, True, True)
    
    # Convert non-dry plots adjacent to lakes and coast
    for y in range(mc.height):
        for x in range(mc.width):
            plot = mmap.plot(x,y)
            if not plot.getPlotType() == PlotTypes.PLOT_PEAK and plot.getPlotType() == PlotTypes.PLOT_LAND:
                if isAnyAdjacentPlotType(x, y, PlotTypes.PLOT_OCEAN):
                    if not isAnyAdjacentPlotTerrainType(x, y, terrainDesert) and not isAnyAdjacentPlotTerrainType(x, y, terrainTundra):
                        if isAnyAdjacentPlotTerrainType(x, y, terrainWetland):
                            if PRand.random() <= wetlandAdjacentChance:
                                plot.setTerrainType(terrainWetland, True, True)
                        else:
                            if PRand.random() <= wetlandChance:
                                plot.setTerrainType(terrainWetland, True, True)
                        
def afterGeneration():
    gc = CyGlobalContext()
    mmap = gc.getMap()
    em.initialize()
    
    europeEast = gc.getInfoTypeForString("EUROPE_EAST")
    europeWest = gc.getInfoTypeForString("EUROPE_WEST")
    
    for y in range(mc.height):
        for x in range(mc.width):
            i = GetIndex(x,y)
            plot = mmap.plot(x,y)
            plot.setEurope(-1)
            if plot.isWater() == False:
                continue
            if x > mc.width/3 and x < 2 * mc.width/3:#dont penetrate past 1/3 of map
                continue
            if y < 4 or y > mc.height - 5:#make room for ice
                continue
            landFound = False
            for yy in range(y - mc.distanceToEurope,y + mc.distanceToEurope + 1,1):
                for xx in range(x - mc.distanceToEurope,x + mc.distanceToEurope + 1,1):
                    ii = GetIndex(xx,yy)
                    if ii == -1:
                        continue
                    newPlot = mmap.plot(xx,yy)
                    if newPlot.isWater() == False:
                        landFound = True
            if landFound == False:
                em.europeMap[i] = 1
                
    europeAreas = Areamap(mc.width,mc.height,False,False)
    europeAreas.defineAreas(europeMatch)
##    europeAreas.PrintAreaMap()
##    print "east europe = %d" % (europeEast)
##    print "west europe = %d" % (europeWest)
    #find europe areas that touch the map east/west edges
    x = 0
    xx = mc.width - 1
    europeAreaIdList = list()
    for y in range(mc.height):
        i = GetIndex(x,y)
        ii = GetIndex(xx,y)
        if em.europeMap[i] == 1:
            areaID = europeAreas.areaMap[i]
            AppendUnique(europeAreaIdList,areaID)
        if em.europeMap[ii] == 1:
            areaID = europeAreas.areaMap[ii]
            AppendUnique(europeAreaIdList,areaID)

    for y in range(mc.height):
        for x in range(mc.width):
            i = GetIndex(x,y)
            plot = mmap.plot(x,y)
            areaID = europeAreas.areaMap[i]
            if IsInList(europeAreaIdList,areaID):
                if x < mc.width/2:
                    plot.setEurope(europeWest)
                else:
                    plot.setEurope(europeEast)
        
    createIce()
    generateShallowCoast()
    generateShrubland()
    generateTaiga()
    generateRockSteppes()
    generateWetland()
    mg = CyMapGenerator()
    mg.addFeaturesOnLand()
    mg.addFeaturesOnWater()
    
def createIce():
    gc = CyGlobalContext()
    mmap = gc.getMap()
    featureIce = gc.getInfoTypeForString("FEATURE_ICE")
    iceChance = 1.0 
    # TAC - Map scripts - koma13 - START
    #for y in range(4):
    if abs(mc.bottomLattitude) == 90 and (mc.bottomLattitude != mc.topLattitude or mc.topLattitude < 0):
       for y in range(4):
            for x in range(mc.width):
                plot = mmap.plot(x,y)
                if plot != 0 and plot.isWater() == True and PRand.random() < iceChance:
                    plot.setFeatureType(featureIce,0)
            iceChance *= .66
    iceChance = 1.0
    #for y in range(mc.height - 1,mc.height - 5,-1):
    if abs(mc.topLattitude) == 90 and (mc.topLattitude != mc.bottomLattitude or mc.topLattitude > 0):
        for y in range(mc.height - 1,mc.height - 5,-1):
            for x in range(mc.width):
                plot = mmap.plot(x,y)
                if plot != 0 and plot.isWater() == True and PRand.random() < iceChance:
                    plot.setFeatureType(featureIce,0)
            iceChance *= .66 
    
    # TAC - Map scripts - koma13 - END

##mc.initialize()
##PRand.seed()
##hm.performTectonics()
##hm.generateHeightMap()
##hm.combineMaps()
##hm.calculateSeaLevel()
##hm.printHeightMap()
##hm.fillInLakes()
##hm.addWaterBands()
##hm.printHeightMap()
##cm.createClimateMaps()
##sm.initialize()
##rm.generateRiverMap()
##hm.printHeightMap()
##sm.printHeightMap()
##sm.printPlotMap()
##sm.printTerrainMap()
##rm.printFlowMap()
##rm.printRiverMap()
##rm.printRiverAndTerrainAlign()

##sm.printHeightMap()
##cm.printTempMap(cm.summerTempsMap)
##cm.printTempMap(cm.winterTempsMap)
##cm.printTempMap(cm.averageTempMap)
            
if not gameAccess:
    mc.initialize()
    PRand.seed()
    hm.performTectonics()
    hm.generateHeightMap()
    hm.combineMaps()
    hm.calculateSeaLevel()
    
    hm.fillInLakes()
    hm.addWaterBands()
    
    cm.createClimateMaps()
    sm.initialize()
    rm.generateRiverMap()
    
    rm.printRiverMap()
    rm.printLakeMap()
    rm.printLakeIDMap()
    
    rm.printRiverAndTerrainAlign(2)
    
    # mc = MapConstants()
    # PRand = PythonRandom()
    # hm = HeightMap()
    # cm = ClimateMap
    # rm = RiverMap
    # sm = SmallMaps()
    # em = EuropeMap()
    
    # mc.initialize()
    # PRand.seed()
    # hm.performTectonics()
    # hm.generateHeightMap()
    # hm.combineMaps()
    # hm.calculateSeaLevel()
    # hm.printHeightMap()
    
    # hm.fillInLakes()
    # hm.addWaterBands()
    # hm.printHeightMap()
    
    # cm.createClimateMaps()
    # cm.printRainFallMap(False)
    # sm.initialize()
    # rm.generateRiverMap()
    # hm.printHeightMap()
    
    # sm.printHeightMap()
    # sm.printPlotMap()
    # sm.printTerrainMap()
    # rm.printFlowMap()
    # rm.printRiverMap()
    # rm.printRiverAndTerrainAlign()
    # rm.printFlowAndLakesAlign(2)
    # rm.printFlowAndLakesAlign(1)
    # rm.printLakeMap()
    # rm.printLakeIDMap()
    
    # sm.printHeightMap()
    # cm.printTempMap(cm.summerTempsMap)
    # cm.printTempMap(cm.winterTempsMap)
    # cm.printTempMap(cm.averageTempMap)
     
    
    # mc.initialize()
    # PRand.seed()
    # hm.performTectonics()
    # hm.generateHeightMap()
    # hm.combineMaps()
    # hm.calculateSeaLevel()
    
    # hm.fillInLakes()
    # hm.addWaterBands()
    
    # cm.createClimateMaps()
    # sm.initialize()
    # rm.generateRiverMap()
    
    # rm.printRiverMap()
    
    # rm.printLakeMap()
