#pragma once

// team.h

#ifndef CIV4_TEAM_H
#define CIV4_TEAM_H

//#include "CvEnums.h"

class CvArea;

class CvTeam
{

public:
	CvTeam();
	virtual ~CvTeam();

	// PatchMod: Victorys START
	int getTotalProductionRate() const;
	// PatchMod: Victorys END
	DllExport void init(TeamTypes eID);
	DllExport void reset(TeamTypes eID = (TeamTypes)0, bool bConstructorCall = false);

protected:

	void uninit();

public:
	void addTeam(TeamTypes eTeam);
	void shareItems(TeamTypes eTeam);
	void shareCounters(TeamTypes eTeam);
	void processFather(FatherTypes eFather, int iChange);

	void doTurn();

	void updateYield();
	bool canChangeWarPeace(TeamTypes eTeam) const;
	DllExport bool canDeclareWar(TeamTypes eTeam) const;
	DllExport void declareWar(TeamTypes eTeam, bool bNewDiplo, WarPlanTypes eWarPlan);
	DllExport void makePeace(TeamTypes eTeam, bool bBumpUnits = true);
	bool canContact(TeamTypes eTeam) const;
	void meet(TeamTypes eTeam, bool bNewDiplo);
	void signOpenBorders(TeamTypes eTeam);
	void signDefensivePact(TeamTypes eTeam);
	bool canSignDefensivePact(TeamTypes eTeam);
	int getAssets() const;
	int getPower() const;
	int getDefensivePower() const;
	int getEnemyPower() const;
	int getAtWarCount() const;
	int getWarPlanCount(WarPlanTypes eWarPlan) const;
	int getAnyWarPlanCount() const;
	int getChosenWarCount() const;
	int getHasMetCivCount() const;
	bool hasMetHuman() const;
	int getDefensivePactCount(TeamTypes eTeam = NO_TEAM) const;

	int getUnitClassMaking(UnitClassTypes eUnitClass) const;
	int getUnitClassCountPlusMaking(UnitClassTypes eIndex) const;
	int getBuildingClassMaking(BuildingClassTypes eBuildingClass) const;
	int getBuildingClassCountPlusMaking(BuildingClassTypes eIndex) const;

	int countTotalCulture();

	int countNumUnitsByArea(CvArea* pArea) const;
	int countNumCitiesByArea(CvArea* pArea) const;
	int countTotalPopulationByArea(CvArea* pArea) const;
	int countPowerByArea(CvArea* pArea) const;
	int countEnemyPowerByArea(CvArea* pArea) const;
	int countNumAIUnitsByArea(CvArea* pArea, UnitAITypes eUnitAI) const;
	int countEnemyDangerByArea(CvArea* pArea) const;

	int getFatherCostModifier() const;
	int getFatherPointCost(FatherTypes eFather, FatherPointTypes ePointType) const;

	int getFatherPoints(FatherPointTypes ePointType) const;
	void changeFatherPoints(FatherPointTypes ePointType, int iChange);
	int getBestFatherPointMultiplier() const;

	bool isFatherIgnore(FatherTypes eFather) const;
	void setFatherIgnore(FatherTypes eFather, bool bValue);

	bool canConvinceFather(FatherTypes eFather) const;
	void convinceFather(FatherTypes eFather, bool bAccept);

	bool isHuman() const;
	bool hasNativePlayer() const;
	bool hasColonialPlayer() const;
	bool hasEuropePlayer() const;
	PlayerTypes getLeaderID() const;
	HandicapTypes getHandicapType() const;
	DllExport CvWString getName() const;

	DllExport int getNumMembers() const;
	DllExport void changeNumMembers(int iChange);
	DllExport int getAliveCount() const;
	DllExport int isAlive() const;
	void changeAliveCount(int iChange);
	int getEverAliveCount() const;
	int isEverAlive() const;
	void changeEverAliveCount(int iChange);
	int getNumCities() const;
	void changeNumCities(int iChange);

	int getTotalPopulation() const;

	int getTotalLand() const;
	void changeTotalLand(int iChange);

	int getMapTradingCount() const;
	bool isMapTrading() const;
	void changeMapTradingCount(int iChange);

	int getGoldTradingCount() const;
	bool isGoldTrading() const;
	void changeGoldTradingCount(int iChange);

	int getOpenBordersTradingCount() const;
	DllExport bool isOpenBordersTrading() const;
	void changeOpenBordersTradingCount(int iChange);

	int getDefensivePactTradingCount() const;
	bool isDefensivePactTrading() const;
	void changeDefensivePactTradingCount(int iChange);

	int getPermanentAllianceTradingCount() const;
	bool isPermanentAllianceTrading() const;
	void changePermanentAllianceTradingCount(int iChange);

	DllExport bool isMapCentering() const;
	void setMapCentering(bool bNewValue);

	TeamTypes getID() const;

	DllExport bool isHasMet(TeamTypes eIndex) const;
	void makeHasMet(TeamTypes eIndex, bool bNewDiplo);
	DllExport bool isAtWar(TeamTypes eIndex) const;
	DllExport void setAtWar(TeamTypes eIndex, bool bNewValue);
	bool isPermanentWarPeace(TeamTypes eIndex) const;
	void setPermanentWarPeace(TeamTypes eIndex, bool bNewValue);

