## WTP - City Interface
## Version 4.1


from CvPythonExtensions import *
import CvUtil
import operator
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

g_NumEmphasizeInfos = 0
g_NumHurryInfos = 0
g_NumActionInfos = 0

abandon_city_ZeroRedraw = True # TAC abandon city - Do we redraw if a pop 0 city is viewed?

BUILDING_DATA = [\
     [30, 18, 20, 12] #Bells
    ,[72, 2,  20, 12] #Courthouse
    ,[44, 20, 20, 12] #Hammers
    ,[58, 18, 20, 12] #Meat
    ,[30, 36, 20, 12] #Coats
    ,[44, 38, 20, 12] #Leather
    ,[44, 2,  20, 12] #Cloth
    ,[58, 36, 20, 12] #Coloured Cloth
    ,[16, 38, 20, 12] #Beer
    ,[58, 0,  20, 12] #Rum
    ,[58, 54, 20, 12] #Wine
    ,[16, 20, 20, 12] #Cigars
    ,[86, 18, 20, 12] #Whale oil
    ,[30, 54, 20, 12] #Furniture
    ,[2,  18, 20, 12] #Tools
    ,[30, 0,  20, 12] #Crosses
    ,[2,  36, 20, 12] #Muskets
    ,[2,  0,  20, 12] #Fireworker
    ,[30, 72, 20, 12] #Horses
    ,[86, 36, 20, 12] #Dock
    ,[86, 54, 20, 12] #WareHouse
    ,[2,  54, 20, 12] #Spices
    ,[44, 74, 20, 12] #Domestic Market
    ,[86,  0, 20, 12] #Tavern
    ,[16,  2, 20, 12] #Education
    ,[87, 78, 20, 10] #Wall
    ,[72, 74, 20, 10] #Print
    ,[2,  89, 20, 9] #Mill
    ,[72, 86, 20, 10] #Trainstation
    ,[32, 89, 20, 7] #Corn
    ,[72, 38, 20, 12] #Medical Care
    ,[18, 88, 20, 8] #Well
    ,[84, 73, 20, 10] #Defense
    ,[44, 56, 20, 12] #Confectioner
    ,[16, 56, 20, 12] #Leather Padder
    ,[2,  72, 20, 12] #Cooking Oil Trader
    ,[58, 72, 20, 12] #Peanut Roaster
    ,[72, 56, 20, 12] #Potter
    ,[72, 20, 20, 12] #Tailor
    ,[16, 74, 20, 12] #Baker
    ,[81, 88, 20, 7] #Statue
    ,[45, 88, 20, 10] #Graveyard
    ,[59, 86, 20, 10]] #Jail



def building_grid(building_data,building_width,building_height):
    scale_factor = [building_width,
                    building_height,
                    building_height,
                    building_width]
    return [ list(map(operator.mul, building_pos, scale_factor))
             for building_pos in building_data]