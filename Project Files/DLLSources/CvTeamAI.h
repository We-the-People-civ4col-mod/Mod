#pragma once

// teamAI.h

#ifndef CIV4_TEAM_AI_H
#define CIV4_TEAM_AI_H

#include "CvTeam.h"
#include "AIStrengthMemoryMap.h" // advc.158

class CvTeamAI : public CvTeam
{

public:

	CvTeamAI();
	virtual ~CvTeamAI();

	// inlined for performance reasons, only in the dll
	static CvTeamAI& getTeam(TeamTypes eTeam)
	{
		FAssertMsg(eTeam != NO_TEAM, "eTeam is not assigned a valid value");
		FAssertMsg(eTeam < MAX_TEAMS, "eTeam is not assigned a valid value");
		return m_aTeams[eTeam];
	}
	DllExport static CvTeamAI& getTeamNonInl(TeamTypes eTeam);

	static bool AI_isChosenWarPlan(WarPlanTypes eWarPlanType); // advc.105

	static bool AI_isImminentWarPlan(WarPlanTypes eWarPlanType)
	{
		return (eWarPlanType == WARPLAN_LIMITED || eWarPlanType == WARPLAN_TOTAL ||
			eWarPlanType == WARPLAN_DOGPILE);
	}

	static void initStatics();
	static void freeStatics();

	void AI_init();
	void AI_uninit();
	void AI_reset();

	void AI_doTurnPre();
	void AI_doTurnPost();

	void AI_makeAssignWorkDirty();

	int AI_getOurPlotStrength(CvPlot* pPlot, int iRange, bool bDefensiveBonuses, bool bTestMoves, bool bIncludeVassals = false) const;	// TAC - AI Assault Sea - koma13, jdog5000(BBAI)

	void AI_updateAreaStrategies(bool bTargets = true);
	void AI_updateAreaTargets();

	int AI_countMilitaryWeight(CvArea const* pArea) const;

	bool AI_isAnyCapitalAreaAlone() const;
	bool AI_isPrimaryArea(CvArea const& kArea) const;
	bool AI_hasCitiesInPrimaryArea(TeamTypes eTeam) const;
	AreaAITypes AI_calculateAreaAIType(CvArea const& kArea, bool bPreparingTotal = false) const;

	int AI_calculateAdjacentLandPlots(TeamTypes eTeam) const;
	int AI_calculateCapitalProximity(TeamTypes eTeam) const;
	int AI_calculatePlotWarValue(TeamTypes eTeam) const;

	bool AI_isLandTarget(TeamTypes eTeam) const;
	bool AI_isAllyLandTarget(TeamTypes eTeam) const;
	bool AI_shareWar(TeamTypes eTeam) const;

	AttitudeTypes AI_getAttitude(TeamTypes eTeam, bool bForced = true) const;
	int AI_getAttitudeVal(TeamTypes eTeam, bool bForced = true) const;
	int AI_getMemoryCount(TeamTypes eTeam, MemoryTypes eMemory) const;

	int AI_startWarVal(TeamTypes eTarget, WarPlanTypes eWarPlan = NO_WARPLAN, // K-Mod
		bool bConstCache = false) const; // advc.001n
	int AI_endWarVal(TeamTypes eTeam) const;
	int AI_mapTradeVal(TeamTypes eTeam) const;
	DenialTypes AI_mapTrade(TeamTypes eTeam) const;

	int AI_makePeaceTradeVal(TeamTypes ePeaceTeam, TeamTypes eTeam) const;
	DenialTypes AI_makePeaceTrade(TeamTypes ePeaceTeam, TeamTypes eTeam) const;

	int AI_declareWarTradeVal(TeamTypes eWarTeam, TeamTypes eTeam) const;
	DenialTypes AI_declareWarTrade(TeamTypes eWarTeam, TeamTypes eTeam, bool bConsiderPower = true) const;

	int AI_openBordersTradeVal(TeamTypes eTeam) const;
	DenialTypes AI_openBordersTrade(TeamTypes eTeam) const;

	int AI_defensivePactTradeVal(TeamTypes eTeam) const;
	DenialTypes AI_defensivePactTrade(TeamTypes eTeam) const;

