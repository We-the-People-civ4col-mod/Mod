## WTP - City Interface
## Version 4.1


from CvPythonExtensions import *
import CvUtil
import operator
import CvScreenEnums
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

g_NumEmphasizeInfos = 0
g_NumHurryInfos = 0
g_NumActionInfos = 0

abandon_city_ZeroRedraw = True # TAC abandon city - Do we redraw if a pop 0 city is viewed?

HIDE_LEVEL_OFF = 0
HIDE_LEVEL_ALL = 1
HIDE_LEVEL_HIDE = 2
HIDE_LEVEL_NORMAL = 3
HIDE_LEVEL_UBER = 4
NUM_HIDE_LEVELS = 5

HIDE_TYPE_CITY = 0

class BuildingData:
#this describes the positions of the various building on the city Screen
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

    def building_grid(self, building_width,building_height):
        scale_factor = [building_width,
                        building_height,
                        building_height,
                        building_width]
        return [ list(map(operator.mul, building_pos, scale_factor))
                 for building_pos in self.building_data]


def place_decoration_element(screen,hide_state,element_name, x_top_left, y_top_left, x_width, y_height, art_element_name=None):
    screen.addPanel(element_name, u"", u"", True, False, x_top_left, y_top_left, x_width, y_height,
                    PanelStyles.PANEL_STYLE_STANDARD, WidgetTypes.WIDGET_GENERAL, -1, -1)
    if art_element_name is not None:
        screen.addDrawControl(element_name, ArtFileMgr.getInterfaceArtInfo(art_element_name).getPath(), x_top_left,
                              y_top_left, x_width, y_height, WidgetTypes.WIDGET_GENERAL, -1, -1)
    hide_state.appendtoHideState(screen, element_name, HIDE_TYPE_CITY, HIDE_LEVEL_ALL)


class CityScreenPosition:
    """ This class pilots the position of teh various elements on the city screen"""
    BOUNDARY_4_3 = 1.666
    BOUNDARY_16_9 = 2.1

    def __new__(cls, *args, **kwargs):
        screen_x_resolution, screen_y_resolution, = args
        ratio = float(screen_x_resolution)/float(screen_y_resolution)
        if ratio <= CityScreenPosition.BOUNDARY_4_3:
            return CityScreenPosition_4_3(*args,**kwargs)
        elif CityScreenPosition.BOUNDARY_4_3 < ratio <= CityScreenPosition.BOUNDARY_16_9:
            return CityScreenPosition_16_9(*args,**kwargs)
        else:
            return CityScreenPosition_21_9(*args,**kwargs)

    def __init__(self, screen_x_resolution, screen_y_resolution):

        self.screen_x_resolution = screen_x_resolution
        self.screen_y_resolution = screen_y_resolution

class CityScreenPosition_4_3(CityScreenPosition):

    pass

class CityScreenPosition_16_9(CityScreenPosition):
    pass


class CityScreenPosition_21_9(CityScreenPosition):
    pass


class CityScreen:

    def __init__(self):
        self.screen_name = "CityInterface"
        self.screen_num = CvScreenEnums.CITY_SCREEN
        self.fully_initialized = False
        self.BOTTOM_BUTTON_ROWS = 3

    def get_new_screen(self):
        """The role if this function is to return a new  instance of screen if the API does require a new object
        If it id well done, creating *one* should be enough"""
        return CyGInterfaceScreen(self.screen_name, self.screen_num)