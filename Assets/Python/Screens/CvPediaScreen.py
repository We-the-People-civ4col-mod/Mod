## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
# Sid Meier's Civilization 4
# Copyright Firaxis Games 2005
#
# CvScreen - Base class for all of the Screens

from CvPythonExtensions import *
import CvUtil
import CvScreen

class CvPediaScreen( CvScreen.CvScreen ):
	"Civilopedia Base Screen"

	def getSortedList( self, numInfos, getInfo ):
		' returned a list of infos sorted alphabetically '

		# count the items we are going to display
		iNumNonGraphical = 0
		for i in range(numInfos):
			#if (not getInfo(i).isGraphicalOnly()):
			if (not getInfo(i).hideFromPedia()):
				iNumNonGraphical += 1

		infoList = [(0,0)] * iNumNonGraphical
		j = 0
		for i in range( numInfos ):
			#if (not getInfo(i).isGraphicalOnly()):
			if (not getInfo(i).hideFromPedia()):
				infoList[j] = (getInfo(i).getDescription(), i)
				j += 1

		infoList.sort()

		return infoList
	