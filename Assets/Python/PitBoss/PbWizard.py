## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
#
# Sample PitBoss window/app framework
# Mustafa Thamer 2-15-05
#
from CvPythonExtensions import *
import wx
import wx.wizard
import wx.lib.scrolledpanel
import time
import string

bPublic = True
bSaved = False
bScenario = False
bPatchConfirmed = False
bPatchOK = False
szPatchName = None
msgBox = None
PB = CyPitboss()
gc = CyGlobalContext()
localText = CyTranslator()
curPage = None

#
# Mod Select Page (first page of wizard)
#
class ModSelectPage(wx.wizard.PyWizardPage):
	def __init__(self, parent):
		wx.wizard.PyWizardPage.__init__(self, parent)
		self.next = self.prev = None
		self.myParent = parent

		pageSizer = wx.BoxSizer(wx.VERTICAL)

		modPanel = wx.lib.scrolledpanel.ScrolledPanel(self, -1, size=(300, 600), style = wx.SUNKEN_BORDER)
		sizer = wx.BoxSizer(wx.VERTICAL)

		header = wx.StaticText(self, -1, localText.getText("TXT_KEY_PITBOSS_CHOOSE_MOD", ()))
		pageSizer.Add(header, 0, wx.ALL, 5)

		# Place the radio buttons
		self.currentMod = 0
		self.rbs = []

		# First choice is no mod
		self.rbs.append( wx.RadioButton(
			modPanel, -1, localText.getText("TXT_KEY_MAIN_MENU_NONE", ()), wx.DefaultPosition, wx.DefaultSize, wx.RB_GROUP
			) )
		sizer.Add(self.rbs[0], 0, wx.ALL, 3)

		if (PB.getModName() == ""):
			self.rbs[0].SetValue(True)

		index = 0
		for index in range(PB.getNumMods()):
			self.rbs.append( wx.RadioButton(
				modPanel, -1, PB.getModAt(index), wx.DefaultPosition, wx.DefaultSize
				) )
			sizer.Add(self.rbs[index+1], 0, wx.ALL, 3)

			if (PB.isCurrentMod(index)):
				self.currentMod = index+1
				self.rbs[index+1].SetValue(True)

		modPanel.SetSizer(sizer)
		modPanel.SetAutoLayout(1)
		modPanel.SetupScrolling()

		pageSizer.Add(modPanel, 0, wx.ALL, 5)

		self.SetSizer(pageSizer)

		self.Bind(wx.wizard.EVT_WIZARD_PAGE_CHANGED, self.OnPageChanged)
		self.Bind(wx.wizard.EVT_WIZARD_PAGE_CHANGING, self.OnPageChanging)

	def enableButtons(self):
		self.myParent.FindWindowById(wx.ID_FORWARD).Enable(True)
		self.myParent.FindWindowById(wx.ID_BACKWARD).Enable(False)

	def OnPageChanged(self, event):
		global curPage
		global bPatchConfirmed
		global bPatchOK

		bPatchConfirmed = False
		bPatchOK = False

		# Determine what buttons should be enabled
		self.enableButtons()

		# We are the current page
		curPage = self

	def OnPageChanging(self, event):
		# Check direction
		if event.GetDirection():
			# We are trying to move forward - have we selected another mod?

			# Determine our selection
			iSelection = 0
			while (not self.rbs[iSelection].GetValue() and iSelection < PB.getNumMods()):
				iSelection = iSelection+1

			# Do we need to load a mod
			if (iSelection != self.currentMod):
				# Yep.
				PB.loadMod(iSelection-1)
				PB.quit()

	def SetNext(self, next):
		self.next = next

	def SetPrev(self, prev):
		self.prev = prev

	def GetNext(self):
		"Select which next page to show based on network selected"
		next = self.next

		# Determine our selection
		iSelection = 0
		while (not self.rbs[iSelection].GetValue() and iSelection < PB.getNumMods()):
			iSelection = iSelection+1

		# Do we need to load a mod
		if (iSelection != self.currentMod):
			next = None

		return next

	def GetPrev(self):
		return self.prev

#
# SMTP Login Page
#
class SMTPLoginPage(wx.wizard.WizardPageSimple):
	def __init__(self, parent):
		wx.wizard.WizardPageSimple.__init__(self, parent)

		self.myParent = parent
		header = wx.StaticText(self, -1, localText.getText("TXT_KEY_PITBOSS_SMTP_HEADER", ()))

		hostLbl = wx.StaticText(self, -1, localText.getText("TXT_KEY_PITBOSS_SMTP_HOST", ()))
		self.host = wx.TextCtrl(self, -1, PB.getSMTPHost(), size=(125,-1))
		self.host.SetHelpText(localText.getText("TXT_KEY_PITBOSS_SMTP_HOST_HELP", ()))
		self.host.SetInsertionPoint(0)

		usernameLbl = wx.StaticText(self, -1, localText.getText("TXT_KEY_PITBOSS_SMTP_LOGIN", ()))
		self.username = wx.TextCtrl(self, -1, PB.getSMTPLogin(), size=(125,-1))
		self.username.SetHelpText(localText.getText("TXT_KEY_PITBOSS_SMTP_LOGIN_HELP", ()))

		passwordLbl = wx.StaticText(self, -1, localText.getText("TXT_KEY_PITBOSS_SMTP_PASSWORD", ()))
		self.password = wx.TextCtrl(self, -1, "", size=(125,-1), style=wx.TE_PASSWORD)
		self.password.SetHelpText(localText.getText("TXT_KEY_PITBOSS_SMTP_PASSWORD_HELP", ()))

		emailLbl = wx.StaticText(self, -1, localText.getText("TXT_KEY_POPUP_DETAILS_EMAIL", ()))
		self.email = wx.TextCtrl(self, -1, PB.getEmail(), size=(125,-1))
		self.email.SetHelpText(localText.getText("TXT_KEY_POPUP_DETAILS_EMAIL", ()))

		self.Bind(wx.wizard.EVT_WIZARD_PAGE_CHANGING, self.OnPageChanging)
		self.Bind(wx.wizard.EVT_WIZARD_PAGE_CHANGED, self.OnPageChanged)

		sizer = wx.FlexGridSizer(cols=2, hgap=4, vgap=4)
		sizer.AddMany([ hostLbl, self.host,
						usernameLbl, self.username,
						passwordLbl, self.password,
						emailLbl, self.email,
						])
		border = wx.BoxSizer(wx.VERTICAL)
		border.Add(sizer, 0, wx.ALL, 25)
		self.SetSizer(border)
		self.SetAutoLayout(True)

	def enableButtons(self):
		self.myParent.FindWindowById(wx.ID_FORWARD).Enable(True)
		self.myParent.FindWindowById(wx.ID_BACKWARD).Enable(True)

	def OnPageChanged(self, event):
		global curPage

		# Determine what buttons should be enabled
		self.enableButtons()

		# We are the current page
		curPage = self

	def OnPageChanging(self, event):
		# Check direction
		if event.GetDirection():
			# We are trying to move forward - set the SMTP values
			PB.setSMTPValues( self.host.GetValue(), self.username.GetValue(), self.password.GetValue(), self.email.GetValue() )


#
# Network Selection Page
#
class NetSelectPage(wx.wizard.PyWizardPage):
	def __init__(self, parent):
		wx.wizard.PyWizardPage.__init__(self, parent)
		self.next = self.prev = None
		self.myParent = parent

		# Place the radio buttons
		selections = [localText.getText("TXT_KEY_PITBOSS_DIRECTIP", ()), localText.getText("TXT_KEY_PITBOSS_LAN", ()), localText.getText("TXT_KEY_PITBOSS_INTERNET", ())]
		sizer = wx.BoxSizer(wx.VERTICAL)
		self.rb = wx.RadioBox(
					self, -1, localText.getText("TXT_KEY_PITBOSS_SELECT_NETWORK", ()), wx.DefaultPosition, wx.DefaultSize,
					selections, 1, wx.RA_SPECIFY_COLS
					)

		self.rb.SetToolTip(wx.ToolTip(localText.getText("TXT_KEY_PITBOSS_SELECT_NETWORK_HELP", ())))
		sizer.Add(self.rb, 0, wx.ALL, 5)

		self.SetSizer(sizer)

		self.Bind(wx.wizard.EVT_WIZARD_PAGE_CHANGED, self.OnPageChanged)

	def enableButtons(self):
		self.myParent.FindWindowById(wx.ID_FORWARD).Enable(True)
		self.myParent.FindWindowById(wx.ID_BACKWARD).Enable(True)

	def OnPageChanged(self, event):
		global curPage

		# Determine what buttons should be enabled
		self.enableButtons()

		# We are the current page
		curPage = self

	def SetNext(self, next):
		self.next = next

	def SetPrev(self, prev):
		self.prev = prev

	def GetNext(self):
		"Select which next page to show based on network selected"
		global bPublic

		next = self.next

		if (self.rb.GetSelection() == 0):
			bPublic = True
			next = next.GetNext()
		elif (self.rb.GetSelection() == 1):
			bPublic = False
			next = next.GetNext()
		else:
			bPublic = True

		return next

	def GetPrev(self):
		return self.prev

