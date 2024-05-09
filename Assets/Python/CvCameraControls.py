## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
##
## CvCameraControls

from CvPythonExtensions import *
import CvUtil

gc = CyGlobalContext()

class CvCameraControls:
	' class to handle moving the camera '

	# GLOBALVARS - add any new variables here so that they exist for the __init__ call
	SINGLE_PLOT_UNITS = 180.0
	ZOOM_SPEED_DEFAULT = 0.5
	TURN_SPEED_DEFAULT = 0.5
	PITCH_SPEED_DEFAULT = 0.5
	CAMERA_MIN_DISTANCE_DEFAULT = gc.getDefineFLOAT("CAMERA_MIN_DISTANCE")

	bLookAt = False
	originalPositionNiPoint3 = None
	newPositionNiPoint3 = None
	lookAtSpeed = 0
	# Rotate
	bRotate = False
	bRotateContinuous = False
	fRotateDestination = None
	fRotateTracking = 0.0
	rotateDirection = None
	fRotateSpeed = 0.0
	# Zoom
	bZoom = False
	fCurrentZoom = 0.0
	fZoomDestination = None
	zoomDirection = None
	fZoomSpeed = 0.0
	# Turn
	bTurn = False
	fCurrentBaseTurn = 0.0
	fTurnDestination = 0.0 # degrees
	turnDirection = None
	fTurnSpeed = 0.0
	# Pitch
	bPitch = False
	fCurrentBasePitch = 0.0
	fPitchDestination = 0.0 # degrees
	pitchDirection = None
	fPitchSpeed = 0.0

	def __init__( self ):
		self.resetCameraControls()

	def resetCameraControls( self ):
		self.resetRotate()
		self.resetZoom()
		self.resetTurn()
		self.resetPitch()

		# interface values
		self.bDebugModeState = CyGame().isDebugMode()
		self.bCityBillboardVisibility = CyEngine().getCityBillboardVisibility()
		self.bUnitFlagVisibility = CyEngine().getUnitFlagVisibility()
		self.bSelectionCursorVisibility = CyEngine().getSelectionCursorVisibility()
		self.InterfaceVisibilityState = CyInterface().getShowInterface()

	def centerCameraByCoords( self, t_Coords ):
		' center camera based on t_Coords '
		iX, iY = t_Coords
		CyCamera().JustLookAtPlot( CyMap().plot( iX, iY ) )

	def centerCamera( self, pPlot ):
		' center camera by plot '
		CyCamera().JustLookAt( pPlot.getPoint() )

	def centerCameraByObject( self, pObject ):
		' center camera on pObject - valid items are cities, units '
		CyCamera().JustLookAt( pObject.getPlot().getPoint() )

	def moveCameraXPlots( self, iNumPlots ):
		' moves the camera iNumPlots on the X coord '
		fRotateUnits = iNumPlots * self.SINGLE_PLOT_UNITS
		self.moveCamera( ( fRotateUnits, 0.0, 0.0 ) )

	def moveCameraYPlots( self, iNumPlots ):
		' moves the camera iNumPlots on the Y coord '
		fRotateUnits = iNumPlots * self.SINGLE_PLOT_UNITS
		self.moveCamera( ( 0.0, fRotateUnits, 0.0 ) )

	def setCameraMovementSpeed( self, iCameraMovementSpeed = CameraMovementSpeeds.CAMERAMOVEMENTSPEED_SLOW ):
		CyCamera().SetCameraMovementSpeed(int(CameraMovementSpeeds.CAMERAMOVEMENTSPEED_SLOW))

	def doMoveCamera( self, t_Coord ):
		' moves the camera based on the x/y/z values in t_Coord '
		if len(t_Coord) == 3:
			CyCamera().Translate( NiPoint3( t_Coord[0], t_Coord[1], t_Coord[2] ) )

	def doRotateGlobe( self ):
		self.resetCameraControls()
		if CyEngine().getCityBillboardVisibility():
			CyEngine().setCityBillboardVisibility(False)
		if CyInterface().getShowInterface() == InterfaceVisibility.INTERFACE_SHOW:
			CyInterface().setShowInterface(InterfaceVisibility.INTERFACE_HIDE_ALL)

		CyCamera().SetZoom(1.0)
		self.doRotateCamera( True, -self.SINGLE_PLOT_UNITS )

