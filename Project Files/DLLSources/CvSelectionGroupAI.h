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
	CvUnit* AI_getBestGroupAttacker(const CvPlot* pPlot, bool bPotentialEnemy, int& iUnitOdds, bool bForce = false, bool bNoBlitz = false) const;
	CvUnit* AI_getBestGroupSacrifice(const CvPlot* pPlot, bool bPotentialEnemy, bool bForce = false, bool bNoBlitz = false) const;
	int AI_compareStacks(const CvPlot* pPlot, bool bPotentialEnemy, bool bCheckCanAttack = false, bool bCheckCanMove = false) const;
	int AI_sumStrength(const CvPlot* pAttackedPlot = NULL, DomainTypes eDomainType = NO_DOMAIN, bool bCheckCanAttack = false, bool bCheckCanMove = false) const;
	void AI_queueGroupAttack(int iX, int iY);
	void AI_cancelGroupAttack();
	bool AI_isGroupAttack();

	bool AI_isControlled();
	bool AI_isDeclareWar(const CvPlot* pPlot = NULL);

	CvPlot* AI_getMissionAIPlot();

	bool AI_isForceSeparate();
	void AI_makeForceSeparate();

	MissionAITypes AI_getMissionAIType();
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

	bool isCoastalTransport();
	std::set<int>::iterator humanOrNonAutomaticFull(std::set<int>::iterator it, CvPlayerAI &kOwner, const IDInfo kEurope, std::map<IDInfo, int> cityValues, std::vector<CvTradeRoute *> routes, std::vector<int> routeValues, std::vector<bool> yieldsDelivered, std::vector<bool> yieldsToUnload, const bool bIgnoreDanger);
	void hasCargo(std::vector<bool> yieldsToUnload, std::vector<bool> yieldsDelivered, CvCity* pPlotCity);
	void AIOrAutomaticFull(CvPlayerAI &kOwner, const IDInfo kEurope, bool bCoastalTransport, std::map<IDInfo, int> cityValues, std::vector<CvTradeRoute *> routes, std::vector<int> routeValues, std::vector<bool> yieldsDelivered, std::vector<bool> yieldsToUnload);
	bool doRouteStuff(std::vector<CvTradeRoute *> &routes, const bool bNoCargo, std::map<IDInfo, int> cityValues, const bool bIgnoreDanger, short * aiYieldsLoaded, const IDInfo kEurope, CvPlayerAI &kOwner, CvCity* pPlotCity, std::vector<int> &routeValues);
	void hasCargo(std::vector<CvTradeRoute *> &routes, CvCity* pPlotCity, short * aiYieldsLoaded, const bool bIgnoreDanger, CvPlayerAI &kOwner, std::map<IDInfo, int> cityValues, std::vector<int> &routeValues);
	bool bestDestinationAmerica(IDInfo kBestDestination, const bool bIgnoreDanger);
	bool checkPath(IDInfo &kBestDestination, CvPlayerAI &kOwner, const bool bIgnoreDanger);
	void doCalculationTradeRoutes(std::vector<CvTradeRoute *> &routes, CvCity* pPlotCity, IDInfo kBestDestination, const bool bIgnoreDanger, short * aiYieldsLoaded, CvPlayerAI &kOwner);
	void findBestDestination(std::map<IDInfo, int> &cityValues, const bool bIgnoreDanger, CvPlayerAI &kOwner, bool bCoastalTransport, int &iBestDestinationValue, IDInfo &kBestDestination);


};

#endif
