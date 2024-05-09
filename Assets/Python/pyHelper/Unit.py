## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import Player as PyPlayer

gc = CyGlobalContext()

""" PyUnit(UnitID) to create instance

	This is the Unit Helper function class. It was designed to handle common CyUnit()
	tasks. Each function is documented using the following format:

		def <functionName>(arguments):
			"<return ObjectType> - description"

	ObjectType Key:
		none	=	doesn't return a value
		int		=	integer
		intlist =   integer list
		str		=	string
		float	=	float
		list	=	list
		tuple	=	tuple
		obj		=	object aka instance
		objlist =	list of objects/instances
		bool	=	boolean (True (1) or False (0))

	GOAL: To create easy to use Unit access functions.

	- bmuzzin (Please feel free to contact with any feedback .. or you could ask Jesse ;) !)
"""

class PyUnit:

	def __init__(self, iPlayerID, iUnitID):
		self.player = gc.getPlayer(iPlayerID)
		self.unit = self.player.getUnit(iUnitID)

	def isNone(self):
		"bool - Is the unit instance valid?"
		return self.unit.isNone()

	############## G E N E R A L ##############

	def NotifyEntity( self, iEvent ):
		"none - tells this unit's entity to perform the given action"
		self.unit.NotifyEntity( iEvent )