	DenialTypes AI_permanentAllianceTrade(TeamTypes eTeam) const;

	TeamTypes AI_getWorstEnemy() const;
	void AI_updateWorstEnemy();

	int AI_getWarPlanStateCounter(TeamTypes eIndex) const;
	void AI_setWarPlanStateCounter(TeamTypes eIndex, int iNewValue);
	void AI_changeWarPlanStateCounter(TeamTypes eIndex, int iChange);

	int AI_getAtWarCounter(TeamTypes eIndex) const;
	void AI_setAtWarCounter(TeamTypes eIndex, int iNewValue);
	void AI_changeAtWarCounter(TeamTypes eIndex, int iChange);

	int AI_getAtPeaceCounter(TeamTypes eIndex) const;
	void AI_setAtPeaceCounter(TeamTypes eIndex, int iNewValue);
	void AI_changeAtPeaceCounter(TeamTypes eIndex, int iChange);

	int AI_getHasMetCounter(TeamTypes eIndex) const;
	void AI_setHasMetCounter(TeamTypes eIndex, int iNewValue);
	void AI_changeHasMetCounter(TeamTypes eIndex, int iChange);

	int AI_getOpenBordersCounter(TeamTypes eIndex) const;
	void AI_setOpenBordersCounter(TeamTypes eIndex, int iNewValue);
	void AI_changeOpenBordersCounter(TeamTypes eIndex, int iChange);

	int AI_getDefensivePactCounter(TeamTypes eIndex) const;
	void AI_setDefensivePactCounter(TeamTypes eIndex, int iNewValue);
	void AI_changeDefensivePactCounter(TeamTypes eIndex, int iChange);

	int AI_getShareWarCounter(TeamTypes eIndex) const;
	void AI_setShareWarCounter(TeamTypes eIndex, int iNewValue);
	void AI_changeShareWarCounter(TeamTypes eIndex, int iChange);

	int AI_getWarSuccess(TeamTypes eIndex) const;
	void AI_setWarSuccess(TeamTypes eIndex, int iNewValue);
	void AI_changeWarSuccess(TeamTypes eIndex, int iChange);

	int AI_getEnemyPeacetimeTradeValue(TeamTypes eIndex) const;
	void AI_setEnemyPeacetimeTradeValue(TeamTypes eIndex, int iNewValue);
	void AI_changeEnemyPeacetimeTradeValue(TeamTypes eIndex, int iChange);

	int AI_getEnemyPeacetimeGrantValue(TeamTypes eIndex) const;
	void AI_setEnemyPeacetimeGrantValue(TeamTypes eIndex, int iNewValue);
	void AI_changeEnemyPeacetimeGrantValue(TeamTypes eIndex, int iChange);

	int AI_getDamages(TeamTypes eIndex) const;
	void AI_setDamages(TeamTypes eIndex, int iNewValue);
	void AI_changeDamages(TeamTypes eIndex, int iChange);

	void AI_doDamages(TeamTypes eTeam, bool bPeace);

	WarPlanTypes AI_getWarPlan(TeamTypes eIndex) const;
	bool AI_isChosenWar(TeamTypes eIndex) const;
	bool AI_isAnyChosenWar() const; // advc.105
	int AI_getNumWarPlans(WarPlanTypes eWarPlanType) const
	{
		return getAnyWarPlanCount();
	}
	bool AI_isAnyWarPlan() const { return (getAnyWarPlanCount() > 0); } // </advc.opt>
	bool AI_isSneakAttackPreparing(TeamTypes eIndex) const;
	bool AI_isSneakAttackReady(TeamTypes eIndex /* K-Mod (any team): */ = NO_TEAM) const;
	void AI_setWarPlan(TeamTypes eIndex, WarPlanTypes eNewValue, bool bWar = true);
	// <advc.opt>
	bool AI_mayAttack(TeamTypes eDefender) const;
	bool AI_mayAttack(CvPlot const& kPlot) const; // </advc.opt>

	int AI_teamCloseness(TeamTypes eIndex, int iMaxDistance = -1) const;
	int AI_targetValidity(TeamTypes eTeam) const;