#
# Login page (optional 2nd page)
#
class LoginPage(wx.wizard.WizardPageSimple):
	def __init__(self, parent):
		wx.wizard.WizardPageSimple.__init__(self, parent)

		self.myParent = parent
		header = wx.StaticText(self, -1, localText.getText("TXT_KEY_PITBOSS_LOGIN", ()))

		usernameLbl = wx.StaticText(self, -1, localText.getText("TXT_KEY_PITBOSS_USERNAME", ()))
		self.username = wx.TextCtrl(self, -1, "", size=(125,-1))
		self.username.SetHelpText(localText.getText("TXT_KEY_PITBOSS_USERNAME_HELP", ()))
		self.username.SetInsertionPoint(0)
		self.Bind(wx.EVT_TEXT, self.OnTextEntered, self.username)

		passwordLbl = wx.StaticText(self, -1, localText.getText("TXT_KEY_PITBOSS_PASSWORD", ()))
		self.password = wx.TextCtrl(self, -1, "", size=(125,-1), style=wx.TE_PASSWORD)
		self.password.SetHelpText(localText.getText("TXT_KEY_PITBOSS_PASSWORD_HELP", ()))
		self.Bind(wx.EVT_TEXT, self.OnTextEntered, self.password)

		self.Bind(wx.wizard.EVT_WIZARD_PAGE_CHANGING, self.OnPageChanging)
		self.Bind(wx.wizard.EVT_WIZARD_PAGE_CHANGED, self.OnPageChanged)

		sizer = wx.FlexGridSizer(cols=2, hgap=4, vgap=4)
		sizer.AddMany([ usernameLbl, self.username,
						passwordLbl, self.password,
						])
		border = wx.BoxSizer(wx.VERTICAL)
		border.Add(sizer, 0, wx.ALL, 25)
		self.SetSizer(border)
		self.SetAutoLayout(True)

	def enableButtons(self):
		global bPatchConfirmed
		global bPatchOK

		if (not bPatchConfirmed):
			# Not confirmed, disable buttons
			self.myParent.FindWindowById(wx.ID_FORWARD).Disable()
			self.myParent.FindWindowById(wx.ID_BACKWARD).Disable()
		# Check to see if there is text in both boxes
		elif ( (self.username.GetValue() == "") or (self.password.GetValue() == "") or (not bPatchOK) ):
			# There isn't, disable the forward button
			self.myParent.FindWindowById(wx.ID_FORWARD).Disable()
			self.myParent.FindWindowById(wx.ID_BACKWARD).Enable(True)
		else:
			# Text entered, enable the forward button
			self.myParent.FindWindowById(wx.ID_FORWARD).Enable(True)
			self.myParent.FindWindowById(wx.ID_BACKWARD).Enable(True)

	def patchAvailable(self, patchName, patchUrl):
		global bPatchConfirmed
		global szPatchName

		# Put up a dialog
		dlg = wx.MessageDialog(
			self, localText.getText("TXT_KEY_PITBOSS_PATCH_REQUIRED_DESC", ()),
			localText.getText("TXT_KEY_PITBOSS_PATCH_REQUIRED_TITLE", ()), wx.OK|wx.CANCEL|wx.ICON_EXCLAMATION)

		# Show the modal dialog and get the response
		if dlg.ShowModal() == wx.ID_OK:
			# They want to download the patch - tell the app
			if (not PB.downloadPatch(patchName, patchUrl)):
				# Patching failed - tell the user
				msg = wx.MessageBox(localText.getText("TXT_KEY_PITBOSS_PATCH_DOWNLOAD_ERROR_DESC", ()), localText.getText("TXT_KEY_PITBOSS_PATCH_DOWNLOAD_ERROR_TITLE", ()), wx.ICON_ERROR)
				bPatchConfirmed = true
				szPatchName = patchName
				self.enableButtons()
		else:
			bPatchConfirmed = true
			self.enableButtons()

	def patchComplete(self):
		global bPatchConfirmed
		global bPatchOK
		global szPatchName

		# Put up a dialog
		dlg = wx.MessageDialog(
			self, localText.getText("TXT_KEY_PITBOSS_PATCH_COMPLETE_DESC", ()),
			localText.getText("TXT_KEY_PITBOSS_PATCH_COMPLETE_TITLE", ()), wx.OK|wx.ICON_EXCLAMATION)

		# Show the dialog and get the response
		if dlg.ShowModal() == wx.ID_OK:
			# They want to restart - tell the app
			PB.installPatch(szPatchName)
		else:
			# Not sure if this can actually happen, but handle it anyway
			bPatchConfirmed = true
			bPatchOK = false

	def OnTextEntered(self, event):
		# Determine what buttons should be enabled
		self.enableButtons()

	def OnPageChanging(self, event):
		# Check direction
		if event.GetDirection():
			# We are trying to move forward - check password
			if ( not PB.login(self.username.GetValue(), self.password.GetValue()) ):
				# Login failed - let the user know
				msg = wx.MessageBox((localText.getText("TXT_KEY_PITBOSS_LOGIN_FAILED", ())),
								(localText.getText("TXT_KEY_PITBOSS_LOGIN_ERROR", ())),	wx.ICON_ERROR)
				# Veto the event to prevent moving forward
				event.Veto()

	def OnPageChanged(self, event):
		global bPatchConfirmed
		global curPage

		# Check for a patch here
		if (not bPatchConfirmed):
			if (not PB.checkPatch()):
				# Error in checking for a patch
				msg = wx.MessageBox(localText.getText("TXT_KEY_PITBOSS_PATCH_CHECK_ERROR_DESC", ()), localText.getText("TXT_KEY_PITBOSS_PATCH_DOWNLOAD_ERROR_TITLE", ()), wx.ICON_ERROR)
				bPatchConfirmed = true

		# Determine what buttons should be enabled
		self.enableButtons()

		# We are the current page
		curPage = self