######################
# Rotate Controls
######################
	def doRotateCamera( self, fRotateDestination, fRotateSpeed ):
		self.bRotate = True
		self.fRotateDestination = fRotateDestination
		self.fRotateSpeed = fRotateSpeed

		if ( self.fRotateDestination == "Continuous" ):
			self.bRotateContinuous = True
			self.rotateDirection = "Continuous"

		if ( self.fRotateDestination < 0.0 ):
			self.rotateDirection = "Left"
			self.rotateTracking = -self.fRotateDestination

		if ( self.fRotateDestination > 0.0 ):
			self.rotateDirection = "Right"
			self.rotateTracking = self.fRotateDestination

		#self.updateRotate( 1.0 )

	def updateRotate( self, fDeltaTime ):
		' updates the Rotate based on fDeltaTime and the set Rotate Speed '
		bAtDestination = False

		if ( self.bRotate and self.rotateDirection ):
			fRotateModifier = self.fRotateSpeed * fDeltaTime
			# Left (-)  use -360.0 for fRotateDestination to rotate left
			if ( self.rotateDirection == "Left" ):
				self.rotateTracking += fRotateModifier
				if ( not self.bRotateContinuous and self.rotateTracking >= 0.0 ):
					bAtDestination = True
			# Right (+) use 360.0 for fRotateDestination to rotate right
			if ( self.rotateDirection == "Right" ):
				self.rotateTracking -= fRotateModifier
				if ( not self.bRotateContinuous and self.rotateTracking <= 0.0 ):
					bAtDestination = True

			if ( not bAtDestination ):
				self.doMoveCamera( ( fRotateModifier, 0.0, 0.0 ) )

			if ( bAtDestination ):
				self.resetRotate()

	def resetRotate( self ):
		' resets Rotate to default values '
		self.bRotate = False
		self.bRotateContinuous = False
		self.fRotateDestination = None
		self.rotateDirection = None
		self.fRotateSpeed = 0.0

######################
# Zoom Controls
######################
	def doZoomCamera( self, fZoomDestination, fZoomSpeed ):
		' moves the camera and checks if camera has reached destination '
		if ( fZoomDestination ):
			# toggle zoom camera control updates on
			self.bZoom = True
			if ( fZoomSpeed ):	# if not None
				self.fZoomSpeed = fZoomSpeed
			else:
				self.fZoomSpeed = self.ZOOM_SPEED_DEFAULT

			self.fZoomDestination = fZoomDestination

			if ( self.fCurrentZoom > self.fZoomDestination ):
				self.zoomDirection = "ZoomIn"

			elif ( self.fCurrentZoom < self.fZoomDestination ):
				self.zoomDirection = "ZoomOut"

			else:
				self.resetZoom()

	def updateZoom( self, fDeltaTime ):
		' updates the Zoom based on fDeltaTime and the set Zoom Speed '
		bAtDestination = False
		fZoomModifier = fDeltaTime * self.fZoomSpeed

		if ( self.zoomDirection):
			if ( self.zoomDirection == 'ZoomIn' ):
				fNewZoom = CyCamera().GetZoom() - fZoomModifier
			elif ( self.zoomDirection == 'ZoomOut' ):
				fNewZoom = CyCamera().GetZoom() + fZoomModifier

			# move camera based on whether its zooming in or out
			CyCamera().SetZoom( fNewZoom )

			if ( self.zoomDirection == 'ZoomIn' and fNewZoom < self.fZoomDestination ):
				bAtDestination = True
			if ( self.zoomDirection == 'ZoomOut' and fNewZoom > self.fZoomDestination ):
				bAtDestination = True

		if ( bAtDestination ):
			self.resetZoom()

	def resetZoom( self ):
		' resets Zoom to default values '
		self.bZoom = False
		self.fCurrentZoom = CyCamera().GetZoom()
		self.fZoomSpeed = 0.0
		self.zoomDirection = None
		self.fZoomDestination = None

######################
# Turn Controls
######################
	def isTurn( self ):
		return self.bTurn

	#def doTurnCamera( self, fTurnDestination, fTurnSpeed = self.TURN_SPEED_DEFAULT ):
	def doTurnCamera( self, fTurnDestination, fTurnSpeed ):
		' moves the camera and checks if camera has reached destination '
		self.fTurnDestination = fTurnDestination + CyCamera().GetBaseTurn()
		if ( fTurnDestination != 0.0 ):
			self.bTurn = True
			self.fTurnDestination = fTurnDestination
			self.fTurnSpeed = fTurnSpeed

		elif ( self.fTurnDestination > 0.0 ):
			self.turnDirection = "Right"
		else:
			self.turnDirection = "Left"

	def updateTurn( self, fDeltaTime ):
		' updates the Turn based on fDeltaTime and the set Turn Speed '
		bAtDestination = False

		if ( self.turnDirection ):
			fTurnModifer = fDeltaTime * self.fTurnSpeed

			if ( self.turnDirection == "Right" ):
				fNewTurn = CyCamera().GetBaseTurn() - fTurnModifier
			if ( self.turnDirection == "Left" ):
				fNewTurn = CyCamera().GetBaseTurn() - fTurnModifier

			CyCamera().SetBaseTurn( fNewTurn )

			if ( self.zoomDirection == "Right" and fNewTurn < self.fTurnDestination ):
				bAtDestination = True
			if ( self.zoomDirection == "Left" and fNewTurn > self.fTurnDestination ):
				bAtDestination = True

		if ( bAtDestination ):
			self.resetTurn()

	def resetTurn( self ):
		' resets Turn to default values '
		self.bTurn = False
		self.fCurrentBaseTurn = CyCamera().GetBaseTurn()
		self.fTurnDestination = 0.0 # degrees
		self.turnDirection = None
		self.fTurnSpeed = 0.0

