#pragma once

// teamAI.h

#ifndef CIV4_TEAM_AI_H
#define CIV4_TEAM_AI_H

#include "CvTeam.h"

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

	static void initStatics();
	static void freeStatics();

	void AI_init();
	void AI_uninit();
	void AI_reset();

	void AI_doTurnPre();
	void AI_doTurnPost();

	void AI_makeAssignWorkDirty();

	int AI_getOurPlotStrength(CvPlot* pPlot, int iRange, bool bDefensiveBonuses, bool bTestMoves, bool bIncludeVassals = false) const;	// TAC - AI Assault Sea - koma13, jdog5000(BBAI)

	void AI_updateAreaStragies(bool bTargets = true);
	void AI_updateAreaTargets();

	int AI_countMilitaryWeight(CvArea* pArea) const;

	bool AI_isAnyCapitalAreaAlone() const;
	bool AI_isPrimaryArea(CvArea* pArea) const;
	bool AI_hasCitiesInPrimaryArea(TeamTypes eTeam) const;
	AreaAITypes AI_calculateAreaAIType(CvArea* pArea, bool bPreparingTotal = false) const;

	int AI_calculateAdjacentLandPlots(TeamTypes eTeam) const;
	int AI_calculateCapitalProximity(TeamTypes eTeam) const;
	int AI_calculatePlotWarValue(TeamTypes eTeam) const;

	bool AI_isLandTarget(TeamTypes eTeam) const;
	bool AI_isAllyLandTarget(TeamTypes eTeam) const;
	bool AI_shareWar(TeamTypes eTeam) const;

	AttitudeTypes AI_getAttitude(TeamTypes eTeam, bool bForced = true) const;
	int AI_getAttitudeVal(TeamTypes eTeam, bool bForced = true) const;
	int AI_getMemoryCount(TeamTypes eTeam, MemoryTypes eMemory) const;

	int AI_startWarVal(TeamTypes eTeam) const;
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
	bool AI_isSneakAttackPreparing(TeamTypes eIndex) const;
	bool AI_isSneakAttackReady(TeamTypes eIndex /* K-Mod (any team): */ = NO_TEAM) const;
	void AI_setWarPlan(TeamTypes eIndex, WarPlanTypes eNewValue, bool bWar = true);

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



	// added so under cheat mode we can call protected functions for testing
	friend class CvGameTextMgr;
	friend class CvDLLWidgetData;
};

// helper for accessing static functions
#define GET_TEAM CvTeamAI::getTeam

#endif
