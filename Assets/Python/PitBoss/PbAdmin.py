## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
#
# Pitboss admin framework
# Dan McGarry 3-24-05
#
from CvPythonExtensions import *
import wx
import wx.lib.scrolledpanel
import time
import string

PB = CyPitboss()
gc = CyGlobalContext()
localText = CyTranslator()
msgBox = None

#
# resource IDs
#
ID_ABOUT = 101
ID_SAVE  = 102
ID_EXIT  = 103

#
# admin frame class
#
class AdminFrame(wx.Frame):
	def __init__(self, parent, ID, title):
		"constructor"
		wx.Frame.__init__(self, parent, ID, title,
						wx.DefaultPosition, wx.Size(675, 480))

		# Create the menu
		menu = wx.Menu()
		menu.Append(ID_ABOUT, (localText.getText("TXT_KEY_PITBOSS_ABOUT", ())), (localText.getText("TXT_KEY_PITBOSS_ABOUT_TEXT", ())))
		menu.AppendSeparator()
		menu.Append(ID_SAVE, (localText.getText("TXT_KEY_PITBOSS_SAVE", ())), (localText.getText("TXT_KEY_PITBOSS_SAVE_TEXT", ())))
		menu.Append(ID_EXIT, (localText.getText("TXT_KEY_PITBOSS_EXIT", ())), (localText.getText("TXT_KEY_PITBOSS_EXIT_TEXT", ())))
		menuBar = wx.MenuBar()
		strFile = localText.getText("TXT_KEY_PITBOSS_FILE", ())
		strFile = localText.stripHTML(strFile)
		menuBar.Append(menu, strFile);
		self.SetMenuBar(menuBar)

		# Create our arrays of information and controls
		self.nameArray = []
		self.pingArray = []
		self.scoreArray = []
		self.kickArray = []

		pageSizer = wx.BoxSizer(wx.VERTICAL)

		# Add the game name and date
		self.gameTurn = PB.getGameturn()
		self.title = wx.StaticText(self, -1, PB.getGamename() + " - " + PB.getGamedate(False))
		font = wx.Font(18, wx.SWISS, wx.NORMAL, wx.NORMAL)
		self.title.SetFont(font)
		self.title.SetSize(self.title.GetBestSize())
		pageSizer.Add(self.title, 0, wx.ALL, 5)

		# Add the turn timer if we have one
		if (PB.getTurnTimer()):
			timerSizer = wx.BoxSizer(wx.HORIZONTAL)

			# Add a button to allow turn timer modification
			timerChangeButton = wx.Button(self, -1, localText.getText("TXT_KEY_MP_OPTION_TURN_TIMER", ()))
			self.Bind(wx.EVT_BUTTON, self.OnChangeTimer, timerChangeButton)
			timerSizer.Add(timerChangeButton, 0, wx.ALL, 5)

			self.timerDisplay = wx.StaticText(self, -1, "")
			font = wx.Font(16, wx.SWISS, wx.NORMAL, wx.NORMAL)
			self.timerDisplay.SetFont(font)
			self.timerDisplay.SetSize(self.timerDisplay.GetBestSize())
			timerSizer.Add(self.timerDisplay, 0, wx.ALL, 5)

			pageSizer.Add(timerSizer, 0, wx.ALL, 5)

		infoSizer = wx.BoxSizer(wx.HORIZONTAL)
		leftSizer = wx.BoxSizer(wx.VERTICAL)

		playerPanel = wx.lib.scrolledpanel.ScrolledPanel(self, -1, size=(370, 280), style = wx.DOUBLE_BORDER)
		playerSizer = wx.BoxSizer(wx.VERTICAL)

		# Create a row for each player in the game
		rowNum = 0
		for rowNum in range(gc.getMAX_CIV_PLAYERS()):
			if (gc.getPlayer(rowNum).isEverAlive()):
				# Create the border box
				border = wx.StaticBox(playerPanel, -1, (localText.getText("TXT_KEY_PITBOSS_PLAYER", (rowNum+1, ))), (0,(rowNum*30)))
				# Create the layout mgr
				rowSizer = wx.StaticBoxSizer(border, wx.HORIZONTAL)

				# Player name
				itemSizer = wx.BoxSizer(wx.VERTICAL)
				lbl = wx.StaticText(playerPanel, -1, (localText.getText("TXT_KEY_PITBOSS_WHO", ())))
				txtValue = wx.StaticText(playerPanel, rowNum, "", size = wx.Size(100, 13))
				itemSizer.Add(lbl)
				itemSizer.Add(txtValue)
				rowSizer.Add(itemSizer, 0, wx.ALL, 5)
				self.nameArray.append(txtValue)

				# Ping times
				itemSizer = wx.BoxSizer(wx.VERTICAL)
				lbl = wx.StaticText(playerPanel, -1, (localText.getText("TXT_KEY_PITBOSS_PING", ())))
				txtValue = wx.StaticText(playerPanel, rowNum, "", size = wx.Size(70, 13))
				itemSizer.Add(lbl)
				itemSizer.Add(txtValue)
				rowSizer.Add(itemSizer, 0, wx.ALL, 5)
				self.pingArray.append(txtValue)

				# Scores
				itemSizer = wx.BoxSizer(wx.VERTICAL)
				lbl = wx.StaticText(playerPanel, -1, (localText.getText("TXT_KEY_PITBOSS_SCORE", ())))
				txtValue = wx.StaticText(playerPanel, rowNum, "", size = wx.Size(30, 13))
				itemSizer.Add(lbl)
				itemSizer.Add(txtValue)
				rowSizer.Add(itemSizer, 0, wx.ALL, 5)
				self.scoreArray.append(txtValue)

				# Kick buttons
				kickButton = wx.Button(playerPanel, rowNum, (localText.getText("TXT_KEY_PITBOSS_KICK", ())))
				rowSizer.Add(kickButton, 0, wx.ALL, 5)
				kickButton.Disable()
				self.Bind(wx.EVT_BUTTON, self.OnKick, kickButton)
				self.kickArray.append(kickButton)

				playerSizer.Add(rowSizer, 0, wx.ALL, 5)

		playerPanel.SetSizer(playerSizer)
		playerPanel.SetAutoLayout(1)
		playerPanel.SetupScrolling()
		leftSizer.Add(playerPanel, 0, wx.ALL, 5)

		# Add a button row
		buttonSizer = wx.BoxSizer(wx.HORIZONTAL)

		# Add the save game button
		saveButton = wx.Button(self, -1, (localText.getText("TXT_KEY_PITBOSS_SAVE_GAME", ())))
		self.Bind(wx.EVT_BUTTON, self.OnSave, saveButton)
		buttonSizer.Add(saveButton, 0, wx.ALL, 5)

		# Add the exit game button
		exitButton = wx.Button(self, -1, (localText.getText("TXT_KEY_MAIN_MENU_EXIT_GAME", ())))
		self.Bind(wx.EVT_BUTTON, self.OnExit, exitButton)
		buttonSizer.Add(exitButton, 0, wx.ALL, 5)

		leftSizer.Add(buttonSizer, 0, wx.ALL, 5)

		# Add the left area to the info area
		infoSizer.Add(leftSizer, 0, wx.ALL, 5)

		# Now create the message area
		messageSizer = wx.BoxSizer(wx.VERTICAL)

		# Create the MotD Panel
		motdBorder = wx.StaticBox(self, -1, localText.getText("TXT_KEY_PITBOSS_MOTD_TITLE", ()))
		motdSizer = wx.StaticBoxSizer(motdBorder, wx.VERTICAL)

		# Check box whether to use MotD or not
		self.motdCheckBox = wx.CheckBox(self, -1, localText.getText("TXT_KEY_PITBOSS_MOTD_TOGGLE", ()))
		self.motdCheckBox.SetValue(false)
		motdSizer.Add(self.motdCheckBox, 0, wx.TOP, 5)

		# Add edit box displaying current MotD
		self.motdDisplayBox = wx.TextCtrl(self, -1, "", size=(225,50), style=wx.TE_MULTILINE|wx.TE_READONLY)
		self.motdDisplayBox.SetHelpText(localText.getText("TXT_KEY_PITBOSS_MOTD_HELP", ()))
		motdSizer.Add(self.motdDisplayBox, 0, wx.ALL, 5)

		# Add a button to allow motd modification
		motdChangeButton = wx.Button(self, -1, localText.getText("TXT_KEY_PITBOSS_MOTD_CHANGE", ()))
		motdChangeButton.SetHelpText(localText.getText("TXT_KEY_PITBOSS_MOTD_CHANGE_HELP", ()))
		self.Bind(wx.EVT_BUTTON, self.OnChangeMotD, motdChangeButton)
		motdSizer.Add(motdChangeButton, 0, wx.ALL, 5)

		# Add the motd area to the message area
		messageSizer.Add(motdSizer, 0, wx.ALL, 5)

		# Create the dialog panel
		dialogBorder = wx.StaticBox(self, -1, localText.getText("TXT_KEY_PITBOSS_CHAT_TITLE", ()))
		dialogSizer = wx.StaticBoxSizer(dialogBorder, wx.VERTICAL)

		# Chat log
		self.chatLog = wx.TextCtrl(self, -1, "", size=(225,100), style=wx.TE_MULTILINE|wx.TE_READONLY)
		self.chatLog.SetHelpText(localText.getText("TXT_KEY_PITBOSS_CHAT_LOG_HELP", ()))
		dialogSizer.Add(self.chatLog, 0, wx.ALL, 5)

		# Chat edit
		self.chatEdit = wx.TextCtrl(self, -1, "", size=(225,-1), style=wx.TE_PROCESS_ENTER)
		self.chatEdit.SetHelpText(localText.getText("TXT_KEY_PITBOSS_CHAT_EDIT_HELP", ()))
		dialogSizer.Add(self.chatEdit, 0, wx.ALL, 5)
		self.Bind(wx.EVT_TEXT_ENTER, self.OnSendChat, self.chatEdit)

		# Add the dialog area to the message area
		messageSizer.Add(dialogSizer, 0, wx.ALL, 5)

		# Add the message area to our info area
		infoSizer.Add(messageSizer, 0, wx.ALL, 5)

		# Add the info area to the page
		pageSizer.Add(infoSizer, 0, wx.ALL, 5)

		self.SetSizer(pageSizer)

		# Register the event handlers
		wx.EVT_MENU(self, ID_ABOUT, self.OnAbout)
		wx.EVT_MENU(self, ID_SAVE, self.OnSave)
		wx.EVT_MENU(self, ID_EXIT, self.OnExit)

		# Other handlers
		self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)

	def getTimerString(self, turnSlices):
		# Get the time string for the turn timer...
		# See if we are out of time

		# Only update every second
		retVal = self.timerDisplay.GetLabel()
		if (turnSlices%4 == 0):
			if (turnSlices < 0):
				retVal = "0:00:00"
			else:
				numHours = 0
				numMinutes = 0
				numSeconds = turnSlices/4
				if (numSeconds > 59):
					numMinutes = numSeconds/60
					numSeconds = numSeconds%60
					if (numMinutes > 59):
						numHours = numMinutes/60
						numMinutes = numMinutes%60

				retVal = ""
				if (numHours > 0):
					retVal = str(numHours)
				else:
					retVal = "0"
				retVal += ":"

				if (numMinutes > 9):
					retVal += str(numMinutes)
				elif (numMinutes > 0):
					retVal += "0" + str(numMinutes)
				else:
					retVal += "00"
				retVal += ":"

				if (numSeconds > 9):
					retVal += str(numSeconds)
				elif (numSeconds > 0):
					retVal += "0" + str(numSeconds)
				else:
					retVal += "00"

		return retVal

	def update(self):
		# We have the widgets created, set the values...
		if (self.gameTurn != PB.getGameturn()):
			self.title.SetLabel(PB.getGamename() + " - " + PB.getGamedate(False))
			self.gameTurn = PB.getGameturn()

		if (PB.getTurnTimer()):
			timerStr = self.getTimerString(PB.getTurnTimeLeft())
			if (timerStr != self.timerDisplay.GetLabel()):
				self.timerDisplay.SetLabel(timerStr)

		rowNum = 0
		for rowNum in range(gc.getMAX_CIV_PLAYERS()):
			if (gc.getPlayer(rowNum).isEverAlive()):
				# Get the player data
				playerData = PB.getPlayerAdminData(rowNum)

				# Set the values
				nameDisplay = ""
				if (not playerData.bTurnActive):
					nameDisplay += "*"
				nameDisplay += playerData.getName()
				if (nameDisplay != self.nameArray[rowNum].GetLabel()):
					self.nameArray[rowNum].SetLabel(nameDisplay)

				if ((playerData.getPing()) != self.pingArray[rowNum].GetLabel()):
					self.pingArray[rowNum].SetLabel((playerData.getPing()))

				if ((playerData.getScore()) != self.scoreArray[rowNum].GetLabel()):
					self.scoreArray[rowNum].SetLabel((playerData.getScore()))

				bEnabled = self.kickArray[rowNum].IsEnabled()
				bShouldEnable = (playerData.bHuman and playerData.bClaimed)
				if (bEnabled != bShouldEnable):
					if (bShouldEnable):
						self.kickArray[rowNum].Enable(True)
					else:
						self.kickArray[rowNum].Disable()

	def OnKick(self, event):
		"'kick' event handler"
		rowNum = event.GetId()
		dlg = wx.MessageDialog(self, (localText.getText("TXT_KEY_PITBOSS_KICK_VERIFY", (PB.getName(rowNum), ))),
				(localText.getText("TXT_KEY_PITBOSS_KICK_VERIFY_TITLE", ())), wx.YES_NO | wx.ICON_QUESTION)

		if (dlg.ShowModal() == wx.ID_YES):
			PB.kick(rowNum)

		dlg.Destroy()

	def OnAbout(self, event):
		"'about' event handler"
		dlg = wx.MessageDialog(self, (localText.getText("TXT_KEY_PITBOSS_VERSION", (PB.getVersion(), ))), (localText.getText("TXT_KEY_PITBOSS_ABOUT_BOX_TITLE", ())), wx.OK | wx.ICON_INFORMATION)
		dlg.ShowModal()
		dlg.Destroy()

	def OnSave(self, event):
		"'save' event handler"
		dlg = wx.FileDialog(
			self, message=(localText.getText("TXT_KEY_PITBOSS_SAVE_AS", ())), defaultDir=".\saves\multi",
			defaultFile="Pitboss_"+PB.getGamedate(True)+".ColonizationSave",
			wildcard=(localText.getText("TXT_KEY_PITBOSS_SAVE_AS_TEXT", ())) + " (*.ColonizationSave)", style=wx.SAVE | wx.OVERWRITE_PROMPT
			)

		if dlg.ShowModal() == wx.ID_OK:
			# Get the file name
			path = dlg.GetPath()
			if (path != ""):
				# Got a file to save - try to save it
				if ( not PB.save(path) ):
					# Saving game failed!  Let the user know
					msg = wx.MessageBox((localText.getText("TXT_KEY_PITBOSS_ERROR_SAVING", ())), (localText.getText("TXT_KEY_PITBOSS_SAVE_ERROR", ())), wx.ICON_ERROR)
				else:
					msg = wx.MessageBox((localText.getText("TXT_KEY_PITBOSS_SAVE_SUCCESS", (path, ))), (localText.getText("TXT_KEY_PITBOSS_SAVED", ())), wx.ICON_INFORMATION)

		dlg.Destroy()

	def OnChangeMotD(self, event):
		"'MotD' event handler"

		# Changing MotD - pop a modal dialog
		dlg = wx.TextEntryDialog(
			self, localText.getText("TXT_KEY_PITBOSS_MOTD_POPUP_DESC", ()),
			localText.getText("TXT_KEY_PITBOSS_MOTD_POPUP_TITLE", ()))

		# Show the modal dialog and get the response
		if dlg.ShowModal() == wx.ID_OK:
			# Set the MotD
			self.motdDisplayBox.SetValue(dlg.GetValue())

	def OnChangeTimer(self, event):
		"Turn timer event handler"

		# Changing Timer - pop a modal dialog
		dlg = wx.TextEntryDialog(
			self, localText.getText("TXT_KEY_PITBOSS_TURN_TIMER_NEW", ()),
			localText.getText("TXT_KEY_MP_OPTION_TURN_TIMER", ()))
		dlg.SetValue("%s" % (gc.getGame().getPitbossTurnTime(), ))

		# Show the modal dialog and get the response
		if dlg.ShowModal() == wx.ID_OK:
			szValue = dlg.GetValue()
			if szValue != "":
				if not self.IsNumericString(szValue):
					dlg2 = wx.MessageDialog(
						self, localText.getText("TXT_KEY_PITBOSS_TURNTIMER_ERROR_DESC", ()),
						localText.getText("TXT_KEY_PITBOSS_TURNTIMER_ERROR_TITLE", ()), wx.OK|wx.ICON_EXCLAMATION)

					if dlg2.ShowModal() == wx.ID_OK:
						# Clear out the TurnTimer Edit box
						dlg.SetValue("")
				else:
					PB.turnTimerChanged((int)(dlg.GetValue()))

	def OnSendChat(self, event):
		"'Chat Send' event handler"

		# Verify we have text to send

		if ( len(self.chatEdit.GetValue()) ):
			PB.sendChat(self.chatEdit.GetValue())
			self.chatEdit.SetValue("")

	def OnExit(self, event):
		"'exit' event handler"
		self.Close(True)

	def OnCloseWindow(self, event):
		"'close window' event handler"
		PB.quit()
		self.Destroy()

	def IsNumericString(self, myStr):
		for myChar in myStr:
			if myChar not in string.digits:
				return False
		return True
