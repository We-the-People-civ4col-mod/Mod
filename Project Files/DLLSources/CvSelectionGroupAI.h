#pragma once

// selectionGroupAI.h

#ifndef CIV4_SELECTION_GROUP_AI_H
#define CIV4_SELECTION_GROUP_AI_H

#include "CvSelectionGroup.h"

class CvTradeRoute;

class CvSelectionGroupAI : public CvSelectionGroup
{

public:

	DllExport CvSelectionGroupAI();
	DllExport virtual ~CvSelectionGroupAI();

	void AI_init();
	void AI_uninit();
	void AI_reset();

	void AI_separate();
	void AI_seperateNonAI(UnitAITypes eUnitAI);
	void AI_seperateAI(UnitAITypes eUnitAI);

	void AI_separateEmptyTransports();		// TAC - AI Assault Sea - koma13, jdog5000(BBAI)

	bool AI_update();

	int AI_attackOdds(const CvPlot* pPlot, bool bPotentialEnemy) const;
	int AI_getWeightedOdds(CvPlot const* pPlot, bool bPotentialEnemy = false); // K-Mod
	// advc.003u: These two had returned CvUnit*
	CvUnitAI* AI_getBestGroupAttacker(const CvPlot* pPlot, bool bPotentialEnemy, int& iUnitOdds, bool bForce = false, bool bNoBlitz = false) const;
	CvUnitAI* AI_getBestGroupSacrifice(const CvPlot* pPlot, bool bPotentialEnemy, bool bForce = false, bool bNoBlitz = false) const;
	int AI_compareStacks(const CvPlot* pPlot, bool bPotentialEnemy, bool bCheckCanAttack = false, bool bCheckCanMove = false) const;
	int AI_sumStrength(const CvPlot* pAttackedPlot = NULL, DomainTypes eDomainType = NO_DOMAIN, bool bCheckCanAttack = false, bool bCheckCanMove = false) const
	{
		return AI_sumStrengthInternal(pAttackedPlot, eDomainType, bCheckCanAttack); // K-Mod has removed bCheckCanMove
	}
	
	int AI_sumStrengthInternal(const CvPlot* pAttackedPlot = NULL, DomainTypes eDomainType = NO_DOMAIN,
		bool bCheckCanAttack = false) const;

	void AI_queueGroupAttack(int iX, int iY);
	void AI_cancelGroupAttack() { m_bGroupAttack = false; } // K-Mod (made inline)
	bool AI_isGroupAttack() { return AI_isGroupAttackInternal(); } // K-Mod (made inline)
	bool AI_isGroupAttackInternal() const { return m_bGroupAttack; } // K-Mod (made inline)

	bool AI_isControlled() const;
	bool AI_isDeclareWar(const CvPlot* pPlot = NULL) { return AI_isDeclareWarInternal(pPlot); }
	bool AI_isDeclareWarInternal(const CvPlot* pPlot = NULL) const;

	CvPlot* AI_getMissionAIPlot() { return AI_getMissionAIPlotInternal(); }
	bool AI_isForceSeparate() { return AI_isForceSeparateInternal(); }

	CvPlot* AI_getMissionAIPlotInternal() const;
	bool AI_isForceSeparateInternal() const;
	
	void AI_makeForceSeparate();
	void AI_setForceSeparate(bool bNewValue = true) { m_bForceSeparate = bNewValue; } // K-Mod

	MissionAITypes AI_getMissionAIType() { return AI_getMissionAITypeInternal(); }
	MissionAITypes AI_getMissionAITypeInternal() const;

	void AI_setMissionAI(MissionAITypes eNewMissionAI, CvPlot* pNewPlot, CvUnit* pNewUnit);
	CvUnit* AI_ejectBestDefender(CvPlot* pTargetPlot);

	CvUnit* AI_getMissionAIUnit();

	bool AI_isFull();

	bool AI_launchAssault(CvPlot* pTargetCityPlot = NULL);
	void AI_groupBombard();

	int AI_getYieldsLoaded(short* piYields) const;

	bool AI_tradeRoutes();

	void read(CvSavegameReader reader);
	void write(CvSavegameWriter writer);

	void AI_resetSavedData();

	bool AI_isControlled() { return AI_isControlledInternal(); }
	bool AI_isControlledInternal() const { return (!isHuman() || isAutomated()); }

	// <advc.003u> Counterparts to CvSelectionGroup::getHeadUnit
	CvUnitAI const* AI_getHeadUnit() const;
	CvUnitAI* AI_getHeadUnit(); // </advc.003u>

	int AI_getBombardTurns(CvCity const* pCity) const;

	bool AI_isHasPathToAreaEnemyCity(bool bMajorOnly = true,
		MovementFlags eFlags = NO_MOVEMENT_FLAGS, int iMaxPathTurns = -1) /* Erik (CODE1): */ const;
	bool AI_isHasPathToAreaPlayerCity(PlayerTypes ePlayer, MovementFlags eFlags = NO_MOVEMENT_FLAGS,
		int iMaxPathTurns = -1) /* Erik (CODE1): */ const;

	// <advc.004c>
	CvUnit* AI_bestUnitForMission(MissionTypes eMission,
		CvPlot const* pMissionPlot = NULL,
		std::vector<int> const* pUnitsToSkip = NULL); // </advc.004c>

	bool AI_isStranded() const { return (AI_getMissionAITypeInternal() == MISSIONAI_STRANDED); } // K-Mod}

	enum UnloadMode
	{
		NoForce,
		Force
	};

protected:

	int m_iMissionAIX;
	int m_iMissionAIY;

	bool m_bForceSeparate;

	MissionAITypes m_eMissionAIType;

	IDInfo m_missionAIUnit;

	bool m_bGroupAttack;
	int m_iGroupAttackX;
	int m_iGroupAttackY;

	// R&R mod, vetiarvind, max yield import limit - start	
	bool getIgnoreDangerStatus() const;
	// R&R mod, vetiarvind, max yield import limit - end

private:
	// R&R mod, vetiarvind, max yield import limit - start
	int estimateYieldsToLoad(CvCity* pDestinationCity, int maxYieldsToLoad, YieldTypes eYield, int turnsToReach, int alreadyLoaded) const;
	void unloadToCity(CvCity* pCity, CvUnit* unit, UnloadMode um = NoForce);
	// R&R mod, vetiarvind, max yield import limit - end

	void processTradeRoute(CvTradeRoute* pRoute, std::map<IDInfo, int>& cityValues, std::vector<CvTradeRoute*>& routes, std::vector<int>& routeValues, std::vector<bool>& yieldsDelivered, std::vector<bool>& yieldsToUnload);
};

#endif
