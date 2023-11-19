# Sid Meier's Civilization 4
# Copyright Firaxis Games 2005
#
# CvScreenUtilsInterface.py
#
# * This file stores the version of CvScreenUtils that is active
# * It is called from CvScreensInterface
# * When modding, this file should be replaced with one that has
#   screenUtils pointing to the mods <Mod>ScreenUtils
#
# No other modules should import this
#
import CvScreenUtils

normalScreenUtils = CvScreenUtils.CvScreenUtils()

def getScreenUtils():
	return normalScreenUtils