	bool AI_performNoWarRolls(TeamTypes eTeam);

	int AI_getAttitudeWeight(TeamTypes eTeam);

	bool AI_isWaterAreaRelevant(CvArea* pArea);

	bool AI_isExploringNeeded(CvUnit* pUnit) const;

	short AI_enemyCityDistance(CvPlot* pPlot) const;
	short AI_enemyUnitDistance(CvPlot* pPlot) const;

	int AI_getGreed(TeamTypes eTeam) const;
	int AI_getAnger(TeamTypes eTeam) const;
	int AI_getFear(TeamTypes eTeam) const;

	int AI_warplanStrength(WarPlanTypes eWarplan) const;

	bool AI_isNative() const;
	bool AI_isKing() const;

	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);

	void AI_resetSavedData();
	void read(CvSavegameReader reader);
	void write(CvSavegameWriter writer);

	int AI_plotDefense(CvPlot const& kPlot, bool bIgnoreBuilding = false,
		bool bGarrisonStrength = false) const; // advc.500b

	// advc.158:
	AIStrengthMemoryMap& AI_strengthMemory() const { return m_strengthMemory; }

	bool AI_isColonialPower() const;
	bool AI_isColonialOrBarbarianPower() const;
	int AI_getWarSuccessRating() const; // K-Mod
	int AI_getEnemyPowerPercent(bool bConsiderOthers = false) const;
	bool AI_isPushover(TeamTypes ePotentialEnemy) const; // advc.105
	bool AI_deduceCitySite(CvCity const& kCity) const; // K-Mod
	// advc: countEnemy... functions moved from CvTeam
	int AI_countEnemyPowerByArea(CvArea const& kArea) const;
	int AI_countEnemyCitiesByArea(CvArea const& kArea) const; // K-Mod
	int AI_countEnemyDangerByArea(CvArea const& kArea, TeamTypes eEnemyTeam = NO_TEAM) const; // bbai
	int AI_roundTradeVal(int iVal) const; // advc.104k

protected:

	static CvTeamAI* m_aTeams;

	TeamTypes m_eWorstEnemy;

	std::vector<short> m_aiEnemyCityDistance;
	std::vector<short> m_aiEnemyUnitDistance;

	EnumMap<TeamTypes, int> m_em_iWarPlanStateCounter;
	EnumMap<TeamTypes, int> m_em_iAtWarCounter;
	EnumMap<TeamTypes, int> m_em_iAtPeaceCounter;
	EnumMap<TeamTypes, int> m_em_iHasMetCounter;
	EnumMap<TeamTypes, int> m_em_iOpenBordersCounter;
	EnumMap<TeamTypes, int> m_em_iDefensivePactCounter;
	EnumMap<TeamTypes, int> m_em_iShareWarCounter;
	EnumMap<TeamTypes, int> m_em_iWarSuccess;
	EnumMap<TeamTypes, int> m_em_iEnemyPeacetimeTradeValue;
	EnumMap<TeamTypes, int> m_em_iEnemyPeacetimeGrantValue;
	EnumMap<TeamTypes, int> m_em_iDamages;

	EnumMap<TeamTypes, WarPlanTypes> m_em_eWarPlan;
	int AI_maxWarRand() const;
	int AI_maxWarNearbyPowerRatio() const;
	int AI_maxWarDistantPowerRatio() const;
	int AI_maxWarMinAdjacentLandPercent() const;
	int AI_limitedWarRand() const;
	int AI_limitedWarPowerRatio() const;
	int AI_dogpileWarRand() const;
	int AI_makePeaceRand() const;
	int AI_noWarAttitudeProb(AttitudeTypes eAttitude) const;

	void AI_doCounter();
	void AI_doWar();
    void AI_doTactics();

	mutable AIStrengthMemoryMap m_strengthMemory; // advc.158

	// added so under cheat mode we can call protected functions for testing
	friend class CvGameTextMgr;
	friend class CvDLLWidgetData;
};

// helper for accessing static functions
#define GET_TEAM CvTeamAI::getTeam
#define TEAMID(x) GET_PLAYER(x).getTeam()

#endif