######################
# Pitch Controls
######################
	def isPitch( self ):
		return self.bPitch

	def doPitchCamera( self, fPitchDestination, fPitchSpeed ):
		' moves the camera and checks if camera has reached destination '
		self.bPitch = True
		self.fCurrentPitch = CyCamera().GetBasePitch()
		self.fPitchDestination = fPitchDestination
		self.fPitchSpeed = fPitchSpeed

		if ( fPitchDestination > 0 ):
			self.pitchDirection = "Up"
		else:
			self.pitchDirection = "Down"

	def updatePitch( self, fDeltaTime ):
		' updates the pitch based on fDeltaTime and the set Pitch Speed '
		bAtDestination = False
		if ( self.PitchDirection ):
			fPitchModifer = fDeltaTime * self.fPitchSpeed
			if ( self.pitchDirection == "Up" ):
				fNewPitch = CyCamera().GetBasePitch() - fPitchModifier
			if ( self.pitchDirection == "Down" ):
				fNewPitch = CyCamera().GetBasePitch() - fPitchModifier
			CyCamera().SetBasePitch(fNewPitch)

			if ( self.pitchDirection == "Up" and fNewPitch < self.fPitchDestination ):
				bAtDestination = True
			elif ( self.pitchDirection == "Down" and fNewPitch > self.fPitchDestination ):
				bAtDestination = True

		if ( bAtDestination ):
			self.resetPitch()

	def resetPitch( self ):
		' resets Pitch to default values '
		self.bPitch = False
		self.fCurrentBasePitch = CyCamera().GetBasePitch()
		self.fPitchDestination = 0.0 # degrees
		self.pitchDirection = None
		self.fPitchSpeed = 0.0

# NEWCONTROLSHERE

######################
# Interface Fxns
######################
	def setCAMERA_MIN_DISTANCE( self, fVal ):
		CyGlobalContext().setDefineFLOAT("CAMERA_MIN_DISTANCE", fVal)

	def setDebugMode( self, bState ):
		if ( self.bSetDebugModeState != bState ):
			CyGame().toggleDebugMode()

	def setCityBillboardVisibility( self, bState ):
		if ( self.bCityBillboardVisibility != bState ):
			CyEngine().setCityBillboardVisibility( bState )

	def setUnitFlagVisibility( self, bState ):
		if ( self.bUnitFlagVisibility != bState ):
			CyEngine().setUnitFlagVisibility( bState )

	def setSelectionCursorVisibility( self, bState ):
		if ( self.bSelectionCursorVisibility != bState ):
			CyEngine().setSelectionCursorVisibility( bState )
######################
## Simple Look At
######################
	def doLookAt(self, ptPosition, ptTarget, fSpeed):
		self.bLookAt = True
		CyCamera().SetDestinationPosition(ptPosition)
		CyCamera().SetTargetDestination(ptTarget)
		CyCamera().SetLookAtSpeed(fSpeed)

#	def updateLookAt(self, fDeltaTime):
#		if not bCyCamera().GetCurrentPosition() == CyCamera().GetDestinationPosition():



######################
# Update / Input Handlers
######################
	def isActive( self ):
		if ( self.bRotate or self.bZoom or self.bTurn or self.bPitch ):
			return True
		return False

	def onUpdate( self, fDeltaTime ):
		' loops through UpdateMaps and returns a list of all controls that are being updated '
		# add any new controls as a CONTROLUPDATE
		if ( self.bLookAt or self.bRotate or self.bZoom or self.bTurn or self.bPitch ):
			if self.bLookAt:
				self.updateLookAt( fDeltaTime )
			if self.bRotate:
				self.updateRotate( fDeltaTime )
			if self.bZoom:
				self.updateZoom( fDeltaTime )
			if self.bTurn:
				self.updateTurn( fDeltaTime )
			if self.bPitch:
				self.updatePitch( fDeltaTime )

	def handleInput( self, theKey ):
		return

g_CameraControls = CvCameraControls()