	DllExport bool isOpenBorders(TeamTypes eIndex) const;
	void setOpenBorders(TeamTypes eIndex, bool bNewValue);
	DllExport bool isDefensivePact(TeamTypes eIndex) const;
	void setDefensivePact(TeamTypes eIndex, bool bNewValue);
	DllExport bool isForcePeace(TeamTypes eIndex) const;
	void setForcePeace(TeamTypes eIndex, bool bNewValue);

	int getUnitClassCount(UnitClassTypes eIndex) const;
	void changeUnitClassCount(UnitClassTypes eIndex, int iChange);
	int getBuildingClassCount(BuildingClassTypes eIndex) const;
	void changeBuildingClassCount(BuildingClassTypes eIndex, int iChange);

	int getEuropeUnitsPurchased(UnitClassTypes eIndex) const;
	void changeEuropeUnitsPurchased(UnitClassTypes eIndex, int iChange);

	bool isFriendlyTerritory(TeamTypes eTeam) const;

	void setForceRevealedBonus(BonusTypes eBonus, bool bRevealed);
	bool isForceRevealedBonus(BonusTypes eBonus) const;

	DllExport int countNumHumanGameTurnActive() const;
	void setTurnActive(bool bNewValue, bool bTurn = true);
	bool isTurnActive() const;

	int getRebelPercent() const;
	bool canDoRevolution() const;
	bool isInRevolution() const;
	void doRevolution();
	bool isParentOf(TeamTypes eChildTeam) const;
	bool checkIndependence() const;

	virtual void AI_init() = 0;
	virtual void AI_reset() = 0;
	virtual void AI_doTurnPre() = 0;
	virtual void AI_doTurnPost() = 0;
	virtual void AI_makeAssignWorkDirty() = 0;
	virtual void AI_updateAreaStragies(bool bTargets = true) = 0;
	virtual bool AI_shareWar(TeamTypes eTeam) const = 0;
	virtual void AI_updateWorstEnemy() = 0;
	virtual int AI_getAtWarCounter(TeamTypes eIndex) const = 0;
	virtual void AI_setAtWarCounter(TeamTypes eIndex, int iNewValue) = 0;
	virtual int AI_getAtPeaceCounter(TeamTypes eIndex) const = 0;
	virtual void AI_setAtPeaceCounter(TeamTypes eIndex, int iNewValue) = 0;
	virtual int AI_getHasMetCounter(TeamTypes eIndex) const = 0;
	virtual void AI_setHasMetCounter(TeamTypes eIndex, int iNewValue) = 0;
	virtual int AI_getOpenBordersCounter(TeamTypes eIndex) const = 0;
	virtual void AI_setOpenBordersCounter(TeamTypes eIndex, int iNewValue) = 0;
	virtual int AI_getDefensivePactCounter(TeamTypes eIndex) const = 0;
	virtual void AI_setDefensivePactCounter(TeamTypes eIndex, int iNewValue) = 0;
	virtual int AI_getShareWarCounter(TeamTypes eIndex) const = 0;
	virtual void AI_setShareWarCounter(TeamTypes eIndex, int iNewValue) = 0;
	virtual int AI_getWarSuccess(TeamTypes eIndex) const = 0;
	virtual void AI_setWarSuccess(TeamTypes eIndex, int iNewValue) = 0;
	virtual void AI_changeWarSuccess(TeamTypes eIndex, int iChange) = 0;
	virtual int AI_getEnemyPeacetimeTradeValue(TeamTypes eIndex) const = 0;
	virtual void AI_setEnemyPeacetimeTradeValue(TeamTypes eIndex, int iNewValue) = 0;
	virtual int AI_getEnemyPeacetimeGrantValue(TeamTypes eIndex) const = 0;
	virtual void AI_setEnemyPeacetimeGrantValue(TeamTypes eIndex, int iNewValue) = 0;
	virtual WarPlanTypes AI_getWarPlan(TeamTypes eIndex) const = 0;
	virtual bool AI_isChosenWar(TeamTypes eIndex) const = 0;
	virtual bool AI_isSneakAttackPreparing(TeamTypes eIndex) const = 0;
	virtual bool AI_isSneakAttackReady(TeamTypes eIndex) const = 0;
	virtual void AI_setWarPlan(TeamTypes eIndex, WarPlanTypes eNewValue, bool bWar = true) = 0;

protected:

	int m_iNumMembers;
	int m_iAliveCount;
	int m_iEverAliveCount;
	int m_iNumCities;
	int m_iTotalLand;
	int m_iMapTradingCount;
	int m_iGoldTradingCount;
	int m_iOpenBordersTradingCount;
	int m_iDefensivePactTradingCount;
	int m_iPermanentAllianceTradingCount;

	bool m_bMapCentering;

	TeamTypes m_eID;

	bool* m_abAtWar;
	bool* m_abHasMet;
	bool* m_abPermanentWarPeace;
	bool* m_abOpenBorders;
	bool* m_abDefensivePact;
	bool* m_abForcePeace;

	bool* m_abFatherIgnore;
	int* m_aiFatherPoints;
	int* m_aiUnitClassCount;
	int* m_aiBuildingClassCount;
	int* m_aiEuropeUnitsPurchased;

	std::vector<BonusTypes> m_aeRevealedBonuses;
	void testFoundingFather();
	void cancelDefensivePacts(TeamTypes eEndingTeam);
	void declareWarNoRevolution(TeamTypes eTeam, bool bNewDiplo, WarPlanTypes eWarPlan, bool bPlaySound);
	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);
};

#endif
