## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUnitController

#"""
#UNIT CONTROL INTERFACE
#"""

#""" Global Unit controller instance. Instead of creating a new one everytime, just keep a global - saves time."""
pGlobalUnitController = CvUnitController.CvUnitController()

def ProcessEvent ( argsList ):
	"""Notifies a unit of a occurring event. This function returns true if it processes the given event, or false if
	it does not (in which case the default processor will do something with it). """
	pCyUnitEntity, iEvent = argsList
	return pGlobalUnitController.ProcessEvent( pCyUnitEntity, iEvent )