#
# Load Select Page
#
class LoadSelectPage(wx.wizard.PyWizardPage):
	def __init__(self, parent):
		wx.wizard.PyWizardPage.__init__(self, parent)
		self.next = self.prev = None
		self.myParent = parent

		# Place the radio buttons
		selections = [localText.getText("TXT_KEY_PITBOSS_NEWGAME", ()), localText.getText("TXT_KEY_PITBOSS_SCENARIO", ()), localText.getText("TXT_KEY_PITBOSS_LOADGAME", ())]
		sizer = wx.BoxSizer(wx.VERTICAL)
		self.rb = wx.RadioBox(
					self, -1, (localText.getText("TXT_KEY_PITBOSS_SELECT_INIT", ())), wx.DefaultPosition, wx.DefaultSize,
					selections, 1, wx.RA_SPECIFY_COLS
					)

		self.rb.SetToolTip(wx.ToolTip((localText.getText("TXT_KEY_PITBOSS_SELECT_INIT_HELP", ()))))
		sizer.Add(self.rb, 0, wx.ALL, 5)

		self.SetSizer(sizer)

		self.Bind(wx.wizard.EVT_WIZARD_PAGE_CHANGED, self.OnPageChanged)
		self.Bind(wx.wizard.EVT_WIZARD_PAGE_CHANGING, self.OnPageChanging)

	def enableButtons(self):
		# If the patch state is ok, enable appropriate buttons
		global bPatchConfirmed

		self.myParent.FindWindowById(wx.ID_FORWARD).Enable(bPatchConfirmed)
		self.myParent.FindWindowById(wx.ID_BACKWARD).Enable(bPatchConfirmed)

	def patchAvailable(self, patchName, patchUrl):
		global bPatchConfirmed

		# Put up a dialog
		dlg = wx.MessageDialog(
			self, localText.getText("TXT_KEY_PITBOSS_PATCH_AVAILABLE_DESC", ()),
			localText.getText("TXT_KEY_PITBOSS_PATCH_AVAILABLE_TITLE", ()), wx.YES_NO|wx.ICON_QUESTION)

		# Show the modal dialog and get the response
		if dlg.ShowModal() == wx.ID_YES:
			# They want to download the patch - tell the app
			if (not PB.downloadPatch(patchName, patchUrl)):
				# Patching failed - tell the user
				msg = wx.MessageBox(localText.getText("TXT_KEY_PITBOSS_PATCH_DOWNLOAD_ERROR_DESC", ()), localText.getText("TXT_KEY_PITBOSS_PATCH_DOWNLOAD_ERROR_TITLE", ()), wx.ICON_ERROR)
				bPatchConfirmed = true
				self.enableButtons()
		else:
			# They didn't want to download it, which is ok for LAN games
			bPatchConfirmed = true
			self.enableButtons()

	def patchComplete(self):
		global bPatchConfirmed
		global bPatchOK
		global szPatchName

		# Put up a dialog
		dlg = wx.MessageDialog(
			self, localText.getText("TXT_KEY_PITBOSS_PATCH_COMPLETE_DESC", ()),
			localText.getText("TXT_KEY_PITBOSS_PATCH_COMPLETE_TITLE", ()), wx.OK|wx.CANCEL|wx.ICON_EXCLAMATION)

		# Show the dialog and get the response
		if dlg.ShowModal() == wx.ID_OK:
			# They want to restart - tell the app
			PB.installPatch(szPatchName)
		else:
			# This is ok for LAN games
			bPatchConfirmed = true
			bPatchOK = false

	def OnPageChanged(self, event):
		global curPage

		# If we haven't already, check for a patch
		global bPatchConfirmed
		if (not bPatchConfirmed):
			if (not PB.checkPatch()):
				# Error in checking for a patch
				msg = wx.MessageBox(localText.getText("TXT_KEY_PITBOSS_PATCH_CHECK_ERROR_DESC", ()), localText.getText("TXT_KEY_PITBOSS_PATCH_CHECK_ERROR_TITLE", ()), wx.ICON_ERROR)
				bPatchConfirmed = true

		# Determine what buttons should be enabled
		self.enableButtons()

		curPage = self

	def SetNext(self, next):
		self.next = next

	def SetPrev(self, prev):
		self.prev = prev

	def GetNext(self):
		"Determine which page to display next"
		next = self.next

		if (self.rb.GetSelection() == 0):
			# If it's a new game, skip the scenario selector
			next = next.GetNext()
		if (self.rb.GetSelection() == 2):
			# If it's a loaded game, launch now
			next = None

		return next

	def GetPrev(self):
		return self.prev

	def OnPageChanging(self, event):

		global bSaved
		global bScenario

		# Check direction
		if event.GetDirection():

			# We are trying to move forward - are we trying to init'ing or loading game?
			if (self.rb.GetSelection() == 2):
				# Loading a game - popup the file browser
				bScenario = false
				dlg = wx.FileDialog(
					self, message=(localText.getText("TXT_KEY_PITBOSS_CHOOSE_SAVE", ())), defaultDir=".\saves\multi",
					defaultFile="", wildcard=localText.getText("TXT_KEY_PITBOSS_SAVE_FILES", ("(*.ColonizationSave)", )), style=wx.OPEN
					)

				# Show the modal dialog and get the response
				if dlg.ShowModal() == wx.ID_OK:
					# Get the file name
					path = dlg.GetPath()
					if (path != ""):
						# Prompt for admin password
						dlg = wx.TextEntryDialog(
							self, localText.getText("TXT_KEY_MAIN_MENU_CIV_ADMINPWD_DESC", ()),
							localText.getText("TXT_MAIN_MENU_CIV_PASSWORD_TITLEBAR", ()))

						# Show the modal dialog and get the response
						if dlg.ShowModal() == wx.ID_OK:
							# Check the game name
							adminPwd = dlg.GetValue()

							# We got a save file - try to load the setup info
							iResult = PB.load(path, adminPwd)
							if ( iResult != 0 ):
								# Loading setup info failed.  Clean up and exit
								if (iResult == 1):
									msg = wx.MessageBox((localText.getText("TXT_KEY_PITBOSS_ERROR_LOADING", ())), (localText.getText("TXT_KEY_PITBOSS_LOAD_ERROR", ())), wx.ICON_ERROR )
								elif (iResult == -1):
									msg = wx.MessageBox((localText.getText("TXT_MAIN_MENU_CIV_PASSWORD_RETRY_DESC", ())), (localText.getText("TXT_KEY_BAD_PASSWORD_TITLE", ())), wx.ICON_ERROR )
								PB.reset()
								event.Veto()
							else:
								# Successfully loaded, try hosting
								PB.setLoadFileName(path)
								if ( not PB.host(bPublic, bScenario) ):
									msg = wx.MessageBox( (localText.getText("TXT_KEY_PITBOSS_ERROR_HOSTING", ())), (localText.getText("TXT_KEY_PITBOSS_HOST_ERROR", ())), wx.ICON_ERROR )
									PB.reset()
									event.Veto()
								else:
									bSaved = True
						else:
							# User cancelled admin password
							PB.reset()
							event.Veto()

					else:
						# Didn't get a save file - veto the page change
						event.Veto()

				else:
					#User hit cancel - veto the page change
					event.Veto()

				# Destroy the dialog
				dlg.Destroy()

			else:
				bSaved = false

				# Check to make sure this is a valid option
				if (self.rb.GetSelection() == 0):
					# New game - check maps
					if (PB.getNumMapScripts() == 0):
						msg = wx.MessageBox((localText.getText("TXT_KEY_PITBOSS_NO_MAPS_DESC", ())), (localText.getText("TXT_KEY_PITBOSS_NO_MAPS_TITLE", ())), wx.ICON_EXCLAMATION)
						event.Veto()
						return

				if (self.rb.GetSelection() == 1):
					# New game - check scenarios
					if (PB.getNumScenarios() == 0):
						msg = wx.MessageBox((localText.getText("TXT_KEY_PITBOSS_NO_SCENARIOS_DESC", ())), (localText.getText("TXT_KEY_PITBOSS_NO_SCENARIOS_TITLE", ())), wx.ICON_EXCLAMATION)
						event.Veto()
						return

				# Hosting a new game - pop the gamename dialog
				dlg = wx.TextEntryDialog(
					self, localText.getText("TXT_KEY_PITBOSS_NAME_GAME_DESC", ()),
					localText.getText("TXT_KEY_PITBOSS_NAME_GAME_TITLE", ()))

				# Show the modal dialog and get the response
				if dlg.ShowModal() == wx.ID_OK:
					# Check the game name
					gamename = dlg.GetValue()
					if (gamename != ""):
						# We got a gamename, save it here
						PB.setGamename(gamename)

						# Prompt for passwords in public games
						bOK = (not bPublic)
						if bPublic:
							dlg = wx.TextEntryDialog(
								self, localText.getText("TXT_KEY_PITBOSS_PWD_GAME_DESC", ()),
								localText.getText("TXT_KEY_PITBOSS_PWD_GAME_TITLE", ()))

							if ( dlg.ShowModal() == wx.ID_OK ):
								bOK = true
								PB.setGamePassword( dlg.GetValue() )

						if bOK:
							# If we are starting a new game, host
							if (self.rb.GetSelection() == 0):
								bScenario = false
								if ( not PB.host(bPublic, bScenario) ):
									# Hosting failed for some reason.  Clean up and exit
									msg = wx.MessageBox((localText.getText("TXT_KEY_PITBOSS_ERROR_HOSTING", ())), (localText.getText("TXT_KEY_PITBOSS_HOST_ERROR", ())), wx.ICON_ERROR)
									PB.reset()
									event.Veto()
						else:
							# User hit cancel
							event.Veto()

					else:
						# Malicious user didn't enter a gamename...
						event.Veto()

				else:
					# User hit cancel
					event.Veto()

				dlg.Destroy()

		else:
			# We are moving backward - reset the network layer
			PB.reset()
			PB.logout()

#
# Scenario Selection page (optional 4th page)
#
class ScenarioSelectPage(wx.wizard.WizardPageSimple):
	def __init__(self, parent):
		wx.wizard.WizardPageSimple.__init__(self, parent)

		self.myParent = parent
		pageSizer = wx.BoxSizer(wx.VERTICAL)

		scenarioPanel = wx.lib.scrolledpanel.ScrolledPanel(self, -1, size=(300, 600), style = wx.SUNKEN_BORDER)
		sizer = wx.BoxSizer(wx.VERTICAL)

		header = wx.StaticText(self, -1, localText.getText("TXT_KEY_PITBOSS_CHOOSE_SCENARIO", ()))
		pageSizer.Add(header, 0, wx.ALL, 5)

		# Place the radio buttons
		self.rbs = []
		index = 0
		for index in range(PB.getNumScenarios()):
			# We need to start a group on the first one
			if (index == 0):
				self.rbs.append( wx.RadioButton(
					scenarioPanel, -1, PB.getScenarioAt(index), wx.DefaultPosition, wx.DefaultSize, wx.RB_GROUP
					) )
			else:
				self.rbs.append( wx.RadioButton(
					scenarioPanel, -1, PB.getScenarioAt(index), wx.DefaultPosition, wx.DefaultSize
					) )

			sizer.Add(self.rbs[index], 0, wx.ALL, 3)

		scenarioPanel.SetSizer(sizer)
		scenarioPanel.SetAutoLayout(1)
		scenarioPanel.SetupScrolling()

		pageSizer.Add(scenarioPanel, 0, wx.ALL, 5)

		self.SetSizer(pageSizer)

		self.Bind(wx.wizard.EVT_WIZARD_PAGE_CHANGED, self.OnPageChanged)
		self.Bind(wx.wizard.EVT_WIZARD_PAGE_CHANGING, self.OnPageChanging)

	def enableButtons(self):
		self.myParent.FindWindowById(wx.ID_FORWARD).Enable(True)
		self.myParent.FindWindowById(wx.ID_BACKWARD).Enable(True)

	def OnPageChanged(self, event):
		global curPage

		# Determine what buttons should be enabled
		self.enableButtons()
		curPage = self

	def OnPageChanging(self, event):
		global bPublic
		global bScenario

		# Check direction
		if event.GetDirection():
			# Determine our selection
			iSelection = 0
			while (not self.rbs[iSelection].GetValue() and iSelection < PB.getNumScenarios()):
				iSelection = iSelection+1

			# We are trying to move forward - Set the selected scenario
			if ( PB.loadScenarioInfo(PB.getScenarioAt(iSelection)) ):
				bScenario = true
				if ( not PB.host(bPublic, bScenario) ):
					# Hosting failed for some reason.  Clean up and exit
					msg = wx.MessageBox((localText.getText("TXT_KEY_PITBOSS_ERROR_HOSTING", ())), (localText.getText("TXT_KEY_PITBOSS_HOST_ERROR", ())), wx.ICON_ERROR)
					PB.reset()
					event.Veto()
			else:
				# Loading the scenario failed
					msg = wx.MessageBox((localText.getText("TXT_KEY_PITBOSS_SCENARIO_ERROR", ())), (localText.getText("TXT_KEY_PITBOSS_SCENARIO_ERROR_TITLE", ())), wx.ICON_ERROR)
					PB.reset()
					event.Veto()
		else:
			# We are moving backward
			PB.reset()



