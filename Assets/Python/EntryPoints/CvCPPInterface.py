import CvUtil
import CvScreensInterface
from CvPythonExtensions import *

#
# The DLL can call this file to access python only functions
#

gc = CyGlobalContext()
localText = CyTranslator()

def isOOSVisible():
	return CyInterface().isOOSVisible()
