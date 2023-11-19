## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
def parseText(argsList):
	szMsg, iValueList = argsList
	print szMsg
	print iValueList
	return 0

def toggleDebugMode():
	import CvDebugTools
	CvDebugTools.toggleDebugMode()