#
# main app class
#
class AdminIFace(wx.App):
	def OnInit(self):
		"create the admin frame"
		self.adminFrame = AdminFrame(None, -1, (localText.getText("TXT_KEY_PITBOSS_SAVE_SUCCESS", (PB.getGamename(), ))))
		self.adminFrame.Show(True)
		self.SetTopWindow(self.adminFrame)
		return True

	def update(self):
		"process events - call in main loop"

		# Create an event loop and make it active.
		# save the old one
		evtloop = wx.EventLoop()
		old = wx.EventLoop.GetActive()
		wx.EventLoop.SetActive(evtloop)

		# Update our view
		self.adminFrame.update()

		# This inner loop will process any GUI events
		# until there are no more waiting.
		while evtloop.Pending():
			evtloop.Dispatch()

		# Send idle events to idle handlers.
		time.sleep(0.01)
		self.ProcessIdle()

		# restore old event handler
		wx.EventLoop.SetActive(old)

	def refreshRow(self, iRow):
		"Stub for refresh row..."
		return True

	def getMotD(self):
		"Message of the day retrieval"
		if (self.adminFrame.motdCheckBox.GetValue()):
			return self.adminFrame.motdDisplayBox.GetValue()
		else:
			return ""

	def addChatMessage(self, message):
		message = localText.stripHTML(message)
		self.adminFrame.chatLog.AppendText("\n")
		self.adminFrame.chatLog.AppendText(message)

	def displayMessageBox(self, title, desc):
		#global msgBox
		#msgBox = wx.MessageDialog( self, desc, title, wx.OK )
		#msgBox.Show(True)
		outMsg = title + ":\n" + desc
		PB.consoleOut(outMsg)