#
# Staging room (last page before launch)
#
class StagingPage(wx.wizard.WizardPageSimple):
	def __init__(self, parent):
		wx.wizard.WizardPageSimple.__init__(self, parent)
		self.myParent = parent

		# Get the game info struct
		gameData = PB.getGameSetupData()

		# Create our array of controls
		self.optionArray = []
		self.mpOptionArray = []
		self.victoriesArray = []
		self.whoArray = []
		self.civArray = []
		self.leaderArray = []
		self.teamArray = []
		self.diffArray = []
		self.statusArray = []

		# Declare storage arrays
		self.customItemSizerArray = []
		self.customMapTextArray = []
		self.customMapOptionArray = []

		# Build the initial selections
		# Map
		mapNameList = []
		rowNum = 0
		for rowNum in range(PB.getNumMapScripts()):
			mapNameList.append((PB.getMapNameAt(rowNum)))

		# World size
		sizeList = []
		rowNum = 0
		for rowNum in range(PB.getNumSizes()):
			sizeList.append((PB.getSizeAt(rowNum)))

		# Climate
		climateList = []
		rowNum = 0
		for rowNum in range(PB.getNumClimates()):
			climateList.append((PB.getClimateAt(rowNum)))

		# Sealevel
		seaLevelList = []
		rowNum = 0
		for rowNum in range(PB.getNumSeaLevels()):
			seaLevelList.append((PB.getSeaLevelAt(rowNum)))

		# Era
		eraList = []
		rowNum = 0
		for rowNum in range(PB.getNumEras()):
			eraList.append((PB.getEraAt(rowNum)))

		# Game speed
		speedList = []
		rowNum = 0
		for rowNum in range(PB.getNumSpeeds()):
			speedList.append((PB.getSpeedAt(rowNum)))

		# Options
		optionList = []
		rowNum = 0
		for rowNum in range(PB.getNumOptions()):
			optionList.append((PB.getOptionDescAt(rowNum)))

		# Create the master page sizer
		self.pageSizer = wx.BoxSizer(wx.VERTICAL)

		# Create the game options area
		masterBorder = wx.StaticBox(self, -1, ((localText.getText("TXT_KEY_PITBOSS_GAME_SETUP", ()))))
		self.optionsSizer = wx.StaticBoxSizer(masterBorder, wx.HORIZONTAL)

		# Create the drop down side
		settingsBorder = wx.StaticBox(self, -1, ((localText.getText("TXT_KEY_PITBOSS_GAME_SETTINGS", ()))))
		self.dropDownSizer = wx.StaticBoxSizer(settingsBorder, wx.VERTICAL)

		# Create label/control pairs for map
		itemSizer = wx.BoxSizer(wx.VERTICAL)
		txt = wx.StaticText(self, -1, (localText.getText("TXT_KEY_PITBOSS_MAP", ())))
		self.mapChoice = wx.Choice(self, -1, (-1,-1), choices=mapNameList)
		self.mapChoice.SetStringSelection(gameData.getMapName())
		itemSizer.Add(txt)
		itemSizer.Add(self.mapChoice)
		self.dropDownSizer.Add(itemSizer, 0, wx.TOP, 3)
		self.Bind(wx.EVT_CHOICE, self.OnGameChoice, self.mapChoice)


		# Create label/control pairs for size
		itemSizer = wx.BoxSizer(wx.VERTICAL)
		txt = wx.StaticText(self, -1, (localText.getText("TXT_KEY_PITBOSS_SIZE", ())))
		self.sizeChoice = wx.Choice(self, -1, (-1,-1), choices=sizeList)
		self.sizeChoice.SetSelection(gameData.iSize)
		itemSizer.Add(txt)
		itemSizer.Add(self.sizeChoice)
		self.dropDownSizer.Add(itemSizer, 0, wx.TOP, 3)
		self.Bind(wx.EVT_CHOICE, self.OnGameChoice, self.sizeChoice)

		# Create label/control pairs for climate
		itemSizer = wx.BoxSizer(wx.VERTICAL)
		txt = wx.StaticText(self, -1, (localText.getText("TXT_KEY_PITBOSS_CLIMATE", ())))
		self.climateChoice = wx.Choice(self, -1, (-1,-1), choices=climateList)
		self.climateChoice.SetSelection(gameData.iClimate)
		itemSizer.Add(txt)
		itemSizer.Add(self.climateChoice)
		self.dropDownSizer.Add(itemSizer, 0, wx.TOP, 3)
		self.Bind(wx.EVT_CHOICE, self.OnGameChoice, self.climateChoice)

		# Create label/control pairs for sealevel
		itemSizer = wx.BoxSizer(wx.VERTICAL)
		txt = wx.StaticText(self, -1, (localText.getText("TXT_KEY_PITBOSS_SEALEVEL", ())))
		self.seaLevelChoice = wx.Choice(self, -1, (-1,-1), choices=seaLevelList)
		self.seaLevelChoice.SetSelection(gameData.iSeaLevel)
		itemSizer.Add(txt)
		itemSizer.Add(self.seaLevelChoice)
		self.dropDownSizer.Add(itemSizer, 0, wx.TOP, 3)
		self.Bind(wx.EVT_CHOICE, self.OnGameChoice, self.seaLevelChoice)

		# Create label/control pairs for era
		itemSizer = wx.BoxSizer(wx.VERTICAL)
		txt = wx.StaticText(self, -1, (localText.getText("TXT_KEY_PITBOSS_ERA", ())))
		self.eraChoice = wx.Choice(self, -1, (-1,-1), choices=eraList)
		self.eraChoice.SetSelection(gameData.iEra)
		itemSizer.Add(txt)
		itemSizer.Add(self.eraChoice)
		self.dropDownSizer.Add(itemSizer, 0, wx.TOP, 3)
		self.Bind(wx.EVT_CHOICE, self.OnGameChoice, self.eraChoice)

		# Create label/control pairs for speed
		itemSizer = wx.BoxSizer(wx.VERTICAL)
		txt = wx.StaticText(self, -1, (localText.getText("TXT_KEY_PITBOSS_SPEED", ())))
		self.speedChoice = wx.Choice(self, -1, (-1,-1), choices=speedList)
		self.speedChoice.SetSelection(gameData.iSpeed)
		itemSizer.Add(txt)
		itemSizer.Add(self.speedChoice)
		self.dropDownSizer.Add(itemSizer, 0, wx.TOP, 3)
		self.Bind(wx.EVT_CHOICE, self.OnGameChoice, self.speedChoice)

		# Create label/control pairs for custom map options
		self.buildCustomMapOptions( gameData.getMapName() )

		self.optionsSizer.Add(self.dropDownSizer, 0, wx.RIGHT, 10)

		# Create the multiplayer option column
		centerSizer = wx.BoxSizer(wx.VERTICAL)

		mpOptionsBorder = wx.StaticBox(self, -1, ((localText.getText("TXT_KEY_PITBOSS_GAME_MPOPTIONS", ()))))
		mpOptionsSizer = wx.StaticBoxSizer(mpOptionsBorder, wx.VERTICAL)

		# Create and add Multiplayer option checkboxes
		rowNum = 0
		for rowNum in range(PB.getNumMPOptions()):
			mpCheckBox = wx.CheckBox(self, (rowNum+1000), (PB.getMPOptionDescAt(rowNum)))
			mpCheckBox.SetValue(gameData.getMPOptionAt(rowNum))
			mpOptionsSizer.Add(mpCheckBox, 0, wx.TOP, 5)
			self.mpOptionArray.append(mpCheckBox)
			self.Bind(wx.EVT_CHECKBOX, self.OnOptionChoice, mpCheckBox)

		# Entry box to set turn timer time
		timerOutputSizer = wx.BoxSizer(wx.HORIZONTAL)
		timerPreText = wx.StaticText(self, -1, localText.getText("TXT_KEY_PITBOSS_TURNTIMER_A", ()))
		self.turnTimerEdit = wx.TextCtrl(self, -1, str(gameData.iTurnTime), size=(30,-1))
		timerPostText = wx.StaticText(self, -1, localText.getText("TXT_KEY_PITBOSS_TURNTIMER_B", ()))
		timerOutputSizer.Add(timerPreText, 0, wx.TOP, 5)
		timerOutputSizer.Add(self.turnTimerEdit, 0, wx.TOP, 5)
		timerOutputSizer.Add(timerPostText, 0, wx.TOP, 5)
		self.Bind(wx.EVT_TEXT, self.OnTurnTimeEntered, self.turnTimerEdit)

		mpOptionsSizer.Add(timerOutputSizer, 0, wx.ALL, 5)

		# Entry box for game turn limit
		maxTurnsSizer = wx.BoxSizer(wx.HORIZONTAL)
		maxTurnsText = wx.StaticText(self, -1, localText.getText("TXT_KEY_PITBOSS_MAX_TURN", ()))
		self.maxTurnsEdit = wx.TextCtrl(self, -1, str(gameData.iMaxTurns), size=(30,-1))
		maxTurnsSizer.Add(maxTurnsText, 0, wx.TOP, 5)
		maxTurnsSizer.Add(self.maxTurnsEdit, 0, wx.TOP, 5)
		self.Bind(wx.EVT_TEXT, self.OnMaxTurnsEntered, self.maxTurnsEdit)

		mpOptionsSizer.Add(maxTurnsSizer, 0, wx.ALL, 5)

		# Entry box for city elimination limit
		cityEliminationSizer = wx.BoxSizer(wx.HORIZONTAL)
		cityEliminationText = wx.StaticText(self, -1, localText.getText("TXT_KEY_PITBOSS_CITY_ELIMINATION", ()))
		self.cityEliminationEdit = wx.TextCtrl(self, -1, str(gameData.iCityElimination), size=(30,-1))
		cityEliminationSizer.Add(cityEliminationText, 0, wx.TOP, 5)
		cityEliminationSizer.Add(self.cityEliminationEdit, 0, wx.TOP, 5)
		self.Bind(wx.EVT_TEXT, self.OnCityEliminationEntered, self.cityEliminationEdit)

		mpOptionsSizer.Add(cityEliminationSizer, 0, wx.ALL, 5)

		centerSizer.Add(mpOptionsSizer, 0, wx.ALIGN_CENTER_HORIZONTAL|wx.LEFT|wx.RIGHT|wx.BOTTOM, 5)

		victoriesBorder = wx.StaticBox(self, -1, ((localText.getText("TXT_KEY_PITBOSS_GAME_VICTORIES", ()))))
		victoriesSizer = wx.StaticBoxSizer(victoriesBorder, wx.VERTICAL)

		# Create and add Victory option checkboxes
		rowNum = 0
		for rowNum in range(PB.getNumVictories()):
			victoryCheckBox = wx.CheckBox(self, (rowNum+2000), (PB.getVictoryDescAt(rowNum)))
			victoryCheckBox.SetValue(gameData.getVictory(rowNum))
			victoriesSizer.Add(victoryCheckBox, 0, wx.TOP, 5)
			self.victoriesArray.append(victoryCheckBox)
			self.Bind(wx.EVT_CHECKBOX, self.OnOptionChoice, victoryCheckBox)

		centerSizer.Add(victoriesSizer, 0, wx.ALIGN_CENTER_HORIZONTAL|wx.ALL, 5)

		# Entry box for admin password
		itemSizer = wx.BoxSizer(wx.VERTICAL)
		txt = wx.StaticText(self, -1, (localText.getText("TXT_KEY_POPUP_ADMIN_PASSWORD", ())))
		self.adminPasswordEdit = wx.TextCtrl(self, -1, "", size=(100,-1))
		itemSizer.Add(txt)
		itemSizer.Add(self.adminPasswordEdit)
		mpOptionsSizer.Add(itemSizer, 0, wx.TOP, 5)
		self.Bind(wx.EVT_TEXT, self.OnAdminPasswordEntered, self.adminPasswordEdit)

		self.optionsSizer.Add(centerSizer, 0, wx.ALIGN_CENTER_HORIZONTAL)

		# Create the CheckBox side
		optionsBorder = wx.StaticBox(self, -1, ((localText.getText("TXT_KEY_PITBOSS_GAME_OPTIONS", ()))))
		checkBoxSizer = wx.StaticBoxSizer(optionsBorder, wx.VERTICAL)

		# Create and add the Options checkboxes
		rowNum = 0
		for rowNum in range(PB.getNumOptions()):
			checkBox = wx.CheckBox(self, rowNum, (PB.getOptionDescAt(rowNum)))
			checkBox.SetValue(gameData.getOptionAt(rowNum))
			checkBoxSizer.Add(checkBox, 0, wx.TOP, 5)
			self.optionArray.append(checkBox)
			self.Bind(wx.EVT_CHECKBOX, self.OnOptionChoice, checkBox)

		self.optionsSizer.Add(checkBoxSizer, 0, wx.LEFT, 10)

		# Entry box for number of advanced start points
		advancedStartPointsSizer = wx.BoxSizer(wx.HORIZONTAL)
		advancedStartPointsText = wx.StaticText(self, -1, localText.getText("TXT_KEY_ADVANCED_START_POINTS", ()))
		self.advancedStartPointsEdit = wx.TextCtrl(self, -1, str(gameData.iAdvancedStartPoints), size=(50,-1))
		advancedStartPointsSizer.Add(advancedStartPointsText, 0, wx.TOP, 5)
		advancedStartPointsSizer.Add(self.advancedStartPointsEdit, 0, wx.TOP, 5)
		self.Bind(wx.EVT_TEXT, self.OnAdvancedStartPointsEntered, self.advancedStartPointsEdit)

		mpOptionsSizer.Add(advancedStartPointsSizer, 0, wx.ALL, 5)

		# Add our options box to the page
		self.pageSizer.Add(self.optionsSizer, 0, wx.ALIGN_CENTER_HORIZONTAL|wx.ALL, 5)

		# Slot status - choices are static
		slotStatusList = [localText.getText("TXT_KEY_PITBOSS_HUMAN", ()), localText.getText("TXT_KEY_PITBOSS_COMPUTER", ()), localText.getText("TXT_KEY_PITBOSS_CLOSED", ())]

		# Civilizations - get from app
		civList = []
		civList.append(localText.getText("TXT_KEY_PITBOSS_RANDOM", ()))
		for rowNum in range(gc.getNumCivilizationInfos()):
			if gc.getCivilizationInfo(rowNum).isPlayable():
				civList.append(gc.getCivilizationInfo(rowNum).getShortDescription(0))

		leaderList = [localText.getText("TXT_KEY_PITBOSS_RANDOM", ())]

		teamList = []
		rowNum = 0
		for rowNum in range(gc.getMAX_CIV_PLAYERS()):
			teamList.append(str(rowNum+1))

		# Handicaps - get from app
		diffList = []
		rowNum = 0
		for rowNum in range(PB.getNumHandicaps()):
			diffList.append((PB.getHandicapAt(rowNum)))

		playerPanel = wx.lib.scrolledpanel.ScrolledPanel(self, -1, size=(425, 300), style = wx.SUNKEN_BORDER)
		panelSizer = wx.BoxSizer(wx.VERTICAL)

		# Create a row - enough for the max players in a Pitboss game
		rowNum = 0
		for rowNum in range(gc.getMAX_CIV_PLAYERS()):
			# Create the border box
			border = wx.StaticBox(playerPanel, -1, (localText.getText("TXT_KEY_PITBOSS_PLAYER", (rowNum+1, ))), (0,(rowNum*30)))
			# Create the layout mgr
			rowSizer = wx.StaticBoxSizer(border, wx.HORIZONTAL)

			# Get the info struct
			playerData = PB.getPlayerSetupData(rowNum)

			# Slot status dropdown
			itemSizer = wx.BoxSizer(wx.VERTICAL)
			txt = wx.StaticText(playerPanel, -1, (localText.getText("TXT_KEY_PITBOSS_WHO", ())))
			dropDown = wx.Choice(playerPanel, rowNum, (-1,-1), choices=slotStatusList)
			dropDown.SetSelection(playerData.iWho)
			itemSizer.Add(txt)
			itemSizer.Add(dropDown)
			rowSizer.Add(itemSizer, 0, wx.TOP, 3)
			self.whoArray.append(dropDown)
			self.Bind(wx.EVT_CHOICE, self.OnPlayerChoice, dropDown)

			# Civ dropdown
			itemSizer = wx.BoxSizer(wx.VERTICAL)
			txt = wx.StaticText(playerPanel, -1, (localText.getText("TXT_KEY_PITBOSS_CIV", ())))
			dropDown = wx.Choice(playerPanel, rowNum, (-1,-1), choices=civList)
			dropDown.SetSelection(playerData.iCiv+1)
			itemSizer.Add(txt)
			itemSizer.Add(dropDown)
			rowSizer.Add(itemSizer, 0, wx.TOP, 3)
			self.civArray.append(dropDown)
			self.Bind(wx.EVT_CHOICE, self.OnPlayerChoice, dropDown)

			# Leader dropdown
			itemSizer = wx.BoxSizer(wx.VERTICAL)
			txt = wx.StaticText(playerPanel, -1, (localText.getText("TXT_KEY_PITBOSS_LEADER", ())))
			dropDown = wx.Choice(playerPanel, rowNum, (-1,-1), choices=leaderList)
			dropDown.SetSelection(playerData.iLeader+1)
			itemSizer.Add(txt)
			itemSizer.Add(dropDown)
			rowSizer.Add(itemSizer, 0, wx.TOP, 3)
			self.leaderArray.append(dropDown)
			self.Bind(wx.EVT_CHOICE, self.OnPlayerChoice, dropDown)

			# Team dropdown
			itemSizer = wx.BoxSizer(wx.VERTICAL)
			txt = wx.StaticText(playerPanel, -1, (localText.getText("TXT_KEY_PITBOSS_TEAM", ())))
			dropDown = wx.Choice(playerPanel, rowNum, (-1,-1), choices=teamList)
			dropDown.SetSelection(playerData.iTeam)
			itemSizer.Add(txt)
			itemSizer.Add(dropDown)
			rowSizer.Add(itemSizer, 0, wx.TOP, 3)
			self.teamArray.append(dropDown)
			self.Bind(wx.EVT_CHOICE, self.OnPlayerChoice, dropDown)

			# Difficulty dropdown
			itemSizer = wx.BoxSizer(wx.VERTICAL)
			txt = wx.StaticText(playerPanel, -1, (localText.getText("TXT_KEY_PITBOSS_DIFFICULTY", ())))
			dropDown = wx.Choice(playerPanel, rowNum, (-1,-1), choices=diffList)
			dropDown.SetSelection(playerData.iDifficulty)
			itemSizer.Add(txt)
			itemSizer.Add(dropDown)
			rowSizer.Add(itemSizer, 0, wx.TOP, 3)
			self.diffArray.append(dropDown)
			self.Bind(wx.EVT_CHOICE, self.OnPlayerChoice, dropDown)

			# Ready status
			itemSizer = wx.BoxSizer(wx.VERTICAL)
			txt = wx.StaticText(playerPanel, -1, (localText.getText("TXT_KEY_PITBOSS_STATUS", ())))
			statusTxt = wx.StaticText(playerPanel, rowNum, playerData.getStatusText())
			itemSizer.Add(txt)
			itemSizer.Add(statusTxt)
			rowSizer.Add(itemSizer, 0, wx.ALL, 5)
			self.statusArray.append(statusTxt)

			# Add row to page Sizer
			panelSizer.Add(rowSizer, 0, wx.ALL, 5)

		playerPanel.SetSizer(panelSizer)
		playerPanel.SetAutoLayout(1)
		playerPanel.SetupScrolling()

		self.pageSizer.Add(playerPanel, 0, wx.ALIGN_CENTER_HORIZONTAL|wx.ALL, 5)

		self.leaderRefresh = False

		self.Bind(wx.wizard.EVT_WIZARD_PAGE_CHANGED, self.OnPageChanged)
		self.Bind(wx.wizard.EVT_WIZARD_PAGE_CHANGING, self.OnPageChanging)

		self.SetSizer(self.pageSizer)

	def enableButtons(self):
		self.myParent.FindWindowById(wx.ID_FORWARD).Enable(True)
		self.myParent.FindWindowById(wx.ID_BACKWARD).Enable(True)

	def OnGameChoice(self, event):
		self.ChangeGameParam()

	def ChangeGameParam(self):
		maxTurnsValue = 0
		cityEliminationValue = 0
		advancedStartPointsValue = 0
		turnTimerValue = 0

		strValue = self.maxTurnsEdit.GetValue()
		if (len(strValue) > 0):
			maxTurnsValue = (int)(self.maxTurnsEdit.GetValue())

		strValue = self.cityEliminationEdit.GetValue()
		if (len(strValue) > 0):
			cityEliminationValue = (int)(self.cityEliminationEdit.GetValue())

		strValue = self.advancedStartPointsEdit.GetValue()
		if (len(strValue) > 0):
			advancedStartPointsValue = (int)(self.advancedStartPointsEdit.GetValue())

		strValue = self.turnTimerEdit.GetValue()
		if (len(strValue) > 0):
			turnTimerValue = (int)(self.turnTimerEdit.GetValue())

		PB.gameParamChanged( self.mapChoice.GetStringSelection(), self.sizeChoice.GetSelection(),
			self.climateChoice.GetSelection(), self.seaLevelChoice.GetSelection(),
			self.eraChoice.GetSelection(), self.speedChoice.GetSelection(), maxTurnsValue, cityEliminationValue,
			advancedStartPointsValue, turnTimerValue, self.adminPasswordEdit.GetValue() )

	def OnCustomMapOptionChoice(self, event):
		# Get the option ID
		optionID = ( (event.GetId()/100) - 1)
		PB.customMapOptionChanged( optionID, self.customMapOptionArray[optionID].GetSelection() )

	def IsNumericString(self, myStr):
		for myChar in myStr:
			if myChar not in string.digits:
				return False
		return True

	def OnMaxTurnsEntered(self, event):
		# Check to see if there is a turn string
		if ( (self.maxTurnsEdit.GetValue() != "")  ):
			# There is, make sure it's a number
			if ( not self.IsNumericString(self.maxTurnsEdit.GetValue()) ):
				# It's not - lay the smack down
				dlg = wx.MessageDialog(
					self, localText.getText("TXT_KEY_PITBOSS_MAXTURN_ERROR_DESC", ()),
					localText.getText("TXT_KEY_PITBOSS_MAXTURN_ERROR_TITLE", ()), wx.OK|wx.ICON_EXCLAMATION)

				# Show the modal dialog and get the response
				if dlg.ShowModal() == wx.ID_OK:
					# Clear out the MaxTurns Edit box
					self.maxTurnsEdit.SetValue("")
			else:
				# It's a number
				self.ChangeGameParam()
		else:
			# It's been cleared
			self.ChangeGameParam()

	def OnCityEliminationEntered(self, event):
		# Check to see if there is an elimination string
		if ( (self.cityEliminationEdit.GetValue() != "")  ):
			# There is, make sure it's a number
			if ( not self.IsNumericString(self.cityEliminationEdit.GetValue()) ):
				# It's not - lay the smack down
				dlg = wx.MessageDialog(
					self, localText.getText("TXT_KEY_PITBOSS_CITYELIMINATION_ERROR_DESC", ()),
					localText.getText("TXT_KEY_PITBOSS_CITYELIMINATION_ERROR_TITLE", ()), wx.OK|wx.ICON_EXCLAMATION)

				# Show the modal dialog and get the response
				if dlg.ShowModal() == wx.ID_OK:
					# Clear out the MaxTurns Edit box
					self.cityEliminationEdit.SetValue("")
			else:
				# It's a number
				self.ChangeGameParam()
		else:
			# It's been cleared
			self.ChangeGameParam()

	def OnAdvancedStartPointsEntered(self, event):
		# Check to see if there is an string
		if ( (self.advancedStartPointsEdit.GetValue() != "")  ):
			# There is, make sure it's a number
			if ( not self.IsNumericString(self.advancedStartPointsEdit.GetValue()) ):
				# It's not - lay the smack down
				dlg = wx.MessageDialog(
					self, localText.getText("TXT_KEY_PITBOSS_CITYELIMINATION_ERROR_DESC", ()),
					localText.getText("TXT_KEY_PITBOSS_CITYELIMINATION_ERROR_TITLE", ()), wx.OK|wx.ICON_EXCLAMATION)

				# Show the modal dialog and get the response
				if dlg.ShowModal() == wx.ID_OK:
					# Clear out the MaxTurns Edit box
					self.advancedStartPointsEdit.SetValue("")
			else:
				# It's a number
				self.ChangeGameParam()
		else:
			# It's been cleared
			self.ChangeGameParam()

	def OnTurnTimeEntered(self, event):
		# Check to see if there is a time string
		if ( (self.turnTimerEdit.GetValue() != "")  ):
			# There is, make sure it's a number
			if ( not self.IsNumericString(self.turnTimerEdit.GetValue()) ):
				# It's not - lay the smack down
				dlg = wx.MessageDialog(
					self, localText.getText("TXT_KEY_PITBOSS_TURNTIMER_ERROR_DESC", ()),
					localText.getText("TXT_KEY_PITBOSS_TURNTIMER_ERROR_TITLE", ()), wx.OK|wx.ICON_EXCLAMATION)

				# Show the modal dialog and get the response
				if dlg.ShowModal() == wx.ID_OK:
					# Clear out the TurnTimer Edit box
					self.turnTimerEdit.SetValue("")
			else:
				# It's a number
				self.ChangeGameParam()
		else:
			# It's been cleared
			self.ChangeGameParam()

	def OnAdminPasswordEntered(self, event):
		self.ChangeGameParam()

	def OnOptionChoice(self, event):
		# Get the option ID
		optionID = event.GetId()

		# Values >= 2000 are victories
		if  (optionID >= 2000):
			PB.victoriesChanged( (optionID-2000), self.victoriesArray[(optionID-2000)].GetValue() )
		# Values >= 1000 are MP options
		elif  (optionID >= 1000):
			PB.mpOptionChanged( (optionID-1000), self.mpOptionArray[(optionID-1000)].GetValue() )
		else:
			PB.gameOptionChanged( optionID, self.optionArray[optionID].GetValue() )

		bEnable = PB.getTurnTimer()
		self.turnTimerEdit.Enable(bEnable)

	def OnPlayerChoice(self, event):
		# Get the row for the player modified
		rowNum = event.GetId()

		# See if the slot status is valid
		if (bScenario and not PB.getNoPlayersScenario()):
			if (PB.getWho(rowNum) != self.whoArray[rowNum].GetSelection()):
				# Closed status is not permitted - change to AI
				if (self.whoArray[rowNum].GetSelection() == 2):
					self.whoArray[rowNum].SetSelection(1)

		# See if we need to update the leader box
		if (not self.leaderRefresh):
			self.leaderRefresh = ( PB.getCiv(rowNum) != (self.civArray[rowNum].GetSelection()-1) );

		PB.playerParamChanged( rowNum, self.whoArray[rowNum].GetSelection(), self.civArray[rowNum].GetSelection()-1, self.teamArray[rowNum].GetSelection(),
			self.diffArray[rowNum].GetSelection(), PB.getGlobalLeaderIndex(self.civArray[rowNum].GetSelection()-1, self.leaderArray[rowNum].GetSelection()-1) )

	def OnPageChanging(self, event):
		# Check direction
		if (not event.GetDirection()):
			# We are trying to move backward - reset the network resources
			PB.reset()

	def OnPageChanged(self, event):
		global curPage

		# Determine what buttons should be enabled
		self.enableButtons()
		self.setDefaults()

		# We are the current page
		curPage = self

	def setDefaults(self):
		# Display the current initialization information
		global bSaved
		global bScenario

		# Get game data first
		PB.resetAdvancedStartPoints()
		gameData = PB.getGameSetupData()

		self.refreshCustomMapOptions(gameData.getMapName())

		# Set the selections currently in our init structure
		if (self.mapChoice.FindString(gameData.getMapName()) == wx.NOT_FOUND):
			self.mapChoice.Append(gameData.getMapName())
		self.mapChoice.SetStringSelection(gameData.getMapName())
		self.mapChoice.Enable(not bSaved and not bScenario)

		self.sizeChoice.SetSelection(gameData.iSize)
		self.sizeChoice.Enable(not bSaved and not bScenario)

		self.climateChoice.SetSelection(gameData.iClimate)
		self.climateChoice.Enable(not bSaved and not bScenario)

		self.seaLevelChoice.SetSelection(gameData.iSeaLevel)
		self.seaLevelChoice.Enable(not bSaved and not bScenario)

		self.eraChoice.SetSelection(gameData.iEra)
		self.eraChoice.Enable(not bSaved and not bScenario)

		self.speedChoice.SetSelection(gameData.iSpeed)
		self.speedChoice.Enable(not bSaved and not PB.forceSpeed())

		self.maxTurnsEdit.SetValue(str(gameData.iMaxTurns))
		self.maxTurnsEdit.Enable(not bSaved and not PB.forceMaxTurns())

		self.cityEliminationEdit.SetValue(str(gameData.iCityElimination))
		self.cityEliminationEdit.Enable(not bSaved and not PB.forceCityElimination())

		self.advancedStartPointsEdit.SetValue(str(gameData.iAdvancedStartPoints))
		self.advancedStartPointsEdit.Enable(not bSaved and not PB.forceAdvancedStart())

		self.turnTimerEdit.SetValue(str(gameData.iTurnTime))
		if (not bSaved):
			bEnable = PB.getTurnTimer()
			self.turnTimerEdit.Enable(bEnable)
		else:
			self.turnTimerEdit.Disable()

		# Set selections of map options
		optionNum = 0
		for optionNum in range(PB.getNumCustomMapOptions(gameData.getMapName())):
			self.customMapOptionArray[optionNum].SetSelection(gameData.getCustomMapOption(optionNum))
			self.customMapOptionArray[optionNum].Enable(not bSaved and not bScenario)

		# set the mp options selection
		rowNum = 0
		for rowNum in range(PB.getNumMPOptions()):
			self.mpOptionArray[rowNum].SetValue(gameData.getMPOptionAt(rowNum))
			self.mpOptionArray[rowNum].Enable(not bSaved)

		# set the victories selected
		rowNum = 0
		for rowNum in range(PB.getNumVictories()):
			self.victoriesArray[rowNum].SetValue(gameData.getVictory(rowNum))
			self.victoriesArray[rowNum].Enable(not bSaved and not PB.forceVictories() and not PB.isPermanentVictory(rowNum))

		# Set the options selected
		rowNum = 0
		for rowNum in range(PB.getNumOptions()):
			self.optionArray[rowNum].SetValue(gameData.getOptionAt(rowNum))
			self.optionArray[rowNum].Enable(not bSaved and not PB.forceOptions() and PB.isOptionValid(rowNum))

		# Have the app suggest number of players based on map size
		PB.suggestPlayerSetup()

		rowNum = 0
		for rowNum in range(gc.getMAX_CIV_PLAYERS()):
			# Get the player data
			playerData = PB.getPlayerSetupData(rowNum)

			# We may need to add/remove items from who box
			self.refreshWhoBox(rowNum, playerData.iWho)
			self.whoArray[rowNum].SetSelection(playerData.iWho)
			if (playerData.iWho == 1):	# AI
				self.whoArray[rowNum].Enable( not bSaved and PB.isPlayableCiv(rowNum) )

			# Civ choices are static inside the instance
			civChoice = playerData.iCiv+1
			self.civArray[rowNum].SetSelection(civChoice)
			self.civArray[rowNum].Enable( not bSaved and (not bScenario or PB.getNoPlayersScenario()) )

			# We may need to add/remove items from the leader box
			self.refreshLeaderBox(rowNum, playerData.iCiv)
			self.leaderRefresh = False
			self.leaderArray[rowNum].SetSelection(PB.getCivLeaderIndex(civChoice-1, playerData.iLeader)+1)
			self.leaderArray[rowNum].Enable( not bSaved and (not bScenario or PB.getNoPlayersScenario()) )

			# Team choices are static
			self.teamArray[rowNum].SetSelection(playerData.iTeam)
			self.teamArray[rowNum].Enable( not bSaved and (not bScenario or PB.getNoPlayersScenario()) )

			# Difficulty choices are static
			self.diffArray[rowNum].SetSelection(playerData.iDifficulty)
			self.diffArray[rowNum].Enable( not bSaved and not PB.forceDifficulty() )

			# Status is static
			self.statusArray[rowNum].SetLabel(playerData.getStatusText())


	def refreshRow(self, iRow):

		global bSaved

		# Disable finish button if all players not ready to start
		bAllReady = True

		# Don't wait for ready's if we're loading
		if (not bSaved):
			index = 0
			for index in range(gc.getMAX_CIV_PLAYERS()):
				if (PB.getWho(index) == 3): # If a row is taken by a human
					if (not PB.getReady(index)): # If this human is not ready for the event to begin
						# Don't allow a launch
						bAllReady = False
						break
			if (bAllReady and PB.isPendingInit()):
				bAllReady = False


		self.myParent.FindWindowById(wx.ID_FORWARD).Enable(bAllReady)

		# Get information from the app for this row
		playerData = PB.getPlayerSetupData(iRow)

		# Refresh the choices in this slot
		self.refreshWhoBox(iRow, playerData.iWho)
		self.whoArray[iRow].SetSelection(playerData.iWho)

		# Get the Civ and see if we should refresh the list of leaders
		dropDown = self.civArray[iRow]
		civChoice = playerData.iCiv+1
		if (not self.leaderRefresh):
			self.leaderRefresh = (civChoice != dropDown.GetSelection())
		dropDown.SetSelection(civChoice)

		if (self.leaderRefresh):
			self.refreshLeaderBox(iRow, playerData.iCiv)
			self.leaderRefresh = False

		# Get the Leader
		dropDown = self.leaderArray[iRow]
		dropDown.SetSelection(PB.getCivLeaderIndex(civChoice-1, playerData.iLeader)+1)

		# Get the Team
		dropDown = self.teamArray[iRow]
		dropDown.SetSelection(playerData.iTeam)

		# Get the Difficulty
		dropDown = self.diffArray[iRow]
		dropDown.SetSelection(playerData.iDifficulty)


		# Modify Status
		self.statusArray[iRow].SetLabel(playerData.getStatusText())


	def refreshWhoBox(self, iRow, iWho):
		# Add or remove choices depending on the state and the change
		dropDown = self.whoArray[iRow]

		if (iWho < 3):  # Status changing to non-taken state
			# Remove the player name from the drop down if it is there
			if (dropDown.GetCount() > 3):
				dropDown.Delete(3)
		else:			# Slot taken!
			if (dropDown.GetCount() == 3):
				# Add and display the player name
				dropDown.Append((PB.getName(iRow)))
			else:
				# Set the current player name with the new one
				dropDown.SetString(3, (PB.getName(iRow)))


	def refreshLeaderBox(self, iRow, iCiv):
		# Need to reset the leader choices - first clear the list
		dropDown = self.leaderArray[iRow]
		dropDown.Clear()

		# Give the Random choice
		dropDown.Append((localText.getText("TXT_KEY_PITBOSS_RANDOM", ())))

		civChoice = iCiv+1
		if (civChoice != 0):
			# If there are leaders to list, list them
			i = 0
			iNumLeaders = PB.getNumLeaders(civChoice-1)
			for i in range(iNumLeaders):
				dropDown.Append((PB.getCivLeaderAt(civChoice-1, i)))

		dropDown.SetSelection(0)

	def refreshCustomMapOptions(self, szMapName):
		# Clear the widgets from the custom option area
		i = 0
		for i in range( len(self.customItemSizerArray) ):
			self.Unbind(wx.EVT_CHOICE, self.customMapOptionArray[i])
			currentSizer = self.customItemSizerArray[i]
			success = currentSizer.Remove(1)	#dropDown
			success = currentSizer.Remove(0)	#txt
			success = self.dropDownSizer.Remove(currentSizer)
			self.customMapOptionArray[i].Destroy()
			self.customMapTextArray[i].Destroy()

		self.buildCustomMapOptions(szMapName)

		# Now rebuild the sizers
		self.dropDownSizer.Layout()
		self.optionsSizer.Layout()
		self.pageSizer.Layout()
		self.Layout()


	def refreshAdvancedStartPoints(self, iPoints):
		self.advancedStartPointsEdit.SetValue(str(iPoints))


	def buildCustomMapOptions(self, szMapName):
		gameData = PB.getGameSetupData()

		self.customItemSizerArray = []
		self.customMapTextArray = []
		self.customMapOptionArray = []

		# Create label/control pairs for custom map option
		customMapOptionsList = []

		optionNum = 0
		for optionNum in range(PB.getNumCustomMapOptions( gameData.getMapName() )):
			customMapOptionValuesList = []
			rowNum = 0
			for rowNum in range(PB.getNumCustomMapOptionValues( optionNum, gameData.getMapName() )):
				customMapOptionValuesList.append(PB.getCustomMapOptionDescAt( optionNum, rowNum, gameData.getMapName() ))
			customMapOptionsList.append(customMapOptionValuesList[:])

		optionNum = 0
		for optionNum in range(PB.getNumCustomMapOptions( szMapName )):
			itemSizer = wx.BoxSizer(wx.VERTICAL)
			txt = wx.StaticText( self, -1, PB.getCustomMapOptionName(optionNum, szMapName) )
			optionDropDown = wx.Choice(self, ((optionNum+1)*100), (-1,-1), choices=customMapOptionsList[optionNum])
			optionDropDown.SetSelection(gameData.getCustomMapOption(optionNum))
			itemSizer.Add(txt)
			itemSizer.Add(optionDropDown)
			self.customItemSizerArray.append(itemSizer)
			self.customMapTextArray.append(txt)
			self.customMapOptionArray.append(optionDropDown)
			self.dropDownSizer.Add(itemSizer, 0, wx.TOP, 3)
			self.Bind(wx.EVT_CHOICE, self.OnCustomMapOptionChoice, self.customMapOptionArray[optionNum])

