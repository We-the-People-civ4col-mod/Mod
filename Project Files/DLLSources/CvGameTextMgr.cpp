//---------------------------------------------------------------------------------------
//
//  *****************   Civilization IV   ********************
//
//  FILE:    CvGameTextMgr.cpp
//
//  PURPOSE: Interfaces with GameText XML Files to manage the paths of art files
//
//---------------------------------------------------------------------------------------
//  Copyright (c) 2004 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------

#include "CvGameCoreDLL.h"
#include "CvGameTextMgr.h"
#include "CvGameCoreUtils.h"
#include "CvDLLUtilityIFaceBase.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLSymbolIFaceBase.h"
#include "CvInfos.h"
#include "CvXMLLoadUtility.h"
#include "CvCity.h"
#include "CvPlayerAI.h"
#include "CvTeamAI.h"
#include "CvGameAI.h"
#include "CvSelectionGroup.h"
#include "CvMap.h"
#include "CvArea.h"
#include "CvPlot.h"
#include "CvPopupInfo.h"
#include "FProfiler.h"
#include "CyArgsList.h"
#include "CvDLLPythonIFaceBase.h"

int shortenID(int iId)
{
	return iId;
}

// For displaying Asserts and error messages
static char* szErrorMsg;

//----------------------------------------------------------------------------
//
//	FUNCTION:	GetInstance()
//
//	PURPOSE:	Get the instance of this class.
//
//----------------------------------------------------------------------------
CvGameTextMgr& CvGameTextMgr::GetInstance()
{
	static CvGameTextMgr gs_GameTextMgr;
	return gs_GameTextMgr;
}

//----------------------------------------------------------------------------
//
//	FUNCTION:	CvGameTextMgr()
//
//	PURPOSE:	Constructor
//
//----------------------------------------------------------------------------
CvGameTextMgr::CvGameTextMgr()
{

}

CvGameTextMgr::~CvGameTextMgr()
{
}

//----------------------------------------------------------------------------
//
//	FUNCTION:	Initialize()
//
//	PURPOSE:	Allocates memory
//
//----------------------------------------------------------------------------
void CvGameTextMgr::Initialize()
{

}

//----------------------------------------------------------------------------
//
//	FUNCTION:	DeInitialize()
//
//	PURPOSE:	Clears memory
//
//----------------------------------------------------------------------------
void CvGameTextMgr::DeInitialize()
{
	for(int i=0;i<(int)m_apbPromotion.size();i++)
	{
		delete [] m_apbPromotion[i];
	}
}

//----------------------------------------------------------------------------
//
//	FUNCTION:	Reset()
//
//	PURPOSE:	Accesses CvXMLLoadUtility to clean global text memory and
//				reload the XML files
//
//----------------------------------------------------------------------------
void CvGameTextMgr::Reset()
{
	CvXMLLoadUtility pXML;
	pXML.LoadGlobalText();
}


// Returns the current language
int CvGameTextMgr::getCurrentLanguage()
{
	return gDLL->getCurrentLanguage();
}

void CvGameTextMgr::setYearStr(CvWString& szString, int iGameTurn, bool bSave, CalendarTypes eCalendar, int iStartYear, GameSpeedTypes eSpeed)
{
	int iTurnYear = getTurnYearForGame(iGameTurn, iStartYear, eCalendar, eSpeed);

	if (iTurnYear < 0)
	{
		if (bSave)
		{
			szString = gDLL->getText("TXT_KEY_TIME_BC_SAVE", CvWString::format(L"%04d", -iTurnYear).GetCString());
		}
		else
		{
			szString = gDLL->getText("TXT_KEY_TIME_BC", -(iTurnYear));
		}
	}
	else if (iTurnYear > 0)
	{
		if (bSave)
		{
			szString = gDLL->getText("TXT_KEY_TIME_AD_SAVE", CvWString::format(L"%04d", iTurnYear).GetCString());
		}
		else
		{
			szString = gDLL->getText("TXT_KEY_TIME_AD", iTurnYear);
		}
	}
	else
	{
		if (bSave)
		{
			szString = gDLL->getText("TXT_KEY_TIME_AD_SAVE", L"0001");
		}
		else
		{
			szString = gDLL->getText("TXT_KEY_TIME_AD", 1);
		}
	}
}


void CvGameTextMgr::setDateStr(CvWString& szString, int iGameTurn, bool bSave, CalendarTypes eCalendar, int iStartYear, GameSpeedTypes eSpeed)
{
	CvWString szYearBuffer;
	CvWString szWeekBuffer;

	setYearStr(szYearBuffer, iGameTurn, bSave, eCalendar, iStartYear, eSpeed);

	switch (eCalendar)
	{
	case CALENDAR_DEFAULT:
		if (0 == (getTurnMonthForGame(iGameTurn + 1, iStartYear, eCalendar, eSpeed) - getTurnMonthForGame(iGameTurn, iStartYear, eCalendar, eSpeed)) % GC.getNumMonthInfos())
		{
			szString = szYearBuffer;
		}
		else
		{
			int iMonth = getTurnMonthForGame(iGameTurn, iStartYear, eCalendar, eSpeed) % GC.getNumMonthInfos();
			if(iMonth < 0)
			{
				iMonth += GC.getNumMonthInfos();
			}

			if (bSave)
			{
				szString = (szYearBuffer + "-" + GC.getMonthInfo((MonthTypes)iMonth).getDescription());
			}
			else
			{
				szString = (GC.getMonthInfo((MonthTypes)iMonth).getDescription() + CvString(", ") + szYearBuffer);
			}
		}
		break;
	case CALENDAR_YEARS:
	case CALENDAR_BI_YEARLY:
		szString = szYearBuffer;
		break;

	case CALENDAR_TURNS:
		szString = gDLL->getText("TXT_KEY_TIME_TURN", (iGameTurn + 1));
		break;

	case CALENDAR_SEASONS:
		if (bSave)
		{
			szString = (szYearBuffer + "-" + GC.getSeasonInfo((SeasonTypes)(iGameTurn % GC.getNumSeasonInfos())).getDescription());
		}
		else
		{
			szString = (GC.getSeasonInfo((SeasonTypes)(iGameTurn % GC.getNumSeasonInfos())).getDescription() + CvString(", ") + szYearBuffer);
		}
		break;

	case CALENDAR_MONTHS:
		if (bSave)
		{
			szString = (szYearBuffer + "-" + GC.getMonthInfo((MonthTypes)(iGameTurn % GC.getNumMonthInfos())).getDescription());
		}
		else
		{
			szString = (GC.getMonthInfo((MonthTypes)(iGameTurn % GC.getNumMonthInfos())).getDescription() + CvString(", ") + szYearBuffer);
		}
		break;

	case CALENDAR_WEEKS:
		szWeekBuffer = gDLL->getText("TXT_KEY_TIME_WEEK", ((iGameTurn % GC.getDefineINT("WEEKS_PER_MONTHS")) + 1));

		if (bSave)
		{
			szString = (szYearBuffer + "-" + GC.getMonthInfo((MonthTypes)((iGameTurn / GC.getDefineINT("WEEKS_PER_MONTHS")) % GC.getNumMonthInfos())).getDescription() + "-" + szWeekBuffer);
		}
		else
		{
			szString = (szWeekBuffer + ", " + GC.getMonthInfo((MonthTypes)((iGameTurn / GC.getDefineINT("WEEKS_PER_MONTHS")) % GC.getNumMonthInfos())).getDescription() + ", " + szYearBuffer);
		}
		break;

	default:
		FAssert(false);
	}
}


void CvGameTextMgr::setTimeStr(CvWString& szString, int iGameTurn, bool bSave)
{
	setDateStr(szString, iGameTurn, bSave, GC.getGameINLINE().getCalendar(), GC.getGameINLINE().getStartYear(), GC.getGameINLINE().getGameSpeedType());
}


void CvGameTextMgr::setInterfaceTime(CvWString& szString, PlayerTypes ePlayer)
{
	CvWString szTempBuffer;

	clear(szString);

	setTimeStr(szTempBuffer, GC.getGameINLINE().getGameTurn(), false);
	szString += CvWString(szTempBuffer);
}


void CvGameTextMgr::setGoldStr(CvWString& szString, PlayerTypes ePlayer)
{
	if (GET_PLAYER(ePlayer).getGold() < 0)
	{
		szString.Format(SETCOLR L"%d" SETCOLR, TEXT_COLOR("COLOR_NEGATIVE_TEXT"), GET_PLAYER(ePlayer).getGold());
	}
	else
	{
		szString.Format(L"%d", GET_PLAYER(ePlayer).getGold());
	}
}

void CvGameTextMgr::setOOSSeeds(CvWString& szString, PlayerTypes ePlayer)
{
	if (GET_PLAYER(ePlayer).isHuman())
	{
		int iNetID = GET_PLAYER(ePlayer).getNetID();
		if (gDLL->isConnected(iNetID))
		{
			szString = gDLL->getText("TXT_KEY_PLAYER_OOS", gDLL->GetSyncOOS(iNetID), gDLL->GetOptionsOOS(iNetID));
		}
	}
}

void CvGameTextMgr::setNetStats(CvWString& szString, PlayerTypes ePlayer)
{
	if (ePlayer != GC.getGameINLINE().getActivePlayer())
	{
		if (GET_PLAYER(ePlayer).isHuman())
		{
			if (gDLL->getInterfaceIFace()->isNetStatsVisible())
			{
				int iNetID = GET_PLAYER(ePlayer).getNetID();
				if (gDLL->isConnected(iNetID))
				{
					szString = gDLL->getText("TXT_KEY_MISC_NUM_MS", gDLL->GetLastPing(iNetID));
				}
				else
				{
					szString = gDLL->getText("TXT_KEY_MISC_DISCONNECTED");
				}
			}
		}
		else
		{
			szString = gDLL->getText("TXT_KEY_MISC_AI");
		}
	}
}


void CvGameTextMgr::setMinimizePopupHelp(CvWString& szString, const CvPopupInfo & info)
{
	switch (info.getButtonPopupType())
	{
	case BUTTONPOPUP_CHOOSEPRODUCTION:
		{
			CvCity* pCity = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getCity(info.getData1());
			if (pCity != NULL)
			{
				UnitTypes eTrainUnit = NO_UNIT;
				BuildingTypes eConstructBuilding = NO_BUILDING;

				switch (info.getData2())
				{
				case (ORDER_TRAIN):
					eTrainUnit = (UnitTypes)info.getData3();
					break;
				case (ORDER_CONSTRUCT):
					eConstructBuilding = (BuildingTypes)info.getData3();
					break;
				default:
					break;
				}

				if (eTrainUnit != NO_UNIT)
				{
					szString += gDLL->getText("TXT_KEY_MINIMIZED_CHOOSE_PRODUCTION_UNIT", GC.getUnitInfo(eTrainUnit).getTextKeyWide(), pCity->getNameKey());
				}
				else if (eConstructBuilding != NO_BUILDING)
				{
					szString += gDLL->getText("TXT_KEY_MINIMIZED_CHOOSE_PRODUCTION_BUILDING", GC.getBuildingInfo(eConstructBuilding).getTextKeyWide(), pCity->getNameKey());
				}
				else
				{
					szString += gDLL->getText("TXT_KEY_MINIMIZED_CHOOSE_PRODUCTION", pCity->getNameKey());
				}
			}
		}
		break;
	case BUTTONPOPUP_CHOOSE_EDUCATION:
		{
			CvPlayer& kPlayer = GET_PLAYER(GC.getGameINLINE().getActivePlayer());
			CvCity* pCity = kPlayer.getCity(info.getData1());
			CvUnit* pUnit = kPlayer.getUnit(info.getData2());
			if (pCity != NULL && pUnit != NULL)
			{
				BuildingTypes eSchoolBuilding = pCity->getYieldBuilding(YIELD_EDUCATION);
				if (eSchoolBuilding != NO_BUILDING)
				{
					szString += gDLL->getText("TXT_KEY_MINIMIZED_CHOOSE_EDUCATION", pUnit->getNameOrProfessionKey(), pCity->getNameKey(), GC.getBuildingInfo(eSchoolBuilding).getTextKeyWide());
				}
			}

		}
		break;
	case BUTTONPOPUP_CHOOSE_YIELD_BUILD:
		{
			CvPlayer& kPlayer = GET_PLAYER(GC.getGameINLINE().getActivePlayer());
			CvCity* pCity = kPlayer.getCity(info.getData1());
			if (pCity != NULL)
			{
				szString += gDLL->getText("TXT_KEY_MINIMIZED_CHOOSE_YIELD_BUILD", pCity->getNameKey(), GC.getYieldInfo((YieldTypes) info.getData2()));
			}

		}
		break;
	}
}

void CvGameTextMgr::setUnitHelp(CvWStringBuffer &szString, const CvUnit* pUnit, bool bOneLine, bool bShort)
{
	PROFILE_FUNC();

	CvWString szTempBuffer;
	BuildTypes eBuild;
	int iI;
	bool bShift = gDLL->shiftKey();
	bool bAlt = gDLL->altKey();

	szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR("COLOR_UNIT_TEXT"), pUnit->getNameAndProfession().GetCString());
	szString.append(szTempBuffer);

	if (pUnit->canFight())
	{
		szString.append(L", ");

		if (pUnit->isFighting())
		{
			szTempBuffer.Format(L"?/%d%c", pUnit->baseCombatStr(), gDLL->getSymbolID(STRENGTH_CHAR));
		}
		else if (pUnit->isHurt())
		{
			szTempBuffer.Format(L"%.1f/%d%c", (((float)(pUnit->baseCombatStr() * pUnit->currHitPoints())) / ((float)(pUnit->maxHitPoints()))), pUnit->baseCombatStr(), gDLL->getSymbolID(STRENGTH_CHAR));
		}
		else
		{
			szTempBuffer.Format(L"%d%c", pUnit->baseCombatStr(), gDLL->getSymbolID(STRENGTH_CHAR));
		}
		szString.append(szTempBuffer);
	}

	if (pUnit->maxMoves() > 0)
	{
		szString.append(L", ");
		int iCurrMoves = ((pUnit->movesLeft() / GC.getMOVE_DENOMINATOR()) + (((pUnit->movesLeft() % GC.getMOVE_DENOMINATOR()) > 0) ? 1 : 0));
		if ((pUnit->baseMoves() == iCurrMoves) || (pUnit->getTeam() != GC.getGameINLINE().getActiveTeam()))
		{
			szTempBuffer.Format(L"%d%c", pUnit->baseMoves(), gDLL->getSymbolID(MOVES_CHAR));
		}
		else
		{
			szTempBuffer.Format(L"%d/%d%c", iCurrMoves, pUnit->baseMoves(), gDLL->getSymbolID(MOVES_CHAR));
		}
		szString.append(szTempBuffer);
	}

	if (pUnit->getYield() != NO_YIELD)
	{
		// R&R, ray, Africa - START
		szString.append(L", ");
		szString.append(gDLL->getText("TXT_KEY_EUROPE_PRICE_HELP")); // new line to old code
		szTempBuffer.Format(L"%d%c", pUnit->getYieldStored(), GC.getYieldInfo(pUnit->getYield()).getChar());
		szString.append(szTempBuffer);
		int iValue = GET_PLAYER(pUnit->getOwnerINLINE()).getSellToEuropeProfit(pUnit->getYield(), pUnit->getYieldStored());
		if (iValue > 0)
		{
			szTempBuffer.Format(L" (%d%c)", iValue, gDLL->getSymbolID(GOLD_CHAR));
			szString.append(szTempBuffer);
		}
		
		szString.append(L", ");
		szString.append(gDLL->getText("TXT_KEY_AFRICA_PRICE_HELP")); // new line
		szTempBuffer.Format(L"%d%c", pUnit->getYieldStored(), GC.getYieldInfo(pUnit->getYield()).getChar());
		szString.append(szTempBuffer);
		int iValueAfrica = GET_PLAYER(pUnit->getOwnerINLINE()).getSellToAfricaProfit(pUnit->getYield(), pUnit->getYieldStored());
		if (iValueAfrica > 0)
		{
			szTempBuffer.Format(L" (%d%c)", iValueAfrica, gDLL->getSymbolID(GOLD_CHAR));
			szString.append(szTempBuffer);
		}
		// R&R, ray, Africa - END
		// R&R, ray, Port Royal
		szString.append(L", ");
		szString.append(gDLL->getText("TXT_KEY_PORT_ROYAL_PRICE_HELP")); // new line
		szTempBuffer.Format(L"%d%c", pUnit->getYieldStored(), GC.getYieldInfo(pUnit->getYield()).getChar());
		szString.append(szTempBuffer);
		int iValuePortRoyal = GET_PLAYER(pUnit->getOwnerINLINE()).getSellToPortRoyalProfit(pUnit->getYield(), pUnit->getYieldStored());
		if (iValuePortRoyal > 0)
		{
			szTempBuffer.Format(L" (%d%c)", iValuePortRoyal, gDLL->getSymbolID(GOLD_CHAR));
			szString.append(szTempBuffer);
		}
		// R&R, ray, Port Royal - END
	}

	eBuild = pUnit->getBuildType();

	if (eBuild != NO_BUILD)
	{
		szString.append(L", ");
		szTempBuffer.Format(L"%s (%d)", GC.getBuildInfo(eBuild).getDescription(), pUnit->plot()->getBuildTurnsLeft(eBuild, 0, 0));
		szString.append(szTempBuffer);
	}

	if (pUnit->getImmobileTimer() > 0)
	{
		szString.append(L", ");
		szString.append(gDLL->getText("TXT_KEY_UNIT_HELP_IMMOBILE", pUnit->getImmobileTimer()));
	}

	if (GC.getGameINLINE().isDebugMode() && !bAlt && !bShift && (pUnit->AI_getUnitAIType() != NO_UNITAI))
	{
	    szTempBuffer.Format(L" %s %s\n", GC.getUnitAIInfo(pUnit->AI_getUnitAIType()).getDescription(), getUnitAIStateString(pUnit->AI_getUnitAIState()).GetCString());
		szString.append(szTempBuffer);
	}

	if ((pUnit->getTeam() == GC.getGameINLINE().getActiveTeam()) || GC.getGameINLINE().isDebugMode())
	{
		if ((pUnit->getExperience() > 0) && !(pUnit->isFighting()))
		{
			szString.append(gDLL->getText("TXT_KEY_UNIT_HELP_LEVEL", pUnit->getExperience(), pUnit->experienceNeeded()));
		}
	}

	//WTP, ray fixed Nationality Text displayed for Barbarians / Wild Animals
	if (pUnit->getOwnerINLINE() != GC.getGameINLINE().getActivePlayer() && !pUnit->getUnitInfo().isHiddenNationality() && pUnit->getOwnerINLINE() != GC.getGameINLINE().getBarbarianPlayer())
	{
		szString.append(L", ");
		szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, GET_PLAYER(pUnit->getOwnerINLINE()).getPlayerTextColorR(), GET_PLAYER(pUnit->getOwnerINLINE()).getPlayerTextColorG(), GET_PLAYER(pUnit->getOwnerINLINE()).getPlayerTextColorB(), GET_PLAYER(pUnit->getOwnerINLINE()).getPlayerTextColorA(), GET_PLAYER(pUnit->getOwnerINLINE()).getName());
		szString.append(szTempBuffer);
	}

	//WTP, ray add LbD Tunrs Worked and LbD Last Profession to Unit Help - START
	if (pUnit->getLbDrounds() != 0 && pUnit->getLastLbDProfession() != NO_PROFESSION)
	{
		const ProfessionTypes lastProfession = pUnit->getLastLbDProfession();
		bool bCanBecomeExpert = pUnit->getUnitInfo().LbD_canBecomeExpert();
		int iLbDRoundsWorked = pUnit->getLbDrounds();

		// Display for become Expert - with turns worked and Expert Unit in Text
		if(bCanBecomeExpert && lastProfession != NO_PROFESSION && GC.getProfessionInfo(lastProfession).LbD_isUsed() && iLbDRoundsWorked >0)
		{
			int expert = GC.getProfessionInfo(lastProfession).LbD_getExpert();
			UnitTypes expertUnitType = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getCivilizationType()).getCivilizationUnits(expert);
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_MISC_HELP_LBD_BECOME_EXPPERT_TURNS_WORKED_MAP", iLbDRoundsWorked, GC.getUnitInfo(expertUnitType).getDescription()));
			szString.append(SEPARATOR);
		}

	}
	//WTP, ray add LbD Tunrs Worked and LbD Last Profession to Unit Help - END

	// WTP, ray, saving 1 more Profession for Fisher Issue - START
	if (pUnit->getLbDroundsBefore() != 0 && pUnit->getLastLbDProfessionBefore() != NO_PROFESSION)
	{
		const ProfessionTypes lastProfessionBefore = pUnit->getLastLbDProfessionBefore();
		bool bCanBecomeExpert = pUnit->getUnitInfo().LbD_canBecomeExpert();
		int iLbDRoundsWorkedBefore = pUnit->getLbDroundsBefore();
		// Display Profession before Last for become Expert - with turns worked and Expert Unit in Text
		if(bCanBecomeExpert && lastProfessionBefore != NO_PROFESSION && GC.getProfessionInfo(lastProfessionBefore).LbD_isUsed() && iLbDRoundsWorkedBefore >0)
		{
			int expertBefore = GC.getProfessionInfo(lastProfessionBefore).LbD_getExpert();
			UnitTypes expertUnitTypeBefore = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getCivilizationType()).getCivilizationUnits(expertBefore);
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_MISC_HELP_LBD_BECOME_EXPPERT_TURNS_WORKED_MAP", iLbDRoundsWorkedBefore, GC.getUnitInfo(expertUnitTypeBefore).getDescription()));
			szString.append(SEPARATOR);
		}
	
	}
	// WTP, ray, saving 1 more Profession for Fisher Issue - END

	for (iI = 0; iI < GC.getNumPromotionInfos(); ++iI)
	{
		if (!GC.getPromotionInfo((PromotionTypes)iI).isGraphicalOnly() && pUnit->isHasPromotion((PromotionTypes)iI))
		{
			szTempBuffer.Format(L"<img=%S size=16></img>", GC.getPromotionInfo((PromotionTypes)iI).getButton());
			szString.append(szTempBuffer);
		}
	}

	//TAC Whaling, ray
	if (pUnit->getGroup() != NULL)
	{
		if (pUnit->getGroup()->headMissionQueueNode() != NULL)
		{
			// R&R, ray, High Sea Fishing - START
			// if (pUnit->getGroup()->headMissionQueueNode()->m_data.eMissionType == MISSION_WHALING)
			bool isWhaler = (pUnit->getGroup()->headMissionQueueNode()->m_data.eMissionType == MISSION_WHALING);
			bool isFisher = (pUnit->getGroup()->headMissionQueueNode()->m_data.eMissionType == MISSION_FISHING);
			// R&R, ray, High Sea Fishing - END
			if (isWhaler || isFisher)
			{
				std::set<YieldTypes> setGatherable;

				for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
				{
					if (pUnit->getUnitInfo().canGatherYield(iYield))
					{
						int iBonusGathered = GC.getBonusInfo(pUnit->plot()->getBonusType()).getYieldChange(iYield);
						if (iBonusGathered > 0)
						{
							setGatherable.insert((YieldTypes)iYield);
						}
					}
				}

				if (!setGatherable.empty())
				{
					CvWString szYieldsList;
					for (std::set<YieldTypes>::iterator it = setGatherable.begin(); it != setGatherable.end(); ++it)
					{
						int iBonusGathered = GC.getBonusInfo(pUnit->plot()->getBonusType()).getYieldChange(*it);
						// R&R, ray, High Sea Fishing - START
						if (iBonusGathered > 0 && iBonusGathered < 10)
						{
							iBonusGathered = 10;
						}
						// R&R, ray, High Sea Fishing - END

						if (!szYieldsList.empty())
						{
							szYieldsList += L", ";
						}

						szYieldsList += CvWString::format(L"%d %c", iBonusGathered, GC.getYieldInfo(*it).getChar());
					}

					// R&R, ray, High Sea Fishing - START
					if (isWhaler)
					{
						szString.append(NEWLINE);
						szTempBuffer.Format(L"%s", GC.getMissionInfo(MISSION_WHALING).getDescription());
						szString.append(szTempBuffer);
						szString.append(NEWLINE);
						szString.append(gDLL->getText("TXT_KEY_WHALER_HELP", szYieldsList.GetCString()));
					}
					else if (isFisher)
					{
						szString.append(NEWLINE);
						szTempBuffer.Format(L"%s", GC.getMissionInfo(MISSION_FISHING).getDescription());
						szString.append(szTempBuffer);
						szString.append(NEWLINE);
						szString.append(gDLL->getText("TXT_KEY_FISHER_HELP", szYieldsList.GetCString()));
					}
					// R&R, ray, High Sea Fishing - END
				}
			}
		}
	}
	//End TAC Whaling, ray

    if (bAlt && (gDLL->getChtLvl() > 0))
    {
		CvSelectionGroup* eGroup = pUnit->getGroup();
		if (eGroup != NULL)
		{
			if (pUnit->isGroupHead())
				szString.append(CvWString::format(L"\nLeading "));
			else
				szString.append(L"\n");

			szTempBuffer.Format(L"Group(%d), %d units", eGroup->getID(), eGroup->getNumUnits());
			szString.append(szTempBuffer);
		}
    }

	if (!bOneLine)
	{
		if (pUnit->cargoSpace() > 0)
		{
			if (pUnit->getTeam() == GC.getGameINLINE().getActiveTeam())
			{
				szTempBuffer = NEWLINE + gDLL->getText("TXT_KEY_UNIT_HELP_CARGO_SPACE", pUnit->getCargo(), pUnit->cargoSpace());
			}
			else
			{
				szTempBuffer = NEWLINE + gDLL->getText("TXT_KEY_UNIT_CARGO_SPACE", pUnit->cargoSpace());
			}
			szString.append(szTempBuffer);

			//TAC Whaling, ray
			if (pUnit->specialCargo() != NO_SPECIALUNIT && !pUnit->getUnitInfo().isGatherBoat())
			{
				szString.append(gDLL->getText("TXT_KEY_UNIT_CARRIES", GC.getSpecialUnitInfo(pUnit->specialCargo()).getTextKeyWide()));
			}
			//End TAC Whaling, ray
		}

		if (pUnit->fortifyModifier() != 0)
		{
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_UNIT_HELP_FORTIFY_BONUS", pUnit->fortifyModifier()));
		}

		if (!bShort)
		{
			if (pUnit->alwaysInvisible())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_INVISIBLE_ALL"));
			}
			// R&R, ray, Rangers
			else if (pUnit->getInvisibleType() == INVISIBLE_RANGER)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_INVISIBLE_RANGER"));
			}
			// R&R, ray, Rangers, END
			else if (pUnit->getInvisibleType() != NO_INVISIBLE)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_INVISIBLE_MOST"));
			}

			for (iI = 0; iI < pUnit->getNumSeeInvisibleTypes(); ++iI)
			{
				if (pUnit->getSeeInvisibleType(iI) != pUnit->getInvisibleType())
				{
					// R&R, ray, Rangers
					if(pUnit->getSeeInvisibleType(iI) == INVISIBLE_RANGER)
					{
						szString.append(NEWLINE);
						szString.append(gDLL->getText("TXT_KEY_SEE_RANGER"));
					}
					else
					{
						szString.append(NEWLINE);
						szString.append(gDLL->getText("TXT_KEY_UNIT_SEE_INVISIBLE", GC.getInvisibleInfo(pUnit->getSeeInvisibleType(iI)).getTextKeyWide()));
					}
					// R&R, ray, Rangers, END
				}
			}

			if (pUnit->canMoveImpassable())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_CAN_MOVE_IMPASSABLE"));
			}

			/// Move Into Peak - start - Nightinggale
			if (pUnit->getUnitInfo().allowsMoveIntoPeak())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_CAN_MOVE_PEAK"));
			}
			/// Move Into Peak - end - Nightinggale

			// R&R, ray, Start Logic for Trains
			if (pUnit->getDomainType() == DOMAIN_LAND && pUnit->cargoSpace() == 6)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_TRAVEL_RAILROAD_ONLY"));
			}
			// R&R, ray, END Logic for Trains

			// R&R, ray, changes to Wild Animals - START
			if (pUnit->getUnitInfo().isAnimal())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_ANIMAL"));

				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_ANIMAL_CANNOT_ENTER_PLAYER_TERRITORY"));

				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_ANIMAL_KILLING_REWARD"));

				for (iI = 0; iI < GC.getNumTerrainInfos(); ++iI)
				{
					if (pUnit->getUnitInfo().getTerrainNative(iI))
					{
						szString.append(NEWLINE);
						szString.append(gDLL->getText("TXT_KEY_UNIT_TERRAIN_NATIVE", GC.getTerrainInfo((TerrainTypes) iI).getDescription()));
					}
				}
			
			}
			// R&R, ray, changes to Wild Animals - END
		}

		if (!bShort)
		{
			if (pUnit->noDefensiveBonus())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_NO_DEFENSE_BONUSES"));
			}

			if (pUnit->flatMovementCost())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_FLAT_MOVEMENT"));
			}

			if (pUnit->ignoreTerrainCost())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_IGNORE_TERRAIN"));
			}

			if (pUnit->isBlitz())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_BLITZ_TEXT"));
			}

			if (pUnit->isAmphib())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_AMPHIB_TEXT"));
			}

			if (pUnit->isRiver())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_RIVER_ATTACK_TEXT"));
			}

			if (pUnit->isEnemyRoute())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_ENEMY_ROADS_TEXT"));
			}

			if (pUnit->isAlwaysHeal())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_ALWAYS_HEAL_TEXT"));
			}

			if (pUnit->isHillsDoubleMove())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_HILLS_MOVE_TEXT"));
			}

			for (iI = 0; iI < GC.getNumTerrainInfos(); ++iI)
			{
				if (pUnit->isTerrainDoubleMove((TerrainTypes)iI))
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_PROMOTION_DOUBLE_MOVE_TEXT", GC.getTerrainInfo((TerrainTypes) iI).getTextKeyWide()));
				}
			}

			for (iI = 0; iI < GC.getNumFeatureInfos(); ++iI)
			{
				if (pUnit->isFeatureDoubleMove((FeatureTypes)iI))
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_PROMOTION_DOUBLE_MOVE_TEXT", GC.getFeatureInfo((FeatureTypes) iI).getTextKeyWide()));
				}
			}

			if (pUnit->getExtraVisibilityRange() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_VISIBILITY_TEXT", pUnit->getExtraVisibilityRange()));
			}

			if (pUnit->getExtraMoveDiscount() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_MOVE_DISCOUNT_TEXT", -(pUnit->getExtraMoveDiscount())));
			}

			if (pUnit->getExtraEnemyHeal() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_HEALS_EXTRA_TEXT", pUnit->getExtraEnemyHeal()) + gDLL->getText("TXT_KEY_PROMOTION_ENEMY_LANDS_TEXT"));
			}

			if (pUnit->getExtraNeutralHeal() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_HEALS_EXTRA_TEXT", pUnit->getExtraNeutralHeal()) + gDLL->getText("TXT_KEY_PROMOTION_NEUTRAL_LANDS_TEXT"));
			}

			if (pUnit->getExtraFriendlyHeal() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_HEALS_EXTRA_TEXT", pUnit->getExtraFriendlyHeal()) + gDLL->getText("TXT_KEY_PROMOTION_FRIENDLY_LANDS_TEXT"));
			}

			if (pUnit->getSameTileHeal() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_HEALS_SAME_TEXT", pUnit->getSameTileHeal()) + gDLL->getText("TXT_KEY_PROMOTION_DAMAGE_TURN_TEXT"));
			}

			if (pUnit->getAdjacentTileHeal() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_HEALS_ADJACENT_TEXT", pUnit->getAdjacentTileHeal()) + gDLL->getText("TXT_KEY_PROMOTION_DAMAGE_TURN_TEXT"));
			}
		}

		if (pUnit->withdrawalProbability() > 0)
		{
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_UNIT_WITHDRAWL_PROBABILITY_SHORT", pUnit->withdrawalProbability()));
		}

		CvCity* pEvasionCity = pUnit->getEvasionCity();
		if (pEvasionCity != NULL)
		{
			// hide the colony name if the unit is foreign and has hidden nationality.
			// If a ship can escape to Jamestown, we know it's English even if it has hidden nationality, hence leaking hidden information.
			bool bSameTeam = pUnit->getTeam() == GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getTeam();
			bool bHidden = !bSameTeam && pUnit->getUnitInfo().isHiddenNationality();
			bool bKnownCity = pEvasionCity->isRevealed(pUnit->getTeam(), true);
			bool bNoVariablesHidden = GC.getGameINLINE().isOption(GAMEOPTION_NO_MORE_VARIABLES_HIDDEN);

			bool bReveal = bSameTeam || (!bHidden && bKnownCity && bNoVariablesHidden);

			if (bReveal)
			{
				if (bShort)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_EVASION_WITH_CITY_SHORT", pEvasionCity->getNameKey()));
				}
				else
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_EVASION_WITH_CITY", pEvasionCity->getNameKey()));
				}
			}
			else if (bNoVariablesHidden)
			{
				// skip revealing the destination
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_EVASION_SHORT"));
			}
		}

		if (pUnit->getExtraCombatPercent() != 0)
		{
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_PROMOTION_STRENGTH_TEXT", pUnit->getExtraCombatPercent()));
		}

		if (pUnit->cityAttackModifier() == pUnit->cityDefenseModifier())
		{
			if (pUnit->cityAttackModifier() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_CITY_STRENGTH_MOD", pUnit->cityAttackModifier()));
			}
		}
		else
		{
			if (pUnit->cityAttackModifier() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_CITY_ATTACK_TEXT", pUnit->cityAttackModifier()));
			}

			if (pUnit->cityDefenseModifier() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PROMOTION_CITY_DEFENSE_TEXT", pUnit->cityDefenseModifier()));
			}
		}

		if (pUnit->hillsAttackModifier() == pUnit->hillsDefenseModifier())
		{
			if (pUnit->hillsAttackModifier() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_HILLS_STRENGTH", pUnit->hillsAttackModifier()));
			}
		}
		else
		{
			if (pUnit->hillsAttackModifier() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_HILLS_ATTACK", pUnit->hillsAttackModifier()));
			}

			if (pUnit->hillsDefenseModifier() != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_HILLS_DEFENSE", pUnit->hillsDefenseModifier()));
			}
		}

		if (pUnit->DomesticBonusModifier() != 0)
		{
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_UNIT_DOMESTIC_BONUS", pUnit->DomesticBonusModifier()));			
		}

		for (iI = 0; iI < GC.getNumTerrainInfos(); ++iI)
		{
			if (pUnit->terrainAttackModifier((TerrainTypes)iI) == pUnit->terrainDefenseModifier((TerrainTypes)iI))
			{
				if (pUnit->terrainAttackModifier((TerrainTypes)iI) != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_STRENGTH", pUnit->terrainAttackModifier((TerrainTypes)iI), GC.getTerrainInfo((TerrainTypes) iI).getTextKeyWide()));
				}
			}
			else
			{
				if (pUnit->terrainAttackModifier((TerrainTypes)iI) != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_ATTACK", pUnit->terrainAttackModifier((TerrainTypes)iI), GC.getTerrainInfo((TerrainTypes) iI).getTextKeyWide()));
				}

				if (pUnit->terrainDefenseModifier((TerrainTypes)iI) != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_DEFENSE", pUnit->terrainDefenseModifier((TerrainTypes)iI), GC.getTerrainInfo((TerrainTypes) iI).getTextKeyWide()));
				}
			}
		}

		for (iI = 0; iI < GC.getNumFeatureInfos(); ++iI)
		{
			if (pUnit->featureAttackModifier((FeatureTypes)iI) == pUnit->featureDefenseModifier((FeatureTypes)iI))
			{
				if (pUnit->featureAttackModifier((FeatureTypes)iI) != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_STRENGTH", pUnit->featureAttackModifier((FeatureTypes)iI), GC.getFeatureInfo((FeatureTypes) iI).getTextKeyWide()));
				}
			}
			else
			{
				if (pUnit->featureAttackModifier((FeatureTypes)iI) != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_ATTACK", pUnit->featureAttackModifier((FeatureTypes)iI), GC.getFeatureInfo((FeatureTypes) iI).getTextKeyWide()));
				}

				if (pUnit->featureDefenseModifier((FeatureTypes)iI) != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_DEFENSE", pUnit->featureDefenseModifier((FeatureTypes)iI), GC.getFeatureInfo((FeatureTypes) iI).getTextKeyWide()));
				}
			}
		}

		for (iI = 0; iI < GC.getNumUnitClassInfos(); ++iI)
		{
			UnitClassTypes eUnitClass = (UnitClassTypes) iI;
			int iAttackModifier = pUnit->unitClassAttackModifier(eUnitClass);
			int iDefenseModifier = pUnit->unitClassDefenseModifier(eUnitClass);
			if (iAttackModifier == iDefenseModifier)
			{
				if (iAttackModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_MOD_VS_TYPE", iAttackModifier, GC.getUnitClassInfo(eUnitClass).getTextKeyWide()));
				}
			}
			else
			{
				if (iAttackModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_ATTACK_MOD_VS_CLASS", iAttackModifier, GC.getUnitClassInfo(eUnitClass).getTextKeyWide()));
				}

				if (iDefenseModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_DEFENSE_MOD_VS_CLASS", iDefenseModifier, GC.getUnitClassInfo(eUnitClass).getTextKeyWide()));
				}
			}
		}

		for (iI = 0; iI < GC.getNumUnitCombatInfos(); ++iI)
		{
			if (pUnit->unitCombatModifier((UnitCombatTypes)iI) != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_MOD_VS_TYPE_NO_LINK", pUnit->unitCombatModifier((UnitCombatTypes)iI), GC.getUnitCombatInfo((UnitCombatTypes) iI).getTextKeyWide()));
			}
		}

		for (iI = 0; iI < NUM_DOMAIN_TYPES; ++iI)
		{
			if (pUnit->domainModifier((DomainTypes)iI) != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_MOD_VS_TYPE_NO_LINK", pUnit->domainModifier((DomainTypes)iI), GC.getDomainInfo((DomainTypes)iI).getTextKeyWide()));
			}
		}

		if (pUnit->bombardRate() > 0)
		{
			if (bShort)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_BOMBARD_RATE_SHORT", ((pUnit->bombardRate() * 100) / GC.getMAX_CITY_DEFENSE_DAMAGE())));
			}
			else
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_UNIT_BOMBARD_RATE", ((pUnit->bombardRate() * 100) / GC.getMAX_CITY_DEFENSE_DAMAGE())));
			}
		}

		if (pUnit->getUnitInfo().isTreasure())
		{
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_UNIT_TREASURE_NUMBER_HELP", pUnit->getYieldStored()));
		}
		
		if (!isEmpty(pUnit->getUnitInfo().getHelp()))
		{
			szString.append(NEWLINE);
			szString.append(pUnit->getUnitInfo().getHelp());
		}

        if (bShift && (gDLL->getChtLvl() > 0))
        {
            szTempBuffer.Format(L"\nUnitAI Type = %s.", GC.getUnitAIInfo(pUnit->AI_getUnitAIType()).getDescription());
            szString.append(szTempBuffer);
            szTempBuffer.Format(L"\nSacrifice Value = %d.", pUnit->AI_sacrificeValue(NULL));
            szString.append(szTempBuffer);
            if (pUnit->getHomeCity() != NULL)
            {
				szTempBuffer.Format(L"\nHome City = %s.", pUnit->getHomeCity()->getName().GetCString());
				szString.append(szTempBuffer);
            }
        }
	}
}

void CvGameTextMgr::setUnitPromotionHelp(CvWStringBuffer &szString, const CvUnit* pUnit)
{
	std::vector<PromotionTypes> aPromotions;
	for (int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); ++iPromotion)
	{
		if (!GC.getPromotionInfo((PromotionTypes) iPromotion).isGraphicalOnly())
		{
			if (pUnit->isHasPromotion((PromotionTypes) iPromotion))
			{
				aPromotions.push_back((PromotionTypes) iPromotion);
			}
		}
	}

	for (uint i = 0; i < aPromotions.size(); ++i)
	{
		if (!szString.isEmpty())
		{
			szString.append(NEWLINE);
		}
		if (aPromotions.size() > 10)
		{
			szString.append(CvWString::format(SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), GC.getPromotionInfo(aPromotions[i]).getDescription()));
		}
		else
		{
			setPromotionHelp(szString, aPromotions[i], false);
		}
	}
}


void CvGameTextMgr::setProfessionHelp(CvWStringBuffer &szBuffer, ProfessionTypes eProfession, bool bCivilopediaText, bool bStrategyText)
{
	CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);

	CvWString szTempBuffer;

	if (!bCivilopediaText)
	{
		szTempBuffer.Format(SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_UNIT_TEXT"), GC.getProfessionInfo(eProfession).getDescription());
		szBuffer.append(szTempBuffer);
	}

	if (!bCivilopediaText)
	{
		// R&R, ray , MYCP partially based on code of Aymerick - START
		std::vector<YieldTypes> eYieldsConsumed;
		for (int i = 0; i < kProfession.getNumYieldsConsumed(); i++)
		{
			YieldTypes eYieldConsumed = (YieldTypes) kProfession.getYieldsConsumed(i);
			if (eYieldConsumed != NO_YIELD)
			{
				eYieldsConsumed.push_back((YieldTypes) eYieldConsumed);
			}
		}
		if (!eYieldsConsumed.empty())
		{
			CvWString szYieldsConsumedList;
			for (std::vector<YieldTypes>::iterator it = eYieldsConsumed.begin(); it != eYieldsConsumed.end(); ++it)
			{
				if (!szYieldsConsumedList.empty())
				{
					if (*it == eYieldsConsumed.back())
					{
						szYieldsConsumedList += CvWString::format(gDLL->getText("TXT_KEY_AND"));
					}
					else
					{
						szYieldsConsumedList += L", ";
					}
				}
				szYieldsConsumedList += CvWString::format(L"%c", GC.getYieldInfo(*it).getChar());
			}
			szTempBuffer.Format(gDLL->getText("TXT_KEY_YIELDS_CONSUMED_WITH_CHAR", szYieldsConsumedList.GetCString()));
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}
		
		std::vector<YieldTypes> eYieldsProduced;
		for (int i = 0; i < kProfession.getNumYieldsProduced(); i++)
		{
			YieldTypes eYieldProduced = (YieldTypes) kProfession.getYieldsProduced(i);
			if (eYieldProduced != NO_YIELD)
			{
				eYieldsProduced.push_back((YieldTypes) eYieldProduced);
			}
		}
		if (!eYieldsProduced.empty())
		{
			CvWString szYieldsProducedList;
			for (std::vector<YieldTypes>::iterator it = eYieldsProduced.begin(); it != eYieldsProduced.end(); ++it)
			{
				if (!szYieldsProducedList.empty())
				{
					if (*it == eYieldsProduced.back())
					{
						szYieldsProducedList += CvWString::format(gDLL->getText("TXT_KEY_AND"));
					}
					else
					{
						szYieldsProducedList += L", ";
					}
				}
				szYieldsProducedList += CvWString::format(L"%c", GC.getYieldInfo(*it).getChar());
			}
			szTempBuffer.Format(gDLL->getText("TXT_KEY_YIELDS_PRODUCED_WITH_CHAR", szYieldsProducedList.GetCString()));
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}
		// R&R, ray , MYCP partially based on code of Aymerick - END
		int iCombatChange = kProfession.getCombatChange();
		if (GC.getGameINLINE().getActivePlayer() != NO_PLAYER)
		{
			iCombatChange += GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getProfessionCombatChange(eProfession);
		}
		int iMovesChange = kProfession.getMovesChange();

		if (iCombatChange != 0 || iMovesChange != 0)
		{
			szBuffer.append(NEWLINE);
			szTempBuffer.Format(L"%c", gDLL->getSymbolID(BULLET_CHAR));
			szBuffer.append(szTempBuffer);

			if (iCombatChange != 0)
			{
				szTempBuffer.Format(L"%d%c ", iCombatChange, gDLL->getSymbolID(STRENGTH_CHAR));
				szBuffer.append(szTempBuffer);
			}

			if (iMovesChange != 0)
			{
				szTempBuffer.Format(L"%d%c ", iMovesChange, gDLL->getSymbolID(MOVES_CHAR));
				szBuffer.append(szTempBuffer);
			}
		}
	}

	if (kProfession.getWorkRate() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CAN_IMPROVE_LAND"));
	}

	if (kProfession.getMissionaryRate() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CAN_ESTABLISH_MISSIONS"));
	}

	// WTP, ray, Native Trade Posts - START
	if (kProfession.getNativeTradeRate() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CAN_ESTABLISH_TRADE_POST"));
	}
	// WTP, ray, Native Trade Posts - END

	if (kProfession.canFound())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CAN_FOUND"));
	}

	//WTP, ray, Large Rivers - START
	if (kProfession.isCanCrossLargeRivers())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_PROFESSION_CAN_CROSS_LARGE_RIVERS"));
	}
	//WTP, ray, END


	/// Move Into Peak - start - Nightinggale
	if (kProfession.allowsMoveIntoPeak())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CAN_MOVE_PEAK"));
	}
	/// Move Into Peak - end - Nightinggale

	if (kProfession.isNoDefensiveBonus())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_NO_DEFENSE_BONUSES"));
	}

	if (kProfession.isUnarmed())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_ONLY_DEFENSIVE"));
	}

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		int iYieldAmount = GC.getGameINLINE().getActivePlayer() != NO_PLAYER ? GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getYieldEquipmentAmount(eProfession, (YieldTypes) iYield) : kProfession.getYieldEquipmentAmount((YieldTypes) iYield);
		if (iYieldAmount != 0)
		{
			szTempBuffer.Format(gDLL->getText("TXT_KEY_UNIT_REQUIRES_YIELD_QUANTITY_STRING", iYieldAmount, GC.getYieldInfo((YieldTypes) iYield).getChar()));
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}
	}

	for (int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); ++iPromotion)
	{
		if (kProfession.isFreePromotion(iPromotion))
		{
			setPromotionHelp(szBuffer, (PromotionTypes) iPromotion, true);
		}
	}
}

void CvGameTextMgr::setPlotListHelp(CvWStringBuffer &szString, const CvPlot* pPlot, bool bOneLine, bool bShort)
{
	PROFILE_FUNC();

	int numPromotionInfos = GC.getNumPromotionInfos();

	// if cheatmode and ctrl, display grouping info instead
	if ((gDLL->getChtLvl() > 0) && gDLL->ctrlKey())
	{
		if (pPlot->isVisible(GC.getGameINLINE().getActiveTeam(), GC.getGameINLINE().isDebugMode()))
		{
			CvWString szTempString;

			CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
			while(pUnitNode != NULL)
			{
				CvUnit* pHeadUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = pPlot->nextUnitNode(pUnitNode);

				// is this unit the head of a group, not cargo, and visible?
				if (pHeadUnit && pHeadUnit->isGroupHead() && !pHeadUnit->isCargo() && !pHeadUnit->isInvisible(GC.getGameINLINE().getActiveTeam(), GC.getGameINLINE().isDebugMode()))
				{
					// head unit name and unitai
					szString.append(CvWString::format(SETCOLR L"%s" ENDCOLR, 255,190,0,255, pHeadUnit->getName().GetCString()));
					szString.append(CvWString::format(L" (%d)", shortenID(pHeadUnit->getID())));
					getUnitAIString(szTempString, pHeadUnit->AI_getUnitAIType());
					szString.append(CvWString::format(SETCOLR L" %s " ENDCOLR, GET_PLAYER(pHeadUnit->getOwnerINLINE()).getPlayerTextColorR(), GET_PLAYER(pHeadUnit->getOwnerINLINE()).getPlayerTextColorG(), GET_PLAYER(pHeadUnit->getOwnerINLINE()).getPlayerTextColorB(), GET_PLAYER(pHeadUnit->getOwnerINLINE()).getPlayerTextColorA(), szTempString.GetCString()));

					// promotion icons
					for (int iPromotionIndex = 0; iPromotionIndex < numPromotionInfos; iPromotionIndex++)
					{
						PromotionTypes ePromotion = (PromotionTypes)iPromotionIndex;
						if (!GC.getPromotionInfo(ePromotion).isGraphicalOnly() && pHeadUnit->isHasPromotion(ePromotion))
						{
							szString.append(CvWString::format(L"<img=%S size=16></img>", GC.getPromotionInfo(ePromotion).getButton()));
						}
					}

					// group
					CvSelectionGroup* pHeadGroup = pHeadUnit->getGroup();
					FAssertMsg(pHeadGroup != NULL, "unit has NULL group");
					if (pHeadGroup->getNumUnits() > 1)
					{
						szString.append(CvWString::format(L"\nGroup:%d [%d units]", shortenID(pHeadGroup->getID()), pHeadGroup->getNumUnits()));

						// get average damage
						int iAverageDamage = 0;
						CLLNode<IDInfo>* pUnitNode = pHeadGroup->headUnitNode();
						while (pUnitNode != NULL)
						{
							CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
							pUnitNode = pHeadGroup->nextUnitNode(pUnitNode);

							iAverageDamage += (pLoopUnit->getDamage() * pLoopUnit->maxHitPoints()) / 100;
						}
						iAverageDamage /= pHeadGroup->getNumUnits();
						if (iAverageDamage > 0)
						{
							szString.append(CvWString::format(L" %d%%", 100 - iAverageDamage));
						}
					}

					// mission ai
					MissionAITypes eMissionAI = pHeadGroup->AI_getMissionAIType();
					if (eMissionAI != NO_MISSIONAI)
					{
						getMissionAIString(szTempString, eMissionAI);
						szString.append(CvWString::format(SETCOLR L"\n%s" ENDCOLR, TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), szTempString.GetCString()));
					}

					// mission
					MissionTypes eMissionType = (MissionTypes) pHeadGroup->getMissionType(0);
					if (eMissionType != NO_MISSION)
					{
						getMissionTypeString(szTempString, eMissionType);
						szString.append(CvWString::format(SETCOLR L"\n%s" ENDCOLR, TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), szTempString.GetCString()));
					}

					// mission unit
					CvUnit* pMissionUnit = pHeadGroup->AI_getMissionAIUnit();
					if (pMissionUnit != NULL && (eMissionAI != NO_MISSIONAI || eMissionType != NO_MISSION))
					{
						// mission unit
						szString.append(L"\n to ");
						szString.append(CvWString::format(SETCOLR L"%s" ENDCOLR, GET_PLAYER(pMissionUnit->getOwnerINLINE()).getPlayerTextColorR(), GET_PLAYER(pMissionUnit->getOwnerINLINE()).getPlayerTextColorG(), GET_PLAYER(pMissionUnit->getOwnerINLINE()).getPlayerTextColorB(), GET_PLAYER(pMissionUnit->getOwnerINLINE()).getPlayerTextColorA(), pMissionUnit->getName().GetCString()));
						szString.append(CvWString::format(L"(%d) G:%d", shortenID(pMissionUnit->getID()), shortenID(pMissionUnit->getGroupID())));
						getUnitAIString(szTempString, pMissionUnit->AI_getUnitAIType());
						szString.append(CvWString::format(SETCOLR L" %s" ENDCOLR, GET_PLAYER(pMissionUnit->getOwnerINLINE()).getPlayerTextColorR(), GET_PLAYER(pMissionUnit->getOwnerINLINE()).getPlayerTextColorG(), GET_PLAYER(pMissionUnit->getOwnerINLINE()).getPlayerTextColorB(), GET_PLAYER(pMissionUnit->getOwnerINLINE()).getPlayerTextColorA(), szTempString.GetCString()));
					}

					// mission plot
					if (eMissionAI != NO_MISSIONAI || eMissionType != NO_MISSION)
					{
						// first try the plot from the missionAI
						CvPlot* pMissionPlot = pHeadGroup->AI_getMissionAIPlot();

						// if MissionAI does not have a plot, get one from the mission itself
						if (pMissionPlot == NULL && eMissionType != NO_MISSION)
						{
							switch (eMissionType)
							{
							case MISSION_MOVE_TO:
							case MISSION_ROUTE_TO:
								pMissionPlot =  GC.getMapINLINE().plotINLINE(pHeadGroup->getMissionData1(0), pHeadGroup->getMissionData2(0));
								break;

							case MISSION_MOVE_TO_UNIT:
								if (pMissionUnit != NULL)
								{
									pMissionPlot = pMissionUnit->plot();
								}
								break;
							}
						}

						if (pMissionPlot != NULL)
						{
							szString.append(CvWString::format(L"\n [%d,%d]", pMissionPlot->getX_INLINE(), pMissionPlot->getY_INLINE()));

							CvCity* pCity = pMissionPlot->getWorkingCity();
							if (pCity != NULL)
							{
								szString.append(L" (");

								if (!pMissionPlot->isCity())
								{
									DirectionTypes eDirection = estimateDirection(dxWrap(pMissionPlot->getX_INLINE() - pCity->plot()->getX_INLINE()), dyWrap(pMissionPlot->getY_INLINE() - pCity->plot()->getY_INLINE()));

									getDirectionTypeString(szTempString, eDirection);
									szString.append(CvWString::format(L"%s of ", szTempString.GetCString()));
								}

								szString.append(CvWString::format(SETCOLR L"%s" ENDCOLR L")", GET_PLAYER(pCity->getOwnerINLINE()).getPlayerTextColorR(), GET_PLAYER(pCity->getOwnerINLINE()).getPlayerTextColorG(), GET_PLAYER(pCity->getOwnerINLINE()).getPlayerTextColorB(), GET_PLAYER(pCity->getOwnerINLINE()).getPlayerTextColorA(), pCity->getName().GetCString()));
							}
							else
							{
								if (pMissionPlot != pPlot)
								{
									DirectionTypes eDirection = estimateDirection(dxWrap(pMissionPlot->getX_INLINE() - pPlot->getX_INLINE()), dyWrap(pMissionPlot->getY_INLINE() - pPlot->getY_INLINE()));

									getDirectionTypeString(szTempString, eDirection);
									szString.append(CvWString::format(L" (%s)", szTempString.GetCString()));
								}

								PlayerTypes eMissionPlotOwner = pMissionPlot->getOwnerINLINE();
								if (eMissionPlotOwner != NO_PLAYER)
								{
									szString.append(CvWString::format(L", " SETCOLR L"%s" ENDCOLR, GET_PLAYER(eMissionPlotOwner).getPlayerTextColorR(), GET_PLAYER(eMissionPlotOwner).getPlayerTextColorG(), GET_PLAYER(eMissionPlotOwner).getPlayerTextColorB(), GET_PLAYER(eMissionPlotOwner).getPlayerTextColorA(), GET_PLAYER(eMissionPlotOwner).getName()));
								}
							}
						}
					}

					// display cargo for head unit
					CLLNode<IDInfo>* pUnitNode2 = pPlot->headUnitNode();
					while(pUnitNode2 != NULL)
					{
						CvUnit* pCargoUnit = ::getUnit(pUnitNode2->m_data);
						pUnitNode2 = pPlot->nextUnitNode(pUnitNode2);

						// is this unit visible?
						if (pCargoUnit && (pCargoUnit != pHeadUnit) && !pCargoUnit->isInvisible(GC.getGameINLINE().getActiveTeam(), GC.getGameINLINE().isDebugMode()))
						{
							// is this unit in cargo of the headunit?
							if (pCargoUnit->getTransportUnit() == pHeadUnit)
							{
								// name and unitai
								szString.append(CvWString::format(SETCOLR L"\n %s" ENDCOLR, TEXT_COLOR("COLOR_ALT_HIGHLIGHT_TEXT"), pCargoUnit->getName().GetCString()));
								szString.append(CvWString::format(L"(%d)", shortenID(pCargoUnit->getID())));
								getUnitAIString(szTempString, pCargoUnit->AI_getUnitAIType());
								szString.append(CvWString::format(SETCOLR L" %s " ENDCOLR, GET_PLAYER(pCargoUnit->getOwnerINLINE()).getPlayerTextColorR(), GET_PLAYER(pCargoUnit->getOwnerINLINE()).getPlayerTextColorG(), GET_PLAYER(pCargoUnit->getOwnerINLINE()).getPlayerTextColorB(), GET_PLAYER(pCargoUnit->getOwnerINLINE()).getPlayerTextColorA(), szTempString.GetCString()));

								// promotion icons
								for (int iPromotionIndex = 0; iPromotionIndex < numPromotionInfos; iPromotionIndex++)
								{
									PromotionTypes ePromotion = (PromotionTypes)iPromotionIndex;
									if (!GC.getPromotionInfo(ePromotion).isGraphicalOnly() && pCargoUnit->isHasPromotion(ePromotion))
									{
										szString.append(CvWString::format(L"<img=%S size=16></img>", GC.getPromotionInfo(ePromotion).getButton()));
									}
								}
							}
						}
					}

					// display grouped units
					CLLNode<IDInfo>* pUnitNode3 = pPlot->headUnitNode();
					while(pUnitNode3 != NULL)
					{
						CvUnit* pUnit = ::getUnit(pUnitNode3->m_data);
						pUnitNode3 = pPlot->nextUnitNode(pUnitNode3);

						// is this unit not head, in head's group and visible?
						if (pUnit && (pUnit != pHeadUnit) && (pUnit->getGroupID() == pHeadUnit->getGroupID()) && !pUnit->isInvisible(GC.getGameINLINE().getActiveTeam(), GC.getGameINLINE().isDebugMode()))
						{
							FAssertMsg(!pUnit->isCargo(), "unit is cargo but head unit is not cargo");
							// name and unitai
							szString.append(CvWString::format(SETCOLR L"\n-%s" ENDCOLR, TEXT_COLOR("COLOR_UNIT_TEXT"), pUnit->getName().GetCString()));
							szString.append(CvWString::format(L" (%d)", shortenID(pUnit->getID())));
							getUnitAIString(szTempString, pUnit->AI_getUnitAIType());
							szString.append(CvWString::format(SETCOLR L" %s " ENDCOLR, GET_PLAYER(pUnit->getOwnerINLINE()).getPlayerTextColorR(), GET_PLAYER(pUnit->getOwnerINLINE()).getPlayerTextColorG(), GET_PLAYER(pUnit->getOwnerINLINE()).getPlayerTextColorB(), GET_PLAYER(pUnit->getOwnerINLINE()).getPlayerTextColorA(), szTempString.GetCString()));

							// promotion icons
							for (int iPromotionIndex = 0; iPromotionIndex < numPromotionInfos; iPromotionIndex++)
							{
								PromotionTypes ePromotion = (PromotionTypes)iPromotionIndex;
								if (!GC.getPromotionInfo(ePromotion).isGraphicalOnly() && pUnit->isHasPromotion(ePromotion))
								{
									szString.append(CvWString::format(L"<img=%S size=16></img>", GC.getPromotionInfo(ePromotion).getButton()));
								}
							}

							// display cargo for loop unit
							CLLNode<IDInfo>* pUnitNode4 = pPlot->headUnitNode();
							while(pUnitNode4 != NULL)
							{
								CvUnit* pCargoUnit = ::getUnit(pUnitNode4->m_data);
								pUnitNode4 = pPlot->nextUnitNode(pUnitNode4);

								// is this unit visible?
								if (pCargoUnit && (pCargoUnit != pUnit) && !pCargoUnit->isInvisible(GC.getGameINLINE().getActiveTeam(), GC.getGameINLINE().isDebugMode()))
								{
									// is this unit in cargo of unit?
									if (pCargoUnit->getTransportUnit() == pUnit)
									{
										// name and unitai
										szString.append(CvWString::format(SETCOLR L"\n %s" ENDCOLR, TEXT_COLOR("COLOR_ALT_HIGHLIGHT_TEXT"), pCargoUnit->getName().GetCString()));
										szString.append(CvWString::format(L"(%d)", shortenID(pCargoUnit->getID())));
										getUnitAIString(szTempString, pCargoUnit->AI_getUnitAIType());
										szString.append(CvWString::format(SETCOLR L" %s " ENDCOLR, GET_PLAYER(pCargoUnit->getOwnerINLINE()).getPlayerTextColorR(), GET_PLAYER(pCargoUnit->getOwnerINLINE()).getPlayerTextColorG(), GET_PLAYER(pCargoUnit->getOwnerINLINE()).getPlayerTextColorB(), GET_PLAYER(pCargoUnit->getOwnerINLINE()).getPlayerTextColorA(), szTempString.GetCString()));

										// promotion icons
										for (int iPromotionIndex = 0; iPromotionIndex < numPromotionInfos; iPromotionIndex++)
										{
											PromotionTypes ePromotion = (PromotionTypes)iPromotionIndex;
											if (!GC.getPromotionInfo(ePromotion).isGraphicalOnly() && pCargoUnit->isHasPromotion(ePromotion))
											{
												szString.append(CvWString::format(L"<img=%S size=16></img>", GC.getPromotionInfo(ePromotion).getButton()));
											}
										}
									}
								}
							}
						}
					}

					// R&R, ray, Natives raiding party - START
					if (!gDLL->altKey())
					{
						if (pPlot->getTeam() == NO_TEAM || GET_TEAM(pHeadGroup->getTeam()).isAtWar(pPlot->getTeam()))
						{
							szString.append(NEWLINE);
							CvWString szTempBuffer;

							CvCity* pTargetCity = pPlot->area()->getTargetCity(pHeadGroup->getOwner());
							if (pTargetCity != NULL)
							{
								szString.append(CvWString::format(L"\nTarget City: %s (%s)", pTargetCity->getName().c_str(), GET_PLAYER(pTargetCity->getOwner()).getName()));
							}
							else
							{
								szString.append(CvWString::format(L"\nTarget City: None"));
							}
						}
					}
					// R&R, ray, Natives raiding party - END

					// double space non-empty groups
					if (pHeadGroup->getNumUnits() > 1 || pHeadUnit->hasCargo())
					{
						szString.append(NEWLINE);
					}

					szString.append(NEWLINE);
				}
			}
		}

		return;
	}


	CvUnit* pLoopUnit;
	static const uint iMaxNumUnits = 10;
	static std::vector<CvUnit*> apUnits;
	static std::vector<int> aiUnitNumbers;
	static std::vector<int> aiUnitStrength;
	static std::vector<int> aiUnitMaxStrength;
	static std::vector<CvUnit *> plotUnits;

	GC.getGame().getPlotUnits(pPlot, plotUnits);

	int iNumVisibleUnits = 0;
	if (pPlot->isVisible(GC.getGameINLINE().getActiveTeam(), GC.getGameINLINE().isDebugMode()))
	{
		CLLNode<IDInfo>* pUnitNode5 = pPlot->headUnitNode();
		while(pUnitNode5 != NULL)
		{
			CvUnit* pUnit = ::getUnit(pUnitNode5->m_data);
			pUnitNode5 = pPlot->nextUnitNode(pUnitNode5);

			if (pUnit && !pUnit->isInvisible(GC.getGameINLINE().getActiveTeam(), GC.getGameINLINE().isDebugMode()))
			{
				++iNumVisibleUnits;
			}
		}
	}

	apUnits.erase(apUnits.begin(), apUnits.end());

	if (iNumVisibleUnits > iMaxNumUnits)
	{
		aiUnitNumbers.erase(aiUnitNumbers.begin(), aiUnitNumbers.end());
		aiUnitStrength.erase(aiUnitStrength.begin(), aiUnitStrength.end());
		aiUnitMaxStrength.erase(aiUnitMaxStrength.begin(), aiUnitMaxStrength.end());

		if (m_apbPromotion.size() == 0)
		{
			for (int iI = 0; iI < (GC.getNumUnitInfos() * MAX_PLAYERS); ++iI)
			{
				m_apbPromotion.push_back(new int[numPromotionInfos]);
			}
		}

		for (int iI = 0; iI < (GC.getNumUnitInfos() * MAX_PLAYERS); ++iI)
		{
			aiUnitNumbers.push_back(0);
			aiUnitStrength.push_back(0);
			aiUnitMaxStrength.push_back(0);
			for (int iJ = 0; iJ < numPromotionInfos; iJ++)
			{
				m_apbPromotion[iI][iJ] = 0;
			}
		}
	}

	int iCount = 0;
	for (int iI = iMaxNumUnits; iI < iNumVisibleUnits && iI < (int) plotUnits.size(); ++iI)
	{
		pLoopUnit = plotUnits[iI];

		if (pLoopUnit != NULL && pLoopUnit != pPlot->getCenterUnit())
		{
			apUnits.push_back(pLoopUnit);

			if (iNumVisibleUnits > iMaxNumUnits)
			{
				int iIndex = pLoopUnit->getUnitType() * MAX_PLAYERS + pLoopUnit->getOwner();
				if (aiUnitNumbers[iIndex] == 0)
				{
					++iCount;
				}
				++aiUnitNumbers[iIndex];

				int iBase = pLoopUnit->baseCombatStr();
				if (iBase > 0 && pLoopUnit->maxHitPoints() > 0)
				{
					aiUnitMaxStrength[iIndex] += 100 * iBase;
					aiUnitStrength[iIndex] += (100 * iBase * pLoopUnit->currHitPoints()) / pLoopUnit->maxHitPoints();
				}

				for (int iJ = 0; iJ < numPromotionInfos; iJ++)
				{
					if (!GC.getPromotionInfo((PromotionTypes)iJ).isGraphicalOnly() && pLoopUnit->isHasPromotion((PromotionTypes)iJ))
					{
						++m_apbPromotion[iIndex][iJ];
					}
				}
			}
		}
	}


	if (iNumVisibleUnits > 0)
	{
		if (pPlot->getCenterUnit())
		{
			setUnitHelp(szString, pPlot->getCenterUnit(), iNumVisibleUnits > iMaxNumUnits, true);
		}

		uint iNumShown = std::min<uint>(iMaxNumUnits, iNumVisibleUnits);
		for (uint iI = 0; iI < iNumShown && iI < (int) plotUnits.size(); ++iI)
		{
			CvUnit* pLoopUnit = plotUnits[iI];
			if (pLoopUnit != pPlot->getCenterUnit())
			{
				szString.append(NEWLINE);
				setUnitHelp(szString, pLoopUnit, true, true);
			}
		}

		bool bFirst = true;
		if (iNumVisibleUnits > iMaxNumUnits)
		{
			for (int iI = 0; iI < GC.getNumUnitInfos(); ++iI)
			{
				for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
				{
					int iIndex = iI * MAX_PLAYERS + iJ;

					if (aiUnitNumbers[iIndex] > 0)
					{
						if (iCount < 5 || bFirst)
						{
							szString.append(NEWLINE);
							bFirst = false;
						}
						else
						{
							szString.append(L", ");
						}
						szString.append(CvWString::format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR("COLOR_UNIT_TEXT"), GC.getUnitInfo((UnitTypes)iI).getDescription()));

						szString.append(CvWString::format(L" (%d)", aiUnitNumbers[iIndex]));

						if (aiUnitMaxStrength[iIndex] > 0)
						{
							int iBase = (aiUnitMaxStrength[iIndex] / aiUnitNumbers[iIndex]) / 100;
							int iCurrent = (aiUnitStrength[iIndex] / aiUnitNumbers[iIndex]) / 100;
							int iCurrent100 = (aiUnitStrength[iIndex] / aiUnitNumbers[iIndex]) % 100;
							if (0 == iCurrent100)
							{
								if (iBase == iCurrent)
								{
									szString.append(CvWString::format(L" %d", iBase));
								}
								else
								{
									szString.append(CvWString::format(L" %d/%d", iCurrent, iBase));
								}
							}
							else
							{
								szString.append(CvWString::format(L" %d.%02d/%d", iCurrent, iCurrent100, iBase));
							}
							szString.append(CvWString::format(L"%c", gDLL->getSymbolID(STRENGTH_CHAR)));
						}


						for (int iK = 0; iK < numPromotionInfos; iK++)
						{
							if (m_apbPromotion[iIndex][iK] > 0)
							{
								szString.append(CvWString::format(L"%d<img=%S size=16></img>", m_apbPromotion[iIndex][iK], GC.getPromotionInfo((PromotionTypes)iK).getButton()));
							}
						}

						if (iJ != GC.getGameINLINE().getActivePlayer() && !GC.getUnitInfo((UnitTypes)iI).isHiddenNationality())
						{
							szString.append(L", ");
							szString.append(CvWString::format(SETCOLR L"%s" ENDCOLR, GET_PLAYER((PlayerTypes)iJ).getPlayerTextColorR(), GET_PLAYER((PlayerTypes)iJ).getPlayerTextColorG(), GET_PLAYER((PlayerTypes)iJ).getPlayerTextColorB(), GET_PLAYER((PlayerTypes)iJ).getPlayerTextColorA(), GET_PLAYER((PlayerTypes)iJ).getName()));
						}
					}
				}
			}
		}
	}
}


// Returns true if help was given...
bool CvGameTextMgr::setCombatPlotHelp(CvWStringBuffer &szString, CvPlot* pPlot)
{
	PROFILE_FUNC();

	CvUnit* pAttacker;
	CvUnit* pDefender;
	CvWString szTempBuffer;
	CvWString szOffenseOdds;
	CvWString szDefenseOdds;
	bool bValid;
	int iModifier;

	if (gDLL->getInterfaceIFace()->getLengthSelectionList() == 0)
	{
		return false;
	}

	bValid = false;

	switch (gDLL->getInterfaceIFace()->getSelectionList()->getDomainType())
	{
	case DOMAIN_SEA:
		bValid = pPlot->isWater();
		break;

	case DOMAIN_LAND:
		bValid = gDLL->getInterfaceIFace()->getSelectionList()->canMoveInto(pPlot, true);
		//bValid = !(pPlot->isWater());
		break;

	case DOMAIN_IMMOBILE:
		break;

	default:
		FAssert(false);
		break;
	}

	if (!bValid)
	{
		return false;
	}

	int iOdds;
	pAttacker = gDLL->getInterfaceIFace()->getSelectionList()->AI_getBestGroupAttacker(pPlot, false, iOdds);

	if (pAttacker == NULL)
	{
		pAttacker = gDLL->getInterfaceIFace()->getSelectionList()->AI_getBestGroupAttacker(pPlot, false, iOdds, true);
	}

	if (pAttacker != NULL)
	{
		// Erik: In case there are units from multiple players on the target plot, we have to check if we're at war with the target player to avoid
		// odds from the wrong unit to be displayed
		// The old "behaviour" is still available by pressing the alt key
		pDefender = pPlot->getBestDefender(NO_PLAYER, pAttacker->getOwnerINLINE(), pAttacker, !GC.altKey(), (NO_TEAM == pAttacker->getDeclareWarUnitMove(pPlot)));

		ProfessionTypes eProfession = NO_PROFESSION;
		if(pDefender != NULL)
		{
			eProfession = pDefender->getProfession();
		}

		CvCity* pCity = pPlot->getPlotCity();
		if(pCity != NULL)
		{
			pDefender = pCity->getBestDefender(&eProfession, pDefender, pAttacker);
		}

		CvUnitTemporaryStrengthModifier kTemporaryStrength(pDefender, eProfession);
		if (pDefender != NULL && pDefender != pAttacker && pDefender->canDefend(pPlot))
		{
			// Erik: I've adapted and modified PieceOfMind's Advanced Combat Odds for use in RaR			
			CvWString szTempBuffer2;
		
			float AttackerKillOdds = getCombatOdds(pAttacker, pDefender) / 1000.0F;
			float PullOutOdds = 0;
			float RetreatOdds = getCombatOddsDraw(pAttacker, pDefender) / 1000.0F;
			// Erik: Ensure that the odds are positive
			float DefenderKillOdds = std::max(0.0F, 1.0F - AttackerKillOdds - RetreatOdds);

			// Erik: We have to add in the actual retreat chance as well
			float prob1 = 100.0f*(AttackerKillOdds + PullOutOdds);//up to win odds
			float prob2 = prob1 + 100.0f*RetreatOdds;//up to retreat odds

			szTempBuffer.Format(L"%.2f%%", 100.0f*(AttackerKillOdds + RetreatOdds + PullOutOdds));
			szTempBuffer2.Format(L"%.2f%%", 100.0f*(RetreatOdds + PullOutOdds + DefenderKillOdds));
			szString.append(gDLL->getText("TXT_ACO_SurvivalOdds"));

			// Assign the color based on the most likely outcome
			if (AttackerKillOdds > RetreatOdds && AttackerKillOdds > DefenderKillOdds)
			{
				szString.append(gDLL->getText("[COLOR_POSITIVE_TEXT]%s1[COLOR_REVERT]", szTempBuffer.GetCString()));
			}
			else if (RetreatOdds > AttackerKillOdds && RetreatOdds > DefenderKillOdds)
			{
				szString.append(gDLL->getText("[COLOR_UNIT_TEXT]%s1[COLOR_REVERT]", szTempBuffer.GetCString()));
			}
			else
			{
				szString.append(gDLL->getText("[COLOR_NEGATIVE_TEXT]%s1[COLOR_REVERT]", szTempBuffer.GetCString()));
			}
			
			szString.append(NEWLINE);

			float prob = 100.0f*(AttackerKillOdds + RetreatOdds + PullOutOdds);
			int pixels_left = 199;// 1 less than 200 to account for right end bar
			int pixels;
			int fullBlocks;
			int lastBlock;

			pixels = (2 * ((int)(prob1 + 0.5))) - 1;  // 1% per pixel // subtracting one to account for left end bar
			fullBlocks = pixels / 10;
			lastBlock = pixels % 10;

			szString.append(L"<img=Art/ACO/green_bar_left_end.dds>");
			for (int i = 0; i < fullBlocks; ++i)
			{
				szString.append(L"<img=Art/ACO/green_bar_10.dds>");
				pixels_left -= 10;
			}
			if (lastBlock > 0)
			{
				szTempBuffer2.Format(L"<img=Art/ACO/green_bar_%d.dds>", lastBlock);
				szString.append(szTempBuffer2);
				pixels_left -= lastBlock;
			}

			pixels = 2 * ((int)(prob2 + 0.5)) - (pixels + 1);//the number up to the next one...
			fullBlocks = pixels / 10;
			lastBlock = pixels % 10;
			for (int i = 0; i < fullBlocks; ++i)
			{
				szString.append(L"<img=Art/ACO/yellow_bar_10.dds>");
				pixels_left -= 10;
			}
			if (lastBlock > 0)
			{
				szTempBuffer2.Format(L"<img=Art/ACO/yellow_bar_%d.dds>", lastBlock);
				szString.append(szTempBuffer2);
				pixels_left -= lastBlock;
			}

			fullBlocks = pixels_left / 10;
			lastBlock = pixels_left % 10;
			for (int i = 0; i < fullBlocks; ++i)
			{
				szString.append(L"<img=Art/ACO/red_bar_10.dds>");
			}
			if (lastBlock > 0)
			{
				szTempBuffer2.Format(L"<img=Art/ACO/red_bar_%d.dds>", lastBlock);
				szString.append(szTempBuffer2);
			}

			szString.append(L"<img=Art/ACO/red_bar_right_end.dds> ");
			szString.append(NEWLINE);

			// Individual outcomes

			szTempBuffer.Format(L": " SETCOLR L"%.2f%% " L"" ENDCOLR,
				TEXT_COLOR("COLOR_POSITIVE_TEXT"), 100.0f*AttackerKillOdds);
			szString.append(gDLL->getText("TXT_ACO_Victory"));
			szString.append(szTempBuffer.GetCString());
			szString.append(NEWLINE);

			szTempBuffer.Format(L": " SETCOLR L"%.2f%% " ENDCOLR SETCOLR L"" ENDCOLR,
				TEXT_COLOR("COLOR_UNIT_TEXT"), 100.0f*RetreatOdds, TEXT_COLOR("COLOR_POSITIVE_TEXT"));
			szString.append(gDLL->getText("TXT_ACO_Retreat"));
			szString.append(szTempBuffer.GetCString());
			szString.append(NEWLINE);

			szTempBuffer.Format(L": " SETCOLR L"%.2f%% " L"" ENDCOLR,
				TEXT_COLOR("COLOR_NEGATIVE_TEXT"), 100.0f*DefenderKillOdds);
			szString.append(gDLL->getText("TXT_ACO_Defeat"));
			szString.append(szTempBuffer.GetCString());
			szString.append(NEWLINE);

			// ACO end

			// Erik: Old combat odds below
			/*
			int iCombatOdds = getCombatOdds(pAttacker, pDefender);

			if (iCombatOdds > 999)
			{
				szTempBuffer = L"&gt; 99.9";
			}
			else if (iCombatOdds < 1)
			{
				szTempBuffer = L"&lt; 0.1";
			}
			else
			{
				szTempBuffer.Format(L"%.1f", ((float)iCombatOdds) / 10.0f);
			}
			szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_ODDS", szTempBuffer.GetCString()));

			int iWithdrawal = 0;

			iWithdrawal += std::min(100, pAttacker->withdrawalProbability()) * (1000 - iCombatOdds);

			if (iWithdrawal > 0)
			{
				if (iWithdrawal > 99900)
				{
					szTempBuffer = L"&gt; 99.9";
				}
				else if (iWithdrawal < 100)
				{
					szTempBuffer = L"&lt; 0.1";
				}
				else
				{
					szTempBuffer.Format(L"%.1f", iWithdrawal / 1000.0f);
				}

				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_ODDS_RETREAT", szTempBuffer.GetCString()));
			}
			*/

			szOffenseOdds.Format(L"%.2f", pAttacker->currCombatStrFloat(NULL, NULL));
			szDefenseOdds.Format(L"%.2f", pDefender->currCombatStrFloat(pPlot, pAttacker));
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_ODDS_VS", szOffenseOdds.GetCString(), szDefenseOdds.GetCString()));

			szString.append(L' ');//XXX

			szString.append(gDLL->getText("TXT_KEY_COLOR_POSITIVE"));

			szString.append(L' ');//XXX

			iModifier = pAttacker->getExtraCombatPercent();

			if (iModifier != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_EXTRA_STRENGTH", iModifier));
			}

			iModifier = pAttacker->unitClassAttackModifier(pDefender->getUnitClassType());

			if (iModifier != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_MOD_VS_TYPE", iModifier, GC.getUnitClassInfo(pDefender->getUnitClassType()).getTextKeyWide()));
			}

			if (pDefender->getUnitCombatType() != NO_UNITCOMBAT)
			{
				iModifier = pAttacker->unitCombatModifier(pDefender->getUnitCombatType());

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_MOD_VS_TYPE", iModifier, GC.getUnitCombatInfo(pDefender->getUnitCombatType()).getTextKeyWide()));
				}
			}

			iModifier = pAttacker->domainModifier(pDefender->getDomainType());

			if (iModifier != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_MOD_VS_TYPE", iModifier, GC.getDomainInfo(pDefender->getDomainType()).getTextKeyWide()));
			}

			if (pPlot->isCity(true, pDefender->getTeam()))
			{
				iModifier = pAttacker->cityAttackModifier();

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_CITY_MOD", iModifier));
				}
			}

			if (pPlot->isHills() || pPlot->isPeak())
			{
				iModifier = pAttacker->hillsAttackModifier();

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_HILLS_MOD", iModifier));
				}
			}

			if (pPlot->getOwner() == pAttacker->getOwner())
			{
				iModifier = pAttacker->DomesticBonusModifier();

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_DOMESTIC_BONUS", iModifier));
				}
			}	

			if (pPlot->getFeatureType() != NO_FEATURE)
			{
				iModifier = pAttacker->featureAttackModifier(pPlot->getFeatureType());

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_UNIT_MOD", iModifier, GC.getFeatureInfo(pPlot->getFeatureType()).getTextKeyWide()));
				}
			}
			else
			{
				iModifier = pAttacker->terrainAttackModifier(pPlot->getTerrainType());

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_UNIT_MOD", iModifier, GC.getTerrainInfo(pPlot->getTerrainType()).getTextKeyWide()));
				}
			}

			iModifier = pAttacker->rebelModifier(pDefender->getOwnerINLINE());
			if (iModifier != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_REBEL_MOD", iModifier));
			}

			if (pDefender->isNative())
			{
				iModifier = GET_PLAYER(pAttacker->getOwnerINLINE()).getNativeCombatModifier();

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_NATIVE_COMBAT_MOD", iModifier));
				}
			}

			if (pAttacker->isHurt())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_HP", pAttacker->currHitPoints(), pAttacker->maxHitPoints()));
			}

			szString.append(gDLL->getText("TXT_KEY_COLOR_REVERT"));

			szString.append(L' ');//XXX

			szString.append(gDLL->getText("TXT_KEY_COLOR_NEGATIVE"));

			szString.append(L' ');//XXX

			if (!(pAttacker->isRiver()))
			{
				if (pAttacker->plot()->isRiverCrossing(directionXY(pAttacker->plot(), pPlot)))
				{
					iModifier = GC.getRIVER_ATTACK_MODIFIER();

					if (iModifier != 0)
					{
						szString.append(NEWLINE);
						szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_RIVER_MOD", -(iModifier)));
					}
				}
			}

			if (!(pAttacker->isAmphib()))
			{
				if (!(pPlot->isWater()) && pAttacker->plot()->isWater())
				{
					iModifier = GC.getAMPHIB_ATTACK_MODIFIER();

					if (iModifier != 0)
					{
						szString.append(NEWLINE);
						szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_AMPHIB_MOD", -(iModifier)));
					}
				}
			}

			iModifier = pDefender->getExtraCombatPercent();

			if (iModifier != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_EXTRA_STRENGTH", iModifier));
			}

			iModifier = pDefender->unitClassDefenseModifier(pAttacker->getUnitClassType());

			if (iModifier != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_MOD_VS_TYPE", iModifier, GC.getUnitClassInfo(pAttacker->getUnitClassType()).getTextKeyWide()));
			}

			if (pAttacker->getUnitCombatType() != NO_UNITCOMBAT)
			{
				iModifier = pDefender->unitCombatModifier(pAttacker->getUnitCombatType());

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_MOD_VS_TYPE", iModifier, GC.getUnitCombatInfo(pAttacker->getUnitCombatType()).getTextKeyWide()));
				}
			}

			iModifier = pDefender->domainModifier(pAttacker->getDomainType());

			if (iModifier != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_MOD_VS_TYPE", iModifier, GC.getDomainInfo(pAttacker->getDomainType()).getTextKeyWide()));
			}

			if (!(pDefender->noDefensiveBonus()))
			{
				iModifier = pPlot->defenseModifier(pDefender->getTeam());

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_TILE_MOD", iModifier));
				}
			}

			iModifier = pDefender->fortifyModifier();

			if (iModifier != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_FORTIFY_MOD", iModifier));
			}

			if (pPlot->isCity(true, pDefender->getTeam()))
			{
				iModifier = pDefender->cityDefenseModifier();

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_CITY_MOD", iModifier));
				}
			}

			if (pPlot->isHills() || pPlot->isPeak())
			{
				iModifier = pDefender->hillsDefenseModifier();

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_HILLS_MOD", iModifier));
				}
			}

			if (pPlot->getOwner() == pDefender->getOwner())
			{
				iModifier = pDefender->DomesticBonusModifier();

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_DOMESTIC_BONUS", iModifier));
				}
			}		

			if (pPlot->getFeatureType() != NO_FEATURE)
			{
				iModifier = pDefender->featureDefenseModifier(pPlot->getFeatureType());

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_UNIT_MOD", iModifier, GC.getFeatureInfo(pPlot->getFeatureType()).getTextKeyWide()));
				}
			}
			else
			{
				iModifier = pDefender->terrainDefenseModifier(pPlot->getTerrainType());

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_UNIT_MOD", iModifier, GC.getTerrainInfo(pPlot->getTerrainType()).getTextKeyWide()));
				}
			}

			iModifier = pDefender->rebelModifier(pAttacker->getOwnerINLINE());
			if (iModifier != 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_REBEL_MOD", iModifier));
			}

			if (pAttacker->isNative())
			{
				iModifier = GET_PLAYER(pDefender->getOwnerINLINE()).getNativeCombatModifier();

				if (iModifier != 0)
				{
					szString.append(NEWLINE);
					szString.append(gDLL->getText("TXT_KEY_UNIT_NATIVE_COMBAT_MOD", iModifier));
				}
			}

			if (pDefender->isHurt())
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_COMBAT_PLOT_HP", pDefender->currHitPoints(), pDefender->maxHitPoints()));
			}

			if ((gDLL->getChtLvl() > 0))
			{
				szTempBuffer.Format(L"\nStack Compare Value = %d",
					gDLL->getInterfaceIFace()->getSelectionList()->AI_compareStacks(pPlot, false));
				szString.append(szTempBuffer);

				int iOurStrengthDefense = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).AI_getOurPlotStrength(pPlot, 1, true, false);
				int iOurStrengthOffense = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).AI_getOurPlotStrength(pPlot, 1, false, false);
				szTempBuffer.Format(L"\nPlot Strength(Ours)= d%d, o%d", iOurStrengthDefense, iOurStrengthOffense);
				szString.append(szTempBuffer);
				int iEnemyStrengthDefense = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).AI_getEnemyPlotStrength(pPlot, 1, true, false);
				int iEnemyStrengthOffense = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).AI_getEnemyPlotStrength(pPlot, 1, false, false);
				szTempBuffer.Format(L"\nPlot Strength(Enemy)= d%d, o%d", iEnemyStrengthDefense, iEnemyStrengthOffense);
				szString.append(szTempBuffer);
			}

			szString.append(gDLL->getText("TXT_KEY_COLOR_REVERT"));

			return true;
		}
	}

	return false;
}

// DO NOT REMOVE - needed for font testing - Moose
void createTestFontString(CvWStringBuffer& szString)
{
	int iI;
	szString.assign(L"!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[?]^_`abcdefghijklmnopqrstuvwxyz\n");
	szString.append(L"{}~\\????G????T??????????S??F?O????????a??de??????µ???p??st?f???????????«»°???????©®?£??????");
	szString.append(L"\n");
	for (iI=0;iI<NUM_YIELD_TYPES;++iI)
		szString.append(CvWString::format(L"%c", GC.getYieldInfo((YieldTypes) iI).getChar()));
	szString.append(L"\n");
	for (iI=0;iI<GC.getNumSpecialBuildingInfos();++iI)
		szString.append(CvWString::format(L"%c", GC.getSpecialBuildingInfo((SpecialBuildingTypes) iI).getChar()));
	szString.append(L"\n");
	for (iI=0;iI<GC.getNumFatherPointInfos();++iI)
		szString.append(CvWString::format(L"%c", GC.getFatherPointInfo((FatherPointTypes) iI).getChar()));
	szString.append(L"\n");
	for (iI=0;iI<GC.getNumCivilizationInfos();++iI)
		szString.append(CvWString::format(L"%c", GC.getCivilizationInfo((CivilizationTypes) iI).getMissionaryChar()));
	szString.append(L"\n");
	for (iI = 0; iI < GC.getNumBonusInfos(); ++iI)
		szString.append(CvWString::format(L"%c", GC.getBonusInfo((BonusTypes) iI).getChar()));
	szString.append(L"\n");
	for (iI=0; iI<MAX_NUM_SYMBOLS; ++iI)
		szString.append(CvWString::format(L"%c", gDLL->getSymbolID(iI)));
}

void CvGameTextMgr::setPlotHelp(CvWStringBuffer& szString, CvPlot* pPlot)
{
	PROFILE_FUNC();

	int iI;

	// DO NOT REMOVE - needed for font testing - Moose
	if (gDLL->getTestingFont())
	{
		createTestFontString(szString);
		return;
	}

	CvWString szTempBuffer;
	ImprovementTypes eImprovement;
	PlayerTypes eRevealOwner;
	BonusTypes eBonus;
	bool bShift;
	bool bAlt;
	bool bCtrl;
	int iDefenseModifier;
	int iYield;
	int iTurns;

	bShift = gDLL->shiftKey();
	bAlt = gDLL->altKey();
	bCtrl = gDLL->ctrlKey();

	if ((gDLL->getChtLvl() > 0) && (bCtrl || bAlt || bShift))
	{
		szTempBuffer.Format(L"\n(%d, %d) (Oc: %d  / Cr: %d)", pPlot->getX_INLINE(), pPlot->getY_INLINE(), pPlot->getDistanceToOcean(), pPlot->getCrumbs());
		szString.append(szTempBuffer);
	}

	if (bShift && !bAlt && (gDLL->getChtLvl() > 0))
	{
		szString.append(L"\n");
		szString.append(GC.getTerrainInfo(pPlot->getTerrainType()).getDescription());

		szTempBuffer.Format(L"\nArea: %d", pPlot->getArea());
		szString.append(szTempBuffer);

		char tempChar = 'x';
		if(pPlot->getRiverNSDirection() == CARDINALDIRECTION_NORTH)
		{
			tempChar = 'N';
		}
		else if(pPlot->getRiverNSDirection() == CARDINALDIRECTION_SOUTH)
		{
			tempChar = 'S';
		}
		szTempBuffer.Format(L"\nNSRiverFlow: %c", tempChar);
		szString.append(szTempBuffer);

		tempChar = 'x';
		if(pPlot->getRiverWEDirection() == CARDINALDIRECTION_WEST)
		{
			tempChar = 'W';
		}
		else if(pPlot->getRiverWEDirection() == CARDINALDIRECTION_EAST)
		{
			tempChar = 'E';
		}
		szTempBuffer.Format(L"\nWERiverFlow: %c", tempChar);
		szString.append(szTempBuffer);

		//Coast
		tempChar = 'x';
		EuropeTypes eNearestEurope = pPlot->getNearestEurope();
		if (eNearestEurope != NO_EUROPE)
		{
			szTempBuffer.Format(L"\nOcean: %s", GC.getEuropeInfo(eNearestEurope).getDescription());
			szString.append(szTempBuffer);
		}

		if(pPlot->getRouteType() != NO_ROUTE)
		{
			szTempBuffer.Format(L"\nRoute: %s", GC.getRouteInfo(pPlot->getRouteType()).getDescription());
			szString.append(szTempBuffer);
		}

		if(pPlot->getRouteSymbol() != NULL)
		{
			szTempBuffer.Format(L"\nConnection: %i", gDLL->getRouteIFace()->getConnectionMask(pPlot->getRouteSymbol()));
			szString.append(szTempBuffer);
		}

		for (iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (pPlot->getCulture((PlayerTypes)iI) > 0)
				{
					szTempBuffer.Format(L"\n%s Culture: %d", GET_PLAYER((PlayerTypes)iI).getName(), pPlot->getCulture((PlayerTypes)iI));
					szString.append(szTempBuffer);
				}
				// TAC - AI Improved Navel AI - koma13 - START
				if (pPlot->getDangerMap((PlayerTypes)iI) > 0)
				{
					szTempBuffer.Format(L"\n%s Danger Map: %d", GET_PLAYER((PlayerTypes)iI).getName(), pPlot->getDangerMap((PlayerTypes)iI));
					szString.append(szTempBuffer);
				}
				// TAC - AI Improved Navel AI - koma13 - END
			}
		}

		PlayerTypes eActivePlayer = GC.getGameINLINE().getActivePlayer();
		int iActualFoundValue = pPlot->getFoundValue(eActivePlayer);
		int iCalcFoundValue = GET_PLAYER(eActivePlayer).AI_foundValue(pPlot->getX_INLINE(), pPlot->getY_INLINE(), -1, false);
		int iStartingFoundValue = GET_PLAYER(eActivePlayer).AI_foundValue(pPlot->getX_INLINE(), pPlot->getY_INLINE(), -1, true);

		szTempBuffer.Format(L"\nFound Value: %d, (%d, %d)", iActualFoundValue, iCalcFoundValue, iStartingFoundValue);
		szString.append(szTempBuffer);

		CvCity* pWorkingCity = pPlot->getWorkingCity();
		if (NULL != pWorkingCity)
		{
		    int iPlotIndex = pWorkingCity->getCityPlotIndex(pPlot);
            int iBuildValue = pWorkingCity->AI_getBestBuildValue(iPlotIndex);
            BuildTypes eBestBuild = pWorkingCity->AI_getBestBuild(iPlotIndex);
			int iCurrentValue = 0;
			BuildTypes eCurrentBuild = NO_BUILD;
            static_cast<CvCityAI*>(pWorkingCity)->AI_bestPlotBuild(pPlot, &iCurrentValue, &eCurrentBuild);

            if (NO_BUILD != eBestBuild)
            {
                szTempBuffer.Format(L"\nBest Build: %s (%d)", GC.getBuildInfo(eBestBuild).getDescription(), iBuildValue);
                szString.append(szTempBuffer);
			}

			if (NO_BUILD != eCurrentBuild)
			{
				szTempBuffer.Format(L"\nCurr Build: %s (%d)", GC.getBuildInfo(eCurrentBuild).getDescription(), iCurrentValue);
                szString.append(szTempBuffer);
            }
		}

		{
			szTempBuffer.Format(L"\nStack Str: land=%d(%d), sea=%d(%d)",
				pPlot->AI_sumStrength(NO_PLAYER, NO_PLAYER, DOMAIN_LAND, false, false, false),
				pPlot->AI_sumStrength(NO_PLAYER, NO_PLAYER, DOMAIN_LAND, true, false, false),
				pPlot->AI_sumStrength(NO_PLAYER, NO_PLAYER, DOMAIN_SEA, false, false, false),
				pPlot->AI_sumStrength(NO_PLAYER, NO_PLAYER, DOMAIN_SEA, true, false, false));
			szString.append(szTempBuffer);
		}
	}
	else if (!bShift && bAlt && (gDLL->getChtLvl() > 0))
	{
		CvUnit* pSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();

		if (pSelectedUnit != NULL)
		{
			int iPathTurns;
			if (pSelectedUnit->generatePath(pPlot, 0, false, &iPathTurns))
			{
				int iPathCost = pSelectedUnit->getPathCost();
				szString.append(CvWString::format(L"\nPathturns = %d, cost = %d", iPathTurns, iPathCost));
			}
		}

		//Distances to various things.
		if (pPlot->isOwned())
		{
			CvPlayerAI& kPlayer = GET_PLAYER(pPlot->getOwnerINLINE());
			CvTeamAI& kTeam = GET_TEAM(kPlayer.getTeam());

			szString.append(CvWString::format(L"\n FriendDist = %d, ECityDist = %d, EUnitDist = %d", kPlayer.AI_cityDistance(pPlot), kTeam.AI_enemyCityDistance(pPlot), kTeam.AI_enemyUnitDistance(pPlot)));
		}


		//Found Values
		szString.append(CvWString::format(L"\nFound Values"));
		for (int i = 0; i < MAX_PLAYERS; ++i)
		{
			CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)i);
			if (kLoopPlayer.isAlive() && !kLoopPlayer.isNative())
			{
				int iFoundValue = kLoopPlayer.AI_foundValue(pPlot->getX_INLINE(), pPlot->getY_INLINE());
				if (iFoundValue > 0)
				{
					szTempBuffer.Format(SETCOLR L"%s " ENDCOLR L"%d\n", kLoopPlayer.getPlayerTextColorR(), kLoopPlayer.getPlayerTextColorG(), kLoopPlayer.getPlayerTextColorB(), kLoopPlayer.getPlayerTextColorA(), kLoopPlayer.getCivilizationAdjective(), iFoundValue);
					szString.append(szTempBuffer);
				}
			}
		}

		// R&R, ray, Natives raiding party - START
		if (pPlot->isOwned())
		{
			CvCity* pTargetCity = pPlot->area()->getTargetCity(pPlot->getOwner());
			if (pTargetCity != NULL)
			{
				szString.append(CvWString::format(L"\nTarget City: %s", pTargetCity->getName().c_str()));
			}
			else
			{
				szString.append(CvWString::format(L"\nTarget City: None"));
			}
		}
		// R&R, ray, Natives raiding party - END

	}
	else if (bShift && bAlt && (gDLL->getChtLvl() > 0))
	{
		CvCity*	pCity = pPlot->getWorkingCity();
		if (pCity != NULL)
		{
			// some functions we want to call are not in CvCity, worse some are protected, so made us a friend
			CvCityAI* pCityAI = static_cast<CvCityAI*>(pCity);

			bool bAvoidGrowth = pCity->AI_avoidGrowth();
			bool bIgnoreGrowth = pCityAI->AI_ignoreGrowth();
			// Custom_House_Mod Start
			bool bIsBestPortCity = pCityAI->isBestPortCity();
			// Custom_House_Mod End

			// if we over the city, then do an array of all the plots
			if (pPlot->getPlotCity() != NULL)
			{

				// check avoid growth
				if (bAvoidGrowth || bIgnoreGrowth)
				{
					szString.append(L"\n");

					// red color
					szString.append(CvWString::format(SETCOLR, TEXT_COLOR("COLOR_NEGATIVE_TEXT")));

					if (bAvoidGrowth)
					{
						szString.append(CvWString::format(L"AvoidGrowth"));

						if (bIgnoreGrowth)
							szString.append(CvWString::format(L", "));
					}

					if (bIgnoreGrowth)
						szString.append(CvWString::format(L"IgnoreGrowth"));

					// end color
					szString.append(CvWString::format( ENDCOLR ));
				}
				// Custom_House_Mod Start
				if (bIsBestPortCity)
				{
					szString.append(L"\n");
					szString.append(CvWString::format(SETCOLR, TEXT_COLOR("COLOR_ALT_HIGHLIGHT_TEXT")));
					szString.append(CvWString::format(L"IsBestPortCity"));
					szString.append(CvWString::format( ENDCOLR ));
					//szTempBuffer.Format(SETCOLR L"\n%s is best port city" ENDCOLR, TEXT_COLOR("COLOR_ALT_HIGHLIGHT_TEXT"), pCity->getName().GetCString());
				}
				// Custom_House_Mod End

				// if control key is down, ignore food
				bool bIgnoreFood = gDLL->ctrlKey();

				// line one is: blank, 20, 9, 10, blank
				szString.append(L"\n");
				setCityPlotYieldValueString(szString, pCity, -1, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 20, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 9, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 10, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);

				// line two is: 19, 8, 1, 2, 11
				szString.append(L"\n");
				setCityPlotYieldValueString(szString, pCity, 19, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 8, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 1, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 2, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 11, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);

				// line three is: 18, 7, 0, 3, 12
				szString.append(L"\n");
				setCityPlotYieldValueString(szString, pCity, 18, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 7, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 0, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 3, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 12, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);

				// line four is: 17, 6, 5, 4, 13
				szString.append(L"\n");
				setCityPlotYieldValueString(szString, pCity, 17, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 6, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 5, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 4, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 13, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);

				// line five is: blank, 16, 15, 14, blank
				szString.append(L"\n");
				setCityPlotYieldValueString(szString, pCity, -1, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 16, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 15, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
				setCityPlotYieldValueString(szString, pCity, 14, bAvoidGrowth, bIgnoreGrowth, bIgnoreFood);
			}
			else
			{
				bool bWorkingPlot = pCity->isUnitWorkingPlot(pPlot);

				if (bWorkingPlot)
					szTempBuffer.Format( SETCOLR L"\n%s is working" ENDCOLR, TEXT_COLOR("COLOR_ALT_HIGHLIGHT_TEXT"), pCity->getName().GetCString());
				else
					szTempBuffer.Format( SETCOLR L"\n%s not working" ENDCOLR, TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), pCity->getName().GetCString());
				szString.append(szTempBuffer);

				int iValue = pCityAI->AI_plotValue(pPlot, bAvoidGrowth, /*bRemove*/ bWorkingPlot, /*bIgnoreFood*/ false, bIgnoreGrowth);
				int iJuggleValue = pCityAI->AI_plotValue(pPlot, bAvoidGrowth, /*bRemove*/ bWorkingPlot, false, bIgnoreGrowth, true);
				int iMagicValue = pCityAI->AI_getPlotMagicValue(pPlot);

				szTempBuffer.Format(L"\nvalue = %d\njuggle value = %d\nmagic value = %d", iValue, iJuggleValue, iMagicValue);
				szString.append(szTempBuffer);
			}
		}
	}
	else
	{
		eRevealOwner = pPlot->getRevealedOwner(GC.getGameINLINE().getActiveTeam(), true);

		if (eRevealOwner != NO_PLAYER)
		{
			if (pPlot->isActiveVisible(true))
			{
				szTempBuffer.Format(L"%d%% " SETCOLR L"%s" ENDCOLR, pPlot->calculateCulturePercent(eRevealOwner), GET_PLAYER(eRevealOwner).getPlayerTextColorR(), GET_PLAYER(eRevealOwner).getPlayerTextColorG(), GET_PLAYER(eRevealOwner).getPlayerTextColorB(), GET_PLAYER(eRevealOwner).getPlayerTextColorA(), GET_PLAYER(eRevealOwner).getCivilizationAdjective());
				szString.append(szTempBuffer);
				szString.append(NEWLINE);

				for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
				{
					if (iPlayer != eRevealOwner)
					{
						CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
						if (kPlayer.isAlive() && pPlot->getCulture((PlayerTypes)iPlayer) > 0)
						{
							szTempBuffer.Format(L"%d%% " SETCOLR L"%s" ENDCOLR, pPlot->calculateCulturePercent((PlayerTypes)iPlayer), kPlayer.getPlayerTextColorR(), kPlayer.getPlayerTextColorG(), kPlayer.getPlayerTextColorB(), kPlayer.getPlayerTextColorA(), kPlayer.getCivilizationAdjective());
							szString.append(szTempBuffer);
							szString.append(NEWLINE);
						}
					}
				}

			}
			else
			{
				szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, GET_PLAYER(eRevealOwner).getPlayerTextColorR(), GET_PLAYER(eRevealOwner).getPlayerTextColorG(), GET_PLAYER(eRevealOwner).getPlayerTextColorB(), GET_PLAYER(eRevealOwner).getPlayerTextColorA(), GET_PLAYER(eRevealOwner).getCivilizationDescription());
				szString.append(szTempBuffer);
				szString.append(NEWLINE);
			}
		}

		iDefenseModifier = pPlot->defenseModifier((eRevealOwner != NO_PLAYER ? GET_PLAYER(eRevealOwner).getTeam() : NO_TEAM), true);

		if (iDefenseModifier != 0)
		{
			szString.append(gDLL->getText("TXT_KEY_PLOT_BONUS", iDefenseModifier));
			szString.append(NEWLINE);
		}

		if (pPlot->getTerrainType() != NO_TERRAIN)
		{
			if (pPlot->isPeak())
			{
				szString.append(gDLL->getText("TXT_KEY_PLOT_PEAK"));
			}
			else
			{
				if (pPlot->isWater())
				{
					szTempBuffer.Format(SETCOLR, TEXT_COLOR("COLOR_WATER_TEXT"));
					szString.append(szTempBuffer);
				}

				if (pPlot->isHills())
				{
					szString.append(gDLL->getText("TXT_KEY_PLOT_HILLS"));
				}

				if (pPlot->getFeatureType() != NO_FEATURE)
				{
					szTempBuffer.Format(L"%s/", GC.getFeatureInfo(pPlot->getFeatureType()).getDescription());
					szString.append(szTempBuffer);
				}

				szString.append(GC.getTerrainInfo(pPlot->getTerrainType()).getDescription());

				if (pPlot->isWater())
				{
					szString.append(ENDCOLR);
				}
			}
		}

		if (pPlot->hasYield())
		{
			for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
			{
				iYield = pPlot->calculatePotentialYield(((YieldTypes)iI), NULL, true);

				if (iYield != 0)
				{
					szTempBuffer.Format(L", %d%c", iYield, GC.getYieldInfo((YieldTypes) iI).getChar());
					szString.append(szTempBuffer);
				}
			}
		}

		//WTP, ray, Lakes - START
		//not needed anymore, because we have Terrain Lake
		/*
		if (pPlot->isLake())
		{
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_PLOT_FRESH_WATER_LAKE"));
		}
		*/
		//WTP, ray, Lakes - END

		if (pPlot->isImpassable())
		{
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_PLOT_IMPASSABLE"));
		}

		if (pPlot->getEurope() != NO_EUROPE)
		{
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_PLOT_EUROPE"));
		}

		eBonus = pPlot->getBonusType();
		if (eBonus != NO_BONUS)
		{
			szTempBuffer.Format(L"%c " SETCOLR L"%s" ENDCOLR, GC.getBonusInfo(eBonus).getChar(), TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), GC.getBonusInfo(eBonus).getDescription());
			szString.append(NEWLINE);
			szString.append(szTempBuffer);
		}

		eImprovement = pPlot->getRevealedImprovementType(GC.getGameINLINE().getActiveTeam(), true);
		if (eImprovement != NO_IMPROVEMENT)
		{
			szString.append(NEWLINE);
			szString.append(GC.getImprovementInfo(eImprovement).getDescription());

			if (GC.getImprovementInfo(eImprovement).getImprovementUpgrade() != NO_IMPROVEMENT)
			{
				// Super Forts begin *text* *upgrade*
				//if ((pPlot->getUpgradeProgress() > 0) || (pPlot->isBeingWorked() && !GC.getImprovementInfo(eImprovement).isUpgradeRequiresFortify())) vetiarvind - todo revert
				if ((pPlot->getUpgradeProgress() > 0) || pPlot->isBeingWorked()) //- Original Code
				// Super Forts end
				{
					iTurns = pPlot->getUpgradeTimeLeft(eImprovement, eRevealOwner);

					szString.append(gDLL->getText("TXT_KEY_PLOT_IMP_UPGRADE", iTurns, GC.getImprovementInfo((ImprovementTypes) GC.getImprovementInfo(eImprovement).getImprovementUpgrade()).getTextKeyWide()));
				}
				else
				{
					// Super Forts begin *text* *upgrade*
					/* todo revert
					if (GC.getImprovementInfo(eImprovement).isUpgradeRequiresFortify())
					{
						szString.append(gDLL->getText("TXT_KEY_PLOT_FORTIFY_TO_UPGRADE", GC.getImprovementInfo((ImprovementTypes) GC.getImprovementInfo(eImprovement).getImprovementUpgrade()).getTextKeyWide()));
					}
					else
					{
						szString.append(gDLL->getText("TXT_KEY_PLOT_WORK_TO_UPGRADE", GC.getImprovementInfo((ImprovementTypes) GC.getImprovementInfo(eImprovement).getImprovementUpgrade()).getTextKeyWide()));
					}
					*/
					// Super Forts end					
				}
			}
		}

		if (pPlot->getRevealedRouteType(GC.getGameINLINE().getActiveTeam(), true) != NO_ROUTE)
		{
			szString.append(NEWLINE);
			szString.append(GC.getRouteInfo(pPlot->getRevealedRouteType(GC.getGameINLINE().getActiveTeam(), true)).getDescription());
		}
	}
	// R&R, Robert Surcouf, Damage on Storm plots, Start
	if (pPlot->getFeatureType() != NO_FEATURE)
	{
		if (GC.getFeatureInfo(pPlot->getFeatureType()).getMovementCost() != 1)
		{	
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_FEATURE_MOVEMENT_COST", GC.getFeatureInfo(pPlot->getFeatureType()).getMovementCost()));
		}
	
		int iDamage = GC.getFeatureInfo(pPlot->getFeatureType()).getTurnDamage();

		if (iDamage > 0)
		{
			szString.append(CvWString::format(SETCOLR, TEXT_COLOR("COLOR_NEGATIVE_TEXT")));
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_PLOT_DAMAGE", iDamage));
			szString.append(CvWString::format( ENDCOLR));
		}
	}
	// R&R, Robert Surcouf, Damage on Storm plots, End
}

// city plot mouse over help - inaiwae - START
void CvGameTextMgr::setCityPlotHelp(CvWStringBuffer& szString, CvPlot* pPlot)
{
	CvWString szTempBuffer;
	BonusTypes eBonus;
	ImprovementTypes eImprovement;
	PlayerTypes eRevealOwner;
	int iI;
	int iYield;
	int iTurns;
	if (pPlot->getTerrainType() != NO_TERRAIN)
	{
		if (pPlot->isPeak())
		{
			szString.append(gDLL->getText("TXT_KEY_PLOT_PEAK"));
		}
		else
		{
			if (pPlot->isWater())
			{
				szTempBuffer.Format(SETCOLR, TEXT_COLOR("COLOR_WATER_TEXT"));
				szString.append(szTempBuffer);
			}
			if (pPlot->isHills())
			{
				szString.append(gDLL->getText("TXT_KEY_PLOT_HILLS"));
			}
			if (pPlot->getFeatureType() != NO_FEATURE)
			{
				szTempBuffer.Format(L"%s/", GC.getFeatureInfo(pPlot->getFeatureType()).getDescription());
				szString.append(szTempBuffer);
			}
			szString.append(GC.getTerrainInfo(pPlot->getTerrainType()).getDescription());
			if (pPlot->isWater())
			{
				szString.append(ENDCOLR);
			}
		}
	}
	if (pPlot->hasYield())
	{
		szString.append(NEWLINE);
		for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
		{
			iYield = pPlot->calculatePotentialYield(((YieldTypes)iI), NULL, true);
			if (iYield != 0)
			{
				szTempBuffer.Format(L"%d%c ", iYield, GC.getYieldInfo((YieldTypes) iI).getChar());
				szString.append(szTempBuffer);
			}
		}
	}
	//WTP, ray, Lakes - START
	//not needed anymore, because we have Terrain Lake
	/*
	if (pPlot->isLake())
	{
		szString.append(NEWLINE);
		szString.append(gDLL->getText("TXT_KEY_PLOT_FRESH_WATER_LAKE"));
	}
	*/
	//WTP, ray, Lakes - END
	eBonus = pPlot->getBonusType();
	if (eBonus != NO_BONUS)
	{
		szTempBuffer.Format(L"%c " SETCOLR L"%s" ENDCOLR, GC.getBonusInfo(eBonus).getChar(), TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), GC.getBonusInfo(eBonus).getDescription());
		szString.append(NEWLINE);
		szString.append(szTempBuffer);
	}
	eImprovement = pPlot->getRevealedImprovementType(GC.getGameINLINE().getActiveTeam(), true);
	if (eImprovement != NO_IMPROVEMENT)
	{
		szString.append(NEWLINE);
		szString.append(GC.getImprovementInfo(eImprovement).getDescription());
		if (GC.getImprovementInfo(eImprovement).getImprovementUpgrade() != NO_IMPROVEMENT)
		{
			if ((pPlot->getUpgradeProgress() > 0) || pPlot->isBeingWorked())
			{
				eRevealOwner = pPlot->getRevealedOwner(GC.getGameINLINE().getActiveTeam(), true);
				iTurns = pPlot->getUpgradeTimeLeft(eImprovement, eRevealOwner);
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_PLOT_IMP_UPGRADE", iTurns, GC.getImprovementInfo((ImprovementTypes) GC.getImprovementInfo(eImprovement).getImprovementUpgrade()).getTextKeyWide()));
			}
		}
	}
	if (pPlot->getRevealedRouteType(GC.getGameINLINE().getActiveTeam(), true) != NO_ROUTE)
	{
		szString.append(NEWLINE);
		szString.append(GC.getRouteInfo(pPlot->getRevealedRouteType(GC.getGameINLINE().getActiveTeam(), true)).getDescription());
	}

}
// city plot mouse over help - inaiwae - END

void CvGameTextMgr::setCityPlotYieldValueString(CvWStringBuffer &szString, CvCity* pCity, int iIndex, bool bAvoidGrowth, bool bIgnoreGrowth, bool bIgnoreFood)
{
	PROFILE_FUNC();

	CvPlot* pPlot = NULL;

	if (iIndex >= 0 && iIndex < NUM_CITY_PLOTS)
		pPlot = pCity->getCityIndexPlot(iIndex);

	if (pPlot != NULL)
	{
		CvCityAI* pCityAI = static_cast<CvCityAI*>(pCity);
		bool bWorkingPlot = pCity->isUnitWorkingPlot(iIndex);

		int iValue = pCityAI->AI_plotValue(pPlot, bAvoidGrowth, /*bRemove*/ bWorkingPlot, bIgnoreFood, bIgnoreGrowth);

		setYieldValueString(szString, iValue, /*bActive*/ bWorkingPlot);
	}
	else
	{
		setYieldValueString(szString, 0, /*bActive*/ false, /*bMakeWhitespace*/ true);
	}
}

void CvGameTextMgr::setYieldValueString(CvWStringBuffer &szString, int iValue, bool bActive, bool bMakeWhitespace)
{
	PROFILE_FUNC();

	static bool bUseFloats = false;

	if (bActive)
		szString.append(CvWString::format(SETCOLR, TEXT_COLOR("COLOR_ALT_HIGHLIGHT_TEXT")));
	else
		szString.append(CvWString::format(SETCOLR, TEXT_COLOR("COLOR_HIGHLIGHT_TEXT")));

	if (!bMakeWhitespace)
	{
		if (bUseFloats)
		{
			float fValue = ((float) iValue) / 10000;
			szString.append(CvWString::format(L"%2.3f " ENDCOLR, fValue));
		}
		else
			szString.append(CvWString::format(L"%04d  " ENDCOLR, iValue/10));
	}
	else
		szString.append(CvWString::format(L"         " ENDCOLR));
}

void CvGameTextMgr::setCityBarHelp(CvWStringBuffer &szString, CvCity* pCity)
{
	PROFILE_FUNC();


	szString.append(pCity->getName());

	bool bFirst = true;
	CvWStringBuffer szTempBuffer;
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		if (GC.getYieldInfo(eYield).isCargo())
		{
			int iStored = pCity->getYieldStored(eYield);
			if (iStored > 0)
			{
				if (bFirst)
				{
					bFirst = false;
				}
				else
				{
					szTempBuffer.append(L", ");
				}
				szTempBuffer.append(CvWString::format(L"%d%c", iStored, GC.getYieldInfo(eYield).getChar()));
			}
		}
	}

	if (!bFirst)
	{
		szString.append(NEWLINE);
		szString.append(gDLL->getText("TXT_KEY_CITY_BAR_STORED", szTempBuffer.getCString()));
	}

	bFirst = true;
	szTempBuffer.clear();
	int aiYields[NUM_YIELD_TYPES];
	pCity->calculateNetYields(aiYields);
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		// WTP, ray, Happiness - START
		//if (aiYields[iYield] > 0 && (YieldTypes) iYield != YIELD_CULTURE  && (YieldTypes) iYield != YIELD_HEALTH) 
		if (aiYields[iYield] > 0 && (YieldTypes) iYield != YIELD_CULTURE  && (YieldTypes) iYield != YIELD_HEALTH && (YieldTypes) iYield != YIELD_HAPPINESS && (YieldTypes) iYield != YIELD_UNHAPPINESS) 
		{
			if (bFirst)
			{
				bFirst = false;
			}
			else
			{
				szTempBuffer.append(L", ");
			}
			szTempBuffer.append(CvWString::format(L"%d%c", aiYields[iYield], GC.getYieldInfo((YieldTypes) iYield).getChar()));
		}
	}

	int netCulture = pCity->getCultureRate();
	if (netCulture > 0)
	{
		szTempBuffer.append(L", ");
		szTempBuffer.append(CvWString::format(L"%d%c", netCulture, GC.getYieldInfo(YIELD_CULTURE).getChar()));
	}
	// R&R, ray, Health - START
	int netHealth = pCity->getCityHealthChange();
	if (netHealth != 0)
	{
		szTempBuffer.append(L", ");
		szTempBuffer.append(CvWString::format(L"%d%c", netHealth, GC.getYieldInfo(YIELD_HEALTH).getChar()));
	}
	// R&R, ray, Health - END
	if (!bFirst)
	{
		szString.append(NEWLINE);
		szString.append(gDLL->getText("TXT_KEY_CITY_BAR_PRODUCING", szTempBuffer.getCString()));
	}

	szString.append(NEWLINE);
	int iFoodDifference = aiYields[YIELD_FOOD];
	if (iFoodDifference <= 0)
	{
		szString.append(gDLL->getText("TXT_KEY_CITY_BAR_GROWTH", pCity->getFood(), pCity->growthThreshold()));
	}
	else
	{
		szString.append(gDLL->getText("TXT_KEY_CITY_BAR_FOOD_GROWTH", pCity->getFood(), pCity->growthThreshold(), pCity->getFoodTurnsLeft()));
	}

	if (pCity->getProductionNeeded(YIELD_HAMMERS) != MAX_INT)
	{
		szString.append(NEWLINE);
		if (aiYields[YIELD_HAMMERS] > 0)
		{
			//ray Hammer Icon Fix
			szString.append(gDLL->getText("TXT_KEY_CITY_BAR_HAMMER_PRODUCTION", pCity->getProductionName(), pCity->getProduction(), pCity->getProductionNeeded(YIELD_HAMMERS), pCity->getProductionTurnsLeft(), GC.getYieldInfo(YIELD_HAMMERS).getChar()));
		}
		else
		{
			//ray Hammer Icon Fix
			szString.append(gDLL->getText("TXT_KEY_CITY_BAR_PRODUCTION", pCity->getProductionName(), pCity->getProduction(), pCity->getProductionNeeded(YIELD_HAMMERS), GC.getYieldInfo(YIELD_HAMMERS).getChar()));
		}
	}

	bFirst = true;
	for (int iI = 0; iI < GC.getNumBuildingInfos(); ++iI)
	{
		if (pCity->isHasRealBuilding((BuildingTypes)iI))
		{
			setListHelp(szString, NEWLINE, GC.getBuildingInfo((BuildingTypes)iI).getDescription(), L", ", bFirst);
			bFirst = false;
		}
	}

	if (pCity->getCultureLevel() != NO_CULTURELEVEL)
	{
		szString.append(NEWLINE);
		szString.append(gDLL->getText("TXT_KEY_CITY_BAR_CULTURE", pCity->getCulture(pCity->getOwnerINLINE()), pCity->getCultureThreshold(), GC.getCultureLevelInfo(pCity->getCultureLevel()).getTextKeyWide()));
	}

	// R&R, ray, Health - START
	if (pCity->getCityHealth() != 0)
	{
		szString.append(NEWLINE);
		szString.append(gDLL->getText("TXT_KEY_TOTAL_CITY_HEALTH", pCity->getCityHealth(), GC.getYieldInfo(YIELD_HEALTH).getChar()));
	}
	// R&R, ray, Health - END
	// WTP, ray, Happiness - START
	int netHappiness = pCity->getCityHappiness();
	if (netHappiness != 0)
	{
		szString.append(NEWLINE);
		szString.append(gDLL->getText("TXT_KEY_TOTAL_CITY_HAPPINESS_BILLBOARD", netHappiness, GC.getYieldInfo(YIELD_HAPPINESS).getChar()));
	}
	int netUnHappiness = pCity->getCityUnHappiness();
	if (netUnHappiness != 0)
	{
		szString.append(NEWLINE);
		szString.append(gDLL->getText("TXT_KEY_TOTAL_CITY_UNHAPPINESS_BILLBOARD", netUnHappiness, GC.getYieldInfo(YIELD_UNHAPPINESS).getChar()));
	}
	// WTP, ray, Happiness - END

	szString.append(NEWLINE);

	szString.append(NEWLINE);
	szString.append(gDLL->getText("TXT_KEY_CITY_BAR_SELECT", pCity->getNameKey()));
	szString.append(NEWLINE);
	szString.append(gDLL->getText("TXT_KEY_CITY_BAR_SELECT_CTRL"));
	szString.append(NEWLINE);
	szString.append(gDLL->getText("TXT_KEY_CITY_BAR_SELECT_ALT"));
}


void CvGameTextMgr::parseTraits(CvWStringBuffer &szHelpString, TraitTypes eTrait, CivilizationTypes eCivilization, bool bDawnOfMan, bool bIndent)
{
	PROFILE_FUNC();

	CvWString szTempBuffer;

	CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);

	// Trait Name
	if (bIndent)
	{
		CvWString szText = kTrait.getDescription();
		if (bDawnOfMan)
		{
			szTempBuffer.Format(L"%s", szText.GetCString());
		}
		else
		{
			szTempBuffer.Format(NEWLINE SETCOLR L"%s" ENDCOLR, TEXT_COLOR("COLOR_ALT_HIGHLIGHT_TEXT"), szText.GetCString());
		}
		szHelpString.append(szTempBuffer);
	}

	if (!bDawnOfMan)
	{
		if (!isEmpty(kTrait.getHelp()))
		{
			szHelpString.append(kTrait.getHelp());
		}

		// iLevelExperienceModifier
		if (kTrait.getLevelExperienceModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_CIVIC_LEVEL_MODIFIER", kTrait.getLevelExperienceModifier()));
		}

		// iGreatGeneralRateModifier
		if (kTrait.getGreatGeneralRateModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_GREAT_GENERAL_MODIFIER", kTrait.getGreatGeneralRateModifier()));
		}

		if (kTrait.getDomesticGreatGeneralRateModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_DOMESTIC_GREAT_GENERAL_MODIFIER", kTrait.getDomesticGreatGeneralRateModifier()));
		}

		if (kTrait.getNativeAngerModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			if (kTrait.getNativeAngerModifier() > 0)
			{
				szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_NATIVE_ANGER_MODIFIER_PLUS"));
			}
			else
			{
				szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_NATIVE_ANGER_MODIFIER_MINUS"));
			}
		}

		if (kTrait.getLearnTimeModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_LEARN_TIME_MODIFIER", kTrait.getLearnTimeModifier()));
		}

		if (kTrait.getMercantileFactor() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			if (kTrait.getMercantileFactor() > 0)
			{
				szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_MARKET_SENSITIVE_HELP"));
			}
			else
			{
				szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_MERCANTILE_HELP"));
			}
		}

		int iTreasureModifier = 100 + kTrait.getTreasureModifier();
		if (iTreasureModifier != 100)
		{
			// R&R, ray, fixed Trait Display)
			/*if (eCivilization != NO_CIVILIZATION)
			{
				iTreasureModifier *= GC.getCivilizationInfo(eCivilization).getTreasure();
				iTreasureModifier /= 100;
			}

			if ((iTreasureModifier > 0) && (iTreasureModifier != 100))
			{*/
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_TREASURE_MODIFIER", iTreasureModifier - 100));
			//}
		}

		// WTP, ray, Happiness - START
		int iUnhappinessFromSlavesModifier = kTrait.getUnhappinessFromSlavesModifier();
		if(iUnhappinessFromSlavesModifier != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_UNHAPPINESS_SLAVES_MODIFIER", -iUnhappinessFromSlavesModifier, GC.getYieldInfo(YIELD_UNHAPPINESS).getChar()));
		}
		// WTP, ray, Happiness - START

		if (kTrait.getChiefGoldModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_CHIEF_GOLD_MODIFIER", kTrait.getChiefGoldModifier()));
		}

		// native combat
		if (kTrait.getNativeCombatModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_UNIT_NATIVE_COMBAT_MOD", kTrait.getNativeCombatModifier()));
		}

		if (kTrait.getMissionaryModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_EXTRA_MISSIONARY_RATE", kTrait.getMissionaryModifier()));
		}

		if (kTrait.getNativeTradeModifier() != 0) // R&R, ray, new Attribute in Traits
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_TRADE_EXTRA_TRADE_MODIFIER", kTrait.getNativeTradeModifier()));
		}

		if (kTrait.getRebelCombatModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_REBEL_COMBAT_MOD", kTrait.getRebelCombatModifier()));
		}

		if (kTrait.getTaxRateThresholdModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_TAX_RATE_THRESHOLD_MOD", kTrait.getTaxRateThresholdModifier()));
		}

		// CityExtraYield
		for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
		{
			if (kTrait.getCityExtraYield(iI) > 0)
			{
				szHelpString.append(NEWLINE);
				if (bIndent)
				{
					szHelpString.append(L"  ");
				}
				szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_CITY_EXTRA_YIELD", kTrait.getCityExtraYield(iI), GC.getYieldInfo((YieldTypes) iI).getChar()));
			}
		}

		// ExtraYieldThresholds
		for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
		{
			if (kTrait.getExtraYieldThreshold(iI) > 0)
			{
				szHelpString.append(NEWLINE);
				if (bIndent)
				{
					szHelpString.append(L"  ");
				}
				szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_EXTRA_YIELD_THRESHOLDS", GC.getYieldInfo((YieldTypes) iI).getChar(), kTrait.getExtraYieldThreshold(iI), GC.getYieldInfo((YieldTypes) iI).getChar()));
			}
		}

		for (int iI = 0; iI < GC.getNumProfessionInfos(); ++iI)
		{
			if (kTrait.getProfessionEquipmentModifier(iI) != 0)
			{
				szHelpString.append(NEWLINE);
				if (bIndent)
				{
					szHelpString.append(L"  ");
				}
				szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_PROFESSION_YIELD_EQUIPMENT_MOD", kTrait.getProfessionEquipmentModifier(iI), GC.getProfessionInfo((ProfessionTypes) iI).getTextKeyWide()));
			}
		}

		// Free Promotions
		bool bFoundPromotion = false;
		szTempBuffer.clear();
		for (int iI = 0; iI < GC.getNumPromotionInfos(); ++iI)
		{
			if (kTrait.isFreePromotion(iI))
			{
				if (bFoundPromotion)
				{
					szTempBuffer += L", ";
				}

				szTempBuffer += CvWString::format(L"<link=literal>%s</link>", GC.getPromotionInfo((PromotionTypes) iI).getDescription());
				bFoundPromotion = true;
			}
		}

		if (bFoundPromotion)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_FREE_PROMOTIONS", szTempBuffer.GetCString()));

			for (int iJ = 0; iJ < GC.getNumUnitCombatInfos(); iJ++)
			{
				if (kTrait.isFreePromotionUnitCombat(iJ))
				{
					szTempBuffer.Format(L"\n        %c%s", gDLL->getSymbolID(BULLET_CHAR), GC.getUnitCombatInfo((UnitCombatTypes)iJ).getDescription());
					szHelpString.append(szTempBuffer);
				}
			}
		}

		// Increase Building/Unit Production Speeds
		int iLast = 0;
		for (int iI = 0; iI < GC.getNumSpecialUnitInfos(); ++iI)
		{
			if (GC.getSpecialUnitInfo((SpecialUnitTypes) iI).getProductionTraits(eTrait) != 0)
			{
				CvWString szText(NEWLINE);
				if (bIndent)
				{
					szText += L"  ";
				}

				if (GC.getSpecialUnitInfo((SpecialUnitTypes) iI).getProductionTraits(eTrait) == 100)
				{
					szText += gDLL->getText("TXT_KEY_TRAIT_DOUBLE_SPEED");
				}
				else
				{
					szText += gDLL->getText("TXT_KEY_TRAIT_PRODUCTION_MODIFIER", GC.getSpecialUnitInfo((SpecialUnitTypes) iI).getProductionTraits(eTrait));
				}
				setListHelp(szHelpString, szText.GetCString(), GC.getSpecialUnitInfo((SpecialUnitTypes) iI).getDescription(), L", ", (GC.getSpecialUnitInfo((SpecialUnitTypes) iI).getProductionTraits(eTrait) != iLast));
				iLast = GC.getSpecialUnitInfo((SpecialUnitTypes) iI).getProductionTraits(eTrait);
			}
		}

		// Unit Classes
		iLast = 0;
		for (int iI = 0; iI < GC.getNumUnitClassInfos();++iI)
		{
			UnitTypes eLoopUnit;
			if (eCivilization == NO_CIVILIZATION)
			{
				eLoopUnit = ((UnitTypes)(GC.getUnitClassInfo((UnitClassTypes)iI).getDefaultUnitIndex()));
			}
			else
			{
				eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(eCivilization).getCivilizationUnits(iI)));
			}

			if (eLoopUnit != NO_UNIT)
			{
				if (GC.getUnitInfo(eLoopUnit).getProductionTraits(eTrait) != 0)
				{
					CvWString szText(NEWLINE);
					if (bIndent)
					{
						szText += L"  ";
					}
					if (GC.getUnitInfo(eLoopUnit).getProductionTraits(eTrait) == 100)
					{
						szText += gDLL->getText("TXT_KEY_TRAIT_DOUBLE_SPEED");
					}
					else
					{
						szText += gDLL->getText("TXT_KEY_TRAIT_PRODUCTION_MODIFIER", GC.getUnitInfo(eLoopUnit).getProductionTraits(eTrait));
					}

					CvWString szUnit;
					szUnit.Format(L"<link=literal>%s</link>", GC.getUnitInfo(eLoopUnit).getDescription());
					setListHelp(szHelpString, szText.GetCString(), szUnit, L", ", (GC.getUnitInfo(eLoopUnit).getProductionTraits(eTrait) != iLast));
					iLast = GC.getUnitInfo(eLoopUnit).getProductionTraits(eTrait);
				}
			}
		}

		// SpecialBuildings
		iLast = 0;
		for (int iI = 0; iI < GC.getNumSpecialBuildingInfos(); ++iI)
		{
			if (GC.getSpecialBuildingInfo((SpecialBuildingTypes) iI).getProductionTraits(eTrait) != 0)
			{
				CvWString szText(NEWLINE);
				if (bIndent)
				{
					szText += L"  ";
				}
				if (GC.getSpecialBuildingInfo((SpecialBuildingTypes) iI).getProductionTraits(eTrait) == 100)
				{
					szText += gDLL->getText("TXT_KEY_TRAIT_DOUBLE_SPEED");
				}
				else
				{
					szText += gDLL->getText("TXT_KEY_TRAIT_PRODUCTION_MODIFIER", GC.getSpecialBuildingInfo((SpecialBuildingTypes) iI).getProductionTraits(eTrait));
				}
				setListHelp(szHelpString, szText.GetCString(), GC.getSpecialBuildingInfo((SpecialBuildingTypes) iI).getDescription(), L", ", (GC.getSpecialBuildingInfo((SpecialBuildingTypes) iI).getProductionTraits(eTrait) != iLast));
				iLast = GC.getSpecialBuildingInfo((SpecialBuildingTypes) iI).getProductionTraits(eTrait);
			}
		}

		// Buildings
		iLast = 0;
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); ++iI)
		{
			BuildingTypes eLoopBuilding;
			if (eCivilization == NO_CIVILIZATION)
			{
				eLoopBuilding = ((BuildingTypes)(GC.getBuildingClassInfo((BuildingClassTypes)iI).getDefaultBuildingIndex()));
			}
			else
			{
				eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(eCivilization).getCivilizationBuildings(iI)));
			}

			if (eLoopBuilding != NO_BUILDING)
			{
				if (GC.getBuildingInfo(eLoopBuilding).getProductionTraits(eTrait) != 0)
				{
					CvWString szText(NEWLINE);
					if (bIndent)
					{
						szText += L"  ";
					}
					if (GC.getBuildingInfo(eLoopBuilding).getProductionTraits(eTrait) == 100)
					{
						szText += gDLL->getText("TXT_KEY_TRAIT_DOUBLE_SPEED");
					}
					else
					{
						szText += gDLL->getText("TXT_KEY_TRAIT_PRODUCTION_MODIFIER", GC.getBuildingInfo(eLoopBuilding).getProductionTraits(eTrait));
					}

					CvWString szBuilding;
					szBuilding.Format(L"<link=literal>%s</link>", GC.getBuildingInfo(eLoopBuilding).getDescription());
					setListHelp(szHelpString, szText.GetCString(), szBuilding, L", ", (GC.getBuildingInfo(eLoopBuilding).getProductionTraits(eTrait) != iLast));
					iLast = GC.getBuildingInfo(eLoopBuilding).getProductionTraits(eTrait);
				}
			}
		}

		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			if (kTrait.getYieldModifier(iYield) != 0)
			{
				szTempBuffer = gDLL->getText("TXT_KEY_PERCENT_CHANGE_ALL_CITIES", kTrait.getYieldModifier(iYield), GC.getYieldInfo((YieldTypes)iYield).getChar());
				szHelpString.append(NEWLINE);
				if (bIndent)
				{
					szHelpString.append(L"  ");
				}
				szHelpString.append(szTempBuffer);
			}
		}

		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			if (kTrait.isTaxYieldModifier(iYield))
			{
				szTempBuffer = gDLL->getText("TXT_KEY_TAX_RATE_YIELD_INCREASE", GC.getYieldInfo((YieldTypes)iYield).getChar());
				szHelpString.append(NEWLINE);
				if (bIndent)
				{
					szHelpString.append(L"  ");
				}
				szHelpString.append(szTempBuffer);
			}
		}

		for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
		{
			BuildingTypes eBuilding = (BuildingTypes) GC.getBuildingClassInfo((BuildingClassTypes) iBuildingClass).getDefaultBuildingIndex();

			if (eCivilization != NO_CIVILIZATION)
			{
				eBuilding = (BuildingTypes) GC.getCivilizationInfo(eCivilization).getCivilizationBuildings(iBuildingClass);
			}

			if (eBuilding != NO_BUILDING)
			{
				for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
				{
					if (kTrait.getBuildingYieldChange(iBuildingClass, iYield) != 0)
					{
						szTempBuffer = gDLL->getText("TXT_KEY_BUILDING_YIELD_INCREASE", kTrait.getBuildingYieldChange(iBuildingClass, iYield), GC.getYieldInfo((YieldTypes)iYield).getChar(), GC.getBuildingInfo(eBuilding).getTextKeyWide());
						szHelpString.append(NEWLINE);
						if (bIndent)
						{
							szHelpString.append(L"  ");
						}
						szHelpString.append(szTempBuffer);
					}
				}
			}
		}

		for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); ++iUnitClass)
		{
			UnitTypes eUnit = (UnitTypes) GC.getUnitClassInfo((UnitClassTypes) iUnitClass).getDefaultUnitIndex();

			if (eCivilization != NO_CIVILIZATION)
			{
				eUnit = (UnitTypes) GC.getCivilizationInfo(eCivilization).getCivilizationUnits(iUnitClass);
			}

			if (eUnit != NO_UNIT)
			{
				if (kTrait.getUnitMoveChange(iUnitClass) != 0)
				{
					szTempBuffer = gDLL->getText("TXT_KEY_UNIT_MOVES_INCREASE", kTrait.getUnitMoveChange(iUnitClass), GC.getUnitInfo(eUnit).getTextKeyWide());
					szHelpString.append(NEWLINE);
					if (bIndent)
					{
						szHelpString.append(L"  ");
					}
					szHelpString.append(szTempBuffer);
				}

				if (kTrait.getUnitStrengthModifier(iUnitClass) != 0)
				{
					szTempBuffer = gDLL->getText("TXT_KEY_UNIT_STRENGTH_INCREASE", kTrait.getUnitStrengthModifier(iUnitClass), GC.getUnitInfo(eUnit).getTextKeyWide());
					szHelpString.append(NEWLINE);
					if (bIndent)
					{
						szHelpString.append(L"  ");
					}
					szHelpString.append(szTempBuffer);
				}
			}
		}

		for (int iProfession = 0; iProfession < GC.getNumProfessionInfos(); ++iProfession)
		{
			if (eCivilization == NO_CIVILIZATION || GC.getCivilizationInfo(eCivilization).isValidProfession(iProfession))
			{
				if (kTrait.getProfessionMoveChange(iProfession) != 0)
				{
					szTempBuffer = gDLL->getText("TXT_KEY_UNIT_MOVES_INCREASE", kTrait.getProfessionMoveChange(iProfession), GC.getProfessionInfo((ProfessionTypes)iProfession).getTextKeyWide());
					szHelpString.append(NEWLINE);
					if (bIndent)
					{
						szHelpString.append(L"  ");
					}
					szHelpString.append(szTempBuffer);
				}
			}
		}

		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); ++iI)
		{
			if (kTrait.isFreeBuildingClass(iI))
			{
				BuildingTypes eFreeBuilding;
				if (eCivilization != NO_CIVILIZATION)
				{
					eFreeBuilding = ((BuildingTypes)(GC.getCivilizationInfo(eCivilization).getCivilizationBuildings(iI)));
				}
				else
				{
					eFreeBuilding = (BuildingTypes)GC.getBuildingClassInfo((BuildingClassTypes)iI).getDefaultBuildingIndex();
				}

				if (NO_BUILDING != eFreeBuilding)
				{
					szHelpString.append(NEWLINE);
					if (bIndent)
					{
						szHelpString.append(L"  ");
					}
					szHelpString.append(gDLL->getText("TXT_KEY_BUILDING_FREE_IN_CITY", GC.getBuildingInfo(eFreeBuilding).getTextKeyWide()));
				}
			}
		}

		iLast = 0;
		for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
		{
			BuildingTypes eBuilding = (BuildingTypes) GC.getBuildingClassInfo((BuildingClassTypes) iBuildingClass).getDefaultBuildingIndex();

			if (eCivilization != NO_CIVILIZATION)
			{
				eBuilding = (BuildingTypes) GC.getCivilizationInfo(eCivilization).getCivilizationBuildings(iBuildingClass);
			}

			if (eBuilding != NO_BUILDING)
			{
				int iModifier = kTrait.getBuildingProductionModifier(iBuildingClass);
				if (iModifier != 0)
				{
					CvWString szText = NEWLINE;
					if (bIndent)
					{
						szText += L"  ";
					}

					if (iModifier == 100)
					{
						szText += gDLL->getText("TXT_KEY_TRAIT_DOUBLE_SPEED");
					}
					else
					{
						szText += gDLL->getText("TXT_KEY_TRAIT_PRODUCTION_MODIFIER", iModifier);
					}

					CvWString szBuilding;
					szBuilding.Format(L"<link=literal>%s</link>", GC.getBuildingInfo(eBuilding).getDescription());
					setListHelp(szHelpString, szText.GetCString(), szBuilding, L", ", iModifier != iLast);
					iLast = iModifier;
				}
			}
		}

		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			int iModifier = kTrait.getBuildingRequiredYieldModifier(iYield);
			if (iModifier != 0)
			{
				szTempBuffer = gDLL->getText("TXT_KEY_REQUIRED_YIELD_MODIFIER", iModifier, GC.getYieldInfo((YieldTypes)iYield).getChar());
				szHelpString.append(NEWLINE);
				if (bIndent)
				{
					szHelpString.append(L"  ");
				}
				szHelpString.append(szTempBuffer);
			}
		}

		if (kTrait.getNativeAttitudeChange() > 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_NATIVE_ATTITUDE_GOOD"));
			// R&R, ray, dirty but easy hack to display traits properly
			if (kTrait.getNativeAttitudeChange() != 2)
			{
				szHelpString.append(NEWLINE);
				szHelpString.append(gDLL->getText("TXT_KEY_FATHER_NATIVE_ATTITUDE_GOOD2"));
			}
		}
		else if (kTrait.getNativeAttitudeChange() < 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_NATIVE_ATTITUDE_BAD"));
		}

		// R&R, ray, new Attribute in Traits - START
		if (kTrait.getEuropeanAttitudeChange() > 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_EUROPEAN_ATTITUDE_GOOD"));
		}
		else if (kTrait.getEuropeanAttitudeChange() < 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_EUROPEAN_ATTITUDE_BAD"));
		}

		if (kTrait.getKingAttitudeChange() > 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_KING_ATTITUDE_GOOD"));
		}
		else if (kTrait.getKingAttitudeChange() < 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_KING_ATTITUDE_BAD"));
		}
		// R&R, ray, new Attribute in Traits - END

		if (kTrait.getCityDefense() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_CITY_DEFENSE", kTrait.getCityDefense()));
		}

		if (kTrait.getLandPriceDiscount() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_LAND_DISCOUNT", kTrait.getLandPriceDiscount()));
		}

		if (kTrait.getRecruitPriceDiscount() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_RECRUIT_DISCOUNT", -kTrait.getRecruitPriceDiscount()));
		}

		if (kTrait.getEuropeTravelTimeModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_EUROPE_TRAVEL_MODIFIER", kTrait.getEuropeTravelTimeModifier()));
		}

		if (kTrait.getImmigrationThresholdModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_IMMIGRATION_THRESHOLD_MODIFIER", kTrait.getImmigrationThresholdModifier(), GC.getYieldInfo(YIELD_CROSSES).getChar()));
		}
		// Schmiddie, 7 new variables for traits for Europeans, START
		if (kTrait.getPopGrowthThresholdModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_GROWTH_THRESHOLD_MODIFIER", kTrait.getPopGrowthThresholdModifier(), GC.getYieldInfo(YIELD_FOOD).getChar()));
		}

		if (kTrait.getCultureLevelModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_CULTURE_LEVEL_MODIFIER", kTrait.getCultureLevelModifier(), GC.getYieldInfo(YIELD_CULTURE).getChar()));
		}

		if (kTrait.getPioneerSpeedModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_PIONEER_SPEED_MODIFIER", kTrait.getPioneerSpeedModifier()));
		}

		if (kTrait.getImprovementPriceModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_IMPROVEMENT_PRICE_MODIFIER", kTrait.getImprovementPriceModifier()));
		}

		if (kTrait.getLearningByDoingModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_LEARNING_BY_DOING_MODIFIER", kTrait.getLearningByDoingModifier()));
		}

		if (kTrait.getSpecialistPriceModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_SPECIALIST_PRICE_MODIFIER", kTrait.getSpecialistPriceModifier()));
		}

		if (kTrait.getStorageCapacityModifier() != 0)
		{
			szHelpString.append(NEWLINE);
			if (bIndent)
			{
				szHelpString.append(L"  ");
			}
			szHelpString.append(gDLL->getText("TXT_KEY_FATHER_STORAGE_CAPACITY_MODIFIER", kTrait.getStorageCapacityModifier()));
		}
		// Schmiddie, 7 new variables for traits for Europeans, END
		for (int iGoody = 0; iGoody < GC.getNumGoodyInfos(); ++iGoody)
		{
			CvGoodyInfo& kGoodyInfo = GC.getGoodyInfo((GoodyTypes) iGoody);
			if (kTrait.getGoodyFactor(iGoody) > 1)
			{
				szHelpString.append(NEWLINE);
				if (bIndent)
				{
					szHelpString.append(L"  ");
				}
				szHelpString.append(gDLL->getText("TXT_KEY_FATHER_EXTRA_GOODY", kTrait.getGoodyFactor(iGoody), kGoodyInfo.getTextKeyWide()));
			}
			else if (kTrait.getGoodyFactor(iGoody) == 0)
			{
				szHelpString.append(NEWLINE);
				if (bIndent)
				{
					szHelpString.append(L"  ");
				}
				szHelpString.append(gDLL->getText("TXT_KEY_FATHER_NO_GOODY", kGoodyInfo.getTextKeyWide()));
			}
		}
	}
}

void CvGameTextMgr::parseLeaderTraits(CvWStringBuffer &szHelpString, LeaderHeadTypes eLeader, CivilizationTypes eCivilization, bool bDawnOfMan, bool bCivilopediaText)
{
	PROFILE_FUNC();

	CvWString szTempBuffer;	// Formatting
	int iI;

	//	Build help string
	if (eLeader != NO_LEADER)
	{
		if (!bDawnOfMan && !bCivilopediaText)
		{
			szTempBuffer.Format( SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), GC.getLeaderHeadInfo(eLeader).getDescription());
			szHelpString.append(szTempBuffer);
		}

		FAssert((GC.getNumTraitInfos() > 0) &&
			"GC.getNumTraitInfos() is less than or equal to zero but is expected to be larger than zero in CvSimpleCivPicker::setLeaderText");

		// R&R, Androrc Trait Description
		/*
		bool bFirst = true;
		for (iI = 0; iI < GC.getNumTraitInfos(); ++iI)
		{
			if (GC.getLeaderHeadInfo(eLeader).hasTrait(iI) || (eCivilization != NO_CIVILIZATION && GC.getCivilizationInfo(eCivilization).hasTrait(iI)))
			{
				if (!bFirst)
				{
					if (bDawnOfMan)
					{
						szHelpString.append(L", ");
					}
				}
				else
				{
					bFirst = false;
				}

				parseTraits(szHelpString, ((TraitTypes)iI), eCivilization, bDawnOfMan);
			}
		}
		*/
		bool bFirst = true;
		bool bFirstCivilizationTrait = true;
		bool bFirstLeaderTrait = true;
		for (iI = 0; iI < GC.getNumTraitInfos(); ++iI)
		{
			if (eCivilization != NO_CIVILIZATION && GC.getCivilizationInfo(eCivilization).hasTrait(iI))
			{
				if (bFirstCivilizationTrait)
				{
					// R&R, Robert Surcouf, Removing extra new lines in Dawn of Man screen, Start
					if (!bDawnOfMan)
					{
						szHelpString.append(NEWLINE);
					}
					//szHelpString.append(NEWLINE);
					//szTempBuffer.Format( SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), gDLL->getText("TXT_KEY_CIVILIZATION_TRAITS").c_str());
					szTempBuffer.Format(gDLL->getText("TXT_KEY_CIVILIZATION_TRAITS").c_str());
					szHelpString.append(szTempBuffer);
					if (bDawnOfMan)
					{
						szHelpString.append(L", ");
					}
					// R&R, Robert Surcouf, Removing extra new lines in Dawn of Man screen, End
					bFirstCivilizationTrait = false;
				}
				if (!bFirst)
				{
					if (bDawnOfMan)
					{
						szHelpString.append(L", ");
					}
				}
				else
				{
					bFirst = false;
				}

				parseTraits(szHelpString, ((TraitTypes)iI), eCivilization, bDawnOfMan);
			}
		}
		for (iI = 0; iI < GC.getNumTraitInfos(); ++iI)
		{
			if (GC.getLeaderHeadInfo(eLeader).hasTrait(iI))
			{
				if (bFirstLeaderTrait)
				{
					if (!bFirstCivilizationTrait)
					{
						szHelpString.append(NEWLINE);
					}
					// R&R, Robert Surcouf, Removing extra new lines in Dawn of Man screen, Start
					if (!bDawnOfMan)
					{
						szHelpString.append(NEWLINE);
					}
					//szHelpString.append(NEWLINE);
					//szTempBuffer.Format( SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), gDLL->getText("TXT_KEY_LEADER_TRAITS").c_str());
					szTempBuffer.Format(gDLL->getText("TXT_KEY_LEADER_TRAITS").c_str());
					// R&R, Robert Surcouf, Removing extra new lines in Dawn of Man screen, End
					szHelpString.append(szTempBuffer);
					bFirstLeaderTrait = false;
				}
				if (!bFirst)
				{
					if (bDawnOfMan)
					{
						szHelpString.append(L", ");
					}
				}
				else
				{
					bFirst = false;
				}

				parseTraits(szHelpString, ((TraitTypes)iI), eCivilization, bDawnOfMan);
			}
		}
		// R&R Androrc End
		
	}
	else
	{
		//	Random leader
		szTempBuffer.Format( SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), gDLL->getText("TXT_KEY_TRAIT_PLAYER_UNKNOWN").c_str());
		szHelpString.append(szTempBuffer);
	}
}

void CvGameTextMgr::parseLeaderShortTraits(CvWStringBuffer &szHelpString, LeaderHeadTypes eLeader)
{
	//	Build help string
	if (eLeader != NO_LEADER)
	{
		FAssert((GC.getNumTraitInfos() > 0) && "GC.getNumTraitInfos() is less than or equal to zero but is expected to be larger than zero in CvSimpleCivPicker::setLeaderText");

		bool bFirst = true;
		for (int iI = 0; iI < GC.getNumTraitInfos(); ++iI)
		{
			if (GC.getLeaderHeadInfo(eLeader).hasTrait(iI))
			{
				if (!bFirst)
				{
					szHelpString.append(L"/");
				}
				else
				{
					szHelpString.append(L"[");
				}
				szHelpString.append(gDLL->getText(GC.getTraitInfo((TraitTypes)iI).getShortDescription()));
				bFirst = false;
			}
		}
		if (!bFirst)
		{
			szHelpString.append(L"]");
		}
	}
	else
	{
		//	Random leader
		szHelpString.append(CvWString("[???/???]"));
	}
}

void CvGameTextMgr::parseCivShortTraits(CvWStringBuffer &szHelpString, CivilizationTypes eCiv)
{
	//	Build help string
	if (eCiv != NO_CIVILIZATION)
	{
		FAssert((GC.getNumTraitInfos() > 0) && "GC.getNumTraitInfos() is less than or equal to zero but is expected to be larger than zero in CvSimpleCivPicker::setLeaderText");

		bool bFirst = true;
		for (int iI = 0; iI < GC.getNumTraitInfos(); ++iI)
		{
			if (GC.getCivilizationInfo(eCiv).hasTrait(iI))
			{
				if (!bFirst)
				{
					szHelpString.append(L"/");
				}
				else
				{
					szHelpString.append(L"[");
				}
				szHelpString.append(gDLL->getText(GC.getTraitInfo((TraitTypes)iI).getShortDescription()));
				bFirst = false;
			}
		}

		if (!bFirst)
		{
			szHelpString.append(L"]");
		}
	}
	else
	{
		//	Random leader
		szHelpString.append(CvWString("[???/???]"));
	}
}

//
// Build Civilization Info Help Text
//
void CvGameTextMgr::parseCivInfos(CvWStringBuffer &szInfoText, CivilizationTypes eCivilization, bool bDawnOfMan, bool bLinks)
{
	PROFILE_FUNC();

	CvWString szBuffer;
	CvWStringBuffer szTempString;
	CvWString szText;
	UnitTypes eDefaultUnit;
	UnitTypes eUniqueUnit;
	BuildingTypes eDefaultBuilding;
	BuildingTypes eUniqueBuilding;

	if (eCivilization != NO_CIVILIZATION)
	{
		CvCivilizationInfo& kCivilizationInfo = GC.getCivilizationInfo(eCivilization);

		if (!bDawnOfMan)
		{
			// Civ Name
			szBuffer.Format(SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), kCivilizationInfo.getDescription());
			szInfoText.append(szBuffer);
		}

		// Free Units
		szText = gDLL->getText("TXT_KEY_FREE_UNITS");
		if (bDawnOfMan)
		{
			szTempString.assign(CvWString::format(L"%s: ", szText.GetCString()));
		}
		else
		{
			szTempString.assign(CvWString::format(NEWLINE SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_ALT_HIGHLIGHT_TEXT"), szText.GetCString()));
		}

		bool bFound = false;
		for (int iI = 0; iI < GC.getNumUnitClassInfos(); ++iI)
		{
			eDefaultUnit = ((UnitTypes)(kCivilizationInfo.getCivilizationUnits(iI)));
			eUniqueUnit = ((UnitTypes)(GC.getUnitClassInfo((UnitClassTypes) iI).getDefaultUnitIndex()));
			if ((eDefaultUnit != NO_UNIT) && (eUniqueUnit != NO_UNIT))
			{
				if (eDefaultUnit != eUniqueUnit)
				{
					// Add Unit
					if (bDawnOfMan)
					{
						if (bFound)
						{
							szInfoText.append(L", ");
						}
						szBuffer.Format((bLinks ? L"<link=literal>%s</link> - (<link=literal>%s</link>)" : L"%s - (%s)"),
							GC.getUnitInfo(eDefaultUnit).getDescription(),
							GC.getUnitInfo(eUniqueUnit).getDescription());
					}
					else
					{
						szBuffer.Format(L"\n  %c%s - (%s)", gDLL->getSymbolID(BULLET_CHAR),
							GC.getUnitInfo(eDefaultUnit).getDescription(),
							GC.getUnitInfo(eUniqueUnit).getDescription());
					}
					szTempString.append(szBuffer);
					bFound = true;
				}
			}
		}
		if (bFound)
		{
			szInfoText.append(szTempString);
		}

		// Free Buildings
		szText = gDLL->getText("TXT_KEY_UNIQUE_BUILDINGS");
		if (bDawnOfMan)
		{
			if (bFound)
			{
				szInfoText.append(NEWLINE);
			}
			szTempString.assign(CvWString::format(L"%s: ", szText.GetCString()));
		}
		else
		{
			szTempString.assign(CvWString::format(NEWLINE SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_ALT_HIGHLIGHT_TEXT"), szText.GetCString()));
		}

		bFound = false;
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); ++iI)
		{
			eDefaultBuilding = ((BuildingTypes)(kCivilizationInfo.getCivilizationBuildings(iI)));
			eUniqueBuilding = ((BuildingTypes)(GC.getBuildingClassInfo((BuildingClassTypes) iI).getDefaultBuildingIndex()));
			if ((eDefaultBuilding != NO_BUILDING) && (eUniqueBuilding != NO_BUILDING))
			{
				if (eDefaultBuilding != eUniqueBuilding)
				{
					// Add Building
					if (bDawnOfMan)
					{
						if (bFound)
						{
							szInfoText.append(L", ");
						}
						szBuffer.Format((bLinks ? L"<link=literal>%s</link> - (<link=literal>%s</link>)" : L"%s - (%s)"),
							GC.getBuildingInfo(eDefaultBuilding).getDescription(),
							GC.getBuildingInfo(eUniqueBuilding).getDescription());
					}
					else
					{
						szBuffer.Format(L"\n  %c%s - (%s)", gDLL->getSymbolID(BULLET_CHAR),
							GC.getBuildingInfo(eDefaultBuilding).getDescription(),
							GC.getBuildingInfo(eUniqueBuilding).getDescription());
					}
					szTempString.append(szBuffer);
					bFound = true;
				}
			}
		}
		if (bFound)
		{
			szInfoText.append(szTempString);
		}

		if (!bDawnOfMan)
		{
			CvWString szDesc;
			for (int iI = 0; iI < kCivilizationInfo.getNumCivilizationFreeUnits(); iI++)
			{
				int iLoopUnitClass = kCivilizationInfo.getCivilizationFreeUnitsClass(iI);
				ProfessionTypes eLoopUnitProfession = (ProfessionTypes) kCivilizationInfo.getCivilizationFreeUnitsProfession(iI);
				UnitTypes eLoopUnit = (UnitTypes)kCivilizationInfo.getCivilizationUnits(iLoopUnitClass);

				if (eLoopUnit != NO_UNIT)
				{
					if (eLoopUnitProfession != NO_PROFESSION)
					{
						szDesc += CvWString::format(L"\n  %c%s (%s)", gDLL->getSymbolID(BULLET_CHAR), GC.getProfessionInfo(eLoopUnitProfession).getDescription(), GC.getUnitInfo(eLoopUnit).getDescription());
					}
					else
					{
						szDesc += CvWString::format(L"\n  %c%s", gDLL->getSymbolID(BULLET_CHAR), GC.getUnitInfo(eLoopUnit).getDescription());
					}
				}
			}
			if (!szDesc.empty())
			{
				szInfoText.append(NEWLINE);
				szInfoText.append(gDLL->getText("TXT_KEY_DAWN_OF_MAN_SCREEN_STARTING_UNITS"));
				szInfoText.append(szDesc);
			}
		}
	}
	else
	{
		//	This is a random civ, let us know here...
		szInfoText.append(gDLL->getText("TXT_KEY_MAIN_MENU_RANDOM"));
	}
}


//
// Promotion Help
//
void CvGameTextMgr::parsePromotionHelp(CvWStringBuffer &szBuffer, PromotionTypes ePromotion, const wchar* pcNewline)
{
	PROFILE_FUNC();

	CvWString szText, szText2;
	int iI;

	if (NO_PROMOTION == ePromotion)
	{
		return;
	}

	CvPromotionInfo& kPromotion = GC.getPromotionInfo(ePromotion);

	if (kPromotion.isBlitz())
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_BLITZ_TEXT"));
	}

	if (kPromotion.isAmphib())
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_AMPHIB_TEXT"));
	}

	if (kPromotion.isRiver())
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_RIVER_ATTACK_TEXT"));
	}

	if (kPromotion.isEnemyRoute())
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_ENEMY_ROADS_TEXT"));
	}

	if (kPromotion.isAlwaysHeal())
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_ALWAYS_HEAL_TEXT"));
	}

	if (kPromotion.isHillsDoubleMove())
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_HILLS_MOVE_TEXT"));
	}

	for (iI = 0; iI < GC.getNumTerrainInfos(); ++iI)
	{
		if (kPromotion.getTerrainDoubleMove(iI))
		{
			szBuffer.append(pcNewline);
			szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_DOUBLE_MOVE_TEXT", GC.getTerrainInfo((TerrainTypes) iI).getTextKeyWide()));
		}
	}

	for (iI = 0; iI < GC.getNumFeatureInfos(); ++iI)
	{
		if (kPromotion.getFeatureDoubleMove(iI))
		{
			szBuffer.append(pcNewline);
			szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_DOUBLE_MOVE_TEXT", GC.getFeatureInfo((FeatureTypes) iI).getTextKeyWide()));
		}
	}

	if (kPromotion.getVisibilityChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_VISIBILITY_TEXT", kPromotion.getVisibilityChange()));
	}

	if (kPromotion.getMovesChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_MOVE_TEXT", kPromotion.getMovesChange()));
	}

	if (kPromotion.getMoveDiscountChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_MOVE_DISCOUNT_TEXT", -(kPromotion.getMoveDiscountChange())));
	}

	if (kPromotion.getWithdrawalChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_WITHDRAWAL_TEXT", kPromotion.getWithdrawalChange()));
	}

	if (kPromotion.getCargoChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_CARGO_TEXT", kPromotion.getCargoChange()));
	}

	if (kPromotion.getBombardRateChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_BOMBARD_TEXT", kPromotion.getBombardRateChange()));
	}

	if (kPromotion.getEnemyHealChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_HEALS_EXTRA_TEXT", kPromotion.getEnemyHealChange()) + gDLL->getText("TXT_KEY_PROMOTION_ENEMY_LANDS_TEXT"));
	}

	if (kPromotion.getNeutralHealChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_HEALS_EXTRA_TEXT", kPromotion.getNeutralHealChange()) + gDLL->getText("TXT_KEY_PROMOTION_NEUTRAL_LANDS_TEXT"));
	}

	if (kPromotion.getFriendlyHealChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_HEALS_EXTRA_TEXT", kPromotion.getFriendlyHealChange()) + gDLL->getText("TXT_KEY_PROMOTION_FRIENDLY_LANDS_TEXT"));
	}

	if (kPromotion.getSameTileHealChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_HEALS_SAME_TEXT", kPromotion.getSameTileHealChange()) + gDLL->getText("TXT_KEY_PROMOTION_DAMAGE_TURN_TEXT"));
	}

	if (kPromotion.getAdjacentTileHealChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_HEALS_ADJACENT_TEXT", kPromotion.getAdjacentTileHealChange()) + gDLL->getText("TXT_KEY_PROMOTION_DAMAGE_TURN_TEXT"));
	}

	if (kPromotion.getCombatPercent() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_STRENGTH_TEXT", kPromotion.getCombatPercent()));
	}

	if (kPromotion.getCityAttackPercent() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_CITY_ATTACK_TEXT", kPromotion.getCityAttackPercent()));
	}

	if (kPromotion.getCityDefensePercent() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_CITY_DEFENSE_TEXT", kPromotion.getCityDefensePercent()));
	}

	if (kPromotion.getHillsAttackPercent() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_HILLS_ATTACK", kPromotion.getHillsAttackPercent()));
	}

	if (kPromotion.getHillsDefensePercent() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_HILLS_DEFENSE_TEXT", kPromotion.getHillsDefensePercent()));
	}

	if (kPromotion.getDomesticBonusPercent() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_DOMESTIC_BONUS_TEXT", kPromotion.getDomesticBonusPercent()));			
	}

	if (kPromotion.getPillageChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_PILLAGE_CHANGE_TEXT", kPromotion.getPillageChange()));
	}

	//WTP, ray, Animal Promotions increase gold from Animals - START
	if (kPromotion.getAnimalGoldChange() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_ANIMAL_GOLD_CHANGE_TEXT", kPromotion.getAnimalGoldChange()));
	}

	//WTP, ray, Animal Promotions increase gold from Animals - END

	if (kPromotion.getUpgradeDiscount() != 0)
	{
		if (100 == kPromotion.getUpgradeDiscount())
		{
			szBuffer.append(pcNewline);
			szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_UPGRADE_DISCOUNT_FREE_TEXT"));
		}
		else
		{
			szBuffer.append(pcNewline);
			szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_UPGRADE_DISCOUNT_TEXT", kPromotion.getUpgradeDiscount()));
		}
	}

	if (kPromotion.getExperiencePercent() != 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_FASTER_EXPERIENCE_TEXT", kPromotion.getExperiencePercent()));
	}

	for (iI = 0; iI < GC.getNumTerrainInfos(); ++iI)
	{
		if (kPromotion.getTerrainAttackPercent(iI) != 0)
		{
			szBuffer.append(pcNewline);
			szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_ATTACK_TEXT", kPromotion.getTerrainAttackPercent(iI), GC.getTerrainInfo((TerrainTypes) iI).getTextKeyWide()));
		}

		if (kPromotion.getTerrainDefensePercent(iI) != 0)
		{
			szBuffer.append(pcNewline);
			szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_DEFENSE_TEXT", kPromotion.getTerrainDefensePercent(iI), GC.getTerrainInfo((TerrainTypes) iI).getTextKeyWide()));
		}
	}

	for (iI = 0; iI < GC.getNumFeatureInfos(); ++iI)
	{
		if (kPromotion.getFeatureAttackPercent(iI) != 0)
		{
			szBuffer.append(pcNewline);
			szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_ATTACK_TEXT", kPromotion.getFeatureAttackPercent(iI), GC.getFeatureInfo((FeatureTypes) iI).getTextKeyWide()));
		}

		if (kPromotion.getFeatureDefensePercent(iI) != 0)
		{
			szBuffer.append(pcNewline);
			szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_DEFENSE_TEXT", kPromotion.getFeatureDefensePercent(iI), GC.getFeatureInfo((FeatureTypes) iI).getTextKeyWide()));
		}
	}

	for (iI = 0; iI < GC.getNumUnitClassInfos(); ++iI)
	{
		if (kPromotion.getUnitClassAttackModifier(iI) == kPromotion.getUnitClassDefenseModifier(iI))
		{
			if (kPromotion.getUnitClassAttackModifier(iI) != 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_UNIT_MOD_VS_TYPE", kPromotion.getUnitClassAttackModifier(iI), GC.getUnitClassInfo((UnitClassTypes)iI).getTextKeyWide()));
			}
		}
		else
		{
			if (kPromotion.getUnitClassAttackModifier(iI) != 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_UNIT_ATTACK_MOD_VS_CLASS", kPromotion.getUnitClassAttackModifier(iI), GC.getUnitClassInfo((UnitClassTypes)iI).getTextKeyWide()));
			}

			if (kPromotion.getUnitClassDefenseModifier(iI) != 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_UNIT_DEFENSE_MOD_VS_CLASS", kPromotion.getUnitClassDefenseModifier(iI), GC.getUnitClassInfo((UnitClassTypes) iI).getTextKeyWide()));
			}
		}
	}

	for (iI = 0; iI < GC.getNumUnitCombatInfos(); ++iI)
	{
		if (kPromotion.getUnitCombatModifierPercent(iI) != 0)
		{
			szBuffer.append(pcNewline);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_MOD_VS_TYPE_NO_LINK", kPromotion.getUnitCombatModifierPercent(iI), GC.getUnitCombatInfo((UnitCombatTypes)iI).getTextKeyWide()));
		}
	}

	for (iI = 0; iI < NUM_DOMAIN_TYPES; ++iI)
	{
		if (kPromotion.getDomainModifierPercent(iI) != 0)
		{
			szBuffer.append(pcNewline);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_MOD_VS_TYPE_NO_LINK", kPromotion.getDomainModifierPercent(iI), GC.getDomainInfo((DomainTypes)iI).getTextKeyWide()));
		}
	}

	if (wcslen(kPromotion.getHelp()) > 0)
	{
		szBuffer.append(pcNewline);
		szBuffer.append(kPromotion.getHelp());
	}

	//WTP, ray, Promotions not earned by XP - START
	if (kPromotion.isNotEarnedByXP())
	{
		szBuffer.append(pcNewline);
		szBuffer.append(gDLL->getText("TXT_KEY_PROMOTION_CAN_NOT_BE_EARNED_BY_XP"));
	}
	//WTP, ray, Promotions not earned by XP - END
}

//	Function:			parseCivicInfo()
//	Description:	Will parse the civic info help
//	Parameters:		szHelpText -- the text to put it into
//								civicInfo - what to parse
//	Returns:			nothing
void CvGameTextMgr::parseCivicInfo(CvWStringBuffer &szHelpText, CivicTypes eCivic, bool bCivilopediaText, bool bPlayerContext, bool bSkipName)
{
	PROFILE_FUNC();

	CvWString szFirstBuffer;
	bool bFirst;
	int iLast;
	int iI, iJ;

	if (NO_CIVIC == eCivic)
	{
		return;
	}
	CvCivicInfo& kCivicInfo = GC.getCivicInfo(eCivic);

	szHelpText.clear();

	FAssert(GC.getGameINLINE().getActivePlayer() != NO_PLAYER || !bPlayerContext);

	if (!bSkipName)
	{
		szHelpText.append(kCivicInfo.getDescription());
	}
	// Special Building Not Required...
	for (iI = 0; iI < GC.getNumSpecialBuildingInfos(); ++iI)
	{
		if (kCivicInfo.isSpecialBuildingNotRequired(iI))
		{
			// XXX "Missionaries"??? - Now in XML
			szHelpText.append(NEWLINE);
			szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_BUILD_MISSIONARIES", GC.getSpecialBuildingInfo((SpecialBuildingTypes)iI).getTextKeyWide()));
		}
	}

	//	Great General Modifier...
	if (kCivicInfo.getGreatGeneralRateModifier() != 0)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_GREAT_GENERAL_MOD", kCivicInfo.getGreatGeneralRateModifier()));
	}

	if (kCivicInfo.getDomesticGreatGeneralRateModifier() != 0)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_DOMESTIC_GREAT_GENERAL_MODIFIER", kCivicInfo.getDomesticGreatGeneralRateModifier()));
	}

	//	Free Experience
	if (kCivicInfo.getFreeExperience() != 0)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_FREE_XP", kCivicInfo.getFreeExperience()));
	}

	//	Worker speed modifier
	if (kCivicInfo.getWorkerSpeedModifier() != 0)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_WORKER_SPEED", kCivicInfo.getWorkerSpeedModifier()));
	}

	//	Improvement upgrade rate modifier
	if (kCivicInfo.getImprovementUpgradeRateModifier() != 0)
	{
		bFirst = true;

		for (iI = 0; iI < GC.getNumImprovementInfos(); ++iI)
		{
			if (GC.getImprovementInfo((ImprovementTypes)iI).getImprovementUpgrade() != NO_IMPROVEMENT)
			{
				szFirstBuffer.Format(L"%s%s", NEWLINE, gDLL->getText("TXT_KEY_CIVIC_IMPROVEMENT_UPGRADE", kCivicInfo.getImprovementUpgradeRateModifier()).c_str());
				CvWString szImprovement;
				szImprovement.Format(L"<link=literal>%s</link>", GC.getImprovementInfo((ImprovementTypes)iI).getDescription());
				setListHelp(szHelpText, szFirstBuffer, szImprovement, L", ", bFirst);
				bFirst = false;
			}
		}
	}

	//	Military unit production modifier
	if (kCivicInfo.getMilitaryProductionModifier() != 0)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_MILITARY_PRODUCTION", kCivicInfo.getMilitaryProductionModifier()));
	}

	//	Experience
	if (0 != kCivicInfo.getExpInBorderModifier())
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_EXPERIENCE_IN_BORDERS", kCivicInfo.getExpInBorderModifier()));
	}

	//	Cross Conversion
	if (kCivicInfo.getImmigrationConversion() != YIELD_CROSSES && kCivicInfo.getImmigrationConversion() != NO_YIELD)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_IMMIGRATION_CONVERSION", GC.getYieldInfo(YIELD_CROSSES).getChar(), GC.getYieldInfo((YieldTypes) kCivicInfo.getImmigrationConversion()).getChar()));
	}

	// native attitude
	if (kCivicInfo.getNativeAttitudeChange() > 0)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_FATHER_NATIVE_ATTITUDE_GOOD"));
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_FATHER_NATIVE_ATTITUDE_GOOD2"));
	}
	else if (kCivicInfo.getNativeAttitudeChange() < 0)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_FATHER_NATIVE_ATTITUDE_BAD"));
	}

	// native combat
	if (kCivicInfo.getNativeCombatModifier() != 0)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_UNIT_NATIVE_COMBAT_MOD", kCivicInfo.getNativeCombatModifier()));
	}

	// native combat
	if (kCivicInfo.getFatherPointModifier() != 0)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_UNIT_FATHER_POINT_MOD", kCivicInfo.getFatherPointModifier()));
	}

	// native borders
	if (kCivicInfo.isDominateNativeBorders())
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_NATIVE_BORDERS"));
	}

	// Always trade with Europe
	if (kCivicInfo.isRevolutionEuropeTrade())
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_REVOLUTION_EUROPE_TRADE"));
	}

	//	Yield Modifiers
	setYieldChangeHelp(szHelpText, L"", L"", gDLL->getText("TXT_KEY_BUILDING_ALL_CITIES").GetCString(), kCivicInfo.getYieldModifierArray(), true);

	//	Capital Yield Modifiers
	setYieldChangeHelp(szHelpText, L"", L"", gDLL->getText("TXT_KEY_CIVIC_IN_CAPITAL").GetCString(), kCivicInfo.getCapitalYieldModifierArray(), true);

	//	Improvement Yields
	for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		iLast = 0;

		for (iJ = 0; iJ < GC.getNumImprovementInfos(); iJ++)
		{
			if (kCivicInfo.getImprovementYieldChanges(iJ, iI) != 0)
			{
				szFirstBuffer.Format(L"%s%s", NEWLINE, gDLL->getText("TXT_KEY_CIVIC_IMPROVEMENT_YIELD_CHANGE", kCivicInfo.getImprovementYieldChanges(iJ, iI), GC.getYieldInfo((YieldTypes)iI).getChar()).c_str());
				CvWString szImprovement;
				szImprovement.Format(L"<link=literal>%s</link>", GC.getImprovementInfo((ImprovementTypes)iJ).getDescription());
				setListHelp(szHelpText, szFirstBuffer, szImprovement, L", ", (kCivicInfo.getImprovementYieldChanges(iJ, iI) != iLast));
				iLast = kCivicInfo.getImprovementYieldChanges(iJ, iI);
			}
		}
	}

	//	Hurry types
	for (iI = 0; iI < GC.getNumHurryInfos(); ++iI)
	{
		if (kCivicInfo.isHurry(iI))
		{
			szHelpText.append(CvWString::format(L"%s%c%s", NEWLINE, gDLL->getSymbolID(BULLET_CHAR), GC.getHurryInfo((HurryTypes)iI).getDescription()));
		}
	}

	if (kCivicInfo.getNumFreeUnitClasses() > 0)
	{
		szHelpText.append(NEWLINE);
		szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_EXTRA_POPULATION", kCivicInfo.getNumFreeUnitClasses()));
	}

	for (int iProfession = 0; iProfession < GC.getNumProfessionInfos(); ++iProfession)
	{
		if (kCivicInfo.getProfessionCombatChange(iProfession) != 0)
		{
			CvProfessionInfo& kProfession = GC.getProfessionInfo((ProfessionTypes) iProfession);
			szHelpText.append(NEWLINE);
			szHelpText.append(gDLL->getText("TXT_KEY_CIVIC_PROFESSION_EXTRA_STRENGTH", kCivicInfo.getProfessionCombatChange(iProfession), kProfession.getTextKeyWide()));
		}
	}

	if (!isEmpty(kCivicInfo.getHelp()))
	{
		szHelpText.append(CvWString::format(L"%s%s", NEWLINE, kCivicInfo.getHelp()).c_str());
	}
}

void CvGameTextMgr::setBasicUnitHelp(CvWStringBuffer &szBuffer, UnitTypes eUnit, bool bCivilopediaText)
{
	PROFILE_FUNC();

	CvWString szTempBuffer;
	bool bFirst;
	int iCount;
	int iI;

	if (NO_UNIT == eUnit)
	{
		return;
	}

	CvUnitInfo& kUnitInfo = GC.getUnitInfo(eUnit);
	int unitClassIntToBeChecked = kUnitInfo.getUnitClassType();

	if (!bCivilopediaText)
	{
		szBuffer.append(NEWLINE);
		if (kUnitInfo.getCombat() > 0)
		{
			szTempBuffer.Format(L"%d%c, ", kUnitInfo.getCombat(), gDLL->getSymbolID(STRENGTH_CHAR));
			szBuffer.append(szTempBuffer);
		}
		szTempBuffer.Format(L"%d%c", kUnitInfo.getMoves(), gDLL->getSymbolID(MOVES_CHAR));
		szBuffer.append(szTempBuffer);
	}

	if (kUnitInfo.getLeaderExperience() > 0)
	{
		if (0 == GC.getDefineINT("WARLORD_EXTRA_EXPERIENCE_PER_UNIT_PERCENT"))
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_LEADER", kUnitInfo.getLeaderExperience()));
		}
		else
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_LEADER_EXPERIENCE", kUnitInfo.getLeaderExperience()));
		}
	}

	if (NO_PROMOTION != kUnitInfo.getLeaderPromotion())
	{
		szBuffer.append(CvWString::format(L"%s%c%s", NEWLINE, gDLL->getSymbolID(BULLET_CHAR), gDLL->getText("TXT_KEY_PROMOTION_WHEN_LEADING").GetCString()));
		parsePromotionHelp(szBuffer, (PromotionTypes)kUnitInfo.getLeaderPromotion(), L"\n   ");
	}

	if (kUnitInfo.getCargoSpace() > 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CARGO_SPACE", kUnitInfo.getCargoSpace()));
		//TAC Whaling, ray
		if (kUnitInfo.getSpecialCargo() != NO_SPECIALUNIT && !kUnitInfo.isGatherBoat())
		{
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CARRIES", GC.getSpecialUnitInfo((SpecialUnitTypes) kUnitInfo.getSpecialCargo()).getTextKeyWide()));
		}
		//End TAC Whaling, ray

	}

	if (kUnitInfo.getRequiredTransportSize() > 1)
	{
		for (int i = 0; i < GC.getNumUnitInfos(); ++i)
		{
			CvUnitInfo& kTransportUnitInfo = GC.getUnitInfo((UnitTypes) i);
			if (kTransportUnitInfo.getCargoSpace() >= kUnitInfo.getRequiredTransportSize())
			{
			    if (kTransportUnitInfo.getSpecialCargo() == NO_SPECIALUNIT || kUnitInfo.getSpecialCargo() == kTransportUnitInfo.getSpecialCargo())
			    {
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CARGO", kTransportUnitInfo.getTextKeyWide()));
			    }
			}
		}
	}

	szTempBuffer.Format(L"%s%s ", NEWLINE, gDLL->getText("TXT_KEY_UNIT_CANNOT_ENTER").GetCString());

	bFirst = true;
	for (iI = 0; iI < GC.getNumTerrainInfos(); ++iI)
	{
		if (kUnitInfo.getTerrainImpassable(iI))
		{
			CvWString szTerrain;
			szTerrain.Format(L"<link=literal>%s</link>", GC.getTerrainInfo((TerrainTypes)iI).getDescription());
			setListHelp(szBuffer, szTempBuffer, szTerrain, L", ", bFirst);
			bFirst = false;
		}
	}

	for (iI = 0; iI < GC.getNumFeatureInfos(); ++iI)
	{
		if (kUnitInfo.getFeatureImpassable(iI))
		{
			CvWString szFeature;
			szFeature.Format(L"<link=literal>%s</link>", GC.getFeatureInfo((FeatureTypes)iI).getDescription());
			setListHelp(szBuffer, szTempBuffer, szFeature, L", ", bFirst);
			bFirst = false;
		}
	}

	// < JAnimals Mod Start >
	if (kUnitInfo.isAnimal())
	{
	    szBuffer.append(NEWLINE);
        szBuffer.append(gDLL->getText("TXT_KEY_UNIT_ANIMAL"));

		szBuffer.append(NEWLINE);
        szBuffer.append(gDLL->getText("TXT_KEY_UNIT_ANIMAL_CANNOT_ENTER_PLAYER_TERRITORY"));

		szBuffer.append(NEWLINE);
        szBuffer.append(gDLL->getText("TXT_KEY_UNIT_ANIMAL_KILLING_REWARD"));

		for (iI = 0; iI < GC.getNumTerrainInfos(); ++iI)
		{
			if (kUnitInfo.getTerrainNative(iI))
			{
				szBuffer.append(NEWLINE);
			 szBuffer.append(gDLL->getText("TXT_KEY_UNIT_TERRAIN_NATIVE", GC.getTerrainInfo((TerrainTypes) iI).getDescription()));
			}
		}      
	}
	// < JAnimals Mod End >

	if (kUnitInfo.isInvisible())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_INVISIBLE_ALL"));
	}
	// R&R, ray, Rangers
	else if (kUnitInfo.getInvisibleType() == INVISIBLE_RANGER)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_INVISIBLE_RANGER"));
	}
	// R&R, ray, Rangers, END
	else if (kUnitInfo.getInvisibleType() != NO_INVISIBLE)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_INVISIBLE_MOST"));
	}
	for (iI = 0; iI < kUnitInfo.getNumSeeInvisibleTypes(); ++iI)
	{
		if (bCivilopediaText || (kUnitInfo.getSeeInvisibleType(iI) != kUnitInfo.getInvisibleType()))
		{
			if(kUnitInfo.getSeeInvisibleType(iI) == INVISIBLE_RANGER)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_SEE_RANGER"));
			}
			else
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_UNIT_SEE_INVISIBLE", GC.getInvisibleInfo((InvisibleTypes) kUnitInfo.getSeeInvisibleType(iI)).getTextKeyWide()));
			}
		}
	}
	if (kUnitInfo.isCanMoveImpassable())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CAN_MOVE_IMPASSABLE"));
	}

	/// Move Into Peak - start - Nightinggale
	if (kUnitInfo.allowsMoveIntoPeak())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CAN_MOVE_PEAK"));
	}
	/// Move Into Peak - end - Nightinggale

	if (kUnitInfo.isNoBadGoodies())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_NO_BAD_GOODIES"));
	}
	if (kUnitInfo.isHiddenNationality())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_HIDDEN_NATIONALITY"));
	}
	if (kUnitInfo.isAlwaysHostile())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_ALWAYS_HOSTILE"));
	}
	if (kUnitInfo.isCapturesCargo())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CAPTURES_CARGO"));
	}
	//TAC Whaling, ray
	if (unitClassIntToBeChecked == GC.getUNITCLASS_WHALING_BOAT())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_GATHER_BOAT_WHALING"));
	}
	//End TAC Whaling, ray
	// R&R, ray, High Sea Fishing - START
	if (unitClassIntToBeChecked == GC.getUNITCLASS_FISHING_BOAT())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_GATHER_BOAT_FISHING"));
	}
	// R&R, ray, High Sea Fishing - END
	if (kUnitInfo.isTreasure())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_TREASURE_HELP"));
	}
	// R&R, ray, Changes for Treasures, START
	if (kUnitInfo.isNoRevealMap())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_NO_REVEAL_MAP"));
	}

	if (kUnitInfo.isTreasure())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_NO_REVEAL_GOODY"));
	}
	// R&R, ray, Changes for Treasures, END

	if (kUnitInfo.isOnlyDefensive())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_ONLY_DEFENSIVE"));
	}
	if (kUnitInfo.isNoCapture())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CANNOT_CAPTURE"));
	}
	if (kUnitInfo.isRivalTerritory())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_EXPLORE_RIVAL"));
	}

	// R&R, ray, Criminals, African Slaves and Naitve Slaves cannot found cities - ONLY for Human Players - START
	if (unitClassIntToBeChecked == GC.getDefineINT("UNITCLASS_PRISONER") || unitClassIntToBeChecked == GC.getDefineINT("UNITCLASS_NATIVE_SLAVE") || unitClassIntToBeChecked == GC.getDefineINT("UNITCLASS_AFRICAN_SLAVE"))
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_NOT_FOUND_CITY"));
	}
	// R&R, ray, Criminals, African Slaves and Naitve Slaves cannot found cities - ONLY for Human Players - END

	else if (kUnitInfo.isFound())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_FOUND_CITY"));
	}

	// R&R, ray, Start Logic for Trains
	if (kUnitInfo.getDomainType() == DOMAIN_LAND && kUnitInfo.getCargoSpace()== 6)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_TRAVEL_RAILROAD_ONLY"));
	}
	// R&R, ray, END Logic for Trains

	if (kUnitInfo.getWorkRate() > 0)
	{
		iCount = 0;
		for (iI = 0; iI < GC.getNumBuildInfos(); ++iI)
		{
			if (kUnitInfo.getBuilds(iI))
			{
				iCount++;
			}
		}
		if (iCount > 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_IMPROVE_PLOTS"));
		}
		else
		{
			bFirst = true;
			for (iI = 0; iI < GC.getNumBuildInfos(); ++iI)
			{
				if (kUnitInfo.getBuilds(iI))
				{
					szTempBuffer.Format(L"%s%s ", NEWLINE, gDLL->getText("TXT_KEY_UNIT_CAN").c_str());
					setListHelp(szBuffer, szTempBuffer, GC.getBuildInfo((BuildTypes) iI).getDescription(), L", ", bFirst);
					bFirst = false;
				}
			}
		}
	}
	if (kUnitInfo.getWorkRateModifier() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_FASTER_WORK_RATE", kUnitInfo.getWorkRateModifier()));
	}
	if (kUnitInfo.getMissionaryRateModifier() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_BETTER_MISSION_RATE", kUnitInfo.getMissionaryRateModifier()));
	}
	// WTP, ray, Native Trade Posts - START
	if (kUnitInfo.getNativeTradeRateModifier() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_BETTER_TRADE_POST_RATE", kUnitInfo.getNativeTradeRateModifier()));
	}
	// WTP, ray, Native Trade Posts - END
	if (kUnitInfo.isNoDefensiveBonus())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_NO_DEFENSE_BONUSES"));
	}
	if (kUnitInfo.isFlatMovementCost())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_FLAT_MOVEMENT"));
	}
	if (kUnitInfo.isIgnoreTerrainCost())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_IGNORE_TERRAIN"));
	}
	if (kUnitInfo.getWithdrawalProbability() > 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_WITHDRAWL_PROBABILITY", kUnitInfo.getWithdrawalProbability()));
	}
	szTempBuffer.clear();
	for (int i = 0; i < GC.getNumBuildingClassInfos(); ++i)
	{
		if (kUnitInfo.isEvasionBuilding(i))
		{
			if (!szTempBuffer.empty())
			{
				szTempBuffer += gDLL->getText("TXT_KEY_OR");
			}
			szTempBuffer += GC.getBuildingClassInfo((BuildingClassTypes) i).getDescription();
		}
	}
	if (!szTempBuffer.empty())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_EVASION_BUILDINGS", szTempBuffer.GetCString()));
	}
	if (kUnitInfo.getCityAttackModifier() == kUnitInfo.getCityDefenseModifier())
	{
		if (kUnitInfo.getCityAttackModifier() != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CITY_STRENGTH_MOD", kUnitInfo.getCityAttackModifier()));
		}
	}
	else
	{
		if (kUnitInfo.getCityAttackModifier() != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CITY_ATTACK_MOD", kUnitInfo.getCityAttackModifier()));
		}

		if (kUnitInfo.getCityDefenseModifier() != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_CITY_DEFENSE_MOD", kUnitInfo.getCityDefenseModifier()));
		}
	}

	if (kUnitInfo.getHillsDefenseModifier() == kUnitInfo.getHillsAttackModifier())
	{
		if (kUnitInfo.getHillsAttackModifier() != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_HILLS_STRENGTH", kUnitInfo.getHillsAttackModifier()));
		}
	}
	else
	{
		if (kUnitInfo.getHillsAttackModifier() != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_HILLS_ATTACK", kUnitInfo.getHillsAttackModifier()));
		}

		if (kUnitInfo.getHillsDefenseModifier() != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_HILLS_DEFENSE", kUnitInfo.getHillsDefenseModifier()));
		}
	}

	if (kUnitInfo.getDomesticBonusModifier() != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_DOMESTIC_BONUS", kUnitInfo.getDomesticBonusModifier()));			
		}

	for (iI = 0; iI < GC.getNumTerrainInfos(); ++iI)
	{
		if (kUnitInfo.getTerrainDefenseModifier(iI) != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_DEFENSE", kUnitInfo.getTerrainDefenseModifier(iI), GC.getTerrainInfo((TerrainTypes) iI).getTextKeyWide()));
		}

		if (kUnitInfo.getTerrainAttackModifier(iI) != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_ATTACK", kUnitInfo.getTerrainAttackModifier(iI), GC.getTerrainInfo((TerrainTypes) iI).getTextKeyWide()));
		}
	}

	for (iI = 0; iI < GC.getNumFeatureInfos(); ++iI)
	{
		if (kUnitInfo.getFeatureDefenseModifier(iI) != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_DEFENSE", kUnitInfo.getFeatureDefenseModifier(iI), GC.getFeatureInfo((FeatureTypes) iI).getTextKeyWide()));
		}

		if (kUnitInfo.getFeatureAttackModifier(iI) != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_ATTACK", kUnitInfo.getFeatureAttackModifier(iI), GC.getFeatureInfo((FeatureTypes) iI).getTextKeyWide()));
		}
	}

	for (iI = 0; iI < GC.getNumUnitClassInfos(); ++iI)
	{
		if (kUnitInfo.getUnitClassAttackModifier(iI) == kUnitInfo.getUnitClassDefenseModifier(iI))
		{
			if (kUnitInfo.getUnitClassAttackModifier(iI) != 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_UNIT_MOD_VS_TYPE", kUnitInfo.getUnitClassAttackModifier(iI), GC.getUnitClassInfo((UnitClassTypes)iI).getTextKeyWide()));
			}
		}
		else
		{
			if (kUnitInfo.getUnitClassAttackModifier(iI) != 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_UNIT_ATTACK_MOD_VS_CLASS", kUnitInfo.getUnitClassAttackModifier(iI), GC.getUnitClassInfo((UnitClassTypes)iI).getTextKeyWide()));
			}

			if (kUnitInfo.getUnitClassDefenseModifier(iI) != 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_UNIT_DEFENSE_MOD_VS_CLASS", kUnitInfo.getUnitClassDefenseModifier(iI), GC.getUnitClassInfo((UnitClassTypes) iI).getTextKeyWide()));
			}
		}
	}

	for (iI = 0; iI < GC.getNumUnitCombatInfos(); ++iI)
	{
		if (kUnitInfo.getUnitCombatModifier(iI) != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_MOD_VS_TYPE_NO_LINK", kUnitInfo.getUnitCombatModifier(iI), GC.getUnitCombatInfo((UnitCombatTypes) iI).getTextKeyWide()));
		}
	}

	for (iI = 0; iI < NUM_DOMAIN_TYPES; ++iI)
	{
		if (kUnitInfo.getDomainModifier(iI) != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_MOD_VS_TYPE_NO_LINK", kUnitInfo.getDomainModifier(iI), GC.getDomainInfo((DomainTypes)iI).getTextKeyWide()));
		}
	}

	if (kUnitInfo.getBombardRate() > 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_BOMBARD_RATE", ((kUnitInfo.getBombardRate() * 100) / GC.getMAX_CITY_DEFENSE_DAMAGE())));
	}

	bFirst = true;

	for (iI = 0; iI < GC.getNumPromotionInfos(); ++iI)
	{
		if (kUnitInfo.getFreePromotions(iI))
		{
			szTempBuffer.Format(L"%s%s", NEWLINE, gDLL->getText("TXT_KEY_UNIT_STARTS_WITH").c_str());
			setListHelp(szBuffer, szTempBuffer, CvWString::format(L"<link=literal>%s</link>", GC.getPromotionInfo((PromotionTypes) iI).getDescription()), L", ", bFirst);
			bFirst = false;
		}
	}

	if (bCivilopediaText)
	{
		// Trait
		for (int i = 0; i < GC.getNumTraitInfos(); ++i)
		{
			if (kUnitInfo.getProductionTraits((TraitTypes)i) != 0)
			{
				if (kUnitInfo.getProductionTraits((TraitTypes)i) == 100)
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_DOUBLE_SPEED_TRAIT", GC.getTraitInfo((TraitTypes)i).getTextKeyWide()));
				}
				else
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_PRODUCTION_MODIFIER_TRAIT", kUnitInfo.getProductionTraits((TraitTypes)i), GC.getTraitInfo((TraitTypes)i).getTextKeyWide()));
				}
			}
		}
	}

	if (!isEmpty(kUnitInfo.getHelp()))
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(kUnitInfo.getHelp());
	}
}

void CvGameTextMgr::setUnitHelp(CvWStringBuffer &szBuffer, UnitTypes eUnit, bool bCivilopediaText, bool bStrategyText, CvCity* pCity)
{
	PROFILE_FUNC();

	CvWString szTempBuffer;
	PlayerTypes ePlayer;
	int iProduction;
	int iI;

	if (NO_UNIT == eUnit)
	{
		return;
	}

	if (pCity != NULL)
	{
		ePlayer = pCity->getOwnerINLINE();
	}
	else
	{
		ePlayer = GC.getGameINLINE().getActivePlayer();
	}

	if (!bCivilopediaText)
	{
		szTempBuffer.Format(SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_UNIT_TEXT"), GC.getUnitInfo(eUnit).getDescription());
		szBuffer.append(szTempBuffer);

		if (GC.getUnitInfo(eUnit).getUnitCombatType() != NO_UNITCOMBAT)
		{
			szTempBuffer.Format(L" (%s)", GC.getUnitCombatInfo((UnitCombatTypes) GC.getUnitInfo(eUnit).getUnitCombatType()).getDescription());
			szBuffer.append(szTempBuffer);
		}
	}

	// test for unique unit
	UnitClassTypes eUnitClass = (UnitClassTypes)GC.getUnitInfo(eUnit).getUnitClassType();
	UnitTypes eDefaultUnit = (UnitTypes)GC.getUnitClassInfo(eUnitClass).getDefaultUnitIndex();

	if (NO_UNIT != eDefaultUnit && eDefaultUnit != eUnit)
	{
		for (iI  = 0; iI < GC.getNumCivilizationInfos(); ++iI)
		{
			UnitTypes eUniqueUnit = (UnitTypes)GC.getCivilizationInfo((CivilizationTypes)iI).getCivilizationUnits((int)eUnitClass);
			if (eUniqueUnit == eUnit)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_UNIQUE_UNIT", GC.getCivilizationInfo((CivilizationTypes)iI).getTextKeyWide()));
			}
		}

		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_REPLACES_UNIT", GC.getUnitInfo(eDefaultUnit).getTextKeyWide()));
	}

	setBasicUnitHelp(szBuffer, eUnit, bCivilopediaText);

	std::map<int, CvWString> mapModifiers;
	std::map<int, CvWString> mapChanges;
	std::map<int, CvWString> mapBonus;
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		YieldTypes eYield = (YieldTypes) iYield;

		int iModifier = GC.getUnitInfo(eUnit).getYieldModifier(eYield);
		if (iModifier != 0)
		{
			mapModifiers[iModifier] += CvWString::format(L"%c", GC.getYieldInfo(eYield).getChar());
		}

		int iChange = GC.getUnitInfo(eUnit).getYieldChange(eYield);
		if (iChange != 0)
		{
			mapChanges[iChange] += CvWString::format(L"%c", GC.getYieldInfo(eYield).getChar());
		}

		iChange = GC.getUnitInfo(eUnit).getBonusYieldChange(eYield);
		if (iChange != 0)
		{
			mapBonus[iChange] += CvWString::format(L"%c", GC.getYieldInfo(eYield).getChar());
		}
	}

	if (GC.getUnitInfo(eUnit).isLandYieldChanges() && GC.getUnitInfo(eUnit).isWaterYieldChanges())
	{
		for (std::map<int, CvWString>::iterator it = mapModifiers.begin(); it != mapModifiers.end(); ++it)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_YIELD_MODIFIER", it->first, it->second.GetCString()));
		}

		for (std::map<int, CvWString>::iterator it = mapChanges.begin(); it != mapChanges.end(); ++it)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_YIELD_CHANGE", it->first, it->second.GetCString()));
		}
	}
	else if (GC.getUnitInfo(eUnit).isLandYieldChanges())
	{
		for (std::map<int, CvWString>::iterator it = mapModifiers.begin(); it != mapModifiers.end(); ++it)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_YIELD_MODIFIER_LAND", it->first, it->second.GetCString()));
		}

		for (std::map<int, CvWString>::iterator it = mapChanges.begin(); it != mapChanges.end(); ++it)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_YIELD_CHANGE_LAND", it->first, it->second.GetCString()));
		}
	}
	else if (GC.getUnitInfo(eUnit).isWaterYieldChanges())
	{
		for (std::map<int, CvWString>::iterator it = mapModifiers.begin(); it != mapModifiers.end(); ++it)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_YIELD_MODIFIER_WATER", it->first, it->second.GetCString()));
		}

		for (std::map<int, CvWString>::iterator it = mapChanges.begin(); it != mapChanges.end(); ++it)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_YIELD_CHANGE_WATER", it->first, it->second.GetCString()));
		}
	}

	if (GC.getUnitInfo(eUnit).isLandYieldChanges() || GC.getUnitInfo(eUnit).isWaterYieldChanges())
	{
		for (std::map<int, CvWString>::iterator it = mapBonus.begin(); it != mapBonus.end(); ++it)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_BONUS_YIELD_CHANGE", it->first, it->second.GetCString()));
		}
	}
	// R&R, Androrc, Domestic Market
	// R&R, ray, adjustment Domestic Markets, displaying as list
	CvWString szYieldsDemandedList;
	const InfoArray<YieldTypes, IntTypes> &infoYieldDemands = GC.getUnitInfo(eUnit).getYieldDemands();
	for (int iI = 0; iI < infoYieldDemands.getLength(); ++iI)
	{
		szYieldsDemandedList += CvWString::format(L"%c", GC.getYieldInfo(infoYieldDemands.getYield(iI)).getChar());
	}
	if(!isEmpty(szYieldsDemandedList))
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getSymbolID(BULLET_CHAR));
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_YIELD_DEMAND", szYieldsDemandedList.GetCString()));
	}
	//Androrc End

	if ((pCity == NULL) || !(pCity->canTrain(eUnit)))
	{
		if (GC.getUnitInfo(eUnit).getPrereqBuilding() != NO_BUILDINGCLASS)
		{
			BuildingTypes eBuilding;
			if (ePlayer == NO_PLAYER)
			{
				eBuilding = (BuildingTypes) GC.getBuildingClassInfo((BuildingClassTypes) GC.getUnitInfo(eUnit).getPrereqBuilding()).getDefaultBuildingIndex();
			}
			else
			{
				eBuilding = (BuildingTypes) GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getCivilizationBuildings(GC.getUnitInfo(eUnit).getPrereqBuilding());
			}
			if(eBuilding != NO_BUILDING)
			{
				if ((pCity == NULL) || (!pCity->isHasConceptualBuilding(eBuilding)))
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_UNIT_REQUIRES_STRING", GC.getBuildingInfo(eBuilding).getTextKeyWide()));
				}
			}
		}

		bool bValid = true;
		bool bFirst = true;
		szTempBuffer.clear();
		for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
		{
			if (GC.getUnitInfo(eUnit).isPrereqOrBuilding(iBuildingClass))
			{
				bValid = false;
				BuildingTypes eBuilding = (BuildingTypes) GC.getBuildingClassInfo((BuildingClassTypes) iBuildingClass).getDefaultBuildingIndex();
				if (ePlayer != NO_PLAYER)
				{
					eBuilding = (BuildingTypes) GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getCivilizationBuildings(iBuildingClass);
				}

				if (NO_BUILDING != eBuilding)
				{
					if (pCity != NULL && pCity->isHasConceptualBuilding(eBuilding) )
					{
						bValid = true;
						break;
					}

					if (!bFirst)
					{
						szTempBuffer += gDLL->getText("TXT_KEY_OR");
					}
					szTempBuffer += GC.getBuildingInfo(eBuilding).getDescription();
					bFirst = false;
				}
			}
		}

		if (!bValid)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_REQUIRES_STRING", szTempBuffer.GetCString()));
		}


		if (!bCivilopediaText)
		{
			for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
			{
				if (GC.getUnitInfo(eUnit).getYieldCost(eYield) > 0)
				{
					if (GC.getYieldInfo(eYield).isCargo())
					{
						int iCost = (ePlayer == NO_PLAYER ? GC.getUnitInfo(eUnit).getYieldCost(eYield) : GET_PLAYER(ePlayer).getYieldProductionNeeded(eUnit, eYield));
						if (NULL == pCity || pCity->getYieldStored(eYield) + pCity->getYieldRushed(eYield) < iCost)
						{
							szBuffer.append(NEWLINE);
							szBuffer.append(gDLL->getText("TXT_KEY_BUILD_CANNOT_AFFORD", iCost, GC.getYieldInfo(eYield).getChar()));
						}
					}
				}
			}
		}
	}

	if (!bCivilopediaText && GC.getGameINLINE().getActivePlayer() != NO_PLAYER)
	{
		if (pCity == NULL)
		{
			int iCost = GET_PLAYER(ePlayer).getYieldProductionNeeded(eUnit, YIELD_HAMMERS);
			if (iCost > 0)
			{
				szTempBuffer.Format(L"%s%d%c", NEWLINE, iCost, GC.getYieldInfo(YIELD_HAMMERS).getChar());
				szBuffer.append(szTempBuffer);
			}
		}
		else
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_TURNS", pCity->getProductionTurnsLeft(eUnit, ((gDLL->ctrlKey() || !(gDLL->shiftKey())) ? 0 : pCity->getOrderQueueLength())), pCity->getYieldProductionNeeded(eUnit, YIELD_HAMMERS), GC.getYieldInfo(YIELD_HAMMERS).getChar()));

			iProduction = pCity->getUnitProduction(eUnit);

			if (iProduction > 0)
			{
				szTempBuffer.Format(L" - %d/%d%c", iProduction, pCity->getYieldProductionNeeded(eUnit, YIELD_HAMMERS), GC.getYieldInfo(YIELD_HAMMERS).getChar());
				szBuffer.append(szTempBuffer);
			}
			else
			{
				szTempBuffer.Format(L" - %d%c", pCity->getYieldProductionNeeded(eUnit, YIELD_HAMMERS), GC.getYieldInfo(YIELD_HAMMERS).getChar());
				szBuffer.append(szTempBuffer);
			}

			for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
			{
				if (GC.getUnitInfo(eUnit).getYieldCost(eYield) > 0)
				{
					if (GC.getYieldInfo(eYield).isCargo())
					{
						int iCost = GET_PLAYER(pCity->getOwnerINLINE()).getYieldProductionNeeded(eUnit, eYield);
						if (iCost > 0)
						{
							szTempBuffer.Format(L" - %d/%d%c", pCity->getYieldStored(eYield) + pCity->getYieldRushed(eYield), iCost, GC.getYieldInfo(eYield).getChar());
							szBuffer.append(szTempBuffer);
						}
					}
				}
			}
		}
	}

	if (bStrategyText)
	{
		if (!isEmpty(GC.getUnitInfo(eUnit).getStrategy()))
		{
			if ((ePlayer == NO_PLAYER) || GET_PLAYER(ePlayer).isOption(PLAYEROPTION_ADVISOR_HELP))
			{
				szBuffer.append(SEPARATOR);
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_SIDS_TIPS"));
				szBuffer.append(L'\"');
				szBuffer.append(GC.getUnitInfo(eUnit).getStrategy());
				szBuffer.append(L'\"');
			}
		}
	}

	if (bCivilopediaText)
	{
		if(NO_UNIT != eDefaultUnit && eDefaultUnit == eUnit)
		{
			for(int iI = 0; iI < GC.getNumUnitInfos(); ++iI)
			{
				if(((UnitTypes)iI) == eUnit)
				{
					continue;
				}

				if(eUnitClass == ((UnitClassTypes)GC.getUnitInfo((UnitTypes)iI).getUnitClassType()))
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_REPLACED_BY_UNIT", GC.getUnitInfo((UnitTypes)iI).getTextKeyWide()));
				}
			}
		}
	}

	if (pCity != NULL)
	{
		if ((gDLL->getChtLvl() > 0) && gDLL->ctrlKey())
		{
			szBuffer.append(NEWLINE);
			for (int iUnitAI = 0; iUnitAI < NUM_UNITAI_TYPES; iUnitAI++)
			{
				int iTempValue = GET_PLAYER(pCity->getOwner()).AI_unitValue(eUnit, (UnitAITypes)iUnitAI, pCity->area());
				if (iTempValue != 0)
				{
					CvWString szTempString;
					getUnitAIString(szTempString, (UnitAITypes)iUnitAI);
					szBuffer.append(CvWString::format(L"(%s : %d) ", szTempString.GetCString(), iTempValue));
				}
			}
		}
	}
}
void CvGameTextMgr::setBuildingHelp(CvWStringBuffer &szBuffer, BuildingTypes eBuilding, bool bCivilopediaText, bool bStrategyText, CvCity* pCity)
{
	PROFILE_FUNC();

	CvWString szFirstBuffer;
	CvWString szTempBuffer;
	BuildingTypes eLoopBuilding;
	PlayerTypes ePlayer;
	bool bFirst;
	int iProduction;
	int iLast;
	int iI;

	if (NO_BUILDING == eBuilding)
	{
		return;
	}

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);


	if (pCity != NULL)
	{
		ePlayer = pCity->getOwnerINLINE();
	}
	else
	{
		ePlayer = GC.getGameINLINE().getActivePlayer();
	}

	if (!bCivilopediaText)
	{
		// R&R, ray , MYCP partially based on code of Aymerick - START
		szTempBuffer.Format( SETCOLR L"<link=literal>%s</link>" ENDCOLR , TEXT_COLOR("COLOR_BUILDING_TEXT"), kBuilding.getDescription());
		szBuffer.append(szTempBuffer);
		// R&R, ray , fix conflict MYCP and MYPB
		// std::vector<YieldTypes> eBuildingYieldsConversion;
		if(kBuilding.getProfessionOutput() != 0)
		{
			for (iI = 0; iI < GC.getNumProfessionInfos(); ++iI)
			{
				// R&R, ray , fix conflict MYCP and MYPB
				std::vector<YieldTypes> eBuildingYieldsConversion;

				if (ePlayer == NO_PLAYER || GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).isValidProfession(iI))
				{
					CvProfessionInfo& kProfession = GC.getProfessionInfo((ProfessionTypes) iI);
					if (kProfession.getSpecialBuilding() == kBuilding.getSpecialBuildingType())
					{
						if (kProfession.getYieldsProduced(0) != NO_YIELD)
						{
							for (int j = 0; j < kProfession.getNumYieldsConsumed(); j++)
							{
								YieldTypes eYieldConsumed = (YieldTypes) kProfession.getYieldsConsumed(j);
								if (eYieldConsumed != NO_YIELD)
								{
									eBuildingYieldsConversion.push_back((YieldTypes) eYieldConsumed);
								}
							}
							if (!eBuildingYieldsConversion.empty())
							{
								CvWString szYieldsList;
								for (std::vector<YieldTypes>::iterator it = eBuildingYieldsConversion.begin(); it != eBuildingYieldsConversion.end(); ++it)
								{
									if (!szYieldsList.empty())
									{
										if (*it == eBuildingYieldsConversion.back())
										{
											szYieldsList += CvWString::format(gDLL->getText("TXT_KEY_AND"));
										}
										else
										{
											szYieldsList += L", ";
										}
									}
									szYieldsList += CvWString::format(L"%c", GC.getYieldInfo(*it).getChar());
								}
								szBuffer.append(NEWLINE);
								szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_YIELDS_CONVERSION", szYieldsList.GetCString(), GC.getYieldInfo((YieldTypes) kProfession.getYieldsProduced(0)).getChar()));
							}
							szBuffer.append(NEWLINE);
							szBuffer.append(gDLL->getSymbolID(BULLET_CHAR));
							szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_PROFESSION_OUTPUT", kBuilding.getProfessionOutput(), GC.getYieldInfo((YieldTypes) kProfession.getYieldsProduced(0)).getChar()));
						}
					}
				}
			}
		}
		// R&R, ray , MYCP partially based on code of Aymerick - END
		int aiYields[NUM_YIELD_TYPES];
		for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
		{
			aiYields[iI] = kBuilding.getYieldChange(iI);

			if (NULL != pCity)
			{
				aiYields[iI] += pCity->getBuildingYieldChange((BuildingClassTypes)kBuilding.getBuildingClassType(), (YieldTypes)iI);
			}

			if (ePlayer != NO_PLAYER)
			{
				aiYields[iI] += GET_PLAYER(ePlayer).getBuildingYieldChange((BuildingClassTypes)kBuilding.getBuildingClassType(), (YieldTypes)iI);
			}
		}
		setYieldChangeHelp(szBuffer, L", ", L"", L"", aiYields, false, false);
		setYieldChangeHelp(szBuffer, L", ", L"", L"", kBuilding.getYieldModifierArray(), true, bCivilopediaText);
	}

	// test for unique building
	BuildingClassTypes eBuildingClass = (BuildingClassTypes)kBuilding.getBuildingClassType();
	BuildingTypes eDefaultBuilding = (BuildingTypes)GC.getBuildingClassInfo(eBuildingClass).getDefaultBuildingIndex();

	if (NO_BUILDING != eDefaultBuilding && eDefaultBuilding != eBuilding)
	{
		for (int iI  = 0; iI < GC.getNumCivilizationInfos(); ++iI)
		{
			BuildingTypes eUniqueBuilding = (BuildingTypes)GC.getCivilizationInfo((CivilizationTypes)iI).getCivilizationBuildings((int)eBuildingClass);
			if (eUniqueBuilding == eBuilding)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_UNIQUE_BUILDING", GC.getCivilizationInfo((CivilizationTypes)iI).getTextKeyWide()));
			}
		}

		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_REPLACES_UNIT", GC.getBuildingInfo(eDefaultBuilding).getTextKeyWide()));
	}

	BuildingTypes eNextBuilding = (BuildingTypes) kBuilding.getIndexOf_NextBuildingType_In_SpecialBuilding();
	while (eNextBuilding != eBuilding)
	{
		CvBuildingInfo& kNextBuilding = GC.getBuildingInfo(eNextBuilding);

		if (kBuilding.getSpecialBuildingPriority() > kNextBuilding.getSpecialBuildingPriority())
		{
			szBuffer.append(NEWLINE);
			// TAC - Messages - Ray - START
			//szBuffer.append(gDLL->getSymbolID(BULLET_CHAR));
			szBuffer.append(gDLL->getText("TXT_KEY_REPLACES_UNIT", kNextBuilding.getTextKeyWide()));
			// TAC - Messages - Ray - END
		}

		eNextBuilding = (BuildingTypes) kNextBuilding.getIndexOf_NextBuildingType_In_SpecialBuilding();
	}

	if (kBuilding.getFreePromotion() != NO_PROMOTION)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_FREE_PROMOTION", GC.getPromotionInfo((PromotionTypes)(kBuilding.getFreePromotion())).getTextKeyWide()));
	}

	if (kBuilding.isCapital())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_CAPITAL"));
	}

	if (kBuilding.isNationalWonder()) // R&R, ray, National Wonders
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_NATIONAL_WONDER"));
	}

	if (kBuilding.getSpecialBuildingType() ==(SpecialBuildingTypes) GC.getDefineINT("SPECIALBUILDING_TAVERN"))
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_ENTERTAINMENT_BUILDING", GC.getYieldInfo(YIELD_CULTURE).getChar()));
		if (kBuilding.getSpecialBuildingPriority() > 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_ENTERTAINMENT_BUILDING_INCREASED", GC.getYieldInfo(YIELD_CULTURE).getChar()));
		}
	}

	CvWString szYieldsDemandedList;
	const InfoArray<YieldTypes, IntTypes> &infoYieldDemands = kBuilding.getYieldDemands();
	for (int iI = 0; iI < infoYieldDemands.getLength(); ++iI)
	{
		szYieldsDemandedList += CvWString::format(L"%c", GC.getYieldInfo(infoYieldDemands.getYield(iI)).getChar());
	}
	if (!isEmpty(szYieldsDemandedList))
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getSymbolID(BULLET_CHAR));
		szBuffer.append(gDLL->getText("TXT_KEY_UNIT_YIELD_DEMAND", szYieldsDemandedList.GetCString()));
	}

	if (kBuilding.getDomesticMarketModifier() > 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_DOMESTIC_MARKET_BUILDING"));

		int iIncrease = kBuilding.getDomesticMarketModifier();
		if (kBuilding.getSpecialBuildingType() == getIndexForType(JIT_ARRAY_BUILDING_SPECIAL, "SPECIALBUILDING_TRADE"))
		{
			// marketplaces enables trading by adding the base 100. However we want the bonus in addition to the 100%
			iIncrease -= 100;
		}
		if (iIncrease > 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_DOMESTIC_MARKET_BUILDING_INCREASED", kBuilding.getDomesticMarketModifier()));
		}
	}

	int iYieldStorage = kBuilding.getYieldStorage();
	if (iYieldStorage > 0)
	{
		if (ePlayer != NO_PLAYER)
		{
			iYieldStorage *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getStoragePercent();
			iYieldStorage /= 100;
		}
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_YIELD_STORAGE", iYieldStorage));
	}

	if (kBuilding.getStorageLossSellPercentage() > 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_YIELD_OVERFLOW_SELL_PERCENT", kBuilding.getStorageLossSellPercentage()));
	}

	if (kBuilding.isWorksWater())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_WATER_WORK"));
	}

	if (kBuilding.getFreeExperience() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_FREE_XP_UNITS", kBuilding.getFreeExperience()));
	}

	if (kBuilding.getFoodKept() > 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_STORES_FOOD", kBuilding.getFoodKept()));
	}

	if (kBuilding.getHealRateChange() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_HEAL_MOD", kBuilding.getHealRateChange()));
	}

	if (kBuilding.getMilitaryProductionModifier() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_MILITARY_MOD", kBuilding.getMilitaryProductionModifier()));
	}

	if (kBuilding.getDefenseModifier() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_DEFENSE_MOD", kBuilding.getDefenseModifier()));
	}

	if (kBuilding.getBombardDefenseModifier() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_BOMBARD_DEFENSE_MOD", -kBuilding.getBombardDefenseModifier()));
	}

	setYieldChangeHelp(szBuffer, gDLL->getText("TXT_KEY_BUILDING_LAND_PLOTS").c_str(), L": ", L"", kBuilding.getLandPlotYieldChangeArray()); // R&R, ray, Landplot Yields

	setYieldChangeHelp(szBuffer, gDLL->getText("TXT_KEY_BUILDING_WATER_PLOTS").c_str(), L": ", L"", kBuilding.getSeaPlotYieldChangeArray());

	setYieldChangeHelp(szBuffer, gDLL->getText("TXT_KEY_BUILDING_RIVER_PLOTS").c_str(), L": ", L"", kBuilding.getRiverPlotYieldChangeArray());

	iLast = 0;

	for (int iI = 0; iI < GC.getNumUnitCombatInfos(); ++iI)
	{
		if (kBuilding.getUnitCombatFreeExperience(iI) != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_FREE_XP", GC.getUnitCombatInfo((UnitCombatTypes)iI).getTextKeyWide(), kBuilding.getUnitCombatFreeExperience(iI)));
		}
	}

	for (int iI = 0; iI < NUM_DOMAIN_TYPES; ++iI)
	{
		if (kBuilding.getDomainFreeExperience(iI) != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_FREE_XP", GC.getDomainInfo((DomainTypes)iI).getTextKeyWide(), kBuilding.getDomainFreeExperience(iI)));
		}
	}

	for (int iI = 0; iI < NUM_DOMAIN_TYPES; ++iI)
	{
		if (kBuilding.getDomainProductionModifier(iI) != 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_BUILDS_FASTER_DOMAIN", GC.getDomainInfo((DomainTypes)iI).getTextKeyWide(), kBuilding.getDomainProductionModifier(iI)));
		}
	}

	bFirst = true;

	for (int iI = 0; iI < GC.getNumUnitInfos(); ++iI)
	{
		if (GC.getUnitInfo((UnitTypes)iI).getPrereqBuilding() == kBuilding.getBuildingClassType() || GC.getUnitInfo((UnitTypes)iI).isPrereqOrBuilding(kBuilding.getBuildingClassType()))
		{
			szFirstBuffer.Format(L"%s%s", NEWLINE, gDLL->getText("TXT_KEY_BUILDING_REQUIRED_TO_TRAIN").c_str());
			szTempBuffer.Format( SETCOLR L"<link=literal>%s</link>" ENDCOLR , TEXT_COLOR("COLOR_UNIT_TEXT"), GC.getUnitInfo((UnitTypes)iI).getDescription());
			setListHelp(szBuffer, szFirstBuffer, szTempBuffer, L", ", bFirst);
			bFirst = false;
		}
	}

	bFirst = true;

	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); ++iI)
	{
		if (ePlayer != NO_PLAYER)
		{
			eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getCivilizationBuildings(iI)));
		}
		else
		{
			eLoopBuilding = (BuildingTypes)GC.getBuildingClassInfo((BuildingClassTypes)iI).getDefaultBuildingIndex();
		}

		if (eLoopBuilding != NO_BUILDING)
		{
			if (GC.getBuildingInfo(eLoopBuilding).isBuildingClassNeededInCity(kBuilding.getBuildingClassType()))
			{
				if ((pCity == NULL) || pCity->canConstruct(eLoopBuilding, false, true))
				{
					szFirstBuffer.Format(L"%s%s", NEWLINE, gDLL->getText("TXT_KEY_BUILDING_REQUIRED_TO_BUILD").c_str());
					szTempBuffer.Format(SETCOLR L"<link=literal>%s</link>" ENDCOLR, TEXT_COLOR("COLOR_BUILDING_TEXT"), GC.getBuildingInfo(eLoopBuilding).getDescription());
					setListHelp(szBuffer, szFirstBuffer, szTempBuffer, L", ", bFirst);
					bFirst = false;
				}
			}
		}
	}

	if (bCivilopediaText)
	{
		// Trait
		for (int i = 0; i < GC.getNumTraitInfos(); ++i)
		{
			if (kBuilding.getProductionTraits((TraitTypes)i) != 0)
			{
				if (kBuilding.getProductionTraits((TraitTypes)i) == 100)
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_DOUBLE_SPEED_TRAIT", GC.getTraitInfo((TraitTypes)i).getTextKeyWide()));
				}
				else
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_PRODUCTION_MODIFIER_TRAIT", kBuilding.getProductionTraits((TraitTypes)i), GC.getTraitInfo((TraitTypes)i).getTextKeyWide()));
				}
			}
		}
	}

	if (bCivilopediaText)
	{
		if (kBuilding.getFreeStartEra() != NO_ERA)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_FREE_START_ERA", GC.getEraInfo((EraTypes)kBuilding.getFreeStartEra()).getTextKeyWide()));
		}
	}

	if (!isEmpty(kBuilding.getHelp()))
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(kBuilding.getHelp());
	}
	buildBuildingRequiresString(szBuffer, eBuilding, bCivilopediaText, pCity);

	if ((pCity == NULL) || !pCity->isHasRealBuilding(eBuilding))
	{
		if (!bCivilopediaText)
		{
			if (pCity == NULL)
			{
				for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
				{
					YieldTypes eYield = (YieldTypes) iYield;
					if (kBuilding.getYieldCost(eYield) > 0)
					{
						szTempBuffer.Format(L"\n%d%c", (ePlayer != NO_PLAYER ? GET_PLAYER(ePlayer).getYieldProductionNeeded(eBuilding, eYield) : kBuilding.getYieldCost(eYield)), GC.getYieldInfo(eYield).getChar());
						szBuffer.append(szTempBuffer);
					}
				}
			}
			else
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_NUM_TURNS", pCity->getProductionTurnsLeft(eBuilding, ((gDLL->ctrlKey() || !(gDLL->shiftKey())) ? 0 : pCity->getOrderQueueLength()))));

				iProduction = pCity->getBuildingProduction(eBuilding);

				int iProductionNeeded = pCity->getYieldProductionNeeded(eBuilding, YIELD_HAMMERS);
				if (iProduction > 0)
				{
					szTempBuffer.Format(L" - %d/%d%c", iProduction, iProductionNeeded, GC.getYieldInfo(YIELD_HAMMERS).getChar());
					szBuffer.append(szTempBuffer);
				}
				else
				{
					szTempBuffer.Format(L" - %d%c", iProductionNeeded, GC.getYieldInfo(YIELD_HAMMERS).getChar());
					szBuffer.append(szTempBuffer);
				}

				for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
				{
					if (GC.getBuildingInfo(eBuilding).getYieldCost(iYield) > 0)
					{
						YieldTypes eYield = (YieldTypes) iYield;
						if (GC.getYieldInfo(eYield).isCargo())
						{
							int iCost = GET_PLAYER(pCity->getOwnerINLINE()).getYieldProductionNeeded(eBuilding, eYield);
							if (iCost > 0)
							{
								szTempBuffer.Format(L" - %d/%d%c", pCity->getYieldStored(eYield) + pCity->getYieldRushed(eYield), iCost, GC.getYieldInfo(eYield).getChar());
								szBuffer.append(szTempBuffer);
							}
						}
					}
				}
			}
		}

		if ((gDLL->getChtLvl() > 0) && gDLL->ctrlKey() && (pCity != NULL))
		{
			int iBuildingValue = pCity->AI_buildingValue(eBuilding);
			szBuffer.append(CvWString::format(L"\nAI Building Value = %d", iBuildingValue));
		}
	}

	if (bStrategyText)
	{
		if (!isEmpty(kBuilding.getStrategy()))
		{
			if ((ePlayer == NO_PLAYER) || GET_PLAYER(ePlayer).isOption(PLAYEROPTION_ADVISOR_HELP))
			{
				szBuffer.append(SEPARATOR);
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_SIDS_TIPS"));
				szBuffer.append(L'\"');
				szBuffer.append(kBuilding.getStrategy());
				szBuffer.append(L'\"');
			}
		}
	}
}
void CvGameTextMgr::buildBuildingRequiresString(CvWStringBuffer& szBuffer, BuildingTypes eBuilding, bool bCivilopediaText, const CvCity* pCity)
{
	bool bFirst;
	PlayerTypes ePlayer;
	CvWString szTempBuffer;
	CvWString szFirstBuffer;
	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	BuildingTypes eLoopBuilding;

	if (pCity != NULL)
	{
		ePlayer = pCity->getOwnerINLINE();
	}
	else
	{
		ePlayer = GC.getGameINLINE().getActivePlayer();
	}

	if (NULL == pCity || (!pCity->canConstruct(eBuilding) && !pCity->isHasConceptualBuilding(eBuilding)))
	{
		bFirst = true;

		if (!bFirst)
		{
			szBuffer.append(ENDCOLR);
		}
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); ++iI)
		{
			if (ePlayer == NO_PLAYER && kBuilding.getPrereqNumOfBuildingClass((BuildingClassTypes)iI) > 0)
			{
				eLoopBuilding = (BuildingTypes)GC.getBuildingClassInfo((BuildingClassTypes)iI).getDefaultBuildingIndex();
				szTempBuffer.Format(L"%s%s", NEWLINE, gDLL->getText("TXT_KEY_BUILDING_REQUIRES_NUM_SPECIAL_BUILDINGS_NO_CITY", GC.getBuildingInfo(eLoopBuilding).getTextKeyWide(), kBuilding.getPrereqNumOfBuildingClass((BuildingClassTypes)iI)).c_str());

				szBuffer.append(szTempBuffer);
			}
			else if (ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).getBuildingClassPrereqBuilding(eBuilding, ((BuildingClassTypes)iI)) > 0)
			{
				if ((pCity == NULL) || (GET_PLAYER(ePlayer).getBuildingClassCount((BuildingClassTypes)iI) < GET_PLAYER(ePlayer).getBuildingClassPrereqBuilding(eBuilding, ((BuildingClassTypes)iI))))
				{
					eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getCivilizationBuildings(iI)));

					if (eLoopBuilding != NO_BUILDING)
					{
						if (pCity != NULL)
						{
							szTempBuffer.Format(L"%s%s", NEWLINE, gDLL->getText("TXT_KEY_BUILDING_REQUIRES_NUM_SPECIAL_BUILDINGS", GC.getBuildingInfo(eLoopBuilding).getTextKeyWide(), GET_PLAYER(ePlayer).getBuildingClassCount((BuildingClassTypes)iI), GET_PLAYER(ePlayer).getBuildingClassPrereqBuilding(eBuilding, ((BuildingClassTypes)iI))).c_str());
						}
						else
						{
							szTempBuffer.Format(L"%s%s", NEWLINE, gDLL->getText("TXT_KEY_BUILDING_REQUIRES_NUM_SPECIAL_BUILDINGS_NO_CITY", GC.getBuildingInfo(eLoopBuilding).getTextKeyWide(), GET_PLAYER(ePlayer).getBuildingClassPrereqBuilding(eBuilding, ((BuildingClassTypes)iI))).c_str());
						}

						szBuffer.append(szTempBuffer);
					}
				}
			}
			else if (kBuilding.isBuildingClassNeededInCity(iI))
			{
				if (NO_PLAYER != ePlayer)
				{
					eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getCivilizationBuildings(iI)));
				}
				else
				{
					eLoopBuilding = (BuildingTypes)GC.getBuildingClassInfo((BuildingClassTypes)iI).getDefaultBuildingIndex();
				}

				if (eLoopBuilding != NO_BUILDING)
				{
					if ((pCity == NULL) || (!pCity->isHasConceptualBuilding(eLoopBuilding)))
					{
						szBuffer.append(NEWLINE);
						szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_REQUIRES_STRING", GC.getBuildingInfo(eLoopBuilding).getTextKeyWide()));
					}
				}
			}
		}

		if (kBuilding.getNumCitiesPrereq() > 0)
		{
			if (NO_PLAYER == ePlayer || GET_PLAYER(ePlayer).getNumCities() < kBuilding.getNumCitiesPrereq())
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_REQUIRES_NUM_CITIES", kBuilding.getNumCitiesPrereq()));
			}
		}

		if (kBuilding.getUnitLevelPrereq() > 0)
		{
			if (NO_PLAYER == ePlayer || GET_PLAYER(ePlayer).getHighestUnitLevel() < kBuilding.getUnitLevelPrereq())
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_REQUIRES_UNIT_LEVEL", kBuilding.getUnitLevelPrereq()));
			}
		}

		if (kBuilding.getMinLatitude() > 0)
		{
			if (NULL == pCity || pCity->plot()->getLatitude() < kBuilding.getMinLatitude())
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_MIN_LATITUDE", kBuilding.getMinLatitude()));
			}
		}

		if (kBuilding.getMaxLatitude() < 90)
		{
			if (NULL == pCity || pCity->plot()->getLatitude() > kBuilding.getMaxLatitude())
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_MAX_LATITUDE", kBuilding.getMaxLatitude()));
			}
		}

		if (kBuilding.isRiver())
		{
			if (NULL == pCity || !pCity->plot()->isRiver())
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_REQUIRES_RIVER"));
			}
		}

		if (!bCivilopediaText)
		{
			for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
			{
				if (kBuilding.getYieldCost(iYield) > 0)
				{
					YieldTypes eYield = (YieldTypes) iYield;
					if (GC.getYieldInfo(eYield).isCargo())
					{
						int iCost = (NO_PLAYER == ePlayer ? GC.getBuildingInfo(eBuilding).getYieldCost(iYield) : GET_PLAYER(ePlayer).getYieldProductionNeeded(eBuilding, eYield));
						if (NULL == pCity || pCity->getYieldStored(eYield) + pCity->getYieldRushed(eYield) < iCost)
						{
							szBuffer.append(NEWLINE);
							szBuffer.append(gDLL->getText("TXT_KEY_BUILD_CANNOT_AFFORD", iCost, GC.getYieldInfo(eYield).getChar()));
						}
					}
				}
			}
		}

		if (bCivilopediaText)
		{
			if (kBuilding.getVictoryPrereq() != NO_VICTORY)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_REQUIRES_VICTORY", GC.getVictoryInfo((VictoryTypes)(kBuilding.getVictoryPrereq())).getTextKeyWide()));
			}

			if (kBuilding.getMaxStartEra() != NO_ERA)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_MAX_START_ERA", GC.getEraInfo((EraTypes)kBuilding.getMaxStartEra()).getTextKeyWide()));
			}

			if (kBuilding.getNumTeamsPrereq() > 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_REQUIRES_NUM_TEAMS", kBuilding.getNumTeamsPrereq()));
			}
		}

		if (pCity != NULL)
		{
			if (pCity->getFirstBuildingOrder(eBuilding) != -1)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_BUILDING_IN_QUEUE", kBuilding.getNumTeamsPrereq()));
			}
		}
	}
}

void CvGameTextMgr::setFatherPointHelp(CvWStringBuffer &szBuffer, FatherPointTypes eFatherPointType)
{
	CvFatherPointInfo& kFatherPoint = GC.getFatherPointInfo(eFatherPointType);

	szBuffer.append(CvWString::format( SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), kFatherPoint.getDescription()));

	if (kFatherPoint.getProductionConversionPoints() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_FATHER_POINT_PRODUCTION", kFatherPoint.getProductionConversionPoints(), GC.getYieldInfo(YIELD_HAMMERS).getChar()));
	}

	std::vector<FatherTypes> aFathers;
	if (GC.getGameINLINE().getRemainingFathers(eFatherPointType, aFathers))
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_FATHER_POINT_FATHERS_ALL"));
	}
	else if (!aFathers.empty())
	{
		CvWStringBuffer szTemp;
		for (uint i = 0; i < aFathers.size(); ++i)
		{
			if (i > 0)
			{
				szTemp.append(L", ");
			}
			szTemp.append(GC.getFatherInfo(aFathers[i]).getDescription());
		}
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_FATHER_POINT_FATHERS", szTemp.getCString()));
	}
}

void CvGameTextMgr::setYieldChangeHelp(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szSpace, const CvWString& szEnd, const int* piYieldChange, bool bPercent, bool bNewLine)
{
	bool bAllTheSame = true;
	int iPrevChange = 0;
	for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		if (piYieldChange[iI] != 0)
		{
			if (iPrevChange != 0 && piYieldChange[iI] != iPrevChange)
			{
				bAllTheSame = false;
				break;
			}

			iPrevChange = piYieldChange[iI];
		}
	}

	bool bStarted = false;
	for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		if (piYieldChange[iI] != 0)
		{
			CvWString szTempBuffer;

			if (!bStarted)
			{
				if (bNewLine)
				{
					szTempBuffer.Format(L"\n%c", gDLL->getSymbolID(BULLET_CHAR));
				}
				szTempBuffer += CvWString::format(L"%s%s%s%d%s%c",
					szStart.GetCString(),
					szSpace.GetCString(),
					piYieldChange[iI] > 0 ? L"+" : L"",
					piYieldChange[iI],
					bPercent ? L"%" : L"",
					GC.getYieldInfo((YieldTypes)iI).getChar());
			}
			else
			{
				if (bAllTheSame)
				{
					szTempBuffer.Format(L",%c", GC.getYieldInfo((YieldTypes)iI).getChar());
				}
				else
				{
					szTempBuffer.Format(L", %s%d%s%c",
						piYieldChange[iI] > 0 ? L"+" : L"",
						piYieldChange[iI],
						bPercent ? L"%" : L"",
						GC.getYieldInfo((YieldTypes)iI).getChar());
				}
			}
			szBuffer.append(szTempBuffer);
			bStarted = true;
		}
	}
	if (bStarted)
	{
		szBuffer.append(szEnd);
	}
}
void CvGameTextMgr::setBonusHelp(CvWStringBuffer &szBuffer, BonusTypes eBonus, bool bCivilopediaText)
{
	if (NO_BONUS == eBonus)
	{
		return;
	}
	if (!bCivilopediaText)
	{
		szBuffer.append(CvWString::format( SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), GC.getBonusInfo(eBonus).getDescription()));
		setYieldChangeHelp(szBuffer, L"", L"", L"", GC.getBonusInfo(eBonus).getYieldChangeArray());
	}
	ImprovementTypes eImprovement = NO_IMPROVEMENT;
	for (int iLoopImprovement = 0; iLoopImprovement < GC.getNumImprovementInfos(); iLoopImprovement++)
	{
		if (GC.getImprovementInfo((ImprovementTypes)iLoopImprovement).isImprovementBonusMakesValid(eBonus))
		{
			eImprovement = (ImprovementTypes)iLoopImprovement;
			break;
		}
	}
	CivilizationTypes eCivilization = GC.getGameINLINE().getActiveCivilizationType();
	for (int i = 0; i < GC.getNumBuildingClassInfos(); i++)
	{
		BuildingTypes eLoopBuilding;
		if (eCivilization == NO_CIVILIZATION)
		{
			eLoopBuilding = ((BuildingTypes)(GC.getBuildingClassInfo((BuildingClassTypes)i).getDefaultBuildingIndex()));
		}
		else
		{
			eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(eCivilization).getCivilizationBuildings(i)));
		}
	}
	if (!isEmpty(GC.getBonusInfo(eBonus).getHelp()))
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(GC.getBonusInfo(eBonus).getHelp());
	}
}

void CvGameTextMgr::setPromotionHelp(CvWStringBuffer &szBuffer, PromotionTypes ePromotion, bool bCivilopediaText)
{
	if (!bCivilopediaText)
	{
		CvWString szTempBuffer;

		if (NO_PROMOTION == ePromotion)
		{
			return;
		}
		CvPromotionInfo& promo = GC.getPromotionInfo(ePromotion);

		szTempBuffer.Format( SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), promo.getDescription());
		szBuffer.append(szTempBuffer);
	}

	parsePromotionHelp(szBuffer, ePromotion);
}

void CvGameTextMgr::setImprovementHelp(CvWStringBuffer &szBuffer, ImprovementTypes eImprovement, bool bCivilopediaText)
{
	CvWString szTempBuffer;
	CvWString szFirstBuffer;
	int iTurns;

	if (NO_IMPROVEMENT == eImprovement)
	{
		return;
	}
	
	int iGrowthPercent = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();

	CvImprovementInfo& info = GC.getImprovementInfo(eImprovement);
	if (!bCivilopediaText)
	{
		szTempBuffer.Format( SETCOLR L"%s" ENDCOLR, TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), info.getDescription());
		szBuffer.append(szTempBuffer);

		setYieldChangeHelp(szBuffer, L", ", L"", L"", info.getYieldIncreaseArray(), false, false);

		setYieldChangeHelp(szBuffer, L"", L"", gDLL->getText("TXT_KEY_MISC_ON_HILLS").c_str(), info.getHillsYieldChangeArray());
		setYieldChangeHelp(szBuffer, L"", L"", gDLL->getText("TXT_KEY_MISC_ALONG_RIVER").c_str(), info.getRiverSideYieldChangeArray());
		//	Civics
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
		{
			for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); iCivic++)
			{
				int iChange = GC.getCivicInfo((CivicTypes)iCivic).getImprovementYieldChanges(eImprovement, iYield);
				if (0 != iChange)
				{
					szTempBuffer.Format( SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), GC.getCivicInfo((CivicTypes)iCivic).getDescription());
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_CIVIC_IMPROVEMENT_YIELD_CHANGE", iChange, GC.getYieldInfo((YieldTypes)iYield).getChar()));
					szBuffer.append(szTempBuffer);
				}
			}
		}
	}

	if (info.isRequiresRiverSide())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_REQUIRES_RIVER"));
	}
	if (bCivilopediaText)
	{
		if (info.isWater())
		{
			//WTP, ray, Large Rivers - START
			if (info.getTerrainMakesValid(TERRAIN_LARGE_RIVERS))
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_BUILD_ONLY_LARGE_RIVERS"));
			}

			//WTP, ray, Lakes
			if (info.getTerrainMakesValid(TERRAIN_LAKE))
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_BUILD_ONLY_LAKE"));
			}

			// old code in else
			else
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_BUILD_ONLY_WATER"));
			}
			//WTP, ray, Large Rivers - END
		}
		if (info.isRequiresFlatlands())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_ONLY_BUILD_FLATLANDS"));
		}
		//WTP, ray, Large Rivers - START
		if (info.getTerrainMakesValid(TERRAIN_LARGE_RIVERS))
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_ALLOWS_CROSSING_OF_LARGE_RIVERS"));
		}
		//WTP, ray, Large Rivers - END

		//WTP, Unit only Goodies
		// START texts in Colopedia
		if (info.isGoodyForSpawningUnits())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_GOODY_FOR_SPAWNING_UNITS"));
		}
		if (info.isGoodyForSpawningHostileAnimals())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_GOODY_FOR_SPAWNING_HOSTILE_ANIMALS"));
		}
		if (info.isGoodyForSpawningHostileNatives())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_GOODY_FOR_SPAWNING_HOSTILE_NATIVES"));
		}
		if (info.isGoodyForSpawningHostileCriminals())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_GOODY_FOR_SPAWNING_HOSTILE_CRIMINALS"));
		}
		// END texts in Colopedia
	}

	if (info.getImprovementUpgrade() != NO_IMPROVEMENT)
	{
		iTurns = GC.getGameINLINE().getImprovementUpgradeTime(eImprovement);

		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_EVOLVES", GC.getImprovementInfo((ImprovementTypes) info.getImprovementUpgrade()).getTextKeyWide(), iTurns));

		// Super Forts begin *text* *upgrade*
		/* todo revert
		if (info.isUpgradeRequiresFortify())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_FORTIFY_TO_UPGRADE"));
		}*/
		// Super Forts end
	}

	int iLast = -1;
	for (int iBonus = 0; iBonus < GC.getNumBonusInfos(); iBonus++)
	{
		int iRand = info.getImprovementBonusDiscoverRand(iBonus);
		if (iRand > 0)
		{
			szFirstBuffer.Format(L"%s%s", NEWLINE, gDLL->getText("TXT_KEY_IMPROVEMENT_CHANCE_DISCOVER").c_str());
			szTempBuffer.Format(L"%c", GC.getBonusInfo((BonusTypes) iBonus).getChar());
			setListHelp(szBuffer, szFirstBuffer, szTempBuffer, L", ", iRand != iLast);
			iLast = iRand;
		}
	}

	if (0 != info.getDefenseModifier())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_DEFENSE_MODIFIER", info.getDefenseModifier()));
	}

	if (info.isActsAsCity())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_DEFENSE_MODIFIER_EXTRA"));
	}
	// R&R, ray, Monasteries and Forts - START
	if (info.isFort())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_FORT_FEATURES"));
	}
	if (info.isMonastery())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_MONASTERY_FEATURES"));
	}
	// R&R, ray, Monasteries and Forts - END
	if (info.getFeatureGrowthProbability() > 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_MORE_GROWTH"));
	}
	else if (info.getFeatureGrowthProbability() < 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_LESS_GROWTH"));
	}

	// Super Forts begin *text* *bombard*
	/* todo revert
	if (info.isBombardable() && (info.getDefenseModifier() > 0))
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_BOMBARD"));
	}
	if (info.getUniqueRange() > 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_UNIQUE_RANGE", info.getUniqueRange()));
	}
	*/
	// Super Forts end

	if (bCivilopediaText)
	{
		
		// Super Forts begin *text*
		if (info.getCulture() > 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_PLOT_CULTURE", info.getCulture()));
		}
		if (info.getCultureRange() > 0 && ((info.getCulture() > 0) || info.isActsAsCity()))
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_CULTURE_RANGE", info.getCultureRange()));
		}
		if (info.getUnitVisibilityChange() > 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_VISIBILITY_RANGE", info.getUnitVisibilityChange()));
		}
		if (info.getSeeFrom() > 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_SEE_FROM", info.getSeeFrom()));
		}
		// Super Forts end
		if (info.getPillageGold() > 0)
		{
			szBuffer.append(NEWLINE);
			int iPillage = info.getPillageGold();
			iPillage *=iGrowthPercent;
			iPillage /= 100;
			szBuffer.append(gDLL->getText("TXT_KEY_IMPROVEMENT_PILLAGE_YIELDS",iPillage ));
		}
	}
}


void CvGameTextMgr::getDealString(CvWStringBuffer& szBuffer, CvDeal& deal, PlayerTypes ePlayerPerspective)
{
	PlayerTypes ePlayer1 = deal.getFirstPlayer();
	PlayerTypes ePlayer2 = deal.getSecondPlayer();

	const CLinkList<TradeData>* pListPlayer1 = deal.getFirstTrades();
	const CLinkList<TradeData>* pListPlayer2 = deal.getSecondTrades();

	getDealString(szBuffer, ePlayer1, ePlayer2, pListPlayer1,  pListPlayer2, ePlayerPerspective);
}

void CvGameTextMgr::getDealString(CvWStringBuffer& szBuffer, PlayerTypes ePlayer1, PlayerTypes ePlayer2, const CLinkList<TradeData>* pListPlayer1, const CLinkList<TradeData>* pListPlayer2, PlayerTypes ePlayerPerspective)
{
	if (NO_PLAYER == ePlayer1 || NO_PLAYER == ePlayer2)
	{
		FAssertMsg(false, "Deal needs two parties");
		return;
	}

	CvWStringBuffer szDealOne;
	if (NULL != pListPlayer1 && pListPlayer1->getLength() > 0)
	{
		CLLNode<TradeData>* pTradeNode;
		bool bFirst = true;
		for (pTradeNode = pListPlayer1->head(); pTradeNode; pTradeNode = pListPlayer1->next(pTradeNode))
		{
			CvWStringBuffer szTrade;
			getTradeString(szTrade, pTradeNode->m_data, ePlayer1, ePlayer2);
			setListHelp(szDealOne, L"", szTrade.getCString(), L", ", bFirst);
			bFirst = false;
		}
	}

	CvWStringBuffer szDealTwo;
	if (NULL != pListPlayer2 && pListPlayer2->getLength() > 0)
	{
		CLLNode<TradeData>* pTradeNode;
		bool bFirst = true;
		for (pTradeNode = pListPlayer2->head(); pTradeNode; pTradeNode = pListPlayer2->next(pTradeNode))
		{
			CvWStringBuffer szTrade;
			getTradeString(szTrade, pTradeNode->m_data, ePlayer2, ePlayer1);
			setListHelp(szDealTwo, L"", szTrade.getCString(), L", ", bFirst);
			bFirst = false;
		}
	}

	if (!szDealOne.isEmpty())
	{
		if (!szDealTwo.isEmpty())
		{
			if (ePlayerPerspective == ePlayer1)
			{
				szBuffer.append(gDLL->getText("TXT_KEY_MISC_OUR_DEAL", szDealOne.getCString(), GET_PLAYER(ePlayer2).getNameKey(), szDealTwo.getCString()));
			}
			else if (ePlayerPerspective == ePlayer2)
			{
				szBuffer.append(gDLL->getText("TXT_KEY_MISC_OUR_DEAL", szDealTwo.getCString(), GET_PLAYER(ePlayer1).getNameKey(), szDealOne.getCString()));
			}
			else
			{
				szBuffer.append(gDLL->getText("TXT_KEY_MISC_DEAL", GET_PLAYER(ePlayer1).getNameKey(), szDealOne.getCString(), GET_PLAYER(ePlayer2).getNameKey(), szDealTwo.getCString()));
			}
		}
		else
		{
			if (ePlayerPerspective == ePlayer1)
			{
				szBuffer.append(gDLL->getText("TXT_KEY_MISC_DEAL_ONESIDED_OURS", szDealOne.getCString(), GET_PLAYER(ePlayer2).getNameKey()));
			}
			else if (ePlayerPerspective == ePlayer2)
			{
				szBuffer.append(gDLL->getText("TXT_KEY_MISC_DEAL_ONESIDED_THEIRS", szDealOne.getCString(), GET_PLAYER(ePlayer1).getNameKey()));
			}
			else
			{
				szBuffer.append(gDLL->getText("TXT_KEY_MISC_DEAL_ONESIDED", GET_PLAYER(ePlayer1).getNameKey(), szDealOne.getCString(), GET_PLAYER(ePlayer2).getNameKey()));
			}
		}
	}
	else if (!szDealTwo.isEmpty())
	{
		if (ePlayerPerspective == ePlayer1)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_MISC_DEAL_ONESIDED_THEIRS", szDealTwo.getCString(), GET_PLAYER(ePlayer2).getNameKey()));
		}
		else if (ePlayerPerspective == ePlayer2)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_MISC_DEAL_ONESIDED_OURS", szDealTwo.getCString(), GET_PLAYER(ePlayer1).getNameKey()));
		}
		else
		{
			szBuffer.append(gDLL->getText("TXT_KEY_MISC_DEAL_ONESIDED", GET_PLAYER(ePlayer2).getNameKey(), szDealTwo.getCString(), GET_PLAYER(ePlayer1).getNameKey()));
		}
	}
}

void CvGameTextMgr::getWarplanString(CvWStringBuffer& szString, WarPlanTypes eWarPlan)
{
	switch (eWarPlan)
	{
		case WARPLAN_ATTACKED_RECENT: szString.assign(L"new defensive war"); break;
		case WARPLAN_ATTACKED: szString.assign(L"defensive war"); break;
		case WARPLAN_PREPARING_LIMITED: szString.assign(L"preparing limited war"); break;
		case WARPLAN_PREPARING_TOTAL: szString.assign(L"preparing total war"); break;
		case WARPLAN_LIMITED: szString.assign(L"limited war"); break;
		case WARPLAN_TOTAL: szString.assign(L"total war"); break;
		case WARPLAN_DOGPILE: szString.assign(L"dogpile war"); break;
		case WARPLAN_EXTORTION: szString.assign(L"extortion war"); break;
		case NO_WARPLAN: szString.assign(L"unplanned war"); break;
		default:  szString.assign(L"unknown war"); break;
	}
}

void CvGameTextMgr::getAttitudeString(CvWStringBuffer& szBuffer, PlayerTypes ePlayer, PlayerTypes eTargetPlayer)
{
	CvWString szTempBuffer;
	int iAttitudeChange;
	int iPass;
	int iI;
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	TeamTypes eTeam = (TeamTypes) kPlayer.getTeam();
	CvTeamAI& kTeam = GET_TEAM(eTeam);

	szBuffer.append(gDLL->getText("TXT_KEY_ATTITUDE_TOWARDS", GC.getAttitudeInfo(GET_PLAYER(ePlayer).AI_getAttitude(eTargetPlayer)).getTextKeyWide(), GET_PLAYER(eTargetPlayer).getNameKey()));

    // R&R, Robert Surcouf, No More Variables Hidden game option START
    if (GC.getGameINLINE().isOption(GAMEOPTION_NO_MORE_VARIABLES_HIDDEN))
    {
        // Base Attitude
		iAttitudeChange = GC.getLeaderHeadInfo(kPlayer.getPersonalityType()).getBaseAttitude();
        if (iAttitudeChange ==0 )
        {
            szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR("COLOR_PLAYER_BLUE_TEXT"), gDLL->getText("TXT_KEY_MISC_BASE_ATTITUDE", iAttitudeChange).GetCString());
        }
        else
        {
            szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange>0)? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT" ), gDLL->getText("TXT_KEY_MISC_BASE_ATTITUDE", iAttitudeChange).GetCString());
        }
        szBuffer.append(NEWLINE);
        szBuffer.append(szTempBuffer);

        // Attitude from Handicap, difficulty level, team-members
        iAttitudeChange = GC.getHandicapInfo(GET_PLAYER(eTargetPlayer).getHandicapType()).getAttitudeChange();
        if (iAttitudeChange ==0 )
        {
            szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR("COLOR_PLAYER_BLUE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_FROM_HANDICAP", iAttitudeChange).GetCString());
        }
        else
        {
            szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange>0)? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT" ), gDLL->getText("TXT_KEY_MISC_ATTITUDE_FROM_HANDICAP", iAttitudeChange).GetCString());
        }
        szBuffer.append(NEWLINE);
        szBuffer.append(szTempBuffer);

        if (kPlayer.getTeam() == GET_PLAYER(eTargetPlayer).getTeam() )
        {
            iAttitudeChange = 999;
            szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR("COLOR_POSITIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_SAME_TEAM", iAttitudeChange).GetCString());
            szBuffer.append(NEWLINE);
            szBuffer.append(szTempBuffer);
        }

        // Native Attitude
        iAttitudeChange = GC.getLeaderHeadInfo(kPlayer.getPersonalityType()).getNativeAttitude();
        if (GET_PLAYER(eTargetPlayer).isNative())
        {
            if (iAttitudeChange ==0 )
            {
                szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR("COLOR_PLAYER_BLUE_TEXT"), gDLL->getText("TXT_KEY_MISC_NATIVE_ATTITUDE", iAttitudeChange).GetCString());
            }
            else
            {
                szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange>0)? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT" ), gDLL->getText("TXT_KEY_MISC_NATIVE_ATTITUDE", iAttitudeChange).GetCString());
            }
            szBuffer.append(NEWLINE);
            szBuffer.append(szTempBuffer);
        }
	}
    // R&R, Robert Surcouf, No More Variables Hidden game option END
	for (iPass = 0; iPass < 2; iPass++)
	{
		iAttitudeChange = kPlayer.AI_getCloseBordersAttitude(eTargetPlayer);
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_LAND_TARGET", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = kPlayer.AI_getStolenPlotsAttitude(eTargetPlayer);
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_STOLEN_LAND", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = kPlayer.AI_getAlarmAttitude(eTargetPlayer);
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_ALARM", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = kPlayer.AI_getRebelAttitude(eTargetPlayer);
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_REBEL", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = kPlayer.AI_getWarAttitude(eTargetPlayer);
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_WAR", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = kPlayer.AI_getPeaceAttitude(eTargetPlayer);
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_PEACE", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = kPlayer.AI_getOpenBordersAttitude(eTargetPlayer);
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_OPEN_BORDERS", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = kPlayer.AI_getDefensivePactAttitude(eTargetPlayer);
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_DEFENSIVE_PACT", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = kPlayer.AI_getRivalDefensivePactAttitude(eTargetPlayer);
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_RIVAL_DEFENSIVE_PACT", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = kPlayer.AI_getShareWarAttitude(eTargetPlayer);
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_SHARE_WAR", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = kPlayer.AI_getTradeAttitude(eTargetPlayer);
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_TRADE", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = kPlayer.AI_getRivalTradeAttitude(eTargetPlayer);
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_RIVAL_TRADE", iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

		iAttitudeChange = GET_PLAYER(ePlayer).AI_getAttitudeExtra(eTargetPlayer);
		if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
		{
			szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText(((iAttitudeChange > 0) ? "TXT_KEY_MISC_ATTITUDE_EXTRA_GOOD" : "TXT_KEY_MISC_ATTITUDE_EXTRA_BAD"), iAttitudeChange).GetCString());
			szBuffer.append(NEWLINE);
			szBuffer.append(szTempBuffer);
		}

        // R&R, Robert Surcouf, No More Variables Hidden game option START
        if (GC.getGameINLINE().isOption(GAMEOPTION_NO_MORE_VARIABLES_HIDDEN))
        {
            iAttitudeChange = GC.getLeaderHeadInfo(kPlayer.getPersonalityType()).getLostWarAttitudeChange();
            if (GET_TEAM(GET_PLAYER(eTargetPlayer).getTeam()).AI_getWarSuccess(kPlayer.getTeam()) > GET_TEAM(kPlayer.getTeam()).AI_getWarSuccess(GET_PLAYER(eTargetPlayer).getTeam()))
            {
                if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
                {
                    szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText(((iAttitudeChange > 0) ? "TXT_KEY_MISC_LOST_WAR_ATTITUDE" : "TXT_KEY_MISC_LOST_WAR_ATTITUDE"), iAttitudeChange).GetCString());
                    szBuffer.append(NEWLINE);
                    szBuffer.append(szTempBuffer);
                }
            }

            iAttitudeChange = - std::max(0, (GET_TEAM(GET_PLAYER(eTargetPlayer).getTeam()).getNumMembers() - GET_TEAM(kPlayer.getTeam()).getNumMembers()));
            if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
            {
                szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText(((iAttitudeChange > 0) ? "TXT_KEY_MISC_TEAMSIZE_ATTITUDE" : "TXT_KEY_MISC_TEAMSIZE_ATTITUDE"), iAttitudeChange).GetCString());
                szBuffer.append(NEWLINE);
                szBuffer.append(szTempBuffer);
            }
        }
        // R&R, Robert Surcouf, No More Variables Hidden game option END

        for (iI = 0; iI < NUM_MEMORY_TYPES; ++iI)
		{
			iAttitudeChange = kPlayer.AI_getMemoryAttitude(eTargetPlayer, ((MemoryTypes)iI));
			if ((iPass == 0) ? (iAttitudeChange > 0) : (iAttitudeChange < 0))
			{
				szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange > 0) ? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT"), gDLL->getText("TXT_KEY_MISC_ATTITUDE_MEMORY", iAttitudeChange, GC.getMemoryInfo((MemoryTypes)iI).getDescription()).GetCString());
				szBuffer.append(NEWLINE);
				szBuffer.append(szTempBuffer);
			}
		}
	}
    // R&R, Robert Surcouf, No More Variables Hidden game option START
    if (GC.getGameINLINE().isOption(GAMEOPTION_NO_MORE_VARIABLES_HIDDEN))
    {
        iAttitudeChange = kPlayer.AI_getAttitudeVal(eTargetPlayer);
        //szBuffer.append(NEWLINE);
        szBuffer.append(SEPARATOR);
        szBuffer.append(NEWLINE);
        if (iAttitudeChange ==0 )
        {
            szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR("COLOR_PLAYER_BLUE_TEXT"), gDLL->getText("TXT_KEY_MISC_TOTAL_ATTITUDE", iAttitudeChange).GetCString());
        }
        else
        {
            szTempBuffer.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR((iAttitudeChange>0)? "COLOR_POSITIVE_TEXT" : "COLOR_NEGATIVE_TEXT" ), gDLL->getText("TXT_KEY_MISC_TOTAL_ATTITUDE", iAttitudeChange).GetCString());
        }
        szBuffer.append(szTempBuffer);
    }
    // R&R, Robert Surcouf, No More Variables Hidden game option END
}

void CvGameTextMgr::getTradeString(CvWStringBuffer& szBuffer, const TradeData& tradeData, PlayerTypes ePlayer1, PlayerTypes ePlayer2)
{
	switch (tradeData.m_eItemType)
	{
	case TRADE_GOLD:
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_GOLD", tradeData.m_iData1));
		break;
	case TRADE_YIELD:
		szBuffer.assign(CvWString::format(L"%s", GC.getYieldInfo((YieldTypes)tradeData.m_iData1).getDescription()));
		break;
	case TRADE_MAPS:
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_WORLD_MAP"));
		break;
	case TRADE_OPEN_BORDERS:
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_OPEN_BORDERS"));
		break;
	case TRADE_DEFENSIVE_PACT:
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_DEFENSIVE_PACT"));
		break;
	case TRADE_PERMANENT_ALLIANCE:
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_PERMANENT_ALLIANCE"));
		break;
	case TRADE_PEACE_TREATY:
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_PEACE_TREATY", GC.getDefineINT("PEACE_TREATY_LENGTH")));
		break;
	case TRADE_CITIES:
		szBuffer.assign(CvWString::format(L"%s", GET_PLAYER(ePlayer1).getCity(tradeData.m_iData1)->getName().GetCString()));
		break;
	case TRADE_PEACE:
	case TRADE_WAR:
	case TRADE_EMBARGO:
		szBuffer.assign(CvWString::format(L"%s", GET_TEAM((TeamTypes)tradeData.m_iData1).getName().GetCString()));
		break;
	default:
		FAssert(false);
		break;
	}
}

void CvGameTextMgr::setFeatureHelp(CvWStringBuffer &szBuffer, FeatureTypes eFeature, bool bCivilopediaText)
{
	if (NO_FEATURE == eFeature)
	{
		return;
	}
	CvFeatureInfo& feature = GC.getFeatureInfo(eFeature);

	int aiYields[NUM_YIELD_TYPES];
	if (!bCivilopediaText)
	{
		szBuffer.append(feature.getDescription());

		for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
		{
			aiYields[iI] = feature.getYieldChange(iI);
		}
		setYieldChangeHelp(szBuffer, L"", L"", L"", aiYields);
	}

	if (feature.getMovementCost() != 1)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_TERRAIN_MOVEMENT_COST", feature.getMovementCost()));
	}

	if (feature.getDefenseModifier() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_TERRAIN_DEFENSE_MODIFIER", feature.getDefenseModifier()));
	}

	if (feature.isImpassable())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_TERRAIN_IMPASSABLE"));
	}

	if (feature.isNoCity())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_TERRAIN_NO_CITIES"));
	}

	if (feature.isNoImprovement())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_FEATURE_NO_IMPROVEMENT"));
	}

	//WTP, ray, Large Rivers - START
	// Pedia text for Crossing Large Rivers
	if (feature.isTerrain(TERRAIN_LARGE_RIVERS))
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_TERRAIN_FEATURE_ALLOWS_CROSSING_OF_LARGE_RIVERS"));
	}
	//WTP, ray, Large Rivers - START
}


void CvGameTextMgr::setTerrainHelp(CvWStringBuffer &szBuffer, TerrainTypes eTerrain, bool bCivilopediaText)
{
	if (NO_TERRAIN == eTerrain)
	{
		return;
	}
	CvTerrainInfo& terrain = GC.getTerrainInfo(eTerrain);

	int aiYields[NUM_YIELD_TYPES];
	if (!bCivilopediaText)
	{
		szBuffer.append(terrain.getDescription());

		for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
		{
			aiYields[iI] = terrain.getYield(iI);
		}
		setYieldChangeHelp(szBuffer, L"", L"", L"", aiYields);
	}

	if (terrain.getMovementCost() != 1)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_TERRAIN_MOVEMENT_COST", terrain.getMovementCost()));
	}

	if (terrain.getBuildModifier() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_TERRAIN_BUILD_MODIFIER", terrain.getBuildModifier()));
	}

	if (terrain.getDefenseModifier() != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_TERRAIN_DEFENSE_MODIFIER", terrain.getDefenseModifier()));
	}

	if (terrain.isImpassable())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_TERRAIN_IMPASSABLE"));
	}
	if (!terrain.isFound())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_TERRAIN_NO_CITIES"));
		bool bFirst = true;
		if (terrain.isFoundCoast())
		{
			szBuffer.append(gDLL->getText("TXT_KEY_TERRAIN_COASTAL_CITIES"));
			bFirst = false;
		}
		if (!bFirst)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_OR"));
		}
	}
}

void CvGameTextMgr::setYieldsHelp(CvWStringBuffer &szBuffer, YieldTypes eYield, bool bCivilopediaText)
{
	if (NO_YIELD == eYield)
	{
		return;
	}

	CvYieldInfo& yield = GC.getYieldInfo(eYield);
	szBuffer.append(yield.getDescription());
}

void CvGameTextMgr::setProductionHelp(CvWStringBuffer &szBuffer, CvCity& city)
{
	FAssertMsg(NO_PLAYER != city.getOwnerINLINE(), "City must have an owner");

	if (city.getCurrentProductionDifference(true) == 0)
	{
		return;
	}

	setYieldHelp(szBuffer, city, YIELD_HAMMERS);

	int iPastOverflow = city.isProductionConvince() ? 0 : city.getOverflowProduction();
	if (iPastOverflow != 0)
	{
		szBuffer.append(NEWLINE);
		//ray Hammer Icon Fix
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_PROD_OVERFLOW", iPastOverflow, GC.getYieldInfo(YIELD_HAMMERS).getChar()));
	}

	int iBaseProduction = city.calculateNetYield(YIELD_HAMMERS) + iPastOverflow;
	int iBaseModifier = 100;

	UnitTypes eUnit = city.getProductionUnit();
	if (NO_UNIT != eUnit)
	{
		CvUnitInfo& unit = GC.getUnitInfo(eUnit);

		// Domain
		int iDomainMod = city.getDomainProductionModifier((DomainTypes)unit.getDomainType());
		if (0 != iDomainMod)
		{
			szBuffer.append(NEWLINE);
			//ray Hammer Icon Fix
			szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_PROD_DOMAIN", iDomainMod, GC.getDomainInfo((DomainTypes)unit.getDomainType()).getTextKeyWide(), GC.getYieldInfo(YIELD_HAMMERS).getChar()));
			iBaseModifier += iDomainMod;
		}

		// Military
		if (unit.isMilitaryProduction())
		{
			int iMilitaryMod = city.getMilitaryProductionModifier() + GET_PLAYER(city.getOwnerINLINE()).getMilitaryProductionModifier();
			if (0 != iMilitaryMod)
			{
				szBuffer.append(NEWLINE);
				//ray Hammer Icon Fix
				szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_PROD_MILITARY", iMilitaryMod, GC.getYieldInfo(YIELD_HAMMERS).getChar()));
				iBaseModifier += iMilitaryMod;
			}
		}

		// Trait
		for (int i = 0; i < GC.getNumTraitInfos(); i++)
		{
			if (city.hasTrait((TraitTypes)i))
			{
				int iTraitMod = unit.getProductionTraits(i);

				if (unit.getSpecialUnitType() != NO_SPECIALUNIT)
				{
					iTraitMod += GC.getSpecialUnitInfo((SpecialUnitTypes) unit.getSpecialUnitType()).getProductionTraits(i);
				}
				if (0 != iTraitMod)
				{
					szBuffer.append(NEWLINE);
					//ray Hammer Icon Fix
					szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_PROD_TRAIT", iTraitMod, unit.getTextKeyWide(), GC.getTraitInfo((TraitTypes)i).getTextKeyWide(), GC.getYieldInfo(YIELD_HAMMERS).getChar()));
					iBaseModifier += iTraitMod;
				}
			}
		}
	}

	BuildingTypes eBuilding = city.getProductionBuilding();
	if (NO_BUILDING != eBuilding)
	{
		CvBuildingInfo& building = GC.getBuildingInfo(eBuilding);

		// Trait
		for (int i = 0; i < GC.getNumTraitInfos(); i++)
		{
			if (city.hasTrait((TraitTypes)i))
			{
				int iTraitMod = building.getProductionTraits(i);

				if (building.getSpecialBuildingType() != NO_SPECIALBUILDING)
				{
					iTraitMod += GC.getSpecialBuildingInfo((SpecialBuildingTypes) building.getSpecialBuildingType()).getProductionTraits(i);
				}
				iTraitMod += GC.getTraitInfo((TraitTypes) i).getBuildingProductionModifier(building.getBuildingClassType());

				if (0 != iTraitMod)
				{
					szBuffer.append(NEWLINE);
					//ray Hammer Icon Fix
					szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_PROD_TRAIT", iTraitMod, building.getTextKeyWide(), GC.getTraitInfo((TraitTypes)i).getTextKeyWide(), GC.getYieldInfo(YIELD_HAMMERS).getChar()));
					iBaseModifier += iTraitMod;
				}
			}
		}
	}

	int iModProduction = (iBaseModifier * iBaseProduction) / 100;

	FAssertMsg(iModProduction == city.getCurrentProductionDifference(true), "Modified Production does not match actual value");

	szBuffer.append(NEWLINE);
	//ray Hammer Icon Fix
	szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_PROD_FINAL_YIELD", iModProduction, GC.getYieldInfo(YIELD_HAMMERS).getChar()));
}


void CvGameTextMgr::parsePlayerTraits(CvWStringBuffer &szBuffer, PlayerTypes ePlayer)
{
	CvCivilizationInfo& kCiv = GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType());
	CvLeaderHeadInfo& kLeader = GC.getLeaderHeadInfo(GET_PLAYER(ePlayer).getLeaderType());
	bool bFirst = true;

	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
	{
		TraitTypes eTrait = (TraitTypes) iTrait;
		if (kLeader.hasTrait(eTrait) || kCiv.hasTrait(eTrait))
		{
			if (bFirst)
			{
				szBuffer.append(L" (");
				bFirst = false;
			}
			else
			{
				szBuffer.append(L", ");
			}
			szBuffer.append(GC.getTraitInfo(eTrait).getDescription());
		}
	}

	if (!bFirst)
	{
		szBuffer.append(L")");
	}
}

void CvGameTextMgr::parseLeaderHeadHelp(CvWStringBuffer &szBuffer, PlayerTypes eThisPlayer, PlayerTypes eOtherPlayer)
{
	if (NO_PLAYER == eThisPlayer)
	{
		return;
	}

	szBuffer.append(CvWString::format(L"%s", GET_PLAYER(eThisPlayer).getName()));

	parsePlayerTraits(szBuffer, eThisPlayer);

	szBuffer.append(L"\n");

	if (eOtherPlayer != NO_PLAYER)
	{
		CvTeam& kThisTeam = GET_TEAM(GET_PLAYER(eThisPlayer).getTeam());
		if (eOtherPlayer != eThisPlayer && kThisTeam.isHasMet(GET_PLAYER(eOtherPlayer).getTeam()))
		{
			getAttitudeString(szBuffer, eThisPlayer, eOtherPlayer);

			getActiveDealsString(szBuffer, eThisPlayer, eOtherPlayer);
		}
	}
}


void CvGameTextMgr::parseLeaderLineHelp(CvWStringBuffer &szBuffer, PlayerTypes eThisPlayer, PlayerTypes eOtherPlayer)
{
	if (NO_PLAYER == eThisPlayer || NO_PLAYER == eOtherPlayer)
	{
		return;
	}
	CvTeam& thisTeam = GET_TEAM(GET_PLAYER(eThisPlayer).getTeam());
	CvTeam& otherTeam = GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam());

	if (thisTeam.getID() == otherTeam.getID())
	{
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_PERMANENT_ALLIANCE"));
		szBuffer.append(NEWLINE);
	}
	else if (thisTeam.isAtWar(otherTeam.getID()))
	{
		szBuffer.append(gDLL->getText("TXT_KEY_WAR"));
		szBuffer.append(NEWLINE);
	}
	else
	{
		if (thisTeam.isDefensivePact(otherTeam.getID()))
		{
			szBuffer.append(gDLL->getText("TXT_KEY_MISC_DEFENSIVE_PACT"));
			szBuffer.append(NEWLINE);
		}
		if (thisTeam.isOpenBorders(otherTeam.getID()))
		{
			szBuffer.append(gDLL->getText("TXT_KEY_MISC_OPEN_BORDERS"));
			szBuffer.append(NEWLINE);
		}
	}
}


void CvGameTextMgr::getActiveDealsString(CvWStringBuffer &szBuffer, PlayerTypes eThisPlayer, PlayerTypes eOtherPlayer)
{
	int iIndex;
	CvDeal* pDeal = GC.getGameINLINE().firstDeal(&iIndex);
	while (NULL != pDeal)
	{
		if ((pDeal->getFirstPlayer() == eThisPlayer && pDeal->getSecondPlayer() == eOtherPlayer)
			|| (pDeal->getFirstPlayer() == eOtherPlayer && pDeal->getSecondPlayer() == eThisPlayer))
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(CvWString::format(L"%c", gDLL->getSymbolID(BULLET_CHAR)));
			getDealString(szBuffer, *pDeal, eThisPlayer);
		}
		pDeal = GC.getGameINLINE().nextDeal(&iIndex);
	}
}

void CvGameTextMgr::buildHintsList(CvWStringBuffer& szBuffer)
{
	for (int i = 0; i < GC.getNumHints(); i++)
	{
		szBuffer.append(CvWString::format(L"%c%s", gDLL->getSymbolID(BULLET_CHAR), GC.getHints(i).getText()));
		szBuffer.append(NEWLINE);
		szBuffer.append(NEWLINE);
	}
}

void CvGameTextMgr::setYieldPriceHelp(CvWStringBuffer &szBuffer, PlayerTypes ePlayer, YieldTypes eYield)
{
	CvYieldInfo& info = GC.getYieldInfo(eYield);
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	szBuffer.append(CvWString::format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), info.getDescription()));
	if (info.isCargo() && kPlayer.isYieldEuropeTradable(eYield))
	{
		CvPlayer& kParent = GET_PLAYER(kPlayer.getParent());
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUY_AND_SELL_YIELD", kParent.getYieldBuyPrice(eYield), kParent.getYieldSellPrice(eYield)));
	}
	// R&R, ray, Africa
	if (info.isCargo() && kPlayer.isYieldAfricaTradable(eYield))
	{
		CvPlayer& kParent = GET_PLAYER(kPlayer.getParent());
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUY_AND_SELL_YIELD_AFRICA", kParent.getYieldAfricaBuyPrice(eYield), kParent.getYieldAfricaSellPrice(eYield)));
	}
	// R&R, ray, Africa - END
	// R&R, ray, Port Royal
	if (info.isCargo() && kPlayer.isYieldPortRoyalTradable(eYield))
	{
		CvPlayer& kParent = GET_PLAYER(kPlayer.getParent());
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_BUY_AND_SELL_YIELD_PORT_ROYAL", kParent.getYieldPortRoyalBuyPrice(eYield), kParent.getYieldPortRoyalSellPrice(eYield)));
	}
	// R&R, ray, Port Royal - END
	szBuffer.append(ENDCOLR);
}

void CvGameTextMgr::setYieldHelp(CvWStringBuffer &szBuffer, CvCity& city, YieldTypes eYieldType)
{
	FAssertMsg(NO_PLAYER != city.getOwnerINLINE(), "City must have an owner");

	if (NO_YIELD == eYieldType)
	{
		return;
	}
	CvYieldInfo& info = GC.getYieldInfo(eYieldType);

	if (NO_PLAYER == city.getOwnerINLINE())
	{
		return;
	}
	CvPlayerAI& owner = GET_PLAYER(city.getOwnerINLINE());

	setYieldPriceHelp(szBuffer, city.getOwnerINLINE(), eYieldType);

	// R&R, ray, adjustment Domestic Markets - START
	int iYieldDomesticDemand = city.getYieldDemand(eYieldType);
	int iYieldDomesticPrice = city.getYieldBuyPrice(eYieldType);
	if (iYieldDomesticDemand > 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_DOMESTIC_INFO_YIELD", iYieldDomesticDemand, iYieldDomesticPrice));
	}

	if (GC.getGameINLINE().isDebugMode())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("(city)AI_estimateYieldValue value: %d1", city.AI_estimateYieldValue(eYieldType, 1)));
		szBuffer.append(gDLL->getText("(player)AI_yieldValue value: %d1", owner.AI_yieldValue(eYieldType)));
	}

	// R&R, ray, adjustment Domestic Markets - START

	if (city.isOccupation())
	{
		return;
	}

	int iBaseProduction = 0;

	int iBuildingYield = 0;
	for (int i = 0; i < GC.getNumBuildingInfos(); ++i)
	{
		if (city.isHasBuilding((BuildingTypes)i))
		{
			iBuildingYield += GC.getBuildingInfo((BuildingTypes) i).getYieldChange(eYieldType);
			iBuildingYield += city.getBuildingYieldChange((BuildingClassTypes)GC.getBuildingInfo((BuildingTypes) i).getBuildingClassType(), eYieldType);
			iBuildingYield += owner.getBuildingYieldChange((BuildingClassTypes)GC.getBuildingInfo((BuildingTypes) i).getBuildingClassType(), eYieldType);
		}
	}

	if (iBuildingYield != 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_BUILDING_YIELD", iBuildingYield, info.getChar()));

		iBaseProduction += iBuildingYield;
	}
	// R&R, ray , MYCP partially based on code of Aymerick - START
	std::vector< std::vector<int> > aaiProfessionYields;
	aaiProfessionYields.resize(GC.getNumProfessionInfos());
	
	// Indoor professions
	for (int i = 0; i < city.getPopulation(); ++i)
	{
		CvUnit* pUnit = city.getPopulationUnitByIndex(i);
		if (NULL != pUnit)
		{
			ProfessionTypes eProfession = pUnit->getProfession();
			if (NO_PROFESSION != eProfession)
			{
				CvProfessionInfo& kProfessionInfo = GC.getProfessionInfo(eProfession);
				aaiProfessionYields[eProfession].resize(kProfessionInfo.getNumYieldsProduced(), 0);
				for (int j = 0; j < kProfessionInfo.getNumYieldsProduced(); j++)
				{
					if (kProfessionInfo.getYieldsProduced(j) == eYieldType)
					{
						int iCityYieldProduction = city.getProfessionOutput(eProfession, pUnit);
						if (iCityYieldProduction != 0)
						{
							int iCitizenYield = city.getProfessionOutput(eProfession, pUnit);
							aaiProfessionYields[eProfession][j] += iCitizenYield;
							iBaseProduction += iCitizenYield;
						}
					}					
				}
			}
		}
	}

	// From plots
	int iPlotYield = 0;
	int iCityPlotYield = 0;
	for (int i = 0; i < NUM_CITY_PLOTS; ++i)
	{
		CvPlot* pPlot = city.getCityIndexPlot(i);
		if (pPlot != NULL)
		{
			if (i == CITY_HOME_PLOT)
			{
				iCityPlotYield = pPlot->getYield(eYieldType);
			}
			else
			{
				CvUnit* pUnit = city.getUnitWorkingPlot(i);
				if (NULL != pUnit && pUnit->getOwnerINLINE() == city.getOwnerINLINE())
				{
					ProfessionTypes eProfession = pUnit->getProfession();
					if (NO_PROFESSION != eProfession)
					{
						for (int j = 0; j < GC.getProfessionInfo(eProfession).getNumYieldsProduced(); j++)
						{
							// R&R, ray, adjustment, START
							if (j == 0 && GC.getProfessionInfo(eProfession).getYieldsProduced(j) == eYieldType)
							{
								int iPlotYield = pPlot->getYield(eYieldType);
								aaiProfessionYields[eProfession][j] += iPlotYield;
								iBaseProduction += iPlotYield;
							}
							else if (GC.getProfessionInfo(eProfession).getYieldsProduced(j) == eYieldType)
							{
								int iPlotYield = pPlot->getYield((YieldTypes) GC.getProfessionInfo(eProfession).getYieldsProduced(0)) / 2;
								aaiProfessionYields[eProfession][j] += iPlotYield;
								iBaseProduction += iPlotYield;
							}
							// R&R, ray, adjustment, END
						}
					}
				}
			}
		}
	}

	for (uint i = 0; i < aaiProfessionYields.size(); ++i)
	{
		for (uint j = 0; j < aaiProfessionYields[i].size(); ++j)
		{
			if (aaiProfessionYields[i][j] > 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_BASE_CITIZEN_YIELD", aaiProfessionYields[i][j], info.getChar(), GC.getProfessionInfo((ProfessionTypes)i).getTextKeyWide()));
			}
		}
	}
	// R&R, ray , MYCP partially based on code of Aymerick - END

	//city plot
	if (iCityPlotYield > 0)
	{
		iBaseProduction += iCityPlotYield;
		szBuffer.append(NEWLINE);
		szBuffer.append(CvWString::format(gDLL->getText("TXT_KEY_MISC_FROM_CITY_YIELD", iCityPlotYield, info.getChar())));
	}

	FAssert(iBaseProduction == city.getBaseRawYieldProduced(eYieldType));

	int aiYields[NUM_YIELD_TYPES];
	int aiRawProducedYields[NUM_YIELD_TYPES];
	int aiRawConsumedYields[NUM_YIELD_TYPES];
	city.calculateNetYields(aiYields, aiRawProducedYields, aiRawConsumedYields);
	int iUnproduced = city.getBaseRawYieldProduced(eYieldType) - aiRawProducedYields[eYieldType];
	if (iUnproduced > 0)
	{
		// R&R, ray , MYCP partially based on code of Aymerick - START
		std::vector<YieldTypes> eMissing;
		for (int i = 0; i < city.getPopulation(); ++i)
		{
			CvUnit* pUnit = city.getPopulationUnitByIndex(i);
			if (NULL != pUnit)
			{
				ProfessionTypes eProfession = pUnit->getProfession();
				if (eProfession != NO_PROFESSION)
				{
					for (int j = 0; j < GC.getProfessionInfo(eProfession).getNumYieldsProduced(); j++)
					{
						if (GC.getProfessionInfo(eProfession).getYieldsProduced(j) == eYieldType)
						{
							for (int k = 0; k < GC.getProfessionInfo(eProfession).getNumYieldsConsumed(); k++)
							{
								YieldTypes eYieldConsumed = (YieldTypes) GC.getProfessionInfo(eProfession).getYieldsConsumed(k);
								if (GC.getProfessionInfo(eProfession).getYieldsConsumed(k) != NO_YIELD)
								{
									if (city.getYieldStored(eYieldConsumed) < city.getRawYieldConsumed(eYieldConsumed))
									{
										eMissing.push_back((YieldTypes) GC.getProfessionInfo(eProfession).getYieldsConsumed(k));
									}
								}
							}
						}
					}
				}
			}
		}
		if (!eMissing.empty())
		{
			CvWString szYieldsList;
			for (std::vector<YieldTypes>::iterator it = eMissing.begin(); it != eMissing.end(); ++it)
			{
				if (!szYieldsList.empty())
				{
					if (*it == eMissing.back())
					{
						szYieldsList += CvWString::format(gDLL->getText("TXT_KEY_AND"));
					}
					else
					{
						szYieldsList += L", ";
					}
				}
				szYieldsList += CvWString::format(L"%c", GC.getYieldInfo(*it).getChar());
			}
			iBaseProduction -= iUnproduced;
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_MISC_UNPRODUCED_CITY_YIELD_SPECIFIC", -iUnproduced, info.getChar(), szYieldsList.GetCString()));
		}
		// R&R, ray , MYCP partially based on code of Aymerick - END
	}

	int iModifiedProduction = iBaseProduction;
	if (iBaseProduction != 0)
	{
		int iModifier = setCityYieldModifierString(szBuffer, eYieldType, city);
		if (iModifier != 100)
		{
			iModifiedProduction *= iModifier;
			iModifiedProduction /= 100;
		}

//		if (iModifiedProduction != iBaseProduction)
//		{
//			szBuffer.append(SEPARATOR);
//			szBuffer.append(NEWLINE);
//			szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_TOTAL_YIELD_PRODUCED", info.getTextKeyWide(), iModifiedProduction, info.getChar()));
//		}
	}

	// from immigration
	int iImmigration = 0;
	if (eYieldType != YIELD_CROSSES)
	{
		if (owner.getImmigrationConversion() == eYieldType)
		{
			iImmigration += aiYields[YIELD_CROSSES];
		}
	}
	if (iImmigration > 0)
	{
		iModifiedProduction += iImmigration;
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_BASE_CITIZEN_IMMIGRATION", iImmigration, info.getChar(), GC.getYieldInfo(YIELD_CROSSES).getChar()));
	}


	int iConsumed = aiRawConsumedYields[eYieldType];
	if (iConsumed > 0)
	{
		// R&R, ray , MYCP partially based on code of Aymerick - START
		std::vector<YieldTypes> eConverted;
		for (int i = 0; i < city.getPopulation(); ++i)
		{
			CvUnit* pUnit = city.getPopulationUnitByIndex(i);
			if (NULL != pUnit)
			{
				ProfessionTypes eProfession = pUnit->getProfession();
				if (eProfession != NO_PROFESSION)
				{
					
					for (int j = 0; j < GC.getProfessionInfo(eProfession).getNumYieldsConsumed(); j++)
					{
						if (GC.getProfessionInfo(eProfession).getYieldsConsumed(j) == eYieldType)
						{
							for (int k = 0; k < GC.getProfessionInfo(eProfession).getNumYieldsProduced(); k++)
							{
								if (GC.getProfessionInfo(eProfession).getYieldsProduced(k) != NO_YIELD)
								{
									eConverted.push_back((YieldTypes) GC.getProfessionInfo(eProfession).getYieldsProduced(k));
								}
							}
						}
					}
				}
			}
		}
		//food consumed
		int iTempConsumed = iConsumed;
		if (eYieldType == YIELD_FOOD)
		{
			int iFoodConsumed = city.foodConsumption();
			iTempConsumed -= iFoodConsumed;
			if (iFoodConsumed != 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_MISC_CONSUMED_FOOD", -iFoodConsumed));
			}
		}

		if (!eConverted.empty())
		{
			CvWString szYieldsList;
			for (std::vector<YieldTypes>::iterator it = eConverted.begin(); it != eConverted.end(); ++it)
			{
				if (!szYieldsList.empty())
				{
					if (*it == eConverted.back())
					{
						szYieldsList += CvWString::format(gDLL->getText("TXT_KEY_AND"));
					}
					else
					{
						szYieldsList += L", ";
					}
				}
				szYieldsList += CvWString::format(L"%c", GC.getYieldInfo(*it).getChar());
			}
			iBaseProduction -= iUnproduced;
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_MISC_CONSUMED_CITY_YIELD_SPECIFIC", -iTempConsumed, info.getChar(), szYieldsList.GetCString()));
		}
		// R&R, ray , MYCP partially based on code of Aymerick - END
	}

	iModifiedProduction -= iConsumed;

	szBuffer.append(SEPARATOR);
	szBuffer.append(NEWLINE);

	// R&R, ray, Health - START
	if (eYieldType == YIELD_HEALTH)
	{
		int iCityHealthChangeFromPopulation = -city.getCityHealthChangeFromPopulation();
		szBuffer.append(gDLL->getText("TXT_KEY_YIELD_HEALTH_POP_CHANGE", info.getTextKeyWide(), iCityHealthChangeFromPopulation, info.getChar()));

		szBuffer.append(SEPARATOR);
		szBuffer.append(NEWLINE);

		int iCityTotalHealthChange = city.getCityHealthChange();
		szBuffer.append(gDLL->getText("TXT_KEY_YIELD_HEALTH_TOTAL_CHANGE", info.getTextKeyWide(), iCityTotalHealthChange, info.getChar()));

		szBuffer.append(SEPARATOR);
		szBuffer.append(NEWLINE);

		int iTotalCityHealth = city.getCityHealth();
		szBuffer.append(gDLL->getText("TXT_KEY_CITY_HEALTH_TOTAL", info.getTextKeyWide(), iTotalCityHealth, info.getChar()));
	}
	// WTP, ray, Happiness - START
	// Code Happiness and Unhappiness YIELD Help
	else if (eYieldType == YIELD_HAPPINESS)
	{
		// this is not needed because already displayed
		/*int iProducedHappiness = city.calculateNetYield(YIELD_HAPPINESS);
		if (iProducedHappiness != 0)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_YIELD_HAPPINESS_PRODUCED", info.getTextKeyWide(), iProducedHappiness, info.getChar()));
			szBuffer.append(NEWLINE);
		}*/
		int iCrossesHappiness = city.getHappinessFromCrosses();
		if (iCrossesHappiness != 0)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_HAPPINESS_CROSSES", info.getTextKeyWide(), iCrossesHappiness, info.getChar()));
			szBuffer.append(NEWLINE);
		}
		int iBellsHappiness = city.getHappinessFromBells();
		if (iBellsHappiness != 0)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_HAPPINESS_BELLS", info.getTextKeyWide(), iBellsHappiness, info.getChar()));
			szBuffer.append(NEWLINE);
		}
		int iHealthHappiness = city.getHappinessFromHealth();
		if (iHealthHappiness != 0)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_HAPPINESS_HEALTH", info.getTextKeyWide(), iHealthHappiness, info.getChar()));
			szBuffer.append(NEWLINE);
		};
		int iCultureHappiness = city.getHappinessFromCulture();
		if (iCultureHappiness != 0)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_HAPPINESS_CULTURE", info.getTextKeyWide(), iCultureHappiness, info.getChar()));
			szBuffer.append(NEWLINE);
		}
		int iEducationHappiness = city.getHappinessFromEducation();
		if (iEducationHappiness != 0)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_HAPPINESS_EDUCATION", info.getTextKeyWide(), iEducationHappiness, info.getChar()));
			szBuffer.append(NEWLINE);
		}
		int iDomesticDemandHappiness = city.getHappinessFromDomesticDemandsFulfilled();
		if (iDomesticDemandHappiness != 0)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_HAPPINESS_DOMESTIC_DEMAND", info.getTextKeyWide(), iDomesticDemandHappiness, info.getChar()));
			szBuffer.append(NEWLINE);
		}
		int iTreatiesHappiness = city.getHappinessFromTreaties();
		if (iTreatiesHappiness != 0)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_HAPPINESS_TREATIES", info.getTextKeyWide(), iTreatiesHappiness, info.getChar()));
			szBuffer.append(NEWLINE);
		}

		szBuffer.append(SEPARATOR);
		szBuffer.append(NEWLINE);

		city.updateCityHappiness();
		int iTotalCityHappiness = city.getCityHappiness();
		szBuffer.append(gDLL->getText("TXT_KEY_TOTAL_CITY_HAPPINESS", info.getTextKeyWide(), iTotalCityHappiness, info.getChar()));

	}
	else if (eYieldType == YIELD_UNHAPPINESS)
	{
		// this is not needed because already displayed
		/*
		int iProducedUnHappiness = city.calculateNetYield(YIELD_UNHAPPINESS); // should not exist but in case it ever does
		if (iProducedUnHappiness != 0)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_YIELD_UNHAPPINESS_PRODUCED", info.getTextKeyWide(), iProducedUnHappiness, info.getChar()));
			szBuffer.append(NEWLINE);
		}*/
		int iPopulationUnHappiness = city.getUnhappinessFromPopulation();
		if (iPopulationUnHappiness != 0)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_UNHAPPINESS_POPULATION", info.getTextKeyWide(), iPopulationUnHappiness, info.getChar()));
			szBuffer.append(NEWLINE);
		}
		int iSlaveryUnHappiness = city.getUnhappinessFromSlavery();
		if (iSlaveryUnHappiness != 0)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_UNHAPPINESS_SLAVERY", info.getTextKeyWide(), iSlaveryUnHappiness, info.getChar()));
			szBuffer.append(NEWLINE);
		}
		int iWarsUnHappiness = city.getUnhappinessFromWars();
		if (iWarsUnHappiness != 0)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_UNHAPPINESS_WARS", info.getTextKeyWide(), iWarsUnHappiness, info.getChar()));
			szBuffer.append(NEWLINE);
		}
		int iMissingDefenseUnHappiness = city.getUnhappinessFromMissingDefense();
		if (iMissingDefenseUnHappiness != 0)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_UNHAPPINES_MISSING_DEFENSES", info.getTextKeyWide(), iMissingDefenseUnHappiness, info.getChar()));
			szBuffer.append(NEWLINE);
		}

		int iTaxRateUnHappiness = city.getUnhappinessFromTaxRate();
		if (iTaxRateUnHappiness != 0)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_UNHAPPINES_TAX_RATE", info.getTextKeyWide(), iTaxRateUnHappiness, info.getChar()));
			szBuffer.append(NEWLINE);
		}


		szBuffer.append(SEPARATOR);
		szBuffer.append(NEWLINE);

		city.updateCityUnHappiness();
		int iTotalCityUnHappiness = city.getCityUnHappiness();
		szBuffer.append(gDLL->getText("TXT_KEY_TOTAL_CITY_UNHAPPINESS", info.getTextKeyWide(), iTotalCityUnHappiness, info.getChar()));
	}
	// WTP, ray, Happiness - END
	else // old code
	{
		szBuffer.append(gDLL->getText("TXT_KEY_YIELD_TOTAL", info.getTextKeyWide(), iModifiedProduction, info.getChar()));
		FAssert(iModifiedProduction == aiYields[eYieldType]);
	}
	// R&R, ray, Health - END

	if (eYieldType == GC.getDefineINT("CULTURE_YIELD"))
	{
		szBuffer.append(SEPARATOR);
		szBuffer.append(NEWLINE);
		int iCulture = city.getCulture(city.getOwnerINLINE());
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_CULTURE", iCulture, city.getCultureThreshold(), info.getChar()));

		int iCultureRate = city.getCultureRate();
		if (iCultureRate > 0)
		{
			int iCultureLeft = city.getCultureThreshold() - iCulture;

			if (iCultureLeft > 0)
			{
				int iTurnsLeft = (iCultureLeft  + iCultureRate - 1) / iCultureRate;

				szBuffer.append(L' ');
				szBuffer.append(gDLL->getText("INTERFACE_CITY_TURNS", std::max(1, iTurnsLeft)));
			}
		}

		szBuffer.append(SEPARATOR);

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				int iCulturePercent = city.plot()->calculateCulturePercent((PlayerTypes)iI);
				if (iCulturePercent > 0)
				{
					CvWString szTempBuffer;
					szTempBuffer.Format(L"\n%d%% " SETCOLR L"%s" ENDCOLR, iCulturePercent, GET_PLAYER((PlayerTypes)iI).getPlayerTextColorR(), GET_PLAYER((PlayerTypes)iI).getPlayerTextColorG(), GET_PLAYER((PlayerTypes)iI).getPlayerTextColorB(), GET_PLAYER((PlayerTypes)iI).getPlayerTextColorA(), GET_PLAYER((PlayerTypes)iI).getCivilizationAdjective());
					szBuffer.append(szTempBuffer);
				}
			}
		}

		szBuffer.append(L"\n=======================\n");
	}

	if (gDLL->shiftKey() && (gDLL->getChtLvl() > 0))
	{
		szBuffer.append(CvWString::format(L"\nValue : %d", GET_PLAYER(city.getOwnerINLINE()).AI_yieldValue(eYieldType)));
		szBuffer.append(CvWString::format(L"\nLevel: %d", city.getMaintainLevel(eYieldType)));
		szBuffer.append(CvWString::format(L"\nTrade: %d", city.AI_getTradeBalance(eYieldType)));
		szBuffer.append(CvWString::format(L"\nAdvant: %d", city.AI_getYieldAdvantage(eYieldType)));
	}
}

int CvGameTextMgr::setCityYieldModifierString(CvWStringBuffer& szBuffer, YieldTypes eYieldType, const CvCity& kCity)
{
	CvYieldInfo& info = GC.getYieldInfo(eYieldType);
	CvPlayer& kOwner = GET_PLAYER(kCity.getOwnerINLINE());

	int iBaseModifier = 100;

	// Buildings
	int iBuildingMod = 0;
	for (int i = 0; i < GC.getNumBuildingInfos(); i++)
	{
		CvBuildingInfo& infoBuilding = GC.getBuildingInfo((BuildingTypes)i);
		if (kCity.isHasBuilding((BuildingTypes)i))
		{
			iBuildingMod += infoBuilding.getYieldModifier(eYieldType);
		}
	}
	if (NULL != kCity.area())
	{
		iBuildingMod += kCity.area()->getYieldRateModifier(kCity.getOwnerINLINE(), eYieldType);
	}
	if (0 != iBuildingMod)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_YIELD_BUILDINGS", iBuildingMod, info.getChar()));
		iBaseModifier += iBuildingMod;
	}

	// Capital
	if (kCity.isCapital())
	{
		int iCapitalMod = kOwner.getCapitalYieldRateModifier(eYieldType);
		if (0 != iCapitalMod)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_YIELD_CAPITAL", iCapitalMod, info.getChar()));
			iBaseModifier += iCapitalMod;
		}
	}

	// Civics
	int iCivicMod = 0;
	for (int i = 0; i < GC.getNumCivicOptionInfos(); i++)
	{
		if (NO_CIVIC != kOwner.getCivic((CivicOptionTypes)i))
		{
			iCivicMod += GC.getCivicInfo(kOwner.getCivic((CivicOptionTypes)i)).getYieldModifier(eYieldType);
		}
	}
	if (0 != iCivicMod)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_YIELD_CIVICS", iCivicMod, info.getChar()));
		iBaseModifier += iCivicMod;
	}

	// Founding Fathers
	for (int i = 0; i < GC.getNumTraitInfos(); i++)
	{
		if (GET_PLAYER(kCity.getOwnerINLINE()).hasTrait((TraitTypes) i))
		{
			CvTraitInfo& kTraitInfo = GC.getTraitInfo((TraitTypes) i);

			int iTraitMod = kTraitInfo.getYieldModifier(eYieldType);
			if (kTraitInfo.isTaxYieldModifier(eYieldType))
			{
				iTraitMod += kOwner.getTaxRate();
			}

			if (0 != iTraitMod)
			{
				iBaseModifier += iTraitMod;
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_YIELD_FATHER", iTraitMod, info.getChar(), kTraitInfo.getTextKeyWide()));
			}
		}
	}

	// WTP, ray, trying to fix Rebel Rate Modifier on Happiness for Balancing - START
	// just if condition added
	if (eYieldType != YIELD_HAPPINESS && eYieldType != YIELD_UNHAPPINESS)
	{
		int iRebelMod = kCity.getRebelPercent() * GC.getMAX_REBEL_YIELD_MODIFIER() / 100;
		if (0 != iRebelMod)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_YIELD_REBEL", iRebelMod, info.getChar()));
			iBaseModifier += iRebelMod;
		}

		// R&R, ray, Health - START
		int iHealthMod = kCity.getCityHealth();
		if (0 != iHealthMod)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_MISC_HELP_YIELD_HEALTH", iHealthMod, info.getChar()));
			iBaseModifier += iHealthMod;
		}
		// R&R, ray, Health - END

		// WTP, ray, Happiness - START
		// YIELD Modifier Helptext would be here - currently not planned
		
	}
	// WTP, ray, trying to fix Rebel Rate Modifier on Happiness for Balancing - END

	FAssertMsg(iBaseModifier == kCity.getBaseYieldRateModifier(eYieldType), "Yield Modifier in setProductionHelp does not agree with actual value");

	return iBaseModifier;
}

void CvGameTextMgr::parseGreatGeneralHelp(CvWStringBuffer &szBuffer, CvPlayer& kPlayer)
{
	szBuffer.assign(gDLL->getText("TXT_KEY_MISC_GREAT_GENERAL", kPlayer.getCombatExperience(), kPlayer.greatGeneralThreshold()));
}


//------------------------------------------------------------------------------------------------

// updated in CyGlobalContext.cpp (called from Domestic Advisor)
// do not use for anything other than the billboard debug display. It will turn buggy in no time if used all over the place.
int iGameFontDebugChar = -1;

void CvGameTextMgr::buildCityBillboardIconString( CvWStringBuffer& szBuffer, CvCity* pCity)
{
	szBuffer.clear();

	if (pCity->getMissionaryPlayer() != NO_PLAYER)
	{
		szBuffer.append(CvWString::format(L" %c", GC.getCivilizationInfo(GET_PLAYER(pCity->getMissionaryPlayer()).getCivilizationType()).getMissionaryChar()));
	}

	// WTP, ray, Native Trade Posts - START
	if (pCity->getTradePostPlayer() != NO_PLAYER)
	{
		szBuffer.append(CvWString::format(L" %c", GC.getYieldInfo(YIELD_TRADE_GOODS).getChar()));
	}
	// WTP, ray, Native Trade Posts - END


	// WTP, ray, improvement Native Trade Indicator, issue #85 - START
	CvPlayer& cityOwnerPlayer =	GET_PLAYER(pCity->getOwnerINLINE());
	if (cityOwnerPlayer.isNative())
	{
		int timeNoTrade= cityOwnerPlayer.getTimeNoTrade();
		if (timeNoTrade > 0)
		{
			szBuffer.append(gDLL->getText("TXT_KEY_CITY_TIME_NO_TRADE", timeNoTrade));
		}
	}
	// WTP, ray, improvement Native Trade Indicator, issue #85 - END

	// WTP, ray, Happiness - START
	// only for Europeans
	if (!cityOwnerPlayer.isNative())
	{
		int iHappiness = pCity->getCityHappiness();
		int iUnHappiness = pCity->getCityUnHappiness();
		if (iHappiness > iUnHappiness)
		{
			szBuffer.append(CvWString::format(L" %c", GC.getYieldInfo(YIELD_HAPPINESS).getChar()));
		}
		if (iUnHappiness > iHappiness)
		{
			szBuffer.append(CvWString::format(L" %c", GC.getYieldInfo(YIELD_UNHAPPINESS).getChar()));
		}
	}
	// WTP, ray, Happiness - END

	// XXX out this in bottom bar???
	if (pCity->isOccupation())
	{
		szBuffer.append(CvWString::format(L" (%c:%d)", gDLL->getSymbolID(OCCUPATION_CHAR), pCity->getOccupationTimer()));
	}

	if (pCity->isVisible(GC.getGameINLINE().getActiveTeam(), true))
	{
		//stored arms
		CvWStringBuffer szTemp;
		std::vector<int> aYieldShown(NUM_YIELD_TYPES, 0);
		for (int iProfession = 0; iProfession < GC.getNumProfessionInfos(); ++iProfession)
		{
			ProfessionTypes eProfession = (ProfessionTypes) iProfession;

			if (GC.getProfessionInfo(eProfession).isCityDefender() && GC.getCivilizationInfo(pCity->getCivilizationType()).isValidProfession(eProfession))
			{
				for(int iYield=0;iYield<NUM_YIELD_TYPES;iYield++)
				{
					YieldTypes eYield = (YieldTypes) iYield;
					int iYieldEquipment = GET_PLAYER(pCity->getOwnerINLINE()).getYieldEquipmentAmount(eProfession, eYield);
					if (iYieldEquipment > 0 && pCity->getYieldStored(eYield) >= iYieldEquipment)
					{
						if (aYieldShown[iYield] == 0)
						{
							aYieldShown[iYield] = 1;
							szTemp.append(CvWString::format(L"%c", GC.getYieldInfo(eYield).getChar()));
						}
					}
				}
			}
		}

		if(!szTemp.isEmpty())
		{
			szBuffer.append(L" ");
			szBuffer.append(szTemp);
		}

		int iDefenseModifier = pCity->getDefenseModifier();
		if (iDefenseModifier != 0)
		{
			szBuffer.append(CvWString::format(L" %c:%s%d%%", gDLL->getSymbolID(DEFENSE_CHAR), ((iDefenseModifier > 0) ? "+" : ""), iDefenseModifier));
		}

		if (pCity->getRebelPercent() > 0)
		{
			szBuffer.append(CvWString::format(L" %c:%d%%", gDLL->getSymbolID(POWER_CHAR), pCity->getRebelPercent()));
		}
	}

	// GameFont debug display
	if (iGameFontDebugChar > -1)
	{
		szBuffer.append(CvWString::format(L"\n\n\n"));
		for (int i = 0; i <= 80; i += 10)
		{
			for (int j = 0; j < 10; ++j)
			{
				int iChar = iGameFontDebugChar + i + j;
				szBuffer.append(CvWString::format(L"%d: %c ", iChar, iChar));
			}
			szBuffer.append(CvWString::format(L"\n"));
		}
	}
}

void CvGameTextMgr::buildCityBillboardCityNameString( CvWStringBuffer& szBuffer, CvCity* pCity)
{
	szBuffer.assign(pCity->getName());
}

void CvGameTextMgr::buildCityBillboardProductionString( CvWStringBuffer& szBuffer, CvCity* pCity)
{
	szBuffer.clear();

	PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
	if (pCity->isNative() && ePlayer != pCity->getOwnerINLINE() && ePlayer != NO_PLAYER)
	{
		UnitClassTypes eUnitClass = pCity->getTeachUnitClass();
		if (eUnitClass != NO_UNITCLASS)
		{
			UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getCivilizationUnits(eUnitClass);
			if (eUnit != NO_UNIT)
			{
				szBuffer.append(GC.getUnitInfo(eUnit).getDescription());
			}
		}
	}
	else if (pCity->getOrderQueueLength() > 0)
	{
		szBuffer.append(pCity->getProductionName());
	}
}


void CvGameTextMgr::buildCityBillboardCitySizeString( CvWStringBuffer& szBuffer, CvCity* pCity, const NiColorA& kColor)
{
#define CAPARAMS(c) (int)((c).r * 255.0f), (int)((c).g * 255.0f), (int)((c).b * 255.0f), (int)((c).a * 255.0f)
	szBuffer.assign(CvWString::format(SETCOLR L"%d" ENDCOLR, CAPARAMS(kColor), pCity->getPopulation()));
#undef CAPARAMS
}

void CvGameTextMgr::setScoreHelp(CvWStringBuffer &szString, PlayerTypes ePlayer)
{
	if (NO_PLAYER != ePlayer)
	{
		CvPlayer& player = GET_PLAYER(ePlayer);

		int iPopScore = 0;
		int iPop = player.getPopScore();
		int iMaxPop = GC.getGameINLINE().getMaxPopulation();
		if (iMaxPop > 0)
		{
			iPopScore = (GC.getDefineINT("SCORE_POPULATION_FACTOR") * iPop) / iMaxPop;
		}

		int iLandScore = 0;
		int iLand = player.getLandScore();
		int iMaxLand = GC.getGameINLINE().getMaxLand();
		if (iMaxLand > 0)
		{
			iLandScore = (GC.getDefineINT("SCORE_LAND_FACTOR") * iLand) / iMaxLand;
		}

		int iFatherScore = 0;
		int iFather = player.getFatherScore();
		int iMaxFather = GC.getGameINLINE().getMaxFather();
		iFatherScore = (GC.getDefineINT("SCORE_FATHER_FACTOR") * iFather) / iMaxFather;

		int iScoreTaxFactor = player.getScoreTaxFactor();
		int iSubTotal = iPopScore + iLandScore + iFatherScore;
		int iTotalScore = iSubTotal * iScoreTaxFactor / 100;

		int iVictoryScore = player.calculateScore(true, true);
		if (iTotalScore == player.calculateScore())
		{
			if (GC.getDefineINT("SCORE_POPULATION_FACTOR") > 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_SCORE_BREAKDOWN_POPULATION", iPopScore, iPop, iMaxPop));
			}
			if (GC.getDefineINT("SCORE_LAND_FACTOR") > 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_SCORE_BREAKDOWN_LAND", iLandScore, iLand, iMaxLand));
			}
			if (GC.getDefineINT("SCORE_LAND_FACTOR") > 0)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_SCORE_BREAKDOWN_FATHERS", iFatherScore, iFather, iMaxFather));
			}

			if (iScoreTaxFactor < 100)
			{
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_SCORE_BREAKDOWN_TAX", iTotalScore - iSubTotal));
			}

			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_SCORE_BREAKDOWN_TOTAL", iTotalScore, iVictoryScore));
		}
	}
}

void CvGameTextMgr::setCitizenHelp(CvWStringBuffer &szString, const CvCity& kCity, const CvUnit& kUnit)
{
	// WTP, ray, showing Profession Name in Citizen Help instead of Unit Name - START
	if(kUnit.getProfession() != NO_PROFESSION) 
	{
		szString.append(gDLL->getText("TXT_KEY_CITIZEN_HELP_PROFESSION_NAME_DISPLAY", GC.getProfessionInfo(kUnit.getProfession()).getTextKeyWide()));
	}

	else
	{
		szString.append(gDLL->getText("TXT_KEY_CITIZEN_HELP_UNIT_NAME_DISPLAY", kUnit.getNameKey()));
	}
	// WTP, ray, showing Profession Name in Citizen Help instead of Unit Name - START

	PlayerTypes ePlayer = kCity.getOwnerINLINE();
	if (ePlayer == NO_PLAYER)
	{
		return;
	}

	for (int iI = 0; iI < GC.getNumPromotionInfos(); ++iI)
	{
		if (!GC.getPromotionInfo((PromotionTypes)iI).isGraphicalOnly() && kUnit.isHasRealPromotion((PromotionTypes) iI))
		{
			szString.append(CvWString::format(L"<img=%S size=16></img>", GC.getPromotionInfo((PromotionTypes)iI).getButton()));
		}
	}

	ProfessionTypes eProfession = kUnit.getProfession();
	if (NO_PROFESSION != eProfession)
	{
		CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);
		// R&R, ray, adjustment to fix display in help
		YieldTypes eProfessionMainYield = (YieldTypes) kProfession.getYieldsProduced(0);
		// R&R, ray , MYCP partially based on code of Aymerick - START
		for (int i = 0; i < kProfession.getNumYieldsProduced(); i++)
		{
			YieldTypes eProfessionYield = (YieldTypes) kProfession.getYieldsProduced(i);
			if (NO_YIELD != eProfessionYield)
			{
				int iProfessionYieldChar = GC.getYieldInfo(eProfessionYield).getChar();
				int iYieldAmount = 0;
				CvPlot* pWorkingPlot = kCity.getPlotWorkedByUnit(&kUnit);
				if (NULL != pWorkingPlot)
				{
					// R&R, ray, adjustment to fix display in help
					if (i == 0)
					{
						iYieldAmount = pWorkingPlot->calculatePotentialYield(eProfessionMainYield, &kUnit, false);
					}
					else
					{
						iYieldAmount = pWorkingPlot->calculatePotentialYield(eProfessionMainYield, &kUnit, false) / 2;
					}
				}
				else
				{
					iYieldAmount = kCity.getProfessionOutput(eProfession, &kUnit);
				}
				szString.append(NEWLINE);
				szString.append(gDLL->getText("TXT_KEY_MISC_HELP_BASE_CITIZEN_YIELD", iYieldAmount, iProfessionYieldChar, kUnit.getNameKey()));
				int iModifier = setCityYieldModifierString(szString, eProfessionYield, kCity);
				int iTotalYieldTimes100 = iModifier * iYieldAmount;
				if (iTotalYieldTimes100 > 0)
				{
					szString.append(SEPARATOR);
					szString.append(NEWLINE);
					CvWString szNumber = CvWString::format(L"%d.%02d", iTotalYieldTimes100 / 100, iTotalYieldTimes100 % 100);
					szString.append(gDLL->getText("TXT_KEY_YIELD_TOTAL_FLOAT", GC.getYieldInfo(eProfessionYield).getTextKeyWide(), szNumber.GetCString(), iProfessionYieldChar));
					szString.append(SEPARATOR);
				}
				if (eProfessionYield == YIELD_EDUCATION)
				{
					int iEducationProduced = kCity.calculateNetYield(YIELD_EDUCATION);
					if (iEducationProduced > 0)
					{
						int iEducationNeeded = kCity.educationThreshold() - kUnit.getYieldStored();
						int iStudentOutput = kCity.getProfessionOutput(kUnit.getProfession(), &kUnit, NULL) * kCity.getBaseYieldRateModifier(YIELD_EDUCATION) / 100;
						iStudentOutput = std::max(iStudentOutput, 1);
						int iTurns = std::max(0, (iEducationNeeded + iStudentOutput - 1) / iStudentOutput);  // round up
						szString.append(SEPARATOR);
						szString.append(NEWLINE);
						szString.append(gDLL->getText("TXT_KEY_MISC_HELP_STUDENT", iTurns));
					}
					for (int iUnit = 0; iUnit < GC.getNumUnitInfos(); ++iUnit)
					{
						int iPrice = kCity.getSpecialistTuition((UnitTypes) iUnit);
						if (iPrice >= 0)
						{
							szString.append(NEWLINE);
							szString.append(gDLL->getText("TXT_KEY_MISC_HELP_GRADUATION_CHANCE", GC.getUnitInfo((UnitTypes) iUnit).getTextKeyWide(), iPrice));
						}
					}
				}
			}
		}
		// R&R, ray , MYCP partially based on code of Aymerick - END
	}

	// WTP, ray, showing turns worked for becoming expert or free - START
	// partially used code of devolution
	const ProfessionTypes lastProfession = kUnit.getLastLbDProfession();
	bool bCanBecomeExpert = kUnit.getUnitInfo().LbD_canBecomeExpert();
	bool bCanGetFree = kUnit.getUnitInfo().LbD_canGetFree();
	int iLbDRoundsWorked= kUnit.getLbDrounds();

	// Display for become Expert - with turns worked and Expert Unit in Text
	if(bCanBecomeExpert && lastProfession != NO_PROFESSION && GC.getProfessionInfo(lastProfession).LbD_isUsed() && iLbDRoundsWorked >0)
	{
		int expert = GC.getProfessionInfo(lastProfession).LbD_getExpert();
		UnitTypes expertUnitType = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getCivilizationType()).getCivilizationUnits(expert);
		szString.append(NEWLINE);
		szString.append(gDLL->getText("TXT_KEY_MISC_HELP_LBD_BECOME_EXPPERT_TURNS_WORKED", iLbDRoundsWorked, GC.getUnitInfo(expertUnitType).getDescription()));
		szString.append(SEPARATOR);
	}

	// WTP, ray, saving 1 more Profession for Fisher Issue - START
	const ProfessionTypes lastProfessionBefore = kUnit.getLastLbDProfessionBefore();
	int iLbDRoundsWorkedBefore= kUnit.getLbDroundsBefore();

	// Display for become Expert - with turns worked and Expert Unit in Text
	if(bCanBecomeExpert && lastProfessionBefore != NO_PROFESSION && GC.getProfessionInfo(lastProfessionBefore).LbD_isUsed() && iLbDRoundsWorkedBefore >0)
	{
		int expertBefore = GC.getProfessionInfo(lastProfessionBefore).LbD_getExpert();
		UnitTypes expertUnitTypeBefore = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getCivilizationType()).getCivilizationUnits(expertBefore);
		szString.append(NEWLINE);
		szString.append(gDLL->getText("TXT_KEY_MISC_HELP_LBD_BECOME_EXPPERT_TURNS_WORKED", iLbDRoundsWorkedBefore, GC.getUnitInfo(expertUnitTypeBefore).getDescription()));
		szString.append(SEPARATOR);
	}


	// Display for become Free - with Expert Unit in Text
	if(bCanGetFree && lastProfession != NO_PROFESSION && GC.getProfessionInfo(lastProfession).LbD_isUsed() && iLbDRoundsWorked >0)
	{
		szString.append(NEWLINE);
		szString.append(gDLL->getText("TXT_KEY_MISC_HELP_LBD_BECOME_FREE_TURNS_WORKED", iLbDRoundsWorked));
		szString.append(SEPARATOR);
	}
	// WTP, ray, showing turns worked for becoming expert or free - END

	// WTP, ray, showing citizen production modifiers - START
	// using Code of devolution to display Yield Modifiers
	std::pair<int, int> yieldModifiers[NUM_YIELD_TYPES];
	std::pair<int, int> yieldChanges[NUM_YIELD_TYPES];


	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		yieldModifiers[iYield].first = kUnit.getUnitInfo().getYieldModifier(iYield);
		yieldModifiers[iYield].second = iYield;

		yieldChanges[iYield].first = kUnit.getUnitInfo().getYieldChange(iYield);
		yieldChanges[iYield].second = iYield;
	}

	// TODO: use static cast
	std::sort(yieldModifiers, yieldModifiers + (int)NUM_YIELD_TYPES, std::greater<std::pair<int,int> >());
	std::sort(yieldChanges, yieldChanges + (int)NUM_YIELD_TYPES, std::greater<std::pair<int, int> >());

	int last = INT_MIN;

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{	
		const int iYieldModifier = yieldModifiers[iYield].first;

		if (iYieldModifier != 0 && iYieldModifier != last)
		{
			// Display modifer
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_PRODUCTION_PERCENT_MODIFIER_CITIZENHELP", iYieldModifier));
			last = iYieldModifier;
			szString.append(NEWLINE);
		}
		if (iYieldModifier != 0)
		{
			// Display yield icon
			szString.append(CvWString::format(L" %c",  GC.getYieldInfo((YieldTypes)yieldModifiers[iYield].second).getChar()));
		}
	}

	last = INT_MIN;


	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		const int iYieldChange = yieldChanges[iYield].first;

		if (iYieldChange != 0 && iYieldChange != last)
		{
			// Display modifer
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_PRODUCTION_PLUS_MODIFIER_CITIZENHELP", iYieldChange));
			last = iYieldChange;
			szString.append(NEWLINE);
		}
		if (iYieldChange != 0)
		{
			// Display yield icon
			szString.append(CvWString::format(L" %c", GC.getYieldInfo((YieldTypes)yieldChanges[iYield].second).getChar()));
		}
	}
	// WTP, ray, showing citizen production modifiers - END

	if ((gDLL->getChtLvl() > 0) && gDLL->shiftKey())
	{
		CvPlayer& kOwner = GET_PLAYER(kCity.getOwnerINLINE());
		szString.append(CvWString::format(L"\nID = %d Count = %d", kUnit.getID(), kOwner.getUnitClassCount(kUnit.getUnitClassType())));
		for (int iI = 0; iI < GC.getNumProfessionInfos(); iI++)
		{
			ProfessionTypes eLoopProfession = (ProfessionTypes) iI;
			if (GC.getCivilizationInfo(kCity.getCivilizationType()).isValidProfession(eLoopProfession))
			{
				int iValue = kCity.AI_professionValue(eLoopProfession, &kUnit, GC.getProfessionInfo(eLoopProfession).isWorkPlot() ? kCity.getPlotWorkedByUnit(&kUnit) : NULL, NULL);
				int iViability = GET_PLAYER(kUnit.getOwnerINLINE()).AI_professionSuitability(&kUnit, eLoopProfession, GC.getProfessionInfo(eLoopProfession).isWorkPlot() ? kCity.getPlotWorkedByUnit(&kUnit) : kCity.plot());

				if (iValue > 0)
				{
					// R&R, ray , MYCP partially based on code of Aymerick - START
					int iYieldChar = GC.getYieldInfo((YieldTypes)GC.getProfessionInfo(eLoopProfession).getYieldsProduced(0)).getChar();
					szString.append(CvWString::format(L"\n %s (%c) = %d (V:%d)", GC.getProfessionInfo(eLoopProfession).getDescription(), iYieldChar, iValue, iViability));
					// R&R, ray , MYCP partially based on code of Aymerick - END
				}
				else if (iViability > 0)
				{
					szString.append(CvWString::format(L"\n %s = %d (V:%d)", GC.getProfessionInfo(eLoopProfession).getDescription(), -1, iViability));
				}
			}
		}
	}

	if ((gDLL->getChtLvl() > 0) && gDLL->shiftKey())
	{
		CvUnit* pSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();

		if ((pSelectedUnit != NULL) && (pSelectedUnit != &kUnit))
		{
			int iValue = kCity.AI_professionValue(kUnit.getProfession(), pSelectedUnit, kCity.getPlotWorkedByUnit(&kUnit), &kUnit);
			szString.append(CvWString::format(L"\n Selected = %d", iValue));
		}
	}
}

void CvGameTextMgr::setEuropeYieldSoldHelp(CvWStringBuffer &szString, const CvPlayer& kPlayer, YieldTypes eYield, int iAmount, int iCommission)
{
	FAssert(kPlayer.getParent() != NO_PLAYER);
	CvPlayer& kPlayerEurope = GET_PLAYER(kPlayer.getParent());

	int iGross = iAmount;
	if (eYield != NO_YIELD)
	{
		iGross *= kPlayerEurope.getYieldBuyPrice(eYield);
		szString.append(gDLL->getText("TXT_KEY_YIELD_SOLD", iAmount, GC.getYieldInfo(eYield).getChar(), kPlayerEurope.getYieldBuyPrice(eYield), iGross));
	}
	else
	{
		szString.append(gDLL->getText("TXT_KEY_TREASURE_DELIVERED", iGross));
	}

	// R&R, ray, Smuggling - START
	if (iCommission == GC.getDefineINT("SMUGGLING_BRIBE_RATE"))
	{
		int iCommissionGold = iGross * iCommission / 100;
		iGross -= iCommissionGold;
		szString.append(NEWLINE);
		szString.append(gDLL->getText("TXT_KEY_YIELD_BRIBE", iCommission, iCommissionGold));
	}
	else
	{
		if (iCommission != 0)
		{
			int iCommissionGold = iGross * iCommission / 100;
			iGross -= iCommissionGold;
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_YIELD_COMMISSION", iCommission, iCommissionGold));
		}
		if (kPlayer.getTaxRate() != 0)
		{
			int iTaxGold = iGross * kPlayer.getTaxRate() / 100;
			iGross -= iTaxGold;
			szString.append(NEWLINE);
			szString.append(gDLL->getText("TXT_KEY_YIELD_TAX", kPlayer.getTaxRate(), iTaxGold));
		}
	}
	// R&R, ray, Smuggling - END

	FAssert(eYield == NO_YIELD || kPlayer.getSellToEuropeProfit(eYield, iAmount * (100 - iCommission) / 100) == iGross);
	szString.append(NEWLINE);
	szString.append(gDLL->getText("TXT_KEY_YIELD_NET_PROFIT", iGross));
}

void CvGameTextMgr::setEuropeYieldBoughtHelp(CvWStringBuffer &szString, const CvPlayer& kPlayer, YieldTypes eYield, int iAmount)
{
	FAssert(kPlayer.getParent() != NO_PLAYER);
	CvPlayer& kPlayerEurope = GET_PLAYER(kPlayer.getParent());
	int iGross = kPlayerEurope.getYieldSellPrice(eYield) * iAmount;
	szString.append(gDLL->getText("TXT_KEY_YIELD_BOUGHT", iAmount, GC.getYieldInfo(eYield).getChar(), kPlayerEurope.getYieldSellPrice(eYield), iGross));
}

void CvGameTextMgr::setEventHelp(CvWStringBuffer& szBuffer, EventTypes eEvent, int iEventTriggeredId, PlayerTypes ePlayer)
{
	//Barthoze : TIME IS THE ISSUE
	int iGrowthPercent = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
	if (NO_EVENT == eEvent || NO_PLAYER == ePlayer)
	{
		return;
	}

	CvEventInfo& kEvent = GC.getEventInfo(eEvent);
	CvPlayer& kActivePlayer = GET_PLAYER(ePlayer);
	EventTriggeredData* pTriggeredData = kActivePlayer.getEventTriggered(iEventTriggeredId);

	if (NULL == pTriggeredData)
	{
		return;
	}

	CvCity* pCity = kActivePlayer.getCity(pTriggeredData->m_iCityId);
	CvCity* pOtherPlayerCity = NULL;
	CvPlot* pPlot = GC.getMapINLINE().plot(pTriggeredData->m_iPlotX, pTriggeredData->m_iPlotY);
	CvUnit* pUnit = kActivePlayer.getUnit(pTriggeredData->m_iUnitId);

	if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
	{
		pOtherPlayerCity = GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCity(pTriggeredData->m_iOtherPlayerCityId);
	}

	CvWString szCity = gDLL->getText("TXT_KEY_EVENT_THE_CITY");
	if (NULL != pCity && kEvent.isCityEffect())
	{
		szCity = pCity->getNameKey();
	}
	else if (NULL != pOtherPlayerCity && kEvent.isOtherPlayerCityEffect())
	{
		szCity = pOtherPlayerCity->getNameKey();
	}

	CvWString szUnit = gDLL->getText("TXT_KEY_EVENT_THE_UNIT");
	if (NULL != pUnit)
	{
		szUnit = pUnit->getNameOrProfessionKey();
	}

	eventGoldHelp(szBuffer, eEvent, ePlayer, pTriggeredData->m_eOtherPlayer);

	if (kEvent.getFood() != 0)
	{
		int iFood = kEvent.getFood();
		iFood *= iGrowthPercent;
		iFood /=100;
		if (kEvent.isCityEffect() || kEvent.isOtherPlayerCityEffect())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_FOOD_CITY", iFood, szCity.GetCString()));
		}
		else
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_FOOD", iFood));
		}
	}

	if (kEvent.getFoodPercent() != 0)
	{
		if (kEvent.isCityEffect() || kEvent.isOtherPlayerCityEffect())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_FOOD_PERCENT_CITY", kEvent.getFoodPercent(), szCity.GetCString()));
		}
		else
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_FOOD_PERCENT", kEvent.getFoodPercent()));
		}
	}

	if (kEvent.getRevoltTurns() > 0)
	{
		int iRevolt = kEvent.getRevoltTurns();
		iRevolt *= iGrowthPercent;
		iRevolt /=100;
		if (kEvent.isCityEffect() || kEvent.isOtherPlayerCityEffect())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_REVOLT_TURNS",iRevolt, szCity.GetCString()));
		}
	}

	if (kEvent.getMaxPillage() > 0)
	{
		if (kEvent.isCityEffect() || kEvent.isOtherPlayerCityEffect())
		{
			if (kEvent.getMaxPillage() == kEvent.getMinPillage())
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_EVENT_PILLAGE_CITY", kEvent.getMinPillage(), szCity.GetCString()));
			}
			else
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_EVENT_PILLAGE_RANGE_CITY", kEvent.getMinPillage(), kEvent.getMaxPillage(), szCity.GetCString()));
			}
		}
		else
		{
			if (kEvent.getMaxPillage() == kEvent.getMinPillage())
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_EVENT_PILLAGE", kEvent.getMinPillage()));
			}
			else
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_EVENT_PILLAGE_RANGE", kEvent.getMinPillage(), kEvent.getMaxPillage()));
			}
		}
	}

	if (kEvent.getPopulationChange() != 0)
	{
		if (kEvent.isCityEffect() || kEvent.isOtherPlayerCityEffect())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_POPULATION_CHANGE_CITY", kEvent.getPopulationChange(), szCity.GetCString()));
		}
		else
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_POPULATION_CHANGE", kEvent.getPopulationChange()));
		}
	}

	if (kEvent.getCulture() != 0)
	{
		// WTP, ray, fixed Bug, in Event Texts for Cuture - START
		// int iCulture = kEvent.getRevoltTurns();
		int iCulture = kEvent.getCulture();
		// WTP, ray, fixed Bug, in Event Texts for Cuture - END
		iCulture *= iGrowthPercent;
		iCulture /=100;
		if (kEvent.isCityEffect() || kEvent.isOtherPlayerCityEffect())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_CULTURE_CITY", iCulture, szCity.GetCString()));
		}
		else
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_CULTURE", iCulture));
		}
	}

	// R&R, ray, change for Health in Events
	if (kEvent.getHealth() != 0)
	{
		if (kEvent.isCityEffect() || kEvent.isOtherPlayerCityEffect())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_HEALTH_CITY", kEvent.getHealth(), szCity.GetCString()));
		}
		else
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_HEALTH", kEvent.getHealth()));
		}
	}


	if (kEvent.getUnitClass() != NO_UNITCLASS)
	{
		CivilizationTypes eCiv = kActivePlayer.getCivilizationType();
		if (NO_CIVILIZATION != eCiv)
		{
			UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(eCiv).getCivilizationUnits(kEvent.getUnitClass());
			if (eUnit != NO_UNIT)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_EVENT_BONUS_UNIT", kEvent.getNumUnits(), GC.getUnitInfo(eUnit).getTextKeyWide()));
			}
		}
	}

	if (kEvent.getBuildingClass() != NO_BUILDINGCLASS)
	{
		CivilizationTypes eCiv = kActivePlayer.getCivilizationType();
		if (NO_CIVILIZATION != eCiv)
		{
			BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(eCiv).getCivilizationBuildings(kEvent.getBuildingClass());
			if (eBuilding != NO_BUILDING)
			{
				if (kEvent.getBuildingChange() > 0)
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_BONUS_BUILDING", GC.getBuildingInfo(eBuilding).getTextKeyWide()));
				}
				else if (kEvent.getBuildingChange() < 0)
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_REMOVE_BUILDING", GC.getBuildingInfo(eBuilding).getTextKeyWide()));
				}
			}
		}
	}

	if (kEvent.getNumBuildingYieldChanges() > 0)
	{
		CvWStringBuffer szYield;
		for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
		{
			CivilizationTypes eCiv = kActivePlayer.getCivilizationType();
			if (NO_CIVILIZATION != eCiv)
			{
				BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(eCiv).getCivilizationBuildings(iBuildingClass);
				if (eBuilding != NO_BUILDING)
				{
					int aiYields[NUM_YIELD_TYPES];
					for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
					{
						aiYields[iYield] = kEvent.getBuildingYieldChange(iBuildingClass, iYield);
					}

					szYield.clear();
					setYieldChangeHelp(szYield, L"", L"", L"", aiYields, false, false);
					if (!szYield.isEmpty())
					{
						szBuffer.append(NEWLINE);
						szBuffer.append(gDLL->getText("TXT_KEY_EVENT_YIELD_CHANGE_BUILDING", GC.getBuildingInfo(eBuilding).getTextKeyWide(), szYield.getCString()));
					}
				}
			}
		}
	}

	if (kEvent.getFeatureChange() > 0)
	{
		if (kEvent.getFeature() != NO_FEATURE)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_FEATURE_GROWTH", GC.getFeatureInfo((FeatureTypes)kEvent.getFeature()).getTextKeyWide()));
		}
	}
	else if (kEvent.getFeatureChange() < 0)
	{
		if (NULL != pPlot && NO_FEATURE != pPlot->getFeatureType())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_FEATURE_REMOVE", GC.getFeatureInfo(pPlot->getFeatureType()).getTextKeyWide()));
		}
	}

	if (kEvent.getImprovementChange() > 0)
	{
		if (kEvent.getImprovement() != NO_IMPROVEMENT)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_IMPROVEMENT_GROWTH", GC.getImprovementInfo((ImprovementTypes)kEvent.getImprovement()).getTextKeyWide()));
		}
	}
	else if (kEvent.getImprovementChange() < 0)
	{
		if (NULL != pPlot && NO_IMPROVEMENT != pPlot->getImprovementType())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_IMPROVEMENT_REMOVE", GC.getImprovementInfo(pPlot->getImprovementType()).getTextKeyWide()));
		}
	}

	if (kEvent.getRouteChange() > 0)
	{
		if (kEvent.getRoute() != NO_ROUTE)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_ROUTE_GROWTH", GC.getRouteInfo((RouteTypes)kEvent.getRoute()).getTextKeyWide()));
		}
	}
	else if (kEvent.getRouteChange() < 0)
	{
		if (NULL != pPlot && NO_ROUTE != pPlot->getRouteType())
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_ROUTE_REMOVE", GC.getRouteInfo(pPlot->getRouteType()).getTextKeyWide()));
		}
	}

	int aiYields[NUM_YIELD_TYPES];
	for (int i = 0; i < NUM_YIELD_TYPES; ++i)
	{
		aiYields[i] = kEvent.getPlotExtraYield(i);
	}

	CvWStringBuffer szYield;
	setYieldChangeHelp(szYield, L"", L"", L"", aiYields, false, false);
	if (!szYield.isEmpty())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_EVENT_YIELD_CHANGE_PLOT", szYield.getCString()));
	}

	if (NO_BONUS != kEvent.getBonusRevealed())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_EVENT_BONUS_REVEALED", GC.getBonusInfo((BonusTypes)kEvent.getBonusRevealed()).getTextKeyWide()));
	}

	if (0 != kEvent.getUnitExperience())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_EVENT_UNIT_EXPERIENCE", kEvent.getUnitExperience(), szUnit.GetCString()));
	}

	if (0 != kEvent.isDisbandUnit())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_EVENT_UNIT_DISBAND", szUnit.GetCString()));
	}

	if (NO_PROMOTION != kEvent.getUnitPromotion())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_EVENT_UNIT_PROMOTION", szUnit.GetCString(), GC.getPromotionInfo((PromotionTypes)kEvent.getUnitPromotion()).getTextKeyWide()));
	}

	for (int i = 0; i < GC.getNumUnitCombatInfos(); ++i)
	{
		if (NO_PROMOTION != kEvent.getUnitCombatPromotion(i))
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_UNIT_COMBAT_PROMOTION", GC.getUnitCombatInfo((UnitCombatTypes)i).getTextKeyWide(), GC.getPromotionInfo((PromotionTypes)kEvent.getUnitCombatPromotion(i)).getTextKeyWide()));
		}
	}

	for (int i = 0; i < GC.getNumUnitClassInfos(); ++i)
	{
		if (NO_PROMOTION != kEvent.getUnitClassPromotion(i))
		{
			UnitTypes ePromotedUnit = ((UnitTypes)(GC.getCivilizationInfo(kActivePlayer.getCivilizationType()).getCivilizationUnits(i)));
			if (NO_UNIT != ePromotedUnit)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_EVENT_UNIT_CLASS_PROMOTION", GC.getUnitInfo(ePromotedUnit).getTextKeyWide(), GC.getPromotionInfo((PromotionTypes)kEvent.getUnitClassPromotion(i)).getTextKeyWide()));
			}
		}
	}

	if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
	{
		if (kEvent.getAttitudeModifier() > 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_ATTITUDE_GOOD", kEvent.getAttitudeModifier(), GET_PLAYER(pTriggeredData->m_eOtherPlayer).getNameKey()));
		}
		else if (kEvent.getAttitudeModifier() < 0)
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_EVENT_ATTITUDE_BAD", kEvent.getAttitudeModifier(), GET_PLAYER(pTriggeredData->m_eOtherPlayer).getNameKey()));
		}
	}

	if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
	{
		TeamTypes eWorstEnemy = GET_TEAM(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam()).AI_getWorstEnemy();
		if (NO_TEAM != eWorstEnemy)
		{
			if (kEvent.getTheirEnemyAttitudeModifier() > 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_EVENT_ATTITUDE_GOOD", kEvent.getTheirEnemyAttitudeModifier(), GET_TEAM(eWorstEnemy).getName().GetCString()));
			}
			else if (kEvent.getTheirEnemyAttitudeModifier() < 0)
			{
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_EVENT_ATTITUDE_BAD", kEvent.getTheirEnemyAttitudeModifier(), GET_TEAM(eWorstEnemy).getName().GetCString()));
			}
		}
	}

	if (kEvent.isDeclareWar())
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_EVENT_DECLARE_WAR", GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCivilizationAdjectiveKey()));
	}

	if (kEvent.getUnitImmobileTurns() > 0)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getText("TXT_KEY_EVENT_IMMOBILE_UNIT", kEvent.getUnitImmobileTurns() * iGrowthPercent/100 , szUnit.GetCString()));
	}

	if (!isEmpty(kEvent.getPythonHelp()))
	{
		CvWString szHelp;
		CyArgsList argsList;
		argsList.add(eEvent);
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pTriggeredData));

		gDLL->getPythonIFace()->callFunction(PYRandomEventModule, kEvent.getPythonHelp(), argsList.makeFunctionArgs(), &szHelp);

		szBuffer.append(NEWLINE);
		szBuffer.append(szHelp);
	}

	CvWStringBuffer szTemp;
	for (int i = 0; i < GC.getNumEventInfos(); ++i)
	{
		if (0 == kEvent.getAdditionalEventTime(i))
		{
			if (kEvent.getAdditionalEventChance(i) > 0)
			{
				if (GET_PLAYER(GC.getGameINLINE().getActivePlayer()).canDoEvent((EventTypes)i, *pTriggeredData))
				{
					szTemp.clear();
					setEventHelp(szTemp, (EventTypes)i, iEventTriggeredId, ePlayer);

					if (!szTemp.isEmpty())
					{
						szBuffer.append(NEWLINE);
						szBuffer.append(NEWLINE);
						szBuffer.append(gDLL->getText("TXT_KEY_EVENT_ADDITIONAL_CHANCE", kEvent.getAdditionalEventChance(i), L""));
						szBuffer.append(NEWLINE);
						szBuffer.append(szTemp);
					}
				}
			}
		}
		else
		{
			szTemp.clear();
			setEventHelp(szTemp, (EventTypes)i, iEventTriggeredId, ePlayer);

			if (!szTemp.isEmpty())
			{
				CvWString szDelay = gDLL->getText("TXT_KEY_EVENT_DELAY_TURNS", (GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent() * kEvent.getAdditionalEventTime((EventTypes)i)) / 100);

				if (kEvent.getAdditionalEventChance(i) > 0)
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_ADDITIONAL_CHANCE", kEvent.getAdditionalEventChance(i), szDelay.GetCString()));
				}
				else
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_DELAY", szDelay.GetCString()));
				}

				szBuffer.append(NEWLINE);
				szBuffer.append(szTemp);
			}
		}
	}
	bool done = false;
	while(!done)
	{
		done = true;
		if(!szBuffer.isEmpty())
		{
			const wchar* wideChar = szBuffer.getCString();
			if(wideChar[0] == L'\n')
			{
				CvWString tempString(&wideChar[1]);
				szBuffer.clear();
				szBuffer.append(tempString);
				done = false;
			}
		}
	}
}

void CvGameTextMgr::eventGoldHelp(CvWStringBuffer& szBuffer, EventTypes eEvent, PlayerTypes ePlayer, PlayerTypes eOtherPlayer)
{
	CvEventInfo& kEvent = GC.getEventInfo(eEvent);
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	int iGold1 = kPlayer.getEventCost(eEvent, eOtherPlayer, false);
	int iGold2 = kPlayer.getEventCost(eEvent, eOtherPlayer, true);

	if (0 != iGold1 || 0 != iGold2)
	{
		if (iGold1 == iGold2)
		{
			if (NO_PLAYER != eOtherPlayer && kEvent.isGoldToPlayer())
			{
				if (iGold1 > 0)
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_GOLD_FROM_PLAYER", iGold1, GET_PLAYER(eOtherPlayer).getNameKey()));
				}
				else
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_GOLD_TO_PLAYER", -iGold1, GET_PLAYER(eOtherPlayer).getNameKey()));
				}
			}
			else
			{
				if (iGold1 > 0)
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_GOLD_GAINED", iGold1));
				}
				else
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_GOLD_LOST", -iGold1));
				}
			}
		}
		else
		{
			if (NO_PLAYER != eOtherPlayer && kEvent.isGoldToPlayer())
			{
				if (iGold1 > 0)
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_GOLD_RANGE_FROM_PLAYER", iGold1, iGold2, GET_PLAYER(eOtherPlayer).getNameKey()));
				}
				else
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_GOLD_RANGE_TO_PLAYER", -iGold1, -iGold2, GET_PLAYER(eOtherPlayer).getNameKey()));
				}
			}
			else
			{
				if (iGold1 > 0)
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_GOLD_RANGE_GAINED", iGold1, iGold2));
				}
				else
				{
					szBuffer.append(NEWLINE);
					szBuffer.append(gDLL->getText("TXT_KEY_EVENT_GOLD_RANGE_LOST", -iGold1, iGold2));
				}
			}
		}
	}
}

void CvGameTextMgr::setFatherHelp(CvWStringBuffer &szBuffer, FatherTypes eFather, bool bCivilopediaText)
{
	CvWString szTempBuffer;
	CvFatherInfo& kFatherInfo = GC.getFatherInfo(eFather);
	PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
	TeamTypes eTeam = GC.getGameINLINE().getActiveTeam();

	if (!bCivilopediaText)
	{
		szTempBuffer.Format(SETCOLR L"%s" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), kFatherInfo.getDescription());
		szBuffer.append(szTempBuffer);

		FatherCategoryTypes eCategory = (FatherCategoryTypes) kFatherInfo.getFatherCategory();
		if (eCategory != NO_FATHERCATEGORY)
		{
			szTempBuffer.Format(SETCOLR L" (%s)" ENDCOLR , TEXT_COLOR("COLOR_HIGHLIGHT_TEXT"), GC.getFatherCategoryInfo(eCategory).getDescription());
			szBuffer.append(szTempBuffer);
		}

		szTempBuffer.clear();

		for(int i=0;i<GC.getNumFatherPointInfos();i++)
		{
			FatherPointTypes ePointType = (FatherPointTypes) i;
			if (kFatherInfo.getPointCost(ePointType) > 0)
			{
				if (!szTempBuffer.empty())
				{
					szTempBuffer += L", ";
				}
				else
				{
					szTempBuffer += L"\n";
				}

				szTempBuffer += CvWString::format(L"%d%c", (eTeam != NO_TEAM ? GET_TEAM(eTeam).getFatherPointCost(eFather, ePointType) : kFatherInfo.getPointCost(ePointType)), GC.getFatherPointInfo(ePointType).getChar());
			}
		}

		szBuffer.append(szTempBuffer);
	}

	for (int iImprovement = 0; iImprovement < GC.getNumImprovementInfos(); ++iImprovement)
	{
		if (kFatherInfo.isRevealImprovement(iImprovement))
		{
			szBuffer.append(NEWLINE);
			szBuffer.append(gDLL->getText("TXT_KEY_FATHER_REVEALS_IMPROVEMENT", GC.getImprovementInfo((ImprovementTypes) iImprovement).getTextKeyWide()));
		}
	}

	for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); ++iUnitClass)
	{
		UnitTypes eUnit = (UnitTypes) GC.getUnitClassInfo((UnitClassTypes) iUnitClass).getDefaultUnitIndex();

		if (ePlayer != NO_PLAYER)
		{
			eUnit = (UnitTypes) GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getCivilizationUnits(iUnitClass);
		}

		if (eUnit != NO_UNIT)
		{
			if (kFatherInfo.getFreeUnits(iUnitClass) > 0)
			{
				szTempBuffer = gDLL->getText("TXT_KEY_FATHER_FREE_UNITS", kFatherInfo.getFreeUnits(iUnitClass), GC.getUnitInfo(eUnit).getTextKeyWide());
				szBuffer.append(NEWLINE);
				szBuffer.append(szTempBuffer);
			}
		}
	}

	if (kFatherInfo.getTrait() != NO_TRAIT)
	{
		CivilizationTypes eCivilization = NO_CIVILIZATION;
		if (ePlayer != NO_PLAYER)
		{
			eCivilization = GET_PLAYER(ePlayer).getCivilizationType();
		}

		parseTraits(szBuffer, (TraitTypes) kFatherInfo.getTrait(), eCivilization, false, false);
	}
}

void CvGameTextMgr::getTradeScreenTitleIcon(CvString& szButton, CvWidgetDataStruct& widgetData, PlayerTypes ePlayer)
{
	szButton = GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getButton();
}

void CvGameTextMgr::getTradeScreenIcons(std::vector< std::pair<CvString, CvWidgetDataStruct> >& aIconInfos, PlayerTypes ePlayer)
{
	aIconInfos.clear();
}

void CvGameTextMgr::getTradeScreenHeader(CvWString& szHeader, PlayerTypes ePlayer, PlayerTypes eOtherPlayer, bool bAttitude, CvCity* pCity)
{
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	if (pCity == NULL || !kPlayer.isNative())
	{
		szHeader.Format(L"%s - %s", kPlayer.getName(), kPlayer.getCivilizationDescription());
	}
	else
	{
		szHeader = gDLL->getText("TXT_KEY_TRADE_SCREEN_VILLAGE_CHIEF", pCity->getNameKey(), kPlayer.getCivilizationDescriptionKey());
	}

	if (bAttitude)
	{
		szHeader += CvWString::format(L" (%s)", GC.getAttitudeInfo(kPlayer.AI_getAttitude(eOtherPlayer)).getDescription());
	}
}

void CvGameTextMgr::setResourceLayerInfo(ResourceLayerOptions eOption, CvWString& szName, CvString& szButton)
{
	switch (eOption)
	{
	case RESOURCE_LAYER_NATIVE_TRADE:
		szName = gDLL->getText("TXT_KEY_GLOBELAYER_RESOURCES_NATIVE_TRADE");
		szButton = "XXX";
		break;
	case RESOURCE_LAYER_NATIVE_TRAIN:
		szName = gDLL->getText("TXT_KEY_GLOBELAYER_RESOURCES_NATIVE_TRAIN");
		szButton = "XXX";
		break;
	case RESOURCE_LAYER_RESOURCES:
		szName = gDLL->getText("TXT_KEY_GLOBELAYER_RESOURCES");
		szButton = "XXX";
		break;
	default:
		FAssertMsg(false, "Invalid option");
		break;
	}
}

void CvGameTextMgr::setUnitLayerInfo(UnitLayerOptionTypes eOption, CvWString& szName, CvString& szButton)
{
	switch (eOption)
	{
	case SHOW_ALL_MILITARY:
		szName = gDLL->getText("TXT_KEY_GLOBELAYER_UNITS_ALLMILITARY");
		szButton = "XXX";
		break;
	case SHOW_TEAM_MILITARY:
		szName = gDLL->getText("TXT_KEY_GLOBELAYER_UNITS_TEAMMILITARY");
		szButton = "XXX";
		break;
	case SHOW_ENEMIES_IN_TERRITORY:
		szName = gDLL->getText("TXT_KEY_GLOBELAYER_UNITS_ENEMY_TERRITORY_MILITARY");
		szButton = "XXX";
		break;
	case SHOW_ENEMIES:
		szName = gDLL->getText("TXT_KEY_GLOBELAYER_UNITS_ENEMYMILITARY");
		szButton = "XXX";
		break;
	case SHOW_PLAYER_DOMESTICS:
		szName = gDLL->getText("TXT_KEY_GLOBELAYER_UNITS_DOMESTICS");
		szButton = "XXX";
		break;
	default:
		FAssertMsg(false, "Invalid option");
		break;
	}
}

