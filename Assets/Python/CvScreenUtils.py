## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
##
## CvScreenUtils.py
## Class specifically for modders to add to CvScreensInterface.py
## Associated Files: CvScreensInterface, CvScreenUtilsInterface
##
## Usage: 1) Create a file in the mods python folder called Cv<Mod>ScreenUtils
##        2) Create a class derived from CvScreenUtils in that file ' class <Mod>ScreenUtils(CvScreenUtils): '
##        3) Copy into Python\EntryPoints\ a copy of CvScreenUtilsInterface
##        4) Change normalScreenUtils to use your mod ScreenUtils ' normalScreenUtils = <Mod>ScreenUtils.<Mod>ScreenUtils '

import ScreenInput as PyScreenInput

class CvScreenUtils:
    # Place any screens that you would like to handle input in HandleInputMap
    # You can also overwrite control from the default interface by returning 1 from any function
    HandleInputMap = {
                    # add new screens here
                    }

    def leftMouseDown (self, argsList):
        # return 1 to consume the input
        screenEnum = argsList[0]
        return 0

    def rightMouseDown (self, argsList):
        # return 1 to consume the input
        screenEnum = argsList[0]
        return 0

    def mouseOverPlot(self, argsList):
        # return 1 to consume the input
        screenEnum = argsList[0]
        return 0

    def update(self, argsList):
        screenEnum = argsList[0]
        if (self.HandleInputMap and self.HandleInputMap.has_key(screenEnum)):
            screen = self.HandleInputMap.get(screenEnum)
            screen.update(argsList[1])
        return 0

    def handleInput (self, argsList):
        ' handle input is called when a screen is up '
        screenEnum, inputClass = argsList
        if (self.HandleInputMap and inputClass and self.HandleInputMap.has_key(screenEnum)):
            # get the screen that is active from the HandleInputMap Dictionary
            screen = self.HandleInputMap.get( inputClass.getPythonFile() )

            # call handle input on that screen
            if ( screen ):
                # return 1 to consume input
                return screen.handleInput(inputClass)
        return 0

    # Screen closing
    def onClose (self, argsList):
        screenEnum = argsList[0]
        return 0

    # Forced screen update
    def forceScreenUpdate (self, argsList):
        screenEnum = argsList[0]
        # place call to update function here
        return 0

    # Forced redraw
    def forceScreenRedraw (self, argsList):
        screenEnum = argsList[0]
        # place call to redraw function here
        return 0

    # Minimap Clicked
    def minimapClicked(self, argsList):
        screenEnum = argsList[0]
        # place call to mini map function here
        return 0