#
# Progress bar dialog
#
class ProgressDialog(wx.Dialog):
	def __init__(self, parent):
		global curPage
		wx.Dialog.__init__(self, curPage, -1, localText.getText("TXT_KEY_PITBOSS_PATCH_PROGRESS_TITLE", ()), wx.DefaultPosition, wx.DefaultSize, wx.STAY_ON_TOP)

		self.myParent = parent

		self.iValue = 0
		self.iTotal = 100	# Arbitrary Value until it's actually set

		# Place the progress bar
		self.sizer = wx.BoxSizer(wx.VERTICAL)

		self.progress = None
		progressSizer = wx.BoxSizer(wx.VERTICAL)
		self.sizer.Add(progressSizer, 0, wx.ALL, 5)

		# Add a cancel button
		cancelButton = wx.Button(self, -1, localText.getText("TXT_KEY_SCREEN_CANCEL", ()))
		cancelButton.SetHelpText(localText.getText("TXT_KEY_CANCEL_PATCH_DOWNLOAD", ()))
		self.Bind(wx.EVT_BUTTON, self.OnCancelDownload, cancelButton)
		self.sizer.Add(cancelButton, 0, wx.ALL, 5)

		self.SetSizer(self.sizer)

	def setValue(self, iValue):
		if (iValue > 0):
			self.iValue = iValue
			if (self.progress != None):
				self.progress.SetValue(self.iValue)

	def setTotal(self, iTotal):
		if (iTotal != self.iTotal):
			if (iTotal > 0):
				self.iTotal = iTotal
				if (self.progress == None):
					self.progress = wx.Gauge(self, self.iValue, self.iTotal)
					self.sizer.Add(self.progress, 0, wx.ALL, 5)


	def OnCancelDownload(self, event):
		"Cancel download handler"
		# Tell the app
		self.myParent.cancelDownload()

		# Return to our caller
		if (self.IsModal()):
			self.EndModal(wx.ID_CANCEL)
		else:
			self.Show(false)
			return wx.ID_CANCEL


