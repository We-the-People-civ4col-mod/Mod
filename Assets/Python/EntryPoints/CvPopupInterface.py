## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil

#globals
gc = CyGlobalContext()

# This is an interface file for handling Python Popup Function Calls
# argsList = (iData1, iData2)

# Assign the function by using the 'addPythonButton' function in pyHelper/Popup
# This allows you to specify a strFunctionName. If the player clicks that button then this file
# will be called with the specified function name.

# ie: addPythonButton( self, strFunctionName, strButtonText, strHelpText, strArtPointer = "Art\Interface\Popups\PopupRadioButton.kfm", iData1 = -1, iData2 = -1, bOption = True):

def initTestPopup():

	# make a popup
	popup=CyPopup(-1,EventContextTypes.NO_EVENTCONTEXT, 1)

	# set the header string, this will go outside the scrollable area
	popup.setHeaderString("Test Popup", CvUtil.FONT_CENTER_JUSTIFY)

	# set a body string
	popup.setBodyString("This is a test popup.", CvUtil.FONT_LEFT_JUSTIFY)

	# make 3 buttons
	for i in range(2):
		strButtonText = "Button " + str(i)
		strHelpText = "Help text for button " + str(i)
		popup.addButton(strButtonText)

	#make 4 radiobuttons in group 0
	popup.createRadioButtons(2, 0)
	for i in range(2):
		strRadioText = "RadioButton " + str(i)
		# set radioButtons text of button i, group 0 to strRadioText
		popup.setRadioButtonText(i, strRadioText, 0)

	# make 5 checkboxes in group 0
	popup.createCheckBoxes(3, 0)
	for i in range(3):
		strCheckboxText = "Checkbox " + str(i)
		# set checkbox text of button i, group 0 to strCheckboxText
		popup.setCheckBoxText(i, strCheckboxText, 0)

	# make a editbox
	popup.createEditBox("EditBox1", 0)

	# make a pulldown
	popup.createPullDown( 0 )
	# make 5 pulldown strings
	for i in range(5):
		szPullDownString = "PullDown String " + str(i)
		popup.addPullDownString( szPullDownString, i, 0 )
	popup.setSelectedPulldownID( 2, 0 );

	# make a listbox
	popup.createListBox( 0 )
	for i in range(6):
		szListBoxString = "ListBox String " + str(i)
		popup.addListBoxString( szListBoxString, i, 0 )
	popup.setSelectedListBoxString( 1, 0 )

	# add a dds
	popup.addDDS( "art/interface/buttons/unit/Caravel.dds", 280, 100, 32, 32 )

	popup.launch(true, PopupStates.POPUPSTATE_IMMEDIATE)

def initPythonTestPopup():

	# make a popup
	popup=CyPopup(-1,EventContextTypes.NO_EVENTCONTEXT, 1)

	# set the header string, this will go outside the scrollable area
	popup.setHeaderString("Test Popup", CvUtil.FONT_CENTER_JUSTIFY)

	# set a body string
	popup.setPythonBodyString("This is a test popup.", "initTestPopup", "Will init the non-python popup", CvUtil.FONT_LEFT_JUSTIFY)

	# make 2 python buttons
	strButtonText = "Button 1"
	strHelpText = "Help text for button 1"
	popup.addPythonButton( "showHelp", strButtonText, strHelpText, "art/interface/buttons/building/Granary.dds"  )

	strButtonText = "Button 2"
	strHelpText = "Help text for button 2"
	popup.addPythonButton( "showHelp", strButtonText, strHelpText, "art/interface/buttons/building/Granary.dds"  )

	#make 4 radiobuttons in group 0
	popup.createPythonRadioButtons(2, 0)
	for i in range(2):
		strRadioText = "RadioButton " + str(i)
		strRadioHelpText = "RadioButton Help Text " + str(i)
		# set radioButtons text of button i, group 0 to strRadioText
		popup.setPythonRadioButtonText(i, strRadioText, strRadioHelpText, 0)

	# make 5 checkboxes in group 0
	popup.createPythonCheckBoxes(3, 0)
	for i in range(3):
		strCheckboxText = "Checkbox " + str(i)
		strCheckBoxHelp = "Checkbox Help Text " + str(i)
		# set checkbox text of button i, group 0 to strCheckboxText
		popup.setPythonCheckBoxText(i, strCheckboxText, strCheckBoxHelp, 0)

	# make a editbox
	popup.createPythonEditBox("EditBox1", "Editbox Help Text", 0)

	# make a pulldown
	popup.createPythonPullDown( "Pulldown Help Text", 0 )
	# make 5 pulldown strings
	for i in range(5):
		szPullDownString = "PullDown String " + str(i)
		popup.addPullDownString( szPullDownString, i, 0 )
	popup.setSelectedPulldownID( 2, 0 );

	# make a listbox
	popup.createPythonListBox( "ListBox Help String", 0 )
	for i in range(6):
		szListBoxString = "ListBox String " + str(i)
		popup.addListBoxString( szListBoxString, i, 0 )
	popup.setSelectedListBoxString( 1, 0 )

	# make a listbox
	popup.createPythonListBox( "ListBox Help String 2", 1 )
	for i in range(4):
		szListBoxString = "ListBox String " + str(i)
		popup.addListBoxString( szListBoxString, i, 1 )
	popup.setSelectedListBoxString( 1, 1 )

	# add a dds
	popup.addPythonDDS( "art/interface/buttons/unit/Caravel.dds", "DDS Help text", 280, 100, 32, 32 )

	popup.launch(true, PopupStates.POPUPSTATE_IMMEDIATE)


