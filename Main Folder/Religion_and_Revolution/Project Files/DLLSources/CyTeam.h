#pragma once
#ifndef CyTeam_h
#define CyTeam_h
//
// Python wrapper class for CvTeam
//

// //#include "CvEnums.h"
class CvTeam;
class CyArea;
class CyUnit;
class CyTeam
{
public:
	CyTeam();
	CyTeam(CvTeam* pTeam);		// Call from C++
	CvTeam* getTeam() { return m_pTeam;	}	// Call from C++

	// PatchMod: Victorys START
	int getTotalProductionRate();
	// PatchMod: Victorys END

	bool isNone() { return (m_pTeam==NULL); }
	void addTeam(int /*TeamTypes*/ eTeam);
	bool canChangeWarPeace(int /*TeamTypes*/ eTeam);
	bool canDeclareWar(int /*TeamTypes*/ eTeam);
	void declareWar(int /*TeamTypes*/ eTeam, bool bNewDiplo, int /*WarPlanTypes*/ eWarPlan);
	void makePeace(int /*TeamTypes*/ eTeam);
	bool canContact(int /*TeamTypes*/ eTeam);
	void meet(int /*TeamTypes*/ eTeam, bool bNewDiplo);
	void signOpenBorders(int /*TeamTypes*/ eTeam);
	void signDefensivePact(int /*TeamTypes*/ eTeam);
	int getAssets();
	int getPower();
	int getDefensivePower();
	int getAtWarCount();
	int getWarPlanCount(int /*WarPlanTypes*/ eWarPlan);
	int getAnyWarPlanCount();
	int getChosenWarCount();
	int getHasMetCivCount();
	bool hasMetHuman();
	int getDefensivePactCount();
	int getUnitClassMaking(int /*UnitClassTypes*/ eUnitClass);
	int getUnitClassCountPlusMaking(int /*UnitClassTypes*/ eUnitClass);
	int getBuildingClassMaking(int /*BuildingClassTypes*/ eBuildingClass);
	int getBuildingClassCountPlusMaking(int /*BuildingClassTypes*/ eUnitClass);
	int countTotalCulture();
	int countNumUnitsByArea(CyArea* pArea);
	int countNumCitiesByArea(CyArea* pArea);
	int countTotalPopulationByArea(CyArea* pArea);
	int countPowerByArea(CyArea* pArea);
	int countEnemyPowerByArea(CyArea* pArea);
	int countNumAIUnitsByArea(CyArea* pArea, int /*UnitAITypes*/ eUnitAI);
	int countEnemyDangerByArea(CyArea* pArea);
	int getFatherPointCost(int /*FatherTypes*/ eFather, int /*FatherPointTypes*/ ePointType);
	int getFatherPoints(int /*FatherPointTypes*/ ePointType);
	// TAC - Python Export - koma13 - START
	bool isFatherIgnore(int /*FatherTypes*/ eFather) const;
	// TAC - Python Export - koma13 - END
	bool canConvinceFather(int /*FatherTypes*/ eFather) const;
	void changeFatherPoints(int /*FatherPointTypes*/ ePointType, int iChange);
	bool isHuman();
	int /*PlayerTypes*/ getLeaderID();
	int /*HandicapTypes*/ getHandicapType();
	std::wstring getName();
	int getNumMembers();
	bool isAlive();
	bool isEverAlive();
	int getNumCities();
	int getTotalPopulation();
	int getTotalLand();
	int getMapTradingCount();
	bool isMapTrading();
	void changeMapTradingCount(int iChange);
	int getGoldTradingCount();
	bool isGoldTrading();
	void changeGoldTradingCount(int iChange);
	int getOpenBordersTradingCount();
	bool isOpenBordersTrading();
	void changeOpenBordersTradingCount(int iChange);
	int getDefensivePactTradingCount();
	bool isDefensivePactTrading();
	void changeDefensivePactTradingCount(int iChange);
	int getPermanentAllianceTradingCount();
	bool isPermanentAllianceTrading();
	void changePermanentAllianceTradingCount(int iChange);

	bool isMapCentering();
	void setMapCentering(bool bNewValue);
	int getID();
	bool isHasMet(int /*TeamTypes*/ eIndex);
	bool isAtWar(int /*TeamTypes*/ eIndex);
	bool isPermanentWarPeace(int /*TeamTypes*/ eIndex);
	void setPermanentWarPeace(int /*TeamTypes*/ eIndex, bool bNewValue);
	bool isOpenBorders(int /*TeamTypes*/ eIndex);
	bool isForcePeace(int /*TeamTypes*/ eIndex);
	bool isDefensivePact(int /*TeamTypes*/ eIndex);
	int getUnitClassCount(int /*UnitClassTypes*/ eIndex);
	int getBuildingClassCount(int /*BuildingClassTypes*/ eIndex);
	bool AI_shareWar(int /*TeamTypes*/ eTeam);
	void AI_setWarPlan(int /*TeamTypes*/ eIndex, int /*WarPlanTypes*/ eNewValue);
	int AI_getAtWarCounter(int /*TeamTypes*/ eTeam) const;
	int AI_getAtPeaceCounter(int /*TeamTypes*/ eTeam) const;
	int AI_getWarSuccess(int /*TeamTypes*/ eIndex) const;
	int getRebelPercent() const;
	void doRevolution();
	bool canDoRevolution() const;
	bool isParentOf(int /*TeamTypes*/ eChildTeam);

protected:
	CvTeam* m_pTeam;
};
#endif	// #ifndef CyTeam