#
# main app class
#
class StartupIFace(wx.App):
	def OnInit(self):
		global curPage

		"Create the Pitboss Setup Wizard"
		self.wizard = wx.wizard.Wizard(None, -1, (localText.getText("TXT_KEY_PITBOSS_TITLE", ())))

		# Create each wizard page
		self.modSelect = ModSelectPage(self.wizard)
		self.smtpLogin = SMTPLoginPage(self.wizard)
		self.netSelect = NetSelectPage(self.wizard)
		self.login = LoginPage(self.wizard)
		self.loadSelect = LoadSelectPage(self.wizard)
		self.scenarioSelect = ScenarioSelectPage(self.wizard)
		self.staging = StagingPage(self.wizard)

		self.modSelect.SetNext(self.smtpLogin)
		self.smtpLogin.SetPrev(self.modSelect)
		self.smtpLogin.SetNext(self.netSelect)
		self.netSelect.SetPrev(self.smtpLogin)
		self.netSelect.SetNext(self.login)
		self.login.SetPrev(self.netSelect)
		self.login.SetNext(self.loadSelect)
		self.loadSelect.SetPrev(self.netSelect)
		self.loadSelect.SetNext(self.scenarioSelect)
		self.scenarioSelect.SetPrev(self.loadSelect)
		self.scenarioSelect.SetNext(self.staging)
		self.staging.SetPrev(self.loadSelect)

		self.progressDlg = None

		curPage = self.modSelect

		self.wizard.FitToPage(curPage)

		# Create a timer callback that will handle our updates
		timerID = wx.NewId()
		self.updateTimer = wx.Timer(self, timerID)
		self.Bind(wx.EVT_TIMER, self.OnTimedUpdate, id=timerID)
		self.updateTimer.Start(250)

		return True

	def startWizard(self):
		global curPage

		# Try starting the wizard
		if (self.wizard.RunWizard(curPage) and not PB.getDone()):
			# launch game here
			self.updateTimer.Stop()
			PB.launch()
			return True
		else:
			# user cancelled...
			self.updateTimer.Stop()
			PB.quit()
			return False

	def OnTimedUpdate(self, event):
		# Handle received net messages
		PB.handleMessages()

	def displayMessageBox(self, title, desc):
		#global msgBox
		#msgBox = wx.MessageDialog( self, desc, title, wx.OK )
		#msgBox.Show(True)
		outMsg = title + ":\n" + desc
		PB.consoleOut(outMsg)

	def patchAvailable(self, patchName, patchUrl):
		global curPage

		# Save info and display a popup to the user
		if ( (curPage == self.login) or (curPage == self.loadSelect) ):
			# Show the popup
			curPage.patchAvailable(patchName, patchUrl)

	def patchProgress(self, bytesRecvd, bytesTotal):
		global bPatchConfirmed
		if (not bPatchConfirmed):
			# Display our progress
			if (self.progressDlg == None):
				# Need to create the dialog
				self.progressDlg = ProgressDialog(self)
				self.progressDlg.Show(true)

			self.progressDlg.setTotal(bytesTotal)
			self.progressDlg.setValue(bytesRecvd)

	def cancelDownload(self):
		global bPatchConfirmed
		bPatchConfirmed = true

		# get rid of the dialog
		if (self.progressDlg != None):
			self.progressDlg.Show(false)
			self.progressDlg = None

		# Tell the application
		PB.cancelPatchDownload()

	def patchDownloadComplete(self, bSuccess):
		# Download complete - check if it was successful
		global curPage
		global bPatchConfirmed
		global bPatchOK

		# get rid of the dialog
		if (self.progressDlg != None):
			self.progressDlg.Show(false)
			self.progressDlg = None

		if (bSuccess):
			curPage.patchComplete()
		else:
			bPatchOK = false
			msg = wx.MessageBox(localText.getText("TXT_KEY_PITBOSS_PATCH_DOWNLOAD_ERROR_DESC", ()), localText.getText("TXT_KEY_PITBOSS_PATCH_DOWNLOAD_ERROR_TITLE", ()), wx.ICON_ERROR)

		bPatchConfirmed = true

		curPage.enableButtons()

	def upToDate(self):
		global curPage

		global bPatchConfirmed
		global bPatchOK
		bPatchConfirmed = true
		bPatchOK = true

		if ( (curPage == self.login) or (curPage == self.loadSelect) ):
			curPage.enableButtons()


	def refreshRow(self, iRow):
		global curPage

		# Get the latest data from the app and display in the view
		if (curPage == self.staging):
			# In the staging room, update the row
			curPage.refreshRow(iRow)

	def refreshCustomMapOptions(self, szMapName):
		global curPage

		# Refresh the page if we in the staging window
		if (curPage == self.staging):
			# Update the custom map options in the staging room
			curPage.refreshCustomMapOptions(szMapName)

	def refreshAdvancedStartPoints(self, iPoints):
		global curPage

		# Refresh the page if we in the staging window
		if (curPage == self.staging):
			# Update the custom map options in the staging room
			curPage.refreshAdvancedStartPoints(iPoints)
		