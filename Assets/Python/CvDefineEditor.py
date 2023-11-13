import wx;
from CvPythonExtensions import *

gc = CyGlobalContext()
gVDS = gc.getCyDefinesVarSystem()

class CvIntEditorPanel( wx.Panel ):

	def __init__( self, kParent, szVarName ):
		wx.Panel.__init__( self, kParent )

		self.hLabel = wx.StaticText( self, -1, szVarName )
		self.hEdit = wx.TextCtrl( self, value = str( gVDS.getValueInt( szVarName ) ) )
		self.szVarName = szVarName;

		self.hSizer = wx.BoxSizer( wx.HORIZONTAL )
		self.hSizer.Add( self.hLabel, 1, wx.EXPAND | wx.ALL, 4 )
		self.hSizer.Add( self.hEdit, 1, wx.ALIGN_RIGHT )
		self.SetSizer( self.hSizer )

		self.Bind( wx.EVT_TEXT_ENTER, self.OnUpdateText, self.hEdit )

	def OnUpdateText( self, kEvent ):
		gVDS.setValueInt( self.szVarName, int(self.hEdit.GetLineText( 0 )) )

class CvFloatEditorPanel( wx.Panel ):

	def __init__( self, kParent, szVarName ):
		wx.Panel.__init__( self, kParent )

		self.hLabel = wx.StaticText( self, -1, szVarName )
		self.hEdit = wx.TextCtrl( self, value = str( gVDS.getValueFloat( szVarName ) ) )
		self.szVarName = szVarName;

		self.hSizer = wx.BoxSizer( wx.HORIZONTAL )
		self.hSizer.Add( self.hLabel, 1, wx.EXPAND | wx.ALL, 4 )
		self.hSizer.Add( self.hEdit, 1, wx.ALIGN_RIGHT )
		self.SetSizer( self.hSizer )

		self.Bind( wx.EVT_TEXT_ENTER, self.OnUpdateText, self.hEdit )

	def OnUpdateText( self, kEvent ):
		gVDS.setValueFloat( self.szVarName, float(self.hEdit.GetLineText( 0 )) )

class CvStringEditorPanel( wx.Panel ):

	def __init__( self, kParent, szVarName ):
		wx.Panel.__init__( self, kParent )

		self.hLabel = wx.StaticText( self, -1, szVarName )
		self.hEdit = wx.TextCtrl( self, value = str( gVDS.getValueString( szVarName ) ) )
		self.szVarName = szVarName;

		self.hSizer = wx.BoxSizer( wx.HORIZONTAL )
		self.hSizer.Add( self.hLabel, 1, wx.EXPAND | wx.ALL, 4 )
		self.hSizer.Add( self.hEdit, 1, wx.ALIGN_RIGHT )
		self.SetSizer( self.hSizer )
		self.Bind( wx.EVT_TEXT_ENTER, self.OnUpdateText, self.hEdit )

	def OnUpdateText( self, kEvent ):
		gVDS.setValueString( self.szVarName, self.hEdit.GetLineText( 0 ) )

class CvDefineEditorFrame( wx.Frame ):

	ID_VARCOMBO = 1000

	def __init__( self ):
		wx.Frame.__init__( self, None, -1, "Info Editor", (-1,-1), (-1,-1),
			wx.MAXIMIZE_BOX | wx.CLOSE_BOX | wx.SYSTEM_MENU | wx.CAPTION | wx.RESIZE_BORDER | wx.VSCROLL  )

		self.hSizer = wx.BoxSizer( wx.VERTICAL )
		self.hVarCombo = wx.ComboBox( self, style = wx.CB_SORT | wx.CB_DROPDOWN | wx.CB_READONLY  )
		self.hSizer.Add( self.hVarCombo, 1, wx.EXPAND )
		self.SetSizer( self.hSizer )

		szVarName = gVDS.getFirstVariableName()

		while szVarName != "":
			self.hVarCombo.Append( szVarName )
			szVarName = gVDS.getNextVariableName()


		self.Bind( wx.EVT_TEXT, self.OnComboSelection, self.hVarCombo )

	def OnComboSelection( self, kEvent ):
		szVarName = self.hVarCombo.GetValue()
		if szVarName != "":
			szVarType = gVDS.getVariableType( szVarName )
			hPanel = None
			if szVarType == "int":
				hPanel = CvIntEditorPanel( self, szVarName )
			elif szVarType == "float":
				hPanel = CvFloatEditorPanel( self, szVarName )
			elif szVarType == "string":
				hPanel = CvStringEditorPanel( self, szVarName )

			if hPanel != None:
				self.hSizer.Add( hPanel, 1, wx.EXPAND )
		self.hSizer.Layout()

class CvDefineEditorApp( wx.App ):

	def MainLoop( self ):
		kEventLoop = wx.EventLoop()
		wx.EventLoop.SetActive( kEventLoop )
		while ( kEventLoop.Pending() ):
			kEventLoop.Dispatch()


	def OnInit( self ):
		self.hFrame = CvDefineEditorFrame()
		self.SetTopWindow( self.hFrame )
		self.hFrame.Show(1)
		self.SetExitOnFrameDelete( False )

		return 1

kApp = CvDefineEditorApp(0)
