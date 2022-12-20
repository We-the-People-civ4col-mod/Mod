## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import re

gc = CyGlobalContext()
file_Text = 0
localText = CyTranslator()


IconMap = {
			"[ICON_BULLET]" : FontSymbols.BULLET_CHAR,
			"[ICON_HAPPY]" : FontSymbols.HAPPY_CHAR,
			"[ICON_UNHAPPY]" : FontSymbols.UNHAPPY_CHAR,
			"[ICON_HEALTHY]" : FontSymbols.HEALTHY_CHAR,
			"[ICON_UNHEALTHY]" : FontSymbols.UNHEALTHY_CHAR,
			"[ICON_STRENGTH]" : FontSymbols.STRENGTH_CHAR,
			"[ICON_MOVES]" : FontSymbols.MOVES_CHAR,
			"[ICON_RELIGION]" : FontSymbols.RELIGION_CHAR,
			"[ICON_STAR]" : FontSymbols.STAR_CHAR,
			"[ICON_SILVER_STAR]" : FontSymbols.SILVER_STAR_CHAR,
			"[ICON_TRADE]" : FontSymbols.TRADE_CHAR,
			"[ICON_DEFENSE]" : FontSymbols.DEFENSE_CHAR,
			"[ICON_GREATPEOPLE]" : FontSymbols.GREAT_PEOPLE_CHAR,
			"[ICON_BAD_GOLD]" : FontSymbols.BAD_GOLD_CHAR,
			"[ICON_BAD_FOOD]" : FontSymbols.BAD_FOOD_CHAR,
			"[ICON_EATENFOOD]" : FontSymbols.EATEN_FOOD_CHAR,
			"[ICON_GOLDENAGE]" : FontSymbols.GOLDEN_AGE_CHAR,
			"[ICON_ANGRYPOP]" : FontSymbols.ANGRY_POP_CHAR,
			"[ICON_OPENBORDERS]" : FontSymbols.OPEN_BORDERS_CHAR,
			"[ICON_DEFENSIVEPACT]" : FontSymbols.DEFENSIVE_PACT_CHAR,
			"[ICON_MAP]" : FontSymbols.MAP_CHAR,
			"[ICON_OCCUPATION]" : FontSymbols.OCCUPATION_CHAR,
			"[ICON_POWER]" : FontSymbols.POWER_CHAR,
			"[ICON_GOLD]" : FontSymbols.GOLD_CHAR,
			"[ICON_REBEL]" : FontSymbols.REBEL_CHAR,
			"[ICON_ANCHOR]": FontSymbols.ANCHOR_CHAR,
			"[ICON_NO_ANCHOR]": FontSymbols.NO_ANCHOR_CHAR,
			"[ANCHOR_EUROPE_CHAR]": FontSymbols.ANCHOR_EUROPE_CHAR,
			"[ICON_BARRACKS]": FontSymbols.BARRACKS_CHAR,
			"[ICON_NO_BARRACKS]": FontSymbols.NO_BARRACKS_CHAR,
			"[ICON_BOMBARD]": FontSymbols.BOMBARD_CHAR,
			"[ICON_POSITIVE_DOMESTIC_MARKET_EVENT]": FontSymbols.POSITIVE_DOMESTIC_MARKET_EVENT_CHAR,
			"[ICON_NEGATIVE_DOMESTIC_MARKET_EVENT]": FontSymbols.NEGATIVE_DOMESTIC_MARKET_EVENT_CHAR,
			"[ICON_OVERFLOW]": FontSymbols.OVERFLOW_CHAR,
		 }


def finishText(argsList):

	szString = argsList[0]

	return szString

def printOutText(argsList):
	global file_Text
	key, text = argsList

	if key == "DONE" or text == "DONE" and file_Text != 0:
		file_Text.close()
		return

	if file_Text == 0:
		file_Text = open("Civ4 GAME Text.txt", 'w')

	if file_Text != 0:
		file_Text.write("\n%s\n\t%s" %(key, text))
	