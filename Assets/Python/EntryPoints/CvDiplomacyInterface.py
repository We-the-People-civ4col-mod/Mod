## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import CvDiplomacy

gc = CyGlobalContext()

def beginDiplomacy (argsList):
	"""
	This is what gets called when you first begin diplomacy
	The first parameter argsList[0] is the 'comment type', or how they feel about you
	"""
	eComment = argsList[0]
	commentArgsSize = argsList[1]
	if (commentArgsSize):
		commentArgs = argsList[2:]
		CvUtil.pyAssert(len(commentArgs)==commentArgsSize, "comment args tuple size mismatch")
		print "tuple size", len(commentArgs), ", commentArgsSize ", commentArgsSize
	else:
		commentArgs=[]
	diploClass = CvDiplomacy.CvDiplomacy()
	diploClass.setAIComment(eComment, *commentArgs)	#unpack args tuple


def handleUserResponse (argsList):
	"First parameter of argsList if the comment they clicked on..."
	diploClass = CvDiplomacy.CvDiplomacy()

	eComment = argsList[0]
	iData1 = argsList[1]
	iData2 = argsList[2]
	diploClass.handleUserResponse(eComment, iData1, iData2)

def dealCanceled ():
	diploClass = CvDiplomacy.CvDiplomacy()

	diploClass.dealCanceled()

def refresh (argsList):
	diploClass = CvDiplomacy.CvDiplomacy()
	diploClass.determineResponses(argsList[0])

def toggleDebugLogging():
	CvDiplomacy.DebugLogging = not CvDiplomacy.DebugLogging

	