#pragma once
#ifndef CyUnit_h
#define CyUnit_h
//
// Python wrapper class for CvUnit
//

//#include "CvEnums.h"
class CyArea;
class CyPlot;
class CyCity;
class CvUnit;
class CySelectionGroup;
class CvArtInfoUnit;
//class CyUnitEntity;
class CyUnit
{
public:
	CyUnit();

	DllExport CyUnit(CvUnit* pUnit);		// Call from C++
	CvUnit* getUnit() { return m_pUnit;	};	// Call from C++
	const CvUnit* getUnit() const { return m_pUnit;	};	// Call from C++
	bool isNone() { return (m_pUnit==NULL); }
	void convert(CyUnit* pUnit);
	void kill(bool bDelay);
	void NotifyEntity(int /*MissionTypes*/ eMission);
	bool isActionRecommended(int i);
	bool canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible);
	void doCommand(CommandTypes eCommand, int iData1, int iData2);
	CyPlot* getPathEndTurnPlot();
	bool generatePath(CyPlot* pToPlot, int iFlags = 0, bool bReuse = false, int* piPathTurns = NULL);
	bool canEnterTerritory(int /*PlayerTypes*/ ePlayer, bool bIgnoreRightOfPassage);
	bool canEnterArea(int /*PlayerTypes*/ ePlayer, CyArea* pArea, bool bIgnoreRightOfPassage);
	bool canMoveInto(CyPlot* pPlot, bool bAttack, bool bDeclareWar, bool bIgnoreLoad);
	bool canMoveOrAttackInto(CyPlot* pPlot, bool bDeclareWar);
	bool canMoveThrough(CyPlot* pPlot);
	bool jumpToNearestValidPlot();
	bool canAutomate(AutomateTypes eAutomate);
	bool canScrap();
	bool canGift(bool bTestVisible);
	bool canLoadUnit(CyUnit* pUnit, CyPlot* pPlot, bool bCheckCity);
	bool canLoad(CyPlot* pPlot, bool bCheckCity);
	void load(bool bCheckCity);
	bool canUnload();
	bool canUnloadAll();
	bool canHold(CyPlot* pPlot);
	bool canSleep(CyPlot* pPlot);
	bool canFortify(CyPlot* pPlot);
	bool canHeal(CyPlot* pPlot);
	bool canSentry(CyPlot* pPlot);
	CyCity* bombardTarget(CyPlot* pPlot);
	bool canBombard(CyPlot* pPlot);
	bool canPillage(CyPlot* pPlot);
	bool IsSelected( void );
	bool canFound(CyPlot* pPlot, bool bTestVisible);
	bool canBuild(CyPlot* pPlot, int /*BuildTypes*/ eBuild, bool bTestVisible);
	int canLead(CyPlot* pPlot, int iUnitId) const;
	bool lead(int iUnitId);
	int canGiveExperience(CyPlot* pPlot) const;
	bool giveExperience();
	bool canPromote(int /*PromotionTypes*/ ePromotion, int iLeaderUnitId);
	void promote(int /*PromotionTypes*/ ePromotion, int iLeaderUnitId);
	bool canKingTransport();
	bool canLearn();
	bool canEstablishMission();
	bool canSpeakWithChief(CyPlot* pPlot);
	bool canTradeYield(CyPlot* pPlot);

	int upgradePrice(int /*UnitTypes*/ eUnit);
	bool upgradeAvailable(int /*UnitTypes*/ eFromUnit, int /*UnitClassTypes*/ eToUnitClass, int iCount);
	bool canUpgrade(int /*UnitTypes*/ eUnit, bool bTestVisible);
	bool hasUpgrade(bool bSearch);
	int /*HandicapTypes*/ getHandicapType();
	int /*CivilizationTypes*/ getCivilizationType();
	int /*SpecialUnitTypes*/ getSpecialUnitType();
	int /*UnitTypes*/ getCaptureUnitType(int /*CivilizationTypes*/ eCivilization);
	int /*UnitCombatTypes*/ getUnitCombatType();
	int /*DomainTypes*/ getDomainType();
	int /*InvisibleTypes*/ getInvisibleType();
	int getNumSeeInvisibleTypes();
	int /*InvisibleTypes*/ getSeeInvisibleType(int i);
	bool isHuman();
	int visibilityRange();
	int baseMoves();
	int maxMoves();
	int movesLeft();
	bool canMove();
	bool hasMoved();
	bool canBuildRoute();
	int /*BuildTypes*/ getBuildType();
	int workRate(bool bMax);
	bool isNoBadGoodies();
	bool isOnlyDefensive();
	bool isNoUnitCapture();
	bool isNoCityCapture();
	bool isRivalTerritory();
	bool canCoexistWithEnemyUnit(int /*TeamTypes*/ eTeam);
	bool isFighting();
	bool isAttacking();
	bool isDefending();
	bool isCombat();
	int maxHitPoints();
	int currHitPoints();
	bool isHurt();
	bool isDead();
	void setBaseCombatStr(int iCombat);
	int baseCombatStr();
	int maxCombatStr(CyPlot* pPlot, CyUnit* pAttacker);
	int currCombatStr(CyPlot* pPlot, CyUnit* pAttacker);
	int currFirepower(CyPlot* pPlot, CyUnit* pAttacker);
	float maxCombatStrFloat(CyPlot* pPlot, CyUnit* pAttacker);
	float currCombatStrFloat(CyPlot* pPlot, CyUnit* pAttacker);
	bool canFight();
	bool canAttack();
	bool canDefend(CyPlot* pPlot);
	bool canSiege(int /*TeamTypes*/ eTeam);
	bool isAutomated();
	bool isWaiting();
	bool isFortifyable();
	int fortifyModifier();
	int experienceNeeded();
	int attackXPValue();
	int defenseXPValue();
	int maxXPValue();
	bool isRanged();
	bool alwaysInvisible();
	bool noDefensiveBonus();
	bool canMoveImpassable();
	bool flatMovementCost();
	bool ignoreTerrainCost();
	bool isNeverInvisible();
	bool isInvisible(int /*TeamTypes*/ eTeam, bool bDebug);
	int withdrawalProbability();
	int cityAttackModifier();
	int cityDefenseModifier();
	int hillsAttackModifier();
	int hillsDefenseModifier();
	int DomesticBonusModifier();	
	int terrainAttackModifier(int /*TerrainTypes*/ eTerrain);
	int terrainDefenseModifier(int /*TerrainTypes*/ eTerrain);
	int featureAttackModifier(int /*FeatureTypes*/ eFeature);
	int featureDefenseModifier(int /*FeatureTypes*/ eFeature);
	int unitClassAttackModifier(int /*UnitClassTypes*/ eUnitClass);
	int unitClassDefenseModifier(int /*UnitClassTypes*/ eUnitClass);
	int unitCombatModifier(int /*UnitCombatTypes*/ eUnitCombat);
	int domainModifier(int /*DomainTypes*/ eDomain);
	int bombardRate();
	int /*SpecialUnitTypes*/ specialCargo();
	int /*DomainTypes*/ domainCargo();
	int cargoSpace();
	bool isFull();
	int cargoSpaceAvailable(int /*SpecialUnitTypes*/ eSpecialCargo, int /*DomainTypes*/ eDomainCargo);
	bool hasCargo();
	bool canCargoAllMove();
	int getUnitAICargo(UnitAITypes eUnitAI);
	bool canAssignTradeRoute(int iRouteId, bool bReusePath) const;
	int getID();
	int getGroupID();
	bool isInGroup();
	bool isGroupHead();
	CySelectionGroup* getGroup();
	int getHotKeyNumber();
	void setHotKeyNumber(int iNewValue);
	int getX();
	int getY();
	void setXY(int iX, int iY, bool bGroup, bool bUpdate, bool bShow);
	bool at(int iX, int iY);
	bool atPlot(CyPlot* pPlot);
	CyPlot* plot();
	CyArea* area();
	int getGameTurnCreated();
	int getDamage();
	void setDamage(int iNewValue);
	void changeDamage(int iChange);
	int getMoves();
	void setMoves(int iNewValue);
	void changeMoves(int iChange);
	void finishMoves();
	int getExperience();
	void setExperience(int iNewValue, int iMax);
	void changeExperience(int iChange, int iMax, bool bFromCombat, bool bInBorders, bool bUpdateGlobal);
	int getLevel();
	void setLevel(int iNewLevel);
	void changeLevel(int iChange);
	int getFacingDirection();
	void rotateFacingDirectionClockwise();
	void rotateFacingDirectionCounterClockwise();
	int getCargo();
	int getFortifyTurns();
	int getBlitzCount();
	bool isBlitz();
	int getAmphibCount();
	bool isAmphib();
	int getRiverCount();
	bool isRiver();
	bool isEnemyRoute();
	bool isAlwaysHeal();
	bool isHillsDoubleMove();

	int getExtraVisibilityRange();
	int getExtraMoves();
	int getExtraMoveDiscount();
	int getExtraWithdrawal();
	int getExtraEnemyHeal();
	int getExtraNeutralHeal();
	int getExtraFriendlyHeal();
	int getSameTileHeal();
	int getAdjacentTileHeal();
	int getExtraCombatPercent();
	int getExtraCityAttackPercent();
	int getExtraCityDefensePercent();
	int getExtraHillsAttackPercent();
	int getExtraHillsDefensePercent();
	int getExtraDomesticBonusPercent();	
	int getPillageChange() const;
	int getAnimalGoldChange() const; //WTP, ray, Animal Promotions increase gold from Animals
	int getSlaveRevoltReductionBonus() const; //WTP, ray, Slave Hunter and Slave Master
	int getSlaveWorkerProductionBonus() const; //WTP, ray, Slave Hunter and Slave Master
	int getUpgradeDiscount() const; 
	int getExperiencePercent() const;
	int getImmobileTimer() const;
	void setImmobileTimer(int iNewValue);
	bool isMadeAttack();
	void setMadeAttack(bool bNewValue);

	bool isPromotionReady();
	void setPromotionReady(bool bNewValue);
	bool isDelayedDeath();
	int getOwner();
	int getVisualOwner();
	int getCombatOwner(int /* TeamTypes*/ iForTeam);
	int getTeam();
	int /*UnitTypes*/ getUnitType();
	int /*UnitClassTypes*/ getUnitClassType();
	int /*UnitTypes*/ getLeaderUnitType();
	void setLeaderUnitType(int /*UnitTypes*/ leaderUnitType);
	CyUnit* getTransportUnit() const;
	bool isCargo();
	int getExtraDomainModifier(int /*DomainTypes*/ eIndex);
	std::wstring getName();
	std::wstring getNameForm(int iForm);
	std::wstring getNameKey();
	std::wstring getNameNoDesc();
	std::wstring getNameAndProfession();
	std::wstring getNameOrProfessionKey();
	void setName(std::basic_string<unsigned short> szNewValue);
	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);
	bool isTerrainDoubleMove(int /*TerrainTypes*/ eIndex);
	bool isFeatureDoubleMove(int /*FeatureTypes*/ eIndex);
	int getExtraTerrainAttackPercent(int /*TerrainTypes*/ eIndex);
	int getExtraTerrainDefensePercent(int /*TerrainTypes*/ eIndex);
	int getExtraFeatureAttackPercent(int /*FeatureTypes*/ eIndex);
	int getExtraFeatureDefensePercent(int /*FeatureTypes*/ eIndex);
	int getExtraUnitCombatModifier(int /*UnitCombatTypes*/ eIndex);
	bool canAcquirePromotion(int /*PromotionTypes*/ ePromotion);
	bool canAcquirePromotionAny();
	bool isPromotionValid(int /*PromotionTypes*/ ePromotion);
	bool isHasPromotion(int /*PromotionTypes*/ ePromotion);
	bool isHasRealPromotion(int /*PromotionTypes*/ ePromotion);
	void setHasRealPromotion(int /*PromotionTypes*/ eIndex, bool bValue);
	void changeFreePromotionCount(int /*PromotionTypes*/ eIndex, int iChange);
	int getProfession();
	void setProfession(int /*ProfessionTypes*/ eProfession);
	bool canHaveProfession(int /*ProfessionTypes*/ eProfession);
	int /*UnitAITypes*/ getUnitAIType();
	void setUnitAIType(int /*UnitAITypes*/ iNewValue);
	int getYieldStored();
	void setYieldStored(int iAmount);
	int /*YieldTypes*/ getYield();
	bool isGoods();
	int getUnitTravelTimer() const;
	void setUnitTravelTimer(int iValue);
	int /*UnitTravelStates*/ getUnitTravelState();
	void setUnitTravelState(int /*UnitTravelStates*/ eState, bool bShowEuropeScreen);
	bool canSailEurope(int iEurope);
	bool isColonistLocked();
	// < JAnimals Mod Start >
	bool isBarbarian();
	void setBarbarian(bool bNewValue);
	// < JAnimals Mod End >
	bool isIgnoreDanger();	// TAC - Trade Routes Advisor - koma13
	// TAC - LbD - Ray - START
	int getLbDrounds();
	int getLastLbDProfession();
	// TAC - LbD - Ray - END

	// WTP, ray, helper methods for Python Event System - Spawning Units and Barbarians on Plots - START
	void spawnOwnPlayerUnitOnPlotOfUnit(int /*UnitClassTypes*/ iIndex) const;
	void spawnBarbarianUnitOnPlotOfUnit(int /*UnitClassTypes*/ iIndex) const;
	void spawnOwnPlayerUnitOnAdjacentPlotOfUnit(int /*UnitClassTypes*/ iIndex) const;
	void spawnBarbarianUnitOnAdjacentPlotOfUnit(int /*UnitClassTypes*/ iIndex) const;

	bool isOwnPlayerUnitOnAdjacentPlotOfUnit(int /*UnitClassTypes*/ iIndex) const;
	bool isBarbarianUnitOnAdjacentPlotOfUnit(int /*UnitClassTypes*/ iIndex) const;
	// WTP, ray, helper methods for Python Event System - Spawning Units and Barbarians on Plots - END

	// WTP, ray, saving 1 more Profession for Fisher Issue - START
	int getLbDroundsBefore();
	int getLastLbDProfessionBefore();
	// WTP, ray, saving 1 more Profession for Fisher Issue - END

	const CvArtInfoUnit* getArtInfo(int i) const;
	std::string getButton() const;
	std::string getFullLengthIcon() const;
	// Python Helper Functions
	void centerCamera();
protected:
	CvUnit* m_pUnit;
};
#endif	// #ifndef CyUnit
